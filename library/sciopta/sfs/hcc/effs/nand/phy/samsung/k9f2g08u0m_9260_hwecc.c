#ifndef _k9f2g08u0m_C_
#define _k9f2g08u0m_C_

/****************************************************************************
 *
 *            Copyright (c) 2003-2005 by HCC Embedded
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
 * Budapest 1132
 * Victor Hugo Utca 11-15
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/

#include "k9f2g08u0m.h"

#define DELAY_100NS		{ asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP");  }

/****************************************************************************
 *
 * Flash Device specific definitions
 *
 * k9f2g08u0m Samsung with SAM9260
 *
 * data is in this way on a data page
 *
 * ddddddddd fEs
 *
 * where:
 *        d - is 2k of data
 *        f - is 0xffffffff for signaling no bad block
 *        E - 32 bit ECC for data
 *		  s - 32 bit signature
 *
 ***************************************************************************/

#define BLOCK_SIZE   0x20000 /* 128k */
#define SECTOR_SIZE  0x2000 /* 8k */
#define BLOCK_NUM    2048   /* 2048 block all */
#define PAGE_SIZE    2048   /* page size is 2048 byte */
#define BLOCK_START  1      /* 1st block is kept for any purpose */
#define SEPARATE_DIR 1      /* directory is in separated block from fat */

/****************************************************************************
 *
 * static definitions
 *
 ***************************************************************************/

static unsigned long lpage[PAGE_SIZE/4];
static unsigned char *page;

#define PAGEPERBLOCK (BLOCK_SIZE/PAGE_SIZE) /* number of pages in a block */

/****************************************************************************
 *
 * HW specific definitions
 *
 ***************************************************************************/

/* base addresses */
#define NANDDATAB (*(volatile unsigned char*)0x40000000)
#define NANDDATAL (*(volatile unsigned long*)0x40000000)
#define NANDCMD   (*(volatile unsigned char*)0x40400000)
#define NANDADDR  (*(volatile unsigned char*)0x40200000)

/* power management controller */
#define _PMC_BASE		0xfffffc00
#define _PMC_PCER		*(volatile unsigned long *)(_PMC_BASE+0x10)
#define _PIOC_ID		(1<<4)

/* static memory controller */
#define _SMC_BASE		0xffffec00
#define _SMC_SETUP(x)	*(volatile unsigned long *)(_SMC_BASE+(0x10*x)+0x00)
#define _SMC_PULSE(x)	*(volatile unsigned long *)(_SMC_BASE+(0x10*x)+0x04)
#define _SMC_CYCLE(x)	*(volatile unsigned long *)(_SMC_BASE+(0x10*x)+0x08)
#define _SMC_MODE(x)	*(volatile unsigned long *)(_SMC_BASE+(0x10*x)+0x0c)

/* bus matrix */
#define _MATRIX_BASE	0xffffee00
#define _EBI_CSA		*(volatile unsigned long *)(_MATRIX_BASE+0x30)

/* PIOC */
#define _PIOC_BASE		0xfffff800
#define _PIOC_PER		*(volatile unsigned long *)(_PIOC_BASE+0x00)
#define _PIOC_PDR		*(volatile unsigned long *)(_PIOC_BASE+0x04)
#define _PIOC_OER		*(volatile unsigned long *)(_PIOC_BASE+0x10)
#define _PIOC_ODR		*(volatile unsigned long *)(_PIOC_BASE+0x14)
#define _PIOC_SODR		*(volatile unsigned long *)(_PIOC_BASE+0x30)
#define _PIOC_CODR		*(volatile unsigned long *)(_PIOC_BASE+0x34)
#define _PIOC_PDSR		*(volatile unsigned long *)(_PIOC_BASE+0x3c)
#define _PIOC_ASR		*(volatile unsigned long *)(_PIOC_BASE+0x70)

