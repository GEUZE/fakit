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
** (c) 2007 Sciopta Systems GmbH Deutschland, Sciopta AG/ Schweiz   **
**                                                                  **
**********************************************************************
** ID: S07171BS1                                                    **
** +Revision: 1.3 +                                                 **
** +Date: 2008/11/11 10:03:33 +                                     **
** fprintf like functions for file-descriptors and sdd_obj_t        **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>
#include <sdd/sdd.h>
#include <errno.h>

#include <unistd.h>
#include <stdarg.h>


union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_devWrite_t write;
};

static const char hex[16] = {
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

typedef struct {
  __u8 *buf;
  int cnt;
  sdd_obj_t *obj;
  sc_msg_t buffer;

} stream_t;

static const sc_msgid_t sel[3] = {
  SDD_DEV_WRITE_REPLY,
  SDD_ERROR,
  0
};

static int fdputc(stream_t *stream,char c)
{
  if ( stream->buffer->write.size == 32 ){
    sc_msgTx(&stream->buffer, stream->obj->sender, 0);

    stream->buffer = sc_msgRx(SC_ENDLESS_TMO, sel, SC_MSGRX_MSGID);
    sc_miscErrnoSet(stream->buffer->base.error);
    if ( stream->buffer->base.error ){
      sc_msgFree(&stream->buffer);
      return -1;
    }
    stream->buffer->id = SDD_DEV_WRITE;
    stream->buffer->write.curpos = 0;
    stream->buffer->write.size = 0;
    stream->buf = &stream->buffer->write.inlineBuf[0];
  }
  stream->buffer->write.size++;
  stream->cnt++;
  *(stream->buf++) = c;

  return 1;
}

static int fdflush(stream_t *stream)
{
  sc_msg_t msg;
  int ret;
  if ( stream->buffer->write.size ){
    sc_msgTx(&stream->buffer, stream->obj->sender, 0);

    msg = sc_msgRx(SC_ENDLESS_TMO, sel, SC_MSGRX_MSGID);
    sc_miscErrnoSet(stream->buffer->base.error);

    ret = msg->base.error ? -1 : stream->cnt;
    sc_msgFree(&msg);
  } else {
    sc_msgFree(&stream->buffer);
    ret = stream->cnt;
  }
  return ret;
}

static int
vobjprintf (sdd_obj_t *obj, const char *fmt, va_list arg)
{
  char c, f;
  char fill;
  char *p;
  int width;
  char h[16];
  unsigned long n;
  stream_t stream;
  int la;

  stream.buffer = sc_msgAllocClr(sizeof(sdd_devWrite_t)+32,
				 SDD_DEV_WRITE,
				 SC_DEFAULT_POOL,
				 SC_FATAL_IF_TMO);

  stream.buf = &stream.buffer->write.inlineBuf[0];
  stream.cnt = 0;
  stream.obj = obj;
  la = 0;

  while ((c = *fmt++)) {
    if (c != '%') {
      if ( c == '\n' ) {
	if (fdputc (&stream, '\r') == -1) {
	  return -1;
	}
      }
      if (fdputc (&stream, c) == -1) {
        return -1;
      }
      continue;
    }
    c = *fmt++;
    if ( c == '-' ){
      la = 1;
      c = *fmt++;
    }
    fill = ' ';
    if (c == '0') {
      if ( !la ){
	fill = c;
      }
      c = *fmt++;
    }
    for (width = 0; '0' <= c && c <= '9'; c = *fmt++) {
      width *= 10;
      width += c - '0';
    }
    width = (width > 15) ? 15 : width;
    p = h;
    *p = '\0';
    f = 0;
    if (c == 'l' || c == 'L') {
      f = 1;
      c = *fmt++;
    }
    switch (c) {
    case 'x':
    case 'X':
    case 'p':
      if (f) {
        n = va_arg (arg, unsigned long);
      }
      else {
        n = (unsigned long) va_arg (arg, unsigned int);
      }
      do {
        *p++ = hex[(int)(n & 0xf)];
        n >>= 4;
      } while (n);
      break;
    case 'd':
    case 'u':
      if (f) {
        n = va_arg (arg, unsigned long);
      }
      else {
        n = (unsigned long) va_arg (arg, unsigned int);
      }
      if (c == 'd' && (signed long) n < 0) {
        if (fdputc(&stream ,'-') == -1) {
          return -1;
        }
        n = (unsigned long) (-(signed long) n);
      }
      do {
        *p++ = hex[(int)(n % 10)];
        n /= 10;
      } while (n);
      break;
    case 'c':
#if defined __GNUC__ || defined __ARMCC_VERSION
      c = va_arg (arg, int);
#else
      c = va_arg (arg, char);
#endif
      *p++ = c;
      break;
    case 's':
      p = va_arg (arg, char *);
      if (p) {
	int sl = strlen(p);
	if ( !la ){
	  for (width -= sl; width > 0; --width) {
	    if (fdputc(&stream ,fill) == -1) {
	      return -1;
	    }
	  }
	}
        while (*p) {
          if (fdputc(&stream ,*p) == -1) {
            return -1;
          }
          ++p;
        }
	if ( la ){
	  for (width -= sl; width > 0; --width) {
	    if (fdputc(&stream ,fill) == -1) {
	      return -1;
	    }
	  }
	}
	
      }
      else {
        if (fdputc(&stream ,'(') == -1) {
          return -1;
        }
        if (fdputc(&stream ,'N') == -1) {
          return -1;
        }
        if (fdputc(&stream ,'I') == -1) {
          return -1;
        }
        if (fdputc(&stream ,'L') == -1) {
          return -1;
        }
        if (fdputc(&stream ,')') == -1) {
          return -1;
        }
      }
      continue;
    default:
      /*  empty */
      break;
    }
    if ( !la ) {
      for (width = width - (int)(p - h); width > 0; --width) {
	if ( fdputc(&stream,fill) == -1) {
	  return -1;
	}
      }
    }
    for (--p; p >= h; --p, --width) {
      if (fdputc(&stream, *p) == -1) {
        return -1;
      }
    }
    if ( la ){
      for( ; width; --width ){
	if ( fdputc(&stream,fill) == -1) {
	  return -1;
	}
      }
    }

  }
  /* fdflush local buffer */
  return fdflush(&stream);
}

int objprintf(sdd_obj_t *con, const char *fmt, ...)
{
  int ret;
  va_list arg;
  va_start (arg, fmt);
  ret = vobjprintf (con, fmt, arg);
  va_end (arg);
  return ret;
}

int fdprintf(int fd, const char *fmt, ...)
{
  int ret;
  sdd_obj_t *obj;
  fd_tab_t NEARPTR fd_tab;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && (fd_tab->magic == SCIO_MAGIC) &&	
	(fd >= 0) && (fd <= fd_tab->max_fd) && (fd_tab->bf[fd] != 0))
    {
      va_list arg;
      obj = fd_tab->bf[fd];
      va_start (arg, fmt);
      ret = vobjprintf (obj, fmt, arg);
      va_end (arg);
      return ret;
    }
    else {
      sc_miscErrnoSet (EBADF);
      return -1;
    }
  }
  return -1;
}
