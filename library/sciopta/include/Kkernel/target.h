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
** ID: S01103BS16                                                   **
** +Revision: 1.14 +                                                **
** +Date: 2008/01/16 13:02:02 +                                     **
** Global system data                                               **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_TARGET_H_
#define _KERNEL_TARGET_H_

#ifdef _SC_CONF_H_

#define SC_CONF_MODULES    0x0000007f
#define SC_CONF_MSG_STAT   0x00000080
#define SC_CONF_CONNECTORS 0x00007f00
#define SC_CONF_PROC_STAT  0x00008000
#define SC_CONF_BUFFERS    0x00030000
#define SC_CONF_PL_CR_HOOK 0x00040000
#define SC_CONF_PL_KL_HOOK 0x00080000
#define SC_CONF_MS_TX_HOOK 0x00100000
#define SC_CONF_MS_RX_HOOK 0x00200000
#define SC_CONF_PR_CR_HOOK 0x00400000
#define SC_CONF_PR_KL_HOOK 0x00800000
#define SC_CONF_PR_SW_HOOK 0x01000000
#define SC_CONF_ERR_HOOK   0x02000000
#define SC_CONF_MSG_CHECK  0x04000000
#define SC_CONF_FRIENDS    0x08000000

#ifndef __ASM_INCLUDED__

#if _SC_CONF_H_ == 0x42
/*
** Possible savings:
** Remove in assembler version max_process,max_pools => 2 bytes
** Remove in assembler version max_connector => 1 byte
** Remove nconnector if not needed.
** make nested_irq_h byte => 1byte (msp430 2 bytes)
** Adjust priomask with nof priorities.
*/
struct sciopta_s {
#if SIZEOF_PTR == 2 && CPU != MSP430
  __u8 curPid;
#else
  sc_pid_t curPid;
#endif
  sc_pcb_t * curPcb;
  sc_pcb_t * curPrioPcb;

  __u16 nested_irq_h;
  __u8 scheduler_lock;
  __u8 schedule_req;

  __u8 max_process;
  __u8 nprocess;
  __u8 max_connector;
  __u8 nconnector;

  __u8 max_pools;
  __u8 npools;
  __u16 ticklength;              /* lenght of a tick in us */
  sc_ticks_t suspend_req;
  __u32 prioMask;                /* bit set => list contains processes */
  sc_time_t tick;               /* system-time in ticks */
  __u8 readyListHead[32];    /* list of processes ready to run */  
  __u8 readyListTail[32];
  __u8 sleeperLists[256];
#if SC_USE_TMO == 1
  sgl_t tmoLists[256];
#endif
#if SC_PROC_STAT == 1
  /*
  ** non-kernel globals
  */
  __u32 swaps;
#endif
  /*
  ** Hooks
  */
#if SC_POOL_HOOK == 1
#if SC_POOLCREATE_HOOK == 1
  sc_poolHook_t *poolCreateHook;
#endif
#if SC_POOLKILL_HOOK == 1
  sc_poolHook_t *poolKillHook;
#endif
#endif

#if SC_MSG_HOOK == 1
#if SC_MSGTX_HOOK == 1
  sc_msgHook_t *msgTxHook;
#endif
#if SC_MSGRX_HOOK == 1
  sc_msgHook_t *msgRxHook;
#endif
#endif

#if SC_PROC_HOOK == 1
#if SC_PROCCREATE_HOOK == 1
  sc_procHook_t *procCreateHook;
#endif
#if SC_PROCKILL_HOOK == 1
  sc_procHook_t *procKillHook;
#endif
#if SC_PROCSWAP_HOOK == 1
  sc_procHook_t *procSwapHook;
#endif
#endif
#if SC_ERR_HOOK == 1
  sc_errHook_t *errHook;
#endif
#if CPU == PPC
  __u32 r2_save;
  __u32 r13_save;
#endif
#if SCIOPTA != NATIVE_HOSTED
  /*
  ** host-system
  */
  host_t host;
#endif
};
typedef struct sciopta_s sciopta_t;
#if SCIOPTA == NATIVE_HOSTED
extern sciopta_t sciopta;
#endif
#endif /* _SC_CONF_H_ == 0x42 */

#else /* __ASM_INCLUDED__ */
#if SIZEOF_PTR == 2
/* SOCNV */
#define SC_CURPID_B       (0x00)
#if CPU == MSP430 
#define SC_CURPCB         (0x02)
#else
#define SC_CURPCB         (0x01)
#endif
#define SC_CURPRIOPCB     SC_CURPCB+2
#define SC_NESTED_IRQ_H   SC_CURPRIOPCB+2

