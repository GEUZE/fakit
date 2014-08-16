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
** ID: S01332CL7                                                    **
** +Revision: 2.55 +                                                **
** +Date: 2008/01/24 00:20:34 +                                     **
** Contains IP version 4 header description.                        **
**********************************************************************
EOC*/

#ifndef _IPS_IPV4_H_
#define _IPS_IPV4_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/defines.h>
#include <sdd/sdd.msg>
#include <ips/addr.h>
#include <ips/link.h>

  /** IP Protocols */
#define IPV4_P_ALL 		0
#define IPV4_P_ICMP 	1
#define IPV4_P_IGMP 	2
#define IPV4_P_IP4IP4 	4
#define IPV4_P_TCP 		6
#define IPV4_P_UDP		17
#define IPV4_P_IP6IP4	41

#define IPV4_FLAG_CE	0x8000	/* Flag: "Congestion"           */
#define IPV4_FLAG_DF	0x4000	/* Flag: "Don't Fragment"       */
#define IPV4_FLAG_MF	0x2000	/* Flag: "More Fragments"       */
#define IPV4_OFFSET		0x1FFF	/* "Fragment Offset" part       */

/* IP socket options */
#define IP_TOS				3 
#define IP_TTL				4 
#define IP_MULTICAST_IF			9 
#define IP_MULTICAST_TTL		10
#define IP_MULTICAST_LOOP		11
#define IP_ADD_MEMBERSHIP		12
#define IP_DROP_MEMBERSHIP		13
#define IP_DONTFRAGMENT			14
#define IP_ADD_SOURCE_MEMBERSHIP	15
#define IP_DROP_SOURCE_MEMBERSHIP	16
#define IP_BLOCK_SOURCE			17
#define IP_UNBLOCK_SOURCE		18
#define IP_PKTINFO			19


#define IPS_IP_VERSION(ip) \
  (ip->vhl >> 4)

#define IPV4_HEADER_LEN(ip) \
  ((ip->vhl & 0xf) << 2)

#define IPV4_FLAGS(ip) \
  (ip->ffo & 0x30)

#define IPV4_FRAG(ip) \
  (ip->ffo & 0x1f)

  /** For direct access to the IP header */
