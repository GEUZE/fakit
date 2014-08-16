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
** ID: S02207BS1                                                    **
** +Revision: 1.4 +                                                 **
** +Date: 2004/10/07 07:37:47 +                                     **
** Asynchronous timeout                                             **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_TMO_H_
#define _KERNEL_TMO_H_

#ifndef __ASM_INCLUDED__

typedef char NEARPTR sc_tmoid_t;
SCFUNC2(sc_tmoid_t,sc_tmoAdd,sc_ticks_t,sc_msgptr_t);
SCFUNC1(sc_msg_t,sc_tmoRm,sc_tmoid_t NEARPTR);

#if !defined(__KERNEL__)
#define sc_tmoAdd(a,b) _FUNC2(sc_tmoAdd,a,b)
#define sc_tmoRm(a) _FUNC1(sc_tmoRm,a)
#endif /* !defined(__KERNEL__) */
#endif /*__ASM_INCLUDED__*/
#endif /* _KERNEL_TMO_H_ */
/*
** $Log: tmo.h,v $
** Revision 1.4  2004/10/07 07:37:47  bastian
** fixed NEAR * to NEARPTR
**
** Revision 1.3  2004/10/04 12:11:37  bastian
** introduced sc_msg_t and sc_msgptr_t
**
** Revision 1.2  2004/09/30 08:33:56  bastian
** fixes for m16c
**
** Revision 1.1  2003/05/19 12:06:05  bastian
** 1st
**
** Revision 1.7  2003/04/15 11:05:42  bastian
** Fixed async-TMO: Must leave msg in the process alloc-list to allow
** removing a TMO if a process gets killed.
**
** Revision 1.6  2003/03/26 13:59:46  bastian
** added system-call/trap interface
** changed 2nd parameter of miscError
**
** Revision 1.5  2003/03/17 09:00:04  bastian
** added header
**
** Revision 1.4  2003/03/17 07:52:17  bastian
** fixed const char
**
** Revision 1.3  2002/12/04 08:29:54  bastian
** started release-preparing
**
** Revision 1.2  2002/07/26 06:49:29  bastian
** added debug and tmoRm
**
** Revision 1.1  2002/07/25 08:35:39  bastian
** added async tmo
**
**
*/
