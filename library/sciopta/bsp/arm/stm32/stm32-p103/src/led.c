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
** ID: S09154BS3                                                    **
** +Revision: 1.2.4.3 +                                                 **
** +Date: 2009/06/03 15:26:58 +                                     **
** board LED functions                                              **
**********************************************************************
EOC*/

#ifndef USE_STDPERIPH_DRIVER
#define USE_STDPERIPH_DRIVER
#endif
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

/*************************************************************************
 * Function Name: LEDsSet
 * Parameters: unsigned int State
 *
 * Return: none
 *
 * Description: Set LEDS State
 *
 *************************************************************************/
void LEDsSet (unsigned int State)
{
  GPIO_WriteBit(GPIOC,GPIO_Pin_12 ,(State & 1)?Bit_RESET:Bit_SET);
}

void initFeedback()
{
  GPIO_InitTypeDef GPIO_InitStructure;

    // Assign PC12 to LED
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void feedback()
{
  static int state = 1;
  LEDsSet(state);
  state = 1-state;
}
