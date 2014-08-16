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
** ID: S06055BS3                                                    **
** +Revision: 1.2 +                                                 **
** +Date: 2006/03/02 15:56:16 +                                     **
** Synchronous timeout                                              **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_SLEEP_H_
#define _KERNEL_SLEEP_H_

/*
** Suspends a process for a given time
*/
SCFUNC1(DLLEXP void,sc_sleep,sc_ticks_t);

#if !defined(__KERNEL__)
#define sc_sleep(tmo) _FUNC1(sc_sleep,tmo)
#endif /* !defined(__KERNEL__) */

#endif
