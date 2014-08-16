#ifndef _PORT_S_H_
#define _PORT_S_H_

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

#ifdef __cplusplus
extern "C" {
#endif

extern void fs_getcurrenttimedate(unsigned short *ptime, unsigned short *pdate);

extern int fs_mutex_get (FS_MUTEX_TYPE *);
extern int fs_mutex_put (FS_MUTEX_TYPE *);
extern int fs_mutex_create (FS_MUTEX_TYPE *);
extern int fs_mutex_delete (FS_MUTEX_TYPE *);

extern long fn_gettaskID(void);

#ifdef __cplusplus
}
#endif

/****************************************************************************
 *
 * end of port_s.h
 *
 ***************************************************************************/

#endif /* _PORT_S_H_ */

