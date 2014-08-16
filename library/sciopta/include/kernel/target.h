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
** +Revision: 2.42.4.2 +                                                **
** +Date: 2009/09/16 14:34:12 +                                     **
** Global system data                                               **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_TARGET_H_
#define _KERNEL_TARGET_H_
#ifdef _SC_CONF_H_
/* SOCNV */
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
#define SC_CONF_STACKCHK   0x10000000
#define SC_CONF_CPUTYPE    0xe0000000

#define SC_CONF_MCF_NOMAC  0x00000000
#define SC_CONF_MCF_MAC    0x20000000
#define SC_CONF_MCF_EMAC   0x40000000
#define SC_CONF_MCF_FPU    0x80000000
/* EOCNV */
#ifndef __ASM_INCLUDED__

/*
** sciopta_t is only available if the kernel if configured, else
** there are different sized versions in the executable.
*/
#if _SC_CONF_H_ == 0x42
struct sciopta_s {
  sc_pid_t curPid;           /* PID of the current running process */
  sc_pcb_t * curPcb;         /* PCB of the current running process */
  sc_pcb_t * curPrioPcb;     /* PCB of the current prioritized process */
  sc_module_cb_t *curModule; /* ModuleCB of the current active module */
  __u32 prioMask;            /* Priority bitmask. One bit per priority */
                             /* for all 32 priorities. */
                             /* A set bit indicates that the corresponding */
                             /* ready list has at least one process */
  __u16 nested_irq_h;        /* Nested interrupt counter */
  __u16 version_h;           /* TBD */

  int scheduler_lock;        /* Scheduler lock counter. No scheduler locks */
  int schedule_req;	     /* Scheduling request flag. If set == request */
  sc_ticks_t suspend_req;    /* TBD */
  sc_ticks_t tick;           /* Current system-time in ticks */
  __u32 conf;                /* Configuration word */
  __uint nmodule;            /* Module counter. Number of active modules */

  __u32 ticklength;          /* Lenght of a tick in us */
  sc_pid_t kerneld;          /* PID of the kernel daemon */
  sc_pid_t procd;            /* PID of the process daemon */
  __u32 spare;               /* Reserved */
#ifdef __C_KERNEL__
  unsigned int max_module;
  unsigned int max_connector;
  unsigned int max_intvector;
#endif
  dbl_t readyList[32];       /* Process list heads ready to run */ 

