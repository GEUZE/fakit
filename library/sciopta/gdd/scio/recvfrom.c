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
** ID: S04147CL2                                                    **
** +Revision: 1.9 +                                                 **
** +Date: 2006/05/24 10:16:39 +                                     **
** recvfrom                                                         **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <scio.h>
#include <scio.msg>
#include <ips/addr.h>
#include <ips/connect.h>
#include <sdd/sdd.h>

union sc_msg {
  sc_msgid_t id;
  scio_recv_t recv;
};

ssize_t
recvfrom (int sd, void *buf, size_t n, int flags,
          struct sockaddr *from, socklen_t * fromlen)
{
  sc_msg_t msg;
  ssize_t ret;
  fd_tab_t NEARPTR fd_tab;

  static const sc_msgid_t select[3] = {
    SCIO_RECV_REPLY, SDD_ERROR, 0
  };

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC &&
        sd >= 0 && sd <= fd_tab->max_fd && fd_tab->bf[sd]) {
      if (*fromlen < sizeof (struct sockaddr)) {
        sc_miscErrnoSet (EINVAL);
        return -1;
      }
      msg =
        sc_msgAlloc (sizeof (scio_recv_t), SCIO_RECV,
                     sc_msgPoolIdGet ((sc_msgptr_t) &fd_tab->bf[sd]),
                     SC_FATAL_IF_TMO);
      msg->recv.read.base.error = 0;
      msg->recv.read.base.handle = fd_tab->bf[sd]->base.handle;
      msg->recv.read.size = n;
      msg->recv.read.curpos = 0;
      msg->recv.read.outlineBuf = (__u8 *)buf;
      msg->recv.flags = flags;
      sc_msgTx (&msg, fd_tab->bf[sd]->receiver, 0);
      msg = sc_msgRx (SC_ENDLESS_TMO,  select, SC_MSGRX_MSGID);
      if (msg->recv.read.base.error) {
        sc_miscErrnoSet (msg->recv.read.base.error);
        sc_msgFree (&msg);
        return -1;
      }
      else {
        if (msg->recv.fromAddr.len == 4) {
          memcpy (&((struct sockaddr_in *) from)->sin_addr.s_addr,
                  msg->recv.fromAddr.addr, 4);
          ((struct sockaddr_in *) from)->sin_port = msg->recv.fromPort;
          ((struct sockaddr_in *) from)->sin_family = AF_INET;
          *fromlen = sizeof (struct sockaddr_in);
        }
        else {
          sc_msgFree (&msg);
          sc_miscErrnoSet (EINVAL);
          return -1;
        }
        ret = msg->recv.read.size;
        sc_msgFree (&msg);
        return ret;
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

