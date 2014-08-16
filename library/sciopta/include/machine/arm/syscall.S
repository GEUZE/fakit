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
** +Revision: 1.4 +                                                 **
** +Date: 2008/01/16 13:02:02 +                                     **
** Trampoline functions for system-calls                            **
**********************************************************************
EOC*/
#include "sconf.h"
#include <machine/syscall.h>
		
#ifdef __GNUC__

#if SC_CDEBUG == 1
#define syscall(name)\
	.globl __##name		; \
	.thumb_func		; \
__##name:			; \
        push {r4-r6}		; \
	swi 1			; \
	.short	__SC_##name,0	; \
                                ; \
	.globl __x##name	; \
	.thumb_func		; \
__x##name:			; \
        push {r4-r6}		; \
	swi 1			; \
	.short	__SC_##name|128,0
#else	
#define syscall(name)\
	.globl __##name		; \
	.thumb_func		; \
__##name:			; \
        push {r4-r6}		; \
	swi 1			; \
	.short	__SC_##name,0
#endif	
	.section .text.krn.trampoline
	.code	16
	
#include <machine/syscall.S>
#endif /* __GNUC__ */
