/****************************************************************************
 *
 *            Copyright (c) 2003-2008 by HCC Embedded
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


#include "thin_usr.h"

#include <hcc/fatfs_sthin/volume.h>
#include <hcc/fatfs_sthin/util.h>
#include "drv.h"
#include <hcc/fatfs_sthin/fat.h>
#include <hcc/fatfs_sthin/dir.h>


F_VOLUME gl_volume;		/* only one volume */
F_FILE gl_file;			/* file */
char gl_sector[F_SECTOR_SIZE];	/* actual sector */

#if (F_FAT32 & F_FORMATTING & F_WRITING)
/* Defines the number of sectors per cluster on a sector number basis */
typedef struct {
  unsigned long max_sectors;
  unsigned char sector_per_cluster;
} t_FAT32_CS;

static const t_FAT32_CS FAT32_CS[]={
  { 0x00020000, 1 },	/* ->64MB */
  { 0x00040000, 2 },	/* ->128MB */
  { 0x00080000, 4 },	/* ->256MB */
  { 0x01000000, 8 },	/* ->8GB */
  { 0x02000000, 16 },	/* ->16GB */
  { 0x0ffffff0, 32 }	/* -> ... */
};
#endif

/****************************************************************************
 *
 * _f_writebootrecord
 *
 * writing boot record onto a volume, it uses number of hidden sector variable
 *
 * INPUTS
 * phy - media physical descriptor
 *
 * RETURNS
 * error code or zero if successful
 *
 ***************************************************************************/
#if (F_FORMATTING & F_WRITING)
static unsigned char _f_writebootrecord(F_PHY *phy) {
unsigned char jump_code[]={0xeb,0x3c,0x90};
unsigned char oem_name[]="MSDOS5.0";
unsigned char executable_marker[]={0x55,0xaa};
unsigned char *ptr=(unsigned char *)gl_sector;
unsigned char rs;
unsigned short mre;

#if F_FAT32
unsigned char ret;
unsigned char _n=0;

   if (gl_volume.mediatype==F_FAT32_MEDIA) 
   { /*write FS_INFO*/
      unsigned char a;

      rs=32+4;
      mre=0;

      __memset(ptr,0,F_SECTOR_SIZE);

      for (a=0; a<rs; a++) {
         ret=_f_writeglsector(a); /*erase reserved area*/
         if (ret) return ret;
      }

      ptr=_setlong(0x41615252,ptr); /*signature*/
      ptr=_setcharzero(480,ptr);    /*reserved*/
      ptr=_setlong(0x61417272,ptr); /*signature*/
      ptr=_setlong(0xffffffff,ptr); /*no last*/
      ptr=_setlong(0xffffffff,ptr); /*no hint*/
      ptr=_setcharzero(12,ptr);     /*reserved*/
      ptr=_setlong(0xaa550000,ptr); /*trail*/


      ret=_f_writeglsector(1); /*write FSINFO*/
      if (ret) return ret;

      ret=_f_writeglsector(1+6); /*write FSINFO*/
      if (ret) return ret;
   }
   else
#endif
   {
     rs=1;
     mre=512;
   }

   ptr=(unsigned char *)gl_sector;
   ptr=_setchar(jump_code,sizeof(jump_code),ptr);
   ptr=_setchar(oem_name,sizeof(oem_name)-1,ptr);
   ptr=_setword(F_SECTOR_SIZE,ptr);
   *ptr++=gl_volume.bootrecord.sector_per_cluster;
   ptr=_setword(rs,ptr);		/* reserved sectors */
   *ptr++=2;			/* number of FATs */
   ptr=_setword(mre,ptr);	/* max root entry */
   if (phy->number_of_sectors<0x10000) ptr=_setword((unsigned short)phy->number_of_sectors,ptr);
                                  else ptr=_setword(0,ptr);
   *ptr++=0xf0;                 /* media descriptor */
   ptr=_setword((unsigned short)gl_volume.bootrecord.sector_per_FAT,ptr);
   ptr=_setword(phy->sector_per_track,ptr);
   ptr=_setword(phy->number_of_heads,ptr);
   ptr=_setlong(0,ptr);		/* number of hidden sectors */
   if (phy->number_of_sectors>=0x10000) ptr=_setlong(phy->number_of_sectors,ptr);
                                   else ptr=_setlong(0,ptr);		/* number of sectors */

#if F_FAT32
   if (gl_volume.mediatype==F_FAT32_MEDIA) 
   {
     ptr=_setlong(gl_volume.bootrecord.sector_per_FAT32,ptr);
     ptr=_setword(0,ptr);
     ptr=_setword(0,ptr);
     ptr=_setlong(2,ptr);
     ptr=_setword(1,ptr);
     ptr=_setword(6,ptr);
     ptr=_setchar(NULL,12,ptr);
     _n=28;
   }
#endif  
                                   
   ptr=_setword(0,ptr);		/* logical drive num */
   *ptr++=0x29;			/* extended signature */
   ptr=_setlong(0x11223344,ptr);
   ptr=_setchar((const unsigned char *)"NO NAME    ",11,ptr);	/* volume name */
   
   switch (gl_volume.mediatype)
   {
#if F_FAT12
     case F_FAT12_MEDIA: ptr=_setchar((const unsigned char *)"FAT12   ",8,ptr); break;
#endif
#if F_FAT16
     case F_FAT16_MEDIA: ptr=_setchar((const unsigned char *)"FAT16   ",8,ptr); break;
#endif
#if F_FAT32
     case F_FAT32_MEDIA: ptr=_setchar((const unsigned char *)"FAT32   ",8,ptr); break;
#endif
     default: return F_ERR_INVALIDMEDIA;
   }

#if F_FAT32
   ptr=_setchar(0,448-_n,ptr); 
#else
   ptr=_setchar(0,448,ptr); 
#endif
   ptr=_setchar(executable_marker,sizeof(executable_marker),ptr);

#if F_FAT32
   if (_n)
   {
     ret=_f_writeglsector(6);
     if (ret) return ret;
   }
#endif

   return _f_writeglsector(0); /*write bootrecord*/
}
#endif


