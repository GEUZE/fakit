#ifndef _CFC_IDE_C_
#define _CFC_IDE_C_

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

#include "../thin_usr.h"

#include "cfc_ide.h"

static int gl_changed=0;
static int gl_state=F_ST_MISSING;

/******************************************************************************
 *
 * CFC register addresses
 *
 *****************************************************************************/
#ifdef DRV_8051
#include <reg52.h>
static unsigned char xdata CFC_DATA _at_ (CFC_BASE+0x00);
static unsigned char xdata CFC_FEATURE _at_ (CFC_BASE+0x01);
static unsigned char xdata CFC_SECTORCOU _at_ (CFC_BASE+0x02);
static unsigned char xdata CFC_SECTORNO _at_ (CFC_BASE+0x03);
static unsigned char xdata CFC_CYLINDERLO _at_ (CFC_BASE+0x04);
static unsigned char xdata CFC_CYLINDERHI _at_ (CFC_BASE+0x05);
static unsigned char xdata CFC_SELC _at_ (CFC_BASE+0x06);
static unsigned char xdata CFC_COMMAND _at_ (CFC_BASE+0x7);
#define CFC_STATE CFC_COMMAND
#define CFC_CDPORT P1
#define CFC_PWPORT P1
#define CFC_RSTPORT P1

sfr WCON = 0xa7;
sfr P1IE = 0x9d;
sfr P1F  = 0x9e;
sfr P1LS = 0x9c;
sfr C_IE0= 0xa8;
sfr C_IE1= 0xb1;
#define RTWE 1
#else
#define CFC_DATA	(*(volatile unsigned char*)(CFC_BASE+0x00))
#define CFC_FEATURE	(*(volatile unsigned char*)(CFC_BASE+0x04))
#define CFC_SECTORCOU	(*(volatile unsigned char*)(CFC_BASE+0x08))
#define CFC_SECTORNO	(*(volatile unsigned char*)(CFC_BASE+0x0c))
#define CFC_CYLINDERLO	(*(volatile unsigned char*)(CFC_BASE+0x10))
#define CFC_CYLINDERHI	(*(volatile unsigned char*)(CFC_BASE+0x14))
#define CFC_SELC	(*(volatile unsigned char*)(CFC_BASE+0x18))
#define CFC_COMMAND	(*(volatile unsigned char*)(CFC_BASE+0x1c))
#define CFC_STATE	CFC_COMMAND
#define CFC_CDPORT	(*(volatile unsigned char*)(CFC_BASE+0x40))
#define CFC_PWPORT	(*(volatile unsigned char*)(CFC_BASE+0x44))
#define CFC_RSTPORT	(*(volatile unsigned char*)(CFC_BASE+0x44))
#endif


#ifdef DRV_8051
/******************************************************************************
 * Card detect interrupt handler 
 *****************************************************************************/
void cd_irq (void) interrupt 8 using 2
{
  if (P1F&CFC_CDBIT)
  {
	P1F&=~CFC_CDBIT;	/* clear interrupt */
    gl_changed=1;		/* set changed bit */
    if (CFC_CDPORT&CFC_CDBIT) CFC_PWPORT|=CFC_PWBIT;	/* switch off power */
	P1IE&=~CFC_CDBIT;	/* switch off CD interrupt */
  }
}
#endif


/******************************************************************************
 *
 * fnCFCwaitBusy
 *
 * wait untile last write  operation is finished
 *
 * RETURNS
 * 0 - ok
 * 1 - any error
 *
 *****************************************************************************/

static unsigned char fnCFCwaitBusy() {
unsigned long dwto=CFC_TOVALUE;

   while (CFC_STATE & CFC_STATE_BUSY) {
      if (!dwto) return 1;
      dwto--;
   }
   return 0;
}

/****************************************************************************
 *
 * cfc_getstatus
 *
 * get status of flash card, missing or/and removed,changed
 *
 * RETURNS
 *
 * F_ST_xxx code for high level
 *
 ***************************************************************************/

