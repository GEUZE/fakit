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
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID:                                                              **
** +Revision: 1.8 +                                                 **
** +Date: 2008/01/16 13:02:06 +                                     **
** Description                                                      **
**********************************************************************
EOC*/

#ifndef _IPS_DHCPSERVER_MSG
#define _IPS_DHCPSERVER_MSG 1

#ifdef __cplusplus
extern "C" {
#endif

#include <sdd/sdd.msg>
#include <bootp/bootp.h>

#define DHCPS_ZONE_ADD         (DHCP_MSG_BASE)
#define DHCPS_ZONE_ADD_REPLY   (DHCPS_ZONE_ADD + 1)
#define DHCPS_ZONE_RM          (DHCP_MSG_BASE + 2)
#define DHCPS_ZONE_RM_REPLY    (DHCPS_ZONE_RM + 1)
  typedef struct dhcps_zone_s {
    sc_msgid_t id;
    sc_errcode_t error;
    __u8  subnet[4];
    __u8  netmask[4];
    __u8  router[4];
    __u32 leaseTime;
    __u8  range[2][4];
    __u8  nameServer[3][4];
  } dhcps_zone_t;

#define DHCPS_TMO              (DHCP_MSG_BASE + 4)

#define DHCPS_SESSION          (DHCP_MSG_BASE + 5)
#ifdef __cplusplus
}
#endif

#endif /* _IPS_DHCPSERVER_MSG */
