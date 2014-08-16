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
** ID: S03070CL53                                                   **
** +Revision: 1.56 +                                                **
** +Date: 2008/01/16 13:01:59 +                                     **
** Sciopta I/O definitions and functions.                           **
**********************************************************************
EOC*/


#ifndef __SCIO_H__
#define __SCIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#include <string.h>
#include <netdb.h>
#include <dirent.h>

#define SCIO_FLAGS_NONE		0
#define SCIO_FLAGS_BACKGROUND	1
#define SCIO_FLAGS_SEND_READ	2
  
  typedef struct fd_tab_s {
    sc_msgid_t magic;
    flags_t flags;
    struct hostent *hostent;
    struct protoent *protoent;
    char *pwd;
    char ipNoNDots[16];
    int max_fd;
    sdd_obj_t NEARPTR manager;
    sdd_obj_t NEARPTR bf[1];
  } fd_tab_t;

#define SCIO_INIT(manager_name,plid,maxfd) \
  do{\
    fd_tab_t NEARPTR fd_tab = \
      (fd_tab_t NEARPTR )sc_msgAllocClr(sizeof(fd_tab_t) + \
	 		      (maxfd) * sizeof(sdd_obj_t*),\
			      SCIO_MAGIC,(plid),SC_FATAL_IF_TMO);\
    fd_tab->max_fd = maxfd;\
    fd_tab->manager = sdd_manGetRoot (manager_name, "/", plid, \
	  			       SC_FATAL_IF_TMO);\
    sc_procVarSet(SCIO_PROCVAR_ID,(sc_var_t)fd_tab);\
  }while(0)

#define SCIO_RELEASE \
  do { \
    int i; \
    fd_tab_t NEARPTR fd_tab; \
    if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR ) & fd_tab)) { \
      if (fd_tab && fd_tab->magic == SCIO_MAGIC) { \
	if (fd_tab->hostent) { \
	  sc_msgFree ((sc_msgptr_t) &fd_tab->hostent); \
	}\
	if (fd_tab->protoent) { \
	  sc_msgFree ((sc_msgptr_t) &fd_tab->protoent); \
	}\
	if (fd_tab->pwd) { \
	  sc_msgFree ((sc_msgptr_t) &fd_tab->pwd); \
	}\
	if (fd_tab->manager) { \
	  sdd_objFree (&fd_tab->manager); \
	}\
	for (i = 0; i < fd_tab->max_fd; i++) { \
	  if (fd_tab->bf[i]) { \
	    sdd_objFree (&fd_tab->bf[i]); \
	  }\
	}\
      } \
    } \
  }while(0)

  fd_tab_t NEARPTR fdTabDuplicate (void);
  
#define SCIO_HAS_FDPRINTF
  int fdprintf(int fd, const char *fmt, ...);
  int objprintf(sdd_obj_t *con, const char *fmt, ...);

#define scio_stdin 0
#define scio_stdout 1
#define scio_stderr 2

