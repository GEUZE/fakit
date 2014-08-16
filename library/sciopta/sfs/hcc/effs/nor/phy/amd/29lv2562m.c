#ifndef _29LV2562M_C_
#define _29LV2562M_C_

/****************************************************************************
 *
 *            Copyright (c) 2005-2008 by HCC Embedded
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

#include  "29lv2562m.h"

/****************************************************************************
 *
 *  29lv2562m AMD
 *
 *  some defintions
 *
 ***************************************************************************/

/****************************************************************************
 *
 * HCC_HW defines used only for HCC Embedded's harware
 *
 ***************************************************************************/

#define HCC_HW

/****************************************************************************
 *
 * Please comment the line above out if another hw is used
 *
 ***************************************************************************/

#ifdef HCC_HW
#define CS0CSAR (*(volatile unsigned long*)0x10000040) /* chipselect address register */
#define CS0CSOR (*(volatile unsigned long*)0x10000044) /* chipselect option register */

#define ADDR_MASK  0xfffff000                /* addr mask in chipselect register */

#define WP_MASK 0x11
#define WP_ON   0x01                /* write protection mask */

#define VALID_MASK 0x00000001                /* chipselect valid bit */

#define FS_FLASHBASE (CS0CSAR & ADDR_MASK)   /* flash base address */
#endif

static volatile unsigned short *g_pwflashbase;        /* flash physical address stored here */

/*  512x128K  */
/*  */
/*  The following is the physical/logical map  */
/* 	Size	logical number	relative physical address	usage	 */
/* 	128k	0		0				file storage		 */
/* 	128k	1		0x0020000			file storage	 */
/* 	128k	2		0x0040000			file storage	 */
/* 	..	.		........			file storage	 */
/* 	128k	509		0x3a00000			file storage	 */
/* 	128k	510		0x3c00000			descriptor	 */
/* 	128k	511		0x3e00000			descriptor	 */


#define BLOCKSIZE	0x20000			/*  128k - size of file storage blocks  */
#define BLOCKSTART	0			/*  logical number of first file storage block  */
#define MAXBLOCKS	510			/*  number of logical blocks for file storage  */

#define DESCSIZE	0x20000			/*  descriptor block size is 128K  */
#define DESCBLOCKSTART	510			/*  logical number of first descriptor block  */
#define DESCBLOCKS	2			/*  number of descriptor blocks  */
#define DESCCACHE	0x08000			/*  32k write cache in descriptor  */


#define SECTORSIZE	8192			/*  8k - sector size  */
#define SECTORPERBLOCK	(BLOCKSIZE/SECTORSIZE)	/*  number of sectors per block  */


/* Please check maximum storeable file and used sector number in FSmem.exe */

/****************************************************************************
 *
 * fnWriteWord
 *
 * external function which write a halfword into flash
 *
 * INPUTS
 *
 * base - flash base address
 * addr - flash relative address
 * data - what to write
 *
 ***************************************************************************/

/* extern void fnWriteWord(volatile unsigned short* base,long addr,unsigned short data); */
static void fnWriteWord(volatile unsigned short* base,long addr,unsigned short data)
{
  *(volatile unsigned short *)((char *)base+addr)=data;
}

#define _xAA 0xAA
#define _x55 0x55
#define _x90 0x90

#define _xF0 0xF0
#define _xA0 0xA0
#define _x80 0x80
#define _x30 0x30

#define _x04   0x04
#define _x01   0x01
#define _x225b 0x225b

#define _x0020 0x0020

#define _x227e 0x227e


/****************************************************************************
 *
 * RemoveWriteProtect
 *
 * Remove write protection on chip select 0,
 * e.g. when flash is accessed by write
 *
 ***************************************************************************/
static void RemoveWriteProtect() {
#ifdef HCC_HW
    CS0CSOR&=~WP_MASK;
#endif
}

/****************************************************************************
 *
 * SetWriteProtect
 *
 * Set write protection on chip select 0 for prevention flash from
 * more write accessing
 *
 ***************************************************************************/
static void SetWriteProtect() {
#ifdef HCC_HW
    unsigned long tmp;
    tmp=CS0CSOR;
    tmp&=~WP_MASK;
    tmp|=WP_ON;
    CS0CSOR=tmp;
#endif
}

