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
**              (c) 2009 SCIOPTA Systems AG/ Schweiz                **
**                                                                  **
**********************************************************************
** ID: S09142BS4                                                    **
** +Revision: 1.1.2.1 +                                                 **
** +Date: 2009/05/22 08:52:59 +                                     **
** Register/Unregister device functions                             **
**********************************************************************
EOC*/
#ifndef _REGISTERDEV_H_
#define _REGISTERDEV_H_
void
registerDev (const char *name, logd_t NEARPTR logd, sc_pid_t driver, void *handle);
void unregisterDev(const char * name, logd_t *logd);

#endif /* _REGISTERDEV_H_ */
