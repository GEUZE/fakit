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
** ID: S04147BS2                                                    **
** +Revision: 1.7 +                                                **
** +Date: 2008/01/16 13:02:00 +                                     **
** Basic device functions: sdd_devRead                              **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_devRead_t read;
};

ssize_t
sdd_devRead (const sdd_obj_t NEARPTR self, __u8 *buf, ssize_t size)
{
  static const sc_msgid_t select[2] = {
    SDD_DEV_READ_REPLY,
    0
  };

  sc_msg_t msg;
  ssize_t ret;

  msg = sc_msgAlloc (sizeof (sdd_devRead_t), 
		     SDD_DEV_READ,
		     sc_msgPoolIdGet ((sc_msgptr_t) &self), 
		     SC_FATAL_IF_TMO);

  msg->base.error = 0;
  msg->base.handle = self->base.handle;
  msg->read.size = size;
  msg->read.curpos = 0;
  msg->read.outlineBuf = buf;
  sc_msgTx (&msg, self->receiver, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  select, SC_MSGRX_MSGID);

  ret = msg->read.size;
  if (msg->base.error) {
    sc_miscErrnoSet (msg->base.error);
    ret = -1;
  }

  sc_msgFree (&msg);
  return ret;
}