/****************************************************************************
 *
 * GetBlockAddr
 *
 * Get a logical block physical relative address in flash
 * relsector - relative sector in the block (<sectorperblock)
 *
 * INPUTS
 *
 * block - logical block number
 * relsector - rel
 *
 * RETURNS
 *
 * relative physical address of block
 *
 ***************************************************************************/
static long GetBlockAddr(long block,long relsector)
{
    return (BLOCKSIZE*block)+(relsector*SECTORSIZE); /* all are 128k */

}

/****************************************************************************
 *
 * WriteCmd
 *
 * Send command Sequence to FLASH
 *   fnWriteWord(0x555<<1,_xAA);
 *   fnWriteWord(0x2AA<<1,_x55);
 *   fnWriteWord(0x555<<1,mit);
 *
 * INPUTS
 *
 * mit - command byte
 *
 ***************************************************************************/
static void WriteCmd(unsigned short mit) {
  fnWriteWord(g_pwflashbase,0x555<<1,_xAA);
  fnWriteWord(g_pwflashbase,0x2AA<<1,_x55);
  fnWriteWord(g_pwflashbase,0x555<<1,mit);
}

/****************************************************************************
 *
 * DataPoll
 *
 * Polling data during programming compares address with chk data
 *
 * INPUTS
 *
 * addr - address to be checked
 * chk  - data what should be there on that address
 *
 * RETURNS
 *
 * 0 - if ok
 * 1 - if different
 * 2 - Timeout error
 *
 ***************************************************************************/
static long DataPoll(long addr, unsigned short chk) {
unsigned short t;
long st=0;

again:
      if (st++==0x1000000L) return 2;    /* enough for 16 sector, all added */

      t=g_pwflashbase[addr>>1];
      if (t==chk) return 0;

   if ((t&_x0020)!=_x0020) goto again;

   t=g_pwflashbase[addr>>1];
   if (t==chk) return 0;

   return 1;
}

/****************************************************************************
 *
 * EraseFlash
 *
 * Calculate and erase sector which are should be cleared in that range
 * for 29LV640D Flash only 64K blocks so only erasable on 64K boundary
 *
 *
 * INPUTS
 *
 * block - physical address of block needs to be erased
 *
 * RETURNS
 *
 * 0 - if ok
 * 1 - if there was any error with prompt
 *
 ***************************************************************************/
static int EraseFlash(long block) {
long addr=GetBlockAddr(block,0);
long ret;

/*  all blocks assigned are physically erasable for this configuration!!  */

    g_pwflashbase = (unsigned short *)FS_FLASHBASE; /* get flash base addr */

    RemoveWriteProtect();

    WriteCmd(_x80);
    fnWriteWord(g_pwflashbase,0x555<<1,_xAA);
    fnWriteWord(g_pwflashbase,0x2AA<<1,_x55);

    fnWriteWord(g_pwflashbase,addr,_x30); /* this erase sector */

    SetWriteProtect();

    ret=DataPoll(addr,0x0ffff);

    return (int)ret;
}

/****************************************************************************
 *
 * WriteFlash
 *
 * Writing (programming) Flash device
 *
 * INPUTS
 *
 * data - where data is
 * block - which block is programmed
 * relsector - relative sector in the block (<sectorperblock)
 * size - length of data
 * relpos - relative position in block of data
 *
 * RETURNS
 *
 * 0 - if ok
 * 1 - if there was any error
 *
 ***************************************************************************/
static int WriteFlash(void *data, long block, long relsector, long size,long relpos) {
long dest=GetBlockAddr(block,relsector)+relpos;
unsigned short *s=(unsigned short*)(data);
long a;
int ret=0;

   size++;

   size>>=1;
   size<<=1; /* word align */

   g_pwflashbase = (unsigned short *)FS_FLASHBASE; /* get flash base addr */

   RemoveWriteProtect();

   for (a=0; a<size; a+=2) {
      WriteCmd(_xA0);
      fnWriteWord(g_pwflashbase,dest,*s);

      if (DataPoll(dest,*s)==2) {
        ret=1;
        break;
      }
      dest+=2;  /* in byte */
      s++;
   }

   WriteCmd(_xF0);

   SetWriteProtect();

   return ret;
}

