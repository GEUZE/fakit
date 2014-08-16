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
** ID: S03217CL4                                                    **
** +Revision: 1.27 +                                                 **
** +Date: 2008/02/14 14:15:49 +                                     **
** SFS message interface.                                           **
**********************************************************************
EOC*/

/*
** backend of the vfs
*/

#ifndef _SFS_SFS_MSG_
#define _SFS_SFS_MSG_

EXTERN_C_BEGIN

#include <ossys/defines.h>
#include <sdd/sdd.msg>

#define SFS_ASSIGN		(SFS_MSG_BASE + 2)
#define SFS_ASSIGN_REPLY	(SFS_ASSIGN + 1)
#define SFS_UNASSIGN		(SFS_MSG_BASE + 4)
#define SFS_UNASSIGN_REPLY	(SFS_UNASSIGN + 1)
typedef struct sfs_assign_s {
  sdd_obj_t object;
} sfs_assign_t;
  
#define SFS_OBSERVE		(SFS_MSG_BASE + 6)
#define SFS_OBSERVE_REPLY	(SFS_OBSERVE + 1)
typedef struct sfs_observe_s {
  sdd_baseMessage_t base;
} sfs_observe_t;
  
#define SFS_SYNC		(SFS_MSG_BASE + 8)
#define SFS_SYNC_REPLY		(SFS_SYNC + 1)
typedef struct sfs_sync_s {
  sdd_baseMessage_t base;
} sfs_sync_t;

#define SFS_RENAME		(SFS_MSG_BASE + 10)
#define SFS_RENAME_REPLY	(SFS_RENAME + 1)
typedef struct sfs_rename_s {
  sdd_obj_t object;
  char newname[SC_NAME_MAX + 1];
} sfs_rename_t;
  
#define SFS_MOUNT		(SFS_MSG_BASE + 12)
#define SFS_MOUNT_REPLY		(SFS_MOUNT + 1)
typedef struct sfs_mount_s {
  sdd_obj_t obj;
} sfs_mount_t;

#define SFS_UMOUNT		(SFS_MSG_BASE + 14)
#define SFS_UMOUNT_REPLY	(SFS_UMOUNT + 1)
typedef struct sfs_umount_s {
  sdd_obj_t obj;
} sfs_umount_t;

#define SFS_ATTR_SET		(SFS_MSG_BASE + 16)
#define SFS_ATTR_SET_REPLY	(SFS_ATTR_SET + 1)
#define SFS_ATTR_GET		(SFS_MSG_BASE + 18)
#define SFS_ATTR_GET_REPLY	(SFS_ATTR_GET + 1)
typedef struct sfs_attr_s {
  sdd_baseMessage_t base;
  __u32 data;
} sfs_attr_t;

#define SFS_FORMAT              (SFS_MSG_BASE + 20)
#define SFS_FORMAT_REPLY        (SFS_MSG_BASE + 21)
#define SFS_FORMAT_FORCE        (SFS_MSG_BASE + 22)
#define SFS_FORMAT_FORCE_REPLY  (SFS_MSG_BASE + 23)
typedef struct sfs_format_s {
  sdd_baseMessage_t base;
  int custom;
} sfs_format_t;

/*
** The real message depends on the filesystem
*/
#define SFS_MOUNT_POINT         (SFS_MSG_BASE + 24)
typedef struct sfs_mount_point_s {
  sc_msgid_t id;
  int custom;
} sfs_mount_point_t;

/*
** The real message depends on the filesystem
*/
#define SFS_FILE                (SFS_MSG_BASE + 25)
typedef struct sfs_file_s {
  sc_msgid_t id;
  int custom;
} sfs_file_t;

#define SFS_FFIRST              (SFS_MSG_BASE + 26)
#define SFS_FFIRST_REPLY        (SFS_MSG_BASE + 27)
typedef struct sfs_ffirst_s {
  sdd_obj_t object;
} sfs_ffirst_t;

EXTERN_C_END
#endif

