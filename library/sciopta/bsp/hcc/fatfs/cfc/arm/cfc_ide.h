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
#include "../../common/common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HCC_HW

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
#define CFC_STATE_BUSY 0x80
#define CFC_STATE_RDY  0x40
#define CFC_STATE_DWF  0x20
#define CFC_STATE_DSC  0x10
#define CFC_STATE_DRQ  0x08
#define CFC_STATE_ERR  0x01

/******************************************************************************
 *
 * command definitions
 *
 *****************************************************************************/
#define CFC_READ	0x20
#define CFC_READMULTIPLE	0xc4
#define CFC_WRITE	0x30
#define CFC_WRITEMULTIPLE	0xc5
#define CFC_SETMULTIPLE	0xc6
#define CFC_ID 		0xec
#define CFC_SETFEA	0xef

#define CFC_TOVALUE  1300000

/******************************************************************************
 *
 * CFC chip select
 *
 *****************************************************************************/
#define CFC_BASE0  0x01000000

#ifdef HCC_HW
#define CFC_CSBASE (*(volatile unsigned long *)(0xffc00040))
#define CFC_CSOPTA (*(volatile unsigned long *)(0xffc00044))
#define CFC_CSOPTB (*(volatile unsigned long *)(0xffc00048))

#define CFC_CPLDCMD1      (*(volatile unsigned short*)(CFC_BASE0+0x2000))
#define CFC_CPLDCMD2      (*(volatile unsigned short*)(CFC_BASE0+0x2002))
#define CFC_CPLDSTATE0    ((volatile unsigned short*)(CFC_BASE0+0x2000))

#define CFC_CPLDSTATE_CDCH0 0x8 /*card changed*/
#define CFC_CPLDSTATE_CFCD0 0x4 /*card removed*/

#endif

/******************************************************************************
 *
 * CFC iobase and its registers
 *
 *****************************************************************************/

#define CFC_DATA		0x1000
#define CFC_FEATURE		0x1002
#define CFC_SECTORCOU		0x1004
#define CFC_SECTORNO		0x1006
#define CFC_CYLINDERLO		0x1008
#define CFC_CYLINDERHI		0x100a
#define CFC_SELC		0x100c
#define CFC_COMMAND		0x100e
#define CFC_STATE		CFC_COMMAND

#define CFC_VAL(n)		(*(volatile unsigned short *)((cfc->base)+n))
#define CFC_PTR(n)		((volatile unsigned short *)((cfc->base)+n))

#define CFC_SELCARD(cfc)	(cfc->select_card)

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

