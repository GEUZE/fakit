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
#ifndef _EHCI_REG_H
#define _EHCI_REG_H

#ifdef __cplusplus
extern "C" {
#endif


#include <hcc/eusbh/usb_driver/ehci/ehci.h>

#undef PPC /* sciopta/gcc for PPC define this */

#define EHCI_PAGE_COUNT		5
#define EHCI_PAGE_SIZE		4096


#define ALIGN_NEXT_PAGE(p)	(((hcc_u32)(p)+EHCI_PAGE_SIZE)&~(EHCI_PAGE_SIZE-1))
#define ALIGN_PAGE(p)		(((hcc_u32)(p)+(EHCI_PAGE_SIZE-1))&~(EHCI_PAGE_SIZE-1))
#define ALIGN4096(p)		(((hcc_u32)(p)+4095)&~4095)
#define ALIGN64(p)			(((hcc_u32)(p)+63)&~63)
#define ALIGN32(p)			(((hcc_u32)(p)+31)&~31)

#define th_itd				0
#define th_qh				1
#define th_sitd				2
#define th_fstn				3



/* write, read, set bit, clear bit, get bit */
#if EHCI_DENDIAN
#define _EHCI_V32(v)		((((v)&0xff)<<24)|(((v)&0xff00)<<8)|(((v)&0xff0000)>>8)|(((v)&0xff000000)>>24))
#else
#define _EHCI_V32(v)		(v)
#endif

#define _r32(d,r)			(d=r,d=_EHCI_V32(d))	/* destination, register */
#define _w32(r,v)			((r)=_EHCI_V32(v))	/* register, value */

#define _sb32(r,b)			((r)|=_EHCI_V32(b))	/* register, bit */
#define _cb32(r,b)			((r)&=~_EHCI_V32(b))	/* regsiter, bit */
#define _gb32(r,b)			((r)&_EHCI_V32(b))	/* register, bit */

#define _rADDR(x)			(void *)(_EHCI_V32(x)&~(0x1f))						/* read 32 byte aligned address */
#define _wADDR(v,x)			v=_EHCI_V32(((hcc_u32)(x))&~(0x1f))|((v)&_EHCI_V32(0x1e))	/* write 32 byte aligned address (T=0) */

/******************** Register access ********************/
#define _mREG(f)			(f##_m<<f##_s)
#define _vF(f,x)			(x<<f##_s)
#define _rF(v,f)			((v>>f##_s)&f##_m)

/******************** HC capability registers ********************/
#define CAPLENGTH			*(hcc_reg8 *)(EHCI_BASE+0x00)

#define HCIVERSION			*(hcc_reg16 *)(EHCI_BASE+0x02)

#define HCSPARAMS			*(hcc_reg32 *)(EHCI_BASE+0x04)
  #define N_PORTS_m			0xf
  #define N_PORTS_s			0

  #define N_PCC_m			0xf
  #define N_PCC_s			8

  #define N_CC_m			0xf
  #define N_CC_s			12

  #define DPN_m				0xf
  #define DPN_s				20

  #define PPC				(1<<4)
  #define PRR				(1<<7)
  #define P_INDICATOR		(1<<16)

#define HCCPARAMS			*(hcc_reg32 *)(EHCI_BASE+0x08)
  #define IST_m				0xf
  #define IST_s				4

  #define EECP_m			0xff
  #define EECP_s			8

  #define ADDR64			(1<<0)
  #define PFLF				(1<<1)
  #define ASPC				(1<<2)

#define HCSP_PORTROUTE		*(hcc_reg8 *)(EHCI_BASE+0x0c)


/******************** HC operational registers ********************/
#define USBCMD				*(hcc_reg32 *)(EHCI_BASE+0x40)
  #define FLS_m				0x3
  #define FLS_s				2
    #define FLS_256			2
    #define FLS_512			1
    #define FLS_1024		0

  #define ASPMC_m			0x3
  #define ASPMC_s			8

  #define ITC_m				0xff
  #define ITC_s				16

  #define RS				(1<<0)
  #define HCRESET			(1<<1)
  #define PSE				(1<<4)
  #define ASE				(1<<5)
  #define IOAAD				(1<<6)
  #define LHCR				(1<<7)
  #define ASPME				(1<<11)

#define USBSTS				*(hcc_reg32 *)(EHCI_BASE+0x44)
  #define USBINT			(1<<0)
  #define USBERRINT			(1<<1)
  #define PCD				(1<<2)
  #define FLR				(1<<3)
  #define HSE				(1<<4)
  #define IOAA				(1<<5)
  #define HCHALTED			(1<<12)
  #define RECLAMATION		(1<<13)
  #define PSS				(1<<14)
  #define ASS				(1<<15)

#define USBINTR				*(hcc_reg32 *)(EHCI_BASE+0x48)
  #define UIE				(1<<0)
  #define UEIE				(1<<1)
  #define PCIE				(1<<2)
  #define FLRE				(1<<3)
  #define HSEE				(1<<4)
  #define IOAAE				(1<<5)

#define FRINDEX				*(hcc_reg32 *)(EHCI_BASE+0x4c)
  #define FI_m				0x3fff
  #define FI_s				0

#define CTRLDSSEGMENT		*(hcc_reg32 *)(EHCI_BASE+0x50)

#define PERIODICLISTBASE	*(hcc_reg32 *)(EHCI_BASE+0x54)

#define ASYNCLISTADDR		*(hcc_reg32 *)(EHCI_BASE+0x58)

#define CONFIGFLAG			*(hcc_reg32 *)(EHCI_BASE+0x80)
  #define CF				(1<<0)

#define PORTSC(N)			*(hcc_reg32 *)(EHCI_BASE+0x80+(N<<2))
  #define LS_m				0x3
  #define LS_s				10

  #define PI_m				0x3
  #define PI_s				14

  #define PTC_m				0xf
  #define PTC_s				16

  #define CCS				(1<<0)
  #define CSC				(1<<1)
  #define PE				(1<<2)
  #define PEC				(1<<3)
  #define OC				(1<<4)
  #define OCC				(1<<5)
  #define FPR				(1<<6)
  #define SUSP				(1<<7)
  #define PR				(1<<8)
  #define PP				(1<<12)
  #define PO				(1<<13)
  #define WCE				(1<<20)
  #define WDE				(1<<21)
  #define WOCE				(1<<22)



/******************** Structures ********************/
#define ST_ACTIVE				(1<<7)
#define ST_HALTED				(1<<6)
#define ST_DATA_BUFFER_ERROR	(1<<5)
#define ST_BABBLE				(1<<4)
#define ST_XACT_ERROR			(1<<3)
#define ST_MISSED_UFRAME		(1<<2)
#define ST_SPLITXSTATE			(1<<1)
#define ST_PERR					(1<<0)


#define _mQTD_T				1u
#define _mQTD_NAKCNT		0xfu
#define _mQTD_STATUS		0xffu
#define _mQTD_PID			0x3u
#define _mQTD_CERR			0x3u
#define _mQTD_CPAGE			0x7u
#define _mQTD_IOC			1u
#define _mQTD_TB			0x7fffu
#define _mQTD_DT			1u
#define _mQTD_CPROGMASK		0xffu
#define _mQTD_FRAMETAG		0x1fu
#define _mQTD_SBYTES		0x7fu

#define _sQTD_T				0
#define _sQTD_NAKCNT		1
#define _sQTD_STATUS		0
#define _sQTD_PID			8
#define _sQTD_CERR			10
#define _sQTD_CPAGE			12
#define _sQTD_IOC			15
#define _sQTD_TB			16
#define _sQTD_DT			31
#define _sQTD_CPROGMASK		0
#define _sQTD_FRAMETAG		0
#define _sQTD_SBYTES		5

#define _mQTD(f)			(_mQTD_##f<<_sQTD_##f)
#define _vQTD(f,x)			(x<<_sQTD_##f)
#define _rQTD(v,f)			((_EHCI_V32(v)>>_sQTD_##f)&_mQTD_##f)
#define _rQTDv(v,f)			(((v)>>_sQTD_##f)&_mQTD_##f)
#define _wQTD(v,f,x)		v=((v)&~_EHCI_V32(_mQTD(f)))|(_EHCI_V32((x)<<_sQTD_##f))

typedef struct {
  volatile hcc_u32 nqtd;
  volatile hcc_u32 anqtd;
  volatile hcc_u32 ctrl0;
  volatile hcc_u32 bptr[5];
} t_qtd;


#define EPS_FULL			0x0
#define EPS_LOW				0x1
#define EPS_HIGH			0x2


#define _mQH_T				1u
#define _mQH_TYP			0x3u
#define _mQH_NQH			0xffffffe0u
#define _mQH_DA				0x7fu
#define _mQH_I				1u
#define _mQH_ENDPT			0xfu
#define _mQH_EPS			0x3u
#define _mQH_DTC			1u
#define _mQH_H				1u
#define _mQH_MPL			0x7ffu
#define _mQH_C				1u
#define _mQH_RL				0xfu
#define _mQH_UFRAME_S		0xffu
#define _mQH_UFRAME_C		0xffu
#define _mQH_HUBADDR		0x7fu
#define _mQH_PORTNUM		0x7fu
#define _mQH_MULT			0x3u

#define _sQH_T				0
#define _sQH_TYP			1
#define _sQH_NQH			0
#define _sQH_DA				0
#define _sQH_I				7
#define _sQH_ENDPT			8
#define _sQH_EPS			12
#define _sQH_DTC			14
#define _sQH_H				15
#define _sQH_MPL			16
#define _sQH_C				27
#define _sQH_RL				28
#define _sQH_UFRAME_S		0
#define _sQH_UFRAME_C		8
#define _sQH_HUBADDR		16
#define _sQH_PORTNUM		23
#define _sQH_MULT			30

#define _mQH(f)				(_mQH_##f<<_sQH_##f)
#define _vQH(f,x)			(x<<_sQH_##f)
#define _rQH(v,f)			((_EHCI_V32(v)>>_sQH_##f)&_mQH_##f)
#define _wQH(v,f,x)			v=((v)&_EHCI_V32(~_mQH(f)))|(_EHCI_V32((x)<<_sQH_##f))

typedef struct {
  volatile hcc_u32 nqh;
  volatile hcc_u32 ctrl0;
  volatile hcc_u32 ctrl1;
  volatile hcc_u32 cqtd;
  volatile t_qtd qtd;
  hcc_u32 r[4];		/* reserved, align to 32 bytes */
} t_qh;

#ifdef __cplusplus
}
#endif


#endif


