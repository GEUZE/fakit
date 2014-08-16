#ifndef _CFC_IDE_H_
#define _CFC_IDE_H_

/****************************************************************************
 *
 *            Copyright (c) 2003-2008 by HCC Embedded
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
#ifdef __cplusplus
extern "C" {
#endif

#define DRV_8051  /* driver for 8051 */

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

#define CFC_SELCARD	0xE0  /*drive 0*/
#define CFC_READ	0x20  /*LBA mode selected on Drive 0*/
#define CFC_WRITE	0x30  /*LBA mode selected on Drive 0*/
#define CFC_ID 		0xEC  /*LBA mode selected on Drive 0*/
#define CFC_SETFEA	0xEF  /*LBA mode selected on Drive 0*/

#define CFC_TOVALUE  130000  


/******************************************************************************
 *
 * CFC iobase and its registers
 *
 *****************************************************************************/
#define CFC_BASE  0xE000

/******************************************************************************
 *
 * Control bits
 *
 *****************************************************************************/
#define CFC_CDBIT 0x08 /*card removed*/
#define CFC_PWBIT 0x02 /*power on*/
#define CFC_RSTBIT 0x01 /*reset*/

/******************************************************************************
 *
 * Errors in CFC
 *
 *****************************************************************************/

#define CFC_ERR_NOTPLUGGED ((unsigned char)-1) /*for high level */

enum {
/*  0 */  CFC_NO_ERROR,  
/*  1 */  CFC_ERR_BUSY_ATCYL,
/*  2 */  CFC_ERR_BUSY_ATSEC,
/*  3 */  CFC_ERR_BUSY_ATCMD,
/*  4 */  CFC_ERR_TIMEOUT,
/*  5 */  CFC_ERR_STATE,
/*  6 */  CFC_ERR_SECCOU
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
unsigned char fnCFC(void *, unsigned long, unsigned char);
#define cfc_readsector(_data,sector) fnCFC(_data,sector,CFC_READ)
#define cfc_writesector(_data,sector) fnCFC(_data,sector,CFC_WRITE)
unsigned char cfc_getstatus(void);
#if F_FORMATTING
unsigned char cfc_getphy(F_PHY *);
#endif
unsigned char cfc_initfunc(void);

#define drv_readsector cfc_readsector
#define drv_writesector cfc_writesector
#define drv_getstatus cfc_getstatus
#define drv_initfunc cfc_initfunc
#if F_FORMATTING
#define drv_getphy cfc_getphy
#endif


#define CFC_PAGE_SIZE 512  /*CFC page size in bytes*/

/******************************************************************************
 *
 * end of cfc_ide.h
 *
 *****************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /*_CFC_IDE_H_*/

