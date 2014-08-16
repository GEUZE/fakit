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
** ID: S04147BS6                                                    **
** +Revision: 1.6 +                                                 **
** +Date: 2006/03/17 06:35:54 +                                     **
** Basic device functions: sdd_devAread                             **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <string.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_devRead_t read;
};

int
sdd_devAread (const sdd_obj_t NEARPTR self, ssize_t size)
{
  sc_msg_t msg;
  msg = sc_msgAllocClr (sizeof (sdd_devRead_t) + size * sizeof (unsigned char),
		        SDD_DEV_READ, 
			sc_msgPoolIdGet ((sc_msgptr_t) &self),
		        SC_FATAL_IF_TMO);
  msg->base.handle = self->base.handle;
  msg->read.size = size;
  sc_msgTx (&msg, self->receiver, 0);
  return 0;
}

