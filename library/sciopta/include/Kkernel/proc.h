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
** ID: S01103BS8                                                    **
** +Revision: 1.16 +                                                **
** +Date: 2008/01/16 13:02:02 +                                     **
** Process API                                                      **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_PROC_H_
#define _KERNEL_PROC_H_ 1
/* SOCNV */
#if SC_PROC_HOOK == 1
#define SC_SET_PROCCREATE_HOOK (1)
#define SC_SET_PROCKILL_HOOK (2)
#define SC_SET_PROCSWAP_HOOK (3)
#endif
#define SC_PROCKILL_KILL          0x1
/* EOCNV */


#if SIZEOF_PTR == 2
/* SOCNV */
#if CPU == M68HC12
#define PDB_STACKSIZE 0
#define PDB_PCB       2
#define PDB_STACK     4
#define PDB_ENTRY     6
#define PDB_NAME      8+BANK_OFF
#define PDB_TYPE     10+(BANK_OFF)
#define PDB_INITIAL  12+(BANK_OFF)
#define PDB_SLICE    14+(BANK_OFF)
#define PDB_VECTOR   12+(BANK_OFF)
#define PDB_PRIO     12+(BANK_OFF)
#define PDB_MAX_SIZE (PDB_SLICE+2)
#else
#define PDB_STACKSIZE 0
#define PDB_PCB       2
#define PDB_STACK     4
#define PDB_ENTRY     6
#define PDB_NAME      8+BANK_OFF
#define PDB_TYPE     10+(2*BANK_OFF)
#define PDB_INITIAL  12+(2*BANK_OFF)
#define PDB_SLICE    14+(2*BANK_OFF)
#define PDB_VECTOR   12+(2*BANK_OFF)
#define PDB_PRIO     12+(2*BANK_OFF)
#define PDB_MAX_SIZE (PDB_SLICE+2)
#endif
/* EOCNV */
#else
#define PDB_STACKSIZE 0
#define PDB_PCB       4
#define PDB_STACK     8
#define PDB_ENTRY    12
#define PDB_NAME     16
#define PDB_TYPE     20
#define PDB_INITIAL  24
#define PDB_SLICE    28
#define PDB_VECTOR   24
#define PDB_PRIO     24
#endif

#define SC_PDB_PRIO (SC_PROCPRIOCREATESTATIC>>24)
#define SC_PDB_INT (SC_PROCINTCREATESTATIC>>24)
#define SC_PDB_TIM (SC_PROCTIMCREATESTATIC>>24)
/* Currently removed 
#define SC_PDB_USRINT (SC_PROCUSRINTCREATESTATIC>>24)
*/

/* SOCNV */
#define SC_PDB_STATE_RUN 1
#define SC_PDB_STATE_STP 0
/* EOCNV */

#ifndef __ASM_INCLUDED__

typedef struct sc_pdbcommon_s {
  sc_bufsize_t stacksize;
  sc_pcb_t NEARPTR pcb;
  char NEARPTR stack;
  void (* entry)(void);
  const char FARPTR name;
  int type;
} sc_pdbcommon_t;

typedef struct sc_pdbtim_s {
  sc_bufsize_t stacksize;
  sc_pcb_t NEARPTR pcb;
  char NEARPTR stack;
  void (* entry)(int);
  const char FARPTR name;
  int type;

  int initial_delay;
  int slice;
} sc_pdbtim_t;

typedef struct sc_pdbint_s {
  sc_bufsize_t stacksize;
  sc_pcb_t NEARPTR pcb;
  char NEARPTR stack;
  void (* entry)(int);
  const char FARPTR name;
  int type;

  int vector;
} sc_pdbint_t;

typedef struct sc_pdbprio_s {
  sc_bufsize_t stacksize;
  sc_pcb_t NEARPTR pcb;
  char NEARPTR stack;
  void (* entry)(void);
  const char FARPTR name;
  int type;

  __u8 prio;
} sc_pdbprio_t;

typedef union sc_pdb_u {
  sc_pdbcommon_t pdb;
  sc_pdbprio_t prio;
  sc_pdbint_t irq;
  sc_pdbtim_t tim;
} sc_pdb_t;

/*
** Create a process.
*/
SCFUNC2(sc_pid_t,sc_procCreate,sc_pdb_t FARPTR ,int);

#if SC_PROC_HOOK == 1
typedef void (sc_procHook_t)(const sc_pcb_t NEARPTR,const sc_pcb_t NEARPTR);
SCFUNC2(sc_procHook_t *,sc_procHookRegister,int,sc_procHook_t *);
#endif  

