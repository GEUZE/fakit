/*SOC -*-c++-*-
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
** (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
**                                                                  **
**********************************************************************
** ID: S03090BS1                                                    **
** +Revision: 1.5.12.1 +                                                 **
** +Date: 2009/09/16 18:25:58 +                                     **
** Trampoline functions for system-calls                            **
**********************************************************************
EOC*/
#include "sconf.h"
#define __ASM_INCLUDED__
#include <machine/syscall.h>
		
#if defined(__GNUC__) || defined(__DCC__)

#if SC_CDEBUG == 1
#define syscall(name)\
   .globl __##name		; \
__##name:			; \
	addi 0,0,__SC_##name	; \
	sc			; \
   .globl __x##name		; \
__x##name:			; \
	addi 0,0,__SC_##name	; \
	oris 0,0,0x8000	; \
	sc			; \
	nop
#else	
#define syscall(name)\
   .globl __##name		; \
__##name:			; \
	addi 0,0,__SC_##name	; \
	sc
#endif	
	.section .text.krn.trampoline

#define syscall1 syscall
#define syscall2 syscall
#define syscall3 syscall
#define syscall4 syscall

#include <machine/syscall.S>
#endif /* __GNUC__ || __DCC__ */

