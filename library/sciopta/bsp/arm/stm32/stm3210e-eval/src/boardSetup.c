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
** ID: S09154BS1                                                    **
** +Revision: 1.1.2.5 +                                             **
** +Date: 2009/08/21 10:14:11 +                                     **
** board setup                                                      **
**********************************************************************
EOC*/
#ifndef USE_STDPERIPH_DRIVER
#define USE_STDPERIPH_DRIVER
#endif
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_flash.h"
#include "core_cm3.h"

void ClkInit (void)
{
  // 1. Clocking the controller from internal HSI RC (8 MHz)
  RCC_HSICmd(ENABLE);
  // wait until the HSI is ready
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
  // 2. Enable ext. high frequency OSC
  RCC_HSEConfig(RCC_HSE_ON);
  // wait until the HSE is ready
  while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);
  // 3. Init PLL
  RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9); // 72MHz
  RCC_PLLCmd(ENABLE);
  // wait until the PLL is ready
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
  // 4. Set system clock dividers
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  RCC_ADCCLKConfig(RCC_PCLK2_Div8);
  RCC_PCLK2Config(RCC_HCLK_Div1);
  RCC_PCLK1Config(RCC_HCLK_Div2);
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  // 5. Init Embedded Flash
  // Zero wait state, if 0 < HCLK 24 MHz
  // One wait state, if 24 MHz < HCLK 56 MHz
  // Two wait states, if 56 MHz < HCLK 72 MHz
  // Flash wait state
  FLASH_SetLatency(FLASH_Latency_2);
  // Half cycle access
  FLASH_HalfCycleAccessCmd(FLASH_HalfCycleAccess_Disable);
  // Prefetch buffer
  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
  // 5. Clock system from PLL
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
}

int reset_hook(void)
{
  GPIO_InitTypeDef gpio;
  extern char __vector_table[];

  SCB->VTOR = (long) __vector_table;

  ClkInit();
  
  // GPIO Init
  // Enable GPIO clock and release reset
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                         RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOF,
                         ENABLE);
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA |
                         RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOF,
                         DISABLE);

  // Gonfigure UART2_Tx pin PA2
  gpio.GPIO_Pin = GPIO_Pin_2;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init (GPIOA, &gpio);

  // Gonfigure UART2_Rx pin PA3
  gpio.GPIO_Pin = GPIO_Pin_3;
  gpio.GPIO_Mode = GPIO_Mode_IPD ;
  GPIO_Init (GPIOA, &gpio);
  //
  // setup fault reporting
  //
#define NVIC_SYS_HND_CTRL_USAGE (1<<18)
#define NVIC_SYS_HND_CTRL_BUS   (1<<17)
#define NVIC_SYS_HND_CTRL_MEM   (1<<16)
  
  SCB->SHCSR = (NVIC_SYS_HND_CTRL_USAGE
		| NVIC_SYS_HND_CTRL_BUS
		| NVIC_SYS_HND_CTRL_MEM);
  

  return 0;
}
