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

#include <hcc/fatfs_sthin/fat.h>
#include <hcc/fatfs_sthin/util.h>
#include <hcc/fatfs_sthin/volume.h>
#include "drv.h"



/****************************************************************************
 *
 * _f_writefatsector
 *
 * writing fat sector into volume, this function check if fat was modified
 * and writes data
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if F_WRITING
unsigned char _f_writefatsector(void) {
unsigned char a;
   if (gl_volume.modified) {
      unsigned long fatsector=gl_volume.firstfat.sector+gl_volume.fatsector;

      if (gl_volume.fatsector>=gl_volume.firstfat.num) return F_ERR_INVALIDSECTOR; 

      for (a=0; a<gl_volume.bootrecord.number_of_FATs; a++) 
      {
	unsigned char ret;
	ret=_f_writeglsector(fatsector);
	if (ret) return ret;
	fatsector+=gl_volume.firstfat.num;
      }

      gl_volume.modified=0;
   }

   return F_NO_ERROR;
}
#endif



/****************************************************************************
 *
 * _f_getfatsector
 *
 * read a fat sector from media
 *
 * INPUTS
 *
 * sector - which fat sector is needed, this sector number is zero based
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
unsigned char _f_getfatsector(unsigned long sector) {
unsigned char a;
   if (gl_volume.fatsector!=sector) 
   {
      unsigned long fatsector;

      gl_volume.fatsector=sector;

      if (gl_volume.fatsector>=gl_volume.firstfat.num) {
         return F_ERR_INVALIDSECTOR;
      }

      fatsector=gl_volume.firstfat.sector+gl_volume.fatsector;

      for (a=0; a<gl_volume.bootrecord.number_of_FATs; a++) 
      {
        if (!_f_readglsector(fatsector)) return F_NO_ERROR;
        fatsector+=gl_volume.firstfat.num;
      }
      return F_ERR_READ;
   }

   return F_NO_ERROR;
}



/****************************************************************************
 *
 * _f_setclustervalue
 *
 * set a cluster value in the FAT
 *
 * INPUTS
 *
 * cluster - which cluster's value need to be modified
 * data - new value of the cluster
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if F_WRITING
unsigned char _f_setclustervalue(unsigned long cluster, unsigned long _tdata) {
unsigned char ret;

	switch (gl_volume.mediatype)
	{
#if F_FAT16
	  case F_FAT16_MEDIA:
	      {
		unsigned long sector=cluster;
		unsigned short s_data=(unsigned short)(_tdata & 0xffff); /*keep 16 bit only*/

		sector/=(F_SECTOR_SIZE/2);
		cluster-=sector*(F_SECTOR_SIZE/2);
		
		ret=_f_getfatsector(sector);
		if (ret) return ret;
		
		if (_f_getword(&gl_sector[cluster<<1])!=s_data) 
		{
		  _f_setword(&gl_sector[cluster<<1],s_data);
		  gl_volume.modified=1;
		}
              }
	      break;
#endif
#if F_FAT12
	  case F_FAT12_MEDIA:
	      {
		unsigned char f12new[2];
		unsigned long sector=cluster;
		unsigned short pos;
		unsigned short s_data=(unsigned short)(_tdata & 0x0fff); /*keep 12 bit only*/

		if (cluster&1) s_data <<= 4;

		_f_setword(f12new,s_data); /*create new data*/

		sector+=sector/2;		  /*1.5 bytes*/
		pos=(unsigned short)(sector%F_SECTOR_SIZE);
 	        sector/=F_SECTOR_SIZE;

		ret=_f_getfatsector(sector);
		if (ret) return ret;

		if (cluster&1) 
		{
		  f12new[0]|=gl_sector[pos]&0x0f;
		}

		if (gl_sector[pos]!=f12new[0]) {
			gl_sector[pos]=f12new[0];
			gl_volume.modified=1;
		}

		pos++;
		if (pos>=512) 
		{
		  ret=_f_getfatsector(sector+1);
		  if (ret) return ret;
		  pos=0;
		}

		if (!(cluster&1)) 
		{
		  f12new[1] |= gl_sector[pos] & 0xf0;
		}

		if (gl_sector[pos]!=f12new[1]) {
			gl_sector[pos]=f12new[1];
			gl_volume.modified=1;
		}
	      }
	      break;
#endif
#if F_FAT32
	  case F_FAT32_MEDIA:
	      {
		unsigned long sector=cluster;
		unsigned long oldv;

	        sector/=(F_SECTOR_SIZE/4);
		cluster-=sector*(F_SECTOR_SIZE/4);

		ret=_f_getfatsector(sector);
		if (ret) return ret;

		oldv=_f_getlong(&gl_sector[cluster<<1]);

		_tdata&=0x0fffffff;
		_tdata|=oldv & 0xf0000000; /*keep 4 top bits*/

		if (_tdata!=oldv) {
			_f_setlong(&gl_sector[cluster<<2],_tdata);
			gl_volume.modified=1;
		}
	      }
	      break;
#endif
	  default: return F_ERR_INVALIDMEDIA;
	}

	return F_NO_ERROR;
}
#endif


