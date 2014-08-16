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
 * Vaci Ut 110
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/

#ifndef _OHCI_REG_H
#define _OHCI_REG_H

#include <hcc/eusbh/usb_driver/ohcib/ohci.h>

#define ALIGN16(a)				((a+0xf)&(~0xf))	/* align address to 16 */
#define ALIGN256(a)				((a+0xff)&(~0xff))	/* align address to 256 */

/* OHCI register and bit definition */
#define HcRevision			*(hcc_reg32 *)(OHCI_BASE+0x00)


#define HcControl			*(hcc_reg32 *)(OHCI_BASE+0x04)
  #define CBSR					(3<<0)
    #define CBSR_1_1			(0<<0)
    #define CBSR_2_1			(1<<0)
    #define CBSR_3_1			(2<<0)
    #define CBSR_4_1			(3<<0)

  #define PLE					(1<<2)
  #define IE					(1<<3)
  #define CLE					(1<<4)
  #define BLE					(1<<5)

  #define HCFS					(3<<6)
  	#define HCFS_RESET			(0<<6)
  	#define HCFS_RESUME			(1<<6)
  	#define HCFS_OPERATIONAL	(2<<6)
  	#define HCFS_SUSPEND		(3<<6)
  	
  #define IR					(1<<8)
  #define RWC					(1<<9)
  #define RWE					(1<<10)


#define HcCommandStatus		*(hcc_reg32 *)(OHCI_BASE+0x08)
  #define HCR					(1<<0)
  #define CLF					(1<<1)
  #define BLF					(1<<2)
  #define OCR					(1<<3)
  #define SOC					(3<<16)


#define HcInterruptStatus	*(hcc_reg32 *)(OHCI_BASE+0x0c)
#define HcInterruptEnable	*(hcc_reg32 *)(OHCI_BASE+0x10)
#define HcInterruptDisable	*(hcc_reg32 *)(OHCI_BASE+0x14)
  #define SO					(1<<0)
  #define WDH					(1<<1)
  #define SF					(1<<2)
  #define RD					(1<<3)
  #define UE					(1<<4)
  #define FNO					(1<<5)
  #define RHSC					(1<<6)
  #define OC					(1<<30)
  #define MIE					(1<<31)


#define HcHCCA				*(hcc_reg32 *)(OHCI_BASE+0x18)
#define HcPeriodCurrentED	*(hcc_reg32 *)(OHCI_BASE+0x1c)
#define HcControlHeadED		*(hcc_reg32 *)(OHCI_BASE+0x20)
#define HcControlCurrentED	*(hcc_reg32 *)(OHCI_BASE+0x24)
#define HcBulkHeadED		*(hcc_reg32 *)(OHCI_BASE+0x28)
#define HcBulkCurrentED		*(hcc_reg32 *)(OHCI_BASE+0x2c)
#define HcDoneHead			*(hcc_reg32 *)(OHCI_BASE+0x30)


#define HcFmInterval		*(hcc_reg32 *)(OHCI_BASE+0x34)
  #define FI					(0x3fff<<0)
  #define FSMPS					(0x7fff<<16)
    #define _FSMPS(x)			(x<<16)
  #define FIT					(1<<31)

#define HcFmRemaining		*(hcc_reg32 *)(OHCI_BASE+0x38)
  #define FR					(0x3fff<<0)
  #define FRT					(1<<31)

#define HcFmNumber			*(hcc_reg32 *)(OHCI_BASE+0x3c)
  #define FN					(0xffff<<0)

#define HcPeriodStart		*(hcc_reg32 *)(OHCI_BASE+0x40)
  #define PS					(0x3fff<<0)
    #define _PS(x)				(x<<0)

#define HcLSThreshold		*(hcc_reg32 *)(OHCI_BASE+0x44)
  #define LST					(0x7ff<<0)


#define HcRhDescriptorA		*(hcc_reg32 *)(OHCI_BASE+0x48)
  #define NDP					(0xff<<0)
  #define PSM					(1<<8)
  #define NPS					(1<<9)
  #define DT					(1<<10)
  #define OCPM					(1<<11)
  #define NOCP					(1<<12)
  #define POTPGT				(0xff<<24)
    #define _POTPGT(x)			(x<<24)

