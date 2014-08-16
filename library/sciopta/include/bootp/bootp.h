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
** +Revision: 1.14.12.1 +                                                **
** +Date: 2009/02/17 09:23:12 +                                     **
** BOOTP packet structure definition                                **
**********************************************************************
EOC*/

#ifndef _BOOTPH
#define _BOOTPH	1

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/defines.h>
#include <ossys/types.h>

  /**
   * bootp packet structure
   */
  typedef struct bootp_pkt_s {
    /** 
     * Operation code: tells bootp if this is a request or reply
     */
#define BOOTPREQ	1
#define BOOTPREP	2
    __u8 opcode;
    /** 
     * Hardware type: see ips/device.h for IPS_DEV_TYPES, normaly it is
     * IPS_DEV_TYPE_ETHER
     */
    __u8 hwtype;
    /** 
     * Hardware address lenght, for IPS_DEV_TYPE_ETHER this is 6
     */
    __u8 hwaddrlen;
    /** 
     * Client sets to zero, optionally used by gateways in cross-gateway 
     * booting.
     */
    __u8 hopcount;
    /** 
     * Transaction ID, a random number, used to match this boot request with 
     * the responses it generates.
     */
    __u32 id;
    /** 
     * Filled in by client, seconds elapsed since client started trying to 
     * boot.
     */
    __u16 seconds;
    /**
     * A reserved entry
     */
    __u16 flags;
#define BOOTP_FLAGS_BC htons(0x8000)

    /** 
     * Client IP address; filled in by client in bootrequest if known.
     */
    __u32 clientIp;
    /** 
     * 'your' (client) IP address; filled by server if client doesn't know 
     * its own address (ciaddr was 0).
     */
    __u32 yourIp;
    /** 
     * Server IP address; returned in bootreply by server.
     */
    __u32 serverIp;
    /**
     * Gateway IP address, used in optional cross-gateway booting.
     */
    __u32 gatewayIp;
    /**
     * Client hardware address, filled in by client. 
     */
    __u8 hwaddr[16];
    /**
     * Optional server host name, null terminated string.
     */
    __u8 serverName[64];
    /**
     * boot file name, null terminated string; 'generic' name or null in 
     * bootrequest, fully qualified directory-path name in bootreply.
     */
    __u8 bootFile[128];
    /**
     * optional vendor-specific area, e.g. could be hardware type/serial on 
     * request, or 'capability' / remote file system handle on reply.  This 
     * info may be set aside for use by a third phase bootstrap or kernel.
     */
    __u8 vendorInfo[64];
  } bootp_pkt_t;

  /**
   * Bootp reply entries
   */
  typedef struct bootp_entry_s {
    /**
     * 
     */
#define BOOTPTAG_NONE			0
#define BOOTPTAG_NETMASK		1
#define BOOTPTAG_GATEWAYS		3
#define BOOTPTAG_NAMESERVERS		5
#define BOOTPTAG_DOMAINNAMESERVERS	6
#define BOOTPTAG_HOSTNAME               12
#define BOOTPTAG_DHCP_REQUESTEDIP	50
#define BOOTPTAG_DHCP_IPLEASETIME	51
#define BOOTPTAG_DHCP_MSGTYPE		53
#define BOOTPTAG_DHCP_SERVERID		54
#define BOOTPTAG_DHCP_PARAMREQLIST	55
#define BOOTPTAG_DHCP_MSGMAX		57
#define BOOTPTAG_DHCP_VENDORID          60
#define BOOTPTAG_DHCP_CLIENTID		61
#define BOOTPTAG_END			255
    __u8 tag;
    /**
     */
    __u8 len;
    /**
     */
    __u8 data[1];
  } bootp_entry_t;

#define DHCP_BOOTP_REPLY (DHCP_MSG_BASE + 10)
  /**
   * Structure to fill in the relevant bootp parametes in the reply. 
   */
  typedef struct bootp_reply_s {
    /**
     * Message id
     */
    sc_msgid_t id;
    /**
     * Size of the tag list at the end of this struct.
     */
    size_t tagListSize;
    /** 
     * 'your' (client) IP address; filled by server if client doesn't know 
     * its own address (ciaddr was 0).
     */
    __u32 clientIp;
    /** 
     * Server IP address; returned in bootreply by server.
     */
    __u32 serverIp;
    /**
     * Gateway IP address, used in optional cross-gateway booting.
     */
    __u32 gatewayIp;
    /**
     * Optional server host name, null terminated string.
     */
    __u8 serverName[64];
    /**
     * boot file name, null terminated string; 'generic' name or null in 
     * bootrequest, fully qualified directory-path name in bootreply.
     */
    __u8 bootFile[128];
    /**
     * If the 'vend' field is used, it is recommended that a 4 byte
     * 'magic number' be the first item within 'vendorInfo'.
     */
    __u8 magicCookie[4];
    /**
     * Entries in the vendorInfo field.
     */
    bootp_entry_t startTags;
  } bootp_reply_t;

  /**
   * Get a spezified tag.
   * \param[in]  start   Start of the tag list
   * \param[in]  tag     Searched tag
   * \param[in]  size    Size of the tag list (to avoid endless searching) 
   * \return             NULL if not found or the tag if found
   */
  bootp_entry_t NEARPTR bootp_getTag (bootp_entry_t NEARPTR start, 
				      __u8 tag,
				      __u8 size);
  
#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: bootp.h,v $
** Revision 1.14.12.1  2009/02/17 09:23:12  bastian
** merge R1949
**
** Revision 1.14.8.1  2009/02/17 08:21:07  bastian
** started host-name support
** hack: hostname instead of server-name
** ARP cache increased
**
** Revision 1.14  2008/01/16 13:02:06  bastian
** merge from 1.9.1.12
**
** Revision 1.13.8.2  2007/07/25 11:04:01  bastian
** fixed memory leak and re-starting
**
** Revision 1.13.8.1  2007/07/20 15:38:50  bastian
** added dynamic
**
** Revision 1.13  2005/07/01 12:28:31  christian
** Merged SCIOPTA_1_7_2 to MAIN
**
** Revision 1.12.4.1  2005/05/25 09:32:57  bastian
** fixes
**
** Revision 1.12  2004/07/27 12:04:13  christian
** Release bugfix: Remove refresh IP.
**
** Revision 1.11  2004/07/26 13:27:16  christian
** Renamed ips_bootpXxxYyy to bootp_xxxYyy.
**
** Revision 1.10  2004/05/26 06:05:23  christian
** Header files documented and spell checks.
**
** Revision 1.9  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 1.8  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 1.7  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 1.6  2003/07/08 14:25:50  christian
** Started with a rfc dhcp client without a fs.
**
** Revision 1.5  2003/07/08 09:57:47  christian
** dhcp is now working (discover -> offer -> request ->ack/nak).
**
** Revision 1.4  2003/07/07 13:47:20  christian
** Have now main parts of dhcp.
**
** Revision 1.3  2003/07/02 13:39:09  christian
** Bugfixed and beautified the code.
**
** Revision 1.2  2003/07/01 11:25:37  christian
** Bugfixes in bootp.
**
** Revision 1.1  2003/07/01 08:30:24  christian
** Do it as with all other net services -> create an include directory with
** the name of the service no matther if the header file also have this name.
**
**/
