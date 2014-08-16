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
** ID: S03090CL1                                                    **
** +Revision: 1.12 +                                                 **
** +Date: 2008/01/16 13:02:10 +                                     **
** ls - list directory contents                                     **
**********************************************************************
EOC*/
#include <sciopta.h>
#undef __STRICT_ANSI__
#include <scio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ossys/libsc.h>
#include <sh/sh.h>

#if defined __ARMCC_VERSION
/* suppress: assignment in condition warning */
#pragma diag_suppress 1293
#endif


static void
ls_usage (const char *name)
{
  fdprintf (scio_stdout, "Usage: %s [OPTION]...\n", name);
  fdprintf (scio_stdout, "  -h     display this help text\n");
}

static void
iterate (DIR NEARPTR dir)
{
  struct dirent NEARPTR e;
  while ((e = readdir (dir))) {
    fdprintf (scio_stdout, "%s\n", e->d_name);
  }
}

int
sh_ls (int argc, char *argv[])
{
  DIR NEARPTR dir;
  int i;
  opt_r_t options;
  int c;
  initopt_r (&options);
  while ((c = getopt_r (&options, argc, argv, "h")) != -1) {
    switch (c) {
    case 'h':
      ls_usage (argv[0]);
      return 0;
    default:
      break;
    }
  }
  if (options.optind == argc) {
    // later we consult env variable and plot the local dir
    dir = opendir ("/");
    if (dir) {
      iterate (dir);
      closedir (dir);
    }
    else {
      fdprintf (scio_stderr, "ls: %s: not found\n", "/");
    }
    return 0;
  }
  i = options.optind;
  while (i < argc) {
    dir = opendir (argv[i]);
    if (dir) {
      iterate (dir);
      closedir (dir);
    }
    else {
      fdprintf (scio_stderr, "%s: %s: not found\n", argv[0], argv[i]);
    }
    ++i;
  }
  return 0;
}
