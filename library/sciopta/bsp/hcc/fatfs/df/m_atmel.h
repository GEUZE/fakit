#ifndef _M_ATMEL_H_
#define _M_ATMEL_H_

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
#ifndef _ADF_H_
#define _ADF_H_

#include "..\common\fat.h"

#ifdef __cplusplus
extern "C" {
#endif

#define F_ATMEL_NO_OF_DEVICES 2

#define USE_CRC
#define CRC_ROM_TABLE

/*#define AT45DB11B*/     /* 1 Mbit chip */
#define AT45DB21B     /* 2 Mbit chip */
/*#define AT45DB41B*/     /* 4 Mbit chip */
/*#define AT45DB81B*/     /* 8 Mbit chip */
/*#define AT45DB161B*/    /* 16 Mbit chip */
/*#define AT45DB321B*/    /* 32 Mbit chip */
/*#define AT45DB642B*/    /* 64 Mbit chip */
/* #define AT45DB642D_1024*/   /* 64 Mbit chip with pagesize set to 1024 bytes */

/****************************************************************************
 *
 *  The version number.
 *
 ***************************************************************************/
#define ADF_VERSION   0x0100u

/****************************************************************************
 *
 * Flash Device specific definitions
 *
 ***************************************************************************/
    /* Reserver sectors at the end of the FLASH area. These will not be managed by the driver, and not used by the
       file system. Note: all sectors before F_ATMEL_MANAGEMENT_SECTOR will be reserved too.
       0 menans no reservation. */
#define F_ATMEL_RESERVE_FROM_SECTOR     0u
    /* Sector number that is used for managenent purposes by the driver. The filesystem area will start with the
       block right after this sector. */
#define F_ATMEL_MANAGEMENT_SECTOR       1u
/****************************************************************************
 *
 * Public type definitions
 *
 ***************************************************************************/
typedef unsigned char adf_bool;
typedef unsigned char adf_u8;
typedef unsigned short adf_u16;
typedef unsigned long adf_u32;

typedef unsigned long adf_address_t;
typedef unsigned short adf_page_address_t;
typedef unsigned int adf_byte_address_t;

/****************************************************************************
 * Flash chip specific macro definitions. There are some more at the
 * beginning of the f_atmel.c file too.
 ***************************************************************************/
#ifdef AT45DB11B     /* 1 Mbit chip */
#define ADF_PAGE_SIZE	            (256u+8u)
#define ADF_REAL_PAGE_COUNT       512u
#define ADF_NUM_OF_SECTORS        3u
#define ADF_PAGES_PER_SECTOR      256u
#define ADF_BYTE_ADDRESS_WIDTH    9u

#elif defined AT45DB21B     /* 2 Mbit chip */
#define ADF_PAGE_SIZE	            (256u+8u)
#define ADF_REAL_PAGE_COUNT       1024
#define ADF_NUM_OF_SECTORS        4u
#define ADF_PAGES_PER_SECTOR      512u
#define ADF_BYTE_ADDRESS_WIDTH    9u

#elif defined AT45DB41B     /* 4 Mbit chip */
#define ADF_PAGE_SIZE	            (256u+8u)
#define ADF_REAL_PAGE_COUNT       2048u
#define ADF_NUM_OF_SECTORS        6u
#define ADF_PAGES_PER_SECTOR      512u
#define ADF_BYTE_ADDRESS_WIDTH    9u

#elif defined AT45DB81B     /* 8 Mbit chip */
#define ADF_PAGE_SIZE	            (256u+8u)
#define ADF_REAL_PAGE_COUNT       4096u
#define ADF_NUM_OF_SECTORS        10u
#define ADF_PAGES_PER_SECTOR      512u
#define ADF_BYTE_ADDRESS_WIDTH    9u

#elif defined AT45DB161B    /* 16 Mbit chip */
#define ADF_PAGE_SIZE	            (512u+16u)
#define ADF_REAL_PAGE_COUNT       4096u
#define ADF_NUM_OF_SECTORS        17u
#define ADF_PAGES_PER_SECTOR      256u
#define ADF_BYTE_ADDRESS_WIDTH    10u

#elif defined AT45DB321B    /* 32 Mbit chip */
#define ADF_PAGE_SIZE	            (512u+16u)
#define ADF_REAL_PAGE_COUNT       8192u
#define ADF_NUM_OF_SECTORS        17u
#define ADF_PAGES_PER_SECTOR      512u
#define ADF_BYTE_ADDRESS_WIDTH    10u

#elif defined AT45DB642B    /* 64 Mbit chip */
#define ADF_PAGE_SIZE	            (1024u+32u)
#define ADF_REAL_PAGE_COUNT       8192u
#define ADF_NUM_OF_SECTORS        33u
#define ADF_PAGES_PER_SECTOR      256u
#define ADF_BYTE_ADDRESS_WIDTH    11u

#elif defined AT45DB642D_1024    /* 64 Mbit chip with pagesize set to 1024 bytes */
#define ADF_PAGE_SIZE	            1024u
#define ADF_REAL_PAGE_COUNT       8192u
#define ADF_NUM_OF_SECTORS        33u
#define ADF_PAGES_PER_SECTOR      256u
#define ADF_BYTE_ADDRESS_WIDTH    10u
#endif
/****************************************************************************
* Errors in F_ATMEL
*****************************************************************************/
enum {
/*  0 */  F_ATMEL_NO_ERROR,
/*  1 */  F_ATMEL_ERROR
};
/****************************************************************************
 *
 *  dll functions device specific
 *
 ***************************************************************************/

extern F_DRIVER *f_dfmdrvinit(unsigned long driver_param);


#ifdef __cplusplus
}
#endif

#endif
#endif /*_M_ATMEL_H_ */