/****************************************************************************
 *
 * VerifyFlash
 *
 * Compares data with flash containes
 *
 * INPUTS
 *
 * data - where data is
 * block - which block is programmed
 * relsector - relative sector in the block (<sectorperblock)
 * size - length of data
 * relpos - relative position of data in block
 *
 * RETURNS
 *
 * 0 - if ok
 * 1 - if there was any error
 *
 ***************************************************************************/
static int VerifyFlash(void *data, long block, long relsector,long size,long relpos) {
long addr=FS_FLASHBASE; /* get the flash base address */
unsigned short *d=(unsigned short*)(addr+GetBlockAddr(block,relsector)+relpos);
unsigned short *s=(unsigned short*)(data);
long a;

   size++;

   size>>=1;
   size<<=1; /* word align */

   for (a=0; a<size; a+=2) {                 /* verify */
     if (*d++!=*s++) {
        return 1;  /* failed */
     }
   }
   return 0; /* ok */
}


/****************************************************************************
 *
 * ReadFlash
 *
 * read data from flash
 *
 * INPUTS
 *
 * data - where to store data
 * block - block number which block to be read
 * blockrel - relative start address in the block
 * datalen - length of data in bytes
 *
 * RETURNS
 * 0 - if sucessfully read
 * other - if any error
 *
 ***************************************************************************/
static int ReadFlash(void *data, long block, long blockrel, long datalen) {
char *addr = (char *)FS_FLASHBASE; /* get flash base addr */

   addr+=GetBlockAddr(block,0)+blockrel;

   fsm_memcpy(data,addr,datalen);

   return 0;
}


/****************************************************************************
 *
 * fs_phy_nor_29lv2562m
 *
 * Identify a flash and fills FS_FLASH structure with data
 *
 * INPUTS
 *
 * flash - structure which is filled with data of flash properties
 *
 * RETURNS
 *
 * 0 - if successfully
 * other if flash cannot be identified
 *
 ***************************************************************************/
int fs_phy_nor_29lv2562m(FS_FLASH *flash) {
unsigned char man_ID;
unsigned char dev_ID;
unsigned char dev_ID1=0;
unsigned char dev_ID2=0;

   flash->ReadFlash=ReadFlash;       /* read content */
   flash->EraseFlash=EraseFlash;     /* erase a block */
   flash->WriteFlash=WriteFlash;     /* write content */
   flash->VerifyFlash=VerifyFlash;   /* verify content */

   g_pwflashbase = (unsigned short*)FS_FLASHBASE; /* get flash base addr */

   RemoveWriteProtect();

   WriteCmd(_xF0);           /* Exit this mode, reset state machine in the flash */

   WriteCmd(_x90);           /* ManID manufacture ID */
   man_ID=g_pwflashbase[0x00<<1];

   WriteCmd(_x90);           /* DevID Device ID */
   dev_ID=g_pwflashbase[0x01<<1];

   if(dev_ID==0x7e)
   {
       dev_ID1=g_pwflashbase[0x0e<<1];
       dev_ID2=g_pwflashbase[0x0f<<1];
   }


   WriteCmd(_xF0);           /* Exit this mode */

   SetWriteProtect();

   if (man_ID==0x01 && dev_ID==0x7e && dev_ID1==0x12 && dev_ID2==0x01)
   {
	flash->maxblock=MAXBLOCKS;
	flash->blocksize=BLOCKSIZE;
	flash->sectorsize=SECTORSIZE;
	flash->sectorperblock=SECTORPERBLOCK;
	flash->blockstart=BLOCKSTART;
	flash->descsize=DESCSIZE;
	flash->descblockstart=DESCBLOCKSTART;
	flash->descblockend=DESCBLOCKSTART+DESCBLOCKS-1; /*  num of last desc block  */
	flash->cacheddescsize=DESCCACHE;		/*  size of write cache in descriptor  */
	return 0;
   }
   return 1;   /* unidentified or wrong device   */
}

/****************************************************************************
 *
 *  end of 29lv2562m.c
 *
 ***************************************************************************/

#endif /* _29LV2562M_C_ */
