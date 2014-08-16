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
** ID:                                                              **
** +Revision: 1.2 +                                                 **
** +Date: 2008/11/11 10:13:08 +                                     **
** crt0 for ARM GCC                                                 **
**********************************************************************
*/

	.syntax	unified
	.extern	main
	.globl	cstartup

	.extern	__section_init
	.extern __section_init_size

	.text
	.thumb
	.thumb_func

	.type	cstartup,function
cstartup:
	ldr	r0,=__section_init
	ldr	r1,=__section_init_size
	lsrs	r1,r1,#5	
	ldr	r7,=main
	it	eq
	bxeq	r7
	mov	r6,#0
cs0:
	ldmia	r0!,{r2,r3,r4}
	cmp	r2,r3		/* src == dst ? */
	beq	cs2
cs1:
	subs	r4,r4,#4
	itt	pl
	ldrpl	r5,[r2],#4
	strpl	r5,[r3],#4
	bpl	cs1
cs2:
	ldmia	r0!,{r2,r3,r4}
	cmp	r2,r3
	beq	cs4
cs3:
	subs	r4,r4,#4
	itt	pl
	ldrpl	r5,[r2],#4
	strpl	r5,[r3],#4
	bpl	cs3
cs4:
	ldmia	r0!,{r2,r4}
cs5:
	subs	r4,r4,#4
	it	pl
	strpl	r6,[r2],#4
	bpl	cs5

	subs	r1,r1,#1
	bne	cs0

	bx	r7

	.ltorg
    .END

