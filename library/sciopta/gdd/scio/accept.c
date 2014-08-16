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
** ID: S03070CL42                                                   **
** +Revision: 1.22 +                                                 **
** +Date: 2006/05/24 10:16:39 +                                     **
** accept                                                           **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.msg>
#include <ossys/libsc.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <scio.h>
#include <ips/addr.h>
#include <ips/connect.h>
#include <sdd/sdd.h>

SC_PROCESS (SCP_socket);

int
accept (int sd, struct sockaddr *addr, socklen_t * addrlen)
{
  fd_tab_t NEARPTR fd_tab;
  sdd_obj_t NEARPTR obj;
  ips_addr_t ip;
  struct sockaddr_in *s_in;
  sc_pid_t socket;
  void NEARPTR handle;
  static const sc_msgid_t select[2] = { SCIO_SOCKET_REPLY, 0 };
  int fd = 0;

  (void) addrlen;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC &&
	sd >= 0 && sd <= fd_tab->max_fd && fd_tab->bf[sd]) {
      s_in = (struct sockaddr_in *)  addr;
      obj = ips_accept (fd_tab->bf[sd], &ip, &s_in->sin_port);
      if (!obj) {
	return -1;
      }
      else {

	/* store the handle form accept */
	handle = obj->base.handle;

	/* get scio socket from listener socket */
        obj->base.id = SCIO_SOCKET;
	sc_msgTx ((sc_msgptr_t) &obj, obj->controller, 0);
	obj = 
	  (sdd_obj_t NEARPTR) sc_msgRx (SC_ENDLESS_TMO,  select, 
				  SC_MSGRX_MSGID);

        /* replace the listener handle with the accepted handle */
	obj->base.handle = handle;

        /* start a socket process */
        if ((socket =
             sc_procTmpCreate ("socket", SCP_socket, 512,
                               SC_DEFAULT_POOL)) == SC_ILLEGAL_PID) {
          sc_miscErrnoSet (ENOMEM);
          return -1;
        }
        obj->base.id = SCIO_SOCKET;
	sc_msgTx ((sc_msgptr_t) &obj, socket, 0);
	obj = 
	  (sdd_obj_t NEARPTR) sc_msgRx (SC_ENDLESS_TMO,  select, 
				  SC_MSGRX_MSGID);
	while (fd < fd_tab->max_fd && fd_tab->bf[fd]) {
	  ++fd;
	}
	if (fd != fd_tab->max_fd) {
	  fd_tab->bf[fd] = obj;
	  return fd;
	}
	else {
	  sc_miscErrnoSet (EMFILE);
	  return -1;
	}
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

