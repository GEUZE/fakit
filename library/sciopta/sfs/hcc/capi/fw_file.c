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

#include <hcc/capi/fw_file.h>

/****************************************************************************
 *
 * Static definitions and variables
 *
 ***************************************************************************/

typedef struct {
  int letter;
  int conv;
  int type;
  int orig;
} ST_FW_VOLUME;

static ST_FW_VOLUME fw_volumes[FW_MAXVOLUME];

static FW_FILE fw_files[FW_MAXFILE];
static FW_MULTI gl_multi[FW_MAXTASK];
static F_MUTEX_TYPE fw_gmutex; /* general mutex */

/****************************************************************************
 *
 * fw_init
 *
 * initialize file system wrapper
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_init()
{
  int a,cou=0;
  int ret;

  (void)memset(fw_volumes,0,sizeof(fw_volumes));
  (void)memset(fw_files,0,sizeof(fw_files));
  (void)memset(gl_multi,0,sizeof(gl_multi));

  for (a=0; a<FW_MAXVOLUME; a++)
  {
    fw_volumes[a].type=FW_NONE;
  }

#if FW_FAT_USED
  for (a=0; a<FN_MAXVOLUME; a++)
  {
    fw_volumes[cou].conv=a;
    fw_volumes[cou++].orig=FW_FAT;
  }
#endif

#if FW_SAFE_USED
  for (a=0; a<FS_MAXVOLUME; a++)
  {
    fw_volumes[cou].conv=a;
    fw_volumes[cou++].orig=FW_SAFE;
  }
#endif

  for (a=0; a<FW_MAXFILE; a++)
  {
    fw_files[a].drvtype=FW_NONE;
  }

#if FW_FAT_USED
  ret=fn_init();
  if (ret) return ret;
#endif

#if FW_SAFE_USED
  ret=fg_init();
  if (ret) return ret;
#endif

  if (fw_mutex_create(&fw_gmutex)) return F_ERR_INITFUNC;

  return F_NO_ERROR;
}

/****************************************************************************
 *
 * _fw_mutex_put
 *
 * Release used mutex
 *
 * INPUTS
 *
 * fm - multi structure to release mutex
 *
 ***************************************************************************/

void _fw_mutex_put(FW_MULTI *fm)
{
  if (fm->pmutex)
  {
    (void)fw_mutex_put(fm->pmutex);
    fm->pmutex=0;
  }
}


/****************************************************************************
 *
 * _fw_gettask
 *
 * Searching for an entry where the task infos are
 *
 * INPUTS
 *
 * fm - where to store information
 *
 * RETURNS
 *
 * 0 - if found
 * 1 - if not found
 *
 ***************************************************************************/

int _fw_gettask(FW_MULTI **fm)
{
  long ID=fn_gettaskID();
  int a;

  for (a=0; a<FW_MAXTASK;a++)
  {
    /* check if exist */
    if (gl_multi[a].ID==ID)
    {
      if (fm)
      {
  *fm=&gl_multi[a];
  gl_multi[a].pmutex=0;
      }
      return 0; /* free to use */
    }
  }

  return 1;
}

/****************************************************************************
 *
 * fw_enterFS
 *
 * Put taskID into the task list, this function has to be called when a new
 * task wants to use FS (before call any FS function)
 *
 * RETURNS
 *
 * errorcode or 0 if successful
 *
 ***************************************************************************/

int fw_enterFS()
{
  long ID=fn_gettaskID();
  int a,b;

  /* check if taskid is exist, no need multistructrure */
  if (!_fw_gettask(0)) return F_NO_ERROR; /* lasterror is already reset in gettask */

  /* wait until global mutex is released */
  for (;;)
  {
    if (!fw_mutex_get(&fw_gmutex)) break;
  }

  for (a=0; a<FW_MAXTASK;a++)
  {
    /* check for empty space*/
    if (!gl_multi[a].ID)
    {
      gl_multi[a].ID=ID;    /* set task ID */
      gl_multi[a].f_curdrive=FW_CURRDRIVE;
      gl_multi[a].fs_curdrive=FW_CURRDRIVE;
      gl_multi[a].fw_curdrive=FW_CURRDRIVE;
      gl_multi[a].lasterror=F_NO_ERROR;

      for (b=0; b<FS_MAXVOLUME; b++)
      {
  gl_multi[a].fs_vols[b].cwd[0]=0;
      }
      for (b=0; b<FN_MAXVOLUME; b++)
      {
  gl_multi[a].f_vols[b].cwd[0]=0;
      }

      gl_multi[a].pmutex=0;

      (void)fw_mutex_put(&fw_gmutex);
      return F_NO_ERROR; /* free to use */
    }
  }

  (void)fw_mutex_put(&fw_gmutex);
  return F_ERR_NOMOREENTRY; /* no space to set up new task */
}

/****************************************************************************
 *
 * fw_releaseFS
 *
 * Release used task specified F_MULTI structure
 *
 * INPUT
 *
 * ID - which ID need to be released
 *
 ***************************************************************************/

void fw_releaseFS(long ID)
{
  int a;

  for (a=0; a<FW_MAXTASK;a++)
  {
     if (gl_multi[a].ID==ID)
    {
      gl_multi[a].ID=0;
      return;
    }
  }
}

/****************************************************************************
 *
 * _fw_checkdrive
 *
 * checking drive of type
 *
 * INPUTS
 *
 * drivenum - which drive need to be checked
 * num - pointer where to store the number in the list
 *
 * RETURNS
 *
 * FW_xxxx, FW_NONE, FW_FAT, FW_SAFE
 *
 ***************************************************************************/

static int _fw_checkdrive(int drivenum,int *num) 
{
  int a;

  for (a=0; a<FW_MAXVOLUME; a++) 
  {
    int type=fw_volumes[a].type;

    if (type!=FW_NONE) 
    {
      if (fw_volumes[a].letter==drivenum) 
      {
  *num=a;
  return type;
      }
    }
  }

    return FW_NONE;
}

/****************************************************************************
 *
 * _fw_checkpath
 *
 * checking where requested path is, on which drive
 *
 * INPUTS
 *
 * fm - where to get original current drive
 * s - path string e.g. a:\hello
 * dest - where to create converted path
 *
 * RETURNS
 *
 * FW_xxxx, FW_NONE, FW_FAT, FW_SAFE
 *
 ***************************************************************************/

static int _fw_checkpath(FW_MULTI *fm,const char *s,char *dest)
{
  int drvnum;
  int a;
  int ret;

  for (a=0; a<F_MAXPATH-1; a++)
  {
    char ch=s[a];
    dest[a]=ch;
    if (!ch) break;
  }
  dest[a]=0; /* terminates it */

  if (!s || !s[1] || s[1]!=':')
  {
    if (fm->fw_curdrive==-1) return FW_NONE;
    return fw_volumes[fm->fw_curdrive].type;
  }

  if (s[0]>='A' && s[0]<='Z') drvnum=s[0]-'A';
  else if (s[0]>='a' && s[0]<='z') drvnum=s[0]-'a';
  else return FW_NONE;

  ret=_fw_checkdrive(drvnum,&a);
  if (ret!=FW_NONE)
  {
    dest[0]=(char)(fw_volumes[a].conv+'A');
  }

  return ret;
}

