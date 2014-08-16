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
** ID: S03070CL70                                                   **
** +Revision: 1.2 +                                                **
** +Date: 2008/01/16 13:02:11 +                                     **
** shell: exit                                                      **
**********************************************************************
EOC*/
#include <sciopta.h>
#undef __STRICT_ANSI__
#include <unistd.h>
#include <sh/sh.h>
#include <scio.h>
#include <ossys/libsc.h>

static void
exit_usage (const char *name)
{
  fdprintf (scio_stdout, "Usage: %s [EXITCODE]...\n", name);
  fdprintf (scio_stdout, "  -h     display this help text\n");
}

int
sh_exit (int argc, char *argv[])
{
  int err;
  opt_r_t options;
  int c;
  initopt_r (&options);

  while ((c = getopt_r (&options, argc, argv, "h")) != -1) {
    switch (c) {
    case 'h':
      exit_usage (argv[0]);
      return 0;
    default:
      break;
    }
  }
  /* check the args */
  err = 0;
  if (argc == 2) {
    err = sh_atoi (argv[1]);
  }
  closeall ();
  sc_miscErrnoSet(err); /* must set errno here, as closeall also
			   sets it */
  sc_procKill (SC_CURRENT_PID, 0);
  /* should never reach here */
  return err;
}
