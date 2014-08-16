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
** +Revision: 1.2.4.1 +                                                 **
** +Date: 2009/02/17 14:11:33 +                                     **
** Vectortable for Cortex-M3                                        **
**********************************************************************
EOC*/

#include "sconf.h"

	SECTION	.intvec:CONST(10)

	EXPORT	__vector_table
	
	IMPORT	irq_stacke

	IMPORT	_start
	IMPORT	exception_handler
	IMPORT	sc_sysIrqDispatcher
	IMPORT	sc_sysSVC
        IMPORT  sc_svc_stack
        IMPORT  sc_sysPendSV
	
__vector_table:       
/* 0*/	DCD	sc_svc_stack
/* 1*/	DCD	_start
/* 2*/	DCD	exception_handler	/*nmi*/
/* 3*/	DCD	exception_handler	/*hard fault*/
/* 4*/	DCD	exception_handler	/*mmu*/
/* 5*/	DCD	exception_handler	/*bus error*/
/* 6*/	DCD	exception_handler	/*undef*/
/* 7*/	DCD	exception_handler	/*reserved*/
/* 8*/	DCD	exception_handler	/*reserved*/
/* 9*/	DCD	exception_handler	/*reserved*/
/*10*/	DCD	exception_handler	/*reserved*/
/*11*/	DCD	sc_sysSVC		/*SVC*/
/*12*/	DCD	exception_handler	/*debug monitor*/
/*13*/	DCD	exception_handler	/*reserved*/
/*14*/	DCD	sc_sysPendSV 	        /*PendSV*/
/*15*/	DCD	sc_sysIrqDispatcher	/*SysTick*/
	REPT	SC_MAX_INT_VECTORS-16
	DCD	sc_sysIrqDispatcher     /* MPU interrupts */
	ENDR

	END
