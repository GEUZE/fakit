/* -*-c++-*-
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
** (c) 2002,2003 Sciopta GmbH, Litronic AG/ Schweiz                 **
**                                                                  **
**********************************************************************
** ID: S01103BS45                                                   **
** +Revision: 2.31 +                                                **
** +Date: 2008/11/11 10:01:40 +                                     ** 
** Kernel messages                                                  **
**********************************************************************
*/
#ifndef _SCIOPTA_MSG_
#define _SCIOPTA_MSG_
#ifndef __ASM_INCLUDED__
#include <sciopta.h>
#endif

#ifdef __IAR_SYSTEMS_ICC__
/* disable MISRA C Rule 14*/
/* We need to use char without signed/unsigned in order to be
** compatible with the libc
*/
#pragma diag_suppress=Pm012
#endif

/*
************************************************** process related messages
*/
#ifndef __KKERNEL__
#define SC_PROCPRIOCREATEMSG   (SC_MSG_BASE+0x100)
#define SC_PROCINTCREATEMSG    (SC_MSG_BASE+0x102)
#define SC_PROCTIMCREATEMSG    (SC_MSG_BASE+0x104)
#define SC_PROCUSRINTCREATEMSG (SC_MSG_BASE+0x106)
#ifndef __ASM_INCLUDED__
typedef struct sc_procCreateMsg_s{
  sc_msgid_t id;
  char name[SC_PROC_NAME_SIZE+1];
  void (*entry)(void);
  sc_bufsize_t stacksize;
  sc_ticks_t slice;
  sc_prio_t prio;
  int state;
  sc_poolid_t plid;
} sc_procCreateMsg_t;
#define SC_PROCCREATEMSG_SZ (sizeof(sc_procCreateMsg_t))
#else /* __ASM_INCLUDED__ */
#define SC_PROCCREATEMSG_ID 0
#define SC_PROCCREATEMSG_NAME      SIZEOF_MSGID
#define SC_PROCCREATEMSG_ENTRY     (SC_PROCCREATEMSG_NAME+SC_PROC_NAME_SIZE+1)
#define SC_PROCCREATEMSG_STACKSIZE (SC_PROCCREATEMSG_ENTRY+4)
#define SC_PROCCREATEMSG_SLICE     (SC_PROCCREATEMSG_STACKSIZE+4)
#define SC_PROCCREATEMSG_PRIO      (SC_PROCCREATEMSG_SLICE+4)
#define SC_PROCCREATEMSG_STATE     (SC_PROCCREATEMSG_PRIO+4)
#define SC_PROCCREATEMSG_PLID      (SC_PROCCREATEMSG_STATE+4)
#define SC_PROCCREATEMSG_SZ        (SC_PROCCREATEMSG_PLID+4)
#endif /* __ASM_INCLUDED__ */

#define SC_PROCPRIOCREATEMSG_REPLY   (SC_MSG_BASE+0x101)
#define SC_PROCINTCREATEMSG_REPLY    (SC_MSG_BASE+0x103)
#define SC_PROCTIMCREATEMSG_REPLY    (SC_MSG_BASE+0x105)
#define SC_PROCUSRINTCREATEMSG_REPLY (SC_MSG_BASE+0x107)

#ifndef __ASM_INCLUDED__
typedef struct sc_procCreateMsgReply_s{
  sc_msgid_t id;
  sc_pid_t pid;
} sc_procCreateMsgReply_t;
#define SC_PROCCREATEMSG_REPLY_SZ (sizeof(sc_procCreateMsgReply_t))
#else /* __ASM_INCLUDED__ */
#define SC_PROCCREATEMSG_REPLY_ID  0
#define SC_PROCCREATEMSG_REPLY_PID SIZEOF_MSGID
#define SC_PROCCREATEMSG_REPLY_SZ  (SC_PROCCREATEMSG_REPLY_PID+SIZEOF_PID_T)
#endif /* __ASM_INCLUDED__ */

