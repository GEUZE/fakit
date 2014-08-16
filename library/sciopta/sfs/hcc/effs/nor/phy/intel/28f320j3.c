#ifndef _28F320J3_C_
#define _28F320J3_C_

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

#include  "28F320J3.h"

/****************************************************************************
 *
 *  28F320J3 Intel
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
#define CS0CSAR (*(volatile unsigned long*)0xffc00050) /* chipselect address register */
#define CS0CSOR (*(volatile unsigned long*)0xffc00054) /* chipselect option register */

#define ADDR_MASK  0xfffff000                /* addr mask in chipselect register */
#define WP_MASK    0x00000002                /* write protection mask */
#define VALID_MASK 0x00000001                /* chipselect valid bit */
#endif


#ifdef HCC_HW
#define FS_FLASHBASE 0x1000000UL   /* flash base address */
#define FS_FLASHDEV(addr) (*(volatile unsigned short *)(FS_FLASHBASE+(unsigned long)(addr)))
#endif

/*  physical  */
/*  32x128k blocks  */

#define BLOCKSIZE	0x20000		/*  128k - size of file storage blocks  */
#define BLOCKSTART 2		      /*  logical number of first file storage block  */
#define MAXBLOCKS	 30		   /*  number of logical blocks for file storage  */

#define DESCSIZE	0x20000		/*  128k descriptor block size  */
#define DESCBLOCKSTART	0		/*  logical number of first descriptor block  */
#define DESCBLOCKS	   2		/*  number of descriptor blocks  */
#define DESCCACHE	0x18000     /*  96K assigned to write cache for descriptor  */

#define SECTORSIZE 	0x1000		/* 4k */
#define SECTORPERBLOCK (BLOCKSIZE/SECTORSIZE)


/* Please check maximum storeable file and used sector number in FSmem.exe */

/****************************************************************************
 *
 * RemoveWriteProtect
 *
 * Remove write protection on chip select 0,
 * e.g. when flash is accessed by write
 *
 ***************************************************************************/

static void RemoveWriteProtect() 
{
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

static void SetWriteProtect() 
{
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

static long GetBlockAddr(long block,long relsector) 
{
   return (block*BLOCKSIZE+relsector*SECTORSIZE);
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

static int DataPoll(long addr, unsigned short chk) 
{
	unsigned short status;
	long st=0;

	while (1) 
	{
		FS_FLASHDEV(0)=0x70;
		status=FS_FLASHDEV(0);
		if (status & 0x80) break;

		if (st++==0x1000000L) 
		{
			FS_FLASHDEV(0)=0xff;
			return 2;
		}
	}

	FS_FLASHDEV(0)=0xff;

	if (FS_FLASHDEV(addr)==chk) return 0;

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

static int EraseFlash(long block) 
{
	long addr=GetBlockAddr(block,0);
	int ret;

    RemoveWriteProtect();

    FS_FLASHDEV(addr)=0x20;
    FS_FLASHDEV(addr)=0xd0;

    ret=DataPoll(addr,(unsigned short)0x0ffff);

    SetWriteProtect();

    return ret;
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

static int WriteFlash(void *data, long block, long relsector, long size,long relpos) 
{
	long dest=GetBlockAddr(block,relsector)+relpos;
	unsigned short *s=(unsigned short*)(data);
	unsigned short status;
	int ret=0;
	long a;

	RemoveWriteProtect();

	while (size>0) 
	{
		if (size>=32) 
		{
			long st=0;
again:
			FS_FLASHDEV(dest)=0xE8;
			FS_FLASHDEV(0)=0x70;
			status=FS_FLASHDEV(0);

			if (!(status & 0x80)) goto again;

			FS_FLASHDEV(dest)=0x0f; /* word count */

			for (a=0; a<32; a+=2) 
			{
				FS_FLASHDEV(dest+a)=*s;
				s++;
			}

			FS_FLASHDEV(dest)=0xD0; /* confirm */

			while (1) 
			{
				FS_FLASHDEV(0)=0x70;
				status=FS_FLASHDEV(0);
				if (status & 0x80) break;

				if (st++==0x1000000L) 
				{
					FS_FLASHDEV(0)=0xff;
					return 2;
				}
			}

			FS_FLASHDEV(0)=0xff;

			size-=32;
			dest+=32;
		}
		else 
		{
			FS_FLASHDEV(dest)=0x40;
			FS_FLASHDEV(dest)=*s;

			if (DataPoll(dest,*s)) 
			{
				ret=1;
				break;
			}
			dest+=2;
			s++;
			size-=2;
		}
	}

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

static int VerifyFlash(void *data, long block, long relsector,long size,long relpos) 
{
	unsigned long addr=FS_FLASHBASE; /* get the flash base address */
	unsigned short *d=(unsigned short*)(addr+GetBlockAddr(block,relsector)+relpos);
	unsigned short *s=(unsigned short*)(data);
	long a;

	/* verify */
	for (a=0; a<size; a+=2) 
	{                 
		if (*d++!=*s++) 
		{
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
	char *addr = (char *)FS_FLASHBASE; /* get flash base addr */

	addr+=GetBlockAddr(block,0)+blockrel;

	fsm_memcpy(data,addr,datalen);

	return 0;
}

/****************************************************************************
 *
 * fs_phy_nor_28f320j3
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

int fs_phy_nor_28f320j3 (FS_FLASH *flash) 
{
	unsigned short man_ID;
	unsigned short dev_ID;

#ifdef HCC_HW
//	CS0CSOR=0xffc00404; /* 4M, 4 wait state, 16 bit device */
	CS0CSOR=0xff000404; /* 4M, 4 wait state, 16 bit device */
	CS0CSAR=0x01000001; /* base address, valid, wrprotect */
#endif

	flash->ReadFlash=ReadFlash;       /* read content */
	flash->EraseFlash=EraseFlash;     /* erase a block */
	flash->WriteFlash=WriteFlash;     /* write content */
	flash->VerifyFlash=VerifyFlash;   /* verify content */

	RemoveWriteProtect();

	FS_FLASHDEV(0)=0x90;
	man_ID=FS_FLASHDEV(0);

	FS_FLASHDEV(0)=0x90;
	dev_ID=FS_FLASHDEV(2);

	FS_FLASHDEV(0)=0xFF;

	SetWriteProtect();

	if (man_ID==0x89 && dev_ID==0x16) 
	{
		flash->maxblock=MAXBLOCKS;
		flash->blocksize=BLOCKSIZE;
		flash->sectorsize=SECTORSIZE;
		flash->sectorperblock=SECTORPERBLOCK;
		flash->blockstart=BLOCKSTART;		/* where 1st block starts */
		flash->descsize=DESCSIZE;	   	/* 128K 	 */
		flash->descblockstart=DESCBLOCKSTART;
		flash->descblockend=DESCBLOCKSTART+DESCBLOCKS-1; /*  num of last desc block  */
		flash->cacheddescsize=DESCCACHE;		/*  size of write cache in descriptor  */

		return 0; /*  28F320J3  4M  */
	}

	return 1;   /* unidentified or wrong device   */
}

/****************************************************************************
 *
 *  end of 28F320J3.c
 *
 ***************************************************************************/

#endif /* _28F320J3_C_ */
