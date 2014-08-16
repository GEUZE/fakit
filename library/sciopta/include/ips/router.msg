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
** ID: S02256CL1                                                    **
** +Revision: 1.22 +                                                 **
** +Date: 2005/07/01 12:28:31 +                                     **
** Router message interface                                         **
**********************************************************************
EOC*/

#ifndef _IPS_IPV4_ROUTE_MSG_
#define _IPS_IPV4_ROUTE_MSG_

#ifdef __cplusplus
extern "C" {
#endif

#include <sciopta.h>
#include <ossys/types.h>
#include <ossys/defines.h>
#include <ips/device.h>
#include <ips/base.msg>

#define IPV4_ROUTE_ADD		(IPV4_ROUTE_MSG_BASE)
#define IPV4_ROUTE_ADD_REPLY	(IPV4_ROUTE_ADD + 1)
#define IPV4_ROUTE_RM		(IPV4_ROUTE_MSG_BASE + 2)
#define IPV4_ROUTE_RM_REPLY	(IPV4_ROUTE_RM + 1)
#define IPV4_ROUTE_GET		(IPV4_ROUTE_MSG_BASE + 4)
#define IPV4_ROUTE_GET_REPLY	(IPV4_ROUTE_GET + 1)
#define IPV4_ROUTE_GET_STOP	(IPV4_ROUTE_MSG_BASE + 6)
  typedef struct ipv4_route_s {
    ips_dev_t device;
    __u8 source[4];
    __u8 netmask[4];
    __u8 router[4];
    __u16 metric;
  } ipv4_route_t;

#define IPV4_ROUTE_WAIT		(IPV4_ROUTE_MSG_BASE + 8)
#define IPV4_ROUTE_WAIT_REPLY	(IPV4_ROUTE_WAIT + 1)
  typedef struct ipv4_routeWait_s {
    sdd_baseMessage_t base;
    __u8 destination[4];
    sc_ticks_t tmo;
    sc_tmoid_t tmoid;
  } ipv4_routeWait_t;

#define IPV4_ROUTE_WAIT_TMO	(IPV4_ROUTE_MSG_BASE + 10)
  typedef struct ipv4_routeWaitTmo_s {
    sc_msgid_t id;
    sc_msg_t waiter;
  } ipv4_routeWaitTmo_t;

#ifdef __cplusplus
}
#endif
#endif

