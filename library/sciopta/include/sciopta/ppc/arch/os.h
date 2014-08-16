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
** ID: S01103BS36                                                   **
** +Revision: 1.29 +                                                 **
** +Date: 2006/11/23 15:09:37 +                                     **
** Static kernel configuration                                      **
**********************************************************************
EOC*/
#ifndef _ARCH_OS_H_
#define _ARCH_OS_H_ 1

#define MPC8xx        1
#define MPC82xx       2
#define MPC5xx        3
#define MPC5200       4
#define MPC5500       5
#define AMCC440GP     6

#ifndef SCIOPTA
#define SCIOPTA       NATIVE_HOSTED
#define CPU           PPC 
#define CPUTYPE       (-1)
#define SC_SCALE SC_MAXIMUM
#endif

#ifndef _SC_CONF_H_
#define _SC_CONF_H_ STATIC_CONF

#define SC_MSG_STAT  1
#define SC_PROC_STAT 1
/*
** pool hooks
*/
#define SC_POOL_HOOK 1
#define SC_POOLCREATE_HOOK 1
#define SC_POOLKILL_HOOK 1
/*
** message hooks
*/
#define SC_MSG_HOOK 1
#define SC_MSGTX_HOOK 1
#define SC_MSGRX_HOOK 1
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
#define SC_MAX_MODULE          (16)
#define SC_MAX_CONNECTOR       (2)

/*
** error checks
*/
#define SC_MSG_PARA_CHECK  1
#define SC_POOL_PARA_CHECK 0
#define SC_PROC_PARA_CHECK 1
#define SC_MSG_CHECK       1

#define SC_USE_TRAP        0

#ifdef SC_CDEBUG
#define SC_CDEBUG          1
#else
#define SC_CDEBUG          0
#endif

/*
** hardware specifica
*/
#define SC_MAX_INT_VECTORS     (256)
#define SC_MAX_INT_VECTOR      (255)

#endif /* !_SC_CONF_H_*/

#ifndef __ASM_INCLUDED__
/*
** Reference Kernel only ... {
*/
#ifdef INTERNAL
externC void enterInterrupt(int vector,void *);
externC void launchProcess(void *mod,int sc_pid,void (*func)(void),int type);
externC void deleteProcess(void *p,int postpone);
externC void q_err(void);
externC void fake_int(void *pcb);
externC void activateTimer(void *pcb);
externC void dispatcher(void);
externC int locked(void);
#define LOCKED() (locked())
#endif
/* } */

externC unsigned int sc_sysLock(void);
externC void sc_sysUnlock(unsigned int);
externC void start_hook(void);   /* user function */
externC void TargetSetup(void);  /* in sconf.c */

/*
** inline functions/macros
*/
#define LOCK(x)  x = sc_sysLock()
#define UNLOCK(x)  sc_sysUnlock(x)


#define ASM_NOP asm(" nop")
#define INLINE __inline__
#define STATIC_INLINE static __inline__

#endif /* __ASM_INCLUDED__ */

#endif


