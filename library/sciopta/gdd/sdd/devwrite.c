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
** ID: S04147BS1                                                    **
** +Revision: 1.7 +                                                **
** +Date: 2008/01/16 13:02:00 +                                     **
** Basic device functions: sdd_devWrite                             **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_devWrite_t write;
};

ssize_t
sdd_devWrite (const sdd_obj_t NEARPTR self, const __u8 *buf, ssize_t size)
{
  static const sc_msgid_t select[2] = {
    SDD_DEV_WRITE_REPLY,
    0
  };

  sc_msg_t msg;
  ssize_t ret;

  msg = sc_msgAlloc (sizeof (sdd_devWrite_t), 
		     SDD_DEV_WRITE,
		     sc_msgPoolIdGet ((sc_msgptr_t) &self), 
		     SC_FATAL_IF_TMO);

  msg->base.error = 0;
  msg->base.handle = self->base.handle;
  msg->write.size = size;
  msg->write.curpos = 0;
  msg->write.outlineBuf = buf;
  sc_msgTx (&msg, self->sender, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  select, SC_MSGRX_MSGID);

  ret = msg->write.size;
  if (msg->base.error) {
    sc_miscErrnoSet (msg->base.error);
    ret = -1;
  }
  sc_msgFree (&msg);
  return ret;
}

