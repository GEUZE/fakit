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

#ifdef _WINDOWS
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#endif

#include "f_atmel.h"
#include "spi.h"

#ifdef _WINDOWS
#include <assert.h>
#else
#define assert(x) ((void)0)
#endif

/* set _ADF_BLOCK_COPY to 1 if static wear is used */
#define _ADF_BLOCK_COPY 1

/****************************************************************************
 *
 * Driver specific macro definitions
 *
 ***************************************************************************/

/* Check if the management sector selection is valid. */
#if F_ATMEL_MANAGEMENT_SECTOR > ADF_NUM_OF_SECTORS
#error "Invalid management sector selection. Please correct the value of F_ATMEL_MANAGEMENT_SECTOR"
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

#ifdef AT45DB11B    /****************************************************************** 1 Mbit chip */
    /* Chip id from the status register. */
#define ADF_ID		         (0x03u << 2)
    /* Returns the number of the sector that contains the specified page. */
#define PAGE2SECTOR(pg)    (((pg) < 8u) ? 0x0u : (((pg) < 256u) ? 1u : (((pg)>>8)+1u)))
    /* Returns the size of a sector. */
#define SECTORSIZE(sc)     (((sc) < 1u) ? 8u : (((sc) < 2u) ? (256u-8u) : ADF_PAGES_PER_SECTOR))
    /* Returns the first page of the sector. */
#define SECTOR2PAGE(sc)    (((sc) < 1u) ? 0x0u : (((sc) < 2u) ? 8u : (((sc)-1u) << 8)))

#elif defined AT45DB21B     /****************************************************************** 2 Mbit chip */
#define ADF_ID		         (0x05u << 2)
#define PAGE2SECTOR(pg)    (((pg) < 8u) ? 0x0u : (((pg) < 256u) ? 1u : (((pg) < 512u) ? 2u : ((pg)>>9)+2u)))
#define SECTORSIZE(sc)     ((sc) < 1u ? 8u : ((sc) < 2u ? (256u-8u) : (((sc) < 3u) ? 256 : ADF_PAGES_PER_SECTOR)))
#define SECTOR2PAGE(sc)    (((sc) < 1u) ? 0x0u : ((sc) < 2u ? 8u : (((sc) < 3u) ? 256 : ((((sc)-2u) << 9)))))

#elif defined AT45DB41B     /****************************************************************** 4 Mbit chip */
#define ADF_ID		         (0x7u << 2)
#define PAGE2SECTOR(pg)    (((pg) < 8u) ? 0x0u : (((pg) < 256u) ? 1u : (((pg) < 512) ? 2u : ((pg)>>9)+2u)))
#define SECTORSIZE(sc)     ((sc) < 1u ? 8u : ((sc) < 2u ? (256u-8u) : (((sc) < 3u) ? 256 : ADF_PAGES_PER_SECTOR)))
#define SECTOR2PAGE(sc)    (((sc) < 1u) ? 0x0u : ((sc) < 2u ? 8u : (((sc) < 3u) ? 256 : ((((sc)-2u) << 9)))))

#elif defined AT45DB81B     /****************************************************************** 8 Mbit chip */
#define ADF_ID		         (0x9u <<2)
#define PAGE2SECTOR(pg)    (((pg) < 8u) ? 0x0u : (((pg) < 256u) ? 1u : (((pg) < 512u) ? 2u : ((pg)>>9)+2u)))
#define SECTORSIZE(sc)     ((sc) < 1u ? 8u : ((sc) < 2u ? (256u-8u) : (((sc) < 3u) ? 256u : ADF_PAGES_PER_SECTOR)))
#define SECTOR2PAGE(sc)    (((sc) < 1u) ? 0x0u : ((sc) < 2u ? 8u : (((sc) < 3u) ? 256u : ((((sc)-2u) << 9)))))

#elif defined AT45DB161B    /***************************************************************** 16 Mbit chip */
#define ADF_ID		         (0x0b << 2)
#define PAGE2SECTOR(pg)    (((pg) < 8u) ? 0x0u : (((pg) < 256u) ? 1u : (((pg)>>8)+1u)))
#define SECTORSIZE(sc)     ((sc) < 1u ? 8u : ((sc) < 2u ? (256u-8u) : ADF_PAGES_PER_SECTOR))
#define SECTOR2PAGE(sc)    (((sc) < 1u) ? 0x0u : ((sc) < 2u ? 8u : (((sc)-1u) << 8)))

#elif defined AT45DB321B    /***************************************************************** 32 Mbit chip */
#define ADF_ID		         (0xdu <<2)
#define PAGE2SECTOR(pg)    (((pg) < 8u) ? 0x0u : (((pg) < 512u) ? 1u : (((pg)>>9)+1u)))
#define SECTORSIZE(sc)     ((sc) < 1u ? 8u : ((sc) < 2u ? (512u-8u) : ADF_PAGES_PER_SECTOR))
#define SECTOR2PAGE(sc)    (((sc) < 1u) ? 0x0u : ((sc) < 2u ? 8u : (((sc)-1u) << 9)))

#elif defined AT45DB321D    /***************************************************************** 32 Mbit chip */
#define ADF_ID		         (0xdu <<2)
#define PAGE2SECTOR(pg)    (((pg) < 8u) ? 0x0u : (((pg) < 128u) ? 1u : (((pg)>>7)+1u)))
#define SECTORSIZE(sc)     ((sc) < 1u ? 8u : ((sc) < 2u ? (128u-8u) : ADF_PAGES_PER_SECTOR))
#define SECTOR2PAGE(sc)    (((sc) < 1u) ? 0x0u : ((sc) < 2u ? 8u : (((sc)-1u) << 7)))

