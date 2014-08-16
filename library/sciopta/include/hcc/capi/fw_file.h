#ifndef _FW_FILE_H_
#define _FW_FILE_H_

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
 * definitions which file system is used
 *
 ***************************************************************************/

#define FW_FAT_USED 1
#define FW_SAFE_USED 1

/****************************************************************************
 *
 * Includes
 *
 ***************************************************************************/

#include "fwerr.h"
#include "fw_port.h"

#if FW_FAT_USED
#include "fat_src/common/fat.h"
#include "fat_src/common/common.h"
#include "fat_src/common/port_f.h"
#endif
#if FW_SAFE_USED
#include "safe_src/common/fsf.h"
#endif

#if FW_SAFE_USED && FW_FAT_USED
#define FW_MAXVOLUME (FN_MAXVOLUME+FS_MAXVOLUME)
#else
#if FW_SAFE_USED
#define FW_MAXVOLUME (FS_MAXVOLUME)
#else
#if FW_FAT_USED
#define FW_MAXVOLUME (FN_MAXVOLUME)
#else
#error FW_FAT_USED and FW_SAFE_USED is not defined
#endif
#endif
#endif

#define FW_MAXFILE   10

enum {
	FW_NONE,
#if FW_FAT_USED
	FW_FAT,
#endif
#if FW_SAFE_USED
	FW_SAFE,
#endif
	FW_MAX,
	FW_LOCKED
};

#if FW_FAT_USED
#define FW_FAT12 F_FAT12_MEDIA
#define FW_FAT16 F_FAT16_MEDIA
#define FW_FAT32 F_FAT32_MEDIA
#endif

typedef struct {
	char filename[F_MAXPATH];	/* file name+ext */
	unsigned char attr;			/* attribute of the file/entry */

	unsigned short ctime;		/* creation time */
	unsigned short cdate;		/* creation date */

	unsigned long filesize;		/* length of file */

#if FW_SAFE_USED || FW_FAT_USED
   union {
#if FW_FAT_USED
      struct {
      	F_NAME findfsname;		   	/* find properties */
	      F_POS pos;
      } fat;
#endif
#if FW_SAFE_USED
      struct {
      	FS_NAME findfsname;		   	/* find properties */
      	unsigned short findpos;		/* find position */
      } safe;
#endif
   } common;
#endif
   int type;
} FW_FIND;

#ifdef HCC_UNICODE
typedef struct {
	W_CHAR filename[F_MAXPATH];	/* file name+ext */
	unsigned char attr;			/* attribute of the file/entry */

	unsigned short ctime;		/* creation time */
	unsigned short cdate;		/* creation date */

	unsigned long filesize;		/* length of file */

#if FW_SAFE_USED || FW_FAT_USED
   union {
#if FW_FAT_USED
      struct {
      	F_NAME findfsname;		   	/* find properties */
	      F_POS pos;
      } fat;
#endif
#if FW_SAFE_USED
      struct {
      	FS_NAME findfsname;		   	/* find properties */
      	unsigned short findpos;		/* find position */
      } safe;
#endif
   } common;
#endif
   int type;
} FW_WFIND;
#endif


#define FW_ATTR_ARC      0x20
#define FW_ATTR_DIR      0x10
#define FW_ATTR_VOLUME   0x08
#define FW_ATTR_SYSTEM   0x04
#define FW_ATTR_HIDDEN   0x02
#define FW_ATTR_READONLY 0x01

typedef struct {
   int drvtype;
   void *ptr;
} FW_FILE;

typedef struct {
   unsigned long total;
   unsigned long free;
   unsigned long used;
   unsigned long bad;

   unsigned long total_high;
   unsigned long free_high;
   unsigned long used_high;
   unsigned long bad_high;
} FW_SPACE;

/****************************************************************************
 *
 * init Functions
 *
 ***************************************************************************/

extern void fw_getversion(char **ver, char **fatver, char **safever);
extern int fw_init(void);
#if FW_FAT_USED
extern int fw_mountfat(int drvnumber,F_DRIVERINIT driver_init,unsigned long driver_param);
extern int fw_mountfatpartition(int drvnumber,F_DRIVER *driver, int partition);
extern int fw_releasedriver(F_DRIVER *driver);
#endif
#if FW_SAFE_USED
extern int fw_mountsafe(int drivenum,void *buffer,long buffsize,FS_DRVMOUNT mountfunc,FS_PHYGETID phyfunc);
#endif
extern int fw_unmountdrive (int drivenum);
extern int fw_format(int drivenum,long type);
extern int fw_enterFS(void);
extern void fw_releaseFS(long ID);
extern int fw_getlasterror(void);

/****************************************************************************
 *
 * directory handler Functions
 *
 ***************************************************************************/
extern int fw_get_volume_count(void);
extern int fw_get_volume_list(int *varray);

