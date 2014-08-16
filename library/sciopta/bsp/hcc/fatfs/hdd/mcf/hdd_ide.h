#ifndef _HDD_IDE_H_
#define _HDD_IDE_H_

/****************************************************************************
 *
 *            Copyright (c) 2003-2007 by HCC Embedded
 *
 * This software is copyrighted by and is the sole property of
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1133
 * Vaci ut 110
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/


#include "../../common/fat.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HCC_HW

#ifdef HCC_HW
#define FAT_CONF_5272
#endif

#define F_HDD_DRIVE0 0
#define F_HDD_DRIVE1 1


/******************************************************************************
 *
 * HDD data line bit selection
 *
 *****************************************************************************/
#define HDD16BITDATA 1   /* 1-means 16 bit data line, 0-means 8 bit data line */

/******************************************************************************
 *
 * Status bit definitions
 *
 *****************************************************************************/
#define HDD_STATE_BUSY 0x800000
#define HDD_STATE_RDY  0x400000
#define HDD_STATE_DWF  0x200000
#define HDD_STATE_DSC  0x100000
#define HDD_STATE_DRQ  0x080000
#define HDD_STATE_ERR  0x010000

/******************************************************************************
 *
 * command definitions
 *
 *****************************************************************************/
#define HDD_READ	0x200000
#define HDD_READMULTIPLE	0xc40000
#define HDD_WRITE	0x300000
#define HDD_WRITEMULTIPLE	0xc50000
#define HDD_SETMULTIPLE	0xc60000
#define HDD_ID 		0xec0000
#define HDD_SETFEA	0xef0000


#ifdef HCC_HW
#define HDD_TOVALUE  1300000
#define HDD_INIT_TO	 100000
#endif

/******************************************************************************
 *
 * HDD chip select
 *
 *****************************************************************************/
#define HDD_BASE0  0xb0000000

#ifdef HCC_HW

#ifdef FAT_CONF_5282
#define HDD_CSADDR0 (*(volatile unsigned short *)(0x4000008c))
#define HDD_CSMASK0 (*(volatile unsigned long *)(0x40000090))
#define HDD_CSOPT0 (*(volatile unsigned short *)(0x40000096))
#else
#define HDD_CSBASE0 (*(volatile unsigned long *)(0x10000048))   /* HDD's Chip Select Base regiter */
#define HDD_CSOPT0 (*(volatile unsigned long *)(0x1000004c))   /* HDD's Chip Select Option regiter */
#endif

#define HDD_CONTROL0 (*(volatile unsigned long *)(HDD_BASE0+0x44)) /* HDD interface (HCC) register */
#endif

/******************************************************************************
 *
 * HDD iobase and its registers
 *
 *****************************************************************************/

#define HDD_DATA		0x00
#define HDD_FEATURE		0x04
#define HDD_SECTORCOU		0x08
#define HDD_SECTORNO		0x0c
#define HDD_CYLINDERLO		0x10
#define HDD_CYLINDERHI		0x14
#define HDD_SELC		0x18
#define HDD_COMMAND		0x1c
#define HDD_STATE		HDD_COMMAND

#define HDD_VAL(hdd,n)		(*(volatile unsigned long *)((hdd->base)+n))
#define HDD_PTR(hdd,n)		((volatile unsigned long *)((hdd->base)+n))

#define HDD_SELCARD(hdd)	(hdd->select_card)

/******************************************************************************
 *
 * HDD controller regs
 *
 *****************************************************************************/

/******************************************************************************
 *
 * Errors in HDD
 *
 *****************************************************************************/

enum {
/* 0 */  HDD_NO_ERROR,
/*101*/  HDD_ERR_BUSY_ATCYL=100,
/*102*/  HDD_ERR_BUSY_ATDRQ,
/*103*/  HDD_ERR_BUSY_ATCMD,
/*104*/  HDD_ERR_TIMEOUT,
/*105*/  HDD_ERR_STATE,
/*106*/  HDD_ERR_SECCOU,
/*107*/  HDD_ERR_LBANOTSUPPORTED,
/*108*/  HDD_ERR_NOTAVAILABLE
};

/******************************************************************************
 *
 *  Big endian definitions
 *
 *****************************************************************************/

#define MOTOWORD(x) ( (((x)>>8) & 0x00ff) | (((x)<<8) & 0xff00) )

/******************************************************************************
 *
 *  Functions
 *
 *****************************************************************************/

extern F_DRIVER *hdd_initfunc(unsigned long driver_param); /* driver init function */

#define HDD_PAGE_SIZE 512  /* HDD page size in bytes */

#ifdef __cplusplus
}
#endif

/******************************************************************************
 *
 * end hdd_ide.h
 *
 *****************************************************************************/

#endif /* _HDD_IDE_H_ */

