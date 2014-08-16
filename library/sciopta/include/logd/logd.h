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
** ID: S02213CL1                                                    **
** +Revision: 1.13 +                                                **
** +Date: 2006/05/23 14:29:26 +                                     **
** logd interface                                                   **
**********************************************************************
EOC*/

#ifndef _UTIL_LOGD_H_
#define _UTIL_LOGD_H_

typedef struct logd_s {
  sc_msgid_t	id;
  sc_poolid_t	plid;
  sc_ticks_t	tmo;
  sc_pid_t	logdPid;
  int		levelMax; /* obsolete, levft for compatiblity */
  char		appl[SC_NAME_MAX];
} logd_t;

#define LOGD_SEVERE	0
#define LOGD_WARNING	1
#define LOGD_INFO	2
#define LOGD_CONFIG	3
#define LOGD_FINE	4
#define LOGD_FINER	5
#define LOGD_FINEST	6
#define LOGD_ALL	6	

EXTERN_C_BEGIN
logd_t NEARPTR logd_new (const char *logdProcessName, 
			 int levelMax, /* obsolete, levft for compatiblity */
			 const char *appl, 
			 sc_poolid_t plid, 
			 sc_ticks_t tmo); 

void logd_printf (logd_t NEARPTR self, 
		  unsigned int level, 
		  const char *fmt, ...);

void logd_levelSet(const char *logdProcessName, unsigned int level);

void logd_destroy (logd_t NEARPTR NEARPTR self);  
EXTERN_C_END

#endif
