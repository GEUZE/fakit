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
** ID: S08284BS1                                                    **
** +Revision: 1.1.2.1 +                                                 **
** +Date: 2009/11/18 09:35:51 +                                     **
** Lowlevel board setup                                             **
**********************************************************************
EOC*/

#include <ossys/types.h>
/* LMI Driver Lib */
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_uart.h"
#include "inc/hw_gpio.h"
#include "inc/hw_nvic.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"

#include <string.h> /* memcpy */

void initFeedback();

int reset_hook(void)
{
  extern char __vector_table[];

  //
  // 1st set vectortable
  //
  *(__vu32 *)NVIC_VTABLE  = (__u32)__vector_table;

  //
  // If running on Rev A2 silicon, turn the LDO voltage up to 2.75V.  This is
  // a workaround to allow the PLL to operate reliably.
  //
  if(REVISION_IS_A2)
  {
    ROM_SysCtlLDOSet(SYSCTL_LDO_2_75V);
  }
  //
  // Set the clocking to run at 50MHz from the PLL.
  //
  ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | 
		 SYSCTL_USE_PLL  | 
		 SYSCTL_OSC_MAIN |
		 SYSCTL_XTAL_8MHZ);

  ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_8);

  //
  // Enable the peripherals used by the application.
  //
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

  //
  // Configure the UART GPIOs as required.
  //
  ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  //
  // Configure GPIO for Status LED
  //
  ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
  ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 1);

  //
  // Configure and enable the first UART for 115,200, 8-N-1 operation.
  //
  ROM_UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
		      (UART_CONFIG_WLEN_8 | 
		       UART_CONFIG_STOP_ONE |
		       UART_CONFIG_PAR_NONE));
  //
  // setup basic MPU setting
  //
  {
    static const unsigned long mpu_init[6] = {
      0x00000000|NVIC_MPU_BASE_VALID,(NVIC_MPU_ATTR_AP_RO_RO|
				      NVIC_MPU_ATTR_SIZE_128K|
				      NVIC_MPU_ATTR_ENABLE),
      0x40000001|NVIC_MPU_BASE_VALID,(NVIC_MPU_ATTR_AP_RW_RW|
				      NVIC_MPU_ATTR_SIZE_64M|
				      NVIC_MPU_ATTR_ENABLE),
      0x20000002|NVIC_MPU_BASE_VALID,(NVIC_MPU_ATTR_AP_RW_RW|
				      NVIC_MPU_ATTR_SIZE_64K|
				      NVIC_MPU_ATTR_ENABLE),
    };
    static const unsigned long mpu_init2[2] = {
            0x01000003|NVIC_MPU_BASE_VALID,(NVIC_MPU_ATTR_AP_RO_RO|
				      NVIC_MPU_ATTR_SIZE_128K|
				      NVIC_MPU_ATTR_ENABLE),

    };
    memcpy((char *)NVIC_MPU_ATTR+4,(char *)mpu_init,sizeof(mpu_init));
    memcpy((char *)NVIC_MPU_ATTR+4,(char *)mpu_init2,sizeof(mpu_init2));
    // enable MPU
    *(__vu32 *)NVIC_MPU_CTRL = 1;
  }
  //
  // setup fault reporting
  //
  *(__vu32 *)NVIC_SYS_HND_CTRL = (NVIC_SYS_HND_CTRL_USAGE
				  | NVIC_SYS_HND_CTRL_BUS
				  | NVIC_SYS_HND_CTRL_MEM);
  
  //
  // return 0 => cold boot
  //
  return 0;
}
