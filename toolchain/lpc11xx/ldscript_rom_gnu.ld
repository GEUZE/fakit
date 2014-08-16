/*
 * GENERATED FILE - DO NOT EDIT
 * (C) Code Red Technologies Ltd, 2008-2012
 * Generated linker script file for LPC11C24/301
 * Created from generic_c.ld (vLPCXpresso v4.1 (5 [Build 187] [12/12/2011] ))
 * By LPCXpresso v4.1.5 [Build 187] [12/12/2011]  on Mon Feb 13 18:04:43 CET 2012
 */

MEMORY
{
  /* Define each memory region */
  MFlash32 (rx) : ORIGIN = 0x0, LENGTH = 0x8000 /* 32k */
  RamLoc8 (rwx) : ORIGIN = 0x10000100, LENGTH = 0x1f00 /* 8k */

}

/* Define a symbol for the top of each memory region */
__top_MFlash32 = 0x0 + 0x8000;
__top_RamLoc8 = 0x10000000 + 0x2000;

ENTRY(ResetISR)

SECTIONS
{

	/* MAIN TEXT SECTION */	
	.text : ALIGN(4)
	{
		FILL(0xff)
		KEEP(*(.isr_vector))
		
		/* Global Section Table */
		. = ALIGN(4) ;
		__section_table_start = .;
		__data_section_table = .;
		LONG(LOADADDR(.data));
		LONG(    ADDR(.data)) ;
		LONG(  SIZEOF(.data));
		__data_section_table_end = .;
		__bss_section_table = .;
		LONG(    ADDR(.bss));
		LONG(  SIZEOF(.bss));
		__bss_section_table_end = .;
		__section_table_end = . ;
		/* End of Global Section Table */
		

		*(.after_vectors*)
		
		*(.text*)
		*(.rodata .rodata.*)
		. = ALIGN(4);
		
	} > MFlash32

	/*
	 * for exception handling/unwind - some Newlib functions (in common
	 * with C++ and STDC++) use this.
	 */
	.ARM.extab : ALIGN(4)
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > MFlash32
	__exidx_start = .;
	
	.ARM.exidx : ALIGN(4)
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > MFlash32
	__exidx_end = .;
	
	_etext = .;
		
	
	/* MAIN DATA SECTION */

	.uninit_RESERVED : ALIGN(4)
	{
		KEEP(*(.bss.$RESERVED*))
	} > RamLoc8

	.data : ALIGN(4)
	{
		FILL(0xff)
		_data = .;
		*(vtable)
		*(.data*)
		. = ALIGN(4) ;
		_edata = .;
	} > RamLoc8 AT>MFlash32


	/* MAIN BSS SECTION */
	.bss : ALIGN(4)
	{
		_bss = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4) ;
		_ebss = .;
		PROVIDE(end = .);
	} > RamLoc8
	
	PROVIDE(_pvHeapStart = .);
	PROVIDE(_vStackTop = __top_RamLoc8 - 0);
}
