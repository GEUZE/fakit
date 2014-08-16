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
** ID: S01332CL12                                                   **
** +Revision: 2.45 +                                                **
** +Date: 2005/08/16 14:47:10 +                                     **
** TCP header definition                                            **
**********************************************************************
EOC*/

#ifndef _TCP_H_
#define _TCP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/defines.h>
#include <sdd/sdd.msg>
#include <ips/base.msg>
#include <ips/addr.h>
#include <ips/connect.h>

  
#define TCP_CONF_VERSION 1

typedef struct tcp_conf_s {
  int version;  
  int rtoAlgorithm;
  unsigned int rtoMin;
  unsigned int rtoMax;
  int maxConn;  
  int sndWnd;
  int rcvWnd;
  int timeWaitTmo;
  int defaultMss;
} tcp_conf_t;


  /**
   * User-settable options (used with setsockopt).
   */
#define	TCP_NODELAY	 1	/* Don't delay send to coalesce packets  */
#define	TCP_MAXSEG	 2	/* Set maximum segment size  */
#define TCP_CORK	 3	/* Control sending of partial frames  */
#define TCP_KEEPIDLE	 4	/* Start keeplives after this period */
#define TCP_KEEPINTVL	 5	/* Interval between keepalives */
#define TCP_KEEPCNT	 6	/* Number of keepalives before death */
#define TCP_SYNCNT	 7	/* Number of SYN retransmits */
#define TCP_LINGER2	 8	/* Life time of orphaned FIN-WAIT-2 state */
#define TCP_DEFER_ACCEPT 9	/* Wake up listener only when data arrive */
#define TCP_WINDOW_CLAMP 10	/* Bound advertised window */
#define TCP_INFO	 11	/* Information about this connection. */
#define	TCP_QUICKACK	 12	/* Bock/reenable quick ACKs.  */
#define	TCP_RTTMO	 13	/* Starting retransmission timeout.  */
  
  /**
   * User read/(write)able tags for TCP 
   */
#define TCP_TAG_RTO_ALGORITHMUS 1 
  /** sizeof data must be at least sizeof (__u32) */
#define TCP_TAG_RTO_MIN         2 
  /** sizeof data must be at least sizeof (__u32) */
#define TCP_TAG_RTO_MAX         3 
  /** sizeof data must be at least sizeof (__u32) */
#define TCP_TAG_MAX_CONN        4 
  /** sizeof data must be at least sizeof (__u32) */
#define TCP_TAG_ACTIVE_OPENS    5 
  /** sizeof data must be at least sizeof (__u32) */
#define TCP_TAG_PASSIVE_OPENS   6 
  /** sizeof data must be at least sizeof (__u32) */
#define TCP_TAG_ATTEMPT_FAILS   7 
  /** sizeof data must be at least sizeof (__u32) */
#define TCP_TAG_ESTAB_RESETS    8
  /** sizeof data must be at least sizeof (__u32) */
#define TCP_TAG_CURR_ESTAB      9
  /** sizeof data must be at least sizeof (__u32) */
#define TCP_TAG_IN_SEGS         10
  /** sizeof data must be at least sizeof (__u32) */
#define TCP_TAG_OUT_SEGS        11
  /** sizeof data must be at least sizeof (__u32) */
#define TCP_TAG_RETRANS_SEGS    12
  /** sizeof data must be at least sizeof (__u32) */
  
  /**
   * User read/(write)able tags for Sockets 
   */
#define TCP_SOCK_TAG_CONN_STATE         1
  /** sizeof data must be at least sizeof (__u32) */
#define TCP_SOCK_TAG_CONN_LOCAL_ADDRESS 2
  /** sizeof data must be at least sizeof (ips_addr_t) */
#define TCP_SOCK_TAG_CONN_LOCAL_PORT    3
  /** sizeof data must be at least sizeof (__u32) */
#define TCP_SOCK_TAG_CONN_REM_ADDRESS   4
  /** sizeof data must be at least sizeof (ips_addr_t) */
#define TCP_SOCK_TAG_CONN_REM_PORT      5
  /** sizeof data must be at least sizeof (__u32) */
  
  /**
   * TCP Options
   */
