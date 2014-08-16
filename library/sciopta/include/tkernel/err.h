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
** ID: S01103BS11                                                   **
** +Revision: 1.2 +                                                **
** +Date: 2005/09/05 14:37:30 +                                     **
** Error and function code definitions                              **
**********************************************************************
EOC*/

#ifndef _KERNEL_ERR_H_
#define _KERNEL_ERR_H_

#define __FUNC(a) (a<<24)
#define __ERR(a)  (a<<12) 
#define __CLASS(a) (a)

#define SC_ERR_TARGET_FATAL     0x01 /* obsolete */
#define SC_ERR_TARGET_WARNING   0x10 /* obsolete */
#define SC_ERR_PROC_WARNING     0x40 /* obsolete */

/*
** This kind of error will stop the whole target
*/
#define SC_ERR_SYSTEM_FATAL     0x01
/*
** This type of error results in killing of the module
*/
#define SC_ERR_MODULE_FATAL     0x02
/*
** This type results in killing the process
*/
#define SC_ERR_PROCESS_FATAL       0x04

#define SC_ERR_FATAL (SC_ERR_PROCESS_FATAL|SC_ERR_MODULE_FATAL|SC_ERR_SYSTEM_FATAL)

#define SC_ERR_SYSTEM_WARNING   0x10
#define SC_ERR_MODULE_WARNING   0x20
#define SC_ERR_PROCESS_WARNING  0x40

#define SC_MSGALLOC             __FUNC(0x01)
#define SC_MSGFREE              __FUNC(0x02)
#define SC_MSGADDRGET           __FUNC(0x03)
#define SC_MSGSNDGET            __FUNC(0x04)
#define SC_MSGSIZEGET           __FUNC(0x05)
#define SC_MSGSIZESET           __FUNC(0x06)
#define SC_MSGOWNERGET          __FUNC(0x07)
#define SC_MSGTX                __FUNC(0x08)
#define SC_MSGTXALIAS           __FUNC(0x09)
#define SC_MSGRX                __FUNC(0x0a)
#define SC_MSGPOOLIDGET         __FUNC(0x0B)
#define SC_MSGACQUIRE           __FUNC(0x0c)
#define SC_MSGALLOCCLR          __FUNC(0x0d)
#define SC_MSGHOOKREGISTER      __FUNC(0x0e)

#define SC_POOLCREATE           __FUNC(0x10)
#define SC_POOLRESET            __FUNC(0x11)
#define SC_POOLKILL             __FUNC(0x12)
#define SC_POOLINFO             __FUNC(0x13)
#define SC_POOLDEFAULT          __FUNC(0x14)
#define SC_POOLIDGET            __FUNC(0x15)
#define SC_SYSPOOLKILL          __FUNC(0x16)
#define SC_POOLHOOKREGISTER     __FUNC(0x17)

#define SC_PROCPRIOGET          __FUNC(0x20)
#define SC_PROCPRIOSET          __FUNC(0x21)
#define SC_PROCSLICEGET         __FUNC(0x22)
#define SC_PROCSLICESET         __FUNC(0x23)
#define SC_PROCIDGET            __FUNC(0x24)
#define SC_PROCPPIDGET          __FUNC(0x25)
#define SC_PROCNAMEGET          __FUNC(0x26)
#define SC_PROCSTART            __FUNC(0x27)
#define SC_PROCSTOP             __FUNC(0x28)
#define SC_PROCVARINIT          __FUNC(0x29)
#define SC_PROCSCHEDUNLOCK      __FUNC(0x2a)
#define SC_PROCPRIOCREATESTATIC __FUNC(0x2b)
#define SC_PROCINTCREATESTATIC  __FUNC(0x2c)
#define SC_PROCTIMCREATESTATIC  __FUNC(0x2d)
#define SC_PROCUSRINTCREATESTATIC  __FUNC(0x2e)
#define SC_PROCPRIOCREATE       __FUNC(0x2f)
#define SC_PROCINTCREATE        __FUNC(0x30)
#define SC_PROCTIMCREATE        __FUNC(0x31)
#define SC_PROCUSRINTCREATE     __FUNC(0x32)
#define SC_PROCKILL             __FUNC(0x33)
#define SC_PROCYIELD            __FUNC(0x34)
#define SC_PROCOBSERVE          __FUNC(0x35)
#define SC_SYSPROCCREATE        __FUNC(0x36)
#define SC_PROCSCHEDLOCK        __FUNC(0x37)