#define SC_SCHEDULER_LOCK_B SC_NESTED_IRQ_H+2
#define SC_SCHEDULE_REQ_B   SC_SCHEDULER_LOCK_B+1
#define SC_MAX_PROCESS_B    SC_SCHEDULE_REQ_B+1
#define SC_NPROCESS_B       SC_MAX_PROCESS_B+1

#define SC_MAX_CONNECTOR_B (SC_NPROCESS_B+1)
#define SC_NCONNECTOR_B   (SC_MAX_CONNECTOR_B+1)
#define SC_MAX_POOLS_B    (SC_NCONNECTOR_B+1)
#define SC_NPOOLS_B       (SC_MAX_POOLS_B+1)

#define SC_TCKLENGTH      (SC_NPOOLS_B+1)
#define SC_SUSPEND_REQ    (SC_TCKLENGTH+2)
#define SC_PRIOMASK       SC_SUSPEND_REQ+2
#define SC_TCK            (SC_PRIOMASK+4)
#define SC_READYLISTHEAD  (SC_TCK+4)

#if _SC_CONF_H_ == 0x42

#define SC_READYLISTTAIL  (SC_READYLISTHEAD+SC_MAX_PRIORITIES)
#define SC_SLEEPERLISTS   (SC_READYLISTTAIL+SC_MAX_PRIORITIES)
#if SC_USE_TMO == 1
#define SC_TMOLISTS       (SC_SLEEPERLISTS+512)
#define SC_TMOLISTSE      (SC_TMOLISTS+512)
#else
#define SC_TMOLISTSE       (SC_SLEEPERLISTS+256)
#endif
#if SC_PROC_STAT == 1
#define SC_SWAPS           (SC_TMOLISTSE)
#define SC_SWAPSE          (SC_TMOLISTSE+4)
#else
#define SC_SWAPSE          (SC_TMOLISTSE)
#endif

#if SC_POOL_HOOK == 1
#if SC_POOLCREATE_HOOK == 1
#define SC_POOLCREATEHOOK (SC_SWAPSE)
#else
#define SC_POOLCREATEHOOK (SC_SWAPSE-SIZEOF_FUNCPTR)
#endif
#if SC_POOLKILL_HOOK == 1
#define SC_POOLKILLHOOK   (SC_POOLCREATEHOOK+SIZEOF_FUNCPTR)
#else
#define SC_POOLKILLHOOK   (SC_POOLCREATEHOOK)
#endif
#else
#define SC_POOLKILLHOOK   (SC_SWAPSE-SIZEOF_FUNCPTR)
#endif

#if SC_MSG_HOOK == 1
#if SC_MSGTX_HOOK == 1
#define SC_MSGTXHOOK     (SC_POOLKILLHOOK+SIZEOF_FUNCPTR)
#else
#define SC_MSGTXHOOK     (SC_POOLKILLHOOK)
#endif
#if SC_MSGRX_HOOK == 1
#define SC_MSGRXHOOK     (SC_MSGTXHOOK+SIZEOF_FUNCPTR)
#else
#define SC_MSGRXHOOK     (SC_MSGTXHOOK)
#endif
#else
#define SC_MSGRXHOOK     (SC_POOLKILLHOOK)
#endif

#if SC_PROC_HOOK == 1
#if SC_PROCCREATE_HOOK == 1
#define SC_PROCCREATEHOOK (SC_MSGRXHOOK+SIZEOF_FUNCPTR)
#else
#define SC_PROCCREATEHOOK (SC_MSGRXHOOK)
#endif
#if SC_PROCKILL_HOOK == 1
#define SC_PROCKILLHOOK   (SC_PROCCREATEHOOK+SIZEOF_FUNCPTR)
#else
#define SC_PROCKILLHOOK   (SC_PROCCREATEHOOK)
#endif
#if SC_PROCSWAP_HOOK == 1
#define SC_PROCSWAPHOOK   (SC_PROCKILLHOOK+SIZEOF_FUNCPTR)
#else
#define SC_PROCSWAPHOOK   (SC_PROCKILLHOOK)
#endif
#else
#define SC_PROCSWAPHOOK   (SC_MSGRXHOOK)
#endif

#if SC_ERR_HOOK == 1
#define SC_ERRHOOK        (SC_PROCSWAPHOOK+SIZEOF_FUNCPTR)
#else
#define SC_ERRHOOK        (SC_PROCSWAPHOOK)
#endif

