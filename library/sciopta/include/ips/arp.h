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
** ID: S03139CL1                                                    **
** +Revision: 1.19 +                                                 **
** +Date: 2005/07/01 12:28:31 +                                     **
** Arp function interface                                           **
**********************************************************************
EOC*/

#ifndef _IPS_ARP_H_
#define _IPS_ARP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/defines.h>
#include <sdd/sdd.msg>
#include <ips/device.msg>
#include <ips/arp.msg>
#include <ips/router.h>

#define ARP_OP_ARP_REQ		0x0001
#define ARP_OP_ARP_REP		0x0002
#define ARP_OP_RARP_REQ		0x0003
#define ARP_OP_RARP_REP		0x0004

#define ARP_E_PERMANENT		0
#define ARP_E_INCOMPLETE	1
#define ARP_E_REACHABLE		2
#define ARP_E_STALE		3
#define ARP_E_DELAY		4
#define ARP_E_PROBE		5
#define ARP_E_FAILED		6

  typedef struct __ipv4Arp_s __ipv4Arp_t;
  typedef struct __ipv4ArpHdr_s {
    __u8 dstaddr[6];
    __u8 srcaddr[6];
    __u16 type;
  } __ipv4ArpHdr_t;

  typedef struct __ipv4ArpPkt_s {
    __u16 hardType;
    __u16 protType;
    __u8 hardSize;
    __u8 protSize;
    __u16 op;
    __u8 senderEth[6];
    __u8 senderIp[4];
    __u8 targetEth[6];
    __u8 targetIp[4];
  } __ipv4ArpPkt_t;

  /* External function, only used outside of router process 
     (SCP_ipv4) */
  int ipv4_arpAdd (ipv4_arp_t NEARPTR NEARPTR arp);
  int ipv4_arpRm (ipv4_arp_t NEARPTR arp);
  int ipv4_arpTest (ips_dev_t NEARPTR dev, __u8 * ip);
#ifdef __IPS_CNF_H__
  /* Internal function, only used inside of router process 
     (SCP_ipv4) */
  __ipv4Arp_t NEARPTR __ipv4Arp (__ipv4Router_t NEARPTR r, 
				 sc_poolid_t plid,
				 sc_ticks_t tmo);
  void __ipv4ArpMsgHdl (__ipv4Arp_t NEARPTR arp, sc_msgptr_t msg);
  int __ipv4ArpAdd (__ipv4Arp_t NEARPTR arp, __u8 state, __u8 * ip,
                    __ipv4ArpHdr_t NEARPTR header);
  void __ipv4ArpSend (__ipv4Arp_t NEARPTR arp, 
		      __u16 op, __u8 * sEth, __u8 * sIp,
                      __u8 * tEth, __u8 * tIp, __u8 * toEth,
                      ips_dev_t NEARPTR dev);
  int __ipv4ArpGetEthHdr (__ipv4Arp_t NEARPTR arp, 
			  ips_dev_t NEARPTR dev, 
			  __u8 * srcIp,
                          __u8 * destIp, 
			  sdd_netbuf_t NEARPTR  netbuf);
  int __ipv4ArpTest (ips_dev_t NEARPTR dev, 
		     __u8 * ip, 
		     sc_poolid_t plid,
                     sc_ticks_t tmo);
#endif
#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: arp.h,v $
** Revision 1.19  2005/07/01 12:28:31  christian
** Merged SCIOPTA_1_7_2 to MAIN
**
** Revision 1.18.4.1  2005/06/10 06:25:59  bastian
** fixes for Tasking M16C
**
** Revision 1.18  2004/11/24 13:57:12  bastian
** fixes for Cosmic
**
** Revision 1.17  2004/10/07 07:37:42  bastian
** fixed NEAR * to NEARPTR
**
** Revision 1.16  2004/10/04 12:27:27  bastian
** started to change for m16c
**
** Revision 1.15  2004/09/29 13:56:43  christian
** Arp test function with more logical order of the parameters.
**
** Revision 1.14  2004/08/23 07:22:01  christian
** Arp test function only works with in the ipv4 process.
**
** Revision 1.13  2004/05/05 08:56:08  christian
** Rename ips_ipv4Any to ipv4_any and IPS_IPV4_ANY IPV4_ANY.
**
** Revision 1.12  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 1.11  2003/07/30 05:59:05  christian
** sdd_netBuffer -> sdd_netbuf
**
** Revision 1.10  2003/07/10 07:37:21  christian
** Arp can now do a client request for probing a ip address.
**
** Revision 1.9  2003/06/16 13:21:44  christian
** Worked up the arp cache. Works fine now.
**
** Revision 1.8  2003/06/16 07:44:52  christian
** Bugfixed many stuff around device, router and arp handling.
**
** Revision 1.7  2003/06/12 12:26:21  christian
** Temp check in.
**
** Revision 1.6  2003/06/10 14:10:57  christian
** Some bugfixes.
**
** Revision 1.5  2003/06/04 07:03:55  christian
** Arp refresh implemented. Arp entries should now stay consistent over time.
**
** Revision 1.4  2003/06/03 14:15:34  christian
** Added pkt cache with tmo controlling.
**
** Revision 1.3  2003/06/02 14:13:47  christian
** Simpler print on read request in all parts of ipv4.
**
** Revision 1.2  2003/05/23 12:22:54  christian
** Router is much faster and cleaner :-)
**
** Revision 1.1  2003/05/20 14:22:28  christian
** Added arp.
**
**
**/
