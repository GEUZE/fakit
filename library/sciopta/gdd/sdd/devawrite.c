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
** ID: S04147BS5                                                    **
** +Revision: 1.6 +                                                 **
** +Date: 2008/01/16 13:02:00 +                                     **
** Basic device functions: sdd_devAwrite                            **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#include <string.h> /* memcpy */

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_devWrite_t write;
};

int
sdd_devAwrite (const sdd_obj_t NEARPTR self, const char *buf, ssize_t size)
{
  sc_msg_t msg;
  msg = sc_msgAlloc (sizeof (sdd_devWrite_t) + size,
		     SDD_DEV_WRITE, 
		     sc_msgPoolIdGet ((sc_msgptr_t) &self),
		     SC_FATAL_IF_TMO);

  msg->base.error = 0;
  msg->base.handle = self->base.handle;
  msg->write.size = size;
  msg->write.curpos = 0;
  msg->write.outlineBuf = NULL;
  memcpy (msg->write.inlineBuf, buf, size);
  sc_msgTx (&msg, self->receiver, 0);
  return 0;
}

