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
** ID: S03070CL2                                                    **
** +Revision: 1.39 +                                                **
** +Date: 2008/11/11 10:03:33 +                                     **
** File device                                                      **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sfs/blkdev.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#include <logd/logd.h>
#include <dev/fildev.h>

union sc_msg {
  sc_msgid_t id;
  sdd_obj_t object;
  sdd_baseMessage_t base;
  sdd_devOpen_t open;
  sdd_devRead_t read;
  sdd_devWrite_t write;
  sdd_devIoctl_t ioctl;
  sdd_fileSeek_t seek;
  sdd_fileResize_t resize;
};

typedef struct fildev_s {
  sc_msgid_t id;
  off_t pos;
} fildev_t;

static void
register_dev (const char *name, logd_t NEARPTR logd)
{
  /* registration */
  sdd_obj_t NEARPTR dev;
  sdd_obj_t NEARPTR man;

  dev = (sdd_obj_t NEARPTR) sc_msgAlloc (sizeof (sdd_obj_t), 0,
                                         SC_DEFAULT_POOL, SC_FATAL_IF_TMO);
  dev->base.error = 0;
  dev->base.handle = NEAR_NULL;
  dev->type = SDD_OBJ_TYPE | SDD_DEV_TYPE | SDD_FILE_TYPE;
  strncpy (dev->name, name, SC_NAME_MAX);
  dev->controller = dev->receiver = dev->sender =
    sc_procIdGet (NULL, SC_NO_TMO);
  /* register to dev man */
  man = sdd_manGetRoot ("/SCP_devman", "/", SC_DEFAULT_POOL, SC_FATAL_IF_TMO);
  if (man) {
    if (sdd_manAdd (man, &dev)) {
      logd_printf (logd, LOGD_SEVERE, "Could not add this device \n");
      sc_procKill (SC_CURRENT_PID, 0);
    }
    sdd_objFree (&man);
  }
  else {
    logd_printf (logd, LOGD_SEVERE, "Could not get /SCP_devman\n");
    sc_procKill (SC_CURRENT_PID, 0);
  }
}

void
fildev (const char *name, unsigned char *memory, size_t size, size_t cursize)
{
  sc_msg_t msg;
  sc_pid_t to;
  logd_t NEARPTR logd;
  fildev_t NEARPTR handle;

  int ref = 0;

  logd =
    logd_new ("/SCP_logd", LOGD_INFO, "fildev", SC_DEFAULT_POOL,
	      SC_FATAL_IF_TMO);
  logd_printf (logd, LOGD_INFO, "Register %s\n", name);

  register_dev(name,logd);

  /* be in save place */
  if (size <= 1) {
    logd_printf (logd, LOGD_SEVERE, "file size too small\n");
    sc_procKill (SC_CURRENT_PID, 0);
  }
  
  if (!memory) {
    memory = (unsigned char *) sc_msgAlloc (sizeof (sc_msgid_t) + size, 0,
                                            SC_DEFAULT_POOL, SC_FATAL_IF_TMO);
  }
  
  for (;;) {
    msg = sc_msgRx (SC_ENDLESS_TMO, SC_MSGRX_ALL, SC_MSGRX_MSGID);
    to = sc_msgSndGet (&msg);
    switch (msg->id) {

    case SDD_DEV_CLOSE:
      {
	--ref;
	if (ref < 0) {
	  sc_miscError (SDD_ERR_BASE + SC_EREFNO, 0);
	}
	if (ref == 0) {
	  /* close it */
	}
	logd_printf (logd, LOGD_FINE, "%8x decr: Reference %d\n", to, ref);
      }
      break;

    case SDD_DEV_OPEN:
      {
	++ref;
	if (ref < 0) {
	  sc_miscError (SDD_ERR_BASE + SC_EREFNO, 0);
	}
	handle = (fildev_t NEARPTR) sc_msgAlloc (sizeof (fildev_t), 
						0,
						SC_DEFAULT_POOL,
						SC_FATAL_IF_TMO);
	handle->pos = 0;
	msg->open.base.error = 0;
	msg->open.base.handle = handle;
      }
      break;

    case SDD_DEV_READ:
      {
	size_t len;
	handle = (fildev_t NEARPTR)msg->read.base.handle;
	if (!msg->read.outlineBuf) {
	  msg->read.outlineBuf = msg->read.inlineBuf;
	}
	if (cursize - handle->pos > (size_t ) msg->read.size) {
	  len = msg->read.size;
	}
	else {
	  len = msg->read.size = (off_t) cursize - handle->pos;
	}
	memcpy (msg->read.outlineBuf, &memory[handle->pos], len);
	handle->pos += len;
      }
      break;

    case SDD_DEV_WRITE:
      {
	size_t len;
	handle = (fildev_t NEARPTR)msg->write.base.handle;
	if (!msg->write.outlineBuf) {
	  msg->write.outlineBuf = msg->write.inlineBuf;
	}
	if (size - handle->pos > (size_t ) msg->write.size) {
	  len = msg->write.size;
	}
	else {
	  len = msg->write.size = (off_t)size - handle->pos;
	}
	memcpy (&memory[handle->pos], msg->write.outlineBuf, len);
	handle->pos += len;
	if (cursize < (size_t ) handle->pos) {
	  cursize = handle->pos; 
	}
      }
      break;

    case SDD_FILE_SEEK:
      {
	handle = (fildev_t NEARPTR)msg->seek.base.handle;
	switch (msg->seek.whence) {
	case SEEK_SET:
	  if (msg->seek.offset > 0 && msg->seek.offset < (off_t)size) {
	    handle->pos = msg->seek.offset;
	  }
	  else {
	    if (msg->seek.offset > 0) {
	      msg->seek.offset = handle->pos = size;
	    }
	    else {
	      msg->seek.offset = handle->pos = 0;
	    }
	  }
	  break;
	case SEEK_CUR:
	  if (msg->seek.offset + handle->pos > 0
	      && msg->seek.offset + handle->pos < (off_t)size) {
	    handle->pos = msg->seek.offset + handle->pos;
	  }
	  else {
	    if (msg->seek.offset + handle->pos > 0) {
	      msg->seek.offset = handle->pos = size;
	    }
	    else {
	      msg->seek.offset = handle->pos = 0;
	    }
	  }
	  break;
	case SEEK_END:
	  if (msg->seek.offset + size > 0 && msg->seek.offset + size < size) {
	    handle->pos = msg->seek.offset + size;
	  }
	  else {
	    if (msg->seek.offset + size > 0) {
	      msg->seek.offset = handle->pos = size;
	    }
	    else {
	      msg->seek.offset = handle->pos = 0;
	    }
	  }
	  break;
	}
      }
      break;
      
    case SDD_DEV_IOCTL:
      {
	switch (msg->ioctl.cmd) {
	case BLKDEVGETPRM:
	  {
	    blkdev_geometry_t param;
	    param.heads = 1;
	    param.sects = cursize/1000;
	    param.size = cursize/1000;
	    memcpy ( (char *)msg->ioctl.outlineArg, &param,
		    sizeof (blkdev_geometry_t));
	  }
	  break;
	default:
	  msg->base.error = EINVAL;
	  break;
	}
      }
      break;

    default:
      msg->base.error = SC_ENOTSUPP;
      break;
    }

    ++msg->id;
    sc_msgTx (&msg, to, 0);
  }
}