/****************************************************************************
 *
 * _f_buildsectors
 *
 * INPUTS
 * phy - media physical descriptor
 *
 * calculate relative sector position from boot record
 *
 ***************************************************************************/
#if (F_FORMATTING & F_WRITING)
static unsigned char _f_buildsectors(F_PHY *phy) 
{
   gl_volume.mediatype=F_UNKNOWN_MEDIA;


   if (gl_volume.bootrecord.sector_per_FAT) 
   {
	   gl_volume.firstfat.sector=1;
	   gl_volume.firstfat.num=gl_volume.bootrecord.sector_per_FAT;
   	   gl_volume.root.sector=gl_volume.firstfat.sector+(gl_volume.firstfat.num*(unsigned long)(gl_volume.bootrecord.number_of_FATs));
    	   gl_volume.root.num=(512*sizeof(F_DIRENTRY))/F_SECTOR_SIZE;
      
	   gl_volume._tdata.sector=gl_volume.root.sector+gl_volume.root.num;
	   gl_volume._tdata.num=0; /*??*/
   }
   else {
#if F_FAT32
	   gl_volume.firstfat.sector=(32+4);
	   gl_volume.firstfat.num=gl_volume.bootrecord.sector_per_FAT32;
	   gl_volume._tdata.sector=gl_volume.firstfat.sector;
	   gl_volume._tdata.sector+=gl_volume.firstfat.num*(unsigned long)(gl_volume.bootrecord.number_of_FATs);
	   gl_volume._tdata.num=0; /*??*/

	   {
	 	 unsigned long sectorcou=gl_volume.bootrecord.sector_per_cluster;
	 	 gl_volume.root.sector=((gl_volume.bootrecord.rootcluster-2)*sectorcou)+gl_volume._tdata.sector;
		 gl_volume.root.num=gl_volume.bootrecord.sector_per_cluster;
	   }
#else
      return F_ERR_INVALIDMEDIA; /*unknown media*/
#endif
   }

   {   
     unsigned long maxcluster;
     maxcluster=phy->number_of_sectors;
     maxcluster-=gl_volume._tdata.sector;
     maxcluster/=gl_volume.bootrecord.sector_per_cluster;
     gl_volume.maxcluster=maxcluster;
   }

   if (gl_volume.maxcluster<(F_CLUSTER_RESERVED&0xfff)) {
#if F_FAT12
	gl_volume.mediatype=F_FAT12_MEDIA;
#else
   	return F_ERR_INVALIDMEDIA;
#endif
   }
   else if (gl_volume.maxcluster<(F_CLUSTER_RESERVED&0xffff)) {
#if F_FAT16
	gl_volume.mediatype=F_FAT16_MEDIA;
#else
   	return F_ERR_INVALIDMEDIA;
#endif
   }
   else {
#if F_FAT32
	gl_volume.mediatype=F_FAT32_MEDIA;
#else
   	return F_ERR_INVALIDMEDIA;
#endif
   }

   return F_NO_ERROR;
}
#endif