unsigned char cfc_getstatus() {
unsigned char state=0;

   if (CFC_CDPORT&CFC_CDBIT) state|=F_ST_MISSING;
   
   if (gl_state!=state) 
   {
     gl_changed=1;
     gl_state=state;
   }

   if (gl_changed) 
   {
      state|=F_ST_CHANGED;

      if (!(state&F_ST_MISSING)) 
      {
         unsigned long dwto;

#ifdef DRV_8051
         P1LS|=CFC_CDBIT;	/* interrupt is generated at high level (disconnect) */
         P1IE|=CFC_CDBIT;	/* enable CD interrupt */
#endif

         CFC_RSTPORT|=CFC_RSTBIT;
         CFC_PWPORT|=CFC_PWBIT;
         for (dwto=5000;dwto;dwto--);
         CFC_PWPORT&=~CFC_PWBIT;
         for (dwto=5000;dwto;dwto--);
	 CFC_RSTPORT&=~CFC_RSTBIT;

         dwto=CFC_TOVALUE;
         while (CFC_STATE & CFC_STATE_BUSY) 
         {
           if (!dwto) return state; 
           dwto--;
         }

         CFC_FEATURE=0x01;       /*set 8 bit mode*/
         CFC_SECTORCOU=0;
         CFC_SELC=CFC_SELCARD;  
         CFC_COMMAND=CFC_SETFEA;

         if (fnCFCwaitBusy()) {
            return CFC_ERR_BUSY_ATCYL; /*check if last write performed*/
         }

         if (CFC_STATE & (CFC_STATE_ERR|CFC_STATE_DRQ|CFC_STATE_BUSY)) {
            return CFC_ERR_BUSY_ATCYL; /*check if last write performed*/
         }

         gl_changed=0; /*clear only if its in*/
      }
   }

   return state; 
}


/******************************************************************************
 *
 * fnCFCwaitReady
 *
 * wait until next command can be written
 *
 * RETURNS
 * 0 - ok
 * 1 - any error
 *
 *****************************************************************************/

static unsigned char fnCFCwaitReady() {
unsigned long dwto=CFC_TOVALUE;

   while ((CFC_STATE & (CFC_STATE_BUSY|CFC_STATE_RDY))!=CFC_STATE_RDY) {
      if (!dwto) return 1;
      dwto--;
   }
   return 0;
}

/******************************************************************************
 *
 * fnCFCwaitDrq
 *
 * wait for data request
 *
 * RETURNS
 * 0 - ok
 * 1 - any error
 *
 *****************************************************************************/

static unsigned char fnCFCwaitDrq() {
unsigned long dwto=CFC_TOVALUE;

   while ((CFC_STATE & (CFC_STATE_BUSY|CFC_STATE_DRQ))!=CFC_STATE_DRQ) {
      if (!dwto) return 1;
      dwto--;
   }

   return 0;
}

/******************************************************************************
 *
 * fnCFCsetCommand 
 *
 * Write a command into flash card command register, it writes also
 * select drive
 *
 * INPUTS
 * dwcmd - command to write
 *
 * RETURNS
 * 0 - ok
 * 1 - any error
 *
 *****************************************************************************/

static unsigned char fnCFCsetCommand (unsigned char dwcmd) {

   CFC_SELC=CFC_SELCARD;          /*select card*/
   CFC_COMMAND=dwcmd;          /*pass the command*/

   if (fnCFCwaitBusy()) {
      return 1;  /*command is not accepted*/
   }

   if (fnCFCwaitDrq()) {
      return 1;   /*no data requested set*/
   }


   return 0;                       /*ready*/
}

/******************************************************************************
 *
 * fnCFC
 *
 * Write or Read data from flash card
 *
 * INPUTS
 * data   - where to read or where to write to the data
 * length - data length of reading or writing
 * cfaddr - flash card address
 * dwcmd  - CFC_READ-for reading  CFC_WRITE-for writing
 *
 * RETURNS
 * 0 - ok
 * 1 - any error
 *
 *****************************************************************************/
