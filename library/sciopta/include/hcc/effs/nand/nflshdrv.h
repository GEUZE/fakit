#ifndef _NFLSHDRV_H_
#define _NFLSHDRV_H_

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

#include <hcc/effs/common/fsf.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FS_NAND_RESERVEDBLOCK 12 /* defines reserved block in nandflash  */

extern int fs_mount_nandflashdrive(FS_VOLUMEDESC *vd,FS_PHYGETID phyfunc);
extern long fs_getmem_nandflashdrive(FS_PHYGETID phyfunc);

#ifdef __cplusplus
}
#endif

/****************************************************************************
 *
 * end of nflshdrv.h
 *
 ***************************************************************************/

#endif	/* _NFLSHDRV_H_ */
