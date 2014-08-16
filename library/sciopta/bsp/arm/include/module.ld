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
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S04215BS1                                                    **
** +Revision: 1.6 +                                                 **
** +Date: 2008/01/16 13:01:14 +                                     **
** Linker script: Module sections (common to all boards)            **
**********************************************************************
*/

SECTIONS
{
/*
*********************************************************************
** Modules other than system
**
** .<module>_text : RAM copy for rodata and text if wanted.
** .<module>_data : rw-data
** .<module>_bss  : BSS
** .<module>_free : free RAM to be used by Sciopta for runtime
**                  allocated memory (pools, pcbs, stacks)
**
** Each module must define for sconf.c :
** <module>_start      : Start address of module-RAM
** <module>_initsize   : Size of initialized RAM
** <module>_size       : Complete size of module(1)
** <module>_mod        : A structure that hold the above three.
**
** (1) This is defined in the main linker-script and should
**     be <= the memory section size (<module>_mod).
*********************************************************************
*/

  .dev_text :
  {
	. = ALIGN(4);
  } > dev_mod AT > rom

  .dev_data :
  {
	. = ALIGN(4);
  } > dev_mod AT > rom

  .dev_bss (NOLOAD) :
  {
	. = ALIGN(4);
	dev_end = .;
  } > dev_mod

  dev_start = ADDR(.dev_text)+0;
  dev_initsize = dev_end - dev_start; 
  dev_free = dev_size - dev_initsize;

  .dev_free (NOLOAD):
  {
     . += dev_free;
  } > dev_mod

/*
******************************************************************************
*/ 
  .ips_text :
  {
	. = ALIGN(4);
  } > ips_mod AT > rom

  .ips_data :
  {
	. = ALIGN(4);
  } > ips_mod AT > rom

  .ips_bss (NOLOAD) :
  {
	. = ALIGN(4);
	ips_end = .;
  } > ips_mod

  ips_start = ADDR(.ips_text)+0;
  ips_initsize = ips_end - ips_start;
  ips_free = ips_size - ips_initsize;

   .ips_free (NOLOAD):
  {
     . += ips_free;
  } > ips_mod

/*
******************************************************************************
*/ 
  .sfs_text :
  {
	. = ALIGN(4);
  } > sfs_mod AT > rom

  .sfs_data :
  {
	. = ALIGN(4);
  } > sfs_mod AT > rom

  .sfs_bss (NOLOAD) :
  {
	. = ALIGN(4);
	sfs_end = .;
  } > sfs_mod

  sfs_start = ADDR(.sfs_text)+0;
  sfs_initsize = sfs_end - sfs_start;
  sfs_free = sfs_size - sfs_initsize;

   .sfs_free (NOLOAD):
  {
     . += sfs_free;
  } > sfs_mod

/*
******************************************************************************
*/ 
  .user_text :
  {
	. = ALIGN(4);
  } > user_mod AT > rom

  .user_data :
  {
     . = ALIGN(4);
  } > user_mod AT > rom

  .user_bss (NOLOAD):
  {
     	. = ALIGN(4);
	user_end = .;
  } > user_mod

  user_start = ADDR(.user_text)+0;
  user_initsize = user_end - user_start;
  user_free = user_size - user_initsize;

  .user_free (NOLOAD):
  {
     . += user_free;
  } > user_mod
/*
******************************************************************************
*/ 

  /*
  ** Create C-startup init table for 
  ** a) ROM->RAM copy of RW-data
  ** b) ROM->RAM copy of code (if wanted)
  ** c) clearing of BSS
  */

/*
** Note: + 0 is essential due to a linker bug !
*/
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

	/* 
	** place module data here 
	*/

	LONG(LOADADDR(.dev_text) + 0);
	LONG(ADDR(.dev_text) + 0);
	LONG(SIZEOF(.dev_text));
	LONG(LOADADDR(.dev_data) + 0);
	LONG(ADDR(.dev_data) + 0);
	LONG(SIZEOF(.dev_data));
	LONG(ADDR(.dev_bss) + 0);	
	LONG(SIZEOF(.dev_bss));

	LONG(LOADADDR(.ips_text) + 0);
	LONG(ADDR(.ips_text) + 0);
	LONG(SIZEOF(.ips_text));
	LONG(LOADADDR(.ips_data) + 0);
	LONG(ADDR(.ips_data) + 0);
	LONG(SIZEOF(.ips_data));
	LONG(ADDR(.ips_bss) + 0);	
	LONG(SIZEOF(.ips_bss));

	LONG(LOADADDR(.sfs_text) + 0);
	LONG(ADDR(.sfs_text) + 0);
	LONG(SIZEOF(.sfs_text));
	LONG(LOADADDR(.sfs_data) + 0);
	LONG(ADDR(.sfs_data) + 0);
	LONG(SIZEOF(.sfs_data));
	LONG(ADDR(.sfs_bss) + 0);	
	LONG(SIZEOF(.sfs_bss));

	LONG(LOADADDR(.user_text) + 0);
	LONG(ADDR(.user_text) + 0);
	LONG(SIZEOF(.user_text));
	LONG(LOADADDR(.user_data) + 0);
	LONG(ADDR(.user_data) + 0);
	LONG(SIZEOF(.user_data));
	LONG(ADDR(.user_bss) + 0);	
	LONG(SIZEOF(.user_bss));
  } > rom

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
	dev_mod = .;
	LONG(dev_start);
	LONG(dev_size);
	LONG(dev_initsize);
	ips_mod = .;
	LONG(ips_start);
	LONG(ips_size);
	LONG(ips_initsize);
	sfs_mod = .;
	LONG(sfs_start);
	LONG(sfs_size);
	LONG(sfs_initsize);
	user_mod = .;
	LONG(user_start);
	LONG(user_size);
	LONG(user_initsize);
   } > rom
}
