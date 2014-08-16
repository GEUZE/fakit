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
** ID: S01332CL3                                                    **
** +Revision: 2.25 +                                                **
** +Date: 2005/07/01 12:28:31 +                                     **
** ICMP header definition                                           **
**********************************************************************
EOC*/

#ifndef _ICMP_H_
#define _ICMP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/types.h>
#include <ips/link.msg>

/** ICMP Types */
#define ICMP_ECHO_REP			0
#define ICMP_DEST_UNREACH		3
#define ICMP_SRC_QUENCH			4
#define ICMP_REDIRECT			5
#define ICMP_ALTERNATE_HOST_ADDR	6
#define ICMP_ECHO_REQ			8
#define ICMP_ROUTER_ADVERTISEMENT	9
#define ICMP_ROUTER_SOLICITATION	10
#define ICMP_TIME_EXCEEDED		11
#define ICMP_PARAMETER_PROBLEM		12
#define ICMP_TIMESTAMP_REQ		13
#define ICMP_TIMESTAMP_REP		14
#define ICMP_INFORMATION_REQ		15
#define ICMP_INFORMATION_REP		16
#define ICMP_ADDR_MASK_REQ		17
#define ICMP_ADDR_MASK_REP		18
#define ICMP_TRACEROUTE			30
#define ICMP_CONVERSION_ERR		31
#define ICMP_MOBILE_HOST_REDIRECT	32
#define ICMP_IPV6_WHERE_ARE_YOU		33
#define ICMP_IPV6_I_AM_HERE		34
#define ICMP_MOBILE_REG_REQ		35
#define ICMP_MOBILE_REG_REP		36
#define ICMP_DOMAIN_NAME_REQ		37
#define ICMP_DOMAIN_NAME_REP		38
#define ICMP_SKIP_ALGO_DISC_PROTOCOL	39
#define ICMP_PHOTURIS_SECURITY_FAILS	40

  /**
   * ICMP process definition
   */ 
  void icmp_process (int protocolMax);
  
  /** 
   * ICMP Header 
   */
  typedef struct icmp_header_s {
    __u8 type;
    __u8 code;
    __u16 chksum;
    __u8 data[1];
  } icmp_header_t;

  typedef struct icmp_user_s {
    __u16 id;
    __u16 seq;
    __u8 data[1];
  } icmp_user_t;

  typedef struct icmp_echo_s {
    __u16 id;
    __u16 seq;
    __u8 data[1];
  } icmp_echo_t;

  int icmp_protocolRegister (ips_protocol_t NEARPTR NEARPTR protocol);
  int icmp_protocolUnregister (ips_protocol_t NEARPTR protocol);

  /**
   * To be like linux 
   */
#define PROTO_ICMP_ECHO		8	
#define PROTO_ICMP_TIMESTAMP	13
#define PROTO_ICMP_INFORMATION	15
#define PROTO_ICMP_ADDR_MASK	17
#define PROTO_ICMP_MOBILE_REG	35
#define PROTO_ICMP_DOMAIN_NAME	37 
  struct icmp_head {
    __u8 type;
    __u8 code;
    __u16 chksum;
    __u16 id;
    __u16 seq;    
  };
  
  struct icmp_echo {
    struct icmp_head head;
    __u8 data[1];
  };

  struct icmp_timestamp {
    struct icmp_head head;
  };

  struct icmp_information {
    struct icmp_head head;
  };

  struct icmp_addr_mask {
    struct icmp_head head;
  };

  struct icmp_mobile_reg {
    struct icmp_head head;
  };

  struct icmp_domain_name {
    struct icmp_head head;
  };
  

