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
** ID: S03078CL4                                                    **
** +Revision: 1.4 +                                                **
** +Date: 2003/04/07 11:35:54 +                                     **
** shell                                                            **
**********************************************************************
EOC*/

#ifndef _SFS_SH_SFS_H_
#define _SFS_SH_SFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/types.h>

  int sh_cd (int argc, char *argv[]);
  int sh_chroot (int argc, char *argv[]);
  int sh_cp (int argc, char *argv[]);
  int sh_ls (int argc, char *argv[]);
  int sh_mkdir (int argc, char *argv[]);
  int sh_mkfatfs (int argc, char *argv[]);
  int sh_mount (int argc, char *argv[]);
  int sh_mv (int argc, char *argv[]);
  int sh_rm (int argc, char *argv[]);
  int sh_setup_root (int argc, char *argv[]);
  int sh_sync (int argc, char *argv[]);
  int sh_umount (int argc, char *argv[]);

#ifdef __cplusplus
}
#endif
#endif
