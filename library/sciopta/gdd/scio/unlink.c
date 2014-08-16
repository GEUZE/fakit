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
** ID: S03070CL31                                                   **
** +Revision: 1.27 +                                                **
** +Date: 2006/05/23 14:29:14 +                                     **
** unlink                                                           **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>
#include <errno.h>
#include <sdd/sdd.h>
#include <unistd.h>

#include <string.h>

int
unlink (const char *pathname)
{
  fd_tab_t NEARPTR fd_tab;
  sdd_obj_t NEARPTR man;
  sdd_obj_t NEARPTR device;
  const char *devName;

  if (!pathname) {
    sc_miscErrnoSet (EFAULT);
    return -1;
  }

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC) {
      man = sdd_objResolve (fd_tab->manager, pathname, &devName);

      if (!man) {
	PRINTF ("Error\n");
	sc_miscErrnoSet (ENOTDIR);
	return -1;
      }

      if (!devName) {
	PRINTF ("Error\n");
	sc_miscErrnoSet (EISDIR);
	return -1;
      }

      device = sdd_manGetByName (man, devName);

      if (!device
	  || !SDD_IS_A (device, SDD_DEV_TYPE)) {
	PRINTF ("Error\n");
	sc_miscErrnoSet (ENOENT);
	return -1;
      }

      if (sdd_manRm (man, device, sizeof (sdd_obj_t)) == 0) {
	if ( man != fd_tab->manager ){
	  sdd_objFree(&man);
	}
	sdd_objFree (&device);
	return 0;
      }
      else {
	if ( man != fd_tab->manager ){
	  sdd_objFree(&man);
	}
	sdd_objFree (&device);
	sc_miscErrnoSet (EPERM);
	return -1;
      }
    }
    else {
      PRINTF ("This is not a SCIO fd_tab \n");
      sc_miscErrnoSet (ENOENT);
      return -1;
    }
  }
  else {
    PRINTF ("There is no SCIO fd_tab installed \n");
    sc_miscErrnoSet (ENOENT);
    return -1;
  }
}

