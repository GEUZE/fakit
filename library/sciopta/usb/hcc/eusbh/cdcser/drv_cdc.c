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
**              (c) 2008 SCIOPTA Systems AG/ Schweiz                **
**                                                                  **
**********************************************************************
** ID: S08318BS1                                                    **
** +Revision: 1.1.2.5 +                                                 **
** +Date: 2009/05/22 08:57:33 +                                     **
** CDC seriel driver                                                **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#include <logd/logd.h>
#include <ossys/libsc.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/termios.h>

#include <hcc/hcc_types.h>
#include <hcc/eusbh/usb_driver/usb_host.h>
#include <hcc/eusbh/usb_driver/usb_utils.h>
#include <hcc/eusbh/usb_driver/pmgr/pmgr.h>
#include <hcc/eusbh/usbh.msg>
#include <hcc/eusbh/registerDev.h>
#include <hcc/eusbh/cdcser/cdcser.h>

union sc_msg {
  sc_msgid_t id;
  sdd_obj_t object;
  sdd_baseMessage_t base;
  sdd_devOpen_t open;
  sdd_devRead_t read;
  sdd_devWrite_t write;
  sdd_devIoctl_t ioctl;
  sdd_fileSeek_t seek;
  sdd_fileResize_t resize;
  sc_procNameGetMsgReply_t nameget;

  hcc_usbhConnect_t    connect;
  hcc_usbhDisconnect_t disconnect;
};


#define RXBUF_SIZE 128

SC_PROCESS(SCP_cdcserWorker)
{  
  sc_msg_t msg;
  char name[32];
  logd_t *logd = NULL;
  sc_pid_t from;
  __u32 len;

  t_cdcser *ci;
  sc_msg_t ciMsg;
  sc_msg_t rx_msg;
  int ref = 0;

  unsigned int rxmax;
  unsigned int rxcurr;

  static const sc_msgid_t sel[10] = {
    SDD_DEV_OPEN,
    SDD_OBJ_DUP,
    SDD_DEV_CLOSE,
    SDD_DEV_WRITE,
    SDD_DEV_READ,
    SDD_DEV_READ_REPLY,
    SDD_OBJ_RELEASE,
    HCC_USBH_CONNECT,    
    HCC_USBH_DISCONNECT,    
    0
  };

  static const sc_msgid_t sel_no_rx[9] = {
    SDD_DEV_OPEN,
    SDD_OBJ_DUP,
    SDD_DEV_CLOSE,
    SDD_DEV_WRITE,
    SDD_DEV_READ_REPLY,
    SDD_OBJ_RELEASE,
    HCC_USBH_CONNECT,    
    HCC_USBH_DISCONNECT,    
    0
  };
  
  rxmax = 0;
  rxcurr = 0;
  rx_msg = NULL;

  ci = NULL;

  for(;;){
    if ( rx_msg ){
      msg = sc_msgRx(SC_ENDLESS_TMO, sel_no_rx, SC_MSGRX_MSGID);
    } else {
      msg = sc_msgRx(SC_ENDLESS_TMO, sel, SC_MSGRX_MSGID);
    }
    from = sc_msgSndGet(&msg);
    switch( msg->id ){
    case SDD_OBJ_DUP:
    case SDD_DEV_OPEN:
      msg->base.error = 0;
      if ( ci ){
	++ref;
      } else {
	msg->base.error = EIO;
      }
      ++msg->id;
      sc_msgTx(&msg, from, 0);
      break;
    case SDD_DEV_CLOSE:
      msg->base.error = 0;
      if ( ref ){
	--ref;
      }
      ++msg->id;
      sc_msgTx(&msg, from, 0);
      break;
    case SDD_DEV_WRITE:
      msg->base.error = 0;
      if (!msg->write.outlineBuf) {
	msg->write.outlineBuf = msg->write.inlineBuf;
      }      
      if ( cdcser_send (ci->device,
			(hcc_u8 *)msg->write.outlineBuf, 
			msg->write.size) != 0)
      {
	msg->base.error = EIO;
      } 
      ++msg->id;
      sc_msgTx(&msg, from, 0);
      break;
    case SDD_DEV_READ_REPLY:
      sc_msgFree(&msg);
      if ( !rx_msg ){	
	break;
      }
      msg = rx_msg;
      rx_msg = NULL;
      // fall thru
    case SDD_DEV_READ:
      if ( msg->id == SDD_DEV_READ ){
	msg->base.error = 0;
	if (!msg->read.outlineBuf) {
	  msg->read.outlineBuf = msg->read.inlineBuf;
	} 
	rxmax = msg->read.size;
	rxcurr = 0;
      }

      if ( cdcser_receive (ci->device,
			   (hcc_u8 *)&msg->read.outlineBuf[rxcurr], 
			   rxmax,
			   &len) != 0)
      {
	msg->base.error = EIO;
	msg->read.size = rxcurr;
      } else {
	rxmax -= len;
	rxcurr += len;
      }
      msg->id = SDD_DEV_READ_REPLY;
      if ( rxmax == 0 ){
	sc_msgTx(&msg, sc_msgSndGet(&msg), 0);
      } else {
	rx_msg = msg;
      }
      break;           
    case SDD_OBJ_RELEASE:
      ++msg->id;
      msg->base.error = 0;
      sc_msgTx(&msg, from, 0);
      break;

    case HCC_USBH_CONNECT:
      ciMsg = msg;
      ci = (t_cdcser *)msg->connect.hdl;
      
      strcpy(name,"cdcser");
      sc_miscInt2Str(ci->device,name+6,2);      

      if ( !logd ){
	logd = logd_new ("/SCP_logd", 
			 LOGD_INFO, name, 
			 SC_DEFAULT_POOL,
			 SC_FATAL_IF_TMO);
	
	logd_printf(logd,LOGD_INFO,"started\n");
      }
      
      registerDev(name,logd, sc_procIdGet(NULL,SC_NO_TMO), ci);
      
      cdcser_set_line_coding(ci->device,115200,
			     CDCSER_BITS_8,CDCSER_PARITY_NONE,CDCSER_STOP_1);

      cdcser_set_events(ci->device,(OS_EVENT_BIT_TYPE *)1,0);
      break;
    case HCC_USBH_DISCONNECT:
      strcpy(name,"cdcser");
      sc_miscInt2Str(ci->device,name+6,2);
      unregisterDev(name,logd);
      sc_msgFree(&msg);
      sc_msgFree((sc_msgptr_t)&ciMsg);
      break;
    }
  }
}

