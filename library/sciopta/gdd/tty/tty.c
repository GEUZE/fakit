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
** ID: S03070CL9                                                    **
** +Revision: 1.60 +                                                 **
** +Date: 2008/01/16 13:02:00 +                                     **
** Tty device                                                       **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <scio.h>
#include <sys/ioctl.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#include <logd/logd.h>
#include <tty/tty.h>
#include <string.h>

#if defined __ARMCC_VERSION
/* suppress: assignment in condition warning */
#pragma diag_suppress 1293
#endif

#define TTY_BUF_SIZE 32

static termios_t tty_termios = {
  0,
  ICRNL, 0, 0, ICANON,
  {
   0177,			/* VINTR */
   034,				/* VQUIT */
   010,				/* VERASE */
   025,				/* VKILL */
   004,				/* VEOF */
   0,				/* VMIN */
   0,				/* VEOL */
   0,				/* VTIME */
   0,				/* VEOL2 */
   0,				/* VSWTCH */
   021,				/* VSTART */
   023,				/* VSTOP */
   032,				/* VSUSP */
   031,				/* VDSUSP */
   026,				/* VLNEXT */
   027,				/* VWERASE */
   022,				/* VREPRINT */
   017,				/* VDISCARD */
   0xd,				/* VCR */
   0xa				/* VNL */
   }
  , B9600
};

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_devOpen_t open;
  sdd_devRead_t read;
  sdd_devWrite_t write;
  sdd_devIoctl_t ioctl;
};

static int
tty_layerEol (termios_t * termios, unsigned char c)
{
  if (!c) {
    return 0;
  }
  if (termios->c_lflag & ICANON) {
    if (termios->c_cc[VNL] && termios->c_cc[VNL] == c) {
      return 1;
    }
    else if (termios->c_cc[VEOL] && termios->c_cc[VEOL] == c) {
      return 1;
    }
    else if (termios->c_cc[VEOL2] && termios->c_cc[VEOL2] == c) {
      return 1;
    }
    else {
      return 0;
    }
  }
  else {
    return 0;
  }
}

static char
tty_translateIn (termios_t * termios, unsigned char c)
{
  char ret = 0;
  if (!(termios->c_iflag & IGNCR && c == termios->c_cc[VCR])) {
    ret = c;
    /* XXX: solve this in serial
       if (termios->c_lflag & ECHO) {
       tty_layerCharPut (self->handle, self->ringbuf[self->in]);
       }
     */
    if (termios->c_iflag & ICRNL && ret == termios->c_cc[VCR]) {
      ret = termios->c_cc[VNL];
    }
    if (termios->c_iflag & INLCR && ret == termios->c_cc[VNL]) {
      ret = termios->c_cc[VCR];
    }
    if (termios->c_iflag & IUCLC && ret >= 'A' && ret <= 'Z') {
      ret += 'a' - 'A';
    }
  }
  return ret;
}

static char
tty_translateOut (termios_t * termios, unsigned char c)
{
  char ret = 0;
  if (!(termios->c_oflag & ONLRET && c == termios->c_cc[VCR])) {
    if (termios->c_oflag & ONLCR && c == termios->c_cc[VNL]) {
      ret = termios->c_cc[VCR];
    }
    if (termios->c_oflag & OCRNL && c == termios->c_cc[VCR]) {
      ret = termios->c_cc[VNL];
    }
    if (termios->c_oflag & OLCUC && c >= 'a' && c <= 'z') {
      ret += (char)('A' - 'a');
    }
  }
  return ret;
}

