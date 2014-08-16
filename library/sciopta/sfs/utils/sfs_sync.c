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
** ID: S04107CL3
** +Revision: 1.4 +
** +Date: 2006/06/09 14:08:30 +
** Sync a filesystem down to its medium 
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sfs/sfs.h>
#include <sfs/sfs.msg>

union sc_msg {
  sc_msgid_t id;
  sfs_sync_t sync;
};

int
sfs_sync (sdd_obj_t NEARPTR  self)
{
  sc_msg_t msg;
  static const sc_msgid_t sel[3] = {
    SDD_ERROR, SFS_SYNC_REPLY, 0
  };

  msg = sc_msgAlloc (sizeof (sfs_sync_t), 
		     SFS_SYNC,
		     sc_msgPoolIdGet ((sc_msgptr_t ) &self), 
		     SC_FATAL_IF_TMO);

  msg->sync.base.handle = self->base.handle;
  sc_msgTx (&msg, self->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO, (void *) sel, SC_MSGRX_MSGID);

  if (msg->sync.base.error) {
    sc_miscErrnoSet (msg->sync.base.error);
    sc_msgFree (&msg);
    return -1;
  } else {
    sc_msgFree (&msg);
    return 0;
  }
}


/*
 * $Log: sfs_sync.c,v $
 * Revision 1.4  2006/06/09 14:08:30  bastian
 * removed superfluos includes
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
