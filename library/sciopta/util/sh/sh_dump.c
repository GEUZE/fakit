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
** ID: S07170BS4                                                    **
** +Revision: 1.2 +                                                **
** +Date: 2008/01/16 13:02:10 +                                     **
** shell: dump memory                                               **
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
  fdprintf (1, "Usage: %s -h [-a address] [-s size]\n", name);
  fdprintf (1, "  -h     display this help text\n");
  fdprintf (1, "  -a     startaddress of dump\n");
  fdprintf (1, "  -s     size of dump\n");

}

int
sh_dump (int argc, char *argv[])
{
  static __u8 *p = NULL;
  static int size = 16;
  int i;
  opt_r_t options;
  int c;
  __u32 help;

  initopt_r (&options);

  while ((c = getopt_r (&options, argc, argv, "ha:s:")) != -1) {
    switch (c) {
    case 'h':
      exit_usage (argv[0]);
      return 0;
    case 'a':
      help = sh_atoi(options.optarg);
      p = (__u8 *)help;
      break;
    case 's':
      size = sh_atoi(options.optarg);

      if ( size < 16 || size > 1024 ){
	size = 16;
      }
    default:
      break;
    }
  }
  i =  (int)((__ptrsize_t)p & 15);
  fdprintf(1,"%08x: ",(__u32)p & ~15);
  for( ; i ; --i ){
    write(1,"   ",3);
  }
  for(i = 0; i < size; ++i ){
    fdprintf(1,"%02x ",*p++);
    if ( ((__u32)p & 15) == 0 ){
      fdprintf(1,"\n");
      if ( i < size-1 ){
	fdprintf(1,"%08x: ",p);
      }
    }
  }
  if ( ((__u32)p & 15) != 15 ){
    fdprintf(1,"\n");
  }
  return 0;
}
