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
** ID: S03070CL72                                                   **
** +Revision: 1.12 +                                                 **
** +Date: 2008/01/16 13:02:10 +                                     **
** shell: more                                                      **
**********************************************************************
EOC*/
#include <sciopta.h>
#include <sciopta.msg>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ossys/libsc.h>
#include <sh/sh.h>
#include <sh/sh.msg>
#include <scio.h>

union sc_msg {
  sc_msgid_t id;
  sh_inherit_t inherit; 
};

int
sh_more (int argc, char *argv[])
{
  char *buf;
  int len;
  (void)argc;
  (void)argv;
  buf = (char *)sc_msgAlloc(SC_PATH_MAX+1,0,SC_DEFAULT_POOL,SC_FATAL_IF_TMO);
  write (scio_stdout, "more: \r\n", strlen ("more: \r\n"));
  while ((len = (int)read(0, buf, SC_PATH_MAX)) > 0) {
    write (scio_stdout, buf, len);    
  }
  write (scio_stdout, "\r\n", 2);
  return 0;
}
