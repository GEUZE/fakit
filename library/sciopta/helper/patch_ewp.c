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
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S06177BS1                                                    **
** +Revision: 1.2.12.5 +                                                 **
** +Date: 2009/10/26 11:42:52 +                                     **
** Small utility to replace $SCIOPTA_HOME in an IAR .ewp file        **
**********************************************************************
EOC*/

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int patch(int dfd, char *line, const char *tag, const char *var)
{
  int l = strlen(tag);
  char *pos;
  
  pos = strstr(line, tag);
  if ( pos ) {
    if ( !var || !*var ) {
      fprintf(stderr, "Error: Could not get value for %s\n", tag);
      exit(-1);
    }
    write(dfd, line, pos-line);
    write(dfd, var, strlen(var));
    write(dfd, pos+l, strlen(pos+l));
  }
  return (int) pos;
}

static void cpyenv(const char *name, char *dst, int errIfNotExist)
{
  char *p;

  p = getenv(name);
  if ( !p ){
    if (errIfNotExist) {
      fprintf(stderr, "Error: Could not get environment variable %s\n", name);
      exit(-1);
    } else {
      *dst = 0;
    }
  } else {
    memset(dst, 0, 256);
    strncpy(dst, p, 255);
  }
}

int main(int argc, char *argv[])
{
  int sfd;
  int dfd;
  char line[256];
  int ret;
  int idx;
  char schome[256];
  char stmlib[256];
  char stmusblib[256];
  char strlib[256];
  char lmilib[256];

  if ( argc != 3 ){
    fprintf(stderr, "Usage: %s <src> <dst>\n", argv[0]);
    return -1;
  }

  cpyenv("SCIOPTA_HOME", schome, 1);
  cpyenv("STM32_FWLIB", stmlib, 0);
  cpyenv("STM32_USBLIB", stmusblib, 0);
  cpyenv("STR9_FWLIB", strlib, 0);
  cpyenv("LMI_DRIVER", lmilib, 0);
  printf("Patching %s => %s with \"%s\"\n", argv[1], argv[2], schome);

  sfd = open(argv[1], O_RDONLY);
  if ( sfd < 0 ){
    fprintf(stderr, "Error: Could not open \"%s\" for reading\n", argv[1]);
    return -1;
  }
  dfd = creat(argv[2], 0644);
  if ( dfd < 0 ){
    fprintf(stderr, "Error: Could not open \"%s\" for writing\n", argv[2]);
    close(sfd);
    return -1;
  }

  idx = 0;
  while( !eof(sfd) ){
    ret = read(sfd, &line[idx], 1);
    if ( ret != 1 ){
      fprintf(stderr, "Error reading\n");
      close(sfd);
      close(dfd);
      return -1;
    }
    if ( line[idx] == '\n' ){
      line[idx+1] = 0;      
      if ( patch(dfd, line, "$SCIOPTA_HOME$", schome) ) ;
      else if ( patch(dfd, line, "$STM32_FWLIB$", stmlib) ) ;
      else if ( patch(dfd, line, "$STM32_USBLIB$", stmusblib) ) ;
      else if ( patch(dfd, line, "$STR9_FWLIB$", strlib) ) ;
      else if ( patch(dfd, line, "$LMI_DRIVER$", lmilib) ) ;
      else
	write(dfd, line, idx+1);

      idx = 0;
    } else {
      ++idx;
    }
  }
  close(sfd);
  close(dfd);
  
  return 0;
}

