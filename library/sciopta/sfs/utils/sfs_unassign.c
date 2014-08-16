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
** +Date: 2008/01/16 13:02:09 +
** Unassign a device from a filesystem 
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

sdd_obj_t *
sfs_unassign (sdd_obj_t NEARPTR  self)
{
  sc_msg_t msg;
  
  static const sc_msgid_t sel[3] = {
    SDD_ERROR, SFS_UNASSIGN_REPLY, 0
  };

  msg = sc_msgAllocClr (sizeof (sfs_assign_t), SFS_UNASSIGN, SC_DEFAULT_POOL, 
                        SC_FATAL_IF_TMO);
  msg->assign.object.manager = self->base.handle;
  sc_msgTx (&msg, self->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO, (void *) sel, SC_MSGRX_MSGID);
  if (msg->assign.object.base.error) {
    sc_miscErrnoSet (msg->assign.object.base.error);
    sc_msgFree (&msg);
  }
  return (sdd_obj_t NEARPTR) msg;
}

/*
 * $Log: sfs_unassign.c,v $
 * Revision 1.3  2008/01/16 13:02:09  bastian
 * merge from 1.9.1.12
 *
 * Revision 1.2.8.1  2007/09/07 04:55:21  bastian
 * added comments
 *
 * Revision 1.2  2005/07/01 12:28:32  christian
 * Merged SCIOPTA_1_7_2 to MAIN
 *
 * Revision 1.1.2.1  2005/05/11 10:07:54  christian
 * Add a unassign function.
 *
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
