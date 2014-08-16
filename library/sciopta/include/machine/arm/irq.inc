
;********************************************************************
;  _______  _______ _________ _______  _______ _________ _______   **
; (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
; | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
; | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
; (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
;       ) || |         | |   | |   | || (         | |   | (   ) |  **
; /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
; \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
;                                                                  **
; (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
;                                                                  **
;********************************************************************
; ID: ---------                                                    **
; +Revision: 1.3 +                                                 **
; +Date: 2006/11/23 15:09:37 +                                     **
; Interrupt prologue/epilogue macros                               **
;********************************************************************
; generated

SC_CURPCB	EQU	(0x04)
SC_NESTED_IRQ_H	EQU	(0x14)
SC_SCHEDULER_LOCK	EQU	(0x18)
SC_SCHEDULE_REQ	EQU	(0x1c)
PCB_STACKPTR	EQU	72
PCB_STACKBTM	EQU	76
PCB_STACKTOP	EQU	80
 INCLUDE machine/arm/arm_asm.inc
 MACRO
 IRQ_PROLOGUE
 EXTERN sciopta 
 EXTERN sc_sysIrqDispatcher 
 sub r14,r14,#4 
 stmfd sp!,{r14} 
 mrs r14,spsr 
 stmfd sp!,{r14} 
 msr cpsr_c,#PSR_SYS_MODE|PSR_I_BIT 
 stmfd sp!,{r0-r3,r12,r14} 
 ldr r12,=sciopta 
 ldrh r14,[r12,#SC_NESTED_IRQ_H] 
 ldr r3,[r12,#SC_CURPCB] 
 add r14,r14,#1 
 strh r14,[r12,#SC_NESTED_IRQ_H] 
 str r13,[r3,#PCB_STACKPTR]
 MEND
 MACRO
$label IRQ_EPILOGUE
 EXTERN sciopta 
 EXTERN sc_sysProcDispatcher 
 ldr r12,=sciopta 
 msr cpsr_c,#PSR_SYS_MODE|PSR_I_BIT 
 ldrh r0,[r12,#SC_NESTED_IRQ_H] 
 ldr r3,[r12,#SC_CURPCB] 
 subs r0,r0,#1 
 strh r0,[r12,#SC_NESTED_IRQ_H] 
 ldr sp,[r3,#PCB_STACKPTR] 
 beq non_nested$label 
no_sched$label 
 ldmfd sp!,{r0-r3,r12,r14} 
 msr cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT 
 ldmfd sp!,{r14} 
 msr spsr_cxsf,r14 
 ldmfd sp!,{pc}^ 
non_nested$label 
 ldr r0,[r12,#SC_SCHEDULE_REQ] 
 cmp r0,#0 
 beq no_sched$label 
 ldr r0,[r12,#SC_SCHEDULER_LOCK] 
 cmp r0,#0 
 bne no_sched$label 
 msr cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT 
 ldmfd sp!,{r0,r1} 
 msr cpsr_c,#PSR_SYS_MODE|PSR_I_BIT 
 stmfd sp!,{r0-r1,r4-r11} 
 orr r2,sp,#1 
 str r2,[r3,#PCB_STACKPTR] 
 bl sc_sysProcDispatcher
 MEND
 MACRO
 IRQ_PROLOGUE_MMU
 EXTERN sciopta 
 EXTERN sc_sysIrqDispatcher 
 sub r14,r14,#4 
 stmfd sp!,{r14} 
 mrs r14,spsr 
 stmfd sp!,{r14} 
 msr cpsr_c,#PSR_SYS_MODE|PSR_I_BIT 
 stmfd sp!,{r0-r3,r12,r14} 
 ldr r12,=sciopta 
 ldrh r14,[r12,#SC_NESTED_IRQ_H] 
 ldr r3,[r12,#SC_CURPCB] 
 add r14,r14,#1 
 strh r14,[r12,#SC_NESTED_IRQ_H] 
 str r13,[r3,#PCB_STACKPTR]
 MEND
 MACRO
$label IRQ_EPILOGUE_MMU
 EXTERN sciopta 
 EXTERN sc_sysProcDispatcher 
 ldr r12,=sciopta 
 msr cpsr_c,#PSR_SYS_MODE|PSR_I_BIT 
 ldrh r0,[r12,#SC_NESTED_IRQ_H] 
 ldr r3,[r12,#SC_CURPCB] 
 subs r0,r0,#1 
 strh r0,[r12,#SC_NESTED_IRQ_H] 
 ldr sp,[r3,#PCB_STACKPTR] 
 beq non_nested$label 
no_sched$label 
 ldmfd sp!,{r0} 
 mcr p15,0,r0,c3,c0,0 
 ldmfd sp!,{r0-r3,r12,r14} 
 msr cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT 
 ldmfd sp!,{r14} 
 msr spsr_cxsf,r14 
 ldmfd sp!,{pc}^ 
non_nested$label 
 ldr r0,[r12,#SC_SCHEDULE_REQ] 
 cmp r0,#0 
 beq no_sched$label 
 ldr r0,[r12,#SC_SCHEDULER_LOCK] 
 cmp r0,#0 
 bne no_sched$label 
 msr cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT 
 ldmfd sp!,{r0,r1} 
 msr cpsr_c,#PSR_SYS_MODE|PSR_I_BIT 
 stmfd sp!,{r0-r1,r4-r11} 
 orr r2,sp,#1 
 str r2,[r3,#PCB_STACKPTR] 
 bl sc_sysProcDispatcher
 MEND
 MACRO
 IRQ_PROLOGUE_XSCALE
 EXTERN sciopta 
 EXTERN sc_sysIrqDispatcher 
 sub r14,r14,#4 
 stmfd sp!,{r14} 
 mrs r14,spsr 
 stmfd sp!,{r14} 
 msr cpsr_c,#PSR_SYS_MODE|PSR_I_BIT 
 stmfd sp!,{r0-r3,r12,r14} 
 mra r0,r1,acc0 
 stmfd sp!,{r0,r1} 
 ldr r12,=sciopta 
 ldrh r14,[r12,#SC_NESTED_IRQ_H] 
 add r14,r14,#1 
 strh r14,[r12,#SC_NESTED_IRQ_H] 
 ldr r14,[r12,#SC_CURPCB] 
 str r13,[r14,#PCB_STACKPTR]
 MEND
 MACRO
$label IRQ_EPILOGUE_XSCALE
 EXTERN sciopta 
 EXTERN sc_sysProcDispatcher 
 ldr r12,=sciopta 
 msr cpsr_c,#PSR_SYS_MODE|PSR_I_BIT 
 ldr r3,[r12,#SC_CURPCB] 
 ldrh r0,[r12,#SC_NESTED_IRQ_H] 
 subs r0,r0,#1 
 strh r0,[r12,#SC_NESTED_IRQ_H] 
 ldr sp,[r3,#PCB_STACKPTR] 
 beq non_nested$label 
no_sched$label 
 ldmfd sp!,{r0-r1} 
 mar acc0,r0,r1 
 ldmfd sp!,{r0-r3,r12,r14} 
 msr cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT 
 ldmfd sp!,{r14} 
 msr spsr_cxsf,r14 
 ldmfd sp!,{pc}^ 
non_nested$label 
 ldr r0,[r12,#SC_SCHEDULE_REQ] 
 cmp r0,#0 
 beq no_sched$label 
 ldr r0,[r12,#SC_SCHEDULER_LOCK] 
 cmp r0,#0 
 bne no_sched$label 
 ldr r3,[r12,#SC_CURPCB] 
 msr cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT 
 ldmfd sp!,{r0,r1} 
 msr cpsr_c,#PSR_SYS_MODE|PSR_I_BIT 
 stmfd sp!,{r0-r1,r4-r11} 
 orr r2,sp,#1 
 str r2,[r3,#PCB_STACKPTR] 
 bl sc_sysProcDispatcher
 MEND
 MACRO
 IRQ_PROLOGUE_XSCALE_MMU
 EXTERN sciopta 
 EXTERN sc_sysIrqDispatcher 
 sub r14,r14,#4 
 stmfd sp!,{r14} 
 mrs r14,spsr 
 stmfd sp!,{r14} 
 msr cpsr_c,#PSR_SYS_MODE|PSR_I_BIT 
 stmfd sp!,{r0-r3,r12,r14} 
 mra r0,r1,acc0 
 mrc p15,0,r2,c3,c0,0 
 stmfd sp!,{r0-r2} 
 ldr r12,=sciopta 
 ldrh r14,[r12,#SC_NESTED_IRQ_H] 
 add r14,r14,#1 
 strh r14,[r12,#SC_NESTED_IRQ_H] 
 ldr r14,[r12,#SC_CURPCB] 
 mov r0,#-1 
 mcr p15,0,r0,c3,c0,0 
 str r13,[r14,#PCB_STACKPTR]
 MEND
 MACRO
$label IRQ_EPILOGUE_XSCALE_MMU
 EXTERN sciopta 
 EXTERN sc_sysProcDispatcher 
 ldr r12,=sciopta 
 msr cpsr_c,#PSR_SYS_MODE|PSR_I_BIT 
 ldr r3,[r12,#SC_CURPCB] 
 ldrh r0,[r12,#SC_NESTED_IRQ_H] 
 subs r0,r0,#1 
 strh r0,[r12,#SC_NESTED_IRQ_H] 
 ldr sp,[r3,#PCB_STACKPTR] 
 beq non_nested$label 
no_sched$label 
 ldmfd sp!,{r0-r2} 
 mcr p15,0,r2,c3,c0,0 
 mar acc0,r0,r1 
 ldmfd sp!,{r0-r3,r12,r14} 
 msr cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT 
 ldmfd sp!,{r14} 
 msr spsr_cxsf,r14 
 ldmfd sp!,{pc}^ 
non_nested$label 
 ldr r0,[r12,#SC_SCHEDULE_REQ] 
 cmp r0,#0 
 beq no_sched$label 
 ldr r0,[r12,#SC_SCHEDULER_LOCK] 
 cmp r0,#0 
 bne no_sched$label 
 ldr r3,[r12,#SC_CURPCB] 
 msr cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT 
 ldmfd sp!,{r0,r1} 
 msr cpsr_c,#PSR_SYS_MODE|PSR_I_BIT 
 stmfd sp!,{r0-r1,r4-r11} 
 orr r2,sp,#1 
 str r2,[r3,#PCB_STACKPTR] 
 bl sc_sysProcDispatcher
 MEND
 END