extern int fw_getdrive(void);
extern int fw_chdrive(int drivenum);

extern int fw_getfreespace(int drivenum, FW_SPACE *pspace);

extern int fw_getcwd(char *buffer, int maxlen );
extern int fw_getdcwd(int drivenum,char *buffer, int maxlen );

extern int fw_mkdir(const char *dirname);
extern int fw_rmdir(const char *dirname);
extern int fw_chdir(const char *dirname);

extern int fw_getlabel(int drivenum, char *label, long len);
extern int fw_setlabel(int drivenum, const char *label);

/****************************************************************************
 *
 * files Functions
 *
 ***************************************************************************/

extern int fw_rename(const char *filename, const char *newname);
extern int fw_move(const char *filename, const char *newname);
extern int fw_delete(const char *filename);
extern long fw_filelength(const char *filename);
extern int fw_findfirst(const char *filename,FW_FIND *find);
extern int fw_findnext(FW_FIND *find);

extern int fw_settimedate(const char *filename,unsigned short ctime,unsigned short cdate);
extern int fw_gettimedate(const char *filename,unsigned short *pctime,unsigned short *pcdate);

extern int fw_getattr(const char *filename,unsigned char *attr);
extern int fw_setattr(const char *filename,unsigned char attr);

/****************************************************************************
 *
 * read/write Functions
 *
 ***************************************************************************/

extern FW_FILE *fw_open(const char *filename,const char *mode);
extern int fw_close(FW_FILE *filehandle);
extern int fw_flush(FW_FILE *filehandle);
extern long fw_write(const void *buf,long size,long size_st,FW_FILE *filehandle);
extern long fw_read(void *buf,long size,long size_st,FW_FILE *filehandle);
extern int fw_seek(FW_FILE *filehandle,long offset,long whence);
extern long fw_tell(FW_FILE *filehandle);
extern int fw_eof(FW_FILE *filehandle);
extern int fw_rewind(FW_FILE *filehandle);
extern int fw_putc(int ch,FW_FILE *filehandle);
extern int fw_getc(FW_FILE *filehandle);
extern FW_FILE *fw_truncate(const char *filename,unsigned long length);
extern int fw_seteof(FW_FILE *filehandle);

extern int fw_ftruncate(FW_FILE *filehandle,unsigned long length);
extern int fw_stat(const char *filename,F_STAT *stat);
extern int fw_checkvolume (int drivenum);
extern int fw_get_oem (int drivenum, char *str, long maxlen);
extern int fw_abortclose(FW_FILE *filehandle);

/* Beginning of file */
#ifdef SEEK_SET
#define FW_SEEK_SET SEEK_SET
#else
#define FW_SEEK_SET 0
#endif

/* Current position of file pointer */
#ifdef SEEK_CUR
#define FW_SEEK_CUR SEEK_CUR
#else
#define FW_SEEK_CUR 1
#endif

/* End of file */
#ifdef SEEK_END
#define FW_SEEK_END SEEK_END
#else
#define FW_SEEK_END 2
#endif

#ifdef HCC_UNICODE
extern int fw_wgetcwd(W_CHAR *buffer, int maxlen );
extern int fw_wgetdcwd(int drivenum,W_CHAR *buffer, int maxlen );
extern int fw_wmkdir(const W_CHAR *dirname);
extern int fw_wrmdir(const W_CHAR *dirname);
extern int fw_wchdir(const W_CHAR *dirname);
extern int fw_wrename(const W_CHAR *filename, const W_CHAR *newname);
extern int fw_wmove(const W_CHAR *filename, const W_CHAR *newname);
extern int fw_wdelete(const W_CHAR *filename);
extern long fw_wfilelength(const W_CHAR *filename);
extern int fw_wfindfirst(const W_CHAR *filename,FW_WFIND *find);
extern int fw_wfindnext(FW_WFIND *find);
extern int fw_wsettimedate(const W_CHAR *filename,unsigned short ctime,unsigned short cdate);
extern int fw_wgettimedate(const W_CHAR *filename,unsigned short *pctime,unsigned short *pcdate);
extern int fw_wgetattr(const W_CHAR *filename,unsigned char *attr);
extern int fw_wsetattr(const W_CHAR *filename,unsigned char attr);
extern FW_FILE *fw_wopen(const W_CHAR *filename,const W_CHAR *mode);
extern FW_FILE *fw_wtruncate(const W_CHAR *filename,unsigned long length);
extern int fw_wstat(const W_CHAR *filename,F_STAT *stat);
#endif

extern void fw_setlasterror_noret (FS_MULTI *fm, int errorcode);
extern int fw_setlasterror (FS_MULTI *fm, int errorcode);

#include "defs.h"

#ifdef __cplusplus
}
#endif

/****************************************************************************
 *
 * end of fw_file.h
 *
 ***************************************************************************/

#endif /* _FW_FILE_H_ */

