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
** ID:  S02010CL2                                                   **
** +Revision: 1.40 +                                                **
** +Date: 2005/07/01 12:28:32 +                                     **
** point to point protocol messages                                 ** 
**********************************************************************
EOC*/


#ifndef _GDD_PPP_MSG_
#define _GDD_PPP_MSG_

#ifdef __cplusplus
extern "C" {
#endif

#include <sciopta.h>
#include <ossys/defines.h>
#include <ips/ppp.h>
  
/*
**  Messages send between the PPP layers 
*/
  
#define PPP_CONFIG		(PPP_MSG_BASE)
#define PPP_CONFIG_REPLY	(PPP_CONFIG + 1)
  typedef struct ppp_config_s {
    sc_msgid_t id;
    sc_errcode_t error;
  } ppp_config_t;
  
#define PPP_EVENT		(PPP_MSG_BASE + 2)
  typedef struct ppp_event_s {
    sc_msgid_t id;
    int eventId;
//    union sc_msg *data;
  } ppp_event_t;

#define PPP_CONFIG_TMO			(PPP_MSG_BASE + 4)
#define PPP_REJECT_TMO			(PPP_MSG_BASE + 5)
#define PPP_TERM_TMO			(PPP_MSG_BASE + 6)
#define PPP_ECHO_TMO			(PPP_MSG_BASE + 7)
  typedef struct ppp_tmo_s {
    sc_msgid_t id;
  } ppp_tmo_t;

#define PPP_REGISTER		(PPP_MSG_BASE + 8)
#define PPP_REGISTER_REPLY	(PPP_REGISTER + 1)
#define PPP_UNREGISTER		(PPP_MSG_BASE + 10)
#define PPP_UNREGISTER_REPLY	(PPP_UNREGISTER + 1)
  typedef struct ppp_register_s {
    sc_msgid_t id;
    sc_errcode_t error;
  } ppp_register_t;

#define PPP_PROT_CODE_ADD	(PPP_MSG_BASE + 12)
#define PPP_PROT_CODE_ADD_REPLY	(PPP_PROT_CODE_ADD + 1)
#define PPP_PROT_CODE_RM	(PPP_MSG_BASE + 14)
#define PPP_PROT_CODE_RM_REPLY	(PPP_PROT_CODE_RM + 1)
  typedef struct ppp_protCode_s {
    sc_msgid_t id;
    sc_errcode_t error;
    __u16 code;
    sc_msgid_t rcvId;
  } ppp_protCode_t;


/*
**  Messages send via configurator
*/
#define PPP_OPTION_ADD		(PPP_MSG_BASE + 16)
#define PPP_OPTION_ADD_REPLY	(PPP_OPTION_ADD + 1)
  typedef struct ppp_optionAdd_s {
    sc_msgid_t id;
    sc_errcode_t error;
    flags_t prio;
    ppp_option_t NEARPTR opt;
  } ppp_optionAdd_t;

#define PPP_OPTION_RM		(PPP_MSG_BASE + 18)
#define PPP_OPTION_RM_REPLY	(PPP_OPTION_RM + 1)
  typedef struct ppp_optionRm_s {
    sc_msgid_t id;
    sc_errcode_t error;
    __u8 type;
  } ppp_optionRm_t;

#define PPP_PROTOCOL_START		(PPP_MSG_BASE + 20)
#define PPP_PROTOCOL_START_REPLY	(PPP_PROTOCOL_START + 1)
  typedef struct ppp_protocolStart_s {
    sc_msgid_t id;
    sc_errcode_t error;
    int unit;
    int debuglevel;
    char transport[SC_NAME_MAX + 1];
    char lower[SC_NAME_MAX + 1];
    char data[SC_NAME_MAX + 1];
    void (*exitCode) (sc_pid_t pid);
  } ppp_protocolStart_t;

#define PPP_KILL		(PPP_MSG_BASE + 26)
#define PPP_KILL_REPLY		(PPP_KILL + 1)
  typedef struct ppp_start_s {
    sc_msgid_t id;
    sc_errcode_t error;
  } ppp_start_t;

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: ppp.msg,v $
** Revision 1.40  2005/07/01 12:28:32  christian
** Merged SCIOPTA_1_7_2 to MAIN
**
** Revision 1.39.4.1  2005/06/10 06:25:59  bastian
** fixes for Tasking M16C
**
** Revision 1.39  2004/03/09 10:37:41  christian
** Changed a little the document structure.
** And removed leftovers (I found while writting the pppman.txt :-)
**
** Revision 1.38  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 1.37  2003/07/30 05:59:05  christian
** sdd_netBuffer -> sdd_netbuf
**
** Revision 1.36  2003/06/23 11:28:29  christian
** Added missing header files with in header files.
**
** Revision 1.35  2003/03/18 15:04:26  christian
** Cleaned.
**
** Revision 1.34  2003/03/14 14:51:01  bastian
** fixes for ADS
**
** Revision 1.33  2003/02/27 09:42:18  christian
** Bugfixed the remove timeout stuff.
**
** Revision 1.32  2003/02/26 09:12:41  christian
** Kill a running or a nearly running stack is possible now.
**
** Revision 1.30  2003/02/24 13:26:21  christian
** Implemented Echo for link down detection.
**
** Revision 1.29  2002/11/01 12:50:13  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 1.28  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 1.27  2002/10/16 12:03:43  christian
** Renamed mother to device.
**
** Revision 1.26  2002/09/11 07:45:33  christian
** Added the internet protocol stack link layer to ipcp
**
** Revision 1.25  2002/09/06 15:03:58  christian
** Rewritten based now completly on bios net messages and methodes.
** --> using sdd_netbuf_t instead of the ugly ips_freighter_t
**
** Revision 1.24  2002/08/23 15:03:41  christian
** Added a const char * data to the parameter list of ech ctrl protocol
** process (i.e. hdlc_process, lcp_process, ipcp_process).
**
** Revision 1.23  2002/08/22 15:09:08  christian
** .
**
** Revision 1.22  2002/08/16 15:17:35  christian
** Smoot shutdown :-)
**
** Revision 1.21  2002/08/15 15:03:46  christian
** Work on a clean shutdown. Intermediate steps.
**
** Revision 1.20  2002/08/14 15:08:22  christian
** Added the possibility to force killing each layer by "hand".
**
** Revision 1.19  2002/08/13 15:21:07  christian
** A cleaned up new version with a pppd
**
** Revision 1.18  2002/08/09 15:14:36  christian
** Every Protocol Layer will be started from pppconf. If it is a statical
** system the neccessary processes musst be started in the scipota.con.
**
** Revision 1.17  2002/08/07 14:57:32  christian
** Split up ppp in a generic ppp part and a hdlc part.
** Started with a ppp configurator. Conception phase.
** Removed pppd this will be a shell part later on.
**
** Revision 1.16  2002/07/30 15:17:05  christian
** A clean boot up of the ppp stack.
**
** Revision 1.15  2002/07/23 15:32:27  christian
** Prepared ppp driver to send splitted freighters.
**
** Revision 1.14  2002/07/19 11:13:15  christian
** NCP means network control protocol and LCP/CHAP/PAP are not NCPs
** -> removed ncp_ and NCP_ befor generic methode-, structur- and define
**    names and replaced this with ppp_ and PPP_
**
** Revision 1.13  2002/07/17 14:35:26  christian
** A cleaner request strategy, implemented add/remov options to a NCP.
**
** Revision 1.12  2002/06/20 16:46:01  christian
** nacks are still missing.
**
** Revision 1.11  2002/06/17 15:21:22  christian
** Intermediate
**
** Revision 1.10  2002/06/05 17:07:32  christian
** Added the protocol remove message id with the same struct as for
** protocol add message id.
**
** Revision 1.9  2002/06/04 15:28:14  christian
** Start/stop of PPP implemented.
** Fixed registration of lcp.
** Fixed ppp_eventToAllUpper call -> container get NULL back if no more items.
**
** Revision 1.8  2002/01/11 17:15:48  christian
** Integrate the hdlc driver.
**
** Revision 1.7  2002/01/11 10:36:46  christian
** Removed all unnecessary things.
**
** Revision 1.6  2002/01/10 14:56:19  christian
** A protocol to pid resolver added. Can now add dynamical and in runtime new
** upper layers like LCP, PAP, CHAP, IPCP, ...
**
** Revision 1.5  2002/01/09 16:21:28  christian
** Separated the statemachine (automaton) from ppp.
**
** Revision 1.4  2002/01/09 10:45:48  christian
** Started with the LCP automaton.
**
** Revision 1.3  2002/01/08 14:43:50  christian
** Can now configure which serial driver ppp should use.
**
** Revision 1.2  2002/01/08 11:47:47  christian
** PPP added.
**
** Revision 1.1.1.1  2002/01/08 07:39:17  christian
** first revision
**
**
**/
