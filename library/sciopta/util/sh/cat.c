/*
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
** ID: S03044CL0                                                    **
** +Revision: 1.15 +                                                **
** Displays a file on stdout.                                       **
**********************************************************************
** Author: Christian Liesch <christian.liesch@litronic.ch           **
** Date: 02/13/03                                                   **
**********************************************************************
*/
#include <sciopta.h>
#undef __STRICT_ANSI__
#include <unistd.h>
#include <fcntl.h>
#include <ossys/libsc.h>
#include <sh/sh.h>
#include <sys/ioctl.h>
#include <scio.h>

#if defined __ARMCC_VERSION
/* suppress: assignment in condition warning */
#pragma diag_suppress 1293
#endif

static void
cat_usage (const char *name)
{
  fdprintf (1, "Usage: %s [OPTION] [FILE]...\n", name);
  fdprintf (1, "  -h     display this help text\n");
}

#define BUF_SIZE 80
#define CTRL_D ('D'&0x1f)

int sh_cat (int argc, char *argv[])
{
  int fd;
  int len;
  int i;
  char *buf;
  opt_r_t options;
  int c;
  initopt_r (&options);
  while ((c = getopt_r (&options, argc, argv, "h")) != -1) {
    switch (c) {
    case 'h':
     cat_usage (argv[0]);
      return 0;
    default:
      break;
    }
  }

  buf = (char *)sc_msgAlloc(BUF_SIZE,0,SC_DEFAULT_POOL,SC_FATAL_IF_TMO);
  if (options.optind == argc) {
    /* Set tmo on stdin to 50ms */
    sc_ticks_t tmo = sc_tickMs2Tick(50);
    if ( ioctl(0,SERTMO,&tmo) < 0 ){
      fdprintf(2,"Could not change stdin tmo\n");
      sc_msgFree((sc_msgptr_t)&buf);
      return -1;
    }
    while(1){
      len = read (0, buf, BUF_SIZE);
      if (!len ) continue;
      for(i = 0; i < len ; ++i ){
	if ( buf[i] == CTRL_D ){
	  break;
	}
	if ( buf[i] == '\r' && buf[i+1] != '\n' ){
	  buf[i] = '\n';
	}
      }
      write (1, buf, i);
      if ( i != len ) break;
    }
    if ( ioctl(0,SERTMO,&tmo) < 0 ){
      fdprintf(2,"Could not change stdin tmo\n");
      sc_msgFree((sc_msgptr_t)&buf);
      return -1;
    }
  } else {
    for (i = options.optind; i < argc; i++) {
      if ((fd = open (argv[i], O_RDONLY)) == -1) {
	sc_msgFree((sc_msgptr_t)&buf);
	fdprintf (2, "file %s not found\n", argv[i]);
	return -1;
      }
      while ((len = read (fd, buf, BUF_SIZE)) > 0) {
	write (1, buf, len);
      }
      close (fd);
    }
  }
  sc_msgFree((sc_msgptr_t)&buf);
  return 0;
}
