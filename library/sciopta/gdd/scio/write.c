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
** ID: S03070CL16                                                   **
** +Revision: 1.22 +                                                **
** +Date: 2008/01/16 13:02:00 +                                     **
** write                                                            **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>
#include <errno.h>
#include <unistd.h>
#include <sdd/sdd.h>

ssize_t write (int fd, const void *buf, size_t size)
{
  fd_tab_t NEARPTR fd_tab;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC &&	
	fd >= 0 && fd <= fd_tab->max_fd && fd_tab->bf[fd]) 
    {
      return sdd_devWrite (fd_tab->bf[fd], (const __u8 *)buf, size);
    }
    else {
      sc_miscErrnoSet (EBADF);
      return -1;
    }
  }
  sc_miscErrnoSet (EMFILE);
  return -1;
}

