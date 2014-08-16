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
** +Revision: 1.1 +                                                **
** +Date: 2005/09/13 05:32:02 +                                     **
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
#define SC_SET_PROCSWAP_HOOK (3)
#endif
#define SC_PDB_STATE_RUN 1
#define SC_PDB_STATE_STP 0

#define SC_PROC_TYPE_EXT_FPU 1
/* EOCNV */

#ifndef __ASM_INCLUDED__

#if SC_PROC_HOOK == 1
typedef void (*sc_procHook_t)(sc_pcb_t *,sc_pcb_t *);
SCFUNC2(sc_procHook_t,sc_procHookRegister,int,sc_procHook_t);
#endif  

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
SCFUNC1(int,sc_procPathCheck,const char *);
/*
** Get the process name of the current running process
*/
externC const char * sc_sysProcCurrentName(void);
/*
** Get the process id of a process.
*/
SCFUNC2(sc_pid_t,sc_procIdGet,const char *,sc_ticks_t);
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
** Get the process's PCB (real address !!).
*/
externC sc_pcb_t * sc_sysProcPcbGet(sc_pid_t pid);
#ifndef __KERNEL__
#define sc_procStart(a)      _FUNC1(sc_procStart,a)
#define sc_procStop(a)       _FUNC1(sc_procStop,a)
//->#define sc_procNameGet(a)    _FUNC1(sc_procNameGet,a)
#define sc_procIdGet(a,b)    _FUNC2(sc_procIdGet,a,b)
#define sc_procPrioGet(a)    _FUNC1(sc_procPrioGet,a)
//->#define sc_procPrioSet(a)    _FUNC1(sc_procPrioSet,a)
#define sc_procSliceGet(a)   _FUNC1(sc_procSliceGet,a)
//->#define sc_procSliceSet(a,b)   _FUNC2(sc_procSliceSet,a,b)
//->#define sc_procYield()       _FUNC0(sc_procYield)
#if SC_PROC_HOOK == 1
#  define sc_procHookRegister(a,b) _FUNC2(sc_procHookRegister,a,b)
#endif
#endif /* __KERNEL__ */
#endif /* __ASM_INCLUDED__ */
#endif /* _KERNEL_PROC_H_ */





