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
** ID: S03070CL21                                                   **
** +Revision: 1.24 +                                                **
** +Date: 2006/05/24 10:16:39 +                                     **
** read                                                             **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>
#include <errno.h>
#include <unistd.h>
#include <sdd/sdd.h>

ssize_t read (int fd, void *buf, size_t size)
{
  fd_tab_t NEARPTR fd_tab;
  ssize_t ret;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC &&	
	fd >= 0 && fd <= fd_tab->max_fd && fd_tab->bf[fd]) 
    {
      if ( (fd_tab->flags & (SCIO_FLAGS_BACKGROUND | SCIO_FLAGS_SEND_READ))
	   == (SCIO_FLAGS_BACKGROUND | SCIO_FLAGS_SEND_READ) ){
	sc_procStop (SC_CURRENT_PID);
	fd_tab->flags &= (flags_t)~SCIO_FLAGS_BACKGROUND;
      }
      fd_tab->flags |= SCIO_FLAGS_SEND_READ; 
      ret = sdd_devRead(fd_tab->bf[fd], (__u8 *)buf, size);
      fd_tab->flags &= (flags_t)~SCIO_FLAGS_SEND_READ; 
      return ret;
    }
    else {
      sc_miscErrnoSet (EBADF);
      return -1;
    }
  }
  return -1;
}

