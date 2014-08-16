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
** ID: S03070CL61                                                   **
** +Revision: 1.13 +                                                 **
** +Date: 2008/02/14 13:50:51 +                                     **
** TCP/IP constant definitions                                      **
**********************************************************************
EOC*/

#ifndef	_NETINET_IN_H
#define	_NETINET_IN_H	1

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/types.h>
#include <ossys/swap.h>
#include <sys/socket.h>
  
/* Standard well-known ports.  */
  enum {
    IPPORT_ECHO = 7,		/* Echo service.  */
    IPPORT_DISCARD = 9,		/* Discard transmissions service.  */
    IPPORT_SYSTAT = 11,		/* System status service.  */
    IPPORT_DAYTIME = 13,	/* Time of day service.  */
    IPPORT_NETSTAT = 15,	/* Network status service.  */
    IPPORT_FTP = 21,		/* File Transfer Protocol.  */
    IPPORT_TELNET = 23,		/* Telnet protocol.  */
    IPPORT_SMTP = 25,		/* Simple Mail Transfer Protocol.  */
    IPPORT_TIMESERVER = 37,	/* Timeserver service.  */
    IPPORT_NAMESERVER = 42,	/* Host Name Server.  */
    IPPORT_WHOIS = 43,		/* Internet Whois service.  */
    IPPORT_DNSSERVER = 53,	/* Domain Name Server.  */
    IPPORT_MTP = 57,

    IPPORT_TFTP = 69,		/* Trivial File Transfer Protocol.  */
    IPPORT_RJE = 77,
    IPPORT_FINGER = 79,		/* Finger service.  */
    IPPORT_TTYLINK = 87,
    IPPORT_SUPDUP = 95,		/* SUPDUP protocol.  */


    IPPORT_EXECSERVER = 512,	/* execd service.  */
    IPPORT_LOGINSERVER = 513,	/* rlogind service.  */
    IPPORT_CMDSERVER = 514,
    IPPORT_EFSSERVER = 520,

    /* UDP ports.  */
    IPPORT_BIFFUDP = 512,
    IPPORT_WHOSERVER = 513,
    IPPORT_ROUTESERVER = 520,

    /* Ports less than this value are reserved for privileged processes.  */
    IPPORT_RESERVED = 1024,

    /* Ports greater this value are reserved for (non-privileged) servers.  */
    IPPORT_USERRESERVED = 5000
  };


/* Internet address.  */
  typedef __u32 in_addr_t;
  struct in_addr {
    in_addr_t s_addr;
  };

/* Multicast structure */
  typedef struct ip_mreq {
    struct in_addr imr_multiaddr;
    struct in_addr imr_interface;
  } ip_mreq_t;

/* Definitions of the bits in an Internet address integer.

   On subnets, host and network parts are found according to
   the subnet mask, not these masks.  */

#define	IN_CLASSA(a)		((((in_addr_t)(a)) & 0x80000000) == 0)
#define	IN_CLASSA_NET		0xff000000
#define	IN_CLASSA_NSHIFT	24
#define	IN_CLASSA_HOST		(0xffffffff & ~IN_CLASSA_NET)
#define	IN_CLASSA_MAX		128

#define	IN_CLASSB(a)		((((in_addr_t)(a)) & 0xc0000000) == 0x80000000)
#define	IN_CLASSB_NET		0xffff0000
#define	IN_CLASSB_NSHIFT	16
#define	IN_CLASSB_HOST		(0xffffffff & ~IN_CLASSB_NET)
#define	IN_CLASSB_MAX		65536

#define	IN_CLASSC(a)		((((in_addr_t)(a)) & 0xe0000000) == 0xc0000000)
#define	IN_CLASSC_NET		0xffffff00
#define	IN_CLASSC_NSHIFT	8
#define	IN_CLASSC_HOST		(0xffffffff & ~IN_CLASSC_NET)

#define	IN_CLASSD(a)		((((in_addr_t)(a)) & 0xf0000000) == 0xe0000000)
#define	IN_MULTICAST(a)		IN_CLASSD(a)

#define	IN_EXPERIMENTAL(a)	((((in_addr_t)(a)) & 0xe0000000) == 0xe0000000)
#define	IN_BADCLASS(a)		((((in_addr_t)(a)) & 0xf0000000) == 0xf0000000)

/* Address to accept any incoming messages.  */
#define	INADDR_ANY		((in_addr_t) 0x00000000)
/* Address to send to all hosts.  */
#define	INADDR_BROADCAST	((in_addr_t) 0xffffffff)
/* Address indicating an error return.  */
#define	INADDR_NONE		((in_addr_t) 0xffffffff)

/* Network number for local host loopback.  */
#define	IN_LOOPBACKNET		127
/* Address to loopback in software to local host.  */
#ifndef INADDR_LOOPBACK
# define INADDR_LOOPBACK	((in_addr_t) 0x7f000001)	/* Inet 127.0.0.1.  */
#endif

/* Defines for Multicast INADDR.  */
#define INADDR_UNSPEC_GROUP	((in_addr_t) 0xe0000000)	/* 224.0.0.0 */
#define INADDR_ALLHOSTS_GROUP	((in_addr_t) 0xe0000001)	/* 224.0.0.1 */
#define INADDR_ALLRTRS_GROUP    ((in_addr_t) 0xe0000002)	/* 224.0.0.2 */
#define INADDR_MAX_LOCAL_GROUP  ((in_addr_t) 0xe00000ff)	/* 224.0.0.255 */

#define __SOCK_SIZE__	16	/* sizeof(struct sockaddr)      */
  struct sockaddr_in {
    sa_family_t sin_family;	/* Address family               */
    unsigned short int sin_port;	/* Port number                  */
    struct in_addr sin_addr;	/* Internet address             */

    /* Pad to size of `struct sockaddr'. */
    unsigned char __pad[__SOCK_SIZE__ - sizeof (short int) -
			sizeof (unsigned short int) -
			sizeof (struct in_addr)];
  };

#ifdef __cplusplus
}
#endif
#endif

/*
** $Log: in.h,v $
** Revision 1.13  2008/02/14 13:50:51  bastian
** pre-1.9.2 merge
**
** Revision 1.12  2004/02/26 15:02:28  christian
** Remove byteorder/generic.h this was a bad idea.
**
** Revision 1.11  2004/02/26 12:35:09  christian
** Separeted the htons functions (and other swapper will follow) into
** byteorder/generic.h
**
** Revision 1.10  2004/01/21 09:23:12  christian
** cheksum calculation is in libsc now.
**
** Revision 1.9  2004/01/20 14:29:31  bastian
** fix
**
** Revision 1.8  2003/12/16 12:32:17  bastian
** fixed cksum type
**
** Revision 1.7  2003/08/20 08:39:15  christian
** Bugfixed and extended sliding window queue pkt swqueue and removed
** -fomit-frame-pointer from i386 Rules.
**
** Revision 1.6  2003/08/18 14:17:47  christian
** Used arch/endian.h instead of compiler flags LITTLE_/BIG_ENDIAN.
**
** Revision 1.5  2003/08/18 12:54:29  christian
** Removed ips/util.h is now all in netinet/in.h
**
** Revision 1.4  2003/08/05 11:14:18  bastian
** fixed header
**
** Revision 1.3  2003/03/11 14:58:30  bastian
** added header
**
** Revision 1.2  2002/10/16 12:03:43  christian
** Renamed mother to device.
**
** Revision 1.1  2002/10/07 15:32:33  christian
** Add include/arpa/inet.h include/netinet/in.h and src/inet.c
**
**
**/
