#ifndef _RAMDRV_H_
#define _RAMDRV_H_

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

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 *  Functions
 *
 *****************************************************************************/

unsigned char ram_readsector(void *_data,unsigned long sector);
unsigned char ram_writesector(void *_data,unsigned long sector);

#if F_FORMATTING
unsigned char ram_getphy(F_PHY *phy);
#endif

/******************************************************************************
 *
 *  definitions for highlevel
 *
 *****************************************************************************/

#define drv_readsector ram_readsector
#define drv_writesector ram_writesector

#if F_FORMATTING
#define drv_getphy ram_getphy
#endif

/******************************************************************************
 *
 *  End of ramdrv.c
 *
 *****************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /*_RAMDRV_H_*/
