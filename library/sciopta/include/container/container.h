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
** ID: S03070CL87                                                   **
** +Revision: 1.16 +                                                **
** +Date: 2006/03/17 06:35:55 +                                     **
** container primitives                                             **
**********************************************************************
EOC*/

#ifndef _UTIL_CONTAINER_H_
#define _UTIL_CONTAINER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <container/container.msg>

  typedef struct container_ops container_ops_t;
  typedef struct container container_t;

#define CONTAINER_OPS(hook_t, entry_t) \
    void 		(*destroy) (hook_t NEARPTR NEARPTR self); \
    /* add an entry, if key allready exist drop it */ \
    entry_t NEARPTR (*add) (hook_t NEARPTR self, entry_t NEARPTR entry); \
    /* remove an entry with same key as reference */ \
    entry_t NEARPTR (*rm) (hook_t NEARPTR self, const entry_t NEARPTR reference); \
    /* get entry with same key as reference */ \
    entry_t NEARPTR (*get) (const hook_t NEARPTR self, const entry_t NEARPTR reference); \
    /* get first entry */ \
    entry_t NEARPTR (*getFirst) (const hook_t NEARPTR self); \
    /* get next entry compared to reference, if no one left return NULL */ \
    entry_t NEARPTR (*getNext) (const hook_t NEARPTR self, const entry_t NEARPTR reference); \
    /* get prev entry compared to reference, if no one left return NULL */ \
    entry_t NEARPTR (*getPrev) (const hook_t NEARPTR self, const entry_t NEARPTR reference); \
    /* get last entry */ \
    entry_t NEARPTR (*getLast) (const hook_t NEARPTR self); \
    /* get entry count */ \
    int 		(*noOfItems) (const hook_t NEARPTR self); \
    entry_t NEARPTR (*getCmp) (const hook_t NEARPTR self, \
                               const entry_t NEARPTR ref,\
                               int (*cmp)(const void NEARPTR,const void NEARPTR)) \
    /* get first entry but use alternate compare function*/ 

  struct container {
    sc_msgid_t id;
    container_ops_t *ops;
  };

  struct container_ops {
    sc_msgid_t magic;
      CONTAINER_OPS (container_t, void);
  };

#define CNT_OPS(container) ((container)->ops)

#ifdef __cplusplus
}
#endif
#endif				/* _UTIL_CONTAINER_H */
