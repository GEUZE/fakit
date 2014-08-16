/*  _____   _    _  _______  __  _  _
 * |  __ \ | |  | ||__   __|/_ || || |
 * | |  | || |  | |   | |    | || || |_
 * | |  | || |  | |   | |    | ||__   _|
 * | |__| || |__| |   | |    | |   | |
 * |_____/  \____/    |_|    |_|   |_|
 *
 * Delft University of Technology
 * Racing Team 2013-2014
 *------------------------------------------
 * Linker script for Sciopta with LPC177x_8x CMSIS
 * Author: Maurice Bos <m-ou.se@m-ou.se>
 *------------------------------------------
 */

/* 32 Bit little endian ELF format. */
OUTPUT_FORMAT("elf32-littlearm")

/* Libraries for things like malloc() and printf(). */
GROUP(-lgcc -lc -lcs3 -lcs3unhosted) /* -lcs3micro */

/* Entrypoint. */
ENTRY(_start)

SECTIONS {
	
	.text : {
		
		/* The reset vector (interrupt handler table) */
		KEEP(*(.reset_vector))
		
		/* The code sections. */
		*(.text .text.*)
		
		/* The read only data sections. */
		*(.rodata .rodata.*)
		
		/* The exception handler of sciopta,
		   which lives for some reason in a different section. */
		*(.exception)
		
		. = ALIGN(8);
		
	} >rom /* Store this in ROM. */
	
	/* The array of initialization function pointers,
	   for C++ constructors and the like. */
	.init_array : {
		
		__init_array = ABSOLUTE(.);
		KEEP(*(.init_array))
		KEEP(*(SORT(.init_array.*))) /* Sorted by priority. */
		__init_array_end = ABSOLUTE(.);
		. = ALIGN(8);
		
	} >rom /* Store this in ROM as well. */
	
	/* Throw away the finalization function pointers (deconstructors),
	   because the code will run 'forever' anyway. */
	/DISCARD/ : {
		*(.fini_array)
		*(.fini_array.*)
	}
	
	/* Pointers to the sections to copy or initialize.
	   Sciopta's 'cstartup' will copy the .data section and initialise .bbs. */
	.section_init : {
		__section_init = ABSOLUTE(.);
		LONG(LOADADDR(.text));
		LONG(ADDR(.text));
		LONG(SIZEOF(.text));
		LONG(LOADADDR(.data));
		LONG(ADDR(.data));
		LONG(SIZEOF(.data));
		LONG(ADDR(.bss));
		LONG(SIZEOF(.bss));
		__section_init_size = ABSOLUTE(.) - __section_init;
		. = ALIGN(8);
	} >rom /* Stored in ROM. */
	
	/* The location and size of the RAM memory,
	   used by the heap location calculation later. */
	__cs3_region_start_ram = ORIGIN(ram);
	__cs3_region_size_ram = LENGTH(ram);
	
	
	/* Initialized data. */
	.data : {
		*(.data .data.*)
		. = ALIGN(8);
	} >ram AT>rom /* Stored in ROM at boot to be copied to RAM. */
	
	/* Uninitialized data. */
	.bss : {
		*(.bss .bss.* COMMON)
		
		/* Sciopta has its own bss section. */
		*(.sc_bss)
		
		. = ALIGN(8);
		
		/* Start the heap here, at the end of the used part of RAM. */
		__cs3_heap_start = ABSOLUTE(.);
		__cs3_heap_end = __cs3_region_start_ram + __cs3_region_size_ram;
		
	} >ram /* Lives in the RAM, of course. */
	
}