#ifdef HCC_UNICODE
static int _fw_wcheckpath(FW_MULTI *fm,const W_CHAR *s,W_CHAR *dest)
{
  int drvnum;
  int a;
  int ret;

  for (a=0; a<F_MAXPATH-1; a++)
  {
    W_CHAR ch=s[a];
    dest[a]=ch;
    if (!ch) break;
  }
  dest[a]=0; /* terminates it */

  if (!s || !s[1] || s[1]!=':')
  {
    if (fm->fw_curdrive==-1) return FW_NONE;
    return fw_volumes[fm->fw_curdrive].type;
  }

  if (s[0]>'A' && s[0]<'Z') drvnum=s[0]-'A';
  else if (s[0]>'a' && s[0]<'z') drvnum=s[0]-'a';
  else return FW_NONE;

  ret=_fw_checkdrive(drvnum,&a);
  if (ret!=FW_NONE)
  {
    dest[0]=(char)(fw_volumes[a].conv+'A');
  }

  return ret;
}
#endif

/****************************************************************************
 *
 * _fw_checkfile
 *
 * check file handle if belongs to any system
 *
 * INPUTS
 *
 * f - file pointer
 *
 * RETURNS
 *
 * FW_xxxx, FW_NONE, FW_FAT, FW_SAFE
 *
 ***************************************************************************/

static int _fw_checkfile(FW_FILE *f)
{
   if (((unsigned long)f>=(unsigned long)(&fw_files[0])) &&
       ((unsigned long)f<(unsigned long)(&fw_files[FW_MAXFILE])))
   {
      return f->drvtype;
   }

   return FW_NONE;
}

/****************************************************************************
 *
 * fw_getdrive
 *
 * Get current drive number
 *
 * RETURNS
 *
 * with the current drive number (0-A, 1-B,...)
 * or -1 if any error
 *
 ***************************************************************************/

int fw_getdrive()
{
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return -1;

  F_SETLASTERROR_NORET(F_NO_ERROR);

  if (fm->fw_curdrive==-1) return -1;

  return fw_volumes[fm->fw_curdrive].letter;
}

/****************************************************************************
 *
 * fw_chdrive
 *
 * Change current drive
 *
 * INPUTS
 *
 * drivenum - new current drive number (0-A, 1-B, ...)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_chdrive(int drivenum)
{
  int ret;
  int num;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkdrive(drivenum,&num))
  {
#if FW_FAT_USED
  case FW_FAT: ret=fn_chdrive(fm,fw_volumes[num].conv);
   if (!ret) fm->fw_curdrive=num;
   break;
#endif
#if FW_SAFE_USED
  case FW_SAFE: ret=fg_chdrive(fm,fw_volumes[num].conv);
    if (!ret) fm->fw_curdrive=num;
   break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

/****************************************************************************
 *
 * fw_getversion
 *
 * returns with the filesystem version string
 *
 * INPUTS
 *
 * fatver - where to set fat version pointer
 * safever - where to set safe version pointer
 *
 *
 ***************************************************************************/

void fw_getversion(char **ver, char **fatver, char **safever)
{
#ifdef HCC_UNICODE
  *ver="WL_UNI ver: 1.36";
#else
    *ver="WL ver: 1.36";
#endif
#if FW_FAT_USED
  *fatver=fn_getversion();
#else
  *fatver=0;
#endif
#if FW_SAFE_USED
  *safever=fg_getversion();
#else
  *safever=0;
#endif
}

/****************************************************************************
 *
 * fw_mountfat
 *
 * initiate a volume, this function has to be called 1st to set physical
 * driver function to a given volume
 *
 * INPUTS
 *
 * drvnum - which drive need to be initialized
 * driver_init - function pointer which will be called to get driver functions
 * driver_param - parameter which will be passed to driver funtion
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

#if FW_FAT_USED

int fw_mountfat(int drvnum,F_DRIVERINIT driver_init,unsigned long driver_param)
{
  int ret=F_ERR_INVALIDDRIVE;
  int a;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  if (drvnum<0 || drvnum>'Z'-'A') return F_SETLASTERROR(F_ERR_INVALIDDRIVE);

  for (a=0; a<FW_MAXVOLUME; a++)
  {
    if (fw_volumes[a].type!=FW_NONE)
    {
      if (fw_volumes[a].letter==drvnum) return F_SETLASTERROR(F_ERR_DRVALREADYMNT);
    }
  }

  for (a=0; a<FW_MAXVOLUME; a++)
  {
    if (fw_volumes[a].orig==FW_FAT)
    {
      if (fw_volumes[a].type==FW_NONE)
      {

    fw_volumes[a].type=FW_FAT;
    fw_volumes[a].letter=drvnum;
  ret=fn_initvolume(fm,fw_volumes[a].conv,driver_init,driver_param);
  break;
      }
    }
  }

  _fw_mutex_put(fm);

  return F_SETLASTERROR(ret);
}

#endif

/****************************************************************************
 *
 * fw_mountsafe
 *
 * Mounting a new drive into filesystem
 *
 * INPUTS
 *
 * drivenum - which drive number is needed to be mount (0-A, 1-B, 2-C)
 * buffer - buffer address where internal memory allocation does
 * buffersize - size in bytes of the useable buffer
 * mountfunc - which drive to be mounted (e.g. fs_mount_ramdrive)
 * phyfunc - physical flash driver if its needed (RAMDRIVE has no physical!)
 *
 * RETURNS
 *
 * 0 - if no error and successfully mounted
 * other if any error FS_VOL_xxx error code
 *
 ***************************************************************************/

#if FW_SAFE_USED

int fw_mountsafe(int drivenum,void *buffer,long buffsize,FS_DRVMOUNT mountfunc,FS_PHYGETID phyfunc)
{
  int a;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  if (drivenum<0 || drivenum>'Z'-'A') return F_SETLASTERROR(F_ERR_INVALIDDRIVE);

  for (a=0; a<FW_MAXVOLUME; a++)
  {
    if (fw_volumes[a].type!=FW_NONE)
    {
      if (fw_volumes[a].letter==drivenum) return F_SETLASTERROR(F_ERR_DRVALREADYMNT);
    }
  }

  for (a=0; a<FW_MAXVOLUME; a++)
  {
    if (fw_volumes[a].orig==FW_SAFE)
    {
      if (fw_volumes[a].type==FW_NONE)
      {
  int ret,cou;
  fw_volumes[a].type=FW_SAFE;
    fw_volumes[a].letter=drivenum;
  ret=fg_mountdrive(fw_volumes[a].conv,buffer,buffsize,mountfunc,phyfunc);
  if (ret) return F_SETLASTERROR(ret);

  for (a=0,cou=0; a<FW_MAXVOLUME; a++)
  {
    if (fw_volumes[a].type!=FW_NONE) cou++;
  }

  if (cou==1) return fw_chdrive(drivenum);
  return F_NO_ERROR;
      }
    }
  }

  return F_SETLASTERROR(F_ERR_INVALIDDRIVE);
}

