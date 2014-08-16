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
;; ID: S04198BS1                                                    **
;; +Revision: 1.6 +                                                 **
;; +Date: 2008/02/14 14:15:42 +                                     **
;; crt0 for ARM ADS                                                 **
;;********************************************************************

	;; Currently no module-setup prepared
MODULE_SETUP EQU 1

	EXTERN	main
	EXPORT	__main

	IMPORT	|Load$$system_init$$Base|[WEAK]
	IMPORT	|Image$$system_init$$Base|[WEAK]
	IMPORT	|Image$$system_init$$Length|[WEAK]
	IMPORT	|Image$$system_init$$ZI$$Base|[WEAK]
	IMPORT	|Image$$system_init$$ZI$$Length|[WEAK]
	IMPORT	|Image$$system_endmark$$ZI$$Limit|[WEAK]
		
	IMPORT	|Load$$dev_init$$Base|[WEAK]
	IMPORT	|Image$$dev_init$$Base|[WEAK]
	IMPORT	|Image$$dev_init$$Length|[WEAK]
	IMPORT	|Image$$dev_init$$ZI$$Base|[WEAK]
	IMPORT	|Image$$dev_init$$ZI$$Length|[WEAK]
	IMPORT	|Image$$dev_endmark$$ZI$$Limit|[WEAK]	

	IMPORT	|Load$$ips_init$$Base|[WEAK]
	IMPORT	|Image$$ips_init$$Base|[WEAK]
	IMPORT	|Image$$ips_init$$Length|[WEAK]
	IMPORT	|Image$$ips_init$$ZI$$Base|[WEAK]
	IMPORT	|Image$$ips_init$$ZI$$Length|[WEAK]
	IMPORT	|Image$$ips_endmark$$ZI$$Limit|[WEAK]	

	IMPORT	|Load$$user_init$$Base|[WEAK]
	IMPORT	|Image$$user_init$$Base|[WEAK]
	IMPORT	|Image$$user_init$$Length|[WEAK]
	IMPORT	|Image$$user_init$$ZI$$Base|[WEAK]
	IMPORT	|Image$$user_init$$ZI$$Length|[WEAK]
	IMPORT	|Image$$user_endmark$$ZI$$Limit|[WEAK]	
	
	AREA |!!!main|,CODE,READONLY
	CODE32
	ALIGN 2
__main	PROC	

	adr	r10,section_init
	adr	r11,section_init_end
	mov	r12,#0
cs0
	ldmia	r10!,{r0-r4}
	;; 
	;; ROM->RAM copy
	;; 
	cmp	r0,r1
	moveq	r2,#0
cs1
	subs	r2,r2,#4
	ldrpl	r9,[r0],#4
	strpl	r9,[r1],#4
	bpl	cs1
	
; --- clear bss
	
cs2
	subs	r4,r4,#4
	strpl	r12,[r3],#4
	bpl	cs2

	cmp	r10,r11
	bne	cs0

;;; ------------------------------------
;;; -- setup Module-info array
;;; ------------------------------------
 IF MODULE_SETUP = 1
	ldr	r10,=system_mod
	adr	r11,module_init
	adr	r12,module_init_end
cs3
	ldmia	r11!,{r0-r2}
	subs	r1,r1,r0	; module size
	movmi	r1,#0
	stmia	r10!,{r0-r2}
	cmp	r11,r12
	bne	cs3
 ENDIF
; Jump to Main ()

	EXPORT	__rt_entry
__rt_entry
	IMPORT	__rt_lib_init
	mov	r0,#0
	mov	r1,#0		; no heap !
	bl	__rt_lib_init
        ldr     r3,=main
        bx  r3
	LTORG
	
section_init
	DCD	|Load$$system_init$$Base|   ; addr. in ROM
	DCD	|Image$$system_init$$Base|  ; addr. in RAM
	DCD	|Image$$system_init$$Length|
	DCD	|Image$$system_init$$ZI$$Base|
	DCD	|Image$$system_init$$ZI$$Length|
	
	DCD	|Load$$dev_init$$Base|   ; addr. in ROM
	DCD	|Image$$dev_init$$Base|  ; addr. in RAM
	DCD	|Image$$dev_init$$Length|
	DCD	|Image$$dev_init$$ZI$$Base|
	DCD	|Image$$dev_init$$ZI$$Length|

	DCD	|Load$$ips_init$$Base|   ; addr. in ROM
	DCD	|Image$$ips_init$$Base|  ; addr. in RAM
	DCD	|Image$$ips_init$$Length|
	DCD	|Image$$ips_init$$ZI$$Base|
	DCD	|Image$$ips_init$$ZI$$Length|

	DCD	|Load$$user_init$$Base|   ; addr. in ROM
	DCD	|Image$$user_init$$Base|  ; addr. in RAM
	DCD	|Image$$user_init$$Length|
	DCD	|Image$$user_init$$ZI$$Base|
	DCD	|Image$$user_init$$ZI$$Length|

section_init_end
 IF MODULE_SETUP = 1
module_init
	DCD	|Image$$system_init$$Base|
	DCD	|Image$$system_endmark$$ZI$$Limit|
	DCD	|Image$$system_init$$Length|+|Image$$system_init$$ZI$$Length|

	DCD	|Image$$dev_init$$Base|
	DCD	|Image$$dev_endmark$$ZI$$Limit|
	DCD	|Image$$dev_init$$Length|+|Image$$dev_init$$ZI$$Length|

	DCD	|Image$$ips_init$$Base|
	DCD	|Image$$ips_endmark$$ZI$$Limit|
	DCD	|Image$$ips_init$$Length|+|Image$$ips_init$$ZI$$Length|

	DCD	|Image$$user_init$$Base|
	DCD	|Image$$user_endmark$$ZI$$Limit|
	DCD	|Image$$user_init$$Length|+|Image$$user_init$$ZI$$Length|

module_init_end
 ENDIF
	ENDP

 IF MODULE_SETUP = 1
	AREA module_setup_section,NOINIT
	EXPORT module_setup
	EXPORT system_mod
	EXPORT dev_mod
	EXPORT ips_mod
	EXPORT user_mod
module_setup
system_mod	SPACE 3*4
dev_mod		SPACE 3*4
ips_mod		SPACE 3*4
user_mod	SPACE 3*4

 ENDIF
    END

