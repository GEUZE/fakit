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
** ID: S03070CL25                                                   **
** +Revision: 1.29 +                                                **
** +Date: 2006/05/24 10:16:39 +                                     **
** chroot                                                           **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>
#include <string.h>
#include <errno.h>
#include <sdd/sdd.h>

int
chroot (const char *dirname)
{
  fd_tab_t NEARPTR fd_tab;
  sdd_obj_t NEARPTR man;
  const char **last = NULL;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC) {
      man = sdd_objResolve (fd_tab->manager, dirname, last);
      if (!man) {
	sc_miscErrnoSet (ENOTDIR);
	return -1;
      }
      sdd_objFree (&fd_tab->manager);
      fd_tab->manager = man;
      return 0;
    }
    else {
      PRINTF ("This is not a SCIO fd_tab \n");
      sc_miscErrnoSet (EPERM);
      return -1;
    }
  }
  else {
    PRINTF ("There is no SCIO fd_tab installed \n");
    sc_miscErrnoSet (EPERM);
    return -1;
  }
}

