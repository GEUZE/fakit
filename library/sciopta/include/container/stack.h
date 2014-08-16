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
** ID: S03070CL86                                                   **
** +Revision: 1.6 +                                                **
** +Date: 2005/01/20 15:20:12 +                                     **
** stack primitives                                                 **
**********************************************************************
EOC*/

#ifndef _UTIL_STACK_H_
#define _UTIL_STACK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <container/container.msg>
  
  typedef struct stack_ops stack_ops_t;
  typedef struct stack sc_stack_t;

  typedef void (*stack_hdlDel) (void *entry);

#define STACK_OPS(hook_t, entry_t) \
    void 		(*destroy) (hook_t NEARPTR NEARPTR self); \
    /* add an entry, if key allready exist drop it */ \
    entry_t *		(*push) (hook_t * self, entry_t * entry); \
    /* remove an entry with same key as reference */ \
    entry_t *		(*pop) (hook_t * self); \
    /* get entry count */ \
    int 		(*noOfItems) (hook_t * self)

#define STACK_PROPS \
    SC_OBJECT_PROPS; \
    stack_hdlDel del; \
    stack_entry_t *first; \
    sc_poolid_t plid; \
    int noOfItems; \
    sc_ticks_t tmo

  typedef struct stack_entry {
    void *entry;
    struct stack_entry *next;
  } stack_entry_t;

  struct stack {
    sc_msgid_t id;
    stack_ops_t *ops;
      STACK_PROPS;
  };

  struct stack_ops {
    sc_msgid_t magic;
    sc_objectOps_t *super;
      STACK_OPS (sc_stack_t, void);
  };

  sc_stack_t * stack_new (stack_hdlDel hdlDel, sc_poolid_t plid, sc_ticks_t tmo);

#ifdef __cplusplus
}
#endif
#endif				/* _UTIL_STACK_H */

