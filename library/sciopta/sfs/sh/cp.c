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
** +Revision: 1.5 +                                                 **
** +Date: 2004/12/14 09:10:12 +                                     **
** cp - copy files and directories                                  **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sys/errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <unistd.h>
#include <fcntl.h>

#include <sh/sfs.h>

static void
usage (char *prog)
{
  fprintf (stderr, "usage: %s SOURCE DEST\n", prog);
  fflush (stderr);
}


#define BUF_SIZE 64

int
sh_cp (int argc, char *argv[])
{
  char *buf;
  char *p;
  int oldfd, newfd;
  int len;
  int error = 0;


  if (argc < 3) {
    usage (argv[0]);
    return -1;
  }

  if ((oldfd = open (argv[1], O_RDONLY)) == -1) {
    return -1;
  }
  if ((newfd = open (argv[2], O_RDWR | O_TRUNC)) == -1) {
    if ((newfd = open (argv[2], O_RDWR | O_CREAT)) == -1) {
      close (oldfd);
      return -1;
    }
  }

  buf = (char *)sc_msgAlloc(BUF_SIZE,0,SC_DEFAULT_POOL,SC_FATAL_IF_TMO);

  while ((len = read (oldfd, buf, BUF_SIZE)) != -1 ){
    p = buf;
    while( len && error != -1 ){
      error = write (newfd, p, len);
      len -= error;
      p += error;
    }
  }

  close (oldfd);
  close (newfd);

  sc_msgFree((sc_msgptr_t )&buf);

  if ( error == -1) {
    return -1;
  }
  else {
    return 0;
  }
}

/*
** $Log: cp.c,v $
** Revision 1.5  2004/12/14 09:10:12  christian
** Remove __GNUC__ for nonexisting includes, because this includes no exist.
**
** Revision 1.4  2004/10/04 13:33:58  bastian
** fix for NEAR/FAR
**
** Revision 1.3  2003/08/07 07:02:35  bastian
** fixes
**
** Revision 1.2  2003/08/07 05:42:14  bastian
** bug fixes
**
** Revision 1.1  2003/04/07 13:13:01  christian
** Add filesystem utilities.
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
