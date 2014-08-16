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
** ID: S04147BS18                                                   **
** +Revision: 1.6 +                                                 **
** +Date: 2006/05/10 09:04:19 +                                     **
** Basic object functions: sdd_objNew                               **
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
  sdd_obj_t object;
};

sdd_obj_t NEARPTR 
sdd_objNew (const char *name, 
	    sc_pid_t controller, 
	    sc_pid_t sender,
	    sc_pid_t receiver, 
	    void NEARPTR handle, 
	    sc_poolid_t plid,
	    sc_ticks_t tmo)
{
  sdd_obj_t NEARPTR tmp;

  tmp = (sdd_obj_t NEARPTR) sc_msgAlloc (sizeof (sdd_obj_t),
					 SDD_OBJ,
					 plid,
					 tmo);
  tmp->base.error = 0;
  tmp->base.handle = handle;
  tmp->type = SDD_OBJ_TYPE;
  (void)strncpy (tmp->name, name, SC_NAME_MAX);
  tmp->controller = controller;
  tmp->sender = sender;
  tmp->receiver = receiver;
  return tmp;
}

