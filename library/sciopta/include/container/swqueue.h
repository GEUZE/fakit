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
** ID: S03070CL85                                                   **
** +Revision: 1.15 +                                                **
** +Date: 2005/01/20 15:20:12 +                                     **
** queue primitives                                                 **
**********************************************************************
EOC*/

#ifndef _UTIL_CONTAINER_SWQUEUE_H_
#define _UTIL_CONTAINER_SWQUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <sciopta.h>
#include <ossys/types.h>
#include <container/container.msg>

  typedef struct swqueue swqueue_t;
  typedef struct swq_entry swq_entry_t;
  struct swq_entry {
    sc_msgid_t id;
    char *buf;
    __u32 seq;
    size_t len;
    sc_msg_t msg;
  };

#define NULL_SWQ ((swq_entry_t NEARPTR )0)
  swqueue_t NEARPTR swq_new (__u32 windowStart,sc_poolid_t plid, sc_ticks_t tmo);
  swq_entry_t NEARPTR swq_entryInit (swq_entry_t NEARPTR e, 
				   swqueue_t NEARPTR self, char *buf,
				   size_t len, sc_msg_t msg);
  swq_entry_t NEARPTR swq_entryNew (swqueue_t NEARPTR self, char *buf, size_t len,
                             sc_msg_t msg);
  void swq_entryFree (swq_entry_t NEARPTR NEARPTR e);
  int swq_add (swqueue_t NEARPTR hook, swq_entry_t NEARPTR e);
  swq_entry_t NEARPTR swq_get (swqueue_t NEARPTR hook, __u32 window, __u32 maxLen);
  swq_entry_t NEARPTR swq_getLast (swqueue_t NEARPTR hook);
  swq_entry_t NEARPTR swq_free (swqueue_t NEARPTR hook, __u32 ack);
  void swq_incrSeq (swqueue_t NEARPTR hook, size_t size);
  int swq_incrAck (swqueue_t NEARPTR hook, __u32 amount);
  int swq_checkAck (swqueue_t NEARPTR hook, __u32 ack);
  int swq_lastAckEqSeq (swqueue_t NEARPTR hook, __u32 seq);
  int swq_lastAckInRange (swqueue_t NEARPTR hook, __u32 lower, __u32 upper);
  __s32 swq_reset (swqueue_t NEARPTR hook, __u32 * seq);
  void swq_destroy (swqueue_t NEARPTR NEARPTR hook);


#ifdef __cplusplus
}
#endif
#endif                          /* _UTIL_CONTAINER_SWQUEUE_H */
