/*SOC -*-c++-*-
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
** (c) 2003 Sciopta Systems GmbH, Litronic AG/ Schweiz              **
**                                                                  **
**********************************************************************
** ID: S04174BS1                                                    **
** +Revision: 1.2 +                                                 **
** +Date: 2008/01/16 13:01:18 +                                     **
** Serial device                                                    **
**********************************************************************
EOC*/


#include <sciopta.h>
#include <sciopta.msg>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <termios.h>

#include <sdd/sdd.h>
#include <sdd/sdd.msg>

#include <logd/logd.h>


/* Prototype needed in private include */
void SCI_tl16c2550(int src,int unit);

/* include private definitions */
#include <tl16c2550_p.h>

/* common definitions */
#include <tl16c2550.h>

/**********************************************************/

/*
** Local definitions & implementations
*/
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
  sc_procNameGetMsgReply_t nameget;
};

static serial_conf_t serial_conf[CNF_MAX_DEVICES*2] = {0};

/*
** low level hardware routines
*/
static void uart_init(int unit);
static void uart_setbaud(int unit,unsigned long baud);
STATIC_INLINE void uart_setRTS(int unit);
STATIC_INLINE void uart_clrRTS(int unit);
STATIC_INLINE void uart_disableTXINT(int unit);
STATIC_INLINE void uart_enableTXINT(int unit);
/*
** local inline functions
*/
STATIC_INLINE void uart_setRTS(int unit)
{
#if USE_RTSCTS != 0
  _UMCR(unit) = UART16550_MCR_nDTR|UART16550_MCR_nRTS;
#endif
}

STATIC_INLINE void uart_clrRTS(int unit)
{
#if USE_RTSCTS != 0
  _UMCR(unit) = UART16550_MCR_nDTR;
#endif
}

STATIC_INLINE void uart_disableTXINT(int unit)
{
  _UIER(unit) &= ~UART16550_IER_ETBEI;
}
STATIC_INLINE void uart_enableTXINT(int unit)
{
  _UIER(unit) |= UART16550_IER_ETBEI;
}

/*
** Common interrupt routine
*/
void SCI_tl16c2550(int src,int unit)
{
  /* static const sc_msgid_t sel[2] = { SDD_DEV_WRITE,0 }; */
  serial_conf_t *conf;
  int flag;
  sc_msg_t msg;
  char c;

  conf = &serial_conf[unit];

  if ( src == 0 ){
    uart_clrRTS(unit);
    switch( _UIIR(unit) & 0xf  ){
    case 0: /* Modem */
      (void)_UMSR(1);/* clear interrupt */
      break;
    case 1<<1: /* THRE */
      while ( conf->tcnt && (_ULSR(unit) & UART16550_LSR_THRE) ){
	c = conf->tbuf[conf->tout];
	_UTHR(unit) = c;
	conf->tout = (conf->tout + 1 ) & (CNF_UART_TXBUF_SIZE - 1);
	--conf->tcnt;
      }
      if ( conf->tcnt && conf->tcnt < 16 ){
	msg = sc_msgAlloc(sizeof(sc_msgid_t),
			  SERIAL_TXBUF,
			  SC_DEFAULT_POOL,
			  SC_FATAL_IF_TMO);
	sc_msgTx(&msg,sc_procPpidGet(SC_CURRENT_PID),0);
      }
      if ( !conf->tcnt ){
	uart_disableTXINT(unit);
      }
      break;
    case 2<<1: /* RDA */
    case 6<<1: /* CTI */
      flag = 0;
      while ( (_ULSR(unit) & UART16550_LSR_DR) ){
	c = (__u8)_URBR(unit);
	conf->rbuf[conf->rin] = c;
	conf->rin = (conf->rin + 1) & (CNF_UART_RXBUF_SIZE-1);
	++conf->rcnt;
	if ( conf->rin == conf->rout ){
	  conf->rout = (conf->rout + 1) & (CNF_UART_RXBUF_SIZE-1);
	  --conf->rcnt;
	}	
	flag |= c == conf->eol;
      }
      if ( conf->rmsg && (flag || conf->rcnt >= (conf->rmax-conf->rcur))  ){
	msg = sc_msgAlloc(sizeof(sc_msgid_t),
			  SERIAL_RXBUF,
			  SC_DEFAULT_POOL,
			  SC_FATAL_IF_TMO);
	sc_msgTx(&msg,sc_procPpidGet(SC_CURRENT_PID),0);
      }
      break;
    case 3<<1: /*  RLS */
      (void)_ULSR(unit); /* clear interrupt */
      break;
    case 4<<1: case 5<<1: case 7<<1:
      /* revered */
      break;
    }
    uart_setRTS(unit);
  } else if ( src == -2 ){
    UART_DISABLEINTR(unit);
  } else if ( src == -1 ){
    /* empty */
  } else if ( src == 1 ){
    /* woken up by illegal msg. */
    sc_miscError(TTYTAG,unit);
  } else {
    /* all other values, esp 2 (trigger) are illegal  */
    sc_miscError(TTYTAG,unit);
  }

}

