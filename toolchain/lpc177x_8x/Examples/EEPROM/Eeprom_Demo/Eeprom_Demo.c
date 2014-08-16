/**********************************************************************
* $Id$		Eeprom_Demo.c	2011-06-02
*//**
* @file		Eeprom_Demo.c
* @brief	This example describes how to use I2S transfer in interrupt
* 			mode
* @version	1.0
* @date		02. June. 2011
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/
#include "LPC177x_8x.h"
#include "lpc177x_8x_eeprom.h"
#include "debug_frmwrk.h"
#include "lpc177x_8x_clkpwr.h"

#define PAGE_OFFSET			0x10
#define PAGE_ADDR			0x01


/* Example group ----------------------------------------------------------- */
/** @defgroup EEPROM_Demo	EEPROM Demo
 * @ingroup EEPROM_Examples
 * @{
 */

/************************** PRIVATE VARIABLES ***********************/
uint8_t menu[]=
"********************************************************************************\n\r"
" Hello NXP Semiconductors \n\r"
" EEPROM demo example \n\r"
"\t - MCU: LPC177x_8x \n\r"
"\t - Core: ARM CORTEX-M3 \n\r"
"\t - Communicate via: UART0\1\2 - 115200 bps \n\r"
" This example used to demo EEPROM operation on LPC177x_8x.\n\r"
" A 'Hello' sentence will be written into EEPROM memory, then read back and check.\n\r"
"********************************************************************************\n\r";
uint8_t read_buffer[];
uint8_t write_buffer[]="NXP Semiconductor LPC177x_8x-CortexM3 \n\r\t--- HELLO WORLD!!!---";


/************************** PRIVATE FUNCTIONS *************************/
/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry (void) {                       /* Main Program */
	uint32_t i;
	uint8_t count;

	debug_frmwrk_init();
	_DBG(menu);
	EEPROM_Init();

	count = sizeof(write_buffer);

	_DBG_("Write data to EEPROM");
	EEPROM_Write(PAGE_OFFSET,PAGE_ADDR,(void*)write_buffer,MODE_8_BIT,count);

	_DBG_("Read data from EEPROM");
	EEPROM_Read(PAGE_OFFSET,PAGE_ADDR,(void*)read_buffer,MODE_8_BIT,count);

	//display eeprom data
	for(i=0;i<count;i++)
	{
		_DBC(read_buffer[i]);
	}

	_DBG_("");
	_DBG_("Demo is terminated");

	while(1);

}

/* With ARM and GHS toolsets, the entry point is main() - this will
   allow the linker to generate wrapper code to setup stacks, allocate
   heap area, and initialize and copy code and data segments. For GNU
   toolsets, the entry point is through __start() in the crt0_gnu.asm
   file, and that startup code will setup stacks and data */
int main(void)
{
    return c_entry();
}


/**
 * @}
*/
