
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
** ID: S06059BS1                                                    **
** +Revision: 1.3 +                                                 **
** +Date: 2006/05/12 10:26:22 +                                     **
** Global system data                                               **
**********************************************************************
EOC*/
#ifndef _KERNEL_MISC_H_
#define _KERNEL_MISC_H_ 2

SCFUNC2(DLLEXP void,sc_sysError,sc_errcode_t,sc_extra_t);
SCFUNC2(DLLEXP void,sc_miscError,sc_errcode_t,sc_extra_t);

typedef int (sc_errHook_t)(sc_errcode_t,sc_extra_t,int user,const sc_pcb_t *);
SCFUNC1(DLLEXP sc_errHook_t *,sc_miscErrorHookRegister,sc_errHook_t *);

#if !defined(__KERNEL__)
//->#define sc_miscCrcContd(a,b,c) _FUNC3(sc_miscCrcContd,a,b,c)
//->#define sc_miscCrc(a,b)        _FUNC2(sc_miscCrc,a,b)
#define sc_miscError(errorcode,extra)      _FUNC2(sc_miscError,errorcode,extra)
//->#define sc_miscErrnoSet(a)     _FUNC1(sc_miscErrnoSet,a)
//->#define sc_miscErrnoGet()      _FUNC0(sc_miscErrnoGet)
#define sc_miscErrorHookRegister(hook) _FUNC1(sc_miscErrorHookRegister,hook)

#endif /* !__KERNEL__ */

#endif /* _KERNEL_MISC_H_ */
