/* -*-c++-*-
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
**              (c) 2009 SCIOPTA Systems AG/ Schweiz                **
**                                                                  **
**********************************************************************
** ID: S03080BS2                                                    **
** +Revision: 1.8.14.1 +                                                 **
** +Date: 2009/09/16 08:00:05 +                                     ** 
** Systemcall numbers                                               **
**********************************************************************
*/
#ifndef _MACHINE_SYSCALL_H_
#define _MACHINE_SYSCALL_H_

#include <sciopta.h>

#define S(a) ((a)>>24)

#define	__SC_sc_msgAlloc		S(SC_MSGALLOC)
#define	__SC_sc_msgFree			S(SC_MSGFREE)
#define	__SC_sc_msgAddrGet		S(SC_MSGADDRGET)
#define	__SC_sc_msgSndGet		S(SC_MSGSNDGET)
#define	__SC_sc_msgSizeGet		S(SC_MSGSIZEGET)
#define	__SC_sc_msgSizeSet		S(SC_MSGSIZESET)
#define	__SC_sc_msgOwnerGet		S(SC_MSGOWNERGET)
#define	__SC_sc_msgTx			S(SC_MSGTX)
#define	__SC_sc_msgTxAlias		S(SC_MSGTXALIAS)
#define	__SC_sc_msgRx			S(SC_MSGRX)
#define	__SC_sc_msgPoolIdGet		S(SC_MSGPOOLIDGET)
#define	__SC_sc_msgAcquire      	S(SC_MSGACQUIRE)
#define	__SC_sc_msgAllocClr		S(SC_MSGALLOCCLR)
#define __SC_sc_msgHookRegister         S(SC_MSGHOOKREGISTER)

#define	__SC_sc_poolCreate      	S(SC_POOLCREATE)
#define	__SC_sc_poolReset       	S(SC_POOLRESET)
#define	__SC_sc_poolKill        	S(SC_POOLKILL)
#define	__SC_sc_poolInfo        	S(SC_POOLINFO)
#define	__SC_sc_poolDefault     	S(SC_POOLDEFAULT)
#define	__SC_sc_poolIdGet       	S(SC_POOLIDGET)
#define	__SC_sc_sysPoolKill		S(SC_SYSPOOLKILL)
#define __SC_sc_poolHookRegister        S(SC_POOLHOOKREGISTER)

#define __SC_sc_miscErrorHookRegister   S(SC_MISCERRORHOOKREGISTER)
#define __SC_sc_miscKerneldRegister     S(SC_MISCKERNELDREGISTER)
#define __SC_sc_miscCrcContd            S(SC_MISCCRCCONTD)
#define __SC_sc_miscCrc                 S(SC_MISCCRC)
#define __SC_sc_miscErrnoSet            S(SC_MISCERRNOSET)
#define __SC_sc_miscErrnoGet            S(SC_MISCERRNOGET)

#define __SC_sc_procWakeupEnable        S(SC_PROCWAKEUPENABLE)
#define __SC_sc_procWakeupDisable       S(SC_PROCWAKEUPDISABLE)
#define	__SC_sc_procPrioGet		S(SC_PROCPRIOGET)
#define	__SC_sc_procPrioSet		S(SC_PROCPRIOSET)
#define	__SC_sc_procSliceGet		S(SC_PROCSLICEGET)
#define	__SC_sc_procSliceSet		S(SC_PROCSLICESET)
#define	__SC_sc_procIdGet		S(SC_PROCIDGET)
#define	__SC_sc_procPpidGet		S(SC_PROCPPIDGET)
#define	__SC_sc_procNameGet		S(SC_PROCNAMEGET)
#define	__SC_sc_procStart		S(SC_PROCSTART)
#define	__SC_sc_procStop		S(SC_PROCSTOP)
#define	__SC_sc_procSchedUnlock		S(SC_PROCSCHEDUNLOCK)
#define	__SC_sc_procSchedLock		S(SC_PROCSCHEDLOCK)
#define	__SC_sc_procYield		S(SC_PROCYIELD)
#define	__SC_sc_procVarInit	        S(SC_PROCVARINIT)
#define __SC_sc_procVarGet		S(SC_PROCVARGET)
#define __SC_sc_procVarSet		S(SC_PROCVARSET)
#define __SC_sc_procVarDel		S(SC_PROCVARDEL)
#define __SC_sc_procVarRm		S(SC_PROCVARRM)

