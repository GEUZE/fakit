/* -*-c++-*-
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
** (c) 2002,2003 Sciopta GmbH, Litronic AG/ Schweiz                 **
**                                                                  **
**********************************************************************
**                                                                  **
** ID: S01222BS6                                                    **
** +Date: 2003/10/23 11:41:46 +                                     ** 
** +Revision: 2.2 +                                                 **
** usec delay                                                       **
**********************************************************************
*/
#ifndef _MACHINE_I386_DELAY_H_
#define _MACHINE_I386_DELAY_H_
#ifdef __cplusplus
extern "C" {
#endif
void delay(unsigned long milliseconds);
unsigned long calibrateDelayLoop(void);
#ifdef __cplusplus
}
#endif
#endif
