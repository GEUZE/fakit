/*SOC -*-c-*-
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
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S04062CL1 
** +Revision: 1.3 +
** +Date: 2008/01/16 13:02:00 +
** Semaphore simulation for sciopta 
**********************************************************************
EOC*/


#ifndef _UTIL_SEM_MSG_
#define _UTIL_SEM_MSG_ 1


#include <sciopta.h>
#include <sdd/sdd.msg>

EXTERN_C_BEGIN 

#define SEM_SET	(SEM_MSG_BASE)
#define SEM_SET_REPLY (SEM_SET + 1)
typedef struct sem_set_s {
  sdd_baseMessage_t base;
  int value;
} sem_set_t;
  
#define SEM_INCR	(SEM_MSG_BASE + 2)
#define SEM_INCR_REPLY	(SEM_INCR + 1)
typedef struct sem_incr_s {
  sdd_baseMessage_t base;
  int value;
} sem_incr_t;
  
#define SEM_DECR	(SEM_MSG_BASE + 4)
#define SEM_DECR_REPLY	(SEM_DECR + 1)
typedef struct sem_decr_s {
  sdd_baseMessage_t base;
  int value;
  int valueSave;
} sem_decr_t;
  
#define SEM_READ	(SEM_MSG_BASE + 6)
#define SEM_READ_REPLY	(SEM_READ + 1)
typedef struct sem_read_s {
  sdd_baseMessage_t base;
  int value;
} sem_read_t;

#define SEM_DECR_W_TMO	(SEM_MSG_BASE + 8)
#define SEM_DECR_W_TMO_REPLY	(SEM_DECR_W_TMO + 1)
typedef struct sem_decr_wtmo_s {
  sdd_baseMessage_t base;
  int value;
  int valueSave;
  sc_ticks_t tmo;
} sem_decr_wtmo_t;
  
#define SEM_TMO	(SEM_MSG_BASE + 10)
/* Message structure private */

EXTERN_C_END

#endif /* _UTIL_SEM_H_ */

