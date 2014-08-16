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
** ID: S04147BS18                                                   **
** +Revision: 1.3 +                                                 **
** +Date: 2005/09/16 11:36:21 +                                     **
** Get a object descriptor from an object by name to handle 
** multiple instances of a device driver forexample in a small 
** system without managers.
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <string.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>

union sc_msg {
  sc_msgid_t id;
  sdd_obj_t obj;
};

sdd_obj_t NEARPTR 
sdd_objDesc (sc_pid_t pid, const char *name)
{
  sc_msg_t msg;

  static const sc_msgid_t sel[2] = { SDD_OBJ_DESC_GET_REPLY, 0 };

  msg = sc_msgAllocClr (sizeof (sdd_obj_t), SDD_OBJ_DESC_GET, SC_DEFAULT_POOL,
                        SC_FATAL_IF_TMO);
  strncpy (msg->obj.name, name, SC_NAME_MAX);
  sc_msgTx (&msg, pid, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  sel, SC_MSGRX_MSGID);
  sc_miscErrnoSet (msg->obj.base.error);
  if (msg->obj.base.error) {
    sc_msgFree (&msg);
  }
  
  return (sdd_obj_t NEARPTR) msg;
}

