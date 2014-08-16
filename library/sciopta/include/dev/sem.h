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
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S04062CL1 
** +Revision: 1.7 +
** +Date: 2008/01/16 13:02:00 +
** Semaphore simulation for sciopta 
**********************************************************************
EOC*/


#ifndef _UTIL_SEM_H_
#define _UTIL_SEM_H_ 1

#include <sciopta.h>
#include <dev/sem.msg>
#include <sdd/sdd.msg>

EXTERN_C_BEGIN
/** 
 * Semaphore process, could be started with any name. Multiple semaphore 
 * processes are also possible.
 */
SC_PROCESS (SCP_sem);

/** 
 * Create a new named semaphore.
 * param[in]  name       Name for this semaphore object.
 * param[in]  value      Initial semaphore value.
 * param[in]  semProc    Semaphore Process (eighter absolut or relative).
 * param[in]  plid       Pool id to alloc the requests.
 * return                A semaphore object.
 */
sdd_obj_t NEARPTR sem_new (const char *name, int value, const char *semProc, 
			   sc_poolid_t plid);

/** 
 * Get a named semaphore.
 * param[in]  name       Name for this semaphore object.
 * param[in]  semProc    Semaphore Process (eighter absolut or relative).
 * param[in]  plid       Pool id to alloc the requests.
 * return                A semaphore object.
 */
sdd_obj_t NEARPTR sem_get (const char *name, const char *semProc, 
			   sc_poolid_t plid);

/** 
 * Remove a semaphore.
 * param[in]  self       Semaphore object.
 * return                -1 on error see sc_miscErrnoGet () for more details.
 */
int sem_rm (sdd_obj_t NEARPTR NEARPTR self);

/** 
 * Set a semaphore to a value.
 * param[in]  self       Semaphore object.
 * param[in]  value      Initial semaphore value.
 * return                -1 on error see sc_miscErrnoGet () for more details.
 */
int sem_set (const sdd_obj_t NEARPTR self, int value);

/** 
 * Increment a semaphore. Leaving a critical section.
 * param[in]  self       Semaphore object.
 * param[in]  value      Increment step.
 * return                -1 on error see sc_miscErrnoGet () for more details.
 */
int sem_incr (const sdd_obj_t NEARPTR self, int value);

/** 
 * Decrement a semphore. Enter a critical section.
 * param[in]  self       Semaphore object.
 * param[in]  value      Decrement step.
 * return                -1 on error see sc_miscErrnoGet () for more details.
 */
int sem_decr (const sdd_obj_t NEARPTR self, int value);

/** 
 * Decrement a semphore. Enter a critical section.
 * param[in]  self       Semaphore object.
 * param[in]  value      Decrement step.
 * param[in]  tmo        timeout in ticks
 * return                -1 on error see sc_miscErrnoGet () for more details.
 *                       ETIME == timeout expired
 */
int sem_decr_w_tmo (const sdd_obj_t NEARPTR self, int value, sc_ticks_t tmo);

/**
 * Read a semaphore value.
 * param[in]  self       Semaphore object.
 * param[out] value      Current semaphore value.
 * return                -1 on error see sc_miscErrnoGet () for more details.
 */
int sem_read (const sdd_obj_t NEARPTR self, int *value);

EXTERN_C_END 

#endif /* _UTIL_SEM_H_ */

