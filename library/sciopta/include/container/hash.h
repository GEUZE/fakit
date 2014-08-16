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
** ID: S03070CL82                                                   **
** +Revision: 1.10 +                                                **
** +Date: 2006/05/10 09:49:14 +                                     **
** hash primitives                                                  **
**********************************************************************
EOC*/

#ifndef _UTIL_CONTAINER_HASH_H_
#define _UTIL_CONTAINER_HASH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <container/container.h>
#include <container/list.h>

  typedef struct hash_ops hash_ops_t;
  typedef struct hash hash_t;

/* hash function handler */
  typedef int (*hash_hdlFunc) (const void *key);

/* delete handler */
  typedef void (*hash_hdlDel) (void *entry);

/* 0 if left == right, -1 if left < right, 1 if left > right */
  typedef int (*hash_hdlCmp) (const void *left, const void *right);

#define HASH_OPS(hook_t, entry_t) \
  CONTAINER_OPS(hook_t, entry_t)

#define HASH_PROPS \
  int noOfItems; \
  hash_hdlFunc function; \
  int maxEntries; \
  int size; \
  int doForAllI; \
  list_t *head[1]

  struct hash {
    sc_msgid_t id;
    hash_ops_t *ops;
      HASH_PROPS;
  };

  struct hash_ops {
    sc_msgid_t magic;
    container_ops_t *super;
      HASH_OPS (hash_t, void);
  };

  hash_t *hash_new (int size, hash_hdlFunc func, hash_hdlCmp cmp,
		    hash_hdlDel del, sc_poolid_t plid, sc_ticks_t tmo);

#ifdef __cplusplus
}
#endif
#endif				/* _UTIL_CONTAINER_HASH_H */
/*
** $Log: hash.h,v $
** Revision 1.10  2006/05/10 09:49:14  bastian
** fixed const qualifier
**
** Revision 1.9  2004/04/23 11:53:51  christian
** Remove the old sciopta object stuff.
**
** Revision 1.8  2004/03/26 12:11:04  christian
** If cmp or del handler are not given (NULL) a default handler will be
** installed.
**
** Revision 1.7  2003/03/11 15:59:21  bastian
** added Header
**
** Revision 1.6  2002/09/05 14:32:15  christian
** Removed manager
**
** Revision 1.5  2002/02/25 12:56:25  christian
** New container is now based on the scobj class. Better type checking.
**
** Revision 1.4  2002/02/18 07:42:25  bastian
** fixed bug
**
** Revision 1.3  2002/01/17 08:31:59  christian
** Fixed PRINTFs new paramter numbering.
**
** Revision 1.2  2002/01/15 12:58:50  christian
** First implementation (linear hashing).
**
**
**
**
**/