#define IPV4_HEADER_SIZE	20
  typedef struct ipv4_header {
    __u8 vhl;		/**< 0-3: version; 4-7: header length */
    __u8 codepoint;	/**< type of service not supported */
    __u16 len;		/**< len of header _and_ payload */
    __u16 id;		/**< identification, seems to be used only for frags */
    __u16 ffo;		/**< 0-2: flags; 3-15: fragment offset */
    __u8 ttl;		/**< time to life */
    __u8 protocol;	/**< which upper protocol is waiting on */
    __u16 chksum;	/**< checksum */
    __u8 src[4];	/**< senders IP address */
    __u8 dst[4];	/**< receivers IP address */
    __u8 payload[40];	/**< max size of IP header is 60 Bytes (options) */
  } ipv4_header_t;

  /** 
   * IP version 4 process definition 
   *
   * \param[in]  protocolMax Maximal number of upper protocols (udp, tcp, 
   *             icmp)
   */
  void ipv4_process (int protocolMax);

  /**
   * If chains are supported a process can register to IP version 4 process
   * to get the netbufs on the path up to the upper protocols.
   */
  int ipv4_chainRegister (void);

  /**
   * With this upper protocols will register to IP version 4 process.
   *
   * \param[in]  protocol    protocol descriptor (see ips/link.msg)
   * \return                 -1 on error (see sc_miscErrnoGet ())
   */
  int ipv4_protocolRegister (ips_protocol_t NEARPTR NEARPTR protocol);

  /**
   * An upper protocol can unregister from the IP version 4 process with
   * this function.
   *
   * \param[in]  protocol    protocol descriptor (see ips/link.msg)
   * \return                 -1 on error (see sc_miscErrnoGet ())
   */
  int ipv4_protocolUnregister (ips_protocol_t NEARPTR protocol);

  /**
   * To initialize a netbuf for IP version 4 process.
   *
   * \param[in]  netbuf      Allocated netbuf which shall be initialized
   * \param[in]  codepoint   IP version 4 codepoint
   * \param[in]  id          Fragmentation ID
   * \param[in]  ffo         Fragmentation offset and flags
   * \param[in]  ttl         Packets time to live 
   * \param[in]  protocol    Protocol identifier
   * \param[in]  src         Source IP address
   * \param[in]  dst         Destination IP address
   * \param[in]  datalen     Length of the data.
   */
  void ipv4_bufInit (sdd_netbuf_t NEARPTR  netbuf, __u8 codepoint, __u16 id,
			__u16 ffo, __u8 ttl, __u8 protocol, __u8 * src,
			__u8 * dst, int datalen);
  /**
   * To allocate and initialize a netbuf for IP version 4 process.
   *
   * \param[in]  codepoint   IP version 4 codepoint
   * \param[in]  id          Fragmentation ID
   * \param[in]  ffo         Fragmentation offset and flags
   * \param[in]  ttl         Packets time to live 
   * \param[in]  protocol    Protocol identifier
   * \param[in]  src         Source IP address
   * \param[in]  dst         Destination IP address
   * \param[in]  datalen     Length of the data
   * \param[in]  reserve     Reserve data length at the tail of netbuf.
   * \return                 netbuf or NULL on error (see sc_miscErrnoGet ()).
   */
  sdd_netbuf_t NEARPTR ipv4_bufAlloc (__u8 codepoint, __u16 id, __u16 ffo,
				     __u8 ttl, __u8 protocol, __u8 * src,
				     __u8 * dst, int datalen, int reserve,
				     sc_poolid_t plid, sc_ticks_t tmo);

  /**
   * Get a upper protocol by name from IP version 4 process.
   *
   * \param[in]  name        Upper protocol name
   * \return                 Procotol object or NULL on error 
   *                         (see sc_miscErrnoGet ()).
   */
  sdd_obj_t NEARPTR ipv4_getProtocol (const char *name);

  /**
   * Network byte order address to ASCII string.
   *
   * \param[in]  addr        Network byte order address
   * \param[in]  str         String to store the address
   * \param[in]  len         String lengthString
   * \return                 -1 on error (see sc_miscErrnoGet ())
   */
  int ipv4_ntoa (__u8 *addr, char *str, int len);

  /**
   * ASCII string to network byte order address.
   *
   * \param[in]  str         String to store the address
   * \param[in]  addr        Network byte order address
   * \return                 -1 on error (see sc_miscErrnoGet ())
   */
  int ipv4_aton (const char *str, __u8 *addr);
  
  /** MIB tag infos */
#define IPV4_TAG_FORWARDING 1
#define IPV4_TAG_DEFAULT_TTL 2
#define IPV4_TAG_IN_RECEIVES 3
#define IPV4_TAG_IN_HDR_ERRORS 4
#define IPV4_TAG_IN_ADDR_ERRORS 5
#define IPV4_TAG_FORW_DATAGRAMS 6
#define IPV4_TAG_IN_UNKNOWN_PROTOS 7
#define IPV4_TAG_IN_DISCARDS 8
#define IPV4_TAG_IN_DELIVERS 9
#define IPV4_TAG_OUT_REQUESTS 10
#define IPV4_TAG_OUT_DISCARDS 11
#define IPV4_TAG_OUT_NO_ROUTES 12
#define IPV4_TAG_REASM_TIMEOUT 13
#define IPV4_TAG_REASM_REQDS 14
#define IPV4_TAG_REASM_OKS 15
#define IPV4_TAG_REASM_FAILS 16
#define IPV4_TAG_FRAG_OKS 17
#define IPV4_TAG_FRAG_FAILS 18
#define IPV4_TAG_FRAG_CREATES 19
  
  /** 
   * raw ip process definition.
   *
   * \param[in]  maxConn     Maximum application sockets
   * \param[in]  maxPktQueue Maximum packets per socket which are not acked
   */
  void rawv4_process (int maxConn, int maxPktQueue);

  /** 
   * raw ip process definition.
   *
   * \param[in]  maxPkts     Maximum number of started fragmentations, must be
   *                         2^x number
   * \param[in]  maxFrag     Maximum fragments per packet
   */
  void fragv4_process (int maxPkts, int maxFrag);

#ifdef __cplusplus
}
#endif

#endif

