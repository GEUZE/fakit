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
** ID: S05223BS2                                                    **
** +Revision: 1.1 +                                                **
** +Date: 2005/08/11 13:55:38 +                                     **
** Miscellaneous functions                                          **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_MISC_H_
#define _KERNEL_MISC_H_
#ifndef __ASM_INCLUDED__

SCFUNC2(void,sc_sysError,sc_errcode_t,sc_extra_t );
SCFUNC2(void,sc_miscError,sc_errcode_t,sc_extra_t);
//->#if SC_ERR_HOOK == 1
//->typedef int (*sc_errHook_t)(sc_errcode_t,sc_extra_t,int user,sc_pcb_t *);
//->SCFUNC1(sc_errHook_t,sc_miscErrorHookRegister,sc_errHook_t);
//->#endif /* SC_ERR_HOOK */

#if !defined(__KERNEL__)
#define sc_miscError(a,b)      _FUNC2(sc_miscError,a,b)
//->#if SC_ERR_HOOK == 1
//->#define sc_miscErrorHookRegister(a) _FUNC1(sc_miscErrorHookRegister,a)
//->#endif /* SC_ERR_HOOK */

#endif /* !__KERNEL__ */

#endif /* __ASM_INCLUDED__ */
#endif /* _KERNEL_MISC_H_ */

