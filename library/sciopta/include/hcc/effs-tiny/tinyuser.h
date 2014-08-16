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
#ifndef _TINYUSER_H
#define _TINYUSER_H

#ifdef __cplusplus
//->extern "C" {
#endif


/*********************** filesystem options *************************/
#define F_WILDCARD		1	/* enable usage of wildcards */

#define F_CHECKNAME		1	/* check for valid file name characters, accept multiple / or \ */
					/* accepts multiple * in wildcard and handles / at the end of a dirname */
					/* if mkdir is called (e.g.: a/b/) or filename at f_open, handles upper/lower case */

#define F_CHECKMEDIA		1	/* check for different media atr startup (tiny with different drive geometry) */

#define F_DIRECTORY		1	/* enable usage of directories */
  #define F_CHDIR		1	/* enable chdir */
  #define F_MKDIR		1	/* enable mkdir */
  #define F_RMDIR		1	/* enable rmdir */
  #define F_GETCWD		1	/* enable getcwd */
  #define F_DIR_OPTIMIZE	1	/* enable directory storage optimization */

#define F_FIND_ENABLE		1	/* enable findfirst/findnext */
  #define F_CHANGE_NOTIFY	1	/* f_findchanged allowed */

#define F_FILELENGTH		1	/* enable filelength */
#define F_GETTIMEDATE		1	/* enable gettimedate */
#define F_GETFREESPACE		1	/* enable getfreespace */
#define F_DELETE		1	/* enable delete */
#define F_RENAME		1	/* enable rename */

#define F_SEEK_WRITE		1	/* allow seeking for write */

#define RTC_PRESENT		0	/* real time clock present in the system */

/************************* filesystem settings *************************/
#define F_MAX_OPEN_FILE		2		/* maximum opened files simultaneously */
#define F_MAX_FILE_NAME_LENGTH	16		/* maximum name length of a file or directory */
#define F_MAX_FILE		32		/* maximum number of files allowed in the system */
#if F_DIRECTORY
  #define F_MAX_DIR		16		/* maximum number of directories allowed in the system */
#endif

/*************************** system features ***************************/
#define _SUL	4	/* sizeof(unsigned long) */
#define	_SUS	2	/* sizeof(unsigned short) */
#define _SUC	1	/* sizeof(unsigned char) */

#define _SALIGN	4	/* use alignment to 4 */


/*********************** memory access functions ***********************/
#include <string.h>
#define _memcmp   memcmp
#define _memcpy   memcpy
#define _memset   memset
#define _memmove  memmove
#define _strlen   strlen
#define _strcmp	  strcmp



#ifdef __cplusplus
//->}
#endif

#endif