#endif

/****************************************************************************
 *
 * fw_unmountdrive
 *
 * unmount a drive, it seta the status of the volume to FS_VOL_NOTMOUNT
 * memory deallocation task is the host's task
 *
 * INPUTS
 *
 * drivenum - which drive needs to be unmounted
 *
 * RETURN
 *
 * errorcode or zero if successful
 *
 ***************************************************************************/

int fw_unmountdrive (int drivenum)
{
  int num;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkdrive(drivenum,&num))
  {
#if FW_FAT_USED
  case FW_FAT:
  fw_volumes[num].type=FW_NONE;
  return fn_delvolume(fm,fw_volumes[num].conv);
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  fw_volumes[num].type=FW_NONE;
  return fg_unmountdrive(fm,fw_volumes[num].conv);
#endif
  }

  _fw_mutex_put(fm); 

  return F_SETLASTERROR(F_ERR_INVALIDDRIVE);
}


/****************************************************************************
 *
 * fw_get_volume_count
 *
 * Returns the number of mounted volumes
 *
 * RETURN
 *
 * number of mounted volumes
 *
 ***************************************************************************/

int fw_get_volume_count ()
{
  int rc=0;
  int i;

  for (i=0;i<FW_MAXVOLUME;i++)
  {
    if (fw_volumes[i].type!=FW_NONE) ++rc;
  }

  return rc;
}

/****************************************************************************
 *
 * fw_get_volume_list
 *
 * getting a list of existing volumes
 *
 * INPUTS
 *
 * varray - which is an array of FW_MAXVOLUME, where to store existing volume
 *
 * RETURNS
 *
 * number of existing volume
 *
 ***************************************************************************/

int fw_get_volume_list(int *varray)
{
  int a;
  int cou=0;

  for (a=0; a<FW_MAXVOLUME; a++)
  {
    if (fw_volumes[a].type!=FW_NONE)
    {
      *varray++=fw_volumes[a].letter;
      cou++;
    }
  }

  return cou;
}

/****************************************************************************
 *
 * fw_format
 *
 * format a media, 1st it checks existing formatting, then master boot record,
 * then media physical
 *
 * INPUTS
 *
 * drivenum - which drive format is needed
 * type - one of this definitions F_FAT12_MEDIA,F_FAT16_MEDIA,F_FAT32_MEDIA
 *  fattype is ignored on safe drive
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_format(int drivenum,long type)
{
  int ret;
  int num;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkdrive(drivenum,&num))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_format(fm,fw_volumes[num].conv,type);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_format(fm,fw_volumes[num].conv);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

/****************************************************************************
 *
 * fw_chdir
 *
 * change current working directory
 *
 * INPUTS
 *
 * dirname - new working directory name
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int fw_chdir(const char *dirname)
{
  int ret;
  char dirname2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkpath(fm,dirname,dirname2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_chdir(fm,dirname2);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_chdir(fm,dirname2);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

#ifdef HCC_UNICODE
int fw_wchdir(const W_CHAR *dirname)
{
  int ret;
  W_CHAR dirname2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_wcheckpath(fm,dirname,dirname2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_wchdir(fm,dirname2);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_wchdir(fm,dirname2);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
   }

   _fw_mutex_put(fm);
   return ret;
}
#endif

/****************************************************************************
 *
 * fw_mkdir
 *
 * making a new directory
 *
 * INPUTS
 *
 * dirname - new directory name
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
 
int fw_mkdir(const char *dirname)
{
  int ret;
  char dirname2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkpath(fm,dirname,dirname2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_mkdir(fm,dirname2);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_mkdir(fm,dirname2);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}


#ifdef HCC_UNICODE
int fw_wmkdir(const W_CHAR *dirname)
{
  int ret;
  W_CHAR dirname2[F_MAXPATH];
  FW_MULTI *fm;
  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_wcheckpath(fm,dirname,dirname2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_wmkdir(fm,dirname2);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_wmkdir(fm,dirname2);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}
#endif

/****************************************************************************
 *
 * fw_rmdir
 *
 * Remove directory, only could be removed if empty
 *
 * INPUTS
 *
 * dirname - which directory needed to be removed
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_rmdir(const char *dirname)
{
  int ret;
  char dirname2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkpath(fm,dirname,dirname2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_rmdir(fm,dirname2);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_rmdir(fm,dirname2);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

#ifdef HCC_UNICODE
int fw_wrmdir(const W_CHAR *dirname)
{
  int ret;
  W_CHAR dirname2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_wcheckpath(fm,dirname,dirname2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_wrmdir(fm,dirname2);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_wrmdir(fm,dirname2);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}
#endif

/****************************************************************************
 *
 * fw_getcwd
 *
 * getting a current working directory of current drive
 *
 * INPUTS
 *
 * buffer - where to store current working folder
 * maxlen - buffer length (possible size is F_MAXPATH)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_getcwd(char *buffer, int maxlen )
{
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  return fw_getdcwd(fw_volumes[fm->fw_curdrive].letter,buffer,maxlen);
}

#ifdef HCC_UNICODE
int fw_wgetcwd(W_CHAR *buffer, int maxlen )
{
  FW_MULTI *fm;

    if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  return fw_wgetdcwd(fw_volumes[fm->fw_curdrive].letter,buffer,maxlen);
}
#endif

/****************************************************************************
 *
 * fw_getdcwd
 *
 * getting a drive current working directory
 *
 * INPUTS
 *
 * drivenum - drive number of which drive current folder needed
 * buffer - where to store current working folder
 * maxlen - buffer length (possible size is F_MAXPATH)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_getdcwd(int drivenum,char *buffer, int maxlen )
{
  int ret;
  int num;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkdrive(drivenum,&num))
  {
#if FW_FAT_USED
  case FW_FAT:
  /*buffer[0]=FW_SEPARATORCHAR;
  ret=fn_getdcwd(fm,fw_volumes[num].conv,buffer+1,maxlen-1);*/
  ret=fn_getdcwd(fm,fw_volumes[num].conv,buffer,maxlen-1);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  buffer[0]=FW_SEPARATORCHAR;
  ret=fg_getdcwd(fm,fw_volumes[num].conv,buffer+1,maxlen-1);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}


#ifdef HCC_UNICODE
int fw_wgetdcwd(int drivenum,W_CHAR *buffer, int maxlen )
{
  int ret;
  int num;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkdrive(drivenum,&num))
  {
#if FW_FAT_USED
  case FW_FAT:
  /*buffer[0]=FW_SEPARATORCHAR;
  ret=fn_wgetdcwd(fm,fw_volumes[num].conv,buffer+1,maxlen-1);*/
  ret=fn_wgetdcwd(fm,fw_volumes[num].conv,buffer,maxlen-1);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  buffer[0]=FW_SEPARATORCHAR;
  ret=fg_wgetdcwd(fm,fw_volumes[num].conv,buffer+1,maxlen-1);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}
