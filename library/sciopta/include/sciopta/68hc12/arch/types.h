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
** ID: S03080BS4                                                    **
** +Revision: 1.15 +                                                 **
** +Date: 2008/01/16 13:02:02 +                                     **
** Systemwide host specific type definitions                        **
**********************************************************************
EOC*/

#ifndef _ARCH_TYPES_H_
#define _ARCH_TYPES_H_

/* SOCNV */
#define SIZEOF_POOLID_T  2
#define SIZEOF_BUFSIZE_T 2
#define SIZEOF_PID_T     2
#define SIZEOF_PTR       2
#define SIZEOF_FUNCPTR   (2+BANK_OFF)
#define SIZEOF_MSGID     2
#define SIZEOF_INT       2
#define SIZEOF_TICKS_T   2
/* EOCNV */
#ifndef __ASM_INCLUDED__

#if defined __GNUC__
#define __PACKED__ __attribute__((packed))
#else
#define __PACKED__
#endif

//->#ifdef __MWERKS__
//->#define NEARPTR * __near
//->#define FARPTR  * __far
//->#elif defined(__GNUC__)
//->#define NEARPTR *
//->#define FARPTR *
//->#endif
#define NEARPTR * 
#define FARPTR  *

#ifndef __cplusplus
#define NEAR_NULL ((void NEARPTR)0)
#else
#define NEAR_NULL (0)
#endif

#include <limits.h>

typedef unsigned short     __u16;
typedef signed short       __s16;
typedef unsigned long      __u32;
typedef signed long        __s32;
typedef unsigned char      __u8;
typedef signed char        __s8;
//typedef unsigned long long __u64;
//typedef signed long long   __s64;

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
#define UINT_MAX 0xffffU
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
typedef union sc_msg NEARPTR sc_msg_t;
typedef union sc_msg NEARPTR NEARPTR sc_msgptr_t;

typedef __u16 sc_ticks_t;
#define SC_TMO_MAX       ((sc_ticks_t)0x3fff)
#define SC_ENDLESS_TMO   ((sc_ticks_t)U16_MAX)
#define SC_FATAL_IF_TMO  ((sc_ticks_t)(U16_MAX-1))
#define SC_NO_TMO        ((sc_ticks_t)0)
typedef __u32 sc_time_t;
typedef __u32 sc_errcode_t;
typedef __u32 sc_extra_t;
typedef __u16 sc_plsize_t;
typedef __u16 sc_modulesize_t;
typedef __u16 sc_bufsize_t;
typedef __u16 sc_msgid_t;
#define SC_MSGID_BASE    0x8000
#define MSGID_16BIT      1
typedef __u16 sc_pid_t;
#define SC_PID_MAX       0x00ffU
#define SC_PID_MSK       0x00ffU
#define SC_PID_INC       0x0100U
#define SC_ILLEGAL_PID   0x7fffU
#define SC_PID_TMO_MAGIC 0xFffeU
#define PID_EXTERN_FLAG  0x8000U
#define SC_CURRENT_PID   ((sc_pid_t)0xffffU)

typedef __u16 sc_poolid_t;
typedef __u16 sc_moduleid_t;
#define MODULE_MASK      0x7000
#define MODULE_SHIFT     (12)
typedef sc_ticks_t  sc_prio_t;
typedef __u16 sc_proc_type_t;
typedef __u16 sc_cpuflags_t;
typedef __u16 sc_tag_t;
typedef __u16 sc_var_t;
typedef int sc_triggerval_t;
typedef __u16 __ptrsize_t;
#else /* __ASM_INCLUDED__ */

#define SC_TMO_MAX       (0xff00)
#define SC_ENDLESS_TMO   (0xffff)
#define SC_FATAL_IF_TMO  (SC_ENDLESS_TMO-1)
#define SC_NO_TMO        (0)

#define MSGID_16BIT      1
#define SC_MSGID_BASE    0x8000

#define SC_PID_MAX       0x00ffUL
#define SC_PID_MSK       0x00ffUL
#define SC_PID_INC       0x0100UL
#define SC_ILLEGAL_PID   0x7fff
#define SC_PID_TMO_MAGIC 0xfffe
#define PID_EXTERN_FLAG  0x8000
#define SC_CURRENT_PID   0xffff

#define MODULE_MASK      0x7000
#define MODULE_SHIFT     (12)

#endif /* __ASM_INCLUDED__ */

#endif
