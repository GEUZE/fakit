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
** ID: S03175CL2                                                    **
** +Revision: 1.1 +                                                **
** +Date: 2004/11/18 15:03:49 +                                     **
** BOOTP definition                                                 **
**********************************************************************
EOC*/

#ifndef _IPS_DHCP_H
#define _IPS_DHCP_H	1

#ifdef __cplusplus
extern "C" {
#endif

#include <bootp/bootp.h>
#include <ips/device.msg>

#define DHCP_DISCOVER	1
#define DHCP_OFFER	2
#define DHCP_REQUEST	3
#define DHCP_DECLINE	4
#define DHCP_ACK	5
#define DHCP_NAK	6
#define DHCP_RELEASE	7
#define DHCP_INFORM	8

#ifdef __cplusplus
}
#endif
#endif

