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
** ID: S05133CL1
** +Revision: 1.5 + 
** +Date: 2008/01/16 13:02:00 + 
** Manager function : sdd_manNotifyRm
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <string.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>

union sc_msg {
  sc_msgid_t id;
  sdd_manNotify_t notify;
};

int
sdd_manNotifyRm (const sdd_obj_t NEARPTR man, const char *name, sc_ticks_t tmo)
{
  static const sc_msgid_t waitNotify[2] = { 
    SDD_MAN_NOTIFY_RM_REPLY, 
    0 
  };
  int ret;
  sc_msg_t msg;



  msg = sc_msgAllocClr (sizeof (sdd_manNotify_t), 
			SDD_MAN_NOTIFY_RM,
                        SC_DEFAULT_POOL, 
			SC_FATAL_IF_TMO);

  strncpy (msg->notify.name.string, name, SC_NAME_MAX);
  msg->notify.name.base.handle = man->base.handle;
  msg->notify.tmo = tmo;

  sc_msgTx (&msg, man->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  waitNotify, SC_MSGRX_MSGID);
  ret = 0;
  if (msg->notify.name.base.error) {
    sc_miscErrnoSet (msg->notify.name.base.error);
    ret = -1;
  }
  sc_msgFree (&msg);
  return ret;
}