#endif

/****************************************************************************
 *
 * fw_rename
 *
 * Rename file or directory
 *
 * INPUTS
 *
 * filename - filename or directory name (with or without path)
 * newname - new name of the file or directory (without path)
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int fw_rename(const char *filename, const char *newname)
{
  int ret;
  char filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_rename(fm,filename2,newname);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_rename(fm,filename2,newname);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

#ifdef HCC_UNICODE
int fw_wrename(const W_CHAR *filename, const W_CHAR *newname)
{
  int ret;
  W_CHAR filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_wcheckpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_wrename(fm,filename2,newname);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_wrename(fm,filename2,newname);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}
#endif

/****************************************************************************
 *
 * fw_move
 *
 * Move file or directory on drive
 *
 * INPUTS
 *
 * filename - filename or directory name (with or without path)
 * newname - new name of the file or directory (with path)
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int fw_move(const char *filename, const char *newname)
{
  int ret;
  char filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_move(fm,filename2,newname);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_move(fm,filename2,newname);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

#ifdef HCC_UNICODE
int fw_wmove(const W_CHAR *filename, const W_CHAR *newname)
{
  int ret;
  W_CHAR filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_wcheckpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_wmove(fm,filename2,newname);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_wmove(fm,filename2,newname);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}
#endif

/****************************************************************************
 *
 * fw_delete
 *
 * delete a file
 *
 * INPUTS
 *
 * filename - file which wanted to be deleted (with or without path)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_delete(const char *filename)
{
  int ret;
  char filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_delete(fm,filename2);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_delete(fm,filename2);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

#ifdef HCC_UNICODE
int fw_wdelete(const W_CHAR *filename)
{
  int ret;
  W_CHAR filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_wcheckpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_wdelete(fm,filename2);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_wdelete(fm,filename2);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}
#endif

/****************************************************************************
 *
 * fw_filelength
 *
 * Get a file length
 *
 * INPUTS
 *
 * filename - file whose length is needed
 *
 * RETURNS
 *
 * length of the file
 *
 ***************************************************************************/

long fw_filelength(const char *filename)
{
  int ret=-1;
  char filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return ret;

  switch (_fw_checkpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_filelength(fm,filename2);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_filelength(fm,filename2);
  break;
#endif
  default:
  F_SETLASTERROR_NORET(F_ERR_INVALIDDRIVE);
  }

  _fw_mutex_put(fm);
  return ret;
}

#ifdef HCC_UNICODE
long fw_wfilelength(const W_CHAR *filename)
{
  int ret=-1;
  W_CHAR filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return ret;

  switch (_fw_wcheckpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_wfilelength(fm,filename2);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_wfilelength(fm,filename2);
  break;
#endif
  default:
  F_SETLASTERROR_NORET(F_ERR_INVALIDDRIVE);
  }

  _fw_mutex_put(fm);
  return ret;
}
#endif

/****************************************************************************
 *
 * fw_findfirst
 *
 * find a file(s) or directory(s) in directory
 *
 * INPUTS
 *
 * filename - filename (with or without wildcards)
 * find - where to store found file information
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_findfirst(const char *filename,FW_FIND *find)
{
  int ret;
  char filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  {
    FN_FIND f;
    ret=fn_findfirst(fm,filename2,&f);
    if (!ret)
    {
      (void)memcpy(find->filename,f.filename,sizeof(find->filename));
      find->attr=f.attr;
      find->ctime=f.ctime;
      find->cdate=f.cdate;
      find->filesize=f.filesize;
      find->type=FW_FAT;
      find->common.fat.findfsname=f.findfsname;
      find->common.fat.pos=f.pos;
    }
    break;
  }
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  {
    FS_FIND f;
    ret=fg_findfirst(fm,filename2,&f);
    if (!ret)
    {
      (void)memcpy(find->filename,f.filename,sizeof(find->filename));

      find->attr=(unsigned char)(f.secure>>(31-6));
      find->ctime=f.ctime;
      find->cdate=f.cdate;
      find->filesize=f.filesize;
      find->type=FW_SAFE;
      find->common.safe.findfsname=f.findfsname;
      find->common.safe.findpos=f.findpos;
    }
  }
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  if (ret) find->type=FW_NONE;

  _fw_mutex_put(fm);
  return ret;
}

#ifdef HCC_UNICODE
int fw_wfindfirst(const W_CHAR *filename,FW_WFIND *find)
{
  int ret;
  wchar filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_wcheckpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  {
    FN_WFIND f;
    ret=fn_wfindfirst(fm,filename2,&f);
    if (!ret)
    {
      (void)memcpy(find->filename,f.filename,sizeof(find->filename));
      find->attr=f.attr;
      find->ctime=f.ctime;
      find->cdate=f.cdate;
      find->filesize=f.filesize;
      find->type=FW_FAT;
      find->common.fat.findfsname=f.findfsname;
      find->common.fat.pos=f.pos;
    }
  }
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  {
    FS_WFIND f;
    ret=fg_wfindfirst(fm,filename2,&f);
    if (!ret)
    {
      (void)memcpy(find->filename,f.filename,sizeof(find->filename));

      find->attr=(unsigned char)(f.secure>>(31-6));
      find->ctime=f.ctime;
      find->cdate=f.cdate;
      find->filesize=f.filesize;
      find->type=FW_SAFE;
      find->common.safe.findfsname=f.findfsname;
      find->common.safe.findpos=f.findpos;
    }
  }
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  if (ret) find->type=FW_NONE;

  _fw_mutex_put(fm);
  return ret;
}
#endif

/****************************************************************************
 *
 * fw_findnext
 *
 * find further file(s) or directory(s) in directory
 *
 * INPUTS
 *
 * find - where to store found file information (findfirst should call 1st)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_findnext(FW_FIND *find)
{
  int ret;
  FW_MULTI *fm;
  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (find->type)
  {
#if FW_FAT_USED
  case FW_FAT:
  {
    FN_FIND f;

    f.findfsname=find->common.fat.findfsname;
    f.pos=find->common.fat.pos;

    ret=fn_findnext(fm,&f);
    if (!ret)
    {
      (void)memcpy(find->filename,f.filename,sizeof(find->filename));
      find->attr=f.attr;
      find->ctime=f.ctime;
      find->cdate=f.cdate;
      find->filesize=f.filesize;
      find->common.fat.findfsname=f.findfsname;
      find->common.fat.pos=f.pos;
    }
  }
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  {
    FS_FIND f;

    f.findfsname=find->common.safe.findfsname;
    f.findpos=find->common.safe.findpos;

    ret=fg_findnext(fm,&f);
    if (!ret)
    {
      (void)memcpy(find->filename,f.filename,sizeof(find->filename));

      find->attr=(unsigned char)(f.secure>>(31-6));
      find->ctime=f.ctime;
      find->cdate=f.cdate;
      find->filesize=f.filesize;
      find->common.safe.findfsname=f.findfsname;
      find->common.safe.findpos=f.findpos;
    }
  }
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

#ifdef HCC_UNICODE
int fw_wfindnext(FW_WFIND *find)
{
  int ret;
  FW_MULTI *fm;
  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (find->type)
  {
#if FW_FAT_USED
  case FW_FAT:
  {
    FN_WFIND f;

    f.findfsname=find->common.fat.findfsname;
    f.pos=find->common.fat.pos;

    ret=fn_wfindnext(fm,&f);
    if (!ret)
    {
      (void)memcpy(find->filename,f.filename,sizeof(find->filename));

      find->attr=f.attr;
      find->ctime=f.ctime;
      find->cdate=f.cdate;
      find->filesize=f.filesize;
      find->common.fat.findfsname=f.findfsname;
      find->common.fat.pos=f.pos;
    }
  }
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  {
    FS_WFIND f;

    f.findfsname=find->common.safe.findfsname;
    f.findpos=find->common.safe.findpos;

    ret=fg_wfindnext(fm,&f);
    if (!ret)
    {
      (void)memcpy(find->filename,f.filename,sizeof(find->filename));

      find->attr=(unsigned char)(f.secure>>(31-6));
      find->ctime=f.ctime;
      find->cdate=f.cdate;
      find->filesize=f.filesize;
      find->common.safe.findfsname=f.findfsname;
      find->common.safe.findpos=f.findpos;
    }
  }
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}
#endif

/****************************************************************************
 *
 * fw_open
 *
 * open a file for reading/writing/appending
 *
 * INPUTS
 *
 * filename - which file need to be opened
 * mode - string how to open ("r"-read, "w"-write, "w+"-overwrite, "a"-append
 *
 * RETURNS
 *
 * FN_FILE pointer if successfully
 * 0 - if any error
 *
 ***************************************************************************/

