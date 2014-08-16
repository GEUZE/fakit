#ifndef _29LV_8BIT_C_
#define _29LV_8BIT_C_

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

#include  "29lv_8bit.h"

/****************************************************************************
 *
 *  29lv_8bit AMD
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
#define CS0CSAR (*(volatile long*)0xffc00010) /* chipselect address register */

#define ADDR_MASK  0xfffff000                /* addr mask in chipselect register */
#define WP_MASK    0x00000002                /* write protection mask */
#define VALID_MASK 0x00000001                /* chipselect valid bit */
#endif

static volatile unsigned char *g_pwflashbase;        /* flash physical address stored here */

#ifdef HCC_HW
#define FS_FLASHBASE 0xff000000   /* flash base address */
#endif

/*  physical  */
/*  */
/*  8x8K blocks at the beginning  */
/*  126x64k blocks  */
/*  8x8k blocks at the end  */
/*  */

#define BLOCKSIZE	0x10000		/*  64k - size of file storage blocks  */
#define BLOCKSTART	8		/*  logical number of first file storage block  */
#define MAXBLOCKS	126		/*  number of logical blocks for file storage  */

#define DESCSIZE	8192		/*  8k descriptor block size  */
#define DESCBLOCKSTART	0		/*  logical number of first descriptor block  */
#define DESCBLOCKS	8		/*  number of descriptor blocks  */
#define	DESCCACHE	2048		/*  2K assigned to write cache for descriptor  */

#define SECTORSIZE 	0x1000		/* 4k */
#define SECTORPERBLOCK (BLOCKSIZE/SECTORSIZE)


/* Please check maximum storeable file and used sector number in FSmem.exe */

/****************************************************************************
 *
 * Flash commands
 *
 ***************************************************************************/

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

#define _x2249 0x2249
#define _x22F9 0x22F9

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
    CS0CSAR |= VALID_MASK;    /* set valid bit */
    CS0CSAR &=~WP_MASK;       /* remove write protection */
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
    CS0CSAR|=WP_MASK;         /* reset write protection back */
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
 * block - block number
 * relsector - rel
 *
 * RETURNS
 *
 * relative physical address of block
 *
 ***************************************************************************/

static long GetBlockAddr(long block,long relsector) {
    if (block<8) return (8192*block);  /*  8x8k  */
    else if (block<8+126) return (block-8)*BLOCKSIZE+BLOCKSIZE+relsector*SECTORSIZE; /* 126x64k */
    else return (BLOCKSIZE+126*BLOCKSIZE+8192*(block-(8+126))); /* 8x8k at the end */
}

/****************************************************************************
 *
 * WriteCmd
 *
 * Send command Sequence to FLASH
 *
 * INPUTS
 *
 * cmd - command byte
 *
 ***************************************************************************/

static void WriteCmd(unsigned char cmd) {
  g_pwflashbase[0x555<<1]=_xAA;
  g_pwflashbase[0x2AA<<1]=_x55;
  g_pwflashbase[0x555<<1]= cmd;
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

static long DataPoll(long addr, unsigned char chk) {
unsigned char t;
long st=0;

again:
      if (st++==0x1000000L) return 2;    /* enough for 16 sector, all added */

      t=g_pwflashbase[addr];
      if (t==chk) return 0;

   if ((t& 0x20)!= 0x20) goto again;

   t=g_pwflashbase[addr];
   if (t==chk) return 0;

   return 1;
}

/****************************************************************************
 *
 * EraseFlash
 *
 * Erasing a complete block
 *
 * INPUTS
 *
 * block - which block needs to be erased
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

    g_pwflashbase = (unsigned char *)FS_FLASHBASE; /* get flash base addr */

    RemoveWriteProtect();

    WriteCmd(_x80);
    g_pwflashbase[0x555<<1]=_xAA;
    g_pwflashbase[0x2AA<<1]=_x55;

    g_pwflashbase[addr]=_x30; /* this erase sector */

    SetWriteProtect();

    ret=DataPoll(addr,(unsigned char)0x0ff);

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
 * relpos - relative position of data in block
 *
 * RETURNS
 *
 * 0 - if ok
 * 1 - if there was any error
 *
 ***************************************************************************/

static int WriteFlash(void *data, long block, long relsector, long size,long relpos) {
long dest=GetBlockAddr(block,relsector)+relpos;
unsigned char *s=(unsigned char*)(data);
long a;
int ret=0;

   g_pwflashbase = (unsigned char*)FS_FLASHBASE; /* get flash base addr */

   RemoveWriteProtect();

   for (a=0; a<size; a++) {
      WriteCmd(_xA0);
      g_pwflashbase[dest]=*s;

      if (DataPoll(dest,*s)==2) {
        ret=1;
        break;
      }
      dest++;  /* in byte */
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
unsigned long addr=FS_FLASHBASE; /* get the flash base address */
unsigned char *d=(unsigned char*)(addr+GetBlockAddr(block,relsector)+relpos);
unsigned char *s=(unsigned char*)(data);
long a;

   for (a=0; a<size; a++) {                 /* verify */
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
 * fs_phy_nor_29lv_8bit
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

int fs_phy_nor_29lv_8bit(FS_FLASH *flash) {
unsigned char man_ID;
unsigned char dev_ID;
unsigned char dev_ID1=0;
unsigned char dev_ID2=0;

   flash->ReadFlash=ReadFlash;       /* read content */
   flash->EraseFlash=EraseFlash;     /* erase a block */
   flash->WriteFlash=WriteFlash;     /* write content */
   flash->VerifyFlash=VerifyFlash;   /* verify content */

   g_pwflashbase = (unsigned char*)FS_FLASHBASE; /* get flash base addr */

   RemoveWriteProtect();

   WriteCmd(_xF0);           /* Exit this mode, reset state machine in the flash */

   WriteCmd(_x90);           /* ManID manufacture ID */
   man_ID=g_pwflashbase[0x00<<1];

   WriteCmd(_x90);           /* DevID Device ID */
   dev_ID=g_pwflashbase[0x01<<1];

   if (dev_ID==0x7e) {
       dev_ID1=g_pwflashbase[0x0e<<1];
       dev_ID2=g_pwflashbase[0x0f<<1];
   }

   WriteCmd(_xF0);           /* Exit this mode */

   SetWriteProtect();

   if (man_ID==0x01 && dev_ID==0x7e && dev_ID1==0x02 && dev_ID2==0x01) {

		flash->maxblock=MAXBLOCKS;
		flash->blocksize=BLOCKSIZE;
		flash->sectorsize=SECTORSIZE;
		flash->sectorperblock=SECTORPERBLOCK;
		flash->blockstart=BLOCKSTART;		/* where 1st block starts */
		flash->descsize=DESCSIZE;	   	/* 8K 	 */
		flash->descblockstart=DESCBLOCKSTART;
		flash->descblockend=DESCBLOCKSTART+DESCBLOCKS-1; /*  num of last desc block  */
		flash->cacheddescsize=DESCCACHE;		/*  size of write cache in descriptor  */

		return 0; /*  AM29DL64G  8M  */
   }



   return 1;   /* unidentified or wrong device   */
}

/****************************************************************************
 *
 *  end of 29lv-8bit.c
 *
 ***************************************************************************/

#endif /* _29LV_8BIT_C_ */
