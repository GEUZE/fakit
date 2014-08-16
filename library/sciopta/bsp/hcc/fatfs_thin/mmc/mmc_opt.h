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
#ifndef __MMC_OPT_H
#define __MMC_OPT_H

#ifdef __cplusplus
extern "C" {
#endif


#define SDHC_ENABLE		1	/* enable SDHC support */
#define USE_CRC			1	/* use CRC for communication */
#define CRC_ROM_TABLE	1	/* put CRC table in ROM */
#define ASM_OPT_8051	0	/* use inline optimized assembler for critical parts */

#ifdef __cplusplus
}
#endif

#endif

