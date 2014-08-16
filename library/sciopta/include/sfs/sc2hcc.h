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
**              (c) 2008 SCIOPTA Systems AG/ Schweiz                **
**                                                                  **
**********************************************************************
** ID: S07192BS3                                                    **
** +Revision: 1.3 +                                                 **
** +Date: 2008/02/14 14:15:49 +                                     **
** Sciopta -> HCC wrapper (interface)                               **
**********************************************************************
EOC*/
#ifndef _SC2HCC_H_
#define _SC2HCC_H_
/*
** Replaces HCC's f_enterFS
*/
int sfs_enterFATFS(const char *fs);
int sfs_enterEFFS(const char *fs);

/*
** Registers filesystems with devman for usage with SDD functions
*/
void effs_process(const char *fsName);

/*
** Registers filesystems with devman for usage with SDD functions
*/
void fatfs_process(const char *fsName);

#endif /* _SC2HCC_H_ */
