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
** ID: S03070CL58                                                   **
** +Revision: 1.21 +                                                **
** +Date: 2008/01/16 13:02:00 +                                     **
** ioctl                                                            **
**********************************************************************
EOC*/

#ifndef _SYS_IOCTL_H
#define _SYS_IOCTL_H 	1

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/defines.h>
  
#ifdef __IAR_SYSTEMS_ICC__
/* disable MISRA C Rule 69*/
/* POSIX definition is like this ... */
#pragma diag_suppress=Pm064
#endif
  int ioctl (int fd, unsigned int cmd, ...);
#ifdef __IAR_SYSTEMS_ICC__
/* enable MISRA C Rule 69*/
#pragma diag_default=Pm064
#endif

/** serial port commands 
  */
#define SERSETEOF	IOCTLW (IOCTL_SERIAL_BASE, 1, unsigned char *) 
  /* set end of file character */
#define SERGETEOF	IOCTLR (IOCTL_SERIAL_BASE, 2, unsigned char *) 
  /* get end of file character currently used in the serial driver */
#define SERSETEOL	IOCTLW (IOCTL_SERIAL_BASE, 3, unsigned char *) 
  /* set end of line character */
#define SERGETEOL	IOCTLR (IOCTL_SERIAL_BASE, 4, unsigned char *) 
  /* get end of line character currently used in the serial driver */
#define SERSETBAUD	IOCTLW (IOCTL_SERIAL_BASE, 5, int *)
  /* set baudrate */
#define SERGETBAUD	IOCTLR (IOCTL_SERIAL_BASE, 6, int *)
  /* get baudrate */
#define SERECHO		IOCTLRW(IOCTL_SERIAL_BASE, 7, int *) 
  /* read/set/clear echo mode */
#define SERTMO		IOCTLRW(IOCTL_SERIAL_BASE, 8, int *) 
  /* read/set/clear tmo mode */
#define SERXONXOFF	IOCTLRW(IOCTL_SERIAL_BASE, 9, int *)
  /* read/set/clear xonxoff */
#define SERRTSCTS	IOCTLRW(IOCTL_SERIAL_BASE, 10, int *) 
  /* read/set/clear rtscts */
#define SERTTY		IOCTLRW(IOCTL_SERIAL_BASE, 11, int *)
  /* set TTY mode */
#define SERGETFLAG	IOCTLW (IOCTL_SERIAL_BASE, 12, unsigned char *)
  /* get serial flags */
#define SERSETFLAG	IOCTLW (IOCTL_SERIAL_BASE, 13, unsigned char *)
  /* set serial flags */

/*
** Console commands
*/
#define CONSETFGCOLOR           IOCTLW(IOCTL_CON_BASE,1,unsigned int *)
#define CONSETBGCOLOR           IOCTLW(IOCTL_CON_BASE,2,unsigned int *)
#define CONSETXY                IOCTLW(IOCTL_CON_BASE,3,unsigned long *)
#define CONGETXY                IOCTLW(IOCTL_CON_BASE,4,unsigned long *)

/** tty commands
  */
#define TCGETS		IOCTLR (IOCTL_TERMIOS_BASE, 1, struct termios *)
  /* get termios infos */
#define TCSETS		IOCTLW (IOCTL_TERMIOS_BASE, 2, const struct termios *)
  /* set termios infos */

/** block device commands
  */
#define BLKDEVGETPRM	IOCTLR (IOCTL_BLKDEV_BASE, 1, blkdev_geometry_t *)
  /* get blockdevice parameter */
#define BLKDEVSETPRM	IOCTLW (IOCTL_BLKDEV_BASE, 2, blkdev_geometry_t *) 
  /* set blockdevice parameter */
 
/** hdlc commands
  */
#define	HDLCIOCGASYNCMAP	IOCTLR (IOCTL_HDLC_BASE, 1, int *)	
  /* get async map */
#define	HDLCIOCSASYNCMAP	IOCTLW (IOCTL_HDLC_BASE, 2, int *)
  /* set async map */
#define	HDLCIOCGMRU		IOCTLR (IOCTL_HDLC_BASE, 3, int *)
  /* get max receive unit */
#define	HDLCIOCSMRU		IOCTLW (IOCTL_HDLC_BASE, 4, int *)
  /* set max receive unit */
#define	HDLCIOCGMTU		IOCTLR (IOCTL_HDLC_BASE, 5, int *)
  /* get max transmit unit */
#define	HDLCIOCSMTU		IOCTLW (IOCTL_HDLC_BASE, 6, int *)
  /* set max transmit unit */
#define HDLCIOCGXASYNCMAP 	IOCTLR (IOCTL_HDLC_BASE, 7, ext_accm *)
  /* get extended ACCM */
#define HDLCIOCSXASYNCMAP	IOCTLW (IOCTL_HDLC_BASE, 8, ext_accm *)
  /* set extended ACCM */
#define HDLCIOCGIDLE		IOCTLR (IOCTL_HDLC_BASE, 9, struct ppp_idle *)
  /* get idle time */

/** connector commands 
*/

#define ASYNCSETBAUD	IOCTLW (IOCTL_CON_BASE, 1, int *)
  /* set baudrate */
#define ASYNCGETBAUD	IOCTLR (IOCTL_CON_BASE, 2, int *)
  /* get baudrate */
#define ASYNCSETFLAG	IOCTLW (IOCTL_CON_BASE, 3, unsigned char *)
  /* get serial flags */
#define ASYNCGETFLAG	IOCTLR (IOCTL_CON_BASE, 4, unsigned char *)
  /* set serial flags */
#define ASYNCSETRCVMAX	IOCTLW (IOCTL_CON_BASE, 5, int *)
  /* set receive max size */
#define ASYNCGETRCVMAX	IOCTLR (IOCTL_CON_BASE, 6, int *)
  /* set receive max size */
#define ASYNCSETPOOLID	IOCTLW (IOCTL_CON_BASE, 7, sc_poolid_t *)
  /* set receive max size */
#define ASYNCGETPOOLID	IOCTLR (IOCTL_CON_BASE, 8, sc_poolid_t *)
  /* set receive max size */

  
#ifdef __cplusplus
}
#endif
#endif
