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
** ID: S03070CL78                                                   **
** +Revision: 1.76 +                                                 **
** +Date: 2008/01/16 13:02:10 +                                     **
** shell parser                                                     **
**********************************************************************
EOC*/
#include <sciopta.h>
#include <ossys/libsc.h>
#include <sh/sh.msg>
#include <sh/sh.h>
#include <scio.h>

#if (defined(__CM16C__) && defined(__TASKING__))
#include <sys/errno.h>
#include <sys/unistd.h>
#else
#include <sys/errno.h>
#include <unistd.h>
#endif
#include <fcntl.h>

#if defined __ARMCC_VERSION
/* suppress: assignment in condition warning */
#pragma diag_suppress 1293
#endif

/*
 * EBNF Description
 *
 * line 	= expression {";" expression} [";"].
 * expression 	= term {"||" term}
 * term		= factor {"&&" factor}
 * factor	= command | "!" command
 * command	= word { word|file }
 * file		= > word | >> word
 * word		= '"' all_but_none_escapated_double_quote '"'
 *		| "'" all_but_none_escapated_single_quote "'"
*/
#define SH_TOKEN_NONE		0
#define SH_TOKEN_ARG		1
#define SH_TOKEN_DELIM		2
#define SH_TOKEN_NOT		3
#define SH_TOKEN_OR		4
#define SH_TOKEN_PIPE		5
#define SH_TOKEN_AND		6
#define SH_TOKEN_BG		7
#define SH_TOKEN_BIGGER		8
#define SH_TOKEN_DOUBLEBIGGER	9
#define SH_TOKEN_ANDBIGGER	10
#define SH_TOKEN_BIGGERAND	11
#define SH_TOKEN_BIGGERPIPE	12
#define SH_TOKEN_SMALLER 	13
#define SH_TOKEN_DOUBLESMALLER 	14
#define SH_TOKEN_SMALLERBIGGER 	15
#define SH_TOKEN_SINGLEQUOTE 	16
#define SH_TOKEN_DOUBLEQUOTE 	17
#define SH_TOKEN_REVERSEQUOTE 	18
#define SH_TOKEN_COMMENT 	19
#define SH_TOKEN_REDIRECTION 	20
#define SH_TOKEN_NUMBER 	21
#define SH_TOKEN_NEWLINE 	254
#define SH_TOKEN_ERROR	 	255

typedef struct sh_info {
  sc_msgid_t id;
  char buf[SC_PATH_MAX + 1];
  char ch;
  int len;
  int cur;
  int token;
  int prevtoken;
  char prevchar;
  int lastno;
  int ret;
} sh_info_t;

union sc_msg {
  sc_msgid_t id;
  sh_inherit_t inherit;
  sh_dead_t dead;
};

static char
sh_nextchar (sh_info_t NEARPTR self)
{
  if (self->cur <= self->len) {
    return self->buf[self->cur++];
  }
  else if (self->token != SH_TOKEN_NEWLINE) {
    PRINTF ("line is longer than SC_PATH_MAX\n");
    self->cur = 0;
    self->len = (int)read (0, self->buf, SC_PATH_MAX);
    if (self->len < 0) {
      PRINTF ("read error\n");
      return '\n';
    }
    return self->buf[self->cur++];
  }
  else {
    return '\n';
  }
}

