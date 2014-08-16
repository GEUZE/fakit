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
** ID: S04107CL13 
** +Revision: 1.6 +
** +Date: 2008/01/16 13:02:09 +
** Get an object with specific properties
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sys/errno.h>
#include <sdd/sdd.h>

sdd_obj_t * 
sfs_get (sdd_obj_t * dir, const char *name, sc_msgid_t type)
{
  const char *last;

  sdd_obj_t * man;
  sdd_obj_t * obj;

  /*
  ** A relative path must have a directory object
  */
  if (name[0] != '/' && !dir) {
    sc_miscErrnoSet (EBADF);
    return NULL;
  }

  if ( !dir ){
    /*
    ** If no dir is set, we try to start with SCP_devman
    ** as root manager.
    */
    dir = sdd_manGetRoot ("/SCP_devman", 
			  "/", 
			  SC_DEFAULT_POOL,
			  SC_FATAL_IF_TMO);
    if ( !dir ){
      sc_miscErrnoSet (ENODEV);
      return NULL;
    }
  }

  man = sdd_objResolve (dir, name, &last);

  if (!last || !last[0]) {
    if (man && (!type || ((man->type & type) == type) )) {
      return man;
    } else if (man && man != dir) {
      sdd_objFree (&man);
      sc_miscErrnoSet (EISDIR);
    } else {
      sc_miscErrnoSet (ENOENT);
    }
  } else if (man) {
    obj = sdd_manGetByName (man, last);

    if (man != dir) {
      sdd_objFree (&man);
    }
    
    if (obj && (!type || ((obj->type & type) == type) )) {
      return obj;
    } else if (obj) {
      sc_miscErrnoSet (ENODEV);
      sdd_objFree (&obj);
    } else {
      sc_miscErrnoSet (ENOENT);
    }    
  }
  return NULL;
}