#define HcRhDescriptorB		*(hcc_reg32 *)(OHCI_BASE+0x4c)
  #define DR					(0xffff<<0)
    #define _DR(x)				(x<<0)
  #define PPCM					(0xffff<<16)
    #define _PPCM(x)			(x<<16)

#define HcRhStatus			*(hcc_reg32 *)(OHCI_BASE+0x50)
  #define LPS					(1<<0)
  #define OCI					(1<<1)
  #define DRWE					(1<<15)
  #define LPSC					(1<<16)
  #define OCIC					(1<<17)
  #define CRWE					(1<<31)

#define HcRhPortStatus(x)	*(hcc_reg32 *)(OHCI_BASE+(0x50+(x<<2)))	/* min=1 (0x54) / max=15 (0x8c) */
  #define CCS					(1<<0)
  #define PES					(1<<1)
  #define PSS					(1<<2)
  #define POCI					(1<<3)
  #define PRS					(1<<4)
  #define PPS					(1<<8)
  #define LSDA					(1<<9)
  #define CSC					(1<<16)
  #define PESC					(1<<17)
  #define PSSC					(1<<18)
  #define POCIC					(1<<19)
  #define PRSC					(1<<20)

#define _rADDR(x)	(void *)((x)&~(0xf))					/* read 16 byte aligned address */
#define _wADDR(v,x)	v=(((hcc_u32)(x))&~(0xf))|((v)&0xf)		/* write 16 byte aligned address */

/*
** Endpoint descriptor
*/
#define _bED_FA		0x7f		/* function address */
#define _bED_EN		0xf			/* endpoint number */
#define _bED_D		0x3			/* direction */
#define _bED_S		1			/* speed */
#define _bED_K		1			/* skip bit */
#define _bED_F		1			/* TD format */
#define _bED_MPS	0x7ff		/* maximum pcaket size */
#define _bED_H		1			/* halt */
#define _bED_C		1			/* toggle carry */

#define _sED_FA		0
#define _sED_EN		7
#define _sED_D		11
#define _sED_S		13
#define _sED_K		14
#define _sED_F		15
#define _sED_MPS	16
#define _sED_H		0
#define _sED_C		1

#define _mED(f)			(_bED_##f<<_sED_##f)				/* mask */
#define _rED(x,f)		(((x)>>_sED_##f)&_bED_##f)			/* read: x-value / f-field */
#define _wED(v,f,x)		v=((v)&~_mED(f))|((x)<<_sED_##f)	/* write: v-variable / f-field / x-value */

typedef volatile struct {
  hcc_u32 ctrl;			/* control word */
  hcc_u32 tailp;		/* tail pointer */
  hcc_u32 headp;		/* head pointer + C/H*/
  hcc_u32 nexted;		/* next ED */
} t_ed;


/*
** Transfer descriptor
*/
#define _bTD_R			1		/* buffer rounding */
#define _bTD_DP			0x3		/* direction PID */
#define _bTD_DI			0x7		/* delay interrupt */
#define _bTD_T			0x3		/* data toggle */
#define _bTD_EC			0x3		/* error count */
#define _bTD_CC			0xf		/* completion code */

#define _sTD_R			18
#define _sTD_DP			19
#define _sTD_DI			21
#define _sTD_T			24
#define _sTD_EC			26
#define _sTD_CC			28

#define _mTD(f)			(_bTD_##f<<_sTD_##f)
#define _rTD(x,f)		(((x)>>_sTD_##f)&_bTD_##f)
#define _wTD(v,f,x)		v=((v)&~_mTD(f))|((x)<<_sTD_##f)

typedef volatile struct {
  hcc_u32 ctrl;
  hcc_u32 cbp;
  hcc_u32 nexttd;
  hcc_u32 be;
} t_td;


/*
** HCCA
*/
#define _bHCCA_UNMASK	1

#define _sHCCA_UNMASK	0

#define _mHCCA(f)		(_bHCCA_##f<<_sHCCA_##f)
#define _rHCCA(x,f)		(((x)>>_sHCCA_##f)&_bHCCA_##f)
#define _wHCCA(v,f,x)	v=((v)&~_mHCCA(f))|((x)<<_sHCCA_##f)

typedef volatile struct {
  hcc_u32 int_table[32];
  hcc_u16 frame_number;
  hcc_u16 pad1;
  hcc_u32 done_head;	/* D0:unmask */
  hcc_u8 reserved[116];
} t_hcca;



#endif