#elif defined AT45DB642B    /**************************************************************** 64 Mbit chip */
#define ADF_ID		         (0xfu << 2)
#define PAGE2SECTOR(pg)    (((pg) < 8u) ? 0x0u : (((pg) < 256u) ? 1u : (((pg)>>8)+1u)))
#define SECTORSIZE(sc)     ((sc) < 1u ? 8u : ((sc) < 2u ? (256u-8u) : ADF_PAGES_PER_SECTOR))
#define SECTOR2PAGE(sc)    (((sc) < 1u) ? 0x0u : ((sc) < 2u ? 8u : (((sc)-1u) << 8)))
#endif

/* Determine if the driver shall use two bitmap sectors. */
#if F_ATMEL_RESERVE_FROM_SECTOR == 0
#define ADF_BMP_SIZE            ((ADF_REAL_PAGE_COUNT-ADF_MGM_END)/8+(ADF_NUM_OF_SECTORS-(F_ATMEL_MANAGEMENT_SECTOR-1)))
#else
#define ADF_BMP_SIZE            (((SECTOR2PAGE(F_ATMEL_RESERVE_FROM_SECTOR)-ADF_MGM_END))/8+(F_ATMEL_RESERVE_FROM_SECTOR-F_ATMEL_MANAGEMENT_SECTOR-1))
#endif

#define ADF_MGM_START  SECTOR2PAGE(F_ATMEL_MANAGEMENT_SECTOR)
#define ADF_MGM_END    SECTOR2PAGE(F_ATMEL_MANAGEMENT_SECTOR+1)
#define ADF_MGM_LENGTH (ADF_MGM_END-ADF_MGM_START)
/* Turns a page address into a flash address that is understanded by the FLASH chip. */
#define PGA2FLA(p)         (((adf_address_t)(p))<<ADF_BYTE_ADDRESS_WIDTH)

/* READY x_bit in the satus register. */
#define ADF_BREADY		(1u<<7)
    /* Calculates wich x_bit of the map byte contains information about a particular page. */
#define PAGE2BITOFS(pg)    ((pg) & 0x7u)
    /* Most significan x_bit of the page address. */
#define PAGE_ADDRESS_MSB  (((adf_page_address_t)1u)<<((sizeof(adf_page_address_t)<<3)-1u))

    /* This will avaluate to 1 if the bitmap will not fit to one flash page. */
#define ADF_2_MAP_SECTOR (ADF_PAGE_SIZE < ADF_BMP_SIZE)

/* Define the size of the management units. This depends on the size of the bitmap. */
#if ADF_2_MAP_SECTOR
#if F_ATMEL_RESERVE_FROM_SECTOR == 0
#define ADF_LAST_0BMP_SECTOR  (F_ATMEL_MANAGEMENT_SECTOR+(ADF_NUM_OF_SECTORS-F_ATMEL_MANAGEMENT_SECTOR)/2)
#else
#define ADF_LAST_0BMP_SECTOR  (F_ATMEL_MANAGEMENT_SECTOR+(F_ATMEL_RESERVE_FROM_SECTOR-F_ATMEL_MANAGEMENT_SECTOR)/2)
#endif
    /* Calculates wich map byte contains information about a particular page. */
#define PAGE2BYTEOFS(pg) (((pg) < SECTOR2PAGE(ADF_LAST_0BMP_SECTOR+1)) ? \
    (((pg) - ADF_MGM_END) >> 3u) : (((pg)-SECTOR2PAGE(ADF_LAST_0BMP_SECTOR+1u))>>3u))
#define BMPCTROFS(sc) ((sc <= ADF_LAST_0BMP_SECTOR) ? \
    ((ADF_PAGE_SIZE-1)-(sc-(F_ATMEL_MANAGEMENT_SECTOR+1))) : (ADF_PAGE_SIZE-(sc-ADF_LAST_0BMP_SECTOR)))
#define BMPOFS2PAGE(bo, sel) (((sel) == 0) ? (adf_page_address_t)(((bo) << 3)+ADF_MGM_END) :\
    ((adf_page_address_t)((bo) << 3)+SECTOR2PAGE(ADF_LAST_0BMP_SECTOR+1u)))
#define OTHERMGMSEL(ndx)    ((~(ndx)) & 0x1u)

#else
    /* Calculates wich map byte contains information about a particular page. */
#define PAGE2BYTEOFS(pg)   (((pg) - ADF_MGM_END) >> 3u)
#define BMPCTROFS(sc) (ADF_PAGE_SIZE-1-(sc-(F_ATMEL_MANAGEMENT_SECTOR+1)))
#define BMPOFS2PAGE(bo, sel) ((adf_page_address_t)(((bo) << 3)+ADF_MGM_END))
#endif

#if ADF_2_MAP_SECTOR
#define ADF_MGM1_START  ADF_MGM_START
#define ADF_MGM1_END    (ADF_MGM_START+(ADF_MGM_LENGTH>>1))
#define ADF_MGM2_START  ADF_MGM1_END
#define ADF_MGM2_END    ADF_MGM_END
/* Will return the next management page pair.*/
#define ADF_NEXT_MGM_BUFFER(b) \
  (((b) < ADF_MGM2_START) ? ((b) < (ADF_MGM1_END-2u) ? (b)+2u : ADF_MGM1_START) :\
        ((b) < (ADF_MGM2_END-2u) ? (b)+2u : (ADF_MGM2_START)))
#else
/* Will return the next management page pair.*/
#define ADF_NEXT_MGM_BUFFER(b) \
  ((b) < (ADF_MGM_END-2u) ? (b)+2u : ADF_MGM_START)
#endif

/****************************************************************************
 *
 * type definitions
 *
 ***************************************************************************/
typedef unsigned char adf_bool;
typedef unsigned char adf_u8;
typedef unsigned short adf_u16;

