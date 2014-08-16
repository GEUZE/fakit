/*SOC
**********************************************************************
**  _______  _______ _________ _______  _______ _________ _______   **
** (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
** | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
** | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
** (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
**       ) || |         | |   | |   | || (         | |   | (   ) |  **
** /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
** \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
**                                                                  **
**              (c) 2008 SCIOPTA Systems AG/ Schweiz                **
**                                                                  **
**********************************************************************
** ID: S09054BS1                                                    **
** +Revision: 1.1.2.1 +                                                 **
** +Date: 2009/02/23 08:56:18 +                                     **
** 29GL128M driver                                                  **
**********************************************************************
EOC*/

#ifndef _29GL128M_C_
#define _29GL128M_C_

#include  "29gl128m.h"

/****************************************************************************
 *
 *  29lg128p spansion
 *
 *  some defintions
 *
 ***************************************************************************/

static volatile unsigned short *g_pwflashbase;    /* flash physical address stored here */


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
STATIC_INLINE void fnWriteWord(volatile unsigned short* base,long addr,unsigned short data)
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
 * DisableCache
 *
 * Remove write protection on chip select 0,
 * e.g. when flash is accessed by write
 *
 ***************************************************************************/
static void DisableCache() 
{
  __asm__ __volatile__ (" move.l acr1_mirror,%d0\n"
			" and.l #~0x8000,%d0\n"
			" movec %d0,%acr1\n");
}

/****************************************************************************
 *
 * EnableCache
 *
 * Set write protection on chip select 0 for prevention flash from
 * more write accessing
 *
 ***************************************************************************/
static void EnableCache() 
{
  __asm__ __volatile__ (" move.l acr1_mirror,%d0\n"
			" movec %d0,%acr1\n");

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
STATIC_INLINE void WriteCmd(unsigned short mit) {
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
static long DataPoll(long addr, unsigned short chk) 
{
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
static int EraseFlash(long block) 
{
  long addr=GetBlockAddr(block,0);
  long ret;
  
/*  all blocks assigned are physically erasable for this configuration!!  */

//  g_pwflashbase = (unsigned short *)FS_FLASHBASE; /* get flash base addr */
  
  DisableCache();
  
  WriteCmd(_x80);
  fnWriteWord(g_pwflashbase,0x555<<1,_xAA);
  fnWriteWord(g_pwflashbase,0x2AA<<1,_x55);
  
  fnWriteWord(g_pwflashbase,addr,_x30); /* this erase sector */
    
  ret=DataPoll(addr,0x0ffff);
  
  EnableCache();

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
static int WriteFlash(void *data, long block, long relsector, long size,long relpos) 
{
  long dest = GetBlockAddr(block,relsector)+relpos;
  unsigned short *s = (unsigned short*)(data);
  long a;
  int ret = 0;
  
  size++;
  
  size>>=1;
  size<<=1; /* word align */
  
//  g_pwflashbase = (unsigned short *)FS_FLASHBASE; /* get flash base addr */
  
  DisableCache();
  
  for (a = 0; a < size; a += 2) {
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
  
  EnableCache();
  
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
static int VerifyFlash(void *data, long block, long relsector,long size,long relpos) 
{
  long addr = (long)g_pwflashbase; /* get the flash base address */
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
static int ReadFlash(void *data, long block, long blockrel, long datalen) 
{
  char *addr = (char *)g_pwflashbase; /* get flash base addr */
  
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
int fs_phy_nor_29gl128m(FS_FLASH *flash) 
{
  extern unsigned short FS_FLASHBASE[];
  unsigned short man_ID;
  unsigned short dev_ID;
  unsigned short dev_ID1=0;
  unsigned short dev_ID2=0;
  
  flash->ReadFlash=ReadFlash;       /* read content */
  flash->EraseFlash=EraseFlash;     /* erase a block */
  flash->WriteFlash=WriteFlash;     /* write content */
  flash->VerifyFlash=VerifyFlash;   /* verify content */
  
  g_pwflashbase = FS_FLASHBASE; /* get flash base addr */
  
  DisableCache();
  
  WriteCmd(_xF0);           /* Exit this mode, reset state machine in the flash */
  
  WriteCmd(_x90);           /* ManID manufacture ID */
  man_ID=g_pwflashbase[0x00];
  
  WriteCmd(_x90);           /* DevID Device ID */
  dev_ID=g_pwflashbase[0x01];
  
  if(dev_ID==0x227e)
  {
    dev_ID1=g_pwflashbase[0x0e];
    dev_ID2=g_pwflashbase[0x0f];
  }
    
  WriteCmd(_xF0);           /* Exit this mode */
  
  EnableCache();
  
  if ((man_ID & 0xff)==0x01 && 
      dev_ID==0x227e && 
      dev_ID1==0x2212 /* LG128 */ && 
      dev_ID2==0x2200)
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

#endif /* _29GL128P_C_ */
