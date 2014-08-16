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
** +Revision: 1.2 +
** +Date: 2006/12/06 11:26:39 +
** Find first file with a given pattern
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sys/errno.h>
#include <sfs/sfs.h>
#include <sfs/sfs.msg>

#include <string.h>

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_obj_t object;
  sfs_ffirst_t ffirst;
};

sdd_obj_t NEARPTR
sfs_findFirst (sdd_obj_t NEARPTR dir, const char *mask)
{
  sc_msg_t msg;
  static const sc_msgid_t sel[3] = {
    SDD_ERROR, SFS_FFIRST_REPLY, 0
  };

  if ( strlen(mask) > SC_NAME_MAX ){
    sc_miscErrnoSet (EBADF);
    return NULL;
  }

  msg = sc_msgAllocClr (sizeof (sfs_ffirst_t), 
			SFS_FFIRST,
			sc_msgPoolIdGet ((sc_msgptr_t ) &dir), 
			SC_FATAL_IF_TMO);

  msg->ffirst.object.manager = dir->base.handle;
  strcpy(msg->ffirst.object.name,mask);

  sc_msgTx (&msg, dir->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO, sel, SC_MSGRX_MSGID);

  sc_miscErrnoSet (msg->base.error);

  if (msg->base.error) {
    sc_msgFree (&msg);
    return NULL;
  } else {
    return &msg->object;
  }
}

