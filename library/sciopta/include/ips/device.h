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
** ID: S02021CL1                                                    **
** +Revision: 1.33 +                                                **
** +Date: 2008/01/16 13:02:06 +                                     **
** IPS device manager                                               **
**********************************************************************
EOC*/

#ifndef _IPS_DEV_H_
#define _IPS_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sciopta.h>
#include <ips/addr.h>
#include <ips/device.msg>

/* Types for ARP hardware */
#define IPS_DEV_TYPE_NETROM		0
#define IPS_DEV_TYPE_ETHER		1
#define IPS_DEV_TYPE_EETHER		2
#define IPS_DEV_TYPE_AX25		3
#define IPS_DEV_TYPE_PRONET		4
#define IPS_DEV_TYPE_CHAOS		5
#define IPS_DEV_TYPE_IEEE802		6
#define IPS_DEV_TYPE_ARCNET		7
#define IPS_DEV_TYPE_APPLETLK	8
#define IPS_DEV_TYPE_DLCI		15
#define IPS_DEV_TYPE_ATM		19

/* Dummy tpyes for non ARP hardware */
#define IPS_DEV_TYPE_SLIP		256
#define IPS_DEV_TYPE_CSLIP		259
#define IPS_DEV_TYPE_PPP		512
#define IPS_DEV_TYPE_TUNNEL		768
#define IPS_DEV_TYPE_LOOPBACK		772
#define IPS_DEV_TYPE_IRDA		783

/* protocols */
#define IPS_P_LOOP	0x0060  /* Ethernet Loopback packet     */
#define IPS_P_PUP	0x0200  /* Xerox PUP packet             */
#define IPS_P_PUPAT	0x0201  /* Xerox PUP Addr Trans packet  */
#define IPS_P_IP	0x0800  /* Internet Protocol packet     */
#define IPS_P_X25	0x0805  /* CCITT X.25                   */
#define IPS_P_ARP	0x0806  /* Address Resolution packet    */
#define	IPS_P_BPQ	0x08ff  /* G8BPQ AX.25 Ethernet Packet  
                                 * [ NOT AN OFFICIAL ]          */
#define IPS_P_IEEEPUP	0x0a00  /* Xerox IEEE802.3 PUP packet   */
#define IPS_P_IEEEPUPAT	0x0a01  /* Xerox IEEE802.3 PUP Addr 
                                 * Trans packet                 */
#define IPS_P_DEC       0x6000  /* DEC Assigned proto           */
#define IPS_P_DNA_DL    0x6001  /* DEC DNA Dump/Load            */
#define IPS_P_DNA_RC    0x6002  /* DEC DNA Remote Console       */
#define IPS_P_DNA_RT    0x6003  /* DEC DNA Routing              */
#define IPS_P_LAT       0x6004  /* DEC LAT                      */
#define IPS_P_DIAG      0x6005  /* DEC Diagnostics              */
#define IPS_P_CUST      0x6006  /* DEC Customer use             */
#define IPS_P_SCA       0x6007  /* DEC Systems Comms Arch       */
#define IPS_P_RARP      0x8035  /* Reverse Addr Res packet      */
#define IPS_P_ATALK	0x809b  /* Appletalk DDP                */
#define IPS_P_AARP	0x80f3  /* Appletalk AARP               */
#define IPS_P_8021Q	0x8100  /* 802.1Q VLAN Extended Header  */
#define IPS_P_IPX	0x8137  /* IPX over DIX                 */
#define IPS_P_IPV6	0x86dd  /* IPv6 over bluebook           */
#define IPS_P_PPP_DISC	0x8863  /* PPPoE discovery messages     */
#define IPS_P_PPP_SES	0x8864  /* PPPoE session messages       */
#define IPS_P_ATMMPOA	0x884c  /* MultiProtocol Over ATM       */
#define IPS_P_ATMFATE	0x8884  /* Frame-based ATM Transport
                                 * over Ethernet                */
