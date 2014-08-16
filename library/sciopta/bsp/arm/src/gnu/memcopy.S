/* -*-asm-*-
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
** ID: S06220BS1                                                    **
** +Revision: 1.2 +                                             **
** +Date: 2008/01/16 13:01:15 +                                     **
** Memcopy routine (based on ST code, see below)                    **
**********************************************************************
*/
	
/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
	 File Name		: memcopy.s				      *
* Date First Issued    : 03/08/2006					      *
* Author		   : MCD Application Team, San Jose, CA		  *
* Description		  : This file contains optimized memcopy functions	    *
*********************************************************************************
* History:								*
*  03/08/2006 : V0.1		Initially created			       *
*  03/09/2006 : V0.2		Additional functions				*
**********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
* CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
************************************************************************************/


	.text
	.code	32

	.globl	MEMCOPY_L2S_BY4
	.globl	MEMCOPY_S2L_BY4
	.globl	MEMCOPY_L2L_BY4



/* ///////////////////////////////////////////////////////////////////////////
; void	MEMCOPY_L2S_BY4 (void *pBUFOUT, void *pBUFIN, long nBytes)
;  Copy N bytes from a 32-bit aligned buffer to a 16-bit aligned buffer.
; Resources:
;  R0,R1,R2: parameters
;  R3-R10: locals
; Notes:
; - N Minimum is 16 bytes
; - pointer source and dest cannot be exactly the same;
;	 but an offset of -4 between dst and source will work 
;/////////////////////////////////////////////////////////////////////////// */

MEMCOPY_L2S_BY4:	
	STMFD	    SP!, {R4-R8,lr}

d_main:	
	LDMIA	    R1!,{R3,R5,R7,R12}
	MOV	    R4, R3, LSR #16
	MOV	    R6, R5, LSR #16
	MOV	    R8, R7, LSR #16
	MOV	    R14, R12, LSR #16
	STRH	    R3, [R0], #2
	STRH	    R4, [R0], #2
	STRH	    R5, [R0], #2
	STRH	    R6, [R0], #2
	STRH	    R7, [R0], #2
	STRH	    R8, [R0], #2
	STRH	    R12, [R0], #2
	STRH	    R14, [R0], #2
	SUB	    R2, R2, #16
	CMP	    R2,#16	      //if ( remaining_bytes >= 16)
	BHS	    d_main	      //keep on looping
				      //else switch(remaining_bytes) d_swit d_0123
	CMP	    R2,#3	      //remaining <= 3 bytes?
	BLS	    d_switch

d_4567:	

	LDR	    R3, [R1], #4
	MOV	    R4, R3, LSR #16
	STRH	    R3, [R0], #2
	STRH	    R4, [R0], #2
	SUB	    R2, R2, #4
	CMP	    R2,#3	    // remaining <= 3 bytes?
	BLS	    d_switch	    //	 switch(remaining_bytes 0,1,2,3)

d_891011:	

	LDR	    R3, [R1], #4
	MOV	    R4, R3, LSR #16
	STRH	    R3, [R0], #2
	STRH	    R4, [R0], #2
	SUB	    R2, R2, #4
	CMP	    R2,#3	  // remaining <= 3 bytes?
	BLS	    d_switch	  //    switch(remaining_bytes 0,1,2,3)

d_12131415:	

	LDR	    R3, [R1], #4
	MOV	    R4, R3, LSR #16
	STRH	    R3, [R0], #2
	STRH	    R4, [R0], #2
	SUB	    R2, R2, #4

