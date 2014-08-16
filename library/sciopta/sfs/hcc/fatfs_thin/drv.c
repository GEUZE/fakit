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
#include "drv.h"
#include <hcc/fatfs_thin/util.h>
#include <hcc/fatfs_thin/volume.h>


/****************************************************************************
 *
 * _f_checkstatus
 *
 * checking a volume driver status, if media is removed or has been changed
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

unsigned char _f_checkstatus(void) {

#ifdef drv_getstatus

	if (drv_getstatus() & (F_ST_MISSING|F_ST_CHANGED)) {
		gl_volume.state=F_STATE_NEEDMOUNT; /*card has been removed;*/
		return F_ERR_CARDREMOVED;
	}

#endif

	return F_NO_ERROR;
}


/****************************************************************************
 *
 * _f_writesector
 *
 * write sector data on a volume, it calls low level driver function, it
 * writes a complete sector
 *
 * INPUTS
 *
 * data -  pointer where the data is
 * sector - which physical sector
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

#if F_WRITING

unsigned char _f_writesector(void *_tdata, unsigned long sector) 
{
  unsigned char ret;
  unsigned char retry;

#ifndef drv_writesector
   gl_volume.state=F_STATE_NEEDMOUNT; /*no write function*/
   return F_ERR_ACCESSDENIED;
#endif


#ifdef drv_getstatus
   {
      unsigned char status=drv_getstatus();

      if (status & (F_ST_MISSING|F_ST_CHANGED)) 
      {
        gl_volume.state=F_STATE_NEEDMOUNT; /*card has been removed;*/
        return F_ERR_CARDREMOVED;
      }

      if (status & (F_ST_WRPROTECT)) 
      {
         gl_volume.state=F_STATE_NEEDMOUNT; /*card has been removed;*/
         return F_ERR_WRITEPROTECT;
      }
   }
#endif

#ifdef drv_writesector
   for (retry=3; retry; retry--) {

   	ret=drv_writesector(_tdata,sector);
   	if (!ret) return F_NO_ERROR;

   	if (ret==(unsigned char)-1) 
   	{
      gl_volume.state=F_STATE_NEEDMOUNT; /*card has been removed;*/
      return F_ERR_CARDREMOVED;
   	}
   	else
   	{
      ret=_f_checkstatus();
      if (ret) return ret;
   	}
   }
#endif

	return F_ERR_ONDRIVE;
}

#endif

/****************************************************************************
 *
 * _f_readsector
 *
 * read sector data from a volume, it calls low level driver function, it
 * reads a complete sector
 *
 * INPUTS
 *
 * data - data pointer where store data
 * sector - which physical sector is read
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

unsigned char _f_readsector(void *_tdata, unsigned long sector) 
{
  unsigned char retry;
  unsigned char ret;

#ifndef drv_readsector
   gl_volume.state=F_STATE_NEEDMOUNT; /*no write function*/
   return F_ERR_ACCESSDENIED;
#endif

	ret=_f_checkstatus();
	if (ret) return ret;

#ifdef drv_readsector
   for (retry=3; retry; retry--) 
   {
   	 ret=drv_readsector(_tdata,sector);
   	 if (!ret) return F_NO_ERROR;

     if (ret==(unsigned char)-1) 
     {
       gl_volume.state=F_STATE_NEEDMOUNT; /*card has been removed;*/
       return F_ERR_CARDREMOVED;
   	 }
     else
     {
       ret=_f_checkstatus();
       if (ret) return ret;
     }
   }
#endif

	return F_ERR_ONDRIVE;
}

