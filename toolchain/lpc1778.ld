/*
 *  _____   _    _ _________ _   _  _
 * |  __ \ | |  | |__   __//_ || || |
 * | |  | || |  | |  | |    | || || |_
 * | |  | || |  | |  | |    | ||__   _|
 * | |__| || |__| |  | |    | |   | |
 * |_____/  \____/   |_|    |_|   |_|
 *
 * Delft University of Technology
 * Racing Team 2013-2014
 *------------------------------------------
 * Memory regions for LPC1778
 *------------------------------------------
 */

MEMORY {
	rom (rx)  : ORIGIN = 0x00000000, LENGTH = 512K
	ram (rwx) : ORIGIN = 0x10000000, LENGTH =  64K
	
	/* TODO memory regions for SRAM banks */
}
