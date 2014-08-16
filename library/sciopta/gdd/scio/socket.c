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
** ID: S03070CL39                                                   **
** +Revision: 1.47 +                                                 **
** +Date: 2008/01/16 13:02:00 +                                     **
** socket getsockname getpeername                                   **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <scio.msg>
#include <ossys/libsc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <scio.h>
#include <ips/ipv4.h>
#include <ips/connect.h>
#include <ips/connect.msg>
#include <sdd/sdd.h>

#if defined __ARMCC_VERSION
/* suppress: assignment in condition warning */
#pragma diag_suppress 1293
#endif

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_obj_t object;
  sdd_devOpen_t open;
  sdd_devRead_t read;
  sdd_devWrite_t write;
  sdd_devClose_t close;
  sdd_netbuf_t netbuf;
  ips_connect_t connect;
  ips_bind_t bind;
  ips_option_t option;
  ips_ack_t ack;
  scio_send_t send;
  scio_recv_t recv;
  /* kernel messages */
  sc_procPathGetMsgReply_t nameGet;
};

SC_PROCESS (SCP_socket)
{
  static const sc_msgid_t selsocket[2] = { SCIO_SOCKET, 0 };
  static const sc_msgid_t selack[2] = { IPS_ACK, 0 };
  static const sc_msgid_t selrecv[2] = { SDD_NET_RECEIVE, 0 };
  sc_msg_t msg;
  sc_msg_t ack;
  sdd_netbuf_t NEARPTR netbuf;
#ifdef NC30
  void FARPTR buf;
#else
  void *buf;
#endif
  sc_pid_t to;
  sdd_obj_t NEARPTR protocol = NEAR_NULL;
  int opt;

  /* local socket options */
  sc_ticks_t tmo = SC_ENDLESS_TMO;
  /* global socket options */
  int setAsync = 0;
  int ref = 1;
  int acked = 1;
  sdd_netbuf_t NEARPTR recvdNetbuf = NEAR_NULL;

  /* wait one protocol descriptor (socket) */
  msg = sc_msgRx (SC_ENDLESS_TMO,  selsocket, SC_MSGRX_MSGID);
  to = sc_msgSndGet (&msg);

  /* allocation to hold the protocol description */
  protocol =
    (sdd_obj_t NEARPTR) sc_msgAlloc (sizeof (sdd_obj_t), 0,
                                     sc_msgPoolIdGet (&msg), SC_NO_TMO);

  /* check if we have enough memory */
  if (!protocol) {
    msg->base.error = ENOMEM;
    goto error;
  }

  /* copy the protocol descriptor */
  memcpy (protocol, msg, sizeof (sdd_obj_t));

  /* set me as proxy protocol in between */
  msg->object.sender = msg->object.receiver = msg->object.controller =
    sc_procIdGet (NULL, 0);

  /* and reply */
  ++msg->base.id;
  msg->base.handle = NEAR_NULL;
  sc_msgTx (&msg, to, 0);

  /* main message loop */
  for (;;) {
    msg = sc_msgRx (SC_ENDLESS_TMO, SC_MSGRX_ALL, SC_MSGRX_MSGID);
    to = sc_msgSndGet (&msg);
    switch (msg->id) {

    case SDD_NET_SEND:
      msg->base.handle = protocol->base.handle;
      sc_msgTxAlias (&msg, protocol->sender, 0, to);
      break;

    case SCIO_RECV:
      if (msg->send.flags == MSG_DONTWAIT) {
        tmo = SC_NO_TMO;
      }

      /* fall thru ... */
    case SDD_DEV_READ:
      if (!setAsync) {
        /* set socket option "async" and "wait on acks" */
        opt = 1;
        if (ips_setOption (protocol,
                           SOL_SOCKET, SO_SC_ASYNC, &opt, sizeof (char)) == -1
            || ips_setOption (protocol,
                              SOL_SOCKET, SO_SC_RET_ACK, &opt,
                              sizeof (char)) == -1) {
          msg->base.error = EIO;
          goto readend;
        }
        setAsync = 1;
      }
      if (msg->read.outlineBuf) {
        buf = msg->read.outlineBuf;
      }
      else {
        buf = msg->read.inlineBuf;
      }
      if (!recvdNetbuf) {
        recvdNetbuf =
          (sdd_netbuf_t NEARPTR) sc_msgRx (tmo,  selrecv,
                                           SC_MSGRX_MSGID);
        if (recvdNetbuf) {
          ips_ack (protocol, recvdNetbuf);
        }
      }
      if (recvdNetbuf && msg->id == SCIO_RECV) {
        ips_getPeerAddr (recvdNetbuf, &msg->recv.fromAddr,
                         &msg->recv.fromPort);
      }
      if (recvdNetbuf && 
	  (ssize_t)SDD_NET_DATA_SIZE (recvdNetbuf) < msg->read.size) 
      {
        memcpy (buf, SDD_NET_DATA (recvdNetbuf),
                SDD_NET_DATA_SIZE (recvdNetbuf));
        msg->read.size = SDD_NET_DATA_SIZE (recvdNetbuf);
        sdd_netbufFree (&recvdNetbuf);
      }
      else if (recvdNetbuf) {
        memcpy (buf, SDD_NET_DATA (recvdNetbuf), msg->read.size);
        sdd_netbufPull (recvdNetbuf, msg->read.size);
      }
      else {
        msg->read.size = 0;
      }
      if (msg->read.size == 0) {
        msg->base.error = EAGAIN;
      }
      else {
        msg->read.curpos = msg->read.size;
      }
    readend:
      tmo = SC_ENDLESS_TMO;
      ++msg->id;
      sc_msgTx (&msg, to, 0);
      break;

    case SCIO_SEND:
      if (msg->send.flags == MSG_DONTWAIT) {
        tmo = SC_NO_TMO;
      }

      /* fall thru ... */
    case SDD_DEV_WRITE:
      if (!setAsync) {
        /* set socket option "async" and "wait on acks" */
        opt = 1;
        if (ips_setOption
            (protocol, SOL_SOCKET, SO_SC_ASYNC, &opt, sizeof (char)) == -1
            || ips_setOption (protocol, SOL_SOCKET, SO_SC_RET_ACK, &opt,
                              sizeof (char)) == -1) {
          msg->base.error = EIO;
          goto writeend;
        }
        setAsync = 1;
      }
      if (msg->write.outlineBuf) {
#ifdef NC30
        buf = msg->write.outlineBuf;
#else
        buf = (void *)msg->write.outlineBuf;
#endif
      }
      else {
        buf = msg->write.inlineBuf;
      }
      if (acked) {
        netbuf =
          ips_alloc (msg->write.size, sc_msgPoolIdGet (&msg), SC_NO_TMO);
        if (!netbuf) {
          msg->base.error = ENOMEM;
          goto writeend;
        }
        memcpy (SDD_NET_DATA (netbuf), buf, msg->write.size);
        ips_send (protocol, &netbuf);
        acked = 0;
        /* wait on ack (perhaps try to collect more writes into netbuf?) */
        ack = sc_msgRx (tmo,  selack, SC_MSGRX_MSGID);
        if (ack) {
          sc_msgFree (&ack);
          acked = 1;
        }
      }
      else {
        msg->base.error = EAGAIN;
      }
    writeend:
      tmo = SC_ENDLESS_TMO;
      ++msg->id;
      sc_msgTx (&msg, to, 0);
      break;

    case SCIO_SOCKET:
      memcpy (msg, protocol, sizeof (sdd_obj_t));
      msg->id = SCIO_SOCKET_REPLY;
      sc_msgTx (&msg, to, 0);
      break;

    case IPS_BIND:
      msg->base.handle = protocol->base.handle;
      sc_msgTxAlias (&msg, protocol->controller, 0, to);
      break;

    case IPS_CONNECT:
      msg->base.handle = protocol->base.handle;
      sc_msgTxAlias (&msg, protocol->controller, 0, to);
      break;

    case IPS_LISTEN:
      msg->base.handle = protocol->base.handle;
      sc_msgTxAlias (&msg, protocol->controller, 0, to);
      break;

    case IPS_ACCEPT:
      msg->base.handle = protocol->base.handle;
      sc_msgTxAlias (&msg, protocol->controller, 0, to);
      break;

    case IPS_SET_OPTION:
      msg->base.handle = protocol->base.handle;
      sc_msgTxAlias (&msg, protocol->controller, 0, to);
      break;

    case IPS_GET_OPTION:
      msg->base.handle = protocol->base.handle;
      sc_msgTxAlias (&msg, protocol->controller, 0, to);
      break;

    case IPS_ACK:
      if (msg->ack.base.handle == protocol->base.handle) {
        acked = 1;
      }
      else {
        msg->base.handle = protocol->base.handle;
        sc_msgTxAlias (&msg, protocol->controller, 0, to);
      }
      break;

    case SDD_DEV_CLOSE:
      msg->base.handle = protocol->base.handle;
      sc_msgTxAlias (&msg, protocol->controller, 0, to);
      --ref;
      if (!ref) {
        goto term;
      }
      break;

    case SDD_OBJ_DUP:
      msg->base.handle = protocol->base.handle;
      sc_msgTxAlias (&msg, protocol->controller, 0, to);
      ++ref;
      break;

    case SDD_ERROR:
      /* hmm what should I do with this? */
      sc_msgFree (&msg);
      break;

    case SCIO_DEAD:
      goto term;

    default:
      {
        msg->base.error = EOPNOTSUPP;
        ++msg->id;
        sc_msgTx (&msg, to, 0);
      }
      break;
    }
  }

error:
  ++msg->id;
  sc_msgTx (&msg, to, 0);

term:
  sc_procKill (SC_CURRENT_PID, 0);
}

