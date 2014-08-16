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
** ID: S04106CL10
** +Revision: 1.23 +
** +Date: 2008/01/16 13:02:08 +
** Mount a filesystem into another (obsolete)
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>

#include <errno.h>
#include <sfs/sfs.h>
#include <sys/mount.h>

#include <string.h>
int
mount (const char *specialfile, const char *dirpath,
       const char *filesystemtype, unsigned long flags, const void *data)
{
  fd_tab_t *fd_tab;
  sdd_obj_t NEARPTR fs, *dir;
  sdd_obj_t NEARPTR mountpoint, *devdir;
  sdd_obj_t NEARPTR dev;
  const char *last;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t *) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC) {

      /* -->The root manager must be presistent !!! */
      mountpoint = sdd_objResolve (fd_tab->manager, dirpath, &last);
      if (!mountpoint || !SDD_IS_A (mountpoint, SFS_DIR_TYPE)) {
	PRINTF ("Mountpoint not found\n");
	sc_miscErrnoSet (EINVAL);
	return -1;
      }

      if (!data) {
	PRINTF1 ("Get filesystemtype: %s \n", filesystemtype);
	fs =
	  sdd_manGetRoot (filesystemtype, filesystemtype, SC_DEFAULT_POOL,
			  SC_FATAL_IF_TMO);
	if (!fs) {
	  sc_miscErrnoSet (EINVAL);
	  return -1;
	}
	dir = sdd_manGetByName (fs, mountpoint->name);
	sdd_objFree (&fs);
      }
      else if (SDD_IS_A ((void *) data, SDD_MAN_TYPE)) {
	PRINTF ("Filsystem hook is given in the const void *data\n");
	dir = (void *) data;
      }
      else {
	PRINTF ("void data is not a manager?!\n");
	sc_miscErrnoSet (EINVAL);
	return -1;
      }

      if (!dir) {
	PRINTF ("Filesystem will not start?!\n");
	sc_miscErrnoSet (EINVAL);
	return -1;
      }

      if (specialfile) {
	PRINTF1 ("Get device: %s \n", specialfile);
        devdir = sdd_objResolve (fd_tab->manager, specialfile, &last);
	if (!last || !devdir) {
	  sc_miscErrnoSet (EISDIR);
	  return -1;
	}
	dev = sdd_manGetByName (devdir, last);
	if (devdir != fd_tab->manager) {
	  sdd_objFree (&devdir);
	}
	if (!dev && !SDD_IS_A (dev, SDD_DEV_TYPE)) {
	  PRINTF1 ("Device: %s not found \n", specialfile);
	  sc_miscErrnoSet (ENODEV);
	  return -1;
	}

	PRINTF1 ("Try to open device: %s \n", specialfile);
	if (sdd_devOpen (dev, flags) == -1) {
	  return -1;
	}

	PRINTF1 ("Assing device: %s \n", specialfile);
	if (sfs_assign (dir, &dev) == -1) {
	  return -1;
	}
      }

      PRINTF ("Mount it ...\n");
      sfs_mount (mountpoint, &dir);
      if (mountpoint != fd_tab->manager) {
        sdd_objFree (&mountpoint);
      }
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
** $Log: mount.c,v $
** Revision 1.23  2008/01/16 13:02:08  bastian
** merge from 1.9.1.12
**
** Revision 1.22.12.1  2007/09/07 04:55:21  bastian
** added comments
**
** Revision 1.22  2004/12/14 09:10:12  christian
** Remove __GNUC__ for nonexisting includes, because this includes no exist.
**
** Revision 1.21  2004/10/07 09:25:28  bastian
** NEAR * => NEARPTR
**
** Revision 1.20  2004/10/04 13:33:58  bastian
** fix for NEAR/FAR
**
** Revision 1.19  2004/04/15 07:38:11  christian
** Clearup the chaos with release/free duple.
**
** Revision 1.18  2004/04/14 14:06:54  christian
** Cleanup namings.
**
** Revision 1.17  2003/11/27 09:52:48  christian
** Bugfixed a mount problem: The mounted fs must have the name of the
** last item in the mount path!
**
** Revision 1.16  2003/11/10 13:38:24  christian
** Short sfs_directory to sfs_dir.
**
** Revision 1.15  2003/11/10 12:39:26  christian
** Shorter name for DIRECTORY to DIR.
**
** Revision 1.14  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 1.13  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 1.12  2003/08/11 10:59:02  bastian
** fixed procVar parameter cast
**
** Revision 1.11  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 1.10  2003/07/30 05:31:47  christian
** sdd_manager -> sdd_man
** SDD_MANAGER -> SDD_MAN
**
** Revision 1.9  2003/07/29 14:43:53  christian
** Manager changes. And is allways changing.
**
** Revision 1.8  2003/04/08 12:35:53  christian
** Fixed mounting. Is not finished, have to kill fs in umounted mode.
**
** Revision 1.7  2003/03/31 14:48:10  christian
** Fixed write. Seams to work now.
** Start with cleaner error handling.
** Have to add memory allocation strategy (no memory case).
**
** Revision 1.6  2003/03/27 15:13:41  christian
** Mount is working, umount is not.
**
** Revision 1.5  2003/03/18 09:14:57  bastian
** fixed char,unsigned char and __u8
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
** Revision 1.16  2002/12/04 16:18:43  christian
** Intermediate.
**
** Revision 1.15  2002/12/04 13:03:20  christian
** First test version.
**
** Revision 1.14  2002/11/15 15:23:39  christian
** Implemented function interface for directory functions
**
** Revision 1.13  2002/11/15 14:51:54  christian
** Reorginized files.
**
** Revision 1.12  2002/11/15 14:31:07  christian
** Reimplementation of the embedded file system based on the new sciopta
** device driver conception (new "bios").
**
** Revision 1.11  2002/11/01 12:50:12  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 1.10  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 1.9  2002/05/29 13:53:12  christian
** Made open able to give back a copy of itself. If open a device allways
** use the device which is returned by open.
**
** Revision 1.8  2002/04/18 15:23:49  christian
** Link is not working for the fatfs :-((
**
** Revision 1.7  2002/04/17 09:51:01  christian
** Forced the mount to work. Not _that_ generic solution :((
**
** Revision 1.6  2002/04/17 08:48:45  christian
** Working mount :))
**
** Revision 1.5  2002/04/15 13:22:56  christian
** A non working mount :(
**
** Revision 1.4  2002/03/20 16:43:17  christian
** Intermediate.
**
** Revision 1.3  2002/03/20 16:20:41  christian
** Internal changings.
**
** Revision 1.2  2002/03/15 15:53:27  christian
** A running filesystem ready for the next developement steps.
**
** Revision 1.1  2002/03/15 10:24:04  christian
** Add mount methode.
**
** Revision 1.1.1.1  2002/03/15 09:55:39  christian
** first revision
**
**
*/
