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
** ID:  S02010CL1                                                   **
** +Revision: 1.25 +                                                **
** +Date: 2004/07/01 10:18:07 +                                     **
** ip control protocol definitinos                                  ** 
**********************************************************************
EOC*/

#ifndef _GDD_IPCP_H_
#define _GDD_IPCP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sciopta.h>
#include <ips/ppp.msg>
#include <ips/ppp.h>

#define IPCP_PROT		0x8021
#define IP_PROT			0x0021

  /* PPP_CONFIG/PPP_CONFIG_REPLY */
  typedef struct ipcp_config_s {
    ppp_config_t config;
    int max_configure;
    int max_failure;
    int max_terminate;
    int restart; 
    int accept_local;
    int accept_remote;
  } ipcp_config_t;
  
#define IPCP_OPTION_COMP_PROT	0x02
  typedef struct ipcp_optionCompProt {
    ppp_option_t option;
    __u16 prot;
    __u8 data[1];
  } ipcp_optionCompProt_t;

#define IPCP_OPTION_ADDRESS	0x03
  typedef struct ipcp_optionAddress {
    ppp_option_t option;
    __u8 addr[4];
  } ipcp_optionAddress_t;

#define IPCP_OPTION_1TH_DNS	0x81
  typedef struct ipcp_option1thDns {
    ppp_option_t option;
    __u8 addr[4];
  } ipcp_option1thDns_t;
 
#define IPCP_OPTION_1TH_NBNS	0x82
  typedef struct ipcp_option1thNbns {
    ppp_option_t option;
    __u8 addr[4];
  } ipcp_option1thNbns_t;
  
#define IPCP_OPTION_2TH_DNS	0x83
  typedef struct ipcp_option2thDns {
    ppp_option_t option;
    __u8 addr[4];
  } ipcp_option2thDns_t;

#define IPCP_OPTION_2TH_NBNS	0x84
  typedef struct ipcp_option2thNbns {
    ppp_option_t option;
    __u8 addr[4];
  } ipcp_option2thNbns_t;

#define IPCP_OPTION_NETMASK	0x90
  typedef struct ipcp_optionNetmask {
    ppp_option_t option;
    __u8 addr[4];
  } ipcp_optionNetmask_t;


#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: ipcp.h,v $
** Revision 1.25  2004/07/01 10:18:07  bastian
** fixed castings
** fixed LOGD handling
**
** Revision 1.24  2003/08/18 12:54:29  christian
** Removed ips/util.h is now all in netinet/in.h
**
** Revision 1.23  2003/07/30 05:59:05  christian
** sdd_netBuffer -> sdd_netbuf
**
** Revision 1.22  2003/03/18 15:04:26  christian
** Cleaned.
**
** Revision 1.21  2003/03/14 14:51:01  bastian
** fixes for ADS
**
** Revision 1.20  2003/02/27 09:42:18  christian
** Bugfixed the remove timeout stuff.
**
** Revision 1.19  2002/12/06 12:21:38  christian
** Wrong includes and wrong paths.
**
** Revision 1.18  2002/11/19 15:55:53  christian
** Clean logging.
**
** Revision 1.17  2002/11/18 15:49:42  christian
** Started with pap.
** Linux like log.
**
** Revision 1.16  2002/11/01 12:50:13  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 1.15  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 1.14  2002/09/06 15:03:58  christian
** Rewritten based now completly on bios net messages and methodes.
** --> using sdd_netbuf_t instead of the ugly ips_freighter_t
**
** Revision 1.13  2002/09/03 15:10:11  christian
** Adjust to the new bios concept.
**
** Revision 1.12  2002/08/23 15:03:41  christian
** Added a const char * data to the parameter list of ech ctrl protocol
** process (i.e. hdlc_process, lcp_process, ipcp_process).
**
** Revision 1.11  2002/08/15 15:03:46  christian
** Work on a clean shutdown. Intermediate steps.
**
** Revision 1.10  2002/08/13 15:21:07  christian
** A cleaned up new version with a pppd
**
** Revision 1.9  2002/08/09 15:14:36  christian
** Every Protocol Layer will be started from pppconf. If it is a statical
** system the neccessary processes musst be started in the scipota.con.
**
** Revision 1.8  2002/07/30 15:17:05  christian
** A clean boot up of the ppp stack.
**
** Revision 1.7  2002/07/29 13:49:49  christian
** Cleaned up the entry point interfaces of the different layers.
**
** Revision 1.6  2002/07/23 15:32:27  christian
** Prepared ppp driver to send splitted freighters.
**
** Revision 1.5  2002/07/19 15:04:26  christian
** ipcp_device gets an additional parameter which defines the interface
** name
**
** Revision 1.4  2002/07/19 11:13:15  christian
** NCP means network control protocol and LCP/CHAP/PAP are not NCPs
** -> removed ncp_ and NCP_ befor generic methode-, structur- and define
**    names and replaced this with ppp_ and PPP_
**
** Revision 1.3  2002/07/17 14:35:26  christian
** A cleaner request strategy, implemented add/remov options to a NCP.
**
** Revision 1.2  2002/06/19 13:37:03  christian
** LCP has now echo funktionality (magic works fine now).
**
** Revision 1.1  2002/06/18 14:40:46  christian
** added ipcp layer.
**
** Revision 1.7  2002/06/17 15:21:22  christian
** Intermediate
**
** Revision 1.6  2002/06/07 14:57:04  christian
** .
**
** Revision 1.5  2002/06/06 15:23:31  christian
** Started option negotiation.
**
** Revision 1.4  2002/06/05 17:08:42  christian
** Added structurs to scann the lcp packets.
**
** Revision 1.3  2002/06/04 15:28:14  christian
** Start/stop of PPP implemented.
** Fixed registration of lcp.
** Fixed ppp_eventToAllUpper call -> container get NULL back if no more items.
**
** Revision 1.2  2002/01/11 17:15:48  christian
** Integrate the hdlc driver.
**
** Revision 1.1  2002/01/10 14:56:19  christian
** A protocol to pid resolver added. Can now add dynamical and in runtime new
** upper layers like LCP, PAP, CHAP, IPCP, ...
**
**
**
**/
