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

#include <hcc/fatfs_thin/volume.h>
#include <hcc/fatfs_thin/util.h>
#include <hcc/fatfs_thin/fat.h>
#include "drv.h"

#if F_LONGFILENAME
#include <hcc/fatfs_thin/dir_lfn.h>
#else
#include <hcc/fatfs_thin/dir.h>
#endif

#if FATBITFIELD_ENABLE && (F_FAT16 || F_FAT32)
#include <stdlib.h>
#endif

F_VOLUME gl_volume;	/*only one volume*/

#if F_LONGFILENAME
F_FILE gl_files[F_MAXFILES+1];
#else
F_FILE gl_files[F_MAXFILES];
#endif

#if F_FAT32
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

#if (F_FORMATTING & F_WRITING)
/****************************************************************************
 *
 * _f_buildsectors
 *
 * calculate relative sector position from boot record
 *
 ***************************************************************************/
static unsigned char _f_buildsectors(void) 
{
   gl_volume.mediatype=F_UNKNOWN_MEDIA;

   gl_volume.firstfat.sector=gl_volume.bootrecord.reserved_sectors+gl_volume.bootrecord.number_of_hidden_sectors; /*maybe partioned*/

   if (gl_volume.bootrecord.sector_per_FAT) {
	   gl_volume.firstfat.num=gl_volume.bootrecord.sector_per_FAT;

   	   gl_volume.root.sector=gl_volume.firstfat.sector+(gl_volume.firstfat.num*(unsigned long)(gl_volume.bootrecord.number_of_FATs));
       gl_volume.root.num=((gl_volume.bootrecord.max_root_entry)*sizeof(F_DIRENTRY))/F_SECTOR_SIZE;
      
	   gl_volume._tdata.sector=gl_volume.root.sector+gl_volume.root.num;
	   gl_volume._tdata.num=0; /*??*/
   }
   else {
#if F_FAT32
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
     maxcluster=gl_volume.bootrecord.number_of_sectors_less32;
     if (!maxcluster) maxcluster=gl_volume.bootrecord.number_of_sectors;
     maxcluster-=gl_volume._tdata.sector;
     maxcluster+=gl_volume.bootrecord.number_of_hidden_sectors; /*out of calc!*/
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
unsigned char *ptr=gl_files[0]._tdata;
unsigned char _n=0;
unsigned long first_sector=0;

   ret=_f_readsector(ptr,0);
   if (ret) return ret;


   if ((ptr[0x1fe]!=0x55) || (ptr[0x1ff]!=0xaa)) return F_ERR_NOTFORMATTED; /*??*/

   if ((ptr[0]!=0xeb) && (ptr[0]!=0xe9)) {

	   first_sector=_f_getlong(&ptr [0x08+0x1be] ); /*start sector for 1st partion*/

       ret=_f_readsector(ptr,first_sector);
	   if (ret) return ret;

	   if ((ptr[0x1fe]!=0x55) || (ptr[0x1ff]!=0xaa)) return F_ERR_NOTFORMATTED; 

	   if ((ptr[0]!=0xeb) && (ptr[0]!=0xe9)) return F_ERR_NOTFORMATTED;
   }

   /*ptr+=sizeof(gl_volume.bootrecord.jump_code)+sizeof(gl_volume.bootrecord.OEM_name);*/
   ptr=_getchar(gl_volume.bootrecord.jump_code,sizeof(gl_volume.bootrecord.jump_code),ptr);
   ptr=_getchar(gl_volume.bootrecord.OEM_name,sizeof(gl_volume.bootrecord.OEM_name),ptr);

   gl_volume.bootrecord.bytes_per_sector=_f_getword(ptr); ptr+=2;
   if (gl_volume.bootrecord.bytes_per_sector!=F_SECTOR_SIZE) return F_ERR_NOTSUPPSECTORSIZE;
   gl_volume.bootrecord.sector_per_cluster=*ptr++;
   gl_volume.bootrecord.reserved_sectors=_f_getword(ptr); ptr+=2;
   gl_volume.bootrecord.number_of_FATs=*ptr++;
   gl_volume.bootrecord.max_root_entry=_f_getword(ptr); ptr+=2;
   gl_volume.bootrecord.number_of_sectors_less32=_f_getword(ptr); ptr+=2;
   gl_volume.bootrecord.media_descriptor=*ptr++;
   gl_volume.bootrecord.sector_per_FAT=_f_getword(ptr); ptr+=2;
   gl_volume.bootrecord.sector_per_Track=_f_getword(ptr); ptr+=2;
   gl_volume.bootrecord.number_of_heads=_f_getword(ptr); ptr+=2;
   gl_volume.bootrecord.number_of_hidden_sectors=_f_getlong(ptr); ptr+=4;
   if (gl_volume.bootrecord.number_of_hidden_sectors<first_sector) 
   {
     gl_volume.bootrecord.number_of_hidden_sectors=first_sector;
   }
   gl_volume.bootrecord.number_of_sectors=_f_getlong(ptr); ptr+=4;


   if (gl_volume.bootrecord.sector_per_FAT==0) 
   {
#if F_FAT32
	gl_volume.bootrecord.sector_per_FAT32=_f_getlong(ptr); ptr+=4;
	gl_volume.bootrecord.extflags=_f_getword(ptr); ptr+=2;
	gl_volume.bootrecord.fsversion=_f_getword(ptr); ptr+=2;
	gl_volume.bootrecord.rootcluster=_f_getlong(ptr); ptr+=4;
	gl_volume.bootrecord.fsinfo=_f_getword(ptr); ptr+=2;
	gl_volume.bootrecord.bkbootsec=_f_getword(ptr); ptr+=2;
	ptr=_getchar(gl_volume.bootrecord.reserved,sizeof(gl_volume.bootrecord.reserved),ptr); 
	_n=28;
#else
   	gl_volume.mediatype=F_UNKNOWN_MEDIA; 
   	return F_ERR_INVALIDMEDIA;
#endif
   }

   gl_volume.bootrecord.logical_drive_num=_f_getword(ptr); ptr+=2;
   gl_volume.bootrecord.extended_signature=*ptr++;
   gl_volume.bootrecord.serial_number=_f_getlong(ptr); ptr+=4;
   ptr=_getchar(gl_volume.bootrecord.volume_name,sizeof(gl_volume.bootrecord.volume_name),ptr);
   ptr=_getchar(gl_volume.bootrecord.FAT_name,sizeof(gl_volume.bootrecord.FAT_name),ptr);
   ptr+=(448-_n);
   ptr=_getchar(gl_volume.bootrecord.executable_marker,sizeof(gl_volume.bootrecord.executable_marker),ptr);

   ret=_f_buildsectors();
   if (ret) return ret;

   if (gl_volume.bootrecord.media_descriptor!=0xf8) {     /*fixdrive*/
	   if (gl_volume.bootrecord.media_descriptor!=0xf0) { /*removable*/
		   return F_ERR_NOTFORMATTED; /*??*/
	   }
   }

   return F_NO_ERROR;
}

#else

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
unsigned char *ptr=gl_files[0]._tdata;
unsigned long maxcluster,_n;
unsigned long first_sector=0;

   gl_volume.mediatype=F_UNKNOWN_MEDIA;


   ret=_f_readsector(ptr,0);
   if (ret) return ret;


   if ((ptr[0x1fe]!=0x55) || (ptr[0x1ff]!=0xaa)) return F_ERR_NOTFORMATTED; /*??*/

   if ((ptr[0]!=0xeb) && (ptr[0]!=0xe9)) {

	   first_sector=_f_getlong(&ptr [0x08+0x1be] ); /*start sector for 1st partioon*/

       ret=_f_readsector(ptr,first_sector);
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
     ptr+=7;
   }
   else
   {
#if F_FAT32
	gl_volume.firstfat.num=_f_getlong(ptr); ptr+=8;
	gl_volume._tdata.sector=gl_volume.firstfat.sector;
	gl_volume._tdata.sector+=gl_volume.firstfat.num*gl_volume.bootrecord.number_of_FATs;
	gl_volume._tdata.num=0;
	gl_volume.bootrecord.rootcluster=_f_getlong(ptr); ptr+=27;
	gl_volume.root.num=gl_volume.bootrecord.sector_per_cluster;
	gl_volume.root.sector=((gl_volume.bootrecord.rootcluster-2)*gl_volume.root.num)+gl_volume._tdata.sector;
#else
   	return F_ERR_INVALIDMEDIA;
#endif
   }

#if (F_GETLABEL)
   ptr=_getchar(gl_volume.bootrecord.volume_name,sizeof(gl_volume.bootrecord.volume_name),ptr); /*getlabel+format*/
#endif

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
#endif




/****************************************************************************
 *
 * _f_writebootrecord
 *
 * writing boot record onto a volume, it uses number of hidden sector variable
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

#if (F_FORMATTING & F_WRITING)

static unsigned char _f_writebootrecord(void) {
unsigned char *ptr=gl_files[0]._tdata;
unsigned char *bptr=ptr;
unsigned char _n=0;

#if F_FAT32
unsigned char ret;

   if (gl_volume.mediatype==F_FAT32_MEDIA) { /*write FS_INFO*/
      unsigned short a;

      __memset(bptr,0,F_SECTOR_SIZE);

      for (a=0; a<gl_volume.bootrecord.reserved_sectors; a++) {
         ret=_f_writesector(bptr,gl_volume.bootrecord.number_of_hidden_sectors+a); /*erase reserved area*/
         if (ret) return ret;
      }

      ptr=_setlong(0x41615252,ptr); /*signature*/
      ptr=_setcharzero(480,ptr);        /*reserved*/
      ptr=_setlong(0x61417272,ptr); /*signature  */
      ptr=_setlong(0xffffffff,ptr); /*no last*/
      ptr=_setlong(0xffffffff,ptr); /*no hint*/
      ptr=_setcharzero(12,ptr);         /*reserved*/
      ptr=_setlong(0xaa550000,ptr); /*trail*/


      ret=_f_writesector(bptr,gl_volume.bootrecord.number_of_hidden_sectors+gl_volume.bootrecord.fsinfo); /*write FSINFO*/
      if (ret) return ret;

      ret=_f_writesector(bptr,gl_volume.bootrecord.number_of_hidden_sectors+gl_volume.bootrecord.fsinfo+gl_volume.bootrecord.bkbootsec); /*write FSINFO*/
      if (ret) return ret;
   }
