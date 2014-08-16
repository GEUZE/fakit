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
**              (c) 2009 SCIOPTA Systems AG/ Schweiz                **
**                                                                  **
**********************************************************************
** ID: S05025BS1                                                    **
** +Revision: 1.7.18.4 +                                                 **
** +Date: 2009/10/19 07:03:11 +                                     **
** Interrupt macros                                                 **
**********************************************************************
EOC*/


#ifndef _SC_CONF_H_
#error Must include "sconf.h" in exception.S
#endif

#include <machine/ppc/ppc_asm.h>

#define SC_EXCEPTION TEXT(.exception)
#define SC_INIT TEXT(.init)
	
#ifdef __GNUC__
	.macro SET_PC val
	. = \val-0x200
	.endm

#endif
#ifdef __MWERKS__
	.macro SET_PC val
	.org \val-0x200
	.endm
#endif

/*
**********************************************
** Interrupt prologue to save current state **
**********************************************
*/
	.macro	IRQ_PROLOGUE
	addi	r1,r1,-148
	/*
	** save all volatile registers
	*/
	stw	r0,SAVED_R0(r1)
	stw	r2,SAVED_R2(r1)
	stw	r3,SAVED_R3(r1)
	stw	r4,SAVED_R4(r1)
	stw	r5,SAVED_R5(r1)	
	stw	r6,SAVED_R6(r1)
	stw	r7,SAVED_R7(r1)
	stw	r8,SAVED_R8(r1)
	stw	r9,SAVED_R9(r1)
	stw	r10,SAVED_R10(r1)
	stw	r11,SAVED_R11(r1)
	stw	r12,SAVED_R12(r1)
	stw	r13,SAVED_R13(r1)

	mfxer	r2
	mfctr	r3
	mfcr	r4
	mflr	r5
	mfspr	r6,srr1
	mfspr	r7,srr0
	stw	r2,SAVED_XER(r1)
	stw	r3,SAVED_CTR(r1)
	stw	r4,SAVED_CR(r1)
	stw	r5,SAVED_LR(r1)
	stw	r6,SAVED_MSR(r1)
	stw	r7,SAVED_PC(r1)

#if !(CPUTYPE == MPC5500 || (CPUTYPE >= AMCC440GP && CPUTYPE <= AMCC460EX))
	mfmsr	r2			// get current MSR
#if CPUTYPE == MPC8xx || CPUTYPE == MPC5200
	ori	r2,r2,MSR_RI|MSR_DR|MSR_IR // enable MMU => caches
#else
	ori	r2,r2,MSR_RI
#endif
	mtmsr	r2
	sync
#endif

	.endm

	.macro	IRQ_PROLOGUE_VLE
	e_add16i r1,r1,-148
	/*
	** save all volatile registers
	*/
	e_stw	r0,SAVED_R0(r1)
	se_stw	r2,SAVED_R2(r1)
	se_stw	r3,SAVED_R3(r1)
	se_stw	r4,SAVED_R4(r1)
	se_stw	r5,SAVED_R5(r1)	
	se_stw	r6,SAVED_R6(r1)
	se_stw	r7,SAVED_R7(r1)
	e_stw	r8,SAVED_R8(r1)
	e_stw	r9,SAVED_R9(r1)
	e_stw	r10,SAVED_R10(r1)
	e_stw	r11,SAVED_R11(r1)
	e_stw	r12,SAVED_R12(r1)
	e_stw	r13,SAVED_R13(r1)

	mfxer	r2
	mfctr	r3
	mfcr	r4
	mflr	r5
	mfspr	r6,srr1
	mfspr	r7,srr0
	se_stw	r2,SAVED_XER(r1)
	se_stw	r3,SAVED_CTR(r1)
	se_stw	r4,SAVED_CR(r1)
	se_stw	r5,SAVED_LR(r1)
	se_stw	r6,SAVED_MSR(r1)
	se_stw	r7,SAVED_PC(r1)
	.endm
