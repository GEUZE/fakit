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
** ID: S03070CL23                                                   **
** +Revision: 1.28 +                                                **
** +Date: 2006/05/24 10:16:39 +                                     **
** close closeall                                                   **
**********************************************************************
EOC*/

#include <string.h>
#include <sciopta.h>
#include <scio.h>
#include <errno.h>
#include <sdd/sdd.h>
#include <sys/unistd.h>

int
close (int fd)
{
  fd_tab_t NEARPTR fd_tab;
  PRINTF1 ("close fd == %d\n", fd);
  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC &&
	fd >= 0 && fd <= fd_tab->max_fd && fd_tab->bf[fd]) {

      if ((sdd_devClose (fd_tab->bf[fd])) != -1) {
	sdd_objFree (&fd_tab->bf[fd]);
	return 0;
      }
      else {
	sdd_objFree (&fd_tab->bf[fd]);
	return -1;
      }
    }
  }
  sc_miscErrnoSet (EBADF);
  return -1;
}

void
closeall (void)
{
  fd_tab_t NEARPTR fd_tab;
  int fd = 0;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC) {
      while (fd < fd_tab->max_fd) {
	if (fd_tab->bf[fd] && (sdd_devClose (fd_tab->bf[fd])) != -1) {
	  sdd_objFree (&fd_tab->bf[fd]);
	}
	++fd;
      }
    }
  }
  sc_miscErrnoSet (0);
}