#endif

   ptr=bptr;

   ptr=_setchar(gl_volume.bootrecord.jump_code,sizeof(gl_volume.bootrecord.jump_code),ptr);
   ptr=_setchar(gl_volume.bootrecord.OEM_name,sizeof(gl_volume.bootrecord.OEM_name),ptr);
   ptr=_setword(F_SECTOR_SIZE,ptr);
   *ptr++=gl_volume.bootrecord.sector_per_cluster;
   ptr=_setword(gl_volume.bootrecord.reserved_sectors,ptr);
   *ptr++=gl_volume.bootrecord.number_of_FATs;
   ptr=_setword(gl_volume.bootrecord.max_root_entry,ptr);
   ptr=_setword(gl_volume.bootrecord.number_of_sectors_less32,ptr);
   *ptr++=gl_volume.bootrecord.media_descriptor;
   ptr=_setword(gl_volume.bootrecord.sector_per_FAT,ptr);
   ptr=_setword(gl_volume.bootrecord.sector_per_Track,ptr);
   ptr=_setword(gl_volume.bootrecord.number_of_heads,ptr);
   ptr=_setlong(gl_volume.bootrecord.number_of_hidden_sectors,ptr);
   ptr=_setlong(gl_volume.bootrecord.number_of_sectors,ptr);

