/* -*-c++-*-
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
** (c) 2002,2003 Sciopta GmbH, Litronic AG/ Schweiz                 **
**                                                                  **
**********************************************************************
** ID: S03076BS9                                                    **
** +Date: 2003/10/23 11:41:46 +                                     ** 
** +Revision: 2.5 +                                                 **
** Floppydisc defines                                               **
**********************************************************************
*/
#ifndef _MACHINE_I386_FDC_H__
#define _MACHINE_I386_FDC_H__

/* datatypes */
#include <ossys/types.h>
#ifdef __cplusplus
extern "C" {
#endif
int fdd_waitUp(sc_ticks_t tmo);
#ifdef __cplusplus
}
#endif

/* drive geometries */
#define DG144_HEADS       2     /* heads per drive (1.44M) */
#define DG144_TRACKS     80     /* number of tracks (1.44M) */
#define DG144_SPT        18     /* sectors per track (1.44M) */
#define DG144_GAP3FMT  0x54     /* gap3 while formatting (1.44M) */
#define DG144_GAP3RW   0x1b     /* gap3 while reading/writing (1.44M) */

#define DG168_HEADS       2     /* heads per drive (1.68M) */
#define DG168_TRACKS     80     /* number of tracks (1.68M) */
#define DG168_SPT        21     /* sectors per track (1.68M) */
#define DG168_GAP3FMT  0x0c     /* gap3 while formatting (1.68M) */
#define DG168_GAP3RW   0x1c     /* gap3 while reading/writing (1.68M) */

/* IO ports */
#define FDC_DOR  (0x3f2)   /* Digital Output Register */
#define FDC_MSR  (0x3f4)   /* Main Status Register (input) */
#define FDC_DRS  (0x3f4)   /* Data Rate Select Register (output) */
#define FDC_DATA (0x3f5)   /* Data Register */
#define FDC_DIR  (0x3f7)   /* Digital Input Register (input) */
#define FDC_CCR  (0x3f7)   /* Configuration Control Register (output) */

/* command bytes (these are 765 commands + options such as MFM, etc) */
#define CMD_SPECIFY (0x03)  /* specify drive timings */
#define CMD_WRITE   (0xc5)  /* write data (+ MT,MFM) */
#define CMD_READ    (0xe6)  /* read data (+ MT,MFM,SK) */
#define CMD_RECAL   (0x07)  /* recalibrate */
#define CMD_SENSEI  (0x08)  /* sense interrupt status */
#define CMD_FORMAT  (0x4d)  /* format track (+ MFM) */
#define CMD_DUMPREGS 0x0e   /* dump the contents of the fdc regs */
#define CMD_SEEK    (0x0f)  /* seek track */
#define CMD_VERSION (0x10)  /* FDC version */
#define CMD_CONFIGURE 0x13
#define CMD_SENSE_INT 0x8
#define CMD_READ_RAW 0x06

#define FDC_MT 0x80 /* Multi-Track */
#define FDC_MFM 0x40 /* Modified Frequency Modulation = double density */
#define FDC_SK 0x20 /* Skip Control */
#define FDC_CMD_READ (FDC_CMD_READ_RAW | FDC_MT | FDC_MFM | FDC_SK)

/* fdc version */
#define FDC_VER_765A 0x80
#define FDC_VER_765B 0x90

/* command bytes */



#define FDC_DRIVE_A 0

#define FDC_BPS_512 2 /* bytes per sector */

/* Data Transfer Rate Encoding */
#define DRATE_500KBPS 0 /* high density */
#define DRATE_300KBPS 1
#define DRATE_250KBPS 2 /* double density */
#define DRATE_1MBPS 3


#define MSR_DRIVE0_BUSY     0x01
#define MSR_DRIVE1_BUSY     0x02
#define MSR_DRIVE2_BUSY     0x04
#define MSR_DRIVE3_BUSY     0x08
#define MSR_BUSY            0x10
#define MSR_NDMA            0x20
#define MSR_DIR             0x40
#define MSR_READY           0x80

#define DSR_DRATE_500KBS    0x00
#define DSR_DRATE_300KBS    0x01
#define DSR_DRATE_250KBS    0x02
#define DSR_DRATE_1000KBS   0x03
#define DSR_LOW_POWER       0x40
#define DSR_SOFTRESET       0x80

#define DIR_CHANGED         0x80

#define DOR_DRIVE_0         0x00
#define DOR_DRIVE_1         0x01
#define DOR_DRIVE_2         0x02
#define DOR_DRIVE_3         0x03
#define DOR_NO_RESET        0x04
#define DOR_DMA_ENABLE      0x08
#define DOR_MOTOR0          0x10
#define DOR_MOTOR1          0x20
#define DOR_MOTOR2          0x40
#define DOR_MOTOR3          0x80

#define TIMEOUT_COUNT 100000

#define FDC_MAX_REPLIES 16
#endif /* FDC_H */








