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
** ID: S04147BS5                                                    **
** +Revision: 1.7 +                                                **
** +Date: 2008/01/16 13:02:00 +                                     **
** Basic device functions: sdd_devIotcl                             **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_devIoctl_t ioctl;
};

int
sdd_devIoctl (const sdd_obj_t NEARPTR self, unsigned int cmd, unsigned long arg)
{
  static const sc_msgid_t select[2] = {
    SDD_DEV_IOCTL_REPLY,
    0
  };
  sc_msg_t msg;
  int ret;

  msg = sc_msgAlloc (sizeof (sdd_devIoctl_t), 
		     SDD_DEV_IOCTL,
		     sc_msgPoolIdGet ((sc_msgptr_t) &self), 
		     SC_FATAL_IF_TMO);

  msg->base.error = 0;
  msg->base.handle = self->base.handle;
  msg->ioctl.cmd = cmd;
  msg->ioctl.ret = 0;
  msg->ioctl.outlineArg = arg;
  sc_msgTx (&msg, self->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  select, SC_MSGRX_MSGID);

  ret = msg->ioctl.ret;
  if (msg->base.error) {
    sc_miscErrnoSet (msg->base.error);
    ret = -1;
  }
  sc_msgFree (&msg);
  return ret;
}

