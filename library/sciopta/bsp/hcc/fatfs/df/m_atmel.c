/****************************************************************************
 *
 *            Copyright (c) 2005-2007 by HCC Embedded
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

#include "m_atmel.h"
/****************************************************************************
 *
 * Select serial driver
 *
 ***************************************************************************/

#include "pc/spi.h"
/* #include "arm/spi.h" */
/* #include "msp430/spi.h" */

/****************************************************************************
 *
 * Driver specific macro definitions
 *
 ***************************************************************************/
   /* Some dataflash chips support fast programming (command opcode  93, 96, 98, 99). If your chip supports these
      features set the value of the macro below to one. */
#define F_ATMEL_USE_FAST_WRITE            0u

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
#if F_ATMEL_USE_FAST_WRITE != 0
  #define ADF_PROGERASE_BUF1	  0x93u
  #define ADF_PROGERASE_BUF2	  0x96u
  #define ADF_PROG_BUF1	        0x98u
  #define ADF_PROG_BUF2	        0x99u
#else
  #define ADF_PROGERASE_BUF1	  0x83u
  #define ADF_PROGERASE_BUF2	  0x86u
  #define ADF_PROG_BUF1	        0x88u
  #define ADF_PROG_BUF2	        0x89u
#endif
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

#elif defined AT45DB642B    /**************************************************************** 64 Mbit chip */
#define ADF_ID		         (0xfu << 2)
#define PAGE2SECTOR(pg)    (((pg) < 8u) ? 0x0u : (((pg) < 256u) ? 1u : (((pg)>>8)+1u)))
#define SECTORSIZE(sc)     ((sc) < 1u ? 8u : ((sc) < 2u ? (256u-8u) : ADF_PAGES_PER_SECTOR))
#define SECTOR2PAGE(sc)    (((sc) < 1u) ? 0x0u : ((sc) < 2u ? 8u : (((sc)-1u) << 8)))

#elif defined AT45DB642D_1024 /****************************** 64 Mbit chip with pagesize set to 1024 bytes */
#define ADF_ID		         (0xfu << 2)
#define PAGE2SECTOR(pg)    (((pg) < 8u) ? 0x0u : (((pg) < 256u) ? 1u : (((pg)>>8)+1u)))
#define SECTORSIZE(sc)     ((sc) < 1u ? 8u : ((sc) < 2u ? (256u-8u) : ADF_PAGES_PER_SECTOR))
#define SECTOR2PAGE(sc)    (((sc) < 1u) ? 0x0u : ((sc) < 2u ? 8u : (((sc)-1u) << 8)))
#endif

/* Determine if the driver shall use two bitmap sectors. */
#if F_ATMEL_RESERVE_FROM_SECTOR == 0
#define ADF_BMP_SIZE            ((ADF_REAL_PAGE_COUNT-ADF_MGM_END)/8+(ADF_NUM_OF_SECTORS-(F_ATMEL_MANAGEMENT_SECTOR-1)))
#define ADF_FS_END ADF_REAL_PAGE_COUNT
#else
#define ADF_BMP_SIZE            (((SECTOR2PAGE(F_ATMEL_RESERVE_FROM_SECTOR)-ADF_MGM_END))/8+(F_ATMEL_RESERVE_FROM_SECTOR-F_ATMEL_MANAGEMENT_SECTOR-1))
#define ADF_FS_END ((adf_page_address_t)(SECTOR2PAGE(F_ATMEL_RESERVE_FROM_SECTOR)))
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

#define ADF_FAT_SECTOR_SIZE       512u
#define ADF_CYLINDERS             1u
#define ADF_HEADS                 1u
#if ADF_PAGE_SIZE < ADF_FAT_SECTOR_SIZE
  #define ADF_SECTOR_PER_TRACK      ((ADF_FS_END-ADF_MGM_END)>>1)
  #define ADF_SECTORS               ((ADF_FS_END-ADF_MGM_END)>>1)
  #define ADF_FAT_SECTOR_2_PAGE(sector, pg, bo) ((pg)=((sector)<<1)+ADF_MGM_END)