#define ICMP_TAG_IN_MSGS 1 
#define ICMP_TAG_IN_ERRORS 2 
#define ICMP_TAG_IN_DEST_UNREACHS 3 
#define ICMP_TAG_IN_TIME_EXCDS 4 
#define ICMP_TAG_IN_PARM_PROBS 5 
#define ICMP_TAG_IN_SRC_QUENCHS 6 
#define ICMP_TAG_IN_REDIRECTS 7 
#define ICMP_TAG_IN_ECHOS 8 
#define ICMP_TAG_IN_ECHO_REPS 9 
#define ICMP_TAG_IN_TIMESTAMPS 10 
#define ICMP_TAG_IN_TIMESTAMPS_REPS 11 
#define ICMP_TAG_IN_ADDR_MASKS 12 
#define ICMP_TAG_IN_ADDR_MASKS_REPS 13 
#define ICMP_TAG_OUT_MSGS 14
#define ICMP_TAG_OUT_ERRORS 15
#define ICMP_TAG_OUT_DEST_UNREACHS 16
#define ICMP_TAG_OUT_TIME_EXCDS 17 
#define ICMP_TAG_OUT_PARM_PROBS 18
#define ICMP_TAG_OUT_SRC_QUENCHS 19 
#define ICMP_TAG_OUT_REDIRECTS 20 
#define ICMP_TAG_OUT_ECHOS 21 
#define ICMP_TAG_OUT_ECHO_REPS 22 
#define ICMP_TAG_OUT_TIMESTAMPS 23 
#define ICMP_TAG_OUT_TIMESTAMPS_REPS 24 
#define ICMP_TAG_OUT_ADDR_MASKS 25 
#define ICMP_TAG_OUT_ADDR_MASKS_REPS 26 

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: icmp.h,v $
** Revision 2.25  2005/07/01 12:28:31  christian
** Merged SCIOPTA_1_7_2 to MAIN
**
** Revision 2.24.2.1  2005/05/25 10:24:27  bastian
** x
**
** Revision 2.24  2005/01/20 14:23:06  christian
** Replace list_t with array_t for protocol holding, because this is more or
** less statical.
**
** Revision 2.23  2004/06/14 14:11:30  christian
** Bugfix the tagging system.
**
** Revision 2.22  2004/06/11 14:02:35  christian
** Switch over to new ioctl command (tagging system).
**
** Revision 2.21  2004/06/01 12:55:29  christian
** Do string to binary translation family specific.
**
** Revision 2.20  2004/05/26 06:05:23  christian
** Header files documented and spell checks.
**
** Revision 2.19  2004/05/12 09:12:17  christian
** Moved all defines used in ips and posix api to ips, because posix does
** build up on the ips/sdd api!
**
** Revision 2.18  2004/04/23 06:13:56  christian
** Did rename to make source more readable.
**
** Revision 2.17  2004/01/27 14:44:52  christian
** Bugfixed a wrong naming.
**
** Revision 2.16  2004/01/27 13:32:21  christian
** Changed namings of some libsc functions to sc_miscXxxYyy
**
** Revision 2.15  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 2.14  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 2.13  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 2.12  2003/06/24 13:26:12  christian
** Added icmp sockets and a ping (reference implementation).
**
** Revision 2.11  2003/03/18 15:04:25  christian
** Cleaned.
**
** Revision 2.10  2003/03/18 10:50:22  christian
** Cleaned.
**
** Revision 2.9  2003/01/15 16:18:08  christian
** Added statistical info (handle == NULL, else you will get socket infos)
**
** Revision 2.8  2003/01/15 14:56:30  christian
** MIB statistics added.
**
** Revision 2.7  2002/11/01 12:50:13  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 2.6  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 2.5  2002/10/29 14:27:07  christian
** Fixed various bugs. Better error handling.
**
** Revision 2.4  2002/09/13 14:11:33  christian
** Send echo replies
**
** Revision 2.3  2002/09/12 13:53:28  christian
** Bugfixes
**
** Revision 2.2  2002/01/31 12:21:56  christian
** Fixed tcp bug (found it in the postArp process).
**
** Revision 2.1  2002/01/04 13:30:43  christian
** generic view to net device.
**
** Revision 2.0  2002/01/03 15:54:46  christian
** Changed the freighter structure.
** This makes the net devices interface IP independent. It is now possible to
** add IPv6 and/or other protocol families.
**
** Revision 1.1.1.1  2001/11/28 12:43:04  christian
** emigrate to cvs
**
 *
 *
*/
