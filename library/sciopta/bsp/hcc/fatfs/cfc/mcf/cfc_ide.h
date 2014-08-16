#ifndef _CFC_IDE_H_
#define _CFC_IDE_H_

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

#define F_CFC_DRIVE0 0
#define F_CFC_DRIVE1 1


/******************************************************************************
 *
 * CFC data line bit selection
 *
 *****************************************************************************/
#define CFC16BITDATA 1   /* 1-means 16 bit data line, 0-means 8 bit data line */

/******************************************************************************
 *
 * Status bit definitions
 *
 *****************************************************************************/
#define CFC_STATE_BUSY 0x800000
#define CFC_STATE_RDY  0x400000
#define CFC_STATE_DWF  0x200000
#define CFC_STATE_DSC  0x100000
#define CFC_STATE_DRQ  0x080000
#define CFC_STATE_ERR  0x010000

/******************************************************************************
 *
 * command definitions
 *
 *****************************************************************************/
#define CFC_READ	0x200000
#define CFC_READMULTIPLE	0xc40000
#define CFC_WRITE	0x300000
#define CFC_WRITEMULTIPLE	0xc50000
#define CFC_SETMULTIPLE	0xc60000
#define CFC_ID 		0xec0000
#define CFC_SETFEA	0xef0000

#define CFC_TOVALUE  1300000

/******************************************************************************
 *
 * CFC chip select
 *
 *****************************************************************************/
#define CFC_BASE0  0xb0000000

#ifdef HCC_HW
#ifdef FAT_CONF_5282
#define CFC_CSADDR0 (*(volatile unsigned short *)(0x4000008c))
#define CFC_CSMASK0 (*(volatile unsigned long *)(0x40000090))
#define CFC_CSOPT0 (*(volatile unsigned short *)(0x40000096))
#else
#define CFC_CSBASE0 (*(volatile unsigned long *)(0x10000048))   /* CFC's Chip Select Base regiter */
#define CFC_CSOPT0 (*(volatile unsigned long *)(0x1000004c))   /* CFC's Chip Select Option regiter */
#endif

#define CFC_CPLDSTATE_CDCH0 0x80000000 /* card changed */
#define CFC_CPLDSTATE_CFCD0 0x40000000 /* card removed */
#define CFC_CPLDSTATE0 ((volatile unsigned long *)(CFC_BASE0+0x40)) /* CFC interface (HCC) register */
#endif

/******************************************************************************
 *
 * CFC iobase and its registers
 *
 *****************************************************************************/

#define CFC_DATA		0x00
#define CFC_FEATURE		0x04
#define CFC_SECTORCOU		0x08
#define CFC_SECTORNO		0x0c
#define CFC_CYLINDERLO		0x10
#define CFC_CYLINDERHI		0x14
#define CFC_SELC		0x18
#define CFC_COMMAND		0x1c
#define CFC_STATE		CFC_COMMAND

#define CFC_VAL(n)		(*(volatile unsigned long *)((cfc->base)+n))
#define CFC_PTR(n)		((volatile unsigned long *)((cfc->base)+n))

#define CFC_SELCARD(p)	(cfc->select_card)

/******************************************************************************
 *
 * CFC controller regs
 *
 *****************************************************************************/

/******************************************************************************
 *
 * Errors in CFC
 *
 *****************************************************************************/
#define CFC_ERR_NOTPLUGGED -1 /* for high level */

enum {
/* 0 */  CFC_NO_ERROR,
/*101*/  CFC_ERR_BUSY_ATCYL=101,
/*102*/  CFC_ERR_BUSY_ATDRQ,
/*103*/  CFC_ERR_BUSY_ATCMD,
/*104*/  CFC_ERR_TIMEOUT,
/*105*/  CFC_ERR_STATE,
/*106*/  CFC_ERR_SECCOU,
/*107*/  CFC_ERR_NOTAVAILABLE
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

extern F_DRIVER *cfc_initfunc(unsigned long driver_param); /* driver init function */

#define CFC_PAGE_SIZE 512  /* CFC page size in bytes */

#ifdef __cplusplus
}
#endif

/******************************************************************************
 *
 * end cfc_ide.h
 *
 *****************************************************************************/

#endif /* _CFC_IDE_H_ */

