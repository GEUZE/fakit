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
** (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
**                                                                  **
**********************************************************************
** ID: S03217CL2                                                    **
** +Revision: 1.28 +                                                 **
** +Date: 2006/12/06 11:26:39 +                                     **
** Internal SFS functions.                                          **
**********************************************************************
EOC*/

#ifndef _SFS_SFS_H
#define _SFS_SFS_H

#ifdef __cplusplus
extern "C" {
#endif


#include <ossys/defines.h>
#include <ossys/types.h>

#include <sdd/sdd.msg>

  /**
   * Assgin a device to a filesystem.
   * \param[in]  root        Filesystem object descriptor.
   * \param[in]  dev         A device object descriptor.
   * \return                 -1 on error.
   */
  int sfs_assign (sdd_obj_t NEARPTR root, sdd_obj_t NEARPTR NEARPTR dev);

  /**
   * Unassgin a device from a filesystem.
   * \param[in]  root        Filesystem object descriptor.
   * \return                 Return device or NULL on error.
   */
  sdd_obj_t NEARPTR sfs_unassign (sdd_obj_t NEARPTR root);

  /**
   * Mount a manager object to an mount point directory. 
   * \param[in]  mountpoint  A directory object descriptor.
   * \param[in]  dir         A manager object descriptor.
   * \return                 -1 on error.
   */
  int sfs_mount (sdd_obj_t NEARPTR mountpoint, sdd_obj_t NEARPTR NEARPTR );
  
  /**
   * Unmount a directory object on a mount point.
   *!!!Attention!!! only real filesystem manager can be unmounted, so be
   * carefull in mounting pure managers like devman.
   * \param[in]  mountpoint  A directory object descriptor.
   * \return                 -1 on error.
   */ 
  int sfs_umount (sdd_obj_t NEARPTR mountpoint);
  
  /**
   * Sync a filesystem with its mounted device.
   * \param[in]  root        Filesystem object descriptor.
   * \return                 -1 on error.
   */ 
  int sfs_sync (sdd_obj_t NEARPTR root);

  /**
   * Move a file or a directory to an other place or just rename the file.
   * \param[in]  root        Filesystem object descriptor.
   * \param[in]  oldpath     Old path
   * \param[in]  newpath     New path
   * \return                 -1
   */
  int sfs_move (sdd_obj_t NEARPTR root, const char * oldpath, 
                const char * newpath);

  /**
   * Copy a file or a directory.
   * \param[in]  root        Filesystem object descriptor.
   * \param[in]  oldpath     Old path
   * \param[in]  newpath     New path
   * \return                 -1
   */
  int sfs_copy (sdd_obj_t NEARPTR root, const char * oldpath, 
                const char * newpath);

  /**
   * Get a object by its name of given type.
   * \param[in]  dir         Starting point
   * \param[in]  name        Path of object
   * \param[in]  type        0 means any type, else given type, must match
   *                         exactly.
   * \return                 NULL on error.
   */
  sdd_obj_t NEARPTR sfs_get (sdd_obj_t NEARPTR dir, const char * name, 
                             sc_msgid_t type);

  /**
   * Delete a object.
   * \param[in]  root        Starting point
   * \param[in]  path        Path of object
   * \return                 -1 on error.
   */
  int sfs_delete (sdd_obj_t NEARPTR root, const char * path);

  /**
   * Create a object of given type.
   * \param[in]  dir         Starting point
   * \param[in]  name        Name of object
   * \param[in]  type        Type of object, SFS_ATTR_FILE and SFS_ATTR_DIR are
   *                         are the only two valid types.
   * \param[in]  mode        Not implemented for every FS.
   * \return                 -1 on error.
   */
  int sfs_create (sdd_obj_t NEARPTR dir, const char * name, sc_msgid_t type,
                  mode_t mode);

  /**
   * Get and open a object of type SDD_DEV_TYPE.
   * \param[in]  dir         Starting point
   * \param[in]  name        Name of object.
   * \param[in]  flags       Flags: O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, ...
   * \return                 NULL on error.
   */ 
  sdd_obj_t NEARPTR sfs_open (sdd_obj_t NEARPTR dir, const char * name, 
                              flags_t flags);

  /**
   * Wrapper for: sdd_devClose, sdd_objFree.
   */ 
  int sfs_close (sdd_obj_t NEARPTR NEARPTR file);

  /**
   * Wrapper for: sdd_devRead.
   */
  ssize_t sfs_read (sdd_obj_t NEARPTR file, void * buf, size_t size);  

  /**
   * Wrapper for: sdd_devWrite.
   */
  ssize_t sfs_write (sdd_obj_t NEARPTR file, const void * buf, size_t size);  

  /**
   * Wrapper for: sdd_fileSeek.
   */
  off_t sfs_seek (sdd_obj_t NEARPTR file, off_t off, flags_t whence);  

  /**
   * Wrapper for: sdd_fileResize.
   */
  ssize_t sfs_resize (sdd_obj_t NEARPTR file, size_t size);  

  /*
  ** Find files with a given mask.
  */
  sdd_obj_t NEARPTR sfs_findFirst (sdd_obj_t NEARPTR dir, const char *mask);

#ifdef __cplusplus
}
#endif
#endif

