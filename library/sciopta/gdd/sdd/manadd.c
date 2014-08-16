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
** ID: S04147BS7                                                    **
** +Revision: 1.7 +                                                 **
** +Date: 2008/01/16 13:02:00 +                                     **
** Manager function: sdd_manAdd                                     **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
};

int
sdd_manAdd (const sdd_obj_t NEARPTR self, sdd_obj_t NEARPTR NEARPTR object)
{
  static const sc_msgid_t select[2] = {
    SDD_MAN_ADD_REPLY, 0
  };
  int ret;
  sc_msg_t msg;

  (*object)->base.id = SDD_MAN_ADD;
  (*object)->manager = self->base.handle;
  sc_msgTx ((sc_msgptr_t) object, self->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  select, SC_MSGRX_MSGID);
  ret = 0;
  if (msg->base.error) {
    sc_miscErrnoSet (msg->base.error);
    ret = -1;
  }
  sc_msgFree (&msg);
  return ret;
}

