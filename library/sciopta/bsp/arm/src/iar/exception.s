/*SOC -*-asm-*-
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
**              (c) 2009 SCIOPTA Systems AG/ Schweiz                **
**                                                                  **
**********************************************************************
** ID: S09224BS1                                                    **
** +Revision: 1.1.2.1 +                                                 **
** +Date: 2009/08/12 09:34:55 +                                     **
** Exception handler for ARM                                        **
**********************************************************************
EOC*/
	
#define __ASM_INCLUDED__
#include <machine/arm/arm_asm.h>
#include "sconf.h"
#include <sciopta.h>

#if CPUTYPE == ARMv5TE || CPUTYPE == XSCALE || SC_PROCMMU_HOOK == 1
#define HAS_MMU
#else
#undef HAS_MMU
#endif

	SECTION .exception:CODE(2)	

	IMPORT	sc_svc_stack
#if defined SC_CDEBUG && SC_CDEBUG == 1
	IMPORT	xsc_miscError
	IMPORT	sciopta
#else
	IMPORT	sc_miscError
#endif
	
	EXPORT Abort_Handler
	CODE32
Abort_Handler:
	push	{r0-r1}			// save r0/r1 on exception stack
	mov	r0,#0x01000000
	b	exception

	EXPORT	Undefined_Handler
Undefined_Handler:
	push	{r0-r1}			// save r0/r1 on exception stack
	mov	r0,#0x02000000
	b	exception
	
	EXPORT	Prefetch_Handler
Prefetch_Handler:
	push	{r0-r1}			// save r0/r1 on exception stack
	mov	r0,#0x03000000
	b	exception
	
	EXPORT 	FIQ_Handler
FIQ_Handler:
	push	{r0-r1}			// save r0/r1 on exception stack
	mov	r0,#0x04000000

exception:
	sub	lr,lr,#4		// adjust LR
	ldr	r1,=(reg_save_area+12)
	stmia	r1,{r2-r14}^		// save USR/SYS-mode registers
	pop	{r3-r4}			// get saved r0/r1
	mov	r2,lr
	stmdb	r1!,{r2-r4}
	
	mov	r2,r0			// save cause
	
	// switch to svc mode and set stack
	msr	cpsr_c,#PSR_SVC_MODE|PSR_I_BIT|PSR_F_BIT
	ldr	sp,=sc_svc_stack
	ldreq r1,=55555555
#ifdef HAS_MMU
	mov	r0,#-1			// activate all domains
	mcr	p15,0,r0,c3,c0,0

	mrc	p15,0,r0,c6,c0,0	// get FAR
	str	r0,[r1,#16*4]
	
	mrc	p15,0,r0,c5,c0,0	// get fault status
	str	r0,[r1,#17*4]
#endif	

	mov	r0,#KERNEL_EILL_EXCEPTION
	orr	r0,r0,#SC_ERR_FATAL
	orr	r0,r0,r2
#if defined SC_CDEBUG && SC_CDEBUG == 1
	ldr	r2,=sciopta
	ldr	r2,[r2,#SC_CURPCB]
	cmp	r2,#0	
	moveq	r2,#__LINE__
	ldreq	r3,=filename
	ldrne	r3,[r2,#PCB_CFILE]
	ldrne	r2,[r2,#PCB_CLINE]
	
        ldr     r4,=xsc_miscError
#else
        ldr     r4,=sc_miscError
#endif
        bx      r4
	
#if defined SC_CDEBUG && SC_CDEBUG == 1
	DATA
filename:
	DC8 __FILE__
#endif
	ALIGNROM 2

	SECTION BSS:DATA
	EXPORT	reg_save_area
reg_save_area:
	DS32	15+1		// lr_abt,r0-r14
#ifdef HAS_MMU
	DS32	1
#endif	
        END
