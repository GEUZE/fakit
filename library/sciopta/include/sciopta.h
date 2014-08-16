/* -*-c++-*-
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
** (c) 2002,2003 Sciopta GmbH, Litronic AG/ Schweiz                 **
**                                                                  **
**********************************************************************
** ID: S01103BS45                                                   **
** +Revision: 2.74.4.4 +                                                **
** +Date: 2009/10/27 14:32:06 +                                     ** 
** API definitions for Sciopta                                      **
**********************************************************************
*/

#ifndef _SCIOPTA_H_
#define _SCIOPTA_H_ 1

#ifdef __cplusplus
#define externC extern "C"
#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END }
#else
#define externC
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#endif /*__cplusplus */

#ifdef __IAR_SYSTEMS_ICC__
/* disable MISRA C Rule 14*/
/* We need to use char without signed/unsigned in order to be
** compatible with the libc
*/
#pragma diag_suppress=Pm012
#endif

#ifdef PPC
#undef PPC /* this is defined by the PPC gcc */
#endif
#ifdef MSP430
#undef MSP430 /* this is defined by the MSP430 gcc */
#endif
#ifdef M16C
#undef M16C  /* NC30 defines M16C */
#endif

/* SOCNV */
/*
** possible host-systems
*/
#define NATIVE_HOSTED   0
#define LINUX_HOSTED    1
#define WIN32_HOSTED    2
#define CYGWIN_HOSTED   3
/*
** possible cpus
*/
#define ANY             0
#define ARM             1
#define STRONGARM       2
#define SH              3
#define I386            4
#define PPC             5
#define COLDFIRE        6
#define M68K            7
#define M68HC12         8
#define C166            9
#define MSP430         10
#define M16C           11
#define R65C02         12
#define ATMEGA         13
#define V850           14
/*
** possible Sciopta models
*/
#define SC_MAXIMUM         0
#define SC_MEDIUM          1
#define SC_MINIMUM         2

/* EOCNV */
/*
** Get personality
*/
#include <arch/os.h>

/*
** helpers
*/
#define _SCC(a,b) a##b
#define __SCC(a,b) _SCC(a,b)

#if defined __SYSCALL__ && defined SC_USER_PREFIX
#define __P__ __SCC(SC_USER_PREFIX,__)
#elif defined __SYSCALL__ && ! defined SC_USER_PREFIX
#define __P__ __
#elif defined SC_USER_PREFIX
#define __P__ __SCC(SC_USER_PREFIX,_)
#else 
#define __P__
#endif

#ifndef __IAR_SYSTEMS_ASM__
/* magic function-name mangling */
#include <sciopta_hlp.h>
#endif

/*
** Get type-definitions
*/
#include <ossys/types.h>

#ifndef __ASM_INCLUDED__
#if !defined(NEARPTR) || !defined(FARPTR)
#error Must define NEARPTR and FARPTR in arch/types.h
#endif
#endif

/******************/
/* Sciopta API    */
/******************/
#ifdef __KKERNEL__
#include <container/sgl_list.h>
#include <container/dbl_list.h>
#include <Kkernel/var.h>
#include <Kkernel/pcb.h>
#include <Kkernel/pool.h>
#include <Kkernel/msg.h>
#include <Kkernel/misc.h>
#include <Kkernel/err.h>
#include <Kkernel/sleep.h>
#include <Kkernel/proc.h>
#include <Kkernel/target.h>
#include <Kkernel/tick.h>
#include <Kkernel/trigger.h>
#include <Kkernel/tmo.h>
#include <Kkernel/connector.h>
#include <kernel/kprintf.h>
#define SC_MODULE_NAME_SIZE (SC_PROC_NAME_SIZE)
#elif defined (__TKERNEL__)
#include <container/sgl_list.h>
#include <tkernel/pcb.h>
#include <tkernel/proc.h>
#include <tkernel/msg.h>
#include <tkernel/err.h>
#include <tkernel/pool.h>
#include <tkernel/system.h>
#include <tkernel/sleep.h>
#include <tkernel/tick.h>
#include <tkernel/misc.h>
#include <tkernel/kprintf.h>
#elif defined (__SCAPI__)
#include <scapi/hostos.h>
#include <container/dbl_list.h>
#include <scapi/msg.h>
#include <scapi/pool.h>
#include <scapi/pcb.h>
#include <scapi/proc.h>
#include <scapi/module.h>
#include <scapi/misc.h>
#include <scapi/system.h>
#include <scapi/tick.h>
#include <scapi/sleep.h>
#include <scapi/connector.h>
#include <scapi/string.h>
#include <kernel/err.h>
#else
#ifndef __ASM_INCLUDED__
#include <container/sgl_list.h>
#include <container/dbl_list.h>
#endif
#include <kernel/var.h>
#include <kernel/pcb.h>
#include <kernel/pool.h>
#include <kernel/msg.h>
#include <kernel/misc.h>
#include <kernel/err.h>
#include <kernel/sleep.h>
#include <kernel/proc.h>
#include <kernel/module.h>
#include <kernel/target.h>
#include <kernel/kprintf.h>
#include <kernel/tick.h>
#include <kernel/trigger.h>
#include <kernel/tmo.h>
#include <kernel/connector.h>
#ifndef __ASM_INCLUDED__
externC void memcpy32(void *dest, const void *src, size_t n);
externC  void memset32(void *from,__u32 c, size_t n);
#endif /* __ASM_INCLUDED__ */
#endif

