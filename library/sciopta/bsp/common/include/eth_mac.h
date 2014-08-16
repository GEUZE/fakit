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
** (c) 2007 Sciopta Systems GmbH Deutschland, Sciopta AG/ Schweiz   **
**                                                                  **
**********************************************************************
** ID: S07064BS3                                                    **
** +Revision: 1.2 +                                                **
** +Date: 2008/01/16 13:01:17 +                                     **
** Common Ethernet MAC definitions                                  **
**********************************************************************
EOC*/
#ifndef ETH_MAC_H
#define ETH_MAC_H

#include <sciopta.h>
#include <sdd/sdd.msg>

EXTERN_C_BEGIN
#define ETH_SUPPORT_ETHERNET_II (1)
#define ETH_SUPPORT_NETWARE	(2)
#define ETH_SUPPORT_LLC_SNAP    (4)
#define ETH_SUPPORT_LLC		(8)

union macAddr_u
{
  __u8 arr[6];
  struct {
    __u32 btm;
    __u16 top;
  };
};
typedef union macAddr_u macAddr_t;

/* Prepares a received (and buffered) valid ethernet packet for 
**  the link layer and then sends it 
*/
void eth_sendToLinkLayer (sdd_netbuf_t NEARPTR netbuf, 
			  int pktSize, 
			  sc_pid_t linkPid);
EXTERN_C_END
#endif /* ETH_MAC_H */
