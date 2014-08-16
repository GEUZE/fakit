/*SOC
**********************************************************************
**  _______  _______ _________ _______  _______ _________ _______   **
** (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
** | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
** | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
** (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
**       ) || |         | |   | |   | || (         | |   | (   ) |  **
** /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
** \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
**                                                                  **
** (c) 2009 Sciopta Systems GmbH Deutschland, Sciopta AG/ Schweiz   **
**                                                                  **
**********************************************************************
** ID: S06163BS1                                                    **
** +Revision: 1.4.4.7 +                                                **
** +Date: 2009/10/08 13:42:23 +                                     **
** HCC integration layer                                       **
**********************************************************************
EOC*/                        
#include <sciopta.h>
#include <sciopta.msg>
#include <sdd/sdd.msg>
#include <sdd/sdd.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sfs/sfs.msg>
#include <sfs/sfs.h>
#include <logd/logd.h>

// newlib
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


// hcc
#ifdef EFFS
#include <hcc/effs/common/fsf.h>
#elif defined FATFS
#include <sfs/blkdev.h>
#include <hcc/fatfs/api_f.h>
#include <hcc/fatfs/port.h>
#else
#error Do not compile this file standalone.
#endif

#include <sfs/sc2hcc.h>
#include <sfs/hcc_fs.msg>

/*
** Local file structure as interface sciopta <-> hcc
*/
#ifdef EFFS
#define Fx_MAXPATH FS_MAXPATH
#define Fx_FILE     FS_FILE
#define Fx_FIND     FS_FIND
#define Fx_SPACE    FS_SPACE
#define FUNCTION    effs_process
#define Fx_SEEK_SET FS_SEEK_SET
#define Fx_SEEK_CUR FS_SEEK_CUR
#define Fx_SEEK_END FS_SEEK_END
#define Fx_ATTR_DIR FS_ATTR_DIR
#define INIT_FLAG    effs_flag
#else
#define Fx_MAXPATH  FN_MAXPATH
#define Fx_FILE     FN_FILE
#define Fx_FIND     FN_FIND
#define Fx_SPACE    FN_SPACE
#define FUNCTION    fatfs_process
#define Fx_SEEK_SET FN_SEEK_SET
#define Fx_SEEK_CUR FN_SEEK_CUR
#define Fx_SEEK_END FN_SEEK_END
#define Fx_ATTR_DIR F_ATTR_DIR
#define fs_getcurrenttimedate  f_getcurrenttimedate
#define fs_getdate  f_getdate
#define INIT_FLAG    fatfs_flag
#endif

#ifndef F_SECTOR_SIZE
#define F_SECTOR_SIZE 512
#endif
/*
** Local file structure as interface sciopta <-> hcc
*/
typedef struct voldesc_s {
  sc_msgid_t id;
  int drive;           /* HCC drive/volume number */
  int partition;
  char mp[32];
  int ref;
  flags_t flags;
#define VOLDESC_FLG_MOUNTED     1
#define VOLDESC_FLG_NOTFORMATED 2
#define VOLDESC_FLG_EFFS        4       /* \                      */
#define VOLDESC_FLG_FAT         8       /*  > only one may be set */
#define VOLDESC_FLG_TINY        16      /* /                      */
} voldesc_t;

typedef struct scfile_s {
  sc_msgid_t id;
  voldesc_t *volume;
  Fx_FILE *fd;
  Fx_FIND find;
  int ref;
  flags_t flags;       /* Sciopta file-flags */
  char path[Fx_MAXPATH]; /* full path */
} scfile_t;


/*
** Handy function to build a full path
*/
STATIC_INLINE char *fs_strcat(char *dst,const char *src)
{
  if ( strlen(dst) + strlen(src) > Fx_MAXPATH ){
    return NULL;
  }

  return strcat(dst,src);
}

STATIC_INLINE void fs_fullpath(char *dst,const char *path, const char *name)
{
  dst[0] = 0;
  if ( path && path[0] ){
    fs_strcat(dst,path);
  }
  if ( name && name[0] ){
    if ( dst[0] ){
      fs_strcat(dst,"/");
    }
    fs_strcat(dst,name);
  }
}



