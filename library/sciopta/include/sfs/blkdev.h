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
** ID: S03217CL3                                                    **
** +Revision: 1.9 +                                                 **
** +Date: 2004/07/30 08:04:46 +                                     **
** Blockdevice message interface.                                   **
**********************************************************************
EOC*/

#ifndef _SFS_BLKDEV_H
#define _SFS_BLKDEV_H

#ifdef __cplusplus
extern "C" {
#endif


#include <ossys/defines.h>
#include <ossys/types.h>

#include <container/container.h>
  
#include <sdd/sdd.msg>

#define BLKDEV_SECSIZE	512

  /* ioctl for block devices */
  typedef struct blkdev_geometry_s {
    sc_msgid_t id;
    int size;
    int tracks;
    int heads;
    int sects;
  } blkdev_geometry_t;
  
  /* the next generation bread, bwrite */
  typedef struct sfs_blkDev {
    sc_msgid_t id;
    sc_ticks_t tmo;
    sc_poolid_t plid;
    size_t cachesize;
    sdd_obj_t *dev;
    container_t *cache;
  } sfs_blkDev_t;

  /** constructor */
  sfs_blkDev_t *sfs_blkDevNew (sdd_obj_t * dev, sc_poolid_t plid,
			       sc_ticks_t tmo, size_t cachsize);

  /** destructor */
  void sfs_blkDevDestroy (sfs_blkDev_t ** dev);

  /** methodes */
  int sfs_blkDevRead (sfs_blkDev_t * blkdev, __u8 *buf, off_t sec,
		      size_t count);
  int sfs_blkDevWrite (sfs_blkDev_t * blkdev, const __u8 *buf,
		       off_t sec, size_t count);
  off_t sfs_blkDevSeek (sfs_blkDev_t * blkdev, off_t offset, int whence);
  void sfs_blkDevSync (sfs_blkDev_t * blkdev);

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: blkdev.h,v $
** Revision 1.9  2004/07/30 08:04:46  christian
** Realy tight integration, also the hcc driver are now capsulated cleanly
** in a real sciopta device :-)
**
** Revision 1.8  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 1.7  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 1.6  2003/08/05 05:53:53  bastian
** Added header
**
** Revision 1.5  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 1.4  2003/03/18 09:14:57  bastian
** fixed char,unsigned char and __u8
**
** Revision 1.3  2002/12/13 16:42:57  christian
** Finished the port of mkdosfs (named here as mkfatfs).
**
** Revision 1.2  2002/12/06 14:31:05  christian
** EFS is now SFS and efs is now sfs
**
** Revision 1.1.1.1  2002/12/06 10:54:27  christian
** first revision based on new sdd
**
** Revision 1.1  2002/11/15 14:31:07  christian
** Reimplementation of the embedded file system based on the new sciopta
** device driver conception (new "bios").
**
**
**/