#define SC_PROCVARGET           __FUNC(0x38)
#define SC_PROCVARSET           __FUNC(0x39)
#define SC_PROCVARDEL           __FUNC(0x3a)
#define SC_PROCVARRM            __FUNC(0x3b)
#define SC_PROCUNOBSERVE        __FUNC(0x3c)
#define SC_PROCPATHGET          __FUNC(0x3d)
#define SC_PROCPATHCHECK        __FUNC(0x3e)
#define SC_PROCHOOKREGISTER     __FUNC(0x3f)

#define SC_MODULECREATE         __FUNC(0x40)
#define SC_MODULEKILL           __FUNC(0x41)
#define SC_MODULENAMEGET        __FUNC(0x42)
#define SC_MODULEIDGET          __FUNC(0x43)
#define SC_MODULEINFO           __FUNC(0x44)
#define SC_SYSMODULEPRIOSET     __FUNC(0x45)
#define SC_SYSMODULEPRIOGET     __FUNC(0x46)
#define SC_MODULEFRIENDADD      __FUNC(0x47)
#define SC_MODULEFRIENDRM       __FUNC(0x48)
#define SC_MODULEFRIENDGET      __FUNC(0x49)
#define SC_MODULEFRIENDNON      __FUNC(0x4a)
#define SC_MODULEFRIENDALL      __FUNC(0x4b)

#define SC_TRIGGERVALUESET      __FUNC(0x50)
#define SC_TRIGGERVALUEGET      __FUNC(0x51)
#define SC_TRIGGER              __FUNC(0x52)
#define SC_TRIGGERWAIT          __FUNC(0x53)

#define SC_TICK                 __FUNC(0x57)
#define SC_TMOADD               __FUNC(0x58)
#define SC_TMO                  __FUNC(0x59)
#define SC_SLEEP                __FUNC(0x5a)
#define SC_TMORM                __FUNC(0x5b)
#define SC_TICKGET              __FUNC(0x5c)
#define SC_TICKLENGTH           __FUNC(0x5d)
#define SC_TICKMS2TICK          __FUNC(0x5e)
#define SC_TICKTICK2MS          __FUNC(0x5f)

#define SC_CONNECTORREGISTER    __FUNC(0x60)
#define SC_CONNECTORUNREGISTER  __FUNC(0x61)
#define SC_DISPATCHER           __FUNC(0x62)
#define SC_PRIVATE1             __FUNC(0x63)

#define KERNEL_EILL_POOL_ID        __ERR(0x001)
#define KERNEL_ENO_MORE_POOL       __ERR(0x002)
#define KERNEL_EILL_POOL_SIZE      __ERR(0x003)
#define KERNEL_EPOOL_IN_USE        __ERR(0x004)
#define KERNEL_EILL_NUM_SIZES      __ERR(0x005)
#define KERNEL_EILL_BUF_SIZES      __ERR(0x006)
#define KERNEL_EILL_BUFSIZE        __ERR(0x007)
#define KERNEL_EOUTSIDE_POOL       __ERR(0x008)
#define KERNEL_EMSG_HD_CORRUPT     __ERR(0x009)
#define KERNEL_ENIL_PTR            __ERR(0x00A)
#define KERNEL_EENLARGE_MSG        __ERR(0x00B)
#define KERNEL_ENOT_OWNER          __ERR(0x00C)
#define KERNEL_EOUT_OF_MEMORY      __ERR(0x00d)
#define KERNEL_EILL_VECTOR         __ERR(0x00e)
#define KERNEL_EILL_SLICE          __ERR(0x00f)
#define KERNEL_ENO_KERNELD         __ERR(0x010)
#define KERNEL_EMSG_ENDMARK_CORRUPT   __ERR(0x011)
#define KERNEL_EMSG_PREV_ENDMARK_CORRUPT   __ERR(0x012)
#define KERNEL_EILL_DEFPOOL_ID     __ERR(0x013)
#define KERNEL_ELOCKED             __ERR(0x014)
#define KERNEL_EILL_PROCTYPE       __ERR(0x015)
#define KERNEL_EILL_INTERRUPT      __ERR(0x016)
#define KERNEL_EILL_EXCEPTION      __ERR(0x017)
#define KERNEL_EPROC_STOP_STOPPED  __ERR(0x018)
#define KERNEL_EUNLOCK_WO_LOCK     __ERR(0x01f)
#define KERNEL_EILL_PID            __ERR(0x020)
#define KERNEL_ENO_MORE_PROC       __ERR(0x021)
#define KERNEL_EMODULE_TOO_SMALL   __ERR(0x022)
#define KERNEL_ESTART_NOT_STOPPED  __ERR(0x023)
#define KERNEL_EILL_PROC           __ERR(0x024)

