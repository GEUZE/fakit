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
** ID: S03070CL29                                                   **
** +Revision: 1.17 +                                                 **
** +Date: 2006/05/24 10:16:39 +                                     **
** Asynchronous write                                               **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>
#include <errno.h>
#include <sdd/sdd.h>

int awrite(int fd,sc_msgptr_t msg)
{
  fd_tab_t NEARPTR fd_tab;
  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC &&	
	fd >= 0 && fd <= fd_tab->max_fd && fd_tab->bf[fd]) 
    {
      return -1; //sdd_devAwrite (fd_tab->bf[fd], msg);
    }
    else {
      sc_miscErrnoSet (EBADF);
      return -1;
    }
  }
  return -1;
}