#define SC_PROCKILLMSG (SC_MSG_BASE+0x108)
#ifndef __ASM_INCLUDED__
typedef struct sc_procKillMsg_s{
  sc_msgid_t id;
  sc_pid_t pid;
  flags_t flag;
} sc_procKillMsg_t;
#define SC_PROCKILLMSG_SZ (sizeof(sc_procKillMsg_t))
#else /* __ASM_INCLUDED__ */
#define SC_PROCKILLMSG_ID   0
#define SC_PROCKILLMSG_PID  SIZEOF_MSGID
#define SC_PROCKILLMSG_FLAG (SC_PROCKILLMSG_PID+SIZEOF_PID_T)
#define SC_PROCKILLMSG_SZ   (SC_PROCKILLMSG_FLAG+4)
#endif /* __ASM_INCLUDED__ */

#define SC_PROCKILLMSG_REPLY (SC_MSG_BASE+0x109)
#ifndef __ASM_INCLUDED__
typedef struct sc_procKillMsgReply_s{
  sc_msgid_t id;
  sc_pid_t pid;  
} sc_procKillMsgReply_t;
#define SC_PROCKILLMSG_REPLY_SZ (sizeof(sc_procKillMsgReply_t))
#else /* __ASM_INCLUDED__ */
#define SC_PROCKILLMSG_REPLY_ID  0
#define SC_PROCKILLMSG_REPLY_PID SIZEOF_MSGID
#define SC_PROCKILLMSG_REPLY_SZ  (SC_PROCKILLMSG_REPLY_PID+SIZEOF_PID_T)
#endif /* __ASM_INCLUDED__ */
#endif /* !__KKERNEL__ */

#define SC_PROCNAMEGETMSG (SC_MSG_BASE+0x10a)
#ifndef __ASM_INCLUDED__
typedef struct sc_procNameGetMsg_s{
  sc_msgid_t id;
  sc_pid_t pid;
} sc_procNameGetMsg_t;
#define SC_PROCNAMEGETMSG_SZ (sizeof(sc_procNameGetMsg_t))
#else /* __ASM_INCLUDED__ */
#define SC_PROCNAMEGETMSG_ID  0
#define SC_PROCNAMEGETMSG_PID (SC_PROCNAMEGETMSG_ID+SIZEOF_MSGID)
#define SC_PROCNAMEGETMSG_SZ  (SC_PROCNAMEGETMSG_PID+SIZEOF_PID_T)
#endif /* __ASM_INCLUDED__ */

#define SC_PROCNAMEGETMSG_REPLY (SC_MSG_BASE+0x10b)
#ifndef __ASM_INCLUDED__
typedef struct sc_procNameGetMsgReply_s{
  sc_msgid_t id;
  sc_errcode_t error;
  char target[SC_MODULE_NAME_SIZE+1];
  char module[SC_MODULE_NAME_SIZE+1];
  char process[SC_PROC_NAME_SIZE+1];
} sc_procNameGetMsgReply_t;
#define SC_PROCNAMEGETMSG_REPLY_SZ (sizeof(sc_procNameGetMsgReply_t))
#else /* __ASM_INCLUDED__ */
#define SC_PROCNAMEGETMSG_REPLY_ERROR   SIZEOF_MSGID
#define SC_PROCNAMEGETMSG_REPLY_TARGET  (SC_PROCNAMEGETMSG_REPLY_ERROR+4)
#define SC_PROCNAMEGETMSG_REPLY_MODULE  (SC_PROCNAMEGETMSG_REPLY_TARGET+SC_MODULE_NAME_SIZE+1)
#define SC_PROCNAMEGETMSG_REPLY_PROCESS (SC_PROCNAMEGETMSG_REPLY_MODULE+SC_MODULE_NAME_SIZE+1)
#define SC_PROCNAMEGETMSG_REPLY_SZ      (SC_PROCNAMEGETMSG_REPLY_PROCESS+SC_MODULE_NAME_SIZE+1)
#endif /* __ASM_INCLUDED__ */

