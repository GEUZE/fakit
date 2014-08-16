/*SOC
**********************************************************************
**  ______  _______ _________ _______  _______ _________ _______    **
** (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
** | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
** | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
** (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
**       ) || |         | |   | |   | || (         | |   | (   ) |  **
** /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
** \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
**                                                                  **
** (c) 2002 Sciopta GmbH, Litronic AG/ Schweiz                      **
**                                                                  **
**********************************************************************
** ID: S03051CL2                                                    **
** +Revision: 1.27 +                                                **
** +Date: 2008/01/16 13:01:59 +                                     **
** pipe device                                                      **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#include <container/list.h>
#include <logd/logd.h>
#include <dev/pipedev.h>

#define PIPE_MAX_BUF	256

union sc_msg {
  sc_msgid_t id;
  sdd_obj_t object;
  sdd_objDup_t duplicate;
  sdd_baseMessage_t base;
  sdd_devDualopen_t open;
  sdd_devRead_t read;
  sdd_devWrite_t write;
  sdd_devIoctl_t ioctl;
  sdd_fileSeek_t seek;
  sdd_fileResize_t resize;
};

typedef struct pipe_ringbuf_s {
  sc_msgid_t id;
  int ref;
  list_t NEARPTR reads;
  list_t NEARPTR writes;
  int in, out, count;
  unsigned char eof;
  char buf[PIPE_MAX_BUF + 1];
} pipe_ringbuf_t;

typedef struct pipe_s {
  sc_msgid_t id;
  flags_t flags;
  int ref;
  pipe_ringbuf_t NEARPTR rb;
} pipe_t;

static int
msgCmp (const void NEARPTR left, const void NEARPTR right)
{
  return (int)((char NEARPTR) left - (char NEARPTR) right);
}

static void
msgDel (void NEARPTR entry)
{
  if (entry) {
    sc_msgFree ((sc_msgptr_t) & entry);
  }
}

static int
__pipeRead (pipe_ringbuf_t NEARPTR rb)
{
  char ch;

  int i = 0;
  sc_msg_t msg = (sc_msg_t)rb->reads->ops->getFirst (rb->reads);

  while (!rb->eof && msg && rb->count > 0
         && msg->read.curpos < msg->read.size) {
    ch = msg->read.outlineBuf[msg->read.curpos] = rb->buf[rb->out];
    ++i;
    rb->out = (rb->out + 1) & (PIPE_MAX_BUF - 1);
    ++msg->read.curpos;
    --rb->count;
    if (ch == '\n') {
      msg->read.size = msg->read.curpos;
    }
  }

  if (rb->eof) {
    /* if eof stop reading and send back what was read until now, next request
       will return zero reading means eof for application */
    msg->read.size = msg->read.curpos;
  }

  if (msg && msg->read.curpos >= msg->read.size) {
    rb->reads->ops->rm (rb->reads, msg);
    msg->id = SDD_DEV_READ_REPLY;
    sc_msgTx (&msg, sc_msgSndGet (&msg), 0);
  }

  return i;
}

static int
__pipeWrite (pipe_ringbuf_t NEARPTR rb)
{
  char ch;

  int i = 0;
  sc_msg_t msg = (sc_msg_t)rb->writes->ops->getFirst (rb->writes);

  while (!rb->eof && msg && (rb->in != rb->out || rb->count)
         && msg->write.curpos < msg->write.size) {
    ch = msg->write.outlineBuf[msg->write.curpos];
    if (ch == 4 /* CTRL-D */ ) {
      rb->eof = 1;
    }
    else {
      rb->buf[rb->in] = ch;
    }
    ++i;
    rb->in = (rb->in + 1) & (PIPE_MAX_BUF - 1);
    ++rb->count;
  }

  if (rb->eof) {
    /* if eof send back how much we have wirtten until eof */
    msg->write.size = msg->write.curpos;
    /* and send back error EPIPE because we do not have UNIX signals */
    msg->write.base.error = EPIPE;
  }

  if (msg && msg->write.curpos >= msg->write.size) {
    rb->writes->ops->rm (rb->writes, msg);
    sc_msgTx (&msg, sc_msgSndGet (&msg), 0);
  }

  return i;
}

