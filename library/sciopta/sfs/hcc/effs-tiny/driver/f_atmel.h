/****************************************************************************
 *
 *            Copyright (c) 2005 by HCC Embedded
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
#ifndef _ADF_H_
#define _ADF_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WINDOWS
#include <assert.h>
#else
#define assert(x) ((void)0)
#endif


/*#define AT45DB11B*/     /* 1 Mbit chip */
/*#define AT45DB21B*/     /* 2 Mbit chip */
/*#define AT45DB41B*/     /* 4 Mbit chip */
/*#define AT45DB81B*/     /* 8 Mbit chip */
#define AT45DB161B    /* 16 Mbit chip */
/*#define AT45DB321B*/    /* 32 Mbit chip */
/*#define AT45DB642B*/    /* 64 Mbit chip */
/*#define AT45DB642D_1024*/   /* 64 Mbit chip with pagesize set to 1024 bytes */

/* Reserver sectors at the end of the FLASH area. These will not be managed by the driver, and not used by the
   file system. Note: all sectors before F_ATMEL_MANAGEMENT_SECTOR will be reserved too.
   0 menans no reservation. */
#define F_ATMEL_RESERVE_FROM_SECTOR     0u
/* Sector number that is used for managenent purposes by the driver. The filesystem area will start with the
   block right after this sector. No page before this sector will be used by the driver or the file system. */
#define F_ATMEL_MANAGEMENT_SECTOR         0u

/****************************************************************************
 *
 *  The version number.
 *
 ***************************************************************************/
#define ADF_VERSION   0x0103u

/****************************************************************************
*
*  Device and compiler specific types
*
****************************************************************************/
typedef unsigned long adf_address_t;
typedef unsigned short adf_page_address_t;
typedef unsigned int adf_byte_address_t;
/****************************************************************************
 * Flash chip specific macor definitions. Thera are some more at the
 * beginning of the f_atmel.c file too.
 ***************************************************************************/
#ifdef AT45DB11B     /* 1 Mbit chip */
#define ADF_PAGE_SIZE	            (256u+8u)
#define ADF_REAL_PAGE_COUNT       512u
#define ADF_NUM_OF_SECTORS        3u
#define ADF_PAGES_PER_SECTOR      256u
#define ADF_BYTE_ADDRESS_WIDTH    9u
#define SECTOR2PAGE(sc)           (((sc) < 1u) ? 0x0u : (((sc) < 2u) ? 8u : (((sc)-1u) << 8)))

#elif defined AT45DB21B     /* 2 Mbit chip */
#define ADF_PAGE_SIZE	            (256u+8u)
#define ADF_REAL_PAGE_COUNT       1024
#define ADF_NUM_OF_SECTORS        4u
#define ADF_PAGES_PER_SECTOR      512u
#define ADF_BYTE_ADDRESS_WIDTH    9u
#define SECTOR2PAGE(sc)           (((sc) < 1u) ? 0x0u : ((sc) < 2u ? 8u : (((sc) < 3u) ? 256 : ((((sc)-2u) << 9)))))

#elif defined AT45DB41B     /* 4 Mbit chip */
#define ADF_PAGE_SIZE	            (256u+8u)
#define ADF_REAL_PAGE_COUNT       2048u
#define ADF_NUM_OF_SECTORS        6u
#define ADF_PAGES_PER_SECTOR      512u
#define ADF_BYTE_ADDRESS_WIDTH    9u
#define SECTOR2PAGE(sc)           (((sc) < 1u) ? 0x0u : ((sc) < 2u ? 8u : (((sc) < 3u) ? 256 : ((((sc)-2u) << 9)))))

#elif defined AT45DB81B     /* 8 Mbit chip */
#define ADF_PAGE_SIZE	            (256u+8u)
#define ADF_REAL_PAGE_COUNT       4096u
#define ADF_NUM_OF_SECTORS        10u
#define ADF_PAGES_PER_SECTOR      512u
#define ADF_BYTE_ADDRESS_WIDTH    9u
#define SECTOR2PAGE(sc)           (((sc) < 1u) ? 0x0u : ((sc) < 2u ? 8u : (((sc) < 3u) ? 256u : ((((sc)-2u) << 9)))))

#elif defined AT45DB161B    /* 16 Mbit chip */
#define ADF_PAGE_SIZE	            (512u+16u)
#define ADF_REAL_PAGE_COUNT       4096u
#define ADF_NUM_OF_SECTORS        17u
#define ADF_PAGES_PER_SECTOR      256u
#define ADF_BYTE_ADDRESS_WIDTH    10u
#define SECTOR2PAGE(sc)           (((sc) < 1u) ? 0x0u : ((sc) < 2u ? 8u : (((sc)-1u) << 8)))

#elif defined AT45DB321B    /* 32 Mbit chip */
#define ADF_PAGE_SIZE	            (512u+16u)
#define ADF_REAL_PAGE_COUNT       8192u
#define ADF_NUM_OF_SECTORS        17u
#define ADF_PAGES_PER_SECTOR      512u
#define ADF_BYTE_ADDRESS_WIDTH    10u
#define SECTOR2PAGE(sc)           (((sc) < 1u) ? 0x0u : ((sc) < 2u ? 8u : (((sc)-1u) << 9)))

#elif defined AT45DB642B    /* 64 Mbit chip */
#define ADF_PAGE_SIZE	            (1024u+32u)
#define ADF_REAL_PAGE_COUNT       8192u
#define ADF_NUM_OF_SECTORS        33u
#define ADF_PAGES_PER_SECTOR      256u
#define ADF_BYTE_ADDRESS_WIDTH    11u
#define SECTOR2PAGE(sc)           (((sc) < 1u) ? 0x0u : ((sc) < 2u ? 8u : (((sc)-1u) << 8)))

#elif defined AT45DB642D_1024    /* 64 Mbit chip with pagesize set to 1024 bytes */
#define ADF_PAGE_SIZE	          1024u
#define ADF_REAL_PAGE_COUNT       8192u
#define ADF_NUM_OF_SECTORS        33u
#define ADF_PAGES_PER_SECTOR      256u
#define ADF_BYTE_ADDRESS_WIDTH    10u
#define SECTOR2PAGE(sc)    (((sc) < 1u) ? 0x0u : ((sc) < 2u ? 8u : (((sc)-1u) << 8)))

#endif

#define ADF_MGM_END               SECTOR2PAGE(F_ATMEL_MANAGEMENT_SECTOR+1)
#if F_ATMEL_RESERVE_FROM_SECTOR == 0
  #define ADF_PAGE_COUNT	          (ADF_REAL_PAGE_COUNT-ADF_MGM_END)
#else
  #define ADF_PAGE_COUNT	          (SECTOR2PAGE(F_ATMEL_RESERVE_FROM_SECTOR)-ADF_MGM_END)
#endif
/****************************************************************************
 *
 *  dll functions device specific
 *
 ***************************************************************************/
extern unsigned char adf_read  (void *ramaddr, unsigned long flashaddr, unsigned int size);
extern unsigned char adf_init  (void);
extern unsigned char adf_erase (unsigned long flashaddr, long size);
extern unsigned char adf_erase_safe(adf_address_t flashaddr, unsigned int size);
extern unsigned char adf_write (unsigned long flashaddr,void *ramaddr, unsigned int size);
extern unsigned char adf_write_safe (unsigned long flashaddr,void *ramaddr, unsigned int size);
extern unsigned char adf_copy (unsigned long dstflashaddr, unsigned long srcflashaddr, unsigned int size);
extern unsigned char adf_low_level_format(void);
#ifdef __cplusplus
}
#endif

#endif