FW_FILE *fw_open(const char *filename,const char *mode)
{
  FW_FILE *file=0;
  int a;
  char filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return 0;

  if (fw_mutex_get(&fw_gmutex)) 
  {
     F_SETLASTERROR_NORET(F_ERR_BUSY);
    return 0;
  }

  for (a=0; a<FW_MAXFILE; a++)
  {
    if (fw_files[a].drvtype==FW_NONE)
    {
      file=&fw_files[a];
      file->drvtype=FW_LOCKED; /* locked it */
      break;
    }
  }

  fw_mutex_put(&fw_gmutex);

  if (!file) 
  {
     F_SETLASTERROR_NORET(F_ERR_NOMOREENTRY);
    return 0; /* no more */
  }

  switch (_fw_checkpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  {
    FN_FILE *f=fn_open(fm,filename2,mode);
    if (!f) 
    {
      file->drvtype=FW_NONE; /* release it */
      file=0;
    }
    else
    {
      file->drvtype=FW_FAT;
      file->ptr=(void*)f;
    }
  }
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  {
    FS_FILE *f=fg_open(fm,filename2,mode);
    if (!f) 
    {
      file->drvtype=FW_NONE; /* release it */
      file=0;
    }
    else
    {
      file->drvtype=FW_SAFE;
      file->ptr=(void*)f;
    }
  }
  break;
#endif
  default:
  file->drvtype=FW_NONE; /* release it */
  file=0;
  F_SETLASTERROR_NORET(F_ERR_INVALIDDRIVE);
  }

  _fw_mutex_put(fm);
  return file;
}

#ifdef HCC_UNICODE
FW_FILE *fw_wopen(const W_CHAR *filename,const W_CHAR *mode)
{
  FW_FILE *file=0;
  int a;
  W_CHAR filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return 0;

  if (fw_mutex_get(&fw_gmutex)) 
  {
     F_SETLASTERROR_NORET(F_ERR_BUSY);
    return 0;
  }

  for (a=0; a<FW_MAXFILE; a++)
  {
    if (fw_files[a].drvtype==FW_NONE)
    {
      file=&fw_files[a];
      file->drvtype=FW_LOCKED; /* locked it */
      break;
    }
  }

  fw_mutex_put(&fw_gmutex);

  if (!file) 
  {
     F_SETLASTERROR_NORET(F_ERR_NOMOREENTRY);
    return 0; /* no more */
  }

  switch (_fw_wcheckpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  {
    FN_FILE *f=fn_wopen(fm,filename2,mode);
    if (!f) 
    {
      file->drvtype=FW_NONE; /* release it */
      file=0;
    }
    else
    {
      file->drvtype=FW_FAT;
      file->ptr=(void*)f;
    }
    }
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  {
    FS_FILE *f=fg_wopen(fm,filename2,mode);
    if (!f) 
    {
      file->drvtype=FW_NONE; /* release it */
      file=0;
    }
    else
    {
      file->drvtype=FW_SAFE;
      file->ptr=(void*)f;
    }
    }
  break;
#endif
   default:
  file->drvtype=FW_NONE; /* release it */
  file=0;
  F_SETLASTERROR_NORET(F_ERR_INVALIDDRIVE);
   }

   _fw_mutex_put(fm);
   return file;
}
#endif


/****************************************************************************
 *
 * fw_write
 *
 * write data into file
 *
 * INPUTS
 *
 * buf - where the store data
 * size - size of items to be read
 * size_st - number of items need to be read
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * with the number of read bytes
 *
 ***************************************************************************/

long fw_write(const void *buf,long size,long size_st,FW_FILE *filehandle)
{
  long ret=0;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return 0;

  switch (_fw_checkfile(filehandle))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_write(fm,buf,size,size_st,(FN_FILE*)filehandle->ptr);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_write(fm,buf,size,size_st,(FS_FILE*)filehandle->ptr);
  break;
#endif
  default:
  F_SETLASTERROR_NORET(F_ERR_INVALIDDRIVE);

  }

  _fw_mutex_put(fm);
  return ret;
}

/****************************************************************************
 *
 * fw_read
 *
 * read data from file
 *
 * INPUTS
 *
 * buf - where the store data
 * size - size of items to be read
 * size_st - number of items need to be read
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * with the number of read bytes
 *
 ***************************************************************************/

long fw_read(void *buf,long size,long size_st,FW_FILE *filehandle)
{
  long ret=0;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return 0;

  switch (_fw_checkfile(filehandle))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_read(fm,buf,size,size_st,(FN_FILE*)filehandle->ptr);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_read(fm,buf,size,size_st,(FS_FILE*)filehandle->ptr);
  break;
#endif
  default:
  F_SETLASTERROR_NORET(F_ERR_INVALIDDRIVE);

  }

  _fw_mutex_put(fm);
  return ret;
}

