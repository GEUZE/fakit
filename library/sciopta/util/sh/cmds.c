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
** +Revision: 1.8 +                                                 **
** Display (parsed) parameters on stdout.                           **
**********************************************************************
*/
#include <sciopta.h>
#include <ossys/libsc.h>
#include <sh/sh.h>
#include <scio.h>

#undef __STRICT_ANSI__
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

extern sh_process_t sh_processes[];

int
sh_cmds (int argc, char *argv[])
{
  int i;
  (void)argc;
  (void)argv;

  i = 0;
  while (sh_processes[i].name[0] != '\0') {
    fdprintf (scio_stdout, 
	      "- %s:\n%s\n", 
	      sh_processes[i].name, 
              sh_processes[i].descr);
    ++i;
  }
  return 0;
}