typedef unsigned long adf_address_t;
typedef unsigned short adf_page_address_t;
typedef unsigned int adf_byte_address_t;

typedef struct
{
  adf_page_address_t my_addr;
  adf_page_address_t p_addr;
} adf_lock_data_t[2];
/****************************************************************************
 *
 * External dependecies
 *
 ***************************************************************************/
/* none */

/****************************************************************************
 *
 * static variables
 *
 ***************************************************************************/

static adf_u16 adf_update_ctr[ADF_NUM_OF_SECTORS+1];

#if ADF_2_MAP_SECTOR
static adf_page_address_t adf_curr_mgm_pg[2];
static adf_u16 adf_map_lock_seq[2];
static adf_u8 adf_mgm_sel;
#else
static adf_page_address_t adf_curr_mgm_pg[1];
static adf_u16 adf_map_lock_seq[1];
#define adf_mgm_sel 0u
#endif

#if _ADF_BLOCK_COPY
static char blkcpy_buff[F_ATMEL_SECTORSIZE]; /* buffer for block copy */
#endif

/****************************************************************************
 *
 * static function predeclarations
 *
 ***************************************************************************/

static adf_bool adf_recover(void);
static adf_bool adf_transfer_buff_1(const adf_page_address_t);
static adf_bool adf_program_buff_1(adf_page_address_t page);
static adf_bool adf_lock_page(const adf_page_address_t, const adf_page_address_t);
static adf_bool update_bmp(adf_page_address_t);
static void adf_cmd (unsigned char cmd, adf_address_t addr);
static adf_bool adf_wait_ready (void);
static adf_bool adf_init(void);
static unsigned char adf_low_level_format(void);

/****************************************************************************
 *
 * adf_get_block_addr
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

static adf_address_t adf_get_block_addr(long block, long relsector) 
{
	return (adf_address_t)((F_ATMEL_BLOCKSIZE*(unsigned long)block)+((unsigned long)relsector*F_ATMEL_SECTORSIZE));
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
	adf_page_address_t page_addr=(adf_page_address_t)((adf_get_block_addr(block,0)+(unsigned long)blockrel)/ADF_PAGE_SIZE);
	unsigned char *dst=(unsigned char *)data;
	long i;

	assert(page_addr < ADF_MGM_START || page_addr >= ADF_MGM_END);

	if (datalen != 0)
	{
		adf_cmd(ADF_READ_CONT,PGA2FLA(page_addr));
		spi_tx_8(0xff);
		spi_tx_8(0xff);
		spi_tx_8(0xff);
		spi_tx_8(0xff);
		for (i=datalen; i; i--)
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
	unsigned int x,y;
	adf_page_address_t page_addr=(adf_page_address_t)(adf_get_block_addr(block,0)/ADF_PAGE_SIZE);

	assert(page_addr < ADF_MGM_START || page_addr >= ADF_MGM_END);
	for(x=0; x < F_ATMEL_BLOCKSIZE/(8*ADF_PAGE_SIZE); x++)
	{
		adf_cmd(ADF_ERASE_BLOCK, PGA2FLA(page_addr));
		SPI_CS_HI;
		if (adf_wait_ready())
		{
			return(1);
		}
		for(y=0; y<8; y++)
		{
			if (update_bmp(page_addr++))
			{
				return(1);
			}
		}
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
	adf_page_address_t page_addr;
	unsigned long i;
	long write;
	unsigned char *src=(unsigned char *)data;

	page_addr=(adf_page_address_t)((adf_get_block_addr(block,relsector)+(unsigned long)relpos)/ADF_PAGE_SIZE);
	assert(page_addr < ADF_MGM_START || page_addr >= ADF_MGM_END);
	while (size)
	{
		write=ADF_PAGE_SIZE;
		if (write > size) write=size;

		adf_cmd(ADF_WRITE_BUF1, 0);
		for (i=(unsigned long)write;i;i--)
		{
			spi_tx_8(*src++);
		}
		SPI_CS_HI;

		size-=write;
		if (adf_program_buff_1(page_addr++))
		{
			return(1);
		}
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
	adf_page_address_t page_addr;
	long write,i;
	unsigned char *src=(unsigned char *)data;

	page_addr=(adf_page_address_t)((adf_get_block_addr(block,relsector)+(unsigned long)relpos)/ADF_PAGE_SIZE);
	assert(page_addr < ADF_MGM_START || page_addr >= ADF_MGM_END);

	while (size)
	{
		write=ADF_PAGE_SIZE;
		if (write>size) write=size;

		adf_cmd(ADF_READ, PGA2FLA(page_addr));
		spi_tx_8(0xff);
		spi_tx_8(0xff);
		spi_tx_8(0xff);
		spi_tx_8(0xff);
		for (i=write;i;i--)
		{
			unsigned char c;
			c=spi_rx_8();
			if (c!=*src++)
			{
				return(1);
			}
		}
		SPI_CS_HI;

		size-=write;
		++page_addr;
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
int adf_block_copy(long destblk, long soublk)
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
 * f_atmel_flash_fs_phy
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

int f_atmel_flash_fs_phy(FS_FLASH *flash) 
{
	flash->ReadFlash=adf_read_flash;       /*read content*/
	flash->EraseFlash=adf_erase_flash;     /*erase a block */
	flash->WriteFlash=adf_write_flash;    /*write content */
	flash->VerifyFlash=adf_verify_flash;   /*verify content*/

#if	_ADF_BLOCK_COPY
	flash->BlockCopy=adf_block_copy;   /*verify content*/
