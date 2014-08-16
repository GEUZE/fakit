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
** ID: S06142BS1                                                    **
** +Revision: 1.2 +                                                **
** +Date: 2006/05/23 14:29:13 +                                     **
** mkdir                                                            **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <errno.h>
#include <scio.h>
#include <sdd/sdd.h>
#include <sys/stat.h>

#include <string.h>

int mkdir(const char *pathname,mode_t mode)
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
      /*
      ** There must something left, else the directory exist already.
      */
      if (!devName || devName[0] == 0) {
	sc_miscErrnoSet (EEXIST);

	if ( man != fd_tab->manager ){
	  sdd_objFree(&man);
	}
	return -1;
      }
      /*
      ** The remaining name must not have a path separator
      */
      if ( strchr(devName,'/') ){
	sc_miscErrnoSet (EINVAL);
	if ( man != fd_tab->manager ){
	  sdd_objFree(&man);
	}
	return -1;
      }
	
      obj = (sdd_obj_t NEARPTR) sc_msgAllocClr (sizeof (sdd_obj_t), 
						SDD_OBJ,
						SC_DEFAULT_POOL, 
						SC_NO_TMO);

      strncpy (obj->name, devName, SC_NAME_MAX);
      obj->type = SDD_OBJ_TYPE | SDD_DEV_TYPE | SFS_DIR_TYPE;

      ret = sdd_manAdd (man, &obj);

      if ( man != fd_tab->manager ){
	sdd_objFree(&man);
      }

      return ret;
    }
  }

  sc_miscErrnoSet (EMFILE);
  return -1;
}
