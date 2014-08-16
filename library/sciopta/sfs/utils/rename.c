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
** ID: S03094CL2                                                    **
** +Revision: 1.14 +                                                **
** +Date: 2004/12/14 09:10:12 +                                     **
** Rename two files moving it between directories if required       **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <fcntl.h>
#include <errno.h>
#include <scio.h>
#include <sdd/sdd.h>
#include <sfs/sfs.h>
#include <string.h>

int
rename (const char *oldpath, const char *newpath)
{
  fd_tab_t *fd_tab;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t *) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC) {
      return sfs_move (fd_tab->manager, oldpath, newpath); 
    }
  }

  sc_miscErrnoSet (EMFILE);
  return -1;
}

/*
** $Log: rename.c,v $
** Revision 1.14  2004/12/14 09:10:12  christian
** Remove __GNUC__ for nonexisting includes, because this includes no exist.
**
** Revision 1.13  2004/04/14 13:26:24  christian
** Clean up of helper functions.
**
** Revision 1.12  2004/01/05 08:07:37  christian
** Some changes.
**
** Revision 1.11  2003/12/12 11:43:52  christian
** Some work on renaming a file.
**
** Revision 1.10  2003/11/10 12:39:26  christian
** Shorter name for DIRECTORY to DIR.
**
** Revision 1.9  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 1.8  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 1.7  2003/08/11 11:00:38  bastian
** fixed procVar parameter cast
**
** Revision 1.6  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 1.5  2003/07/30 05:31:47  christian
** sdd_manager -> sdd_man
** SDD_MANAGER -> SDD_MAN
**
** Revision 1.4  2003/07/29 13:18:23  bastian
** added FIXME
**
** Revision 1.3  2003/04/07 11:35:54  christian
** Bugfixes (memory leaks, NULL pointer and other stuff).
**
** Revision 1.2  2003/04/04 15:01:29  christian
** Finished rename function.
**
** Revision 1.1  2003/04/04 10:01:18  christian
** Started with sys call rename
**
** Revision 1.43  2003/04/01 13:17:52  christian
** Fixed sync prototype.
** Have a look if we have a / in the file name.
**
** Revision 1.42  2003/03/18 08:34:32  bastian
** char __u8 and unsigned char
**
** Revision 1.41  2003/03/14 14:55:44  bastian
** more fixes
**
** Revision 1.40  2003/03/14 14:51:53  bastian
** fixes for ADS
**
** Revision 1.39  2003/03/11 14:58:30  bastian
** added header
**
** Revision 1.38  2003/02/18 16:16:58  christian
** Open create should be possible now.
**
** Revision 1.37  2003/02/14 10:19:06  bastian
** fixed memory-leaks
**
** Revision 1.36  2002/12/23 11:17:41  christian
** Handle background process with read requests.
**
** Revision 1.35  2002/12/06 14:19:35  christian
** Release used manager after open the wished file.
**
** Revision 1.34  2002/11/01 12:50:13  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 1.33  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 1.32  2002/11/01 09:06:36  christian
** Intermediate.
**
** Revision 1.31  2002/10/29 14:35:33  christian
** Various bugfixes. Better error handling. Added new helper functions to make
** life easier.
**
** Revision 1.30  2002/10/18 15:28:59  christian
** Cleaner implementation of connect, bind, listen, accept.
** A cleaner error handling.
**
** Revision 1.29  2002/10/02 14:48:38  christian
** Head toward the POSIX sockets functions.
**
** Revision 1.28  2002/09/17 09:56:20  christian
** Bios interface has changed
**
** Revision 1.27  2002/09/16 12:23:28  christian
** Rename sdd_handle_t to sdd_obj_t (prepare for futur changes)
**
** Revision 1.26  2002/09/06 15:01:27  christian
** sdd_resolve changed to sdd_objResolve
**
** Revision 1.25  2002/09/03 15:11:46  christian
** Make use of the newlib.
**
** Revision 1.24  2002/09/02 14:29:04  christian
** Rewritten scio because of the completly new bios strategy.
**
** Revision 1.23  2002/05/30 13:04:20  christian
** .
**
** Revision 1.22  2002/05/29 13:51:52  christian
** Made open able to give back a copy of itself. If open a device allways
** use the device which is returned by open.
**
** Revision 1.21  2002/05/28 14:22:08  christian
** Check for clone and if clone check if clone is allowed.
**
** Revision 1.20  2002/05/24 09:46:11  christian
** incr and decr now clean and ready.
**
** Revision 1.19  2002/04/17 13:07:45  christian
** Increment manager at the beginning of the getObject is not needed,
** because of the new decrement strategie.
**
** Revision 1.18  2002/04/17 12:49:04  christian
** Patched the getObject and the getManager. "/" is threaded as an exception.
**
** Revision 1.17  2002/04/12 11:23:51  christian
** Cleaned some mistery codesequences.
**
** Revision 1.16  2002/04/10 13:11:47  christian
** Create files implemented.
**
** Revision 1.15  2002/04/04 10:14:12  bastian
** simplyfied
**
** Revision 1.14  2002/03/25 11:02:08  bastian
** Added getManager
**
** Revision 1.13  2002/03/22 16:36:38  bastian
** intermediate
**
** Revision 1.12  2002/03/20 16:23:08  christian
** Fixed bad namings...
**
** Revision 1.11  2002/03/19 14:51:33  christian
** Add a better isA methode on sc_object_t. Perhaps there is a better
** solution?!
**
** Revision 1.10  2002/03/18 14:24:14  christian
** Fixed the no-device-case.
**
** Revision 1.9  2002/03/14 10:39:02  christian
** Made getDevice more general and made it global accessible.
**
** Revision 1.8  2002/03/07 09:57:55  bastian
** Added test for manager
**
** Revision 1.7  2002/03/07 08:29:40  christian
** open incr/decr manager to get ride of the references.
**
** Revision 1.6  2002/03/06 13:15:45  bastian
** added path traversal
**
** Revision 1.5  2002/03/05 13:30:46  bastian
** New sdd/device/resource structure introduced.
** Now a rcsman handles all requests for SCOBJ objects.
** scio functions check if a SCOBJ is a sdd_dev.
**
** Revision 1.4  2002/03/04 14:35:57  bastian
** Redesign for new BSP concept
**
** Revision 1.3  2002/02/18 07:38:21  bastian
** cleaned up
**
** Revision 1.2  2002/01/31 12:21:15  christian
** Strong sciopta naming and GNU style formating.
**
** Revision 1.1.1.1  2002/01/17 13:43:13  christian
** frist revision
**
**
**
*/