#if F_FAT32
   if (gl_volume.mediatype==F_FAT32_MEDIA) {

		ptr=_setlong(gl_volume.bootrecord.sector_per_FAT32,ptr);
		ptr=_setword(gl_volume.bootrecord.extflags,ptr);   /*?*/
		ptr=_setword(gl_volume.bootrecord.fsversion,ptr);  /*?*/
		ptr=_setlong(gl_volume.bootrecord.rootcluster,ptr); /*?*/
		ptr=_setword(gl_volume.bootrecord.fsinfo,ptr);      /*?*/
		ptr=_setword(gl_volume.bootrecord.bkbootsec,ptr);   /*?*/
		ptr=_setchar(gl_volume.bootrecord.reserved,sizeof(gl_volume.bootrecord.reserved),ptr); /*?*/
      _n=28;
   }
#endif

   {
      ptr=_setword(gl_volume.bootrecord.logical_drive_num,ptr);
      *ptr++=gl_volume.bootrecord.extended_signature;
      ptr=_setlong(gl_volume.bootrecord.serial_number,ptr);
      ptr=_setchar(gl_volume.bootrecord.volume_name,sizeof(gl_volume.bootrecord.volume_name),ptr);
      ptr=_setchar(gl_volume.bootrecord.FAT_name,sizeof(gl_volume.bootrecord.FAT_name),ptr);
      ptr=_setchar(0,448-_n,ptr); 
      ptr=_setchar(gl_volume.bootrecord.executable_marker,sizeof(gl_volume.bootrecord.executable_marker),ptr);
   }

#if F_FAT32
   if (_n)
   {
      ret=_f_writesector(bptr,gl_volume.bootrecord.number_of_hidden_sectors+gl_volume.bootrecord.bkbootsec); /*write backup*/
      if (ret) return ret;
   }
#endif
   return _f_writesector(bptr,gl_volume.bootrecord.number_of_hidden_sectors); /*write bootrecord*/
}
#endif






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
			unsigned char a;

#if F_WRITING
			gl_volume.fatmodified=0;
			gl_volume.lastalloccluster=0; 
#endif
			gl_volume.fatsector=(unsigned long)(-1);
			gl_volume.direntrysector=(unsigned long)(-1);

			for (a=0;a<F_MAXFILES;gl_files[a++].mode=F_FILE_CLOSE)
			  ;

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
#if F_GETFREESPACE_QUICK || (FATBITFIELD_ENABLE && (F_FAT16 || F_FAT32))
				gl_volume.cspace_valid=0;
