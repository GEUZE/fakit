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
** ID: S04147BS15                                                   **
** +Revision: 1.9 +                                                **
** +Date: 2008/01/16 13:02:00 +                                     **
** Basic object functions: sdd_objDup                               **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <string.h> /* memcpy */
#include <sdd/sdd.h>
#include <sdd/sdd.msg>

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_objDup_t duplicate;
};

sdd_obj_t NEARPTR 
sdd_objDup (const sdd_obj_t NEARPTR self)
{
  sc_msg_t msg;
  int size;
  sc_poolid_t plid;

  static const sc_msgid_t select[2] = {
    SDD_OBJ_DUP_REPLY, 0
  };

  if (!self) {
    return NEAR_NULL;
  }

  size = sc_msgSizeGet ((sc_msgptr_t) &self);
  plid = sc_msgPoolIdGet ((sc_msgptr_t) &self);

  if (self->controller == sc_procIdGet (NULL,SC_NO_TMO)) {
    /* If it is me, just do a clone */
    /* This should be save, cause a driver should never use himself */
    sc_msg_t dup = sc_msgAlloc (size, SDD_OBJ_DUP, plid, SC_FATAL_IF_TMO);
    memcpy (dup, self, size);
    return (sdd_obj_t NEARPTR) dup;
  }

  msg = sc_msgAlloc(sizeof (sdd_objDup_t), SDD_OBJ_DUP, plid, SC_FATAL_IF_TMO);
  msg->base.error = 0;
  msg->base.handle = self->base.handle;
  sc_msgTx (&msg, self->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  select, SC_MSGRX_MSGID);

  if (msg->base.error) {
    sc_miscErrnoSet (msg->base.error);
    sc_msgFree (&msg);
    return NEAR_NULL;
  }
  else {
    sc_msg_t dup = sc_msgAlloc (size, SDD_OBJ_DUP, plid, SC_FATAL_IF_TMO);
    memcpy (dup, self, size);
    dup->base.handle = msg->base.handle;
    sc_msgFree (&msg);
    return (sdd_obj_t NEARPTR) dup;
  }
}

