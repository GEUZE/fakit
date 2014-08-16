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

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "../../../common/fsf.h"
#include "m25pe40.h"
#include "spi.h"

/****************************************************************************
 *
 * Flash Device specific definitions
 *
 ***************************************************************************/

#define MAXPAGE 2048
#define PAGESIZE 256

#define SECTORSIZE (PAGESIZE*8)   /* 8 pages as one logical sector */
#define BLOCKSIZE (SECTORSIZE*8)  /* 8 sectors per block */
#define MAXBLOCK (MAXPAGE*PAGESIZE/BLOCKSIZE) /* Total block number */

/****************************************************************************
 *
 * Flash CMDs
 *
 ***************************************************************************/

#define CMD_WREN 0x06  /* Write Enable */
#define CMD_RDID 0x9f  /* Read ID */
#define CMD_RDSR 0x05  /* Read Status   */
#define CMD_READ 0x03  /* Read cmd */
#define CMD_PP   0x02  /* Page Program */
#define CMD_PE   0xdb  /* Page Erase   */

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
 *
 * RETURNS
 *
 * relative physical address of block
 *
 ***************************************************************************/

static long GetBlockAddr(long block,long relsector) {
   return BLOCKSIZE*block+relsector*SECTORSIZE;
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
long addr=GetBlockAddr(block,0)+blockrel;
unsigned char *pdata=(unsigned char*)data;

   SPI_CS_LO;

   spi_tx_8(CMD_READ);
   spi_tx_8((unsigned char)(addr>>16));
   spi_tx_8((unsigned char)(addr>>8));
   spi_tx_8((unsigned char)(addr));

   while (datalen--) {
      *pdata++=spi_rx_8();
   }

   SPI_CS_HI;

	return 0;
}

/****************************************************************************
 *
 * EraseFlash
 *
 * Erasing block
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
	long size=BLOCKSIZE;

	while (size) 
	{
		SPI_CS_LO;
		spi_tx_8(CMD_WREN);
		SPI_CS_HI;

		SPI_CS_LO;
		spi_tx_8(CMD_PE);
		spi_tx_8((unsigned char)(addr>>16));
		spi_tx_8((unsigned char)(addr>>8));
		spi_tx_8((unsigned char)(addr));
		SPI_CS_HI;

		for (;;) 
		{
			unsigned char st;
			SPI_CS_LO;

			spi_tx_8(CMD_RDSR);
			st=spi_rx_8();

			SPI_CS_HI;

			if (!(st&1)) break;
		}

		size-=PAGESIZE;
		addr+=PAGESIZE;
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
 * relpos - in NOR flash this is a relative position in the block
 *
 * RETURNS
 *
 * 0 - if ok
 * 1 - if there was any error
 *
 ***************************************************************************/

static int WriteFlash(void *data, long block, long relsector, long size,long relpos) 
{
	long addr=GetBlockAddr(block,relsector)+relpos;
	unsigned char *ptr=(unsigned char*)data;

	while (size>0) 
	{
		long wrsize=size;
		if (wrsize>PAGESIZE) wrsize=PAGESIZE;
		size-=wrsize;

		SPI_CS_LO;
		spi_tx_8(CMD_WREN);
		SPI_CS_HI;

		SPI_CS_LO;
		spi_tx_8(CMD_PP);
		spi_tx_8((unsigned char)(addr>>16));
		spi_tx_8((unsigned char)(addr>>8));
		spi_tx_8((unsigned char)(addr));
		while (wrsize--) 
		{
			spi_tx_8( *ptr++);
		}
		SPI_CS_HI;

		for (;;) 
		{
			unsigned char st;
			SPI_CS_LO;

			spi_tx_8(CMD_RDSR);
			st=spi_rx_8();

			SPI_CS_HI;

			if (!(st&1)) break;
		}

		addr+=PAGESIZE;
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
 * relpos - in NOR flash this is a relative position in the block
 *
 * RETURNS
 *
 * 0 - if ok
 * 1 - if there was any error
 *
 ***************************************************************************/

static int VerifyFlash(void *data, long block, long relsector,long size,long relpos) {
long addr=GetBlockAddr(block,relsector)+relpos;
unsigned char *pdata=(unsigned char*)data;

   SPI_CS_LO;

   spi_tx_8(CMD_READ);
   spi_tx_8((unsigned char)(addr>>16));
   spi_tx_8((unsigned char)(addr>>8));
   spi_tx_8((unsigned char)(addr));

   while (size--) {
      if (*pdata++!=spi_rx_8()) {
         SPI_CS_HI;
         return 1;
      }
   }

   SPI_CS_HI;

	return 0;
}


/****************************************************************************
 *
 * fs_phy_dataflash_m25pe40
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

int fs_phy_dataflash_m25pe40(FS_FLASH *flash) {
unsigned char id,d0,d1;

	flash->ReadFlash=ReadFlash;       /* read content */
	flash->EraseFlash=EraseFlash;     /* erase a block */
	flash->WriteFlash=WriteFlash;     /* write content */
	flash->VerifyFlash=VerifyFlash;   /* verify content */

   spi_init();

   SPI_CS_LO;
   spi_tx_8(CMD_RDID);
   id=spi_rx_8();
   d0=spi_rx_8();
   d1=spi_rx_8();
   SPI_CS_HI;

   if (id==0x20 && d0==0x80 && d1==0x13) {
   	flash->maxblock=MAXBLOCK-2;	    /* maxblock  - desc blocks */
   	flash->blocksize=BLOCKSIZE;
   	flash->sectorsize=SECTORSIZE;
   	flash->sectorperblock=flash->blocksize/flash->sectorsize; /* sector per block */
   	flash->blockstart=2;		/* where 1st block starts */

   	flash->descsize=BLOCKSIZE;
   	flash->cacheddescsize=PAGESIZE*32; /* 1024 is for cache from 8192 */
   	flash->cachedpagesize=PAGESIZE;

   	flash->descblockstart=0;	/* fats start block  */
   	flash->descblockend=1;		/* fats end block (range 1-2, 2fats) */

   	return 0; /* ok m25pe40 from stmicro */
   }


   return 1; /* not identified */
}

/****************************************************************************
 *
 *  end of m25pe40.c
 *
 ***************************************************************************/



