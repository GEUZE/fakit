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
** ID: S03070CL19                                                   **
** +Revision: 1.13 +                                                 **
** +Date: 2006/05/24 10:16:39 +                                     **
** setsockopt / getsockopt                                          **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sdd/sdd.h>
#include <ips/connect.msg>
#include <ips/connect.h>

int
getsockopt (int fd, int level, int optname, void *optval, 
            socklen_t * optlen)
{
  fd_tab_t NEARPTR fd_tab;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC &&
	fd >= 0 && fd <= fd_tab->max_fd && fd_tab->bf[fd]) {
      return ips_getOption (fd_tab->bf[fd], level, optname, optval, 
	                    (size_t *) optlen);
    }
  }
  sc_miscErrnoSet (EBADF);
  return -1;
}

int
setsockopt (int fd, int level, int optname, const void *optval,
            socklen_t optlen)
{
  fd_tab_t NEARPTR fd_tab;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC &&
	fd >= 0 && fd <= fd_tab->max_fd && fd_tab->bf[fd]) {
      return ips_setOption (fd_tab->bf[fd], level, optname, optval, 
	                    (size_t ) optlen);
    }
  }
  sc_miscErrnoSet (EBADF);
  return -1;
}

