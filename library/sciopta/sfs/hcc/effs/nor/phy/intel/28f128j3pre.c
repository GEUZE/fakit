#ifndef _28F128J3PRE_C_
#define _28F128J3PRE_C_

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

#include "28F128J3pre.h"

/****************************************************************************
 *
 *  28F128J3 Intel
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

/*  physical        */
/*  64x128k blocks  */

#define NUMOFBLOCKS   64      /* 64 x 128k blocks */
#define BLOCKSIZE	0x20000		/*  128k - size of file storage blocks  */
#define BLOCKSTART 2		      /*  logical number of first file storage block  */
#define MAXBLOCKS	 62		   /*  number of logical blocks for file storage  */

#define DESCSIZE	0x20000		/*  128k descriptor block size  */
#define DESCBLOCKSTART	0		/*  logical number of first descriptor block  */
#define DESCBLOCKS	   2		/*  number of descriptor blocks  */
#define DESCCACHE	0x18000     /*  96K assigned to write cache for descriptor  */

#define SECTORSIZE 	0x1000		/* 4k */
#define SECTORPERBLOCK (BLOCKSIZE/SECTORSIZE)

/* Please check maximum storeable file and used sector number in FSmem.exe */

static unsigned char fl_chkeraseblk[NUMOFBLOCKS];
static unsigned char fl_erasedblk[NUMOFBLOCKS];
static volatile long fl_blknum=0xffffUL;
static FS_MUTEX_TYPE gl_premutex;

/****************************************************************************
 *
 * SuspendErase
 *
 * This function checks if erase is pending then suspends it
 *
 ***************************************************************************/

static void SuspendErase() 
{
	unsigned short status;

	if (fl_blknum!=0xffff) 
	{
/*	fnpr(" suspend ");   */
		FS_FLASHDEV(0)=0xB0;     /* suspend command */
		for (;;) 
		{
			status=FS_FLASHDEV(0);
			if (status & 0x80) break;
		}

		FS_FLASHDEV(0)=0xff;     /* set flash array back */
		/* check if erase is finished */
		if (!(status & 0x40)) 
		{  
			fl_erasedblk[fl_blknum]=1;
			fl_blknum=0xffff;
		}
	}
}

/****************************************************************************
 *
 * ResumeErase
 *
 * This function checks if erase was suspended then resumes erase
 *
 ***************************************************************************/

static void ResumeErase() 
{
	if (fl_blknum!=0xffff) 
	{
/*	fnpr(" resume \n");      */
		FS_FLASHDEV(0)=0xD0; /* resume command */
	}
}

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
 * WaitForMutex
 *
 * Wait for a mutex to be free without timeout
 *
 * INPUTS
 *
 * pmutex - mutex for waiting for
 *
 ***************************************************************************/

static void WaitForMutex(FS_MUTEX_TYPE *pmutex) 
{
	for (;;) 
	{
		if (!fs_mutex_get(pmutex)) return;
	}
}

/****************************************************************************
 *
 * LowFlashErase
 *
 * function for Preerasing block, this routine should start as a low
 * priority task, need to be called cyclically
 *
 ***************************************************************************/

