
ENTRY(_start)

MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 128K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 20K

    system_mod : ORIGIN = 0x20000000, LENGTH = 20K
}

system_size = LENGTH(system_mod);

SECTIONS
{
    .text :
    {
        KEEP(*(.reset_vector))
	*(.exception)
    } > FLASH

    .system_text :
    {
    } > system_mod AT > FLASH
  
    .text :
    {
        *(.text.krn*)
        *(.text.debkrn*)
        *(.text*)
	*(.glue_7*)
	*(.vfp11*)
	*(.v4_bx*)
        *(.rodata*)
      	. = ALIGN(4);
    } > FLASH
  .ARM.extab : { *(.ARM.extab*) *(.gnu.linkonce.armextab.*) } > FLASH
      __exidx_start = .;
   .ARM.exidx :
   {
     *(.ARM.exidx*) *(.gnu.linkonce.armexidx.*)
   } > FLASH
   __exidx_end = .;

  .section_init :
  {
	LONG(LOADADDR(.system_text) + 0)
	LONG(ADDR(.system_text) + 0);
	LONG(SIZEOF(.system_text));

	LONG(LOADADDR(.data) + 0);
	LONG(ADDR(.data) + 0);
	LONG(SIZEOF(.data));

	LONG(ADDR(.bss) + 0);
	LONG(SIZEOF(.bss));
    } > FLASH
  __section_init = ADDR(.section_init);
  __section_init_size = SIZEOF(.section_init);

  /*
  ** Setup sc_module_addr_t variables for symbolic
  ** initialisation of static modules.
  */

  .module_init :
  {
	system_mod = .;
        LONG(system_start);
	LONG(system_size);
	LONG(system_initsize);
   } > FLASH

    .data :
    {
      	. = ALIGN(4);
        *(vtable)
      	. = ALIGN(4);
        *(.data*)
      	. = ALIGN(4);
    } > system_mod AT > FLASH

    .bss :
    {
      	. = ALIGN(4);
        *(.bss*)
        *(COMMON)    
      	. = ALIGN(4);
    } > system_mod
    .sc_bss :
    {
      	. = ALIGN(8);
	*(.sc_bss)
      	. = ALIGN(4);
	__system_init_end = .;

    } > system_mod

  system_start = ADDR(.system_text) + 0;
  system_initsize = __system_init_end - system_start;
  system_free = system_size - system_initsize;

  .system_free (NOLOAD):
  {
	. += system_free;
  } > system_mod

  _end = .;
  PROVIDE (end = .);	
  /* Stabs debugging sections.  */
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .stab.excl 0 : { *(.stab.excl) }
  .stab.exclstr 0 : { *(.stab.exclstr) }
  .stab.index 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 : { *(.comment) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
  /* These must appear regardless of  .  */

}
	
