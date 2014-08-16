;;********************************************************************
;;  _______  _______ _________ _______  _______ _________ _______   **
;; (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
;; | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
;; | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
;; (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
;;       ) || |         | |   | |   | || (         | |   | (   ) |  **
;; /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
;; \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
;;                                                                  **
;; (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
;;                                                                  **
;;********************************************************************
;; ID: S06291BS1                                                    **
;; +Revision: 1.4 +                                                 **
;; +Date: 2008/11/11 10:13:07 +                                     **
;; Exception handler for ARM                                        **
;;********************************************************************

NATIVE_HOSTED	EQU 1		; for sciopta.cnf
ARM		EQU 1		; for sciopta.cnf
ARMv4T		EQU 1		; for sciopta.cnf
ARMv5T		EQU 2		; for sciopta.cnf
ARMv5TE		EQU 3		; for sciopta.cnf

KERNEL_EILL_EXCEPTION EQU 0x17<<12 ; from kernel/err.h
SC_ERR_FATAL	EQU 7		 ; from kernel/err.h
	
 INCLUDE machine/arm/arm_asm.inc
 INCLUDE sciopta.cnf
	
	AREA exception_code,CODE

	IMPORT	sc_svc_stack
 IF SC_CDEBUG = 1
	IMPORT	xsc_miscError
 ELSE
	IMPORT	sc_miscError
 ENDIF

exception_handler PROC
	CODE32
	EXPORT Abort_Handler
Abort_Handler
	push	{r0-r1}		; save r0/r1 on exception stack
	mov	r0,#0x01000000
	b	exception

	EXPORT	Undefined_Handler
Undefined_Handler
	push	{r0-r1}		; save r0/r1 on exception stack
	mov	r0,#0x02000000
	b	exception
	
	EXPORT	Prefetch_Handler
Prefetch_Handler
	push	{r0-r1}		; save r0/r1 on exception stack
	mov	r0,#0x03000000
	b	exception
	
	EXPORT 	FIQ_Handler
FIQ_Handler
	push	{r0-r1}		; save r0/r1 on exception stack
	mov	r0,#0x04000000


exception
	sub	lr,lr,#4		;; adjust LR
	ldr	r1,=(reg_save_area+12)
	stmia	r1,{r2-r14}^		;; save USR/SYS-mode registers
	nop
	pop	{r3-r4}			;; get saved r0/r1
	mov	r2,lr
	stmdb	r1!,{r2-r4}
	
	mov	r2,r0			;; save cause
	
	;; switch to svc mode and set stack
	msr	cpsr_c,#PSR_SVC_MODE|PSR_I_BIT|PSR_F_BIT
	ldr	sp,=sc_svc_stack

 IF SC_PROCMMU_HOOK == 1	
	mov	r0,#-1			;; activate all domains
	mcr	p15,0,r0,c3,c0,0

	mrc	p15,0,r3,c6,c0,0	;; get FAR
	str	r0,[r1,#16*4]
	
	mrc	p15,0,r0,c5,c0,0	;; get fault status
	str	r0,[r1,#17*4]
 ENDIF	

	mov	r0,#KERNEL_EILL_EXCEPTION
	orr	r0,r0,#SC_ERR_FATAL
	orr	r0,r0,r2
 IF SC_CDEBUG == 1
	IMPORT	sciopta
SC_CURPCB  EQU 4
PCB_CLINE  EQU 120
PCB_CFILE  EQU 124
	
	ldr	r2,=sciopta
	ldr	r2,[r2,#SC_CURPCB]
	cmp	r2,#0	
	moveq	r2,#0
	ldreq	r3,=filename
	ldrne	r3,[r2,#PCB_CFILE]
	ldrne	r2,[r2,#PCB_CLINE]
	
        ldr     r4,=xsc_miscError
 ELSE
        ldr     r4,=sc_miscError
 ENDIF
        bx      r4

	ENDP

	AREA	excpetion_text,CODE
 IF SC_CDEBUG = 1
filename DATA
	DCB	"exception.s"
 ENDIF

	ALIGN

	AREA	exception_bss,DATA
	EXPORT	reg_save_area
reg_save_area
	SPACE	16*4
 IF SC_PROCMMU_HOOK == 1
	SPACE	2*4
 ENDIF
        END
