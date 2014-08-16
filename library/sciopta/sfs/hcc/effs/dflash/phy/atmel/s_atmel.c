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

#include "s_atmel.h"
#include "spi.h"

/* set _ADF_BLOCK_COPY to 1 if static wear is used */
#define _ADF_BLOCK_COPY 1

#if _ADF_BLOCK_COPY
static char blkcpy_buff[F_ATMEL_SECTORSIZE]; /* buffer for block copy */
#endif

/* Command opcodes for the flash chip. */
#define ADF_READ_CONT	        0xe8u
#define ADF_READ		          0xd2u
#define ADF_READ_BUF1	        0xd4u
#define ADF_READ_BUF2	        0xd6u
#define ADF_STATUS		        0xd7u
#define ADF_WRITE_BUF1	      0x84u
#define ADF_WRITE_BUF2	      0x87u
#define ADF_PROGERASE_BUF1	  0x83u
#define ADF_PROGERASE_BUF2	  0x86u
#define ADF_PROG_BUF1	        0x88u
#define ADF_PROG_BUF2	        0x89u
#define ADF_ERASE_PAGE	      0x81u
#define ADF_ERASE_BLOCK	      0x50u
#define ADF_READ_MAIN2BUF1	  0x53u
#define ADF_READ_MAIN2BUF2	  0x55u

/* READY x_bit in the satus register. */
#define ADF_BREADY		(1u<<7)

/****************************************************************************
 * adf_wait_ready
 *
 * Wait till the flash chip finiseh all operations (exits busy state).
 *
 * INPUTS
 *
 * none.
 *
 * RETURNS
 *
 * 0: if all ok
 * 1: if timeout passed
 *
 ***************************************************************************/

static char adf_wait_ready (void)
{
	unsigned char st;
	unsigned long tmout=60000;

	SPI_CS_LO;
	spi_tx_8(ADF_STATUS);
	do 
	{
		tmout--;
		if (!tmout) 
		{
			SPI_CS_HI;
			return 1;
		}
		
		st=spi_rx_8();
	} while ((st & ADF_BREADY)==0);
	SPI_CS_HI;

	return 0;
}

/****************************************************************************
 *
 * get_block_addr
 *
 * Get a logical block physical relative address in flash
 *
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

static unsigned long get_block_addr(long block, long relsector) 
{
	return (unsigned long)((F_ATMEL_BLOCKSIZE*(unsigned long)block)+((unsigned long)relsector*F_ATMEL_SECTORSIZE));
}

/****************************************************************************
 *
 * adf_read_flash
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

static int adf_read_flash(void *data, long block, long blockrel, long datalen) 
{
	unsigned long addr=get_block_addr(block,0)+(unsigned long)blockrel;
	unsigned char *dst=(unsigned char *)data;

	addr/=ADF_PAGE_SIZE;
	addr<<=ADF_BYTE_ADDRESS_WIDTH;

	if (datalen)
	{
		SPI_CS_LO;
		spi_tx_8(ADF_READ_CONT);
		spi_tx_8((unsigned char)(addr>>16));
		spi_tx_8((unsigned char)((addr>>8)&0xff));
		spi_tx_8((unsigned char)(addr&0xff));

		spi_tx_8(0xff);
		spi_tx_8(0xff);
		spi_tx_8(0xff);
		spi_tx_8(0xff);

		while (datalen--)
		{
			*dst++=spi_rx_8();
		}

		SPI_CS_HI;
	}

	return 0;
}

/****************************************************************************
 *
 * adf_erase_flash
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

static int adf_erase_flash(long block) 
{
	unsigned long addr=get_block_addr(block,0);
	int num=F_ATMEL_BLOCKSIZE/(8*ADF_PAGE_SIZE);

	addr/=ADF_PAGE_SIZE;
	addr<<=ADF_BYTE_ADDRESS_WIDTH;

	while (num--)
	{
		SPI_CS_LO;
		spi_tx_8(ADF_ERASE_BLOCK);
		spi_tx_8((unsigned char)(addr>>16));
		spi_tx_8((unsigned char)((addr>>8)&0xff));
		spi_tx_8((unsigned char)(addr&0xff));
		SPI_CS_HI;

		if (adf_wait_ready())
		{
			return 1;
		}

		addr+=8 << ADF_BYTE_ADDRESS_WIDTH;
	}

	return 0;
}

/****************************************************************************
 *
 * adf_write_flash
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

static int adf_write_flash(void *data, long block, long relsector, long size,long relpos) 
{
	unsigned long addr=get_block_addr(block,relsector)+(unsigned long)relpos;
	unsigned char *src=(unsigned char *)data;

	addr/=ADF_PAGE_SIZE;
	addr<<=ADF_BYTE_ADDRESS_WIDTH;

	while (size)
	{
		long write=ADF_PAGE_SIZE;
		if (write > size) write=size;

		size-=write;

		SPI_CS_LO;
		spi_tx_8(ADF_WRITE_BUF1);
		spi_tx_8(0);
		spi_tx_8(0);
		spi_tx_8(0);

		while (write--)
		{
			spi_tx_8(*src++);
		}
		SPI_CS_HI;


		SPI_CS_LO;
		spi_tx_8(ADF_PROG_BUF1);
		spi_tx_8((unsigned char)(addr>>16));
		spi_tx_8((unsigned char)((addr>>8)&0xff));
		spi_tx_8((unsigned char)(addr&0xff));
		SPI_CS_HI;

		if (adf_wait_ready())
		{
			return 1;
		}

		addr+=1 << ADF_BYTE_ADDRESS_WIDTH;
	}

	return 0;
}

/****************************************************************************
 *
 * adf_verify_flash
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

static int adf_verify_flash(void *data, long block, long relsector,long size,long relpos) 
{
	unsigned long addr=get_block_addr(block,relsector)+(unsigned long)relpos;
	unsigned char *src=(unsigned char *)data;

	addr/=ADF_PAGE_SIZE;
	addr<<=ADF_BYTE_ADDRESS_WIDTH;

	if (size)
	{
		SPI_CS_LO;
		spi_tx_8(ADF_READ_CONT);
		spi_tx_8((unsigned char)(addr>>16));
		spi_tx_8((unsigned char)((addr>>8)&0xff));
		spi_tx_8((unsigned char)(addr&0xff));

		spi_tx_8(0xff);
		spi_tx_8(0xff);
		spi_tx_8(0xff);
		spi_tx_8(0xff);

		while (size--)
		{
			unsigned char ch0=*src++;
			unsigned char ch1=spi_rx_8();
			if (ch0!=ch1)
			{
				SPI_CS_HI;
				return 1;
			}
		}

		SPI_CS_HI;
	}

	return 0;
}

/****************************************************************************
 *
 * adf_block_copy
 *
 * Copy whole block information to another
 *
 * INPUTS
 *
 * destblk - destination block
 * soublk - source block
 *
 * RETURNS
 *
 * 0 - if ok
 * 1 - if there was any error
 *
 ***************************************************************************/