#define KERNEL_EILL_NAME           __ERR(0x025)
#define KERNEL_EILL_TARGET_NAME    KERNEL_EILL_NAME
#define KERNEL_EILL_MODULE_NAME    KERNEL_EILL_NAME
#define KERNEL_EILL_MODULE         __ERR(0x027)
#define KERNEL_EILL_PRIORITY       __ERR(0x028)
#define KERNEL_EILL_STACKSIZE      __ERR(0x029)
#define KERNEL_ENO_MORE_MODULE     __ERR(0x02A)
#define KERNEL_EILL_PARAMETER      __ERR(0x02b)
#define KERNEL_EILL_PROC_NAME      KERNEL_EILL_NAME

#define KERNEL_EPROC_NOT_PRIO      __ERR(0x02d)
#define KERNEL_ESTACK_OVERFLOW     __ERR(0x02e)
#define KERNEL_ESTACK_UNDERFLOW    __ERR(0x02f)
#define KERNEL_EILL_VALUE          __ERR(0x030)
#define KERNEL_EALREADY_DEFINED    __ERR(0x031)
#define KERNEL_ENO_MORE_CONNECTOR  __ERR(0x032)

#define KERNEL_EPROC_TERMINATE  __ERR(0xfff)


#define SC_MAXFUNC 0x63
#define SC_MAXERR  0x32
#endif

