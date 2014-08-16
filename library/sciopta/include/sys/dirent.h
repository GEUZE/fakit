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
** ID: S03070CL54                                                   **
** +Revision: 1.16 +                                                 **
** +Date: 2004/10/07 07:37:41 +                                     **
** directory                                                        **
**********************************************************************
EOC*/

#ifndef __SYS_DIRENT_H
#define __SYS_DIRENT_H	1

#ifdef __cplusplus
extern "C" {
#endif

#include <sdd/sdd.msg>
#include <ossys/defines.h>

  enum {
    DT_UNKNOWN = 0,
# define DT_UNKNOWN     DT_UNKNOWN
    DT_FIFO = 1,
# define DT_FIFO        DT_FIFO
    DT_CHR = 2,
# define DT_CHR         DT_CHR
    DT_DIR = 4,
# define DT_DIR         DT_DIR
    DT_BLK = 6,
# define DT_BLK         DT_BLK
    DT_REG = 8,
# define DT_REG         DT_REG
    DT_LNK = 10,
# define DT_LNK         DT_LNK
    DT_SOCK = 12,
# define DT_SOCK        DT_SOCK
    DT_WHT = 14
# define DT_WHT         DT_WHT
  };

  struct dirent {
    unsigned char d_type;
    char d_name[SC_NAME_MAX];
  };

  typedef struct DIR {
    sdd_obj_t NEARPTR man;
    sdd_obj_t NEARPTR cur;
    sc_ticks_t tmo;
    struct dirent dirent;
  } DIR;

  DIR NEARPTR opendir (const char *name);
  int closedir (DIR NEARPTR dir);
  struct dirent NEARPTR readdir (DIR NEARPTR dir);
  void rewinddir (DIR NEARPTR dir);

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: dirent.h,v $
** Revision 1.16  2004/10/07 07:37:41  bastian
** fixed NEAR * to NEARPTR
**
** Revision 1.15  2004/10/04 12:24:40  bastian
** fixes for m16c
**
** Revision 1.14  2004/04/01 11:05:13  christian
** Remove this namespacing macro, it is not working properly.
**
** Revision 1.13  2004/03/11 10:18:41  christian
** Only do rename POSIX calls to scio_POSIX if it is LINUX_HOSTED.
**
** Revision 1.12  2004/02/25 15:07:47  christian
** To be able to use native POSIX calls in the softkernel, we have to
** rename all POSIX calls by appending a prefix scio_ before every call.
** With defines the name is switch back to posix namespace. If you want to use
** nativ POSIX call of your host you must not include the sciopta posix stuff.
**
** Revision 1.11  2003/09/08 07:57:15  christian
** ips_device -> ips_dev.
**
** Revision 1.10  2003/09/08 07:07:29  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 1.9  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 1.8  2003/03/26 16:08:46  christian
** Bugfixes
**
** Revision 1.7  2003/03/21 14:50:17  christian
** Bugfixed everthing around opendir, readdir, closedir.
** Manager: get next, get prev, ... has wrong message ids.
**
** Revision 1.6  2003/03/11 14:58:30  bastian
** added header
**
** Revision 1.5  2002/11/01 12:50:13  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 1.4  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 1.3  2002/09/16 12:23:28  christian
** Rename sdd_handle_t to sdd_obj_t (prepare for futur changes)
**
** Revision 1.2  2002/09/03 15:11:46  christian
** Make use of the newlib.
**
** Revision 1.1  2002/09/02 14:29:04  christian
** Rewritten scio because of the completly new bios strategy.
**
**
**/
