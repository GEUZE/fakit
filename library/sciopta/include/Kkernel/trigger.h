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
** +Revision: 1.2 +                                                 **
** +Date: 2003/07/23 07:13:23 +                                     **
** Process variables                                                **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_TRIGGER_H_
#define _KERNEL_TRIGGER_H_ 1
#ifndef __ASM_INCLUDED__
SCFUNC1(sc_triggerval_t,sc_triggerValueGet,sc_pid_t);
SCFUNC1(void,sc_triggerValueSet,sc_triggerval_t);
SCFUNC1(void,sc_trigger,sc_pid_t);
SCFUNC2(int,sc_triggerWait,sc_triggerval_t,sc_ticks_t);
#if !defined(__KERNEL__)
#define sc_triggerValueGet(a) _FUNC1(sc_triggerValueGet,a)
#define sc_triggerValueSet(a) _FUNC1(sc_triggerValueSet,a)
#define sc_trigger(a)         _FUNC1(sc_trigger,a)
#define sc_triggerWait(a,b)   _FUNC2(sc_triggerWait,a,b)
#endif /* !defined(__KERNEL__) */
#endif /* __ASM_INCLUDED__ */
#endif






