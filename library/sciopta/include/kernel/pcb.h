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
** ID: S01103BS49                                                   **
** +Revision: 2.48.12.1 +                                                **
** +Date: 2009/08/14 04:43:38 +                                     **
** Process control block definitions                                **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif
#ifndef _KERNEL_PCB_H_
#define _KERNEL_PCB_H_ 1

#if SCIOPTA == LINUX_HOSTED
#include <setjmp.h>
#endif

#ifdef SC_MODULE_NAME_SIZE
#define SC_PROC_NAME_SIZE (SC_MODULE_NAME_SIZE)
#else
/* SOCNV */
#define SC_PROC_NAME_SIZE 31
/* EOCNV */
#endif

/*
****************************************
** Process-states
****************************************
*/
#if SIZEOF_PTR == 2
#define ST_TMO                0x01  /* a timeout occured */
#define ST_TMO_POS            0
#define ST_TRIGGER            0x02  /* process has been triggered */
#define ST_READY              0x03
#define ST_WAIT_MSGALLOC      0x10  /* process waits for a free msg */
#define ST_WAIT_MSGALLOC_POS  4
#define ST_WAIT_MSGRX         0x20
#define ST_WAIT_TRIGGER       0x40

#define ST_TMO_MASK           0x07

#define ST_WAIT_TMO           0x08  /* process wait for an timeout */
#define ST_WAIT_TMO_POS       3
#define ST_STOPPED            0x80  /* process is stopped */
#define ST_STOPPED_POS        7

#define ST_NOT_READY_MASK     0xfc
#else
/* SOCNV */
#define ST_TMO                0x0001  /* a timeout occured */
#define ST_TMO_POS            0
#define ST_TRIGGER            0x0002  /* process has been triggered */
#define ST_READY              0x000f
#define ST_READY_POS          3
#define ST_WAIT_MSGALLOC      0x0010  /* process waits for a free msg */
#define ST_WAIT_MSGALLOC_POS  4
#define ST_WAIT_MSGRX         0x0020
#define ST_WAIT_MSGRX_POS     5
#define ST_WAIT_TRIGGER       0x0040
#define ST_WAIT_TRIGGER_POS   6
#define ST_TMO_MASK           0xff0f
#define ST_NOT_TMO_MASK       0x00f0
#define ST_WAIT_TMO           0x0080  /* process wait for an timeout */
#define ST_WAIT_TMO_POS       7
#define ST_STOPPED            0x0200  /* process is stopped */
#define ST_STOPPED_POS        9

#define ST_NOT_READY_MASK     0xfff0
/* EOCNV */
#endif
/*
****************************************
** Process-flags
****************************************
*/
#if SIZEOF_PTR == 2
#define PCB_FLG_READY            0x0001  /* process is ready */
#define PCB_FLG_IS_TIMER         0x0002  /* process is a timer */
#define PCB_FLG_IS_IRQ           0x0004  /* process is an interrupt */
#define PCB_FLG_IS_UIRQ          0x0008  /* process is a user interrupt */
#define PCB_FLG_IS_PRIO          0x0010  /* process is a prio. process */
#define PCB_FLG_IS_STATIC        0x0020  /* process is a static process */
#define PCB_FLG_WAKEUP           0x0040  /* wakeup allowed */
#define PCB_FLG_IS_CONNECTOR     0x0080  /* process is a connector */
#else
/* SOCNV */
#define PCB_FLG_READY            0x0001  /* process is ready */
#define PCB_FLG_READY_POS        0
#define PCB_FLG_IS_TIMER         0x0002  /* process is a timer */
#define PCB_FLG_IS_TIMER_POS     1
#define PCB_FLG_IS_IRQ           0x0004  /* process is an interrupt */
#define PCB_FLG_IS_IRQ_POS       2 
#define PCB_FLG_IS_FIRQ          0x0008  /* process is a fast interrupt */
#define PCB_FLG_IS_FIRQ_POS      3 
#define PCB_FLG_IS_UIRQ          0x0010  /* process is a user interrupt */
#define PCB_FLG_IS_UIRQ_POS      4 
#define PCB_FLG_IS_PRIO          0x0020  /* process is a prio. process */
#define PCB_FLG_IS_PRIO_POS      5
#define PCB_FLG_IS_STATIC        0x0040  /* process is a static process */
#define PCB_FLG_IS_STATIC_POS    6
#define PCB_FLG_WAKEUP_DIS       0x0080  /* wakeup not allowed */
#define PCB_FLG_WAKEUP_DIS_POS   7

