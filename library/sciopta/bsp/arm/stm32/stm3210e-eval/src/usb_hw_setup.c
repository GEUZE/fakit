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
**        (c) 2009 Sciopta Systems GmbH, Sciopta AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S09155BS1                                                    **
** +Revision: 1.1.2.1 +                                                 **
** +Date: 2009/08/20 09:04:01 +                                     **
** USB hardware setup for stm3210                                   **
**********************************************************************
EOC*/

#include <sciopta.h>

#include "config.h"
#ifndef USE_STDPERIPH_DRIVER
#define USE_STDPERIPH_DRIVER
#endif
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

#include "usb_lib.h"

void USBDInit()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable USB_DISCONNECT GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_DISCONNECT, ENABLE);

  /* Configure USB pull-up pin */
  GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);

  /* USBCLK = PLLCLK / 1.5 */
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  /* Enable USB clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);

}

/*******************************************************************************
* Function Name  : USB_Cable_Config
* Description    : Software Connection/Disconnection of USB Cable
* Input          : None.
* Return         : Status
*******************************************************************************/
void USB_Cable_Config (FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }
  else
  {
    GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }
}

/*******************************************************************************
* Function Name  : Get_SerialNum.
* Description    : Create the serial number string descriptor.
* Input          : array of u32.
* Output         : filled array.
* Return         : status.
*******************************************************************************/
int Get_SerialNum(uint32_t serial[3])
{
  if (*(__IO uint32_t*)(0x1FFFF7E8) != 0)
  {
    serial[0] = *(__IO uint32_t*)(0x1FFFF7E8);
    serial[1] = *(__IO uint32_t*)(0x1FFFF7EC);
    serial[2] = *(__IO uint32_t*)(0x1FFFF7f0);
    return 1;
  } 
  return 0;
}