SC_PROCESS(SCP_cdcser)
{
  sc_msg_t msg;
  int error;
  logd_t *logd;
  sdd_obj_t *root;
  t_cdcser *ci;
  int i;
  char name[32];

  sc_pid_t worker[CDCSER_MAX_DEVICE];

  /*
  ** Init worker array and check for static workers.
  */
  for(i = 0 ; i < CDCSER_MAX_DEVICE; ++i){
    strcpy(name,"SCP_cdcser");
    sc_miscInt2Str(i,name+10,2);      
    worker[i] = sc_procIdGet(name,sc_tickMs2Tick(3));
    worker[i] = SC_ILLEGAL_PID;
  }

  logd = logd_new ("/SCP_logd", 
		   LOGD_INFO, "cdcser", 
		   SC_DEFAULT_POOL,
		   SC_FATAL_IF_TMO);
    /* First get root manager */
  root = sdd_manGetRoot ("/SCP_devman", "/", SC_DEFAULT_POOL, SC_ENDLESS_TMO);
  if ( !root ){
    logd_printf(logd, LOGD_SEVERE, "Error could not get root-manager\n");
    sc_sleep(10);
    sc_miscError(SC_ERR_SYSTEM_FATAL,1);
    sc_procKill(SC_CURRENT_PID,0);
  }
  
  /* 
  ** Setup CDCSER
  */
  logd_printf(logd, LOGD_INFO, "Waiting for USBH_pmgr\n");

  /* wait for USBH_pmgr to appear */
  error = sdd_manNotifyAdd(root, "USBH_pmgr", SC_ENDLESS_TMO);

  if ( error < 0 ){
    logd_printf(logd, LOGD_SEVERE, "Error could not get USBH_pmgr\n");
    sc_miscError(SC_ERR_SYSTEM_FATAL,2);
  } 
  logd_printf(logd, LOGD_INFO, 
	      "USBH_pmgr found ... setting up CDCSER\n");
  
  error = cdcser_init ();

  if ( error ){
    logd_printf(logd, LOGD_SEVERE,"Error setting up CDCSER: %d\n",error);
    sc_sleep(10);
    sc_miscError(SC_ERR_SYSTEM_FATAL,2);
  }

  for(;;){
    static const sc_msgid_t sel[3] = {
      HCC_USBH_CONNECT,
      HCC_USBH_DISCONNECT,
      0
    };

    msg = sc_msgRx(SC_ENDLESS_TMO, sel, SC_MSGRX_MSGID);
    switch( msg->id ){
    case HCC_USBH_CONNECT:
      ci = (t_cdcser *)msg->connect.hdl;
      strcpy(name,"SCP_cdcser");
      sc_miscInt2Str(ci->device,name+10,2);      
      if ( worker[ci->device] == SC_ILLEGAL_PID ){
	worker[ci->device] = sc_procPrioCreate(name,
					       SCP_cdcserWorker,
					       1024,
					       0,
					       0,//sc_procPrioGet(SC_CURRENT_PID),
					       1,
					       SC_DEFAULT_POOL);
      }
      sc_msgTx(&msg,worker[ci->device],0);
      break;
    case HCC_USBH_DISCONNECT:
      ci = (t_cdcser *)msg->disconnect.hdl;
      if ( worker[ci->device] != SC_ILLEGAL_PID ){
	sc_msgTx(&msg, worker[ci->device], 0 );
      } else {
	sc_msgFree(&msg);      
      }
      break;
    }

  }
}