#if _ADF_BLOCK_COPY
static int adf_block_copy(long destblk, long soublk)
{
	long sector;

/*	fnPr("adf_block_copy: %d <- %d\n",destblk,soublk);*/
	/* erase destination block 1st */
	if (adf_erase_flash(destblk))
	{
		return 1;
	}

	/* loop for whole sector in the block */
	for (sector=0; sector<(long)(F_ATMEL_BLOCKSIZE/F_ATMEL_SECTORSIZE); sector++)
	{
		/* read original sector */
		if (adf_read_flash(blkcpy_buff, soublk, sector*F_ATMEL_SECTORSIZE, F_ATMEL_SECTORSIZE))
		{
			return 1;
		}

		/* write destionation sector */
		if (adf_write_flash(blkcpy_buff,destblk, sector,F_ATMEL_SECTORSIZE,0))
		{
			return 1;
		}
	}

	return 0;
}
#endif

/****************************************************************************
 *
 * s_atmel_flash_fs_phy
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

int s_atmel_flash_fs_phy(FS_FLASH *flash)
{
	unsigned char id;

	/* Initialize the SPI. */
	spi_init();

	/* Read and check the type of the FLASH chip. */
	SPI_CS_LO;
	spi_tx_8(ADF_STATUS);
	id=spi_rx_8();
	SPI_CS_HI;

	/* check ID */
	if ((id & (0x0fu<<2)) != ADF_ID)
	{
		return 1;
	}

	/* If the MCU was reset and the flash chip not, it may be busy. So wait till it
     finishes. */
	if (adf_wait_ready() != 0 )
	{
		return 1;
	}

	flash->ReadFlash=adf_read_flash;       /*read content*/
	flash->EraseFlash=adf_erase_flash;     /*erase a block */
	flash->WriteFlash=adf_write_flash;    /*write content */
	flash->VerifyFlash=adf_verify_flash;   /*verify content*/

#if	_ADF_BLOCK_COPY
	flash->BlockCopy=adf_block_copy;   /*verify content*/
#endif

	flash->maxblock = ADF_REAL_PAGE_COUNT * ADF_PAGE_SIZE / F_ATMEL_BLOCKSIZE;
	flash->maxblock -= F_ATMEL_NO_OF_DESC_BLOCKS;

	flash->blockstart = F_ATMEL_NO_OF_DESC_BLOCKS;

	flash->blocksize=F_ATMEL_BLOCKSIZE;
	flash->sectorsize=F_ATMEL_SECTORSIZE;
	flash->sectorperblock=F_ATMEL_BLOCKSIZE/F_ATMEL_SECTORSIZE; /*sector per block*/
	flash->descsize=F_ATMEL_BLOCKSIZE;
	flash->cacheddescsize=ADF_PAGE_SIZE*F_ATMEL_CACHED_DESC; /* for cache */
	flash->cachedpagesize=ADF_PAGE_SIZE;

	flash->descblockstart=flash->blockstart-(long)F_ATMEL_NO_OF_DESC_BLOCKS;
	flash->descblockend=flash->blockstart-1;

	return 0;
}
