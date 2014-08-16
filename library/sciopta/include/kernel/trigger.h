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
** ID: S01301BS2                                                    **
** +Revision: 2.11.14.1 +                                                 **
** +Date: 2009/10/04 09:53:43 +                                     **
** Process variables                                                **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_TRIGGER_H_
#define _KERNEL_TRIGGER_H_ 1

#define SC_TRIGGER_TMO     -1
#define SC_TRIGGER_NO_WAIT 0
#define SC_TRIGGER_TRIGGERED 1

#ifndef __ASM_INCLUDED__
SCFUNCn(sc_triggerval_t,sc_triggerValueGet,sc_pid_t);
SCFUNCn(void,sc_triggerValueSet,sc_triggerval_t);
SCFUNCn(void,sc_trigger,sc_pid_t);
SCFUNCn(int,sc_triggerWait,sc_triggerval_t,sc_ticks_t);
#if !defined(__KERNEL__)
#define sc_triggerValueGet(pid)       _FUNCn(sc_triggerValueGet,pid)
#define sc_triggerValueSet(value)     _FUNCn(sc_triggerValueSet,value)
#define sc_trigger(pid)               _FUNCn(sc_trigger,pid)
#define sc_triggerWait(decrement,tmo) _FUNCn(sc_triggerWait,decrement,tmo)
#endif /* !defined(__KERNEL__) */
#endif /* __ASM_INCLUDED__ */
#endif






