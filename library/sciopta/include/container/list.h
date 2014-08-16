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
** ID: S03070CL84                                                   **
** +Revision: 1.15 +                                                **
** +Date: 2008/01/16 13:02:10 +                                     **
** list primitives                                                  **
**********************************************************************
EOC*/

#ifndef _UTIL_CONTAINER_LIST_H_
#define _UTIL_CONTAINER_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <container/container.h>

  typedef struct list_ops list_ops_t;
  typedef struct list list_t;
  typedef struct list_entry list_entry_t;

// delete handler 
  typedef void (*list_hdlDel) (void NEARPTR entry);

// 0 if left == right, -1 if left < right, 1 if left > right
  typedef int (*list_hdlCmp) (const void NEARPTR left, 
			      const void NEARPTR right);

#define LIST_OPS(hook_t, entry_t) \
    CONTAINER_OPS(hook_t, entry_t)

#define LIST_PROPS    \
    int noOfItems;    \
    list_entry_t NEARPTR first; \
    list_hdlCmp cmp;   \
    list_hdlDel del;   \
    sc_poolid_t plid;  \
    sc_ticks_t tmo

  struct list_entry {
    sc_msgid_t id;
    void NEARPTR entry;
    list_entry_t NEARPTR prev;
    list_entry_t NEARPTR next;
  };

  struct list {
    sc_msgid_t id;
    const list_ops_t *ops;
    LIST_PROPS;
  };

  struct list_ops {
    sc_msgid_t magic;
    LIST_OPS (list_t, void);
  };

  list_t NEARPTR list_new (list_hdlCmp, list_hdlDel, sc_poolid_t, sc_ticks_t);

#ifdef __cplusplus
}
#endif
#endif				/* _UTIL_CONTAINER_LIST_H  */
