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

#ifndef _SPI_H
#define _SPI_H

#ifdef __cplusplus
extern "C" {
#endif


#define _MSP430x1xx		/* use MSP430x1xx driver */

#ifdef _MSP430x1xx
#include  <msp430x16x.h>
#elif defined _MSP430x4xx
#include  <msp430x44x.h>
#endif

#if (defined _MSP430x1xx) | (defined _MSP430x4xx)
#ifdef _MSP430x1xx

  /*#define CLK 768*/		/* clock speed in kHz (SMCLK)*/
  #define CLK 1843		/* clock speed in kHz (SMCLK)*/

  /* chip select */
  #define SPIPORT_CS_DIR	P4DIR	
  #define SPIPORT_CS_SEL	P4SEL
  #define SPIPORT_CS		P4OUT
  /*#define SPICS			(1<<0)*/  /*chip select*/
  #define SPICS			(1<<1)  /*chip select*/

  #define SPI_PORT	0	/* use SPI port 0/1 */

#elif defined _MSP430x4xx

  /*#define CLK 32*/		/* clock speed in kHz (ACLK)*/
  #define CLK 1024		/* clock speed in kHz (SMCLK)*/

  /* chip select */
  #define SPIPORT_CS_DIR	P3DIR	
  #define SPIPORT_CS_SEL	P3SEL
  #define SPIPORT_CS		P3OUT
  #define SPICS			(1<<0)  /*chip select*/

  #define SPI_PORT	0	/* use SPI port 0/1 */

#endif


#if SPI_PORT==0
  #define SPI_CTL	U0CTL
  #define SPI_TCTL	U0TCTL
  #define SPI_RCTL	U0RCTL
  #define SPI_BR0	U0BR0
  #define SPI_BR1	U0BR1
  #define SPI_RXBUF	U0RXBUF
  #define SPI_TXBUF	U0TXBUF
  #define SPI_TXEPT_BIT	(1<<0)

  #define SPI_ME	ME1
  #define SPI_ME_BIT	(1<<6)

  #define SPI_IE	IE1

  #define SPI_IFG	IFG1
  #define SPI_IFG_BIT	((1<<7)|(1<<6))

  #define SPI_PORT_SEL	P3SEL
  #define SPI_PORT_DIR	P3DIR
#else
  #define SPI_CTL	U1CTL
  #define SPI_TCTL	U1TCTL
  #define SPI_RCTL	U1RCTL
  #define SPI_BR0	U1BR0
  #define SPI_BR1	U1BR1
  #define SPI_RXBUF	U1RXBUF
  #define SPI_TXBUF	U1TXBUF
  #define SPI_TXEPT_BIT	(1<<0)

  #define SPI_ME	ME2
  #define SPI_ME_BIT	(1<<4)

  #define SPI_IE	IE2

  #define SPI_IFG	IFG2
  #define SPI_IFG_BIT	((1<<5)|(1<<4))

  #define SPI_PORT_SEL	P5SEL
  #define SPI_PORT_DIR	P5DIR
#endif
#endif

#define SPI_CS_LO	SPIPORT_CS &= ~SPICS
#define SPI_CS_HI	SPIPORT_CS |= SPICS

/*#define spi_tx_8(_data8) { SPI_RCTL=0; SPI_TXBUF=_data8; while ((SPI_TCTL&SPI_TXEPT_BIT)==0); while ((SPI_IFG&SPI_IFG_BIT)!=SPI_IFG_BIT); }*/
void spi_tx_8 (unsigned char) ;
unsigned char spi_rx_8 (void);
unsigned long spi_rx_32 (void);
void spi_set_baudrate (unsigned long);
void spi_init (void);


#ifdef __cplusplus
}
#endif

#endif
