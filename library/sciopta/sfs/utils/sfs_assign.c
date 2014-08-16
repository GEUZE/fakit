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
** ID: S04106CL1 
** +Revision: 1.3 +
** +Date: 2004/10/07 09:25:28 +
** Assign a device to a filesystem 
**********************************************************************
EOC*/

#include <sciopta.h>
#include <string.h>

#include <sdd/sdd.h>

#include <sfs/sfs.h>
#include <sfs/sfs.msg>

union sc_msg {
  sc_msgid_t id;
  sfs_assign_t assign;
};

int
sfs_assign (sdd_obj_t NEARPTR  self, sdd_obj_t NEARPTR  NEARPTR   dev)
{
  sc_msg_t msg;
  static const sc_msgid_t sel[3] = {
    SDD_ERROR, SFS_ASSIGN_REPLY, 0
  };

  (*dev)->base.id = SFS_ASSIGN;
  (*dev)->manager = self->base.handle;
  sc_msgTx ((sc_msgptr_t ) dev, self->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO, (void *) sel, SC_MSGRX_MSGID);
  if (msg->assign.object.base.error) {
    sc_miscErrnoSet (msg->assign.object.base.error);
    sc_msgFree (&msg);
    return -1;
  }
  else {
    sc_msgFree (&msg);
    return 0;
  }
}

/*
 * $Log: sfs_assign.c,v $
 * Revision 1.3  2004/10/07 09:25:28  bastian
 * NEAR * => NEARPTR
 *
 * Revision 1.2  2004/10/04 13:33:58  bastian
 * fix for NEAR/FAR
 *
 * Revision 1.1  2004/04/15 11:47:11  christian
 * Split up files into single files to have a better scalability on code size
 *
 *
 */
