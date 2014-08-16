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
** ID:  S02010CL6                                                   **
** +Revision: 1.59 +                                                **
** +Date: 2005/07/01 12:28:32 +                                     **
** point to point protocol defintions                               **
**********************************************************************
EOC*/

#ifndef _GDD_PPP_H_
#define _GDD_PPP_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PPP_VERSION "0.0.1"

#include <sciopta.h>
#include <sys/errno.h>
//#include <ossys/scobj.h>

#include <container/list.h>

#include <sdd/sdd.h>
#include <sdd/sdd.msg>

#if defined IPS_CNF_PPP_LOGD_ENABLED && IPS_CNF_PPP_LOGD_ENABLED == 1
#include <logd/logd.h>
#endif

#define HDLC_PROT			0x0001

/** automaton definitions
  */

/** automaton events */
#define PPP_EVENT_UP            0
#define PPP_EVENT_DOWN          1
#define PPP_EVENT_OPEN          2
#define PPP_EVENT_CLOSE         3
#define PPP_EVENT_TO_PLUS       4
#define PPP_EVENT_TO_MINUS      5
#define PPP_EVENT_RCR_PLUS      6
#define PPP_EVENT_RCR_MINUS     7
#define PPP_EVENT_RCA           8
#define PPP_EVENT_RCN           9
#define PPP_EVENT_RTR           10
#define PPP_EVENT_RTA           11
#define PPP_EVENT_RUC           12
#define PPP_EVENT_RXJ_PLUS      13
#define PPP_EVENT_RXJ_MINUS     14
#define PPP_EVENT_RXR           15
#define PPP_EVENT_KILL         	99

/** automaton states */
#define PPP_STATE_INITIAL       0
#define PPP_STATE_STARTING      1
#define PPP_STATE_CLOSED        2
#define PPP_STATE_STOPPED       3
#define PPP_STATE_CLOSING       4
#define PPP_STATE_STOPPING      5
#define PPP_STATE_REQ_SENT      6
#define PPP_STATE_ACK_RCVD      7
#define PPP_STATE_ACK_SENT      8
#define PPP_STATE_OPENED        9

/** automaton defaults */
#define PPP_RESTART_CONFIG_COUNT	10
#define PPP_RESTART_REJECT_COUNT	10
#define PPP_RESTART_TERM_COUNT		10
#define PPP_RESTART_ECHO_COUNT		3 

/** auotmaton hook and methodes */
  typedef struct ppp_automaton_s ppp_automaton_t;
  typedef struct ppp_automatonOps_s ppp_automatonOps_t;
#if defined IPS_CNF_PPP_LOGD_ENABLED && IPS_CNF_PPP_LOGD_ENABLED == 1
#define PPP_AUTOMATON_PROPS \
    sc_errcode_t error; \
    char name[SC_NAME_MAX+1]; \
    flags_t flags; \
    int unit; \
    \
    __u16 protocol; \
    \
    sc_poolid_t ctrPool;\
    sc_poolid_t outPool;\
    \
    logd_t NEARPTR logd; \
    \
    int curState; \
    int preState; \
    \
    container_t NEARPTR optMeMust; \
    container_t NEARPTR optMeShould; \
    container_t NEARPTR optReqMust; \
    container_t NEARPTR optReqShould; \
    \
    sdd_netbuf_t NEARPTR acks; \
    sdd_netbuf_t NEARPTR rejs; \
    sdd_netbuf_t NEARPTR nacks; \
    \
    container_t NEARPTR upper; \
    sc_pid_t transport; \
    sc_pid_t lower; \
    \
    int ident; \
    int foreign; \
    int resendTmo; \
    sc_tmoid_t configTmoId; \
    sc_tmoid_t rejectTmoId; \
    sc_tmoid_t termTmoId; \
    int restartConfigCount; \
    int restartRejectCount; \
    int restartTermCount; \
    int restartConfigCountDefault; \
    int restartRejectCountDefault; \
    int restartTermCountDefault
