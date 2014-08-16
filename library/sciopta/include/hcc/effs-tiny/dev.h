/****************************************************************************
 *
 *            Copyright (c) 2005 by HCC Embedded 
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
 * Budapest 1132
 * Victor Hugo Utca 11-15
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/
#ifndef _DEVSER_H
#define _DEVSER_H

#ifdef __cplusplus
//->extern "C" {
#endif

#include <hcc/effs-tiny/tiny.h>

#define DEV_SER_ADDR	(F_FS_ID_ADDR+64)

unsigned long dev_get_serial (void);		/* get serial number */
unsigned char dev_set_serial (unsigned long);	/* set serial number */
#define dev_size()	F_SIZE			/* get device size */

#ifdef __cplusplus
//->}
#endif

#endif


