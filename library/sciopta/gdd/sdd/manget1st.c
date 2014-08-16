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
** ID: S04147BS9                                                    **
** +Revision: 1.9 +                                                 **
** +Date: 2008/01/16 13:02:00 +                                     **
** Manager function : sdd_manGetFirst                               **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_obj_t object;
};

sdd_obj_t NEARPTR 
sdd_manGetFirst (const sdd_obj_t NEARPTR self, __uint size)
{
  sc_msg_t msg;
  
  static const sc_msgid_t select[2] = {
    SDD_MAN_GET_FIRST_REPLY, 0
  };

  if (size < sizeof (sdd_obj_t)) {
    size = sizeof (sdd_obj_t);
  }

  msg = sc_msgAllocClr (size, 
			SDD_MAN_GET_FIRST,
			sc_msgPoolIdGet ((sc_msgptr_t) &self), 
			SC_FATAL_IF_TMO);

  msg->object.manager = self->base.handle;
  sc_msgTx (&msg, self->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  select, SC_MSGRX_MSGID);

  if (msg->base.error) {
    sc_miscErrnoSet (msg->base.error);
    sc_msgFree (&msg);
    return NEAR_NULL;
  }
  else {
    return &msg->object;
  }
}

