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
** ID: S01332CL5                                                    **
** +Revision: 2.21 +                                                 **
** +Date: 2008/01/24 00:20:34 +                                     **
** Base for general signals for various general stuffs              ** 
**********************************************************************
EOC*/

/** \file
*/

#ifndef _IPS_BASE_MSG_
#define _IPS_BASE_MSG_

#include <ossys/defines.h>

#define IPV4_MSG_BASE		(IPS_MSG_BASE + 0)
#define IPV4_ROUTE_MSG_BASE	(IPS_MSG_BASE + 0x20)
#define IPV4_ARP_MSG_BASE	(IPS_MSG_BASE + 0x40)
#define IPS_CONNECT_MSG_BASE	(IPS_MSG_BASE + 0x60)
#define TCP_MSG_BASE		(IPS_MSG_BASE + 0x80)
#define IPS_CHAINS_MSG_BASE	(IPS_MSG_BASE + 0xa0)
#define PROTECTOR_MSG_BASE	(IPS_MSG_BASE + 0xc0)
#define IPS_TEST_MSG_BASE	(IPS_MSG_BASE + 0xe0)
#define IGMP_MSG_BASE	        (IPS_MSG_BASE + 0xf0)

#endif

