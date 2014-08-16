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
** ID: S03078CL5                                                    **
** +Revision: 1.4 +                                                 **
** +Date: 2004/12/14 09:10:12 +                                     **
** Setup root                                                       **
**********************************************************************
EOC*/

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

#include <sh/sfs.h>

static void
usage (char *prog)
{
  fprintf (stderr,
    "usage: %s <special_device> <directory_name> <filesystem> <r|w|rw>\n", 
    prog);
  fflush (stderr);
}

int
sh_setup_root (int argc, char *argv[])
{
  if (argc < 5) {
    usage (argv[0]);
    return -1;
  }

  if (strncmp (argv[4], "rw", 3) == 0) {
    return setup_root (argv[1], argv[2], argv[3], O_RDWR);
  }
  else if (strncmp (argv[4], "r", 2) == 0) {
    return setup_root (argv[1], argv[2], argv[3], O_RDONLY);
  }
  else if (strncmp (argv[4], "w", 2) == 0) {
    return setup_root (argv[1], argv[2], argv[3], O_WRONLY);
  }
  else {
    usage (argv[0]);
    return -1;
  }
}

/*
** $Log: setup_root.c,v $
** Revision 1.4  2004/12/14 09:10:12  christian
** Remove __GNUC__ for nonexisting includes, because this includes no exist.
**
** Revision 1.3  2003/03/26 16:09:40  christian
** Cleaner handling of the seeks (data region starts at cluster 2).
**
** Revision 1.2  2003/03/19 15:58:15  christian
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
