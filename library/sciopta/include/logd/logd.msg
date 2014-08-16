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
** ID: S02213CL2                                                    **
** +Revision: 1.9 +                                                 **
** +Date: 2006/05/23 14:29:26 +                                     **
** logd message interface                                           **
**********************************************************************
EOC*/

#ifndef _UTIL_LOGD_MSG_
#define _UTIL_LOGD_MSG_

#include <ossys/defines.h>

#define LOGD_TEXT_MAX	159

#define LOGD_PRINT		(LOGD_MSG_BASE + 1)
typedef struct logd_print_s {
  sc_msgid_t id;
  unsigned int level;
  sc_time_t time;
  char appl[SC_NAME_MAX + 1];
  char text[LOGD_TEXT_MAX + 1];
} logd_print_t;

#define LODG_DESCRIPTOR        (LOGD_MSG_BASE + 3)
/* type of logd_t defined in logd.h */

#define LOGD_LEVEL_SET         (LOGD_MSG_BASE + 5)
typedef struct logd_level_set_s {
  sc_msgid_t id;
  unsigned int level;
} logd_level_set_t;

SC_PROCESS (SCP_logd);


#endif
