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
** ID: S01332CL13                                                   **
** +Revision: 1.5 +                                                **
** +Date: 2004/05/12 09:12:17 +                                     **
** ips include all in one file                                      **
**********************************************************************
EOC*/

#ifndef _IPS_H_
#define _IPS_H_

#include <arpa/inet.h>
#include <dns/parser.h>
#include <http/printf.h>
#include <http/server.h>
#include <ips/addr.h>
#include <ips/arp.h>
#include <ips/arp.h>
#include <ips/connect.h>
#include <ips/device.h>
#include <ips/hdlc.h>
#include <ips/icmp.h>
#include <ips/ipcp.h>
#include <ips/ipv4.h>
#include <ips/ipv6.h>
#include <ips/lcp.h>
#include <ips/link.h>
#include <ips/pap.h>
#include <ips/ppp.h>
#include <ips/pppconf.h>
#include <ips/router.h>
#include <ips/tcp.h>
#include <ips/udp.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <scio.h>
#include <sdd/sdd.h>
#include <sh/ips.h>
#include <sys/dirent.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/termios.h>
#include <sys/unistd.h>
#include <telnet/telnet.h>
#include <test/ttcp.h>
#include <tftp/tftp.h>

#endif
