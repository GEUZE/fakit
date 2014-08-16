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
** ID: S04147BS14                                                   **
** +Revision: 1.8 +                                                 **
** +Date: 2008/01/16 13:02:00 +                                     **
** Manager function: sdd_manRm                                      **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <string.h>  /* memcpy */
#include <sdd/sdd.h>
#include <sdd/sdd.msg>

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_obj_t object;
};

int
sdd_manRm (const sdd_obj_t NEARPTR self, 
	   const sdd_obj_t NEARPTR reference, 
	   __uint size)
{
  static const sc_msgid_t select[2] = {
    SDD_MAN_RM_REPLY, 0
  };
  int ret;
  sc_msg_t msg;


  if (size < sizeof (sdd_obj_t)) {
    size = sizeof (sdd_obj_t);
  }

  msg = sc_msgAlloc (size, 
		     SDD_MAN_RM,
		     sc_msgPoolIdGet ((sc_msgptr_t) &self), 
		     SC_FATAL_IF_TMO);

  memcpy (msg, reference, size);
  msg->id = SDD_MAN_RM;
  msg->object.manager = self->base.handle;
  sc_msgTx (&msg, self->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  select, SC_MSGRX_MSGID);
  ret = 0;
  if (msg->base.error) {
    sc_miscErrnoSet (msg->base.error);
    ret = -1;
  }
  sc_msgFree (&msg);
  return ret;
}

