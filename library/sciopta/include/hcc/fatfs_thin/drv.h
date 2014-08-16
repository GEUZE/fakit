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


#ifndef __DRV_H
#define __DRV_H

/*#include "mst.h"*/
/*#include "ramdrv.h"*/
#include "mmc.h"
/*#include "cfc_ide.h"*/
/*#include "cfc.h"*/


#ifdef __cplusplus
extern "C" {
#endif

unsigned char _f_checkstatus(void);
unsigned char _f_readsector(void *, unsigned long);
#if F_WRITING
unsigned char _f_writesector(void *, unsigned long);
#endif

#ifdef __cplusplus
}
#endif

#endif
