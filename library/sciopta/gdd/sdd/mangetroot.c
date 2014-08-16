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
** ID: S04147BS12                                                   **
** +Revision: 1.8 +                                                 **
** +Date: 2008/01/16 13:02:00 +                                     **
** Manager function: sdd_manGetRoot                                 **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#include <string.h>  /* strncpy */

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_obj_t object;
};

sdd_obj_t NEARPTR 
sdd_manGetRoot (const char *process, 
		const char *name, 
		sc_poolid_t plid,
		sc_ticks_t tmo)
{
  sc_pid_t pid;
  sdd_obj_t NEARPTR tmp = NEAR_NULL;

  pid = sc_procIdGet (process, tmo);

  if (pid != SC_ILLEGAL_PID) {
    tmp = (sdd_obj_t NEARPTR) sc_msgAllocClr (sizeof (sdd_obj_t),
					      SDD_OBJ,
					      plid,
					      SC_FATAL_IF_TMO);
    tmp->type = SDD_OBJ_TYPE | SDD_MAN_TYPE;
    (void)strncpy (tmp->name, name, SC_NAME_MAX);
    tmp->controller = tmp->sender = tmp->receiver = pid;
  }
  return tmp;
}
