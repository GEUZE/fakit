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
** ID: S01103BS10                                                   **
** +Revision: 1.5 +                                                **
** +Date: 2009/01/23 14:15:02 +                                     **
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
SCFUNC1(void,sc_miscErrnoSet,sc_errcode_t err);
SCFUNC0(sc_errcode_t,sc_miscErrnoGet);

#if SC_ERR_HOOK == 1
typedef int (sc_errHook_t)(sc_errcode_t,sc_extra_t,int user,const sc_pcb_t *);
SCFUNC1(sc_errHook_t *,sc_miscErrorHookRegister,sc_errHook_t *);
#endif /* SC_ERR_HOOK */

/* Not part of the kernel: See libsc/chksum.c */
__u16 sc_miscCrcContd(const __u8 FARPTR data,unsigned int len,__u16 hash);
__u16 sc_miscCrc(const __u8 FARPTR data,unsigned int len);
__u16 sc_miscCrcString(const char FARPTR data);

#if !defined(__KERNEL__)
#define sc_miscError(a,b)      _FUNC2(sc_miscError,a,b)
#define sc_miscErrnoSet(a)     _FUNC1(sc_miscErrnoSet,a)
#define sc_miscErrnoGet()      _FUNC0(sc_miscErrnoGet)
#if SC_ERR_HOOK == 1
#define sc_miscErrorHookRegister(a) _FUNC1(sc_miscErrorHookRegister,a)
#endif /* SC_ERR_HOOK */

#endif /* !__KERNEL__ */

#endif /*__ASM_INCLUDED__*/
#endif

/*
****************************************************************************
** $Log: misc.h,v $
** Revision 1.5  2009/01/23 14:15:02  bastian
** merge from 1.9.4.8
**
** Revision 1.4.8.1  2001/05/17 07:26:36  bastian
** fix: No xsc_ for the crc functions
**
** Revision 1.4  2008/01/16 13:02:02  bastian
** merge from 1.9.1.12
**
** Revision 1.3.6.2  2007/09/28 08:17:48  bastian
** fixes
**
** Revision 1.3.6.1  2007/09/14 07:40:13  bastian
** fixes for nc30
**
** Revision 1.3  2005/09/20 09:43:15  bastian
** added const
**
** Revision 1.2  2004/01/29 09:30:18  bastian
** added miscErrorHookRegister
**
** Revision 1.1  2003/05/19 12:06:05  bastian
** 1st
**
** Revision 2.13  2003/03/26 13:59:46  bastian
** added system-call/trap interface
** changed 2nd parameter of miscError
**
** Revision 2.12  2003/03/17 09:00:03  bastian
** added header
**
** Revision 2.11  2003/03/17 07:52:17  bastian
** fixed const char
**
** Revision 2.10  2002/12/17 10:52:57  bastian
** refixed order of parameters for errHook
**
** Revision 2.9  2002/12/17 10:36:26  bastian
** added user-flag to errorHook parameters
**
** Revision 2.8  2002/12/09 09:31:25  bastian
** renamed msgPool functions
** fixed typos
** added pool-name for sciopta.con
**
** Revision 2.7  2002/12/04 08:29:54  bastian
** started release-preparing
**
** Revision 2.6  2002/11/29 15:44:06  bastian
** WE
**
** Revision 2.5  2002/10/29 14:42:51  bastian
** Fixed errnoGet macro
**
** Revision 2.4  2002/07/24 10:39:17  bastian
** Fixed ccp-bug
**
** Revision 2.3  2002/07/24 10:26:14  bastian
** added new function
**
** Revision 2.2  2002/03/21 14:46:41  bastian
** Added debugging support for nearly all functions.
** Added error-checking if swap-out fails.
** Fixed friend-concept.
** Misc. fixes.
**
** Revision 2.1  2001/12/21 09:18:23  bastian
** Renamed sc_error into sc_sysError.
** Added error-classifier, to enable error to continue if possible.
**
** Revision 2.0.0.1  2001/10/31 00:52:42  bastian
** moved over to cvs
**
** 
**    Rev 1.7   07 Nov 2001 08:26:44   skywalker
** intermediate checkin
** 
**    Rev 1.6   16 Oct 2001 15:45:58   skywalker
** intermediate check-in
** 
**    Rev 1.5   25 Sep 2001 15:39:08   skywalker
** raqe cond. fixes
** 
**    Rev 1.4   10 Sep 2001 10:34:02   skywalker
** ?
** 
**    Rev 1.3   Aug 27 2001 14:37:08   bastian
** moved tick
** 
**    Rev 1.2   Aug 20 2001 10:37:46   bastian
** Changed LOCK/UNLOCK macros
** 
**    Rev 1.1   Jul 16 2001 13:43:04   bastian
** fixed (?) API
**
****************************************************************************
*/
