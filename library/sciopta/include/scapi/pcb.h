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
** ID: S06055BS8                                                    **
** +Revision: 1.8 +                                                 **
** +Date: 2006/10/11 14:01:40 +                                     **
** Process Controll Block                                           **
**********************************************************************
EOC*/
#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_PCB_H_
#define _KERNEL_PCB_H_

#define SC_PROC_NAME_SIZE 31

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

/*
** Process Controll Block
*/
typedef struct sc_pcb_s {
  dbl_t queue;
  dbl_t alloced;
  dbl_t pcbList;
  dbl_t observe;
  char name[SC_PROC_NAME_SIZE+1];
  sc_pid_t pid;
  sc_prio_t prio;
  sc_time_t slice;
  sc_time_t initial;
  void (*func)(void);
  unsigned int defaultPool;
  unsigned int flags;
  sc_errcode_t errorcode;
  unsigned int cline;
  const char *cfile;
#ifdef WIN32
  /*
  ** win32 specific part
  */
  CRITICAL_SECTION queueMutex;
  HANDLE waitRx; 
  HANDLE thisThread;
  DWORD tid;
#else
  pthread_cond_t waitRx;
  pthread_t thisThread;
  pthread_mutex_t queueMutex;
#endif
} sc_pcb_t;


#endif