/*
** set uart to a known state
*/
static void uart_init(int unit)
{
  /*
  ** Setup host-cpu specific things
  */
  uart_chip_init(unit);
  /*
  ** setup UART
  */
  _UIER(unit) = 0;
  _UFCR(unit) = UART16550_FCR_RX_RS|UART16550_FCR_TX_RS;
  _UFCR(unit) = UART16550_FCR_EN|UART16550_FCR_TRG_1;
  _ULCR(unit) = UART16550_LCR_DLAB;
  _UMCR(unit) = 0x8; /* OUT high, no flow-control */
  _UDLL(unit) = (CNF_TL16C2550_BASE_CLK/16/19200) & 0xff;
  _UDLM(unit) = (CNF_TL16C2550_BASE_CLK/16/19200)>> 8;
  _ULCR(unit) = UART16550_LCR_8BIT|UART16550_LCR_1STP;

}

static void uart_setbaud(int unit,unsigned long baud)
{
  _ULCR(unit) |= UART16550_LCR_DLAB;
  _UDLL(unit) = (CNF_TL16C2550_BASE_CLK/16/baud) & 0xff;
  _UDLM(unit) = (CNF_TL16C2550_BASE_CLK/16/baud)>> 8;
  _ULCR(unit) &= ~UART16550_LCR_DLAB;
}


/*****************************************************************************/

