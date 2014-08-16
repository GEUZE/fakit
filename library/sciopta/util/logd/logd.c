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
** ID: S02213CL3                                                    **
** +Revision: 1.35 +                                                **
** +Date: 2008/11/11 09:57:02 +                                     **
** Logging daemon                                                   **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <logd/logd.h>
#include <logd/logd.msg>

#include <stdarg.h>
#include <string.h>

#if defined __ARMCC_VERSION
/* suppress: assignment in condition warning */
#pragma diag_suppress 1293
#endif

union sc_msg {
  sc_msgid_t id;
  logd_print_t print;
  logd_level_set_t levelSet;
};

static const char hex[16] = {
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

STATIC_INLINE void
logd_putchar (char c,sc_msg_t msg, int i)
{
  if (i < LOGD_TEXT_MAX - 1) {
    msg->print.text[i] = c;
  }
}

logd_t NEARPTR 
logd_new (const char *logdProcessName, 
	  int levelMax, /* obsolete ! */
	  const char *appl,
	  sc_poolid_t plid, 
	  sc_ticks_t tmo)
{
  logd_t NEARPTR tmp = NEAR_NULL;

  sc_pid_t logdPid = sc_procIdGet (logdProcessName, SC_NO_TMO);

  if (logdPid != SC_ILLEGAL_PID) {
    tmp = (logd_t NEARPTR)sc_msgAlloc(sizeof (logd_t), 
				     LODG_DESCRIPTOR, 
				     plid, 
				     tmo);
    if (tmp) {
      tmp->logdPid = logdPid;
/*      tmp->levelMax = levelMax; obsolete !*/
      strncpy (tmp->appl, appl, SC_NAME_MAX);
      tmp->plid = plid;
      tmp->tmo = tmo;
    }
  }
  else {
    PRINTF ("logd down\n");
  }
  return tmp;
}

void
logd_destroy (logd_t NEARPTR NEARPTR self)
{
  if (self && *self) {
    sc_msgFree ((sc_msgptr_t) self);
  }
}

void
logd_levelSet(const char *logdProcessName, unsigned int level)
{
  sc_pid_t logd_pid;
  sc_msg_t msg;

  logd_pid = sc_procIdGet(logdProcessName,SC_NO_TMO);

  if ( logd_pid != SC_ILLEGAL_PID ){

    msg = sc_msgAlloc(sizeof(logd_level_set_t),
		      LOGD_LEVEL_SET,
		      SC_DEFAULT_POOL,
		      SC_FATAL_IF_TMO);

    if ( level > LOGD_ALL ){
      level = LOGD_ALL;
    }

    msg->levelSet.level = level;

    sc_msgTx(&msg,logd_pid,0);
  }
}

void
logd_printf (logd_t NEARPTR self, unsigned int level, const char *fmt, ...)
{
  va_list arg;
  char c, f;
  char fill;
  char *p;
  int width;
  char h[16];
  unsigned long n;
  int i = 0;
  sc_msg_t msg;

  if (!self || self->logdPid == SC_ILLEGAL_PID) {
    PRINTF ("logd is down\n");
    return;
  }

  msg = sc_msgAlloc (sizeof (logd_print_t), LOGD_PRINT, self->plid, self->tmo);

  if (!msg) {
    return;
  }
  msg->print.time = sc_tickGet();
  msg->print.level = level;

  va_start (arg, fmt);
  while ((c = *fmt++)) {
    if (c != '%') {
      logd_putchar (c, msg, i++);
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
	logd_putchar ('-', msg, i++);
	n = (unsigned long)(-(signed long)n);
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
	  logd_putchar (fill, msg, i++);
	}
	while (*p) {
	  logd_putchar (*p, msg, i++);
	  ++p;
	}
      }
      else {
	logd_putchar ('(', msg, i++);
	logd_putchar ('N', msg, i++);
	logd_putchar ('I', msg, i++);
	logd_putchar ('L', msg, i++);
	logd_putchar (')', msg, i++);
      }
      continue;
    default:
      /*  empty */
      break;
    }
    for (width = width - (int)(p - h); width > 0; --width) {
      logd_putchar (fill, msg, i++);
    }
    for (--p; p >= h; --p) {
      logd_putchar (*p, msg, i++);
    }
  }
  va_end (arg);

  if ( i > LOGD_TEXT_MAX-1 ){
    i = LOGD_TEXT_MAX-1;
  }
  msg->print.text[i] = 0;

  msg->print.level = level;
  strncpy (msg->print.appl, self->appl, SC_NAME_MAX);
  sc_msgTx (&msg, self->logdPid, 0);
}

SC_PROCESS (SCP_logd)
{
  static const sc_msgid_t sel[3] = {
    LOGD_PRINT,
    LOGD_LEVEL_SET,
    0
  };
  sc_msg_t msg;
  int len;
  int newline;
  sc_time_t time;
  const char *tc;
  sc_pid_t from;
  sc_pid_t last;
  unsigned int max_level = LOGD_ALL;

  last = SC_ILLEGAL_PID;
  newline = 1;

  kprintf (0,"logd: Log Daemon +Revision: 1.35 +\n");
  for (;;) {
    msg = sc_msgRx (SC_ENDLESS_TMO, sel, SC_MSGRX_MSGID);

    switch (msg->id) {
    case LOGD_LEVEL_SET:
      max_level = msg->levelSet.level;
      if ( max_level > LOGD_ALL ){
	max_level = LOGD_ALL;
      }
      break;
    case LOGD_PRINT:
      if ( msg->print.level > max_level ){
	break;
      }

      from = sc_msgSndGet (&msg);
      
      if (newline || last != from ) {
	tc = "ms";
	time = sc_tickTick2Ms (msg->print.time);
	if ( msg->print.time > 65535 ){
	  time /= 1000;
	  tc = "s";
	}
	if ( !newline ) {
	  kprintf(0,"\n");
	}
	kprintf (0, "%d%s: %s[%08x]: ", 
		 time,
		 tc,
		 msg->print.appl, 
		 from);
      }
      len = strlen (msg->print.text);
      if (len > LOGD_TEXT_MAX) {
	len = LOGD_TEXT_MAX;
	msg->print.text[len] = 0;
      }
      kprintf (0, "%s", msg->print.text);
      newline = (msg->print.text[len - 1] == '\n');
      last = from;
      break;
      
    default:
      sc_miscError(LOGD_MSG_BASE|SC_ERR_FATAL,msg->id);
      break;
    }
    sc_msgFree (&msg);
  }
}