int
socket (int domain, int type, int proto)
{
  int tmo;
  fd_tab_t NEARPTR fd_tab;
  sc_pid_t socket;
  int fd;

  static const sc_msgid_t select[2] = { SCIO_SOCKET_REPLY, 0 };
  sdd_obj_t NEARPTR protocol = NEAR_NULL;
  const char *protstr = NULL;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC) {
      if (domain != PF_INET) {
        sc_miscErrnoSet (EINVAL);
        return -1;
      }
      if (type == SOCK_DGRAM) {
        protstr = "udp";
      }
      else if (type == SOCK_STREAM) {
        protstr = "tcp";
      }
      else if (type == SOCK_ICMP) {
        protstr = "icmp";
      }
      else {
        sc_miscErrnoSet (EPROTONOSUPPORT);
        return -1;
      }
      for (tmo = 100;
           tmo < 10000 && !(protocol = ipv4_getProtocol (protstr));
           tmo *= 2) {
        sc_sleep ((sc_ticks_t) sc_tickMs2Tick (tmo));
      }
      if (!protocol) {
        sc_miscErrnoSet (EPROTONOSUPPORT);
        return -1;
      }
      if (SDD_IS_A (protocol, SDD_DEV_TYPE)) {
        for (fd = 0; fd < fd_tab->max_fd && fd_tab->bf[fd]; ++fd) {
          /* empty */
        }
        if (fd != fd_tab->max_fd) {
          /* start a socket process */
          if ((socket = sc_procTmpCreate ("socket", SCP_socket, 512,
                                          SC_DEFAULT_POOL)) ==
              SC_ILLEGAL_PID) {
            sc_miscErrnoSet (ENOMEM);
            return -1;
          }
          if ((sdd_netOpen (protocol, proto, SC_CURRENT_PID)) != -1) {
            /* send protocol to socket */
            protocol->base.id = SCIO_SOCKET;
            sc_msgTx ((sc_msgptr_t) & protocol, socket, 0);
            protocol =
              (sdd_obj_t NEARPTR) sc_msgRx (SC_ENDLESS_TMO,  select,
                                            SC_MSGRX_MSGID);
            fd_tab->bf[fd] = protocol;
            return fd;
          }
          else {
            return -1;
          }
        }
        else {
          sc_miscErrnoSet (EMFILE);
          return -1;
        }
      }
      else {
        sc_miscErrnoSet (EINVAL);
        return -1;
      }
    }
  }
  sc_miscErrnoSet (EMFILE);
  return -1;
}

int
getsockname (int fd, struct sockaddr *addr, socklen_t * len)
{
  fd_tab_t NEARPTR fd_tab;
  (void) addr;
  (void) len;
  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC &&
        fd <= fd_tab->max_fd && fd_tab->bf[fd]) {
    }
  }
  sc_miscErrnoSet (EBADF);
  return -1;
}

int
getpeername (int fd, struct sockaddr *addr, socklen_t * len)
{
  fd_tab_t NEARPTR fd_tab;
  (void) addr;
  (void) len;
  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC &&
        fd <= fd_tab->max_fd && fd_tab->bf[fd]) {
    }
  }
  sc_miscErrnoSet (EBADF);
  return -1;
}

