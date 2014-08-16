/****************************************************************************
 *
 *            Copyright (c) 2007 by HCC Embedded 
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
#ifndef _MMC_H_
#define _MMC_H_

#include "mmc_opt.h"
#include "sam7s256/mmc_drv.h"
#include "mmc_dsc.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
 *
 * Errors in MMC
 *
 *****************************************************************************/
#define MMC_ERR_NOTPLUGGED ((unsigned char)-1)
enum {
  MMC_NO_ERROR,
  MMC_ERR_NOTINITIALIZED,
  MMC_ERR_INIT,
  MMC_ERR_CMD,
  MMC_ERR_STARTBIT,
  MMC_ERR_BUSY,
  MMC_ERR_CRC,
  MMC_ERR_WRITE,
  MMC_ERR_WRITEPROTECT,
  MMC_ERR_NOTAVAILABLE
};


/******************************************************************************
 *
 *  Functions
 *
 *****************************************************************************/
unsigned char mmc_readsector(unsigned char *,unsigned long);
unsigned char mmc_writesector(unsigned char *,unsigned long);
unsigned char mmc_getstatus(void);
#if F_FORMATTING
unsigned char mmc_getphy(F_PHY *);
#endif
unsigned char mmc_initfunc(void);
#define drv_readsector mmc_readsector
#define drv_writesector mmc_writesector
#define drv_getstatus mmc_getstatus
#define drv_initfunc mmc_initfunc

#if F_FORMATTING
#define drv_getphy mmc_getphy
#endif


/******************************************************************************
 *
 * end of mmc.h
 *
 *****************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /*_MMC_H_*/