#define SC_PROCIDGETMSG (SC_MSG_BASE+0x10c)
#ifndef __ASM_INCLUDED__
typedef struct sc_procIdGetMsg_s{
  sc_msgid_t id;
  sc_ticks_t tmo;
  char path[1];
} sc_procIdGetMsg_t;
#define SC_PROCIDGETMSG_SZ (sizeof(sc_procIdGetMsg_t))
#else /* __ASM_INCLUDED__ */
#define SC_PROCIDGETMSG_ID   0
#define SC_PROCIDGETMSG_TMO  SIZEOF_MSGID
#define SC_PROCIDGETMSG_PATH (SC_PROCIDGETMSG_TMO+4)
#define SC_PROCIDGETMSG_SZ   (SC_PROCIDGETMSG_PATH+1)
#endif /* __ASM_INCLUDED__ */

#define SC_PROCIDGETMSG_REPLY (SC_MSG_BASE+0x10d)
#ifndef __ASM_INCLUDED__
typedef struct sc_procIdGetMsgReply_s{
  sc_msgid_t id;
  sc_pid_t pid;
  sc_errcode_t error;
  int more;
} sc_procIdGetMsgReply_t;
#define SC_PROCIDGETMSG_REPLY_SZ (sizeof(sc_procIdGetMsgReply_t))
#else /* __ASM_INCLUDED__ */
#define SC_PROCIDGETMSG_REPLY_ID    0
#define SC_PROCIDGETMSG_REPLY_PID   SIZEOF_MSGID
#define SC_PROCIDGETMSG_REPLY_MORE  (SC_PROCIDGETMSG_REPLY_PID+SIZEOF_PID_T)
#define SC_PROCIDGETMSG_REPLY_ERROR (SC_PROCIDGETMSG_REPLY_MORE+SIZEOF_INT)
#define SC_PROCIDGETMSG_REPLY_SZ    (SC_PROCIDGETMSG_REPLY_ERROR+4)
#endif /* __ASM_INCLUDED__ */

#define SC_PROCPATHGETMSG (SC_MSG_BASE+0x10e)
#ifndef __ASM_INCLUDED__
typedef struct sc_procPathGetMsg_s{
  sc_msgid_t id;
  sc_pid_t pid;
  int flag;
} sc_procPathGetMsg_t;
#define SC_PROCPATHGETMSG_SZ (sizeof(sc_procPathGetMsg_t))
#else /* __ASM_INCLUDED__ */
#define SC_PROCPATHGETMSG_ID   0
#define SC_PROCPATHGETMSG_PID  (SC_PROCPATHGETMSG_ID+SIZEOF_MSGID)
#define SC_PROCPATHGETMSG_FLAG (SC_PROCPATHGETMSG_PID+SIZEOF_PID_T)
#define SC_PROCPATHGETMSG_SZ   (SC_PROCPATHGETMSG_FLAG+SIZEOF_INT)
#endif /* __ASM_INCLUDED__ */