/****************************************************************************
 *
 * _f_prepareformat
 *
 * preparing boot record for formatting, it sets and calculates values
 *
 * INPUTS
 * phy - media physical descriptor
 * f_bootrecord - which bootrecord need to be prepare
 * number_of_hidden_sectors - where boot record starts
 * fattype - one of this definitions F_FAT12_MEDIA,F_FAT16_MEDIA,F_FAT32_MEDIA
 *
 * RETURNS
 * error code or zero if successful
 *
 ***************************************************************************/
#if (F_FORMATTING & F_WRITING)
static unsigned char _f_prepareformat(F_PHY *phy,unsigned char fattype) 
{
	if (!phy->number_of_sectors) return F_ERR_INVALIDSECTOR;

	gl_volume.bootrecord.number_of_FATs=2;
	gl_volume.bootrecord.media_descriptor=0xf0;

#if F_FAT32
        if (fattype!=F_FAT32_MEDIA)
#endif
	{
	  unsigned long _n;
	  switch (fattype)
	  {
#if F_FAT12
	    case F_FAT12_MEDIA: _n=F_CLUSTER_RESERVED&0xfff; break;
#endif
#if F_FAT16
	    case F_FAT16_MEDIA: _n=F_CLUSTER_RESERVED&0xffff; break;
#endif
	    default: return F_ERR_INVFATTYPE;
	  }

	  gl_volume.bootrecord.sector_per_cluster=1;
	  while (gl_volume.bootrecord.sector_per_cluster) 
	  {
	    if (phy->number_of_sectors / gl_volume.bootrecord.sector_per_cluster < _n) break;
	    gl_volume.bootrecord.sector_per_cluster<<=1;
	  }
	  
	  if (!gl_volume.bootrecord.sector_per_cluster) return F_ERR_MEDIATOOLARGE;
	}
#if F_FAT32
	else
	{
          unsigned char i;
          for (i=0;i<(sizeof(FAT32_CS)/sizeof(t_FAT32_CS))-1 && phy->number_of_sectors>FAT32_CS[i].max_sectors;i++)
            ;
          gl_volume.bootrecord.sector_per_cluster=FAT32_CS[i].sector_per_cluster;
	}
#endif
	if (!gl_volume.bootrecord.sector_per_cluster) return F_ERR_INVALIDMEDIA; /*fat16 cannot be there*/


	{
		long secpercl=gl_volume.bootrecord.sector_per_cluster;
		long nfat=gl_volume.bootrecord.number_of_FATs;
		unsigned long roots;
		unsigned long fatsec;

   		roots=(512*sizeof(F_DIRENTRY))/F_SECTOR_SIZE;

		switch(fattype)
		{
#if F_FAT32
		case F_FAT32_MEDIA:
		      {
			unsigned long _n=(unsigned long)(128*secpercl+nfat);
   			fatsec=(phy->number_of_sectors-(32+4)+2*secpercl);
			fatsec+=(_n-1);
			fatsec/=_n;
			gl_volume.bootrecord.sector_per_FAT32=fatsec;
			gl_volume.bootrecord.sector_per_FAT=0;
		      }
		      break;
#endif
#if F_FAT16
		  case F_FAT16_MEDIA:
		      {
            		unsigned long _n=(unsigned long)(256*secpercl+nfat);
   			fatsec=(phy->number_of_sectors-1-roots+2*secpercl);
			fatsec+=(_n-1);
			fatsec/=_n;
   			gl_volume.bootrecord.sector_per_FAT=(unsigned short)(fatsec);
		      }
		      break;
#endif
#if F_FAT12
		  case F_FAT12_MEDIA:
		      {
            		unsigned long _n=(unsigned long)(1024*secpercl+3*nfat);
   			fatsec=(phy->number_of_sectors-1-roots+2*secpercl);
			fatsec*=3;
			fatsec+=(_n-1); 
			fatsec/=_n;
   			gl_volume.bootrecord.sector_per_FAT=(unsigned short)(fatsec);
		      }
		      break;
#endif
		  default: return F_ERR_INVALIDMEDIA;
		}
		return F_NO_ERROR;
	}
}
#endif



