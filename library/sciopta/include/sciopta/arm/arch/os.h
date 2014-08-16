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
** ID: S02338BS3                                                    **
** +Revision: 1.28 +                                                 **
** +Date: 2008/11/11 10:01:40 +                                     **
** Static kernel configuration                                      **
**********************************************************************
EOC*/

#ifndef _ARCH_OS_H_
#define _ARCH_OS_H_ 1

/* SOCNV */
/* CPU types */
#define ARMv4T    1
#define ARMv5T    2
#define ARMv5TE   3
#define Xscale    4
#define ARMv6     5
#define ARMv6M    6
#define ARMv7     7
#define ARMv7M    8
/* EOCNV */

#ifndef SCIOPTA
#define SCIOPTA       NATIVE_HOSTED
#define CPU           ARM
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

#define SC_USE_TMO         1
#define SC_USE_TRAP        0
#define SC_BANKED          0

#ifdef SC_CDEBUG
#undef SC_CDEBUG
#define SC_CDEBUG          1
#else
#define SC_CDEBUG          0
#endif
/*
** hardware specifica
*/
#ifdef SC_MAX_INT_VECTORS
#else
#define SC_MAX_INT_VECTORS      (128)
#define SC_MAX_INT_VECTOR       (128-1)
#endif

#endif

#ifndef __ASM_INCLUDED__
/*
** Reference Kernel only ... {
*/
#ifdef INTERNAL
#ifdef __KKERNEL__
externC void launchProcess(void *_pcb);
externC void activateTimer(void *pcb,int flag);
externC void fake_int(void *pcb_,int flag);
#else
externC void launchProcess(void *_pcb);
externC void activateTimer(void *pcb);
externC void fake_int(void *pcb);
#endif

externC void enterInterrupt(int vector,void *);
externC void deleteProcess(void *p,int postpone);
externC void q_err(void);


externC void *dispatcher(void);
externC int locked(void);
#define LOCKED() (locked())
#endif
/* } */

externC unsigned int sc_sysLock(void);
externC void sc_sysUnlock(unsigned int);
externC void start_hook(void);   // user function
externC void TargetSetup(void);  // in sconf.c

/*
** inline functions/macros
*/
#define LOCK(x)  x = sc_sysLock()
#define UNLOCK(x)  sc_sysUnlock(x)


#ifdef __GNUC__
#define ASM_NOP __asm__(" nop")
#define INLINE __inline__
#define STATIC_INLINE static __inline__
#elif defined __MWERKS__
#define ASM_NOP asm{ nop }
#define INLINE
#define STATIC_INLINE static
#elif defined __ARMCC_VERSION
#define ASM_NOP __nop()
#define INLINE __inline
#define STATIC_INLINE static __inline
#elif defined __IAR_SYSTEMS_ICC__
#define ASM_NOP __asm(" nop")
#define INLINE _Pragma("inline")
#define STATIC_INLINE _Pragma("inline") static
#define DECL_MODULE(module, size)  \
  __no_init char (module##_module)[(size)] @ CC(#module,"_mod");      \
  sc_module_addr_t (module##_mod) = {            \
     (module##_module),          /* start */     \
     sizeof((module##_module)),  /* size */      \
     0                           /* code/initialized data size */  \
  }
#else
#define ASM_NOP
#define INLINE
#endif

#endif /* __ASM_INCLUDED__ */
#endif /* _ARCH_OS_H_ */

