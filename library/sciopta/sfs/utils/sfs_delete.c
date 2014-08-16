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
** ID: S04107CL13
** +Revision: 1.5 +
** +Date: 2008/01/16 13:02:09 +
** Delete a file 
**********************************************************************
EOC*/

#include <sciopta.h>
#include <errno.h>
#include <sdd/sdd.h>
#include <sfs/sfs.h>

#include <string.h>

int 
sfs_delete (sdd_obj_t NEARPTR root, const char * path)
{
  char *ptr;
  sdd_obj_t *dir;
  sdd_obj_t *obj;
  char *dirpath;
  int err;

  sc_miscErrnoSet (0);

  /*
  ** relative path needs a root directory
  */
  if (path[0] != '/' && !root) {
    sc_miscErrnoSet (EBADF);
    return -1;
  }

  if ( !root ){
    root = sdd_manGetRoot ("/SCP_devman", 
			   "/", 
			   SC_DEFAULT_POOL,
			   SC_FATAL_IF_TMO);
    if ( !root ){
      sc_miscErrnoSet (ENODEV);
      return -1;
    }
  }

  /* get end of path */
  ptr = strrchr (path, '/');

  /* check path */
  if (ptr && ptr[1] == '\0') {
    /* there is nothing do be removed */
    sc_miscErrnoSet (ENOENT);
    return -1;
  }

  if ( ptr ) {
    /*
    ** First find the manager (parent-dir of file)
    */
    dirpath = (char *) sc_msgAlloc (sizeof (sc_msgid_t) + strlen (path), 
				    0,
				    SC_DEFAULT_POOL, 
				    SC_FATAL_IF_TMO);
    
    strncpy (dirpath, path, ptr - path);
    dirpath[ptr - path] = '\0';
    dir = sfs_get (root, dirpath, SFS_ATTR_DIR);
    sc_msgFree((sc_msgptr_t)&dirpath);

    if (!dir) {
      return -1;
    }
    /*
    ** Now get the object for the file.
    */
    obj = sdd_manGetByName (dir, ptr+1);
  } else {
    /*
    ** No path, use root-manager directly.
    */
    dir = root;
    obj = sdd_manGetByName (root, path);
  }

  err = -1;

  if ( obj ){
    sdd_manRm (dir, obj, sizeof (sdd_obj_t));
    sdd_objFree (&obj);
    err = 0;
  }
  if ( dir != root ){
    sdd_objFree (&dir);
  }

  return err;
}
