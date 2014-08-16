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
** (c) 2006 Sciopta Systems GmbH Deutschland, Sciopta AG/ Schweiz   **
**                                                                  **
**********************************************************************
** ID: S06055BS2                                                    **
** +Revision: 1.5 +                                                 **
** +Date: 2006/03/09 14:26:10 +                                     **
** Static kernel configuration                                      **
**********************************************************************
EOC*/

#ifndef _ARCH_OS_H_
#define _ARCH_OS_H_ 1

#ifndef SCIOPTA
#define SCIOPTA       WIN32_HOSTED
#define CPU           I386
#define CPUTYPE       (-1)
#define SC_SCALE SC_MAXIMUM
#endif

#ifndef _SC_CONF_H_
#define _SC_CONF_H_ 42


#define SC_MSG_STAT            1   /* ( 0 | 1 ) */
#define SC_PROC_STAT           1   /* ( 0 | 1 ) */
/*
** pool hooks
*/
#define SC_POOL_HOOK           0   /* ( 0 | 1 ) */
#define SC_POOLCREATE_HOOK     0   /* ( 0 | 1 ) */
#define SC_POOLKILL_HOOK       0   /* ( 0 | 1 ) */
/*
** message hooks
*/
#define SC_MSG_HOOK            0   /* ( 0 | 1 ) */
#define SC_MSGTX_HOOK          0   /* ( 0 | 1 ) */
#define SC_MSGRX_HOOK          0   /* ( 0 | 1 ) */
/*
** process hooks
*/
#define SC_PROC_HOOK           0   /* ( 0 | 1 ) */
#define SC_PROCCREATE_HOOK     0   /* ( 0 | 1 ) */
#define SC_PROCKILL_HOOK       0   /* ( 0 | 1 ) */
#define SC_PROCSWAP_HOOK       0   /* ( 0 | 1 ) */
/*
** error hook
*/
#define SC_ERR_HOOK            1   /* ( 0 | 1 ) */

/*
** design constants
*/
/* 0 means, no connector resp. no pool
** 1 means, support for connectors resp. pool
** Number is runtime selectable
*/
#define SC_MAX_CONNECTOR       1   /* ( 0 | 1 ) */
#define SC_MAX_POOL            1   /* ( 0 | 1 ) */

#define SC_MAX_NUM_BUFFERSIZES 8   /* 4,8 or 16  */

/* Fix, ATM no module support */
#define SC_MAX_MODULE          1   /* 1 */

/*
** error checks
*/
#define SC_MSG_CHECK           0   /* ( 0 | 1 ) */
#define SC_MSG_PARA_CHECK      1   /* ( 0 | 1 ) */
#define SC_POOL_PARA_CHECK     1   /* ( 0 | 1 ) */
#define SC_PROC_PARA_CHECK     0   /* ( 0 | 1 ) */


#define SC_USE_SYSCALL         0   /* ( 0 | 1 ) */

#ifdef SC_CDEBUG
#define SC_CDEBUG              1   /* ( 0 | 1 ) */
#else
#define SC_CDEBUG              0   /* ( 0 | 1 ) */
#endif

#endif /* _SC_CONF_H_ */

#ifdef __GNUC__
#define INLINE __inline__
#define STATIC_INLINE static __inline__
#else
#define INLINE inline
#define STATIC_INLINE __inline static
#endif

#endif /* _ARCH_OS_H_ */


