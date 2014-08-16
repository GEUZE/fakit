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
** +Revision: 2.45.4.2 +                                                **
** +Date: 2009/10/08 07:07:23 +                                     **
** Error and function code definitions                              **
**********************************************************************
EOC*/

#ifndef _KERNEL_ERR_H_
#define _KERNEL_ERR_H_

#define __FUNC(a) (a<<24)
#define __ERR(a)  (a<<12) 
#define __CLASS(a) (a)

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
#define SC_MISCERRORHOOKREGISTER __FUNC(0x18)
#define SC_MISCKERNELDREGISTER   __FUNC(0x19)
#define SC_MISCCRCCONTD          __FUNC(0x1a)
#define SC_MISCCRC               __FUNC(0x1b)
#define SC_MISCERRNOSET          __FUNC(0x1c)
#define SC_MISCERRNOGET          __FUNC(0x1d)

#define SC_PROCWAKEUPENABLE     __FUNC(0x1e)
#define SC_PROCWAKEUPDISABLE    __FUNC(0x1f)
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
#define SC_MODULEFRIENDNONE     __FUNC(0x4a)
#define SC_MODULEFRIENDALL      __FUNC(0x4b)

#define SC_PROCDAEMONUNREGISTER  __FUNC(0x4e)
#define SC_PROCDAEMONREGISTER    __FUNC(0x4f)

#define SC_TRIGGERVALUESET      __FUNC(0x50)
#define SC_TRIGGERVALUEGET      __FUNC(0x51)
#define SC_TRIGGER              __FUNC(0x52)
#define SC_TRIGGERWAIT          __FUNC(0x53)

#define SC_MISCERROR             __FUNC(0x55)

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
#define SC_PROCREGISTERIRQCALLBACK __FUNC(0x64)

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
#define KERNEL_EILL_SYSCALL        __ERR(0x018)
#define KERNEL_EILL_NESTING        __ERR(0x019)

#define KERNEL_EUNLOCK_WO_LOCK     __ERR(0x01f)
#define KERNEL_EILL_PID            __ERR(0x020)
#define KERNEL_ENO_MORE_PROC       __ERR(0x021)
#define KERNEL_EMODULE_TOO_SMALL   __ERR(0x022)
#define KERNEL_ESTART_NOT_STOPPED  __ERR(0x023)
#define KERNEL_EILL_PROC           __ERR(0x024)

#define KERNEL_EILL_NAME           __ERR(0x025)
#define KERNEL_EILL_SYSTEM_NAME    KERNEL_EILL_NAME

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
#define KERNEL_EPROC_TERMINATE     __ERR(0xfff)

#define SC_MAXFUNC 0x63
#define SC_MAXERR  0x32

/* deprecated */
#if 0
#define KERNEL_EILL_TARGET_NAME    KERNEL_EILL_NAME
#define SC_ERR_TARGET_FATAL     0x01 /* obsolete */
#define SC_ERR_TARGET_WARNING   0x10 /* obsolete */
#define SC_ERR_PROC_WARNING     0x40 /* obsolete */
#endif

#endif