static char *
sh_nextword (sh_info_t NEARPTR self)
{
  int i, esc, stop, number;
  char *tmp;
  char quote;
  // XXX: FIXME: No boundary check
#ifdef NC30
  char word[80];
#else
  char word[SC_PATH_MAX + 1];
#endif
  /* init word scanner */
  stop = 0;
  i = 0;
  esc = 0;
  quote = 0;
  self->token = 0;
  /* prev charakter to detec a space bevor special character */
  self->prevchar = self->buf[self->cur];
  self->prevtoken = self->token;
  /* eat space till next word */
  while (self->ch == ' ') {
    self->ch = sh_nextchar (self);
  }
  switch (self->ch) {
  case '\n':
  case '\r':
    stop = 1;
    self->token = SH_TOKEN_NEWLINE;
    break;
  case '\'':
    self->token = SH_TOKEN_SINGLEQUOTE;
    quote = self->ch;
    self->ch = sh_nextchar (self);
    break;
  case '"':
    self->token = SH_TOKEN_DOUBLEQUOTE;
    quote = self->ch;
    self->ch = sh_nextchar (self);
    break;
  case '`':
    self->token = SH_TOKEN_REVERSEQUOTE;
    quote = self->ch;
    self->ch = sh_nextchar (self);
    break;
  case ';':
    self->token = SH_TOKEN_DELIM;
    stop = 1;
    word[0] = self->ch;
    i = 1;
    self->ch = sh_nextchar (self);
    break;
  case '!':
    self->token = SH_TOKEN_NOT;
    stop = 1;
    word[0] = self->ch;
    i = 1;
    self->ch = sh_nextchar (self);
    break;
  case '&':
    stop = 1;
    word[0] = self->ch;
    self->ch = sh_nextchar (self);
    if (self->ch == '&') {
      self->token = SH_TOKEN_AND;
      word[1] = self->ch;
      i = 2;
      self->ch = sh_nextchar (self);
    }
    else if (self->ch == '>') {
      self->token = SH_TOKEN_ANDBIGGER;
      word[1] = self->ch;
      i = 2;
      self->ch = sh_nextchar (self);
    }
    else {
      self->token = SH_TOKEN_BG;
      i = 1;
    }
    break;
  case '¦':
  case '|':
    stop = 1;
    word[0] = self->ch;
    self->ch = sh_nextchar (self);
    if (self->ch == '|' || self->ch == '¦') {
      self->token = SH_TOKEN_OR;
      word[1] = self->ch;
      i = 2;
      self->ch = sh_nextchar (self);
    }
    else {
      self->token = SH_TOKEN_PIPE;
      i = 1;
    }
    break;
  case '>':
    stop = 1;
    word[0] = self->ch;
    self->ch = sh_nextchar (self);
    if (self->ch == '>') {
      self->token = SH_TOKEN_DOUBLEBIGGER;
      word[1] = self->ch;
      i = 2;
      self->ch = sh_nextchar (self);
    }
    else if (self->ch == '&') {
      self->token = SH_TOKEN_BIGGERAND;
      word[1] = self->ch;
      i = 2;
      self->ch = sh_nextchar (self);
    }
    else if (self->ch == '|') {
      self->token = SH_TOKEN_BIGGERPIPE;
      word[1] = self->ch;
      i = 2;
      self->ch = sh_nextchar (self);
    }
    else {
      self->token = SH_TOKEN_BIGGER;
      i = 1;
    }
    break;
  case '<':
    stop = 1;
    word[0] = '<';
    self->ch = sh_nextchar (self);
    if (self->ch == '<') {
      self->token = SH_TOKEN_DOUBLESMALLER;
      word[1] = self->ch;
      i = 2;
      self->ch = sh_nextchar (self);
    }
    else if (self->ch == '>') {
      self->token = SH_TOKEN_SMALLERBIGGER;
      word[1] = self->ch;
      i = 2;
      self->ch = sh_nextchar (self);
    }
    else {
      self->token = SH_TOKEN_SMALLER;
      i = 1;
    }
    break;
  case '#':
    stop = 1;
    self->token = SH_TOKEN_COMMENT;
    /* ignor rest of line */
    self->ch = sh_nextchar (self);
    while (self->ch != '\n') {
      self->ch = sh_nextchar (self);
    }
    break;
  default:
    self->token = SH_TOKEN_ARG;
  }				/* switch */
  /* read hole word */
  if (!stop) {
    number = self->ch >= '0' && self->ch <= '9';
  }
  else {
    number = 0;
  }
  while (!stop) {
    if (!esc && !quote && self->ch == '\\') {
      esc = 1;
      self->ch = sh_nextchar (self);
    }
    else if (!esc && !quote
	     && (self->ch == ' ' || self->ch == ';' || self->ch == '\n'
		 || self->ch == '>' || self->ch == '<' || self->ch == '&'
		 || self->ch == '|' || self->ch == '\r')) {
      stop = 1;
    }
    else if (quote && self->ch == quote) {
      stop = 1;
      self->ch = sh_nextchar (self);
    }
    else {
      esc = 0;
      word[i] = self->ch;
      number &= self->ch >= '0' && self->ch <= '9';
      ++i;
      self->ch = sh_nextchar (self);
    }
  }
  if (number) {
    self->token = SH_TOKEN_NUMBER;
    self->lastno = dectoi (word);
    PRINTF1 ("last number %d\n", self->lastno);
  }
  tmp =
    (char *) sc_msgAlloc (i + sizeof (sc_msgid_t), 0, SC_DEFAULT_POOL,
			  SC_FATAL_IF_TMO);
  memcpy (tmp, word, i);
  tmp[i] = '\0';
  PRINTF4 ("nextword: %s, len: %d, cur: %d, token: %d\n", tmp, i, self->cur,
	   self->token);
  /* return word without " or ' ('") */
  return tmp;
}

