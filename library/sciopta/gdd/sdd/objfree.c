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
** ID: S04147BS16                                                   **
** +Revision: 1.7 +                                                **
** +Date: 2008/01/16 13:02:00 +                                     **
** Basic object functions: sdd_objFree                              **
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

static void
sdd_objRelease (const sdd_obj_t NEARPTR self)
{
  sc_msg_t msg;

  static const sc_msgid_t select[2] = {
    SDD_OBJ_RELEASE_REPLY, 0
  };

  if (self->controller == sc_procIdGet (NULL, 0)) {
    return;
  }

  msg = sc_msgAlloc (sizeof (sdd_objRelease_t), 
		     SDD_OBJ_RELEASE,
		     sc_msgPoolIdGet ((sc_msgptr_t) &self), 
		     SC_FATAL_IF_TMO);

  msg->base.error = 0;
  msg->base.handle = self->base.handle;
  sc_msgTx (&msg, self->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  select, SC_MSGRX_MSGID);
  sc_msgFree (&msg);
}

void
sdd_objFree(sdd_obj_t NEARPTR NEARPTR self)
{
  if (self && *self) {
    sdd_objRelease (*self);
    sc_msgFree((sc_msgptr_t) self);
  }
} 
