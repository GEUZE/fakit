/****************************************************************************
 *
 *            Copyright (c) 2003-2008 by HCC Embedded
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
#include <ioat91sam7se256.h>

#define CLK 48000000UL		/* clock speed in Hz */

/************************************ SPI routines *****************************************/
/* transmit 1 byte */
void spi_tx1(unsigned char data8)
{
  *AT91C_SPI_TDR=data8;
  while((*AT91C_SPI_SR & AT91C_SPI_TXEMPTY) ==0)
    ;
}

/* transmit 512 bytes */
void spi_tx512 (unsigned char *buf)
{
  unsigned int i;
  for (i=0;i<512;i++)
  {
    spi_tx1(*buf++);
  }
}

/* receive 1 byte */
unsigned char spi_rx1 (void)
{
  *AT91C_SPI_TDR=0xff;
  while((*AT91C_SPI_SR & AT91C_SPI_TXEMPTY) ==0)
    ;
  while((*AT91C_SPI_SR & AT91C_SPI_RDRF) ==0)
    ;
  return(*AT91C_SPI_RDR);
}

/* receive 512 bytes */
void spi_rx512 (unsigned char *buf)
{
  unsigned int i;
  for (i=0;i<512;i++)
  {
    *buf++=spi_rx1();
  }
}

/* set chip select low */
void spi_cs_lo (void)
{
  *AT91C_PIOA_CODR = 1u<<20;
}

/* set chip select high */
void spi_cs_hi (void)
{
  *AT91C_PIOA_SODR = 1<<20;
}


/*
** Init SPI port
*/
int spi_init (void)
{
  /* Configure PIO to enable SPI pins. */
  /* Disable pull ups on SPI pins. */
  /* Disable pull-up on: NPCS0, MISO, MOSI, SCLK. */
  *AT91C_PIOA_PPUDR = 1<<11 | 1<<12 | 1<<13 | 1<<14;
    /* Select SPI function, for SPI pins. */
  *AT91C_PIOA_ASR = 1<<11 | 1<<12 | 1<<13 | 1<<14;
    /* Disable GPIO on SPI pins. */
  *AT91C_PIOA_PDR = 1<<11 | 1<<12| 1<<13 | 1<<14;
  spi_cs_hi();
  /* Set PA20 to output (manual chip select). */
  *AT91C_PIOA_OER = 1<<20;
  /* Configure spi */
    /* enable SPI clock */
  *AT91C_PMC_PCER |= (1u<<AT91C_ID_SPI);

    /* Reset the SPI. */
  *AT91C_SPI_CR = AT91C_SPI_SWRST;
    /* Select master mode, fixed CS to device 0, no CS decode, using MCLK,
       mode fault decetion on, LLB disabled, DLYBCS=6 clk. */
    /* Note: spi has a BUG (see errata). Do not use AT91C_SPI_FDIV=1. */
  *AT91C_SPI_MR = AT91C_SPI_MSTR;
    /* Disable all interrupts.*/
  *AT91C_SPI_IDR = -1UL;
  /* Sample at the starting edge. */
  AT91C_SPI_CSR[0] = AT91C_SPI_NCPHA;
    /* Start *AT91C_*/
  *AT91C_SPI_CR = AT91C_SPI_SPIEN;
    /* Wait till SPI starts. */
  while((*AT91C_SPI_SR & AT91C_SPI_SPIENS) == 0)
   ;

  return(0);
}


/*
** Set baudrate
** INPUT: br - baudrate
*/
void spi_set_baudrate (unsigned long br)
{
  unsigned long val=(CLK+br-1)/br;
  if (val>0xff) val=0xff;
  if (val==0) val=1;
  AT91C_SPI_CSR[0] &= ~AT91C_SPI_SCBR;
  AT91C_SPI_CSR[0] |= AT91C_SPI_SCBR & (val<<8);
}


/*
** Get baudrate
** RETURN: baudrate
*/
unsigned long spi_get_baudrate (void)
{
  unsigned long val = CLK/((AT91C_SPI_CSR[0] & AT91C_SPI_SCBR) >>8);
  return(val);
}

/*
** Get Card Detect state
** RETURN: 0 - card is removed
**         1 - card present
*/
int get_cd (void)
{
  return((*AT91C_PIOA_ODSR & 1<<18)==0);
}

/*
** Get Write Protect state
** RETURN: 0 - not protected
**         1 - write protected
*/
int get_wp (void)
{
  return(*AT91C_PIOA_ODSR & 1<<15);
}

