/*SOC
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
** ID: S02262CL1                                                    **
** +Revision: 2.48 +                                                **
** +Date: 2006/03/16 16:14:58 +                                     **
** Contains connection methode independant to whatever protocol.    **
**********************************************************************
EOC*/

#ifndef _IPS_CONNECT_H_
#define _IPS_CONNECT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ips/addr.h>
#include <ips/connect.msg>

#include <sdd/sdd.msg>

  /**
   * Get a protocol descriptor.
   * \param[in]  family  Protocol family: ipv4, ipv6
   * \param[in]  prot    Protocol: tcp, udp, icmp
   * \param[in]  subProt Sub protocol: currently only for icmp (ips/icmp.h)
   *                     PROTO_ICMP_ECHO, PROTO_ICMP_TIMESTAMP, 
   *                     PROTO_ICMP_INFORMATION, PROTO_ICMP_ADDR_MASK, 
   *                     PROTO_ICMP_MOBILE_REG, PROTO_ICMP_DOMAIN_NAME
   * \return             NULL on error else the protocol descriptor
   */                     
  sdd_obj_t NEARPTR  ips_open (const char *family, const char *prot, int subProt);
  /**
   * Close the procotol and free the protocol descriptor.
   * \param[in]  self    Protocol descriptor
   * \return             -1 on error
   */
  int ips_close (sdd_obj_t NEARPTR  NEARPTR self);
  /**
   * Connect to destination.
   * \param[in]  self    Protocol descriptor
   * \param[in]  dstAddr destination address
   * \param[in]  dstPort destination port
   * \return             -1 on error
   */
  int ips_connect (const sdd_obj_t NEARPTR self,
		   const ips_addr_t * dstAddr, 
		   __u16 dstPort);
  /**
   * Bind to source.
   * \param[in]  self    Protocol descriptor
   * \param[in]  srcAddr source address
   * \param[in]  srcPort source port
   * \return             -1 on error
   */
  int ips_bind (const sdd_obj_t NEARPTR self, const ips_addr_t * srcAddr, __u16 srcPort);
  /**
   * Listen to bounded protocol descriptor.
   * \param[in]  self    Protocol descriptor
   * \param[in]  backlog Number of accepting connections in the background
   * \return             -1 on error
   */
  int ips_listen (const sdd_obj_t NEARPTR  self, int backlog);
  /**
   * Accept a connection from a bounded protocol descriptor.
   * \param[in]  self    Protocol descriptor
   * \param[out] dstAddr Destination address
   * \param[out] dstPort Destination port
   * \return             NULL on error else the protocol descriptor for the
   *                     accepted connection.
   */
  sdd_obj_t NEARPTR ips_accept (const sdd_obj_t NEARPTR  self,
				ips_addr_t * dstAddr, 
				__u16 * dstPort);
  /**
   * Allocation of a netbuffer for optimized throughput and the right header.
   * \param[in]  size    Size of data
   * \param[in]  plid    Poolid for memory allocation
   * \param[in]  tmo     Timeoute for memory allocation
   * \return             NULL if timeout else new netbuf
   */
  sdd_netbuf_t NEARPTR ips_alloc (size_t size, sc_poolid_t plid, sc_ticks_t tmo);
  /**
   * Frees up a netbuf.
   * \param[in]  netbuf  Netbuf which shall be freed.
   */
  void ips_free (sdd_netbuf_t NEARPTR  NEARPTR  netbuf);
  /**
   * Sends a netbuf to a connected protocol descriptor.
   * \param[in]  self    Protocol descriptor
   * \param[in]  netbuf  Netbuf which shall be sent.
   */
  void ips_send (const sdd_obj_t NEARPTR self, sdd_netbuf_t NEARPTR NEARPTR netbuf);
  /**
   * Acknowledg a received netbuf. This is done for flow control, so TCP/IP
   * can not overrun an application with netbufs and cause then a out of
   * memory condition.
   * \param[in]  self    Protocol descriptor
   * \param[in]  netbuf  Received netbuf
   */
  void ips_ack (const sdd_obj_t NEARPTR  self, sdd_netbuf_t NEARPTR netbuf);
  /**
   * Address to string translation.
   * \param[in]  addr    Address which shall be translated
   * \param[out] str     String in which the address will be returned
   * \param[in]  len     Max len of the string. Will be cut down.
   * \return             -1 on error else the length of the address string
   */
  int ips_getOption (const sdd_obj_t NEARPTR  self, int level, int optname,
                     void *optval, size_t * optlen);
  /**
   * set a specific option value.
   * \param[in]  self    Protocol descriptor
   * \param[in]  level   Level: SOL_XXX see below
   * \param[in]  optname Option name: SO_XXX see below
   * \param[in]  optval  Option value
   * \param[in]  optlen  Option Lenght
   * \return             -1 on error
   */
  int ips_setOption (const sdd_obj_t NEARPTR  self, int level, int optname,
                     const void *optval, size_t optlen);
  /**
   * Acknowledg a received netbuf.
   * \param[in]  self    Protocol descriptor
   * \param[in]  netbuf  Received netbuf
   */
  int ips_getLocalAddr (const sdd_netbuf_t NEARPTR netbuf,
			ips_addr_t *local, __u16 *port);
  /**
   * Get the peer address and port.
   * \param[in]  netbuf  Netbuf from which the address shall be extracted
   * \param[out] local   Local address
   * \param[out] port    Local port
   * \return             -1 on error
   */
  int ips_getPeerAddr (const sdd_netbuf_t NEARPTR netbuf, ips_addr_t *peer, __u16 *port);
  
