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
** ID: S03070CL14                                                   **
** +Revision: 1.19 +                                                 **
** +Date: 2006/05/24 10:16:39 +                                     **
** lseek                                                            **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>
#include <errno.h>
#include <sdd/sdd.h>

off_t
lseek (int fd, off_t off, flags_t flags)
{
  fd_tab_t NEARPTR fd_tab;
  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC &&
	fd >= 0 && fd <= fd_tab->max_fd && fd_tab->bf[fd]) {
      return sdd_fileSeek (fd_tab->bf[fd], off, flags);
    }
    else {
      sc_miscErrnoSet (EBADF);
      return (off_t) - 1;
    }
  }
  return (off_t) - 1;
}

