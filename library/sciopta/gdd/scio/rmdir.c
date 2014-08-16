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
** ID: S06143BS1                                                    **
** +Revision: 1.1 +                                                **
** +Date: 2006/05/24 11:07:41 +                                     **
** rmdir                                                            **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <errno.h>
#include <scio.h>
#include <sdd/sdd.h>

#include <unistd.h>
#include <string.h>

int rmdir(const char *pathname)
{
  fd_tab_t NEARPTR fd_tab;
  sdd_obj_t NEARPTR obj;
  sdd_obj_t NEARPTR man;
  const char *devName;
  int ret = -1;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC) {

      sc_miscErrnoSet (0);

      man = sdd_objResolve (fd_tab->manager, pathname, &devName);
      if (!man || (devName && devName[0]) || ! SDD_IS_A(man,SFS_DIR_TYPE)) {
	sc_miscErrnoSet (ENOENT);

	if ( man != fd_tab->manager ){
	  sdd_objFree(&man);
	}
	return -1;
      }
      
      /*
      ** Create a dummy object a we actually want to
      ** remove the manager.
      */
      obj = (sdd_obj_t NEARPTR) sc_msgAllocClr (sizeof (sdd_obj_t), 
						SDD_OBJ,
						SC_DEFAULT_POOL, 
						SC_NO_TMO);

      obj->type = SDD_OBJ_TYPE | SDD_DEV_TYPE | SFS_DIR_TYPE;

      ret = sdd_manRm (man, obj,sizeof(sdd_obj_t));

      if ( man != fd_tab->manager ){
	sdd_objFree(&man);
      }

      return ret;
    }
  }

  sc_miscErrnoSet (EMFILE);
  return -1;
}