#define PCB_FLG_WAKEUP_BY_TX     0x0100  /* txMsg (CKernel) */
#define PCB_FLG_WAKEUP_BY_Tx_POS 8
#define PCB_FLG_FPU              0x0100  /* process may use FPU */
#define PCB_FLG_FPU_POS          8
#define PCB_FLG_WAKEUP_BY_TRG    0x0200  /* triggger (CKernel) */
#define PCB_FLG_WAKEUP_BY_TRG_POS 9
#define PCB_FLG_IS_CONNECTOR     0x0400  /* process is a connector */
#define PCB_FLG_IS_CONNECTOR_POS 10
#define PCB_FLG_IS_IDLE          0x0800  /* idle process */
#define PCB_FLG_IS_IDLE_POS      11
#define PCB_FLG_IS_RP            0x1000  /* process is reverse-proxy */
#define PCB_FLG_IS_RP_POS        12
#define PCB_FLG_ACTIVE           0x4000  /* timer is active */
#define PCB_FLG_ACTIVE_POS       14
#define PCB_FLG_PCB_LOCKED       0x8000
#define PCB_FLG_PCB_LOCKED_POS   15
/* EOCNV */
#endif

/* SOCNV */
#define SC_PCB_MAGIC (SC_MSG_BASE + 0x2207)
#define SC_PCB_MAGIC_STATIC (SC_MSG_BASE + 0x3008)
/* EOCNV */

/*
****************************************
** offsets
****************************************
*/
#if SIZEOF_PTR == 2

#define PCB_MAGIC        0
#define PCB_MSGQUEUE     2
#define PCB_ALLOCED      6
#define PCB_CONNECTED    10
#define PCB_PCBLIST      14
#define PCB_SLEEPERS     16
#define PCB_TMO          18
#define PCB_PID          20
#define PCB_CREATOR      22
#define PCB_PRIO         24
#define PCB_PRIO0        25
#define PCB_SLICE        26
#define PCB_VECTOR       26
#define PCB_STACKPTR     28
#define PCB_STACKBTM     30
#define PCB_STACKTOP     32
#define PCB_NAME         34
#define PCB_ERRORCODE    36
#define PCB_ENTRY        40
#define PCB_FLAGS        42
#define PCB_STATE        43
#define PCB_STOPPED      44
#define PCB_HASH         45
#define PCB_TRIGGER_VAL  47
#define PCB_NQUEUE       49
#define PCB_NALLOC       50
#define PCB_DEFAULTPOOL  51
#define PCB_LOCALS       52
#define PCB_CLINE        54
#define PCB_CFILE        56
#define SIZEOF_PCB       58
#else /* SIZEOF_PTR == 2*/
/* SOCNV */
#define PCB_MAGIC        0

#define PCB_FLAGS_H      4
#define PCB_STATE_H      6
#define PCB_STOPPED_H    8
#define PCB_HASH_H       10

/* prio process */
#define PCB_PRIO_B       12
#define PCB_PRIO0_B      13
#define PCB_DEFAULTPOOL_H 14
#define PCB_CURSLICE     16
#define PCB_PRI_SLICE    20
#define PCB_PRI_TMO      24
/* timer process */
#define PCB_TIMSPARE_H   12
#define PCB_TIM_DEFAULTPOOL_H 14
#define PCB_INITIAL      16
#define PCB_SLICE        20
#define PCB_TMO          24
/* interrupt */
#define PCB_NESTED_H     12
#define PCB_INT_DEFAULTPOOL_H 14
#define PCB_INTSPARE2    16
#define PCB_VECTOR       20
#define PCB_INTSPARE3    24