#define SC_PROCPATHGETMSG_REPLY (SC_MSG_BASE+0x10f)
#ifndef __ASM_INCLUDED__
typedef struct sc_procPathGetMsgReply_s{
  sc_msgid_t id;
  sc_pid_t pid;
  sc_errcode_t error;
  char path[1];
} sc_procPathGetMsgReply_t;
#define SC_PROCPATHGETMSG_REPLY_SZ (sizeof(sc_procPathGetMsgReply_t))
#else /* __ASM_INCLUDED__ */
#define SC_PROCPATHGETMSG_REPLY_ID    0
#define SC_PROCPATHGETMSG_REPLY_PID   (SC_PROCPATHGETMSG_REPLY_ID+SIZEOF_MSGID)
#define SC_PROCPATHGETMSG_REPLY_ERROR (SC_PROCPATHGETMSG_REPLY_PID+SIZEOF_PID_T)
#define SC_PROCPATHGETMSG_REPLY_PATH  (SC_PROCPATHGETMSG_REPLY_ERROR+4)
#define SC_PROCPATHGETMSG_REPLY_SZ    (SC_PROCPATHGETMSG_REPLY_PATH+1)
#endif /* __ASM_INCLUDED__ */
/*
************************************************** module related messages
*/
#ifndef __KKERNEL__
#define SC_MODULECREATEMSG (SC_MSG_BASE+0x200)
#ifndef __ASM_INCLUDED__
typedef struct sc_moduleCreateMsg_s{
  sc_msgid_t id;
  char module[SC_MODULE_NAME_SIZE+1];
  void (*entry)(void);
  sc_bufsize_t stacksize;
  sc_prio_t prio;
  char *start;
  sc_modulesize_t size;
  sc_modulesize_t textsize;
  unsigned int max_pools;
  unsigned int max_procs;
} sc_moduleCreateMsg_t;
#define SC_MODULECREATEMSG_SZ (sizeof(sc_moduleCreateMsg_t))
#else /* __ASM_INCLUDED__ */
#define SC_MODULECREATEMSG_ID 0
#define SC_MODULECREATEMSG_MODULE    SIZEOF_MSGID
#define SC_MODULECREATEMSG_ENTRY     (SC_MODULECREATEMSG_MODULE+SC_MODULE_NAME_SIZE+1)
#define SC_MODULECREATEMSG_STACKSIZE (SC_MODULECREATEMSG_ENTRY+4)
#define SC_MODULECREATEMSG_PRIO      (SC_MODULECREATEMSG_STACKSIZE+4)
#define SC_MODULECREATEMSG_START     (SC_MODULECREATEMSG_PRIO+4)
#define SC_MODULECREATEMSG_SIZE      (SC_MODULECREATEMSG_START+4)
#define SC_MODULECREATEMSG_TEXTSIZE  (SC_MODULECREATEMSG_SIZE+4)
#define SC_MODULECREATEMSG_MAX_POOLS (SC_MODULECREATEMSG_TEXTSIZE+4)
#define SC_MODULECREATEMSG_MAX_PROCS (SC_MODULECREATEMSG_MAX_POOLS+4)
#define SC_MODULECREATEMSG_SZ        (SC_MODULE_NAME_SIZE+1+9*4)
#endif /* __ASM_INCLUDED__ */

#define SC_MODULECREATEMSG_REPLY (SC_MSG_BASE+0x201)
#ifndef __ASM_INCLUDED__
typedef struct sc_moduleCreateMsgReply_s {
  sc_msgid_t id;
  sc_moduleid_t mid;
} sc_moduleCreateMsgReply_t;
#define SC_MODULECREATEMSG_REPLY_SZ (sizeof(sc_moduleCreateMsgReply_t))
#else /* __ASM_INCLUDED__ */
#define SC_MODULECREATEMSG_REPLY_ID  0
#define SC_MODULECREATEMSG_REPLY_MID SIZEOF_MSGID
#define SC_MODULECREATEMSG_REPLY_SZ  (SC_MODULECREATEMSG_REPLY_MID+SIZEOF_PID_T)
#endif /* __ASM_INCLUDED__ */

#define SC_MODULEKILLMSG (SC_MSG_BASE+0x202)
#ifndef __ASM_INCLUDED__
typedef struct sc_moduleKillMsg_s {
  sc_msgid_t id;
  sc_moduleid_t mid;
  flags_t flags;
} sc_moduleKillMsg_t;
#define SC_MODULEKILLMSG_SZ (sizeof(sc_moduleKillMsg_t))
#else /* __ASM_INCLUDED__ */
#define SC_MODULEKILLMSG_ID    0
#define SC_MODULEKILLMSG_MID   SIZEOF_MSGID
#define SC_MODULEKILLMSG_FLAGS (SC_MODULEKILLMSG_MID+SIZEOF_PID_T)
#define SC_MODULEKILLMSG_SZ    (SC_MODULEKILLMSG_FLAGS+4)
#endif /* __ASM_INCLUDED__ */

