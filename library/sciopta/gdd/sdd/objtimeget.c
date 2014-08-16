/*SOC
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
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S04168CL1
** +Revision: 1.7 + 
** +Date: 2008/01/16 13:02:00 + 
** Get time of object. 
**********************************************************************
EOC*/

#include <sciopta.h>
#include <string.h>
#include <sdd/sdd.msg>
#include <sdd/sdd.h>

union sc_msg {
  sc_msgid_t id;
  sdd_objTime_t time;
};

int
sdd_objTimeGet (const sdd_obj_t NEARPTR obj, __u32 *data)
{
  sc_msg_t msg;

  static const sc_msgid_t sel[3] = { SDD_OBJ_TIME_GET_REPLY, 0 };

  msg = sc_msgAllocClr (sizeof (sdd_objTime_t), 
			SDD_OBJ_TIME_GET,
			SC_DEFAULT_POOL, 
			SC_FATAL_IF_TMO);

  msg->time.base.handle = obj->base.handle;
  sc_msgTx (&msg, obj->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  sel, SC_MSGRX_MSGID);

  if (msg->time.base.error) {
    sc_miscErrnoSet (msg->time.base.error);
    sc_msgFree (&msg);
    return -1;
  }
  else {
    *data = msg->time.data;
    sc_msgFree (&msg);
    return 0;
  }
}

