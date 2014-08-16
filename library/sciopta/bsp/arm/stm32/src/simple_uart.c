/*
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
**                       (c) 2008 Sciopta AG/ Schweiz               **
**                                                                  **
**********************************************************************
** ID: S09049BS2                                                    **
** +Revision: 1.1.2.3 +                                             **
** +Date: 2009/06/03 15:25:42 +                                     **
** Simple UART routines                                             **
**********************************************************************
*/
#include <sciopta.h>


#ifndef USE_STDPERIPH_DRIVER
#define USE_STDPERIPH_DRIVER
#endif
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

static USART_TypeDef * const uart[3] = {
#ifdef USART1
  USART1,
#else 
  0,
#endif
#ifdef USART2
  USART2,
#else 
  0,
#endif
#ifdef USART3
  USART3,
#else 
  0,
#endif
};

#define MAX_USART sizeof(uart)/sizeof(USART_TypeDef *)

void uart_init(unsigned int unit,unsigned int baudrate)
{
  USART_InitTypeDef init;
  USART_ClockInitTypeDef clock;
  /* USART1 configuration ------------------------------------------------------*/
  /* USART configured as follow:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the second edge 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  */
  switch( unit ){
  case 0:
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    break;
  case 1:
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    break;
  case 2:
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    break;
  default:
    sc_miscError(SC_ERR_SYSTEM_FATAL,unit);
  }

  init.USART_BaudRate = baudrate;
  init.USART_WordLength = USART_WordLength_8b;
  init.USART_StopBits = USART_StopBits_1;
  init.USART_Parity = USART_Parity_No ;
  init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  clock.USART_Clock = USART_Clock_Disable;
  clock.USART_CPOL = USART_CPOL_Low;
  clock.USART_CPHA = USART_CPHA_2Edge;
  clock.USART_LastBit = USART_LastBit_Disable;
  
  if ( uart[unit] ){
    USART_ClockInit(uart[unit], &clock);
    USART_Init(uart[unit], &init);
    /* Enable the USART */
    USART_Cmd(uart[unit], ENABLE);
  }
}

void uart_putchar(int unit,int ch)
{
  if ( unit < MAX_USART && uart[unit] ){
    while( USART_GetFlagStatus(uart[unit],USART_FLAG_TXE) == RESET){
      // empty
    }
    USART_SendData(uart[unit],ch);
  }
}

int uart_getchar(int unit)
{
  int c = -1;
  if ( unit < MAX_USART && uart[unit] ){
    while( USART_GetFlagStatus(uart[unit],USART_FLAG_RXNE) == RESET ){
      // empty
    }
    c = USART_ReceiveData(uart[unit]);
  }
  return c;
}