#endif

    /* Calculate at which block the filesystem starts. */
	flash->blockstart = (ADF_MGM_END*ADF_PAGE_SIZE)+(F_ATMEL_NO_OF_DESC_BLOCKS*F_ATMEL_BLOCKSIZE);
	if ((flash->blockstart % (long)F_ATMEL_BLOCKSIZE) != 0)
	{
		flash->blockstart += (long)F_ATMEL_BLOCKSIZE;
	}
	flash->blockstart /= (long)F_ATMEL_BLOCKSIZE;

    /* Calculate how many blocks may be used by the filesystem. */
	flash->maxblock=ADF_REAL_PAGE_COUNT*ADF_PAGE_SIZE;
    /* First count with pages reserved by the user. */
#if F_ATMEL_RESERVE_FROM_SECTOR != 0
	flash->maxblock -= (long)((ADF_REAL_PAGE_COUNT-SECTOR2PAGE(F_ATMEL_RESERVE_FROM_SECTOR))*ADF_PAGE_SIZE);
#endif

	if ((flash->maxblock % (long)F_ATMEL_BLOCKSIZE) != 0)
	{
		flash->maxblock +=(long)F_ATMEL_BLOCKSIZE;
	}
	flash->maxblock /= (long)F_ATMEL_BLOCKSIZE;
	flash->maxblock -= flash->blockstart;
	flash->maxblock -= (long)F_ATMEL_NO_OF_DESC_BLOCKS;

	flash->blocksize=F_ATMEL_BLOCKSIZE;
	flash->sectorsize=F_ATMEL_SECTORSIZE;
	flash->sectorperblock=F_ATMEL_BLOCKSIZE/F_ATMEL_SECTORSIZE; /*sector per block*/
	flash->descsize=F_ATMEL_BLOCKSIZE;
	flash->cacheddescsize=ADF_PAGE_SIZE*F_ATMEL_CACHED_DESC; /* for cache */
	flash->cachedpagesize=ADF_PAGE_SIZE;

	flash->descblockstart=flash->blockstart-(long)F_ATMEL_NO_OF_DESC_BLOCKS;
	flash->descblockend=flash->blockstart-1;

	return adf_init();
}

/****************************************************************************
 * adf_cmd
 *
 * Will send a command and the address bytes trough the SPI to the flash
 * chip.
 *
 * INPUTS
 *
 * cmd: command byte to be sent
 * addr: address
 *
 * RETURNS
 *
 * none.
 *
 ***************************************************************************/

static void adf_cmd (unsigned char cmd, adf_address_t addr)
{
	SPI_CS_LO;
	spi_tx_8(cmd);
	spi_tx_8((unsigned char)(addr>>16));
	spi_tx_8((unsigned char)((addr>>8)&0xff));
	spi_tx_8((unsigned char)(addr&0xff));
}

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

static adf_bool adf_wait_ready (void)
{
	unsigned char st;
	unsigned int tmout=60000;

	SPI_CS_LO;
	spi_tx_8(ADF_STATUS);
	do 
	{
		tmout--;
		st=spi_rx_8();
	} while (((st & ADF_BREADY)==0) && (tmout != 0));
	SPI_CS_HI;

	return((adf_bool)(tmout == 0));
}

/****************************************************************************
 * adf_init
 *
 * Initialises the flash chip and the SPI.
 *
 * INPUTS
 *
 * none.
 *
 * RETURNS
 *
 * 0: if all ok.
 * 1: if an error is encountered.
 *
 ***************************************************************************/

unsigned char adf_init  (void)
{
	unsigned char ch;
	adf_u8 sector;

	/* Initialize the SPI. */
	spi_init();

	/* Read and check the type of the FLASH chip. */
	SPI_CS_LO;
	spi_tx_8(ADF_STATUS);
	ch=spi_rx_8();
	SPI_CS_HI;

	if ((ch & (0xfu<<2)) != ADF_ID)
	{
		return 1;
	}

	/* If the MCU was reset and the flash chip not, it may be busy. So wait till it
     finishes. */
	if (adf_wait_ready() != 0 )
	{
		return(1);
	}

	/* initialize global variables with default values. */
	/* The first access to any sector will cause a bitmap update. */
	for (sector=0; sector < (sizeof(adf_update_ctr)/sizeof(adf_update_ctr[0])); sector++)
	{
		adf_update_ctr[sector] = (adf_u8) -1;
	}

  /* These will be set by recover too, but anyway we set them to a known value. */
#if ADF_2_MAP_SECTOR
	adf_curr_mgm_pg[1]=ADF_MGM_END/2;
	adf_map_lock_seq[1]=0u;
#endif
	adf_curr_mgm_pg[0]=ADF_MGM_START;
	adf_map_lock_seq[0]=0u;

	ch=adf_recover();
	return(ch);
}

/*******************************************************************************
 *
 * adf_transfer_buff_1
 *
 * This function fill the ram buffer 1 with the contents of the FLASH main area.
 * page_addr: Number of the FLASH page.
 *
 * INPUTS
 *
 * page_addr - page address to be transferred from
 *
 * RETURNS
 *
 * 0: if all ok
 * 1: if timeout passed
 *
 *******************************************************************************/

static adf_bool adf_transfer_buff_1(const adf_page_address_t page_addr)
{
	/* Read page into the buffer. */
	adf_cmd(ADF_READ_MAIN2BUF1, PGA2FLA(page_addr));
	SPI_CS_HI;
	return(adf_wait_ready());
}

/*******************************************************************************
 *
 * adf_transfer_buff_2
 *
 * This function fill the ram buffer 2 with the contents of the FLASH main area.
 * page_addr: Number of the FLASH page.
 *
 * INPUTS
 *
 * page_addr - page address to be transferred from
 *
 * RETURNS
 *
 * 0: if all ok
 * 1: if timeout passed
 *
 *******************************************************************************/

