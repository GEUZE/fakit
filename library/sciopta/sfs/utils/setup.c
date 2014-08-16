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
** ID: S04106CL11
** +Revision: 1.18 +
** +Date: 2008/01/16 13:02:09 +
** Setup a root-fs
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>
#include <errno.h>
#include <sfs/sfs.h>


int
setup_root (const char *specialfile, const char *dirname, 
     	    const char *filesystemtype, unsigned long flags)
{
  fd_tab_t *fd_tab;
  sdd_obj_t NEARPTR man;
  sdd_obj_t NEARPTR dir;
  sdd_obj_t NEARPTR dev;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t *) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC) {
      const char *devName;

      PRINTF1 ("Get filesystemtype: %s \n", filesystemtype);
      man =
	sdd_manGetRoot (filesystemtype, filesystemtype, SC_DEFAULT_POOL,
			SC_FATAL_IF_TMO);
      if (!man) {
	sc_miscErrnoSet (EINVAL);
	return -1;
      }
      
      dir = sdd_manGetByName (man, dirname);

      sdd_objFree (&man);

      if (!dir) {
	sc_miscErrnoSet (EINVAL);
	return -1;
      }

      PRINTF1 ("Get device: %s \n", specialfile);
      man = sdd_objResolve (fd_tab->manager, specialfile, &devName);
      if (!man || !devName) {
	sc_miscErrnoSet (EISDIR);
	return -1;
      }
      dev = sdd_manGetByName (man, devName);

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

      return sdd_manAdd (fd_tab->manager, &dir);
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
** $Log: setup.c,v $
** Revision 1.18  2008/01/16 13:02:09  bastian
** merge from 1.9.1.12
**
** Revision 1.17.12.1  2007/09/07 04:55:21  bastian
** added comments
**
** Revision 1.17  2004/12/14 09:10:12  christian
** Remove __GNUC__ for nonexisting includes, because this includes no exist.
**
** Revision 1.16  2004/10/07 09:25:28  bastian
** NEAR * => NEARPTR
**
** Revision 1.15  2004/10/04 13:33:58  bastian
** fix for NEAR/FAR
**
** Revision 1.14  2004/04/15 07:38:11  christian
** Clearup the chaos with release/free duple.
**
** Revision 1.13  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 1.12  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 1.11  2003/08/07 05:20:17  bastian
** fixed procVar casting
**
** Revision 1.10  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 1.9  2003/07/30 05:31:47  christian
** sdd_manager -> sdd_man
** SDD_MANAGER -> SDD_MAN
**
** Revision 1.8  2003/07/29 14:43:53  christian
** Manager changes. And is allways changing.
**
** Revision 1.7  2003/03/26 16:09:40  christian
** Cleaner handling of the seeks (data region starts at cluster 2).
**
** Revision 1.6  2003/03/19 15:57:56  christian
** Mount and setup_root has now the same parameter succession.
**
** Revision 1.5  2003/03/18 07:30:38  bastian
** added ARM and fixed for ADS
**
** Revision 1.4  2002/12/13 16:42:57  christian
** Finished the port of mkdosfs (named here as mkfatfs).
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
** Revision 1.15  2002/12/04 16:18:43  christian
** Intermediate.
**
** Revision 1.14  2002/12/04 13:03:20  christian
** First test version.
**
** Revision 1.13  2002/12/03 17:23:10  christian
** Intermediate.
**
** Revision 1.12  2002/11/15 14:51:54  christian
** Reorginized files.
**
** Revision 1.11  2002/11/15 14:31:07  christian
** Reimplementation of the embedded file system based on the new sciopta
** device driver conception (new "bios").
**
** Revision 1.10  2002/11/01 12:50:12  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 1.9  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 1.8  2002/05/29 13:53:12  christian
** Made open able to give back a copy of itself. If open a device allways
** use the device which is returned by open.
**
** Revision 1.7  2002/04/03 14:26:26  bastian
** Fixed debug-text
**
** Revision 1.6  2002/03/26 10:44:25  christian
** Setup root stores its father.
**
** Revision 1.5  2002/03/20 16:20:41  christian
** Internal changings.
**
** Revision 1.4  2002/03/18 08:42:07  christian
** Added umount.
**
** Revision 1.3  2002/03/15 15:53:27  christian
** A running filesystem ready for the next developement steps.
**
** Revision 1.2  2002/03/15 10:24:04  christian
** Add mount methode.
**
** Revision 1.1.1.1  2002/03/15 09:55:39  christian
** first revision
**
**
*/