/* Supported address families. */
#define AF_UNSPEC	0
#define AF_UNIX		1	/* Unix domain sockets          */
#define AF_LOCAL	1	/* POSIX name for AF_UNIX       */
#define AF_INET		2	/* Internet IP Protocol         */
#define AF_AX25		3	/* Amateur Radio AX.25          */
#define AF_IPX		4	/* Novell IPX                   */
#define AF_APPLETALK	5	/* AppleTalk DDP                */
#define AF_NETROM	6	/* Amateur Radio NET/ROM        */
#define AF_BRIDGE	7	/* Multiprotocol bridge         */
#define AF_ATMPVC	8	/* ATM PVCs                     */
#define AF_X25		9	/* Reserved for X.25 project    */
#define AF_INET6	10	/* IP version 6                 */
#define AF_ROSE		11	/* Amateur Radio X.25 PLP       */
#define AF_DECnet	12	/* Reserved for DECnet project  */
#define AF_NETBEUI	13	/* Reserved for 802.2LLC project */
#define AF_SECURITY	14	/* Security callback pseudo AF */
#define AF_KEY		15	/* PF_KEY key management API */
#define AF_NETLINK	16
#define AF_ROUTE	AF_NETLINK	/* Alias to emulate 4.4BSD */
#define AF_PACKET	17	/* Packet family                */
#define AF_ASH		18	/* Ash                          */
#define AF_ECONET	19	/* Acorn Econet                 */
#define AF_ATMSVC	20	/* ATM SVCs                     */
#define AF_SNA		22	/* Linux SNA Project (nutters!) */
#define AF_IRDA		23	/* IRDA sockets                 */
#define AF_PPPOX	24	/* PPPoX sockets                */
#define AF_WANPIPE	25	/* Wanpipe API Sockets */
#define AF_BLUETOOTH	31	/* Bluetooth sockets            */
#define AF_MAX		32	/* For now.. */

/* Protocol families, same as address families. */
#define PF_UNSPEC	AF_UNSPEC
#define PF_UNIX		AF_UNIX
#define PF_LOCAL	AF_LOCAL
#define PF_INET		AF_INET
#define PF_AX25		AF_AX25
#define PF_IPX		AF_IPX
#define PF_APPLETALK	AF_APPLETALK
#define	PF_NETROM	AF_NETROM
#define PF_BRIDGE	AF_BRIDGE
#define PF_ATMPVC	AF_ATMPVC
#define PF_X25		AF_X25
#define PF_INET6	AF_INET6
#define PF_ROSE		AF_ROSE
#define PF_DECnet	AF_DECnet
#define PF_NETBEUI	AF_NETBEUI
#define PF_SECURITY	AF_SECURITY
#define PF_KEY		AF_KEY
#define PF_NETLINK	AF_NETLINK
#define PF_ROUTE	AF_ROUTE
#define PF_PACKET	AF_PACKET
#define PF_ASH		AF_ASH
#define PF_ECONET	AF_ECONET
#define PF_ATMSVC	AF_ATMSVC
#define PF_SNA		AF_SNA
#define PF_IRDA		AF_IRDA
#define PF_PPPOX	AF_PPPOX
#define PF_WANPIPE	AF_WANPIPE
#define PF_BLUETOOTH	AF_BLUETOOTH
#define PF_MAX		AF_MAX

/* Setsockoptions(2) level. Thanks to BSD these must match IPPROTO_xxx */
#define SOL_IP		0
#define SOL_ICMP	1
#define SOL_TCP		6
#define SOL_UDP		17
#define SOL_IPV6	41
#define SOL_ICMPV6	58
#define SOL_RAW		255
#define SOL_IPX		256
#define SOL_AX25	257
#define SOL_ATALK	258
#define SOL_NETROM	259
#define SOL_ROSE	260
#define SOL_DECNET	261
#define	SOL_X25		262
#define SOL_PACKET	263
#define SOL_ATM		264	/* ATM layer (cell level) */
#define SOL_AAL		265	/* ATM Adaption Layer (packet level) */
#define SOL_IRDA        266

/* For setsockoptions(2) */
#define SOL_SOCKET	1

#define SO_DEBUG	1
#define SO_REUSEADDR	2
#define SO_TYPE		3
#define SO_ERROR	4
#define SO_DONTROUTE	5
#define SO_BROADCAST	6
#define SO_SNDBUF	7
#define SO_RCVBUF	8
#define SO_KEEPALIVE	9
#define SO_OOBINLINE	10
#define SO_NO_CHECK	11
#define SO_PRIORITY	12
#define SO_LINGER	13
#define SO_BSDCOMPAT	14
#define SO_REUSEPORT	15
#define SO_PASSCRED	16
#define SO_PEERCRED	17
#define SO_RCVLOWAT	18
#define SO_SNDLOWAT	19
#define SO_RCVTIMEO	20
#define SO_SNDTIMEO	21

