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
** ID:  S02010CL4                                                   **
** +Revision: 1.27 +                                                **
** +Date: 2004/07/01 10:18:07 +                                     **
** link control protocol definitinos                                ** 
**********************************************************************
EOC*/

#ifndef _GDD_LCP_H_
#define _GDD_LCP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sciopta.h>
#include <ips/ppp.msg>
#include <ips/ppp.h>

/* Protocol identification
 */  
#define LCP_PROT			0xc021  
#define UNSUPP_PROT			0xffff  

/* LCP configuration
 */
  /* PPP_CONFIG/PPP_CONFIG_REPLY */
  typedef struct lcp_config_s {
    ppp_config_t config;
    int max_configure;
    int max_failure;
    int max_terminate;
    int restart;
    int echo_failure;
    int echo_interval;
  } lcp_config_t; 

/* LCP specific options 
 */
#define LCP_OPTION_MRU			0x01
  typedef struct lcp_optionMRU {
    ppp_option_t	option;
    __u16		mru;
  } lcp_optionMRU_t;
  
#define LCP_OPTION_ACCM			0x02
  typedef struct lcp_optionACCM {
    ppp_option_t	option;
    __u16		asyncCharMap[2];
  } lcp_optionACCM_t;

#define LCP_OPTION_AUTH			0x03
  typedef struct lcp_optionAuth {
    ppp_option_t	option;
    __u16		protCode;
  } lcp_optionAuth_t;

#define LCP_OPTION_MAGIC		0x05	
  typedef struct lcp_optionMagic {
    ppp_option_t	option;
    __u16		magic[2];
  } lcp_optionMagic_t;

#define LCP_OPTION_PFC			0x07	
  typedef struct lcp_optionPFC {
    ppp_option_t	option;
  } lcp_optionPFC_t;

#define LCP_OPTION_ACFC			0x08	
  typedef struct lcp_optionACFC {
    ppp_option_t	option;
  } lcp_optionACFC_t;

#define LCP_OPTION_CALLBACK		0x0d  
  typedef struct lcp_optionCallback {
    ppp_option_t	option;
    __u8		operation;
    __u8		message[1];
  } lcp_optionCallback_t;
  
#define LCP_OPTION_MRRU			0x11  
  typedef struct lcp_optionMRRU {
    ppp_option_t	option;
  } lcp_optionMRRU_t;
  
#define LCP_OPTION_SSN			0x12  
  typedef struct lcp_optionSSN {
    ppp_option_t	option;
  } lcp_optionSSN_t;
  
#define LCP_OPTION_ED			0x13  
  typedef struct lcp_optionED {
    ppp_option_t	option;
  } lcp_optionED_t;
  

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: lcp.h,v $
** Revision 1.27  2004/07/01 10:18:07  bastian
** fixed castings
** fixed LOGD handling
**
** Revision 1.26  2003/07/30 05:59:05  christian
** sdd_netBuffer -> sdd_netbuf
**
** Revision 1.25  2003/06/23 11:28:29  christian
** Added missing header files with in header files.
**
** Revision 1.24  2003/03/18 15:04:26  christian
** Cleaned.
**
** Revision 1.23  2003/03/14 14:51:01  bastian
** fixes for ADS
**
** Revision 1.22  2003/02/27 09:42:18  christian
** Bugfixed the remove timeout stuff.
**
** Revision 1.21  2003/02/17 15:58:37  christian
** Protocol reject implemented.
** Bad packet skip implemented.
**
** Revision 1.20  2002/12/06 12:21:38  christian
** Wrong includes and wrong paths.
**
** Revision 1.19  2002/11/01 12:50:13  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 1.18  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 1.17  2002/09/06 15:03:58  christian
** Rewritten based now completly on bios net messages and methodes.
** --> using sdd_netbuf_t instead of the ugly ips_freighter_t
**
** Revision 1.16  2002/09/03 15:10:11  christian
** Adjust to the new bios concept.
**
** Revision 1.15  2002/08/23 15:03:41  christian
** Added a const char * data to the parameter list of ech ctrl protocol
** process (i.e. hdlc_process, lcp_process, ipcp_process).
**
** Revision 1.14  2002/08/15 15:03:46  christian
** Work on a clean shutdown. Intermediate steps.
**
** Revision 1.13  2002/08/13 15:21:07  christian
** A cleaned up new version with a pppd
**
** Revision 1.12  2002/07/29 13:49:49  christian
** Cleaned up the entry point interfaces of the different layers.
**
** Revision 1.11  2002/07/19 11:13:15  christian
** NCP means network control protocol and LCP/CHAP/PAP are not NCPs
** -> removed ncp_ and NCP_ befor generic methode-, structur- and define
**    names and replaced this with ppp_ and PPP_
**
** Revision 1.10  2002/07/17 14:35:26  christian
** A cleaner request strategy, implemented add/remov options to a NCP.
**
** Revision 1.9  2002/06/19 13:37:03  christian
** LCP has now echo funktionality (magic works fine now).
**
** Revision 1.8  2002/06/18 14:40:46  christian
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
