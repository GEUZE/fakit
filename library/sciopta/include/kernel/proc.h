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
** +Revision: 2.45.4.1 +                                                **
** +Date: 2009/10/04 09:53:43 +                                     **
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
#define SC_SET_PROCMMU_HOOK (4)
#endif
/* EOCNV */
#ifdef SC_MODULEKILL_KILL
#define SC_PROCKILL_KILL          (SC_MODULEKILL_KILL)
#else
/* SOCNV */
#define SC_PROCKILL_KILL          0x4000
#define SC_PROCKILL_KILL_POS      14
/* EOCNV */
#endif
/* SOCNV */
#define SC_PDB_STATE_RUN 1
#define SC_PDB_STATE_STP 0

/* extended type information */
#define SC_PROC_TYPE_EXT_FPU 1
#define SC_PROC_TYPE_EXT_USR 2

#define SC_PROC_TYPE_EXT_FPU_POS 0
#define SC_PROC_TYPE_EXT_USR_POS 1
/* EOCNV */

#ifndef __ASM_INCLUDED__

/*
** Create a process within a given module.
*/
#if SC_USE_SYSCALL == 0
SCFUNCn(sc_pid_t,sc_sysProcCreate,
	 const char *,       /* name */
	 void (*)(void),     /* function */
	 sc_bufsize_t,       /* stacksize */
	 sc_ticks_t,         /* slice */
	 sc_prio_t,
	 int,
	 sc_moduleid_t,
	 sc_pcb_t *,
	 char *,
	 __uint);
#endif

#if SC_PROC_HOOK == 1
typedef void (sc_procHook_t)(const sc_pcb_t *old_pcb,const sc_pcb_t *new_pcb);
SCFUNCn(sc_procHook_t *,sc_procHookRegister,int,sc_procHook_t *);
#endif  

/*
** Request kerneld to create either a
** a) prioritzed,
** b) interrupt,
** c) timer or
** d) user interrupt
** process.
*/
typedef void (void_f)(void);
typedef void (int_f)(int);

SCFUNCn(sc_pid_t,sc_procPrioCreate,
	const char *,     /* name */
	void_f,           /* function */
	sc_bufsize_t,     /* stack size */
	sc_ticks_t,       /* slice */
	sc_prio_t,        /* prio */
	int,              /* state */
	sc_poolid_t);     /* pool (for pcb/stack) */


SCFUNCn(sc_pid_t,sc_procIntCreate,const char *,int_f,sc_bufsize_t,int,sc_prio_t,int,sc_poolid_t);
SCFUNCn(sc_pid_t,sc_procTimCreate,const char *,int_f,sc_bufsize_t,sc_ticks_t,sc_ticks_t,int,sc_poolid_t);
SCFUNCn(sc_pid_t,sc_procUsrIntCreate,const char *,int_f,sc_bufsize_t,int,sc_prio_t,int,sc_poolid_t);
/*
** Request kerneld to kill a process.
*/
SCFUNCn(void,sc_procKill,sc_pid_t,flags_t);
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
SCFUNCn(void,sc_procStart,sc_pid_t);
/*
** Stop a process (can be a timer too).
*/
SCFUNCn(void,sc_procStop,sc_pid_t);
/*
** Get the full name of a process
*/
SCFUNCn(sc_msg_t,sc_procNameGet,sc_pid_t);
/*
** Get the path of a process
*/
SCFUNCn(sc_msg_t,sc_procPathGet,sc_pid_t,flags_t);
/*
** Check if path is a correct sciopta path: {//system}[/module][/]process
*/
SCFUNCn(int,sc_procPathCheck,const char *);
/*
** Get the process name of the current running process
*/
externC const char * sc_sysProcCurrentName(void);
/*
** Get the process id of a process.
*/
SCFUNCn(sc_pid_t,sc_procIdGet,const char *,sc_ticks_t);
/*
** Get the parent id of a process
*/
SCFUNCn(sc_pid_t,sc_procPpidGet,sc_pid_t);
/*
** Get a process's priority.
*/
SCFUNCn(sc_prio_t,sc_procPrioGet,sc_pid_t);
/*
** Change the priority of the current process.
*/
SCFUNCn(void,sc_procPrioSet,sc_prio_t);
/*
** Get the timeslice of a process.
** Works only for timers.
*/
SCFUNCn(sc_ticks_t,sc_procSliceGet,sc_pid_t);
/*
** Set the timeslice of a timer-process
*/
SCFUNCn(void,sc_procSliceSet,sc_pid_t,sc_ticks_t);
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
SCFUNCn(void,sc_procObserve,sc_msgptr_t,sc_pid_t);
/*
** Remove all observe msg from process pid
*/
SCFUNCn(void,sc_procUnobserve,sc_pid_t,sc_pid_t);
/*
** Register a procd which is responsible for 
** idGet request, the kernel can not immediatly
** answer.
*/
SCFUNC0(int,sc_procDaemonRegister);
/*
** Unregister the procd.
*/
SCFUNC0(void,sc_procDaemonUnregister);
/*
** Disable wakeup for caller (must be timer or interrupt)
*/
SCFUNC0(void,sc_procWakeupDisable);
/*
** Enable wakeup for caller (must be timer or interrupt) (default).
*/
SCFUNC0(void,sc_procWakeupEnable);