/****************************************************************************
 *
 * _f_getclustervalue
 *
 * get a cluster value from FAT
 *
 * INPUTS
 *
 * cluster - which cluster value is requested
 * pvalue - where to store data 
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
unsigned char _f_getclustervalue(unsigned long cluster,unsigned long *pvalue) {
unsigned long val;
unsigned char ret;
	switch(gl_volume.mediatype)
	{
#if F_FAT16
	  case F_FAT16_MEDIA:
	      {
		unsigned long sector=cluster;
	        sector/=(F_SECTOR_SIZE/2);
		cluster-=sector*(F_SECTOR_SIZE/2);

		ret=_f_getfatsector(sector);
		if (ret) return ret;

		val=_f_getword(&gl_sector[cluster<<1]);
		if (val>=(F_CLUSTER_RESERVED&0xffff)) val|=0x0ffff000;   /*extends it*/

		if (pvalue) *pvalue=val;
	      }
	      break;
#endif
#if F_FAT12
	  case F_FAT12_MEDIA:
	      {
		unsigned char dataf12[2];
		unsigned long sector=cluster;
		unsigned short pos;

		sector+=sector/2;		  /*1.5 bytes*/
		pos=(unsigned short)(sector%F_SECTOR_SIZE);
	        sector/=F_SECTOR_SIZE;

		ret=_f_getfatsector(sector);
		if (ret) return ret;

		dataf12[0]=gl_sector[pos++];

		if (pos>=512) {
			ret=_f_getfatsector(sector+1);
			if (ret) return ret;
			pos=0;
		}

		dataf12[1]=gl_sector[pos];

		val=_f_getword(dataf12);

		if (cluster&1) {
			val = val >> 4;
		}
		else {
			val = val & 0xfff;
		}

		if (val>=(F_CLUSTER_RESERVED&0xfff)) val|=0x0ffff000;   /*extends it*/
		
		if (pvalue) *pvalue=val;
	      }
	      break;
#endif
#if F_FAT32
	  case F_FAT32_MEDIA:
	      {
		unsigned long sector=cluster;
	        sector/=(F_SECTOR_SIZE/4);
		cluster-=sector*(F_SECTOR_SIZE/4);

		ret=_f_getfatsector(sector);
		if (ret) return ret;

		if (pvalue) *pvalue=_f_getlong(&gl_sector[cluster<<2]) & 0x0fffffff; /*28bit*/
	      }
	      break;
#endif
	  default: return F_ERR_INVALIDMEDIA;
	}
  return(F_NO_ERROR);
}






/****************************************************************************
 *
 * _f_clustertopos
 *
 * convert a cluster position into physical sector position
 *
 * INPUTS
 *
 * cluster - original cluster position
 * pos - position structure to fills the position
 *
 ***************************************************************************/
void _f_clustertopos(unsigned long cluster, F_POS *pos) {
 	pos->cluster=cluster;

	if (!cluster) 
	{
		pos->sector=gl_volume.root.sector;
		pos->sectorend=pos->sector+gl_volume.root.num;
	}
	else 
	{
	 	unsigned long sectorcou=gl_volume.bootrecord.sector_per_cluster;
	 	pos->sector=(pos->cluster-2)*sectorcou+gl_volume._tdata.sector;
		pos->sectorend=pos->sector+sectorcou;
	}

	if (cluster>=F_CLUSTER_RESERVED) {
		pos->sectorend=0;
	}

	pos->pos=0;
}




/****************************************************************************
 *
 * _f_getcurrsector
 *
 * read current sector according in file structure
 *
 * INPUTS
 * f - internal file pointer
 *
 * RETURNS
 * error code or zero if successful
 *
 ***************************************************************************/
unsigned char _f_getcurrsector(void) {
unsigned char ret;
unsigned long cluster;

	if (gl_file.pos.sector==gl_file.pos.sectorend)
	{
		gl_volume.fatsector=(unsigned long)-1;
		ret=_f_getclustervalue(gl_file.pos.cluster,&cluster);
		if (ret) return ret;
		if (cluster>=F_CLUSTER_RESERVED) return F_ERR_EOF;
		gl_file.prevcluster=gl_file.pos.cluster;
		_f_clustertopos(cluster,&gl_file.pos);
	}

	return _f_readglsector(gl_file.pos.sector);
}



/****************************************************************************
 *
 * _f_alloccluster
 *
 * allocate cluster from FAT
 *
 * INPUTS
 * pcluster - where to store the allocated cluster number
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if F_WRITING

unsigned char _f_alloccluster(unsigned long *pcluster) {
unsigned long maxcluster=gl_volume.maxcluster;
unsigned long cou;
unsigned long cluster=gl_volume.lastalloccluster;
unsigned long value;
unsigned char ret;

	for (cou=0; cou<maxcluster; cou++) {

		if (cluster>=maxcluster) cluster=0;

		ret=_f_getclustervalue(cluster,&value);
		if (ret) return ret;

		if (!value) {
			gl_volume.lastalloccluster=cluster+1; /*set next one*/
			*pcluster=cluster;

			return F_NO_ERROR;
		}

		cluster++;
	}

	return F_ERR_NOMOREENTRY;
}

#endif



/****************************************************************************
 *
 * _f_removechain
 *
 * remove cluster chain from fat
 *
 * INPUTS
 * cluster - first cluster in the cluster chain
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if F_WRITING

unsigned char _f_removechain(unsigned long cluster) {

	gl_volume.fatsector=(unsigned long)-1;

	if (cluster<gl_volume.lastalloccluster) { /*this could be the begining of alloc*/
		gl_volume.lastalloccluster=cluster;
	}

	while (cluster<F_CLUSTER_RESERVED && cluster>=2) {
		unsigned long nextcluster;
		
		unsigned char ret=_f_getclustervalue(cluster,&nextcluster);
		if (ret) return ret;

		ret=_f_setclustervalue(cluster,F_CLUSTER_FREE);
		if (ret) return ret;

		cluster=nextcluster;
	}	

	return _f_writefatsector();
}

#endif



