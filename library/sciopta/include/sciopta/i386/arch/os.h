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
** ID: S01103BS44                                                   **
** +Revision: 2.28 +                                                 **
** +Date: 2008/04/11 07:54:15 +                                     **
** Static kernel configuration                                      **
**********************************************************************
EOC*/
#ifndef _ARCH_OS_H_
#define _ARCH_OS_H_ 1

#ifndef SCIOPTA
#define SCIOPTA       NATIVE_HOSTED
#define CPU           I386
#define CPUTYPE       (-1)
#define SC_SCALE SC_MAXIMUM
#endif

#ifndef _SC_CONF_H_
#define _SC_CONF_H_ 42

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
#define SC_MSG_CHECK 1

#define SC_USE_SYSCALL     0

#ifdef SC_CDEBUG
#define SC_CDEBUG          1
#else
#define SC_CDEBUG          0
#endif

#endif

/*
** hardware specifica
*/
#define SC_MAX_INT_VECTORS     (256)
#define SC_MAX_INT_VECTOR      (255)

#ifndef __ASM_INCLUDED__
externC void enterInterrupt(int vector,void *);
externC void launchProcess(void *_pcb);
externC void deleteProcess(void *p,int postpone);
externC void q_err(void);
externC void fake_int(void *pcb,int src);
externC void activateTimer(void *pcb,int src);
externC void dispatcher(void);
externC unsigned long lock(void);
externC void unlock(unsigned long);
externC int locked(void);

externC void start_hook(void);   // user function
externC void TargetSetup(void); // sconf.c
/*
** inline functions/macros
*/
#define LOCK(x)  x = lock()
#define UNLOCK(x)  unlock(x)
#define LOCKED() (locked())

#define DI() asm volatile ("cli")
#define EI() asm volatile ("sti")

#define ASM_NOP asm(" nop")

#ifdef __GNUC__
#define ASM_NOP asm(" nop")
#define INLINE __inline__
#define STATIC_INLINE static __inline__
#endif

#else
#define LOCK(x) pushfl;popl x;cli
#define UNLOCK(x) bt $9,x; jnc 1f; sti; 1:


#define LOCKED() call locked

#define DI() cli
#define EI() sti
#define INLINE __inline__
#define STATIC_INLINE static __inline__
#endif

#endif

