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
** ID: S03070CL59                                                   **
** +Revision: 1.7 +                                                **
** +Date: 2003/05/28 06:47:49 +                                     **
** termios                                                          **
**********************************************************************
EOC*/

#ifndef _SYS_TERMIOS_H
#define _SYS_TERMIOS_H	1

#include <ossys/defines.h>
#include <ossys/types.h>

/*
0x00005401 TCGETS struct termios *
0x00005402 TCSETS const struct termios *
0x00005403 TCSETSW const struct termios *
0x00005404 TCSETSF const struct termios *
0x00005405 TCGETA struct termio *
0x00005406 TCSETA const struct termio *
0x00005407 TCSETAW const struct termio *
0x00005408 TCSETAF const struct termio *
0x00005409 TCSBRK int 
0x0000540A TCXONC int 
0x0000540B TCFLSH int 
0x0000540C TIOCEXCL void 
0x0000540D TIOCNXCL void 
0x0000540E TIOCSCTTY int 
0x0000540F TIOCGPGRP pid_t *
0x00005410 TIOCSPGRP const pid_t *
0x00005411 TIOCOUTQ int *
0x00005412 TIOCSTI const char *
0x00005413 TIOCGWINSZ struct winsize *
0x00005414 TIOCSWINSZ const struct winsize *
0x00005415 TIOCMGET int *
0x00005416 TIOCMBIS const int *
0x00005417 TIOCMBIC const int *
0x00005418 TIOCMSET const int *
0x00005419 TIOCGSOFTCAR int *
0x0000541A TIOCSSOFTCAR const int *
0x0000541B FIONREAD int *
0x0000541B TIOCINQ int *
0x0000541C TIOCLINUX const char *	// MORE 
0x0000541D TIOCCONS void 
0x0000541E TIOCGSERIAL struct serial_struct *
0x0000541F TIOCSSERIAL const struct serial_struct *
0x00005420 TIOCPKT const int *
0x00005421 FIONBIO const int *
0x00005422 TIOCNOTTY void 
0x00005423 TIOCSETD const int *
0x00005424 TIOCGETD int *
0x00005425 TCSBRKP int 
0x00005426 TIOCTTYGSTRUCT struct tty_struct *
0x00005450 FIONCLEX void 
0x00005451 FIOCLEX void 
0x00005452 FIOASYNC const int *
0x00005453 TIOCSERCONFIG void 
0x00005454 TIOCSERGWILD int *
0x00005455 TIOCSERSWILD const int *
0x00005456 TIOCGLCKTRMIOS struct termios *
0x00005457 TIOCSLCKTRMIOS const struct temios *
0x00005458 TIOCSERGSTRUCT struct async_struct *
0x00005459 TIOCSERGETLSR int *
0x0000545A TIOCSERGETMULTI struct serial_multiport_struct *
0x0000545B TIOCSERSETMULTI const struct serial_multiport_struct *
*/

/* c_iflag */
#define IGNBRK  1	/* ignore BREAK condition on input */
#define BRKINT  2	/* If IGNBRK is not set, generate SIGINT on BREAK con­
		  	dition, else read BREAK as character \0. */
#define IGNPAR  4	/* ignore framing errors and parity errors. */
#define PARMRK  16	/* if  IGNPAR  is  not  set, prefix a character with a
	          	parity error or framing error  with  \377  \0.   If
	          	neither  IGNPAR nor PARMRK is set, read a character
	          	with a parity error or framing error as \0. */
#define INPCK   32	/* enable input parity checking */
#define ISTRIP  64	/* strip off eighth bit */
#define INLCR   128	/* translate NL to CR on input */
#define IGNCR   256	/* ignore carriage return on input */
#define ICRNL   512	/* translate  carriage  return  to  newline  on  input
		  	(unless IGNCR is set) */
#define IUCLC   1024	/* map uppercase characters to lowercase on input */
#define IXON    2048	/* enable XON/XOFF flow control on output */
#define IXANY   4096	/* enable any character to restart output */
#define IXOFF   8192	/* enable XON/XOFF flow control on input */
#define IMAXBEL 16384	/* ring bell when input queue is full */

/* c_oflag */
#define OPOST  	1	/* enable implementation-defined output processing */
#define OLCUC  	2	/* map lowercase characters to uppercase on output */
#define ONLCR  	4	/* map NL to CR-NL on output */
#define OCRNL  	8	/* map CR to NL on output */
#define ONOCR  	16	/* don't output CR at column 0 */
#define ONLRET 	32	/* don't output CR */
#define OFILL	128	/* send fill characters for a delay, rather than using
			a timed delay */
#define OFDEL  	256	/* fill character is ASCII DEL.  If unset, fill  char­
			acter is ASCII NUL */
#define NLDLY  	512 	/* newline delay mask.  Values are NL0 and NL1. */
#define CRDLY  	1024	/* carriage  return  delay mask.  Values are CR0, CR1,
			CR2, or CR3. */
#define TABDLY 	2048	/* horizontal tab delay mask.  Values are TAB0,  TAB1,
			TAB2,  TAB3,  or  XTABS.   A value of XTABS expands
			tabs  to  spaces  (with  tab  stops   every   eight
			columns). */
