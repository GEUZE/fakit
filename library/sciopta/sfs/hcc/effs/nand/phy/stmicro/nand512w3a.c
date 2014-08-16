
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

#include "nand512w3a.h"
#include <io.h>
#include <system.h>
#include "nand.h"

/****************************************************************************
 *
 * Flash Device specific definitions
 *
 * nand512w3a ST Micro
 *
 * Original bulding of FLASH
 * 512+16 bytes/pages
 * 32 pages/block (16K block size)
 * 4096 block
 *
 *
 ***************************************************************************/

#define BLOCK_SIZE   0x8000 /* 32k 2x16k */
#define SECTOR_SIZE  0x2000 /* 8k */
#define BLOCK_NUM    2048   /* 4096x16k block all, 2048x32k in config*/
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

#define PAGEPERBLOCK (BLOCK_SIZE/PAGE_SIZE) /* number of pages in a block */

/****************************************************************************
 *
 * HW specific definitions
 *
 ***************************************************************************/

/* send NAND command */
inline void NANDcmd(unsigned char cmd)
{
    IOWR_NAND_DATA(cmd);   //write command for device signature
    IOWR_NAND_COMMAND_LATCH(1); //write CLE
    IOWR_NAND_WRITE(0);
    IOWR_NAND_WRITE(1);
    IOWR_NAND_COMMAND_LATCH(0);
}
 
/* send NAND address */
inline void NANDaddr(unsigned char addr)
{
    IOWR_NAND_DATA(addr);       //write address data
    IOWR_NAND_ADDRESS_LATCH(1); //write ALE
    IOWR_NAND_WRITE(0);
    IOWR_NAND_WRITE(1);
    IOWR_NAND_ADDRESS_LATCH(0);
}

/* send NAND data */
inline void NANDdatawr(unsigned char data)
{
    IOWR_NAND_DATA(data);       //write address data
    IOWR_NAND_WRITE(0);
    IOWR_NAND_WRITE(1);
}

/* read NAND data */
inline unsigned char NANDdatard()
{
    unsigned char data;
    IOWR_NAND_READ(0);          //read MFG ID
    data = IORD_NAND_DATA;
    IOWR_NAND_READ(1);
    return data;
}
    
/* enable NAND */
inline void NANDenable()
{
    IOWR_NAND_CE(0);    //enable flash
}

/* disable NAND */
inline void NANDdisable()
{
    IOWR_NAND_CE(1);    //disable flash
}

/* initialize NAND signals */
inline void NANDinit()
{
    NANDdisable();
    IOWR_NAND_WRITE(1);
    IOWR_NAND_READ(1);  
    IOWR_NAND_WP(1);    //disable write protect
}

/* command of device */

#define NCMD_READ       0x00
#define NCMD_READSPARE  0x50

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
 * NANDwaitrb
 *
 * Wait until RB (ready/busy) signal goes high
 *
 ***************************************************************************/

static void NANDwaitrb() 
{
	for (;;) 
	{
		int state;

		NANDcmd(NCMD_READST);
		state=NANDdatard();

		if (state & ST_READY) 
		{
      		NANDcmd(NCMD_READ);
			return;
		}
	}
}

/****************************************************************************
 *
 * ReadPage
 *
 * Read one page from device to page buffer with ECC
 *
 * INPUTS
 * pagenum - relative page number what to read
 *
 * RETURNS
 * 0 - if successfully read
 * -1 - if only erased paged (ECC signalled error, and 0xFF check success)
 * other if any error
 *
 ***************************************************************************/

