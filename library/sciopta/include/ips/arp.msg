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
** ID: S03139CL3                                                    **
** +Revision: 1.9 +                                                 **
** +Date: 2004/11/19 13:57:04 +                                     **
** Arp message interface                                            **
**********************************************************************
EOC*/

#ifndef _IPS_IPV4_ARP_MSG_
#define _IPS_IPV4_ARP_MSG_

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/types.h>
#include <ossys/defines.h>
#include <ips/device.h>
#include <ips/base.msg>

#define IPV4_ARP_ADD 	(IPV4_ARP_MSG_BASE)
#define IPV4_ARP_ADD_REPLY	(IPV4_ARP_ADD + 1)
#define IPV4_ARP_RM 	(IPV4_ARP_MSG_BASE + 2)
#define IPV4_ARP_RM_REPLY	(IPV4_ARP_RM + 1)
  typedef struct ipv4_arp_s {
    sc_msgid_t id;
    sc_errcode_t error;
#define ARP_TYPE_PERMANENT		0
#define ARP_TYPE_DYNAMIC 		2
    __u8 state;
    __u8 ipaddr[4];
    __u8 hwaddr[6];
  } ipv4_arp_t;

#define IPV4_ARP_REFRESH	(IPV4_ARP_MSG_BASE + 4)
  typedef struct ipv4_arpRefreshTmo_s {
    sc_msgid_t id;
  } ipv4_arpRefreshTmo_t;

#define IPV4_ARP_TEST_TMO	(IPV4_ARP_MSG_BASE + 5)
#define IPV4_ARP_DATA   	(IPV4_ARP_MSG_BASE + 6)
#define IPV4_ARP_CACHE_ENTRY   	(IPV4_ARP_MSG_BASE + 7)

#ifdef __cplusplus
}
#endif
#endif

/*
** $Log: arp.msg,v $
** Revision 1.9  2004/11/19 13:57:04  christian
** Arp entries can be added now with a state (permanent or dynamic).
**
** Revision 1.8  2004/11/12 15:40:38  bastian
** added new msg-ids
**
** Revision 1.7  2004/05/05 08:56:08  christian
** Rename ips_ipv4Any to ipv4_any and IPS_IPV4_ANY IPV4_ANY.
**
** Revision 1.6  2004/02/11 10:50:40  christian
** Renamed router and arp msg ids to IPV4_ROUTE_XXX and IPV4_ARP_XXX.
**
** Revision 1.5  2003/07/10 07:37:21  christian
** Arp can now do a client request for probing a ip address.
**
** Revision 1.4  2003/06/12 12:26:21  christian
** Temp check in.
**
** Revision 1.3  2003/06/02 14:13:47  christian
** Simpler print on read request in all parts of ipv4.
**
** Revision 1.2  2003/05/30 14:22:30  christian
** Weekend.
**
** Revision 1.1  2003/05/20 14:22:28  christian
** Added arp.
**
** Revision 1.6  2003/03/18 15:04:26  christian
** Cleaned.
**
** Revision 1.5  2002/10/04 12:40:10  christian
** Adjusted ips toward the new scipota specific system call ips_alloc.
**
** Revision 1.4  2002/09/26 15:13:03  christian
** intermediate.
**
** Revision 1.3  2002/09/18 10:02:29  christian
** A check if a received packet is for the receiving host or has to
** be routed.
**
** Revision 1.2  2002/09/16 15:00:10  christian
** Intermediate
**
** Revision 1.1  2002/09/13 14:46:08  christian
** Separate router from ipv4 (Can be custumized)
**
**
**/
