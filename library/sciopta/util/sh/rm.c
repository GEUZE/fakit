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
** ID: S03093CL1                                                    **
** +Revision: 1.10 +                                                 **
** +Date: 2008/01/16 13:02:10 +                                     **
** remove manager entries (depending on manager implementation)     **
**********************************************************************
EOC*/
#include <sciopta.h>
#undef __STRICT_ANSI__
#include <scio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

#if defined __GNUC__ && !defined __WIN32__
#define fprintf fiprintf
#endif

static void
usage (const char *prog)
{
  fdprintf (scio_stderr, "Usage: %s <path_name>\n", prog);
}

int
sh_rm (int argc, char *argv[])
{
  if (argc < 2) {
    usage (argv[0]);
    return -1;
  }
  return unlink (argv[1]);
}
