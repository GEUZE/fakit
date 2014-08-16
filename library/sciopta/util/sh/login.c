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
** (c) 2002,2003 Sciopta GmbH, Litronic AG/ Schweiz                 **
**                                                                  **
**********************************************************************
** ID: S03072CL0                                                    **
** +Revision: 1.17 +                                                 **
** +Date: 2008/01/16 13:02:10 +                                     **
** Login command                                                    **
**********************************************************************
EOC*/
#include <sciopta.h>
#include <scio.h>
#undef __STRICT_ANSI__
#include <string.h>
#include <sys/ioctl.h>
#include <sys/unistd.h>
#include <ossys/libsc.h>
#include <sh/sh.h>


extern const int login_passwdLines;

extern const char *login_passwd[];

static void
login_usage (const char *name)
{
  fdprintf (scio_stdout, "Usage: %s [OPTIONS] ...\n", name);
  fdprintf (scio_stdout, "  -h     display this help text\n");
}

int
sh_login (int argc, char *argv[])
{
  int i, c, echo;
  opt_r_t options;
  char name[SC_NAME_MAX + 1];
  char pass[SC_NAME_MAX + 1];
  char ch;
  int len;
  initopt_r (&options);
  while ((c = getopt_r (&options, argc, argv, "h")) != -1) {
    switch (c) {
    case 'h':
      login_usage (argv[0]);
      return 0;
    default:
      break;
    }
  }
  fdprintf (scio_stdout, "User: ");
  i = 0;
  len = (int)read (0, &ch, 1);
  while (i < SC_NAME_MAX && len && ch != '\n') {
    name[i++] = ch;
    len = (int)read (0, &ch, 1);
  }
  name[i] = 0;
  /* switch echo off */
  echo = 0;
  ioctl (0, SERECHO, &echo);
  fdprintf (scio_stdout, "Password: ");
  i = 0;
  len = (int)read (0, &ch, 1);
  while (i < SC_NAME_MAX && len && ch != '\n') {
    pass[i++] = ch;
    len = (int)read (0, &ch, 1);
  }
  pass[i] = 0;
  /* switch restore echo setting */
  ioctl (0, SERECHO, &echo);
  fdprintf (scio_stdout, "\n\n");

  if (login_passwdLines >= 1) {
    i = 0;
    while (i < login_passwdLines
	   && (strncmp (name, login_passwd[i << 1], SC_NAME_MAX) != 0
	       || strncmp (pass, login_passwd[(i << 1) + 1], SC_NAME_MAX) != 0))
      ++i;
    if (i >= login_passwdLines) {
      return -1;
    }
    else {
      return 0;
    }
  }
  return 0;
}