#define PCB_MSGQUEUE     28
#define PCB_ALLOCED      36
#define PCB_CONNECTED    44
#define PCB_PCBLIST      52
#define PCB_SLEEPERS     60

#define PCB_PID          68
#define PCB_STACKPTR     72
#define PCB_STACKBTM     76
#define PCB_STACKTOP     80
#define PCB_ERRORCODE    84
#define PCB_ENTRY        88
#define PCB_TRIGGER_VAL  92
#define PCB_NQUEUE       96
#define PCB_NALLOC       100
#define PCB_LOCALS       104
#define PCB_PRIO_MASK    108
#define PCB_READYLIST    112
#define PCB_NAME         116
#define PCB_CLINE        120
#define PCB_CFILE        124

#define SIZEOF_PCB       128

/* EOCNV */
#endif /* SIZEOF_PTR */

#ifndef __ASM_INCLUDED__
#define PCB(ptr,member) DBL_ITEM(sc_pcb_t,ptr,member)

struct sc_pcb_s {
  magic_t magic;
  __u16 flags;                  /* misc flags */
  __u16 state;                  /* process state */
  __u16 stopped;
  __u16 hash;
  union {
    struct  {
      __u8 prio;               /* effective process' priority */
      __u8 prio0;              /* original priority */
      __u16 defaultPool;
      sc_ticks_t curSlice;
      sc_ticks_t slice;             /* size of time slice */
      sc_ticks_t tmo;               /* timeout-value */
    }priproc;
    struct {
      __u16 nested_h;
      __u16 defaultPool;
      __u32 spare0;
      __u32 vector;
      __u32 spare1;
    } intproc;
    struct {
      __u16 spare_h;
      __u16 defaultPool;
      sc_ticks_t initial;
      sc_ticks_t slice;             /* size of time slice */
      sc_ticks_t tmo;               /* timeout-value */
    } timproc;
  } u1;

  dbl_t msgQueue;               /* root of msgQueue */
  dbl_t alloced;                /* root of list of allocated buffers */
  dbl_t connected;              /* root of list of process that should be */
                                /* notified if the process dies */
  dbl_t pcblist;                /* list of processes */
  dbl_t sleepers;               /* list of sleeping process */
  sc_pid_t pid;                 /* process' own ID */
  char *stackptr;               /* current stack-pointer */
  char *stackbtm;               /* first byte of stack */
  char *stacktop;               /* first byte above stack */
  sc_errcode_t errorcode;      
  void (*entry)(int);
  sc_triggerval_t triggerValue;
  __u32 nQueue;
  __u32 nAlloc;
  sc_local_t *locals;           /* pointer to an array with local vars */
  __u32 priomask;               /* ARM/Coldfire: 1<<prio */
  dbl_t *readylist;             /* ARM/Coldfire: &sciopta.readyLists[prio] */
  const char *name;             /* process' name */
  int cline;                    /* C-line debug: Line of last system call */
  const char *cfile;            /* C-line debug: File of last system call */
  /* 
  ** host-system specific
  */
#if SCIOPTA == LINUX_HOSTED
#include <setjmp.h>
  jmp_buf env;
#endif
#if SCIOPTA == WIN32_HOSTED
  pcb_host_t host;
#endif
};

typedef struct sc_pcb_s sc_pcb_t;

#define PID2IDX(module,pid) ((pid) & 0x3fff)
#define PID_IS_EXTERN(pid)  (pid & PID_EXTERN_FLAG)
#define PID_IS_TARGET(pid) MODULE_IS_TARGET(((pid) & MODULE_MASK))
#endif /* __ASM_INCLUDED__ */
#endif