#define SC_MODULEKILLMSG_REPLY (SC_MSG_BASE+0x203)
#ifndef __ASM_INCLUDED__
typedef struct sc_moduleKillMsgReply_s {
  sc_msgid_t id;
  sc_moduleid_t mid;
} sc_moduleKillMsgReply_t;
#define SC_MODULEKILLMSG_REPLY_SZ (sizeof(sc_moduleKillMsgReply_t))
#else /* __ASM_INCLUDED__ */
#define SC_MODULEKILLMSG_REPLY_ID  0
#define SC_MODULEKILLMSG_REPLY_MID SIZEOF_MSGID
#define SC_MODULEKILLMSG_REPLY_SZ  (SC_MODULEKILLMSG_REPLY_MID+SIZEOF_PID_T)
#endif /* __ASM_INCLUDED__ */
#endif /* !__KKERNEL__ */
/*
************************************************** procd related messages
*/

#define SC_PROCCREATENOTIFYMSG (SC_MSG_BASE+0x300)
#ifndef __ASM_INCLUDED__
typedef struct sc_procCreateNotifyMsg_s{
  sc_msgid_t id;
  sc_pid_t pid;
  char target[SC_MODULE_NAME_SIZE+1];
  char module[SC_MODULE_NAME_SIZE+1];
  char process[SC_PROC_NAME_SIZE+1];
  __u16 thash;
  __u16 mhash;
  __u16 phash;
} sc_procCreateNotifyMsg_t;
#define SC_PROCCREATENOTIFYMSG_SZ (sizeof(sc_procCreateNotifyMsg_t))
#else /* __ASM_INCLUDED__ */
#define SC_PROCCREATENOTIFYMSG_ID      0
#define SC_PROCCREATENOTIFYMSG_PID     SIZEOF_MSGID
#define SC_PROCCREATENOTIFYMSG_TARGET  8
#define SC_PROCCREATENOTIFYMSG_MODULE  (SC_PROCCREATENOTIFYMSG_TARGET+SC_PROC_NAME_SIZE+1)
#define SC_PROCCREATENOTIFYMSG_PROCESS (SC_PROCCREATENOTIFYMSG_MODULE+SC_PROC_NAME_SIZE+1)
#define SC_PROCCREATENOTIFYMSG_THASH   (SC_PROCCREATENOTIFYMSG_PROCESS+SC_PROC_NAME_SIZE+1)
#define SC_PROCCREATENOTIFYMSG_MHASH   (SC_PROCCREATENOTIFYMSG_THASH+2)
#define SC_PROCCREATENOTIFYMSG_PHASH   (SC_PROCCREATENOTIFYMSG_MHASH+2)
#define SC_PROCCREATENOTIFYMSG_SZ      (SC_PROCCREATENOTIFYMSG_PHASH+2)
#endif /* __ASM_INCLUDED__ */

