/* -*-asm-*-
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
*/



	.text	
	.globl	dcc_getchar
	.globl	dcc_getchar32
	.code	16
	.thumb_func
dcc_getchar:
	bx	pc
	nop
	.code	32
dcc_getchar32:
	mov	r2,#0xf000000
dcc_getchar32a:
	subs	r2,r2,#1
	bxeq	lr
	mrc	p14, 0, r1, c0, c0
	tst	r1,#1
	beq	dcc_getchar32a
	
	mrc	p14, 0, r0, c1, c0
	bx	lr
	.ltorg
	
	.code	16
	.globl	dcc_putchar
	.thumb_func
dcc_putchar:
	bx	pc
	nop
	.globl	dcc_putchar32
        .code   32
dcc_putchar32:
	mov	r2,#0xf00000
dcc_putchar32a:
	subs	r2,r2,#1
	bxeq	lr
	mrc	p14, 0, r1, c0, c0
	tst	r1,#2
	bne	dcc_putchar32a
	
	mcr	p14, 0, r0, c1, c0
	bx	lr
