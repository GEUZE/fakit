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
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S04029BS1                                                    **
** +Revision: 2.44.10.1 +                                                 **
** +Date: 2009/02/17 09:23:12 +                                     **
** Global IPS configuration                                         **
**********************************************************************
** This file contains configuration options for all parts of IPS.
** If you like to change some option you may do it here, or
** provide your own configuration and add
** -DIPS_CONF="<name of your conf>"
** to CFLAGS.
**
EOC*/

#define __IPS_CNF_H__

/*
 * Do not modify
 */ 
#define IPS_CNF_DBL				0
#define IPS_CNF_LIST			        1
#define IPS_CNF_HASH                            2

/*
 * link
 */
#define IPS_CNF_LINK_RAW_ENABLED                1
#define IPS_CNF_LINK_POOL                       ""
#define IPS_CNF_LINK_TMO                        SC_FATAL_IF_TMO

#define IPS_CNF_LINK_LOGD_ENABLED               0 
#define IPS_CNF_LINK_LOGD_LEVEL                 LOGD_ALL

/*
 * ipv4
 */
#define IPS_CNF_IPV4_PKT_CACHE_SIZE            4 /* 1 .. 16  */
#define IPS_CNF_IPV4_FRAG_ENABLED              0 /* 0 | 1    */
#define IPS_CNF_IPV4_FORWARD_ENABLED           0 /* 0 | 1    */
#define IPS_CNF_IPV4_CHAINS_ENABLED            0 /* 0 | 1    */
#define IPS_CNF_IPV4_ARP_ENABLED               1 /* 0 | 1    */
#define IPS_CNF_IPV4_ARP_DEV_ENABLED           1 /* 0 | 1    */
#define IPS_CNF_IPV4_ARP_CACHE_SIZE            8 /* 1 .. 16  */
#define IPS_CNF_IPV4_ARP_MAX_PROBES            2 /* 1 .. 255 */
#define IPS_CNF_IPV4_ROUTER_ENABLED            1 /* 0 | 1    */
#define IPS_CNF_IPV4_ROUTER_DEV_ENABLED        1 /* 0 | 1    */
#define IPS_CNF_IPV4_ROUTER_ROUTE_GET_ENABLED  1 /* 0 | 1    */
#define IPS_CNF_IPV4_ROUTER_ROUTE_WAIT_ENABLED 1 /* 0 | 1    */
#define IPS_CNF_IPV4_STAT_ENABLED              0 /* 0 | 1    */
#define IPS_CNF_IPV4_RAW_ENABLED               0 /* 0 | 1    */

#define IPS_CNF_IPV4_LOGD_ENABLED              0
#define IPS_CNF_IPV4_LOGD_LEVEL                LOGD_ALL

/*
 * icmp
 */
#define IPS_CNF_ICMP_TMO                        SC_FATAL_IF_TMO
#define IPS_CNF_ICMP_STAT_ENABLED               0 /* 0 | 1    */
#define IPS_CNF_ICMP_DEV_ENABLED                1 /* 0 | 1    */

#define IPS_CNF_ICMP_LOGD_ENABLED               0
#define IPS_CNF_ICMP_LOGD_LEVEL                 LOGD_ALL

/*
 * igmp
 */
#define IPS_CNF_IGMP_MAX_APPL                   4 /* > 0 */

#define IPS_CNF_IGMP_LOGD_ENABLED               0
#define IPS_CNF_IGMP_LOGD_LEVEL                 LOGD_ALL

/*
 * udp
 */
#define IPS_CNF_UDP_CONN_LIST                   IPS_CNF_DBL 
#define IPS_CNF_UDP_CHKSUM_ENABLED              1   /* 0 | 1             */
#define IPS_CNF_UDP_SOCKET_ENUM                 0   /* 0 | 1             */
#define IPS_CNF_UDP_STAT_ENABLED                0   /* 0 | 1             */
#define IPS_CNF_UDP_MULTIPLE_GROUPS             0   /* 0 | 1             */

#define IPS_CNF_UDP_LOGD_ENABLED                0   /* 0 | 1             */
#define IPS_CNF_UDP_LOGD_LEVEL                  LOGD_ALL

/*
 * tcp
 */
#define IPS_CNF_TCP_CONN_LIST                   IPS_CNF_DBL 
#define IPS_CNF_TCP_SWQ                         1 /* 0 | 1    */
#define IPS_CNF_TCP_MULTIPLE_POOLS              1 /* 0 | 1    */
#define IPS_CNF_TCP_ALG_CORK                    1 /* 0 | 1    */
#define IPS_CNF_TCP_ALG_PIGGYPACK               1 /* 0 | 1    */
#define IPS_CNF_TCP_ALG_NAGLE                   1 /* 0 | 1    */
#define IPS_CNF_TCP_ALG_SILLYWINDOW             1 /* 0 | 1    */
#define IPS_CNF_TCP_OPTION_TSTAMP               0 /* 0 | 1    */
#define IPS_CNF_TCP_STAT_ENABLED                0 /* 0 | 1    */
#define IPS_CNF_TCP_SOCKET_ENUM                 0 /* 0 | 1    */

#define IPS_CNF_TCP_LOGD_ENABLED                0   /* 0 | 1             */
#define IPS_CNF_TCP_LOGD_LEVEL                  LOGD_ALL

/*
 * dns
 */
#define IPS_CNF_DNS_POOL                        SC_DEFAULT_POOL
#define IPS_CNF_DNS_TMO                         SC_FATAL_IF_TMO
#define IPS_CNF_DNS_MAX_ANSWERS                 10  /* 1 ..              */
#define IPS_CNF_DNS_NO_DBENTRIES                3   /* 1 ..              */ 
#define IPS_CNF_IPV4_ARP_MAX_PROBES             2   /* 1 .. 255          */

/*
 * vpn
 */
#define IPS_CNF_VPN_CRYPTO_ENABLED              0   /* 0 | 1             */

/*
 * PPP
 */
#define IPS_CNF_PPP_LOGD_ENABLED                0   /* 0 | 1             */
#define IPS_CNF_PPP_LOGD_LEVEL                  LOGD_ALL
 
/*
 * Stream
 */
#define IPS_CNF_BUF_SIZE                 	512 /* 1 ..              */
