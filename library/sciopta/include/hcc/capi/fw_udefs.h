#ifndef _FW_UDEFS_H_
#define _FW_UDEFS_H_

/****************************************************************************
 *
 *            Copyright (c) 2003 by HCC Embedded
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

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************
 *
 * volumes definitions
 *
 ***************************************************************************/

#define FS_MAXVOLUME	5     /* maximum number of volumes SAFE */
#define FN_MAXVOLUME    5     /* maximum number of volumes FAT */

#define FW_MAXTASK		10    /* maximum number of task */

#define FW_CURRDRIVE	0	/* setting the current drive at startup (-1 means no default current drive)*/

/****************************************************************************
 *
 *	if Unicode is used then comment in HCC_UNICODE define
 *
 ***************************************************************************/
/* #define HCC_UNICODE */

typedef unsigned short wchar;

#ifndef HCC_UNICODE
#define F_LONGFILENAME 0 /*  0 - 8+3 names   1 - long file names   */
#define W_CHAR char
#else
#define F_LONGFILENAME 1 /* don't change it, because unicode version alvays uses longfile */
#define W_CHAR wchar
#endif


#if 1
#define FW_SEPARATORCHAR '/'
#else
#define FW_SEPARATORCHAR '\\'
#endif

/****************************************************************************
 *
 * Last error usage
 *
 ***************************************************************************/

#if 1
/* simple asignment */
#define F_SETLASTERROR(ec) (fm->lasterror=(ec))
#define F_SETLASTERROR_NORET(ec) (fm->lasterror=(ec))
#elif 0
/* function calls used for it */
#define F_SETLASTERROR(ec) fw_setlasterror(fm,ec)
#define F_SETLASTERROR_NORET(ec) fw_setlasterror_noret(fm,ec)
#elif 0
/* no last error is used (save code space) */
#define F_SETLASTERROR(ec) (ec)
#define F_SETLASTERROR_NORET(ec) 
#endif

/****************************************************************************
 *
 * end of CPP declares
 *
 ***************************************************************************/

#ifdef __cplusplus
}
#endif

/****************************************************************************
 *
 * end of fw_udefs.h
 *
 ***************************************************************************/

#endif /* _FW_UDEFS_H_ */

