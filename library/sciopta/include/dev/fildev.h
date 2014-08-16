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
** ID: S03024CL3                                                    **
** +Revision: 1.5 +                                                 **
** +Date: 2003/04/23 13:46:42 +                                     **
** file device                                                      **
**********************************************************************
EOC*/

#ifndef _DEV_FILDEV_H_
#define _DEV_FILDEV_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

  void fildev (const char *name, __u8 *memory, size_t size, size_t cursize);
  
#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: fildev.h,v $
** Revision 1.5  2003/04/23 13:46:42  christian
** Bugfix: fildev needs max size and cur size parameter.
**
** Revision 1.4  2003/03/21 14:50:17  christian
** Bugfixed everthing around opendir, readdir, closedir.
** Manager: get next, get prev, ... has wrong message ids.
**
** Revision 1.3  2003/03/11 14:58:30  bastian
** added header
**
** Revision 1.2  2002/11/19 15:56:20  christian
** Read, write implemented.
**
** Revision 1.1.1.1  2002/11/19 15:28:26  christian
** first revision
**
** Revision 1.1  2002/11/18 15:49:42  christian
** Started with pap.
** Linux like log.
**
**
**/
