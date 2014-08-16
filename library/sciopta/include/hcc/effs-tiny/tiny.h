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


#ifndef _TINY_H
#define _TINY_H

#ifdef __cplusplus
//->extern "C" {
#endif

#include <hcc/effs-tiny/tinyuser.h>
#include <hcc/effs-tiny/_tiny.h>



/****************************************************************************
 *  return error codes
 ***************************************************************************/
enum {
/*0*/	F_NOERR,		/* no error, successfully returned */
/*1*/	F_ERR_INVVOLUME,	/* no volume available */
/*2*/	F_ERR_INVHANDLE,	/* invalid file handle */
/*3*/	F_ERR_INVOFFSET,	/* invalid offset */
/*4*/	F_ERR_INVMODE,		/* invalid mode */
/*5*/	F_ERR_EOF,		/* end of file */
/*6*/	F_ERR_NOTFOUND,		/* file not found */
/*7*/	F_ERR_DIRFULL,		/* directory full */
/*8*/	F_ERR_INVNAME,		/* invalid name */
/*9*/	F_ERR_INVDIR,		/* invalid dir */
/*10*/	F_ERR_OPENED,		/* file/directory in use */
/*11*/	F_ERR_NOTOPENED,	/* file not opened or opened in different mode */
/*12*/	F_ERR_NOTFORMATTED,	/* not formatted */
/*13*/	F_ERR_DIFFMEDIA,	/* different media */
/*14*/	F_ERR_NOMOREENTRY,	/* no more entry available */
/*15*/	F_ERR_DUPLICATED,	/* duplicated filename */
/*16*/	F_ERR_NOTEMPTY,		/* trying to remove a directory that is not empty */
/*17*/	F_ERR_INVSIZE,		/* when buffer size is too small (getcwd) */
/*18*/	F_ERR_ACCESSDENIED	/* access is denied */
};

#define F_INVALID (-1)		/* invalid value DON'T CHANGE!!! */
#define F_ERR_INVALID F_INVALID	/* invalid data (e.g. f_filelength if file not found) */


/****************************************************************************
 *  File attributes 
 ***************************************************************************/
#define F_ATTR_ARC      0x20
#define F_ATTR_DIR      0x10
#define F_ATTR_VOLUME   0x08
#define F_ATTR_SYSTEM   0x04
#define F_ATTR_HIDDEN   0x02
#define F_ATTR_READONLY 0x01


/****************************************************************************
 *  f_seek whence params
 ***************************************************************************/
/* from Beginning of file */
#ifdef SEEK_SET
#define F_SEEK_SET SEEK_SET
#else
#define F_SEEK_SET 0
#endif

/* from Current position of file pointer */
#ifdef SEEK_CUR
#define F_SEEK_CUR SEEK_CUR
#else
#define F_SEEK_CUR 1
#endif

/* from End of file */
#ifdef SEEK_END
#define F_SEEK_END SEEK_END
#else
#define F_SEEK_END 2
#endif


/****************************************************************************
 *  findfirst, findnext structure
 ***************************************************************************/
typedef struct {
  unsigned char attr;			/* file attr */
  unsigned short ctime;			/* creation time */
  unsigned short cdate;			/* creation date */
  F_LENGTH_TYPE length;			/* length of the file */

  char filename[F_MAX_FILE_NAME_LENGTH+1];	/* file name */

#if F_DIRECTORY
  F_DIR_ID_TYPE dirid;			/* current directory position */
#endif

  F_FIND_ID_TYPE findpos;		/* internally used position */

  char *findname;			/* original filename for searching */
} F_FIND;

/****************************************************************************
 *  F_FILE structure
 ***************************************************************************/
typedef struct {
  F_LENGTH_TYPE length;					/* length of te file */
  F_LENGTH_TYPE abs_pos;				/* absolute position in the file */

  F_CLUSTER_TYPE cluster_pos;				/* actual cluster position */
  F_CLUSTER_COUNT_TYPE cluster;				/* actual cluster */
#if (F_SEEK_WRITE)
  F_CLUSTER_COUNT_TYPE orig_cluster;			/* to hold original cluster value of the actual position */
#endif

  F_FILE_MGM_CLUSTER_PER_PAGE_COUNT_TYPE page_cl_pos;	/* actual cluster position in the page */
  F_FILE_MGM_PAGE_COUNT_TYPE page_cnt;			/* actual page count */

  F_FILE_ID_TYPE fileid;				/* fileid */
  F_FILE_MGM_PAGE_COUNT_TYPE first_mgm_page;		/* first file management page */
  F_FILE_MGM_PAGE_COUNT_TYPE act_mgm_page;		/* actual management page */

#if F_DIRECTORY
  F_DIR_ID_TYPE dirid;					/* dirid */
#endif

  char mode;
} F_FILE;



/****************************************************************************
 *  F_SPACE structure
 ***************************************************************************/
typedef struct {
  F_LENGTH_TYPE total;	/* total size on volume */
  F_LENGTH_TYPE free;	/* free space on volume */
} F_SPACE;



/****************************************************************************
 *  functions
 ***************************************************************************/
#define f_getversion() _TINY_VERSION
extern unsigned char f_initvolume(void);
#if F_FIND_ENABLE
extern unsigned char f_findfirst(const char *filename,F_FIND *find);
extern unsigned char f_findnext(F_FIND *find);
#endif
#if F_FILELENGTH
extern unsigned long f_filelength(const char *filename);
#endif
#if F_GETTIMEDATE
extern int f_gettimedate(const char *filename,unsigned short *pctime,unsigned short *pcdate);
#endif
extern F_FILE *f_open(const char *filename,const char *mode);
extern int f_close(F_FILE *filehandle);
extern long f_read(void *bbuf,long size,long size_st,F_FILE *filehandle);
extern long f_write(const void *bbuf,long size,long size_st,F_FILE *filehandle);
extern int f_seek(F_FILE *,long ,long);
extern long f_tell(F_FILE *);
extern int f_getc(F_FILE *);
extern int f_putc(int,F_FILE *);
extern int f_rewind(F_FILE *);
extern int f_eof(F_FILE *);
extern unsigned char f_format (void);
#if F_DELETE
extern int f_delete(const char *);
#endif
#if F_RENAME
extern int f_rename(const char *, const char *);
#endif
#if F_GETFREESPACE
extern unsigned char f_getfreespace (F_SPACE *);
#endif
#if F_CHANGE_NOTIFY
extern unsigned char f_findchanged(F_FIND *);
#endif

#if F_DIRECTORY
#if F_CHDIR
extern int f_chdir (const char *path);
#endif
#if F_MKDIR
extern int f_mkdir (const char *);
#endif
#if F_RMDIR
extern int f_rmdir (const char *);
#endif
#if F_GETCWD
extern int f_getcwd (char *, int);
#endif
#endif


#ifdef __cplusplus
//->}
#endif

#endif