#elif ADF_PAGE_SIZE < 1024
  #define ADF_SECTOR_PER_TRACK      (ADF_FS_END-ADF_MGM_END)
  #define ADF_SECTORS               (ADF_FS_END-ADF_MGM_END)
  #define ADF_FAT_SECTOR_2_PAGE(sector, pg, bo) ((pg)=(sector)+ADF_MGM_END, (bo)=0)
#else
  #define ADF_SECTOR_PER_TRACK      ((ADF_FS_END-ADF_MGM_END)<<1)
  #define ADF_SECTORS               ((ADF_FS_END-ADF_MGM_END)<<1)
  #define ADF_FAT_SECTOR_2_PAGE(sector, pg, bo) ((pg)=((sector)>>1)+ADF_MGM_END, (bo)=(((sector)&0x1u)<<9))
#endif

/****************************************************************************
 *
 * Type definitions
 *
 ***************************************************************************/
typedef struct
{
  adf_page_address_t my_addr;
  adf_page_address_t p_addr;
} adf_lock_data_t[2];

/****************************************************************************
 *
 * static variables
 *
 ***************************************************************************/

typedef struct {
   adf_u8 cs; /* chip select */
   adf_u16 adf_update_ctr[ADF_NUM_OF_SECTORS+1];

#if ADF_2_MAP_SECTOR
   adf_page_address_t adf_curr_mgm_pg[2];
   adf_u16 adf_map_lock_seq[2];
   adf_u8 adf_mgm_sel;
} ADF_ST;
#else
   adf_page_address_t adf_curr_mgm_pg[1];
   adf_u16 adf_map_lock_seq[1];
   adf_u8 adf_mgm_sel;
} ADF_ST;
/* #define adf_mgm_sel 0u Imi */
#endif

static ADF_ST *adf_var;
static ADF_ST adf_vars[F_ATMEL_NO_OF_DEVICES];

static struct {
  adf_page_address_t page;
  adf_bool changed;
} adf_buf1_info;

static F_DRIVER adf_driver;

/****************************************************************************
 *
 * CS (chipselect) defs
 *
 ***************************************************************************/

#define SPI_CS_HI { \
   switch (adf_var->cs) {    \
   case 0: SPI_CS0_HI; break;\
   case 1: SPI_CS1_HI; break;\
   }\
}


#define SPI_CS_LO { \
   switch (adf_var->cs) {    \
   case 0: SPI_CS0_LO; break;\
   case 1: SPI_CS1_LO; break;\
   }\
}

/****************************************************************************
 *
 * static function predeclarations
 *
 ***************************************************************************/
static adf_bool adf_recover(void);
static adf_bool adf_transfer_buff_1(const adf_page_address_t);
static adf_bool adf_program_buff_1(void);
static adf_bool adf_lock_page(const adf_page_address_t, const adf_page_address_t);
static adf_bool update_bmp(adf_page_address_t);
static void adf_cmd (unsigned char cmd, adf_address_t addr);
static adf_bool adf_wait_ready (void);
static adf_bool adf_init(void);
static unsigned char adf_low_level_format(void);

/****************************************************************************
 *
 * adf_read_sector
 *
 * read data from flash
 *
 * INPUTS
 *
 * driver - driver structure
 * data - where to store data
 * sector - which sector
 *
 * RETURNS
 * 0 - if sucessfully read
 * other - if any error
 *
 ***************************************************************************/

