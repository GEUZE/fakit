/*SOC -*-c-*-
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
** ID: S07200BS1                                                    **
** +Revision: 1.2.12.1 +                                                 **
** +Date: 2009/02/17 09:23:12 +                                     **
** Public dhcp-client messages                                     **
**********************************************************************
EOC*/

#ifndef _IPS_DHCPCLIENT_MSG
#define _IPS_DHCPCLIENT_MSG 1

#ifdef __cplusplus
extern "C" {
#endif

#include <sdd/sdd.msg>
#include <bootp/bootp.h>

#define DHCPC_TMO_1	(DHCP_MSG_BASE + 0x10)
#define DHCPC_TMO_2	(DHCP_MSG_BASE + 0x11)
#define DHCPC_RESND	(DHCP_MSG_BASE + 0x12)

#define DHCPC_STOP             (DHCP_MSG_BASE+0x13)
#define DHCPC_STOP_REPLY       (DHCP_MSG_BASE+0x14)
typedef struct dhcpc_stop_s {
    sc_msgid_t id;
    sc_errcode_t error;
} dhcpc_stop_t;

#define DHCPC_START            (DHCP_MSG_BASE+0x15)
#define DHCPC_START_REPLY      (DHCP_MSG_BASE+0x16)
typedef struct dhcpc_start_s {
    sc_msgid_t id;
    sc_errcode_t error;
} dhcpc_start_t;

#define DHCPC_SESSION          (DHCP_MSG_BASE + 0x17)

#define DHCPC_SET_HOSTNAME     (DHCP_MSG_BASE + 0x18)
typedef struct dhcpc_setHostname_s {
  sc_msgid_t id;
  char name[1];
} dhcpc_setHostname_t;
#ifdef __cplusplus
}
#endif

#endif /* _IPS_DHCPCLIENT_MSG */
