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
;; (c) 2006 Sciopta Systems GmbH, Sciopta AG/ Schweiz               **
;;                                                                  **
;;********************************************************************
;; ID: S06308BS2                                                    **
;; +Revision: 1.1 +                                                 **
;; +Date: 2006/11/23 15:23:03 +                                     **
;; Exception handler for ARM                                        **
;;********************************************************************

NATIVE_HOSTED	EQU 1		; for sciopta.cnf
ARM		EQU 1		; for sciopta.cnf
ARMv4T		EQU 1		; for sciopta.cnf

KERNEL_EILL_EXCEPTION EQU 0x17<<12 ; from kernel/err.h
SC_ERR_FATAL	EQU 7		 ; from kernel/err.h
	
 INCLUDE machine/arm/arm_asm.inc
 INCLUDE sciopta.cnf

	IMPORT	sc_svc_stack
 IF SC_CDEBUG = 1
	IMPORT	xsc_miscError
 ELSE
	IMPORT	sc_miscError
 ENDIF
	
	AREA user_swi_code,CODE
	CODE32
	EXPORT user_swi_handler
user_swi_handler PROC
	mrs	r12,spsr		;; get saved CPU state
	tst	r12,#0x20		;; is it thumb mode ?
	
        ldrneh  r12,[lr,#-2]            ;; yes, swi only 16bit
        bicne   r12,r12,#0xff00
        
	ldreq   r12,[lr,#-4]            ;; no, swi 32bit
        biceq   r12,r12,#0xff000000
	
	sub	r12,r12,#4		;; 0..3 reserved for sciopta
	cmp	r12,#8
	ldrls	pc,[pc,r12, lsl #2]
	b	ush_error

	DCD	0,0,0,0
	DCD	0,0,0,0

ush_error
	ldr	r0,=(0x05000000|KERNEL_EILL_EXCEPTION|SC_ERR_FATAL)
	mov	r1,lr
 IF SC_CDEBUG = 1
	mov	r2,#68 		; line number
	ldr	r3,=ush_filename
        ldr     r4,=xsc_miscError
 ELSE
        ldr     r4,=sc_miscError
 ENDIF
        bx      r4
exception1
	b	exception1

	ENDP

	AREA	excpetion_text,CODE
 IF SC_CDEBUG = 1
ush_filename DATA
	DCB	"user_swi_handler.s"
 ENDIF
	ALIGN
	
        END
