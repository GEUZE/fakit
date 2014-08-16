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
** ID: S03070CL75                                                   **
** +Revision: 1.10 +                                                 **
** +Date: 2008/01/16 13:02:10 +                                     **
** shell: mid - print module id                                     **
**********************************************************************
EOC*/
#include <sciopta.h>
#include <sciopta.msg>
#include <unistd.h>
#include <scio.h>
#include <string.h>
#include <stdlib.h>
#include <sh/sh.h>
#include <sh/sh.msg>

int
sh_mid (int argc, char *argv[])
{
#ifdef __KKERNEL__
  fdprintf(scio_stdout,"0\n");
#else
  sc_pid_t mid;
  if (argc != 2) {
    fdprintf (scio_stdout,"usage: %s <modulename>\n", argv[0]);
    return -1;
  }
  mid = sc_moduleIdGet (argv[1]);
  fdprintf (scio_stdout,"%ld\n", mid);
#endif
  return 0;
}
