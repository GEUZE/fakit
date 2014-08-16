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
** ID: S03070CL17                                                   **
** +Revision: 1.20 +                                                **
** +Date: 2006/05/24 10:16:39 +                                     **
** ioctl                                                            **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <scio.h>
#include <errno.h>
#include <sdd/sdd.h>

int 
ioctl(int fd, unsigned int cmd, ...)
{
  fd_tab_t NEARPTR fd_tab;
  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC &&	
	fd >= 0 && fd <= fd_tab->max_fd && fd_tab->bf[fd]) 
    {
      va_list arg;
      unsigned long n;

      va_start (arg, cmd);
      n = va_arg (arg, unsigned long);
      va_end (arg);

      return sdd_devIoctl (fd_tab->bf[fd], cmd, n);
    }
    else {
      sc_miscErrnoSet (EBADF);
	return -1;
    }
  }
  return -1;
}