/*
** Get the process's PCB (real address !!).
*/
SCFUNCn(sc_pcb_t *,sc_sysProcPcbGet,sc_pid_t);

#ifdef _WIN32
SCFUNCn(void,sc_procRegisterIrqCallback,void (*)(int));
// XXX: SCFUNCn(void,sc_procUnregisterIrqCallback,void (*)(int));
#endif

#ifndef __KERNEL__
#define sc_sysProcCreate(a,b,c,d,e,f,g,h,i,k)\
	  _FUNCn(sc_sysProcCreate,a,b,c,d,e,f,g,h,i,k)

#define sc_procPrioCreate(name,function,stacksize,slice,prio,state,poolid)\
	  _FUNCn(sc_procPrioCreate,name,function,stacksize,slice,prio,state,poolid)
#define sc_procIntCreate(name,function,stacksize,vectornum,prio,state,poolid)\
	  _FUNCn(sc_procIntCreate,name,function,stacksize,vectornum,prio,state,poolid)
#define sc_procTimCreate(name,function,stacksize,slice,prio,state,poolid)\
	  _FUNCn(sc_procTimCreate,name,function,stacksize,slice,prio,state,poolid)
#define sc_procUsrIntCreate(name,function,stacksize,vectornum,prio,state,poolid)\
	  _FUNCn(sc_procUsrIntCreate,name,function,stacksize,vectornum,prio,state,poolid)
#define sc_procKill(pid,flags)	     _FUNCn(sc_procKill,pid,flags)
#define sc_procSchedLock()           _FUNC0(sc_procSchedLock)
#define sc_procSchedUnlock()         _FUNC0(sc_procSchedUnlock)
#define sc_procStart(pid)            _FUNCn(sc_procStart,pid)
#define sc_procStop(pid)             _FUNCn(sc_procStop,pid)
#define sc_procNameGet(pid)          _FUNCn(sc_procNameGet,pid)
#define sc_procPathGet(pid,fullflag) _FUNCn(sc_procPathGet,pid,fullflag)
#define sc_procPathCheck(path)       _FUNCn(sc_procPathCheck,path)
#define sc_procIdGet(name,tmo)	     _FUNCn(sc_procIdGet,name,tmo)
#define sc_procPpidGet(pid)          _FUNCn(sc_procPpidGet,pid)
#define sc_procPrioGet(pid)          _FUNCn(sc_procPrioGet,pid)
#define sc_procPrioSet(prio)         _FUNCn(sc_procPrioSet,prio)
#define sc_procSliceGet(pid)         _FUNCn(sc_procSliceGet,pid)
#define sc_procSliceSet(pid,slice)   _FUNCn(sc_procSliceSet,pid,slice)
#define sc_procYield()               _FUNC0(sc_procYield)
#define sc_procObserve(msgptr,pid)   _FUNCn(sc_procObserve,msgptr,pid)
#define sc_procUnobserve(observer_pid,target_pid) _FUNCn(sc_procUnobserve,observer_pid,target_pid)
#define sc_procDaemonRegister()      _FUNC0(sc_procDaemonRegister)
#define sc_procDaemonUnregister()    _FUNC0(sc_procDaemonUnregister)
#define sc_procWakeupEnable()        _FUNC0(sc_procWakeupEnable)
#define sc_procWakeupDisable()      _FUNC0(sc_procWakeupDisable)

#if SC_PROC_HOOK == 1
#  define sc_procHookRegister(hook,flag) _FUNCn(sc_procHookRegister,hook,flag)
#endif
#endif /* __KERNEL__ */
#endif /* __ASM_INCLUDED__ */
#endif /* _KERNEL_PROC_H_ */





