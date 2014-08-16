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
** +Revision: 1.4 +                                                **
** +Date: 2008/01/16 13:02:09 +                                     **
** array primitives                                                 **
**********************************************************************
EOC*/

#ifndef _UTIL_CONTAINER_ARRAY_H_
#define _UTIL_CONTAINER_ARRAY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <container/container.h>

  typedef struct array_ops_s array_ops_t;
  typedef struct array_s array_t;

// delete handler 
  typedef void (*array_hdlDel) (void NEARPTR entry);

// 0 if left == right, -1 if left < right, 1 if left > right
  typedef int (*array_hdlCmp) (const void NEARPTR left, 
			       const void NEARPTR right);

#define ARRAY_OPS(hook_t, entry_t) \
    CONTAINER_OPS(hook_t, entry_t)

#define ARRAY_PROPS \
    array_hdlCmp cmp; \
    array_hdlDel del; \
    size_t size

  struct array_s {
    sc_msgid_t id;
    const array_ops_t *ops;
    ARRAY_PROPS;
  };

  struct array_ops_s {
    sc_msgid_t magic;
    ARRAY_OPS (array_t, void);
  };

  array_t NEARPTR array_new (array_hdlCmp, array_hdlDel, size_t, sc_poolid_t,
                             sc_ticks_t);
  void array_init (array_t NEARPTR, array_hdlCmp, array_hdlDel, size_t);

#ifdef __cplusplus
}
#endif
#endif                          /* _UTIL_CONTAINER_ARRAY_H  */