/*
** Kill a process.
*/
SCFUNC2(void,sc_procKill,sc_pid_t,flags_t);
/*
** Locks the scheduler and returns the number of times
** it has been locked before.
*/
SCFUNC0(int,sc_procSchedLock);
/*
** Unlock the scheduler. Actually, decrement the lock
** counter by one.
*/
SCFUNC0(void,sc_procSchedUnlock);
/*
** Start a process (can be a timer too).
*/
SCFUNC1(void,sc_procStart,sc_pid_t);
/*
** Stop a process (can be a timer too).
*/
SCFUNC1(void,sc_procStop,sc_pid_t);
/*
** Get the full name of a process
*/
SCFUNC1(sc_msg_t,sc_procNameGet,sc_pid_t);
/*
** Get the path of a process
*/
SCFUNC2(sc_msg_t,sc_procPathGet,sc_pid_t,flags_t);
/*
** Check if path is a correct sciopta path: {//system}[/module][/]process
*/
SCFUNC1(int,sc_procPathCheck,const char FARPTR);
/*
** Get the process name of the current running process
*/
externC const char FARPTR sc_sysProcCurrentName(void);
/*
** Get the process id of a process.
*/
SCFUNC2(sc_pid_t,sc_procIdGet,const char FARPTR,sc_ticks_t);
/*
** Get the parent id of a process
*/
SCFUNC1(sc_pid_t,sc_procPpidGet,sc_pid_t);
/*
** Get a process's priority.
*/
SCFUNC1(sc_prio_t,sc_procPrioGet,sc_pid_t);
/*
** Change the priority of the current process.
*/
SCFUNC1(void,sc_procPrioSet,sc_prio_t);
/*
** Get the timeslice of a process.
** Works only for timers.
*/
SCFUNC1(sc_ticks_t,sc_procSliceGet,sc_pid_t);
/*
** Set the timeslice of a timer-process
*/
SCFUNC2(void,sc_procSliceSet,sc_pid_t,sc_ticks_t);
/*
** Get the interrupt vector.
** Works only for interrupts
*/
#define sc_procVectorGet(a) (int)sc_procSliceGet(a)

/*
** Yield CPU to the next ready process within the
** current process's priority group.
*/
SCFUNC0(void,sc_procYield);
/*
** Request a message if process pid dies.
*/
SCFUNC2(void,sc_procObserve,sc_msgptr_t,sc_pid_t);
/*
** Register a procd which is responsible for 
** idGet request, the kernel can not immediatly
** answer.
*/
SCFUNC0(int,sc_procRegisterDaemon);
/*
** Unregister the procd.
*/
SCFUNC0(void,sc_procUnregisterDaemon);
/*
** Get the process's PCB (real address !!).
*/
externC sc_pcb_t * sc_sysProcPcbGet(sc_pid_t pid);
#ifndef __KERNEL__
#define sc_procCreate(a,b)   _FUNC2(sc_procCreate,a,b)
#define sc_procKill(a,b)     _FUNC2(sc_procKill,a,b)
#define sc_procSchedLock()   _FUNC0(sc_procSchedLock)
#define sc_procSchedUnlock() _FUNC0(sc_procSchedUnlock)
#define sc_procStart(a)      _FUNC1(sc_procStart,a)
#define sc_procStop(a)       _FUNC1(sc_procStop,a)
#define sc_procNameGet(a)    _FUNC1(sc_procNameGet,a)
#define sc_procPathGet(a,b)    _FUNC2(sc_procPathGet,a,b)
#define sc_procPathCheck(a)  _FUNC1(sc_procPathCheck,a)
#define sc_procIdGet(a,b)    _FUNC2(sc_procIdGet,a,b)
#define sc_procPpidGet(a)    _FUNC1(sc_procPpidGet,a)
#define sc_procPrioGet(a)    _FUNC1(sc_procPrioGet,a)
#define sc_procPrioSet(a)    _FUNC1(sc_procPrioSet,a)
#define sc_procSliceGet(a)   _FUNC1(sc_procSliceGet,a)
#define sc_procSliceSet(a,b) _FUNC2(sc_procSliceSet,a,b)
#define sc_procYield()       _FUNC0(sc_procYield)
#define sc_procObserve(a,b)  _FUNC2(sc_procObserve,a,b)
#define sc_procRegisterDaemon() _FUNC0(sc_procRegisterDaemon)
#define sc_procUnregisterDaemon() _FUNC0(sc_procUnregisterDaemon)
#if SC_PROC_HOOK == 1
#  define sc_procHookRegister(a,b) _FUNC2(sc_procHookRegister,a,b)
#endif
#endif /* __KERNEL__ */
#endif /* __ASM_INCLUDED__ */
#endif /* _KERNEL_PROC_H_ */

