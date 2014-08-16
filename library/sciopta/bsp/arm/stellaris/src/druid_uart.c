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
**              (c) 2008 SCIOPTA Systems AG/ Schweiz                **
**                                                                  **
**********************************************************************
** ID: S08287BS1                                                    **
** +Revision: 1.3.2.1 +                                             **
** +Date: 2009/07/24 14:49:14 +                                     **
** Serial driver for druid.                                         **
**********************************************************************
EOC*/

#include "sconf.h"
#include <sciopta.h>
#include <druid/device.h>

#include "config.h"
/* LMI driver lib */
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_uart.h"
#include "inc/hw_ints.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

/*
** Use LMI driver lib
*/
#define druid_putc(a) UARTCharPut(CNF_DRUID_UNIT,a)
#define druid_getc()  UARTCharGet(CNF_DRUID_UNIT)

static druid_isr_f rxisr;

void
druid_intrEnable (void)
{
  UARTIntEnable(CNF_DRUID_UNIT,UART_INT_RX|UART_INT_RT);
}

void
druid_intrDisable (void)
{
  UARTIntDisable(CNF_DRUID_UNIT,0xffffffff);
}

SC_INT_PROCESS (druid, src)
{
  unsigned long c;

  if (src == 0) {
    while( (c = UARTCharGetNonBlocking(CNF_DRUID_UNIT)) != -1 ){
      rxisr ((char)c);
    }
  }
  else if (src == -1) {
    //
    // Enable the UART interrupt.
    //
#if CNF_DRUID_UNIT == UART0_BASE
    IntEnable(INT_UART0);
#elif CNF_DRUID_UNIT == UART1_BASE
    IntEnable(INT_UART1);
#elif CNF_DRUID_UNIT == UART2_BASE
    IntEnable(INT_UART2);
#endif
    /* switch on irq */
    druid_intrEnable ();
  }
  else if (src == -2) {
    druid_intrDisable ();
    //
    // Disable the UART interrupt.
    //
#if CNF_DRUID_UNIT == UART0_BASE
    IntDisable(INT_UART0);
#elif CNF_DRUID_UNIT == UART1_BASE
    IntDisable(INT_UART1);
#elif CNF_DRUID_UNIT == UART2_BASE
    IntDisable(INT_UART2);
#endif
  }
}

/**
 * Druid time get
 */
static __u32 druid_time = 0;

int
druid_timeUsPerTick (void)
{
  int tickLen = sc_tickLength (0);

  if (tickLen) {
    return tickLen;
  }
  else {
    return CNF_TICKLENGTH;
  }
}

void
druid_timeReset (void)
{
  druid_time = sc_tickGet ();
}

__u32
druid_timeGet (void)
{
  /* simplest impl is using sc_tickGet () */
  return sc_tickGet () - druid_time;
}

/*
****************************************************************************
** druid internal stuff, leave untouched !
****************************************************************************
*/
void
druid_init (druid_isr_f druid_rxisr, druid_isr_f druid_txisr)
{
  (void)druid_txisr; /* not yet used */
  rxisr = druid_rxisr;
  //
  // Configure the first UART for 115,200, 8-N-1 operation.
  //
  UARTConfigSetExpClk(CNF_DRUID_UNIT, SysCtlClockGet(), 115200,
		      (UART_CONFIG_WLEN_8 | 
		       UART_CONFIG_STOP_ONE |
		       UART_CONFIG_PAR_NONE));

  UARTFIFOLevelSet(CNF_DRUID_UNIT,UART_FIFO_TX1_8,UART_FIFO_RX4_8);
}

void druid_write(const __u8 *buf, int len)
{
  while( len--){
    druid_putc(*buf++);
  }
}

int druid_read(void)
{
  return (int) druid_getc();          // command
}