/* pseudo protocol types to make life easier */
#define IPS_P_802_3	0x0001  /* Dummy type for 802.3 frames  */
#define IPS_P_AX25	0x0002  /* Dummy protocol id for AX.25  */
#define IPS_P_ALL	0x0003  /* Every packet (be careful!!!) */
#define IPS_P_802_2	0x0004  /* 802.2 frames                 */
#define IPS_P_SNAP	0x0005  /* Internal only                */
#define IPS_P_DDCMP     0x0006  /* DEC DDCMP: Internal only     */
#define IPS_P_WAN_PPP   0x0007  /* Dummy type for WAN PPP frames */
#define IPS_P_PPP_MP    0x0008  /* Dummy type for PPP MP frames */
#define IPS_P_LOCALTALK 0x0009  /* Localtalk pseudo type        */
#define IPS_P_PPPTALK	0x0010  /* Dummy type for Atalk over PPP */
#define IPS_P_TR_802_2	0x0011  /* 802.2 frames                 */
#define IPS_P_MOBITEX	0x0015  /* Mobitex (kaz@cafe.net)       */
#define IPS_P_CONTROL	0x0016  /* Card specific control frames */
#define IPS_P_IRDA	0x0017  /* Sciopta-IrDA                 */
#define IPS_P_ECONET	0x0018  /* Acorn Econet                 */

