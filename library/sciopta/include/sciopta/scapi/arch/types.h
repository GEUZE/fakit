/*SOC
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
** (c) 2006 Sciopta Systems GmbH Deutschland, Sciopta AG/ Schweiz   **
**                                                                  **
**********************************************************************
** ID: S06055BS1                                                    **
** +Revision: 1.6 +                                                 **
** +Date: 2006/03/09 08:19:35 +                                     **
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

#define NEARPTR *
#define FARPTR *

#define NEAR_NULL (NULL)

/*
** Basic types 
*/
typedef unsigned char  __u8;
typedef signed char    __s8;
typedef unsigned short __u16;
typedef signed short   __s16;
typedef unsigned long  __u32;
typedef signed long    __s32;

typedef unsigned int   __uint;
typedef signed int     __sint;
/*
** Simple sciopta types
*/
typedef __u32 sc_msgid_t;
typedef __u32 sc_pid_t;
typedef __u32 sc_ticks_t;
typedef __u32 sc_time_t;
typedef __u32 sc_bufsize_t;
typedef __u32 sc_prio_t;
typedef __u32 sc_poolid_t;
typedef __u32 sc_errcode_t;
typedef __uint sc_flags_t;
typedef __u32 sc_plsize_t;
typedef __u32 sc_moduleid_t;
typedef __u32 sc_tmoid_t;
typedef __u32 sc_modulesize_t;
//->typedef __u32 flags_t;
#ifdef _M_X64
typedef __u64 __ptrsize_t;
#else
typedef __u32 __ptrsize_t;
#endif
typedef __ptrsize_t sc_extra_t;

/*
** message types 
*/
union sc_msg;
typedef union sc_msg * sc_msg_t;
typedef union sc_msg ** sc_msgptr_t;


#define SC_FATAL_IF_TMO (sc_ticks_t)-2
#define SC_ENDLESS_TMO  (sc_ticks_t)-1
#define SC_NO_TMO       (sc_ticks_t)0
#define SC_TMO_MAX      (sc_ticks_t)0x00ffffff
#define SC_ILLEGAL_PID  (sc_pid_t)0x7fffffff
#define SC_CURRENT_PID   ((sc_pid_t)-1)
#endif /* _ARCH_TYPES_H_ */