#define TCP_OPT_EOL	0x00
#define TCP_OPT_NOP	0x01
#define TCP_OPT_MSS	0x02
#define TCP_OPT_WSCALE	0x03
#define TCP_OPT_TSTAMP	0x08

  /**
   * TCP option header. This part is for all options the same.
   * \param type   Option name
   * \param len    Length of the option
   */
  typedef struct tcp_option_s {
    void *next;
    __u8 res1;
    __u8 res2;
    __u8 type;
    __u8 len;
  } tcp_option_t;
  
  /**
   * TCP NOP.
   */
#define TCP_NOP_SIZE	1
  typedef struct tcp_optionNop_s {
    tcp_option_t head;
  } tcp_optionNop_t;

  /**
   * TCP Maximum Segment Size option.
   * \param head    Option header
   * \param mss     Maximum Segment Size value
   */
#define TCP_MSS_SIZE	4
  typedef struct tcp_optionMss_s {
    tcp_option_t head;
    __u16 mss;
  } tcp_optionMss_t;
  
  /**
   * TCP Time Stamp option.
   * \param head    Option header
   * \param TSval   Value of time stamp
   * \param TSecr   Value of the received time stamp
   */
#define TCP_TSTAMP_SIZE	10 
  typedef struct tcp_optionTStamp_s {
    tcp_option_t head;
    __u32 TSval;
    __u32 TSecr;
  } tcp_optionTStamp_t;

  /**
   * TCP Window Scale option.
   * \param head    Option header
   * \param shifCnt * 2^shiftCnt
   */ 
#define TCP_WSCALE_SIZE 3
  typedef struct tcp_optionWScale_s {
    tcp_option_t head;
    __u8 shiftCnt;
  } tcp_optionWScale_s;

#define TCP_FLAG_FIN	0x01
#define TCP_FLAG_SYN	0x02
#define TCP_FLAG_RST	0x04
#define TCP_FLAG_PUSH	0x08
#define TCP_FLAG_ACK	0x10
#define TCP_FLAG_URGE	0x20
#define TCP_FLAG_CLR	0x00
#define TCP_HLEN_BITS	0xf0
#define TCP_RESERVED_BITS	0x0f

#define TCP_HP_BITS (~0x0f080000)

#define TCP_HLEN(head) (unsigned int) ((head->hlen & TCP_HLEN_BITS) >> 2)

  /**
   * TCP header.
   * \param srcPort source port
   * \param dstPort destination port
   * \param seq     Sequence number
   * \param ack     Ackknowlegd number
   * \param hlen    Header length
   * \param flags   TCP flags see above TCP_FLAG_XXX
   * \param window  Window size
   * \param chksum  Checksum
   * \param urgent  Urgent pointer
   */
#define TCP_HEADER_SIZE	20
  typedef struct tcp_header {
    __u16 srcPort; 
    __u16 dstPort; 
    __u32 seq; 
    __u32 ack; 
    __u8 hlen; 
    __u8 flags; 
    __u16 window; 
    __u16 chksum; 
    __u16 urgent; 
  } tcp_header_t;

  /**
   * TCP Process.
   * \param rtoAlgorithm Retransmission timeout algorithme
   * \param rtoMin       Minimal retransmission timeout
   * \param rtoMax       Maximal retransmission timeout
   * \param maxConn      Maximal connectinos (if -1: not limited)
   * \param sndWnd       Send window
   * \param rcvWnd       Receive window
   * \param timeWaitTmo  Time wait timeout (if 0: RFC violation) 
   */
#define TCP_MIB_TCP_RTO_ALGORITHM_OTHER		1
#define TCP_MIB_TCP_RTO_ALGORITHM_CONSTANT	2
#define TCP_MIB_TCP_RTO_ALGORITHM_RSRE		3
#define TCP_MIB_TCP_RTO_ALGORITHM_VANJ		4

  void tcp_process(const tcp_conf_t *conf);
