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
** ID: S03090BS1                                                    **
** +Revision: 1.2 +                                                 **
** +Date: 2008/11/11 10:01:40 +                                     **
** Trampoline functions for system-calls                            **
**********************************************************************
EOC*/

/* Note: This file is included by a machine/<cpu>/syscall.S */
	
 syscall(sc_msgAlloc)
 syscall(sc_msgFree)
 syscall(sc_msgAddrGet)
 syscall(sc_msgSndGet)
 syscall(sc_msgSizeGet)
 syscall(sc_msgSizeSet)
 syscall(sc_msgOwnerGet)
 syscall(sc_msgTx)
 syscall(sc_msgTxAlias)
 syscall(sc_msgRx)
 syscall(sc_msgPoolIdGet)
 syscall(sc_msgAcquire)
 syscall(sc_msgAllocClr)
 syscall(sc_msgHookRegister)

 syscall(sc_poolCreate)
 syscall(sc_poolReset)
 syscall(sc_poolKill)
 syscall(sc_poolInfo)
 syscall(sc_poolDefault)
 syscall(sc_poolIdGet)
 syscall(sc_poolHookRegister)

 syscall(sc_miscError)
 syscall(sc_miscErrorHookRegister)
 syscall(sc_miscKerneldRegister)
 syscall(sc_miscErrnoSet)
 syscall(sc_miscErrnoGet)
 syscall(sc_miscCrc)
 syscall(sc_miscCrcContd)

 syscall(sc_procWakeupEnable)
 syscall(sc_procWakeupDisable)
 syscall(sc_procPrioGet)
 syscall(sc_procPrioSet)
 syscall(sc_procSliceGet)
 syscall(sc_procSliceSet)
 syscall(sc_procIdGet)
 syscall(sc_procPpidGet)
 syscall(sc_procNameGet)
 syscall(sc_procStart)
 syscall(sc_procStop)
 syscall(sc_procVarInit)
 syscall(sc_procSchedUnlock)

 syscall(sc_procPrioCreate)
 syscall(sc_procIntCreate)
 syscall(sc_procTimCreate)
 syscall(sc_procUsrIntCreate)
 syscall(sc_procKill)
 syscall(sc_procYield)
 syscall(sc_procObserve)

 syscall(sc_procSchedLock)
 syscall(sc_procVarGet)
 syscall(sc_procVarSet)
 syscall(sc_procVarDel)
 syscall(sc_procVarRm)

 syscall(sc_procUnobserve)
 syscall(sc_procPathGet)
 syscall(sc_procPathCheck)
 syscall(sc_procHookRegister)

 syscall(sc_moduleCreate)
 syscall(sc_moduleKill)
 syscall(sc_moduleNameGet)
 syscall(sc_moduleIdGet)
 syscall(sc_moduleInfo)
 syscall(sc_sysModulePrioSet)
 syscall(sc_sysModulePrioGet)
//#if SC_USE_FRIENDS == 1
 syscall(sc_moduleFriendAdd)
 syscall(sc_moduleFriendRm)
 syscall(sc_moduleFriendGet)
 syscall(sc_moduleFriendNone)
 syscall(sc_moduleFriendAll)
//#endif	/* SC_USE_FRIENDS == 1 */



 syscall(sc_triggerValueSet)
 syscall(sc_triggerValueGet)
 syscall(sc_trigger)
 syscall(sc_triggerWait)
 syscall(sc_tmoAdd)
 syscall(sc_sleep)
 syscall(sc_tmoRm)
 syscall(sc_tickGet)
 syscall(sc_tickLength)
 syscall(sc_tickMs2Tick)
 syscall(sc_tickTick2Ms)
//#if SC_MAX_CONNECTOR > 0
 syscall(sc_connectorRegister)
 syscall(sc_connectorUnregister)
//#endif
 syscall(sc_sysModuleSDASet)
 syscall(sc_procDaemonRegister)
 syscall(sc_procDaemonUnregister)
