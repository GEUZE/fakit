#ifndef _K9F2816X0C_16bit_C_
#define _K9F2816X0C_16bit_C_

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

#include "k9f2816x0c_16bit.h"
#include "ecc.h"

/****************************************************************************
 *
 * Flash Device specific definitions
 *
 * K9F2816X0C Samsung
 *
 ***************************************************************************/

#define BLOCK_SIZE   0x4000 /* 16k */
#define SECTOR_SIZE  0x1000 /* 4k */
#define BLOCK_NUM    1024   /* 1024 block all */
#define PAGE_SIZE    512    /* page size is 512 byte */
#define BLOCK_START  1      /* 1st block is kept for any purpose */
#define SEPARATE_DIR 1      /* directory is in separated block from fat */

/****************************************************************************
 *
 * static definitions
 *
 ***************************************************************************/

static unsigned long l_page[PAGE_SIZE/4];
static unsigned char *page;

static unsigned char eccs[6];
static unsigned char ecc_calc[6];

#define PAGEPERBLOCK (BLOCK_SIZE/PAGE_SIZE) /* number of pages in a block */

/****************************************************************************
 *
 * HW specific definitions
 *
 ***************************************************************************/

/* base addresses */

#define NANDCMD  (*(volatile unsigned long*)0x1000000)
#define NANDADDR (*(volatile unsigned long*)0x1000004)
#define NANDDATA (*(volatile unsigned long*)0x1000008)
#define NANDRB   (*(volatile unsigned long*)0x100000c)

/* command of device */

#define NCMD_READ1     0x00
#define NCMD_READ2     0x50
#define NCMD_READID    0x90
#define NCMD_RESET     0xFF
#define NCMD_PAGEPRG   0x80
#define NCMD_PAGEPRG2  0x10
#define NCMD_BLKERASE  0x60
#define NCMD_BLKERASE2 0xD0
#define NCMD_READST    0x70

/*  status bits  */

#define ST_ERROR 0x01
#define ST_READY 0x40
#define ST_WPROT 0x80

/****************************************************************************
 *
 * NANDcmd
 *
 * Send command to chip ( CLE-hi  ALE-lo WR-hi)
 *
 * INPUTS
 *
 * cmd - what byte command to send
 *
 ***************************************************************************/

static void NANDcmd(long cmd) 
{
	NANDCMD = cmd | 0xff0000;
}

/****************************************************************************
 *
 * NANDaddr
 *
 * send address to chip ( CLE-lo ALE-hi WR-hi )
 *
 * INPUTS
 *
 * addr - page number
 *
 ***************************************************************************/

static void NANDaddr(long addr) 
{
	addr&=0xff;
	NANDADDR = addr;
}

/****************************************************************************
 *
 * NANDwaitrb
 *
 * Wait until RB (ready/busy) signal goes high
 *
 ***************************************************************************/

static void NANDwaitrb() 
{
	for (;;) 
	{
		if (NANDRB & 1) return;
	}
}

/****************************************************************************
 *
 * ReadPage
 *
 * Read one page from device to page buffer with ECC
 *
 * INPUTS
 * dest - where to read it
 * pagenum - relative page number what to read
 *
 * RETURNS
 * 0 - if successfully read
 * -1 - if only erased paged (ECC signalled error, and 0xFF check success)
 * other if any error
 *
 ***************************************************************************/

static int ReadPage(unsigned char *dest,long pagenum,unsigned long *psignature) 
{
	long a;
	unsigned long *rp=(unsigned long*)dest;
	unsigned short sdata;
	unsigned long sigdata;

	NANDcmd(NCMD_READ1);
	NANDaddr(0);
	NANDaddr(pagenum);
	NANDaddr(pagenum>> 8);
	NANDwaitrb();

	/* get whole page data */
	for (a=0; a<PAGE_SIZE; a+=4) 
	{  
		register long data=NANDDATA & 0xffff;
		data<<=16;
		data|=NANDDATA & 0xffff; /* 2times 16bit */

		*rp++=data;
	}

	sdata=(unsigned short)NANDDATA;
	eccs[0]=(unsigned char)(sdata>>8);
	eccs[1]=(unsigned char)(sdata);

	sdata=(unsigned short)NANDDATA;
	eccs[2]=(unsigned char)(sdata>>8);
	eccs[3]=(unsigned char)(sdata);

	sigdata=(unsigned short)NANDDATA; //signature hi
	sigdata<<=16;
	sigdata|=(unsigned short)NANDDATA; //signature lo

	if (psignature) *psignature=sigdata;

	sdata=(unsigned short)NANDDATA; //bad hi
	sdata=(unsigned short)NANDDATA; //bad lo

	sdata=(unsigned short)NANDDATA;
	eccs[4]=(unsigned char)(sdata>>8);
	eccs[5]=(unsigned char)(sdata);


	_CalculateSwECC(dest,ecc_calc);
	_CalculateSwECC(dest+256,ecc_calc+3);

	if (_CorrectSwECC(dest,eccs,ecc_calc)==-1) return 1;
	if (_CorrectSwECC(dest+256,eccs+3,ecc_calc+3)==-1) return 1;

	if (sigdata!=0xffffffffUL) return 0;

	for (a=0; a<6; a++) 
	{
		if (eccs[a]!=0xff) return 0;
	}

	for (a=0; a<512; a++) 
	{
		if (dest[a]!=0xff) return 0;
	}

	return -1; /* erased not written */
}

