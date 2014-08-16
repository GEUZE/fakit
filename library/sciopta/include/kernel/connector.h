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
** ID: S01328BS1                                                    **
** +Revision: 2.6.14.1 +                                                 **
** +Date: 2009/10/04 09:53:43 +                                     **
** Connector API                                                    **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_CONNECTOR_H_
#define _KERNEL_CONNECTOR_H_ 1

#ifndef __ASM_INCLUDED__
SCFUNC0(void,sc_connectorUnregister);
SCFUNCn(sc_pid_t,sc_connectorRegister,int);
#if !defined(__KERNEL__)
#define sc_connectorUnregister() _FUNC0(sc_connectorUnregister)
#define sc_connectorRegister(defaultflag)  _FUNCn(sc_connectorRegister,defaultflag)
#endif /* !__KERNEL__ */

#endif /* __ASM_INCLUDED__ */
#endif /* _KERNEL_CONNECTOR_H_ */

