/*SOC -*-c++-*-
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
** ID: S01103BS48                                                   **
** +Revision: 2.40.14.1 +                                                **
** +Date: 2009/10/04 09:53:43 +                                     **
** Message definitions and API                                      **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

/*
****************************************************************************
** Log at end.
****************************************************************************
*/
#ifndef _KERNEL_MSG_H_
#define _KERNEL_MSG_H_
/* SOCNV */
#define SC_MSGRX_PID       0x01
#define SC_MSGRX_PID_POS   0
#define SC_MSGRX_MSGID     0x02
#define SC_MSGRX_MSGID_POS 1
#define SC_MSGRX_BOTH      (SC_MSGRX_PID+SC_MSGRX_MSGID)
#define SC_MSGRX_NOT       0x04
#define SC_MSGRX_NOT_POS   2
/* EOCNV */
#define SC_MSGRX_ALL  ((const void *)0)
#define NULL_MSG ((sc_msg_t)0)

/* SOCNV */
#if SC_MSG_HOOK == 1
#define SC_SET_MSGTX_HOOK (1)
#define SC_SET_MSGRX_HOOK (2)
#endif

/* flags for msgTx */
#define SC_MSGTX_OBSERVE 0x0001
#define SC_MSGTX_OBSERVE_POS 0
/* EOCNV */

#if SIZEOF_PTR == 2
#define MSG_LIST         0
#define MSG_SIZE         2
#define MSG_PLID         4
#define MSG_BUFSIZE_IDX  6
#define MSG_SENDER       8
#define MSG_ADDRESSEE    10
#define MSG_OWNER        12
#define MSG_ID           14

#define MSG_HEAD        MSG_ID
#else
/* SOCNV */
#define MSG_LIST         0
#define MSG_SIZE         8
#define MSG_PLID         12
#define MSG_BUFSIZE_IDX  16
#define MSG_SENDER       20
#define MSG_ADDRESSEE    24
#define MSG_OWNER        28
#define MSG_ID           32

#define MSG_HEAD        MSG_ID

#define TMOMSG_LIST         0
#define TMOMSG_SIZE         8
#define TMOMSG_PLID         12
#define TMOMSG_TMO          16
#define TMOMSG_SENDER       20
#define TMOMSG_TMOLIST      24
#define TMOMSG_ID           32

#define TMOMSG_HEAD        MSG_ID
/* EOCNV */
#endif
#ifndef __ASM_INCLUDED__

#if SIZEOF_PTR == 2
struct sc_msg_hd_s{
  sgl_t list;
  sc_bufsize_t size;                   /* allocated size */
  sc_poolid_t plid;                    /* pool this message is from */
  int bufsize_idx;                     /* index to fixed buffer size */
  sc_pid_t sender;                     /* last sender */
  sc_pid_t addressee;                  /* last addressee */
  sc_pid_t owner;                      /* current owner */
  sc_msgid_t id;
};
typedef struct sc_msg_hd_s sc_msg_hd_t;
#define MSGHEAD (offsetof(sc_msg_hd_t,id))
#define MSG(ptr,member) SGL_ITEM(sc_msg_hd_t,ptr,member)
#else
struct sc_msg_hd_s{
  dbl_t list;
  sc_bufsize_t size;                   /* allocated size */
  sc_poolid_t plid;                    /* pool this message is from */
  int bufsize_idx;                     /* index to fixed buffer size */
  sc_pid_t sender;                     /* last sender */
  sc_pid_t addressee;                  /* last addressee */
  sc_pid_t owner;                      /* current owner */
  sc_msgid_t id;
};
typedef struct sc_msg_hd_s sc_msg_hd_t;
#define MSGHEAD (offsetof(sc_msg_hd_t,id))
#define MSG(ptr,member) DBL_ITEM(sc_msg_hd_t,ptr,member)

struct sc_tmomsg_hd_s{
  dbl_t list;
  sc_bufsize_t size;
  sc_poolid_t plid;
  sc_ticks_t tmo;  
  sc_pid_t sender;                     /* owner */
  dbl_t tmolist;
  sc_msgid_t id;
};
typedef struct sc_tmomsg_hd_s sc_tmomsg_hd_t;
#define TMOMSGHEAD (offsetof(sc_tmomsg_hd_t,id))
#define TMOMSG(ptr,member) DBL_ITEM(sc_tmomsg_hd_t,ptr,member)

#endif


struct sc_msg_rx_s{
  sc_pid_t pid;
  sc_msgid_t msgid;
};
typedef struct sc_msg_rx_s sc_msg_rx_t;

