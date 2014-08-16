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
** (c) 2006 Sciopta Systems GmbH Deutschland, Sciopta AG/ Schweiz   **
**                                                                  **
**********************************************************************
** ID: S06061BS1                                                    **
** +Revision: 1.4 +                                                 **
** +Date: 2006/11/23 15:09:37 +                                     **
** Host OS depending definitions                                    **
**********************************************************************
EOC*/

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef WIN32_EXPORTS
#define DLLEXP __declspec(dllexport)
#define DLLIMP __declspec(dllimport)
#elif defined WIN32_IMPORTS
#define DLLEXP __declspec(dllimport)
#else
#define DLLEXP /*empty*/
#endif
#else
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#define DLLEXP /*empty*/
#endif /* WIN32 */
