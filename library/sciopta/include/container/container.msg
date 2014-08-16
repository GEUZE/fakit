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
** +Revision: 1.3 +                                                **
** +Date: 2005/08/29 06:52:26 +                                     **
** container primitives                                             **
**********************************************************************
EOC*/

#ifndef _UTIL_CONTAINER_MSG_
#define _UTIL_CONTAINER_MSG_
#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/defines.h>

#define CONTAINER_LIST_HOOK	CONTAINER_MSG_BASE
#define CONTAINER_LIST_ENTRY	CONTAINER_MSG_BASE + 1
#define CONTAINER_ARRAY_HOOK	CONTAINER_MSG_BASE + 2
#define CONTAINER_ARRAY_ENTRY	CONTAINER_MSG_BASE + 3
#define CONTAINER_HASH_HOOK	CONTAINER_MSG_BASE + 4
#define CONTAINER_HASH_ENTRY	CONTAINER_MSG_BASE + 5
#define CONTAINER_TREE_HOOK	CONTAINER_MSG_BASE + 6
#define CONTAINER_TREE_ENTRY	CONTAINER_MSG_BASE + 7
#define CONTAINER_STACK_HOOK	CONTAINER_MSG_BASE + 8
#define CONTAINER_STACK_ENTRY	CONTAINER_MSG_BASE + 9
#define CONTAINER_SWQ_HOOK	CONTAINER_MSG_BASE + 10
#define CONTAINER_SWQ_ENTRY	CONTAINER_MSG_BASE + 11
#define CONTAINER_FIFO_HOOK	CONTAINER_MSG_BASE + 12
#define CONTAINER_FIFO_ENTRY	CONTAINER_MSG_BASE + 13

#ifdef __cplusplus
}
#endif
#endif				/* _UTIL_CONTAINER_H */
