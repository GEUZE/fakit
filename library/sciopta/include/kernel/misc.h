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
** +Revision: 2.19.12.1 +                                                **
** +Date: 2009/10/04 09:53:43 +                                     **
** Miscellaneous functions                                          **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_MISC_H_
#define _KERNEL_MISC_H_
#ifndef __ASM_INCLUDED__

SCFUNCn(void,sc_sysError,sc_errcode_t,sc_extra_t );

SCFUNCn(__u16,sc_miscCrcContd,const __u8 *,unsigned int,__u16);
SCFUNCn(__u16,sc_miscCrc,const __u8 *,unsigned int);
SCFUNCn(__u16,sc_miscCrcString,const char *);
SCFUNCn(void,sc_miscError,sc_errcode_t,sc_extra_t);
SCFUNCn(void,sc_miscErrnoSet,sc_errcode_t);
SCFUNC0(sc_errcode_t,sc_miscErrnoGet);
SCFUNC0(int,sc_miscKerneldRegister);
SCFUNC0(int,sc_miscState);
#if SC_ERR_HOOK == 1
typedef int (sc_errHook_t)(sc_errcode_t err,
			   sc_extra_t extra,
			   int user,
			   const sc_pcb_t *pcb);
SCFUNCn(sc_errHook_t *,sc_miscErrorHookRegister,sc_errHook_t *);
#endif /* SC_ERR_HOOK */

#if !defined(__KERNEL__)
#define sc_miscState()                    _FUNC0(sc_miscState)
#define sc_miscCrcContd(dataptr,size,startvalue) \
                               _FUNCn(sc_miscCrcContd,dataptr,size,startvalue)
#define sc_miscCrc(dataptr,size)         _FUNCn(sc_miscCrc,dataptr,size)
#define sc_miscCrcString(strptr)         _FUNCn(sc_miscCrcString,strptr)
#define sc_miscError(errcode,extra)      _FUNCn(sc_miscError,errcode,extra)
#define sc_miscErrnoSet(errcode)         _FUNCn(sc_miscErrnoSet,errcode)
#define sc_miscErrnoGet()                _FUNC0(sc_miscErrnoGet)
#if SC_ERR_HOOK == 1
#define sc_miscErrorHookRegister(hook) _FUNCn(sc_miscErrorHookRegister,hook)
#endif /* SC_ERR_HOOK */

#endif /* !__KERNEL__ */

#endif /* __ASM_INCLUDED__ */
#endif /* _KERNEL_MISC_H_ */

/*
****************************************************************************
** $Log: misc.h,v $
** Revision 2.19.12.1  2009/10/04 09:53:43  bastian
** added more function and fixes for das bug
**
** Revision 2.19  2008/01/16 13:02:02  bastian
** merge from 1.9.1.12
**
** Revision 2.18.2.3  2007/07/10 13:06:03  bastian
** fixed prototype (MISRA complained)
**
** Revision 2.18.2.2  2007/07/04 14:22:43  bastian
** removed // comments
**
** Revision 2.18.2.1  2007/01/03 08:49:00  bastian
** new syscall
**
** Revision 2.18  2006/10/11 14:01:40  bastian
** merger
**
** Revision 2.17.2.1  2006/06/29 12:27:36  bastian
** cleartext parameters
**
** Revision 2.17  2006/01/31 07:44:22  bastian
** fixes for syscall support
**
** Revision 2.16  2005/10/27 10:07:38  bastian
** fixed hook parameter
**
** Revision 2.15  2005/09/20 09:43:15  bastian
** added const
**
** Revision 2.14  2003/06/10 13:22:53  bastian
** added err-hook register
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
