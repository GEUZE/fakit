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
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S04106CL6 
** +Revision: 1.17 +
** +Date: 2008/01/16 13:02:08 +
** blkdev.c 
**********************************************************************
EOC*/

#include <sciopta.h>
#include <container/list.h>

#include <sdd/sdd.h>
#include <sfs/sfs.h>
#include <sfs/blkdev.h>

#include <string.h>
#include <unistd.h>
#include <errno.h>

/* the next generation bread, bwrite */
union sc_msg {
  sc_msgid_t id;
};

typedef struct sfs_blkbuf {
  sc_msgid_t sec;
  flags_t flags;
  sc_ticks_t atime;
#define SFS_BLKDEV_CLEAN 	0
#define SFS_BLKDEV_DIRTY 	1
  __u8 buf[1];
} sfs_blkbuf_t;

static int
cacheCmp (const void *left, const void *right)
{
  return ((sc_msg_t ) left)->id - ((sc_msg_t ) right)->id;
}

static void
cacheDel (void *entry)
{
  sc_msgFree ((sc_msgptr_t ) &entry);
}

sfs_blkDev_t *
sfs_blkDevNew (sdd_obj_t NEARPTR  dev, sc_poolid_t plid, sc_ticks_t tmo,
	       size_t cachesize)
{
  sfs_blkDev_t *tmp;

  if (!dev) {
    sc_miscErrnoSet (ENODEV);
    return NULL;
  }

  PRINTF1 ("sfs_blkDevNew for %s\n", dev->name);

  tmp = (sfs_blkDev_t *) sc_msgAlloc (sizeof (sfs_blkDev_t), 0, plid, tmo);
  if (tmp) {
    tmp->tmo = tmo;
    tmp->plid = plid;
    tmp->cachesize = cachesize;
    tmp->dev = dev;
    tmp->cache =
      (container_t *) list_new (cacheCmp, cacheDel, SC_DEFAULT_POOL,
				SC_FATAL_IF_TMO);
  }
  else {
    sc_miscErrnoSet (ENOMEM);
    return NULL;
  }

  return tmp;
}

void
sfs_blkDevDestroy (sfs_blkDev_t ** blkdev)
{
  if ((*blkdev)) {
    (*blkdev)->cache->ops->destroy (&(*blkdev)->cache);
    sc_msgFree ((sc_msgptr_t ) blkdev);
  }
}

int
sfs_blkDevRead (sfs_blkDev_t * blkdev, __u8 *buf, off_t sec, size_t count)
{
  union sc_msg ref;
  sfs_blkbuf_t *tmp;

  if (!blkdev) {
    sc_miscErrnoSet (ENODEV);
    return 0;
  }

  PRINTF1 ("sfs_blkDevRead for %s\n", blkdev->dev->name);

  /* get the buffer check first the cache */
  ref.id = sec;
  tmp = blkdev->cache->ops->get (blkdev->cache, &ref);
  if (!tmp) {
    PRINTF1 ("sfs_blkDevRead sector %d is not in the cache\n", sec);
    tmp =
      (sfs_blkbuf_t *) sc_msgAlloc (sizeof (sfs_blkbuf_t) +
				    count * BLKDEV_SECSIZE, sec, blkdev->plid,
				    blkdev->tmo);
    if (tmp) {
      off_t off;
      off = sec * BLKDEV_SECSIZE;
      tmp->flags = SFS_BLKDEV_CLEAN;
      if ((sdd_fileSeek (blkdev->dev, off, 0) == off)
	  && (sdd_devRead (blkdev->dev,tmp->buf,count * BLKDEV_SECSIZE)
	      >= 0)) {
	if (blkdev->cache->ops->noOfItems (blkdev->cache) > blkdev->cachesize) {
	  sfs_blkDevSync (blkdev);
	}
	blkdev->cache->ops->add (blkdev->cache, tmp);
      }
      else {
	sc_miscErrnoSet (EIO);
	return 0;
      }
    }
    else {
      sc_miscErrnoSet (ENOMEM);
      return 0;
    }
  }
  else {
    PRINTF1 ("sfs_blkDevRead sector %d is in the cache\n", sec);
  }

  tmp->atime = sc_tickGet ();
  memcpy (buf, tmp->buf, count * BLKDEV_SECSIZE);
  return 1;
}