static int adf_read_sector(F_DRIVER *driver,void *data, adf_u32 sector)
{

  adf_page_address_t page_addr;
  adf_byte_address_t byte_ofs;
  adf_byte_address_t i;
  unsigned char *dst=(unsigned char *)data;

  adf_var=&adf_vars[sector/ADF_SECTORS]; /* select chip */
  sector%=ADF_SECTORS;                   /* keep remaining */

  ADF_FAT_SECTOR_2_PAGE(sector, page_addr, byte_ofs);

#if ADF_PAGE_SIZE < ADF_FAT_SECTOR_SIZE
  /* Read the needed bytes from buffer one. */
  byte_ofs=ADF_FAT_SECTOR_SIZE;
  while(1)
  {
    /* Read in the current page into buffer one. */
    if (adf_transfer_buff_1(page_addr) != 0)
    {
      return(F_ATMEL_ERROR);
    }

    i=byte_ofs > ADF_PAGE_SIZE ? ADF_PAGE_SIZE : byte_ofs;
    adf_cmd(ADF_READ_BUF1, 0);
    spi_tx_8(0xff);
    for (; i; i--)
    {
      *dst++=spi_rx_8();
    }
    SPI_CS_HI;
    if (byte_ofs < ADF_PAGE_SIZE)
    {
      break;
    }
    byte_ofs-=ADF_PAGE_SIZE;
    page_addr++;
  }
#else
  /* Read in the current page into buffer one. */
  if (adf_transfer_buff_1(page_addr) != 0)
  {
    return(F_ATMEL_ERROR);
  }

  /* Read the needed bytes from buffer one. */
  adf_cmd(ADF_READ_BUF1, byte_ofs);
  spi_tx_8(0xff);
  for (i=ADF_FAT_SECTOR_SIZE; i; i--)
  {
    *dst++=spi_rx_8();
  }
  SPI_CS_HI;
#endif
  return F_ATMEL_NO_ERROR;
}
/****************************************************************************
 *
 * adf_write_sector
 *
 * Writing (programming) Flash device
 *
 * INPUTS
 *
 * driver - driver structure
 * data - where data is
 * sector - which sector
 *
 * RETURNS
 *
 * 0 - if ok
 * 1 - if there was any error
 *
 ***************************************************************************/

static int adf_write_sector(F_DRIVER *driver, void *data, adf_u32 sector)
{
  adf_page_address_t page_addr;
  adf_byte_address_t byte_ofs;
  adf_byte_address_t i;
  unsigned char *src=(unsigned char *)data;

  adf_var=&adf_vars[sector/ADF_SECTORS]; /* select chip */
  sector%=ADF_SECTORS;                   /* keep remaining */

  ADF_FAT_SECTOR_2_PAGE(sector, page_addr, byte_ofs);

  /* Build new data block in buffer one. */
#if ADF_PAGE_SIZE < ADF_FAT_SECTOR_SIZE
  byte_ofs=ADF_FAT_SECTOR_SIZE;
  /* If buffer does not contains the contents of the page to be modified
      reda the page in. */
  while(1)
  {
    if (adf_buf1_info.page != page_addr)
    {
      if (adf_transfer_buff_1(page_addr) !=0 )
      {
        return(F_ATMEL_ERROR);
      }
    }

    adf_cmd(ADF_WRITE_BUF1, 0);
    i=byte_ofs > ADF_PAGE_SIZE ? ADF_PAGE_SIZE : byte_ofs;
    for (;i;i--)
    {
      spi_tx_8(*src++);
    }
    SPI_CS_HI;
    adf_buf1_info.changed=1;
    if (adf_program_buff_1())
    {
      return(F_ATMEL_ERROR);
    };

    if (byte_ofs <= ADF_PAGE_SIZE)
    {
      break;
    }
    page_addr++;
    byte_ofs-=ADF_PAGE_SIZE;
  }
#else
  /* If buffer does not contains the contents of the page to be modified
      reda the page in. */
  if (adf_buf1_info.page != page_addr)
  {
    if (adf_transfer_buff_1(page_addr) !=0 )
    {
      return(F_ATMEL_ERROR);
    }
  }

  adf_cmd(ADF_WRITE_BUF1, byte_ofs);
  for (i=ADF_FAT_SECTOR_SIZE;i;i--)
  {
    spi_tx_8(*src++);
  }
  SPI_CS_HI;
  adf_buf1_info.changed=1;
  if (adf_program_buff_1())
  {
    return(F_ATMEL_ERROR);
  };
#endif
  return(F_ATMEL_NO_ERROR);
}
/****************************************************************************
 *
 * adf_getphy
 *
 * Identify a flash and fills FS_FLASH structure with data
 *
 * INPUTS
 *
 * driver - driver structure
 * phy - where to store information
 *
 * RETURNS
 *
 * 0 - if successfully
 * other if flash cannot be identified
 *
 ***************************************************************************/

