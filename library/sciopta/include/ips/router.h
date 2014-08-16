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
** ID: S01332CL9                                                    **
** +Revision: 1.25 +                                                 **
** +Date: 2005/07/01 12:28:31 +                                     **
** Router function interface                                        **
**********************************************************************
EOC*/

#ifndef _IPS_ROUTER_H_
#define _IPS_ROUTER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sciopta.h>
#include <ossys/defines.h>
#include <ossys/types.h>
#include <ips/router.msg>

  /* External function, only used outside of router process 
     (SCP_ipv4) */
  int ipv4_routeAdd (ipv4_route_t NEARPTR NEARPTR route);
  int ipv4_routeRm (ipv4_route_t NEARPTR route);
  ipv4_route_t NEARPTR ipv4_routeGet (ipv4_route_t NEARPTR ref);
  ipv4_route_t NEARPTR ipv4_routeGetNext (void);
   void ipv4_routeGetSkip (void);
  int ipv4_routeWait (__u8 destination[4], sc_ticks_t tmo);

  /* Internal function, only used inside of router process 
     (SCP_ipv4) */
#ifdef __IPS_CNF_H__
  typedef struct __ipv4Router_s __ipv4Router_t;

  __ipv4Router_t NEARPTR __ipv4Router (sc_poolid_t plid, sc_ticks_t tmo);
  void __ipv4RouterMsgHdl (__ipv4Router_t NEARPTR router, sc_msgptr_t msg);
#if IPS_CNF_IPV4_ROUTER_ENABLED == 1
  ipv4_route_t NEARPTR __ipv4RouteGet (__ipv4Router_t NEARPTR router, __u8 * dest);
#else
  ipv4_route_t NEARPTR __ipv4RouteGet (__ipv4Router_t NEARPTR router);
#endif
  ips_dev_t NEARPTR __ipv4DeviceGet (__ipv4Router_t NEARPTR router, __u8 * src);
  int __ipv4Pkt4Me (__ipv4Router_t NEARPTR router, __u8 * src);

#endif

#ifdef __cplusplus
}
#endif
#endif