#else
#define PPP_AUTOMATON_PROPS \
    sc_errcode_t error; \
    char name[SC_NAME_MAX+1]; \
    flags_t flags; \
    int unit; \
    \
    __u16 protocol; \
    \
    sc_poolid_t ctrPool;\
    sc_poolid_t outPool;\
    \
    int curState; \
    int preState; \
    \
    container_t NEARPTR optMeMust; \
    container_t NEARPTR optMeShould; \
    container_t NEARPTR optReqMust; \
    container_t NEARPTR optReqShould; \
    \
    sdd_netbuf_t NEARPTR acks; \
    sdd_netbuf_t NEARPTR rejs; \
    sdd_netbuf_t NEARPTR nacks; \
    \
    container_t NEARPTR upper; \
    sc_pid_t transport; \
    sc_pid_t lower; \
    \
    int ident; \
    int foreign; \
    int resendTmo; \
    sc_tmoid_t configTmoId; \
    sc_tmoid_t rejectTmoId; \
    sc_tmoid_t termTmoId; \
    int restartConfigCount; \
    int restartRejectCount; \
    int restartTermCount; \
    int restartConfigCountDefault; \
    int restartRejectCountDefault; \
    int restartTermCountDefault
#endif
#define PPP_AUTOMATON_OPS(automaton_t) \
    void (*destroy) (automaton_t NEARPTR NEARPTR self); \
    void (*thisLayerUp) (automaton_t NEARPTR self); \
    void (*thisLayerDown) (automaton_t NEARPTR self); \
    void (*thisLayerStarted) (automaton_t NEARPTR self); \
    void (*thisLayerFinished) (automaton_t NEARPTR self); \
    void (*sndReq) (automaton_t NEARPTR self); \
    void (*sndAck) (automaton_t NEARPTR self); \
    void (*sndRej) (automaton_t NEARPTR self); \
    void (*sndTermReq) (automaton_t NEARPTR self); \
    void (*sndTermAck) (automaton_t NEARPTR self); \
    void (*sndCodeRej) (automaton_t NEARPTR self); \
    void (*sndProtRej) (automaton_t NEARPTR self); \
    void (*illegalEvent) (automaton_t NEARPTR self)

  struct ppp_automaton_s {
    sc_msgid_t msgId;
    ppp_automatonOps_t *ops;
      PPP_AUTOMATON_PROPS;
  };

  struct ppp_automatonOps_s {
    sc_msgid_t msgId;
      PPP_AUTOMATON_OPS (ppp_automaton_t);
  };

/** ppp automation event handler */
  void ppp_eventHandler (ppp_automaton_t NEARPTR hook, int event);


/** ppp packets 
  */
  typedef struct ppp_packet {
    __u8 code;
    __u8 id;
    __u16 len;
  } ppp_packet_t;

  typedef struct ppp_packetCodeRej {
    ppp_packet_t packet;
    __u8 badCode;
    __u8 rcvdId;
    __u16 rcvdLen;
    __u8 rcvdData[1];
  } ppp_packetCodeRej_t;

  typedef struct ppp_packetProtRej {
    ppp_packet_t packet;
    __u16 badProt;
    __u8 rcvdData[1];
  } ppp_packetProtRej_t;

  typedef struct ppp_packetMagicText {
    ppp_packet_t packet;
    __u32 magic;
    __u8 text[1];
  } ppp_packetMagicText_t;

#define PPP_CODE_00		0x00
#define PPP_CODE_CONF_REQ	0x01
#define PPP_CODE_CONF_ACK	0x02
#define PPP_CODE_CONF_NACK	0x03
#define PPP_CODE_CONF_REJ	0x04
#define PPP_CODE_TERM_REQ	0x05
#define PPP_CODE_TERM_ACK	0x06
#define PPP_CODE_CODE_REJ	0x07
#define PPP_CODE_PROT_REJ	0x08
#define PPP_CODE_ECHO_REQ	0x09
#define PPP_CODE_ECHO_REP	0x0a
#define PPP_CODE_DISC_REQ	0x0b
#define PPP_CODE_IDENT		0x0c
#define PPP_CODE_TIME		0x0d
#define PPP_CODE_RESET_REQ	0x0e
#define PPP_CODE_RESET_ACK	0x0f


/** some tools 
  */
  void ppp_clrMsgQueue (void);

/** send a event to a automaton */
  void ppp_eventTx (int event, sc_pid_t layer, sc_poolid_t plid);

/** send event to all upper automatons */
  void
    ppp_eventToAllUpper (int event, container_t NEARPTR upper, sc_poolid_t plid);


/** ppp options  
  */

/** basic option header */
  typedef struct ppp_option {
    __u8 type;
    __u8 len;
  } ppp_option_t;