static void register_dev(int unit,const char *name, logd_t *logd)
{
  /* registration */
  char irq[SC_PROC_NAME_SIZE+1] = "SCI_";
  sdd_obj_t *dev;
  sdd_obj_t *man;
  dev = (sdd_obj_t *) sc_msgAlloc (sizeof (sdd_obj_t), 0,
                                      SC_DEFAULT_POOL, SC_FATAL_IF_TMO);
  dev->base.error = 0;
  dev->base.handle = NULL;
  dev->type = SDD_OBJ_TYPE | SDD_DEV_TYPE | SDD_FILE_TYPE;
  strncpy (dev->name, name, SC_NAME_MAX);
  dev->receiver =
    dev->sender =
    dev->controller = sc_procIdGet(NULL,SC_NO_TMO);

  strcat(irq,name);
  /*
  ** Create interrupt routine (only if vector is specified)
  */
  if ( uart_irqVector[unit] != -1 ){
    sc_procIntCreate(irq,
		     irqtab[unit],
		     512,
		     uart_irqVector[unit],
		     0,
		     SC_PDB_STATE_RUN,
		     SC_DEFAULT_POOL);
  }

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

STATIC_INLINE char readFromRB(serial_conf_t *conf)
{
  __u8 c;
  __u8 *p;
  sc_cpuflags_t oldmask;

  c = conf->eol ^ 1;
  p = &conf->rmsg->read.outlineBuf[conf->rcur];
  LOCK(oldmask);
  while( conf->rcnt &&
	 conf->rcur < conf->rmax &&
	 (!conf->eol || (conf->eol != c)) )
  {
    c = conf->rbuf[conf->rout];
    conf->rout = ( conf->rout + 1) & (CNF_UART_RXBUF_SIZE-1);
    --conf->rcnt;
    UNLOCK(oldmask);
    *p++ = c;
    ++conf->rcur;
    LOCK(oldmask);
  }
  UNLOCK(oldmask);

  return c;
}

STATIC_INLINE void writeToRB(serial_conf_t *conf)
{
  __u8 *p;
  sc_cpuflags_t oldmask;

  p = &conf->tmsg->write.outlineBuf[conf->tcur];
  LOCK(oldmask);
  while( conf->tcur < conf->tmax && conf->tcnt < CNF_UART_TXBUF_SIZE ){
    conf->tbuf[conf->tin] = *p++;
    conf->tin = (conf->tin + 1) & (CNF_UART_TXBUF_SIZE - 1);
    ++conf->tcnt;
    UNLOCK(oldmask);
    ++conf->tcur;
    LOCK(oldmask);
  }
  UNLOCK(oldmask);
}

/** Mother process */
SC_PROCESS(SCP_tl16c2550)
{
  sc_msgid_t select[] = {
    /*0*/SDD_DEV_OPEN,
    /*1*/SDD_DEV_CLOSE,
    /*2*/SDD_DEV_IOCTL,
    /*3*/SDD_FILE_SEEK,
    /*4*/SDD_OBJ_DUP,
    /*5*/SDD_OBJ_RELEASE,
    /*6*/SERIAL_RTMO,
    /*7*/SERIAL_RXBUF,
    /*8*/SERIAL_TXBUF,
#define SELECT_FREE_POS 9
    /*9*/0,
   /*10*/0,
    0
  };

  sc_msg_t msg;
  sc_msg_t msg2;
  sc_pid_t to;
  logd_t *logd;
  int unit = -1;
  serial_conf_t *conf;
  int i;
  sc_ticks_t tmo;
  char c;

  logd = logd_new ("/SCP_logd",
		   LOGD_INFO, "tl16c2550", SC_DEFAULT_POOL, SC_FATAL_IF_TMO);

  /* check process-name, must contain the device name:
  ** e.g. SCP_serial0 for device "serial0"
  ** or   ttyS0_task for device "ttyS0"
  */
  msg = sc_procNameGet(SC_CURRENT_PID);
  for(unit = 0; unit < CNF_MAX_DEVICES*2; ++unit){
    if ( strstr(msg->nameget.process,uart_devName[unit]) ){
      break;
    }
  }
  sc_msgFree(&msg);
  if ( unit == CNF_MAX_DEVICES*2 ){
    logd_printf(logd,LOGD_SEVERE,"Illegal process-name\n");
    sc_procKill(SC_CURRENT_PID,0);
  }

  /*
  ** register device
  */
  register_dev(unit,uart_devName[unit],logd);

  uart_init(unit);

  conf = &serial_conf[unit];
  conf->unit = unit;
  conf->baud = 38400;
  conf->eof = TTY_EOF;
  conf->eol = '\r';         /* minicom does not send '\n' */

  for (;;) {
    i = SELECT_FREE_POS;
    if ( !conf->rmsg ){
      select[i++] = SDD_DEV_READ;
    }
    if ( !conf->tmsg ){
      select[i++] = SDD_DEV_WRITE;
    }

    msg = sc_msgRx (SC_ENDLESS_TMO, select, SC_MSGRX_MSGID);

    to = sc_msgSndGet (&msg);
    switch (msg->id) {
    case SDD_OBJ_RELEASE:
      /* nothing */
      break;
    case SDD_OBJ_DUP:
      ++conf->ref;
      break;
    case SDD_DEV_OPEN:
      ++conf->ref;
      msg->open.base.error = 0;
      msg->open.base.handle = NULL;
      conf->rin = conf->rout = conf->rcnt = 0;
      conf->tin = conf->tout = conf->tcnt = 0;
      conf->rmsg = conf->tmsg = 0;
      conf->flags = msg->open.flags;

      UART_ENABLEINTR(unit);
      break;
    case SDD_DEV_CLOSE:
      --conf->ref;
      if (conf->ref < 0) {
	sc_miscError (SDD_ERR_BASE + SC_EREFNO, 0);
      }
      if (conf->ref == 0) {
	/* close it */
	UART_DISABLEINTR(unit);

	if ( conf->rmsg ){
	  sc_msgFree( &conf->rmsg );
	}
	if ( conf->tmsg ){
	  sc_msgFree( &conf->tmsg );
	}
	if ( conf->rtmoId ){
	  msg2 = sc_tmoRm(&conf->rtmoId);
	  sc_msgFree(&msg2);
	}
	conf->rtmo = SC_ENDLESS_TMO;	
      }
      break;
      /*
      ** read TMO expired
      */
    case SERIAL_RTMO:
      sc_msgFree(&msg);
      conf->rtmoId = 0;

      (void)readFromRB(conf);

      conf->rmsg->id++;
      conf->rmsg->read.size = conf->rcur;
      sc_msgTx(&conf->rmsg,sc_msgSndGet(&conf->rmsg),0);
      break;

    case SERIAL_RXBUF:
      sc_msgFree(&msg);

      if ( conf->rmsg ){	
	c = readFromRB(conf);
	if ( (conf->eol && c == conf->eol) || conf->rcur == conf->rmax){
	  /* remove pending TMO */
	  if ( conf->rtmoId ){
	    msg = sc_tmoRm(&conf->rtmoId);
	    sc_msgFree(&msg);
	  }

	  conf->rmsg->id++;
	  conf->rmsg->read.size = conf->rcur;
	  sc_msgTx(&conf->rmsg,sc_msgSndGet(&conf->rmsg),0);
	}
      }
      break;
    case SDD_DEV_READ:
      if (!msg->read.outlineBuf) {
	msg->read.outlineBuf = msg->read.inlineBuf;
      }
      conf->rmax = msg->read.size;
      conf->rcur = 0;
      conf->rmsg = msg;
      msg = 0;

      c = readFromRB(conf);

      if ( (conf->eol && (c == conf->eol)) ||
	   conf->rcur == conf->rmax ||
	   (conf->flags & O_NONBLOCK) )
      {
	conf->rmsg->id++;
	conf->rmsg->read.size = conf->rcur;
	sc_msgTx(&conf->rmsg,sc_msgSndGet(&conf->rmsg),0);
      } else {
	if ( SC_ENDLESS_TMO != conf->rtmo ){
	  msg = sc_msgAlloc(4,SERIAL_RTMO,SC_DEFAULT_POOL,SC_FATAL_IF_TMO);
	  conf->rtmoId = sc_tmoAdd(conf->rtmo, &msg);
	}
      }
      break;
    case SERIAL_TXBUF:
      if ( conf->tmsg ){
	writeToRB(conf);
	
	if ( conf->tcur == conf->tmax ){
	  conf->tmsg->id++;
	  sc_msgTx(&conf->tmsg,sc_msgSndGet(&conf->tmsg),0);
	}
      }
      if ( conf->tcnt ){
	uart_enableTXINT(unit);
      }
      sc_msgFree(&msg);
      break;
    case SDD_DEV_WRITE:
      if (!msg->write.outlineBuf) {
	msg->write.outlineBuf = msg->write.inlineBuf;
      }
      conf->tmax = msg->write.size;
      conf->tcur = 0;
      conf->tmsg = msg;
      msg = 0;

      writeToRB(conf);

      if ( conf->tcur == conf->tmax ){
	conf->tmsg->id++;
	sc_msgTx(&conf->tmsg,sc_msgSndGet(&conf->tmsg),0);	
      }
      uart_enableTXINT(unit);
      break;
    case SDD_FILE_SEEK:
    {
      msg->seek.base.error = 0;
    }
    break;
    case SDD_DEV_IOCTL:
    {
      switch (msg->ioctl.cmd) {
      case SERSETEOL:
      {
	char oldeol = conf->eol;
	conf->eol = *(char *) msg->ioctl.outlineArg;
	*(char *) msg->ioctl.outlineArg = oldeol;
      }
      break;
      case SERGETEOL:
	*(char *) msg->ioctl.outlineArg = conf->eol;
	break;	
      case SERSETEOF:
      {
	char oldeof = conf->eof;
	conf->eof = *(unsigned char *) msg->ioctl.outlineArg;
	*(char *) msg->ioctl.outlineArg = oldeof;
      }
      break;
      case SERGETEOF:
	*(char *) msg->ioctl.outlineArg = conf->eof;
	break;	
      case SERTMO:
	  {
	    sc_ticks_t oldi = conf->rtmo;

	    tmo = *(sc_ticks_t*)msg->ioctl.outlineArg;
	    if ( tmo >0 && tmo <= SC_TMO_MAX ){
	      conf->flags |= SERIAL_FLAGS_TMO;
	      conf->rtmo = tmo;
	    } else {
	      conf->flags &= ~SERIAL_FLAGS_TMO;
	      conf->rtmo = SC_ENDLESS_TMO;
	    }
	    *(sc_ticks_t *)msg->ioctl.outlineArg = oldi;
	  }
	break;
      case SERSETBAUD:
      {
	int bdidx = *(int *) msg->ioctl.outlineArg;
	switch ( bdidx ){
	case B9600:  bdidx = 9600; break;
	case B19200:  bdidx = 19200; break;
	case B38400:  bdidx = 38400; break;
	case B57600:  bdidx = 57600; break;
	case B115200:  bdidx = 115200; break;
	default:
	  if ( bdidx < B230400 ){
	    bdidx = 0;
	  }
	}	
	conf->baud = bdidx;
	uart_setbaud(unit,bdidx);
      }
      break;
      case SERTTY:
      {
	int oldi = conf->flags & SERIAL_FLAGS_TTY ? 1 : 0;
	if (*(int *) msg->ioctl.outlineArg > 0) {
	  conf->flags |= SERIAL_FLAGS_TTY;
	}
	else if (*(int *) msg->ioctl.outlineArg == 0) {
	  conf->flags &= ~SERIAL_FLAGS_TTY;
	}
	*(int *) msg->ioctl.outlineArg = oldi;
      }
      break;
      case SERXONXOFF:
      {
	int oldi = conf->flags & SERIAL_FLAGS_XONXOFF ? 1 : 0;
	if (*(int *) msg->ioctl.outlineArg > 0) {
	  conf->flags |= SERIAL_FLAGS_XONXOFF;
	}
	else if (*(int *) msg->ioctl.outlineArg == 0) {
	  conf->flags &= ~SERIAL_FLAGS_XONXOFF;
	}
	*(int *) msg->ioctl.outlineArg = oldi;
      }
      break;
      case SERRTSCTS:
      {
	int oldi = conf->flags & SERIAL_FLAGS_RTSCTS ? 1 : 0;
	if (*(int *) msg->ioctl.outlineArg > 0) {
	  conf->flags |= SERIAL_FLAGS_RTSCTS;
	}
	else if (*(int *) msg->ioctl.outlineArg == 0) {
	  conf->flags &= ~SERIAL_FLAGS_RTSCTS;
	}
	*(int *) msg->ioctl.outlineArg = oldi;

	if ( conf->flags & SERIAL_FLAGS_RTSCTS ){
	  _UMCR(conf->unit) |= 0x22;  /* enable AFE + RTS */
	} else {
	  _UMCR(conf->unit) &= ~0x22; /* disable AFE + RTS */
	}
      }
      break;
      case SERECHO:
      {
	int oldi = conf->flags & SERIAL_FLAGS_ECHO ? 1 : 0;
	if (*(int *) msg->ioctl.outlineArg > 0) {
	  conf->flags |= SERIAL_FLAGS_ECHO;
	}
	else if (*(int *) msg->ioctl.outlineArg == 0) {
	  conf->flags &= ~SERIAL_FLAGS_ECHO;
	}
	*(int *) msg->ioctl.outlineArg = oldi;
      }
      break;
      case SERGETBAUD:
	*(int *) msg->ioctl.outlineArg = conf->baud;
	break;
      default:
	msg->base.error = EINVAL;
	break;
      }
    }
    break;
    default:
      /* unwanted message, fatal error */
      sc_miscError(SDD_ERROR|SC_ERR_SYSTEM_FATAL,msg->id);
      break;
    }
    if ( msg ){
      ++msg->id;
      sc_msgTx (&msg, to, 0);
    }
  }
/* Code unreachable */
/*  logd_destroy (&logd); */
}