static int adf_getphy(F_DRIVER *driver,F_PHY *phy)
{
	phy->number_of_cylinders = ADF_CYLINDERS;
	phy->sector_per_track = ADF_SECTOR_PER_TRACK * F_ATMEL_NO_OF_DEVICES;
	phy->number_of_heads = ADF_HEADS;
	phy->number_of_sectors = ADF_SECTORS * F_ATMEL_NO_OF_DEVICES;

	return(adf_init());
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
  do {
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

static unsigned char adf_init_sub  (void)
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
     return(F_ATMEL_ERROR);
  }
  /* If the MCU was reset and the flash chip not, it may be busy. So wait till it
     finishes. */
  if (adf_wait_ready() != 0 )
  {
    return(F_ATMEL_ERROR);
  }
  /* initialize global variables with default values. */
  adf_buf1_info.page=(adf_page_address_t)-1;
  adf_buf1_info.changed=0;

  /* The first access to any sector will cause a bitmap update. */
  for (sector=0; sector < (sizeof(adf_var->adf_update_ctr)/sizeof(adf_var->adf_update_ctr[0])); sector++)
  {
    adf_var->adf_update_ctr[sector] = (adf_u8) -1;
  }
  /* These will be set by recover too, but anyway we set them to a known value. */
#if ADF_2_MAP_SECTOR
  /*lint -e{506, 648, 778} */
  adf_var->adf_curr_mgm_pg[1]=ADF_MGM_END/2;
  adf_var->adf_map_lock_seq[1]=0u;
#endif
  /*lint -e{506, 778, 648} */
  adf_var->adf_curr_mgm_pg[0]=ADF_MGM_START;
  adf_var->adf_map_lock_seq[0]=0u;

  ch=adf_recover();
  return(ch);
}

static unsigned char adf_init  (void) {
int a;
   for (a=0; a<F_ATMEL_NO_OF_DEVICES; a++) {
      adf_var=&adf_vars[a];
      adf_var->cs=a;

#ifndef ADF_2_MAP_SECTOR
      adf_var->mgm_sel=0u;
#endif

      if (adf_init_sub()) return 1;
   }
   return 0;
}

/*******************************************************************************
 * This function fill the ram buffer 1 with the contents of the FLASH main area.
 * page_addr: Number of the FLASH page.
 *******************************************************************************/
static adf_bool adf_transfer_buff_1(const adf_page_address_t page_addr)
{
    /* If the buffer contains a different page */
  if (adf_buf1_info.page != page_addr)
  { /* if the buffer conatains pending data */
    if (adf_buf1_info.changed)
    {/* Write it into the main flash area. */
      if (adf_program_buff_1() != 0)
      {
        return(F_ATMEL_ERROR);
      }
    }
    /* Read page into the buffer. */
    adf_cmd(ADF_READ_MAIN2BUF1, PGA2FLA(page_addr));
    SPI_CS_HI;
    /* Remember which page is in the buffer. Note: adf_buf1_info.changed will be cleared
      by ...program_buf_1() above. */
    adf_buf1_info.page=page_addr;
    return(adf_wait_ready());
  }
  return(F_ATMEL_NO_ERROR);
}
/*******************************************************************************
 * This function fill the ram buffer 2 with the contents of the FLASH main area.
 * page_addr: Number of the FLASH page.
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
 * This function programs the contents of ram buffer 2 to the FLASH main area.
 *******************************************************************************/
