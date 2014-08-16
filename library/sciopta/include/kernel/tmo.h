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
** +Revision: 1.10.12.1 +                                                 **
** +Date: 2009/10/04 09:53:43 +                                     **
** Asynchronous timeout                                             **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_TMO_H_
#define _KERNEL_TMO_H_

#define MSG_TMO_MASK  0x00ffff00
#define MSG_TMO_MAGIC 0x00AFFE00

#ifndef __ASM_INCLUDED__

typedef char * sc_tmoid_t;
SCFUNCn(sc_tmoid_t,sc_tmoAdd,sc_ticks_t,sc_msgptr_t);
SCFUNCn(sc_msg_t,sc_tmoRm,sc_tmoid_t *);

#if !defined(__KERNEL__)
#define sc_tmoAdd(tmo,msgptr) _FUNCn(sc_tmoAdd,tmo,msgptr)
#define sc_tmoRm(tmoid) _FUNCn(sc_tmoRm,tmoid)
#endif /* !defined(__KERNEL__) */
#endif /*__ASM_INCLUDED__*/
#endif /* _KERNEL_TMO_H_ */
/*
** $Log: tmo.h,v $
** Revision 1.10.12.1  2009/10/04 09:53:43  bastian
** added more function and fixes for das bug
**
** Revision 1.10  2008/01/16 13:02:02  bastian
** merge from 1.9.1.12
**
** Revision 1.9.2.1  2007/07/04 14:22:43  bastian
** removed // comments
**
** Revision 1.9  2006/10/11 14:01:40  bastian
** merger
**
** Revision 1.8.10.1  2006/06/29 12:27:36  bastian
** cleartext parameters
**
** Revision 1.8  2004/10/04 12:11:37  bastian
** introduced sc_msg_t and sc_msgptr_t
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