/** Controlling process */
void
pipedev (const char *name)
{
  sc_msg_t msg;
  sc_pid_t to;
  /* registration */
  sdd_obj_t NEARPTR dev;
  sdd_obj_t NEARPTR man;
  logd_t NEARPTR logd;

  logd =
    logd_new ("/SCP_logd", LOGD_INFO, "null", SC_DEFAULT_POOL,
              SC_FATAL_IF_TMO);

  /* register to dev man */
  dev = NEAR_NULL;
  dev =
    (sdd_obj_t NEARPTR) sc_msgAlloc (sizeof (sdd_obj_t), 0,
                                     SC_DEFAULT_POOL, SC_FATAL_IF_TMO);
  dev->base.error = 0;
  dev->base.handle = NEAR_NULL;
  dev->type = SDD_OBJ_TYPE | SDD_DEV_TYPE | SDD_FILE_TYPE;
  strncpy (dev->name, name, SC_NAME_MAX);
  dev->controller = dev->receiver = dev->sender =
    sc_procIdGet (NULL, SC_NO_TMO);
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

  for (;;) {
    msg = sc_msgRx (SC_ENDLESS_TMO, SC_MSGRX_ALL, SC_MSGRX_MSGID);
    to = sc_msgSndGet (&msg);
    switch (msg->id) {

    case SDD_OBJ_DUP:
      {
        pipe_t NEARPTR handle = (pipe_t NEARPTR)msg->duplicate.base.handle;
        ++handle->ref;
        if (handle->ref < 0) {
          sc_miscError (SC_EBADBAD, 0);
        }
      }
      break;

    case SDD_DEV_DUALOPEN:
      {
        sc_msg_t repl;
        sc_poolid_t plid;
        /* free open message and create a special pipe_open_t message for
           reply */
        plid = sc_msgPoolIdGet (&msg);
        repl =
          sc_msgAlloc (sizeof (sdd_devDualopen_t), SDD_DEV_DUALOPEN_REPLY,
                       plid, SC_NO_TMO);
        if (!repl) {
          msg->open.base.error = ENOMEM;
        }
        else {
          pipe_ringbuf_t NEARPTR rb =
            (pipe_ringbuf_t NEARPTR) sc_msgAlloc (sizeof (pipe_ringbuf_t), 0,
                                                  plid,
                                                  SC_NO_TMO);
          pipe_t NEARPTR inst1 =
            (pipe_t NEARPTR) sc_msgAlloc (sizeof (pipe_t), 0, plid,
                                          SC_NO_TMO);
          pipe_t NEARPTR inst2 =
            (pipe_t NEARPTR) sc_msgAlloc (sizeof (pipe_t), 0, plid,
                                          SC_NO_TMO);
          if (rb && inst1 && inst2) {
            rb->ref = 2;
            rb->reads = list_new (msgCmp, msgDel, plid, 0);
            rb->writes = list_new (msgCmp, msgDel, plid, 0);
            rb->eof = 0;
            rb->in = rb->out = rb->count = 0;
            inst1->rb = rb;
            inst1->flags = O_RDONLY;
            inst1->ref = 1;
            inst2->rb = rb;
            inst2->flags = O_WRONLY;
            inst2->ref = 1;
            repl->open.base.error = 0;
            /* read handle */
            repl->open.base.handle = inst1;
            /* write handle */
            repl->open.handle2 = inst2;
          }
          sc_msgFree (&msg);
          msg = repl;
        }
      }
      break;

    case SDD_DEV_CLOSE:
      {
        pipe_t NEARPTR handle = (pipe_t NEARPTR)msg->duplicate.base.handle;
        --handle->ref;
        if (handle->ref < 0) {
          sc_miscError (SC_EBADBAD, 0);
        }
        else if (handle->ref == 0) {
          --handle->rb->ref;
          if (handle->rb->ref < 0) {
            sc_miscError (SC_EBADBAD, 0);
          }
          else if (handle->rb->ref == 0) {
            sc_msgFree ((sc_msgptr_t) & handle->rb);
          }
          sc_msgFree ((sc_msgptr_t) & handle);
        }
      }
      break;

    case SDD_DEV_READ:
      {
        pipe_t NEARPTR handle = (pipe_t NEARPTR)msg->read.base.handle;
        pipe_ringbuf_t NEARPTR rb = handle->rb;
        if (!msg->read.outlineBuf) {
          msg->read.outlineBuf = msg->read.inlineBuf;
        }
        rb->reads->ops->add (rb->reads, msg);
        while (__pipeRead (rb) && __pipeWrite (rb));
      }
      break;

    case SDD_DEV_WRITE:
      {
        pipe_t NEARPTR handle = (pipe_t NEARPTR)msg->write.base.handle;
        pipe_ringbuf_t NEARPTR rb = handle->rb;
        if (!msg->write.outlineBuf) {
          msg->write.outlineBuf = msg->write.inlineBuf;
        }
        rb->writes->ops->add (rb->writes, msg);
        while (__pipeWrite (rb) && __pipeRead (rb));
      }
      break;

    case SDD_FILE_SEEK:
      {
      }
      break;

    case SDD_DEV_IOCTL:
      {
        switch (msg->ioctl.cmd) {
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

SC_PROCESS (SCP_pipedev)
{
  pipedev ("pipe");
}