/*
****************************************************************************
** $Log: err.h,v $
** Revision 1.2  2005/09/05 14:37:30  bastian
** vacation
**
** Revision 1.1  2005/08/08 06:50:42  bastian
** 1st
**
** Revision 2.42  2005/07/05 06:52:29  bastian
** added error-code for illegal exception
**
** Revision 2.41  2005/07/01 11:18:25  bastian
** merge from branch-1.7.2
**
** Revision 2.40.4.2  2005/06/21 05:24:14  bastian
** added hook-syscalls
**
** Revision 2.40.4.1  2005/05/20 11:41:35  bastian
** changed target to system
**
** Revision 2.40  2004/04/07 10:54:32  bastian
** changed order
**
** Revision 2.39  2004/01/08 13:53:35  bastian
** added sc_procPathCheck
**
** Revision 2.38  2003/10/24 09:39:24  bastian
** added new function: sc_procPathGet
**
** Revision 2.37  2003/09/26 12:43:53  bastian
** extended error-check
**
** Revision 2.36  2003/03/28 15:07:25  bastian
** r2/r13 saved in module
**
** Revision 2.35  2003/03/26 13:59:46  bastian
** added system-call/trap interface
** changed 2nd parameter of miscError
**
** Revision 2.34  2003/03/25 14:10:25  bastian
** removed UL from assembly defines
**
** Revision 2.33  2003/03/19 08:46:25  bastian
** fixed sc_sysModulePrioSet
**
** Revision 2.32  2003/03/17 09:00:03  bastian
** added header
**
** Revision 2.31  2003/02/26 09:38:32  bastian
** Added tmoRm
**
** Revision 2.30  2003/01/30 08:23:38  bastian
** added schedlock
**
** Revision 2.29  2002/12/17 06:46:28  bastian
** fixed conflict
**
** Revision 2.28  2002/12/17 06:43:07  bastian
** rearranged function codes
**
** Revision 2.27  2002/12/16 08:51:49  bastian
** started connector
**
** Revision 2.26  2002/12/09 09:46:14  bastian
** changed pool-function names
**
** Revision 2.25  2002/12/09 09:31:25  bastian
** renamed msgPool functions
** fixed typos
** added pool-name for sciopta.con
**
** Revision 2.24  2002/12/06 16:13:26  bastian
** added pool-names
**
** Revision 2.23  2002/11/22 14:09:57  bastian
** week end
**
** Revision 2.22  2002/11/15 16:17:11  bastian
** WE
**
** Revision 2.21  2002/08/30 13:44:35  bastian
** renamed connect => observe
**
** Revision 2.20  2002/07/25 08:34:15  bastian
** async tmo added
**
** Revision 2.19  2002/07/24 13:29:26  bastian
** added usrint
**
** Revision 2.18  2002/06/07 13:29:32  bastian
** added ill_interrupt
**
** Revision 2.17  2002/06/04 12:01:39  bastian
** fixed/added functions
**
** Revision 2.16  2002/04/16 07:02:41  bastian
** many many changes ...
**
** Revision 2.15  2002/03/27 09:27:47  bastian
** more Error checking
**
** Revision 2.14  2002/03/22 16:36:17  bastian
** added ownerget
** protection for stdio in scio
**
** Revision 2.13  2002/03/21 14:46:41  bastian
** Added debugging support for nearly all functions.
** Added error-checking if swap-out fails.
** Fixed friend-concept.
** Misc. fixes.
**
** Revision 2.12  2002/03/20 16:16:59  bastian
** intermediate
**
** Revision 2.11  2002/03/19 16:31:47  bastian
** intermediate
**
** Revision 2.10  2002/03/08 07:40:59  bastian
** Added info
** Rearanged and cleaned up module header
** Added infos to the module header.
**
** Revision 2.9  2002/03/04 07:02:42  bastian
** Removed timeout server from kernel. Maybe should be a daemon.
**
** Revision 2.8  2002/02/20 07:28:12  bastian
** Added error for procSchedUnlock
**
** Revision 2.7  2002/02/19 13:32:12  bastian
** Added new error for msg checks
**
** Revision 2.6  2001/12/21 16:25:20  bastian
** Added special pid to retrieve current process's data. 0 is no longer valid
**
** Revision 2.5  2001/12/21 09:18:23  bastian
** Renamed sc_error into sc_sysError.
** Added error-classifier, to enable error to continue if possible.
**
** Revision 2.4  2001/12/19 11:55:22  bastian
** Added procVar
**
** Revision 2.3  2001/12/14 15:42:49  bastian
** Added new error for buffer corruption.
**
** Revision 2.2  2001/12/12 07:56:35  bastian
** Fixes for IPS
**
** Revision 2.1  2001/12/11 10:02:40  bastian
** Added local vars.
**
** Revision 2.0.0.1  2001/11/02 05:20:16  bastian
** moved over to cvs
**
** 
**    Rev 1.9   07 Nov 2001 08:26:36   skywalker
** intermediate checkin
** 
**    Rev 1.8   16 Oct 2001 15:45:52   skywalker
** intermediate check-in
** 
**    Rev 1.7   10 Sep 2001 10:33:14   skywalker
** added error-codes
** 
**    Rev 1.6   03 Sep 2001 07:33:18   skywalker
** Errorcode for sleep
** 
**    Rev 1.5   Aug 27 2001 14:36:50   bastian
** added connect
** 
**    Rev 1.4   Aug 20 2001 10:37:50   bastian
** Changed LOCK/UNLOCK macros
** 
**    Rev 1.3   Jul 23 2001 13:34:52   bastian
** Bug fixes
** 
**    Rev 1.2   Jul 19 2001 16:20:22   bastian
** Added interrupt processes
** 
**    Rev 1.1   Jul 16 2001 13:42:58   bastian
** fixed (?) API
**
****************************************************************************
*/
