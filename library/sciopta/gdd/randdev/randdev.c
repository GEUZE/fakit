/*SOC -*-c++-*-
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
** (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
**                                                                  **
**********************************************************************
** ID: S03004CL2                                                    **
** +Revision: 1.27 +                                                 **
** +Date: 2008/01/16 13:01:59 +                                     **
** Random device                                                    **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <ossys/libsc.h>

#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#include <logd/logd.h>
#include <dev/randdev.h>

#include <string.h>

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
};

typedef struct fildev_s {
  sc_msgid_t id;
  int pos;
} fildev_t;

static void 
register_dev(const char *name, logd_t NEARPTR logd)
{
  /* registration */
  sdd_obj_t NEARPTR dev;
  sdd_obj_t NEARPTR man;

  dev = (sdd_obj_t NEARPTR) sc_msgAlloc (sizeof (sdd_obj_t), 0,
                                      SC_DEFAULT_POOL, SC_FATAL_IF_TMO);
  dev->base.error = 0;
  dev->base.handle = NEAR_NULL;
  dev->type = SDD_OBJ_TYPE | SDD_DEV_TYPE | SDD_FILE_TYPE;
  strncpy (dev->name, name, SC_NAME_MAX);
  dev->controller = dev->receiver = dev->sender = sc_procIdGet(NULL,SC_NO_TMO);
  /* register to dev man */
  man = sdd_manGetRoot ("/SCP_devman", "/", SC_DEFAULT_POOL, SC_FATAL_IF_TMO);
  if (man) {
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

/** Mother process */ 
SC_PROCESS(randdev)
{
  sc_msg_t msg;
  sc_pid_t to;
  
  int ref = 0;
  logd_t NEARPTR logd =
    logd_new ("/SCP_logd", LOGD_INFO, "random", SC_DEFAULT_POOL,
	      SC_FATAL_IF_TMO);
  
  register_dev("random",logd);

  for (;;) {
    msg = sc_msgRx (SC_ENDLESS_TMO, SC_MSGRX_ALL, SC_MSGRX_MSGID);
    to = sc_msgSndGet (&msg);
    switch (msg->id) {
      
    case SDD_DEV_CLOSE:
      {
	--ref;
	if (ref < 0) {
	  sc_miscError (SDD_ERR_BASE + SC_EREFNO, 0);
	}
	if (ref == 0) {
	  /* close it */
	}
	logd_printf (logd, LOGD_FINE, "%8x decr: Reference %d\n", to, ref);
      }
      break;

    case SDD_OBJ_DUP:
    case SDD_DEV_OPEN:
      {
	++ref;
	if (ref < 0) {
	  sc_miscError (SDD_ERR_BASE + SC_EREFNO, 0);
	}
	msg->open.base.error = 0;
	msg->open.base.handle = NEAR_NULL;
      }
      break;

    case SDD_DEV_READ:
      {
	msg->read.base.error = 0;
	if (!msg->read.outlineBuf) {
	  msg->read.outlineBuf = msg->read.inlineBuf;
	}
	sc_miscRandSeed ((int)sc_tickGet ());
	msg->read.curpos = 0;
	while (msg->read.curpos < msg->read.size) {
	  msg->read.outlineBuf[msg->read.curpos] = (__u8)(sc_miscRand() & 255);
	  ++msg->read.curpos;
	}
      }
      break;

    case SDD_DEV_WRITE:
      {
	msg->write.base.error = 0;
      }
      break;
      
    case SDD_FILE_SEEK:
      {
	msg->seek.base.error = 0;
      }
      break;

    case SDD_DEV_IOCTL:
      {
	switch (msg->ioctl.cmd) {
	default:
	  msg->base.error = EINVAL;
	  break;
	}
      }
      break;

    default:
      msg->base.error = SC_ENOTSUPP;
      break;
    }
    ++msg->id;
    sc_msgTx (&msg, to, 0);
  }
}

