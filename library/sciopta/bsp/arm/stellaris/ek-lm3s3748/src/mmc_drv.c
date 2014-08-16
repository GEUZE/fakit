/*SOC
**********************************************************************
**  _______  _______ _________ _______  _______ _________ _______   **
** (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
** | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
** | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
** (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
**       ) || |         | |   | |   | || (         | |   | (   ) |  **
** /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
** \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
**                                                                  **
**              (c) 2009 SCIOPTA Systems AG/ Schweiz                **
**                                                                  **
**********************************************************************
** ID: S09322BS2                                                    **
** +Revision: 1.1.2.1 +                                                 **
** +Date: 2009/11/18 09:38:45 +                                     **
** Low-level MMC driver (direct)                                    **
**********************************************************************
EOC*/
#include <ossys/types.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"




/* Peripheral definitions for EK-LM3S3748 board */
// SSI port
#define SDC_SSI_BASE            SSI0_BASE
#define SDC_SSI_SYSCTL_PERIPH   SYSCTL_PERIPH_SSI0

// GPIO for SSI pins
#define SDC_GPIO_PORT_BASE      GPIO_PORTA_BASE
#define SDC_GPIO_SYSCTL_PERIPH  SYSCTL_PERIPH_GPIOA
#define SDC_SSI_CLK             GPIO_PIN_2
#define SDC_SSI_TX              GPIO_PIN_5
#define SDC_SSI_RX              GPIO_PIN_4
#define SDC_SSI_FSS             GPIO_PIN_3
#define SDC_SSI_PINS            (SDC_SSI_TX | SDC_SSI_RX | SDC_SSI_CLK | SDC_SSI_FSS)


/* transmit 1 byte */
void spi_tx1(unsigned char data8)
{
  __u32 rcvdat;
  
  ROM_SSIDataPut(SDC_SSI_BASE, data8); /* Write the data to the tx fifo */
  
  ROM_SSIDataGet(SDC_SSI_BASE, &rcvdat); /* flush data read during the write */
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
  __u32 rcvdat;
  
  ROM_SSIDataPut(SDC_SSI_BASE, 0xFF); /* write dummy data */
  
  ROM_SSIDataGet(SDC_SSI_BASE, &rcvdat); /* read data frm rx fifo */
  
  return (unsigned char)rcvdat;
  
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
  ROM_GPIOPinWrite(SDC_GPIO_PORT_BASE, SDC_SSI_FSS, 0);
}

/* set chip select high */
void spi_cs_hi (void)
{
  ROM_GPIOPinWrite(SDC_GPIO_PORT_BASE, SDC_SSI_FSS, SDC_SSI_FSS);
}


/*
** Init SPI port
*/
/*-----------------------------------------------------------------------*/
/* Send 80 or so clock transitions with CS and DI held high. This is     */
/* required after card power up to get it into SPI mode                  */
/*-----------------------------------------------------------------------*/
static
void send_initial_clock_train(void)
{
  unsigned int i;
  __u32 dat;
  
  /* Ensure CS is held high. */
  spi_cs_hi();
  
  /* Switch the SSI TX line to a GPIO and drive it high too. */
  ROM_GPIOPinTypeGPIOOutput(SDC_GPIO_PORT_BASE, SDC_SSI_TX);
  ROM_GPIOPinWrite(SDC_GPIO_PORT_BASE, SDC_SSI_TX, SDC_SSI_TX);
  
  /* Send 10 bytes over the SSI. This causes the clock to wiggle the */
  /* required number of times. */
  for(i = 0 ; i < 10 ; i++)
  {
    /* Write DUMMY data. SSIDataPut() waits until there is room in the */
    /* FIFO. */
    ROM_SSIDataPut(SDC_SSI_BASE, 0xFF);
    
    /* Flush data read during data write. */
    ROM_SSIDataGet(SDC_SSI_BASE, &dat);
  }
  
  /* Revert to hardware control of the SSI TX line. */
  ROM_GPIOPinTypeSSI(SDC_GPIO_PORT_BASE, SDC_SSI_TX);
}

int spi_init (void)
{
  /*
   * This doesn't really turn the power on, but initializes the
   * SSI port and pins needed to talk to the card.
   */
  
  /* Enable the peripherals used to drive the SDC on SSI, and the CS */
  ROM_SysCtlPeripheralEnable(SDC_SSI_SYSCTL_PERIPH);
  ROM_SysCtlPeripheralEnable(SDC_GPIO_SYSCTL_PERIPH);
  
  /* Configure the appropriate pins to be SSI instead of GPIO */
  ROM_GPIOPinTypeSSI(SDC_GPIO_PORT_BASE, SDC_SSI_TX | SDC_SSI_RX | SDC_SSI_CLK);
  ROM_GPIOPinTypeGPIOOutput(SDC_GPIO_PORT_BASE, SDC_SSI_FSS);
  ROM_GPIOPadConfigSet(SDC_GPIO_PORT_BASE, SDC_SSI_RX, GPIO_STRENGTH_4MA,
		   GPIO_PIN_TYPE_STD_WPU);
  ROM_GPIOPadConfigSet(SDC_GPIO_PORT_BASE, 
		   SDC_SSI_TX | SDC_SSI_FSS | SDC_SSI_CLK, GPIO_STRENGTH_4MA,
		   GPIO_PIN_TYPE_STD_WPU);
  
  /* Configure the SSI0 port */
  ROM_SSIConfigSetExpClk(SDC_SSI_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
		     SSI_MODE_MASTER, 400000, 8);
  ROM_SSIEnable(SDC_SSI_BASE);
  
  /* Set DI and CS high and apply more than 74 pulses to SCLK for the card */
  /* to be able to accept a native command. */
  send_initial_clock_train();
  
  return(0);
}


/*
** Set baudrate
** INPUT: br - baudrate
*/
void spi_set_baudrate (unsigned long br)
{
  unsigned long max = SysCtlClockGet()/2;

  if ( max > 12500000 ) max = 12500000;
  if ( br > max ) br = max;
			  
  /* Disable the SSI */
  ROM_SSIDisable(SDC_SSI_BASE);
  
  /* Configure the SSI0 port */
  ROM_SSIConfigSetExpClk(SDC_SSI_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
		     SSI_MODE_MASTER, br, 8);
  
  /* Enable the SSI */
  ROM_SSIEnable(SDC_SSI_BASE);
}


/*
** Get baudrate
** RETURN: baudrate
*/
unsigned long spi_get_baudrate (void)
{
  unsigned long val = SysCtlClockGet();
  
  val /= HWREG(SDC_SSI_BASE+SSI_O_CPSR);

  val /= HWREG(SDC_SSI_BASE+SSI_O_CR0)>>8;

  return(val/2);
}

/*
** Get Card Detect state
** RETURN: 0 - card is removed
**         1 - card present
*/
int get_cd (void)
{
  return 1;
}

/*
** Get Write Protect state
** RETURN: 0 - not protected
**         1 - write protected
*/
int get_wp (void)
{
  return 0;
}