#define SIZEOF_SCIOPTA    (SC_ERRHOOK+SIZEOF_FUNCPTR)
#endif /* _SC_CONF_H_ == 0x42 */
/* EOCNV */
#else
#define SC_CURPID         (0x00)
#define SC_CURPCB         (0x04)
#define SC_CURPRIOPCB     (0x08)
#define SC_NESTED_IRQ_H   (0x0c)
#define SC_SCHEDULER_LOCK_B (0x0e)
#define SC_SCHEDULE_REQ_B (0x0f)
#define SC_MAX_PROCESS_B  (0x10)
#define SC_NPROCESS_B     (0x11)
#define SC_MAX_CONNECTOR_B (0x12)
#define SC_NCONNECTOR_B   (0x13)
#define SC_MAX_POOLS_B    (0x14)
#define SC_NPOOLS_B       (0x15)
#define SC_TCKLENGTH      (0x16)
#define SC_SUSPEND_REQ    (0x18)
#define SC_PRIOMASK       (0x1c)
#define SC_TCK            (0x20)
#define SC_READYLIST      (0x24)

#if _SC_CONF_H_ == 0x42

#define SC_SLEEPERLISTS   (SC_READYLIST+((32*SIZEOF_PTR)*2))
#define SC_TMOLISTS       (SC_SLEEPERLISTS+((256*SIZEOF_PTR)*2))
#define SC_CONNECTOR      (SC_TMOLISTS+((256*SIZEOF_PTR)*2))
#if SC_PROC_STAT == 1
#define SC_SWAPS           (SC_CONNECTOR+(SC_MAX_CONNECTOR*SIZEOF_PTR))
#else
#define SC_SWAPS           (SC_CONNECTOR+((SC_MAX_CONNECTOR-1)*SIZEOF_PTR))
#endif

#if SC_POOL_HOOK == 1
#if SC_POOLCREATE_HOOK == 1
#define SC_POOLCREATEHOOK (SC_SWAPS+SIZEOF_PTR)
#else
#define SC_POOLCREATEHOOK (SC_SWAPS)
#endif
#if SC_POOLKILL_HOOK == 1
#define SC_POOLKILLHOOK   (SC_POOLCREATEHOOK+SIZEOF_PTR)
#else
#define SC_POOLKILLHOOK   (SC_POOLCREATEHOOK)
#endif
#else
#define SC_POOLKILLHOOK   (SC_SWAPS)
#endif

#if SC_MSG_HOOK == 1
#if SC_MSGTX_HOOK == 1
#define SC_MSGTXHOOK     (SC_POOLKILLHOOK+SIZEOF_PTR)
#else
#define SC_MSGTXHOOK     (SC_POOLKILLHOOK)
#endif
#if SC_MSGRX_HOOK == 1
#define SC_MSGRXHOOK     (SC_MSGTXHOOK+SIZEOF_PTR)
#else
#define SC_MSGRXHOOK     (SC_MSGTXHOOK)
#endif
#else
#define SC_MSGRXHOOK     (SC_POOLKILLHOOK)
#endif

#if SC_PROC_HOOK == 1
#if SC_PROCCREATE_HOOK == 1
#define SC_PROCCREATEHOOK (SC_MSGRXHOOK+SIZEOF_PTR)
#else
#define SC_PROCCREATEHOOK (SC_MSGRXHOOK)
#endif
#if SC_PROCKILL_HOOK == 1
#define SC_PROCKILLHOOK   (SC_PROCCREATEHOOK+SIZEOF_PTR)
#else
#define SC_PROCKILLHOOK   (SC_PROCCREATEHOOK)
#endif
#if SC_PROCSWAP_HOOK == 1
#define SC_PROCSWAPHOOK   (SC_PROCKILLHOOK+SIZEOF_PTR)
#else
#define SC_PROCSWAPHOOK   (SC_PROCKILLHOOK)
#endif
#else
#define SC_PROCSWAPHOOK   (SC_MSGRXHOOK)
#endif

#if SC_ERR_HOOK == 1
#define SC_ERRHOOK        (SC_PROCSWAPHOOK+SIZEOF_PTR)
#else
#define SC_ERRHOOK        (SC_PROCSWAPHOOK)
#endif

#if CPU == PPC
#define SC_R2_SAVE (SC_ERRHOOK+SIZEOF_PTR)
#define SC_R13_SAVE (SC_R2_SAVE+SIZEOF_PTR)
#else
#define SC_R13_SAVE (SC_ERRHOOK)
#endif

#define SIZEOF_SCIOPTA    (SC_R13_SAVE+SIZEOF_PTR)
#endif /* _SC_CONF_H_ == 0x42 */
#endif /* SIZEOF_PTR == 2 */ 
#endif /* __ASM_INCLUDED__ */
#endif /* _SC_CONF_H_ */
#endif /* _KERNEL_TARGET_H_ */