/****************************************************************************
 *
 * _f_postformat
 *
 * erase fats, erase root directory, reset variables after formatting
 *
 * INPUTS
 * phy - media physical descriptor
 * fattype - one of this definitions F_FAT12_MEDIA,F_FAT16_MEDIA,F_FAT32_MEDIA
 *
 * RETURNS
 * error code or zero if successful
 *
 ***************************************************************************/
#if (F_FORMATTING & F_WRITING)
static unsigned char _f_postformat(F_PHY *phy, unsigned char fattype) {
unsigned long a;
unsigned char ret;

	_f_buildsectors(phy); /*get positions*/
	if (gl_volume.mediatype!=fattype) return F_ERR_MEDIATOOSMALL;

	gl_volume.fatsector=(unsigned long)(-1);

        {
          unsigned char *ptr=(unsigned char *)gl_sector;
          unsigned char j=2;
          unsigned long i;

          __memset(ptr,0,F_SECTOR_SIZE);

          switch (gl_volume.mediatype)
          {
#if F_FAT16
	    case F_FAT16_MEDIA: j=3; break;
#endif
#if F_FAT32
	    case F_FAT32_MEDIA: j=11; break;
#endif
          }

          *ptr=gl_volume.bootrecord.media_descriptor;
          __memset(ptr+1,0xff,(__size_t)j);
#if F_FAT32
          if (gl_volume.mediatype==F_FAT32_MEDIA) *(ptr+8)=(unsigned char)(F_CLUSTER_LAST&0xff);
#endif
          (void)_f_writeglsector(gl_volume.firstfat.sector);
          (void)_f_writeglsector(gl_volume.firstfat.sector+gl_volume.firstfat.num);
          __memset(ptr,0,(__size_t)(j+1));
          
          for (i=1;i<gl_volume.firstfat.num;i++)
          {
            (void)_f_writeglsector(gl_volume.firstfat.sector+i);
            (void)_f_writeglsector(gl_volume.firstfat.sector+i+gl_volume.firstfat.num);
          }
        }

 	for (a=0; a<gl_volume.root.num;a++) { /*reset root direntries*/
  		ret = _f_writeglsector(gl_volume.root.sector+a);
  		if (ret) return ret;
	}

	return _f_writebootrecord(phy);
}
#endif


/****************************************************************************
 *
 * fn_hardformat
 *
 * Making a complete format on media, independently from master boot record,
 * according to media physical
 *
 * INPUTS
 * fattype - one of this definitions F_FAT12_MEDIA,F_FAT16_MEDIA,F_FAT32_MEDIA
 *
 * RETURNS
 * error code or zero if successful
 *
 ***************************************************************************/
#if (F_FORMATTING & F_WRITING)
unsigned char fn_hardformat(unsigned char fattype) 
{
  unsigned char ret;
  F_PHY phy;

  ret=_f_getvolume();
  if (ret && ret!=F_ERR_NOTFORMATTED) return ret;

  gl_volume.state=F_STATE_NEEDMOUNT;

  __memset(&phy,0,sizeof(F_PHY));

#ifndef drv_getphy
  return F_ERR_INITFUNC;
#else
  ret=drv_getphy(&phy);
  if (ret) return F_ERR_ONDRIVE;
#endif

  ret=_f_prepareformat(&phy,fattype); /*no partition*/
  if (ret) return ret;

  return _f_postformat(&phy,fattype);
}

#endif