//->  void tcp_process (int rtoAlgorithm, int rtoMin, int rtoMax, int maxConn,
//->                    int sndWnd, int rcvWnd, int timeWaitTmo);

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: tcp.h,v $
** Revision 2.45  2005/08/16 14:47:10  christian
** Time Stamp Option now working. But target does not yet calculate
** retransmit timeout upon the investigated round trip time.
**
** Revision 2.44  2005/08/15 09:07:56  christian
** Prepared to add multiple options.
**
** Revision 2.43  2005/07/13 14:21:07  christian
** Localate the dup ack problem.
**
** Revision 2.42  2004/11/24 13:57:12  bastian
** fixes for Cosmic
**
** Revision 2.41  2004/11/17 15:47:41  bastian
** changed tcp-config
**
** Revision 2.40  2004/06/15 11:17:24  christian
** All needed tags are implemented.
**
** Revision 2.39  2004/06/11 14:02:36  christian
** Switch over to new ioctl command (tagging system).
**
** Revision 2.38  2004/06/01 12:55:29  christian
** Do string to binary translation family specific.
**
** Revision 2.37  2004/05/28 05:40:07  christian
** Started with MIB injection.
**
** Revision 2.36  2004/05/12 09:12:18  christian
** Moved all defines used in ips and posix api to ips, because posix does
** build up on the ips/sdd api!
**
** Revision 2.35  2004/05/06 09:06:50  christian
** Bugfix the time wait tmo handler.
**
** Revision 2.34  2004/04/23 06:13:56  christian
** Did rename to make source more readable.
**
** Revision 2.33  2004/03/11 10:19:27  christian
** Made tcp socket structure smaller by moving all global stuff to tcp_t
** structur (-> tcp hook).
**
** Revision 2.32  2004/03/11 06:27:10  christian
** Replaced netinet/in.h with ossys/swap.h
**
** Revision 2.31  2004/03/09 15:12:41  christian
** Start with a kompact tcp.
**
** Revision 2.30  2004/01/27 14:33:53  christian
** Tcp uses now sc_miscChksumContd.
** Tcp does now correctly handle the maxConn (max number of connections).
** Did wrongly renamed struct member chksum to sc_miscChksum :-((
**
** Revision 2.29  2004/01/27 13:32:22  christian
** Changed namings of some libsc functions to sc_miscXxxYyy
**
** Revision 2.28  2004/01/22 07:59:12  christian
** Remove all __inline
**
** Revision 2.27  2003/12/24 09:54:55  christian
** Intermediate.
**
** Revision 2.26  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 2.25  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 2.24  2003/08/18 12:54:29  christian
** Removed ips/util.h is now all in netinet/in.h
**
** Revision 2.23  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 2.22  2003/07/30 05:59:05  christian
** sdd_netBuffer -> sdd_netbuf
**
** Revision 2.21  2003/03/18 15:04:26  christian
** Cleaned.
**
** Revision 2.20  2003/03/14 14:51:02  bastian
** fixes for ADS
**
** Revision 2.19  2003/03/11 14:59:28  christian
** Bugfixes.
**
** Revision 2.18  2003/03/07 10:26:34  christian
** Hooked the statical information and pass them to the functions.
**
** Revision 2.16  2003/03/05 16:07:38  christian
** TCP configurable no of connections and packet size.
**
** Revision 2.15  2003/02/04 09:57:21  christian
** Remove freighter.h
**
** Revision 2.14  2003/01/15 16:18:08  christian
** Added statistical info (handle == NULL, else you will get socket infos)
**
** Revision 2.13  2003/01/15 14:56:30  christian
** MIB statistics added.
**
** Revision 2.12  2002/11/01 12:50:13  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 2.11  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 2.10  2002/10/24 14:03:35  christian
** Implemented a dynamic way to get the mtu/mru for a connection.
**
** Revision 2.9  2002/10/22 14:35:28  christian
** Bugfixes. A correct implementation of connect, listen/bind/accept :-)
**
** Revision 2.8  2002/10/15 14:03:02  christian
** Packet sending without slow start and congestion avoidance.
**
** Revision 2.7  2002/09/30 15:09:30  christian
** Intermediate.
**
** Revision 2.6  2002/09/27 15:00:39  christian
** Correct connection scenario implemented & tested.
**
** Revision 2.5  2002/09/26 15:13:03  christian
** intermediate.
**
** Revision 2.4  2002/09/25 14:59:35  christian
** Intermediate.
**
** Revision 2.3  2002/09/24 15:08:57  christian
** Re-implementation of tcp based on the new bios concept
**
** Revision 2.2  2002/02/05 12:27:34  christian
** Make the socket init funktion available.
**
** Revision 2.1  2002/01/04 13:30:43  christian
** generic view to net device.
**
** Revision 2.0  2002/01/03 15:54:46  christian
** Changed the freighter structure.
** This makes the net devices interface IP independent. It is now possible to
** add IPv6 and/or other protocol families.
**
** Revision 1.2  2001/12/11 13:24:26  christian
** Removed ips/data.h cause do not need any more.
**
** Revision 1.1.1.1  2001/11/28 12:43:04  christian
** emigrate to cvs
**
 *
 *
**/
