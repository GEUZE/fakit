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
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S04106CL20
** +Revision: 1.16 +
** +Date: 2008/01/16 13:02:09 +
** Unmount directory
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>
#include <string.h>
#include <sys/errno.h>

#include <ossys/defines.h>
#include <ossys/types.h>

#include <sdd/sdd.h>

#include <sfs/sfs.h>
#include <sfs/sfs.msg>
#include <sys/mount.h>

int
umount (const char *dirpath)
{
  fd_tab_t *fd_tab;
  sdd_obj_t NEARPTR mountpoint;
  const char *last;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t *) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC) {

      mountpoint = sdd_objResolve (fd_tab->manager, dirpath, &last);

      if (last[0]) {
	sc_miscErrnoSet (ENOTDIR);
	return -1;
      }
      
      if (!mountpoint || !SDD_IS_A (mountpoint, SFS_DIR_TYPE)) {
	sc_miscErrnoSet (EINVAL);
	return -1;
      }

      sfs_umount (mountpoint);

      return 0;
    }
    else {
      PRINTF ("This is not a SCIO fd_tab \n");
      sc_miscErrnoSet (EINVAL);
      return -1;
    }
  }
  else {
    PRINTF ("There is no SCIO fd_tab installed \n");
    sc_miscErrnoSet (EPERM);
    return -1;
  }
}

/*
** $Log: umount.c,v $
** Revision 1.16  2008/01/16 13:02:09  bastian
** merge from 1.9.1.12
**
** Revision 1.15.12.1  2007/09/07 04:55:21  bastian
** added comments
**
** Revision 1.15  2004/10/07 09:25:28  bastian
** NEAR * => NEARPTR
**
** Revision 1.14  2004/10/04 13:33:58  bastian
** fix for NEAR/FAR
**
** Revision 1.13  2004/04/14 14:06:54  christian
** Cleanup namings.
**
** Revision 1.12  2003/11/10 13:38:24  christian
** Short sfs_directory to sfs_dir.
**
** Revision 1.11  2003/11/10 12:39:26  christian
** Shorter name for DIRECTORY to DIR.
**
** Revision 1.10  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 1.9  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 1.8  2003/08/11 10:59:22  bastian
** fixed procVar parameter cast
**
** Revision 1.7  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 1.6  2003/04/09 15:31:09  christian
** Bugfixes.
**
** Revision 1.5  2003/04/08 08:31:20  christian
** Umount is working.
**
** Revision 1.4  2003/03/27 15:13:41  christian
** Mount is working, umount is not.
**
** Revision 1.3  2003/03/18 07:30:38  bastian
** added ARM and fixed for ADS
**
** Revision 1.2  2002/12/06 14:31:05  christian
** EFS is now SFS and efs is now sfs
**
** Revision 1.1.1.1  2002/12/06 10:54:27  christian
** first revision based on new sdd
**
** Revision 1.5  2002/11/15 14:51:54  christian
** Reorginized files.
**
** Revision 1.4  2002/11/15 14:31:07  christian
** Reimplementation of the embedded file system based on the new sciopta
** device driver conception (new "bios").
**
** Revision 1.3  2002/11/01 12:50:12  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 1.2  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 1.1  2002/03/18 08:42:07  christian
** Added umount.
**
**
**
*/
