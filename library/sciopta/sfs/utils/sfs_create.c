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
** ID: S04107CL12
** +Revision: 1.7 +
** +Date: 2008/01/16 13:02:09 +
** Create a file 
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sys/errno.h>
#include <sdd/sdd.h>
#include <sfs/sfs.h>
#include <string.h>

int
sfs_create (sdd_obj_t NEARPTR dir, const char *name, sc_msgid_t type,
            mode_t mode)
{
  sdd_obj_t NEARPTR newobj;
  const char *last;
  int ret;
  sdd_obj_t NEARPTR man;

  man = sdd_objResolve (dir, name, &last);

  if (!man) {
    sc_miscErrnoSet (ENOTDIR);
    return -1;
  }

  if (last && strchr (last, '/')) {
    sc_miscErrnoSet (ENOTDIR);
    if (man != dir) {
      sdd_objFree (&man);
    }
    return -1;
  }

  if (!last || !last[0]) {
    sc_miscErrnoSet (EEXIST);
    if (man != dir) {
      sdd_objFree (&man);
    }
    return -1;
  }
  
  newobj =
    (sdd_obj_t NEARPTR) sc_msgAllocClr (sizeof (sdd_obj_t),
					SDD_OBJ,
					sc_msgPoolIdGet ((sc_msgptr_t) & dir),
					SC_FATAL_IF_TMO);
  newobj->type = type;
  strncpy (newobj->name, last, SC_NAME_MAX);

  ret = sdd_manAdd (man, &newobj);

  if (man != dir) {
    sdd_objFree (&man);
  }
  
  return ret;
}