#if ADF_2_MAP_SECTOR
static adf_bool adf_transfer_buff_2(const adf_page_address_t page_addr)
{
	adf_cmd(ADF_READ_MAIN2BUF2, PGA2FLA(page_addr));
	SPI_CS_HI;
	return(adf_wait_ready());
}
#endif

/*******************************************************************************
 *
 * adf_program_buff_2
 *
 * This function programs the contents of ram buffer 2 to the FLASH main area.
 *
 * INPUTS
 *
 * pg - page address program erase
 *
 * RETURNS
 *
 * 0: if all ok
 * 1: if timeout passed
 *
 *******************************************************************************/

static adf_bool adf_program_buff_2(adf_page_address_t pg)
{
	adf_cmd(ADF_PROGERASE_BUF2, PGA2FLA(pg));
	SPI_CS_HI;
	if (0 != adf_wait_ready())
	{
		return(1);
	}
	return(0);
}

/*******************************************************************************
 *
 * adf_program_buff_1
 *
 * This function programs the contents of ram buffer 1 to the FLASH main area.
 *
 * INPUTS
 *
 * pg - page address program erase
 *
 * RETURNS
 *
 * 0: if all ok
 * 1: if timeout passed
 *
 *******************************************************************************/

static adf_bool adf_program_buff_1(adf_page_address_t pg)
{ /* If the contents of the buffer where changed. */
	adf_cmd(ADF_PROGERASE_BUF1, PGA2FLA(pg));
	SPI_CS_HI;
	if (0 != adf_wait_ready())
	{
		return(1);
	}
	/* If this is not a reserved page, update the 10000 bitmap. */
	if ((pg < ADF_MGM_START) ||  (pg >= ADF_MGM_END))
	{
		if (update_bmp(pg) != 0)
		{
			return(1);
		}
	}
	return(0);
}

/*******************************************************************************
 *
 * adf_lock_page
 *
 * This function will generate and program a flash page that locks a page.
 * safe_page_addr: address of the management page where the lock is programed
 * page_sddr: address of the page that is locked.
 *
 * INPUTS
 *
 * safe_page_addr - safe page address 
 * page_addr - page address
 *
 * RETURNS
 *
 * 0: if all ok
 * 1: if timeout passed
 *
 *******************************************************************************/

static adf_bool adf_lock_page(const adf_page_address_t safe_page_addr, const adf_page_address_t page_addr)
{
	unsigned int x;
	unsigned int y;
	adf_lock_data_t lock;

	lock[0].my_addr=safe_page_addr;
	lock[0].p_addr=page_addr;
	lock[1].my_addr=~safe_page_addr;
	lock[1].p_addr=~page_addr;

	adf_cmd(ADF_WRITE_BUF1,0);
	for(y=0; y < 2; y++)
	{
		for(x=0; x < sizeof(lock[y].my_addr); x++)
		{
			spi_tx_8(lock[y].my_addr & 0xff);
			lock[y].my_addr = lock[y].my_addr >> 8;
		}
		for(x=0; x < sizeof(lock[y].p_addr); x++)
		{
			spi_tx_8(lock[y].p_addr & 0xff);
			lock[y].p_addr = lock[y].p_addr >> 8;
		}
	}
	SPI_CS_HI;
	if (adf_program_buff_1(safe_page_addr))
	{
		return(1);
	}
	return(0);
}

/*******************************************************************************
 *
 * adf_unlock_page
 *
 * This function will generate and program a flash page that locks a page.
 * safe_page_addr: address of the management page where the lock is programed
 * page_sddr: address of the page that is locked.
 *
 * INPUTS
 *
 * safe_page_addr - safe page address 
 *
 * RETURNS
 *
 * 0: if all ok
 * 1: if timeout passed
 *
 *******************************************************************************/

static adf_bool adf_unlock_page(const adf_page_address_t safe_page_addr)
{
	adf_cmd(ADF_ERASE_PAGE, PGA2FLA(safe_page_addr));
	SPI_CS_HI;
	return(adf_wait_ready());
}

/*******************************************************************************
 *
 * adf_check_lock
 *
 * This function will generate and program a flash page that locks a page.
 * safe_page_addr: address of the management page where the lock is programed
 * page_sddr: address of the page that is locked.
 *
 * INPUTS
 *
 * safe_page_addr - safe page address 
 * lock - data lock
 *
 * RETURNS
 *
 * 0: if all ok
 * 1: if timeout passed
 *
 *******************************************************************************/
static int adf_check_lock(const adf_page_address_t safe_page_addr, adf_lock_data_t *lock)
{
	unsigned int x;
	unsigned int y;

	adf_cmd(ADF_READ, PGA2FLA(safe_page_addr));
	spi_tx_8(0xff);
	spi_tx_8(0xff);
	spi_tx_8(0xff);
	spi_tx_8(0xff);
	for(y=0; y < 2; y++)
	{
		(*lock)[y].my_addr= 0;
		(*lock)[y].p_addr= 0;
		for(x=0; x < sizeof((*lock)[y].my_addr); x++)
		{
			(*lock)[y].my_addr |= ((adf_page_address_t)spi_rx_8()) << (x<<3);
		}
		for(x=0; x < sizeof((*lock)[y].p_addr); x++)
		{
			(*lock)[y].p_addr |= ((adf_page_address_t)spi_rx_8()) << (x<<3);
		}
	}
	SPI_CS_HI;

	x=0;
	/* Is this a valid lock? */
	if (((*lock)[0].my_addr == (adf_u16)~(*lock)[1].my_addr)
		&& ((*lock)[0].my_addr == safe_page_addr)
		&& ((*lock)[0].p_addr == (adf_u16)~(*lock)[1].p_addr))
	{
		x=1;
	}
	return((int)x);
}

