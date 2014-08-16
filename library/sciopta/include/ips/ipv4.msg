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
** ID: S01332CL8                                                    **
** +Revision: 2.19 +                                                 **
** +Date: 2005/10/31 07:16:07 +                                     **
** Contains IP version 4 messages.                                  **
**********************************************************************
EOC*/

#ifndef _IPS_IPV4_MSG_
#define _IPS_IPV4_MSG_

#ifdef __cplusplus
extern "C" {
#endif

#include <sdd/sdd.msg>
#include <ips/base.msg>
#include <ips/ipv4.h>  
/**
* To skip incomplet IP pakets after 30 seconds
*/
#define IPV4_FRAG_TMO		IPV4_MSG_BASE
  
#define IPV4_MSG_CACHE_TMO		IPV4_MSG_BASE + 1
  typedef struct ipv4_msgCacheTmo_s {
    sc_msgid_t id;
    int entry;
  } ipv4_msgCacheTmo_t;


#ifdef __cplusplus
}
#endif
#endif

