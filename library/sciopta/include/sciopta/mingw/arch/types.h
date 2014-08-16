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
** ID: S01103BS43                                                   **
** +Revision: 1.11 +                                                **
** +Date: 2008/01/16 13:02:02 +                                     **
** Systemwide host specific type definitions                        **
**********************************************************************
EOC*/
#ifndef _ARCH_TYPES_H_
#define _ARCH_TYPES_H_

#define SIZEOF_POOLID_T  4
#define SIZEOF_BUFSIZE_T 4
#define SIZEOF_PID_T     4
#define SIZEOF_PTR       4
#define SIZEOF_MSGID     4
#define SIZEOF_INT       4
#define SIZEOF_TICKS_T   4

#ifndef __ASM_INCLUDED__

#if defined __GNUC__
#define __PACKED__ __attribute__((packed))
#elif defined _WIN32
#define __PACKED__
#else
#error Unsupported Compiler
#endif

#define FARPTR  *
#define NEARPTR  *

#define NEAR_NULL NULL

#include <limits.h>

typedef unsigned short     __u16;
typedef signed short       __s16;
typedef unsigned long      __u32;
typedef signed long        __s32;
typedef unsigned char      __u8;
typedef signed char        __s8;
#ifndef __cplusplus
#if (_MSC_VER > 1200) || !defined(_MSC_VER)
typedef unsigned long long __u64;
typedef signed long long   __s64;
#endif
#endif

#define __UINT __uint
#define __SINT __sint

typedef unsigned int       __uint;
typedef signed int         __sint;

#ifndef UCHAR_MAX
#define UCHAR_MAX ((unsigned char)0xff)
#endif
#ifndef USHRT_MAX
#define USHRT_MAX ((unsigned short)0xffff)
#endif
#ifndef ULONG_MAX
#define ULONG_MAX 0xffffffffU
#endif
#ifndef UINT_MAX
#define UINT_MAX 0xffffffffU
#endif
#ifndef U8_MAX
#define U8_MAX UCHAR_MAX
#endif
#ifndef U16_MAX
#define U16_MAX USHRT_MAX
#endif
#ifndef U32_MAX
#define U32_MAX ULONG_MAX
#endif

/*
** SCIOPTA types
*/
union sc_msg;
typedef union sc_msg * sc_msg_t;
typedef union sc_msg * * sc_msgptr_t;

typedef __u32 sc_ticks_t;
#define SC_TMO_MAX       ((sc_ticks_t)((1<<24)-1))
#define SC_ENDLESS_TMO   ((sc_ticks_t)U32_MAX)
#define SC_FATAL_IF_TMO  ((sc_ticks_t)(U32_MAX-1))
#define SC_NO_TMO        ((sc_ticks_t)0)
typedef __u32 sc_time_t;
typedef __u32 sc_errcode_t;
typedef __u32 sc_extra_t;
typedef __u32 sc_plsize_t;
typedef __u32 sc_modulesize_t;
typedef __u32 sc_bufsize_t;
typedef __u32 sc_msgid_t;
#define SC_MSGID_BASE    0x80000000
typedef __u32 sc_pid_t;
#define SC_PID_MAX       0x00003fffUL
#define SC_PID_MSK       0x00003fffUL
#define SC_PID_INC       0x00004000UL
#define SC_ILLEGAL_PID   0x7fffffff
#define SC_PID_TMO_MAGIC 0x7ffffffe
#define PID_EXTERN_FLAG  0x80000000UL
#define SC_CURRENT_PID   ((sc_pid_t)-1)

typedef __u32 sc_poolid_t;
typedef __u32 sc_moduleid_t;
#define MODULE_MASK      0x7f000000UL
#define MODULE_SHIFT     (24)
typedef sc_ticks_t  sc_prio_t;
typedef __u16 sc_proc_type_t;
typedef __u32 sc_cpuflags_t;
typedef __u32 sc_tag_t;
typedef __u32 sc_var_t;
typedef int sc_triggerval_t;
typedef __u32 __ptrsize_t;

typedef struct host_s {
  volatile long lock;
  int normal_exit;
  void *dispatcherHandle;
} host_t;

typedef struct pcb_host_s {
  __u32 pid;
  void * handle;
  void * con;
} pcb_host_t;

#else /* __ASM_INCLUDED__ */

#define SC_TMO_MAX       (0x00ffffff)
#define SC_ENDLESS_TMO   (0xffffffff)
#define SC_FATAL_IF_TMO  (SC_ENDLESS_TMO-1)
#define SC_NO_TMO        (0)


#define SC_MSGID_BASE    0x80000000

#define SC_PID_MAX       0x00003fff
#define SC_PID_MSK       0x00003fff
#define SC_PID_INC       0x00004000
#define SC_ILLEGAL_PID   0x7fffffff
#define SC_PID_TMO_MAGIC 0x7ffffffe
#define PID_EXTERN_FLAG  0x80000000
#define SC_CURRENT_PID   0xffffffff

#define MODULE_MASK      0x7f000000
#define MODULE_SHIFT     (24)

#endif /* __ASM_INCLUDED__ */

#endif