/*
** Message union
*/
union sc_msg {
  sc_msgid_t id;
  sc_procNameGetMsgReply_t nameGet;
  sdd_baseMessage_t	   base;
  sdd_obj_t		   obj;
  sdd_objDup_t		   duplicate;
  sdd_objRelease_t 	   release;
  sdd_objTime_t 	   time;
  sdd_objSize_t 	   size;
  sdd_devOpen_t 	   open;
  sdd_devClose_t 	   close;
  sdd_devRead_t 	   read;
  sdd_devWrite_t 	   write;
  sdd_fileSeek_t 	   seek;
  sdd_fileResize_t 	   resize;
  sdd_manGet_t 	   	   get;
  sdd_manGetPath_t 	   getPath;
  sdd_manGetFirst_t 	   getFirst;
  sdd_manGetNext_t 	   getNext;
  sdd_manAdd_t 	   	   add;
  sdd_manRm_t 	   	   rm;
  sdd_manNotify_t          notify;
  sfs_sync_t               sync;
  sfs_mount_t              umount;
  sfs_format_t             format;
  hcc_fatfs_mount_t        fatfs_mount;
  hcc_fatfs_partmount_t    fatfs_partmount;
  hcc_effs_mount_t         effs_mount;
};

/*
** Error map from HCC to Sciopta
*/

__u32 errMap[] = {
  /* F_NO_ERROR           0 */ 0,
  /* F_ERR_INVALIDDRIVE   1 */ ENODEV,
  /* F_ERR_NOTFORMATTED   2 */ EFTYPE,
  /* F_ERR_INVALIDDIR     3 */ ENOTDIR,
  /* F_ERR_INVALIDNAME    4 */ EINVAL,
  /* F_ERR_NOTFOUND       5 */ ENOENT,
  /* F_ERR_DUPLICATED     6 */ EEXIST,
  /* F_ERR_NOMOREENTRY    7 */ ENOSPC,
  /* F_ERR_NOTOPEN        8 */ EBADF,
  /* F_ERR_EOF            9 */ EIO,
  /* F_ERR_RESERVED      10 */ EBADF,
  /* F_ERR_NOTUSEABLE    11 */ ESPIPE,
  /* F_ERR_LOCKED        12 */ EAGAIN,
  /* F_ERR_ACCESSDENIED  13 */ EACCES,
  /* F_ERR_NOTEMPTY      14 */ ENOTEMPTY,
  /* F_ERR_INITFUNC      15 */ EIO,
  /* F_ERR_CARDREMOVED   16 */ ENOMEDIUM,
  /* F_ERR_ONDRIVE       17 */ EIO,
  /* F_ERR_INVALIDSECTOR 18 */ EIO,
  /* F_ERR_READ          19 */ EBADF,
  /* F_ERR_WRITE         20 */ EBADF,
  /* F_ERR_INVALIDMEDIA  21 */ ENODEV,
  /* F_ERR_BUSY          22 */ EBUSY,
  /* F_ERR_WRITEPROTECT  23 */ EACCES,
  /* F_ERR_INVFATTYPE    24 */ EIO,
  /* F_ERR_MEDIATOOSMALL 25 */ EINVAL,
  /* F_ERR_MEDIATOOLARG  26 */ EINVAL,
  /* F_ERR_NOTSUPPSECTORSIZE 27 */ EINVAL,
  /* F_ERR_UNKNOWN       28 */ EIO,
  /* F_ERR_DRVALREADYMNT 29 */ EEXIST,
  /* F_ERR_TOOLONGNAME   30 */ EINVAL,
  /* F_ERR_NOTFORREAD    31 */ EACCES,
};


#ifdef EFFS
int sfs_enterEFFS(const char *mp)
#elif defined(FATFS)
int sfs_enterFATFS(const char *mp)
#endif
{
  sdd_obj_t *root;
  sdd_obj_t *fs;
  int error;

  /* First get root manager */
  root = sdd_manGetRoot ("/SCP_devman", "/", SC_DEFAULT_POOL, SC_ENDLESS_TMO);
  if ( !root ){
    kprintf(1,"Error could not get root-manager\n");
    sc_miscError(SC_ERR_SYSTEM_FATAL,1);
    return ENODEV;
  }

  error = sdd_manNotifyAdd(root, mp, SC_ENDLESS_TMO);

  if ( error < 0 ){
    sdd_objFree(&root);
    return error;
  } 

  /*
  ** get FS
  */
  if (!(fs = sdd_manGetByName (root, mp))) {
    sdd_objFree(&root);
    return -1;
  }

  /*
  ** XXX: Check type of filesystem
  */

  sdd_objFree(&root);

  
  return f_enterFS();
}


/*
** EFFS Process
**
** This process is between Sciopta processes and the HCC filesystem.
** It will
** a) serialize access to the filesystem => no need for a mutex
** b) translate Sciopta's FS structure to the more DOS like system
**    of HCC
*/

