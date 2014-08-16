;;SOC -*-asm-*-
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
;; ID: S06057BS1                                                    **
;; +Revision: 1.2 +                                                 **
;; +Date: 2008/01/16 13:01:14 +                                     **
;; UART using DCC                                                   **
;;********************************************************************
;;EOC



	AREA dcc_output,CODE
	CODE32

	EXPORT	dcc_getchar
	EXPORT	dcc_getchar32
	CODE16
dcc_getchar PROC
	bx	pc
	nop
dcc_getchar32
	mrc	p14, 0, r1, c0, c0
	tst	r1,#1
	beq	dcc_getchar32
	
	mrc	p14, 0, r0, c1, c0
	bx	lr
	ENDP
		
	EXPORT	dcc_putchar
	EXPORT	dcc_putchar32
	CODE16
dcc_putchar PROC
	bx	pc
	nop
dcc_putchar32
	mrc	p14, 0, r1, c0, c0
	tst	r1,#2
	bne	dcc_putchar32
	
	mcr	p14, 0, r0, c1, c0
	bx	lr
	ENDP
	
	END
