/* -*-c++-*-
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
** (c) 2002,2003 Sciopta GmbH, Litronic AG/ Schweiz                 **
**                                                                  **
**********************************************************************
** ID: S03216CL1                                                    **
** +Revision: 1.11 +                                                 **
** Display (parsed) parameters on stdout.                           **
**********************************************************************
*/
#include <sciopta.h>
#undef __STRICT_ANSI__

#include <scio.h>
#include <ossys/libsc.h>
#include <sh/sh.h>

static void
echo_usage (const char *name)
{
  fdprintf (scio_stdout, "Usage: %s [OPTION]... [STRING]...\n", name);
  fdprintf (scio_stdout, "  -n     do not output the trailing newline\n");
  fdprintf (scio_stdout, "  -h     display this help text\n");
}

int
sh_echo (int argc, char *argv[])
{
  int i, c;
  int newline = 1;
  opt_r_t options;
  initopt_r (&options);
  while ((c = getopt_r (&options, argc, argv, "hn")) != -1) {
    switch (c) {
    case 'n':
      newline = 0;
      break;
    case 'h':
      echo_usage (argv[0]);
      return 0;
    default:
      break;
    }
  }
  i = options.optind;
  while (i < argc) {
    fdprintf (scio_stdout, argv[i]);
    ++i;
    if (i < argc) {
      fdprintf (scio_stdout, " ");
    }
  }
  if (newline){
    fdprintf (scio_stdout, "\n");
  }
  return 0;
}
