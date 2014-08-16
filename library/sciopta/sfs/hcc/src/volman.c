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
**              (c) 2009 SCIOPTA Systems AG/ Schweiz                **
**                                                                  **
**********************************************************************
** ID: S08314BS1                                                    **
** +Revision: 1.1.2.4 +                                                 **
** +Date: 2009/08/24 12:51:47 +                                     **
** Volume manager                                                   **
**********************************************************************
EOC*/

/*
** Volume manager:
** This process initializes HCC-FATFS and registers all possible devices with 
** following order:
** - LUN-0-LUN-n of all USB-MSC devices
** - LUN-0 of all SD-cards
** - LUN-0 of all CF-cards
**
** It registers drives as soon as they appear and unregisters those if they
** are detached.
*/

#include <sciopta.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#include <logd/logd.h>
#include <ossys/libsc.h>
#include <sys/errno.h>
#include <sfs/sfs.h>
#include "fatfs_cfg.h"
#include <hcc/hcc_types.h>
#include <hcc/fatfs/fat.h>

/* USB-stick support */
#if SCSI_MAX_DEVICE > 0
#include <hcc/eusbh/mst/usb_mst.h>
#include "./mst/mst.h"
#include <hcc/eusbh/usbh.msg>

typedef struct lunInfo_s {
  __u8 used;
  __u8 device;
  __u8 lun;
  __u8 inserted;
  sc_tmoid_t checkTmo;
} lunInfo_t;

#endif /* SCSI_MAX_DEVICE */

#if SD_MAX_DEVICE > 0
#include "mmc.h"

#if SD_MAX_DEVICE > 32
#error Only 32 SD cards supported !
#endif

#endif /* SD_MAX_DEVICE */


/*
** Message union of accepted messages
*/
union sc_msg {
  sc_msgid_t        id;
  sdd_baseMessage_t base;
  sdd_obj_t         obj;
  hcc_fatfs_mount_t mount;
#if SCSI_MAX_DEVICE > 0
  hcc_usbhConnect_t    connect;
  hcc_usbhDisconnect_t disconnect;
  hcc_usbhLunPollMsg_t lunPoll;
#endif
};
/*
** handy functions to (un)register a device
*/
/*
***************************************
** 
** Register a file-device with devman/fatfs
**
*****************************************
*/
static void registerDev (sdd_obj_t *root,     /* where to register            */
			 const char *name,    /* base name: sd,mmc,cf         */
			 logd_t NEARPTR logd, /* logd handler                 */
			 sc_pid_t sddFs_pid,  /* sdd_fatfs or SC_ILLEGAL_PID  */
			 unsigned int device, /* counter of sd,mmc or cf      */
			 unsigned int volume) /* total volume id              */
{
  /* registration */
  if ( sddFs_pid != SC_ILLEGAL_PID ){
    static const sc_msgid_t selMount[2] = {
      HCC_FATFS_MOUNT_REPLY,
      0
    };
    sc_msg_t msg;
    /* mount volume */
    msg = sc_msgAlloc(sizeof(hcc_fatfs_mount_t),
		      HCC_FATFS_MOUNT,
		      SC_DEFAULT_POOL,
		      SC_FATAL_IF_TMO);
    msg->mount.dev = NULL;
    msg->mount.drive = volume;
    strcpy(msg->mount.mp,name);    
    sc_miscInt2Str(device,msg->mount.mp+strlen(msg->mount.mp),2);
    sc_msgTx(&msg,sddFs_pid,0);
    msg = sc_msgRx(SC_ENDLESS_TMO,selMount,SC_MSGRX_MSGID);
    sc_msgFree(&msg);
  } else {
    sdd_obj_t NEARPTR dev;  
    /* only register with devman */    
    dev = (sdd_obj_t NEARPTR) sc_msgAllocClr (sizeof (sdd_obj_t), 
					      SDD_OBJ,
					      SC_DEFAULT_POOL, 
					      SC_FATAL_IF_TMO);
    
    dev->type = SDD_OBJ_TYPE;
    strncpy (dev->name, name, SC_NAME_MAX);
    sc_miscInt2Str(device,dev->name+strlen(dev->name),2);
    logd_printf (logd, LOGD_INFO, "Register %s\n", dev->name);
    dev->controller = 
      dev->receiver = 
      dev->sender = sc_procIdGet(NULL,SC_NO_TMO);    
    root->base.handle = (void *)volume; /* !!! hack !!! */
    /* register to devman */
    if (sdd_manAdd (root, &dev)) {
      logd_printf (logd, LOGD_SEVERE, "Could not add %s%d \n",name,device);
    }
  }
}
  