/****************************************************************************
 *
 * WritePage
 *
 * write page data into flash device, with ECC
 *
 * INPUTS
 * data - data ptr to original data
 * pagenum - relativ pagenumber where to store data
 * size - size of data always less than page_size
 * sdata - signature data for page
 *
 * RETURNS
 * 0 - if successfully written
 * other if any error
 *
 ***************************************************************************/

static int WritePage(void *data, long pagenum, long size,long sdata) 
{
	long io;
	long a;
	unsigned long *rp;
	unsigned short spdata;

	if (size<0 || size>PAGE_SIZE)
	{
		return 1; /* invalid length */
	}

	if (( !(((long)data) & 3) ) && size==PAGE_SIZE) 
	{
		rp=(unsigned long*)data;
	}
	else 
	{
		unsigned char *s=(unsigned char*)data;
		rp=(unsigned long*)page;

   		for (a=0; a<size; a++) page[a]=*s++;   /* copy data into page temporarly	 */
		for (; a<PAGE_SIZE; a++) page[a]=(unsigned char)0xff; /* fills remainings with FF-s */
	}

	NANDcmd(NCMD_READ1);
	NANDcmd(NCMD_PAGEPRG);
	NANDaddr(0);
	NANDaddr(pagenum);
	NANDaddr(pagenum>> 8);

	/* write one page data */
	for (a=0; a<PAGE_SIZE; a+=4) 
	{  
		unsigned long rdata=*rp++;

		NANDDATA= (rdata>>16) & 0xffff;
		NANDDATA= rdata&0xffff; /* 2times 16bit */

	}

	_CalculateSwECC(((unsigned char*)rp)-PAGE_SIZE,eccs);
	_CalculateSwECC(((unsigned char*)rp)-PAGE_SIZE/2,eccs+3);

	spdata=eccs[0];
	spdata<<=8;
	spdata|=eccs[1];
	NANDDATA= spdata;   /* write ecc also, but this is in spare area yet */

	spdata=eccs[2];
	spdata<<=8;
	spdata|=eccs[3];
	NANDDATA= spdata;        /* 16bit */

	NANDDATA= (sdata >> 16) & 0xffff; /* add signature data into spare 2nd word */
	NANDDATA= sdata & 0xffff;        /* 16bit */

	NANDDATA=0xffff; /* bad hi */
	NANDDATA=0xffff; /* bad lo */

	spdata=eccs[4];
	spdata<<=8;
	spdata|=eccs[5];
	NANDDATA= spdata;        /* 16bit */


	NANDcmd(NCMD_PAGEPRG2);
	NANDwaitrb();

	NANDcmd(NCMD_READST);
	io=NANDDATA; /* holding status */

	if (io & ST_ERROR) return 1;

	if (!(io & ST_READY)) return 1;

	return 0;
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
 * 0 - if successfully read
 * other if any error
 *
 ***************************************************************************/

static int ReadFlash(void *data, long block, long blockrel, long datalen) 
{
	long pagenum=block*PAGEPERBLOCK;
	unsigned char *s=(unsigned char*)data;
	long psize;
	int ret;

	/* calculate start */
	for (;blockrel>=PAGE_SIZE;) 
	{ 
		blockrel-=PAGE_SIZE;
		pagenum++;
	}

	if (blockrel) 
	{
		psize=PAGE_SIZE-blockrel;
		ret=ReadPage(page,pagenum,0);
		if (ret) return ret;
		pagenum++;

		if (psize>datalen) psize=datalen;

		fsm_memcpy(s,page+blockrel,psize);	/* copy head */

		datalen-=psize;
		s+=psize;
	}

	/* check if buffer is long aligned*/
	if (!(((long)s) & 3)) 
	{       
		/* if it is, accelarated reading */
		while (datalen>=PAGE_SIZE) 
		{ 
   			ret=ReadPage(s,pagenum,0);
   			if (ret) return ret;
   			pagenum++;

   			datalen-=PAGE_SIZE;
   			s+=PAGE_SIZE;
		}
	}

	while (datalen) 
	{
		psize=datalen;
		if (psize>=PAGE_SIZE) psize=PAGE_SIZE;

		ret=ReadPage(page,pagenum,0);
		if (ret) return ret;
		pagenum++;

		fsm_memcpy(s,page,psize);

		datalen-=psize;
		s+=psize;
	}

	return 0;
}

/****************************************************************************
 *
 * EraseFlash
 *
 * erase block
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
	long pagenum=block*PAGEPERBLOCK;
	long io;

/*   fnPr("Erase: %d blk\n",block);    */

	NANDcmd(NCMD_BLKERASE);
	NANDaddr(pagenum);
	NANDaddr(pagenum>> 8);

	NANDcmd(NCMD_BLKERASE2);
	NANDwaitrb();

	NANDcmd(NCMD_READST);
	io=NANDDATA; /* holding status */

	if (io & ST_ERROR) return 1;

	if (!(io & ST_READY)) return 1;

	return 0;
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
 * sdata - signature data to block
 *
 * RETURNS
 *
 * 0 - if ok
 * 1 - if there was any error
 *
 ***************************************************************************/

static int WriteFlash(void *data, long block,long relsector, long size,long sdata) 
{
	unsigned char *s=(unsigned char*)data;
	long pagenum=block*PAGEPERBLOCK+(SECTOR_SIZE/PAGE_SIZE)*relsector;

	while (size) 
	{
		long psize=size;
		if (psize>PAGE_SIZE) psize=PAGE_SIZE;

		if (WritePage(s,pagenum,psize,sdata)) return 1;
		pagenum++;
		s+=psize;
		size-=psize;
	}

	return 0;
}

/****************************************************************************
 *
 * WriteVerifyPage
 *
 * Writing (programming) Flash device pages and verify them, chache routine
 * uses this only
 *
 * INPUTS
 *
 * data - where data is
 * block - which block is programmed
 * startpage - relative pagenumber in block
 * pagenum - number of page needs to be written
 * sdata - signature data to block
 *
 * RETURNS
 *
 * 0 - if ok
 * 1 - if there was any error
 *
 ***************************************************************************/

static int WriteVerifyPage(void *data, long block, long startpage, long pagenum, long sdata) 
{
	unsigned long readsign;
	unsigned char *s=(unsigned char*)data;
	long spage=block*PAGEPERBLOCK+startpage;
	long a;

	for (a=0; a<pagenum; a++) 
	{
		if (WritePage(s,spage+a,PAGE_SIZE,sdata)) return 1;
		s+=PAGE_SIZE;
	}

	s=(unsigned char*)data;
	for (a=0; a<pagenum; a++) 
	{
		int b;
		unsigned char *d=page;
		if (ReadPage(page,spage+a,&readsign)) return 1;

		if (readsign!=(unsigned long)sdata) return 1; /* check signature 1st */

		for (b=0; b<PAGE_SIZE; b++) 
		{
			if (*d++!=*s++) return 1;
		}
	}

	return 0;
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
 * sdata - signature data to block
 *
 * RETURNS
 *
 * 0 - if ok
 * 1 - if there was any error
 *
 ***************************************************************************/

static int VerifyFlash(void *data, long block,long relsector, long size,long sdata) 
{
	unsigned char *d;
	unsigned char *s=(unsigned char*)data;
	long pagenum=block*PAGEPERBLOCK+(SECTOR_SIZE/PAGE_SIZE)*relsector;
	unsigned long readsign;

	while (size) 
	{
		long psize=size;
		if (psize>PAGE_SIZE) psize=PAGE_SIZE;

		if (ReadPage(page,pagenum,&readsign)) return 1; /* read error */

		if (readsign!=(unsigned long)sdata) return 1; /* check signature 1st */

		d=page;
		size-=psize;

		while (psize--) 
		{
			if (*d++!=*s++) 
			{
				return 1;
			}
		}

		pagenum++;
	}

	return 0;
}

/****************************************************************************
 *
 * CheckBadBlock
 *
 * this function is called from higher layer to determinate if a given block
 * is bad or not bad.
 *
 * INPUTS
 *
 * block - which block needs to be checked
 *
 * RETURNS
 *
 * 0 - not bad
 * 1 - bad block
 *
 ***************************************************************************/

static int CheckBadBlock(long block) 
{
	long pagenum=block*PAGEPERBLOCK;
	unsigned long data;
	int a;

	for (a=0; a<2; a++,pagenum++) 
	{
		NANDcmd(NCMD_READ2);   /* read spare area */
		NANDaddr(0);
		NANDaddr(pagenum);
		NANDaddr(pagenum >> 8);
		NANDwaitrb();

		data=NANDDATA; /* 1st <ecc> hi */
		data=NANDDATA; /* 2nd <ecc> lo */
		data=NANDDATA; /* 3rd <signature> hi */
		data=NANDDATA; /* 4rd <signature> lo */

		data=NANDDATA & 0xffff;
		data<<=16;
		data|=NANDDATA & 0xffff; /* 6th word contains no ffffffff if bad */

		if (data!=0xffffffffUL) return 1; /* signal BAD */
	}

	return 0;
}


/****************************************************************************
 *
 * GetBlockSignature
 *
 * this function is called from higher layer to retreve block
 * signature data
 *
 * INPUTS
 *
 * block - which block needs to be checked
 *
 * RETURNS
 * with the signature of block
 *
 ***************************************************************************/

static long GetBlockSignature(long block) 
{
	unsigned long pagenum=block*PAGEPERBLOCK;
	long data;

	NANDcmd(NCMD_READ2);   /* read spare area */
	NANDaddr(0);
	NANDaddr(pagenum);
	NANDaddr(pagenum >> 8);
	NANDwaitrb();

	data=NANDDATA; /* 1st <ecc> hi */
	data=NANDDATA; /* 2nd <ecc> lo */

	data=NANDDATA & 0xffff; /* 3rd <signature> hi */
	data<<=16;
	data|=NANDDATA & 0xffff; /* 4rd <signature> lo */

	return data;
}

/****************************************************************************
 *
 * fs_phy_nand_K9F2816X0C
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

int fs_phy_nand_K9F2816X0C_16bit(FS_FLASH *flash) 
{
	long manID, devID;

	page=(unsigned char*)l_page; /* select a long aligned buffer */

/*   fnPr("NandFlash with new ECC algorithm\n"); */

	flash->ReadFlash=ReadFlash;         /* read content */
	flash->EraseFlash=EraseFlash;        /* erase a block */
	flash->WriteFlash=WriteFlash;        /* write content */
	flash->VerifyFlash=VerifyFlash;       /* verify content */
	flash->CheckBadBlock=CheckBadBlock;     /* check if block is bad block (NAND) */
	flash->GetBlockSignature=GetBlockSignature; /* get block signature data (NAND) */
	flash->WriteVerifyPage=WriteVerifyPage;

	NANDcmd(NCMD_READID);
	NANDaddr(0);

	manID=NANDDATA & 0xffff;
	devID=NANDDATA & 0xffff;

	if ((manID==0x00ec) && (devID==0x0053)) 
	{
   		flash->maxblock=BLOCK_NUM-BLOCK_START;/* 2047; 		//1023*8K 1st block is separated */
   		flash->blocksize=BLOCK_SIZE;	        /* 8k	 */
   		flash->sectorsize=SECTOR_SIZE;        /* 8k */
   		flash->sectorperblock=flash->blocksize/flash->sectorsize; /* sector per block */
   		flash->blockstart=BLOCK_START;	/* where 1st block starts */
   		flash->descsize=BLOCK_SIZE;		/* 8K 	 */
   		flash->separatedir=SEPARATE_DIR; /* fat+dir */

		flash->cachedpagenum=PAGEPERBLOCK;
		flash->cachedpagesize=PAGE_SIZE;
   		return 0; /* ok */
	}

	return 1; /*  unknown type  */
}

/****************************************************************************
 *
 *  end of K9F2816X0C_16bit.c
 *
 ***************************************************************************/

#endif /* _K9F2816X0C_16bit_C_ */
