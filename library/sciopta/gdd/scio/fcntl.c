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
** ID: S03070CL38                                                   **
** +Revision: 1.23 +                                                 **
** +Date: 2006/05/24 10:16:39 +                                     **
** fcntl                                                            **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <scio.h>
#include <sdd/sdd.h>

int
fcntl (int fd, int cmd, ...)
{
  fd_tab_t NEARPTR fd_tab;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC &&
	fd >= 0 && fd <= fd_tab->max_fd 
	/* do not test if fd_tab->bf[fd] is != NULL */) {
      switch (cmd) {
      case F_SC_SETSDDOBJ:
	{
	  va_list arg;
	  sdd_obj_t NEARPTR n;
	  va_start (arg, cmd);
	  n = va_arg (arg, sdd_obj_t NEARPTR);
	  va_end (arg);
	  fd_tab->bf[fd] = n;
	  return fd;
	}
      case F_SC_GETSDDOBJ:
	{
	  va_list arg;
	  sdd_obj_t NEARPTR NEARPTR n;
	  va_start (arg, cmd);
	  n = va_arg (arg, sdd_obj_t NEARPTR NEARPTR);
	  va_end (arg);
	  *n = fd_tab->bf[fd];
	  return 0;
	}
      case F_DUPFD:
	return dup (fd);
      default:
	sc_miscErrnoSet (EINVAL);
	return -1;
      }
    }
    else {
      sc_miscErrnoSet (EBADF);
      return -1;
    }
  }
  sc_miscErrnoSet (EMFILE);
  return -1;
}