/** Interface implementations 
*/
void
tty_process (const char *name, const char *transport, void *handle)
{
  sc_msg_t msg;
  sc_pid_t to;
  /* registration */
  sdd_obj_t NEARPTR dev;
  sdd_obj_t NEARPTR man;

  sdd_obj_t NEARPTR trans = NEAR_NULL;
  int ref = 0;
  logd_t NEARPTR logd;
  termios_t termios;
  sc_msg_t inbuf = NEAR_NULL;
  int inI = 0;

  logd =
    logd_new ("/SCP_logd", LOGD_INFO, name, SC_DEFAULT_POOL, SC_FATAL_IF_TMO);

  memcpy (&termios, &tty_termios, sizeof (termios_t));
  
  /* register to dev man */
  man = sdd_manGetRoot ("/SCP_devman", "/", SC_DEFAULT_POOL, SC_FATAL_IF_TMO);
  
  if (man && (trans = sdd_manGetByName (man, transport))
      && sdd_devOpen (trans, O_RDWR)
      && sdd_devIoctl (trans, SERSETEOF, termios.c_cc[VNL])) {
    dev =
      (sdd_obj_t NEARPTR) sc_msgAlloc (sizeof (sdd_obj_t), 0,
				    SC_DEFAULT_POOL, SC_FATAL_IF_TMO);
    dev->base.error = 0;
    dev->base.handle = handle;
    dev->type = SDD_OBJ_TYPE | SDD_DEV_TYPE;
    strncpy (dev->name, name, SC_NAME_MAX);
    dev->controller = 
      dev->receiver = 
      dev->sender = sc_procIdGet(NULL,SC_NO_TMO);
    if (sdd_manAdd (man, &dev)) {
      logd_printf (logd, LOGD_SEVERE, "Could not add this device \n");
      sc_procKill (SC_CURRENT_PID, 0);
    }
    sdd_objFree (&man);
  }
  else {
    logd_printf (logd, LOGD_SEVERE, "Registration failt\n");
    sc_procKill (SC_CURRENT_PID, 0);
  }
  
  /* set up trans (normaly this is a serial driver) */
  sdd_devAread (trans, TTY_BUF_SIZE);
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
	logd_printf (logd, LOGD_FINE, "%8x open: Reference %d\n", to, ref);
      }
      break;

    case SDD_DEV_READ_REPLY:
      inbuf = msg;
      if (!inbuf->read.outlineBuf) {
	inbuf->read.outlineBuf = inbuf->read.inlineBuf;
      }
      break;

    case SDD_DEV_READ:
      {
	int j = 0;
	unsigned char *buf, ch;
	static const sc_msgid_t select[2] = {
	  SDD_DEV_READ_REPLY, 0
	};
	if (!msg->read.outlineBuf) {
	  msg->read.outlineBuf = msg->read.inlineBuf;
	}
	while (j < msg->read.size) {
	  if (!inbuf) {
	    inbuf =
	      sc_msgRx (SC_ENDLESS_TMO,  select, SC_MSGRX_MSGID);
	    if (!inbuf->read.outlineBuf) {
	      inbuf->read.outlineBuf = inbuf->read.inlineBuf;
	    }
	  }
	  buf = inbuf->read.outlineBuf;
	  ch = tty_translateIn (&termios, buf[inI]);
	  inI = 0;
	  while (j < msg->read.size && !tty_layerEol (&termios, ch)
		 && inI < TTY_BUF_SIZE) {
	    if (ch) {
	      msg->read.outlineBuf[j] = ch;
	      ++j;
	    }
	    ch = tty_translateIn (&termios, buf[inI++]);
	  }
	  if (inI >= TTY_BUF_SIZE) {
	    inbuf = NEAR_NULL;
	    sdd_devAread (trans, TTY_BUF_SIZE);
	  }
	  if (j < msg->read.size && tty_layerEol (&termios, ch)) {
	    msg->read.outlineBuf[j] = ch;
	    msg->read.size = j;
	  }
	}
      }
      break;

    case SDD_DEV_WRITE:
      {
	int j = 0, i = 0;
	__u8 outbuf[TTY_BUF_SIZE + 1], ch;
	const unsigned char *buf;
	if (!msg->write.outlineBuf) {
	  msg->write.outlineBuf = msg->write.inlineBuf;
	}
	buf = msg->write.outlineBuf;
	while (j < msg->write.size) {
	  if (i >= TTY_BUF_SIZE) {
	    sdd_devWrite (trans, outbuf, i);
	    i = 0;
	  }
	  if ((ch = tty_translateOut (&termios, buf[j++]))) {
	    outbuf[i++] = ch;
	  }
	}
      }
      break;

    case SDD_DEV_IOCTL:
      {
	int i = 0;
	termios_t *tmp;
	tmp = (termios_t *) msg->ioctl.outlineArg;
	switch (msg->ioctl.cmd) {
	case TCSETS:
	  msg->ioctl.ret = 0;
	  for (i = 0; i < NCCS; i++) {
	    termios.c_cc[i] = tmp->c_cc[i];
	  }
	  if (termios.c_iflag != tmp->c_iflag) {
	    termios.c_iflag = tmp->c_iflag;
	  }
	  else if (termios.c_oflag != tmp->c_oflag) {
	    termios.c_oflag = tmp->c_oflag;
	  }
	  else if (termios.c_cflag != tmp->c_cflag) {
	    termios.c_cflag = tmp->c_cflag;
	  }
	  else if (termios.c_lflag != tmp->c_lflag) {
	    termios.c_lflag = tmp->c_lflag;
	  }
	  if (termios.c_speed != tmp->c_speed) {
	    termios.c_speed = tmp->c_speed;
	    msg->ioctl.ret =
	      sdd_devIoctl (trans, SERSETBAUD, termios.c_speed);
	  }
	  break;
	case TCGETS:
	  msg->ioctl.ret = 0;
	  memcpy (tmp, &termios, sizeof (termios_t));
	  break;
	default:
	  {
	    /* redirect msg to transmission layer (i.e. serial) */
	    static const sc_msgid_t select[2] = {
	      SDD_DEV_IOCTL_REPLY, 0
	    };
	    sc_msgTx (&msg, trans->controller, 0);
	    msg = sc_msgRx (SC_ENDLESS_TMO,  select, SC_MSGRX_MSGID);
	  }
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

