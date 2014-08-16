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
** ID: S04147BS33                                                   **
** +Revision: 1.8 +                                                 **
** +Date: 2008/01/16 13:02:00 +                                     **
** Basic file function: sdd_fileSeek                                **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <errno.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_fileSeek_t seek;
};

off_t 
sdd_fileSeek (const sdd_obj_t NEARPTR self, off_t off, int whence)
{
  static const sc_msgid_t select[2] = { 
    SDD_FILE_SEEK_REPLY, 0 
  };

  sc_msg_t msg;
  ssize_t ret;

  msg = sc_msgAlloc (sizeof (sdd_fileSeek_t), 
		     SDD_FILE_SEEK,
		     sc_msgPoolIdGet ((sc_msgptr_t) &self), 
		     SC_FATAL_IF_TMO);

  msg->base.error = 0;
  msg->base.handle = self->base.handle;
  msg->seek.offset = off;
  msg->seek.whence = whence;
  sc_msgTx (&msg, self->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  select, SC_MSGRX_MSGID);
  
  ret = (ssize_t) msg->seek.offset;

  if (msg->base.error) {
    ret = -1;
    sc_miscErrnoSet (msg->base.error);
  }

  sc_msgFree (&msg);
  return ret;
}