static adf_bool adf_program_buff_2(adf_page_address_t pg)
{
  adf_cmd(ADF_PROGERASE_BUF2, PGA2FLA(pg));
  SPI_CS_HI;
  return(adf_wait_ready());
}
/*******************************************************************************
 * This function programs the contents of ram buffer 1 to the FLASH main area.
 *******************************************************************************/
static adf_bool adf_program_buff_1()
{ /* If the contents of the buffer where changed. */
  if (adf_buf1_info.changed)
  {
    adf_buf1_info.changed=0;
    adf_cmd(ADF_PROGERASE_BUF1, PGA2FLA(adf_buf1_info.page));
    SPI_CS_HI;
    if (0 != adf_wait_ready())
    {
      return(F_ATMEL_ERROR);
    }
    /* If this is not a reserved page, update the 10000 bitmap. */
    /*lint -e{506} */
    if (adf_buf1_info.page < ADF_MGM_START || adf_buf1_info.page >= ADF_MGM_END)
    {
      if (update_bmp(adf_buf1_info.page) != 0)
      {
        return(F_ATMEL_ERROR);
      }
    }
  }
  return(F_ATMEL_NO_ERROR);
}

/*******************************************************************************
 * This function will generate and program a flash page that locks a page.
 * safe_page_addr: address of the management page where the lock is programed
 * page_sddr: address of the page that is locked.
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
      spi_tx_8((adf_u8)lock[y].p_addr);
      lock[y].p_addr = lock[y].p_addr >> 8;
    }
  }
  SPI_CS_HI;
  adf_buf1_info.page=safe_page_addr;
  adf_buf1_info.changed=1;
  if (adf_program_buff_1())
  {
    return(F_ATMEL_ERROR);
  }
  return(F_ATMEL_NO_ERROR);
}
/*******************************************************************************
 * This function will generate and program a flash page that locks a page.
 * safe_page_addr: address of the management page where the lock is programed
 * page_sddr: address of the page that is locked.
 *******************************************************************************/
static adf_bool adf_unlock_page(const adf_page_address_t safe_page_addr)
{
  adf_cmd(ADF_ERASE_PAGE, PGA2FLA(safe_page_addr));
  SPI_CS_HI;
  return(adf_wait_ready());
}
/*******************************************************************************
 * This function will generate and program a flash page that locks a page.
 * safe_page_addr: address of the management page where the lock is programed
 * page_sddr: address of the page that is locked.
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
static adf_bool find_bmp_page(adf_page_address_t start_pg, adf_page_address_t end_pg, adf_u16 *seq, adf_page_address_t *cpg)
{
  adf_lock_data_t lock;
  adf_page_address_t i;
  adf_u16 bigest_seq=1u<<15;
  adf_u16 curr_seq=(adf_u16)-1;
  adf_page_address_t curr_mgm_pg=0;

  for(i=start_pg; i < end_pg; i+=2)
  {
    curr_seq++;
    /*lint -e{545} */
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
            return(F_ATMEL_ERROR);
          }
          adf_cmd(ADF_PROGERASE_BUF1, (adf_address_t)PGA2FLA(lock[0].p_addr & ~PAGE_ADDRESS_MSB));
        }
        SPI_CS_HI;
        if ( 0 != adf_wait_ready())
        {
          return(F_ATMEL_ERROR);
        }
        /* Log transaction success. */
        if (adf_unlock_page(i))
        {
          return(F_ATMEL_ERROR);
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
      return(F_ATMEL_ERROR);
    }
  }
  return(F_ATMEL_NO_ERROR);
}
/*******************************************************************************
 * This function will recover any interrupted safe writes.
 *******************************************************************************/