static void
sh_factor (sh_info_t NEARPTR self)
{
  int error = 0;
  int fd = -1, oldfd = -1;
  char *word;
  int _not;
  PRINTF ("factor\n");

  _not = 0;
  word = sh_nextword (self);
  if (self->token == SH_TOKEN_NOT) {
    _not = 1;
    word = sh_nextword (self);
  }

  do {
    int count = 0;
    sc_msg_t msg;
    sc_pid_t pid;
    msg =  sc_msgAlloc (sizeof (sh_inherit_t)+(SH_ARG_MAX + 1)*sizeof (char *),
			SH_BEGIN,
			SC_DEFAULT_POOL,
			SC_FATAL_IF_TMO);

    while ((self->token == SH_TOKEN_ARG
	    || self->token == SH_TOKEN_NUMBER
	    || self->token == SH_TOKEN_BIGGER
	    || self->token == SH_TOKEN_DOUBLEBIGGER
	    || self->token == SH_TOKEN_SINGLEQUOTE
	    || self->token == SH_TOKEN_DOUBLEQUOTE
	    || self->token == SH_TOKEN_REVERSEQUOTE))
    {
      if (count < SH_ARG_MAX
	  && !(self->token == SH_TOKEN_BIGGER
	       || self->token == SH_TOKEN_DOUBLEBIGGER)) {
	msg->inherit.argv[count] = word;
        ++count;
      }
      else if (self->token == SH_TOKEN_BIGGER
	       || self->token == SH_TOKEN_DOUBLEBIGGER) {
	int token = self->token;
	word = sh_nextword (self);
	if (self->token == SH_TOKEN_ARG) {
	  int flags;
	  /* if cascading > take the last one */
	  if (fd != -1) {
	    close (fd);
	  }
	  else {
	    if ((oldfd = dup (1)) != -1) {
	      close (1);
	    }
	    else {
	      sc_miscError (SH_ERR_BASE + SC_EBADBAD, sc_miscErrnoGet ());
	    }
	  }
	  if (token == SH_TOKEN_DOUBLEBIGGER) {
	    flags = O_WRONLY | O_APPEND;
	  }
	  else {
	    flags = O_WRONLY | O_TRUNC;
	  }
	  fd = open (word, flags);
	  if (fd == -1) {
	    fd = open (word, O_CREAT | O_WRONLY);
	    if (fd == -1) {
	      /* could not create file for write */
	      error = EACCES;
	      /* reopen stdout on fd == 1 */
	      if (dup (oldfd) == 1) {
		close (oldfd);
	      }
	      else {
		sc_miscError (SH_ERR_BASE + SC_EBADBAD, 0);
	      }
	    }
	  }
	}
	else {
	  /* this is not a file name */
	  error = EINVAL;
	}
      }
      else {
	/* too many arguments :( */
	error = ENOMEM;
      }
      word = sh_nextword (self);
    }
    if (count > SH_ARG_MAX) {
      count = SH_ARG_MAX;
    }
    msg->inherit.argc = count;
    msg->inherit.argv[count] = NULL;
    if (_not && !error) {
      self->ret = !sh_launcher (msg, self->token == SH_TOKEN_PIPE
				|| self->token == SH_TOKEN_BG, &pid);
    }
    else if (!error) {
      self->ret = sh_launcher (msg, self->token == SH_TOKEN_PIPE
			       || self->token == SH_TOKEN_BG, &pid);
    }
    else {
      self->ret = -1;
      sc_miscErrnoSet (error);
    }
    if (fd != -1) {
      close (fd);
      /* reopen stdout on fd == 1 */
      if (dup (oldfd) != 1) {
	sc_miscError (SH_ERR_BASE + SC_EBADBAD, 0);
      }
      close (oldfd);
    }
    if (self->token == SH_TOKEN_PIPE) {
      word = sh_nextword (self);
      if (self->token != SH_TOKEN_ARG) {
	/* need arg token after a pipe else this is an error!! */
	self->ret = -1;
      }
      else {
	/* prepare pipe */
      }
    }
  } while (self->token == SH_TOKEN_ARG && self->ret != -1);
  if (self->token == SH_TOKEN_BG) {
    /* fake the system */
    self->token = SH_TOKEN_DELIM;
  }
}

