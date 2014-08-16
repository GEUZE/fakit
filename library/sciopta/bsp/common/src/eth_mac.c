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
** +Date: 2008/01/16 13:01:18 +                                     **
** Common Ethernet MAC functions                                    **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sdd/sdd.msg>	/* sdd_rxbuf_t, SDD_NET_RECEIVE, SDD_NET_DATA */
#include <eth_mac.h>
#include "config.h"	/* CNF_ETH_SUPPORT */

/* default: just Ethernet II */
#ifndef CNF_ETH_SUPPORT
#warning CNF_ETH_SUPPORT not defined, setting to (ETH_SUPPORT_ETHERNET_II)
#define CNF_ETH_SUPPORT	  (ETH_SUPPORT_ETHERNET_II)
#endif

void
eth_sendToLinkLayer (sdd_netbuf_t NEARPTR rxbuf, int pktSize, sc_pid_t linkPid)
{
  __u8 *p;
  __uint fld;

  p = SDD_NET_DATA(rxbuf);
  rxbuf->base.id = SDD_NET_RECEIVE;
  rxbuf->base.error = 0;

#if CNF_ETH_SUPPORT > 0

  fld = (p[12] << 8) | p[13];	/* read length/type field */
  if (fld == 0x8100) {		/* tagged frame? */
    p = (__u8*) &p[4];		/* skip tag */
    rxbuf->data += 4;		/* skip tag */
    fld = (p[12] << 8) | p[13];	/* re-read length/type field */
  }
  /* check for etherType field */
  if (fld > 1500) {
#if CNF_ETH_SUPPORT & ETH_SUPPORT_ETHERNET_II
    rxbuf->protocol = fld;
    rxbuf->data += 14;
#else /* !ETHERNET_II */
    goto raw;
#endif /* ETHERNET_II */
  } else {
#if CNF_ETH_SUPPORT & ETH_SUPPORT_NETWARE
    /* check for IPX header */
    if (((p[14] << 8) | p[15]) == 0xffff) {
      rxbuf->protocol = 0x8137;
      rxbuf->data += 14;
    } else {
      goto raw;
    }
#else /* !NETWARE */

#if CNF_ETH_SUPPORT & (ETH_SUPPORT_LLC_SNAP | ETH_SUPPORT_LLC)
      /* check for SNAP header */
      if (((p[14] << 8) | p[15]) == 0xaaaa) {
#if CNF_ETH_SUPPORT & ETH_SUPPORT_LLC_SNAP
        rxbuf->protocol = p[20] | (p[21] << 8);
        rxbuf->data += 22;
#else /* !LCC_SNAP */
        goto raw;
#endif /* LCC_SNAP */
      } else {
#if CNF_ETH_SUPPORT & ETH_SUPPORT_LLC
#if CNF_ETH_SUPPORT & ETH_SUPPORT_NETWARE
	/* an IPX packet in an LLC frame? */
	if ((((p[14] << 8) | p[15] << 8) | p[16]) == 0xe0e003) {
	  rxbuf->protocol = 0x8137;
	  rxbuf->data += 17;
	} else {
	  goto raw;
	}
#else /* !NETWARE */
	goto raw;
#endif /* NETWARE */
#else /* !LCC */
	goto raw;
#endif /* LLC */
      }
#else /* !(ETH_SUPPORT_LLC_SNAP | ETH_SUPPORT_LLC) */
    goto raw;
#endif /* (ETH_SUPPORT_LLC_SNAP | ETH_SUPPORT_LLC) */

#endif /* NETWARE */
  }

#else /* CNF_ETH_SUPPORT <= 0 */

  goto raw;

#endif /* CNF_ETH_SUPPORT */

  goto ret;

  /* can't determine type of packet */
raw:
  rxbuf->protocol = 0;
  rxbuf->data += 14;

ret:
  rxbuf->tail = rxbuf->data + pktSize;
  sc_msgTx ((sc_msgptr_t) &rxbuf, linkPid, 0);
}