/** option packing */
  sdd_netbuf_t NEARPTR ppp_packetOpen (__u8 code, __u8 id, __u16 prot,
				   sc_poolid_t plid);
  void ppp_packetPushOpt (sdd_netbuf_t NEARPTR  netbuf, ppp_option_t NEARPTR option);
  ppp_option_t NEARPTR ppp_packetPopOpt (sdd_netbuf_t NEARPTR netbuf);
  void ppp_packetClose (sdd_netbuf_t NEARPTR  netbuf);
  void ppp_packetIterateOpt (void *data, sdd_netbuf_t NEARPTR netbuf,
			     void (*proc) (void *data,ppp_option_t NEARPTR o));

  ppp_option_t NEARPTR ppp_packetCheckOpt (sdd_netbuf_t NEARPTR netbuf,
				    ppp_option_t NEARPTR opt);


/** ppp ioctl structs 
  */
  typedef struct ppp_idle {
    time_t xmitIdle;		/* time since last NP packet sent */
    time_t recvIdle;		/* time since last NP packet received */
  } ppp_idle_t;


/** ppp configuration 
  */
#define PPP_OPTION_MUST		1
#define PPP_OPTION_SHOULD	2
#define PPP_OPTION_ME		4
#define PPP_OPTION_REQ		8
  int ppp_optionAdd (ppp_option_t NEARPTR opt, flags_t flags, sc_pid_t to);

/** ppp deamon 
 */
  int ppp_daemon (int argc, char *argv[]);