static void registerDev(voldesc_t NEARPTR volume,
			const char *name, 
			logd_t NEARPTR logd)
{
  /* registration */
  sdd_obj_t NEARPTR dev;
  sdd_obj_t NEARPTR man;

  logd_printf(logd,LOGD_INFO,"Register filesystem/device (%s)\n",name);

  dev = (sdd_obj_t NEARPTR) sc_msgAlloc(sizeof (sdd_obj_t),
					SDD_OBJ,
					SC_DEFAULT_POOL, 
					SC_FATAL_IF_TMO);

  dev->base.error = 0;
  dev->base.handle = volume;
  if ( !volume ){
    /* The filesystem is only a manager, no size and could not be opened */
    dev->type = SDD_OBJ_TYPE | SDD_MAN_TYPE;
  } else {
    dev->type = SDD_OBJ_TYPE | SFS_MOUNTP_TYPE | SDD_MAN_TYPE | SFS_DIR_TYPE;
  }
  strncpy (dev->name, name, SC_NAME_MAX);
  dev->receiver =
    dev->sender =
    dev->controller = sc_procIdGet(NULL,SC_NO_TMO);

  /* register to dev man */
  man = sdd_manGetRoot ("/SCP_devman", "/", SC_DEFAULT_POOL, SC_FATAL_IF_TMO);

  if (man) {
    man->base.handle = (void *)volume->drive; /* !!! hack !!! */
    if (sdd_manAdd (man, &dev)) {
      logd_printf (logd, LOGD_SEVERE, "Could not add this device \n");
      sc_procKill (SC_CURRENT_PID, 0);
    }
    sdd_objFree (&man);
  }
  else {
    logd_printf (logd, LOGD_SEVERE, "Could not get /SCP_devman\n");
    sc_procKill (SC_CURRENT_PID, 0);
  }
}

static void unregisterDev(const char *name, logd_t NEARPTR logd)
{
  /* registration */
  sdd_obj_t NEARPTR dev;
  sdd_obj_t NEARPTR man;

  logd_printf(logd,LOGD_INFO,"Unregister device (%s)\n",name);

  dev = (sdd_obj_t NEARPTR) sc_msgAlloc(sizeof (sdd_obj_t),
					SDD_OBJ,
					SC_DEFAULT_POOL,
					SC_FATAL_IF_TMO);
  dev->base.error = 0;
  strncpy (dev->name, name, SC_NAME_MAX);

  /* unregister from dev man */
  man = sdd_manGetRoot ("/SCP_devman", "/", SC_DEFAULT_POOL, SC_FATAL_IF_TMO);

  if (man) {
    if ( sdd_manRm (man, dev, sizeof(sdd_obj_t)) ) {
      logd_printf (logd, LOGD_SEVERE, "Could not remove this filesystem \n");
      sc_procKill (SC_CURRENT_PID, 0);
    }
    sdd_objFree(&man);
    sc_msgFree((sc_msgptr_t)&dev);
  }
  else {
    logd_printf (logd, LOGD_SEVERE, "Could not get /SCP_devman\n");
    sc_procKill (SC_CURRENT_PID, 0);
  }
}