#ifndef __KKERNEL__
#define SC_MODULECREATENOTIFYMSG (SC_MSG_BASE+0x302)
#ifndef __ASM_INCLUDED__
typedef struct sc_moduleCreateNotifyMsg_s{
  sc_msgid_t id;
  sc_moduleid_t mid;
  char target[SC_MODULE_NAME_SIZE+1];
  char module[SC_MODULE_NAME_SIZE+1];
  __u16 thash;
  __u16 mhash;
} sc_moduleCreateNotifyMsg_t;
#define SC_MODULECREATENOTIFYMSG_SZ (sizeof(sc_moduleCreateNotifyMsg_t))
#else /* __ASM_INCLUDED__ */
#define SC_MODULECREATENOTIFYMSG_ID 0
#define SC_MODULECREATENOTIFYMSG_MID    (SC_MODULECREATENOTIFYMSG_ID+SIZEOF_MSGID)
#define SC_MODULECREATENOTIFYMSG_TARGET (SC_MODULECREATENOTIFYMSG_MID+SIZEOF_PID_T)
#define SC_MODULECREATENOTIFYMSG_MODULE (SC_MODULECREATENOTIFYMSG_TARGET+SC_MODULE_NAME_SIZE+1)
#define SC_MODULECREATENOTIFYMSG_THASH  (SC_MODULECREATENOTIFYMSG_MODULE+SC_MODULE_NAME_SIZE+1)
#define SC_MODULECREATENOTIFYMSG_MHASH  (SC_MODULECREATENOTIFYMSG_THASH+2)
#define SC_MODULECREATENOTIFYMSG_SZ     (SC_MODULECREATENOTIFYMSG_MHASH+2)
#endif /* __ASM_INCLUDED__ */
#endif /* !__KKERNEL__ */
/*
********************************************** connector related messages
*/
#define SC_CONNPROCIDGETMSG (SC_MSG_BASE+0x400)
#ifndef __ASM_INCLUDED__
typedef struct sc_connProcIdGetMsg_s {
  sc_msgid_t id;
  sc_ticks_t tmo;
  int request;
  char path[1];
} sc_connProcIdGetMsg_t;
#define SC_CONNPROCIDGETMSG_SZ (sizeof(sc_connProcIdGetMsg_t))
#else /* __ASM_INCLUDED__ */
#define SC_CONNPROCIDGETMSG_ID      0
#define SC_CONNPROCIDGETMSG_TMO     SIZEOF_MSGID
#define SC_CONNPROCIDGETMSG_REQUEST (SC_CONNPROCIDGETMSG_TMO+SIZEOF_TICKS_T)
#define SC_CONNPROCIDGETMSG_PATH    (SC_CONNPROCIDGETMSG_REQUEST+SIZEOF_INT)
#define SC_CONNPROCIDGETMSG_SZ      (SC_CONNPROCIDGETMSG_PATH+1)
#endif /* __ASM_INCLUDED__ */

#define SC_CONNPROCIDGETMSG_REPLY (SC_MSG_BASE+0x401)
#ifndef __ASM_INCLUDED__
typedef struct sc_connProcIdGetMsgReply_s {
  sc_msgid_t id;
  sc_pid_t pid;
  int request;
  sc_errcode_t error;
  int more;
} sc_connProcIdGetMsgReply_t;
#define SC_CONNPROCIDGETMSG_REPLY_SZ (sizeof(sc_connProcIdGetMsgReply_t))
#else /* __ASM_INCLUDED__ */
#define SC_CONNPROCIDGETMSG_REPLY_ID      0
#define SC_CONNPROCIDGETMSG_REPLY_PID     SIZEOF_MSGID
#define SC_CONNPROCIDGETMSG_REPLY_REQUEST (SC_CONNPROCIDGETMSG_REPLY_PID+SIZEOF_PID_T)
#define SC_CONNPROCIDGETMSG_REPLY_ERROR   (SC_CONNPROCIDGETMSG_REPLY_REQUEST+SIZEOF_INT)
#define SC_CONNPROCIDGETMSG_REPLY_MORE    (SC_CONNPROCIDGETMSG_REPLY_ERROR+4)
#define SC_CONNPROCIDGETMSG_REPLY_SZ      (SC_CONNPROCIDGETMSG_REPLY_MORE+SIZEOF_INT)
#endif /* __ASM_INCLUDED__ */


#define SC_CONNPROCOBSERVEMSG   (SC_MSG_BASE+0x402)
#ifndef __ASM_INCLUDED__
typedef struct sc_connProcObserveMsg_s {
  sc_msgid_t id;
  sc_pid_t pid;
  union sc_msg *msg;
} sc_connProcObserveMsg_t;
#endif
#define SC_CONNPROCOBSERVEMSG_ID  0
#define SC_CONNPROCOBSERVEMSG_PID SIZEOF_MSGID
#define SC_CONNPROCOBSERVEMSG_MSG (SC_CONNPROCOBSERVEMSG_PID+SIZEOF_PID_T)
#define SC_CONNPROCOBSERVEMSG_SZ  (SC_CONNPROCOBSERVEMSG_MSG+SIZEOF_PTR)