void LowFlashErase() 
{
	long a,addr;
	unsigned short *s;

	for(;;)
	{
		char isempty;

		WaitForMutex (&gl_premutex);
		RemoveWriteProtect();

		/* check if a block is requested to be deleted */
		for (a=0; a<NUMOFBLOCKS; a++) 
		{
			if (fl_chkeraseblk[a] && (!fl_erasedblk[a])) 
			{
				fl_blknum=a;
				break;
			}
		}

		/* check if any block found */
		if (fl_blknum==0xffff) 
		{
			SetWriteProtect();
			(void)fs_mutex_put(&gl_premutex);

			/* no block found so return */
			return;
		}

		/* check if block is empty */
		addr=GetBlockAddr(fl_blknum,0);
		s=(unsigned short *)(FS_FLASHBASE+addr);

		for (a=0, isempty=1; a<BLOCKSIZE; a+=2) 
		{
			/* is it empty */
			if (*s++!=0xffff) 
			{
				isempty=0;
				break;
			}
		}

/* fnpr("LowFlashErase chkblk %d\n",fl_blknum); */
		if (isempty)
		{
	    /* block is already erased so just set erased flag */
		fl_erasedblk[fl_blknum]=1; /* erased, contains 0xffff all */
		fl_blknum=0xffff;

		SetWriteProtect();
		(void)fs_mutex_put(&gl_premutex);
		}
		else
		{
/* fnpr("LowFlashErase Started\n"); */

			/* execute erase command */
		FS_FLASHDEV(addr)=0x20;
		FS_FLASHDEV(addr)=0xd0;

		SetWriteProtect();
		(void)fs_mutex_put(&gl_premutex);

			/* polling erase until finished */
		while (fl_blknum!=0xffff) 
		{
			WaitForMutex (&gl_premutex);
			RemoveWriteProtect();

			if (fl_blknum!=0xffff) 
			{
				unsigned short status;

					/* execute read status command */
				FS_FLASHDEV(0)=0x70;
				status=FS_FLASHDEV(0);
				FS_FLASHDEV(0)=0xff;

					/* is erased finished? */
				if (status & 0x80) 
				{
						/* set erased flag */
					fl_erasedblk[fl_blknum]=1;
					fl_blknum=0xffff;
				}
			}

			SetWriteProtect();
			(void)fs_mutex_put(&gl_premutex);
		}
	}
	}
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

	for(;;)
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

/* fnpr("EraseFlash\n");   */
    WaitForMutex (&gl_premutex);
    RemoveWriteProtect();

	/* check if erase pending */
    if (fl_blknum!=0xffff) 
	{
		unsigned short status;

		for(;;) 
		{
			FS_FLASHDEV(0)=0x70;
			status=FS_FLASHDEV(0);
			if (status & 0x80) break;
		}
		fl_erasedblk[fl_blknum]=1;
		fl_blknum=0xffff;
		FS_FLASHDEV(0)=0xff;
    }

	/* check if block is already erased */
    if (fl_erasedblk[block]) 
	{
       fl_erasedblk[block]=0;
       fl_chkeraseblk[block]=0;
	   SetWriteProtect();
       (void)fs_mutex_put(&gl_premutex);
       return 0;
    }

/* fnpr("Real Erase \n");*/
	/* erase it */
    FS_FLASHDEV(addr)=0x20;
    FS_FLASHDEV(addr)=0xd0;

    ret=DataPoll(addr,(unsigned short)0x0ffff);

    fl_erasedblk[block]=0;
    fl_chkeraseblk[block]=0;

    SetWriteProtect();
    (void)fs_mutex_put(&gl_premutex);

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

/* fnpr("WriteFlash\n"); */

	WaitForMutex (&gl_premutex);

	RemoveWriteProtect();

	SuspendErase();

	while (size>0) 
	{
		/* write 16 word (32 byte) at once */
		if (size>=32) 
		{
			long st=0;
again:
			FS_FLASHDEV(dest)=0xE8;
			FS_FLASHDEV(0)=0x70;
			status=FS_FLASHDEV(0);

			if (!(status & 0x80)) 
			{
				goto again;
			}

			FS_FLASHDEV(dest)=0x0f; /* word count */

			for (a=0; a<32; a+=2) 
			{
				FS_FLASHDEV(dest+a)=*s;
				s++;
			}

			FS_FLASHDEV(dest)=0xD0; /* confirm */

			for(;;) 
			{
				FS_FLASHDEV(0)=0x70;
				status=FS_FLASHDEV(0);
				if (status & 0x80) 
				{
					break;
				}

				if (st++==0x1000000L) 
				{
					FS_FLASHDEV(0)=0xff;
					ret=2;
					goto vege;
				}
			}

			FS_FLASHDEV(0)=0xff;

			size-=32;
			dest+=32;
		}
		else 
		{
			/* write remaining */ 

			FS_FLASHDEV(dest)=0x40;
			FS_FLASHDEV(dest)=*s;

			if (DataPoll(dest,*s)) 
			{
				ret=1;
				goto vege;
			}
			dest+=2;
			s++;
			size-=2;
		}
	}

vege:

	ResumeErase();

	SetWriteProtect();

	(void)fs_mutex_put(&gl_premutex);

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

	WaitForMutex (&gl_premutex);
	RemoveWriteProtect();
	SuspendErase();

	/* verify */
	for (a=0; a<size; a+=2) 
	{                 
		if (*d++!=*s++) 
		{
			ResumeErase();
			SetWriteProtect();
			(void)fs_mutex_put(&gl_premutex);
			return 1;  /* failed */
		}
	}

	ResumeErase();
	SetWriteProtect();
	(void)fs_mutex_put(&gl_premutex);
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

/* fnpr("ReadFlash\n");   */

	WaitForMutex (&gl_premutex);
	RemoveWriteProtect();
	SuspendErase();

	fsm_memcpy(data,addr,datalen);

	ResumeErase();
	SetWriteProtect();
	(void)fs_mutex_put(&gl_premutex);

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

static int gl_initiated=0;

int fs_phy_nor_28f128j3 (FS_FLASH *flash) 
{
	unsigned short man_ID;
	unsigned short dev_ID;

#ifdef HCC_HW
	CS0CSOR=0xffc00404; /* 4M, 4 wait state, 16 bit device */
	CS0CSAR=0x01000001; /* base address, valid, wrprotect */
#endif

	if (!gl_initiated) 
	{
		if (fs_mutex_create(&gl_premutex))
		{
			return 1; /* signal error */
		}
		gl_initiated=1;
	}

	flash->ReadFlash=ReadFlash;       /* read content */
	flash->EraseFlash=EraseFlash;     /* erase a block */
	flash->WriteFlash=WriteFlash;     /* write content */
	flash->VerifyFlash=VerifyFlash;   /* verify content */

	flash->chkeraseblk=fl_chkeraseblk;
	flash->erasedblk=fl_erasedblk;

	WaitForMutex (&gl_premutex);
	RemoveWriteProtect();
	SuspendErase();

	FS_FLASHDEV(0)=0x90;
	man_ID=FS_FLASHDEV(0);

	FS_FLASHDEV(0)=0x90;
	dev_ID=FS_FLASHDEV(2);

	FS_FLASHDEV(0)=0xFF;

	ResumeErase();
	SetWriteProtect();
	(void)fs_mutex_put(&gl_premutex);

	if (man_ID==0x89 && dev_ID==0x17) 
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

		return 0; /*  28F128J3  8M  */
	}

	return 1;   /* unidentified or wrong device   */
}

/****************************************************************************
 *
 *  end of 28F128J3PRE.c
 *
 ***************************************************************************/

#endif /* _28F128J3PRE_C_ */
