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
** (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
**                                                                  **
**********************************************************************
** ID: S01332CL13                                                   **
** +Revision: 2.10 +                                                **
** +Date: 2004/11/12 15:40:13 +                                     **
** TCP message interface                                            **
**********************************************************************
EOC*/

#ifndef _TCP_MSG_
#define _TCP_MSG_

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/defines.h>
#include <ips/base.msg>

#define TCP_TMO_RESEND	(TCP_MSG_BASE)
#define TCP_TMO_FLAGS_SENT	(TCP_MSG_BASE + 1)
#define TCP_TMO_PIGGYPACK	(TCP_MSG_BASE + 2)
#define TCP_TMO_NAGLE	(TCP_MSG_BASE + 3)
#define TCP_TMO_TIME_WAIT	(TCP_MSG_BASE + 4)
#define TCP_TMO_ZERO_WND	(TCP_MSG_BASE + 5)

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: tcp.msg,v $
** Revision 2.10  2004/11/12 15:40:13  bastian
** fixed PIGGYPACK
**
** Revision 2.9  2004/04/23 06:13:56  christian
** Did rename to make source more readable.
**
** Revision 2.8  2003/03/18 15:04:26  christian
** Cleaned.
**
** Revision 2.7  2003/03/05 16:07:38  christian
** TCP configurable no of connections and packet size.
**
** Revision 2.6  2002/10/14 15:35:52  christian
** Started with send. not finished yet.
**
** Revision 2.5  2002/10/11 15:24:08  christian
** slow path implementation started. zero window probing added.
**
** Revision 2.4  2002/09/30 15:09:30  christian
** Intermediate.
**
** Revision 2.3  2002/09/27 15:00:39  christian
** Correct connection scenario implemented & tested.
**
** Revision 2.2  2002/09/25 14:59:35  christian
** Intermediate.
**
**
**/
