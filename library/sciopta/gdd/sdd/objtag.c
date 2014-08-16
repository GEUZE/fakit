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
** Object generic ioctl call. 
**********************************************************************
EOC*/

#include <sciopta.h>
#include <string.h>
#include <sdd/sdd.msg>
#include <sdd/sdd.h>

union sc_msg {
  sc_msgid_t id;
  sdd_objIoctl_t ioctl;
};

int
sdd_objTagGet (const sdd_obj_t NEARPTR obj, 
	       sc_tag_t tag, 
	       void *data, 
	       size_t size)
{
  sc_msg_t msg;

  static const sc_msgid_t sel[2] = { SDD_OBJ_TAG_GET_REPLY, 0 };

  msg = sc_msgAllocClr (sizeof (sdd_objIoctl_t) + size, 
			SDD_OBJ_TAG_GET,
			SC_DEFAULT_POOL, 
			SC_FATAL_IF_TMO);

  msg->ioctl.u.ioctl.tag = tag;
  msg->ioctl.base.handle = obj->base.handle;
  sc_msgTx (&msg, obj->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  sel, SC_MSGRX_MSGID);

  if (msg->ioctl.base.error) {
    sc_msgFree (&msg);
    return -1;
  }
  else {
    memcpy (data, msg->ioctl.u.ioctl.data, size);
    sc_msgFree (&msg);
    return 0;
  }
}

int
sdd_objTagSet (const sdd_obj_t NEARPTR obj, 
	       sc_tag_t tag, 
	       const void *data, 
	       size_t size)
{
  sc_msg_t msg;
  static const sc_msgid_t sel[3] = { SDD_OBJ_TAG_SET_REPLY, SDD_ERROR, 0 };

  msg = sc_msgAllocClr (sizeof (sdd_objIoctl_t) + size, 
			SDD_OBJ_TAG_GET,
			SC_DEFAULT_POOL, 
			SC_FATAL_IF_TMO);

  memcpy (msg->ioctl.u.ioctl.data, data, size);
  msg->ioctl.u.ioctl.tag = tag;
  msg->ioctl.base.handle = obj->base.handle;
  sc_msgTx (&msg, obj->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  sel, SC_MSGRX_MSGID);

  if (msg->ioctl.base.error) {
    sc_msgFree (&msg);
    return -1;
  }
  else {
    sc_msgFree (&msg);
    return 0;
  }
}