/* HECC */
#define _HECC_PR   *(volatile unsigned long *)(0xFFFFE80C) /* (HECC)  ECC Parity register */
#define _HECC_MR   *(volatile unsigned long *)(0xFFFFE804) /* (HECC)  ECC Page size register */
#define _HECC_NPR  *(volatile unsigned long *)(0xFFFFE810) /* (HECC)  ECC Parity N register */
#define _HECC_SR   *(volatile unsigned long *)(0xFFFFE808) /* (HECC)  ECC Status register */
#define _HECC_CR   *(volatile unsigned long *)(0xFFFFE800) /* (HECC)  ECC reset register */
#define _HECC_VR   *(volatile unsigned long *)(0xFFFFE8FC) /* (HECC)  ECC Version register */

#define _ECC_RECERR      ((unsigned int) 0x1 <<  0) /* (ECC) ECC error */
#define _ECC_ECCERR      ((unsigned int) 0x1 <<  1) /* (ECC) ECC single error */
#define _ECC_MULERR      ((unsigned int) 0x1 <<  2) /* (ECC) ECC_MULERR */


#define NAND_CE			(1<<14)
#define NAND_RB			(1<<13)

#define NAND_CE_HI		_PIOC_SODR=NAND_CE;
#define NAND_CE_LO		_PIOC_CODR=NAND_CE;

/* command of device */
#define NCMD_READ      0x00
#define NCMD_READ2     0x30

#define NCMD_READID    0x90

#define NCMD_RESET     0xFF

#define NCMD_PAGEPRG   0x80
#define NCMD_PAGEPRG2  0x10

#define NCMD_BLKERASE  0x60
#define NCMD_BLKERASE2 0xD0

#define NCMD_READST    0x70

#define NCMD_RDM_READ1 0x05
#define NCMD_RDM_READ2 0xE0

/*  status bits  */

#define ST_ERROR 0x01
#define ST_READY 0x40
#define ST_WPROT 0x80

#define GOODBLKSIGN  0xc3e4b5cbul

/****************************************************************************
 *
 * NANDwaitrb
 *
 * Wait until RB (ready/busy) signal goes high
 *
 ***************************************************************************/