/****************************************************************************
 *
 * _f_readbootrecord
 *
 * read boot record from a volume, it detects if there is MBR on the media
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
static unsigned char _f_readbootrecord(void) {
unsigned char ret;
unsigned char *ptr=(unsigned char *)gl_sector;
unsigned long maxcluster,_n;
unsigned long first_sector=0;

   gl_volume.mediatype=F_UNKNOWN_MEDIA;


   ret=_f_readglsector(0);
   if (ret) return ret;


   if ((ptr[0x1fe]!=0x55) || (ptr[0x1ff]!=0xaa)) return F_ERR_NOTFORMATTED; /*??*/

   if ((ptr[0]!=0xeb) && (ptr[0]!=0xe9)) {

	   first_sector=_f_getlong(&ptr [0x08+0x1be] ); /*start sector for 1st partioon*/

           ret=_f_readglsector(first_sector);
	   if (ret) return ret;

	   if ((ptr[0x1fe]!=0x55) || (ptr[0x1ff]!=0xaa)) return F_ERR_NOTFORMATTED; /*??*/

	   if ((ptr[0]!=0xeb) && (ptr[0]!=0xe9)) return F_ERR_NOTFORMATTED; /*??*/
   }

   ptr+=11;
   if (_f_getword(ptr)!=F_SECTOR_SIZE) return F_ERR_NOTSUPPSECTORSIZE;
   ptr+=2;
   gl_volume.bootrecord.sector_per_cluster=*ptr++;
   gl_volume.firstfat.sector=_f_getword(ptr); ptr+=2;
   gl_volume.bootrecord.number_of_FATs=*ptr++;
   gl_volume.root.num=_f_getword(ptr); ptr+=2;
   gl_volume.root.num*=sizeof(F_DIRENTRY);
   gl_volume.root.num/=F_SECTOR_SIZE;
   maxcluster=_f_getword(ptr); ptr+=2;
   gl_volume.bootrecord.media_descriptor=*ptr++;
   gl_volume.firstfat.num=_f_getword(ptr); ptr+=6;
   _n=_f_getlong(ptr); ptr+=4;
   if (_n<first_sector) _n=first_sector;
   gl_volume.firstfat.sector+=_n;
   if (!maxcluster) maxcluster=_f_getlong(ptr); ptr+=4;


   if (gl_volume.firstfat.num)
   {
     gl_volume.root.sector=gl_volume.firstfat.sector+(gl_volume.firstfat.num*gl_volume.bootrecord.number_of_FATs);
     gl_volume._tdata.sector=gl_volume.root.sector+gl_volume.root.num;
     gl_volume._tdata.num=0;
     ptr+=3;
   }
   else
   {
#if F_FAT32
     gl_volume.firstfat.num=_f_getlong(ptr); ptr+=8;
     gl_volume._tdata.sector=gl_volume.firstfat.sector;
     gl_volume._tdata.sector+=gl_volume.firstfat.num*gl_volume.bootrecord.number_of_FATs;
     gl_volume._tdata.num=0;
     gl_volume.bootrecord.rootcluster=_f_getlong(ptr); ptr+=23;
     gl_volume.root.num=gl_volume.bootrecord.sector_per_cluster;
     gl_volume.root.sector=((gl_volume.bootrecord.rootcluster-2)*gl_volume.root.num)+gl_volume._tdata.sector;
#else
     return F_ERR_INVALIDMEDIA;
#endif
   }

   gl_volume.bootrecord.serial_number=_f_getlong(ptr);

   maxcluster-=gl_volume._tdata.sector;
   maxcluster+=_n;
   gl_volume.maxcluster=maxcluster/gl_volume.bootrecord.sector_per_cluster;

   if (gl_volume.maxcluster<(F_CLUSTER_RESERVED&0xfff)) {
#if F_FAT12
	   gl_volume.mediatype=F_FAT12_MEDIA;
#else
           return F_ERR_INVALIDMEDIA;
#endif
   }
   else if (gl_volume.maxcluster<(F_CLUSTER_RESERVED&0xffff)) {
#if F_FAT16
	   gl_volume.mediatype=F_FAT16_MEDIA;
#else
           return F_ERR_INVALIDMEDIA;
#endif
   }
   else {
#if F_FAT32
	gl_volume.mediatype=F_FAT32_MEDIA;
#else
   	return F_ERR_INVALIDMEDIA;
#endif
   }

   if (gl_volume.bootrecord.media_descriptor!=0xf8) {     /*fixdrive*/
	   if (gl_volume.bootrecord.media_descriptor!=0xf0) { /*removable*/
		   return F_ERR_NOTFORMATTED; /*??*/
	   }
   }

   return F_NO_ERROR;
}




