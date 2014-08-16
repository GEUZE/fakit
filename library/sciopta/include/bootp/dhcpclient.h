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
** ID: S03175CL2                                                    **
** +Revision: 1.17 +                                                **
** +Date: 2008/01/16 13:02:06 +                                     **
** BOOTP definition                                                 **
**********************************************************************
EOC*/

#ifndef _IPS_DHCPCLIENT_H
#define _IPS_DHCPCLIENT_H	1

#ifdef __cplusplus
extern "C" {
#endif

#include <bootp/bootp.h>
#include <ips/device.msg>

  /**
   * DHCP session instance type.
   */
  typedef struct dhcpc_session_s dhcpc_session_t;

  /**
   * Open a DHCP session.
   * \param[in]  ethDev     Target ethernet device
   * \param[in]  serverName Server name, NULL terminated
   * \param[in]  bootFile   Logical boot file name
   * \param[in]  maxMsgSize Limit the message size to avoid system crash
   * \return                DHCP session instance
   */
  dhcpc_session_t NEARPTR dhcpc_open (const char *ethDev, 
				      const char *serverName,
				      const char *bootFile, 
				      __u16 maxMsgSize);
  /**
   * Close a DHCP session and free the session instance.
   * \param[in]  self       DHCP session instance
   */
  void dhcpc_close (dhcpc_session_t NEARPTR NEARPTR self);
  /**
   * Connect to a specific server
   * \param[in]  self       DHCP session instance
   * \param[in]  dst        DHCP server address
   * \return                -1 on error
   */
  int dhcpc_connect (dhcpc_session_t NEARPTR self, const __u8 * dst);
  /**
   * DHCP discover. 
   * \param[in]  self       DHCP session instance
   * \param[in]  paramReqList Parameters for request 
   *                        see BOOTPTAG_XXX in <bootp/bootp.h>
   * \param[in]  paramReqListLen Parameter list length
   * \return                -1 on error
   */
  int dhcpc_discover (dhcpc_session_t NEARPTR self, 
		      const __u8 * paramReqList,
                      int paramReqListLen);
  /**
   * DHCP request. 
   * \param[in]  self       DHCP session instance
   * \param[in]  offer      A offer from a server on a DHCP discover.
   * \param[in]  paramReqList Parameters for request
   *                        see BOOTPTAG_XXX in <bootp/bootp.h>
   * \param[in]  paramReqListLen Parameter list length
   * \param[in]  ip         My assigned IP address if any else 0 
   * \return                -1 on error
   */
  int dhcpc_request (dhcpc_session_t NEARPTR self, 
		     bootp_reply_t NEARPTR offer,
                     const __u8 * paramReqList, 
		     int paramReqListLen, 
		     __u32 ip);
  /**
   * Lookup type of DHCP server reply message.
   * \param[in]  self       DHCP session instance
   * \param[in]  dhcpMsg    DHCP message type DHCP_XXX see below
   * \param[in]  msg        Received message (UDP netbuf)
   * \return                -1 on error
   */
  int dhcpc_is (dhcpc_session_t NEARPTR self, __u8 dhcpMsg, sc_msg_t msg);
  /**
   * Extract the reply from the message and free the message.
   * \param[in]  msg        Received message (UDP netbuf)
   * \return                NULL on error else the reply as a bootp_reply_t
   */
  bootp_reply_t NEARPTR dhcpc_get (sc_msgptr_t msg);
  /**
   * DHCP decline. Decline a offer.
   * \param[in]  self       DHCP session instance
   * \param[in]  bad        Bad DHCP offer
   * \return                -1 on error
   */
  int dhcpc_decline (dhcpc_session_t NEARPTR self, bootp_reply_t NEARPTR bad);
  /**
   * DHCP release. Release a assigned IP address.
   * \param[in]  self       DHCP session instance
   * \return                -1 on error
   */
  int dhcpc_release (dhcpc_session_t NEARPTR self);
  /**
   * Tell on what ethernet device the dhcp is running. For hardware address
   * Lookup.
   * \param[in]  self       DHCP session instance
   * \return                NULL on error else the installed ethernet device
   *                        instance
   */
  ips_dev_t NEARPTR dhcpc_etherDev (dhcpc_session_t NEARPTR self);
  /**
   * Does a secure ips_netbufAck.
   * \param[in]  self       DHCP session instance
   * \param[in]  msg        Received message (UDP netbuf) which shall be acked
   */
  void dhcpc_netbufAck (dhcpc_session_t NEARPTR self, sc_msg_t msg);

  /**
   * A simple client which does demonstrate how to use the functions above,
   * with in a statemachine :-)
   * \param[in]  ethDev     Ethernet device on which the DHCP shall run
   * \param[in]  serverIp   DHCP server IP address
   * \param[in]  bootFile   Optional logical bootp file name
   * \param[in]  maxMsgSize The maximum message size to avoid system crashs
   * \param[in]  proxy      Proxy process, if SC_ILLEGAL_PID no prox. 
   */
  void dhcp_client (const char *ethDev, const char *serverIp,
                    const char *bootFile, __u16 maxMsgSize, sc_pid_t proxy);


#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: dhcpclient.h,v $
** Revision 1.17  2008/01/16 13:02:06  bastian
** merge from 1.9.1.12
**
** Revision 1.16.8.1  2007/07/20 15:38:50  bastian
** added dynamic
**
** Revision 1.16  2005/07/01 12:28:31  christian
** Merged SCIOPTA_1_7_2 to MAIN
**
** Revision 1.15.4.2  2005/05/25 09:07:28  bastian
** fixes for tasking
**
** Revision 1.15.4.1  2005/05/09 13:24:08  christian
** Can now set a proxy process between dhcpclient and router to controll
** ipv4_routeAdd from dhcp client.
**
** Revision 1.15  2004/11/18 15:03:49  christian
** DHCP server second edition :-)
**
** Revision 1.14  2004/08/03 13:51:01  christian
** DHCP client with renewing leases.
**
** Revision 1.13  2004/07/27 12:04:13  christian
** Release bugfix: Remove refresh IP.
**
** Revision 1.12  2004/07/26 13:27:16  christian
** Renamed ips_bootpXxxYyy to bootp_xxxYyy.
**
** Revision 1.11  2004/05/26 06:05:23  christian
** Header files documented and spell checks.
**
** Revision 1.10  2004/05/05 08:56:08  christian
** Rename ips_ipv4Any to ipv4_any and IPS_IPV4_ANY IPV4_ANY.
**
** Revision 1.9  2004/02/05 06:51:50  christian
** Fixed a memory violation -> sizeof (ipv4_route_t *) was not correct.
**
** Revision 1.8  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 1.7  2003/07/10 07:37:21  christian
** Arp can now do a client request for probing a ip address.
**
** Revision 1.6  2003/07/09 15:00:57  christian
** Bugfix and configureable.
**
** Revision 1.5  2003/07/09 14:53:22  christian
** Client is running now (rfc 2131 conform but without the reboot part).
**
** Revision 1.4  2003/07/08 09:57:47  christian
** dhcp is now working (discover -> offer -> request ->ack/nak).
**
** Revision 1.3  2003/07/07 13:47:20  christian
** Have now main parts of dhcp.
**
** Revision 1.2  2003/07/02 13:39:09  christian
** Bugfixed and beautified the code.
**
** Revision 1.1  2003/07/01 13:03:17  christian
** Added bootpclient and dhcpclient header files.
**
**
**/