#ifdef __cplusplus
}
#endif
#endif	/* !__SCIO_H__ */
/*
** $Log: scio.h,v $
** Revision 1.56  2008/01/16 13:01:59  bastian
** merge from 1.9.1.12
**
** Revision 1.55.4.2  2007/07/24 14:21:04  bastian
** fixes
**
** Revision 1.55.4.1  2007/07/24 09:06:05  bastian
** added fdprintf and objprintf
**
** Revision 1.55  2006/06/14 14:53:05  bastian
** xxx
**
** Revision 1.54  2005/07/01 11:28:23  bastian
** merge from branch-1.7.2
**
** Revision 1.53.2.1  2005/05/23 11:36:40  bastian
** m16ciar added
**
** Revision 1.53  2005/03/21 07:29:21  bastian
** changed type of max_fd to fix warings (2^31 fdS are enough)
**
** Revision 1.52  2004/12/20 15:48:31  bastian
** re-wind
**
** Revision 1.51  2004/12/20 15:47:29  bastian
** added IAR ARM support
**
** Revision 1.50  2004/12/14 09:10:11  christian
** Remove __GNUC__ for nonexisting includes, because this includes no exist.
**
** Revision 1.49  2004/10/07 07:37:41  bastian
** fixed NEAR * to NEARPTR
**
** Revision 1.48  2004/10/04 12:24:40  bastian
** fixes for m16c
**
** Revision 1.47  2004/04/15 10:43:06  christian
** Remove some leftovers.
**
** Revision 1.46  2003/11/19 10:56:28  christian
** Changed netBuffer to netbuf also for the netbuf functions.
**
** Revision 1.45  2003/09/08 07:57:15  christian
** ips_device -> ips_dev.
**
** Revision 1.44  2003/09/08 07:07:29  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 1.43  2003/08/05 11:15:16  bastian
** fixed header
**
** Revision 1.42  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 1.41  2003/07/30 05:31:46  christian
** sdd_manager -> sdd_man
** SDD_MANAGER -> SDD_MAN
**
** Revision 1.40  2003/07/29 14:43:53  christian
** Manager changes. And is allways changing.
**
** Revision 1.39  2003/07/03 13:26:32  bastian
** fixed casting
**
** Revision 1.38  2003/06/05 12:30:22  christian
** Fixed bind and added a missing include to scio.h
**
** Revision 1.37  2003/03/14 14:51:53  bastian
** fixes for ADS
**
** Revision 1.36  2003/03/11 14:58:30  bastian
** added header
**
** Revision 1.35  2003/02/17 13:23:36  bastian
** fixed release
**
** Revision 1.34  2003/02/13 09:05:08  bastian
** fixed SCIO_RELEASE
**
** Revision 1.33  2003/01/07 16:09:06  christian
** Added getprotobyname.
**
** Revision 1.32  2002/12/23 11:17:41  christian
** Handle background process with read requests.
**
** Revision 1.31  2002/11/01 12:50:13  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 1.30  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 1.29  2002/11/01 09:06:36  christian
** Intermediate.
**
** Revision 1.28  2002/10/29 14:35:33  christian
** Various bugfixes. Better error handling. Added new helper functions to make
** life easier.
**
** Revision 1.27  2002/10/07 15:32:32  christian
** Add include/arpa/inet.h include/netinet/in.h and src/inet.c
**
** Revision 1.26  2002/10/03 15:13:42  christian
** Started with the netdb stuff (gethostbyname, gethostbyaddr, ...).
**
** Revision 1.25  2002/09/23 12:50:38  christian
** Incr/Decr can cause a resource dead lock in some special cases.
** --> replace Incr/Decr with Release and Dublicate (dup, dup2) to be
**     able to handle on the fly objects (files/directories in a filesystem)
**
** Revision 1.24  2002/09/16 12:23:28  christian
** Rename sdd_handle_t to sdd_obj_t (prepare for futur changes)
**
** Revision 1.23  2002/09/05 14:30:19  christian
** Fixed ioctl
**
** Revision 1.22  2002/09/03 15:11:46  christian
** Make use of the newlib.
**
** Revision 1.21  2002/09/02 14:29:04  christian
** Rewritten scio because of the completly new bios strategy.
**
** Revision 1.20  2002/05/10 12:42:16  christian
** fixed syscall pipe, dup and dup2.
**
** Revision 1.19  2002/04/18 15:24:12  christian
** Link is a fs feature.
**
** Revision 1.18  2002/04/18 12:35:00  christian
** chroot and unlink is now part of the scio package.
**
** Revision 1.17  2002/04/02 15:16:00  bastian
** changed DIR
**
** Revision 1.16  2002/03/28 10:53:07  christian
** Added stuff to the struct DIR
**
** Revision 1.15  2002/03/27 12:36:19  christian
** Add readdir, dirent, rewinddir.
**
** Revision 1.14  2002/03/27 09:27:47  bastian
** more Error checking
**
** Revision 1.13  2002/03/26 10:39:34  christian
** DIR is now a own struct.
**
** Revision 1.12  2002/03/25 11:05:15  bastian
** Added getManager
**
** Revision 1.11  2002/03/22 16:36:17  bastian
** added ownerget
** protection for stdio in scio
**
** Revision 1.10  2002/03/15 16:13:47  christian
** Add opendir function.
**
** Revision 1.9  2002/03/15 15:57:49  christian
** Added a helpful methode to resolve path names for rcsman.
**
** Revision 1.8  2002/03/14 10:39:02  christian
** Made getDevice more general and made it global accessible.
**
** Revision 1.7  2002/03/06 16:19:50  bastian
** Changes for manager
**
** Revision 1.6  2002/03/06 08:56:16  bastian
** .
**
** Revision 1.5  2002/03/05 14:05:10  bastian
** All bios function prototypes now.
**
** Revision 1.4  2002/02/25 07:35:05  bastian
** Added comments.
**
** Revision 1.3  2002/02/18 07:31:38  bastian
** renamed macro and added error-checking
**
** Revision 1.2  2002/01/17 13:42:43  christian
** Removed all unused stuff.
**
** Revision 1.1  2002/01/17 13:30:24  christian
** Started with filedescriptors.
**
**
**
**/