/*
***************************************
** 
** Unregister a device from devman
**
*****************************************
*/
static void unregisterDev(sdd_obj_t *root,    /* where to unregister from     */
			  const char * name,  /* base name: sd,msc or cf      */
			  logd_t *logd,       /* logd handle                  */
			  unsigned int device,/* counter of sd,mmc or cf      */
                          sc_pid_t sddFs_pid) /* sdd_fatfs or SC_ILLEGAL_PID  */
{
  sdd_obj_t NEARPTR dev;
  char mp[32];

  /* build full name, e.g. msc0 */
  strcpy(mp,name);
  sc_miscInt2Str(device,mp+strlen(name),2);

  if ( sddFs_pid != SC_ILLEGAL_PID ){
    /* unmount volume */
    dev = sdd_manGetByName(root,mp);
    if ( !dev ){
      logd_printf (logd, LOGD_SEVERE, "Could not remove %s\n",mp);
    }
    sfs_umount(dev);
  } else {
    /* unregister from devman */
    logd_printf (logd, LOGD_INFO, "Unregister %s\n", mp);
    
    dev = (sdd_obj_t NEARPTR) sc_msgAllocClr(sizeof (sdd_obj_t), 
					     SDD_OBJ,
					     SC_DEFAULT_POOL, 
					     SC_FATAL_IF_TMO);
    dev->controller = sc_procIdGet(NULL,SC_NO_TMO);
    strncpy (dev->name, mp, SC_NAME_MAX);
    
    /* unregister from dev man */
    if (sdd_manRm (root, dev, sizeof(sdd_obj_t))) {
      logd_printf (logd, LOGD_SEVERE, "Could not remove %s\n",mp);
    }
  }
  sdd_objFree(&dev);
}


int scsi_getstatus(unsigned long driver_param);

