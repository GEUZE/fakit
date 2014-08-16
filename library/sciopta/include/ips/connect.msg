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
** ID: S03217CL5                                                    **
** +Revision: 2.29 +                                                 **
** +Date: 2005/11/22 09:24:05 +                                     **
** Basic IPS message interface.                                     **
**********************************************************************
EOC*/

#ifndef _IPS_CONNECT_MSG_
#define _IPS_CONNECT_MSG_

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/types.h>
#include <ips/base.msg>
#include <sdd/sdd.msg>
#include <ips/addr.h>
#include <container/dbl_list.h>
/*
** User Interface 
** The udp data block is send first followed by more data blocks if any.
*/

#define IPS_CONNECT		(IPS_CONNECT_MSG_BASE)
#define IPS_CONNECT_REPLY	(IPS_CONNECT + 1)
  typedef struct ips_connect_s {
    sdd_baseMessage_t base;
    __u16 dstPort;
    ips_addr_t dstAddr;
  } ips_connect_t;
  
#define IPS_BIND		(IPS_CONNECT_MSG_BASE + 2)
#define IPS_BIND_REPLY		(IPS_BIND + 1)
  typedef struct ips_bind_s {
    sdd_baseMessage_t base;
#define IPS_BIND_TO_EPHEMERAL_PORT	0xffff
    __u16 srcPort;
    ips_addr_t srcAddr;
  } ips_bind_t;
  
#define IPS_LISTEN		(IPS_CONNECT_MSG_BASE + 4)
#define IPS_LISTEN_REPLY	(IPS_LISTEN + 1)
  typedef struct ips_listen_s {
    sdd_baseMessage_t base;
    int backlog;
  } ips_listen_t;

#define IPS_ACCEPT		(IPS_CONNECT_MSG_BASE + 6)
#define IPS_ACCEPT_REPLY	(IPS_ACCEPT + 1)
  typedef struct ips_accept_s {
    ips_connect_t connect;
    dbl_t alist;		/* private member to link acceptors */
  } ips_accept_t;

#define IPS_SET_OPTION		(IPS_CONNECT_MSG_BASE + 8)
#define IPS_SET_OPTION_REPLY	(IPS_SET_OPTION + 1)

#define IPS_GET_OPTION		(IPS_CONNECT_MSG_BASE + 10)
#define IPS_GET_OPTION_REPLY	(IPS_GET_OPTION + 1)
  typedef struct ips_option_s {
    sdd_baseMessage_t base;
    int level;
    int optname;
    size_t optlen; /* BS42: 200305 */
    char optval[1];
  } ips_option_t;
 
#define IPS_ACK			(IPS_CONNECT_MSG_BASE + 12)
  typedef struct ips_ack_s {
    sdd_baseMessage_t base;
    size_t size;
  } ips_ack_t;
  
#ifdef __cplusplus
}
#endif
#endif

