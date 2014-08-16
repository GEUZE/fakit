#ifndef _SPI_H_
#define _SPI_H_

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

#ifdef __cplusplus
extern "C" {
#endif

extern void spi_init(void);
extern unsigned char spi_rx_8(void);
extern void spi_tx_8(unsigned char data);

extern void spi_cs0_lo(void);
extern void spi_cs0_hi(void);
extern void spi_cs1_lo(void);
extern void spi_cs1_hi(void);

#ifdef _HCC_F_ATMEL_H_
#define SPI_CS_LO   spi_cs0_lo()
#define SPI_CS_HI   spi_cs0_hi()
#endif

#ifdef _HCC_M_ATMEL_H_
#define SPI_CS0_LO   spi_cs0_lo()
#define SPI_CS0_HI   spi_cs0_hi()

#define SPI_CS1_LO   spi_cs1_lo()
#define SPI_CS1_HI   spi_cs1_hi()
#endif

#ifdef __cplusplus
}
#endif

#endif

