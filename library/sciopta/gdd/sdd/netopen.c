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
** ID: S04147BS32                                                   **
** +Revision: 1.10 +                                                 **
** +Date: 2008/01/16 13:02:00 +                                     **
** Basic net functions: sdd_netOpen                                 **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <errno.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_netOpen_t open;
};

int
sdd_netOpen (sdd_obj_t NEARPTR self, __u32 proto, sc_pid_t proxy)
{
  static const sc_msgid_t select[2] = {
    SDD_NET_OPEN_REPLY, 0
  };

  sc_msg_t msg;
  int ret;

  msg =  sc_msgAlloc (sizeof (sdd_netOpen_t), 
		      SDD_NET_OPEN,
		      sc_msgPoolIdGet ((sc_msgptr_t) &self), 
		      SC_FATAL_IF_TMO);

  msg->open.base.error = 0;
  msg->open.base.handle = self->base.handle;
  msg->open.proto = proto;
  if (proxy == SC_CURRENT_PID) {
    msg->open.proxy = sc_procIdGet (NULL, 0);
  }
  else {
    msg->open.proxy = proxy;
  }

  sc_msgTx (&msg, self->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  select, SC_MSGRX_MSGID);
  self->base.handle = msg->base.handle;
  ret = 0;
  if (msg->base.error) {
    sc_miscErrnoSet (msg->base.error);
    ret = -1;
  }
  sc_msgFree (&msg);

  return ret;
}

