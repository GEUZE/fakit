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
** ID: S06055BS6                                                    **
** +Revision: 1.7 +                                                 **
** +Date: 2006/05/12 10:26:22 +                                     **
** Process definitions and API                                      **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_PROC_H_
#define _KERNEL_PROC_H_ 2

#if SC_PROC_HOOK == 1
#define SC_SET_PROCCREATE_HOOK (1)
#define SC_SET_PROCKILL_HOOK (2)
#define SC_SET_PROCSWAP_HOOK (3)
#endif

#define SC_PROCKILL_KILL          0x4000

#define SC_PDB_STATE_RUN 1
#define SC_PDB_STATE_STP 0

/*
** Create a prioritzed process.
*/
typedef void (void_f)(void);

SCFUNC7(DLLEXP sc_pid_t,sc_procPrioCreate,
	const char *,     /* name */
	void_f,           /* function */
	sc_bufsize_t,     /* stack size */
	sc_ticks_t,       /* slice */
	sc_prio_t,        /* prio */
	int,              /* state */
	sc_poolid_t);     /* pool (for pcb/stack) */


/*
** Get the process id of a process.
*/
SCFUNC2(DLLEXP sc_pid_t,sc_procIdGet,const char *,sc_ticks_t);

/*
** Kill a process.
*/
SCFUNC2(DLLEXP void,sc_procKill,sc_pid_t,sc_flags_t);

/*
** Non-public function (use with care !)
*/

/*
** Get the PCB of a process.
*/
SCFUNC1(DLLEXP sc_pcb_t *,sc_sysProcPcbGet,sc_pid_t);

/*
** Get the name of the current process.
*/
SCFUNC0(DLLEXP __u8 *,sc_sysProcCurrentName);

/*
** Get the number of swaps since API start
*/
SCFUNC0(DLLEXP __u32,sc_sysProcSwapsGet);

/*
** Change the priority of the calling process.
*/
SCFUNC1(DLLEXP void,sc_procPrioSet,sc_prio_t);

/*
** Request a message if process pid dies.
*/
SCFUNC2(DLLEXP void,sc_procObserve,sc_msgptr_t,sc_pid_t);
/*
** Remove all observe msg from process pid
*/
SCFUNC2(DLLEXP void,sc_procUnobserve,sc_pid_t,sc_pid_t);

#ifndef __KERNEL__
#define sc_procPrioCreate(name,function,stacksize,slice,prio,state,poolid)\
	  _FUNC7(sc_procPrioCreate,name,function,stacksize,slice,prio,state,poolid)
#define sc_procKill(pid,flags)	    _FUNC2(sc_procKill,pid,flags)
#define sc_procIdGet(name,tmo)	    _FUNC2(sc_procIdGet,name,tmo)
#define sc_procObserve(msgptr,pid)  _FUNC2(sc_procObserve,msgptr,pid)
#define sc_procUnobserve(observer_pid,target_pid) _FUNC2(sc_procUnobserve,observer_pid,target_pid)
#endif /* __KERNEL__ */

#endif /* _KERNEL_PROC_H_ */
