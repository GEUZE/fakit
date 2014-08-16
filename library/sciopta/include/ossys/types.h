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
** ID: S03076BS14                                                   **
** +Revision: 2.37 +                                                **
** +Date: 2008/01/16 13:02:02 +                                     **
** System wide typedefinitions                                      **
**********************************************************************
EOC*/
/*
*****************************
** Log moved to end !
*****************************
*/

#ifndef _OSSYS_TYPES_H_
#define _OSSYS_TYPES_H_

#ifdef __GNUC__
#define PACKED __attribute__((packed))
#else
#define PACKED
#endif

#include <arch/types.h>
#include <ossys/defines.h>

#ifndef __ASM_INCLUDED__

#if defined __GNUC__ || defined _WIN32 
#include <sys/types.h>
typedef __u16 umode_t;
#if defined _WIN32 
typedef int ssize_t;
typedef __u16 mode_t;
typedef __u32 nlink_t;
typedef sc_pid_t pid_t; 
typedef sc_pid_t uid_t; 
typedef sc_pid_t gid_t; 
#endif
#if defined __MINGW32_VERSION
// MINGW lacks those types
typedef __u32 nlink_t;
typedef sc_pid_t uid_t; 
typedef sc_pid_t gid_t; 
#endif

#else

typedef unsigned int dev_t;
typedef long fd_t;
typedef __u16 umode_t;  
typedef __u16 mode_t;  
typedef __u16 fd_set;  
typedef __u32 ino_t;   
typedef __u32 nlink_t;
#ifndef _OFF_T_DEF
#define _OFF_T_DEF /* Tasking */
typedef __s32 off_t; 
#endif
typedef sc_pid_t pid_t; 
typedef sc_pid_t uid_t; 
typedef sc_pid_t gid_t; 
typedef void * daddr_t; 
typedef __u16 key_t; 
typedef __u32 suseconds_t; 
typedef __u32 loff_t;
#if !defined(_TIME_T) && !defined(_TIME_T_DEFINED)
#define _TIME_T  1
#define _TIME_T_DEFINED 1
typedef __u32 time_t; 
#endif
#ifndef _CLOCK_T
#define _CLOCK_T 1
typedef __u32 clock_t; 
#endif
typedef __u32 caddr_t; 
typedef __u32 fsid_t;
#if defined(NC30) && !defined(__SIZET_16__)
typedef long ssize_t;
#else
typedef int ssize_t;
#endif
#if !defined __SIZE_TYPE && !defined __SIZE_T_TYPE__ && !defined _SIZE_T_DEF && !defined __SIZE_TYPE__ && !defined __SIZE_T__ && !defined __SIZE_T && !defined _MSL_SIZE_T_DEFINED

#define _MSL_SIZE_T_DEFINED
#define __SIZE_T /* ImageCraft */
#if defined(NC30) && !defined(__SIZET_16__)
#define __SIZE_TYPE__ unsigned long
#else
#define __SIZE_TYPE__ unsigned int
#endif
#define _SIZE_T
#define __SIZE_T__  /* cosmic */
#define _SIZE_T_DEF /* tasking C166/m16c , NC30*/
#if defined(__MWERKS__) && defined(__cplusplus)
namespace std { typedef __SIZE_TYPE__ size_t;}
#else
typedef __SIZE_TYPE__ size_t;
#endif
#endif			 
#endif
typedef sc_msgid_t magic_t;
typedef unsigned int flags_t;

#define __VU32__
typedef volatile __u32 __vu32;
typedef volatile __u16 __vu16;
typedef volatile __u8  __vu8;
typedef volatile __s32 __vs32;
typedef volatile __s16 __vs16;
typedef volatile __s8  __vs8;

