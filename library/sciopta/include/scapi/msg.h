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
** (c) 2006 Sciopta Systems GmbH Deutschland, Sciopta AG/ Schweiz   **
**                                                                  **
**********************************************************************
** ID: S06055BS4                                                    **
** +Revision: 1.4 +                                                 **
** +Date: 2006/05/12 10:26:22 +                                     **
** Message definitions and API                                      **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_MSG_H_
#define _KERNEL_MSG_H_ 2

#define SC_MSGRX_MSGID  1
#define SC_MSGRX_PID    2
#define SC_MSGRX_BOTH   3
#define SC_MSGRX_NOT    4

#define SC_MSGRX_ALL  ((const void *)0)
#define NULL_MSG ((sc_msg_t)0)

/*
** Message header
*/
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

struct sc_msg_rx_s{
  sc_pid_t pid;
  sc_msgid_t msgid;
};
typedef struct sc_msg_rx_s sc_msg_rx_t;

SCFUNC4(DLLEXP sc_msg_t,sc_msgAlloc,sc_bufsize_t,sc_msgid_t,sc_poolid_t,sc_ticks_t);
SCFUNC4(DLLEXP sc_msg_t,sc_msgAllocClr,sc_bufsize_t,sc_msgid_t,sc_poolid_t,sc_ticks_t);
SCFUNC1(DLLEXP void,sc_msgFree,sc_msgptr_t);
SCFUNC1(DLLEXP void,sc_msgAcquire,sc_msgptr_t);
SCFUNC1(DLLEXP sc_pid_t,sc_msgAddrGet,sc_msgptr_t);
SCFUNC1(DLLEXP sc_pid_t,sc_msgSndGet,sc_msgptr_t);
SCFUNC1(DLLEXP sc_pid_t,sc_msgOwnerGet,sc_msgptr_t);
SCFUNC1(DLLEXP sc_bufsize_t,sc_msgSizeGet,sc_msgptr_t);
SCFUNC1(DLLEXP sc_poolid_t,sc_msgPoolIdGet,sc_msgptr_t);
SCFUNC2(DLLEXP sc_bufsize_t,sc_msgSizeSet,sc_msgptr_t,sc_bufsize_t);
SCFUNC3(DLLEXP void,sc_msgTx,sc_msgptr_t,sc_pid_t,flags_t);
SCFUNC4(DLLEXP void,sc_msgTxAlias,sc_msgptr_t,sc_pid_t,flags_t,sc_pid_t);
SCFUNC3(DLLEXP sc_msg_t,sc_msgRx,sc_ticks_t,const void *,flags_t);
//->#if SC_MSG_HOOK == 1
//->typedef void (sc_msgHook_t)(const sc_msg_hd_t *);
//->SCFUNC2(sc_msgHook_t *,sc_msgHookRegister,int,sc_msgHook_t *);
//->#endif


/*
** Non-public function (use with care !)
*/
void sc_sysMsgFree(sc_msg_hd_t *mh);

#ifndef __KERNEL__
#define sc_msgAlloc(size,id,pool,tmo)     _FUNC4(sc_msgAlloc,size,id,pool,tmo)
#define sc_msgAllocClr(size,id,pool,tmo)  _FUNC4(sc_msgAllocClr,size,id,pool,tmo)
#define sc_msgFree(msgptr)		  _FUNC1(sc_msgFree,msgptr)
#define sc_msgTx(msgptr,pid,flags)        _FUNC3(sc_msgTx,msgptr,pid,flags)
#define sc_msgTxAlias(msgptr,pid,flags,alias_pid)   _FUNC4(sc_msgTxAlias,msgptr,pid,flags,alias_pid)
#define sc_msgRx(tmo,select,flags)        _FUNC3(sc_msgRx,tmo,select,flags)
#define sc_msgAcquire(msgptr)		  _FUNC1(sc_msgAcquire,msgptr)
#define sc_msgAddrGet(msgptr)		  _FUNC1(sc_msgAddrGet,msgptr)
#define sc_msgSndGet(msgptr)		  _FUNC1(sc_msgSndGet,msgptr)
#define sc_msgOwnerGet(msgptr)		  _FUNC1(sc_msgOwnerGet,msgptr)
#define sc_msgSizeGet(msgptr)		  _FUNC1(sc_msgSizeGet,msgptr)
#define sc_msgSizeSet(msgptr,size)	  _FUNC2(sc_msgSizeSet,msgptr,size)
#define sc_msgPoolIdGet(msgptr)		  _FUNC1(sc_msgPoolIdGet,msgptr)
//->#if SC_MSG_HOOK == 1
//->#define sc_msgHookRegister(a,b)  _FUNC2(sc_msgHookRegister,a,b)
//->#endif
#endif /* __KERNEL__ */

#endif /* _KERNEL_MSG_H_ */

