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
** ID: S03076BS12                                                   **
** +Revision: 2.39 +                                                **
** +Date: 2008/11/11 10:01:40 +                                     **
** Error and function code texts                                    **
**********************************************************************
EOC*/

#ifndef _OSSYS_ERRTXT_H_
#define _OSSYS_ERRTXT_H_
static const char * const err_txt[] = {
  "Unknown error",
  "Illegal pool-ID",
  "No more pool",
  "Illegal pool-size",
  "Pool still in use",
  "Illegal number of buffer-sizes",
  "Illegal buffersizes",
  "Illegal buffersize",
  "Message outside pool",
  "Message header corrupted",
  "NIL pointer",
  "Message enlarged",
  "Not owner",
  "Out of Memory",
  "Illegale interrupt vector",
  "Illegale timeslice",
  "No kerneld started",
  "Message endmark corrupted",
  "Prev. message's endmark corrupted",
  "Illegal default pool-ID",
  "Illegal syscall while scheduler locked",
  "Illegal Processtype",
  "Illegal Interrupt",
  "Illegal Exception",
  "Illegal syscall",
  "Illegal interrupt nesting",
  "1a",
  "1b",
  "1c",
  "1d",
  "1e",
  "Unlock without lock",
  "Illegal pid",
  "No more tasks",
  "Modulesize to small",
  "Starting of a not stopped process",
  "Illegal process",
  "Illegal name",
  "26",
  "Illegal module-id",
  "Illegal priority",
  "Illegal stacksize",
  "No more modules available",
  "Illegal parameter",
  "2c",
  "Not a prio-process",
  "Stack overflow",
  "Stack underflow",
  "Illegal Value",
  "Already defined",
  "No more connectors",
  ""
};

static const char * const func_txt[] = {
/*00*/ "<unknown function>",
/*01*/ "sc_msgAlloc",
/*02*/ "sc_msgFree",
/*03*/ "sc_msgAddrGet",
/*04*/ "sc_msgSndGet",
/*05*/ "sc_msgSizeGet",
/*06*/ "sc_msgSizeSet",
/*07*/ "sc_msgOwnerGet",
/*08*/ "sc_msgTx",
/*09*/ "sc_msgTxAlias",
/*0A*/ "sc_msgRx",
/*0B*/ "sc_msgPoolIdGet",
/*0C*/ "sc_msgAcquire",
/*0D*/ "sc_msgAllocClr",
/*0E*/ "sc_msgHookRegister",
/*0F*/ "f",
/*10*/ "sc_poolCreate",
/*11*/ "sc_poolReset",
/*12*/ "sc_poolKill",
/*13*/ "sc_poolInfo",
/*14*/ "sc_poolDefault",
/*15*/ "sc_poolIdGet",
/*16*/ "sc_sysPoolKill",
/*17*/ "sc_poolHookRegister",
/*18*/ "sc_miscErrorHookRegister",
/*19*/ "sc_miscKerneldRegister",
/*1A*/ "sc_miscCrcContd",
/*1B*/ "sc_miscCrc",
/*1C*/ "sc_miscErrnoSet",
/*1D*/ "sc_miscErrnoGet",

/*1E*/ "sc_procWakeupEnable",
/*1F*/ "sc_procWakeupDisable",
/*20*/ "sc_procPrioGet",
/*21*/ "sc_procPrioSet",
/*22*/ "sc_procSliceGet",
/*23*/ "sc_procSliceSet",
/*24*/ "sc_procIdGet",
/*25*/ "sc_procPpidGet",
/*26*/ "sc_procNameGet",
/*27*/ "sc_procStart",
/*28*/ "sc_procStop",
/*29*/ "sc_procVarInit",
/*2A*/ "sc_procSchedUnlock",
/*2B*/ "sc_procPrioCreateStatic",
/*2C*/ "sc_procIntCreateStatic",
/*2D*/ "sc_procTimCreateStatic",
/*2E*/ "sc_procUsrIntCreateStatic",
/*2F*/ "sc_procPrioCreate",
/*30*/ "sc_procIntCreate",
/*31*/ "sc_procTimCreate",
/*32*/ "sc_procUsrIntCreate",
/*33*/ "sc_procKill",
/*34*/ "sc_procYield",
/*35*/ "sc_procObserve",
/*36*/ "sc_sysProcCreate",
/*37*/ "sc_procSchedLock",
/*38*/ "sc_procVarGet",
/*39*/ "sc_procVarSet",
/*3A*/ "sc_procVarDel",
/*3B*/ "sc_procVarRm",
/*3C*/ "sc_procUnobserve",
/*3D*/ "sc_procPathGet",
/*3E*/ "sc_procPathCheck",
/*3F*/ "sc_procHookRegister",

/*40*/ "sc_moduleCreate",
/*41*/ "sc_moduleKill",
/*42*/ "sc_moduleNameGet",
/*43*/ "sc_moduleIdGet",
/*44*/ "sc_moduleInfo",
/*45*/ "sc_modulePrioSet",
/*46*/ "sc_modulePrioGet",
/*47*/ "sc_moduleFriendAdd",
/*48*/ "sc_moduleFriendRm",
/*49*/ "sc_moduleFriendGet",
/*4A*/ "sc_moduleFriendNone",
/*4B*/ "sc_moduleFriendAll",
/*4C*/ "4c",
/*4D*/ "4d",
/*4E*/ "sc_procDaemonUnregister",
/*4F*/ "sc_procDaemonRegister",
/*50*/ "sc_triggerValueSet",
/*51*/ "sc_triggerValueGet",
/*52*/ "sc_trigger",
/*53*/ "sc_triggerWait",
/*54*/ "sc_sysError",
/*55*/ "sc_miscError",
/*56*/ "56",
/*57*/ "sc_tick",
/*58*/ "sc_tmoAdd",
/*59*/ "sc_tmo",
/*5A*/ "sc_sleep",
/*5B*/ "sc_tmoRm",
/*5C*/ "sc_tickGet",
/*5D*/ "sc_tickLength",
/*5E*/ "sc_tickMs2Tick",
/*5F*/ "sc_tickTick2Ms",
/*60*/ "sc_connectorRegister",
/*61*/ "sc_connectorUnregister",
/*62*/ "<dispatcher>",
/*00*/ ""
};
#endif