int
sfs_blkDevWrite (sfs_blkDev_t * blkdev, const __u8 *buf, off_t sec,
		 size_t count)
{
  union sc_msg ref;
  sfs_blkbuf_t *tmp;

  if (!blkdev) {
    sc_miscErrnoSet (ENODEV);
    return 0;
  }

  PRINTF1 ("sfs_blkDevWrite for %s\n", blkdev->dev->name);

  /* get the buffer check first the cache */
  ref.id = sec;
  tmp = blkdev->cache->ops->get (blkdev->cache, &ref);
  if (!tmp) {
    PRINTF1 ("sfs_blkDevWrite sector %d is not in the cache\n", sec);
    tmp =
      (sfs_blkbuf_t *) sc_msgAlloc (sizeof (sfs_blkbuf_t) +
				    count * BLKDEV_SECSIZE, sec, blkdev->plid,
				    blkdev->tmo);
    if (tmp) {
      off_t off;
      off = sec * BLKDEV_SECSIZE;
      tmp->flags = SFS_BLKDEV_DIRTY;
      if ((sdd_fileSeek (blkdev->dev, off, 0) == off)
	  && (sdd_devRead (blkdev->dev,tmp->buf, count * BLKDEV_SECSIZE)
	      >= 0)) {
	if (blkdev->cache->ops->noOfItems (blkdev->cache) > blkdev->cachesize) {
	  sfs_blkDevSync (blkdev);
	}
	blkdev->cache->ops->add (blkdev->cache, tmp);
      }
      else {
	sc_miscErrnoSet (EINVAL);
	return 0;
      }
    }
    else {
      sc_miscErrnoSet (ENOMEM);
      return 0;
    }
  }
  else {
    PRINTF1 ("sfs_blkDevWrite sector %d is in the cache\n", sec);
  }
  tmp->flags = SFS_BLKDEV_DIRTY;
  tmp->atime = sc_tickGet ();
  memcpy (tmp->buf, buf, count * BLKDEV_SECSIZE);
  return 1;
}

void
sfs_blkDevSync (sfs_blkDev_t * blkdev)
{
  sfs_blkbuf_t *cur, *old;
  sc_ticks_t maxTime;

  if (blkdev) {

    PRINTF1 ("sfs_blkDevSync for %s\n", blkdev->dev->name);

    /* write all dirty buffer down and remove the oldest free buffer from 
       cache */
    old = NULL;
    maxTime = 0;
    cur = blkdev->cache->ops->getFirst (blkdev->cache);
    while (cur) {
      if (cur->flags == SFS_BLKDEV_DIRTY) {
	/* write it back to disk */
	off_t off;
	PRINTF ("sfs_blkDevSync write to disk\n");
	off = cur->sec * BLKDEV_SECSIZE;
	if (sdd_fileSeek (blkdev->dev, off, SEEK_SET) == off) {
	  sdd_devWrite (blkdev->dev,cur->buf, BLKDEV_SECSIZE);
	}
	cur->flags = SFS_BLKDEV_CLEAN;
      }
      /* get the one with the biggest idle time */
      if (sc_tickGet () - cur->atime > maxTime) {
	old = cur;
      }
      cur = blkdev->cache->ops->getNext (blkdev->cache, cur);
    }
    if (old
	&& blkdev->cache->ops->noOfItems (blkdev->cache) >=
	blkdev->cachesize) {
      old = blkdev->cache->ops->rm (blkdev->cache, old);
      if (old) {
	sc_msgFree ((sc_msgptr_t ) &old);
      }
      else {
	PRINTF ("error: sfs_blkDevSync could not remove cached object?!\n");
      }
    }
  }
  else {
    PRINTF ("error: sfs_blkDevSync no devcie\n");
  }
}

/*
** $Log: blkdev.c,v $
** Revision 1.17  2008/01/16 13:02:08  bastian
** merge from 1.9.1.12
**
** Revision 1.16.4.1  2007/09/07 04:55:21  bastian
** added comments
**
** Revision 1.16  2006/03/17 10:26:26  bastian
** fixed order of includes
**
** Revision 1.15  2006/03/17 06:35:55  bastian
** added const qualifier where possible
**
** Revision 1.14  2004/12/14 09:10:12  christian
** Remove __GNUC__ for nonexisting includes, because this includes no exist.
**
** Revision 1.13  2004/10/07 09:25:28  bastian
** NEAR * => NEARPTR
**
** Revision 1.12  2004/10/04 13:33:58  bastian
** fix for NEAR/FAR
**
** Revision 1.11  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 1.10  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 1.9  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 1.8  2003/04/01 13:18:49  christian
** Fixed strncmp of fatfs names (ignore upper/lower case).
** Added a GPL tag to GPL software.
**
** Revision 1.7  2003/03/18 09:14:57  bastian
** fixed char,unsigned char and __u8
**
** Revision 1.6  2003/03/18 07:30:38  bastian
** added ARM and fixed for ADS
**
** Revision 1.5  2003/03/17 08:11:44  christian
** Moved SEEK_SET, SEEK_CUR, SEEK_END to unistd
**
** Revision 1.4  2002/12/13 16:42:57  christian
** Finished the port of mkdosfs (named here as mkfatfs).
**
** Revision 1.3  2002/12/09 15:36:53  christian
** Indent formating
**
** Revision 1.2  2002/12/06 14:31:05  christian
** EFS is now SFS and efs is now sfs
**
** Revision 1.1.1.1  2002/12/06 10:54:27  christian
** first revision based on new sdd
**
** Revision 1.8  2002/11/15 14:51:54  christian
** Reorginized files.
**
** Revision 1.7  2002/11/15 14:31:07  christian
** Reimplementation of the embedded file system based on the new sciopta
** device driver conception (new "bios").
**
** Revision 1.6  2002/11/01 12:50:12  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 1.5  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 1.4  2002/04/04 15:52:41  christian
** A running lseek.
** have to reimplement the write methode.
**
** Revision 1.3  2002/04/04 06:44:54  bastian
** fixed write
**
** Revision 1.2  2002/04/03 06:54:03  christian
** A smarter sync methode.
**
** Revision 1.1  2002/04/02 15:12:03  christian
** First working fatfs with cache (in the middle block device with cache).
**
**
**
**
*/
