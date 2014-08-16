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
** ID: S04107CL14
** +Revision: 1.3 +
** +Date: 2008/01/16 13:02:09 +
** Open a file 
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sys/fcntl.h>
#include <sdd/sdd.h>
#include <sfs/sfs.h>

sdd_obj_t NEARPTR
sfs_open (sdd_obj_t NEARPTR dir, const char *name, flags_t flags) 
{
  sdd_obj_t * file;

  file = sfs_get (dir, name, SFS_ATTR_FILE);
  
  if (!file && flags & O_CREAT) {
    flags &= ~O_CREAT;
    if (sfs_create (dir, name, SFS_ATTR_FILE, 0) != -1) {
      file = sfs_get (dir, name, SFS_ATTR_FILE);
    }
  }
  
  if (!file || sdd_devOpen (file, flags) != -1) {
    return file;
  }
  else {
    sdd_objFree (&file);
  }	

  return file;
}

