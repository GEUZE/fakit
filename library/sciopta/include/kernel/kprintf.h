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
** ID: S01103BS12                                                   **
** +Revision: 2.5 +                                                **
** +Date: 2008/01/16 13:02:02 +                                     **
** Kernel printf (found in util/logd)                               **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_KPRINTF_H_
#define _KERNEL_KPRINTF_H_
#ifndef __ASM_INCLUDED__

#ifdef __IAR_SYSTEMS_ICC__
/* disable MISRA C Rule 69*/
/* printf - like implementation need variable arguments */
#pragma diag_suppress=Pm064
#endif

externC void kprintf(int loglevel,const char *fmt,...);

#ifdef __IAR_SYSTEMS_ICC__
/* enable MISRA C Rule 69*/
#pragma diag_default=Pm064
#endif

#endif /* __ASM_INCLUDED__ */
#endif

