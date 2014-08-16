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
** ID: S01103BS32                                                   **
** +Revision: 2.8.24.1 +                                                 **
** +Date: 2009/10/04 09:53:43 +                                     **
** Synchronous timeout                                              **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_SLEEP_H_
#define _KERNEL_SLEEP_H_
#ifndef __ASM_INCLUDED__

/*
** Suspends a process for a given time
*/
SCFUNCn(void,sc_sleep,sc_ticks_t);

#if !defined(__KERNEL__)
#define sc_sleep(tmo) _FUNCn(sc_sleep,tmo)
#endif /* !defined(__KERNEL__) */

#endif /* __ASM_INCLUDED__ */
#endif
