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
** +Revision: 1.13 +                                                **
** +Date: 2008/01/16 13:02:02 +                                     **
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

#define SC_MSGRX_ALL  ((const void FARPTR)0)
#define NULL_MSG ((sc_msg_t)0)

/* SOCNV */
#define SC_MSGRX_PID   0x01
#define SC_MSGRX_MSGID 0x02
#define SC_MSGRX_BOTH  0x03
#define SC_MSGRX_NOT   0x04

#if SIZEOF_PTR == 2
#define MSG_TMO_MAGIC  0x42
#else
#define MSG_TMO_MAGIC 0x4200
#endif

#if SC_MSG_HOOK == 1
#define SC_SET_MSGTX_HOOK (1)
#define SC_SET_MSGRX_HOOK (2)
#endif

/* flags for msgTx */
#define SC_MSGTX_OBSERVE 0x0001
/* EOCNV */

#ifdef __ASM_INCLUDED__
#if SIZEOF_PTR == 2
/* SOCNV */
#define MSG_LIST         0
#define MSG_SIZE         2
#define MSG_PLID         4
#define MSG_BUFSIZE_IDX  5
#define MSG_SENDER       6
#define MSG_ADDRESSEE    7
#define MSG_OWNER        8
//->#if SC_MAX_CONNECTOR > 0
#define MSG_CONNECTOR    9
#define MSG_CONNECTOR_ADDR_MSK    0xF0
#define MSG_CONNECTOR_SNDR_MSK    0x0F
#define MSG_ID           10
//->#else
//->#define MSG_ID           9
//->#endif

#define TMOMSG_LIST         0
#define TMOMSG_SIZE         2
#define TMOMSG_PLID         4
#define TMOMSG_BUFSIZE_IDX  5
#define TMOMSG_SENDER       6
#if CPU == M16C
#define TMOMSG_TMOIDX       7
#define TMOMSG_TMO          8
#else
#define TMOMSG_TMO          7
#define TMOMSG_TMOIDX       8
#endif
//->#if SC_MAX_CONNECTOR > 0
#define TMOMSG_CONNECTOR    9
#define TMOMSG_ID           10
//->#else
//->#define TMOMSG_ID           9
//->#endif

#define TMOMSG_HEAD     TMOMSG_ID
#define MSG_HEAD        MSG_ID
/* EOCNV */
#else /* SIZEOF_PTR == 2 */
#define MSG_LIST         0
#define MSG_SIZE         4
#define MSG_PLID         8
#define MSG_BUFSIZE_IDX  10
#define MSG_SENDER       12
#define MSG_ADDRESSEE    13
#define MSG_OWNER        14
#define MSG_CONNECTOR    15
#define MSG_ID           16

#define MSG_HEAD        MSG_ID

#define TMOMSG_LIST         0
#define TMOMSG_SIZE         4
#define TMOMSG_PLID         10
#define TMOMSG_SENDER       12
#define TMOMSG_TMO          16
#define TMOMSG_TMOIDX       18
#define TMOMSG_SPARE        19
#define TMOMSG_TMOLIST      20
#define TMOMSG_ID           24

#define TMOMSG_HEAD        MSG_ID

#endif /* SIZEOF_PTR == 2 */
#else /* __ASM_INCLUDED__ */

#if SIZEOF_PTR == 2
struct sc_msg_hd_s{
  sgl_t list;
  sc_bufsize_t size;                   /* allocated size */
  __u8 plid;                    /* pool this message is from */
  __u8 bufsize_idx;                     /* index to fixed buffer size */
  __u8 sender;                     /* last sender */
  __u8 addressee;                  /* last addressee */
  __u8 owner;                      /* current owner */
//->#if SC_MAX_CONNECTOR > 0
  __u8 connector;
//->#endif
  sc_msgid_t id;
};
typedef struct sc_msg_hd_s sc_msg_hd_t;
#define MSGHEAD (offsetof(sc_msg_hd_t,id))
#define MSG(ptr,member) SGL_ITEM(sc_msg_hd_t,ptr,member)

struct sc_tmomsg_hd_s{
  sgl_t tmolist;
  sc_bufsize_t size;
  __u8 plid;
  __u8 bufsize_idx;
  __u8 sender;                     /* owner */
  __u8 tmo;
  __u8 tmoidx;
//->#if SC_MAX_CONNECTOR > 0
  __u8 connector;
//->#endif  
  sc_msgid_t id;
};
typedef struct sc_tmomsg_hd_s sc_tmomsg_hd_t;
#define TMOMSGHEAD (offsetof(sc_tmomsg_hd_t,id))
#define TMOMSG(ptr,member) SGL_ITEM(sc_tmomsg_hd_t,ptr,member)

#else /* SIZEOF_PTR == 2 */
struct sc_msg_hd_s{
  sgl_t list;
  sc_bufsize_t size;                   /* allocated size */
  __u16 plid;                          /* pool this message is from */
  __u16 bufsize_idx;                   /* index to fixed buffer size */
  __u8 sender;                     /* last sender */
  __u8 addressee;                  /* last addressee */
  __u8 owner;                      /* current owner */
  __u8 connector;
  sc_msgid_t id;
};
typedef struct sc_msg_hd_s sc_msg_hd_t;


#define MSGHEAD (offsetof(sc_msg_hd_t,id))
#define MSG(ptr,member) SGL_ITEM(sc_msg_hd_t,ptr,member)

