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
** ID: S08307BS1                                                    **
** +Revision: 1.1.2.2 +                                                 **
** +Date: 2009/06/15 11:33:42 +                                     **
** OS interface for HCC-EUSBH                                       **
**********************************************************************
EOC*/
#include <sciopta.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#include <dev/sem.h>
#include <logd/logd.h>

#include <hcc/eusbh/port_os.h>
#include <hcc/eusbh/os.h>
#include <hcc/eusbh/usbh.msg>
#include "usb_host_def.h"

#if OS_MUTEX_COUNT
int os_mutex_create(OS_MUTEX_TYPE **pmutex)
{
  static int cnt = 0;
  sc_cpuflags_t oldmask;
  sdd_obj_t *sem;
  char name[12] = "EUSBHmutex00";

  name[9] = (cnt >> 4)+'0';
  name[10] = (cnt & 0xf)+'0';
  
  sem = sem_new(name,1,"/SCP_sem",SC_DEFAULT_POOL);
  if ( sem == NULL ) return 1;

  LOCK(oldmask);
  ++cnt;
  UNLOCK(oldmask);

  *pmutex = sem;
  return OS_SUCCESS;
}

int os_mutex_get(OS_MUTEX_TYPE *pmutex)
{
  return sem_decr(pmutex,1);
}


int os_mutex_put(OS_MUTEX_TYPE *pmutex)
{
  return sem_incr(pmutex,1);
}
#endif


#ifdef EHCI
void os_requestDoorbell()
{
  extern sc_pid_t dev_HCCI_ehci_pid;
  sc_msg_t msg;
  static const sc_msgid_t sel[2] = { 
    HCC_USBH_DOORBELL_REPLY,
    0 
  };

  msg = sc_msgAlloc(sizeof(hcc_usbhDoorBell_t),
		    HCC_USBH_DOORBELL,
		    SC_DEFAULT_POOL,
		    SC_FATAL_IF_TMO);
  sc_msgTx(&msg, dev_HCCI_ehci_pid, 0);

/* wait doorbell request to finish */
//->  kprintf(1,"Wait for doorbell\n");
  msg = sc_msgRx( SC_ENDLESS_TMO, sel, SC_MSGRX_MSGID);
  sc_msgFree(&msg);
//->  kprintf(1,"Got doorbell\n");
}
#endif

int os_isr_init (hcc_u32 pos, void(*ih)(void))
{
  return 0;
}
/*
***************************************
** 
** Register a file-device with devman
**
*****************************************
*/
static void registerDev (const char *name, logd_t NEARPTR logd, sc_pid_t driver)
{
  /* registration */
  sdd_obj_t NEARPTR dev;
  sdd_obj_t NEARPTR man;

  logd_printf (logd, LOGD_INFO, "Register %s\n", name);

  dev = (sdd_obj_t NEARPTR) sc_msgAlloc (sizeof (sdd_obj_t), 0,
                                         SC_DEFAULT_POOL, SC_FATAL_IF_TMO);
  dev->base.error = 0;
  dev->base.handle = NEAR_NULL;
  dev->type = SDD_OBJ_TYPE;
  strncpy (dev->name, name, SC_NAME_MAX);
  dev->controller = dev->receiver = dev->sender = driver;

  /* register to dev man */
  man = sdd_manGetRoot ("/SCP_devman", "/", SC_DEFAULT_POOL, SC_FATAL_IF_TMO);
  if (man) {
    if (sdd_manAdd (man, &dev)) {
      logd_printf (logd, LOGD_SEVERE, "Could not add %s \n",name);
      sc_procKill (SC_CURRENT_PID, 0);
    }
    sdd_objFree (&man);
  }
  else {
    logd_printf (logd, LOGD_SEVERE, "Could not get /SCP_devman\n");
    sc_procKill (SC_CURRENT_PID, 0);
  }
}

/*
****************************************
** 
** PMGR task, handles attaching/detaching
** of USB devices
**
****************************************
*/

extern int eusbh_init(void);     /* eusbh.c */

SC_PROCESS(HCCP_pmgr)
{
  logd_t *logd;

  logd = logd_new ("/SCP_logd", LOGD_INFO, "USBH_pmgr", SC_DEFAULT_POOL,
		   SC_FATAL_IF_TMO);

  os_buffer_init ();
  
  eusbh_init(); 

  /* now start stack */
  usbh_start();

  registerDev("USBH_pmgr",logd, sc_procIdGet(NULL, SC_NO_TMO));

  pmgr_pcd_task();
}

/*
****************************************
**
** Transfer task
**
****************************************
*/
//->extern void ehci_transfer_task(void); /* ehci.c (HCC)*/

SC_PROCESS(HCCP_transfer)
{
  int error;
  logd_t *logd;
  sdd_obj_t *root;

  logd = logd_new ("/SCP_logd", LOGD_INFO, "USBH_transfer", SC_DEFAULT_POOL,
		   SC_FATAL_IF_TMO);

  registerDev("USBH_transfer",logd, sc_procIdGet(NULL, SC_NO_TMO));

  logd_printf(logd, LOGD_INFO, "Waiting for USBH_pmgr...");

  /* First get root manager */
  root = sdd_manGetRoot ("/SCP_devman", "/", SC_DEFAULT_POOL, SC_ENDLESS_TMO);
  if ( !root ){
    logd_printf(logd, LOGD_SEVERE, "Error could not get root-manager\n");
    sc_sleep(10);
    sc_miscError(SC_ERR_SYSTEM_FATAL,1);
    sc_procKill(SC_CURRENT_PID,0);
  }

  /* wait for FS to appear */
  error = sdd_manNotifyAdd(root, "USBH_pmgr", SC_ENDLESS_TMO);

  sdd_objFree(&root);

  if ( error < 0 ){
    logd_printf(logd, LOGD_SEVERE, "Error could not get USBH_pmgr\n");
    sc_miscError(SC_ERR_SYSTEM_FATAL,2);
  } 

  logd_printf(logd, LOGD_INFO, "found.\n");

  _usbhd_transfer_task();
}
