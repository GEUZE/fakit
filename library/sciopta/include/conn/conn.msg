/* -*-c-*-
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
** ID: S030321GB1                                                   **
** +Revision: 1.5 +                                                 **
** +Date: 2009/01/23 14:43:20 +                                     **
** connector messages                                               **
**********************************************************************
*/
#ifndef _CONN_MSG_
#define _CONN_MSG_

#include <sciopta.h>
#include <sciopta.msg>
#include <sdd/sdd.msg>

EXTERN_C_BEGIN

typedef enum {
  RS232 = 1,
  RS485,
  ETHERNET,
  CAN
} lnktype_t;

typedef struct strmsg_s{
  sc_msgid_t id;
  __s8 data[1];
}strmsg_t;

#define CON_INIT  (SCC_MSG_BASE+8)
struct conn_init_s{
  sc_msgid_t id;
  sc_pid_t lnkPid;
  sc_pid_t initPid;
  const char FARPTR const FARPTR remotePath;
  int nbr_remote_proc;
  lnktype_t lnkType;
  __u32 nodeAddr;
};
typedef struct conn_init_s conn_init_t;

struct conn_msg_s{
  sc_msgid_t id;
  sc_procPathGetMsgReply_t * addresse;
  sc_procPathGetMsgReply_t * sender;
  strmsg_t * fwd;
  int size;
};
typedef struct conn_msg_s conn_msg_t;


#define CON_ERROR    (SCC_MSG_BASE+9)
#define LNK_ERROR    (SCC_MSG_BASE+10)
struct conn_error_s{
  sc_msgid_t id;
  sc_errcode_t nbr;
  sc_msg_t msg;
};
typedef struct conn_error_s conn_error_t;

enum {
  CON_NOERROR_SND = 0,
  CON_ERROR_SND_TIMEOUT,
  CON_ERROR_SND_NRDY,
  CON_ERROR_SND_BAD_SNDR,
  CON_NOERROR_RCV,
  CON_ERROR_RCV_BAD_ADDR,
  CON_ERROR_RCV_BAD_SNDR,
  LNK_ERROR_DRV_NOT_FOUND,
  LNK_ERROR_DEVMAN_NOT_FOUND,
  LNK_ERROR_RCV_BAD_CRC,
  LNK_ERROR_RCV_ACK_AFTER_TMO
};

#define CON_SND	(SCC_MSG_BASE+2)
struct conn_snd_s{
  sc_msgid_t id;
  sc_procPathGetMsgReply_t * addresse;
  sc_procPathGetMsgReply_t * sender;
  strmsg_t * fwd;
};
typedef struct conn_snd_s conn_snd_t;

#define CON_LINKCFG (SCC_MSG_BASE+6)
#define CON_LINKCFG_REPLY (SCC_MSG_BASE+7)

/*
** serial bus (rs485) link layer configuration
*/
struct conn_serialbus_lnkcfg_s{
  sc_msgid_t id;
  const char FARPTR drvname;	
  sc_poolid_t plid;
  sc_bufsize_t msgSizeMax;
  sc_ticks_t timeout;
  __u32 baud;
  __u8 nodeAddr;
};
typedef struct conn_serialbus_lnkcfg_s conn_serialbus_lnkcfg_t;

/*
** peertopeer (rs232) link layer configuration
*/
struct conn_peertopeer_lnkcfg_s{
  sc_msgid_t id;
  sc_pid_t connPid;
  const char FARPTR drvname;	
  sc_poolid_t plid;
  sc_bufsize_t msgSizeMax;
  sc_ticks_t timeout;
  __u32 baud;
};
typedef struct conn_peertopeer_lnkcfg_s conn_peertopeer_lnkcfg_t;

/*
** CAN bus link layer configuration
*/
typedef struct conn_canbus_lnkcfg_s{
  sc_msgid_t id;
  sc_pid_t connPid;
  char drvname[32];
  sc_bufsize_t msgSizeMax;
  sc_ticks_t timeout;
  __u32 baud;
  __u16 nodeAddr;
} conn_canbus_lnkcfg_t;

/*
** TCP link layer configuration
*/
struct conn_tcp_lnkcfg_s{
  sc_msgid_t id;
  sc_pid_t connPid;
  const __u8 * ip;
  const __u8 * mac;
  int port;
  __u32 ackTmo;
};
typedef struct conn_tcp_lnkcfg_s conn_tcp_lnkcfg_t;


/*
** TCP
*/
struct conn_tcpcfg_s{
  sc_msgid_t id;
  sc_pid_t connPid;
  const __u8 FARPTR ip;
  int portClient;
  int portServer;
};
typedef struct conn_tcpcfg_s conn_tcpcfg_t;

struct lnk_tcp_fork_s{
  sc_msgid_t id;
  sc_pid_t connPid;
  sdd_obj_t socket;
};
typedef struct lnk_tcp_fork_s lnk_tcp_fork_t;

#define CON_SNDTMO_REPLY (SCC_MSG_BASE+4)
#define CON_SNDNRDY_REPLY (SCC_MSG_BASE+5)
struct conn_acktmo_s{
  sc_msgid_t id;
  sc_pid_t connPid;
};
typedef struct conn_acktmo_s conn_acktmo_t;

#define CON_DSTNODEADDR (SCC_MSG_BASE+11)
struct conn_dstNodeAddr_s{
  sc_msgid_t id;
  __u32 dstNodeAddr;
};
typedef struct conn_dstNodeAddr_s conn_dstNodeAddr_t;


#define CON_SNDACK_REPLY (SCC_MSG_BASE+3)
#define LNK_FORK_INIT (SCC_MSG_BASE+13)
#define LNK_FORK_INIT_REPLY (SCC_MSG_BASE+14)
#define FORK_SDD_ERROR (SCC_MSG_BASE+15)
#define LNK_ACK_SND_TMO (SCC_MSG_BASE+16)

EXTERN_C_END

#endif /* _CONN_MSG_ */
