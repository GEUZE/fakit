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
** ID: S03070CL63                                                   **
** +Revision: 1.9 +                                                 **
** +Date: 2003/10/30 08:26:44 +                                     **
** netdb.h                                                          **
**********************************************************************
EOC*/
#ifndef _NETDB_H
#define _NETDB_H	1

#ifdef __cplusplus
extern "C" {
#endif

  struct hostent {
    char *h_name;		/* official name of host */
    char **h_aliases;		/* alias list */
    int h_addrtype;		/* host address type */
    int h_length;		/* length of address */
    char **h_addr_list;		/* list of addresses */
#define h_addr  h_addr_list[0]
  };

  struct hostent *gethostbyname (const char *name);
  struct hostent *gethostbyaddr (const char *addr, int len, int type);
  void sethostent (int stayopen);
  void endhostent (void);

  struct protoent {
    char *p_name;		/* official protocol name */
    char **p_aliases;		/* alias list */
    int p_proto;		/* protocol number */
  };
 
  struct protoent *getprotobyname (const char *name);
  struct protoent *getprotobynumber (int proto);

#ifdef __cplusplus
}
#endif
#endif				/* !_NETDB_H */
/*
** $Log: netdb.h,v $
** Revision 1.9  2003/10/30 08:26:44  christian
** Bugfix.
**
** Revision 1.8  2003/09/08 07:57:15  christian
** ips_device -> ips_dev.
**
** Revision 1.7  2003/09/08 07:07:29  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 1.6  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 1.5  2003/03/11 14:58:30  bastian
** added header
**
** Revision 1.4  2003/01/07 16:09:06  christian
** Added getprotobyname.
**
** Revision 1.3  2002/11/01 12:50:13  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 1.2  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 1.1  2002/10/03 15:13:42  christian
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
