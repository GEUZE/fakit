/* -*-c++-*- 
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
**              (c) 2008 SCIOPTA Systems AG/ Schweiz                **
**                                                                  **
**********************************************************************
** ID: S08035BS1                                                    **
** +Revision: 1.9 +                                                **
** +Date: 2008/11/11 09:58:53 +                                     **
** HCC filesystem specific messages                                 **
**********************************************************************
EOC*/

#ifndef _HCC_FS_MSG_
#define _HCC_FS_MSG_ 1

#include <sciopta.h>
#include <sdd/sdd.msg>
#include <sdd/sdd.h>

#define HCC_EFFS_MOUNT       (HCC_MSG_BASE + 1)
#define HCC_EFFS_MOUNT_REPLY (HCC_MSG_BASE + 2)
typedef struct hcc_effs_mount_s {
  sc_msgid_t id;
  sc_errcode_t error;
  char mp[SC_NAME_MAX+1];
  unsigned int drive;

  void *buffer;
  long bufsize;
  void *mountFunc;
  void *phyFunc;
} hcc_effs_mount_t;

#define HCC_FATFS_MOUNT       (HCC_MSG_BASE + 3)
#define HCC_FATFS_MOUNT_REPLY (HCC_MSG_BASE + 4)
typedef struct hcc_fatfs_mount_s {
  sc_msgid_t id;
  sc_errcode_t error;
  char mp[SC_NAME_MAX+1];
  unsigned int drive;

  sdd_obj_t NEARPTR dev;
} hcc_fatfs_mount_t;
  
#define HCC_FS_FINDFIRST       (HCC_MSG_BASE + 5)
#define HCC_FS_FINDFIRST_REPLY (HCC_MSG_BASE + 6)
typedef struct hcc_fs_findfirst_s {
  sc_msgid_t id;
  sc_errcode_t error;
  sdd_obj_t NEARPTR cur;
  size_t size;
  sdd_obj_t NEARPTR dir;
  char mask[SC_NAME_MAX+1];
} hcc_fs_findfirst_t;


#define HCC_FS_FINDNEXT       (HCC_MSG_BASE + 7)
#define HCC_FS_FINDNEXT_REPLY (HCC_MSG_BASE + 8)
typedef struct hcc_fs_findnext_s {
  sc_msgid_t id;
  sc_errcode_t error;
  sdd_obj_t NEARPTR cur;
  size_t size;
} hcc_fs_findnext_t;

#define HCC_FS_MOUNTP           (HCC_MSG_BASE + 9)
#define HCC_FS_DESC             (HCC_MSG_BASE + 10)
#define HCC_FATFS_MALLOC        (HCC_MSG_BASE + 11)

/* The same as hcc_fatfs_mount_t only one element added at end */
#define HCC_FATFS_PARTITION_MOUNT       (HCC_MSG_BASE + 13)
#define HCC_FATFS_PARTITION_MOUNT_REPLY (HCC_MSG_BASE + 14)
typedef struct hcc_fatfs_partmount_s {
  sc_msgid_t id;
  sc_errcode_t error;
  char mp[SC_NAME_MAX+1];
  unsigned int drive;

  sdd_obj_t NEARPTR dev;

  int partition;
} hcc_fatfs_partmount_t;

#endif