static void
sh_term (sh_info_t NEARPTR self)
{
  PRINTF ("term\n");
  sh_factor (self);
  while (self->ret == 0 && self->token == SH_TOKEN_AND) {
    sh_factor (self);
  }
}

static void
sh_expression (sh_info_t NEARPTR self)
{
  PRINTF ("expression\n");
  sh_term (self);
  while (self->ret != 0 && self->token == SH_TOKEN_OR) {
    sh_term (self);
  }
}

static int
sh_line (sh_info_t NEARPTR self)
{
  PRINTF ("line\n");
  self->ch = sh_nextchar (self);
  sh_expression (self);
  while (self->token == SH_TOKEN_DELIM) {
    sh_expression (self);
  }
  return self->ret;
}

int
sh_system (const char *line)
{
  sh_info_t hook;
  strncpy (hook.buf, line, SC_PATH_MAX);
  strncat (hook.buf, "\n", SC_PATH_MAX);
  /* init hook for this line */
  hook.len = strlen (hook.buf);
  hook.cur = 0;
  hook.token = 0;
  hook.prevtoken = 0;
  hook.prevchar = '\0';
  hook.lastno = -1;
  hook.ret = 0;
  /* call line interpreter */
  return sh_line (&hook);
}

int
sh_shell (int argc, char *argv[])
{
  static const sc_msgid_t sel[2] = { SH_END, 0 };
  sc_msg_t msg;
  int len;
  sh_info_t hook;
  char prompt[16] = "sh>";
  opt_r_t options;
  int c;
  initopt_r (&options);
  while ((c = getopt_r (&options, argc, argv, "c:p:")) != -1) {
    switch (c) {
    case 'c':
      sh_system (options.optarg);
      break;
    case 'p':
      strncpy(prompt,options.optarg,sizeof(prompt)-2);
      prompt[sizeof(prompt)-1] = 0;
      break;
    default:
      break;
    }
  }

  for (;;) {
    fdprintf(scio_stdout,"\n%s",prompt);
    /* read next line */
    len = (int)read (0, hook.buf, SC_PATH_MAX);
    if (len < 0) {
      PRINTF ("read error\n");
      return -1;
    }
    if (len == 0) {
      PRINTF ("EOF");
      return 0;
    }
    fdprintf(scio_stdout,"\n\r");
    /* init hook for this line */
    hook.len = len;
    hook.cur = 0;
    hook.token = 0;
    hook.prevtoken = 0;
    hook.prevchar = '\0';
    hook.lastno = -1;
    hook.ret = 0;
    /* call line interpreter */
    sh_line (&hook);
    /* check for dead messages */
    while ((msg = sc_msgRx (0, sel, SC_MSGRX_MSGID))) {
#if SIZEOF_PID_T == 4
      fdprintf (scio_stderr, "done: %08x\n", sc_msgSndGet (&msg));
#else
      fdprintf (scio_stderr, "done: %04x\n", sc_msgSndGet (&msg));
#endif
      sc_msgFree (&msg);
    }
  }
#ifdef __GNUC__
  return 0;
#endif
}
