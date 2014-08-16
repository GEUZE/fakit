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
** (c) 2007 Sciopta Systems GmbH Deutschland, Sciopta AG/ Schweiz   **
**                                                                  **
**********************************************************************
** ID: S07065BS1                                                    **
** +Revision: 1.2 +                                                 **
** +Date: 2008/01/16 13:01:14 +                                     **
** Header for fast memcpy                                           **
**********************************************************************
EOC*/
#ifndef _MEMCOPY_H_
#define _MEMCOPY_H_

#include <sciopta.h>

EXTERN_C_BEGIN
/* Copy N bytes from a 32-bit aligned buffer to a 16-bit aligned buffer. */
void MEMCOPY_L2S_BY4 (void *pBUFOUT, void *pBUFIN, __u32 nBytes);

/* Copy N bytes from a 32-bit aligned buffer to a 32-bit aligned buffer. */
void MEMCOPY_L2L_BY4 (void *pBUFOUT, void *pBUFIN, __u32 nBytes);

/* Copy N bytes from a 32-bit aligned buffer to a 16-bit aligned buffer. */
void MEMCOPY_S2L_BY4 (void *pBUFOUT, void *pBUFIN, __u32 nBytes);

EXTERN_C_END
#endif /* _MEMCOPY_H_ */