#include <stdio.h>
unsigned char fnCFC(void *_pdata, unsigned long sector, unsigned char dwcmd) {
unsigned char *cdata=(unsigned char*)_pdata;
unsigned short length;
unsigned long cylinder;

   if (cfc_getstatus()&F_ST_MISSING) return CFC_ERR_NOTPLUGGED; /*check if FlashCard is there*/

   if (fnCFCwaitBusy()) {
      return CFC_ERR_BUSY_ATCYL; /*check if last write performed*/
   }

   if (fnCFCwaitReady()) {
      return CFC_ERR_BUSY_ATCYL; /*not ready to accept new command*/
   }

   cylinder=(sector>>8);       /*get the current cylinder (256sector / cylinder)*/
   CFC_CYLINDERLO=(cylinder&0xff);    /*xxxxcccc cccccccs sssssssb bbbbbbbb*/
   CFC_CYLINDERHI=(cylinder>>8); /*xxxxcccc cccccccs sssssssb bbbbbbbb*/
   CFC_SECTORNO=sector;        /*set sector no and cou=1*/
   CFC_SECTORCOU=1;            /*set sector no and cou=1*/
      

   if (fnCFCsetCommand(dwcmd)) {
      return CFC_ERR_BUSY_ATCMD; /*check if command can be started*/
   }

   length=CFC_PAGE_SIZE;
   while (length) {
      if (dwcmd==CFC_WRITE) CFC_DATA=*cdata++;
                       else *cdata++=CFC_DATA; 
      length--;
   }

   if (fnCFCwaitBusy()) {
      return CFC_ERR_BUSY_ATCMD; 
   }

   if (CFC_STATE & CFC_STATE_ERR) {
      return CFC_ERR_STATE;
   }

   if (dwcmd!=CFC_ID)
   {
     if (CFC_SECTORCOU) {
        return CFC_ERR_SECCOU;
     }
   } 

   if (cfc_getstatus()&F_ST_MISSING) return CFC_ERR_NOTPLUGGED; /*check if FlashCard is there*/

   return CFC_NO_ERROR;
}


#if F_FORMATTING
/****************************************************************************
 *
 * get16
 *
 * get a 16bit value always in little endian
 *
 * INPUTS
 *
 * s - data pointer where the data is
 *
 * RETURNS
 *
 * 16bit number
 *
 ***************************************************************************/

static unsigned short get16(unsigned char *s) {
unsigned short ret;
   ret=s[1];
   ret<<=8;
   ret|=s[0];
   return ret;
}

/****************************************************************************
 *
 * get32
 *
 * get a 32bit value always in little endian
 *
 * INPUTS
 *
 * s - data pointer where the data is
 *
 * RETURNS
 *
 * 32bit number
 *
 ***************************************************************************/

static unsigned long get32(unsigned char *s) {
unsigned long ret;
   ret=get16(s);
   ret<<=16;
   ret|=get16(s+2);
   return ret;
}

/****************************************************************************
 *
 * cfc_getphy
 *
 * determinate flash card physicals
 *
 * INPUTS
 *
 * phy - this structure has to be filled with physical information
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
unsigned char cfc_getphy(F_PHY *phy) {
unsigned char gl_data[512]; /*this variable need for determination*/
unsigned char ret=fnCFC(gl_data,0,CFC_ID);
   if (ret) return ret;

   phy->number_of_cylinders=get16(gl_data+2);
   phy->sector_per_track=get16(gl_data+6);
   phy->number_of_heads=get16(gl_data+0x0c);
   phy->number_of_sectors=get32(gl_data+0x0e);

   return CFC_NO_ERROR;
}
#endif


/****************************************************************************
 *
 * cfc_initfunc
 *
 * this init function has to be passed for highlevel to initiate the
 * driver functions
 *
 * INPUTS
 *
 * func - structure, where to fill functions
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
unsigned char cfc_initfunc(void)
{
#ifdef DRV_8051
  WCON|=RTWE;
  C_IE1|=1;			/* enable P1 interrupts */
  C_IE0|=0x80;		/* enable global interrupts */
#endif
  return 0;
}


/******************************************************************************
 *
 *  End of cfc_ide.c
 *
 *****************************************************************************/

#endif /*_CFC_IDE_C_*/