/****************************************************************************
 *
 * _f_getvolume
 *
 * getting back a volume info structure of a given drive, it try to mounts 
 * drive if it was not mounted before
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

unsigned char _f_getvolume(void) {

	switch (gl_volume.state) {
	case F_STATE_NONE: return F_ERR_ONDRIVE;
	case F_STATE_WORKING: 
		
		if (!_f_checkstatus()) return F_NO_ERROR;

      /* here we don't stop case flow,  */
      /* because we have to clean up this volume! */

	case F_STATE_NEEDMOUNT: 
		{
#if F_WRITING
            gl_file.modified=0;
			gl_volume.modified=0;
			gl_volume.lastalloccluster=0; 
#endif
			gl_volume.actsector=(unsigned long)(-1);
			gl_volume.fatsector=(unsigned long)(-1);

			gl_file.mode=F_FILE_CLOSE;

#if F_DIRECTORIES
			gl_volume.cwd[0]=0; /*reset cwd*/
#endif
			gl_volume.mediatype=F_UNKNOWN_MEDIA;

#ifdef drv_getstatus
			if (drv_getstatus() & F_ST_MISSING) {
				gl_volume.state=F_STATE_NEEDMOUNT; /*card missing*/
				return F_ERR_CARDREMOVED;
			}        
#endif

			if (!_f_readbootrecord()) {
	
				gl_volume.state=F_STATE_WORKING;
				return F_NO_ERROR;
			}

			gl_volume.mediatype=F_UNKNOWN_MEDIA;
			return F_ERR_NOTFORMATTED;
		}

	}

	return F_ERR_ONDRIVE;
}



/****************************************************************************
 *
 * fn_getfreespace
 *
 * get total/free/used/bad diskspace
 *
 * INPUTS
 * pspace - pointer where to store the information
 *
 * RETURNS
 * error code
 *
 ***************************************************************************/
#if F_GETFREESPACE
unsigned char fn_getfreespace(F_SPACE *pspace) { 
unsigned char ret;
unsigned long a;


	ret=_f_getvolume();
	if (ret) return ret;

   	__memset(pspace,0,sizeof(F_SPACE));
	pspace->total=gl_volume.maxcluster;

	gl_volume.fatsector=(unsigned long)-1;
	for (a=2; a<gl_volume.maxcluster+2; a++) {
		unsigned long value;

		ret=_f_getclustervalue(a,&value);
		if (ret) return ret;

		if (!value) ++(pspace->free);
		else if (value==F_CLUSTER_BAD) ++(pspace->bad);
		else ++(pspace->used); 
	}

	a=(unsigned long)((unsigned long)gl_volume.bootrecord.sector_per_cluster*F_SECTOR_SIZE);
	pspace->total*=a;
	pspace->free*=a;
	pspace->used*=a;
	pspace->bad*=a;

	return F_NO_ERROR;
}
#endif


/****************************************************************************
 *
 * fn_getserial
 *
 * get serial number
 *
 * INPUTS
 * serial - pointer where to store the serial number 
 *
 * RETURNS
 * error code
 *
 ***************************************************************************/
unsigned char fn_getserial (unsigned long *serial) 
{ 
  unsigned char ret;
  ret=_f_getvolume();
  if (ret) return ret;
  *serial=gl_volume.bootrecord.serial_number;
  return 0;
}


/****************************************************************************
 *
 * fn_initvolume
 *
 * initiate a volume, this function has to be called 1st to set physical
 * driver function to a given volume
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
unsigned char fn_initvolume(void) {

	gl_volume.state=F_STATE_NONE;

#ifdef drv_initfunc
   	if (drv_initfunc()) return F_ERR_INITFUNC;
#endif

	gl_volume.state=F_STATE_NEEDMOUNT;

	return _f_getvolume();
}