/* Notifier events */
#define IPS_DEV_EVENT_REGISTER	1
#define IPS_DEV_EVENT_UNREGISTER	2
#define IPS_DEV_EVENT_CHANGE		3

  /**
   * Tunnel device:
   * This device can be handled as a normal net device. The IPS do then send
   * netbufs to this device depending on the routing information. An 
   * application can open this device and receive the outging netbuffers from 
   * IPS. The opening application can also send netbuffers to the tunnel 
   * device, which will send it up the IPS. 
   * 
   * \param[in]  name      Name of the device, normaly tun0, tun1, ...
   * \param[in]  mtu       Max. transmit unit
   * \param[in]  mru       Max. receive unit
   * \param[in]  protocol  IPS_P_IP, IPS_P_IPV6, IPS_P_IPX, ... (ips/device.h)
   * \param[in]  plid      Pool id for allocations.
   * \return         0 if success -1 if error
   */
  int tun_process (const char *name, int mtu, int mru, __u16 protocol,
                   sc_poolid_t plid);

  /**
   * Loopback device:
   * This device can be handled as a normal net device. The IPS do then send
   * netbufs to this device depending on the routing information. The loopback
   * device does now loopback every packet back into the IPS. 
   * 
   * \param[in]  name     Name of the device, normaly lo0, lo1, ...
   * \param[in]  mtu      Max. transmit unit
   * \param[in]  mru      Max. receive unit
   * \param[in]  plid     Pool id for allocations.
   * \return         0 if success -1 if error
   */
  int lo_process (const char *name, int mru, int mtu, sc_poolid_t plid);

  /**
   * Network device registration:
   * To register network devices to /SCP_netman.
   *
   * \param[in]  dev      Device which should be registered, must be a message 
   *                      allocation.
   * \return              0 if success -1 if error
   */
  int ips_devRegister (ips_dev_t NEARPTR NEARPTR dev);

  /** 
   * Network device registration:
   * To remove network devices from /SCP_netman.
   * 
   * \param[in]  dev     Device which should be removed.
   * \return             0 if success -1 if error
   */
  int ips_devUnregister (ips_dev_t NEARPTR dev);

  /** 
   * Get a network device by name:
   * To get a network device by its name.
   *
   * \param[in]  name    Requested name of wished network device.
   * \return             device if success NULL if error
   */
  ips_dev_t NEARPTR ips_devGetByName (const char *name);

  /** 
   * Get a network device by name:
   * To get a network device by its name.
   *
   * \param[in]  name    Requested name of wished network device.
   * \param[in]  tmo     Return if device does not appear until
   * \return             device if success NULL if error
   */
  ips_dev_t NEARPTR ips_devGetByNameWait (const char *name, sc_ticks_t tmo);

  /** 
   * Get a network device by hardware address:
   * To get a network device by its hardware address (MAC).
   *
   * \param[in]  addr    Requested hardware address (MAC) of wished network 
   *                     device.
   * \return             device if success NULL if error
   */
  ips_dev_t NEARPTR ips_devGetByHwAddr (ips_addr_t * addr);

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: device.h,v $
** Revision 1.33  2008/01/16 13:02:06  bastian
** merge from 1.9.1.12
**
** Revision 1.32.8.1  2007/07/24 08:59:10  bastian
** added function to wait for a device
**
** Revision 1.32  2005/07/01 12:28:31  christian
** Merged SCIOPTA_1_7_2 to MAIN
**
** Revision 1.31.4.1  2005/05/25 09:32:57  bastian
** fixes
**
** Revision 1.31  2004/05/14 05:50:37  christian
** Added Comments
**
** Revision 1.30  2004/01/29 09:58:07  christian
** Renamed tunnel to tun_process and loopback to lo_process.
**
** Revision 1.29  2003/09/17 09:06:50  christian
** Prepare sciopta vpn for cyrptographical operations.
**
** Revision 1.28  2003/09/11 12:10:17  christian
** Added tunnel device type to ipv4.
**
** Revision 1.27  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 1.26  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 1.25  2003/09/05 08:24:25  christian
** Bugfixed a reconnection crash if no route to host.
**
** Revision 1.24  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 1.23  2003/07/08 12:52:19  christian
** Bugfixed the error strategy.
**
** Revision 1.22  2003/06/23 14:14:19  christian
** Added a loopback device.
**
** Revision 1.21  2003/05/27 14:43:51  christian
** Router is near finished. A telnet problem occurs forgets chars?!
** option message added.
**
** Revision 1.20  2003/03/18 15:04:25  christian
** Cleaned.
**
** Revision 1.19  2002/11/01 12:50:13  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 1.18  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 1.17  2002/09/17 09:56:45  christian
** Bios interface has changed
**
** Revision 1.16  2002/09/11 13:46:42  christian
** Check if we have a broadcast/multicast/otherhost typed packet.
**
** Revision 1.15  2002/09/11 12:22:31  christian
** Moved protocol types from link/include/ips/link.h over here.
**
** Revision 1.14  2002/09/11 07:47:02  christian
** Bufixes
**
** Revision 1.13  2002/09/10 12:11:12  christian
** Running net device manager
**
** Revision 1.12  2002/09/09 14:57:59  christian
** Added a net device manager.
**
** Revision 1.11  2002/06/05 09:24:14  christian
** added methode network_star and network_stop, to launch a network device.
**
** Revision 1.10  2002/06/04 07:39:16  christian
** removed device/src/device.c and replaced it with SCP_rcsman as
** SCP_netman.
**
** Revision 1.9  2002/05/24 14:31:09  christian
** Removed start and stop methode.
** -> replaced them with incr and decr
**
** Revision 1.8  2002/03/12 12:11:20  christian
** Removed incr and decr from ips_dev_t.
**
** Revision 1.7  2002/02/25 12:57:09  christian
** New container.
**
** Revision 1.6  2002/02/13 13:36:15  christian
** IPS device registry is now owner of all IPS devices.
**
** Revision 1.5  2002/02/01 10:11:49  christian
** Moved old connect.h over to connect.msg. In the new connect.h the socket
** methodes are defined.
**
** Revision 1.4  2002/01/23 16:04:45  christian
** Fixed message bases.
**
** Revision 1.3  2002/01/23 15:02:23  christian
** Use arp functions instead of own hacket arp packets.
**
** Revision 1.2  2002/01/23 10:59:58  christian
** Owerworked the interface design. New device registration mechanisme.
** -> Devices are now protocol independant.
**
** Revision 1.1.1.1  2002/01/21 16:45:49  christian
** first revision
**
**
**
**/
