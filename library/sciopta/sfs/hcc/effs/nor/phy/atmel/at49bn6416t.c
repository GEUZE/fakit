#ifndef _AT49BN6416T_C_
#define _AT49BN6416T_C_

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

#include  "at49bn6416t.h"

/****************************************************************************
 *
 *  AT49BN6416T
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

#define FS_FLASHBASE (CS0CSAR & ADDR_MASK)   /* flash base address */
#endif

static volatile unsigned short *g_pwflashbase;        /* flash physical address stored here */

/*  system config  */
/*  block 0	 - 64k reserved for booting  */
/*  block 1-126 	 - 126x64k blocks used for data storage  */
/*  block 127-134 - 8x8k blocks for descriptors  */


#define BLOCKSIZE	0x10000		/* 64k - size of file storage blocks */
#define BLOCKSTART	1		/*  logical number of first file storage block  */
#define MAXBLOCKS	126		/*  number of logical blocks for file storage  */

#define DESCSIZE	8192		/*  8K descriptor blocks  */
#define DESCBLOCKSTART	127		/*  logical number of first descriptor block  */
#define DESCBLOCKS	8		/*  8x8k number of descriptor blocks  */
#define DESCCACHE	0		/*  descriptor cache size  */


#define SECTORSIZE	0x01000			/* 4k - sector size */
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

extern void fnWriteWord(volatile unsigned short* base,long addr,unsigned short data);

/****************************************************************************
 *
 * Commands and datas for flash
 *
 ***************************************************************************/


#define _xAA 0xAA
#define _x55 0x55
#define _x90 0x90

#define _xF0 0xF0
#define _xA0 0xA0
#define _x80 0x80
#define _x30 0x30

#define _x001f 0x001f
#define _x00d2 0x00d2
#define _x0020 0x0020

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
 *
 * INPUTS
 *
 * block - block number
 * relsector - relative sector in the block (<sectorperblock)
 *
 * RETURNS
 *
 * relative physical address of block
 *
 ***************************************************************************/

static long GetBlockAddr(long block,long relsector) {
  if (block<127) return block*BLOCKSIZE+relsector*SECTORSIZE;  /* 0-126 64K */
  else  return 127*BLOCKSIZE+(block-127)*8192; /* 127-135 8K blocks */
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
 * Erase a block in flash
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
 * len - length of data
 * relpos - offset to data in block
 *
 * RETURNS
 *
 * 0 - if ok
 * 1 - if there was any error
 *
 ***************************************************************************/

static int WriteFlash(void *data, long block, long relsector, long len,long relpos) {
long dest=GetBlockAddr(block,relsector)+relpos;
unsigned short *s=(unsigned short*)(data);
long a;
int ret=0;

   len++;

   len>>=1;
   len<<=1; /* word align */

   g_pwflashbase = (unsigned short *)FS_FLASHBASE; /* get flash base addr */

   RemoveWriteProtect();

   for (a=0; a<len; a+=2) {
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
 * len - length of data
 * relpos - offset to data in block
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
 * fs_phy_nor_at49bn6416t
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

int fs_phy_nor_at49bn6416t(FS_FLASH *flash) {
unsigned short man_ID;
unsigned short dev_ID;

    flash->ReadFlash=ReadFlash;       /* read content */
    flash->EraseFlash=EraseFlash;     /* erase a block */
    flash->WriteFlash=WriteFlash;     /* write content */
    flash->VerifyFlash=VerifyFlash;   /* verify content */

    g_pwflashbase = (unsigned short *)FS_FLASHBASE; /* get flash base addr */

    RemoveWriteProtect();


    WriteCmd(_xF0);           /* Exit this mode, reset state machine in the flash */

    WriteCmd(_x90);           /* ManID manufacture ID */
    man_ID=g_pwflashbase[0x00];

    WriteCmd(_x90);           /* DevID Device ID */
    dev_ID=g_pwflashbase[0x01];

    WriteCmd(_xF0);           /* Exit this mode */


    SetWriteProtect();


    if (man_ID==_x001f && dev_ID==_x00d2) {
		flash->maxblock=MAXBLOCKS;	/* 126x64k filesystem, (1block reserved for boot!) */
		flash->blocksize=BLOCKSIZE;	/* 64k	 */
		flash->sectorsize=SECTORSIZE; 	/* 4k */
	        flash->sectorperblock=SECTORPERBLOCK;
		flash->blockstart=BLOCKSTART;		   /* where 1st block starts, (1block reserved for boot) */
		flash->descsize=DESCSIZE;	   /* 8K 	 */
		flash->descblockstart=DESCBLOCKSTART; 		/* fat block start */
		flash->descblockend=DESCBLOCKSTART+DESCBLOCKS-1;   /* fat block end (8 FATS!!!) */
		flash->cacheddescsize=DESCCACHE;			/*  set the descriptor cache size	  */

		return 0; /* AT48BN6416 */
	 }

    return 1;   /* unidentified or wrong device   */
}

/****************************************************************************
 *
 *  end of AT49BN6416T.c
 *
 ***************************************************************************/

#endif /* _AT49BN6416T_C_ */