#define SC_PROC_PATH_SIZE ( 3*SC_PROC_NAME_SIZE+4 )

/******************/
/*    Macros      */
/******************/
#ifndef __ASM_INCLUDED__
#ifdef __WIN32__
#define SC_PROCESS(x) void x(void)
#define SC_INT_PROCESS(x,y) void x(int (y))
#define SC_TIM_PROCESS(x,y) void x(int (y))
#elif defined __TKERNEL__ && defined __IAR_SYSTEMS_ICC__
#define SC_PROCESS(x) externC __task void x(void)
#define SC_INT_PROCESS(x,y) externC __task void x(int (y))
#define SC_TIM_PROCESS(x,y) externC __task void x(int (y))
#elif defined __ICCCF__
#define SC_PROCESS(x) externC __task void x(void)
#define SC_INT_PROCESS(x,y) externC void x(int (y))
#define SC_TIM_PROCESS(x,y) externC void x(int (y))
#else
#define SC_PROCESS(x) externC void x(void)
#define SC_INT_PROCESS(x,y) externC void x(int (y))
#define SC_TIM_PROCESS(x,y) externC void x(int (y))
#endif
#endif /* __ASM_INCLUDED__ */

#define CC(x,y) x##y
#define OS_NAMING(x) CC(SC,x)

#ifdef __cplusplus

#include <sciopta.msg>

/*
** Convenient macros to define new and delete
** privatly in new classes.
*/
#define OPERATOR_NEW(MSGID) \
inline void * operator new(size_t size) {      \
  void *tmp;                                   \
  tmp = sc_msgAllocClr(size+sizeof(sc_msgid_t),\
			(MSGID),               \
			SC_DEFAULT_POOL,       \
			SC_FATAL_IF_TMO);      \
  tmp = (void *)((__ptrsize_t ) tmp + sizeof (sc_msgid_t)); \
  return tmp;                                  \
}                                              \
inline void * operator new[]( size_t size )    \
{ return operator new(size); }

#define OPERATOR_DELETE() \
inline void operator delete(void *ptr){   \
  if (ptr) {                              \
    ptr = (void *)((__ptrsize_t )ptr - sizeof (sc_msgid_t)); \
    sc_msgFree((sc_msgptr_t) &ptr);       \
  }                                       \
}                                         \
inline void operator delete[]( void * ptr ){ operator delete(ptr); }

#define OPERATOR_DELETE_COMMON() \
inline void operator delete(void *ptr){   \
  if (ptr) {                              \
    ptr = (void *)((__ptrsize_t )ptr - sizeof (sc_msgid_t)); \
    /* public delete needs acquire !! */  \
    sc_msgAcquire((sc_msgptr_t) &ptr);    \
    sc_msgFree((sc_msgptr_t) &ptr);       \
  }                                       \
}                                         \
inline void operator delete[]( void * ptr ){ operator delete(ptr); }


#endif /* __cplusplus */

#ifdef __IAR_SYSTEMS_ICC__
/* Enable MISRA C Rule 14*/
#pragma diag_default=Pm012
#endif
#endif /* _SCIOPTA_H_ */
