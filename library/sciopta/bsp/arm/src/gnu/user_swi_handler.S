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
** (c) 2006 Sciopta Systems GmbH, Sciopta AG/ Schweiz               **
**                                                                  **
**********************************************************************
** ID: S06308BS1                                                    **
** +Revision: 1.1 +                                                **
** +Date: 2006/11/23 15:23:03 +                                     **
** user SWI handler (called from kernel)                            **
**********************************************************************
*/

#define __ASM_INCLUDED__
#include "sconf.h"
#include <sciopta.h>
#include <machine/arm/arm_asm.h>

	.text
	.code	32
	.align	2
	.globl user_SWI_handler
	.type	user_SWI_handler,function
user_SWI_handler:
	mrs	r12,spsr		// get saved CPU state
	tst	r12,#0x20		// is it thumb mode ?
	
        ldrneh  r12,[lr,#-2]            // yes, swi only 16bit
        bicne   r12,r12,#0xff00
        
	ldreq   r12,[lr,#-4]            // no, swi 32bit
        biceq   r12,r12,#0xff000000
	
	sub	r12,r12,#4		// 0..3 reserved for sciopta
	cmp	r12,#8
	ldrls	pc,[pc,r12, lsl #2]
	b	ush_error

	.long	0,0,0,0
	.long	0,0,0,0

ush_error:
	ldr	r0,=(0x05000000|KERNEL_EILL_EXCEPTION|SC_ERR_FATAL)
	mov	r1,lr
#if defined SC_CDEBUG && SC_CDEBUG == 1
	mov	r2,#__LINE__
	ldr	r3,=ush_filename
	bl	xsc_miscError
#else
	bl	sc_miscError
#endif
ush_error1:
	b	ush_error1

	.ltorg

	.size	user_SWI_handler,.-user_SWI_handler
			
#if defined SC_CDEBUG && SC_CDEBUG == 1
	.section .rodata,"a"
ush_filename:
	.asciz __FILE__
	.align 2
#endif