/******************************************************************************
 *
 * find_bmp_page
 *
 * Finds the most recent bitmap page in the specified management sector based
 * on the stored sequence numbers.
 *
 * INPUT
 *
 * start_pg: start address of management area.
 * end_pg:   the next page after the last page of the management area
 * seq: pointer to seguence number variable taht will be used for the mgm area.
 * cpg: pointer to the variable that keeps info about which is the most recent bitmap page.
 *
 * OUTPUT
 *
 * 0: if all ok
 * 1: in case of any error
 *
 ******************************************************************************/

adf_bool find_bmp_page(adf_page_address_t start_pg, adf_page_address_t end_pg, adf_u16 *seq, adf_page_address_t *cpg)
{
	adf_lock_data_t lock;
	adf_page_address_t i;
	adf_u16 bigest_seq=1u<<15;
	adf_u16 curr_seq=(adf_u16)-1;
	adf_page_address_t curr_mgm_pg=0;

	for(i=start_pg; i < end_pg; i+=2)
	{
		curr_seq++;
		if (adf_check_lock(i,&lock))
		{
			if (lock[0].p_addr & PAGE_ADDRESS_MSB)
			{/* If this is the log of a broken transaction,  */
				if (lock[0].p_addr & (PAGE_ADDRESS_MSB >> 1))
				{/* We found a broken safe erase operation. */
					adf_cmd(ADF_ERASE_PAGE, PGA2FLA(lock[0].p_addr & ~(PAGE_ADDRESS_MSB | (PAGE_ADDRESS_MSB>>1))));
				}
				else
				{/* We found a broken safe write operation. */
					/* Resume the broken transaction. */
					if (adf_transfer_buff_1(i+1) != 0)
					{
						return(1);
					}
					adf_cmd(ADF_PROGERASE_BUF1, (adf_address_t)PGA2FLA(lock[0].p_addr & ~PAGE_ADDRESS_MSB));
				}
				SPI_CS_HI;
				if ( 0 != adf_wait_ready())
				{
					return(1);
				}

				/* Log transaction success. */
				if (adf_unlock_page(i))
				{
					return(1);
				}

				/* If we already found a valid bitmap sector, then we found the most recent mgm pagepair. */
				if ((bigest_seq & (1u<<15)) == 0)
				{
					break;
				}
			}
			else
			{ /* This is a lock of a bitmap sector. */
				if (((bigest_seq & (1u<<15)) != 0)) /* This is te first valid mgm page pair. */
				{
					curr_seq=(adf_u16)lock[0].p_addr;
					goto found_goodseq;
				}

				/* If it has the right seq. */
				if ((curr_seq & 0x7fff) == (adf_u16)lock[0].p_addr)
				{ /* then treat this page pair as the most recent one. */
					found_goodseq:
					bigest_seq=(adf_u16)lock[0].p_addr;
					curr_mgm_pg=i;
				}
				else
				{ /* A wrong sequence number means that we already hit the end of the seq. list and thus
					the most recent page pair is already found. */
					break;
				}
			}
		}/* endif: valid lock found.*/
	}

	if ((bigest_seq & 0x8000) == 0)
	{
		*seq=bigest_seq;
		*cpg=curr_mgm_pg;
	}
	else
	{
		if (adf_low_level_format())
		{
			return(1);
		}
	}
	return(0);
}

/*******************************************************************************
 *
 * adf_recover
 *
 * This function will recover any interrupted safe writes.
 *
 * OUTPUT
 *
 * 0: if all ok
 * 1: in case of an error
 *
 *******************************************************************************/

static adf_bool adf_recover(void)
{
  /******************* Find map page and load it into buffer 2. */
#if ADF_2_MAP_SECTOR
	if (find_bmp_page(ADF_MGM1_START, ADF_MGM1_END, &adf_map_lock_seq[0], &adf_curr_mgm_pg[0]))
	{
		return(1);
	}
	if (find_bmp_page(ADF_MGM2_START, ADF_MGM2_END, &adf_map_lock_seq[1], &adf_curr_mgm_pg[1]))
	{
		return(1);
	}
#else
	if (find_bmp_page(ADF_MGM_START, ADF_MGM_END, &adf_map_lock_seq[0]
           , &adf_curr_mgm_pg[0]))
	{
		return(1);
	}
#endif
	return(0);
}

/******************************************************************************
 *
 * inc_mgm_wr_ctr
 *
 * Increases the write counter of the management sector. Will do an update if
 * needed to avoid the 10000 problem.
 *
 * INPUT
 *
 * value: the amount of the increase
 *
 * OUTPUT
 *
 * 0: if all ok
 * 1: in case of an error
 *
 ******************************************************************************/

#if ADF_2_MAP_SECTOR
adf_bool inc_mgm_wr_ctr(adf_u8 value)
{
	adf_update_ctr[ADF_NUM_OF_SECTORS]+=value;
	/* Worst case the counter will be updated after 3 writes. This means that the
		ctr may be less by 3 than the real value. */

	if (adf_update_ctr[ADF_NUM_OF_SECTORS] > (10000 - 3))
	{
		adf_page_address_t next_pg;
		adf_u8 omgm=OTHERMGMSEL(adf_mgm_sel);

		if (adf_transfer_buff_2(adf_curr_mgm_pg[omgm]+1))
		{
			return(1);
		}
		adf_update_ctr[ADF_NUM_OF_SECTORS]=2;

		next_pg=ADF_NEXT_MGM_BUFFER(adf_curr_mgm_pg[omgm]);

		if(adf_program_buff_2(next_pg+1))
		{
			return(1);
		}
		if (adf_lock_page(next_pg, ++adf_map_lock_seq[omgm] & 0x7fff))
		{
			return(1);
		}
		adf_mgm_sel=omgm;
		adf_curr_mgm_pg[adf_mgm_sel]=next_pg;
	}

	return(0);
}
#endif

