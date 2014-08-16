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
** ID: S03070CL22                                                   **
** +Revision: 1.21 +                                                 **
** +Date: 2004/12/14 09:10:11 +                                     **
** closedir                                                         **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>
#include <sys/errno.h>
#include <dirent.h>
#include <sdd/sdd.h>

int
closedir (DIR NEARPTR dir)
{
  fd_tab_t NEARPTR fd_tab;
  if (!dir) {
    return EBADF;
  }
  if (dir->cur) {
    sdd_objFree (&dir->cur);
  }
  sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab);
  if (dir->man && dir->man != fd_tab->manager) {
    sdd_objFree (&dir->man);
  }
  sc_msgFree ((sc_msgptr_t) &dir);
  return 0;
}

