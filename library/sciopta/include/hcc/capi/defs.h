#ifndef _DEFS_H_
#define _DEFS_H_

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

#define F_FILE FW_FILE
#define F_FIND FW_FIND
#define F_SPACE FW_SPACE
#define F_SEEK_SET FW_SEEK_SET
#define F_SEEK_END FW_SEEK_END
#define F_SEEK_CUR FW_SEEK_CUR
#define F_MAXVOLUME FW_MAXVOLUME

/****************************************************************************
 *
 * init Functions
 *
 ***************************************************************************/

#define f_getversion(ver,fatver,safever) fw_getversion(ver,fatver,safever)
#define f_init fw_init
#if FW_FAT_USED
#define f_mountfat(drvnumber,pfunc,user_ptr) fw_mountfat(drvnumber,pfunc,user_ptr)
#define f_mountfatpartition(drvnumber,driver,partition)	fw_mountfatpartition(drvnumber,driver,partition)
#define f_releasedriver(driver) fw_releasedriver(driver)
#define f_createdriver(driver,driver_init,driver_param) fn_createdriver(driver,driver_init,driver_param)
#define f_createpartition(driver,parnum,par) fn_createpartition(driver,parnum,par)
#define f_getpartition(driver,parnum,par) fn_getpartition(driver,parnum,par)
#endif
#if FW_SAFE_USED
#define f_mountsafe(drivenum,buffer,buffsize,mountfunc,phyfunc)	fw_mountsafe(drivenum,buffer,buffsize,mountfunc,phyfunc)
#endif
#define f_unmountdrive(drivenum) fw_unmountdrive(drivenum)
#define f_format(drivenum,type) fw_format(drivenum,type)
#define f_enterFS fw_enterFS
#define f_releaseFS(ID) fw_releaseFS(ID)
#define f_getlasterror fw_getlasterror


/****************************************************************************
 *
 * directory handler Functions
 *
 ***************************************************************************/

#define f_get_volume_count fw_get_volume_count
#define f_get_volume_list(varray) fw_get_volume_list(varray)

#define f_getdrive fw_getdrive
#define f_chdrive(drivenum) fw_chdrive(drivenum)

#define f_getfreespace(drivenum,pspace) fw_getfreespace(drivenum,pspace)

#define f_getcwd(buffer,maxlen) fw_getcwd(buffer,maxlen)
#define f_getdcwd(drivenum,buffer,maxlen) fw_getdcwd(drivenum,buffer,maxlen)

#define f_mkdir(dirname) fw_mkdir(dirname)
#define f_rmdir(dirname) fw_rmdir(dirname)
#define f_chdir(dirname) fw_chdir(dirname)

#define f_getlabel(drivenum,label,len) fw_getlabel(drivenum,label,len)
#define f_setlabel(drivenum,label) fw_setlabel(drivenum,label)

/****************************************************************************
 *
 * files Functions
 *
 ***************************************************************************/

#define f_rename(filename,newname) fw_rename(filename,newname)
#define f_move(filename,newname) fw_move(filename,newname)
#define f_delete(filename) fw_delete(filename)
#define f_filelength(filename) fw_filelength(filename)
#define f_findfirst(filename,find) fw_findfirst(filename,find)
#define f_findnext(find) fw_findnext(find)

#define f_settimedate(filename,ctime,cdate) fw_settimedate(filename,ctime,cdate)
#define f_gettimedate(filename,pctime,pcdate) fw_gettimedate(filename,pctime,pcdate)

#define f_getattr(filename,attr) fw_getattr(filename,attr)
#define f_setattr(filename,attr) fw_setattr(filename,attr)

/****************************************************************************
 *
 * read/write Functions
 *
 ***************************************************************************/

#define f_open(filename,mode) fw_open(filename,mode)
#define f_close(filehandle) fw_close(filehandle)
#define f_flush(filehandle) fw_flush(filehandle)
#define f_write(buf,size,size_st,filehandle) fw_write(buf,size,size_st,filehandle)
#define f_read(buf,size,size_st,filehandle) fw_read(buf,size,size_st,filehandle)
#define f_seek(filehandle,offset,whence) fw_seek(filehandle,offset,whence)
#define f_tell(filehandle) fw_tell(filehandle)
#define f_eof(filehandle) fw_eof(filehandle)
#define f_rewind(filehandle) fw_rewind(filehandle)
#define f_putc(ch,filehandle) fw_putc(ch,filehandle)
#define f_getc(filehandle) fw_getc(filehandle)
#define f_truncate(filename,length) fw_truncate(filename,length)
#define f_seteof(filehandle) fw_seteof(filehandle)

#define f_ftruncate(filehandle,length) fw_ftruncate(filehandle,length)
#define f_stat(filename,stat) fw_stat(filename,stat)
#define f_checkvolume(drvnumber) fw_checkvolume(drvnumber)
#define f_get_oem(drivenum,str,maxlen) fw_get_oem(drivenum,str,maxlen)
#define f_abortclose(filehandle) fw_abortclose(filehandle)


#ifdef HCC_UNICODE
#define f_wgetcwd(buffer,maxlen) fw_wgetcwd(buffer,maxlen)
#define f_wgetdcwd(drivenum,buffer,maxlen) fw_wgetdcwd(drivenum,buffer,maxlen)
#define f_wmkdir(dirname) fw_wmkdir(dirname)
#define f_wrmdir(dirname) fw_wrmdir(dirname)
#define f_wchdir(dirname) fw_wchdir(dirname)
#define f_wrename(filename,newname) fw_wrename(filename,newname)
#define f_wmove(filename,newname) fw_wmove(filename,newname)
#define f_wdelete(filename) fw_wdelete(filename)
#define f_wfilelength(filename) fw_wfilelength(filename)
#define f_wfindfirst(filename,find) fw_wfindfirst(filename,find)
#define f_wfindnext(find) fw_wfindnext(find)
#define f_wsettimedate(filename,ctime,cdate) fw_wsettimedate(filename,ctime,cdate)
#define f_wgettimedate(filename,pctime,pcdate) fw_wgettimedate(filename,pctime,pcdate)
#define f_wgetattr(filename,attr) fw_wgetattr(filename,attr)
#define f_wsetattr(filename,attr) fw_wsetattr(filename,attr)
#define f_wopen(filename,mode) fw_wopen(filename,mode)
#define f_wtruncate(filename,length) fw_wtruncate(filename,length)
#define f_wstat(filename,stat) fw_wstat(filename,stat)
#endif

#ifdef __cplusplus
}
#endif

/****************************************************************************
 *
 * end of defs.h
 *
 ***************************************************************************/

#endif /* _DEFS_H_ */