/******************************************************************************
 *
 * This function writes an mgm page pair to flash.
 *
 * INPUTS
 *
 * mgm_sel - select managment
 *
 * OUTPUT
 *
 * 0: if all ok
 * 1: in case of an error
 *
 ******************************************************************************/

#if ADF_2_MAP_SECTOR
adf_u8 adf_write_mgm(adf_u8 mgm_sel)
{
	adf_page_address_t next_pg;

	next_pg=ADF_NEXT_MGM_BUFFER(adf_curr_mgm_pg[mgm_sel]);
	if(adf_program_buff_2(next_pg+1))
	{
		return(1);
	}
	if (adf_lock_page(next_pg, ++adf_map_lock_seq[mgm_sel] & 0x7fff))
	{
		return(1);
	}
	adf_curr_mgm_pg[mgm_sel] = next_pg;
	if (inc_mgm_wr_ctr(2))
	{
		return(1);
	}
	return(0);
}
#else
adf_u8 adf_write_mgm(void)
{
	adf_page_address_t next_pg;

	next_pg=ADF_NEXT_MGM_BUFFER(adf_curr_mgm_pg[0]);
	if(adf_program_buff_2(next_pg+1))
	{
		return(1);
	}
	if (adf_lock_page(next_pg, ++adf_map_lock_seq[0] & 0x7fff))
	{
		return(1);
	}
	adf_curr_mgm_pg[0] = next_pg;
	return(0);
}
#endif

/******************************************************************************
 * This function will update the "update bitmap" after a write or safe write
 * operation.
 ******************************************************************************/
static adf_bool update_bmp(adf_page_address_t page)
{
	adf_byte_address_t map_byte_offset;
	adf_u8 sector;
#if ADF_2_MAP_SECTOR
	adf_u8 new_mgm_sel;
#endif
	unsigned char bmp_byte;
	sector=PAGE2SECTOR(page);
	assert(sector < ADF_NUM_OF_SECTORS);

    /************** Some calculations */
	map_byte_offset=PAGE2BYTEOFS(page);
	assert(map_byte_offset < ADF_PAGE_SIZE);
#if ADF_2_MAP_SECTOR
	/***** Load right map page into buffer 2. */
	/* Determine which map page do we need. */
	new_mgm_sel= (sector <= ADF_LAST_0BMP_SECTOR) ? 0u : 1u;

	/* if the page in the buffer is not the right one, load the right one. */
	if (adf_mgm_sel != new_mgm_sel)
	{ /* First write the other page out. */
		if (adf_write_mgm(adf_mgm_sel))
		{
			return(1);
		}

		if (adf_transfer_buff_2(adf_curr_mgm_pg[new_mgm_sel]+1))
		{
			return(1);
		}
		adf_update_ctr[ADF_NUM_OF_SECTORS] = 0;
		adf_mgm_sel=new_mgm_sel;
	}
#endif


	/************** Update the flag of this page. */
	/* Load byte from buffer. */
	adf_cmd(ADF_READ_BUF2, map_byte_offset);
	spi_tx_8(0xff);
	bmp_byte=spi_rx_8();
	SPI_CS_HI;
	/* Update bitmap only if the status of the bit would be changed. */
	if (bmp_byte & (1u << PAGE2BITOFS(page)))
	{
		bmp_byte &= ~(1u << PAGE2BITOFS(page));
		/* Write back to buffer. */
		adf_cmd(ADF_WRITE_BUF2, map_byte_offset);
		spi_tx_8(bmp_byte);
		SPI_CS_HI;
	}

	/************** Check if the update of another sector is due */
	/* If we need to update the next page in the sector.*/
	if (adf_update_ctr[sector]++ >= (((10000u/2u)/SECTORSIZE(sector))-1))
	{
		adf_byte_address_t bmp_ndx;
		unsigned char x_bit;
		adf_page_address_t pg_ndx;
		unsigned char x;
		adf_byte_address_t ctr_ofs;

		adf_update_ctr[sector]=1;

		/* Read bitmap index for this sector. */
		ctr_ofs=BMPCTROFS(sector);

#if ADF_2_MAP_SECTOR
		assert(((adf_mgm_sel == 0) && (ctr_ofs >= PAGE2BYTEOFS(SECTOR2PAGE(ADF_LAST_0BMP_SECTOR+1)-1)+1))
			|| ((adf_mgm_sel == 1) && (ctr_ofs >= PAGE2BYTEOFS(ADF_REAL_PAGE_COUNT))));
#else
		assert(ctr_ofs >= PAGE2BYTEOFS(ADF_MGM_END));
#endif
		adf_cmd(ADF_READ_BUF2, ctr_ofs);
		spi_tx_8(0xff);
		bmp_ndx=spi_rx_8();
		SPI_CS_HI;
		assert(bmp_ndx < (ADF_PAGES_PER_SECTOR/8));
		map_byte_offset=PAGE2BYTEOFS(SECTOR2PAGE(sector)) + bmp_ndx;
		/* Read in the bitmap byte for this sector; */
		adf_cmd(ADF_READ_BUF2, map_byte_offset);
		spi_tx_8(0xff);
		bmp_byte=spi_rx_8();
		SPI_CS_HI;
		/* Scan for the first x_bit set to one. Scan starts from LSB. */
		for(x_bit=1, x=0; x_bit; x_bit= x_bit << 1, x++)
		{
			if (x_bit & bmp_byte)
			{
				break;
			}
		}
		/* Calculate to which page the bitmap bit is assigned. */
		pg_ndx = (adf_page_address_t)(BMPOFS2PAGE(map_byte_offset, adf_mgm_sel)+x);
		/**************** First update the bitmap. */
		bmp_byte &= ~x_bit;
		/* If we need to go for the next byte in the bitmap. */
		if(!x_bit || x_bit==0x80)
		{
			/* First clear the current bitmap byte. */
			bmp_byte=0xff;

			/* Write new bitmap index to buffer. */
			adf_cmd(ADF_WRITE_BUF2, ctr_ofs);

			/* If this was the last page of the sector. */
			if (sector != PAGE2SECTOR(pg_ndx+1))
			{
				spi_tx_8(0);
			}
			else
			{
				spi_tx_8((adf_u8)(bmp_ndx+1));
			}
			SPI_CS_HI;
		}

		/* Write updated bitmap byte into the buffer. */
		adf_cmd(ADF_WRITE_BUF2, map_byte_offset);
		spi_tx_8(bmp_byte);
		SPI_CS_HI;

		/* Burn buffer2 into the main flash area.*/
#if ADF_2_MAP_SECTOR
		if (adf_write_mgm(adf_mgm_sel))
#else
		if (adf_write_mgm())
#endif
		{
			return(1);
		}

		/***************** Do the page update. */
		/* If we found a page to be updated. */
		if (x_bit)
		{
			adf_page_address_t safe;
			/* Update with safe write. */
			/* Read page into buffer 1. Note update_bmp may be called during buf1 flush.
				because of this update_bmp need to bee reentrant!!! */
			if (adf_transfer_buff_1(pg_ndx) != 0)
			{
				return(1);
			}

			/* First write data into safe. */
			safe=ADF_NEXT_MGM_BUFFER(adf_curr_mgm_pg[adf_mgm_sel]);

			if (adf_program_buff_1(safe+1) != 0)
			{
				return(1);
			}

			/* Log transatcion start. */
			if (adf_lock_page(safe, pg_ndx | PAGE_ADDRESS_MSB) != 0)
			{
				return(1);
			}
			/* Do the transaction. */
			if (adf_transfer_buff_1(safe+1) != 0)
			{
				return(1);
			}
			adf_cmd(ADF_PROGERASE_BUF1, PGA2FLA(pg_ndx));
			SPI_CS_HI;
			if ( 0 != adf_wait_ready())
			{
				return(1);
			}
			/* Log transaction success. */
			if (adf_unlock_page(safe))
			{
				return(1);
			}
#if ADF_2_MAP_SECTOR
			if (inc_mgm_wr_ctr(3))
			{
				return(1);
			}
#endif
		}
	}

	return(0);
}

