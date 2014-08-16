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
** ID: S04107CL4
** +Revision: 1.5 +
** +Date: 2008/02/14 14:15:49 +
** Umount a mounted directory. 
**********************************************************************
EOC*/

#include <sciopta.h>
#include <string.h> /* memcpy */

#include <sdd/sdd.h>

#include <sfs/sfs.h>
#include <sfs/sfs.msg>

union sc_msg {
  sc_msgid_t id;
  sfs_mount_t mount;
};

int
sfs_umount (sdd_obj_t NEARPTR  self)
{
  sc_msg_t msg;
  static const sc_msgid_t sel[2] = {
    SFS_UMOUNT_REPLY, 0
  };
  int ret = 0;

  msg = sc_msgAlloc (sizeof (sdd_obj_t), 
		     SFS_UMOUNT,
		     sc_msgPoolIdGet ((sc_msgptr_t ) &self), 
		     SC_FATAL_IF_TMO);
  
  memcpy (msg, self, sizeof (sdd_obj_t));

  msg->mount.obj.base.id = SFS_UMOUNT;
  sc_msgTx (&msg, self->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO, sel, SC_MSGRX_MSGID);
  if (msg->mount.obj.base.error) {
    sc_miscErrnoSet (msg->mount.obj.base.error);
    ret = -1;
  }
  sc_msgFree (&msg);
  return ret;
}