/****************************************************************************
 *
 * fw_seek
 *
 * moves position into given offset in given file
 *
 * INPUTS
 *
 * filehandle - FW_FILE structure which file position needed to be modified
 * offset - relative position
 * whence - where to calculate position (F_SEEK_SET,F_SEEK_CUR,F_SEEK_END)
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int fw_seek(FW_FILE *filehandle,long offset,long whence)
{
  int ret;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkfile(filehandle))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_seek(fm,(FN_FILE*)filehandle->ptr,offset,whence);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_seek(fm,(FS_FILE*)filehandle->ptr,offset,whence);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

/****************************************************************************
 *
 * fw_tell
 *
 * Tells the current position of opened file
 *
 * INPUTS
 *
 * filehandle - which file needs the position
 *
 * RETURNS
 *
 * position in the file from start
 *
 ***************************************************************************/

long fw_tell(FW_FILE *filehandle)
{
  long ret=0;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return 0;

  switch (_fw_checkfile(filehandle))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_tell(fm,(FN_FILE*)filehandle->ptr);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_tell(fm,(FS_FILE*)filehandle->ptr);
  break;
#endif
  default:
  F_SETLASTERROR_NORET(F_ERR_INVALIDDRIVE);
  }

  _fw_mutex_put(fm);
  return ret;
}

/****************************************************************************
 *
 * fw_eof
 *
 * Tells if the current position is end of file or not
 *
 * INPUTS
 *
 * filehandle - which file needs the checking
 *
 * RETURNS
 *
 * 0 - if not EOF
 * other - if EOF or invalid file handle
 *
 ***************************************************************************/

int fw_eof(FW_FILE *filehandle)
{
  int ret;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkfile(filehandle))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_eof(fm,(FN_FILE*)filehandle->ptr);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_eof(fm,(FS_FILE*)filehandle->ptr);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

/****************************************************************************
 *
 * fw_rewind
 *
 * set the fileposition in the opened file to the begining
 *
 * INPUTS
 *
 * filehandle - which file needs to be rewinded
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_rewind(FW_FILE *filehandle)
{
  int ret;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkfile(filehandle))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_rewind(fm,(FN_FILE*)filehandle->ptr);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_rewind(fm,(FS_FILE*)filehandle->ptr);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

/****************************************************************************
 *
 * fw_putc
 *
 * write a character into file
 *
 * INPUTS
 *
 * ch - what to write into file
 * filehandle - file where to write
 *
 * RETURNS
 *
 * with the written character or -1 if any error
 *
 ***************************************************************************/

int fw_putc(int ch,FW_FILE *filehandle)
{
  int ret=-1;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return -1;

  switch (_fw_checkfile(filehandle))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_putc(fm,ch,(FN_FILE*)filehandle->ptr);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_putc(fm,ch,(FS_FILE*)filehandle->ptr);
  break;
#endif
  default:
  F_SETLASTERROR_NORET(F_ERR_INVALIDDRIVE);
  }

  _fw_mutex_put(fm);
  return ret;
}

/****************************************************************************
 *
 * fw_getc
 *
 * get a character from file
 *
 * INPUTS
 *
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * with the read character or -1 if read was not successfully
 *
 ***************************************************************************/

int fw_getc(FW_FILE *filehandle)
{
  int ret=-1;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return -1;

  switch (_fw_checkfile(filehandle))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_getc(fm,(FN_FILE*)filehandle->ptr);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_getc(fm,(FS_FILE*)filehandle->ptr);
  break;
#endif
  default:
  F_SETLASTERROR_NORET(F_ERR_INVALIDDRIVE);
  }

  _fw_mutex_put(fm);
  return ret;
}

/****************************************************************************
 *
 * fw_close
 *
 * close a previously opened file
 *
 * INPUTS
 *
 * filehandle - which file needs to be closed
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_close(FW_FILE *filehandle)
{
  int ret;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkfile(filehandle))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_close(fm,(FN_FILE*)filehandle->ptr);
  filehandle->drvtype=FW_NONE;
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_close(fm,(FS_FILE*)filehandle->ptr);
  filehandle->drvtype=FW_NONE;
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}


/****************************************************************************
 *
 * fn_flush
 *
 * flushing current content a file into physical.
 *
 * INPUTS
 *
 * filehandle - which file needs to be flushed
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int fw_flush(FW_FILE *filehandle)
{
  int ret;
  FW_MULTI *fm;
  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkfile(filehandle))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_flush(fm,(FN_FILE*)filehandle->ptr);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_flush(fm,(FS_FILE*)filehandle->ptr);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

/****************************************************************************
 *
 * fw_settimedate
 *
 * set a file time and date
 *
 * INPUTS
 *
 * filename - which file time and date wanted to be set
 * ctime - new ctime of the file
 * cdate - new cdate of the file
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_settimedate(const char *filename,unsigned short ctime,unsigned short cdate)
{
  int ret;
  char filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_settimedate(fm,filename2,ctime,cdate);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_settimedate(fm,filename2,ctime,cdate);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

#ifdef HCC_UNICODE
int fw_wsettimedate(const W_CHAR *filename,unsigned short ctime,unsigned short cdate)
{
  int ret;
  W_CHAR filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_wcheckpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_wsettimedate(fm,filename2,ctime,cdate);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_wsettimedate(fm,filename2,ctime,cdate);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}
#endif

/****************************************************************************
 *
 * fw_gettimedate
 *
 * get a file time and date
 *
 * INPUTS
 *
 * filename - which file time and date wanted to be retrive
 * pctime - ctime of the file where to store
 * pcdate - cdate of the file where to store
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_gettimedate(const char *filename,unsigned short *pctime,unsigned short *pcdate)
{
  int ret;
  char filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_gettimedate(fm,filename2,pctime,pcdate);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_gettimedate(fm,filename2,pctime,pcdate);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

#ifdef HCC_UNICODE
int fw_wgettimedate(const W_CHAR *filename,unsigned short *pctime,unsigned short *pcdate)
{
  int ret;
  W_CHAR filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_wcheckpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_wgettimedate(fm,filename2,pctime,pcdate);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_wgettimedate(fm,filename2,pctime,pcdate);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}
#endif

/****************************************************************************
 *
 * fw_getfreespace
 *
 * get total/free/used/bad diskspace
 *
 * INPUTS
 *
 * drivenum - which drive free space is requested (0-A, 1-B, 2-C)
 * pspace - pointer where to store the information
 *
 * RETURNS
 * error code
 *
 ***************************************************************************/

