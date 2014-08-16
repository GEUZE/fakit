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
#ifndef _TEST_H
#define _TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_ENABLE		0	/* enable test suite */

#define BIGFILETEST_MAXFSSIZE	0	/* use all available memory in the flash for testing */
#if BIGFILETEST_MAXFSSIZE==0
#define BIGFILETEST_SIZE	65536	/* otherwise, the maximum file size tested is this number */
#endif
#define TEST_BUF_SIZE 256		/* buffer size for testing, doesn't affect the test, just slows it down */

extern unsigned short tinytest (void);

#ifdef __cplusplus
}
#endif

#endif