SCFUNCn(sc_msg_t,sc_msgAlloc,sc_bufsize_t,sc_msgid_t,sc_poolid_t,sc_ticks_t);
SCFUNCn(sc_msg_t,sc_msgAllocClr,sc_bufsize_t,sc_msgid_t,sc_poolid_t,sc_ticks_t);
SCFUNCn(void,sc_msgFree,sc_msgptr_t);
SCFUNCn(void,sc_msgAcquire,sc_msgptr_t);
SCFUNCn(sc_pid_t,sc_msgAddrGet,sc_msgptr_t);
SCFUNCn(sc_pid_t,sc_msgSndGet,sc_msgptr_t);
SCFUNCn(sc_pid_t,sc_msgOwnerGet,sc_msgptr_t);
SCFUNCn(sc_bufsize_t,sc_msgSizeGet,sc_msgptr_t);
SCFUNCn(sc_bufsize_t,sc_msgSizeSet,sc_msgptr_t,sc_bufsize_t);
SCFUNCn(void,sc_msgTx,sc_msgptr_t,sc_pid_t,flags_t);
SCFUNCn(void,sc_msgTxAlias,sc_msgptr_t,sc_pid_t,flags_t,sc_pid_t);
SCFUNCn(sc_msg_t,sc_msgRx,sc_ticks_t,const void *,flags_t);
SCFUNCn(sc_poolid_t,sc_msgPoolIdGet,sc_msgptr_t);
#if SC_MSG_HOOK == 1
typedef void (sc_msgHook_t)(const sc_msg_hd_t *);
SCFUNCn(sc_msgHook_t *,sc_msgHookRegister,int,sc_msgHook_t *);
#endif

#ifndef __KERNEL__
#define sc_msgAlloc(size,msgid,poolid,tmo)     _FUNCn(sc_msgAlloc,size,msgid,poolid,tmo)
#define sc_msgAllocClr(size,msgid,poolid,tmo)  _FUNCn(sc_msgAllocClr,size,msgid,poolid,tmo)
#define sc_msgFree(msgptr)                     _FUNCn(sc_msgFree,msgptr)
#define sc_msgTx(msgptr,addressee,flag)        _FUNCn(sc_msgTx,msgptr,addressee,flag)
#define sc_msgTxAlias(msgptr,addressee,flag,alias)   _FUNCn(sc_msgTxAlias,msgptr,addressee,flag,alias)
#define sc_msgRx(tmo,select,flag)              _FUNCn(sc_msgRx,tmo,select,flag)
#define sc_msgAcquire(msgptr)                  _FUNCn(sc_msgAcquire,msgptr)
#define sc_msgAddrGet(msgptr)                  _FUNCn(sc_msgAddrGet,msgptr)
#define sc_msgSndGet(msgptr)                   _FUNCn(sc_msgSndGet,msgptr)
#define sc_msgOwnerGet(msgptr)                 _FUNCn(sc_msgOwnerGet,msgptr)
#define sc_msgSizeGet(msgptr)                  _FUNCn(sc_msgSizeGet,msgptr)
#define sc_msgSizeSet(msgptr,size)             _FUNCn(sc_msgSizeSet,msgptr,size)
#define sc_msgPoolIdGet(msgptr)                _FUNCn(sc_msgPoolIdGet,msgptr)
#if SC_MSG_HOOK == 1
#define sc_msgHookRegister(hook,flag)  _FUNCn(sc_msgHookRegister,hook,flag)
#endif
#endif /* __KERNEL__ */
#endif /* __ASM_INCLUDED__ */
#endif /* MSG_H */