struct sc_tmomsg_hd_s{
  sgl_t list;
  sc_bufsize_t size;
  __u16 plid;
  __u16 bufsize_idx;                   /* index to fixed buffer size */
  sc_pid_t sender;                     /* owner */
  __u16 tmo;
  __u8 tmoidx;
  __u8 spare;
  sgl_t tmolist;
  sc_msgid_t id;
};
typedef struct sc_tmomsg_hd_s sc_tmomsg_hd_t;
#define TMOMSGHEAD (offsetof(sc_tmomsg_hd_t,id))
#define TMOMSG(ptr,member) SGL_ITEM(sc_tmomsg_hd_t,ptr,member)

#endif /* SIZEOF_PTR == 2 */


struct sc_msg_rx_s{
  sc_pid_t pid;
  sc_msgid_t msgid;
};
typedef struct sc_msg_rx_s sc_msg_rx_t;

SCFUNC4(sc_msg_t,sc_msgAlloc,sc_bufsize_t,sc_msgid_t,sc_poolid_t,sc_ticks_t);
SCFUNC4(sc_msg_t,sc_msgAllocClr,sc_bufsize_t,sc_msgid_t,sc_poolid_t,sc_ticks_t);
SCFUNC1(void,sc_msgFree,sc_msgptr_t);
SCFUNC1(void,sc_msgAcquire,sc_msgptr_t);
SCFUNC1(sc_pid_t,sc_msgAddrGet,sc_msgptr_t);
SCFUNC1(sc_pid_t,sc_msgSndGet,sc_msgptr_t);
SCFUNC1(sc_pid_t,sc_msgOwnerGet,sc_msgptr_t);
SCFUNC1(sc_bufsize_t,sc_msgSizeGet,sc_msgptr_t);
SCFUNC2(sc_bufsize_t,sc_msgSizeSet,sc_msgptr_t,sc_bufsize_t);
SCFUNC3(void,sc_msgTx,sc_msgptr_t,sc_pid_t,flags_t);
SCFUNC4(void,sc_msgTxAlias,sc_msgptr_t,sc_pid_t,flags_t,sc_pid_t);
SCFUNC3(sc_msg_t,sc_msgRx,sc_ticks_t,const void FARPTR,flags_t);
SCFUNC1(sc_poolid_t,sc_msgPoolIdGet,sc_msgptr_t);
#if SC_MSG_HOOK == 1
typedef void (sc_msgHook_t)(const sc_msg_hd_t NEARPTR);
SCFUNC2(sc_msgHook_t *,sc_msgHookRegister,int,sc_msgHook_t *);
#endif

#ifndef __KERNEL__
#define sc_msgAlloc(a,b,c,d)     _FUNC4(sc_msgAlloc,a,b,c,d)
#define sc_msgAllocClr(a,b,c,d)  _FUNC4(sc_msgAllocClr,a,b,c,d)
#define sc_msgFree(a)            _FUNC1(sc_msgFree,a)
#define sc_msgTx(a,b,c)          _FUNC3(sc_msgTx,a,b,c)
#define sc_msgTxAlias(a,b,c,d)   _FUNC4(sc_msgTxAlias,a,b,c,d)
#define sc_msgRx(a,b,c)          _FUNC3(sc_msgRx,a,b,c)
#define sc_msgAcquire(a)         _FUNC1(sc_msgAcquire,a)
#define sc_msgAddrGet(a)         _FUNC1(sc_msgAddrGet,a)
#define sc_msgSndGet(a)          _FUNC1(sc_msgSndGet,a)
#define sc_msgOwnerGet(a)        _FUNC1(sc_msgOwnerGet,a)
#define sc_msgSizeGet(a)         _FUNC1(sc_msgSizeGet,a)
#define sc_msgSizeSet(a,b)       _FUNC2(sc_msgSizeSet,a,b)
#define sc_msgPoolIdGet(a)       _FUNC1(sc_msgPoolIdGet,a)
#if SC_MSG_HOOK == 1
#define sc_msgHookRegister(a,b)  _FUNC2(sc_msgHookRegister,a,b)
#endif
#endif /* __KERNEL__ */
#endif /* __ASM_INCLUDED__ */
#endif /* MSG_H */

/*
** $Log: msg.h,v $
** Revision 1.13  2008/01/16 13:02:02  bastian
** merge from 1.9.1.12
**
** Revision 1.12.6.3  2007/10/04 16:22:28  bastian
** fix
**
** Revision 1.12.6.2  2007/09/14 07:40:13  bastian
** fixes for nc30
**
** Revision 1.12.6.1  2007/08/16 07:29:30  bastian
** fixed sizeSet macro
**
** Revision 1.12  2005/09/20 09:43:15  bastian
** added const
**
** Revision 1.11  2005/09/16 11:36:21  bastian
** x
**
** Revision 1.10  2004/10/13 09:18:29  bastian
** fix
**
** Revision 1.9  2004/10/07 07:37:47  bastian
** fixed NEAR * to NEARPTR
**
** Revision 1.8  2004/10/04 12:11:37  bastian
** introduced sc_msg_t and sc_msgptr_t
**
** Revision 1.7  2004/09/30 08:33:56  bastian
** fixes for m16c
**
** Revision 1.6  2004/02/10 09:48:02  bastian
** fixed connector handling
**
** Revision 1.5  2003/08/18 09:34:38  bastian
** fixed dependcy for connectors
**
** Revision 1.4  2003/07/14 08:50:23  bastian
** fix num-o-para for sizeset
**
** Revision 1.3  2003/07/04 06:27:47  bastian
** various
**
** Revision 1.2  2003/05/23 14:30:02  bastian
** week-end
**
** Revision 1.1  2003/05/19 12:06:05  bastian
** 1st
**
*/