/* Security levels - as per NRL IPv6 - don't actually do anything */
#define SO_SECURITY_AUTHENTICATION		22
#define SO_SECURITY_ENCRYPTION_TRANSPORT	23
#define SO_SECURITY_ENCRYPTION_NETWORK		24

#define SO_BINDTODEVICE	25

/* Socket filtering */
#define SO_ATTACH_FILTER        26
#define SO_DETACH_FILTER        27

#define SO_PEERNAME		28
#define SO_TIMESTAMP		29
#define SCM_TIMESTAMP		SO_TIMESTAMP

#define SO_ACCEPTCONN		30

/* some sciopta specific */
#define SO_SC_ASYNC		255
#define SO_SC_RET_ACK		256
#define SO_SC_RET_BUF		257

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: connect.h,v $
** Revision 2.48  2006/03/16 16:14:58  bastian
** added const qualifier
**
** Revision 2.47  2004/10/07 07:37:42  bastian
** fixed NEAR * to NEARPTR
**
** Revision 2.46  2004/10/04 12:27:27  bastian
** started to change for m16c
**
** Revision 2.45  2004/07/08 09:46:15  christian
** Bugfix documentation of ips_ack.
**
** Revision 2.44  2004/06/01 12:55:29  christian
** Do string to binary translation family specific.
**
** Revision 2.43  2004/05/28 05:40:38  christian
** ips_ack is free: ips_netbufAck --> ips_ack
**
** Revision 2.42  2004/05/26 06:05:23  christian
** Header files documented and spell checks.
**
** Revision 2.41  2004/05/17 10:48:30  christian
** sdd_netSend --> ips_send.
**
** Revision 2.40  2004/05/12 09:12:17  christian
** Moved all defines used in ips and posix api to ips, because posix does
** build up on the ips/sdd api!
**
** Revision 2.39  2004/05/04 13:47:07  christian
** Removed swq replacement, have to do this in an other way.
**
** Revision 2.38  2004/02/25 15:08:24  christian
** Namespace pollution.
**
** Revision 2.37  2004/02/05 14:41:31  christian
** Tftp is now compiling, but not running.
** Have not removed _all_ posix stuff, will do this in the next revision.
**
** Revision 2.36  2004/02/05 06:47:56  christian
** Splitted up ips api functions to have lesser code size after linking.
**
** Revision 2.35  2004/02/02 12:09:01  christian
** dhcp and bootp are working now with new api.
** resolver fixed a memory leak.
** connect add a addr2Str and str2Addr function.
**
** Revision 2.34  2004/01/27 14:33:53  christian
** Tcp uses now sc_miscChksumContd.
** Tcp does now correctly handle the maxConn (max number of connections).
** Did wrongly renamed struct member chksum to sc_miscChksum :-((
**
** Revision 2.33  2004/01/16 08:16:26  christian
** Moves all net stuff into sdd module.
**
** Revision 2.32  2003/11/19 10:56:28  christian
** Changed netBuffer to netbuf also for the netbuf functions.
**
** Revision 2.31  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 2.30  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 2.29  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 2.28  2003/07/30 05:59:05  christian
** sdd_netBuffer -> sdd_netbuf
**
** Revision 2.27  2003/07/21 09:58:22  christian
** Remove a experimental function.
**
** Revision 2.26  2003/07/21 09:29:26  christian
** Added a ips_socketTest for ips interface and a ips_test for socket
** interface to test if the received buffer is of a specific socket.
** returns a boolean value.
**
** Revision 2.25  2003/06/26 13:39:40  christian
** Bugfixed the connect call. A full qualified connection is now weaker.
**
** Revision 2.24  2003/06/26 11:14:14  christian
** Bugfixes around wait on route.
**
** Revision 2.23  2003/06/16 09:22:06  christian
** Bugfixes.
**
** Revision 2.22  2003/06/16 07:44:52  christian
** Bugfixed many stuff around device, router and arp handling.
**
** Revision 2.21  2003/03/18 15:04:25  christian
** Cleaned.
**
** Revision 2.20  2003/03/05 16:07:38  christian
** TCP configurable no of connections and packet size.
**
** Revision 2.19  2003/03/04 15:26:23  christian
** Added flow controlling primitives.
**
** Revision 2.18  2002/11/01 12:50:13  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 2.17  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 2.16  2002/10/24 14:40:30  christian
** Added tcp socket options.
**
** Revision 2.15  2002/10/18 15:28:24  christian
** Cleaner implementation of connect, bind, listen, accept.
**
** Revision 2.14  2002/10/17 14:41:13  christian
** intermediate.
**
** Revision 2.13  2002/10/09 11:58:00  christian
** Added a possiblity to set/get socket options.
**
** Revision 2.12  2002/10/02 14:47:34  christian
** Head toward the POSIX sockets functions.
**
** Revision 2.11  2002/09/19 15:17:08  christian
** Connect message definition for various protocols
**
**
*/
