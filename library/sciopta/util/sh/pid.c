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
** ID: S03070CL77                                                   **
** +Revision: 1.11 +                                                 **
** +Date: 2008/01/16 13:02:10 +                                     **
** shell pid                                                        **
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
sh_pid (int argc, char *argv[])
{
  sc_pid_t pid;
  if (argc != 2) {
    fdprintf (scio_stdout,"usage: %s <processname>\n", argv[0]);
    return -1;
  }
  pid = sc_procIdGet (argv[1], SC_NO_TMO);
#if SIZEOF_PID_T == 4
  fdprintf (scio_stdout,"%08x\n", pid);
#else
  fdprintf (scio_stdout,"%04x\n", pid);
#endif
  return 0;
}