void FUNCTION(const char *fsName)
{
  char path[Fx_MAXPATH];
  scfile_t NEARPTR directory;
  scfile_t NEARPTR refFile;
  scfile_t NEARPTR file;
  voldesc_t *volume;
  sdd_obj_t NEARPTR obj = NEAR_NULL;

  __u16 time;
  __u16 day;
  off_t cur;
  flags_t accmode;
  Fx_FILE *fd;
  Fx_FIND find;
  Fx_SPACE space;
  int ret;

  sc_pid_t _this;
  sc_pid_t to;
  sc_msg_t msg;
  logd_t NEARPTR logd;

  _this = sc_procIdGet(NULL,SC_NO_TMO);

  logd = logd_new ("/SCP_logd",
		   LOGD_INFO,
		   fsName,
		   SC_DEFAULT_POOL,
		   SC_FATAL_IF_TMO);

  logd_printf(logd,LOGD_INFO,"FS Version: %s\n",f_getversion());

  /*
  ** Now register with dev_man for mounting
  */
  registerDev(NULL,fsName,logd);
#ifdef FATFS
  /*
  ** wait for volume-manager
  */
  {
    sdd_obj_t *root;
    root = sdd_manGetRoot ("/SCP_devman", "/", 
			   SC_DEFAULT_POOL, SC_FATAL_IF_TMO);
    (void) sdd_manNotifyAdd(root, "volman", SC_ENDLESS_TMO);
  }
#else
  fg_init();
#endif
  f_enterFS();
  
  ret = 0;
  for (;;) {
    msg = sc_msgRx (SC_ENDLESS_TMO, SC_MSGRX_ALL, SC_MSGRX_MSGID);
    to = sc_msgSndGet (&msg);
    switch (msg->id) {
    case SFS_FORMAT_FORCE:
    case SFS_FORMAT:
      ++msg->id;
      volume = (voldesc_t *)msg->base.handle;
      if ( !volume || volume->id != HCC_FS_MOUNTP ){
	msg->base.error = EBADF;
	break;
      }
      
      if ( !(volume->flags & VOLDESC_FLG_MOUNTED) ){
	msg->base.error = ENODEV;
	break;
      }
      logd_printf(logd,LOGD_INFO,"Formating drive <%s>\n",volume->mp);
      if ( volume->flags & VOLDESC_FLG_NOTFORMATED ||
	   msg->id == SFS_FORMAT_FORCE_REPLY )
      {
#ifdef FATFS
	if ( volume->flags & VOLDESC_FLG_FAT ){
	  ret = errMap[f_format (volume->drive, msg->format.custom)];
	}
#elif defined EFFS
	if ( volume->flags & VOLDESC_FLG_EFFS ){
	  ret = errMap[fsm_format(volume->drive)];
	}
#endif
      } else {
	ret = ENODEV;
      }
      if ( ret ){
	volume->flags |= VOLDESC_FLG_NOTFORMATED;
	logd_printf(logd,LOGD_INFO,"format <%s>failed.\n",volume->mp);
      } else {
	volume->flags &= ~VOLDESC_FLG_NOTFORMATED;
      }
      msg->base.error = ret;
      break;
    case SFS_UMOUNT:
      msg->id = SFS_UMOUNT_REPLY;
      volume = (voldesc_t *)msg->base.handle;
      
      if ( !volume || volume->id != HCC_FS_MOUNTP ) {
	msg->base.error = EBADF;
	break;
      }
      
      if ( volume->ref ){
	msg->base.error = EBUSY;
	break;
      }
      
#if defined EFFS
      if ( volume->flags & VOLDESC_FLG_EFFS ){
	ret = fsm_unmountdrive(volume->drive);
      }
#endif
      unregisterDev(volume->mp,logd);
      msg->base.error = errMap[ret];
      sc_msgFree((sc_msgptr_t)&volume);
	   
      break;
#ifdef FATFS
/*
****************************************
** HCC specific messages
****************************************
*/      
    case HCC_FATFS_MOUNT:
    case HCC_FATFS_PARTITION_MOUNT:
      msg->id = HCC_FATFS_MOUNT_REPLY;
      msg->fatfs_mount.error = 0;

      volume = (voldesc_t *)sc_msgAllocClr(sizeof(voldesc_t),
					   HCC_FS_MOUNTP,
					   SC_DEFAULT_POOL,
					   SC_FATAL_IF_TMO);
      
      if ( msg->id == HCC_FATFS_PARTITION_MOUNT ){
	msg->id = HCC_FATFS_PARTITION_MOUNT_REPLY;
	volume->partition = msg->fatfs_partmount.partition;
      }

      logd_printf(logd,LOGD_INFO,
		  "Mount physical device <%d> as drive <%s> ...\n",
		  msg->fatfs_mount.drive,
		  msg->fatfs_mount.mp);
 
      memcpy(volume->mp,msg->fatfs_mount.mp,31);
      volume->drive = msg->fatfs_mount.drive;
      volume->flags = VOLDESC_FLG_MOUNTED|VOLDESC_FLG_FAT;
      logd_printf(logd,LOGD_INFO,"Mount done\n");
      registerDev(volume,volume->mp,logd);
      break;
#endif /* FATFS */
#ifdef EFFS
    case HCC_EFFS_MOUNT:
      msg->id = HCC_EFFS_MOUNT_REPLY;
      msg->effs_mount.error = 0;

      volume = (voldesc_t *)sc_msgAllocClr(sizeof(voldesc_t),
					   HCC_FS_MOUNTP,
					   SC_DEFAULT_POOL,
					   SC_FATAL_IF_TMO);
      
      volume->drive = msg->effs_mount.drive;
      memcpy(volume->mp,msg->effs_mount.mp,31);

      ret = fsm_mountdrive(volume->drive,
			   msg->effs_mount.buffer,
			   msg->effs_mount.bufsize,
			   (FS_DRVMOUNT)msg->effs_mount.mountFunc,
			   (FS_PHYGETID)msg->effs_mount.phyFunc);
      
      if (ret && ret != F_ERR_NOTFORMATTED ) {	
	logd_printf(logd,LOGD_INFO,"Mount failed: %d\n",ret);
	ret = EFAULT;
      } else if ( ret == F_ERR_NOTFORMATTED ){
	logd_printf(logd,LOGD_INFO,"Drive not formated\n");
	volume->flags = VOLDESC_FLG_NOTFORMATED;
      } else {
	logd_printf(logd,LOGD_INFO,"Mount done\n");
      }
      msg->effs_mount.error = errMap[ret];

      if ( !ret || ret == F_ERR_NOTFORMATTED ) {
	volume->flags |= VOLDESC_FLG_MOUNTED|VOLDESC_FLG_EFFS;
	registerDev(volume,volume->mp,logd);
      } else {
	sc_msgFree((sc_msgptr_t)&volume);
      }
      break;
#endif /* EFFS */

/*
****************************************
** SDD object messages
****************************************
*/
    case SDD_OBJ_DUP:
      msg->id = SDD_OBJ_DUP_REPLY;
      msg->base.error = 0;
      break;
    case SDD_OBJ_RELEASE:
      msg->id = SDD_OBJ_RELEASE_REPLY;      
      file = (scfile_t NEARPTR)msg->base.handle;

      if ( file && file->id == HCC_FS_DESC) {
	sc_msgFree((sc_msgptr_t)&file);
      }
      break;

    case SDD_OBJ_SIZE_GET:
      msg->id = SDD_OBJ_SIZE_GET_REPLY;
      obj = &msg->obj;
      obj->base.error = 0;
      file = (scfile_t NEARPTR)obj->base.handle;
      if ( file && file->id == HCC_FS_MOUNTP ){	
	if (!f_getfreespace (((voldesc_t *)file)->drive, &space)) {
	  msg->size.total = space.total;
	  msg->size.free = space.free;
	  msg->size.bad = space.bad;
	} else {
	  msg->size.base.error = EIO;
	}
	break;
      }
      
      if ( !file ){
	obj->base.error = EBADF;
	break;
      }
      
      volume = file->volume;
      /* file */
      if ( file->fd ){
	/* file is open, so find end by seeking */
	cur = f_tell(file->fd);
	f_seek(file->fd,0,F_SEEK_END);
	msg->size.total = f_tell(file->fd);
	f_seek(file->fd,cur,F_SEEK_SET);
      } else {
	f_chdrive(volume->drive);
	
	fs_fullpath(path,
		    file->find.findfsname.path,
		    file->find.filename);	
	
	msg->size.total = f_filelength (path);
      }
      msg->size.free = 0;
      msg->size.bad = 0;

      break;

    case SDD_OBJ_TIME_GET:
      file = (scfile_t NEARPTR)msg->time.base.handle;
      msg->time.data = 0;

      if (file && file->id == HCC_FS_DESC ){
	fs_fullpath(path,
		    file->find.findfsname.path,
		    file->find.filename);	
	
	msg->time.base.error =
	  errMap[f_gettimedate (path, &time, &day)];
	if ( !msg->time.base.error ){
	  msg->time.data = (__u32)day << 16;
	  msg->time.data |= time;
	}
      }
      msg->id = SDD_OBJ_TIME_GET_REPLY;
      sc_msgTx (&msg, to, 0);
      break;

    case SDD_OBJ_TIME_SET:
      file = (scfile_t NEARPTR)msg->time.base.handle;
      if (file && file->id == HCC_FS_DESC ){
	fs_fullpath(path,
		    file->find.findfsname.path,
		    file->find.filename);	

        day = (msg->time.data & 0xffff0000) >> 16;
        time = (msg->time.data & 0x0000ffff);
        msg->time.base.error =
          errMap[f_settimedate (path, time, day)];
      }
      msg->id = SDD_OBJ_TIME_SET_REPLY;
      sc_msgTx (&msg, to, 0);
      break;
/*
****************************************
** SDD device/file messages
****************************************
*/
    case SDD_DEV_OPEN:
      msg->id = SDD_DEV_OPEN_REPLY;

      file = (scfile_t NEARPTR)msg->open.base.handle;
      msg->open.base.error = 0;
      
      if (!file || file->id == HCC_FS_MOUNTP ){
	/* no direct volume access currently */
        msg->open.base.error = EBADF;
	break;	
      }
      

      /* Open file with different flags is an error ! */
      if ( file->fd && (file->flags != msg->open.flags) ){
	msg->open.base.error = EACCES;
	break;
      }
	
      if ( file->fd ){
	file->ref++;
	break;
      }

      volume = file->volume;

      fs_fullpath(path,
		  file->find.findfsname.path,
		  file->find.filename);

      f_chdrive(volume->drive);

      ++volume->ref;

      file->flags = msg->open.flags;

      accmode = file->flags & O_ACCMODE;
      /* truncate only if we are not in read only mode */
      if ( accmode != O_RDONLY && (file->flags & O_TRUNC)) {
	fd = f_open (path, "w");
	if ( fd ) {
	  f_close ( fd );
	}
      }
      	
      /* this order depends on the bitmap definition of O_RDONLY,
	 O_WRONLY and O_WRONLY which can be only used exclusivly! */
      if ( accmode == O_RDONLY) {
	file->fd = f_open (path, "r");
      } else if ( accmode == O_WRONLY) {
	file->fd = f_open (path, "a");
	
	/* If not append, move filepoint to start of file */
	if ((file->flags & O_APPEND)) {
	  f_seek (file->fd, 0, F_SEEK_END);
	}
      } else /* O_RDWR */ {	
	if (file->flags & O_APPEND) {
	  file->fd = f_open (path, "a+");
	} else {
	  file->fd = f_open (path, "r+");
	}
      }

      if ( file->fd == NULL) {
	msg->open.base.error = EACCES;
      } else {
	file->ref++;
      }

//->      logd_printf(logd,LOGD_INFO,"open: %s %d\n",path,file->fd != 0);
      break;

    case SDD_DEV_CLOSE:
      file = (scfile_t NEARPTR)msg->close.base.handle;
      volume = file->volume;
      msg->id = SDD_DEV_CLOSE_REPLY;
      if (file && file->id == HCC_FS_DESC ){
	fs_fullpath(path,
		    file->find.findfsname.path,
		    file->find.filename);
//->      logd_printf(logd,LOGD_INFO,"close: %s\n",path);
	
	file->ref--;
	if ( file->ref == 0 ){
	  f_close(file->fd);
	  file->fd = NULL;
	  file->flags = 0;
	  --volume->ref;
	} else if ( file->ref < 0 ){
	  sc_miscError ((SFS_ERR_BASE + SC_EBADBAD)|SC_ERR_FATAL, file->ref);
	}
      }
      break;

    case SDD_DEV_READ:
      file = (scfile_t NEARPTR)msg->read.base.handle;
      msg->read.base.error = 0;
      msg->id = SDD_DEV_READ_REPLY;

      if (!file || file->id == HCC_FS_MOUNTP ){
	/* no direct reading currently */
        msg->read.base.error = EBADF;
	break;	
      }

      accmode = file->flags & O_ACCMODE;

      fs_fullpath(path,
		  file->find.findfsname.path,
		  file->find.filename);
//->      logd_printf(logd,LOGD_INFO,"read(%s,%d)\n",path,msg->read.size);

      if ( accmode == O_WRONLY ){
        msg->read.base.error = EPERM;
	break;
      }

      if (msg->read.outlineBuf) {
	msg->read.curpos =
	  f_read (msg->read.outlineBuf, 1, msg->read.size, file->fd);
      } else {
	msg->read.curpos =
	  f_read (msg->read.inlineBuf, 1, msg->read.size, file->fd);
      }
      msg->read.size = msg->read.curpos;
      break;

    case SDD_DEV_WRITE:
      file = (scfile_t NEARPTR)msg->write.base.handle;
      msg->read.base.error = 0;
      msg->id = SDD_DEV_WRITE_REPLY;

      if (!file || file->id == HCC_FS_MOUNTP ){
	/* no direct writing currently */
        msg->write.base.error = EBADF;
	break;	
      }

      accmode = file->flags & O_ACCMODE;
      fs_fullpath(path,
		  file->find.findfsname.path,
		  file->find.filename);
//->      logd_printf(logd,LOGD_INFO,"write(%s,%d)\n",path,msg->write.size);

      if ( accmode == O_RDONLY ){
        msg->write.base.error = EPERM;
	break;
      }

      if (msg->write.outlineBuf) {
	msg->write.curpos =
	  f_write (msg->write.outlineBuf, 1, msg->write.size, file->fd);
      }
      else {
	msg->write.curpos =
	  f_write (msg->write.inlineBuf, 1, msg->write.size, file->fd);
      }
      if (msg->write.size && !msg->write.curpos) {
	msg->write.base.error = ENOSPC;
      }
      msg->write.size = msg->write.curpos;
      break;
/*
** file message implementation
*/
    case SDD_FILE_SEEK:
      msg->id = SDD_FILE_SEEK_REPLY;      
      file = (scfile_t NEARPTR)msg->seek.base.handle;
      if (!file || file->id == HCC_FS_MOUNTP || !file->fd){
	/* no direct seeking */
        msg->seek.base.error = EBADF;
	break;
      }

	switch( msg->seek.whence ){
	case SEEK_SET:
	  msg->seek.base.error =
	    errMap[f_seek (file->fd, msg->seek.offset, Fx_SEEK_SET)];
	  break;
	case SEEK_CUR:
	  msg->seek.base.error =
	    errMap[f_seek (file->fd, msg->seek.offset, Fx_SEEK_CUR)];
	  break;
	case SEEK_END:
	  msg->seek.base.error =
	    errMap[f_seek (file->fd, msg->seek.offset, Fx_SEEK_END)];
	  break;
	default:
          msg->seek.base.error = EINVAL;
        }

        if (!msg->seek.base.error) {
          msg->seek.offset = f_tell (file->fd);
        }
      break;

    case SDD_FILE_RESIZE:
      msg->id = SDD_FILE_RESIZE_REPLY;
      file = (scfile_t NEARPTR)msg->resize.base.handle;

      if (!file || file->id == HCC_FS_MOUNTP ){
	/* No volume resizing */
        msg->resize.base.error = EBADF;
	break;
      }
	fs_fullpath(path,
		    file->find.findfsname.path,
		    file->find.filename);
//->	logd_printf(logd,LOGD_INFO,"resize(%s,%d)\n",path,msg->resize.size);

	fd = f_truncate (path, msg->resize.size);
	if ( fd ){
	  f_close(fd);
	} else {
          msg->resize.base.error = ENOENT;
	}
      break;
/*
****************************************
** SDD manager messages
****************************************
*/
      /*
      ** create a new file or directory
      */
    case SDD_MAN_ADD:
      obj = &msg->obj;
//->      logd_printf(logd, LOGD_INFO, "manAdd: %s\n",obj->name);

      msg->id = SDD_MAN_ADD_REPLY;

      obj->base.error = 0;
      
      directory = (scfile_t NEARPTR)obj->manager;

      if ( directory->id == HCC_FS_DESC ){
	fs_fullpath(path, directory->find.findfsname.path, obj->name);
	volume = directory->volume;
      } else {
	fs_fullpath(path, "", obj->name);
	volume = (voldesc_t *)directory;
      }

      f_chdrive(volume->drive);

//->      logd_printf(logd,LOGD_INFO,"add: %s\n",path);

      if ( !strcmp(obj->name,"..") || !strcmp(obj->name,".") ){
	/*
	** . and .. should be handled in an upper layer.
	*/
	obj->base.error = EINVAL;
      } else if( obj->type == SFS_ATTR_FILE ){
	fd = f_open(path,"w+");	
	if ( !fd ){
	  obj->base.error = EEXIST;
	} else {
	  f_close (fd);
	  fs_getcurrenttimedate(&time,&day);
	  f_settimedate (path, time, day);
	}
      } else if (f_mkdir (path)) {
	obj->base.error = EEXIST;
      }
      break;
    case SDD_MAN_RM:
      obj = &msg->obj;
      msg->id = SDD_MAN_RM_REPLY;

      directory = (scfile_t NEARPTR)obj->manager;

      if ( directory->id == HCC_FS_DESC ){
	fs_fullpath(path, directory->find.findfsname.path, obj->name);
	volume = directory->volume;
      } else {
	fs_fullpath(path, "", obj->name);
	volume = (voldesc_t *)directory;
      }
      f_chdrive(volume->drive);

//->      logd_printf(logd,LOGD_INFO,"rm: %s\n",path);

      if ( !strcmp(obj->name,"..") || !strcmp(obj->name,".") ){
	/*
	** . and .. should be handled in an upper layer.
	*/
	obj->base.error = EINVAL;
      } else if( obj->type == SFS_ATTR_FILE ){
	if ( f_delete(path) ){
	  obj->base.error = EBUSY;
	}
      } else if (f_rmdir (path)) {
	obj->base.error = EBUSY;
      }
      break;

    case SDD_MAN_GET:
      obj = &msg->obj;
      msg->id = SDD_MAN_GET_REPLY;
//->      logd_printf(logd, LOGD_INFO, "manGet: %s\n",obj->name);

      obj->sender =
	obj->controller =
	obj->receiver = _this;
      obj->base.error = 0;
      obj->base.handle = NULL;

      /* get parent of this request */
      directory = (scfile_t *)obj->manager;

      if ( directory->id == HCC_FS_DESC ){
	fs_fullpath(path, directory->find.findfsname.path, obj->name);
	volume = directory->volume;
      } else {
	fs_fullpath(path, "", obj->name);
	volume = (voldesc_t *)directory;
      }

      f_chdrive(volume->drive);

//->      logd_printf(logd,LOGD_INFO,"get: %s\n",path);

      if ( !strcmp(obj->name,"..") || !strcmp(obj->name,".") ){
	/*
	** . and .. should be handled in an upper layer.
	*/
	obj->base.error = EINVAL;
      }else 
	if ( f_findfirst(path,&find) ){
	obj->base.error = ENOENT;
      } else {
	/*
	** Allocate a handle
	*/
	refFile = (scfile_t NEARPTR)sc_msgAllocClr(sizeof(scfile_t),
					 HCC_FS_DESC,
					 SC_DEFAULT_POOL,
					 SC_FATAL_IF_TMO);	
	memcpy((char *)&refFile->find,(char *)&find,sizeof(Fx_FIND));
	strncpy(obj->name,find.filename,SC_NAME_MAX);
	obj->name[SC_NAME_MAX] = 0;
	refFile->volume = volume;

	if ( find.attr == Fx_ATTR_DIR ){
	  /* f_findfirst returns the found entry in lname
	  ** we need it as path, so we move it.
	  */
	  obj->type = SFS_ATTR_DIR;
	  fs_strcat(refFile->find.findfsname.path,"/");
	  fs_strcat(refFile->find.findfsname.path,refFile->find.filename);
	  refFile->find.filename[0] = 0;
	} else {
	  obj->type =  SFS_ATTR_FILE;
	}
	obj->base.handle = refFile;
      }
      break;

    case SFS_FFIRST:
      // get first with mask
    case SDD_MAN_GET_FIRST:
      ++msg->id;

      obj = &msg->obj;
      obj->sender =
	obj->controller =
	obj->receiver = _this;
      obj->base.error = 0;
      obj->base.handle = NULL;

      /* get parent of this request */
      directory = (scfile_t NEARPTR)obj->manager;

      if ( !directory ){
	obj->base.error = ENOENT;
	break;
      }
      if ( directory->id == HCC_FS_DESC ){
	fs_fullpath(path, directory->find.findfsname.path, 
		    obj->name[0] ? obj->name : "*.*");

	volume = directory->volume;
      } else {
	fs_fullpath(path, "", obj->name[0] ? obj->name : "*.*");
	volume = (voldesc_t *)obj->manager;
      }

      f_chdrive(volume->drive);

//->      logd_printf(logd,LOGD_INFO,"get first: %s\n",path);

      if ( !strcmp(obj->name,"..") || !strcmp(obj->name,".") ){
	/*
	** . and .. should be handled in an upper layer.
	*/
	obj->base.error = EINVAL;
      }else if ( f_findfirst(path,&find) ){
	obj->base.error = ENOENT;
      } else {
	/*
	** Allocate a handle
	*/
//->	logd_printf(logd,LOGD_INFO,"%s\n",find.filename);

	refFile = (scfile_t NEARPTR)sc_msgAllocClr(sizeof(scfile_t),
						   SDD_OBJ,
						   SC_DEFAULT_POOL,
						   SC_FATAL_IF_TMO);	
	memcpy((char *)&refFile->find,(char *)&find,sizeof(Fx_FIND));
	strncpy(obj->name,find.filename,SC_NAME_MAX);
	refFile->volume = volume;
	obj->type = find.attr == F_ATTR_DIR ? SFS_ATTR_DIR : SFS_ATTR_FILE;
	obj->base.handle = refFile;
      }
      break;

    case SDD_MAN_GET_NEXT:
      msg->id = SDD_MAN_GET_NEXT_REPLY;
      obj = &msg->obj;

      /* get parent of this request */
      directory = (scfile_t NEARPTR)obj->manager;

      /* get referenced object */
      refFile = (scfile_t NEARPTR)obj->base.handle;

      if (!refFile || refFile->id == HCC_FS_MOUNTP ){
	obj->base.error = EBADF;
	break;
      }

      /* we need the find structure only */
      memcpy((char *)&find,(char *)&refFile->find,sizeof(Fx_FIND));

      /*
      ** setup object
      */
      obj->sender =
	obj->controller =
	obj->receiver = _this;
      obj->base.error = 0;
      obj->base.handle = NULL;

      volume = directory->volume;
      f_chdrive(volume->drive);

      if ( f_findnext(&find) ){
	obj->base.error = ENOENT;
      } else {
	/*
	** Found an dir-entry, build a new handle.
	*/
//->	logd_printf(logd,LOGD_INFO,"found: %s\n",refFile->find.filename);

	file = (scfile_t NEARPTR)sc_msgAllocClr(sizeof(scfile_t),
						HCC_FS_DESC,
						SC_DEFAULT_POOL,
						SC_FATAL_IF_TMO);	
	memcpy((char *)&file->find,(char *)&find,sizeof(Fx_FIND));
	strncpy(obj->name,find.filename,SC_NAME_MAX);
	file->volume = volume;
	obj->type = find.attr == Fx_ATTR_DIR ? SFS_ATTR_DIR : SFS_ATTR_FILE;
	obj->base.handle = file;
      }
      break;
/*
****************************************
** SFS messages
****************************************
*/
    case SFS_SYNC:
      obj = &msg->obj;
      msg->id = SFS_SYNC_REPLY;
      file = (scfile_t NEARPTR)msg->sync.base.handle;
      if (file && file->id == HCC_FS_DESC ){
       ret = f_flush(file->fd);
       obj->base.error = errMap[ret];
      } else {
	// XXX: Maybe should flush all open files.
	obj->base.error = EBADF;
      }
      break;
/*
****************************************
** unknown message
****************************************
*/
    default:
      logd_printf(logd,LOGD_SEVERE,"Got %08x\n",msg->id);
      break;
    }

    if (msg) {
      sc_msgTx (&msg, to, 0);
    }
  }
}
