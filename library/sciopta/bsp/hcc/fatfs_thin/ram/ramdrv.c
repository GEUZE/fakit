#ifndef _RAMDRV_C_
#define _RAMDRV_C_

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

#include "../thin_usr.h"

#include "ramdrv.h"

#define RAMDRV_SIZE 0x8000          /*defintion for size of ramdrive*/

#define RAMDRV_MAXSECTOR (RAMDRV_SIZE / F_SECTOR_SIZE)

static char ramdrv[RAMDRV_SIZE];      /*local variable, filesystem is here*/

/****************************************************************************
 *
 * ram_readsector
 *
 * read one sector from ramdrive
 *
 * INPUTS
 *
 * data - data pointer where to store data
 * sector - where to read data from
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

unsigned char ram_readsector(void *_data, unsigned long sector) {
unsigned int len;
char *d=(char*)_data;
char *s=ramdrv;

   if (sector>=RAMDRV_MAXSECTOR) return 1;

   s+=sector*F_SECTOR_SIZE;
   len=F_SECTOR_SIZE;

   while (len--) {
      *d++=*s++;
   }

   return 0;
} 

/****************************************************************************
 *
 * ram_writesector
 *
 * write one sector into ramdrive
 *
 * INPUTS
 *
 * data - data pointer 
 * sector - where to write data
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

unsigned char ram_writesector(void *_data, unsigned long sector) {
unsigned int len;
char *s=(char*)_data;
char *d=ramdrv;

   if (sector>=RAMDRV_MAXSECTOR) return 1;

   d+=sector*F_SECTOR_SIZE;
   len=F_SECTOR_SIZE;

   while (len--) {
      *d++=*s++;
   }

   return 0;
}

/****************************************************************************
 *
 * ram_getphy
 *
 * determinate ramdrive physicals
 *
 * INPUTS
 *
 * phy - this structure has to be filled with physical information
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

#if F_FORMATTING

unsigned char ram_getphy(F_PHY *phy) {
   phy->number_of_sectors=RAMDRV_MAXSECTOR;
   return 0;
}

#endif

/******************************************************************************
 *
 *  End of ramdrv.c
 *
 *****************************************************************************/

#endif /*_RAMDRV_C_*/