static void NANDwaitrb()
{
  	DELAY_100NS;
  	DELAY_100NS;
  	DELAY_100NS;
  	DELAY_100NS;
  	DELAY_100NS;
	
#if 1
    while ((_PIOC_PDSR&NAND_RB)==0);
#else
	/* won't work with hw ECC, because of extra datareads */
 	for( ; ; )
  	{
    	int state;

    	NANDCMD = NCMD_READST;
    	state=NANDDATAB;
		
    	if (state & ST_READY)
    	{
      		NANDCMD  = NCMD_READ;
      		return;
    	}
  	}
#endif
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

static int ReadPage(unsigned char *dest,long pagenum, unsigned long *readsign)
{
	int a;
	unsigned long *rp=(unsigned long*)dest;
	char iserased=1;

	NAND_CE_LO;

	NANDCMD  = NCMD_READ;
	NANDADDR = 0;
	NANDADDR = 0;
	NANDADDR = (unsigned char)(pagenum & 0xff);
	NANDADDR = (unsigned char)((pagenum >> 8) & 0xff);
	NANDADDR = (unsigned char)((pagenum >> 16) & 0x1);
	NANDCMD  = NCMD_READ2;

	NANDwaitrb();

	/* get whole page data */
	for (a=0; a<PAGE_SIZE; a+=4)
	{
		register unsigned long data=NANDDATAL;
		
		if (data!=0xffffffffUL) iserased=0;
		
		*rp++=data;
	}

    /* Jump to the ECC code location, skip badblock 32bit entry */
#if 0	
    NANDCMD = NCMD_RDM_READ1; /* Page Random Read Command 1 */
    NANDADDR = (unsigned char)(2048 & 0xff); /* Column address lsb */
    NANDADDR = (unsigned char)((2048 >> 8 ) &0xff); /* Column address msb */
    NANDCMD = NCMD_RDM_READ2; /* Page Random Read Command 2 */
#endif	
	
	{
	    /* get ecc from podition 32bit in spare area */
		unsigned long ecc=NANDDATAL;
	    if (ecc!=0xffffffffUL) iserased=0;
	}

	if (iserased)
	{
	  	NAND_CE_HI;
	  	return -1; /* erased not written */
	}
	
	/* check/correct hwECC */
	{
	   	unsigned char sr=(_HECC_SR) & 0x7;
	
		if (sr & _ECC_MULERR)
		{
		  	/* multiple error cannot be recovered */
			NAND_CE_HI;
			return 1;
		}
		
	   	if (sr & _ECC_RECERR)
	   	{
			if (sr & _ECC_ECCERR)
			{
				/* error in ECC no need to correct anything */
			}
			else
			{
				/* 1 bit error, let's correct it */
				unsigned long reg=_HECC_PR;
				unsigned char bit= (unsigned char)(1 << (reg&0x0f));
	  			unsigned long addr=( reg >> 4)&0xfff;
				dest[addr]^=bit;
		  	}
	   	}
	}

	if (readsign)
	{
	  *readsign=NANDDATAL;
	}
	
    NAND_CE_HI;
	
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
	int a;
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

	
    NAND_CE_LO;

	NANDCMD  = NCMD_PAGEPRG;
	NANDADDR = 0;
	NANDADDR = 0;
	NANDADDR = (unsigned char)(pagenum & 0xff);
	NANDADDR = (unsigned char)((pagenum >> 8) & 0xff);
	NANDADDR = (unsigned char)((pagenum >> 16) & 0x1);

	DELAY_100NS;

	/* write one page data */
	for (a=0; a<PAGE_SIZE; a+=4)
	{
	  	register unsigned long ldata=*rp++;
		NANDDATAL = ldata;
	}

  	/* add spare area */
	{
		unsigned long pr=_HECC_PR;
		unsigned long npr=_HECC_NPR;
	
		/* 32bit hw ECC */
		NANDDATAB = (unsigned char)(pr);
		NANDDATAB = (unsigned char)((pr)>>8);

		NANDDATAB = (unsigned char)(npr);
		NANDDATAB = (unsigned char)((npr)>>8);

		/* add signature data into spare 2nd word */
		NANDDATAL = sdata;
		
		/* this is not a bad block */
		NANDDATAL = GOODBLKSIGN;		
	}

	NANDCMD = NCMD_PAGEPRG2;
	NANDwaitrb();

	NANDCMD = NCMD_READST;
	DELAY_100NS;
	io=NANDDATAB; /* holding status */
    NAND_CE_HI;

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
		ret=ReadPage(page,pagenum,NULL);
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
   			ret=ReadPage(s,pagenum,NULL);
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

		ret=ReadPage(page,pagenum,NULL);
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

 int EraseFlash(long block)
{
	long pagenum=block*PAGEPERBLOCK;
	long io;

/*    fnPr("Erase: %d\n",block);    */

    NAND_CE_LO;
	NANDCMD = NCMD_BLKERASE;
	NANDADDR = (unsigned char)(pagenum & 0xff);
	NANDADDR = (unsigned char)((pagenum >> 8) & 0xff);
	NANDADDR = (unsigned char)((pagenum >> 16) & 0x1);
	NANDCMD  = NCMD_BLKERASE2;
	NANDwaitrb();

	NANDCMD = NCMD_READST;
	DELAY_100NS;
	io=NANDDATAB; /* holding status */
    NAND_CE_HI;

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
        NAND_CE_LO;
   		NANDCMD  = NCMD_READ;
		NANDADDR = 0;
		NANDADDR = 8; /* this bit signal that spare area reading */
		NANDADDR = (unsigned char)(pagenum & 0xff);
		NANDADDR = (unsigned char)((pagenum >> 8) & 0xff);
	    NANDADDR = (unsigned char)((pagenum >> 16) & 0x1);
   		NANDCMD  = NCMD_READ2;

   		NANDwaitrb();

		data=NANDDATAL; /* bb info, ecc value */

		if (data!=0xffffffffUL)
		{
  			data=NANDDATAL; /* signature */
			data=NANDDATAL; /* goodblksignature*/
			NAND_CE_HI;
			
			if (data == GOODBLKSIGN)
			{
			
			  	return 0;
			}
			
		 	return 1; /* signal BAD */
		}
		else
		{
			NAND_CE_HI;
		}
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

    NAND_CE_LO;
	NANDCMD  = NCMD_READ;
	NANDADDR = 4;
	NANDADDR = 8; /* this bit signal that spare area reading */
	NANDADDR = (unsigned char)(pagenum & 0xff);
	NANDADDR = (unsigned char)((pagenum >> 8) & 0xff);
	NANDADDR = (unsigned char)((pagenum >> 16) & 0x1);
	NANDCMD  = NCMD_READ2;

  	NANDwaitrb();

	data=NANDDATAL; /* signature */
	
    NAND_CE_HI;

	/* 1bi ECC is checked on higher layer */
	return data;
}

/****************************************************************************
 *
 * fs_phy_nand_k9f2g08u0m
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

int fs_phy_nand_k9f2g08u0m(FS_FLASH *flash)
{
	long manID, devID;

    _PMC_PCER=_PIOC_ID;					/* enable PC peripheral */
    _EBI_CSA|=(1<<3);					/* enable smartmedia logic on CS3 */
    _PIOC_PDR=(1<<0)|(1<<1);
    _PIOC_ASR=(1<<0)|(1<<1);			/* NANDOE / NANDWE are peripheral A */
    _PIOC_PER=NAND_CE|NAND_RB;
    _PIOC_OER=NAND_CE;
    _PIOC_ODR=NAND_RB;					/* set NAND_CE as output and NAND_RB as input */
	
    _SMC_SETUP(3)=0x00010002;			/* SMC3 setup */
    _SMC_PULSE(3)=0x04040303;			/* SMC3 pulse */
    _SMC_CYCLE(3)=0x00050005;			/* SMC3 cycle */
    _SMC_MODE(3)=0x00000003;			/* SMC3 mode */

	NAND_CE_HI;

	_HECC_CR = 0x01;  /* reset it */
	_HECC_MR = 0x02; /* 2112 bytes per page (2048) */

	page=(unsigned char*)(lpage);
	
	flash->ReadFlash=ReadFlash;         /* read content */
	flash->EraseFlash=EraseFlash;        /* erase a block */
	flash->WriteFlash=WriteFlash;        /* write content */
	flash->VerifyFlash=VerifyFlash;       /* verify content */
	flash->CheckBadBlock=CheckBadBlock;     /* check if block is bad block (NAND) */
	flash->GetBlockSignature=GetBlockSignature; /* get block signature data (NAND) */
	flash->WriteVerifyPage=WriteVerifyPage;

	NAND_CE_LO;
	NANDCMD  = NCMD_READID;
	NANDADDR = 0;
	manID=NANDDATAB;
	devID=NANDDATAB;
	NAND_CE_HI;

	if ((manID==0x00ec) && (devID==0x00da))
	{
   		flash->maxblock=BLOCK_NUM-BLOCK_START;/* 2047 1st block is separated */
   		flash->blocksize=BLOCK_SIZE;          /* 128k	 */
   		flash->sectorsize=SECTOR_SIZE;        /* 8k */
   		flash->sectorperblock=flash->blocksize/flash->sectorsize; /* sector per block */
   		flash->blockstart=BLOCK_START;        /* where 1st block starts */
   		flash->descsize=BLOCK_SIZE;           /* 128K */
   		flash->separatedir=SEPARATE_DIR;      /* fat+dir */

		flash->cachedpagenum=PAGEPERBLOCK;
		flash->cachedpagesize=PAGE_SIZE;

   		return 0; /* ok k9f2g08u0m */
	}

	return 1; /*  unknown type  */
}

/****************************************************************************
 *
 *  end of k9f2g08u0m.c
 *
 ***************************************************************************/

#endif /* _k9f2g08u0m_C_ */