  dbl_t sleeperLists[256];   /* Sleeper process list heads */
#if SC_ASYNC_TMO == 1
  dbl_t tmoLists[256];       /* Time-out message list heads */
#endif

#if SC_PROC_STAT == 1
  /*
  ** non-kernel globals
  */
  __u32 swaps;               /* Number of swaps since system start */
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
#if SC_PROCMMU_HOOK == 1
  sc_procHook_t *procMMUhook;
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

#if SCIOPTA == WIN32_HOSTED
externC int sciopta_start(char *cmdline,
			  sciopta_t *psciopta,
			  sc_pcb_t **connectors,
			  sc_pcb_t **pirq_vectors,
			  sc_module_cb_t **modules,
			  void (*start_hook)(void),
			  void (*TargetSetup)(void),
			  void (*sysPutchar)(int ),
			  void (*idle_hook)(void));

externC void sciopta_end(void);
#endif

#endif /* _SC_CONF_H_ == 0x42 */

#endif /* __ASM_INCLUDED__ */

/* SOCNV */
/* ARM asm-kernel private */
#if CPU == ARM
#define SC_RECI_TCKLENGTH (-4)
#endif
/* all kernels */

#define SC_CURPID         (0x00)
#define SC_CURPCB         (0x04)
#define SC_CURPRIOPCB     (0x08)
#define SC_CURMODULE      (0x0c)
#define SC_PRIOMASK       (0x10)
#define SC_NESTED_IRQ_H   (0x14)
#define SC_VERSION_H      (0x16)
#define SC_SCHEDULER_LOCK (0x18)
#define SC_SCHEDULE_REQ   (0x1c)
#define SC_SUSPEND_REQ    (0x20)
#define SC_TCK            (0x24)
#define SC_CONF           (0x28)
#define SC_NMODULE        (0x2c)
#define SC_TCKLENGTH      (0x30)
#define SC_KERNELD        (0x34)
#define SC_PROCD          (0x38)
#define SC_SPARE          (0x3c)

#ifdef __C_KERNEL__
#define SC_MAXMODULE      (0x40)
#define SC_MAXCONNECTOR   (0x44)
#define SC_MAXINTVECTOR   (0x48)
#define SC_READYLIST      (0x4c)
#else
#define SC_READYLIST      (0x40)
#endif

/*
** Following defines depend on the actual configuaration and
** thus are not available without sconf.h or sciopta.cnf
*/

#if _SC_CONF_H_ == 0x42

#define SC_SLEEPERLISTS   (SC_READYLIST+32*8)

#define SC_TMOLISTS       (SC_SLEEPERLISTS+256*8)
#if SC_ASYNC_TMO == 1
#define SC_TMOLISTSE      ((SC_TMOLISTS+256*8))
#else
#define SC_TMOLISTSE      (SC_TMOLISTS)
#endif

#if SC_PROC_STAT == 1
#define SC_SWAPS           (SC_TMOLISTSE)
#else
#define SC_SWAPS           (SC_TMOLISTSE-4)
#endif

#if SC_POOL_HOOK == 1
#if SC_POOLCREATE_HOOK == 1
#define SC_POOLCREATEHOOK (SC_SWAPS+4)
#else
#define SC_POOLCREATEHOOK (SC_SWAPS)
#endif
#if SC_POOLKILL_HOOK == 1
#define SC_POOLKILLHOOK   (SC_POOLCREATEHOOK+4)
#else
#define SC_POOLKILLHOOK   (SC_POOLCREATEHOOK)
#endif
#else
#define SC_POOLKILLHOOK   (SC_SWAPS)
#endif

#if SC_MSG_HOOK == 1
#if SC_MSGTX_HOOK == 1
#define SC_MSGTXHOOK     (SC_POOLKILLHOOK+4)
#else
#define SC_MSGTXHOOK     (SC_POOLKILLHOOK)
#endif
#if SC_MSGRX_HOOK == 1
#define SC_MSGRXHOOK     (SC_MSGTXHOOK+4)
#else
#define SC_MSGRXHOOK     (SC_MSGTXHOOK)
#endif
#else
#define SC_MSGRXHOOK     (SC_POOLKILLHOOK)
#endif

#if SC_PROC_HOOK == 1
#if SC_PROCCREATE_HOOK == 1
#define SC_PROCCREATEHOOK (SC_MSGRXHOOK+4)
#else
#define SC_PROCCREATEHOOK (SC_MSGRXHOOK)
#endif
#if SC_PROCKILL_HOOK == 1
#define SC_PROCKILLHOOK   (SC_PROCCREATEHOOK+4)
#else
#define SC_PROCKILLHOOK   (SC_PROCCREATEHOOK)
#endif
#if SC_PROCSWAP_HOOK == 1
#define SC_PROCSWAPHOOK   (SC_PROCKILLHOOK+4)
#else
#define SC_PROCSWAPHOOK   (SC_PROCKILLHOOK)
#endif
#if SC_PROCMMU_HOOK == 1
#define SC_PROCMMUHOOK (SC_PROCSWAPHOOK+4)
#else
#define SC_PROCMMUHOOK (SC_PROCSWAPHOOK)
#endif
#else
#define SC_PROCMMUHOOK   (SC_MSGRXHOOK)
#endif

#if SC_ERR_HOOK == 1
#define SC_ERRHOOK        (SC_PROCMMUHOOK+4)
#else
#define SC_ERRHOOK        (SC_PROCMMUHOOK)
#endif

#if CPU == PPC
#define SC_R2_SAVE (SC_ERRHOOK+4)
#define SC_R13_SAVE (SC_R2_SAVE+4)
#else
#define SC_R13_SAVE (SC_ERRHOOK)
#endif

#define SIZEOF_SCIOPTA    (SC_R13_SAVE+4)
#endif /* _SC_CONF_H_ == 0x42 */
/* EOCNV */
#endif /* _SC_CONF_H_ */
#endif /* _KERNEL_TARGET_H_ */

