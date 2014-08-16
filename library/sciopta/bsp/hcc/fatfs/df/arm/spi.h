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

/****************************************************************************
 *
 * port defintions
 *
 * use one of this definitions for SPI where this is used on PORT_A or PORT_C
 *
 ***************************************************************************/

#define SPI_ON_PORT_A

//#define SPI_ON_PORT_C


/****************************************************************************
 *
 * port defintions
 *
 ***************************************************************************/

#define PORTA ((volatile unsigned long*)0xffb00020)
#define PORTB ((volatile unsigned long*)0xffb00024)
#define PORTC ((volatile unsigned long*)0xffb00028)

#define MODE(x) ((unsigned long)(x)<<24)
#define DIR(x)  ((unsigned long)(x)<<16)
#define CSF(x)  ((unsigned long)(x)<<8)
#define DATA(x) ((unsigned long)(x))

#define PBIT0 0x01UL
#define PBIT1 0x02UL
#define PBIT2 0x04UL
#define PBIT3 0x08UL
#define PBIT4 0x10UL
#define PBIT5 0x20UL
#define PBIT6 0x40UL
#define PBIT7 0x80UL

/****************************************************************************
 *
 * USED SPI port defintions
 *
 ***************************************************************************/

#ifdef SPI_ON_PORT_A
#define SPIPORT PORTA
#endif

#ifdef SPI_ON_PORT_C
#define SPIPORT PORTC
#endif

#define SPICS  PBIT0  //chip select 0
#define SPICS1 PBIT5  //chip select 1
#define SPICD  PBIT1  //card detect
#define SPIWP  PBIT2  //write protect
#define SPIDI  PBIT3  //host data in
#define SPICLK PBIT4  //clock
#define SPIDO  PBIT7  //host data out


/****************************************************************************
 *
 * Control lines definitions
 *
 ***************************************************************************/

#define SPI_MULTI_CS 1 /* set this to 0 if only one chipselect is used */

#if (!SPI_MULTI_CS)

#define SPI_CS_LO   *SPIPORT &= ~DATA(SPICS)
#define SPI_CS_HI   *SPIPORT |= DATA(SPICS)

#else

#define SPI_CS0_LO   *SPIPORT &= ~DATA(SPICS)
#define SPI_CS0_HI   *SPIPORT |= DATA(SPICS)

#define SPI_CS1_LO   *SPIPORT &= ~DATA(SPICS1)
#define SPI_CS1_HI   *SPIPORT |= DATA(SPICS1)

#endif

#define SPI_DATA_LO *SPIPORT &= ~DATA(SPIDO)
#define SPI_DATA_HI *SPIPORT |= DATA(SPIDO)
#define SPI_CLK_LO  *SPIPORT &= ~DATA(SPICLK)
#define SPI_CLK_HI  *SPIPORT |= DATA(SPICLK)

#define SPI_DATA_IN ((*SPIPORT)&DATA(SPIDI))
#define SPI_CD_IN   ((*SPIPORT)&DATA(SPICD))
#define SPI_WP_IN   ((*SPIPORT)&DATA(SPIWP))


#ifdef __cplusplus
}
#endif

#endif

