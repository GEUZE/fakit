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
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S04086BS3                                                    **
** +Revision: 1.16.4.2 +                                                 **
** +Date: 2009/08/19 21:24:00 +                                     **
** Interrupt prologue/epilogue macros                               **
**********************************************************************
EOC*/

#ifndef __CONVERT__
#define __ASM_INCLUDED__
#include <sciopta.h>
#include <machine/arm/arm_asm.h>
#define EXTERN .extern
#define LONG .long
#endif /* __CONVERT__ */

/* Note: the double ";" will be converted into NL */

#ifndef __TKERNEL__
/*
** (v4,v5,XScale)
** Stacklayout (IRQ stack)
**
**      r14_irq
**      spsr
** SP ->
**
** Stacklayout (USR/SYS stack)
**      r14_sys
**      r12
**      r3
**      ...
**      r0
** IF MMU
**      dac
** ENDIF
** IF XSCALE
**      acc0.h
**      acc0.l
** ENDIF
**      r11
**      ...
**      r4
**      r14_irq
**      spsr
** SP->
**
**
** 4T  5TE
** 1    1	sub	r14,r14,#4			// adjust lr
** 2    2	stmfd	sp!,{r14}			// save
** 1    2	mrs	r14,spsr			// get and save spsr
** 2    2	stmfd	sp!,{r14}		
** 1    3	msr	cpsr_c,#PSR_SYS_MODE|PSR_I_BIT	// switch to SYS mode
** 7    6	stmfd	sp!,{r0-r3,r12,r14}	// save scratch registers
** 3	2	ldr	r12,=sciopta		
** 3	1	ldrh	r14,[r12,#SC_NESTED_IRQ_H] // increase nested counter
** 3	1	ldr	r3,[r12,#SC_CURPCB]	
** 1	1	add	r14,r14,#1		
** 2	1	strh	r14,[r12,#SC_NESTED_IRQ_H]
** 2	1	str	r13,[r3,#PCB_STACKPTR]// and save SP of inter'ed proc
** -------
** 28  23
**
** EPILOGUE
**	EXTERN sciopta			
**	EXTERN sc_sysProcDispatcher	
**	ldr	r12,=sciopta		
**	msr	cpsr_c,#PSR_SYS_MODE|PSR_I_BIT	// SYS-mode, no interrupts
**	ldrh	r0,[r12,#SC_NESTED_IRQ_H]	// decrease nested counter
**	ldr	r3,[r12,#SC_CURPCB]	
**	subs	r0,r0,#1			// and test for zero
**	strh	r0,[r12,#SC_NESTED_IRQ_H]
**	ldr	sp,[r3,#PCB_STACKPTR]		// SP of interrupted process
**	beq	label##non_nested		// nested == 0 => might resched
**no_sched##label:			
**	ldmfd	sp!,{r0-r3,r12,r14}		// restore scratch registers
**	msr	cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT	// back to IRQ mode
**	ldmfd	sp!,{r14}			// restore SPSR
**	msr	spsr,r14		
**	ldmfd	sp!,{pc}^			// return to inter'ed process
**label##non_nested:			
**	ldr	r0,[r12,#SC_SCHEDULE_REQ]	// re-scheduling needed ?
**	cmp	r0,#0			
**	beq	no_sched##label			// no => return to inter`ed
**	ldr	r0,[r12,#SC_SCHEDULER_LOCK]	// scheduler locked ?
**	cmp	r0,#0			
**	bne	no_sched##label			// yes, return to inter`ed pr.
**	msr	cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT	// switch to IRQ mode
**	ldmfd	sp!,{r0,r1}			// and fetch LR_irq,SPSR_irq
**	msr	cpsr_c,#PSR_SYS_MODE|PSR_I_BIT	// back to SYS mode
**	stmfd	sp!,{r0-r1,r4-r11}		// save remaining registers
**	orr	r2,sp,#1			// set full-swapin flag
**	str	r2,[r3,#PCB_STACKPTR]		// and save SP
**	bl	sc_sysProcDispatcher		// now re-schedule
*/
	