#define SC_CONNPROCUNOBSERVEMSG       (SC_MSG_BASE+0x404)
#define SC_CONNPROCUNOBSERVEMSG_REPLY (SC_MSG_BASE+0x405)
#ifndef __ASM_INCLUDED__
typedef struct sc_connProcUnobserveMsg_s {
  sc_msgid_t id;
  sc_pid_t observer;
  sc_pid_t pid;
}sc_connProcUnobserveMsg_t;
#endif
#define SC_CONNPROCUNOBSERVEMSG_ID 0
#define SC_CONNPROCUNOBSERVEMSG_OBSERVER SIZEOF_MSGID
#define SC_CONNPROCUNOBSERVEMSG_PID (SC_CONNPROCUNOBSERVEMSG_OBSERVER+SIZEOF_PID_T)
#define SC_CONNPROCUNOBSERVEMSG_SZ  (SC_CONNPROCUNOBSERVEMSG_PID+SIZEOF_PID_T)

/*
********************************************** procd messages
*/

#define SC_PROCDREQUESTMSG (SC_MSG_BASE+0x500)
#define SC_PROCDCONNREQUESTMSG (SC_MSG_BASE+0x501)

#ifndef __ASM_INCLUDED__
typedef struct sc_procdRequest_s{
  sc_msgid_t id;
  dbl_t list;
  sc_pid_t requestor;
  sc_tmoid_t tmoid;
  int request_id;
  char module[SC_MODULE_NAME_SIZE+1];
  char process[SC_PROC_NAME_SIZE+1];
  __u16 mhash;
  __u16 phash;
}sc_procdRequest_t;
#define SC_PROCDREQUESTMSG_SZ (sizeof(sc_procdRequest_t))
#else /* __ASM_INCLUDED__ */
#define SC_PROCDREQUESTMSG_ID         0
#define SC_PROCDREQUESTMSG_LIST       SIZEOF_MSGID
#define SC_PROCDREQUESTMSG_REQUESTOR  (SC_PROCDREQUESTMSG_LIST+SIZEOF_PTR*2)
#define SC_PROCDREQUESTMSG_TMOID      (SC_PROCDREQUESTMSG_REQUESTOR+SIZEOF_PID_T)
#define SC_PROCDREQUESTMSG_REQUEST_ID (SC_PROCDREQUESTMSG_TMOID+SIZEOF_PTR)
#define SC_PROCDREQUESTMSG_MODULE     (SC_PROCDREQUESTMSG_REQUEST_ID+SIZEOF_INT)
#define SC_PROCDREQUESTMSG_PROCESS    (SC_PROCDREQUESTMSG_MODULE+SC_MODULE_NAME_SIZE+1)
#define SC_PROCDREQUESTMSG_MHASH      (SC_PROCDREQUESTMSG_PROCESS+SC_PROC_NAME_SIZE+1)
#define SC_PROCDREQUESTMSG_PHASH      (SC_PROCDREQUESTMSG_MHASH+2)
#define SC_PROCDREQUESTMSG_SZ         (SC_PROCDREQUESTMSG_PHASH+2)
#endif /* __ASM_INCLUDED__ */

#define SC_PROCDTMOREQMSG (SC_MSG_BASE+0x502)

#ifndef __ASM_INCLUDED__
typedef struct sc_procdTmoreq_s
{
  sc_msgid_t id;
  sc_procdRequest_t *request;
} sc_procdTmoreq_t;
#else /* __ASM_INCLUDED__ */
#define SC_PROCDTMOREQMSG_ID      0
#define SC_PROCDTMOREQMSG_REQUEST SIZEOF_MSGID
#define SC_PROCDTMOREQMSG_SZ      (SC_PROCDTMOREQMSG_REQUEST+SIZEOF_PTR)
#endif /* __ASM_INCLUDED__ */
/*
********************************************** malloc
*/

#define SC_MALLOC_MSGID (SC_MSG_BASE+0x600)

#ifdef __IAR_SYSTEMS_ICC__
/* enable MISRA C Rule 14*/
#pragma diag_default=Pm012
#endif

#endif /* _SCIOPTA_MSG_ */


