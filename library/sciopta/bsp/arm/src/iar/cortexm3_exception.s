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
**              (c) 2008 SCIOPTA Systems AG/ Schweiz                **
**                                                                  **
**********************************************************************
** ID: S08244BS3                                                    **
** +Revision: 1.3.2.1 +                                                 **
** +Date: 2009/03/27 10:57:36 +                                     **
** Exception handler for Cortex-M3                                  **
**********************************************************************
EOC*/

#define __ASM_INCLUDED__
#include "sconf.h"
#include <sciopta.h>

        
	SECTION .exception:CODE(2)

	PUBLIC	exception_handler
 
        IMPORT  sc_miscError
        THUMB
exception_handler
	ldr	r1,=reg_save_area
	str	r0,[r1,#4]
	str	r2,[r1,#12]
	str	r3,[r1,#16]
	ldr	r0,[sp,#4]
	str	r0,[r1,#8]
	add	r1,r1,#16
	stm	r1,{r3-r12}
	sub	r1,r1,#16
	str	r13,[r1,#14*4]
	ldr	r0,[sp,#5*4]	// last LR
	str	r0,[r1,#15*4]
	
        ldr     r0,[sp,#6*4]	// get exception address
	str	r0,[r1,#0]
	ldr	r0,=0xe000ed28
	ldr	r2,[r0,#0x34-0x28]
	str	r2,[r1,#16*4]
	ldr	r2,[r0,#0x38-0x28]
	str	r2,[r1,#17*4]
	ldr	r2,[r0,#0]
	str	r2,[r1,#18*4]
	
	mrs	r0,IPSR
	str	r0,[r1,#19*4]	// exception-number
		
	ldr	r0,=(0x01000000|KERNEL_EILL_EXCEPTION|SC_ERR_FATAL)
	b.w	sc_miscError

	SECTION BSS:DATA
	EXPORT	reg_save_area
reg_save_area:
	DS8	20*4
	END
