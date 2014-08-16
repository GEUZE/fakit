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
** +Revision: 1.2 +                                                **
** +Date: 2008/01/16 13:02:10 +                                     **
** fifo primitives                                                  **
**********************************************************************
EOC*/

#ifndef _UTIL_CONTAINER_FIFO_H_
#define _UTIL_CONTAINER_FIFO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <container/container.h>

  typedef struct fifo_ops fifo_ops_t;
  typedef struct fifo fifo_t;
  typedef struct fifo_entry fifo_entry_t;

// delete handler 
  typedef void (*fifo_hdlDel) (void NEARPTR entry);

#define FIFO_OPS(hook_t, entry_t) \
    CONTAINER_OPS(hook_t, entry_t)

#define FIFO_PROPS    \
    int noOfItems;    \
    fifo_entry_t NEARPTR first; \
    fifo_entry_t NEARPTR last; \
    fifo_hdlDel del;   \
    sc_poolid_t plid;  \
    sc_ticks_t tmo

  struct fifo_entry {
    sc_msgid_t id;
    void NEARPTR entry;
    fifo_entry_t NEARPTR next;
  };

  struct fifo {
    sc_msgid_t id;
    const fifo_ops_t *ops;
    FIFO_PROPS;
  };

  struct fifo_ops {
    sc_msgid_t magic;
    FIFO_OPS (fifo_t, void);
  };

  fifo_t NEARPTR fifo_new (fifo_hdlDel, sc_poolid_t, sc_ticks_t);

#ifdef __cplusplus
}
#endif
#endif				/* _UTIL_CONTAINER_FIFO_H  */