/*
** $Log: msg.h,v $
** Revision 2.40.14.1  2009/10/04 09:53:43  bastian
** added more function and fixes for das bug
**
** Revision 2.40  2006/10/11 14:01:40  bastian
** merger
**
** Revision 2.39.4.2  2006/06/30 04:47:02  bastian
** fixed typos
**
** Revision 2.39.4.1  2006/06/29 12:27:36  bastian
** cleartext parameters
**
** Revision 2.39  2005/10/27 10:07:38  bastian
** fixed hook parameter
**
** Revision 2.38  2005/09/20 09:43:15  bastian
** added const
**
** Revision 2.37  2005/09/16 11:36:21  bastian
** x
**
** Revision 2.36  2005/09/09 14:44:14  bastian
** changed select type (for MISRA)
**
** Revision 2.35  2005/07/01 11:18:25  bastian
** merge from branch-1.7.2
**
** Revision 2.34.4.1  2005/06/21 05:24:14  bastian
** added hook-syscalls
**
** Revision 2.34  2004/10/04 12:11:37  bastian
** introduced sc_msg_t and sc_msgptr_t
**
** Revision 2.33  2004/07/05 05:26:09  bastian
** added tags for conversion
**
** Revision 2.32  2003/09/26 12:43:53  bastian
** extended error-check
**
** Revision 2.31  2003/09/09 10:43:42  bastian
** added bit pos for flag
**
** Revision 2.30  2003/07/25 15:05:09  bastian
** make head-defs available in C
**
** Revision 2.29  2003/07/14 08:50:53  bastian
** fix num-o-para for sizeset (2nd)
**
** Revision 2.28  2003/07/14 08:49:58  bastian
** fix num-o-para for sizeset
**
** Revision 2.27  2003/04/17 13:44:26  bastian
** fixed asm offsets
**
** Revision 2.26  2003/04/15 11:05:42  bastian
** Fixed async-TMO: Must leave msg in the process alloc-list to allow
** removing a TMO if a process gets killed.
**
** Revision 2.25  2003/03/26 13:59:46  bastian
** added system-call/trap interface
** changed 2nd parameter of miscError
**
** Revision 2.24  2003/03/25 14:10:25  bastian
** removed UL from assembly defines
**
** Revision 2.23  2003/03/25 07:02:49  bastian
** started 16bit support
**
** Revision 2.22  2003/03/17 11:07:50  bastian
** fixed IDs
**
** Revision 2.21  2003/03/17 09:00:03  bastian
** added header
**
** Revision 2.20  2003/03/17 07:52:17  bastian
** fixed const char
**
** Revision 2.19  2003/02/12 11:11:40  bastian
** allocClr not called if DEBUG
**
** Revision 2.18  2002/12/17 10:15:27  bastian
** fixed forward decl
**
** Revision 2.17  2002/12/17 08:11:45  bastian
** started to add externC macro
**
** Revision 2.16  2002/12/09 10:04:27  bastian
** changed SCIOPTA_ into SC_
**
** Revision 2.15  2002/08/30 13:44:35  bastian
** renamed connect => observe
**
** Revision 2.14  2002/07/25 06:19:55  bastian
** Fixed typo.
** Moved log to end.
**
** Revision 2.13  2002/06/03 13:03:31  bastian
** fixed name for acquire
**
** Revision 2.12  2002/05/16 05:44:12  bastian
** changed msgAlloc into sc_sysMsgAlloc
**
** Revision 2.11  2002/05/13 09:30:11  bastian
** Removed obsolete flag-value
** compacted flags.
**
** Revision 2.10  2002/05/08 05:52:46  bastian
** 0705-1
**
** Revision 2.9  2002/05/07 09:47:35  bastian
** replace msgTx with sysMsgTx
**
** Revision 2.8  2002/04/29 14:38:31  bastian
** 2904-2
**
** Revision 2.7  2002/03/22 16:36:17  bastian
** added ownerget
** protection for stdio in scio
**
** Revision 2.6  2002/03/21 14:46:41  bastian
** Added debugging support for nearly all functions.
** Added error-checking if swap-out fails.
** Fixed friend-concept.
** Misc. fixes.
**
** Revision 2.5  2002/03/20 16:16:59  bastian
** intermediate
**
** Revision 2.4  2002/03/19 12:27:42  bastian
** intermediate
**
** Revision 2.3  2002/03/07 06:35:54  bastian
** Changed parameters for internal alloc to speed up re-direction.
** Added new function sc_msgAllocClr, which resets buffer to zero w/o id.
**
** Revision 2.2  2002/02/28 06:25:53  bastian
** added comment for double use of addressee
**
** Revision 2.1  2001/12/12 07:56:35  bastian
** Fixes for IPS
**
** Revision 2.0.0.1  2001/11/15 22:48:44  bastian
** moved over to cvs
**
 * 
 *    Rev 1.7   15 Nov 2001 17:15:06   skywalker
 * some changes
 * 
 *    Rev 1.6   25 Sep 2001 15:39:18   skywalker
 * raqe cond. fixes
 * 
 *    Rev 1.5   10 Sep 2001 10:34:40   skywalker
 * ?
 * 
 *    Rev 1.4   Aug 28 2001 14:13:04   bastian
 * added connect-support
 * 
 *    Rev 1.3   Jul 23 2001 13:35:06   bastian
 * Bug fixes
 * 
 *    Rev 1.1   Jul 16 2001 13:43:18   bastian
 * fiexed (?) API
**
*/
