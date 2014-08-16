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
** ID: S04197CL2                                                    **
** +Revision: 1.4 +                                                 **
** +Date: 2008/01/16 13:02:00 +                                     **
** SDD stream functin                                               **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <string.h>
#include <sys/errno.h>
#include <sdd/stream.h>

#if defined __ARMCC_VERSION
/* suppress: assignment in condition warning */
#pragma diag_suppress 1293
#endif

static const char hex[16] = {
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_devWrite_t write;
};

static int sdd_dummyPutc (sdd_stream_t NEARPTR self, void *dummy,const char c);

static void 
sdd_streamReset (sdd_stream_t NEARPTR self)
{

  self->out = 0;
  self->outbuf->write.base.id = SDD_DEV_WRITE;
  self->outbuf->write.base.error = 0;
  self->outbuf->write.base.handle = self->conn->base.handle;
  self->outbuf->write.curpos = 0;
  self->outbuf->write.size = SDD_STREAM_BUF_SIZE;
  self->outbuf->write.outlineBuf = NULL;
}

sdd_stream_t NEARPTR
sdd_streamNew (sdd_obj_t NEARPTR dev, 
	       void *data, 
	       sdd_putc_f putc,
	       sc_poolid_t plid, 
	       sc_ticks_t tmo)
{
  sdd_stream_t NEARPTR tmp;

  tmp = (sdd_stream_t NEARPTR) sc_msgAllocClr (sizeof (sdd_stream_t), 0, plid, 
                                               tmo);
  if (!tmp) {
    return NEAR_NULL;
  }

  tmp->conn = dev;
  tmp->data = data;

  tmp->outbuf = sc_msgAlloc (sizeof (sdd_devWrite_t) + SDD_STREAM_BUF_SIZE, 
                             SDD_DEV_WRITE, 
			     plid, 
			     tmo);

  sdd_streamReset (tmp);

  if (putc) {
    tmp->putc = putc;
  }
  else {
    tmp->putc = sdd_dummyPutc;
  }

  return tmp;
}

void
sdd_streamDestroy (sdd_stream_t NEARPTR NEARPTR self)
{
  if (!self || !*self) {
    return;
  }
  sdd_flush ((*self));
  sc_msgFree ((sc_msgptr_t) self);
}

static int
sdd_dummyPutc (sdd_stream_t NEARPTR self, void *dummy, const char c)
{
  (void)dummy;
  return sdd_putc (self, c);
}

int
sdd_putc (sdd_stream_t NEARPTR self, const char c)
{
  if (self->out >= SDD_STREAM_BUF_SIZE) {
    sdd_flush (self);
  }

  self->outbuf->write.inlineBuf[self->out] = c;
  self->out++;
  sc_miscErrnoSet (0);
  return (unsigned int) c;
}

int
sdd_flush (sdd_stream_t NEARPTR self)
{
  static const sc_msgid_t selAck[3] = { SDD_DEV_WRITE_REPLY, SDD_ERROR, 0 };

  if (self->out && self->outbuf) {
    self->outbuf->write.size = self->out;
    self->outbuf->write.inlineBuf[self->out] = 0;
    sc_msgTx (&self->outbuf, self->conn->sender, 0);
    self->outbuf = sc_msgRx (SC_ENDLESS_TMO, selAck, SC_MSGRX_MSGID);
    sc_miscErrnoSet (self->outbuf->write.base.error);
    sdd_streamReset (self);
    if (sc_miscErrnoGet ()) {
      return -1;
    }
  }
  return 0;
}

int
sdd_vprintf (sdd_stream_t NEARPTR self, const char *fmt, va_list arg)
{
  char c, f;
  char fill;
  char *p;
  int width;
  char h[16];
  unsigned long n;
  int start = self->out;
  while ((c = *fmt++)) {
    if (c != '%') {
      if (self->putc (self, self->data, c) == -1) {
        return -1;
      }
      continue;
    }
    c = *fmt++;
    fill = ' ';
    if (c == '0') {
      fill = c;
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
        if (self->putc (self, self->data, '-') == -1) {
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
        for (width -= strlen (p); width > 0; --width) {
          if (self->putc (self, self->data, fill) == -1) {
            return -1;
          }
        }
        while (*p) {
          if (self->putc (self, self->data, *p) == -1) {
            return -1;
          }
          ++p;
        }
      }
      else {
        if (self->putc (self, self->data, '(') == -1) {
          return -1;
        }
        if (self->putc (self, self->data, 'N') == -1) {
          return -1;
        }
        if (self->putc (self, self->data, 'I') == -1) {
          return -1;
        }
        if (self->putc (self, self->data, 'L') == -1) {
          return -1;
        }
        if (self->putc (self, self->data, ')') == -1) {
          return -1;
        }
      }
      continue;
    default:
      /*  empty */
      break;
    }
    for (width = width - (int)(p - h); width > 0; --width) {
      if (self->putc (self, self->data, fill) == -1) {
        return -1;
      }
    }
    for (--p; p >= h; --p) {
      if (self->putc (self, self->data, *p) == -1) {
        return -1;
      }
    }
  }
  return self->out - start;
}

int
sdd_printf (sdd_stream_t NEARPTR self, const char *fmt, ...)
{
  int ret;
  va_list arg;
  va_start (arg, fmt);
  ret = sdd_vprintf (self, fmt, arg);
  va_end (arg);
  return ret;
}

