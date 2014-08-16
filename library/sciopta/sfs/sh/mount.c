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
** ID: S03078CL6                                                    **
** +Revision: 1.11 +                                                 **
** +Date: 2004/12/14 09:10:12 +                                     **
** Setup root                                                       **
**********************************************************************
EOC*/

#include <sys/errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <unistd.h>
#include <fcntl.h>

#include <sciopta.h>
#include <sciopta.msg>

#include <ossys/defines.h>
#include <ossys/types.h>
#include <ossys/libsc.h>

#include <sdd/sdd.msg>
#include <sdd/sdd.h>

#include <sh/sfs.h>

static void
usage (char *prog)
{
  fprintf (stderr,
	   "usage: %s <special_device> <directory> <filesystem> <r|w|rw>\n",
	   prog);
  fprintf (stderr, "or: %s <directory> <manager> <r|w|rw>\n", prog);
  fflush (stderr);
}


int
sh_mount (int argc, char *argv[])
{
  if (argc != 4 && argc != 5) {
    usage (argv[0]);
    return -1;
  }

  if (argc == 5) {
    if (strncmp (argv[4], "rw", 3) == 0) {
      return mount (argv[1], argv[2], argv[3], O_RDWR, NULL);
    }
    else if (strncmp (argv[4], "r", 2) == 0) {
      return mount (argv[1], argv[2], argv[3], O_RDONLY, NULL);
    }
    else if (strncmp (argv[4], "w", 2) == 0) {
      return mount (argv[1], argv[2], argv[3], O_WRONLY, NULL);
    }
    else {
      usage (argv[0]);
      return -1;
    }
  }
  else {
    sdd_obj_t NEARPTR man = 
      sdd_manGetRoot (argv[2], "tmp", SC_DEFAULT_POOL, SC_FATAL_IF_TMO);
    if (!man) {
      return EINVAL;
    }
    if (strncmp (argv[3], "rw", 3) == 0) {
      return mount (NULL, argv[1], NULL, O_RDWR, man);
    }
    else if (strncmp (argv[3], "r", 2) == 0) {
      return mount (NULL, argv[1], NULL, O_RDONLY, man);
    }
    else if (strncmp (argv[3], "w", 2) == 0) {
      return mount (NULL, argv[1], NULL, O_WRONLY, man);
    }
    else {
      usage (argv[0]);
      return -1;
    }
  }
}

/*
** $Log: mount.c,v $
** Revision 1.11  2004/12/14 09:10:12  christian
** Remove __GNUC__ for nonexisting includes, because this includes no exist.
**
** Revision 1.10  2004/10/07 09:25:28  bastian
** NEAR * => NEARPTR
**
** Revision 1.9  2004/10/04 13:33:58  bastian
** fix for NEAR/FAR
**
** Revision 1.8  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 1.7  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 1.6  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 1.5  2003/07/30 05:31:47  christian
** sdd_manager -> sdd_man
** SDD_MANAGER -> SDD_MAN
**
** Revision 1.4  2003/07/29 14:43:53  christian
** Manager changes. And is allways changing.
**
** Revision 1.3  2003/03/27 15:13:41  christian
** Mount is working, umount is not.
**
** Revision 1.2  2003/03/26 16:09:40  christian
** Cleaner handling of the seeks (data region starts at cluster 2).
**
** Revision 1.1  2003/03/19 15:58:15  christian
** Added chroot, mkdir, mount, umount shell commandi
**
** Revision 1.1  2003/03/19 15:14:19  christian
** Started a sh subproject for sfs. All shell commandi will be collected here.
**
** Revision 1.2  2003/03/14 11:30:18  bastian
** fixes for ADS
**
** Revision 1.1  2003/03/13 10:41:54  christian
** Added echo.
**
**
**/
