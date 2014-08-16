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
** ID: S09142BS2                                                    **
** +Revision: 1.1.2.2 +                                                 **
** +Date: 2009/05/22 08:57:33 +                                     **
** MIDI class driver                                                **
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
#include <hcc/eusbh/midi/midi.h>

union sc_msg {
  sc_msgid_t id;
  sdd_obj_t object;
  sdd_baseMessage_t base;
  sdd_devOpen_t open;
  sdd_devClose_t close;

  midi_event_msg_t     midi_event;
  hcc_usbhConnect_t    connect;
  hcc_usbhDisconnect_t disconnect;
};

SC_PROCESS(SCP_midi)
{
  sc_msg_t msg;
  int error;
  logd_t *logd;
  sdd_obj_t *root;
  int md;
  int i;
  char name[32];
  __u32 devices = 0;
  __u32 deviceOpen = 0;
  __u8 openCnt[32];

  sc_pid_t self;

  int nEvent = 0;
  __u8 cin,cout;

  devices = 0;
  deviceOpen = 0;
  memset(openCnt,0,sizeof(openCnt));
  self = sc_procIdGet(NULL,SC_NO_TMO);

  logd = logd_new ("/SCP_logd", 
		   LOGD_INFO, "midi", 
		   SC_DEFAULT_POOL,
		   SC_NO_TMO);
    /* First get root manager */
  root = sdd_manGetRoot ("/SCP_devman", "/", SC_DEFAULT_POOL, SC_ENDLESS_TMO);
  if ( !root ){
    logd_printf(logd, LOGD_SEVERE, "Error could not get root-manager\n");
    sc_sleep(10);
    sc_miscError(SC_ERR_SYSTEM_FATAL,1);
    sc_procKill(SC_CURRENT_PID,0);
  }
  
  /* 
  ** Setup MIDI
  */
  logd_printf(logd, LOGD_INFO, "Waiting for USBH_pmgr\n");

  /* wait for USBH_pmgr to appear */
  error = sdd_manNotifyAdd(root, "USBH_pmgr", SC_ENDLESS_TMO);

  if ( error < 0 ){
    logd_printf(logd, LOGD_SEVERE, "Error could not get USBH_pmgr\n");
    sc_miscError(SC_ERR_SYSTEM_FATAL,2);
  } 
  logd_printf(logd, LOGD_INFO, 
	      "USBH_pmgr found ... setting up MIDI\n");
  
  error = midi_init ();

  if ( error ){
    logd_printf(logd, LOGD_SEVERE,"Error setting up MIDI: %d\n",error);
    sc_sleep(10);
    sc_miscError(SC_ERR_SYSTEM_FATAL,2);
  }

  for(;;){
    static const sc_msgid_t sel[] = {
      HCC_USBH_CONNECT,
      HCC_USBH_DISCONNECT,
      HCC_USBH_MIDI_EVENT,
      0
    };

    msg = sc_msgRx(SC_ENDLESS_TMO, sel, SC_MSGRX_MSGID);
    switch( msg->id ){
    case SDD_DEV_OPEN:
      msg->id = SDD_DEV_OPEN_REPLY;
      md = (int)msg->base.handle;
      if ( devices & (1<<md) ){
	deviceOpen |= (1<<md);
	++openCnt[md];
      } else {
	msg->base.error = ENOENT;
      }
      sc_msgTx(&msg, sc_msgSndGet(&msg), 0);
      break;
    case SDD_DEV_CLOSE:
      msg->id = SDD_DEV_CLOSE_REPLY;
      md = (int)msg->base.handle;
      if ( devices & (1<<md) ){
	--openCnt[md];
	if ( openCnt[md] == 0 ){
	  deviceOpen &= ~(1<<md);
	}
      } else {
	msg->base.error = ENOENT;
      }
      sc_msgTx(&msg, sc_msgSndGet(&msg), 0);
      break;
    case HCC_USBH_CONNECT:
      md = (int)msg->connect.hdl;
      devices |= (1<<md);
      strcpy(name,"midi");
      sc_miscInt2Str(md,name+4,2);      
      registerDev(name, logd, self, (void *)md);
      midi_get_info(md,&cin,&cout);
      logd_printf(logd,LOGD_INFO,"Device %d:%d in, %d out\n",md,cin,cout);
      sc_msgFree(&msg);      
      break;
    case HCC_USBH_DISCONNECT:
      md = (int)msg->disconnect.hdl;
      if ( devices & (1<<md) ){
	devices &= ~(1<<md);
	strcpy(name,"midi");
	sc_miscInt2Str(md,name+4,2);
	unregisterDev(name,logd);
      }
      sc_msgFree(&msg);      
      break;
    case HCC_USBH_MIDI_EVENT:
      logd_printf(logd,LOGD_INFO,"%d events\n",msg->midi_event.n);
      for(i = 0; i < msg->midi_event.n; ++i){
	logd_printf(logd, LOGD_ALL,"%08d:%02x-%02x : %02x %02x %02x %02x\n",
                    msg->midi_event.time,
		    msg->midi_event.device,
		    msg->midi_event.event[i].cn,
		    msg->midi_event.event[i].cin,
		    msg->midi_event.event[i].event[0],
		    msg->midi_event.event[i].event[1],
		    msg->midi_event.event[i].event[2]);
	++nEvent;
	if ( midi_add_event(msg->midi_event.device,&msg->midi_event.event[i])== MIDI_FULL){
	  midi_send(msg->midi_event.device);
	  nEvent = 0;
	}      
      }
      if ( nEvent == 1 ){
	if ( midi_send(msg->midi_event.device) ){
	  kprintf(0,"Send error\n");
	}
	nEvent = 0;
      }
      sc_msgFree(&msg);
      break;
    }
  }
}