#endif /* __ASM_INCLUDED */
#endif
/*
** $Log: types.h,v $
** Revision 2.37  2008/01/16 13:02:02  bastian
** merge from 1.9.1.12
**
** Revision 2.36.2.1  2007/09/14 07:40:13  bastian
** fixes for nc30
**
** Revision 2.36  2006/12/12 12:40:55  bastian
** fix for win32
**
** Revision 2.35  2006/12/06 11:47:33  bastian
** fixes for scwin32
**
** Revision 2.34  2006/11/23 15:09:37  bastian
** wetzer pre-delivery check-in
**
** Revision 2.33  2006/05/23 14:36:19  bastian
** fixed defines
**
** Revision 2.32  2006/03/02 10:04:45  bastian
** added SCAPI support
**
** Revision 2.31  2006/01/19 13:05:58  bastian
** merge with 1.8.0.2
**
** Revision 2.30.2.1  2005/11/22 13:18:13  bastian
** time_t/clock_t fix for IAR
**
** Revision 2.30  2005/10/07 10:08:09  bastian
** fix for CW 6.0
**
** Revision 2.29  2005/07/01 11:18:25  bastian
** merge from branch-1.7.2
**
** Revision 2.28.4.1  2005/05/12 04:57:02  bastian
** added dev_t
**
** Revision 2.28  2004/10/07 07:37:47  bastian
** fixed NEAR * to NEARPTR
**
** Revision 2.27  2004/10/01 10:38:51  bastian
** fix for tasking
**
** Revision 2.26  2004/09/30 08:34:51  bastian
** fixes for m16c
**
** Revision 2.25  2004/08/20 05:01:40  bastian
** added PACKED macro
**
** Revision 2.24  2004/08/16 06:59:32  bastian
** x
**
** Revision 2.23  2004/07/16 05:28:50  christian
** IOCTL -> TAG.
**
** Revision 2.22  2004/06/09 10:26:40  bastian
** fix
**
** Revision 2.21  2004/06/09 09:01:51  bastian
** addede volatile types (for HW definitions)
**
** Revision 2.20  2003/12/09 10:58:16  bastian
** added MW CF support
**
** Revision 2.19  2003/07/09 09:22:07  bastian
** fix for Cosmic hc12
**
** Revision 2.18  2003/05/23 14:31:41  bastian
** fixes for Tasking
**
** Revision 2.17  2003/05/19 13:34:56  bastian
** fix for cc65
**
** Revision 2.16  2003/03/18 15:28:14  bastian
** off_t is signed !
**
** Revision 2.15  2003/03/17 11:07:50  bastian
** fixed IDs
**
** Revision 2.14  2003/03/17 09:09:16  bastian
** added header
**
** Revision 2.13  2003/03/17 09:06:37  bastian
** simplyfied
**
** Revision 2.12  2003/03/07 10:24:31  bastian
** added m68k support
**
** Revision 2.11  2003/03/06 07:58:20  bastian
** arm port started
**
** Revision 2.10  2003/02/25 11:23:10  bastian
** moved log at end
** added coldfire-gnu
** fixed typo
**
** Revision 2.9  2003/02/25 07:36:42  ralf
** fixed for CF
**
** Revision 2.8  2002/12/04 08:29:54  bastian
** started release-preparing
**
** Revision 2.7  2002/10/10 12:57:32  christian
** .
**
** Revision 2.6  2002/02/04 08:44:54  bastian
** Powerpc kernel added
**
** Revision 2.5  2002/01/18 16:08:38  bastian
** SH support
**
** Revision 2.4  2002/01/17 12:31:22  christian
** Moved the basic_file_t stuff over to the bios.
**
** Revision 2.3  2001/12/17 07:48:54  bastian
** Changed f_owner to make it fit better for Sciopta
**
** Revision 2.2  2001/12/03 13:03:47  christian
** Move the IOCTL commands to the bios.h (much better).
**
** Revision 2.1  2001/12/03 09:27:07  christian
** Standard IOCTL commands added.
**
** Revision 2.0.0.1  2001/11/08 16:09:50  bastian
** moved over to cvs
**
 * 
 *    Rev 1.12   Nov 12 2001 03:25:36   christian
 * minor changes.
 * 
 *    Rev 1.11   10 Sep 2001 13:06:08   skywalker
 * ?
 * 
 *    Rev 1.10   Aug 20 2001 10:40:36   bastian
 * Changed LOCK/UNLOCK macros
 * 
 *    Rev 1.8   Jul 16 2001 14:17:52   bastian
 * merged working version 
 * 
 *    Rev 1.7   06 Jul 2001 16:13:34   christian
 * 
 *    Rev 1.6   13 Jun 2001 11:02:48   christian
 * Add string type (to solve segment problem).
 * 
 *    Rev 1.5   08 Jun 2001 16:32:32   christian
 * add some new types like signal_t
 * 
 *    Rev 1.4   Apr 08 2001 13:18:06   christian
 * changed u8 to __u8, u16 to __u16 and u32 to __u32
 * 
 *    Rev 1.3   May 17 2001 15:02:26   christian
 * fix the size_t bug
 * 
 *    Rev 1.2   May 15 2001 13:43:02   christian
 * first clean revision
 * 
 *    Rev 1.1   15 May 2001 12:22:38   christian
 * change all structs to ##_s and all types to ##_t.
 * 
 *    Rev 1.0   Apr 04 2001 07:29:34   christian
 * Initial revision.
**
*/
