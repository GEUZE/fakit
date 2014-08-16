/****************************************************************************
 *
 *            Copyright (c) 2005-2008 by HCC Embedded
 *
 * This software is copyrighted by and is the sole property of
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1133
 * Vaci ut 110
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/

#include <hcc/effs/ram/ramdrv_s.h>

/****************************************************************************
 *
 * static definitions
 *
 ***************************************************************************/

#define MAXFILE 4    /* number of file could be opened at once */
#define RAM_SECSIZE 4096 /* this is typical sectorsize */

#define MEMCPY_LONG 1	/* set this value to 1 if 32 bit access of ram is faster */


/****************************************************************************
 *
 * ram_memcpy
 *
 * copy memory to memory, if it is long aligned then it uses long ptr to copy
 *
 * INPUTS
 *
 * dest - destination ptr
 * sou - source ptr
 * len - length of data
 *
 ***************************************************************************/

#if MEMCPY_LONG
static void ram_memcpy(void *dest, void *sou, long len) 
{
	if ( (!(((long)dest)&3)) && (!(((long)sou)&3)) && (!(len&3)) ) 
	{
		long *s=(long*)sou;
		long *d=(long*)dest;
		len >>= 2;
		while (len--) 
		{
			*d++=*s++;
		}
	}
	else 
	{
		char *s=(char*)sou;
		char *d=(char*)dest;
		while (len--) 
		{
			*d++=*s++;
		}
	}
}
#else
#define ram_memcpy fsm_memcpy
#endif

/****************************************************************************
 *
 * StoreFat
 *
 * Not used in ramdrive
 *
 ***************************************************************************/

static int StoreFat(FS_VOLUMEINFO *vi) 
{
	if (vi) return 0;
	return 1;
}

/****************************************************************************
 *
 * StoreSector
 *
 * Called from higher level to store one sector into ram
 *
 * INPUTS
 *
 * vi - volumeinfo where the free sectors are
 * file - internal file pointer where last sector is
 * data - data pointer where data is
 * len - length of data need to be stored
 *
 * RETURNS
 *
 * 0 - if successfully stored
 * other if any error
 *
 ***************************************************************************/

static int StoreSector(FS_VOLUMEINFO *vi,FS_FILEINT *file,void *data,long len) 
{
	unsigned short a;

	if (!len) return 0; /* nothing need to store */

	for (a=0; a<vi->maxsectornum; a++) 
	{
		if (fsm_checksectorfree(vi,a)) 
		{

			fsm_addsectorchain(vi,file,a);

			{
				char *ptr=vi->ramdrivedata;
				ptr+=(long)a * vi->sectorsize;
				ram_memcpy (ptr,data,len);
			}

			return 0;
		}
	}

	return 1; /* no space */
}

/****************************************************************************
 *
 * GetSector
 *
 * Get sector data back from ram
 *
 * INPUTS
 *
 * vi - volumeinfo wich volume to belong it
 * data - where to store data
 * offset - relative offset in sector to start reading
 * datalen - length of retreived data
 *
 * RETURNS
 *
 * 0 - if successfully restored
 * other if any error
 *
 ***************************************************************************/

static int GetSector (const FS_VOLUMEINFO *vi,long secnum,void *data,long offset,long datalen)
{
	char *ptr=vi->ramdrivedata;

	ptr+=secnum * vi->sectorsize;

	ram_memcpy (data,ptr+offset,datalen);

	return 0;
}

/****************************************************************************
 *
 * Format
 *
 * Format a volume
 *
 * INPUTS
 *
 * vi - volumeinfo which volume needed to be formatted
 *
 * RETURNS
 *
 * 0 - if successfully formatted
 * other if any error
 *
 ***************************************************************************/

static int Format(FS_VOLUMEINFO *vi)
{
	long a;
	unsigned int b;

	/* reset FAT */
	for (a=0; a<vi->maxsectornum; a++)
	{
		vi->_fat[a]=FS_FAT_FREE;
		vi->fatmirror[a]=FS_FAT_FREE;
	}

	/* reset directory */
	for (b=0; b<vi->maxdirentry; b++)
	{
		vi->direntries[b].attr=0;
	}

	return 0;
}

/****************************************************************************
 *
 * fs_mount_ramdrive
 *
 * Mount function for this driver, called from fs_mount function
 *
 * INPUTS
 *
 * vd - volumedescriptor of the volume
 *
 * RETURNS
 *
 * 0 - if successfully mounted
 * other if any error
 *
 ***************************************************************************/

int fs_mount_ramdrive(FS_VOLUMEDESC *vd,FS_PHYGETID phyfunc)
{
	FS_VOLUMEINFO *vi=vd->vi;

	/* check phy function */
	if (phyfunc)
	{
		vd->state=FS_VOL_DRVERROR;	 /* no physical function needed */
		return 1;
	}

	/* set dll functions */
	vd->storefat       =StoreFat;
	vd->storesector    =StoreSector;
	vd->getsector	   =GetSector;
	vd->format		   =Format;

	/* alloc write buffer  */
	if (fsm_setsectorsize(vi,RAM_SECSIZE))
	{
		vd->state=FS_VOL_NOMEMORY;
		return 1;
	}

	/* alloc files */
	if (fsm_setmaxfile(vi,MAXFILE))
	{
		vd->state=FS_VOL_NOMEMORY;
		return 1;
	}

	/* alloc blockdata information temporary wr/rd buffers */
	vi->rdbuffer=(char*)fsm_allocdata(vi,vi->sectorsize);
	if (!vi->rdbuffer)
	{
		vd->state=FS_VOL_NOMEMORY;
		return 1;
	}

	/* calculate maximum number of direntry and alloc directory */
	vi->maxdirentry=(unsigned int)(vi->freemem / vi->sectorsize);  /* calculate directory entry numbers */
	vi->direntries=(FS_DIRENTRY *)fsm_allocdata(vi,(long)vi->maxdirentry*sizeof(FS_DIRENTRY));
	if (!vi->direntries)
	{
		vd->state=FS_VOL_NOMEMORY;
		return 1;
	}

	/* alloc FAT */
	vi->maxsectornum= (vi->freemem - ( (vi->freemem / vi->sectorsize)*(long)sizeof(unsigned long) )) / vi->sectorsize;
	vi->_fat=(unsigned short *)fsm_allocdata(vi,vi->maxsectornum*sizeof(unsigned short));	 /* allocate FAT */
	if (!vi->_fat)
	{
		vd->state=FS_VOL_NOMEMORY;
		return 1;
	}

	/* alloc mirror FAT */
	vi->fatmirror=(unsigned short *)fsm_allocdata(vi,vi->maxsectornum*sizeof(unsigned short));	 /* allocate FAT */
	if (!vi->fatmirror)
	{
		vd->state=FS_VOL_NOMEMORY;
		return 1;
	}

	/* alloc sectors */
	vi->ramdrivedata=(char*)fsm_allocdata(vi,vi->maxsectornum * vi->sectorsize);
	if (!vi->ramdrivedata)
	{
		vd->state=FS_VOL_NOMEMORY;
		return 1;
	}

	/* RAM drive always empty at start up */
	if (Format(vi))
	{
		vd->state=FS_VOL_NOTFORMATTED;
		return 1;
	}

	/* set working state */
	vd->state=FS_VOL_OK;

	return 0;
}


/****************************************************************************
 *
 * end of ramdrv_s.c
 *
 ***************************************************************************/

