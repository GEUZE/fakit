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
** ID: S03070CL34                                                   **
** +Revision: 1.23 +                                                **
** +Date: 2005/03/21 07:30:17 +                                     **
** dup dup2                                                         **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>
#include <errno.h>
#include <sdd/sdd.h>

#if defined __ARMCC_VERSION
/* suppress: assignment in condition warning */
#pragma diag_suppress 1293
#endif

int
dup (int oldfd)
{
  fd_tab_t NEARPTR fd_tab;

  sc_miscErrnoSet (0);
  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC) {
      int fd;
      if (oldfd < 0 || oldfd >= fd_tab->max_fd || !fd_tab->bf[oldfd]) {
	sc_miscErrnoSet (EBADF);
	return -1;
      }
      fd = 0;
      while (fd < fd_tab->max_fd && fd_tab->bf[fd]) {
	++fd;
      }
      if (fd >= fd_tab->max_fd) {
	sc_miscErrnoSet (EMFILE);
	return -1;
      }
      if ((fd_tab->bf[fd] = sdd_objDup (fd_tab->bf[oldfd]))) {
	return fd;
      }
      else {
	return -1;
      }
    }
  }
  else {
    sc_miscErrnoSet (EMFILE);
  }

  return -1;
}

int
dup2 (int oldfd, int newfd)
{
  fd_tab_t NEARPTR fd_tab;

  sc_miscErrnoSet (0);
  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC) {
      if (newfd < 0 || newfd >= fd_tab->max_fd) {
	sc_miscErrnoSet (EINVAL);
	return -1;
      }
      if (fd_tab->bf[newfd]) {
	sc_miscErrnoSet (EEXIST);
	return -1;
      }
      if (oldfd < 0 || oldfd >= fd_tab->max_fd || !fd_tab->bf[oldfd]) {
	sc_miscErrnoSet (EBADF);
	return -1;
      }
      if ((fd_tab->bf[newfd] = sdd_objDup (fd_tab->bf[oldfd]))) {
        return newfd;
      }
      else {
	return -1;
      }
    }
  }
  else {
    sc_miscErrnoSet (EMFILE);
  }
  return -1;
}

