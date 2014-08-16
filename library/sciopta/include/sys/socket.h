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
** ID: S03070CL55                                                   **
** +Revision: 1.24 +                                                 **
** +Date: 2004/06/29 12:10:33 +                                     **
** socket functions                                                 **
**********************************************************************
EOC*/

#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H 	1

#ifdef __cplusplus
extern "C" {
#endif

#include <sdd/sdd.msg>
#include <ossys/types.h>
#include <sys/unistd.h>
#include <ips/connect.h>
#include <ips/icmp.h>

  typedef unsigned short sa_family_t;

  struct sockaddr {
    sa_family_t sa_family;
    char sa_data[14];
  };

  int socket (int domain, int type, int protocol);
  int socketpair (int domain, int type, int protocol, int fds[2]);
  int bind (int fd, struct sockaddr *addr, socklen_t len);
  int getsockname (int fd, struct sockaddr *addr, socklen_t * len);
  int connect (int fd, const struct sockaddr *addr, socklen_t len);
  int getpeername (int fd, struct sockaddr *addr, socklen_t * len);
  ssize_t send (int fd, const void *buf, size_t n, int flags);
  ssize_t recv (int fd, void *buf, size_t n, int flags);
  ssize_t sendto (int fd, const void *buf, size_t n, int flags,
                  const struct sockaddr *addr, socklen_t len);
  ssize_t recvfrom (int fd, void *buf, size_t n, int flags,
                    struct sockaddr *addr, socklen_t * len);
  int getsockopt (int fd, int level, int optname, void *optval,
                  socklen_t * optlen);
  int setsockopt (int fd, int level, int optname, const void *optval,
                  socklen_t optlen);
  int listen (int fd, int n);
  int accept (int fd, struct sockaddr *addr, socklen_t * len);

/* Maximum queue length specifiable by listen.  */
#define SOMAXCONN	128

/* Socket types. */
#define SOCK_STREAM	1	/* stream (connection) socket   */
#define SOCK_DGRAM	2	/* datagram (conn.less) socket  */
#define SOCK_RAW	3	/* raw socket                   */
#define SOCK_RDM	4	/* reliably-delivered message   */
#define SOCK_SEQPACKET	5	/* sequential packet socket     */
#define SOCK_PACKET	10	/* linux specific way of        */
  /* getting packets at the dev   */
  /* level.  For writing rarp and */
  /* other similar things on the  */
  /* user level.                  */
#define SOCK_ICMP	11	/* sciopta specific way of      */
  /* using icmp sockets           */
  /* the protocol number is then  */
  
/* Flags we can use with send/ and recv. 
   Added those for 1003.1g not all are supported yet
 */
#define MSG_OOB		1
#define MSG_PEEK	2
#define MSG_DONTROUTE	4
#define MSG_TRYHARD     4	/* Synonym for MSG_DONTROUTE for DECnet */
#define MSG_CTRUNC	8
#define MSG_PROBE	0x10	/* Do not send. Only probe path f.e. for MTU */
#define MSG_TRUNC	0x20
#define MSG_DONTWAIT	0x40	/* Nonblocking io                */
#define MSG_EOR         0x80	/* End of record */
#define MSG_WAITALL	0x100	/* Wait for a full request */
#define MSG_FIN         0x200
#define MSG_SYN		0x400
#define MSG_CONFIRM	0x800	/* Confirm path validity */
#define MSG_RST		0x1000
#define MSG_ERRQUEUE	0x2000	/* Fetch message from error queue */
#define MSG_NOSIGNAL	0x4000	/* Do not generate SIGPIPE */
#define MSG_MORE	0x8000	/* Sender will send more */
#define MSG_EOF         MSG_FIN

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: socket.h,v $
** Revision 1.24  2004/06/29 12:10:33  bastian
** fixed  unistd.h include
**
** Revision 1.23  2004/06/22 13:25:57  christian
** Bugfix wrong typings.
**
** Revision 1.22  2004/05/17 10:51:07  christian
** sdd_netSend --> ips_send.
**
** Revision 1.21  2004/05/12 09:12:17  christian
** Moved all defines used in ips and posix api to ips, because posix does
** build up on the ips/sdd api!
**
** Revision 1.20  2004/04/01 11:05:13  christian
** Remove this namespacing macro, it is not working properly.
**
** Revision 1.19  2004/03/11 10:18:41  christian
** Only do rename POSIX calls to scio_POSIX if it is LINUX_HOSTED.
**
** Revision 1.18  2004/02/25 15:07:47  christian
** To be able to use native POSIX calls in the softkernel, we have to
** rename all POSIX calls by appending a prefix scio_ before every call.
** With defines the name is switch back to posix namespace. If you want to use
** nativ POSIX call of your host you must not include the sciopta posix stuff.
**
** Revision 1.17  2004/01/27 14:44:35  christian
** Bugfixed a wrong naming.
**
** Revision 1.16  2004/01/27 13:31:24  christian
** Changed namings of some libsc functions to sc_miscXxxYyy
**
** Revision 1.15  2003/07/30 05:59:04  christian
** sdd_netBuffer -> sdd_netbuf
**
** Revision 1.14  2003/07/21 09:58:01  christian
** Add get socket to get a socket descriptor of a received netbuffer.
**
** Revision 1.13  2003/07/21 09:29:26  christian
** Added a ips_socketTest for ips interface and a ips_test for socket
** interface to test if the received buffer is of a specific socket.
** returns a boolean value.
**
** Revision 1.12  2003/07/18 12:40:55  christian
** Removed the not ips_free (it is reuundant and dangerous in use).
**
** Revision 1.11  2003/06/24 13:25:29  christian
** Added icmp socket (this is a sciopta spezific way to do this).
**
** Revision 1.10  2003/06/23 08:27:29  christian
** Can now get acks or buffer back if requestet with setsockopt.
**
** Revision 1.9  2003/04/22 09:10:15  christian
** Add the get peer/socket name of a received sdd_netbuf_t message.
**
** Revision 1.8  2003/03/11 14:58:30  bastian
** added header
**
** Revision 1.7  2003/03/04 15:25:35  christian
** Added flow controlling primitivs
**
** Revision 1.6  2002/11/01 12:50:13  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 1.5  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 1.4  2002/10/25 14:02:30  christian
** Replaced ttys with serial (ttys is a line discpline based on serial!!)
**
** Revision 1.3  2002/10/07 15:32:33  christian
** Add include/arpa/inet.h include/netinet/in.h and src/inet.c
**
** Revision 1.2  2002/10/04 12:51:26  christian
** Add two sciopta ips specific system calls to sys/socket.h
**
** Revision 1.1  2002/10/02 14:48:38  christian
** Head toward the POSIX sockets functions.
**
**
**/