int fw_getfreespace(int drivenum, FW_SPACE *pspace)
{
  int ret;
  int num;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkdrive(drivenum,&num))
  {
#if FW_FAT_USED
  case FW_FAT:
  {
    FN_SPACE space;

    ret=fn_getfreespace(fm,fw_volumes[num].conv,&space);
    pspace->total=space.total;
    pspace->free=space.free;
    pspace->used=space.used;
    pspace->bad=space.bad;
    pspace->total_high=space.total_high;
    pspace->free_high=space.free_high;
    pspace->used_high=space.used_high;
    pspace->bad_high=space.bad_high;
    break;
  }
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  {
    FS_SPACE space;

    ret=fg_getfreespace(fm,fw_volumes[num].conv,&space);
    pspace->total=space.total;
    pspace->free=space.free;
    pspace->used=space.used;
    pspace->bad=space.bad;
    pspace->total_high=0;
    pspace->free_high=0;
    pspace->used_high=0;
    pspace->bad_high=0;
    break;
    }
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

/****************************************************************************
 *
 * fw_getattr
 *
 * get file attribute
 *
 * INPUTS
 *
 * filename - which file attribute is needed
 * attr - pointer to a characterter where to store attribute information
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_getattr(const char *filename,unsigned char *attr)
{
  int ret;
  char filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_getattr(fm,filename2,attr);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  {
    unsigned long sec;
    ret=fg_getpermission(fm,filename2,&sec);
    if (!ret)
    {
      *attr=(unsigned char)(sec>>(31-6));
    }
    break;
  }
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

#ifdef HCC_UNICODE
int fw_wgetattr(const W_CHAR *filename,unsigned char *attr)
{
  int ret;
  W_CHAR filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_wcheckpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_wgetattr(fm,filename2,attr);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  {
    unsigned long sec;
    ret=fg_wgetpermission(fm,filename2,&sec);
    if (!ret)
    {
      *attr=(unsigned char)(sec>>(31-6));
    }
    break;
  }
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}
#endif

/****************************************************************************
 *
 * fw_setattr
 *
 * set attribute of a file
 *
 * INPUTS
 *
 * filename - which file attribute set
 * attr - new attribute of the file
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_setattr(const char *filename,unsigned char attr)
{
  int ret;
  char filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_setattr(fm,filename2,attr);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  {
    unsigned long sec=attr;
    sec=sec<<(31-6);
    ret=fg_setpermission(fm,filename2,sec);
  }
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

#ifdef HCC_UNICODE
int fw_wsetattr(const W_CHAR *filename,unsigned char attr)
{
  int ret;
  W_CHAR filename2[F_MAXPATH];
  FW_MULTI *fm;
  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_wcheckpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_wsetattr(fm,filename2,attr);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  {
    unsigned long sec=attr;
    sec<<=31-6;
    ret=fg_wsetpermission(fm,filename2,sec);
  }
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}
#endif

/****************************************************************************
 *
 * fw_getlabel
 *
 * get a label of a media
 *
 * INPUTS
 *
 * drivenum - drive number which label's is needed
 * label - char pointer where to store label
 * len - length of label buffer
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_getlabel(int drivenum, char *label, long len)
{
  int ret;
  int num;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkdrive(drivenum,&num))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_getlabel(fm,fw_volumes[num].conv,label,len);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_getlabel(fm,fw_volumes[num].conv,label,len);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

/****************************************************************************
 *
 * fw_setlabel
 *
 * set a label of a media
 *
 * INPUTS
 *
 * drivenum - drive number which label's need to be set
 * label - new label for the media
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_setlabel(int drivenum, const char *label)
{
  int ret;
  int num;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkdrive(drivenum,&num))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_setlabel(fm,fw_volumes[num].conv,label);
    break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_setlabel(fm,fw_volumes[num].conv,label);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

/****************************************************************************
 *
 * fw_truncate
 *
 * truncate a file to a specified length, filepointer will be set to the
 * end
 *
 * INPUTS
 *
 * filename - which file need to be truncated
 * length - length of new file
 *
 * RETURNS
 *
 * filepointer or zero if fails
 *
 ***************************************************************************/

FW_FILE *fw_truncate(const char *filename,unsigned long length)
{
  FW_FILE *file=0;
  int a;
  char filename2[F_MAXPATH];
  FW_MULTI *fm;
  if (_fw_gettask(&fm)) return 0;

  if (fw_mutex_get(&fw_gmutex)) 
  {
    F_SETLASTERROR_NORET(F_ERR_BUSY);
    return 0;
  }

  for (a=0; a<FW_MAXFILE; a++)
  {
    if (fw_files[a].drvtype==FW_NONE)
    {
      file=&fw_files[a];
      file->drvtype=FW_LOCKED; /* locked it */
      break;
    }
  }

  fw_mutex_put(&fw_gmutex);

  if (!file) 
  {
    F_SETLASTERROR_NORET(F_ERR_NOMOREENTRY);
    return 0; /* no more */
  }

  switch (_fw_checkpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  {
    FN_FILE *f=fn_truncate(fm,filename2,length);
    if (!f) 
    {
      file->drvtype=FW_NONE; /* release it */
      file=0;
    }
    else
    {
      file->drvtype=FW_FAT;
      file->ptr=(void*)f;
    }
  }
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  {
    FS_FILE *f=fg_truncate(fm,filename2,length);
    if (!f) 
    {
      file->drvtype=FW_NONE; /* release it */
      file=0;
    }
    else {
      file->drvtype=FW_SAFE;
      file->ptr=(void*)f;
    }
    }
  break;
#endif
  default:
  file->drvtype=FW_NONE; /* release it */
  file=0;
  F_SETLASTERROR_NORET(F_ERR_INVALIDDRIVE);
  }

  _fw_mutex_put(fm);
  return file;
}

#ifdef HCC_UNICODE
FW_FILE *fw_wtruncate(const W_CHAR *filename,unsigned long length)
{
  FW_FILE *file=0;
  int a;
  W_CHAR filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return 0;

  if (fw_mutex_get(&fw_gmutex)) 
  {
    F_SETLASTERROR_NORET(F_ERR_BUSY);
    return 0;
  }

  for (a=0; a<FW_MAXFILE; a++)
  {
    if (fw_files[a].drvtype==FW_NONE)
    {
      file=&fw_files[a];
      file->drvtype=FW_LOCKED; /* locked it */
      break;
    }
  }

  fw_mutex_put(&fw_gmutex);

  if (!file) 
  {
    F_SETLASTERROR_NORET(F_ERR_NOMOREENTRY);
    return 0; /* no more */
  }

  switch (_fw_wcheckpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  {
    FN_FILE *f=fn_wtruncate(fm,filename2,length);
    if (!f) 
    {
      file->drvtype=FW_NONE; /* release it */
      file=0;
    }
    else
    {
      file->drvtype=FW_FAT;
      file->ptr=(void*)f;
    }
  }
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  {
    FS_FILE *f=fg_wtruncate(fm,filename2,length);
    if (!f) 
    {
      file->drvtype=FW_NONE; /* release it */
      file=0;
    }
    else
    {
      file->drvtype=FW_SAFE;
      file->ptr=(void*)f;
    }
    }
  break;
#endif
  default:
  file->drvtype=FW_NONE; /* release it */
  file=0;
  F_SETLASTERROR_NORET(F_ERR_INVALIDDRIVE);
  }

  _fw_mutex_put(fm);
  return file;
}
#endif

/****************************************************************************
 *
 * fw_seteof
 *
 * set end of file at the current position
 *
 * INPUTS
 *
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_seteof(FW_FILE *filehandle)
{
  int ret;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkfile(filehandle))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_seteof(fm,(FN_FILE*)filehandle->ptr);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_seteof(fm,(FS_FILE*)filehandle->ptr);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);
  return ret;
}

/****************************************************************************
 *
 * fw_stat
 *
 * get status information on a file
 *
 * INPUTS
 *
 * filename - which file time and date wanted to be retrive
 * stat - pointer where to store information
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_stat(const char *filename,F_STAT *stat)
{
  int ret;
  char filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_stat(fm,filename2,stat);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  {
    FS_STAT fs_stat;
    ret=fg_stat(fm,filename2,&fs_stat);
    stat->attr=(unsigned char)(fs_stat.secure>>(31-6));
    stat->createdate=fs_stat.createdate;
    stat->createtime=fs_stat.createtime;
    stat->drivenum=fs_stat.drivenum;
    stat->filesize=fs_stat.filesize;
    /* not used vars in effs */
    stat->lastaccessdate=0;
    stat->modifieddate=0;
    stat->modifiedtime=0;
  }
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);

  return ret;
}

