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
#ifndef _F_DIR_H
#define _F_DIR_H

#ifdef __cplusplus
//->extern "C" {
#endif

F_FILE_MGM_PAGE_COUNT_TYPE _f_add_entry (const char *name, F_FILE_ID_TYPE fileid, F_FILE_MGM_PAGE_COUNT_TYPE *page);
#if F_DIRECTORY
F_FIND_ID_TYPE _f_find (const char *filename, F_FILE_PAR *par, F_DIR_ID_TYPE odirid);
#else
F_FIND_ID_TYPE _f_find (const char *filename, F_FILE_PAR *par);
#endif
#if F_DIRECTORY
F_DIR_ID_TYPE _f_check_path (const char *name, char **filename);
#endif

#ifdef __cplusplus
//->}
#endif

#endif