d_switch:	
	LDR	PC,[PC, R2, LSL#2]
	nop
	.long    d_END
	.long    d_1B
	.long    d_2B
	.long    d_3B

d_1B:	
	LDRB	R3, [R1]
	STRB	R3, [R0]
	LDMFD	SP!, {R4-R8,lr}
	bx	lr

d_2B:	
	LDRH	R3, [R1]
	STRH	R3, [R0]
	LDMFD	SP!, {R4-R8,lr}
	bx	lr

d_3B:	
	LDRH	R3, [R1], #2
	LDRB	R2, [R1]
	STRH	R3, [R0], #2
	STRB	R2, [R0]
d_END:
	LDMFD	SP!, {R4-R8,lr}
	bx	lr


/* ;///////////////////////////////////////////////////////////////////////////
; void	MEMCOPY_L2L_BY4 (void *pBUFOUT, void *pBUFIN, long nBytes)
;  Copy N bytes from a 32-bit aligned buffer to a 32-bit aligned buffer.
; Resources:
;  R0,R1,R2: parameters
;  R3-R10: locals
; Notes:
; - N Minimum is 16 bytes
; - pointer source and dest cannot be exactly the same; but an offset of -4 between dst and source will work
;/////////////////////////////////////////////////////////////////////////// */

MEMCOPY_L2L_BY4:	
	STMFD	    SP!, {R4,lr}

e_main:	
	LDMIA	    R1!,{R3,R4,R12,r14}
	STMIA	    R0!,{R3,R4,r12,r14}
	SUB	    R2, R2, #16
	CMP	    R2,#16	    // if ( remaining_bytes >= 16)
	BHS	    e_main	    // keep on looping
				    // else switch(remaining_bytes)
e_0123:	
	CMP	    R2,#3	    // remaining <= 3 bytes?
	BLS	    e_switch
e_4567:	

	LDR	    R3, [R1], #4
	STR	    R3, [R0], #4
	SUB	    R2, R2, #4
	CMP	    R2,#3	      // remaining <= 3 bytes?
	BLS	    e_switch	      // switch(remaining_bytes 0,1,2,3)
e_891011:	
	LDR	    R3, [R1], #4
	STR	    R3, [R0], #4
	SUB	    R2, R2, #4
	CMP	    R2,#3	       // remaining <= 3 bytes?
	BLS	    e_switch	       //    switch(remaining_bytes 0,1,2,3)
e_12131415:	
	LDR	    R3, [R1], #4
	STR	    R3, [R0], #4
	SUB	    R2, R2, #4

e_switch:	
	LDR	PC,[PC, R2, LSL#2]
	nop
	.long	e_END
	.long	e_1B
	.long	e_2B
	.long	e_3B
e_1B:	
	LDRB	    R3, [R1]
	STRB	    R3, [R0]
	LDMFD	SP!, {R4,lr}
	bx	lr
e_2B:	
	LDRH	    R3, [R1]
	STRH	    R3, [R0]
	LDMFD	SP!, {R4,lr}
	bx	lr
e_3B:	
	LDRH	    R3, [R1], #2
	LDRB	    R2, [R1]
	STRH	    R3, [R0], #2
	STRB	    R2, [R0]
e_END:
	LDMFD	SP!, {R4,lr}
	bx	lr

/* ;///////////////////////////////////////////////////////////////////////////
; void	MEMCOPY_S2L_BY4 (void *pBUFOUT, void *pBUFIN, long nBytes)
;  Copy N bytes from a 32-bit aligned buffer to a 16-bit aligned buffer.
; Resources:
;  R0,R1,R2: parameters
;  R3-R10: locals
; Notes:
; - N Minimum is 16 bytes
; - pointer source and dest cannot be exactly the same; but an offset of -4 between dst and source will work
;/////////////////////////////////////////////////////////////////////////// */
MEMCOPY_S2L_BY4:	
	STMFD	    SP!, {R4-R8,lr}

f_main:	
	LDRH	    R3, [R1], #2
	LDRH	    R4, [R1], #2
	LDRH	    R5, [R1], #2
	LDRH	    R6, [R1], #2
	LDRH	    R7, [R1], #2
	LDRH	    R8, [R1], #2
	LDRH	    R12, [R1], #2
	LDRH	    R14, [R1], #2
	ORR	    R3,R3,R4,  LSL#16
	ORR	    R5,R5,R6,  LSL#16
	ORR	    R7,R7,R8,  LSL#16
	ORR	    R12,R12,R14, LSL#16
	STMIA	    R0!,{R3,R5,R7,R12}
	SUB	    R2, R2, #16
	CMP	    R2,#16	// if ( remaining_bytes >= 16)
	BHS	    f_main	// keep on looping
				// else switch(remaining_bytes)
f_swit:	
f_0123:	
	CMP	    R2,#3	 // remaining <= 3 bytes?
	BLS	    f_switch
f_4567:	
	LDRH	    R3, [R1], #2
	LDRH	    R4, [R1], #2
	ORR	    R3,R3,R4, LSL#16
	STR	    R3, [R0], #4
	SUB	    R2, R2, #4
	CMP	    R2,#3	 // remaining <= 3 bytes?
	BLS	    f_switch	 //    switch(remaining_bytes 0,1,2,3)
f_891011:	
	LDRH	    R3, [R1], #2
	LDRH	    R4, [R1], #2
	ORR	    R3,R3,R4, LSL#16
	STR	    R3, [R0], #4
	SUB	    R2, R2, #4
	CMP	    R2,#3	  // remaining <= 3 bytes?
	BLS	    f_switch	  //    switch(remaining_bytes 0,1,2,3)
f_12131415:	
	LDRH	    R3, [R1], #2
	LDRH	    R4, [R1], #2
	ORR	    R3,R3,R4, LSL#16
	STR	    R3, [R0], #4
	SUB	    R2, R2, #4

f_switch:
	LDR	PC,[PC, R2, LSL#2]
	nop
	.long	f_END
	.long	f_1B
	.long	f_2B
	.long	f_3B

f_1B:	
	LDRB	    R3, [R1]
	STRB	    R3, [R0]
	LDMFD	SP!, {R4-R8,lr}
	bx	lr
f_2B:	
	LDRH	    R3, [R1]
	STRH	    R3, [R0]
	LDMFD	SP!, {R4-R8,lr}
	bx	lr
f_3B:	
	LDRH	    R3, [R1], #2
	LDRB	    R4, [R1]
	STRH	    R3, [R0], #2
	STRB	    R4, [R0]
f_END:
	LDMFD	SP!, {R4-R8,lr}
	bx	lr


