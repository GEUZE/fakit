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
** +Date: 2006/03/09 14:26:10 +                                     **
** Connector API                                                    **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_CONNECTOR_H_
#define _KERNEL_CONNECTOR_H_ 1

#ifndef __ASM_INCLUDED__
//SCFUNC0(DLLEXP void,sc_connectorUnregister);
SCFUNC1(DLLEXP sc_pid_t,sc_connectorRegister,int);
#if !defined(__KERNEL__)
//#define sc_connectorUnregister() _FUNC0(sc_connectorUnregister)
#define sc_connectorRegister(a)  _FUNC1(sc_connectorRegister,a)
#endif /* !__KERNEL__ */

#endif /* __ASM_INCLUDED__ */
#endif /* _KERNEL_CONNECTOR_H_ */
