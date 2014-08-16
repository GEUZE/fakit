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
** ID: S03070CL29                                                   **
** +Revision: 1.2 +                                                 **
** +Date: 2004/12/14 09:10:12 +                                     **
** mv - move (rename) files                                         **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>
#include <stdio.h>
#include <errno.h>

#include <sh/sfs.h>

static void
usage (const char *prog)
{
  fprintf (stderr, "Usage: %s SOURCE DEST\n", prog);
}

int
sh_mv (int argc, char *argv[])
{
  if (argc < 3) {
    usage (argv[0]);
    return -1;
  }
  
  if (rename (argv[1], argv[2]) == -1) {
    if (sc_miscErrnoGet () == EXDEV) {
      /* we have to copy and remove oldpath */
      return 0;
    }
  }
  return 0;
}
