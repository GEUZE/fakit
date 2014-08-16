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


#ifndef __UTIL_H
#define __UTIL_H

#include <hcc/fatfs_sthin/port.h>
#include <hcc/fatfs_sthin/util_sfn.h>

#ifdef __cplusplus
extern "C" {
#endif


unsigned short _f_getword(void *);
unsigned long _f_getlong(void *);
char _f_toupper(char); 
void _f_memset(void *,unsigned char,int);
void _f_memcpy(void *, void *, int);

#if F_WRITING
void _f_setword(void *,unsigned short);
void _f_setlong(void *,unsigned long);
#endif
#if (F_FORMATTING & F_WRITING)
unsigned char *_setcharzero(int ,unsigned char *);
unsigned char *_setchar(const unsigned char *,int,unsigned char *);
unsigned char *_setword(unsigned short,unsigned char *);
unsigned char *_setlong(unsigned long,unsigned char *);
#endif

#ifdef __cplusplus
}
#endif

#endif