/******************************************************************************
 *
 * format_mgm
 *
 * This function will format a management sector (write bitmap pages and locks to it).
 *
 * INPUT
 *
 * start_pg: Starting page of management area.
 * bmp_size: number of bitmap bytes needed.
 * *seq: pointer to seguence number variable taht will be used for the mgm area.
 * *cpg: pointer to the variable that keeps info about which is the most recent bitmap page.
 *
 * OUTPUT
 *
 * 0: if all ok
 * 1: in case of any error
 *
 ******************************************************************************/

adf_bool format_mgm(adf_page_address_t start_pg, adf_u16 bmp_size, adf_u16 *seq, adf_page_address_t *cpg)
{
	/* Create a cleared bitmap sector. */
	*seq=0u;
	adf_cmd(ADF_WRITE_BUF2, 0);
	{
		adf_byte_address_t x;

		for(x=0; x < bmp_size; x++)
		{
			spi_tx_8(0xff);
		}
		for(/*empty*/ ; x<ADF_PAGE_SIZE; x++)
		{
			spi_tx_8(0x0);
		}
	}
	SPI_CS_HI;

	/* Burn the cleared bitmap sector where it is needed. */
	adf_cmd(ADF_PROGERASE_BUF2, PGA2FLA( (adf_page_address_t)(start_pg+1) ));

	SPI_CS_HI;
	if (0 != adf_wait_ready())
	{
		return(1);
	}
	if (adf_lock_page((adf_page_address_t)start_pg,(++*seq) & 0x7fff) != 0)
	{
		return(1);
	}

	if (adf_unlock_page(ADF_NEXT_MGM_BUFFER(start_pg)) != 0)
	{
		return(1);
	}

	*cpg=start_pg;
	adf_update_ctr[ADF_NUM_OF_SECTORS]=2;
	return(0);
}

/******************************************************************************
 *
 * adf_low_level_format
 *
 * This function will write default contents into the management area of the
 * flash chip. (Clean bitmap, etc...)
 *
 * INPUT
 *
 * none
 *
 * OUTPUT
 *
 * none
 *
 ******************************************************************************/

static unsigned char adf_low_level_format(void)
{
#if ADF_2_MAP_SECTOR
	if (format_mgm(ADF_MGM1_START, PAGE2BYTEOFS(SECTOR2PAGE(ADF_LAST_0BMP_SECTOR+1)-1)+1
           , &adf_map_lock_seq[0], &adf_curr_mgm_pg[0]))
	{
		return(1);
	}
	if (format_mgm(ADF_MGM2_START, PAGE2BYTEOFS(ADF_REAL_PAGE_COUNT), &adf_map_lock_seq[1]
           , &adf_curr_mgm_pg[1]))
	{
		return(1);
	}
	adf_mgm_sel=1;
#else
	if (format_mgm(ADF_MGM_START, PAGE2BYTEOFS(ADF_REAL_PAGE_COUNT), &adf_map_lock_seq[0]
           , &adf_curr_mgm_pg[0]))
	{
		return(1);
	}
#endif
	return(0);
}

/****************************************************************************
 *
 *  end of f_atmel.c
 *
 ***************************************************************************/
