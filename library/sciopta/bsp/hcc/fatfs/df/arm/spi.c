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

/****************************************************************************
 *
 * spi_init
 *
 * Init SPI ports, directions
 *
 * spics - output
 * spiwp - input
 * spidi - input
 * spiclk - output
 * spicd - input
 * spido - output
 *
 ***************************************************************************/

void spi_init() {

#if (!SPI_MULTI_CS)

   *SPIPORT &= ~ ( MODE(SPICS | SPICLK | SPIDO | SPIDI|SPICD|SPIWP) | DIR(SPIDI|SPICD|SPIWP) );
   *SPIPORT |= DIR(SPICS | SPICLK | SPIDO);

#ifdef SPI_ON_PORT_C
   *SPIPORT |= CSF(SPICS | SPICLK | SPIDO | SPIDI|SPICD|SPIWP); //additional portc settings
#endif

   SPI_CS_HI;
   SPI_CLK_LO;
//   SPI_CLK_HI;
#else
   *SPIPORT &= ~ ( MODE(SPICS | SPICS1 | SPICLK | SPIDO | SPIDI|SPICD|SPIWP) | DIR(SPIDI|SPICD|SPIWP) );
   *SPIPORT |= DIR(SPICS | SPICS1 | SPICLK | SPIDO);

#ifdef SPI_ON_PORT_C
   *SPIPORT |= CSF(SPICS | SPICS1 | SPICLK | SPIDO | SPIDI|SPICD|SPIWP); //additional portc settings
#endif

   SPI_CS0_HI;
   SPI_CS1_HI;
   SPI_CLK_LO;
//   SPI_CLK_HI;
#endif
}


/****************************************************************************
 *
 * spi_tx_8
 *
 * Send  8 bits on spi
 *
 * INPUTS
 *
 * data - send data to spi
 *
 ***************************************************************************/

void spi_tx_8(unsigned char data) {
int a;
int idata=data;

   for (a=0; a<8; a++) {

      if (idata & 0x80) SPI_DATA_HI;
      else SPI_DATA_LO;

      SPI_CLK_HI;

      idata<<=1;

      SPI_CLK_LO;
   }

}


/****************************************************************************
 *
 * spiDataBits
 *
 * Receive 8 bits
 *
 * RETURNS
 *
 * received any bits data
 *
 ***************************************************************************/

unsigned char spi_rx_8() {
int a;
int data=0;

   for (a=0; a<8; a++) {

      SPI_DATA_HI;
      SPI_CLK_HI;

      data<<=1;
      if ( SPI_DATA_IN ) {
         data|=1;
      }

      SPI_CLK_LO;
   }

   return (unsigned char)data;
}
