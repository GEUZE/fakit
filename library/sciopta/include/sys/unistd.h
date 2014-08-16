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
** ID: S03070CL57                                                   **
** +Revision: 1.14 +                                                 **
** +Date: 2004/12/16 13:36:07 +                                     **
** unistd                                                           **
**********************************************************************
EOC*/
#ifndef _SYS_UNISTD_H
#define _SYS_UNISTD_H 	1

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/types.h>
  
  typedef unsigned int socklen_t;

  int close (int fd);
  void closeall (void);
  ssize_t write (int fd, const void *buffer, size_t size);
  ssize_t read (int fd, void *buffer, size_t size);
  int awrite (int fd, union sc_msg **msg);
  int aread (int fd, size_t size);
  off_t lseek (int fd, off_t off, flags_t flags);
  int unlink (const char *pathname);
  int pipe (int fildes[2]);
  int dup (int oldfd);
  int dup2 (int oldfd, int newfd);
  int link(const char *oldpath, const char *newpath);
  int rmdir(const char *pathname);
  int chroot (const char *path);
  int sync (void);

#ifndef SEEK_SET
#define SEEK_SET       0       /* Seek from beginning of file.  */
#define SEEK_CUR       1       /* Seek from current position.  */
#define SEEK_END       2       /* Seek from end of file.  */
#endif

#define STDIN_FILENO    0       /* standard input file descriptor */
#define STDOUT_FILENO   1       /* standard output file descriptor */
#define STDERR_FILENO   2       /* standard error file descriptor */

#ifdef __cplusplus
}
#endif
#endif