SC_PROCESS(SCP_volman)
{
#if SCSI_MAX_DEVICE > 0
  t_mst_info *mi;
  lunInfo_t lunId[SCSI_MAX_DEVICE*SCSI_MAX_LUN];
#endif
#if SD_MAX_DEVICE > 0
  __u32 sdSet = 0;
#endif
#if CF_MAX_DEVICE > 0
  int nCfDevice = 0;
#endif
  int i;
  sc_msg_t msg;
  int error;
  logd_t *logd;
  sdd_obj_t *root;
  sdd_obj_t *obj;

  sc_pid_t self_pid;
  sc_pid_t sddFs_pid;
  sc_poolid_t pl;
  char name[32];
  unsigned int volume;

  pl = sc_poolIdGet("fs_pool");
  if ( pl != SC_ILLEGAL_POOLID ){
    sc_poolDefault(pl);
  }

  self_pid = sc_procIdGet(NULL,SC_NO_TMO);

  logd = logd_new ("/SCP_logd", 
		   LOGD_INFO, "volman", 
		   SC_DEFAULT_POOL,
		   SC_FATAL_IF_TMO);
  
  
  /* 
  ** Initialize FATFS
  */
  f_init();

  logd_printf(logd, LOGD_INFO, "FATFS setup : %s\n",f_getversion());

  /* First get root manager */
  root = sdd_manGetRoot ("/SCP_devman", "/", SC_DEFAULT_POOL, SC_ENDLESS_TMO);
  if ( !root ){
    logd_printf(logd, LOGD_SEVERE, "Error could not get root-manager\n");
    sc_sleep(10);
    sc_miscError(SC_ERR_SYSTEM_FATAL,1);
    sc_procKill(SC_CURRENT_PID,0);
  }

  /* now register volan */
  logd_printf (logd, LOGD_INFO, "Register with devman\n");
  
  obj = (sdd_obj_t NEARPTR) sc_msgAlloc (sizeof (sdd_obj_t), 0,
					 SC_DEFAULT_POOL, SC_FATAL_IF_TMO);
  
  obj->base.error = 0;
  obj->base.handle = NULL;
  obj->type = SDD_OBJ_TYPE | SDD_MAN_TYPE;
  strcpy (obj->name, "volman");
  obj->controller = 
    obj->receiver = 
    obj->sender = self_pid;
  
  /* register to obj man */
  if (sdd_manAdd (root, &obj)) {
    logd_printf (logd, LOGD_SEVERE, "Could not add volman \n");
    sc_procKill (SC_CURRENT_PID, 0);
  }

#if SCSI_MAX_DEVICE > 0
  /* 
  ** Setup MSC
  */
  logd_printf(logd, LOGD_INFO, "Waiting for USBH_pmgr\n");

  /* wait for USBH_pmgr to appear */
  error = sdd_manNotifyAdd(root, "USBH_pmgr", SC_ENDLESS_TMO);

  if ( error < 0 ){
    logd_printf(logd, LOGD_SEVERE, "Error could not get USBH_pmgr\n");
    sc_miscError(SC_ERR_SYSTEM_FATAL,2);
  } 
  logd_printf(logd, LOGD_INFO, 
	      "USBH_pmgr found.\n"
	      "Setting up MSC\n");
  
  /*
  ** Init MSC and register this process as sink for connect/disconnect
  ** messages
  */
  error = usb_mst_init ();
  
  if ( error ){
    logd_printf(logd, LOGD_SEVERE,"Error setting up MSC\n");
    sc_sleep(10);
    sc_miscError(SC_ERR_SYSTEM_FATAL,2);
  }
  memset((char *)lunId,0,sizeof(lunId));
#endif /* SCSI_MAX_DEVICE > 0 */

  /*
  ** check if sc2hcc wrapper is available
  */
  logd_printf(logd, LOGD_INFO, "sdd_fatfs present ?\n");
  {
    sdd_obj_t *fatfs;
    error = sdd_manNotifyAdd(root, "sdd_fatfs", sc_tickMs2Tick(20));
    if ( error ){
      logd_printf(logd, LOGD_INFO, "No sdd_fats => no SDD mode available\n");
      sddFs_pid = SC_ILLEGAL_PID;
    } else {
      fatfs = sdd_manGetByName(root,"sdd_fatfs");
      if ( fatfs ){
	sddFs_pid = fatfs->controller;
      } else {
	sddFs_pid = SC_ILLEGAL_PID;
      }
      sdd_objFree(&fatfs);
    }
  }

  /*
  ** Register volman with FATFS
  */
  f_enterFS();

#if SCSI_MAX_DEVICE > 0
  /*
  ** Now setup all USB-drives (even though not yes connected)
  */  
  for(i = 0; i < SCSI_MAX_DEVICE*SCSI_MAX_LUN; ++i ){  
    f_initvolume(i,mst_initfunc,i);
  }
#endif
#if SD_MAX_DEVICE > 0
  /*
  ** Now setup all SD-drives (even though not yes connected)
  */  
  for(i = 0; i < SD_MAX_DEVICE; ++i ){
    f_initvolume(SCSI_MAX_DEVICE*SCSI_MAX_LUN+i,mmc_initfunc,i);
  }
#endif
#if CF_MAX_DEVICE > 0
  /*
  ** Now setup all CF-drives (even though not yes connected)
  */  
  for(i = 0; i < CF_MAX_DEVICE; ++i ){
    f_initvolume(SCSI_MAX_DEVICE*SCSI_MAX_LUN+SD_MAX_DEVICE+i,cf_initfunc,i);
  }
#endif

  /*
  ** release FATFS
  */
  f_releaseFS(self_pid);

  /*
  ** Now wait for volume attachment/detachment
  */
  for(;;){
    static const sc_msgid_t sel[] = {
      /* USB message */
#if SCSI_MAX_DEVICE > 0
      HCC_USBH_CONNECT,
      HCC_USBH_DISCONNECT,
      (HCC_USBH_BASE+14),
#endif
      /* Dummy SDD messages if no sc2hcc wrapper is used */
      SDD_MAN_GET,
      SDD_MAN_GET_FIRST,
      SDD_DEV_OPEN,
      SDD_OBJ_DUP,
      /* devman needs this */
      SDD_OBJ_RELEASE,
      /* device handling */
      SDD_MAN_ADD,
      SDD_MAN_RM,
      0
    };

    msg = sc_msgRx(SC_ENDLESS_TMO, sel, SC_MSGRX_MSGID);

    switch( msg->id ){

    case SDD_OBJ_DUP:     
    case SDD_DEV_OPEN:
    case SDD_MAN_GET:
    case SDD_MAN_GET_FIRST:
      ++msg->id;
      msg->base.error = ENOENT;
      sc_msgTx(&msg, sc_msgSndGet(&msg), 0);
      break;

    case SDD_MAN_ADD:
      i = (unsigned int)msg->base.handle;
      volume = SCSI_MAX_DEVICE*SCSI_MAX_LUN+i;
#if SD_MAX_DEVICE > 0
      if ( !strncmp(msg->obj.name,"sdmmc",5) ){
	while ( f_enterFS() != F_NO_ERROR ){
          sc_sleep(2);
	}
        error = f_checkvolume(volume);
        if ( error == F_ERR_NOTFORMATTED ){
           logd_printf(logd, LOGD_INFO, "volume %d will be formated...\n",
                       volume);
           error = f_format(volume,F_FAT32_MEDIA);
           if ( error == F_NO_ERROR ){
             logd_printf(logd, LOGD_INFO, "done");
           } else {
             logd_printf(logd, LOGD_INFO, "failed\n");
           }
        }
	if ( error == F_NO_ERROR ){
	  f_releaseFS(self_pid);
	  if ( (sdSet & (1<<i)) == 0 ){
	    registerDev(root,
			"sd",
			logd,
			sddFs_pid,
			i,
			volume);
	    sdSet |= (1<<i);
	  } 
        } 
      } else 
#endif
      {
	msg->base.error = EBADF;
      }
      ++msg->id;
      sc_msgTx(&msg, sc_msgSndGet(&msg), 0);
      break;
    case SDD_MAN_RM:
      obj = (sdd_obj_t *)msg;
      i = (unsigned int)obj->base.handle;
      volume = SCSI_MAX_DEVICE*SCSI_MAX_LUN+i;
#if SD_MAX_DEVICE > 0
      if ( !strncmp(obj->name,"sdmmc",5) ){
	while ( f_enterFS() != F_NO_ERROR ){
	  sc_sleep(2);
	}
	if ( f_checkvolume(volume) != F_NO_ERROR && 
	     (sdSet & (1<<i)) )
	{
	  f_releaseFS(self_pid);
	  sdSet &= ~(1<<i);
	  unregisterDev(root,"sd",logd,i,sddFs_pid);
	}
      } else 
#endif
      {
	msg->base.error = EBADF;
      }
      ++msg->id;
      sc_msgTx(&msg, sc_msgSndGet(&msg), 0);
      break;

#if SCSI_MAX_DEVICE > 0
    case HCC_USBH_CONNECT:
    {
      int nLun;
      int lun;
      sc_msg_t tmoMsg;
      mi = (t_mst_info *)msg->connect.hdl;      
      nLun = mi->nlun;
      lun = 0;
      logd_printf(logd, LOGD_INFO,"Attached: %d\n",mi->device);
      while ( f_enterFS() != F_NO_ERROR ){
	sc_sleep(2);
      }
      for(i = 0; nLun && i < sizeof(lunId)/sizeof(lunInfo_t); ++i){
	if ( lunId[i].used == 0 ){
	  error = f_checkvolume(i);
	  if ( error == F_ERR_NOTFORMATTED ){
	    logd_printf(logd, LOGD_INFO, "volume %d will be formated...\n",i);
	    error = f_format(i,F_FAT32_MEDIA);
	    if ( error == F_NO_ERROR ){
	      logd_printf(logd, LOGD_INFO, "done");
	    } else {
	      logd_printf(logd, LOGD_INFO, "failed\n");
	    }
	  }
	  if ( error == F_NO_ERROR ){
	    registerDev(root,"msc",logd, sddFs_pid,i,i);
	    lunId[i].inserted = 1;
	  } else if ( error == F_ERR_CARDREMOVED ){
	    logd_printf(logd, LOGD_INFO, "Device %d:Slot %d empty\n",
			mi->device,lun);	    
	  } else {
	    logd_printf(logd, LOGD_INFO,"Error %d: Device %d,%d\n",
			error,mi->device,lun);
	    continue;
	  }
	  lunId[i].used = 1;
	  lunId[i].device = mi->device;
	  lunId[i].lun = lun;	 
	  tmoMsg = sc_msgAlloc(sizeof(hcc_usbhLunPollMsg_t),
			       HCC_USBH_LUN_POLL,
			       SC_DEFAULT_POOL,
			       SC_FATAL_IF_TMO);
	  tmoMsg->lunPoll.volume = i;
	  lunId[i].checkTmo = sc_tmoAdd(sc_tickMs2Tick(1000),&tmoMsg);

	  ++lun;
	  --nLun;
	}
      }
      f_releaseFS(self_pid);
    }
    break;
    case HCC_USBH_LUN_POLL:      
      i = msg->lunPoll.volume;
      lunId[i].checkTmo = NULL;
//->      logd_printf(logd, LOGD_INFO,"Poll %d\n",i);

      error = scsi_getstatus(i);

      if ( !error ){
	if ( lunId[i].inserted == 0){
	  registerDev(root,"msc",logd, sddFs_pid,i,i);
	  logd_printf(logd, LOGD_INFO,"Attached: %d,%d as volume %d\n",
		      lunId[i].device,lunId[i].lun,i);
	  lunId[i].inserted = 1;
	}
	lunId[i].checkTmo = sc_tmoAdd(sc_tickMs2Tick(1000),&msg);
      } else { //if ( error == F_ERR_CARDREMOVED ){
	if ( lunId[i].inserted == 1 ){
	  unregisterDev(root,"msc",logd,i,sddFs_pid);
	  lunId[i].inserted = 0;
	}
	lunId[i].checkTmo = sc_tmoAdd(sc_tickMs2Tick(1000),&msg);
      }
      break;
    case HCC_USBH_DISCONNECT:
    {
      int nLun;
      int lun;
      mi = (t_mst_info *)msg->disconnect.hdl;
      sc_msgFree(&msg);
      nLun = mi->nlun;
      lun = 0;
      logd_printf(logd,LOGD_INFO,"Detached: %d\n",mi->device);
      for(i = 0; nLun && i < sizeof(lunId)/sizeof(__u16); ++i){
	if ( lunId[i].device == mi->device && lunId[i].lun == lun ){
          if ( lunId[i].inserted ){
	    unregisterDev(root,"msc",logd,i,sddFs_pid);
            lunId[i].inserted = 0;
          }
	  lunId[i].used = 0;
	  if ( lunId[i].checkTmo ){
	    msg = sc_tmoRm(&lunId[i].checkTmo);
	    sc_msgFree(&msg);
	  }
	  --nLun;
	  ++lun;
	}
      }
    }
    break;
#endif
    case SDD_OBJ_RELEASE:
      msg->id = SDD_OBJ_RELEASE_REPLY;      
      msg->base.error = 0;
      sc_msgTx(&msg, sc_msgSndGet(&msg), 0);
      break;
    default:
      sc_miscError(SC_ERR_PROCESS_FATAL,msg->id);
    }
    if ( msg ){
      sc_msgFree(&msg);
    }
  }
}
