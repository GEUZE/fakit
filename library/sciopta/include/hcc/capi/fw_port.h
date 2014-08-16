#ifndef _FW_PORT_H_
#define _FW_PORT_H_

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

#include "fw_udefs.h"

#define FS_SEPARATORCHAR FW_SEPARATORCHAR
#define F_SEPARATORCHAR FW_SEPARATORCHAR

/****************************************************************************
 *
 * mutexes definitions
 *
 ***************************************************************************/

#define F_MUTEX_TYPE unsigned long

#define FS_MUTEX_TYPE F_MUTEX_TYPE
#define fs_mutex_get 	 fw_mutex_get
#define fs_mutex_put 	 fw_mutex_put
#define fs_mutex_create  fw_mutex_create
#define fs_mutex_delete  fw_mutex_delete

#define FN_MUTEX_TYPE F_MUTEX_TYPE
#define f_mutex_get 	 fw_mutex_get
#define f_mutex_put 	 fw_mutex_put
#define f_mutex_create  fw_mutex_create
#define f_mutex_delete  fw_mutex_delete

extern int fw_mutex_get (F_MUTEX_TYPE *);
extern int fw_mutex_put (F_MUTEX_TYPE *);
extern int fw_mutex_create (F_MUTEX_TYPE *);
extern int fw_mutex_delete (F_MUTEX_TYPE *);

#define F_MAXPATH 256
#define FS_MAXPATH F_MAXPATH
#define FN_MAXPATH  F_MAXPATH

typedef struct 
{
	long ID;
	int fw_curdrive;	  			/* current drive on CAPI*/
	int fs_curdrive;	  			/* current drive on SAFE*/
	int f_curdrive;					/* current drive on FAT*/
	int lasterror;					/* last error on fat */

	struct 
	{
		W_CHAR cwd[FS_MAXPATH];		/* current working folder in this volume */
	} fs_vols[FS_MAXVOLUME];

	struct 
	{
		W_CHAR cwd[F_MAXPATH];		/* current working folders in this volume */
	} f_vols[FN_MAXVOLUME];

	F_MUTEX_TYPE *pmutex;		/* for multitasking */

} FW_MULTI;

#define FS_MULTI FW_MULTI
#define F_MULTI  FW_MULTI

extern void fw_setlasterror_noret (FW_MULTI *fm, int errorcode);
extern int fw_setlasterror (FW_MULTI *fm, int errorcode);

/****************************************************************************
 *
 * Set HCC_16BIT_CHAR to 1 if a char is 16bit length (e.g. on DSPs)
 *
 ***************************************************************************/

#define	HCC_16BIT_CHAR 0


extern long fn_gettaskID(void);

#ifdef __cplusplus
}
#endif

/****************************************************************************
 *
 * end of fw_port.h
 *
 ***************************************************************************/

#endif /* _FW_PORT_H_ */