/** some shell commands able to use it together with ppp_deamon 
 */
  int chat (int argc, char *argv[]);

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: ppp.h,v $
** Revision 1.59  2005/07/01 12:28:32  christian
** Merged SCIOPTA_1_7_2 to MAIN
**
** Revision 1.58.4.1  2005/06/10 06:25:59  bastian
** fixes for Tasking M16C
**
** Revision 1.58  2004/12/10 07:25:30  christian
** Remove scobj completly!
**
** Revision 1.57  2004/10/07 07:37:42  bastian
** fixed NEAR * to NEARPTR
**
** Revision 1.56  2004/10/04 12:27:27  bastian
** started to change for m16c
**
** Revision 1.55  2004/07/03 13:18:08  bastian
** fix
**
** Revision 1.54  2004/07/01 10:18:07  bastian
** fixed castings
** fixed LOGD handling
**
** Revision 1.53  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 1.52  2003/07/30 05:59:05  christian
** sdd_netBuffer -> sdd_netbuf
**
** Revision 1.51  2003/03/18 15:04:26  christian
** Cleaned.
**
** Revision 1.50  2003/03/14 14:51:01  bastian
** fixes for ADS
**
** Revision 1.49  2003/02/27 09:42:18  christian
** Bugfixed the remove timeout stuff.
**
** Revision 1.48  2003/02/24 13:26:21  christian
** Implemented Echo for link down detection.
**
** Revision 1.47  2003/01/30 10:55:00  christian
** A better way to add options.
**
** Revision 1.46  2003/01/23 11:05:32  christian
** Start new option strategy first for lcp
**
** Revision 1.45  2003/01/21 13:59:16  christian
** Bugfixes: ppp_eventTx and ppp_eventToAllUpper changed -> hdlc
** Changes: ppp_optionAdd (.) can now specify if in request or only for Ack.
**
** Revision 1.44  2003/01/16 13:52:36  christian
** PPP is using now the inpPool, ctrPool and outPool by name.
**
** Revision 1.43  2003/01/03 16:11:21  christian
** Weekend :-))
**
** Revision 1.42  2002/11/21 14:52:08  christian
** Added chat to the ppp tool suite.
**
** Revision 1.41  2002/11/19 15:55:53  christian
** Clean logging.
**
** Revision 1.40  2002/11/18 15:49:42  christian
** Started with pap.
** Linux like log.
**
** Revision 1.39  2002/11/13 15:20:12  christian
** Fixed the misterious delay in the ppp connection phase.
** --> Did not send the open event to ipcp.
** --> Automaton had a bad transition.
**
** Revision 1.38  2002/11/01 12:50:13  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 1.37  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 1.36  2002/09/23 12:50:38  christian
** Incr/Decr can cause a resource dead lock in some special cases.
** --> replace Incr/Decr with Release and Dublicate (dup, dup2) to be
**     able to handle on the fly objects (files/directories in a filesystem)
**
** Revision 1.35  2002/09/06 15:03:58  christian
** Rewritten based now completly on bios net messages and methodes.
** --> using sdd_netbuf_t instead of the ugly ips_freighter_t
**
** Revision 1.34  2002/09/03 15:10:11  christian
** Adjust to the new bios concept.
**
** Revision 1.33  2002/08/21 15:35:14  christian
** On shutdown -> clear the message queue of each control protocol layer.
**
** Revision 1.32  2002/08/15 15:03:46  christian
** Work on a clean shutdown. Intermediate steps.
**
** Revision 1.31  2002/08/14 15:08:22  christian
** Added the possibility to force killing each layer by "hand".
**
** Revision 1.30  2002/08/13 15:21:07  christian
** A cleaned up new version with a pppd
**
** Revision 1.29  2002/08/07 14:57:32  christian
** Split up ppp in a generic ppp part and a hdlc part.
** Started with a ppp configurator. Conception phase.
** Removed pppd this will be a shell part later on.
**
** Revision 1.28  2002/08/06 14:17:28  christian
** Clean logging.
**
** Revision 1.27  2002/08/05 12:39:42  christian
** Logd infos
**
** Revision 1.26  2002/07/30 15:17:05  christian
** A clean boot up of the ppp stack.
**
** Revision 1.25  2002/07/29 13:49:49  christian
** Cleaned up the entry point interfaces of the different layers.
**
** Revision 1.24  2002/07/29 12:24:50  christian
** removed ppp_ipv4InterfaceUp -> this depends on ipcp
**
** Revision 1.23  2002/07/23 15:32:27  christian
** Prepared ppp driver to send splitted freighters.
**
** Revision 1.22  2002/07/19 14:52:26  christian
** moved over the ncpOptionGetNext and ipcpOptionGetNext to
** ppp_packetOptGetNext.
**
** Revision 1.21  2002/07/19 11:13:15  christian
** NCP means network control protocol and LCP/CHAP/PAP are not NCPs
** -> removed ncp_ and NCP_ befor generic methode-, structur- and define
**    names and replaced this with ppp_ and PPP_
**
** Revision 1.20  2002/07/18 07:57:53  christian
** Changed ppp_addOption/ppp_rmOption to standard naming
** ppp_optionAdd/ppp_optionRm
**
** Revision 1.19  2002/07/17 14:35:26  christian
** A cleaner request strategy, implemented add/remov options to a NCP.
**
** Revision 1.18  2002/06/20 16:46:01  christian
** nacks are still missing.
**
** Revision 1.17  2002/06/19 15:25:27  christian
** made a function for writing a ppp message to ttys. ready for protocol
** reject.
**
** Revision 1.16  2002/06/19 13:37:03  christian
** LCP has now echo funktionality (magic works fine now).
**
** Revision 1.15  2002/06/18 14:40:46  christian
** added ipcp layer.
**
** Revision 1.14  2002/06/17 15:21:22  christian
** Intermediate
**
** Revision 1.13  2002/06/07 14:57:04  christian
** .
**
** Revision 1.12  2002/06/05 17:06:28  christian
** Removed ppp_start and ppp_stop, is replaced by the generic call
** network_start and network_stop in net/device.
**
** Revision 1.11  2002/06/04 15:28:14  christian
** Start/stop of PPP implemented.
** Fixed registration of lcp.
** Fixed ppp_eventToAllUpper call -> container get NULL back if no more items.
**
** Revision 1.10  2002/06/03 13:31:10  christian
** Fixed some interface changeings.
**
** Revision 1.9  2002/05/30 15:07:30  christian
** A running starting point for the ppp device development.
**
** Revision 1.8  2002/05/29 13:51:52  christian
** Made open able to give back a copy of itself. If open a device allways
** use the device which is returned by open.
**
** Revision 1.7  2002/02/25 12:56:54  christian
** New container.
**
** Revision 1.6  2002/01/24 09:00:47  christian
** IPS interface has changed. Fixed this.
**
** Revision 1.5  2002/01/10 14:56:19  christian
** A protocol to pid resolver added. Can now add dynamical and in runtime new
** upper layers like LCP, PAP, CHAP, IPCP, ...
**
** Revision 1.4  2002/01/09 16:21:28  christian
** Separated the statemachine (automaton) from ppp.
**
** Revision 1.3  2002/01/09 10:45:48  christian
** Started with the LCP automaton.
**
** Revision 1.2  2002/01/08 14:43:50  christian
** Can now configure which serial driver ppp should use.
**
** Revision 1.1.1.1  2002/01/08 07:39:17  christian
** first revision
**
**
**/
