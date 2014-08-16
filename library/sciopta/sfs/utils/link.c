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
** ID: S04106CL7
** +Revision: 1.5 +
** +Date: 2008/01/16 13:02:08 +
** link.c
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>

#include <unistd.h>
#include <errno.h>

int
link (const char *oldpath, const char *newpath)
{
  fd_tab_t *fd_tab;

  if (!oldpath || !newpath) {
    sc_miscErrnoSet (EFAULT);
    return -1;
  }

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t *) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC) {
      return -1;
    }
  }
  else {
    PRINTF ("There is no SCIO fd_tab installed \n");
    sc_miscErrnoSet (ENOENT);
    return -1;
  }
  return -1;
}

/*
** $Log: link.c,v $
** Revision 1.5  2008/01/16 13:02:08  bastian
** merge from 1.9.1.12
**
** Revision 1.4.12.1  2007/09/07 04:55:21  bastian
** added comments
**
** Revision 1.4  2004/12/14 09:10:12  christian
** Remove __GNUC__ for nonexisting includes, because this includes no exist.
**
** Revision 1.3  2003/08/11 11:00:16  bastian
** fixed procVar parameter cast
**
** Revision 1.2  2003/03/18 07:30:38  bastian
** added ARM and fixed for ADS
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
** Revision 1.1  2002/04/19 14:34:09  christian
** Add the syscall link
**
** Revision 1.3  2002/04/18 12:56:58  christian
** Working unlink.
**
** Revision 1.2  2002/04/18 12:35:00  christian
** chroot and unlink is now part of the scio package.
**
** Revision 1.1  2002/04/18 12:22:33  christian
** Add unlink methode.
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
