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
** ID: S01338CL2                                                    **
** +Revision: 1.9 +                                                 **
** +Date: 2004/11/24 13:57:12 +                                     **
** DNS packet caputure                                              **
**********************************************************************
EOC*/

#ifndef _DNS_PCAP_H_
#define _DNS_PCAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ips/addr.h>
#include <sdd/sdd.msg>

#define DNS_NAME_MAX 255
#define DNS_LABLE_MAX 63

  /**
   * DNS parser base structure.
   */
  typedef struct dns_s {
    sc_msgid_t msgId;
    sc_errcode_t error;
  } dns_t;
  /**
   * DNS name holder structure.
   */
  typedef struct dns_name_s {
    dns_t parent;
    int noOf;
    struct dns_nameEntry {
      int ttl;
      __u8 name[DNS_NAME_MAX + 1];
    } entry[1];
  } dns_name_t;
  /**
   * DNS IP holder structure.
   */
  typedef struct dns_ip {
    dns_t parent;
    int noOf;
    struct dns_ipEntry {
      int ttl;
      ips_addr_t ip;
    } entry[1];
  } dns_ip_t;
  /**
   * DNS host holder structure.
   */
  typedef struct dns_host {
    dns_t parent;
    __u8 cpu[SC_NAME_MAX + 1];
    __u8 os[SC_NAME_MAX + 1];
  } dns_host_t;
  /**
   * DNS alias holder structure.
   */
  typedef struct dns_alias {
    dns_t parent;
    int noOf;
    struct dns_aliasEntry {
      __u8 name[DNS_NAME_MAX + 1];
      int noOf;
      __u8 alias[1][DNS_NAME_MAX + 1];
    } entry[1];
  } dns_alias_t;

  /**
   * Start the resolver in a process.
   * \param[in]  port    UDP DNS port.
   * \param[in]  tmo     Timeout while waiting for a response
   */
  void resolver_process (__u16 port, unsigned int tmo);
  /**
   * Do a query to a specified server. This function can be used to build
   * up an own resolver process.
   * \param[in]  server  Server to which the query will go.
   * \param[in]  port    Port to which you suppose the server is listening
   *                     Normaly this is port 53.
   * \param[in]  ident   Ident to get the answer of this session.
   * \param[in]  query   A ready to send query in a netbuf.
   * \param[in]  tmo     Timeout to wait for an answer of the server.
   * \return             NULL if no response or the response packet.
   */
  sdd_netbuf_t NEARPTR  dns_query (ips_addr_t * server, __u16 port, int ident, 
                            sdd_netbuf_t NEARPTR  query, sc_ticks_t tmo);
  /**
   * Make ip request ready for sending.
   * \param[in]  ident   Session ID
   * \param[in]  name    Name string
   * \return             NULL on error else a ready to send netbuf
   */
  sdd_netbuf_t NEARPTR dns_ipMkReq (__u16 ident, const char *name);
  /**
   * Make name request ready for sending.
   * \param[in]  ident   Session ID
   * \param[in]  ip      IP adddress
   * \return             NULL on error else a ready to send netbuf
   */
  sdd_netbuf_t NEARPTR dns_nameMkReq (__u16 ident, ips_addr_t * ip);
  /**
   * Make host request ready for sending.
   * \param[in]  ident   Session ID
   * \param[in]  cpu     CPU string
   * \param[in]  os      OS string
   * \return             NULL on error else a ready to send netbuf
   */
  sdd_netbuf_t NEARPTR dns_hostMkReq (__u16 ident, const char *cpu,
                               const char *os);
  /**
   * Make alias request ready for sending.
   * \param[in]  ident   Session ID
   * \param[in]  name    Alias string
   * \return             NULL on error else a ready to send netbuf
   */
  sdd_netbuf_t NEARPTR dns_aliasMkReq (__u16 ident, const char *name);
  /**
   * Cat two netbufs together.
   * \param[in]  pkt1    First netbuf
   * \param[in]  pkt2    Second netbuf
   * \return             Resulting netbuf on success
   */
  sdd_netbuf_t NEARPTR dns_catReq (sdd_netbuf_t NEARPTR  pkt1, sdd_netbuf_t NEARPTR  pkt2);
  /**
   * Check session ID.
   * \param[in]  pkt     Netbuf
   * \param[in]  ident   Session ID
   * \return             0 on success
   */
  int dns_identCheck (sdd_netbuf_t NEARPTR  pkt, __u16 ident);
  /**
   * Parse the names from received pkt.
   * \param[in]  pkt     Received netbuf
   * \return             NULL on error else a name holder structure
   */
  dns_name_t *dns_nameParse (sdd_netbuf_t NEARPTR  pkt);
  /**
   * Parse the IPs from received pkt.
   * \param[in]  pkt     Received netbuf
   * \return             NULL on error else a IP holder structure
   */
  dns_ip_t *dns_ipParse (sdd_netbuf_t NEARPTR  pkt);
  /**
   * Parse the host from received pkt.
   * \param[in]  pkt     Received netbuf
   * \return             NULL on error else a host holder structure
   */
  dns_host_t *dns_hostParse (sdd_netbuf_t NEARPTR  pkt);
  /**
   * Parse the alias from received pkt.
   * \param[in]  pkt     Received netbuf
   * \return             NULL on error else a alias holder structure
   */
  dns_alias_t *dns_aliasParse (sdd_netbuf_t NEARPTR  pkt);

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: parser.h,v $
** Revision 1.9  2004/11/24 13:57:12  bastian
** fixes for Cosmic
**
** Revision 1.8  2004/10/07 07:37:42  bastian
** fixed NEAR * to NEARPTR
**
** Revision 1.7  2004/10/04 12:27:27  bastian
** started to change for m16c
**
** Revision 1.6  2004/07/19 10:50:43  christian
** resolver is used for something different. we now use the quasi standard
** and name it resolver_process (like tcp_process, udp_process, ...).
**
** Revision 1.5  2004/07/16 13:24:46  christian
** Have now a set of function, that a customer could write an own resolver.
**
** Revision 1.4  2004/05/26 06:05:23  christian
** Header files documented and spell checks.
**
** Revision 1.3  2004/02/04 11:31:14  christian
** Replace pcap.h with parser.h
**
** Revision 2.9  2003/07/30 05:59:05  christian
** sdd_netBuffer -> sdd_netbuf
**
** Revision 2.8  2003/06/10 14:10:57  christian
** Some bugfixes.
**
** Revision 2.7  2003/03/18 15:04:25  christian
** Cleaned.
**
** Revision 2.6  2002/11/01 12:50:13  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 2.5  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 2.4  2002/10/03 13:02:17  christian
** A better request strategy to make name lookups.
**
** Revision 2.3  2002/09/20 14:56:17  christian
** Adjust to the new bios concept.
**
** Revision 2.2  2002/02/01 10:11:49  christian
** Moved old connect.h over to connect.msg. In the new connect.h the socket
** methodes are defined.
**
** Revision 2.1  2002/01/04 13:30:43  christian
** generic view to net device.
**
** Revision 2.0  2002/01/03 15:54:45  christian
** Changed the freighter structure.
** This makes the net devices interface IP independent. It is now possible to
** add IPv6 and/or other protocol families.
**
** Revision 1.4  2001/12/07 08:41:54  christian
** Changed the workflow of the resolver process.
**
** Revision 1.3  2001/12/06 08:13:42  christian
** Add ip to name resolution.
**
** Revision 1.2  2001/12/04 13:15:12  christian
** A new interface and a running version.
**
** Revision 1.1  2001/12/04 08:45:21  christian
** replace parser.h with .h
**
**
**
**/
