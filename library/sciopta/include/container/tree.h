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
** ID: S03070CL83                                                   **
** +Revision: 1.10 +                                                **
** +Date: 2004/10/07 07:37:41 +                                     **
** tree primitives                                                  **
**********************************************************************
EOC*/

#ifndef _UTIL_CONTAINER_TREE_H_
#define _UTIL_CONTAINER_TREE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <container/container.h>

  typedef struct tree_ops tree_ops_t;
  typedef struct tree tree_t;

// delete handler 
  typedef void (*tree_hdlDel) (void NEARPTR entry);

// 0 if left == right, -1 if left < right, 1 if left > right
  typedef int (*tree_hdlCmp) (void NEARPTR left, void NEARPTR right);

#define TREE_OPS(hook_t, entry_t) \
  CONTAINER_OPS(hook_t, entry_t)

#define TREE_PROPS \
  int noOfItems; \
  sc_poolid_t plid; \
  sc_ticks_t tmo; \
  tree_entry_t NEARPTR first; \
  tree_hdlCmp cmp; \
  tree_hdlDel del

  typedef struct tree_entry_s tree_entry_t;

  struct tree_entry_s {
    sc_msgid_t id;
    void NEARPTR entry;
    tree_entry_t NEARPTR left;
    tree_entry_t NEARPTR right;
  };

#define NULL_TREE ((tree_entry_t *)0)
  struct tree {
    sc_msgid_t id;
    tree_ops_t *ops;
      TREE_PROPS;
  };

  struct tree_ops {
    sc_msgid_t magic;
      TREE_OPS (tree_t, void);
  };

  tree_t NEARPTR tree_new (tree_hdlCmp cmp_handler, tree_hdlDel del_handler,
			 sc_poolid_t plid, sc_ticks_t tmo);

#ifdef __cplusplus
}
#endif
#endif				/*  _UTIL_CONTAINER_TREE_H */