#define IRQ_PROLOGUE				\
	EXTERN sciopta				;; \
	EXTERN	sc_sysIrqDispatcher		;; \
	sub	r14,r14,#4			;; \
	stmfd	sp!,{r14}			;; \
	mrs	r14,spsr			;; \
	stmfd	sp!,{r14}			;; \
	msr	cpsr_c,#PSR_SYS_MODE|PSR_I_BIT	;; \
	stmfd	sp!,{r0-r3,r12,r14}		;; \
	ldr	r12,=sciopta			;; \
	ldrh	r14,[r12,#SC_NESTED_IRQ_H]	;; \
	ldr	r3,[r12,#SC_CURPCB]		;; \
	add	r14,r14,#1			;; \
	strh	r14,[r12,#SC_NESTED_IRQ_H]	;; \
	str	r13,[r3,#PCB_STACKPTR]

#define IRQ_PROLOGUE_MMU			\
	EXTERN sciopta				;; \
	EXTERN	sc_sysIrqDispatcher		;; \
	sub	r14,r14,#4			;; \
	stmfd	sp!,{r14}			;; \
	mrs	r14,spsr			;; \
	stmfd	sp!,{r14}			;; \
	msr	cpsr_c,#PSR_SYS_MODE|PSR_I_BIT	;; \
	stmfd	sp!,{r0-r3,r12,r14}		;; \
	ldr	r12,=sciopta			;; \
	ldrh	r14,[r12,#SC_NESTED_IRQ_H]	;; \
	ldr	r3,[r12,#SC_CURPCB]		;; \
	add	r14,r14,#1			;; \
	strh	r14,[r12,#SC_NESTED_IRQ_H]	;; \
	mrc	p15,0,r0,c3,c0,0 /* save old domains */ ;; \
	stmfd	sp!,{r0}			;; \
	mov	r0,#-1		/* master in all domains */ ;; \
	mcr	p15,0,r0,c3,c0,0		;; \
	str	r13,[r3,#PCB_STACKPTR]

#define IRQ_PROLOGUE_XSCALE			\
	EXTERN sciopta				;; \
	EXTERN	sc_sysIrqDispatcher		;; \
	sub	r14,r14,#4			;; \
	stmfd	sp!,{r14}			;; \
	mrs	r14,spsr			;; \
	stmfd	sp!,{r14}			;; \
	msr	cpsr_c,#PSR_SYS_MODE|PSR_I_BIT	;; \
	stmfd	sp!,{r0-r3,r12,r14}		;; \
	mra	r0,r1,acc0			;; \
	stmfd	sp!,{r0,r1}			;; \
	ldr	r12,=sciopta			;; \
	ldrh	r14,[r12,#SC_NESTED_IRQ_H]	;; \
	add	r14,r14,#1			;; \
	strh	r14,[r12,#SC_NESTED_IRQ_H]	;; \
	ldr	r14,[r12,#SC_CURPCB]		;; \
	str	r13,[r14,#PCB_STACKPTR]

#define IRQ_PROLOGUE_XSCALE_MMU			\
	EXTERN sciopta				;; \
	EXTERN	sc_sysIrqDispatcher		;; \
	sub	r14,r14,#4			;; \
	stmfd	sp!,{r14}			;; \
	mrs	r14,spsr			;; \
	stmfd	sp!,{r14}			;; \
	msr	cpsr_c,#PSR_SYS_MODE|PSR_I_BIT	;; \
	stmfd	sp!,{r0-r3,r12,r14}		;; \
	mra	r0,r1,acc0			;; \
	mrc	p15,0,r2,c3,c0,0 /* save old domains */ ;; \
	stmfd	sp!,{r0-r2}			;; \
	ldr	r12,=sciopta			;; \
	ldrh	r14,[r12,#SC_NESTED_IRQ_H]	;; \
	add	r14,r14,#1			;; \
	strh	r14,[r12,#SC_NESTED_IRQ_H]	;; \
	ldr	r14,[r12,#SC_CURPCB]		;; \
	mov	r0,#-1		/* master in all domains */ ;; \
	mcr	p15,0,r0,c3,c0,0		;; \
	str	r13,[r14,#PCB_STACKPTR]


#define IRQ_EPILOGUE(label)			\
	EXTERN sciopta				;; \
	EXTERN sc_sysProcDispatcher		;; \
	ldr	r12,=sciopta			;; \
	msr	cpsr_c,#PSR_SYS_MODE|PSR_I_BIT	;; \
	ldrh	r0,[r12,#SC_NESTED_IRQ_H]	;; \
	ldr	r3,[r12,#SC_CURPCB]		;; \
	subs	r0,r0,#1			;; \
	strh	r0,[r12,#SC_NESTED_IRQ_H]	;; \
	ldr	sp,[r3,#PCB_STACKPTR]		;;\
	beq	non_nested##label		;;\
no_sched##label:				;; \
	ldmfd	sp!,{r0-r3,r12,r14}		;; \
	msr	cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT	;; \
	ldmfd	sp!,{r14}			;; \
	msr	spsr_cxsf,r14			;; \
	ldmfd	sp!,{pc}^			;;\
non_nested##label:				;; \
	ldr	r0,[r12,#SC_SCHEDULE_REQ]	;; \
	cmp	r0,#0				;; \
	beq	no_sched##label			;; \
	ldr	r0,[r12,#SC_SCHEDULER_LOCK]	;; \
	cmp	r0,#0				;; \
	bne	no_sched##label			;; \
	msr	cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT	;; \
	ldmfd	sp!,{r0,r1}			;; \
	msr	cpsr_c,#PSR_SYS_MODE|PSR_I_BIT	;; \
	stmfd	sp!,{r0-r1,r4-r11}		;; \
	orr	r2,sp,#1 /* full-swapin flag */	;; \
	str	r2,[r3,#PCB_STACKPTR]		;; \
	bl	sc_sysProcDispatcher

#define IRQ_EPILOGUE_MMU(label)			\
	EXTERN sciopta				;; \
	EXTERN sc_sysProcDispatcher		;; \
	ldr	r12,=sciopta			;; \
	msr	cpsr_c,#PSR_SYS_MODE|PSR_I_BIT	;; \
	ldrh	r0,[r12,#SC_NESTED_IRQ_H]	;; \
	ldr	r3,[r12,#SC_CURPCB]		;; \
	subs	r0,r0,#1			;; \
	strh	r0,[r12,#SC_NESTED_IRQ_H]	;; \
	ldr	sp,[r3,#PCB_STACKPTR]		;;\
	beq	non_nested##label		;;\
no_sched##label:				;; \
	ldmfd	sp!,{r0} /* restore domains */	;; \
	mcr	p15,0,r0,c3,c0,0		;; \
	ldmfd	sp!,{r0-r3,r12,r14}		;; \
	msr	cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT	;; \
	ldmfd	sp!,{r14}			;; \
	msr	spsr_cxsf,r14			;; \
	ldmfd	sp!,{pc}^			;;\
non_nested##label:				;; \
	ldr	r0,[r12,#SC_SCHEDULE_REQ]	;; \
	cmp	r0,#0				;; \
	beq	no_sched##label			;; \
	ldr	r0,[r12,#SC_SCHEDULER_LOCK]	;; \
	cmp	r0,#0				;; \
	bne	no_sched##label			;; \
	msr	cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT	;; \
	ldmfd	sp!,{r0,r1} /* spsr,lr_sys */	;; \
	msr	cpsr_c,#PSR_SYS_MODE|PSR_I_BIT	;; \
	stmfd	sp!,{r0-r1,r4-r11}		;; \
	orr	r2,sp,#1 /* full-swapin flag */	;; \
	str	r2,[r3,#PCB_STACKPTR]		;; \
	bl	sc_sysProcDispatcher

#define IRQ_EPILOGUE_XSCALE(label)		\
	EXTERN sciopta				;; \
	EXTERN sc_sysProcDispatcher		;; \
	ldr	r12,=sciopta			;; \
	msr	cpsr_c,#PSR_SYS_MODE|PSR_I_BIT	;; \
	ldr	r3,[r12,#SC_CURPCB]		;; \
	ldrh	r0,[r12,#SC_NESTED_IRQ_H]	;; \
	subs	r0,r0,#1			;; \
	strh	r0,[r12,#SC_NESTED_IRQ_H]	;; \
	ldr	sp,[r3,#PCB_STACKPTR]		;;\
	beq	non_nested##label		;;\
no_sched##label:				;; \
	ldmfd	sp!,{r0-r1}			;; \
	mar	acc0,r0,r1			;; \
	ldmfd	sp!,{r0-r3,r12,r14}		;; \
	msr	cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT	;; \
	ldmfd	sp!,{r14}			;; \
	msr	spsr_cxsf,r14			;; \
	ldmfd	sp!,{pc}^			;;\
non_nested##label:				;; \
	ldr	r0,[r12,#SC_SCHEDULE_REQ]	;; \
	cmp	r0,#0				;; \
	beq	no_sched##label			;; \
	ldr	r0,[r12,#SC_SCHEDULER_LOCK]	;; \
	cmp	r0,#0				;; \
	bne	no_sched##label			;; \
	ldr	r3,[r12,#SC_CURPCB]		;; \
	msr	cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT	;; \
	ldmfd	sp!,{r0,r1}			;; \
	msr	cpsr_c,#PSR_SYS_MODE|PSR_I_BIT	;; \
	stmfd	sp!,{r0-r1,r4-r11}		;; \
	orr	r2,sp,#1 /* full-swapin flag */	;; \
	str	r2,[r3,#PCB_STACKPTR]		;; \
	bl	sc_sysProcDispatcher

#define IRQ_EPILOGUE_XSCALE_MMU(label)		\
	EXTERN sciopta				;; \
	EXTERN sc_sysProcDispatcher		;; \
	ldr	r12,=sciopta			;; \
	msr	cpsr_c,#PSR_SYS_MODE|PSR_I_BIT	;; \
	ldr	r3,[r12,#SC_CURPCB]		;; \
	ldrh	r0,[r12,#SC_NESTED_IRQ_H]	;; \
	subs	r0,r0,#1			;; \
	strh	r0,[r12,#SC_NESTED_IRQ_H]	;; \
	ldr	sp,[r3,#PCB_STACKPTR]		;;\
	beq	non_nested##label		;;\
no_sched##label:				;; \
	ldmfd	sp!,{r0-r2}			;; \
	mcr	p15,0,r2,c3,c0,0		;; \
	mar	acc0,r0,r1			;; \
	ldmfd	sp!,{r0-r3,r12,r14}		;; \
	msr	cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT	;; \
	ldmfd	sp!,{r14}			;; \
	msr	spsr_cxsf,r14			;; \
	ldmfd	sp!,{pc}^			;;\
non_nested##label:				;; \
	ldr	r0,[r12,#SC_SCHEDULE_REQ]	;; \
	cmp	r0,#0				;; \
	beq	no_sched##label			;; \
	ldr	r0,[r12,#SC_SCHEDULER_LOCK]	;; \
	cmp	r0,#0				;; \
	bne	no_sched##label			;; \
	ldr	r3,[r12,#SC_CURPCB]		;; \
	msr	cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT	;; \
	ldmfd	sp!,{r0,r1} /* spsr_irq,lr_irq */ ;; \
	msr	cpsr_c,#PSR_SYS_MODE|PSR_I_BIT	;; \
	stmfd	sp!,{r0-r1,r4-r11}		;; \
	orr	r2,sp,#1 /* full-swapin flag */	;; \
	str	r2,[r3,#PCB_STACKPTR]		;; \
	bl	sc_sysProcDispatcher

/*
** v6
**
** Stacklayout (USR/SYS stack)
**	spsr
**	r14_irq
**      r14_sys
**      r12
**      r3
**      ...
**      r0
** IF MMU
**      dac
** ENDIF
**      r11
**      ...
**      r4
** SP->
**
*/
#define IRQ_PROLOGUE_V6 \
	EXTERN sciopta				;; \
	EXTERN	sc_sysIrqDispatcher		;; \
	sub	r14,r14,#4			;; \
	srsfd	r13_sys!			;; \
	cpsid	i,#PSR_SYS_MODE			;; \
	stmfd	sp!,{r0-r3,r12,r14}		;; \
	ldr	r12,=sciopta			;; \
	ldrh	r14,[r12,#SC_NESTED_IRQ_H]	;; \
	ldr	r3,[r12,#SC_CURPCB]		;; \
	add	r14,r14,#1			;; \
	strh	r14,[r12,#SC_NESTED_IRQ_H]	;; \
	str	r13,[r3,#PCB_STACKPTR]
	
#define IRQ_EPILOGUE_V6(label)			\
	EXTERN sciopta				;; \
	EXTERN sc_sysProcDispatcher		;; \
	ldr	r12,=sciopta			;; \
	cpsid	i,#PSR_SYS_MODE			;; \
	ldrh	r0,[r12,#SC_NESTED_IRQ_H]	;; \
	ldr	r3,[r12,#SC_CURPCB]		;; \
	subs	r0,r0,#1			;; \
	strh	r0,[r12,#SC_NESTED_IRQ_H]	;; \
	ldr	sp,[r3,#PCB_STACKPTR]		;;\
	beq	non_nested##label		;;\
no_sched##label:				;; \
	ldmfd	sp!,{r0-r3,r12,r14}		;; \
	rfefd	r13!				;; \
non_nested##label:				;; \
	ldr	r0,[r12,#SC_SCHEDULE_REQ]	;; \
	cmp	r0,#0				;; \
	beq	no_sched##label			;; \
	ldr	r0,[r12,#SC_SCHEDULER_LOCK]	;; \
	cmp	r0,#0				;; \
	bne	no_sched##label			;; \
	stmfd	sp!,{r4-r11}			;; \
	orr	r2,sp,#1 /* full-swapin flag */	;; \
	str	r2,[r3,#PCB_STACKPTR]		;; \
	bl	sc_sysProcDispatcher

#else /* !__TKERNEL__ */

/* TODO: Nested interrupts do not work on TINY !*/
#define IRQ_PROLOGUE				\
	EXTERN sciopta				;; \
	EXTERN	sc_sysIrqDispatcher		;; \
	str	r14,[sp,#4]			;; \
	mrs	r14,spsr			;; \
	str	r14,[sp,#0]			;; \
	msr	cpsr_c,#PSR_SYS_MODE|PSR_I_BIT	;; \
	stmfd	sp!,{r0-r3,r12,r14}		;; \
	ldr	r3,=(sciopta-8)			;; \
	ldmia	r3!,{r0-r1}			;; \
	stmfd	sp!,{r0,r1}			;; \
	ldrb	r0,[r3,#SC_NESTED_IRQ_B]	;; \
	ldr	r1,[r3,#SC_CURPCB]		;; \
	add	r0,r0,#1			;; \
	strb	r0,[r3,#SC_NESTED_IRQ_B]	;; \
	str	sp,[r1,#PCB_STACKPTR]		;; \
	sub	sp,r3,#8

#define IRQ_EPILOGUE(label)			\
	EXTERN sciopta				;; \
	EXTERN sc_sysProcDispatcher		;; \
	msr	cpsr_c,#PSR_SYS_MODE|PSR_I_BIT	;; \
	ldr	r3,sciopta##label		;; \
	ldr	r2,[r3,#SC_CURPCB]		;; \
	ldr	sp,[r2,#PCB_STACKPTR]		;; \
	ldrb	r0,[r3,#SC_NESTED_IRQ_B]	;; \
	subs	r0,r0,#1			;; \
	strb	r0,[r3,#SC_NESTED_IRQ_B]	;; \
	ldreqb	r1,[r3,#SC_SCHEDULE_REQ_B]	;; \
	bne	irqepi0##label			;; \
	cmp	r1,#0				;; \
	bne	irqepi1##label			;;\
irqepi0##label:					;; \
	ldmia	sp!,{r0-r1}			;; \
	stmfd	r3,{r0-r1}			;; \
	ldmia	sp!,{r0-r3,r12,r14}		;; \
	msr	cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT	;; \
	ldr	r14,[sp,#0]			;; \
	msr	spsr,r14			;; \
	ldr	r14,[sp,#4]			;; \
	nop					;; \
	subs	pc,lr,#4			;;\
sciopta##label:					;; \
	LONG sciopta				;;\
irqepi1##label:					;; \
	ldrb	r0,[r2,#PCB_PRIO_B]		;; \
	orr	r0,r0,#0x80			;; \
	strb	r0,[r2,#PCB_PRIO_B]		;; \
	stmfd	sp!,{r4-r11}			;; \
	str	sp,[r2,#PCB_STACKPTR]		;; \
	bl	sc_sysProcDispatcher

#endif
