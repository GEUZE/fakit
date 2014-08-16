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
** ID: S04147BS10                                                   **
** +Revision: 1.11 +                                                 **
** +Date: 2008/01/16 13:02:00 +                                     **
** Manager function: sdd_manGetByName                               **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <errno.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#include <string.h>

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_obj_t object;
};

sdd_obj_t NEARPTR 
sdd_manGetByName (const sdd_obj_t NEARPTR self, const char *name)
{
  sc_msg_t msg;
  sc_msg_rx_t select[2];

  msg = sc_msgAllocClr (sizeof (sdd_manGet_t), 
			SDD_MAN_GET,
			sc_msgPoolIdGet ((sc_msgptr_t) &self), 
			SC_FATAL_IF_TMO);

  msg->object.manager = self->base.handle;
  (void)strncpy (msg->object.name, name, SC_NAME_MAX);
  sc_msgTx (&msg, self->controller, 0);

  select[0].msgid = SDD_MAN_GET_REPLY;
  select[1].msgid = 0;
  select[0].pid = self->controller;
  select[1].pid = 0;
  
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

