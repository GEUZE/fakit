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
** ID: S08244BS2                                                    **
** +Revision: 1.2.6.1 +                                                 **
** +Date: 2009/02/17 14:11:33 +                                     **
** Vectortable for Cortex-M3                                        **
**********************************************************************
EOC*/

#include "sconf.h"
	
	.syntax	unified
	.section .reset_vector,"a"

	.globl	__vector_table
	
	.extern	_start
	.extern	exception_handler
	.extern	sc_sysIrqDispatcher
	.extern	sc_sysSVC
        .extern sc_sysPendSV
        .extern sc_svc_stack
__vector_table:       
/* 0*/	.long	sc_svc_stack
/* 1*/	.long	_start
/* 2*/	.long	exception_handler	/*nmi*/
/* 3*/	.long	exception_handler	/*hard fault*/
/* 4*/	.long	exception_handler	/*mmu*/
/* 5*/	.long	exception_handler	/*bus error*/
/* 6*/	.long	exception_handler	/*undef*/
/* 7*/	.long	exception_handler	/*reserved*/
/* 8*/	.long	exception_handler	/*reserved*/
/* 9*/	.long	exception_handler	/*reserved*/
/*10*/	.long	exception_handler	/*reserved*/
/*11*/	.long	sc_sysSVC		/*SVC*/
/*12*/	.long	exception_handler	/*debug monitor*/
/*13*/	.long	exception_handler	/*reserved*/
/*14*/	.long	sc_sysPendSV 	        /*PendSV*/
/*15*/	.long	sc_sysIrqDispatcher	/*SysTick*/
	.rept	SC_MAX_INT_VECTORS-16
	.long	sc_sysIrqDispatcher     /* MPU interrupts */
	.endr

	.end