#define	__SC_sc_procKill		S(SC_PROCKILL)
#define	__SC_sc_procObserve		S(SC_PROCOBSERVE)
#define	__SC_sc_procUnobserve		S(SC_PROCUNOBSERVE)

#define __SC_sc_procPathGet             S(SC_PROCPATHGET)
#define __SC_sc_procPathCheck           S(SC_PROCPATHCHECK)
#define __SC_sc_procHookRegister        S(SC_PROCHOOKREGISTER)
#define __SC_sc_procDaemonUnregister    S(SC_PROCDAEMONUNREGISTER)
#define __SC_sc_procDaemonRegister      S(SC_PROCDAEMONREGISTER)

#define	__SC_sc_procPrioCreate		S(SC_PROCPRIOCREATE)
#define	__SC_sc_procIntCreate		S(SC_PROCINTCREATE)
#define	__SC_sc_procTimCreate	        S(SC_PROCTIMCREATE)
#define	__SC_sc_procUsrIntCreate	S(SC_PROCUSRINTCREATE)

#define	__SC_sc_moduleKill		S(SC_MODULEKILL)
#define	__SC_sc_moduleNameGet		S(SC_MODULENAMEGET)
#define	__SC_sc_moduleIdGet		S(SC_MODULEIDGET)
#define	__SC_sc_moduleInfo		S(SC_MODULEINFO)
#define	__SC_sc_modulePrioGet		S(SC_MODULEPRIOGET)
#define	__SC_sc_moduleCreate		S(SC_MODULECREATE)
#define __SC_sc_moduleStart             S(SC_MODULESTART)
#define __SC_sc_moduleStop              S(SC_MODULESTOP)
#define	__SC_sc_moduleFriendAdd		S(SC_MODULEFRIENDADD)
#define	__SC_sc_moduleFriendRm		S(SC_MODULEFRIENDRM)
#define	__SC_sc_moduleFriendGet		S(SC_MODULEFRIENDGET)
#define	__SC_sc_moduleFriendNone	S(SC_MODULEFRIENDNONE)
#define	__SC_sc_moduleFriendAll		S(SC_MODULEFRIENDALL)

#define	__SC_sc_triggerValueSet		S(SC_TRIGGERVALUESET)
#define	__SC_sc_triggerValueGet		S(SC_TRIGGERVALUEGET)
#define	__SC_sc_trigger			S(SC_TRIGGER)
#define	__SC_sc_triggerWait		S(SC_TRIGGERWAIT)
#define __SC_sc_sysError		S(SC_SYSERROR)
#define __SC_sc_miscError		S(SC_MISCERROR)

#define __SC_sc_tick                    S(SC_TICK)
#define __SC_sc_tickActivationGet       S(SC_TICKACTIVATIONGET)
#define __SC_sc_tick			S(SC_TICK)
#define __SC_sc_tickGet			S(SC_TICKGET)
#define __SC_sc_tickLength		S(SC_TICKLENGTH)
#define __SC_sc_tickMs2Tick		S(SC_TICKMS2TICK)
#define __SC_sc_tickTick2Ms		S(SC_TICKTICK2MS)
#define	__SC_sc_sleep			S(SC_SLEEP)

#define	__SC_sc_tmoAdd			S(SC_TMOADD)
#define	__SC_sc_tmoRm			S(SC_TMORM)

#define	__SC_sc_connectorRegister       S(SC_CONNECTORREGISTER)
#define	__SC_sc_connectorUnregister     S(SC_CONNECTORUNREGISTER)


#define	__SC_sc_sysModulePrioSet	0x45
#define	__SC_sc_sysModulePrioGet	0x46

// PPC only
#define __SC_sc_sysModuleSDASet         0x63
#endif