static int ReadPage(unsigned char *dest,long pagenum, unsigned long *sdata) 
{
	long a,b,num;
	unsigned long *rp=(unsigned long*)dest;
	unsigned long ecc,eccori,ecchi,ecclo;

	ecc=0;
	num=0x0000ffff; /* 24 bit ecc  */

	NANDenable();

	NANDcmd (NCMD_READ);
    NANDaddr(0);
    NANDaddr ((unsigned char)(pagenum & 0xff));
    NANDaddr ((unsigned char)((pagenum >> 8) & 0xff));
    NANDaddr ((unsigned char)((pagenum >> 16) & 0xff));

	NANDwaitrb();

	/* get whole page data */
	for (a=0; a<PAGE_SIZE; a+=4) 
	{  
		register long data=NANDdatard();
		data<<=8;
		data|=NANDdatard();
		data<<=8;
		data|=NANDdatard();
		data<<=8;
		data|=NANDdatard();

		*rp++=data;
		/* calculate ECC */
		for (b=0; b<32; b++) 
		{       
			if (data&1) ecc^=num;
			num+=0x0000ffff;
			data>>=1;
		}
	}

	/* get ecc from 1st 32bit in spare area */
	eccori=NANDdatard();
	eccori<<=8;
	eccori|=NANDdatard();
	eccori<<=8;
	eccori|=NANDdatard();
	eccori<<=8;
	eccori|=NANDdatard();

	if (sdata)
	{
		unsigned long data;

		/* bad blk info */
		data=NANDdatard();
		data=NANDdatard();
		data=NANDdatard();
		data=NANDdatard();

		data=NANDdatard();
		data<<=8;
		data|=NANDdatard();
		data<<=8;
		data|=NANDdatard();
		data<<=8;
		data|=NANDdatard();

		/* put data into sdata */
		*sdata=data;
	}

	NANDdisable();

	ecc^=eccori;
	if (!ecc) return 0; /* no bit error */
	if (!(ecc & ((unsigned long)(ecc-1)))) return 0; /* 1 bit error in ecc, so data is ok */

	ecchi=ecc>>16;
	ecclo=ecc&0x0ffff;

	if ( (ecchi + ecclo) !=0x0ffffUL ) 
	{
   		for (a=0; a<PAGE_SIZE; a++) 
		{
	   		if (dest[a]!=(unsigned char)0xff) return 1; /* ecc error */
   		}

   		if (eccori==0xffffffffUL) return -1; /* erased not written */

		return 1; /* ecc error */
	}

	dest[ ecchi >> 3] ^= (1<< (ecchi&7) ); /* correcting error */

	return 0;
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
	long a,b;
	unsigned long ecc,num;
	unsigned long *rp;

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

	ecc=0;
	num=0x0000ffff; /* 24 bit ecc  */

	NANDenable();

	NANDcmd  (NCMD_PAGEPRG);
    NANDaddr (0);
    NANDaddr ((unsigned char)(pagenum & 0xff));
    NANDaddr ((unsigned char)((pagenum >> 8) & 0xff));
    NANDaddr ((unsigned char)((pagenum >> 16) & 0xff));

	/* write one page data */
	for (a=0; a<PAGE_SIZE; a+=4) 
	{  
		unsigned long rdata=*rp++;

		NANDdatawr ((unsigned char)((rdata >>24) & 0xff));
		NANDdatawr ((unsigned char)((rdata >>16) & 0xff));
		NANDdatawr ((unsigned char)((rdata >>8) & 0xff));
		NANDdatawr ((unsigned char)(rdata & 0xff));

		/* calculate ECC */
		for (b=0; b<32; b++) 
		{       
			if (rdata&1) ecc^=num;
			num+=0x0000ffff;
			rdata>>=1;
		}
	}

	/* write ecc also, but this is in spare area yet */
	NANDdatawr ((unsigned char)((ecc >>24) & 0xff));
	NANDdatawr ((unsigned char)((ecc >>16) & 0xff));
	NANDdatawr ((unsigned char)((ecc >>8) & 0xff));
	NANDdatawr ((unsigned char)(ecc & 0xff));

    /* this is not a bad block */
	NANDdatawr ((unsigned char)0xff);
	NANDdatawr ((unsigned char)0xff);
	NANDdatawr ((unsigned char)0xff);
	NANDdatawr ((unsigned char)0xff);

	/* add signature data into spare 3rd word */
	NANDdatawr ((unsigned char)((sdata >>24) & 0xff));
	NANDdatawr ((unsigned char)((sdata >>16) & 0xff));
	NANDdatawr ((unsigned char)((sdata >>8) & 0xff));
	NANDdatawr ((unsigned char)(sdata & 0xff));


	NANDcmd (NCMD_PAGEPRG2);
	NANDwaitrb();

	NANDcmd (NCMD_READST);
	io=NANDdatard(); /* holding status */

	NANDdisable();

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
	int num;

/*    fnPr("Erase: %d\n",block);    */

	/* erase 2 blocks at once in 2048 block config */
	/* where 2x16k block is a 32k logical block */
	for (num=0; num<2; num++)
	{
		NANDenable();

		NANDcmd (NCMD_BLKERASE);
	    NANDaddr ((unsigned char)(pagenum & 0xff));
	    NANDaddr ((unsigned char)((pagenum >> 8) & 0xff));
	    NANDaddr ((unsigned char)((pagenum >> 16) & 0xff));
		NANDcmd  (NCMD_BLKERASE2);

		NANDwaitrb();

		NANDcmd (NCMD_READST);
		io=NANDdatard(); /* holding status */

		NANDdisable();

		if (io & ST_ERROR) return 1;

		if (!(io & ST_READY)) return 1;

		/* goto next block (1 physical block is 32 pages) */
		pagenum+=32; 
	}

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
 * CheckSignature
 *
 * checking signature with 1 bit error included
 *
 * INPUTS
 *
 * value - read from nflash signature (could content 1 bit error)
 * mask - mask need to be checked
 *
 * RETURNS
 *
 * 0 - signature is match
 * other - signature is invalid or having 2 bits error
 *
 ***************************************************************************/

static unsigned long CheckSignature(unsigned long value, unsigned long mask)
{
	value^=mask;
	return (value & ((unsigned long)(value-1)));
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

		/* check signature 1st */
		if (CheckSignature(readsign,(unsigned long)sdata)) return 1;

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

		/* check signature 1st */
		if (CheckSignature(readsign,(unsigned long)sdata)) return 1;

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
		NANDenable();

   		NANDcmd  (NCMD_READSPARE);
		NANDaddr (4); /* ecc,badblk,signature*/
		NANDaddr ((unsigned char)(pagenum & 0xff));
		NANDaddr ((unsigned char)((pagenum >> 8) & 0xff));
		NANDaddr ((unsigned char)((pagenum >> 16) & 0xff));

   		NANDwaitrb();

		/* bad block info*/
		data=NANDdatard();
		data<<=8;
		data|=NANDdatard();
		data<<=8;
		data|=NANDdatard();
		data<<=8;
		data|=NANDdatard();

		NANDdisable();

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

	NANDenable();

	NANDcmd (NCMD_READSPARE);
	NANDaddr (8); /* ecc,badblk,signature*/
	NANDaddr ((unsigned char)(pagenum & 0xff));
	NANDaddr ((unsigned char)((pagenum >> 8) & 0xff));
	NANDaddr ((unsigned char)((pagenum >> 16) & 0xff));

  	NANDwaitrb();

	/* signature */
	data=NANDdatard();
	data<<=8;
	data|=NANDdatard();
	data<<=8;
	data|=NANDdatard();
	data<<=8;
	data|=NANDdatard();

	NANDdisable();

	return data;
}

/****************************************************************************
 *
 * fs_phy_nand_nand512w3a
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

int fs_phy_nand_nand512w3a(FS_FLASH *flash) 
{
	long manID, devID;

	page=(unsigned char*)l_page; /* select a long aligned buffer */

	flash->ReadFlash=ReadFlash;         /* read content */
	flash->EraseFlash=EraseFlash;        /* erase a block */
	flash->WriteFlash=WriteFlash;        /* write content */
	flash->VerifyFlash=VerifyFlash;       /* verify content */
	flash->CheckBadBlock=CheckBadBlock;     /* check if block is bad block (NAND) */
	flash->GetBlockSignature=GetBlockSignature; /* get block signature data (NAND) */
	flash->WriteVerifyPage=WriteVerifyPage;

	NANDinit();

	NANDenable();

	NANDcmd (NCMD_READID);
	NANDaddr(0);

	manID=NANDdatard();
	devID=NANDdatard();

	NANDdisable();

	if ((manID==0x0020) && (devID==0x0076)) 
	{
   		flash->maxblock=BLOCK_NUM-BLOCK_START;/* 1023 1st block is separated */
	   	flash->blocksize=BLOCK_SIZE;	        /* 16k	 */
	   	flash->sectorsize=SECTOR_SIZE;        /* 4k */
	   	flash->sectorperblock=flash->blocksize/flash->sectorsize; /* sector per block */
	   	flash->blockstart=BLOCK_START;	/* where 1st block starts */
	   	flash->descsize=BLOCK_SIZE;		/* 16K 	 */
	   	flash->separatedir=SEPARATE_DIR; /* fat+dir */

		flash->cachedpagenum=PAGEPERBLOCK;
		flash->cachedpagesize=PAGE_SIZE;

	   	return 0; /* ok nand128w3a */
	}

	return 1; /*  unknown type  */
}

/****************************************************************************
 *
 *  end of nand512w3a.c
 *
 ***************************************************************************/

