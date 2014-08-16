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
** +Revision: 1.8 +                                                 **
** +Date: 2008/01/16 13:02:02 +                                     **
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
#define CPU           M16C
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

#define SC_USE_TMO 0
#define SC_USE_SYSCALL     0
#define SC_BANKED          1

#ifdef SC_CDEBUG
#define SC_CDEBUG          1
#else
#define SC_CDEBUG          0
#endif

#endif
/*
** hardware specifica
*/
#define SC_MAX_INT_VECTOR      (31)
#define SC_MAX_INT_VECTORS      (32)

#ifndef __ASM_INCLUDED__
externC void enterInterrupt(int vector,void *);
externC void launchProcess(void *pcb);
externC void deleteProcess(void *p,int postpone);
externC void q_err(void);
externC void fake_int(void *pcb,int kind);
externC void activateTimer(void *pcb,int kind);
externC void *dispatcher(void);
externC unsigned int sc_sysLock(void);
externC void sc_sysUnlock(unsigned int);
externC int sc_sysLocked(void);

externC void start_hook(void);   // user function
externC void TargetSetup(void); // sconf.c

#define LOCK(x)  x = sc_sysLock()
#define UNLOCK(x)  sc_sysUnlock(x)
#define LOCKED() (locked())

#if defined(__IAR_SYSTEMS_ICC__) 
#define STATIC_INLINE
#define ASM_NOP __asm(" nop")
#elif defined(__TASKING__)
#define STATIC_INLINE
#define ASM_NOP __nop()
#elif defined(NC30)
#define ASM_NOP asm(" nop")
#define STATIC_INLINE
#else
#define ASM_NOP
#endif

#endif /* __ASM_INCLUDED__ */

#endif