static adf_bool adf_recover(void)
{
  /******************* Find map page and load it into buffer 2. */
#if ADF_2_MAP_SECTOR
  /*lint -e{506, 648, 778 } */
  if (find_bmp_page(ADF_MGM1_START, ADF_MGM1_END, &adf_var->adf_map_lock_seq[0], &adf_var->adf_curr_mgm_pg[0]))
  {
    return(F_ATMEL_ERROR);
  }
  /*lint -e{506, 648, 778 } */
  if (find_bmp_page(ADF_MGM2_START, ADF_MGM2_END, &adf_var->adf_map_lock_seq[1], &adf_var->adf_curr_mgm_pg[1]))
  {
    return(F_ATMEL_ERROR);
  }
#else
  /*lint -e{506, 648, 778 } */
  if (find_bmp_page(ADF_MGM_START, ADF_MGM_END, &adf_var->adf_map_lock_seq[0]
           , &adf_var->adf_curr_mgm_pg[0]))
  {
    return(F_ATMEL_ERROR);
  }
#endif
  return(F_ATMEL_NO_ERROR);
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
static adf_bool inc_mgm_wr_ctr(adf_u8 value)
{
  adf_var->adf_update_ctr[ADF_NUM_OF_SECTORS]+=value;
  /* Worst case the counter will be updated after 3 writes. This means that the
    ctr may be less by 3 than the real value. */

  if (adf_var->adf_update_ctr[ADF_NUM_OF_SECTORS] > (10000 - 3))
  {
    adf_page_address_t next_pg;
    adf_u8 omgm=OTHERMGMSEL(adf_var->adf_mgm_sel);

    if (adf_transfer_buff_2(adf_var->adf_curr_mgm_pg[omgm]+1))
    {
      return(F_ATMEL_ERROR);
    }
    adf_var->adf_update_ctr[ADF_NUM_OF_SECTORS]=2;

    /*lint -e{506, 648, 778 } */
    next_pg=ADF_NEXT_MGM_BUFFER(adf_var->adf_curr_mgm_pg[omgm]);

    if(adf_program_buff_2(next_pg+1))
    {
      return(F_ATMEL_ERROR);
    }
    if (adf_lock_page(next_pg, ++adf_var->adf_map_lock_seq[omgm] & 0x7fff))
    {
      return(F_ATMEL_ERROR);
    }
    adf_var->adf_mgm_sel=omgm;
    adf_var->adf_curr_mgm_pg[adf_var->adf_mgm_sel]=next_pg;
  }

  return(F_ATMEL_NO_ERROR);
}
#endif
/******************************************************************************
 * This function writes an mgm page pair to flash.
 ******************************************************************************/
#if ADF_2_MAP_SECTOR
static adf_u8 adf_write_mgm(adf_u8 mgm_sel)
{
  adf_page_address_t next_pg;

  /*lint -e{506, 648, 778 } */
  next_pg=ADF_NEXT_MGM_BUFFER(adf_var->adf_curr_mgm_pg[mgm_sel]);
  if(adf_program_buff_2(next_pg+1))
  {
    return(F_ATMEL_ERROR);
  }
  if (adf_lock_page(next_pg, ++adf_var->adf_map_lock_seq[mgm_sel] & 0x7fff))
  {
    return(F_ATMEL_ERROR);
  }
  adf_var->adf_curr_mgm_pg[mgm_sel] = next_pg;
  if (inc_mgm_wr_ctr(2))
  {
    return(F_ATMEL_ERROR);
  }
  return(F_ATMEL_NO_ERROR);
}
#else
static adf_u8 adf_write_mgm(void)
{
  adf_page_address_t next_pg;

  /*lint -e{506, 648, 778 } */
  next_pg=ADF_NEXT_MGM_BUFFER(adf_var->adf_curr_mgm_pg[0]);
  if(adf_program_buff_2(next_pg+1))
  {
    return(F_ATMEL_ERROR);
  }
  if (adf_lock_page(next_pg, ++adf_var->adf_map_lock_seq[0] & 0x7fff))
  {
    return(F_ATMEL_ERROR);
  }
  adf_var->adf_curr_mgm_pg[0] = next_pg;
  return(F_ATMEL_NO_ERROR);
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
    /************** Some calculations */
  /*lint -e{506, 778} */
  map_byte_offset=PAGE2BYTEOFS(page);
#if ADF_2_MAP_SECTOR
  /***** Load right map page into buffer 2. */
  /* Determine which map page do we need. */
  new_mgm_sel= (sector <= ADF_LAST_0BMP_SECTOR) ? 0u : 1u;

  /* if the page in the buffer is not the right one, load the right one. */
  if (adf_var->adf_mgm_sel != new_mgm_sel)
  { /* First write the other page out. */
    if (adf_write_mgm(adf_var->adf_mgm_sel))
    {
      return(F_ATMEL_ERROR);
    }

    if (adf_transfer_buff_2(adf_var->adf_curr_mgm_pg[new_mgm_sel]+1))
    {
      return(F_ATMEL_ERROR);
    }
    adf_var->adf_update_ctr[ADF_NUM_OF_SECTORS] = 0;
    adf_var->adf_mgm_sel=new_mgm_sel;
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
  if (adf_var->adf_update_ctr[sector]++ >= (((10000u/2u)/SECTORSIZE(sector))-1))
  {
    adf_byte_address_t bmp_ndx;
    unsigned char x_bit;
    adf_page_address_t pg_ndx;
		unsigned char x;
    adf_byte_address_t ctr_ofs;

		adf_var->adf_update_ctr[sector]=1;
    /* Read bitmap index for this sector. */
    ctr_ofs=BMPCTROFS(sector);
    adf_cmd(ADF_READ_BUF2, ctr_ofs);
    spi_tx_8(0xff);
    bmp_ndx=spi_rx_8();
    SPI_CS_HI;
    /*lint -e{506, 778 } */
		map_byte_offset=PAGE2BYTEOFS(SECTOR2PAGE(sector)) + bmp_ndx;
    /* Read in the bitmap byte for this sector; */
    adf_cmd(ADF_READ_BUF2, map_byte_offset);
    spi_tx_8(0xff);
    bmp_byte=spi_rx_8();
    SPI_CS_HI;
    /* Scan for the first x_bit set to one. Scan starts from LSB. */
    /*lint -e{734} loss of precision is ok here */
    for(x_bit=1, x=0; x_bit; x_bit= x_bit << 1, x++)
    {
      if (x_bit & bmp_byte)
      {
        break;
      }
    }
    /* Calculate to which page the bitmap bit is assigned. */
    /*lint -e{506, 778 } */
    pg_ndx = (adf_page_address_t)(BMPOFS2PAGE(map_byte_offset, adf_var->adf_mgm_sel)+x);
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
    if (adf_write_mgm(adf_var->adf_mgm_sel))
#else
    if (adf_write_mgm())
#endif
    {
      return(F_ATMEL_ERROR);
    }
    /***************** Do the page update. */
    /* If we found a page to be updated. */
    if (x_bit)
    {
      adf_page_address_t safe;

      /* Update with safe write. */
      /* Read page into buffer 1. */
      if (adf_transfer_buff_1(pg_ndx) != 0)
      {
        return(F_ATMEL_ERROR);
      }
      safe=ADF_NEXT_MGM_BUFFER(adf_var->adf_curr_mgm_pg[adf_var->adf_mgm_sel]);
      /* First write data into safe. */
      adf_buf1_info.page=safe+1;
      adf_buf1_info.changed=1;
      if (adf_program_buff_1() != 0)
      {
        return(F_ATMEL_ERROR);
      }
      /* Log transatcion start. */
      if (adf_lock_page(safe, pg_ndx | PAGE_ADDRESS_MSB) != 0)
      {
        return(F_ATMEL_ERROR);
      }
      /* Do the transaction. */
      if (adf_transfer_buff_1(safe+1) != 0)
      {
        return(F_ATMEL_ERROR);
      }

      adf_buf1_info.page=pg_ndx;
      adf_buf1_info.changed=1;
      if (adf_program_buff_1() != 0)
      {
        return(F_ATMEL_ERROR);
      }
      /* Log transaction success. */
      if (adf_unlock_page(safe))
      {
        return(F_ATMEL_ERROR);
      }
#if ADF_2_MAP_SECTOR
      if (inc_mgm_wr_ctr(3))
      {
        return(F_ATMEL_ERROR);
      }
#endif
    }
  }
  return(F_ATMEL_NO_ERROR);
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
static adf_bool format_mgm(adf_page_address_t start_pg, adf_u16 bmp_size, adf_u16 *seq, adf_page_address_t *cpg)
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
  /*lint -e{506, 778} */
  adf_cmd(ADF_PROGERASE_BUF2, PGA2FLA(start_pg+1));
  SPI_CS_HI;
  if (0 != adf_wait_ready())
  {
    return(F_ATMEL_ERROR);
  }
  if (adf_lock_page((adf_page_address_t)start_pg,(++*seq) & 0x7fff) != 0)
  {
    return(F_ATMEL_ERROR);
  }
  /*lint -e{506, 648, 778 } */
  if (adf_unlock_page(ADF_NEXT_MGM_BUFFER(start_pg)) != 0)
  {
    return(F_ATMEL_ERROR);
  }

  *cpg=start_pg;
  adf_var->adf_update_ctr[ADF_NUM_OF_SECTORS]=2;
  return(F_ATMEL_NO_ERROR);
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
  /*lint -e{506, 648, 778 } */
  if (format_mgm(ADF_MGM1_START, PAGE2BYTEOFS(SECTOR2PAGE(ADF_LAST_0BMP_SECTOR+1)-1)+1
           , &adf_var->adf_map_lock_seq[0], &adf_var->adf_curr_mgm_pg[0]))
  {
    return(F_ATMEL_ERROR);
  }
  /*lint -e{506, 648, 778 } */
  if (format_mgm(ADF_MGM2_START, PAGE2BYTEOFS(ADF_REAL_PAGE_COUNT), &adf_var->adf_map_lock_seq[1]
           , &adf_var->adf_curr_mgm_pg[1]))
  {
    return(F_ATMEL_ERROR);
  }
  adf_var->adf_mgm_sel=1;
#else
  /*lint -e{506, 648, 778 } */
  if (format_mgm(ADF_MGM_START, PAGE2BYTEOFS(ADF_REAL_PAGE_COUNT), &adf_var->adf_map_lock_seq[0]
           , &adf_var->adf_curr_mgm_pg[0]))
  {
    return(F_ATMEL_ERROR);
  }
#endif
  return(F_ATMEL_NO_ERROR);
}

/******************************************************************************
 *
 * adf_getstatus
 *
 * Get status of card, missing or/and removed,changed,writeprotect
 *
 * INPUT
 *
 * driver - driver structure
 *
 * OUTPUT
 *
 * always 0
 *
 ******************************************************************************/

static long adf_getstatus (F_DRIVER *driver)
{
	return 0;
}


/****************************************************************************
 *
 * f_dfmdrvinit
 *
 * this init function has to be passed for highlevel to initiate the
 * driver functions
 *
 * INPUTS
 *
 * driver_param - driver parameter
 *
 * RETURNS
 *
 * driver structure
 *
 ***************************************************************************/

F_DRIVER *f_dfmdrvinit(unsigned long driver_param)
{
	(void)memset (&adf_driver,0,sizeof(adf_driver));

	adf_driver.readsector=adf_read_sector;
	adf_driver.writesector=adf_write_sector;
	adf_driver.getphy=adf_getphy;
	adf_driver.getstatus=adf_getstatus;

	if (adf_init()) return 0;

	return &adf_driver;
}

/****************************************************************************
 *
 *  end of m_atmel.c
 *
 ***************************************************************************/

