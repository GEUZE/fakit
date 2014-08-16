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
** (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
**                                                                  **
**********************************************************************
** ID: S03217CL7                                                    **
** +Revision: 1.4 +                                                 **
** +Date: 2003/08/05 06:01:20 +                                     **
** sfs shell commands                                               **
**********************************************************************
EOC*/

#ifndef _SFS_MOUNT_H
#define _SFS_MOUNT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/defines.h>
#include <ossys/types.h>

  int setup_root (const char *spezialfile, const char *dirname, 
      		  const char *filesystemtype, unsigned long flags);

  int mount (const char *specialfile, const char *dir, 
	     const char *filesystemtype, unsigned long flags, 
	     const void *data);

  int umount (const char *dir);

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: mount.h,v $
** Revision 1.4  2003/08/05 06:01:20  bastian
** Added header
**
** Revision 1.3  2003/03/19 15:57:56  christian
** Mount and setup_root has now the same parameter succession.
**
** Revision 1.2  2002/12/06 14:31:05  christian
** EFS is now SFS and efs is now sfs
**
** Revision 1.1.1.1  2002/12/06 10:54:27  christian
** first revision based on new sdd
**
** Revision 1.1  2002/11/15 14:31:07  christian
** Reimplementation of the embedded file system based on the new sciopta
** device driver conception (new "bios").
**
** Revision 1.3  2002/04/17 09:51:01  christian
** Forced the mount to work. Not _that_ generic solution :((
**
** Revision 1.2  2002/04/12 14:43:22  christian
** dot and dotdot added to the new directories.
**
** Revision 1.1  2002/03/20 10:25:55  christian
** Moved mount.h
** Added stat.h
** Modified fs.h
**
** Revision 1.4  2002/03/15 10:24:04  christian
** Add mount methode.
**
** Revision 1.3  2002/03/15 09:56:09  christian
** Added chroot
**
** Revision 1.2  2002/03/14 16:21:29  christian
** Have now a base ready for testing...
**
** Revision 1.1  2002/03/13 13:49:57  christian
** Intermediate.
**
**
**
**/
