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
** +Revision: 1.9 +                                                 **
** +Date: 2008/02/14 13:50:51 +                                     **
** Static kernel configuration                                      **
**********************************************************************
EOC*/

#ifndef _ARCH_OS_H_
#define _ARCH_OS_H_ 1

#ifndef SCIOPTA
#define SCIOPTA       WIN32_HOSTED
#define CPU           I386
#define CPUTYPE       (-1)
#define SC_SCALE      SC_MAXIMUM
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
void enterInterrupt(int vector,void *);
void launchProcess(void *_pcb);
void deleteProcess(void *p,int postpone);
void q_err(void);
void fake_int(void *pcb,int src);
void activateTimer(void *pcb,int src);
void dispatcher(void);

unsigned long sc_sysLock(void);
void sc_sysUnlock(unsigned long);
int sc_sysLocked(void);

/*
** inline functions/macros
*/

#ifdef __GNUC__
#define INLINE __inline__
#define STATIC_INLINE static __inline__
#else
#define INLINE __inline
#define STATIC_INLINE __inline static
#endif


#define LOCK(x)  x = sc_sysLock()
#define UNLOCK(x)  sc_sysUnlock(x)
#define LOCKED() (sc_sysLocked())

#define DI() asm volatile ("cli")
#define EI() asm volatile ("sti")

#define ASM_NOP Sleep(INFINITE)

#else
#define LOCK(x) pushfl;popl x;cli
#define UNLOCK(x) bt $9,x; jnc 1f; sti; 1:


#define LOCKED() call locked

#define DI() cli
#define EI() sti
#endif

#endif

