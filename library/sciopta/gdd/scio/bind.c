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
** ID: S03070CL32                                                   **
** +Revision: 1.15 +                                                 **
** +Date: 2006/05/24 10:16:39 +                                     **
** bind                                                             **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <scio.h>
#include <ips/addr.h>
#include <ips/connect.h>
#include <sdd/sdd.h>

int
bind (int sd, struct sockaddr *addr, socklen_t addrlen)
{
  fd_tab_t NEARPTR fd_tab;
  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC &&
	sd >= 0 && sd <= fd_tab->max_fd && fd_tab->bf[sd]) {
      if (addr && addr->sa_family == AF_INET && addrlen == 16) {
        ips_addr_t ip;
	struct sockaddr_in *s_in;
	s_in = (struct sockaddr_in *) addr;
	if (s_in->sin_addr.s_addr) {
	  ip.len = 4;
	  memcpy (&ip.addr, &s_in->sin_addr.s_addr, 4);
	}
	else {
	  ip.len = 0;
	}
	return ips_bind (fd_tab->bf[sd], &ip, s_in->sin_port);
      }
      else {
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

