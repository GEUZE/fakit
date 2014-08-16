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
** ID: S01103BS29                                                   **
** +Revision: 1.9 +                                                 **
** +Date: 2005/07/01 11:18:25 +                                     **
** Static kernel configuration                                      **
**********************************************************************
EOC*/
#ifndef _ARCH_OS_H_
#define _ARCH_OS_H_ 1

#ifndef __KKERNEL__
#define __KKERNEL__ 42
#endif

#ifndef SCIOPTA
#define SCIOPTA       NATIVE_HOSTED
#define CPU           M68HC12
#define SC_SCALE      SC_MEDIUM
#endif

#ifndef _SC_CONF_H_
#define _SC_CONF_H_        42
#define SC_MAX_POOL        4
#define SC_MAX_PROCESS     128

#define SC_MSG_STAT        1
#define SC_PROC_STAT       1
/*
** pool hooks
*/
#define SC_POOL_HOOK       1
#define SC_POOLCREATE_HOOK 1
#define SC_POOLKILL_HOOK   1
/*
** message hooks
*/
#define SC_MSG_HOOK        1
#define SC_MSGTX_HOOK      1
#define SC_MSGRX_HOOK      1
/*
** process hooks
*/
#define SC_PROC_HOOK 1
#define SC_PROCCREATE_HOOK 1
#define SC_PROCKILL_HOOK 1
#define SC_PROCSWAP_HOOK 1
/*
** error hook
*/
#define SC_ERR_HOOK 1

/*
** design constants
*/
#define SC_USE_FRIENDS 1
#define SC_MAX_NUM_BUFFERSIZES (8)
#define SC_MAX_MODULE          (1)
#define SC_MAX_CONNECTOR       (2)
/*
** error checks
*/
#define SC_MSG_PARA_CHECK  1
#define SC_POOL_PARA_CHECK 0
#define SC_PROC_PARA_CHECK 1
#define SC_MSG_CHECK 1

#define SC_USE_TMO 1
#define SC_USE_SYSCALL     0
#define SC_BANKED          0

#ifdef SC_CDEBUG
#define SC_CDEBUG          1
#else
#define SC_CDEBUG          0
#endif

#endif /* _SC_CONF_H_ */
/*
** hardware specifica
*/
#define SC_MAX_INT_VECTOR      (63)
#define SC_MAX_INT_VECTORS      (64)

#define ASM_NOP
#ifndef __ASM_INCLUDED__
unsigned short sc_sysLock(void);
void sc_sysUnlock(unsigned short);

#define LOCK(a) a = sc_sysLock()
#define UNLOCK(a) sc_sysUnlock(a)
#endif
#endif