#ifdef HCC_UNICODE
int fw_wstat(const W_CHAR *filename,F_STAT *stat)
{
  int ret;
  W_CHAR filename2[F_MAXPATH];
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_wcheckpath(fm,filename,filename2))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_wstat(fm,filename2,stat);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  {
    FS_STAT fs_stat;
    ret=fg_wstat(fm,filename2,&fs_stat);
    stat->attr=(unsigned char)(fs_stat.secure>>(31-6));
    stat->createdate=fs_stat.createdate;
    stat->createtime=fs_stat.createtime;
    stat->drivenum=fs_stat.drivenum;
    stat->filesize=fs_stat.filesize;
    /* not used vars in effs */
    stat->lastaccessdate=0;
    stat->modifieddate=0;
    stat->modifiedtime=0;
  }
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fw_ftruncate
 *
 * truncate a file to a specified length, filepointer will be set to the
 * end
 *
 * INPUTS
 *
 * filehandle - which file need to be truncated
 * length - length of new file
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_ftruncate(FW_FILE *filehandle,unsigned long length)
{
  int ret;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkfile(filehandle))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_ftruncate(fm,(FN_FILE*)filehandle->ptr,length);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:

  ret=fg_ftruncate(fm,(FS_FILE*)filehandle->ptr,length);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);

  return ret;
}

/****************************************************************************
 *
 * fw_checkvolume
 *
 * checking volume.is working
 *
 * INPUT:
 *
 * drvnumber - number of drive to check
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_checkvolume (int drivenum)
{
  int ret;
  int num;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkdrive(drivenum,&num))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_checkvolume(fm,fw_volumes[num].conv);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_checkvolume(fm,fw_volumes[num].conv);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);

  return ret;
}

/****************************************************************************
 *
 * fw_get_oem
 *
 * Get OEM name
 *
 * INPUTS
 *
 * drivenum - which drive information is requested
 * str - where to store OEM info
 * maxlen - length of the buffer
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fw_get_oem (int drivenum, char *str, long maxlen)
{
  int ret;
  int num;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkdrive(drivenum,&num))
  {
#if FW_FAT_USED
  case FW_FAT:
  ret=fn_get_oem(fm,fw_volumes[num].conv,str,maxlen);
  break;
#endif
#if FW_SAFE_USED
  case FW_SAFE:
  ret=fg_get_oem(fm,fw_volumes[num].conv,str,maxlen);
  break;
#endif
  default:
  ret=F_ERR_INVALIDDRIVE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);

  return ret;
}


/****************************************************************************
 *
 * fw_abortclose
 *
 * abort and close a previously opened file
 *
 * INPUTS
 *
 * filehandle - which file needs to be aborted
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int fw_abortclose(FW_FILE *filehandle)
{
  int ret;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  switch (_fw_checkfile(filehandle))
  {
#if FW_SAFE_USED
  case FW_SAFE:

  ret=fg_abortclose(fm,(FS_FILE*)filehandle->ptr);
  break;
#endif
  default:
  ret=F_ERR_NOTAVAILABLE;
  F_SETLASTERROR_NORET(ret);
  }

  _fw_mutex_put(fm);

  return ret;
}

/****************************************************************************
 *
 * fw_initvolumepartition
 *
 * initiate a volume, this function has to be called 1st to set physical
 * driver function to a given volume and a given partition
 *
 * INPUTS
 *
 * drvnumber - which drive need to be initialized
 * driver - driver to be used for this volume
 * partition - selected partition on the drive
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

#if FW_FAT_USED

int fw_mountfatpartition(int drvnum,F_DRIVER *driver, int partition)
{
  int ret=F_ERR_INVALIDDRIVE;
  int a;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  if (drvnum<0 || drvnum>'Z'-'A') return F_SETLASTERROR(F_ERR_INVALIDDRIVE);

  for (a=0; a<FW_MAXVOLUME; a++)
  {
    if (fw_volumes[a].type!=FW_NONE)
    {
      if (fw_volumes[a].letter==drvnum) return F_SETLASTERROR(F_ERR_DRVALREADYMNT);
    }
  }

  for (a=0; a<FW_MAXVOLUME; a++)
  {
    if (fw_volumes[a].orig==FW_FAT)
    {
      if (fw_volumes[a].type==FW_NONE)
      {

	fw_volumes[a].type=FW_FAT;
	fw_volumes[a].letter=drvnum;
	ret=fn_initvolumepartition(fm,fw_volumes[a].conv,driver,partition);
	break;
      }
    }
  }

  _fw_mutex_put(fm);

  return F_SETLASTERROR(ret);
}

#endif

/****************************************************************************
 *
 * fw_releasedriver
 *
 * releasing a driver
 *
 * INPUTS
 *
 * driver - driver structure
 *
 * RETURNS
 *
 * errorcode
 *
 ***************************************************************************/

#if FW_FAT_USED
int fw_releasedriver(F_DRIVER *driver)
{
  int ret;
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND;

  ret=fn_releasedriver(fm,driver);

  _fw_mutex_put(fm);

  return ret;
}
#endif

/****************************************************************************
 *
 * fw_getlasterror
 *
 * returns with the last set error
 *
 * RETURNS
 *
 * error code which was set as lasterror
 *
 ***************************************************************************/

int fw_getlasterror()
{
  FW_MULTI *fm;

  if (_fw_gettask(&fm)) return F_ERR_TASKNOTFOUND; /* busy! */

  return fm->lasterror;
}

/****************************************************************************
 *
 * fw_setlasterror
 *
 * setting errorcode in multi structure, maybe functions is not used
 * depending on thse settings in udefs_f.h
 * fn_setlasterror differs only it has no return value.
 *
 * INPUTS
 *
 * fm - multi structure where to set
 * errorcode - errorcode to be set
 *
 * RETURNS
 *
 * simple return with errorcode
 *
 ***************************************************************************/

int fw_setlasterror (FW_MULTI *fm, int errorcode)
{
  fm->lasterror=errorcode;
  return errorcode;
}

void fw_setlasterror_noret (FW_MULTI *fm, int errorcode)
{
  fm->lasterror=errorcode;
}

/****************************************************************************
 *
 * End of fw_file.c
 *
 ***************************************************************************/