#endif
#if FATBITFIELD_ENABLE && (F_FAT16 || F_FAT32)
				if (gl_volume.fatbitfield) 
				{
				  free(gl_volume.fatbitfield);
				  gl_volume.fatbitfield=NULL;
				}
				if (gl_volume.mediatype==F_FAT16_MEDIA || gl_volume.mediatype==F_FAT32_MEDIA)
				{
				  gl_volume.fatbitfield=(unsigned char *)malloc((gl_volume.firstfat.num+7)>>3);
				  if (gl_volume.fatbitfield==NULL) return F_ERR_ALLOCATION;
				  __memset(gl_volume.fatbitfield,0,(__size_t)((gl_volume.firstfat.num+7)>>3));
				}
#endif
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
 * _f_prepareformat
 *
 * preparing boot record for formatting, it sets and calculates values
 *
 * INPUTS
 *
 * phy - media physical descriptor
 * f_bootrecord - which bootrecord need to be prepare
 * number_of_hidden_sectors - where boot record starts
 * fattype - one of this definitions F_FAT12_MEDIA,F_FAT16_MEDIA,F_FAT32_MEDIA
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

#if (F_FORMATTING & F_WRITING)

static unsigned char _f_prepareformat(F_PHY *phy,unsigned long number_of_hidden_sectors,unsigned char fattype) {
	if (!phy->number_of_sectors) return F_ERR_INVALIDSECTOR;

	gl_volume.bootrecord.jump_code[0]=0xeb;
	gl_volume.bootrecord.jump_code[1]=0x3c;
	gl_volume.bootrecord.jump_code[2]=0x90;

	__memcpy(gl_volume.bootrecord.OEM_name,"MSDOS5.0",8);
	gl_volume.bootrecord.bytes_per_sector=F_SECTOR_SIZE;

	gl_volume.bootrecord.number_of_FATs=2;

#if F_FAT32
	if (fattype==F_FAT32_MEDIA) {
		gl_volume.bootrecord.reserved_sectors=32+4; 
   		gl_volume.bootrecord.max_root_entry=0;
	}
	else
#endif
	{
		gl_volume.bootrecord.reserved_sectors=1; 
   		gl_volume.bootrecord.max_root_entry=512;
	}

	gl_volume.bootrecord.media_descriptor=0xf0;  /*0xf0 for removable. 0xf8 for fix*/

	gl_volume.bootrecord.sector_per_Track=phy->sector_per_track;
	gl_volume.bootrecord.number_of_heads=phy->number_of_heads;

	gl_volume.bootrecord.number_of_hidden_sectors=number_of_hidden_sectors; /*if this is not partioned*/
	gl_volume.bootrecord.logical_drive_num=0;

	gl_volume.bootrecord.extended_signature=0x29;

 	__memcpy(gl_volume.bootrecord.volume_name,"NO NAME    ",11);

	gl_volume.bootrecord.executable_marker[0]=0x55;
	gl_volume.bootrecord.executable_marker[1]=0xaa;

	gl_volume.bootrecord.serial_number=f_getrand(gl_volume.bootrecord.serial_number^phy->number_of_sectors);

	if (phy->number_of_sectors<0x10000 && (fattype==F_FAT16_MEDIA || fattype==F_FAT12_MEDIA))
        {
	  gl_volume.bootrecord.number_of_sectors_less32=(unsigned short)(phy->number_of_sectors);
	}
	else 
	{
 	  gl_volume.bootrecord.number_of_sectors_less32=0;
	  gl_volume.bootrecord.number_of_sectors=phy->number_of_sectors;
	}


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

   		roots=(gl_volume.bootrecord.max_root_entry*sizeof(F_DIRENTRY))/F_SECTOR_SIZE;

	switch(fattype)
	{
#if F_FAT32
		case F_FAT32_MEDIA:
		      {
			unsigned long _n=(unsigned long)(128*secpercl+nfat);
   			fatsec=(phy->number_of_sectors-gl_volume.bootrecord.reserved_sectors+2*secpercl);
			fatsec+=(_n-1);
			fatsec/=_n;

			gl_volume.bootrecord.sector_per_FAT32=fatsec;
			gl_volume.bootrecord.sector_per_FAT=0;

			gl_volume.bootrecord.extflags=0;
   			gl_volume.bootrecord.fsversion=0;
	   		gl_volume.bootrecord.rootcluster=2;
   			gl_volume.bootrecord.fsinfo=1;
   			gl_volume.bootrecord.bkbootsec=6;

   			__memset(gl_volume.bootrecord.reserved,0,sizeof(gl_volume.bootrecord.reserved));
		      }
		      break;
#endif
#if F_FAT16
		case F_FAT16_MEDIA:
		      {
            		unsigned long _n=(unsigned long)(256*secpercl+nfat);
   			fatsec=(phy->number_of_sectors-gl_volume.bootrecord.reserved_sectors-roots+2*secpercl);
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
   			fatsec=(phy->number_of_sectors-gl_volume.bootrecord.reserved_sectors-roots+2*secpercl);
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
 *
 * fattype - one of this definitions F_FAT12_MEDIA,F_FAT16_MEDIA,F_FAT32_MEDIA
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

#if (F_FORMATTING & F_WRITING)

static unsigned char _f_postformat(unsigned char fattype) {
unsigned long a;

unsigned char ret;

	ret=_f_buildsectors(); /*get positions*/
	if (ret) return ret;

	if (gl_volume.mediatype!=fattype) return F_ERR_MEDIATOOSMALL;

	switch (gl_volume.mediatype)
	{
#if F_FAT12
		case F_FAT12_MEDIA: __memcpy(gl_volume.bootrecord.FAT_name,"FAT12   ",8); break;
#endif
#if F_FAT16
		case F_FAT16_MEDIA: __memcpy(gl_volume.bootrecord.FAT_name,"FAT16   ",8); break;
#endif
#if F_FAT32
		case F_FAT32_MEDIA: __memcpy(gl_volume.bootrecord.FAT_name,"FAT32   ",8); break;
#endif
		default: return F_ERR_INVALIDMEDIA;
	}

	gl_volume.fatmodified=0;
	gl_volume.fatsector=(unsigned long)(-1);
	gl_volume.direntrysector=(unsigned long)(-1);

        {
          unsigned char *ptr=gl_files[0]._tdata;
          unsigned char j=2;
          unsigned long i;

          __memset(ptr,0,F_SECTOR_SIZE);
          
	  switch (gl_volume.mediatype)
	  {
#if F_FAT16
	    case F_FAT16_MEDIA: j=3; break;
#endif
#if F_FAT32
	    case F_FAT32_MEDIA: j=7; break;
#endif
	  }

          *ptr=gl_volume.bootrecord.media_descriptor;
          __memset(ptr+1,0xff,(__size_t)j);
          (void)_f_writesector(ptr,gl_volume.firstfat.sector);
          (void)_f_writesector(ptr,gl_volume.firstfat.sector+gl_volume.firstfat.num);
          __memset(ptr,0,(__size_t)(j+1));
          
          for (i=1;i<gl_volume.firstfat.num;i++)
          {
            (void)_f_writesector(ptr,gl_volume.firstfat.sector+i);
            (void)_f_writesector(ptr,gl_volume.firstfat.sector+i+gl_volume.firstfat.num);
          }
        }


#if F_FAT32
	if (gl_volume.mediatype==F_FAT32_MEDIA) {
		ret=_f_setclustervalue(gl_volume.bootrecord.rootcluster,F_CLUSTER_LAST);
		if (ret) return ret;
		ret=_f_writefatsector();
		if (ret) return ret;
	}
#endif


 	for (a=0; a<gl_volume.root.num;a++) { /*reset root direntries*/
  		ret = _f_writesector(gl_files[0]._tdata,gl_volume.root.sector+a);
  		if (ret) return ret;
	}

	return _f_writebootrecord();
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
 *
 * fattype - one of this definitions F_FAT12_MEDIA,F_FAT16_MEDIA,F_FAT32_MEDIA
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

#if (F_FORMATTING & F_WRITING)

unsigned char fn_hardformat(unsigned char fattype) {
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

	ret=_f_prepareformat(&phy,0,fattype); /*no partition*/
	if (ret) return ret;

	return _f_postformat(fattype);
}

#endif

/****************************************************************************
 *
 * fn_format
 *
 * format a media, 1st it checks existing formatting, then master boot record,
 * then media physical
 *
 * INPUTS
 *
 * fattype - one of this definitions F_FAT12_MEDIA,F_FAT16_MEDIA,F_FAT32_MEDIA
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

#if (F_FORMATTING & F_WRITING)

unsigned char fn_format(unsigned char fattype) {
unsigned char ret;
F_PHY phy;

	ret=_f_getvolume();
	if (ret && ret!=F_ERR_NOTFORMATTED) return ret;

	gl_volume.state=F_STATE_NEEDMOUNT;

	if (!ret) { /*we have existing formating*/

		phy.number_of_heads=gl_volume.bootrecord.number_of_heads;
		phy.sector_per_track=gl_volume.bootrecord.sector_per_Track;

		phy.number_of_sectors=gl_volume.bootrecord.number_of_sectors_less32;
		if (!phy.number_of_sectors) phy.number_of_sectors=gl_volume.bootrecord.number_of_sectors;

		
		ret=_f_prepareformat(&phy,gl_volume.bootrecord.number_of_hidden_sectors,fattype); 

		if (ret) return ret;
	}
	else if (ret==F_ERR_NOTFORMATTED) { /*we dont have existing formatting*/
		unsigned char *ptr=gl_files[0]._tdata;
		unsigned long startsec=0;

		__memset(&gl_volume.bootrecord,0,sizeof(F_BOOTRECORD));
		__memset(&phy,0,sizeof(F_PHY));

#ifndef drv_getphy
		return F_ERR_INITFUNC;
#else
		ret=drv_getphy(&phy);
		if (ret) return F_ERR_ONDRIVE;
#endif

	   ret=_f_readsector(ptr,0); /*check MBR*/
		if (ret) return ret;

		if ((ptr[0x1fe]==0x55) && (ptr[0x1ff]==0xaa)) { /*check valid*/
			if ((ptr[0]!=0xeb) && (ptr[0]!=0xe9)) {   /*check MBR*/
				if (ptr[0x1be]==0x80) {				  /*check active*/
					startsec=_f_getlong(&ptr[0x08+0x1be]); /*start sector for 1st partioon*/
					phy.number_of_sectors=_f_getlong(&ptr[0x0c+0x1be]); /*number of sectors*/
				}
			}
		}

		ret=_f_prepareformat(&phy,startsec,fattype); /*no partition*/
		if (ret) return ret;
	}

	return _f_postformat(fattype);
}

#endif


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
#if F_GETFREESPACE_QUICK || (FATBITFIELD_ENABLE && (F_FAT16 || F_FAT32))
unsigned char fn_getfreespace(F_SPACE *pspace) 
{ 
  unsigned char ret;
  unsigned long sector;
  unsigned long i;

  ret=_f_getvolume();
  if (ret) return ret;

  if (gl_volume.cspace_valid==0)
  {
#if FATBITFIELD_ENABLE
    unsigned short f=0;
#endif
    unsigned char *cls;
    unsigned long value;
    unsigned long a;
    unsigned short b;

    __memset(pspace,0,sizeof(F_SPACE));
  
    sector=0;
    i=gl_volume.maxcluster;
    switch (gl_volume.mediatype)
    {
#if F_FAT16
      case F_FAT16_MEDIA:
      {
        for (a=0; a<gl_volume.firstfat.num; a++,sector++) 
        {
          ret=_f_getfatsector(sector);
          if (ret) return ret;
          cls=gl_volume.fat;
          if (a==0) { b=2; cls+=4; }
               else b=0;
          for (; b<(F_SECTOR_SIZE/2); b++) 
          {
            if (!i--) break;
            value=_f_getword(cls);
            cls+=2;
            if (!value) 
            {
#if FATBITFIELD_ENABLE
              ++f;
#endif
              ++pspace->free; 
            }
            else if (value==(unsigned short)F_CLUSTER_BAD) ++pspace->bad;
            else ++pspace->used;
          }
#if FATBITFIELD_ENABLE
          if (!f) _SET_BIT(gl_volume.fatbitfield,sector);
#endif
        }
      }
      break;
#endif
#if F_FAT12
      case F_FAT12_MEDIA:
      {
        unsigned short a;
        for (a=0; a<i; a++) 
        {
          unsigned long value;
          ret=_f_getclustervalue((unsigned short)(a+2),&value);
          if (ret) return ret;
          if (!value) ++pspace->free;
          else if (value==F_CLUSTER_BAD) ++pspace->bad;
          else ++pspace->used;
        }
      }
      break;
#endif
#if F_FAT32
      case F_FAT32_MEDIA:
      {
        for (a=0; a<gl_volume.firstfat.num; a++,sector++) 
        {
          ret=_f_getfatsector(sector);
          if (ret) return ret;
          cls=gl_volume.fat;
          if (a==0) { b=2; cls+=8; }
               else b=0;
          for (; b<(F_SECTOR_SIZE/4); b++) 
          {
            if (!i--) break; 
            value=_f_getlong(cls);
            cls+=4;
            value&=0x0fffffff;
            if (!value)
            {
#if FATBITFIELD_ENABLE
              ++f;
#endif
              ++pspace->free;
            }
            else if (value==F_CLUSTER_BAD) ++pspace->bad;
            else ++pspace->used;
          }
#if FATBITFIELD_ENABLE
          if (!f) _SET_BIT(gl_volume.fatbitfield,sector);
#endif
        }
      }
      break;
#endif
      default: return F_ERR_INVALIDMEDIA;
    }
    gl_volume.cspace.free=pspace->free;
    gl_volume.cspace.used=pspace->used;
    gl_volume.cspace.bad=pspace->bad;
    gl_volume.cspace_valid=1;
  }
  else
  {
    pspace->free=gl_volume.cspace.free;
    pspace->used=gl_volume.cspace.used;
    pspace->bad=gl_volume.cspace.bad;
  }

  i=(unsigned long)((unsigned long)gl_volume.bootrecord.sector_per_cluster*F_SECTOR_SIZE);
  pspace->total=gl_volume.maxcluster*i;
  pspace->free*=i;
  pspace->used*=i;
  pspace->bad*=i;

  return F_NO_ERROR;
}
#else
unsigned char fn_getfreespace(F_SPACE *pspace) { 
unsigned char ret;
unsigned long a;


	ret=_f_getvolume();
	if (ret) return ret;

   	__memset(pspace,0,sizeof(F_SPACE));
	pspace->total=gl_volume.maxcluster;

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
#endif


/****************************************************************************
 *
 * fn_getlabel
 *
 * get a label of a media
 *
 * INPUTS
 * label - char pointer where to store label
 * len - length of label buffer
 *
 * RETURNS
 * error code or zero if successful
 *
 ***************************************************************************/
#if F_GETLABEL

#if F_LONGFILENAME
unsigned char fn_getlabel(char *label, unsigned char len) {
char tmplabel[11];
long a;
unsigned char ret;
F_FILE *file;

	if (!len) return F_NO_ERROR; /*how to store volume name?*/

	ret=_f_getvolume();
	if (ret) return ret;

	__memcpy(tmplabel,gl_volume.bootrecord.volume_name,11);

	file=_f_opendir(0); /*open root;*/
	if (!file) return F_ERR_READ;

	while (1) {
		F_DIRENTRY de;

		ret=_f_readdir(&de,file);
		if (ret==F_ERR_EOF) break;
		if (ret) break;

   		if (!de.name[0]) continue;						 /*empty*/
	   	if ((unsigned char)(de.name[0])==0xe5) continue; /*deleted*/

		if (de.attr == F_ATTR_VOLUME) {
			__memcpy(tmplabel,de.name,8);
   			__memcpy(tmplabel+8,de.ext,3);
			break;
   		}

	}

	ret=_f_closedir(file,F_NO_ERROR);
	if (ret) return ret;

	len--;
	for (a=0; a<11 && a<len; a++) {
		label[a]=tmplabel[a];
	}
	label [a]=0; /*terminate it, this is ok because of len--!*/

	for (;a>=0; a--) { /*cut the spaces*/
		if (label[a]<=32) label[a]=0;
		else break;
	}

	return F_NO_ERROR;
}
#else
unsigned char fn_getlabel(char *label, unsigned char len) {
char tmplabel[11];
unsigned char a;
F_POS pos;
unsigned char ret;
	if (!len) return F_NO_ERROR; /*how to store volume name?*/

	ret=_f_getvolume();
	if (ret) return ret;

	_f_clustertopos(0,&pos);
	__memcpy(tmplabel,gl_volume.bootrecord.volume_name,11);

	while (pos.cluster<F_CLUSTER_RESERVED) {
	   	for (;pos.sector<pos.sectorend;pos.sector++) {
   			F_DIRENTRY *de=(F_DIRENTRY*)(gl_volume.direntry);

	   		 ret=_f_getdirsector(pos.sector);
	   		 if (ret) return ret;

	   		 for (pos.pos=0; pos.pos<F_SECTOR_SIZE/sizeof(F_DIRENTRY);de++,pos.pos++) {

   				if (!de->name[0]) continue;						 /*empty*/
	   			if ((unsigned char)(de->name[0])==0xe5) continue; /*deleted*/
		   		if (de->attr == F_ATTR_VOLUME) {
			   		__memcpy(tmplabel,de->name,8);
   					__memcpy(tmplabel+8,de->ext,3);
   					goto vege;
   				}
   			 }
	   	}

		if (!pos.cluster) {
#if F_FAT32
			if (gl_volume.mediatype==F_FAT32_MEDIA) {
				pos.cluster=gl_volume.bootrecord.rootcluster;
			}
			else 
#endif
			goto vege;
		}

		{
			unsigned long nextcluster;
			if (_f_getclustervalue(pos.cluster,&nextcluster)) goto vege;

	   		if (nextcluster>=F_CLUSTER_RESERVED) goto vege;

			_f_clustertopos(nextcluster,&pos);
		}
  	}

vege:
	len--;
	for (a=0; a<11 && a<len; a++) 
	{
		label[a]=tmplabel[a];
	}
	label[a]=0; /*terminate it, this is ok because of len--!*/

	for (;a>0; a--) { /*cut the spaces*/
		if (label[a-1]<=32) label[a-1]=0;
		else break;
	}

	return F_NO_ERROR;
}
#endif
#endif


/****************************************************************************
 *
 * fn_setlabel
 *
 * set a label of a media
 *
 * INPUTS
 * label - new label for the media
 *
 * RETURNS
 * error code or zero if successful
 *
 ***************************************************************************/
#if (F_SETLABEL & F_WRITING)

#if F_LONGFILENAME
unsigned char fn_setlabel(const char *label) {
unsigned char ret;
char tmplabel[11];
long a,b;
F_FILE *file;
F_POS pos;
F_NAME fsname;
F_DIRENTRY *pde;

	ret=_f_getvolume();
	if (ret) return ret;

	for (a=b=0; a<11; a++) {
		char ch=_f_toupper(label[b]);
		if (ch>=32) b++;
		       else ch=32;
		tmplabel[a]=ch;
	}

	file=_f_opendir(0); /*open root;*/
	if (!file) return F_ERR_READ;

	while (1) {
		F_DIRENTRY de;

		ret=_f_readdir(&de,file);
		if (ret==F_ERR_EOF) break;
		if (ret) return _f_closedir(file,ret);

   		if (!de.name[0]) continue;						 /*empty*/
	   	if ((unsigned char)(de.name[0])==0xe5) continue; /*deleted*/

		if (de.attr == F_ATTR_VOLUME) {
			__memcpy(de.name,tmplabel,8);
			__memcpy(de.ext,tmplabel+8,3);

			ret=_f_seekdir(file,-1,F_SEEK_CUR);
			if (ret) return _f_closedir(file,ret);

			ret=_f_writedir(&de,file);
			if (ret) return _f_closedir(file,ret);

			return _f_closedir(file,F_NO_ERROR);
   		}

	}

	ret=_f_closedir(file,F_NO_ERROR);
	if (ret) return ret;

	_f_clustertopos(0,&pos);

	fsname.path[0]=0;
	fsname.lname[0]='X';  /*for dummy*/
	fsname.lname[1]=0;

	if (_f_addentry(&fsname,&pos,&pde)) return F_ERR_NOMOREENTRY;  /*couldnt be added*/

	_f_initentry(pde,tmplabel,tmplabel+8);
	pde->attr=F_ATTR_VOLUME;

	return _f_writedirsector();
}

#else	/* F_LONGFILENAME */

unsigned char fn_setlabel(const char *label) {
F_NAME fsname;
unsigned char ret;
char tmplabel[11];
unsigned char a,b;
F_POS pos;
F_DIRENTRY *de;

	ret=_f_getvolume();
	if (ret) return ret;

	_f_clustertopos(0,&pos);

	for (a=b=0; a<11; a++) {
		char ch=_f_toupper(label[b]);

		if (ch>=32) b++;
		else ch=32;

		tmplabel[a]=ch;
	}

	while (pos.cluster<F_CLUSTER_RESERVED) {
	   	for (;pos.sector<pos.sectorend;pos.sector++) {
			de=(F_DIRENTRY*)(gl_volume.direntry);

			ret=_f_getdirsector(pos.sector);
			if (ret) return ret;

			for (pos.pos=0; pos.pos<F_SECTOR_SIZE/sizeof(F_DIRENTRY);de++,pos.pos++) {

   				if (!de->name[0]) continue;						 /*empty*/
	   			if ((unsigned char)(de->name[0])==0xe5) continue; /*deleted*/
		   		if (de->attr == F_ATTR_VOLUME) {
					__memcpy(de->name,tmplabel,8);
					__memcpy(de->ext,tmplabel+8,3);
					return _f_writedirsector();
				}
			}
   		}

		if (!pos.cluster) {
#if F_FAT32
			if (gl_volume.mediatype==F_FAT32_MEDIA) {
				pos.cluster=gl_volume.bootrecord.rootcluster;
			}
			else 
#endif
			break;
		}

		{
			unsigned long nextcluster;
			if (_f_getclustervalue(pos.cluster,&nextcluster)) break;

	   		if (nextcluster>=F_CLUSTER_RESERVED) break;

			_f_clustertopos(nextcluster,&pos);
		}
  	}

	_f_clustertopos(0,&pos);

   __memcpy(fsname.filename,tmplabel,F_MAXNAME);
   __memcpy(fsname.fileext,tmplabel+F_MAXNAME,F_MAXEXT);

	if (_f_addentry(&fsname,&pos,&de)) return F_ERR_NOMOREENTRY;  /*couldnt be added*/

	de->attr=F_ATTR_VOLUME;
	return _f_writedirsector();
}
#endif

#endif




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

	__memset(&gl_volume,0,sizeof(gl_volume));
	gl_volume.state=F_STATE_NONE;

#ifdef drv_initfunc
   	if (drv_initfunc()) return F_ERR_INITFUNC;
#endif

	gl_volume.state=F_STATE_NEEDMOUNT;

#if FATBITFIELD_ENABLE && (F_FAT16 || F_FAT32)
        gl_volume.fatbitfield=NULL;
#endif

	return _f_getvolume();
}


/****************************************************************************
 *
 * fn_delvolume
 *
 ***************************************************************************/
unsigned char fn_delvolume (void)
{
#if FATBITFIELD_ENABLE && (F_FAT16 || F_FAT32)
	if (gl_volume.fatbitfield) free(gl_volume.fatbitfield); 
#endif
	return 0;
}

