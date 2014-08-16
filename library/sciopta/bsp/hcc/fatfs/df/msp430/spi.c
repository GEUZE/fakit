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
#include "spi.h"

/*
** Sends 8 bits through SPI
*/
void spi_tx_8 (unsigned char _data8)
{
  SPI_RCTL=0;
  SPI_TXBUF=_data8;
  while ((SPI_TCTL&SPI_TXEPT_BIT)==0);
  while ((SPI_IFG&SPI_IFG_BIT)!=SPI_IFG_BIT);
}


/*
** Gets 1 byte on the SPI
**
** Output: received byte
*/
unsigned char spi_rx_8 (void)
{
  SPI_RXBUF;
  spi_tx_8(0xff);
  return SPI_RXBUF;
}


/*
** Gets 4 bytes on the SPI
**
** Output: 4 bytes received
*/
unsigned long spi_rx_32 (void)
{
  unsigned long r;

  SPI_RXBUF;
  spi_tx_8(0xff);
  r=SPI_RXBUF;

  r<<=8;
  spi_tx_8(0xff);
  r|=SPI_RXBUF;

  r<<=8;
  spi_tx_8(0xff);
  r|=SPI_RXBUF;

  r<<=8;
  spi_tx_8(0xff);
  r|=SPI_RXBUF;

  return r;
}


/*
** Set baud rate.
**
** Input: required speed in kHz.
*/
void spi_set_baudrate (unsigned long khz)
{
  unsigned long div=CLK/khz;

  if (div<2) div=2;
  if (div&0xffff0000) div=0xffff;

  SPI_CTL|=1;
  SPI_BR0=div&0xff;
  SPI_BR1=div>>8;
  SPI_CTL&=~1;
}


/*
** initializes SPI on the device
*/
void spi_init (void)
{
  SPIPORT_CS_SEL&=~SPICS;
  SPIPORT_CS_DIR|=SPICS;
  SPI_CS_HI;

  SPI_PORT_SEL|=0x0e;	/* select MOSI, MISO and CLK on port */
  SPI_PORT_DIR|=0x0a;	/* set MOSI and CLK to be output */
  SPI_PORT_DIR&=~0x04;	/* set MISO to be input */

  SPI_CTL=0x17;
  SPI_TCTL=0xb2;
  SPI_RCTL=0x0;
  SPI_ME=SPI_ME_BIT;
  SPI_IE=0x0;
  spi_set_baudrate(20000);
}

