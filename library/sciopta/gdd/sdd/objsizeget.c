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
** +Revision: 1.6 + 
** +Date: 2008/01/16 13:02:00 + 
** Get size of object. 
**********************************************************************
EOC*/

#include <sciopta.h>
#include <string.h>
#include <sdd/sdd.msg>
#include <sdd/sdd.h>

union sc_msg {
  sc_msgid_t id;
  sdd_objSize_t size;
};

int
sdd_objSizeGet (const sdd_obj_t NEARPTR obj, sdd_size_t * size)
{
  sc_msg_t msg;

  static const sc_msgid_t sel[2] = { SDD_OBJ_SIZE_GET_REPLY, 0 };

  msg = sc_msgAllocClr (sizeof (sdd_objSize_t), 
			SDD_OBJ_SIZE_GET,
			SC_DEFAULT_POOL,
			SC_FATAL_IF_TMO);

  msg->size.base.handle = obj->base.handle;
  sc_msgTx (&msg, obj->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  sel, SC_MSGRX_MSGID);

  if (msg->size.base.error) {
    sc_miscErrnoSet (msg->size.base.error);
    sc_msgFree (&msg);
    return -1;
  }
  else {
    size->total  = msg->size.total;
    size->free = msg->size.free;
    size->bad = msg->size.bad;
    size->used = msg->size.total - size->free - size->bad;
    sc_msgFree(&msg);
    return 0;
  }
}

