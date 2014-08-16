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
** ID: S04106CL19
** +Revision: 1.12 +
** +Date: 2008/01/16 13:02:09 +
** Sync current root
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>
#include <sfs/sfs.h>

int
sync (void)
{
  fd_tab_t *fd_tab;
  sdd_obj_t NEARPTR rootdir;

  PRINTF ("sync\n");
  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t *) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC) {
      if (SDD_IS_A (fd_tab->manager, SFS_DIR_TYPE)) {
	/* send sync to my root directory */
	rootdir = fd_tab->manager;
	sfs_sync (rootdir);
      }
      else {
	PRINTF ("no root filesystem, do a chroot\n");
	return -1;
      }
    }
  }
  return -1;
}

/*
** $Log: sync.c,v $
** Revision 1.12  2008/01/16 13:02:09  bastian
** merge from 1.9.1.12
**
** Revision 1.11.12.1  2007/09/07 04:55:21  bastian
** added comments
**
** Revision 1.11  2004/10/07 09:25:28  bastian
** NEAR * => NEARPTR
**
** Revision 1.10  2004/10/04 13:33:58  bastian
** fix for NEAR/FAR
**
** Revision 1.9  2003/11/10 12:39:26  christian
** Shorter name for DIRECTORY to DIR.
**
** Revision 1.8  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 1.7  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 1.6  2003/08/11 10:59:45  bastian
** fixed procVar parameter cast
**
** Revision 1.5  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 1.4  2003/03/18 07:30:38  bastian
** added ARM and fixed for ADS
**
** Revision 1.3  2002/12/06 16:28:07  christian
** Intermediate.
**
** Revision 1.2  2002/12/06 14:31:05  christian
** EFS is now SFS and efs is now sfs
**
** Revision 1.1.1.1  2002/12/06 10:54:27  christian
** first revision based on new sdd
**
** Revision 1.8  2002/11/15 14:51:54  christian
** Reorginized files.
**
** Revision 1.7  2002/11/15 14:31:07  christian
** Reimplementation of the embedded file system based on the new sciopta
** device driver conception (new "bios").
**
** Revision 1.6  2002/11/01 12:50:12  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 1.5  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 1.4  2002/04/15 13:22:56  christian
** A non working mount :(
**
** Revision 1.3  2002/04/12 14:43:22  christian
** dot and dotdot added to the new directories.
**
** Revision 1.2  2002/04/04 15:52:41  christian
** A running lseek.
** have to reimplement the write methode.
**
** Revision 1.1  2002/04/03 09:53:01  christian
** Started with the sync POSIX system call
**
**
**
*/