#define BSDLY  	4096	/* backspace delay mask.  Values are BS0 or BS1. */
#define VTDLY  	8192	/* vertical tab delay mask.  Values are VT0 or VT1. */
#define FFDLY   16384	/* form feed delay mask.  Values are FF0 or FF1. */

/* c_cflag */
#define CSIZE	1	/* character  size mask.  Values are CS5, CS6, CS7, or
                     	CS8. */
#define CSTOPB 	2	/* set two stop bits, rather than one. */
#define CREAD  	4	/* enable receiver. */
#define PARENB 	8	/* enable  parity  generation  on  output  and  parity
			checking for input. */
#define PARODD 	16	/* parity for input and output is odd. */
#define HUPCL  	32	/* lower modem control lines after last process closes
			the device (hang up). */
#define CLOCAL 	64	/* ignore modem control lines */
#define CIBAUD 	128	/* mask for input speeds (not used). */
#define CRTSCTS	256	/* flow control. */

/* c_lflag */
#define ISIG   	1	/* when any of the characters  INTR,  QUIT,  SUSP,  or
                     	DSUSP are received, generate the corresponding sig­
		        nal. */
#define ICANON 	2	/* enable canonical mode.  This  enables  the  special
			characters  EOF,  EOL,  EOL2, ERASE, KILL, REPRINT,
			STATUS, and WERASE, and buffers by lines. */
#define XCASE  	4	/* if ICANON is also set, terminal is uppercase  only.
		        Input is converted to lowercase, except for charac­
		        ters preceded by \.  On output,  uppercase  charac­
		        ters are preceded by \ and lowercase characters are
		        converted to uppercase. */
#define ECHO   	8	/* echo input characters. */
#define ECHOE  	16	/* if ICANON is also set, the ERASE  character  erases
		        the  preceding  input  character, and WERASE erases
		        the preceding word. */
#define ECHOK  	32	/* if ICANON is also set, the  KILL  character  erases
			the current line. */
#define ECHONL 	64	/* if  ICANON  is also set, echo the NL character even
	                if ECHO is not set. */
#define ECHOCTL	128	/* if ECHO is also set, ASCII  control  signals  other
			than  TAB,  NL,  START,  and STOP are echoed as ^X,
			where X is  the  character  with  ASCII  code  0x40
			greater  than  the  control  signal.   For example,
			character 0x08 (BS) is echoed as ^H. */
#define ECHOPRT	256	/* if ICANON and IECHO are also  set,  characters  are
			printed as they are being erased. */
#define ECHOKE 	512	/* if  ICANON  is  also set, KILL is echoed by erasing
		        each character on the line, as specified  by  ECHOE
		        and ECHOPRT. */
#define FLUSHO 	1024	/* output  is  being flushed.  This flag is toggled by
		        typing the DISCARD character. */
#define NOFLSH 	2048	/* disable flushing the input and output  queues  when
			generating  the  SIGINT  and  SIGQUIT  signals, and
			flushing the input queue when generating  the  SIG­
			SUSP signal. */
#define TOSTOP 	4096	/* send  the  SIGTTOU signal to the process group of a
			background process which tries to write to its con­
			trolling terminal. */
#define PENDIN 	8192	/* all  characters  in  the  input queue are reprinted
			when the next character  is  read.   (bash  handles
			typeahead this way.) */
#define IEXTEN 	16384	/* enable implementation-defined input processing. */

/* c_cc */
#define NCCS		20	

#define VINTR		0
#define VQUIT		1
#define VERASE		2
#define VKILL		3
#define VEOF		4
#define VMIN		5
#define VEOL		6
#define VTIME		7
#define VEOL2		8
#define VSWTCH		9
#define VSTART		10	
#define VSTOP		11	
#define VSUSP		12
#define VDSUSP		13
#define VLNEXT		14
#define VWERASE		15
#define VREPRINT	16
#define VDISCARD	17
#define VCR		18
#define VNL		19

/* c_speed */
#define B0	0
#define B50	1	
#define B75	2
#define B110	3
#define B134	4
#define B150	5
#define B300	6
#define B600	7
#define B1200	8
#define B1800	9 
#define B2000	10
#define B2400	11
#define B3600	12
#define B4800	13
#define B7200	14
#define B9600	15
#define B19200	16
#define B38400	17
#define	B57600	18
#define B115200	19
#define B230400	20

#define BAUD_STR_ARRAY \
"0",\
"50",\
"75", \
"110", \
"134", \
"150", \
"300", \
"600", \
"1200", \
"1800", \
"2000", \
"2400", \
"3600", \
"4800", \
"7200", \
"9600", \
"19200", \
"38400", \
"57600", \
"115200", \
"230400"

typedef flags_t tcflag_t;
typedef unsigned char cc_t;
typedef struct termios {
  sc_msgid_t	id;
  tcflag_t 	c_iflag;	/* input modes */
  tcflag_t 	c_oflag;	/* output modes */
  tcflag_t	c_cflag;	/* control modes */
  tcflag_t 	c_lflag;	/* local modes */
  cc_t 		c_cc[NCCS];	/* control chars */
  int		c_speed;	/* baudrate */
} termios_t;

#endif
