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
** ID: S04147BS3                                                    **
** +Revision: 1.9 +                                                **
** +Date: 2008/11/11 10:03:33 +                                     **
** Basic device functions: sdd_devOpenByName                        **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <errno.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#include <string.h>

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_obj_t object;
};

sdd_obj_t NEARPTR 
sdd_devOpenByName (const sdd_obj_t NEARPTR root, 
		   const char *path, flags_t flags)
{
  const char *name;

  sdd_obj_t NEARPTR folder = NEAR_NULL;
  sdd_obj_t NEARPTR file = NEAR_NULL;

  if (!root) {
    sc_miscErrnoSet (EINVAL);
    return NEAR_NULL;
  }
  /* walk the path */
  folder = sdd_objResolve (root, path, &name);
  if (!folder || !name || strchr (name, '/')) {
    sc_miscErrnoSet (ENOENT);
    return NEAR_NULL;
  }
  file = sdd_manGetByName (folder, name);
  /* release on-fly folder but not root */
  if ( folder != root ){
    sdd_objFree (&folder);
  }
  if (!file) {
    sc_miscErrnoSet (ENOENT);
    return NEAR_NULL;
  }
  if (sdd_devOpen (file, flags) != -1) {
    return file;
  }
  sdd_objFree (&file);

  return NEAR_NULL;
}

