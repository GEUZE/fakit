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
** ID: S03070CL62                                                   **
** +Revision: 1.5 +                                                 **
** +Date: 2004/05/12 09:12:17 +                                     **
** TCP socket options                                               **
**********************************************************************
EOC*/

#ifndef _NETINET_TCP_H
#define _NETINET_TCP_H	1

#include <ips/tcp.h>

#endif /* netinet/tcp.h */

/*
** $Log: tcp.h,v $
** Revision 1.5  2004/05/12 09:12:17  christian
** Moved all defines used in ips and posix api to ips, because posix does
** build up on the ips/sdd api!
**
** Revision 1.4  2003/08/28 09:11:51  christian
** Added the possibilty to change the resnd tmo.
** Alloced only 60 Bytes for IP header and 8 Bytes for udp, tcp is anyway not
** send directly.
**
** Revision 1.3  2003/08/05 11:13:43  bastian
** fixed header
**
** Revision 1.2  2003/03/11 14:58:30  bastian
** added header
**
** Revision 1.1  2002/10/24 14:40:12  christian
** Added tcp socket options.
**
**
**/
