/*   
**********************************************************************
**
**  _______  _______ _________ _______  _______ _________ _______
** (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )
** | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |
** | (_____ | |         | |   | |   | || (____)|   | |   | (___) |
** (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |
**       ) || |         | |   | |   | || (         | |   | (   ) |
** /\____) || (____/\___) (___| (___) || )         | |   | )   ( |
** \_______)(_______/\_______/(_______)|/          )_(   |/     \|
**                            _  _     
**                           |_||_)|V|
**                           | || \| |
**
** Direct Message Passing Real Time Kernel
**
**              (c) 2009 SCIOPTA Systems AG/ Schweiz
**
**********************************************************************
** Sciopta Kernel for ARM
** Revision: 1.9.6.8
** Date: 19-11-2009
*********************************************************************
**
** kernel.S             1.57.2.9 
** kernel_v7m.S         1.4.2.9 
** module.S             1.41.2.2 
** misc.S               1.23.2.2 
** msg.S                1.48.2.12 
** addrget.S            1.9.2.1 
** sizeget.S            1.9.2.1 
** sizeset.S            1.12.2.3 
** sndget.S             1.8.2.1 
** ownerget.S           1.8.2.1 
** poolget.S            1.14.2.1 
** proc.S               1.81.2.10 
** pool.S               1.31.2.3 
** tick.S               1.39.2.5 
** tmo.S                1.15.2.1 
** trigger.S            1.18.2.10 
** procd.S              1.11.2.1 
** kerneld.S            1.10.2.1 
** connector.S          1.13.2.3 
** syscall.S            1.6 
** iar.S                1.6 
**
*/
#define THUMB_MODE
#define __ASM_INCLUDED__
#include "sciopta.cnf"
#include <sciopta.h>
#include <sciopta.msg>
#include <machine/endian.h>
#if BYTE_ORDER == BIG_ENDIAN
#undef LITTLE_ENDIAN
LITTLE_ENDIAN = 0
#else
#undef LITTLE_ENDIAN
LITTLE_ENDIAN = 1
#endif
#define SC_USE_RP 0
#define SCVER 0x1,0x9,0x6,0x8
#define FAKE_INT swi 0
#ifdef __GNUC__
 .syntax unified
#define NEG(r) rsbs r,r,#0
#define IF .if
#define ELSE .else
#define ENDIF .endif
#define LONG .long
#define WORD .short
#define BYTE .byte
#define LALIGN .align 2
#define SPACE .space
#define CODE16 .code 16
#define CODE32 .code 32
#define LTORG .pool 
#define EXPORT .globl
#define EXTERN .extern
#define IMPORT .extern
#define WEAK .weak
#define ENTRY
#define LEXTERN(a)
#define DATA
#define EQU =
dbl_prev = 0
dbl_next = 4
 .macro SC_BSS
 .section .sc_bss,"aw",%nobits
 .endm
 .macro SC_FUNC name 
 .section .text.krn.\name,"ax"
 .type \name,function
 .globl \name
 .align 2
 IF CPUTYPE <= Xscale
 .code 32
 ELSE
 .thumb
 .thumb_func
 ENDIF
\name:
 .endm
 .macro SC_PUBLIC name
 .globl \name
 IF CPUTYPE <= Xscale
 .code 32
 ELSE
 .thumb
 .thumb_func
 ENDIF
\name:
 .endm
 .macro SC_TPUBLIC name
 .globl \name
 IF CPUTYPE <= Xscale
 .code 16
 ELSE
 .thumb
 ENDIF
 .thumb_func
\name:
 .endm
 .macro SC_TFUNC name
 .section .text.krn.\name,"ax"
 .type \name,function
 .globl \name
 .align 2
 IF CPUTYPE <= Xscale
 .code 16
 ELSE
 .thumb
 ENDIF
 .thumb_func
\name:
 .endm
 .macro SC_ENDFUNC name
 .align 2
 .pool
 .size \name,.-\name
 .endm
 .macro LSC_TFUNC name,para
 IF SC_LONGJUMP == 1
 .section .text.longkrn.\name,"ax"
 .type l\name,function
 .globl l\name
 .align 2
 IF CPUTYPE <= Xscale
 .code 16
 ELSE
 .thumb
 ENDIF
 .thumb_func
l\name:
 IF \para > 4
 push {r0}
 ldr r0,llit1\name
 mov r12,r0
 pop {r0}
 bx r12
 ELSE
 ldr r3,llit1\name
 bx r3
 ENDIF
 ENDIF
llit1\name: LONG \name
 .size l\name,.-l\name
 .endm
 .macro XSC_TFUNC name,off
 IF SC_CDEBUG == 1
 .section .text.debkrn.\name,"ax"
 .type x\name,function
 .globl x\name
 .align 2
 IF CPUTYPE <= Xscale
 .code 16
 ELSE
 .thumb
 ENDIF
 .thumb_func
x\name:
 IF \off >= 0
 push {r0-r1}
 ldr r0,xlit\name
 ldr r0,[r0,#SC_CURPCB]
 ldr r1,[sp,#8+\off]
 cmp r0,#0
 beq 999f
 str r1,[r0,#PCB_CLINE]
 ldr r1,[sp,#8+\off+4]
 str r1,[r0,#PCB_CFILE]
999: 
 ldr r0,xlit1\name
 mov r12,r0
 pop {r0-r1}
 bx r12
 ENDIF
 IF \off == -4
 mov r12,r0
 ldr r0,xlit\name
 ldr r0,[r0,#SC_CURPCB]
 cmp r0,#0
 beq 999f
 str r3,[r0,#PCB_CLINE]
 ldr r3,[sp,#0]
 str r3,[r0,#PCB_CFILE]
999: 
 ldr r3,xlit1\name
 mov r0,r12
 bx r3
 ENDIF
 IF \off == -8
 mov r12,r0
 ldr r0,xlit\name
 ldr r0,[r0,#SC_CURPCB]
 cmp r0,#0
 beq 999f
 str r2,[r0,#PCB_CLINE]
 str r3,[r0,#PCB_CFILE]
999: 
 ldr r3,xlit1\name
 mov r0,r12
 bx r3
 ENDIF 
 IF \off == -12
 ldr r3,xlit\name
 ldr r3,[r3,#SC_CURPCB]
 cmp r3,#0
 beq 999f
 str r1,[r3,#PCB_CLINE] 
 str r2,[r3,#PCB_CFILE]
999:
 ldr r3,xlit1\name
 bx r3
 ENDIF
 IF \off == -16
 ldr r2,xlit\name
 ldr r2,[r2,#SC_CURPCB]
 cmp r2,#0
 beq 999f
 str r0,[r2,#PCB_CLINE] 
 str r1,[r2,#PCB_CFILE]
999: 
 ldr r3,xlit1\name
 bx r3
 ENDIF
 LALIGN
xlit\name: LONG sciopta
xlit1\name: LONG \name
 .size x\name,.-x\name
 ENDIF 
 .endm
 .macro XXSC_TFUNC name,off
 IF SC_CDEBUG == 1
 .section .text.debkrn.x\name,"ax"
 .type xx\name,function
 .globl xx\name
 .align 2
 IF CPUTYPE <= Xscale
 .code 16
 ELSE
 .thumb
 ENDIF
 .thumb_func
xx\name:
 IF \off >= 0
 push {r0-r1}
 ldr r0,xxlit\name
 ldr r0,[r0,#SC_CURPCB]
 ldr r1,[sp,#20+\off]
 cmp r0,#0
 beq 999f
 str r1,[r0,#PCB_CLINE]
 ldr r1,[sp,#20+\off+4]
 str r1,[r0,#PCB_CFILE]
999: 
 ldr r0,xxlit1\name
 mov r12,r0
 pop {r0-r1}
 bx r12
 ENDIF
 IF \off == -4
 mov r12,r0
 ldr r0,xxlit\name
 ldr r0,[r0,#SC_CURPCB]
 cmp r0,#0
 beq 999f
 str r3,[r0,#PCB_CLINE]
 ldr r3,[sp,#12]
 str r3,[r0,#PCB_CFILE]
999: 
 ldr r3,xxlit1\name
 mov r0,r12
 bx r3
 ENDIF
 IF \off < -4
 .error 10
 ENDIF
 LALIGN
xxlit\name: LONG sciopta
xxlit1\name: LONG \name
 .size xx\name,.-xx\name
 ENDIF 
 .endm
 .macro RET_R0
 IF CPUTYPE >= ARMv5T
 pop {r0,pc}
 ELSE
 pop {r0,r1}
 bx r1
 ENDIF
 .endm
 .macro RET mask
 IF CPUTYPE >= ARMv5T
 pop {\mask,pc}
 ELSE
 pop {\mask}
 pop {r3}
 bx r3
 ENDIF
 .endm
 .macro ARET mask
 IF CPUTYPE >= ARMv5T
 ldmfd sp!,{\mask,pc}
 ELSE
 ldmfd sp!,{\mask,lr}
 bx lr
 ENDIF
 .endm
 .macro EXIT
 IF CPUTYPE >= ARMv5T
 pop {pc}
 ELSE
 pop {r3}
 bx r3
 ENDIF
 .endm
 .macro AEXIT
 IF CPUTYPE >= ARMv5T
 ldmfd sp!,{pc}
 ELSE
 ldmfd sp!,{lr}
 bx lr
 ENDIF
 .endm
 .macro SWAP_OUT
 IF CPUTYPE > Xscale
 bl swap_out
 ELSE
 IF CPUTYPE >= ARMv5T
 blx swap_out32
 ELSE
 bl swap_out
 ENDIF 
 ENDIF
 .endm
 .macro SWAP_OUT_SAVE mask
 push \mask
 IF CPUTYPE > Xscale
 bl swap_out
 ELSE
 IF CPUTYPE >= ARMv5T
 blx swap_out32
 ELSE
 bl swap_out
 ENDIF
 ENDIF
 pop \mask
 .endm
 .macro LIT name,value
 LALIGN
\name: .long \value
 .endm
 .macro SET name,value
\name = \value
 .endm
 .macro BLX_R2_t
 IF CPUTYPE >= ARMv5T
 blx r2
 ELSE
 bl via_r2_t
 ENDIF
 .endm
 .macro BLX_R2_a
 IF CPUTYPE >= ARMv5T
 blx r2
 ELSE
 mov lr,pc
 bx r2
 ENDIF
 .endm
 .macro BLXEQ_R2_a
 IF CPUTYPE >= ARMv5T
 blxeq r2
 ELSE
 moveq lr,pc
 bxeq r2
 ENDIF
 .endm
 .macro BLXNE_R2_a
 IF CPUTYPE > Xscale
 it ne
 blxne r2
 ELSE
 IF CPUTYPE >= ARMv5T
 blxne r2
 ELSE
 movne lr,pc
 bxne r2
 ENDIF
 ENDIF
 .endm
 .macro BLX_R12_a
 IF CPUTYPE >= ARMv5T
 blx r12
 ELSE
 mov lr,pc
 bx r12
 ENDIF
 .endm
 .macro rezi x,r,tmp1,tmp2
 mov \tmp1,\x
 cmp \tmp1,#0x00010000
 movlo \tmp1,\tmp1,lsl #16
 cmp \tmp1,#0x01000000
 movlo \tmp1,\tmp1,lsl #8
 cmp \tmp1,#0x10000000
 movlo \tmp1,\tmp1,lsl #4
 cmp \tmp1,#0x40000000
 movlo \tmp1,\tmp1,lsl #2
 cmp \tmp1,#0x80000000
 movlo \tmp1,\tmp1,lsl #1
 mov \r,#1
 rsc \tmp2,\tmp1,#0
 mov \tmp1,\tmp1,lsr #1
1:
 cmp \tmp2,\tmp1
 adc \r,\r,\r
 subcs \tmp2,\tmp2,\tmp1
 cmp \x,\tmp1 
 movne \tmp1,\tmp1,lsr #1
 bne 1b
 cmp \tmp1,#0
 addne \r,\r,#1
 .endm
 .macro LOCK
 IF CPUTYPE > Xscale
 bl cpu_lock_r12
 ELSE
 IF CPUTYPE >= ARMv5T
 blx cpu_lock_r12_a
 ELSE
 bl cpu_lock_r12
 ENDIF
 ENDIF
 .endm
 .macro UNLOCK
 IF CPUTYPE > Xscale
 bl cpu_unlock_r12
 ELSE
 IF CPUTYPE >= ARMv5T
 blx cpu_unlock_r12_a
 ELSE
 bl cpu_unlock_r12
 ENDIF
 ENDIF
 .endm
#endif 
#ifdef __ADS__ 
 IF :DEF: BIG_ENDIAN
LITTLE_ENDIAN EQU 0
 ELSE
LITTLE_ENDIAN EQU 1
 ENDIF
#define NEG(r) rsbs r,r,#0
#define LONG DCD
#define WORD DCW
#define BYTE DCB
#define LALIGN ALIGN
#define LEXTERN(a)
 MACRO 
 WEAK $name
 EXTERN $name[WEAK]
 MEND
 MACRO
 SC_TEXT
 AREA |.text.krn|,CODE,READONLY,ALIGN=2
 CODE32
 MEND
 MACRO 
 SC_BSS
 AREA |sc_bss|,NOINIT,ALIGN=3
 MEND
 MACRO 
$label SC_FUNC $name
 AREA sciopta_$name,CODE,ALIGN=2
 IF CPUTYPE <= Xscale
 CODE32
 ELSE
 THUMB
 ENDIF
$name PROC
 EXPORT $name
 MEND
 MACRO 
 SC_PUBLIC $name
 EXPORT $name
 IF CPUTYPE <= Xscale
 CODE32
 ELSE
 THUMB
 ENDIF
$name
 MEND
 MACRO 
 SC_TPUBLIC $name
 EXPORT $name
 IF CPUTYPE <= Xscale
 CODE16
 ELSE
 THUMB
 ENDIF
$name
 MEND
 MACRO 
 SC_TFUNC $name
 AREA sciopta_$name,CODE,ALIGN=2
 IF CPUTYPE <= Xscale
 CODE16
 ELSE
 THUMB
 ENDIF
$name PROC
 EXPORT $name
 MEND
 MACRO
 LSC_TFUNC $name,$para
 IF SC_LONGJUMP = 1
 AREA sciopta_l$name,CODE,ALIGN=2
 CODE16
l$name PROC
 EXPORT l$name
 IF $para > 4
 push {r0}
 ldr r0,llit$name
 mov r12,r0
 pop {r0}
 bx r12
 ELSE
 ldr r3,llit$name
 bx r3
 ENDIF
llit$name DCD $name
 ENDIF
 MEND
 MACRO
 XSC_TFUNC $name,$off
 IF SC_CDEBUG = 1
 AREA sciopta_x$name,CODE,ALIGN=2
 IF CPUTYPE <= Xscale
 CODE16
 ELSE
 THUMB
 ENDIF
x$name PROC
 EXPORT x$name
 IF $off = -4
 mov r12,r0
 ldr r0,xlit$name
 ldr r0,[r0,#SC_CURPCB]
 cmp r0,#0
 beq m1$name
 str r3,[r0,#120]
 ldr r3,[sp,#0]
 str r3,[r0,#124]
m1$name
 ldr r3,xlit1$name
 mov r0,r12
 bx r3
 ELSE
 IF $off = -8
 mov r12,r0
 ldr r0,xlit$name
 ldr r0,[r0,#SC_CURPCB]
 cmp r0,#0
 beq m1$name
 str r2,[r0,#120]
 str r3,[r0,#124]
m1$name
 ldr r2,xlit1$name
 mov r0,r12
 bx r2
 ELSE
 IF $off = -12
 ldr r3,xlit$name
 ldr r3,[r3,#SC_CURPCB]
 cmp r3,#0
 beq m1$name
 str r1,[r3,#120]
 str r2,[r3,#124]
m1$name
 ldr r3,xlit1$name
 bx r3
 ELSE
 IF $off = -16
 ldr r2,xlit$name
 ldr r2,[r2,#SC_CURPCB]
 cmp r2,#0
 beq m1$name
 str r0,[r2,#120]
 str r1,[r2,#124]
m1$name
 ldr r2,xlit1$name
 bx r2
 ELSE
 push {r0-r1}
 ldr r0,xlit$name
 ldr r0,[r0,#SC_CURPCB]
 cmp r0,#0
 beq m1$name
 ldr r1,[sp,#8+$off]
 str r1,[r0,#120]
 ldr r1,[sp,#8+$off+4]
 str r1,[r0,#124]
m1$name
 ldr r0,xlit1$name
 mov r12,r0
 pop {r0-r1}
 bx r12
 ENDIF
 ENDIF
 ENDIF
 ENDIF
 ALIGN 4
xlit$name DCD sciopta
xlit1$name DCD $name
 ENDP
 ENDIF
 MEND
 MACRO
 XXSC_TFUNC $name,$off
 IF SC_CDEBUG = 1
 AREA sciopta_xx$name,CODE,ALIGN=2
 IF CPUTYPE <= Xscale
 CODE16
 ELSE
 THUMB
 ENDIF
xx$name PROC
 EXPORT x$name
 IF $off = -4
 mov r12,r0
 ldr r0,xxlit$name
 ldr r0,[r0,#SC_CURPCB]
 cmp r0,#0
 beq m2$name
 str r3,[r0,#120]
 ldr r3,[sp,#12]
 str r3,[r0,#124]
m2$name
 ldr r3,xxlit1$name
 mov r0,r12
 bx r3
 ELSE
 IF $off >= 0
 push {r0-r1}
 ldr r0,xxlit$name
 ldr r0,[r0,#SC_CURPCB]
 cmp r0,#0
 beq m2$name
 ldr r1,[sp,#20+$off]
 str r1,[r0,#120]
 ldr r1,[sp,#20+$off+4]
 str r1,[r0,#124]
m2$name
 ldr r0,xxlit1$name
 mov r12,r0
 pop {r0-r1}
 bx r12
 ELSE
 ERROR
 ENDIF
 ENDIF
 ALIGN 4
xxlit$name DCD sciopta
xxlit1$name DCD $name
 ENDP
 ENDIF
 MEND
 MACRO
 SC_ENDFUNC $name
 ALIGN 4
 LTORG
 ENDP
 MEND
 MACRO 
 RET_R0
 IF CPUTYPE >= ARMv5T
 pop {r0,pc}
 ELSE
 pop {r0,r1}
 bx r1
 ENDIF
 MEND
 MACRO 
 RET $mask
 IF CPUTYPE >= ARMv5T
 pop {$mask,pc}
 ELSE
 pop {$mask}
 pop {r3}
 bx r3
 ENDIF
 MEND
 MACRO 
 ARET $mask
 IF CPUTYPE >= ARMv5T
 ldmfd sp!,{$mask,pc}
 ELSE
 ldmfd sp!,{$mask,lr}
 bx lr
 ENDIF
 MEND
 MACRO 
 EXIT
 IF CPUTYPE >= ARMv5T
 pop {pc}
 ELSE
 pop {r3}
 bx r3
 ENDIF
 MEND
 MACRO 
 AEXIT
 IF CPUTYPE >= ARMv5T
 ldmfd sp!,{pc}
 ELSE
 ldmfd sp!,{lr}
 bx lr
 ENDIF
 MEND
 MACRO 
 SWAP_OUT
 IF CPUTYPE > Xscale
 bl swap_out
 ELSE
 IF CPUTYPE >= ARMv5T
 blx swap_out32
 ELSE
 bl swap_out
 ENDIF 
 ENDIF
 MEND
 MACRO 
 SWAP_OUT_SAVE $mask
 push $mask
 IF CPUTYPE > Xscale
 bl swap_out
 ELSE
 IF CPUTYPE >= ARMv5T
 blx swap_out32
 ELSE
 bl swap_out
 ENDIF
 ENDIF
 pop $mask
 MEND
 MACRO 
 LIT $name,$value
 ALIGN
$name DCD $value
 MEND
 MACRO 
 SET $name,$value
$name EQU $value
 MEND
 MACRO 
 BLX_R2_t
 IF CPUTYPE >= ARMv5T
 blx r2
 ELSE
 bl via_r2_t
 ENDIF
 MEND
 MACRO 
 BLX_R2_a
 IF CPUTYPE >= ARMv5T
 blx r2
 ELSE
 mov lr,pc
 bx r2
 ENDIF
 MEND
 MACRO 
 BLXEQ_R2_a
 IF CPUTYPE >= ARMv5T
 blxeq r2
 ELSE
 moveq lr,pc
 bxeq r2
 ENDIF
 MEND
 MACRO 
 BLXNE_R2_a
 IF CPUTYPE >= ARMv5T
 blxne r2
 ELSE
 movne lr,pc
 bxne r2
 ENDIF
 MEND
 MACRO 
 BLX_R12_a
 IF CPUTYPE >= ARMv5T
 blx r12
 ELSE
 mov lr,pc
 bx r12
 ENDIF
 MEND
dbl_prev EQU 0
dbl_next EQU 4
 MACRO 
 LOCK
 IF CPUTYPE > Xscale
 bl cpu_lock_r12
 ELSE
 IF CPUTYPE >= ARMv5T
 blx cpu_lock_r12_a
 ELSE
 bl cpu_lock_r12
 ENDIF
 ENDIF
 MEND
 MACRO 
 UNLOCK
 IF CPUTYPE > Xscale
 bl cpu_unlock_r12
 ELSE
 IF CPUTYPE >= ARMv5T
 blx cpu_unlock_r12_a
 ELSE
 bl cpu_unlock_r12
 ENDIF
 ENDIF
 MEND
#endif 
#if 0
#define ALOCK(reg) \
 mrs reg,cpsr ;; \
 stmfd sp!,{reg} ;; \
 orr reg,reg,#PSR_I_BIT;; \
 msr cpsr_c,reg
#define AUNLOCK(reg) \
 ldmfd sp!,{reg} ;; \
 msr cpsr_c,reg
#endif
#ifdef __IAR__
 RTMODEL "__cpu_mode","__pcs__interwork"
 RTMODEL "__endian","little"
 RTMODEL "__rt_version","6"
#define LONG DC32
#define WORD DCW
#define BYTE DCB
#define SPACE DS8
#define WEAK IMPORT
#define ENTRY
#define LEXTERN(a) IMPORT a
#define NEG(r) neg r,r
dbl_prev EQU 0
dbl_next EQU 4
LALIGN MACRO
 ALIGNROM 2
 DATA
 ENDM
SC_BSS MACRO 
 RSEG DATA_Z
 ENDM
SC_FUNC MACRO
 RSEG CODE:CODE:NOROOT(2)
 EXPORT \1
 ALIGNROM 2
 CODE32
\1: 
 ENDM
SC_PUBLIC MACRO
 EXPORT \1
 CODE32
\1: 
 ENDM
SC_TPUBLIC MACRO
 EXPORT \1
 CODE16
\1:
 ENDM
SC_TFUNC MACRO 
 RSEG CODE:CODE:NOROOT(2)
 EXPORT \1
 CODE16
\1: 
 ENDM
SC_ENDFUNC MACRO
 ALIGNROM 2
 LTORG
\1_size: EQU . - \1
 ENDM
XSC_TFUNC MACRO name,off
 LOCAL x1,xname,xsc
 IF SC_CDEBUG = 1
 RSEG CODE:CODE:NOROOT(2)
 EXPORT x\1
 ALIGNROM 2
 CODE16
x\1:
 IF off >= 0
 push {r0-r1}
 ldr r0,xsc
 ldr r0,[r0,#SC_CURPCB]
 ldr r1,[sp,#8+off]
 cmp r0,#0
 beq x1
 str r1,[r0,#PCB_CLINE]
 ldr r1,[sp,#8+off+4]
 str r1,[r0,#PCB_CFILE]
x1: 
 ldr r0,xname
 mov r12,r0
 pop {r0-r1}
 bx r12
 ENDIF
 IF off == -4
 mov r12,r0
 ldr r0,xsc
 ldr r0,[r0,#SC_CURPCB]
 cmp r0,#0
 beq x1
 str r3,[r0,#PCB_CLINE]
 ldr r3,[sp,#0]
 str r3,[r0,#PCB_CFILE]
x1: 
 ldr r3,xname
 mov r0,r12
 bx r3
 ENDIF
 IF off == -8
 mov r12,r0
 ldr r0,xsc
 ldr r0,[r0,#SC_CURPCB]
 cmp r0,#0
 beq x1
 str r2,[r0,#PCB_CLINE]
 str r3,[r0,#PCB_CFILE]
x1: 
 ldr r3,xname
 mov r0,r12
 bx r3
 ENDIF 
 IF off == -12
 ldr r3,xsc
 ldr r3,[r3,#SC_CURPCB]
 cmp r3,#0
 beq x1
 str r1,[r3,#PCB_CLINE] 
 str r2,[r3,#PCB_CFILE]
x1: 
 ldr r3,xname
 bx r3
 ENDIF
 IF off == -16
 ldr r2,xsc
 ldr r2,[r2,#SC_CURPCB]
 cmp r2,#0
 beq x1
 str r0,[r2,#PCB_CLINE] 
 str r1,[r2,#PCB_CFILE]
x1: 
 ldr r3,xname
 bx r3
 ENDIF
 LALIGN
xname: DC32 \1
xsc DC32 sciopta
 ENDIF 
 ENDM
XXSC_TFUNC MACRO name,off
 LOCAL x1,xname,xsc
 IF SC_CDEBUG = 1
 RSEG CODE:CODE:NOROOT(2)
 EXPORT xx\1
 ALIGNROM 2
 CODE16
xx\1:
 IF off >= 0
 push {r0-r1}
 ldr r0,xsc
 ldr r0,[r0,#SC_CURPCB]
 ldr r1,[sp,#20+off]
 cmp r0,#0
 beq x1
 str r1,[r0,#PCB_CLINE]
 ldr r1,[sp,#20+off+4]
 str r1,[r0,#PCB_CFILE]
x1: 
 ldr r0,xname
 mov r12,r0
 pop {r0-r1}
 bx r12
 ENDIF
 IF off == -4
 mov r12,r0
 ldr r0,xsc
 ldr r0,[r0,#SC_CURPCB]
 cmp r0,#0
 beq x1
 str r3,[r0,#PCB_CLINE]
 ldr r3,[sp,#12]
 str r3,[r0,#PCB_CFILE]
x1: 
 ldr r3,xname
 mov r0,r12
 bx r3
 ENDIF
 LALIGN
xname: DC32 \1
xsc DC32 sciopta
 ENDIF 
 ENDM
RET_R0 MACRO
 IF CPUTYPE >= ARMv5T
 pop {r0,pc}
 ELSE
 pop {r0,r1}
 bx r1
 ENDIF
 ENDM
RET MACRO mask
 IF CPUTYPE >= ARMv5T
 pop {mask,pc}
 ELSE
 pop {mask}
 pop {r3}
 bx r3
 ENDIF
 ENDM
ARET MACRO mask
 IF CPUTYPE >= ARMv5T
 ldmfd sp!,{mask,pc}
 ELSE
 ldmfd sp!,{mask,lr}
 bx lr
 ENDIF
 ENDM
EXIT MACRO
 IF CPUTYPE >= ARMv5T
 pop {pc}
 ELSE
 pop {r3}
 bx r3
 ENDIF
 ENDM
AEXIT MACRO
 IF CPUTYPE >= ARMv5T
 ldmfd sp!,{pc}
 ELSE
 ldmfd sp!,{lr}
 bx lr
 ENDIF
 ENDM
SWAP_OUT MACRO
 IF CPUTYPE >= ARMv5T
 blx swap_out32
 ELSE
 bl swap_out
 ENDIF 
 ENDM
SWAP_OUT_SAVE MACRO mask
 push mask
 IF CPUTYPE >= ARMv5T
 blx swap_out32
 ELSE
 bl swap_out
 ENDIF
 pop mask
 ENDM
BLX_R2_t MACRO 
 IF CPUTYPE >= ARMv5T
 blx r2
 ELSE
 bl via_r2_t
 ENDIF
 ENDM
BLX_R2_a MACRO 
 IF CPUTYPE >= ARMv5T
 blx r2
 ELSE
 mov lr,pc
 bx r2
 ENDIF
 ENDM
BLXNE_R2_a MACRO
 IF CPUTYPE >= ARMv5T
 blxne r2
 ELSE
 movne lr,pc
 bxne r2
 ENDIF
 ENDM
LOCK MACRO
 IF CPUTYPE >= ARMv5T
 blx cpu_lock_r12_a
 ELSE
 bl cpu_lock_r12
 ENDIF
 ENDM
BLX_R12_a MACRO 
 IF CPUTYPE >= ARMv5T
 blx r12
 ELSE
 mov lr,pc
 bx r12
 ENDIF
 ENDM
UNLOCK MACRO
 IF CPUTYPE >= ARMv5T
 blx cpu_unlock_r12_a
 ELSE
 bl cpu_unlock_r12
 ENDIF
 ENDM
#endif 
#ifdef __IAR5__
 RTMODEL "__cpu_mode","__pcs__interwork"
 RTMODEL "__endian","little"
 RTMODEL "__rt_version","6"
#define LONG DC32
#define WORD DCW
#define BYTE DCB
#define SPACE DS8
#define WEAK IMPORT
#define ENTRY
#define LEXTERN(a) IMPORT a
#define NEG(r) rsbs r,r,#0
dbl_prev EQU 0
dbl_next EQU 4
LALIGN MACRO
 ALIGNROM 2
 DATA
 ENDM
SC_BSS MACRO
 SECTION .noinit:DATA
 ENDM
SC_FUNC MACRO
 SECTION .text_krn:CODE(2)
 EXPORT \1
 ALIGNROM 2
 IF CPUTYPE <= Xscale
 CODE32
 ELSE
 THUMB
 ENDIF
\1: 
 ENDM
SC_PUBLIC MACRO
 EXPORT \1
 IF CPUTYPE <= Xscale
 CODE32
 ELSE
 THUMB
 ENDIF
\1: 
 ENDM
SC_TPUBLIC MACRO
 EXPORT \1
 THUMB
\1:
 ENDM
SC_TFUNC MACRO 
 SECTION .text_krn_\1:CODE:NOROOT(2)
 EXPORT \1
 THUMB
\1: 
 ENDM
SC_ENDFUNC MACRO
 ALIGNROM 2
 LTORG
\1_size: EQU . - \1
 ENDM
XSC_TFUNC MACRO name,off
 LOCAL x1,xname,xsc
 IF SC_CDEBUG = 1
 SECTION .debug_\1:CODE:NOROOT(2)
 EXPORT x\1
 THUMB 
 IF CPUTYPE <= Xscale
x\1:
 IF off >= 0
 push {r0-r1}
 ldr r0,xsc
 ldr r0,[r0,#SC_CURPCB]
 ldr r1,[sp,#8+off]
 cmp r0,#0
 beq x1
 str r1,[r0,#PCB_CLINE]
 ldr r1,[sp,#8+off+4]
 str r1,[r0,#PCB_CFILE]
x1: 
 ldr r0,xname
 mov r12,r0
 pop {r0-r1}
 bx r12
 ENDIF
 IF off == -4
 mov r12,r0
 ldr r0,xsc
 ldr r0,[r0,#SC_CURPCB]
 cmp r0,#0
 beq x1
 str r3,[r0,#PCB_CLINE]
 ldr r3,[sp,#0]
 str r3,[r0,#PCB_CFILE]
x1: 
 ldr r3,xname
 mov r0,r12
 bx r3
 ENDIF
 IF off == -8
 mov r12,r0
 ldr r0,xsc
 ldr r0,[r0,#SC_CURPCB]
 cmp r0,#0
 beq x1
 str r2,[r0,#PCB_CLINE]
 str r3,[r0,#PCB_CFILE]
x1: 
 ldr r3,xname
 mov r0,r12
 bx r3
 ENDIF 
 IF off == -12
 ldr r3,xsc
 ldr r3,[r3,#SC_CURPCB]
 cmp r3,#0
 beq x1
 str r1,[r3,#PCB_CLINE] 
 str r2,[r3,#PCB_CFILE]
x1: 
 ldr r3,xname
 bx r3
 ENDIF
 IF off == -16
 ldr r2,xsc
 ldr r2,[r2,#SC_CURPCB]
 cmp r2,#0
 beq x1
 str r0,[r2,#PCB_CLINE] 
 str r1,[r2,#PCB_CFILE]
x1: 
 ldr r3,xname
 bx r3
 ENDIF
 ELSE
x\1:
 IF off >= 0
 push {r0-r1}
 ldr r0,xsc
 ldr r0,[r0,#SC_CURPCB]
 cmp r0,#0
 itttt ne
 ldrne r1,[sp,#8+off]
 strne r1,[r0,#PCB_CLINE]
 ldrne r1,[sp,#8+off+4]
 strne r1,[r0,#PCB_CFILE]
 ldr r12,xname
 pop {r0-r1}
 bx r12
 ENDIF
 IF off == -4
 mov r12,r0
 ldr r0,xsc
 cmp r0,#0
 itttt ne
 ldrne r0,[r0,#SC_CURPCB]
 strne r3,[r0,#PCB_CLINE]
 ldrne r3,[sp,#0]
 strne r3,[r0,#PCB_CFILE]
 ldr r3,xname
 mov r0,r12
 bx r3
 ENDIF
 IF off == -8
 mov r12,r0
 ldr r0,xsc
 ldr r0,[r0,#SC_CURPCB]
 cmp r0,#0
 itt ne
 strne r2,[r0,#PCB_CLINE]
 strne r3,[r0,#PCB_CFILE]
 ldr r3,xname
 mov r0,r12
 bx r3
 ENDIF 
 IF off == -12
 ldr r3,xsc
 ldr r3,[r3,#SC_CURPCB]
 cmp r3,#0
 itt ne
 strne r1,[r3,#PCB_CLINE] 
 strne r2,[r3,#PCB_CFILE]
 ldr r3,xname
 bx r3
 ENDIF
 IF off == -16
 ldr r2,xsc
 ldr r2,[r2,#SC_CURPCB]
 cmp r2,#0
 itt ne
 strne r0,[r2,#PCB_CLINE] 
 strne r1,[r2,#PCB_CFILE]
 ldr r3,xname
 bx r3
 ENDIF
 ENDIF
 LALIGN
xname: DC32 \1
xsc DC32 sciopta
 ENDIF 
 ENDM
XXSC_TFUNC MACRO name,off
 LOCAL x1,xname,xsc
 IF SC_CDEBUG = 1
 SECTION .text.x\1:CODE:NOROOT(2)
 EXPORT xx\1
 ALIGNROM 2
 THUMB
xx\1:
 IF off >= 0
 push {r0-r1}
 ldr r0,xsc
 ldr r0,[r0,#SC_CURPCB]
 ldr r1,[sp,#20+off]
 cmp r0,#0
 beq x1
 str r1,[r0,#PCB_CLINE]
 ldr r1,[sp,#20+off+4]
 str r1,[r0,#PCB_CFILE]
x1: 
 ldr r0,xname
 mov r12,r0
 pop {r0-r1}
 bx r12
 ENDIF
 IF off == -4
 mov r12,r0
 ldr r0,xsc
 ldr r0,[r0,#SC_CURPCB]
 cmp r0,#0
 beq x1
 str r3,[r0,#PCB_CLINE]
 ldr r3,[sp,#12]
 str r3,[r0,#PCB_CFILE]
x1: 
 ldr r3,xname
 mov r0,r12
 bx r3
 ENDIF
 ENDIF 
 LALIGN
xname: DC32 \1
xsc DC32 sciopta
 ENDIF 
 ENDM
RET_R0 MACRO
 IF CPUTYPE >= ARMv5T
 pop {r0,pc}
 ELSE
 pop {r0,r1}
 bx r1
 ENDIF
 ENDM
RET MACRO mask
 IF CPUTYPE >= ARMv5T
 pop {mask,pc}
 ELSE
 pop {mask}
 pop {r3}
 bx r3
 ENDIF
 ENDM
ARET MACRO mask
 IF CPUTYPE >= ARMv5T
 ldmfd sp!,{mask,pc}
 ELSE
 ldmfd sp!,{mask,lr}
 bx lr
 ENDIF
 ENDM
EXIT MACRO
 IF CPUTYPE >= ARMv5T
 pop {pc}
 ELSE
 pop {r3}
 bx r3
 ENDIF
 ENDM
AEXIT MACRO
 IF CPUTYPE >= ARMv5T
 ldmfd sp!,{pc}
 ELSE
 ldmfd sp!,{lr}
 bx lr
 ENDIF
 ENDM
SWAP_OUT MACRO
 IF CPUTYPE > Xscale
 bl swap_out
 ELSE
 IF CPUTYPE >= ARMv5T
 blx swap_out32
 ELSE
 bl swap_out
 ENDIF
 ENDIF
 ENDM
SWAP_OUT_SAVE MACRO mask
 push mask
 IF CPUTYPE > Xscale
 bl swap_out
 ELSE
 IF CPUTYPE >= ARMv5T
 blx swap_out32
 ELSE
 bl swap_out
 ENDIF
 ENDIF
 pop mask
 ENDM
BLX_R2_t MACRO 
 IF CPUTYPE >= ARMv5T
 blx r2
 ELSE
 bl via_r2_t
 ENDIF
 ENDM
BLX_R2_a MACRO 
 IF CPUTYPE >= ARMv5T
 blx r2
 ELSE
 mov lr,pc
 bx r2
 ENDIF
 ENDM
BLXNE_R2_a MACRO
 IF CPUTYPE > Xscale
 it ne
 blxne r2
 ELSE
 IF CPUTYPE >= ARMv5T
 blxne r2
 ELSE
 movne lr,pc
 bxne r2
 ENDIF
 ENDIF
 ENDM
LOCK MACRO
 IF CPUTYPE > Xscale
 bl cpu_lock_r12
 ELSE
 IF CPUTYPE >= ARMv5T
 blx cpu_lock_r12_a
 ELSE
 bl cpu_lock_r12
 ENDIF
 ENDIF
 ENDM
UNLOCK MACRO
 IF CPUTYPE > Xscale
 bl cpu_unlock_r12
 ELSE
 IF CPUTYPE >= ARMv5T
 blx cpu_unlock_r12_a
 ELSE
 bl cpu_unlock_r12
 ENDIF
 ENDIF
 ENDM
BLX_R12_a MACRO
 IF CPUTYPE >= ARMv5T
 blx r12
 ELSE
 mov lr,pc
 bx r12
 ENDIF
 ENDM
#endif 
#define dbl_tailAdd(head,item,tmp)\
 str head,[item,#dbl_next] ;; \
 ldr tmp,[head,#dbl_prev] ;; \
 str item,[head,#dbl_prev] ;; \
 str tmp,[item,#dbl_prev] ;; \
 str item,[tmp,#dbl_next]
#define dbl_headDel(head,item,tmp,label) \
 movs item,#0 ;; \
 ldr tmp,[head,#dbl_next] ;; \
 cmp tmp,head ;; \
 beq label##hd999 ;; \
 movs item,tmp ;; \
 ldr tmp,[tmp,#dbl_next] ;; \
 str tmp,[head,#dbl_next] ;; \
 str head,[tmp,#dbl_prev] ;; \
 movs tmp,#0 ;; \
 str tmp,[item,#dbl_next] ;; \
 str tmp,[item,#dbl_prev] ;;\
label##hd999: 
#define dbl_del(pos,tmp1,tmp2) \
 ldr tmp1,[pos,#dbl_next] ;; \
 ldr tmp2,[pos,#dbl_prev] ;; \
 str tmp1,[tmp2,#dbl_next] ;; \
 str tmp2,[tmp1,#dbl_prev]
#define _makeReady(pcb,tmp1,tmp2,tmp3,varbase,label) \
 ldrh tmp1,[pcb,#PCB_FLAGS_H] ;; \
 movs tmp2,#PCB_FLG_READY ;; \
 tst tmp1,tmp2 ;; \
 bne label##mr9999 ;; \
 orrs tmp1,tmp1,tmp2 ;; \
 strh tmp1,[pcb,#PCB_FLAGS_H] ;; \
 ldrb tmp1,[pcb,#PCB_PRIO_B] ;; \
 movs tmp2,#1 ;; \
 lsls tmp2,tmp2,tmp1 ;; \
 ldr tmp3,[varbase,#SC_PRIOMASK] ;; \
 orrs tmp3,tmp3,tmp2 ;; \
 str tmp3,[varbase,#SC_PRIOMASK] ;; \
 lsls tmp1,tmp1,#3 ;; \
 movs tmp2,#SC_READYLIST ;; \
 adds tmp1,tmp1,tmp2 ;; \
 adds tmp1,tmp1,varbase ;; \
 adds pcb,pcb,#PCB_PCBLIST ;; \
 dbl_tailAdd(tmp1,pcb,tmp2) ;; \
 subs pcb,pcb,#PCB_PCBLIST ;;\
label##mr9999:
#define _makeNotReady(pcb,tmp1,tmp2,varbase,label) \
 ldrh tmp1,[pcb,#PCB_FLAGS_H] ;; \
 movs tmp2,#PCB_FLG_READY ;; \
 tst tmp1,tmp2 ;; \
 beq label##9999 ;; \
 bics tmp1,tmp1,tmp2 ;; \
 strh tmp1,[pcb,#PCB_FLAGS_H] ;; \
 ldr tmp1,[pcb,#PCB_PCBLIST+dbl_next] ;; \
 ldr tmp2,[pcb,#PCB_PCBLIST+dbl_prev] ;; \
 str tmp1,[tmp2,#dbl_next] ;; \
 str tmp2,[tmp1,#dbl_prev] ;; \
 cmp tmp1,tmp2 ;; \
 bne label##9998 ;; \
 ldrb tmp1,[pcb,#PCB_PRIO_B] ;; \
 movs tmp2,#1 ;; \
 lsls tmp2,tmp2,tmp1 ;; \
 ldr tmp1,[varbase,#SC_PRIOMASK] ;; \
 bics tmp1,tmp1,tmp2 ;; \
 str tmp1,[varbase,#SC_PRIOMASK] ;;\
label##9998: ;; \
 movs tmp1,#0 ;; \
 str tmp1,[pcb,#PCB_PCBLIST+dbl_next] ;; \
 str tmp1,[pcb,#PCB_PCBLIST+dbl_prev] ;;\
label##9999:
#define prioMask(pcb,prio,tmp1,varbase) \
 strb prio,[pcb,#PCB_PRIO_B] ;; \
 movs tmp1,#1 ;; \
 IF CPUTYPE >= ARMv5T ;; \
 lsls tmp1,tmp1,#31 ;; \
 lsrs tmp1,tmp1,prio ;; \
 ELSE ;; \
 lsls tmp1,tmp1,prio ;; \
 ENDIF ;; \
 str tmp1,[pcb,#PCB_PRIO_MASK] ;; \
 lsls prio,prio,#3 ;; \
 movs tmp1,#SC_READYLIST ;; \
 adds tmp1,tmp1,prio ;; \
 adds tmp1,tmp1,varbase ;; \
 str tmp1,[pcb,#PCB_READYLIST]
#define makeReady(pcb,tmp1,tmp2,tmp3,varbase,label) \
 ldrh tmp1,[pcb,#PCB_FLAGS_H] ;; \
 movs tmp2,#PCB_FLG_READY ;; \
 tst tmp1,tmp2 ;; \
 bne label##mr9999 ;; \
 orrs tmp1,tmp1,tmp2 ;; \
 strh tmp1,[pcb,#PCB_FLAGS_H] ;; \
 ldr tmp3,[varbase,#SC_PRIOMASK] ;; \
 ldr tmp2,[pcb,#PCB_PRIO_MASK] ;; \
 ldr tmp1,[pcb,#PCB_READYLIST] ;; \
 orrs tmp3,tmp3,tmp2 ;; \
 str tmp3,[varbase,#SC_PRIOMASK] ;; \
 adds pcb,pcb,#PCB_PCBLIST ;; \
 dbl_tailAdd(tmp1,pcb,tmp2) ;; \
 subs pcb,pcb,#PCB_PCBLIST ;;\
label##mr9999:
#define makeNotReady(pcb,tmp1,tmp2,varbase,label) \
 ldrh tmp1,[pcb,#PCB_FLAGS_H] ;; \
 movs tmp2,#PCB_FLG_READY ;; \
 tst tmp1,tmp2 ;; \
 beq label##9999 ;; \
 bics tmp1,tmp1,tmp2 ;; \
 strh tmp1,[pcb,#PCB_FLAGS_H] ;; \
 ldr tmp1,[pcb,#PCB_PCBLIST+dbl_next] ;; \
 ldr tmp2,[pcb,#PCB_PCBLIST+dbl_prev] ;; \
 str tmp1,[tmp2,#dbl_next] ;; \
 str tmp2,[tmp1,#dbl_prev] ;; \
 cmp tmp1,tmp2 ;; \
 bne label##9998 ;; \
 ldr tmp2,[pcb,#PCB_PRIO_MASK] ;; \
 ldr tmp1,[varbase,#SC_PRIOMASK] ;; \
 bics tmp1,tmp1,tmp2 ;; \
 str tmp1,[varbase,#SC_PRIOMASK] ;;\
label##9998: ;; \
 movs tmp1,#0 ;; \
 str tmp1,[pcb,#PCB_PCBLIST+dbl_next] ;; \
 str tmp1,[pcb,#PCB_PCBLIST+dbl_prev] ;;\
label##9999:
#define enterTimer(pcb,tmo,idx,tmp2,varbase,label) \
 ldr idx,[varbase,#SC_TCK] ;; \
 adds idx,idx,tmo ;; \
 lsrs tmp2,tmo,#18 ;; \
 bne label##992 ;; \
 lsrs tmp2,tmo,#12 ;; \
 bne label##991 ;; \
 lsrs tmp2,tmo,#6 ;; \
 bne label##990 ;; \
 movs tmp2,#0x3f ;; \
 ands tmp2,tmp2,idx ;; \
 movs tmo,#0 ;; \
 b label##996 ;;\
label##990: ;; \
 movs tmo,#0x3f ;; \
 ands tmo,tmo,idx ;; \
 lsrs idx,idx,#6 ;; \
 movs tmp2,#0x3f ;; \
 ands tmp2,tmp2,idx ;; \
 adds tmp2,tmp2,#64 ;; \
 b label##996 ;;\
label##991: ;; \
 lsls tmo,idx,#20 ;; \
 lsrs tmo,tmo,#20 ;; \
 lsrs idx,idx,#12 ;; \
 movs tmp2,#0x3f ;; \
 ands tmp2,tmp2,idx ;; \
 adds tmp2,tmp2,#128 ;; \
 b label##996 ;;\
label##992: ;; \
 lsls tmo,idx,#14 ;; \
 lsrs tmo,tmo,#14 ;; \
 lsrs idx,idx,#18 ;; \
 movs tmp2,#0x3f ;; \
 ands tmp2,tmp2,idx ;; \
 adds tmp2,tmp2,#192 ;;\
label##996: ;; \
 lsls tmp2,tmp2,#3 ;; \
 str tmo,[pcb,#PCB_TMO] ;; \
 adds tmp2,tmp2,#SC_SLEEPERLISTS-255 ;; \
 adds tmp2,tmp2,#255 ;; \
 movs idx,pcb ;; \
 adds idx,idx,#PCB_SLEEPERS ;; \
 adds tmp2,tmp2,varbase ;; \
 dbl_tailAdd(tmp2,idx,tmo)
#define enterTimer_T2(pcb,tmo,idx,tmp2,varbase,label) \
 ldr idx,[varbase,#SC_TCK] ;; \
 adds idx,idx,tmo ;; \
 lsrs tmp2,tmo,#18 ;; \
 bne label##992 ;; \
 lsrs tmp2,tmo,#12 ;; \
 bne label##991 ;; \
 lsrs tmp2,tmo,#6 ;; \
 bne label##990 ;; \
 and tmp2,idx,#0x3f ;; \
 movs tmo,#0 ;; \
 b label##996 ;;\
label##990: ;; \
 and tmo,idx,#0x3f ;; \
 ubfx tmp2,idx,#6,#6 ;; \
 adds tmp2,tmp2,#64 ;; \
 b label##996 ;;\
label##991: ;; \
 ubfx tmo,idx,#0,#12 ;; \
 ubfx tmp2,idx,#12,#6 ;; \
 adds tmp2,tmp2,#128 ;; \
 b label##996 ;;\
label##992: ;; \
 ubfx tmo,idx,#0,#18 ;; \
 ubfx tmp2,idx,#18,#6 ;; \
 adds tmp2,tmp2,#192 ;;\
label##996: ;; \
 str tmo,[pcb,#PCB_TMO] ;; \
 add tmp2,varbase,tmp2,lsl #3 ;; \
 add tmp2,tmp2,#SC_SLEEPERLISTS ;; \
 adds idx,pcb,#PCB_SLEEPERS ;; \
 dbl_tailAdd(tmp2,idx,tmo)
#define removeTimer(pcb,flags,mask,tmp1) \
 bics flags,flags,mask ;; \
 adds pcb,pcb,#PCB_SLEEPERS ;; \
 dbl_del(pcb,tmp1,mask) ;; \
 subs pcb,pcb,#PCB_SLEEPERS ;; \
 movs mask,#0 ;; \
 str mask,[pcb,#PCB_TMO]
#define enterAsyncTmo(msg,tmo,idx,tmp2,varbase,tmolist,label) \
 ldr idx,[varbase,#SC_TCK] ;; \
 adds idx,idx,tmo ;; \
 lsrs tmp2,tmo,#18 ;; \
 bne label##992 ;; \
 lsrs tmp2,tmo,#12 ;; \
 bne label##991 ;; \
 lsrs tmp2,tmo,#6 ;; \
 bne label##990 ;; \
 movs tmp2,#0x3f ;; \
 ands tmp2,tmp2,idx ;; \
 movs tmo,#0 ;; \
 b label##996 ;;\
label##990: ;; \
 movs tmo,#0x3f ;; \
 ands tmo,tmo,idx ;; \
 lsrs idx,idx,#6 ;; \
 movs tmp2,#0x3f ;; \
 ands tmp2,tmp2,idx ;; \
 adds tmp2,tmp2,#64 ;; \
 b label##996 ;;\
label##991: ;; \
 lsls tmo,idx,#20 ;; \
 lsrs tmo,tmo,#20 ;; \
 lsrs idx,idx,#12 ;; \
 movs tmp2,#0x3f ;; \
 ands tmp2,tmp2,idx ;; \
 adds tmp2,tmp2,#128 ;; \
 b label##996 ;;\
label##992: ;; \
 lsls tmo,idx,#14 ;; \
 lsrs tmo,tmo,#14 ;; \
 lsrs idx,idx,#18 ;; \
 movs tmp2,#0x3f ;; \
 ands tmp2,tmp2,idx ;; \
 adds tmp2,tmp2,#192 ;;\
label##996: ;; \
 ldr idx,tmolist ;; \
 lsls tmp2,tmp2,#3 ;; \
 str tmo,[msg,#TMOMSG_TMO] ;; \
 adds tmp2,tmp2,idx ;; \
 adds msg,msg,#TMOMSG_TMOLIST ;; \
 dbl_tailAdd(tmp2,msg,idx) ;; \
 subs msg,msg,#TMOMSG_TMOLIST
#define enterAsyncTmo_T2(msg,tmo,idx,tmp2,varbase,label) \
 ldr idx,[varbase,#SC_TCK] ;; \
 adds idx,idx,tmo ;; \
 lsrs tmp2,tmo,#18 ;; \
 bne label##992 ;; \
 lsrs tmp2,tmo,#12 ;; \
 bne label##991 ;; \
 lsrs tmp2,tmo,#6 ;; \
 bne label##990 ;; \
 movs tmp2,#0x3f ;; \
 ands tmp2,tmp2,idx ;; \
 movs tmo,#0 ;; \
 b label##996 ;;\
label##990: ;; \
 movs tmo,#0x3f ;; \
 ands tmo,tmo,idx ;; \
 lsrs idx,idx,#6 ;; \
 movs tmp2,#0x3f ;; \
 ands tmp2,tmp2,idx ;; \
 adds tmp2,tmp2,#64 ;; \
 b label##996 ;;\
label##991: ;; \
 lsls tmo,idx,#20 ;; \
 lsrs tmo,tmo,#20 ;; \
 lsrs idx,idx,#12 ;; \
 movs tmp2,#0x3f ;; \
 ands tmp2,tmp2,idx ;; \
 adds tmp2,tmp2,#128 ;; \
 b label##996 ;;\
label##992: ;; \
 lsls tmo,idx,#14 ;; \
 lsrs tmo,tmo,#14 ;; \
 lsrs idx,idx,#18 ;; \
 movs tmp2,#0x3f ;; \
 ands tmp2,tmp2,idx ;; \
 adds tmp2,tmp2,#192 ;;\
label##996: ;; \
 add idx,varbase,#SC_TMOLISTS ;; \
 str tmo,[msg,#TMOMSG_TMO] ;; \
 add tmp2,idx,tmp2,lsl #3 ;; \
 adds tmo,msg,#TMOMSG_TMOLIST ;; \
 dbl_tailAdd(tmp2,tmo,idx)
#define amakeNotReady(pcb,tmp1,tmp2,varbase,label) \
 ldrh tmp1,[pcb,#PCB_FLAGS_H] ;; \
 tst tmp1,#PCB_FLG_READY ;; \
 beq label##9999 ;; \
 bic tmp1,tmp1,#PCB_FLG_READY ;; \
 strh tmp1,[pcb,#PCB_FLAGS_H] ;; \
 ldr tmp1,[pcb,#PCB_PCBLIST+dbl_next] ;; \
 ldr tmp2,[pcb,#PCB_PCBLIST+dbl_prev] ;; \
 str tmp1,[tmp2,#dbl_next] ;; \
 str tmp2,[tmp1,#dbl_prev] ;; \
 cmp tmp1,tmp2 ;; \
 bne label##9998 ;; \
 ldr tmp2,[pcb,#PCB_PRIO_MASK] ;; \
 ldr tmp1,[varbase,#SC_PRIOMASK] ;; \
 bic tmp1,tmp1,tmp2 ;; \
 str tmp1,[varbase,#SC_PRIOMASK] ;;\
label##9998: ;; \
 mov tmp1,#0 ;; \
 str tmp1,[pcb,#PCB_PCBLIST+dbl_next] ;; \
 str tmp1,[pcb,#PCB_PCBLIST+dbl_prev] ;;\
label##9999:
#define aenterTimer(pcb,tmo,tmp1,tmp2,varbase,label) \
 ldr tmp1,[varbase,#SC_TCK] ;; \
 add tmp1,tmp1,tmo ;; \
 movs tmp2,tmo,lsr #18 ;; \
 bne label##992 ;; \
 movs tmp2,tmo,lsr #12 ;; \
 bne label##991 ;; \
 movs tmp2,tmo,lsr #6 ;; \
 andeq tmp2,tmp1,#0x3f ;; \
 moveq tmo,#0 ;; \
 beq label##996 ;; \
 and tmo,tmp1,#0x3f ;; \
 mov tmp2,#0x3f ;; \
 and tmp2,tmp2,tmp1,lsr #6 ;; \
 add tmp2,tmp2,#64 ;; \
 b label##996 ;;\
label##991: ;; \
 mov tmo,tmp1,lsl #20 ;; \
 mov tmo,tmo,lsr #20 ;; \
 mov tmp2,#0x3f ;; \
 and tmp2,tmp2,tmp1,lsr #12 ;; \
 add tmp2,tmp2,#128 ;; \
 b label##996 ;;\
label##992: ;; \
 mov tmo,tmp1,lsl #14 ;; \
 mov tmo,tmo,lsr #14 ;; \
 mov tmp2,#0x3f ;; \
 and tmp2,tmp2,tmp1,lsr #18 ;; \
 add tmp2,tmp2,#192 ;;\
label##996: ;; \
 str tmo,[pcb,#PCB_TMO] ;; \
 movs tmo,#SC_SLEEPERLISTS ;; \
 add tmp2,tmo,tmp2,lsl #3 ;; \
 add tmp1,pcb,#PCB_SLEEPERS ;; \
 add tmp2,tmp2,varbase ;; \
 dbl_tailAdd(tmp2,tmp1,tmo)
#include <machine/arm/arm_asm.h>
#if defined __GNUC__ 
#if SC_MAX_MODULE > 128 || SC_MAX_MODULE == 0
#error SC_MAX_MODULE out of range: 1..128
#endif
#if SC_MAX_CONNECTOR > 128
#error SC_MAX_CONNECTOR out of range: 0..128
#endif
#elif defined __ADS__
 IF SC_MAX_MODULE > 128 || SC_MAX_MODULE == 0
 INFO 4,"SC_MAX_MODULE out of range: 1..127"
 ENDIF
 IF SC_MAX_CONNECTOR > 128
 INFO 4,"SC_MAX_CONNECTOR out of range: 0..127"
 ENDIF
 IF {ARMASM_VERSION} > 220348
 PRESERVE8
 ENDIF
#elif defined __IAR__
 LIMIT SC_MAX_MODULE,1,128,"SC_MAX_MODULE out of range: 1..128"
 LIMIT SC_MAX_CONNECTOR,0,128,"SC_MAX_CONNECTOR out of range:0..128"
#endif
 IF SC_MAX_CONNECTOR > 0
 EXTERN sc_connectors
 ENDIF
 EXTERN sc_modules
#ifdef __GNUC__
 .syntax unified
 WEAK start_hook
 WEAK reset_hook
 WEAK cstartup 
 WEAK user_swi_handler
#elif defined __ADS__
 IMPORT reset_hook
 IMPORT __main
 WEAK start_hook
 WEAK user_swi_handler
#define cstartup __main
#elif defined __IAR__
 IMPORT reset_hook
 IMPORT start_hook
 IMPORT ?main
 IMPORT user_swi_handler
#define cstartup ?main
#elif defined __IAR__ 
 EXTWEAK reset_hook
 EXTWEAK start_hook
 IMPORT ?main
 EXTWEAK user_swi_handler
#define cstartup ?main
#elif defined __IAR5__
 EXTWEAK reset_hook
 EXTWEAK start_hook
 IMPORT __iar_program_start
 EXTWEAK user_swi_handler
#define cstartup __iar_program_start
#endif
 IF CPUTYPE <= Xscale
 SC_FUNC _start
 ldr r0,=reset_hook
 cmp r0,#0
 movne lr,pc
 bxne r0
 ldr r1,=sc_sysProcDispatcher
 cmp r0,#0
 bxne r1
 ldr r0,=cstartup
 cmp r0,#0
 ldreq r0,=main
 bx r0
 SC_ENDFUNC _start 
 SC_FUNC sc_sysSWI
 mrs r12,spsr 
 tst r12,#0x20 
 ldrhne r12,[lr,#-2] 
 bicne r12,r12,#0xff00
 ldreq r12,[lr,#-4] 
 biceq r12,r12,#0xff000000
 cmp r12,#4 
 blo ssw1
 ldr r12,=user_swi_handler 
 cmp r12,#0
 bxne r12 
ssw_error: 
 ldr r0,=(0x05000000|KERNEL_EILL_EXCEPTION|SC_ERR_FATAL)
 mov r1,lr
ssw_error_cont: 
 IF CPUTYPE >= ARMv5T
 blx sc_sysError
 ELSE
 ldr lr,=sc_sysError
 bx lr
 ENDIF
ssw1:
 cmp r12,#0
 beq sc_sysFakeInt
 IF SC_USE_TRAP == 1
 cmp r12,#1
 bne ssw_error
 ldrh r12,[lr,#0] 
 mrs r5,spsr 
 and r4,r5,#PSR_I_BIT
 orr r4,r4,#PSR_SYS_MODE
 msr cpsr_c,r4
 mov r4,lr 
 IF SC_PROCMMU_HOOK == 1
 mrc p15,0,r6,c3,c0,0 
 mov lr,#-1
 mcr p15,0,lr,c3,c0,0 
 ENDIF
 IF SC_CDEBUG == 1
 tst r12,#0x80
 ldrne lr,=xsyscall_tab 
 ldreq lr,=syscall_tab
 ELSE
 ldr lr,=syscall_tab
 ENDIF 
 and r12,r12,#0x7f 
 cmp r12,#SC_MAXFUNC
 bhi ssw_error1
 ldr r12,[lr,r12,lsl #2]
 cmp r12,#0
 beq ssw_error1
 BLX_R12_a
 IF SC_PROCMMU_HOOK == 1
 mcr p15,0,r6,c3,c0,0 
 ENDIF
 bic r5,r5,#0x20 
 msr cpsr_cxsf,r5 
 mov r12,r4
 ldmfd sp!,{r4-r6}
 bx r12 
ssw_error1:
 ldr r0,=(0x06000000|KERNEL_EILL_EXCEPTION|SC_ERR_FATAL)
 mov r1,r12
 IF CPUTYPE >= ARMv5T
 blx sc_sysError
 ELSE
 b ssw_error_cont
 ENDIF
 ELSE 
 b ssw_error
 ENDIF 
 SC_PUBLIC sc_sysFakeInt
 mrs r2,spsr
 mov r3,lr
 sub sp,sp,#32
 stmia sp,{r2-r7,sp-lr}^
 msr cpsr_c,#PSR_SYS_MODE|PSR_I_BIT
 ldr sp,=sc_irq_stack
 ldr r4,=sciopta
 ldr r5,[r4,#SC_CURPCB]
 ldr r6,[r4,#SC_CURMODULE]
 str r1,[r4,#SC_CURPCB]
 ldr r2,[r1,#PCB_PID]
 str r2,[r4,#SC_CURPID]
 mov r2,r2,lsr #24
 ldr r3,=sc_modules
 ldr r3,[r3,r2,lsl #2]
 cmp r3,r6
 beq sswi_nochange1
 str r3,[r4,#SC_CURMODULE]
 IF SC_PROC_HOOK == 1
 IF SC_PROCMMU_HOOK == 1
 mrc p15,0,r7,c3,c0,0 
 mov r3,#3
 adds r2,r2,r2
 mov r3,r3,lsl r2
 movne r2,#0x40000001 
 ldreq r2,=0x55555555 
 orr r2,r3,r2
 mcr p15,0,r2,c3,c0,0
 ENDIF
 ENDIF
sswi_nochange1: 
 ldr r2,[r1,#PCB_ENTRY]
 ldr sp,[r1,#PCB_STACKPTR]
 ldr r1,[r1,#PCB_VECTOR]
 BLX_R2_a
 ldr r1,[r4,#SC_CURMODULE]
 cmp r1,r6
 strne r6,[r4,#SC_CURMODULE]
 IF SC_PROC_HOOK == 1
 IF SC_PROCMMU_HOOK == 1
 mcrne p15,0,r7,c3,c0,0 
 ENDIF
 ENDIF
 ldr r0,[r5,#PCB_PID]
 str r5,[r4,#SC_CURPCB]
 str r0,[r4,#SC_CURPID]
 msr cpsr_c,#PSR_SVC_MODE+PSR_I_BIT
 ldmia sp,{r2-r7,sp-lr}^
 msr spsr_cxsf,r2
 add sp,sp,#32
 movs pc,r3
 SC_ENDFUNC sc_sysSWI
 SC_FUNC sc_sysIrqDispatcher
 ldr r2,=sc_irq_vectors
 IF SC_PROC_PARA_CHECK == 1
 cmp r0,#SC_MAX_INT_VECTORS
 bhs idp_error
 ENDIF
 ldr r3,[r2,r0,lsl #2] 
 IF SC_PROC_PARA_CHECK == 1
 cmp r3,#0
 beq idp_error
 ENDIF
 ldr sp,[r3,#PCB_STACKTOP] 
 IF SC_PROC_PARA_CHECK == 1
 ldrsh r1,[r3,#PCB_NESTED_H]
 adds r1,r1,#1
 bne idp_error1
 strh r1,[r3,#PCB_NESTED_H]
 ENDIF
 stmfd sp!,{r4,r5,r6,lr}
 ldr r4,=sciopta
 ldr r1,[r3,#PCB_PID]
 ldmia r4,{r2,r5} 
 ldr r6,[r4,#SC_CURMODULE]
 stmia r4,{r1,r3} 
 mov r1,r1,lsr #24
 eors r2,r1,r2,lsr #24 
 beq idp1
 ldr r2,=sc_modules
 ldr r2,[r2,r1,lsl #2]
 str r2,[r4,#SC_CURMODULE]
 IF SC_PROC_HOOK == 1
 IF SC_PROCMMU_HOOK == 1
 mov r2,#3
 adds r1,r1,r1
 mov r2,r2,lsl r1
 movne r1,#0x40000001 
 ldreq r1,=0x55555555 
 orr r1,r2,r1
 mcr p15,0,r1,c3,c0,0
 ENDIF
 ENDIF
idp1:
 mov r1,r0 
 mov r0,#0
 ldr r2,[r3,#PCB_ENTRY]
 BLX_R2_a
 msr cpsr_c,#PSR_SYS_MODE|PSR_I_BIT 
 IF SC_PROC_HOOK == 1
 IF SC_PROCMMU_HOOK == 1
 mov r0,#-1
 mcr p15,0,r0,c3,c0,0
 ENDIF
 ENDIF
 ldr r3,[r4,#SC_CURPCB]
 IF SC_PROC_PARA_CHECK == 1
 ldrh r0,[r3,#PCB_NESTED_H]
 sub r0,r0,#1
 strh r0,[r3,#PCB_NESTED_H]
 ENDIF
 ldr r0,[r5,#PCB_PID]
 ldr r1,[r4,#SC_CURMODULE]
 stmia r4,{r0,r5} 
 str r6,[r4,#SC_CURMODULE]
 ldmfd sp!,{r4,r5,r6,pc}
 IF SC_PROC_PARA_CHECK == 1
idp_error1:
 mov r1,r0
 ldr r0,=(KERNEL_EILL_NESTING|SC_ERR_SYSTEM_FATAL)
 ldr r3,=sc_sysError
 bx r3
idp_error:
 mov r1,r0
 bl sc_sysIllegalIrq
 ENDIF
 SC_ENDFUNC sc_sysIrqDispatcher
 SC_FUNC main
 msr cpsr_c,#PSR_IRQ_MODE+PSR_I_BIT
 ldr sp,=sc_irq_stack
 msr cpsr_c,#PSR_SVC_MODE+PSR_I_BIT
 ldr sp,=sc_svc_stack
 ldr r0,=__kernel_bss_start
 ldr r1,=__kernel_bss_size
 mov r2,#0
main1:
 subs r1,r1,#4
 str r2,[r0],#4
 bne main1
 mov r0,#SC_MAX_MODULE
 orr r0,r0,#SC_MAX_CONNECTOR<<8
 IF SC_MSG_STAT == 1
 orr r0,r0,#SC_CONF_MSG_STAT
 ENDIF
 IF SC_MSG_CHECK == 1
 orr r0,r0,#SC_CONF_MSG_CHECK
 ENDIF
 IF SC_PROC_STAT == 1
 orr r0,r0,#SC_CONF_PROC_STAT
 ENDIF
 IF SC_MAX_NUM_BUFFERSIZES == 8
 orr r0,r0,#0x10000
 ENDIF
 IF SC_MAX_NUM_BUFFERSIZES == 16
 orr r0,r0,#0x20000
 ENDIF
 IF SC_POOL_HOOK == 1
 IF SC_POOLCREATE_HOOK == 1
 orr r0,r0,#SC_CONF_PL_CR_HOOK
 ENDIF
 IF SC_POOLKILL_HOOK == 1
 orr r0,r0,#SC_CONF_PL_KL_HOOK
 ENDIF
 ENDIF
 IF SC_MSG_HOOK == 1
 IF SC_MSGTX_HOOK == 1
 orr r0,r0,#SC_CONF_MS_TX_HOOK
 ENDIF
 IF SC_MSGRX_HOOK == 1
 orr r0,r0,#SC_CONF_MS_RX_HOOK
 ENDIF
 ENDIF
 IF SC_PROC_HOOK == 1
 IF SC_PROCCREATE_HOOK == 1
 orr r0,r0,#SC_CONF_PR_CR_HOOK
 ENDIF
 IF SC_PROCKILL_HOOK == 1
 orr r0,r0,#SC_CONF_PR_KL_HOOK
 ENDIF
 IF SC_PROCSWAP_HOOK == 1
 orr r0,r0,#SC_CONF_PR_SW_HOOK
 ENDIF
 ENDIF
 IF SC_ERR_HOOK == 1
 orr r0,r0,#SC_CONF_ERR_HOOK
 ENDIF
 IF SC_USE_FRIENDS == 1
 orr r0,r0,#SC_CONF_FRIENDS
 ENDIF
 IF SC_STACK_CHECK == 1
 orr r0,r0,#SC_CONF_STACKCHK
 ENDIF
 ldr r11,=sciopta
 str r0,[r11,#SC_CONF]
 ldr r0,=10000
 str r0,[r11,#-8]
 add r0,r11,#SC_READYLIST
 mov r1,#32
main2:
 subs r1,r1,#1
 str r0,[r0,#0]
 str r0,[r0,#4]
 add r0,r0,#8
 bne main2
 add r0,r11,#SC_SLEEPERLISTS
 mov r1,#256
main3:
 subs r1,r1,#1
 str r0,[r0,#0]
 str r0,[r0,#4]
 add r0,r0,#8
 bne main3
 IF SC_ASYNC_TMO == 1
 mov r1,#256
main4:
 subs r1,r1,#1
 str r0,[r0,#0]
 str r0,[r0,#4]
 add r0,r0,#8
 bne main4
 ENDIF
 mov r0,#0
 ldr r1,=sc_irq_vectors
 mov r2,#SC_MAX_INT_VECTORS
main5:
 subs r2,r2,#1
 str r0,[r1],#4
 bne main5
 ldr r2,=start_hook
 cmp r2,#0
 IF CPUTYPE >= ARMv5T
 blxne r2
 ELSE
 movne lr,pc
 bxne r2
 ENDIF
 IMPORT TargetSetup
 ldr r2,=TargetSetup
 BLX_R2_a
 ldr r1,=sc_modules
 ldr r0,[r1,#0]
 str r0,[r11,#SC_CURMODULE]
 MSR cpsr_c,#PSR_SYS_MODE|PSR_I_BIT
 ldr r0,=sc_sysProcDispatcher
 bx r0
 SC_PUBLIC sciopta_cpu
 DATA
 LONG CPU
 SC_PUBLIC sciopta_version
 DATA
 BYTE SCVER
 SC_ENDFUNC main
 IF CPUTYPE >= ARMv5T
 ELSE
 SC_TFUNC via_r2_t
 bx r2
 SC_ENDFUNC via_r2_t
 ENDIF
 SC_TFUNC sc_sysLock
 bx pc
 LALIGN
 CODE32
 EXPORT sc_sysLock_a
sc_sysLock_a:
 mrs r0,cpsr
 orr r1,r0,#PSR_I_BIT
 msr cpsr_c,r1
 bx lr
 SC_ENDFUNC sc_sysLock
 SC_TFUNC sc_sysUnlock
 bx pc
 LALIGN
 CODE32
 EXPORT sc_sysUnlock_a
sc_sysUnlock_a:
 and r0,r0,#PSR_I_BIT
 mrs r1,cpsr
 bic r1,r1,#PSR_I_BIT
 orr r1,r1,r0
 msr cpsr_c,r1
 bx lr
 SC_ENDFUNC sc_sysUnlock
 SC_FUNC sc_sysIllegalIrq
 ldr r0,=(KERNEL_EILL_INTERRUPT|SC_ERR_SYSTEM_FATAL)
 IF CPUTYPE >= ARMv5T
 blx sc_sysError
 ELSE
 ldr r3,=sc_sysError
 bx r3
 ENDIF
 SC_ENDFUNC sc_sysIllegalIrq
#ifdef __ADS__
 SC_TFUNC __rt_errno_addr
 ldr r3,=sciopta
 ldr r3,[r3,#SC_CURPCB]
 mov r0,#PCB_ERRORCODE
 add r0,r3
 bx lr
 SC_ENDFUNC __rt_errno_addr
 SC_TFUNC __rt_raise
 push {lr}
 bl sc_miscError
 EXIT
 SC_ENDFUNC __rt_raise
 SC_TFUNC __sys_exit
 mov r1,r0
 ldr cr0,=KERNEL_EPROC_TERMINATE|SC_ERR_SYSTEM_FATAL
 bl sc_sysError
 SC_ENDFUNC __sys_exit
 SC_TFUNC __rt_stackheap_init
 movs r0,#0
 movs r1,#0
 bx lr
 SC_ENDFUNC __rt_stackheap_init
 SC_TFUNC __rt_stackheap_extend
 movs r0,#0
 bx lr
 SC_ENDFUNC __rt_stackheap_extend
 SC_TFUNC __user_initial_stackheap
 ldr r0,=heap_s
 ldr r1,=sc_svc_stack_s
 ldr r2,=heap
 ldr r3,=sc_svc_stack
 bx lr
 SC_ENDFUNC __user_initial_stackheap
 SC_TFUNC __user_heap_extend
 movs r0,#0
 bx lr
 SC_ENDFUNC __user_heap_extend
 SC_TFUNC __use_no_heap_region
 bx lr
 SC_ENDFUNC __use_no_heap_region
 SC_TFUNC __ARM_get_argv
 movs r0,#0
 bx lr
 SC_ENDFUNC __ARM_get_argv
#endif
 SC_TFUNC cpu_lock_r12
 bx pc
 LALIGN
 CODE32
cpu_lock_r12_a: 
 mrs r12,cpsr
 stmfd sp!,{r12}
 orr r12,r12,#PSR_I_BIT
 msr cpsr_c,r12
 bx lr
 SC_ENDFUNC cpu_lock_r12
 SC_TFUNC cpu_unlock_r12
 bx pc
 LALIGN
 CODE32
cpu_unlock_r12_a:
 ldmfd sp!,{r12}
 msr cpsr_c,r12
 bx lr
 SC_ENDFUNC cpu_unlock_r12
 SC_BSS
 EXPORT sc_irq_stack
 EXPORT sc_irq_vectors
 EXPORT sc_svc_stack
 EXPORT sciopta
__kernel_bss_start: 
 SPACE 256 
sc_irq_stack:
 SPACE 8
 SPACE 4 
 SPACE 4 
sciopta:
 SPACE SIZEOF_SCIOPTA
 EXPORT ERR_MSG
ERR_MSG:
 SPACE 6*4
__kernel_bss_size EQU .-__kernel_bss_start
sc_irq_vectors:
 SPACE SC_MAX_INT_VECTORS*4
sc_svc_stack_s: 
 SPACE SC_KERNELSTACK_SIZE
sc_svc_stack:
#ifdef __ADS__
heap_s
 SPACE 4
heap
#endif
 ENDIF
/*   
****************************************************************************
** ARM Kernel
**
** S02338BS1
**
** +Date: 2009/10/08 07:20:59 +
** +Revision: 1.4.2.9 +
****************************************************************************
*/
 IF CPUTYPE == ARMv7M
 SC_FUNC _start
 cpsid if
 ldr r0,=sc_svc_stack
 msr msp,r0
 ldr r0,=reset_hook
 cmp r0,#0
#if defined(__IAR_SYSTEMS_ASM__) && (__VER__ == 5040000)
 beq xx
 blx r0
xx:
#else
 it ne
 blxne r0
#endif
 ldr r1,=sc_sysProcDispatcher
 cmp r0,#0
 it ne
 bxne r1
 ldr r0,=cstartup
 cmp r0,#0
 it eq
 ldreq r0,=main
 bx r0
 SC_ENDFUNC _start 
 SC_TFUNC sc_sysSVC
 tst.w lr,#4 
 ite eq
 mrseq r0,msp 
 mrsne r0,psp 
 ldr r1,[r0,#6*4] 
 ldrh r1,[r1,#-2] 
 uxtb r1,r1 
 cmp r1,#4 
 blo ssw1
 ldr r12,=user_swi_handler 
 cmp r12,#0
 itt ne
 ldmfdne r0,{r0-r1} 
 bxne r12 
ssw_error: 
 ldr r0,=(0x05000000|KERNEL_EILL_EXCEPTION|SC_ERR_FATAL)
 mov r1,r12
ssw_error_cont: 
 b.w sc_sysError
ssw1:
 cmp r1,#0
 beq sc_sysFakeInt
 IF SC_USE_TRAP == 1
 cmp r1,#1
 bne ssw_error
 ldr r1,[r0,#5*4] 
 str r1,[r0,#6*4] 
 ldr r1,[r0,#7*4] 
 push {r4-r6,lr}
 mov r6,r12
 IF SC_PROCMMU_HOOK == 1
 ENDIF
 IF SC_CDEBUG == 1
 lsrs r1,r6,#8
 ite cs
 ldrcs r5,=xsyscall_tab 
 ldrcc r5,=syscall_tab
 ELSE
 ldr r5,=syscall_tab
 ENDIF 
 and r6,r6,#0x7f 
 cmp r6,#SC_MAXFUNC
 bhi ssw_error1
 ldr r5,[r5,r6,lsl #2]
 cmp r5,#0
 beq ssw_error1
 ldmfd r0,{r0-r1} 
 blx r5
 cpsid i
 IF SC_PROCMMU_HOOK == 1
 ENDIF
 pop {r4-r6,pc} 
ssw_error1:
 ldr r0,=(0x06000000|KERNEL_EILL_EXCEPTION|SC_ERR_FATAL)
 mov r1,r6
 b.w sc_sysError
 ELSE
 b ssw_error
 ENDIF
 SC_PUBLIC sc_sysFakeInt
 ldmfd r0,{r0-r1} 
 push {r4-r7,lr}
 cpsid i
 ldr r4,=sciopta
 ldr r5,[r4,#SC_CURPCB]
 ldr r6,[r4,#SC_CURMODULE]
 str r1,[r4,#SC_CURPCB]
 ldr r2,[r1,#PCB_PID]
 str r2,[r4,#SC_CURPID]
 lsrs r2,r2,#24
 ldr r3,=sc_modules
 ldr r3,[r3,r2,lsl #2]
 cmp r3,r6
 beq sswi_nochange1
 str r3,[r4,#SC_CURMODULE]
 IF SC_PROC_HOOK == 1
 IF SC_PROCMMU_HOOK == 1
 ENDIF
 ENDIF
sswi_nochange1:
 mov r7,sp 
 ldr r2,[r1,#PCB_STACKPTR]
 mov sp,r2
 ldr r2,[r1,#PCB_ENTRY]
 ldr r1,[r1,#PCB_VECTOR]
 blx r2
 cpsid i
 mov sp,r7
 ldr r0,[r5,#PCB_PID]
 ldr r1,[r4,#SC_CURMODULE]
 str r5,[r4,#SC_CURPCB]
 str r0,[r4,#SC_CURPID]
 cmp r1,r6
 beq sswi_nochange2
 str r6,[r4,#SC_CURMODULE]
 IF SC_PROC_HOOK == 1
 IF SC_PROCMMU_HOOK == 1
 ENDIF
 ENDIF
sswi_nochange2:
 cpsie i
 pop {r4-r7,pc}
 SC_ENDFUNC sc_sysSVC
 SC_FUNC sc_sysIrqDispatcher
 push {r4-r7,lr}
 ldr r1,=sc_irq_vectors
 mrs r0,IPSR
 IF SC_PROC_PARA_CHECK == 1
 cmp r0,#SC_MAX_INT_VECTORS
 bhs sid_illirq
 ENDIF
 ldr.w r1,[r1,r0,lsl #2]
 ldr r4,=sciopta
 cbz r1,sid_illirq
 ldr r2,[r1,#PCB_PID]
 cpsid i
 ldrh r3,[r4,#SC_NESTED_IRQ_H]
 adds r3,r3,#1
 strh r3,[r4,#SC_NESTED_IRQ_H]
 ldr r5,[r4,#SC_CURPCB]
 ldr r6,[r4,#SC_CURMODULE]
 str r1,[r4,#SC_CURPCB]
 str r2,[r4,#SC_CURPID]
 lsrs r2,r2,#24
 ldr r3,=sc_modules
 ldr r3,[r3,r2,lsl #2]
 cmp r3,r6
 beq sid_nochange1
 str r3,[r4,#SC_CURMODULE]
 IF SC_PROC_HOOK == 1
 IF SC_PROCMMU_HOOK == 1
 ENDIF
 ENDIF
sid_nochange1:
 mov r7,sp 
 ldr r2,[r1,#PCB_STACKPTR]
 mov sp,r2
 ldr r2,[r1,#PCB_ENTRY]
 ldr r1,[r1,#PCB_VECTOR]
 mov r1,r0 
 movs r0,#0
 blx r2
 cpsid i 
 mov sp,r7 
 ldrh r0,[r4,#SC_NESTED_IRQ_H]
 subs r0,r0,#1
 strh r0,[r4,#SC_NESTED_IRQ_H]
 bne sid_no_sched
 ldr r0,[r4,#SC_SCHEDULE_REQ]
 ldr r1,[r4,#SC_SCHEDULER_LOCK]
 cbz r0,sid_no_sched
 cbnz r1,sid_no_sched
#ifdef __IAR5__
 ldr r0,=0xe000ed04 
#else
 movw r0,#0xed04 
 movt r0,#0xe000
#endif
 mov r1,#0x10000000
 str r1,[r0,#0] 
sid_no_sched:
 movs r2,r5 
 ldr r0,[r5,#PCB_PID] 
 ldr r1,[r4,#SC_CURMODULE]
 str r5,[r4,#SC_CURPCB]
 str r0,[r4,#SC_CURPID] 
 cmp r1,r6
 beq sid_nochange2
 str r6,[r4,#SC_CURMODULE]
 IF SC_PROC_HOOK == 1
 IF SC_PROCMMU_HOOK == 1
 ENDIF
 ENDIF
sid_nochange2:
 pop {r4-r7,lr}
 cpsie i
 bx lr
sid_illirq: 
 b.w sc_sysIllegalIrq
 SC_ENDFUNC sc_sysIrqDispatcher
 SC_FUNC main
 cpsid if
 ldr r0,=sc_svc_stack
 mov sp,r0
 ldr r0,=__kernel_bss_start
 ldr r1,=__kernel_bss_size
 mov r2,#0
main1:
 subs r1,r1,#4
 str r2,[r0],#4
 bne main1
 ldr r7,=sciopta
 movs r0,#SC_MAX_MODULE
 orr r0,r0,#SC_MAX_CONNECTOR<<8
 IF SC_MSG_STAT == 1
 orr r0,r0,#SC_CONF_MSG_STAT
 ENDIF
 IF SC_MSG_CHECK == 1
 orr r0,r0,#SC_CONF_MSG_CHECK
 ENDIF
 IF SC_PROC_STAT == 1
 orr r0,r0,#SC_CONF_PROC_STAT
 ENDIF
 IF SC_MAX_NUM_BUFFERSIZES == 8
 orr r0,r0,#0x10000
 ENDIF
 IF SC_MAX_NUM_BUFFERSIZES == 16
 orr r0,r0,#0x20000
 ENDIF
 IF SC_POOL_HOOK == 1
 IF SC_POOLCREATE_HOOK == 1
 orr r0,r0,#SC_CONF_PL_CR_HOOK
 ENDIF
 IF SC_POOLKILL_HOOK == 1
 orr r0,r0,#SC_CONF_PL_KL_HOOK
 ENDIF
 ENDIF
 IF SC_MSG_HOOK == 1
 IF SC_MSGTX_HOOK == 1
 orr r0,r0,#SC_CONF_MS_TX_HOOK
 ENDIF
 IF SC_MSGRX_HOOK == 1
 orr r0,r0,#SC_CONF_MS_RX_HOOK
 ENDIF
 ENDIF
 IF SC_PROC_HOOK == 1
 IF SC_PROCCREATE_HOOK == 1
 orr r0,r0,#SC_CONF_PR_CR_HOOK
 ENDIF
 IF SC_PROCKILL_HOOK == 1
 orr r0,r0,#SC_CONF_PR_KL_HOOK
 ENDIF
 IF SC_PROCSWAP_HOOK == 1
 orr r0,r0,#SC_CONF_PR_SW_HOOK
 ENDIF
 ENDIF
 IF SC_ERR_HOOK == 1
 orr r0,r0,#SC_CONF_ERR_HOOK
 ENDIF
 IF SC_USE_FRIENDS == 1
 orr r0,r0,#SC_CONF_FRIENDS
 ENDIF
 IF SC_STACK_CHECK == 1
 orr r0,r0,#SC_CONF_STACKCHK
 ENDIF
 str r0,[r7,#SC_CONF]
 add r0,r7,#SC_READYLIST
 movs r1,#32
main2:
 str r0,[r0,#0]
 str r0,[r0,#4]
 adds r0,r0,#8
 subs r1,r1,#1
 bne main2
 add r0,r7,#SC_SLEEPERLISTS
 movs r1,#255
main3:
 str r0,[r0,#0]
 str r0,[r0,#4]
 adds r0,r0,#8
 subs r1,r1,#1
 bpl main3
 IF SC_ASYNC_TMO == 1
 movs r1,#255
main4:
 str r0,[r0,#0]
 str r0,[r0,#4]
 adds r0,r0,#8
 subs r1,r1,#1
 bpl main4
 ENDIF
 movs r0,#0
 ldr r1,=sc_irq_vectors
 movs r2,#SC_MAX_INT_VECTORS
main5:
 subs r2,r2,#1
 str r0,[r1],#4
 bne main5
 ldr r0,=10000
 str r0,[r7,#-4]
 ldr r2,=start_hook
 cmp r2,#0
#if defined(__IAR_SYSTEMS_ASM__) && (__VER__ == 5040000)
 beq yy
 blx r2
yy:
#else
 it ne
 blxne r2
#endif
 IMPORT TargetSetup
 ldr r2,=TargetSetup
 blx r2
 ldr r6,=sc_modules
 ldr r0,[r6,#0] 
 str r0,[r7,#SC_CURMODULE]
 ldr r0,=0xe000ed00
 ldr r1,[r0,#0x20]
 orr r1,r1,#0x00ff0000 
 str r1,[r0,#0x20]
 movs r2,#0 
 mov r1,#0x10000000
 str r1,[r0,#0x4] 
main6:
 cpsie if
 b main6
 LALIGN 
 SC_PUBLIC sciopta_cpu
 DATA
 LONG CPU
 SC_PUBLIC sciopta_version
 DATA
 BYTE SCVER
 SC_ENDFUNC main
 SC_TFUNC cpu_lock_r12
 mrs.w r12,PRIMASK
 push.w {r12}
 cpsid i
 bx lr
 SC_ENDFUNC cpu_lock_r12
 SC_TFUNC sc_sysLock
 mrs r0,PRIMASK
 cpsid i
 bx lr
 SC_ENDFUNC sc_sysLock
 SC_TFUNC cpu_unlock_r12
 pop.w {r12}
 msr.w PRIMASK,r12
 bx lr
 SC_ENDFUNC cpu_unlock_r12
 SC_TFUNC sc_sysUnlock
 lsrs r0,r0,#1
 it cs
 bxcs lr
 cpsie i
 bx lr
 SC_ENDFUNC sc_sysUnlock
 SC_FUNC sc_sysIllegalIrq
 mov r1,r0
 ldr r0,=(KERNEL_EILL_INTERRUPT|SC_ERR_SYSTEM_FATAL)
 b.w sc_sysError
 SC_ENDFUNC sc_sysIllegalIrq
#ifdef __ADS__
 SC_TFUNC __rt_errno_addr
 ldr r3,=sciopta
 ldr r3,[r3,#SC_CURPCB]
 mov r0,#PCB_ERRORCODE
 add r0,r3
 bx lr
 SC_ENDFUNC __rt_errno_addr
 SC_TFUNC __rt_raise
 push {lr}
 bl sc_miscError
 EXIT
 SC_ENDFUNC __rt_raise
 SC_TFUNC __sys_exit
 mov r1,r0
 ldr r0,=KERNEL_EPROC_TERMINATE|SC_ERR_SYSTEM_FATAL
 bl sc_sysError
 SC_ENDFUNC __sys_exit
 SC_TFUNC __rt_stackheap_init
 mov r0,#0
 mov r1,#0
 bx lr
 SC_ENDFUNC __rt_stackheap_init
 SC_TFUNC __rt_stackheap_extend
 mov r0,#0
 bx lr
 SC_ENDFUNC __rt_stackheap_extend
 SC_TFUNC __user_initial_stackheap
 ldr r0,=heap_s
 ldr r1,=sc_svc_stack_s
 ldr r2,=heap
 ldr r3,=sc_svc_stack
 bx lr
 SC_ENDFUNC __user_initial_stackheap
 SC_TFUNC __user_heap_extend
 mov r0,#0
 bx lr
 SC_ENDFUNC __user_heap_extend
 SC_TFUNC __use_no_heap_region
 bx lr
 SC_ENDFUNC __use_no_heap_region
 SC_TFUNC __ARM_get_argv
 mov r0,#0
 bx lr
 SC_ENDFUNC __ARM_get_argv
#endif 
 SC_BSS
 EXPORT sc_irq_vectors
 EXPORT sc_svc_stack
 EXPORT sciopta
__kernel_bss_start: 
 SPACE 8
 SPACE 4 
sciopta:
 SPACE SIZEOF_SCIOPTA
 EXPORT ERR_MSG
ERR_MSG:
 SPACE 6*4
__kernel_bss_size EQU .-__kernel_bss_start
sc_irq_vectors:
 SPACE SC_MAX_INT_VECTORS*4
#if defined __GNUC__ 
 .align 3
#elif defined __IAR5__
 ALIGNRAM 3
#elif defined __ADS__
 ALIGN 8
#endif
sc_svc_stack_s: 
 SPACE SC_KERNELSTACK_SIZE
sc_svc_stack:
#ifdef __ADS__
heap_s
 SPACE 4
heap
#endif
 ENDIF 
 XSC_TFUNC sc_sysModulePrioSet,-8
 SC_TFUNC sc_sysModulePrioSet
 push {r0-r1,r4-r7,lr}
 ldr r7,smps_sciopta
 ldr r6,smps_sc_modules
 cmp r0,#31
 bgt smps_err0
 lsrs r2,r1,#24
 cmp r2,#SC_MAX_MODULE
 bge smps_err1
 lsls r2,r2,#2
 ldr r6,[r6,r2]
 cmp r6,#1
 bls smps_warn
 movs r4,r0
 bl sc_procSchedLock
 ldr r3,[r6,#MODULE_PCBS]
 str r4,[r6,#MODULE_PRIO]
 ldr r5,[r6,#MODULE_MAX_PROCESS]
smps1:
 ldmia r3!,{r2}
 cmp r2,#0
 beq smps3
 ldrh r1,[r2,#PCB_FLAGS_H]
 lsrs r1,r1,#PCB_FLG_IS_PRIO_POS+1
 bcc smps3
 ldrb r1,[r2,#PCB_PRIO0_B]
 adds r1,r1,r4
 cmp r1,#31
 bls smps2
 movs r1,#31
smps2:
 ldrb r0,[r2,#PCB_PRIO_B]
 cmp r1,r0
 beq smps3
 LOCK
 makeNotReady(r2,r0,r6,r7,smps)
 prioMask(r2,r1,r0,r7)
 makeReady(r2,r0,r1,r6,r7,smps1)
 UNLOCK
smps3:
 subs r5,r5,#1
 bne smps1
 bl sc_procSchedUnlock
smps_exit: 
 add sp,sp,#2*4
 RET r4-r7
smps_err1:
 ldr r0,smps_lit1
 b smps_err_cont
smps_err0:
 movs r1,r0
 ldr r0,smps_lit0
 b smps_err_cont
smps_warn:
 ldr r0,smps_lit2
smps_err_cont:
 bl sc_sysError
 b smps_exit
 LALIGN
smps_lit0:
 LONG SC_SYSMODULEPRIOSET|KERNEL_EILL_PRIORITY|SC_ERR_SYSTEM_FATAL
smps_lit1:
 LONG SC_SYSMODULEPRIOSET|KERNEL_EILL_MODULE|SC_ERR_SYSTEM_FATAL
smps_lit2:
 LONG SC_SYSMODULEPRIOSET|KERNEL_EILL_MODULE
smps_sciopta:
 LONG sciopta
smps_sc_modules:
 LONG sc_modules
 SC_ENDFUNC sc_sysModulePrioSet
 XSC_TFUNC sc_sysModulePrioGet,-12
 SC_TFUNC sc_sysModulePrioGet
 ldr r3,smpg_sciopta
 ldr r2,smpg_sc_modules
 lsrs r1,r0,#24
 cmp r1,#SC_MAX_MODULE
 bge smpg_err
 lsls r1,r1,#2
 ldr r1,[r2,r1]
#if 0 
 cmp r1,#1
 bls spmg_warn
#endif
 ldr r0,[r1,#MODULE_PRIO]
 bx lr
#if 0 
smpg_warn:
 push {lr}
 movs r1,r0
 ldr r0,smpg_lit1
smpg_err_cont: 
 bl sc_sysError
 EXIT
#endif
smpg_err: 
 movs r1,r0
 ldr r0,smpg_lit0
 bl sc_sysError
 LALIGN
smpg_sciopta:
 LONG sciopta
smpg_sc_modules:
 LONG sc_modules
smpg_lit0: 
 LONG KERNEL_EILL_MODULE|SC_SYSMODULEPRIOGET|SC_ERR_MODULE_FATAL
#if 0 
smpg_lit1: 
 LONG KERNEL_EILL_MODULE|SC_SYSMODULEPRIOGET
#endif
 SC_ENDFUNC sc_sysModulePrioGet
 XSC_TFUNC sc_moduleNameGet,-12
 SC_TFUNC sc_moduleNameGet
 push {lr}
 movs r1,r0
 ldr r3,mng_sciopta
 ldr r0,[r3,#SC_CURMODULE]
 cmp r0,#0
 beq mng_exit
 adds r2,r1,#-SC_CURRENT_MID
 beq mng9
 ldr r2,[r0,#MODULE_ID]
 eors r2,r2,r1
 lsrs r2,r2,#24
 beq mng9
 lsrs r2,r1,#24
 cmp r2,#SC_MAX_MODULE
 ldr r3,mng_sc_modules
 bge mng_err0
 lsls r2,r2,#2
 ldr r0,[r3,r2]
 cmp r0,#1
 bls mng_warn
mng9:
 adds r0,r0,#MODULE_NAME 
 EXIT
mng_warn:
 ldr r0,mng_lit2
mng_err_cont:
 bl sc_sysError
mng_exit:
 adr r0,mng_empty
 EXIT
mng_err0:
 ldr r0,mng_lit1
 b mng_err_cont
 LALIGN
mng_empty:
 LONG 0
mng_sciopta:
 LONG sciopta
mng_lit1:
 LONG KERNEL_EILL_MODULE|SC_MODULENAMEGET|SC_ERR_MODULE_FATAL
mng_lit2:
 LONG KERNEL_EILL_MODULE|SC_MODULENAMEGET
mng_sc_modules:
 LONG sc_modules
 SC_ENDFUNC sc_moduleNameGet
 XSC_TFUNC sc_moduleKill,-8
 SC_TFUNC sc_moduleKill
 push {r0-r1,lr}
 ldr r3,mk_SC_KERNELD
 ldr r0,[r3,#0]
 cmp r0,#0
 beq mk_err
 ldr r1,mk_lit1
 movs r2,#0
 movs r3,#0
 push {r0-r3} 
 movs r0,#SC_MODULEKILLMSG_SZ
 subs r1,r1,#1
 movs r2,#1
 subs r3,r2,#1+(-SC_ENDLESS_TMO)
 adds r2,r2,#SC_DEFAULT_POOL-1
 bl sc_msgAlloc
 ldr r1,[sp,#4*4]
 ldr r2,[sp,#5*4]
 push {r0}
 str r1,[r0,#4]
 str r2,[r0,#8]
 mov r0,sp
 ldr r1,[sp,#4]
 movs r2,#0
 bl sc_msgTx
 add r1,sp,#4
 movs r2,#SC_MSGRX_BOTH
 subs r0,r2,#SC_MSGRX_BOTH+(-SC_ENDLESS_TMO)
 bl sc_msgRx
 str r0,[sp,#0]
 mov r0,sp
 bl sc_msgFree
 add sp,sp,#7*4
 EXIT
mk_err:
 ldr r0,mk_lit0
 movs r1,#0
 bl sc_sysError
 add sp,sp,#2*4
 EXIT
 LALIGN
mk_SC_KERNELD:
 LONG sciopta+SC_KERNELD
mk_lit0:
 LONG SC_MODULEKILL|KERNEL_ENO_KERNELD
mk_lit1:
 LONG SC_MODULEKILLMSG_REPLY
 SC_ENDFUNC sc_moduleKill
 XSC_TFUNC sc_sysModuleKill,-8
 SC_TFUNC sc_sysModuleKill
 push {r4-r7,lr} 
 movs r5,r1 
 ldr r1,smk_sc_modules 
 lsrs r2,r0,#24 
 cmp r2,#SC_MAX_MODULE 
 bhs smk_err1 
 movs r3,#1 
 lsls r2,r2,#2
 beq smk1 
 movs r3,#0
smk1:
 ldr r4,[r1,r2] 
 ldr r7,smk_sciopta 
 cmp r4,#1 
 bls smk_err1 
 ldr r1,[r7,#SC_CURMODULE]
 cmp r1,r4 
 bne smk1a 
 ldr r1,=sc_svc_stack 
 mov sp,r1 
 movs r1,#0
 str r1,[r7,#SC_CURPID] 
smk1a: 
 LOCK
 ldr r0,[r7,#SC_SCHEDULER_LOCK]
 adds r0,r0,#1
 str r0,[r7,#SC_SCHEDULER_LOCK] 
 UNLOCK
 ldr r6,[r4,#MODULE_MAX_PROCESS]
 ldr r0,[r4,#MODULE_PCBS]
 subs r6,r6,r3
 beq smk3a 
 lsls r7,r3,#2
 adds r7,r7,r0
smk2:
 ldmia r7!,{r0} 
 cmp r0,#0 
 beq smk3 
 ldr r0,[r0,#PCB_PID] 
 movs r1,r5 
 bl sc_sysProcKill 
smk3:
 subs r6,r6,#1 
 bne smk2
smk3a:
 ldr r7,[r4,#MODULE_POOL]
 ldr r6,[r4,#MODULE_MAX_POOLS]
 lsrs r0,r5,#SC_PROCKILL_KILL_POS+1 
 bcs smk6 
 movs r5,#0
smk4: 
 ldmia r7!,{r0} 
 cmp r0,#0 
 beq smk5 
 movs r0,r5 
 ldr r1,[r4,#MODULE_ID] 
 bl sc_sysPoolKill 
smk5:
 adds r5,r5,#1 
 subs r6,r6,#1 
 bne smk4
 b smk8
smk6:
 ldmia r7!,{r5} 
 bl sysPoolKill 
smk7:
 subs r6,r6,#1
 bne smk6
smk8:
 ldr r1,[r4,#MODULE_ID]
 ldr r0,smk_sc_modules
 ldr r7,smk_sciopta
 lsrs r1,r1,#24
 lsls r1,r1,#2
 bne smk9
 ldr r3,[r4,#MODULE_PCBS]
 ldr r3,[r3,#0]
 LOCK
 makeNotReady(r3,r6,r5,r7,smk)
 UNLOCK
smk9:
 movs r6,#0
 ldr r3,[r7,#SC_NMODULE]
 ldr r5,[r7,#SC_CURMODULE]
 str r6,[r0,r1] 
 subs r3,r3,#1
 str r3,[r7,#SC_NMODULE]
 cmp r5,r4 
 bne smk10 
 str r6,[r7,#SC_CURMODULE] 
 str r6,[r7,#SC_CURPCB]
 str r6,[r7,#SC_CURPRIOPCB]
 LOCK
 movs r0,#0
 str r0,[r7,#SC_SCHEDULER_LOCK]
 ldr r3,smk_dispatcher
 bx r3 
smk10:
 LOCK
 ldr r1,[r7,#SC_SCHEDULER_LOCK]
 subs r0,r1,#1
 bmi smk_err2
 str r0,[r7,#SC_SCHEDULER_LOCK]
 UNLOCK
 ldr r0,[r7,#SC_SCHEDULE_REQ]
 cmp r0,#0
 beq smk11
 SWAP_OUT
smk11: 
 RET r4-r7
smk_err2:
 ldr r0,smk_unlockNoLock
 b smk_err_cont
smk_err1:
 movs r1,r0
 ldr r0,smk_illModule
smk_err_cont:
 bl sc_sysError
 LALIGN
smk_illModule:
 LONG KERNEL_EILL_MODULE|SC_MODULEKILL|SC_ERR_SYSTEM_FATAL
smk_unlockNoLock:
 LONG (KERNEL_EUNLOCK_WO_LOCK|SC_ERR_MODULE_FATAL|SC_PROCSCHEDUNLOCK)
smk_sciopta:
 LONG sciopta
smk_sc_modules:
 LONG sc_modules
smk_dispatcher:
 LONG sc_sysProcDispatcher
 SC_ENDFUNC sc_sysModuleKill
 XSC_TFUNC sc_moduleInfo,-8
 SC_TFUNC sc_moduleInfo
 IF SC_PROC_PARA_CHECK == 1
 cmp r1,#0 
 beq mi_err0 
 ENDIF
 mov r12,r1 
 ldr r3,mi_sciopta 
 adds r2,r0,#1 
 ldr r2,[r3,#SC_CURMODULE] 
 bne mi1 
 cmp r2,#0 
 bne mi2 
mi_exit:
 movs r0,#0 
 bx lr 
mi1:
 lsrs r2,r0,#24 
 cmp r2,#SC_MAX_MODULE 
 ldr r3,mi_sc_modules
 bge mi_err1 
 lsls r2,r2,#2
 ldr r2,[r3,r2] 
 cmp r2,#1 
 bls mi_exit 
mi2:
 ldr r1,[r2,#MODULE_ID] 
 mov r0,r12 
 mov r12,r2 
 stmia r0!,{r1} 
 adds r2,r2,#MODULE_NAME 
 ldmia r2!,{r1,r3}
 stmia r0!,{r1,r3}
 ldmia r2!,{r1,r3}
 stmia r0!,{r1,r3}
 ldmia r2!,{r1,r3}
 stmia r0!,{r1,r3}
 ldmia r2!,{r1,r3}
 stmia r0!,{r1,r3}
 mov r2,r12 
 ldr r1,[r2,#MODULE_TEXT] 
 subs r3,r2,r1 
 stmia r0!,{r1,r3} 
 ldr r1,[r2,#MODULE_DATA]
 ldr r3,[r2,#MODULE_SIZE]
 stmia r0!,{r1,r3} 
 ldr r1,[r2,#MODULE_MAX_PROCESS]
 ldr r3,[r2,#MODULE_NPROCESS]
 stmia r0!,{r1,r3} 
 ldr r1,[r2,#MODULE_MAX_POOLS]
 ldr r3,[r2,#MODULE_NPOOLS]
 stmia r0!,{r1,r3} 
 movs r0,#1 
 bx lr
mi_err1:
 movs r1,r0
 ldr r0,mi_lit1
mi_err_cont: 
 bl sc_sysError
 IF SC_PROC_PARA_CHECK == 1
mi_err0:
 ldr r0,mi_lit0
 b mi_err_cont
 ENDIF
 LALIGN
mi_lit1:
 LONG KERNEL_EILL_MODULE|SC_MODULEINFO|SC_ERR_SYSTEM_FATAL
 IF SC_PROC_PARA_CHECK == 1
mi_lit0:
 LONG KERNEL_ENIL_PTR|SC_MODULEINFO|SC_ERR_SYSTEM_FATAL
 ENDIF
mi_sciopta:
 LONG sciopta
mi_sc_modules:
 LONG sc_modules
 SC_ENDFUNC sc_moduleInfo
 XSC_TFUNC sc_moduleIdGet,-12
 SC_TFUNC sc_moduleIdGet
 push {r4-r6,lr}
 ldr r6,mig_sciopta
 ldr r2,[r6,#SC_CURMODULE]
 adds r4,r0,#0
 ldr r0,[r2,#MODULE_ID]
 beq mig_exit
 ldrb r3,[r4,#0]
 cmp r3,#0
 beq mig_exit
 movs r3,#SC_MODULE_NAME_SIZE
mig0:
 ldrb r0,[r4,r3]
 cmp r0,#0
 beq mig1
 subs r3,r3,#1
 bpl mig0
 ldr r0,mig_lit0
 movs r1,r4
 bl sc_sysError
 ldr r0,mig_lit1
 b mig_exit
mig1:
 movs r0,r4
 bl sc_miscCrcString
 mov lr,r0
 mov r12,r4
 movs r1,#SC_MAX_MODULE
 ldr r2,mig_sc_modules
mig2:
 ldmia r2!,{r3}
 cmp r3,#1
 bls mig5
 movs r4,r3
 adds r4,r4,#MODULE_HASH_H
 ldrh r4,[r4,#0]
 cmp r4,lr
 bne mig5
 ldr r0,[r3,#MODULE_ID]
 adds r3,r3,#MODULE_NAME
 mov r4,r12
mig3:
 ldrb r5,[r3,#0]
 adds r3,r3,#1
 ldrb r6,[r4,#0]
 adds r4,r4,#1
 cmp r5,r6
 bne mig5
 cmp r5,#0
 bne mig3
mig_exit: 
 RET r4-r6
mig5:
 subs r1,r1,#1
 bne mig2
 ldr r0,mig_lit1
 RET r4-r6
 LALIGN
mig_sc_modules:
 LONG sc_modules
mig_sciopta:
 LONG sciopta
mig_lit0:
 LONG SC_MODULEIDGET|KERNEL_EILL_MODULE_NAME
mig_lit1:
 LONG SC_ILLEGAL_MID
 SC_ENDFUNC sc_moduleIdGet
#define PARA_NAME 16
#define PARA_INIT 20
#define PARA_STACKSIZE 24
#define PARA_INITPRIO 28
#define PARA_MODSTART 36
#define PARA_SIZE 40
#define PARA_TEXTSIZE 44
#define PARA_MAX_POOLS 48
#define PARA_MAX_PROCS 52
 XSC_TFUNC sc_moduleCreate,20
 SC_TFUNC sc_moduleCreate
 push {r0-r3,lr}
 ldr r0,mc_sciopta
 ldr r0,[r0,#0]
 cmp r0,#0
 beq mc_err 
 movs r2,#0
 movs r3,#0
 ldr r1,mc_SC_MODULECREATEMSG_REPLY
 push {r0-r3} 
 movs r0,#SC_MODULECREATEMSG_SZ
 subs r1,r1,#1
 movs r2,#1
 subs r3,r2,#1+(-SC_ENDLESS_TMO)
 adds r2,r2,#SC_DEFAULT_POOL-1
 bl sc_msgAlloc
 ldr r1,[sp,#PARA_NAME]
 adds r2,r0,#SC_MODULECREATEMSG_MODULE
 push {r0}
 push {r4-r6}
 ldmia r1!,{r3-r6} 
 stmia r2!,{r3-r6}
 ldmia r1!,{r3-r6}
 stmia r2!,{r3-r6}
 add r1,sp,#PARA_INIT+4*4
 ldmia r1!,{r3-r5} 
 adds r1,r1,#4
 stmia r2!,{r3-r5}
 ldmia r1!,{r0,r3-r6} 
 stmia r2!,{r0,r3-r6}
 pop {r4-r6}
 mov r0,sp
 ldr r1,[sp,#4]
 movs r2,#0
 bl sc_msgTx
 add sp,sp,#4
 mov r1,sp
 movs r2,#SC_MSGRX_BOTH
 subs r0,r2,#SC_MSGRX_BOTH+(-SC_ENDLESS_TMO)
 bl sc_msgRx
 push {r0}
 ldr r0,[r0,#SC_MODULECREATEMSG_REPLY_MID]
 push {r0}
 add r0,sp,#4
 bl sc_msgFree
 pop {r0}
 add sp,sp,#9*4
 EXIT
mc_err:
 ldr r0,mc_lit0
 movs r1,#0
 bl sc_sysError
 add sp,sp,#4*4
 ldr r0,mc_lit1
 EXIT
 LALIGN
mc_sciopta:
 LONG sciopta+SC_KERNELD
mc_SC_MODULECREATEMSG_REPLY:
 LONG SC_MODULECREATEMSG_REPLY
mc_lit0:
 LONG SC_MODULECREATE|KERNEL_ENO_KERNELD
mc_lit1:
 LONG SC_ILLEGAL_PID
 SC_ENDFUNC sc_moduleCreate
#undef PARA_NAME
#undef PARA_INIT
#undef PARA_INITPRIO
#undef PARA_MODSTART
#undef PARA_STACKSIZE
#undef PARA_SIZE
#undef PARA_TEXTSIZE
#undef PARA_MAX_POOLS
#undef PARA_MAX_PROCS
#define PARA_INIT 0
#define PARA_STACKSIZE 4
#define PARA_MODSTART 28
#define PARA_SIZE 32
#define PARA_TEXTSIZE 36
#define PARA_MAX_POOLS 40
#define PARA_MAX_PROCS 44
 XSC_TFUNC sc_sysModuleCreate,20
 SC_TFUNC sc_sysModuleCreate
 push {r1-r2,r4-r7,lr}
 mov r12,r0
 ldr r7,smc_sciopta
 cmp r3,#32
 bhs smc_err4
 cmp r0,#0
 beq smc_ill_name
smc00:
 movs r4,#0xdf
 movs r6,#SC_PROC_NAME_SIZE+1
smc0:
 ldrb r5,[r0,#0]
 adds r0,r0,#1
 cmp r5,#0
 beq smc1
 cmp r5,#'_'
 beq smc01
 cmp r5,#'0'
 blt smc_ill_name
 cmp r5,#'9'+1
 blt smc01
 ands r5,r5,r4
 cmp r5,#'A'
 blt smc_ill_name
 cmp r5,#'Z'
 bgt smc_ill_name
smc01: 
 subs r6,r6,#1
 bne smc0
smc_ill_name:
 mov r1,r12
 ldr r0,smc_ill_name_lit
smc_err_cont:
 bl sc_sysError
smc_err4:
 movs r0,r3
smc_ill_value:
 movs r1,r0
 ldr r0,smc_ill_value_lit
 b smc_err_cont
smc1:
 subs r6,r6,#SC_PROC_NAME_SIZE+1
 beq smc_ill_name 
 ldr r0,[sp,#PARA_MAX_PROCS]
 cmp r0,#0
 beq smc_ill_value
 lsrs r4,r0,#14 
 bne smc_ill_value
 movs r4,#128+4
 muls r0,r4,r0
 ldr r4,[sp,#PARA_MAX_POOLS]
 cmp r4,#0
 beq smc_ill_value
 cmp r4,#128
 bhi smc_ill_value
 lsls r4,r4,#2 
 adds r0,r0,r4
 movs r5,#SIZEOF_POOL_CB>>2 
 muls r4,r5,r4 
 ldr r5,[sp,#PARA_TEXTSIZE]
 adds r0,r0,r4 
 ldr r4,[sp,#PARA_SIZE]
 adds r0,r0,r2 
 IF CPUTYPE >= ARMv5T
 adds r5,r5,#7
 lsrs r5,r5,#3
 lsls r5,r5,#3
 ELSE
 adds r5,r5,#3
 lsrs r5,r5,#2
 lsls r5,r5,#2
 ENDIF
 str r5,[sp,#PARA_TEXTSIZE]
 adds r0,r0,r5 
 movs r5,#SIZEOF_MODULE_CB>>2
 lsls r5,r5,#2
 adds r0,r0,r5
 cmp r0,r4
 bhi smc_too_small
 ldr r0,[r7,#SC_NMODULE]
 cmp r0,#SC_MAX_MODULE
 beq smc_no_module
 adds r0,r0,#1
 str r0,[r7,#SC_NMODULE]
 ldr r5,=(sc_modules-4)
 movs r7,#0
 subs r7,r7,#1
smc2:
 ldr r0,[r5,#4]
 adds r7,r7,#1
 adds r5,r5,#4
 cmp r0,#0
 bne smc2
 ldr r4,[sp,#PARA_MODSTART]
 ldr r0,[sp,#PARA_TEXTSIZE]
 cmp r4,#0
 bne smc21
 cmp r0,#0
 bne smc21
 movs r0,#4
smc21: 
 adds r4,r4,r0
 ldr r0,[sp,#PARA_MAX_POOLS]
 ldr r1,[sp,#PARA_MAX_PROCS]
 adds r1,r1,r0
 adds r1,r1,#SIZEOF_MODULE_CB>>2
 movs r2,#0
 movs r0,r4
smc3:
 subs r1,r1,#1
 stmia r0!,{r2}
 bne smc3
 IF SC_CDEBUG == 1
 ldr r1,[sp,#PARA_SIZE]
 ldr r2,[sp,#PARA_TEXTSIZE]
 adds r1,r1,r4
 subs r1,r1,r0
 subs r1,r1,r2
 lsrs r1,r1,#2
 ldr r2,smc_modulevirgin
smc4:
 subs r1,r1,#1
 stmia r0!,{r2}
 bne smc4
 ENDIF
 str r3,[r4,#MODULE_PRIO]
 add r0,sp,#PARA_MODSTART
 ldmia r0!,{r1-r2}
 str r1,[r4,#MODULE_TEXT]
 str r2,[r4,#MODULE_SIZE]
 adds r0,r0,#4
 ldmia r0!,{r1-r2}
 str r1,[r4,#MODULE_MAX_POOLS]
 str r2,[r4,#MODULE_MAX_PROCESS]
 mov r0,r12
 movs r3,r4
 adds r3,r3,#MODULE_NAME-1
smc5:
 ldrb r1,[r0,#0]
 adds r3,r3,#1
 adds r0,r0,#1
 strb r1,[r3,#0]
 cmp r1,#0
 bne smc5
 mov r0,r12
 bl sc_miscCrcString
 movs r1,r4
 adds r1,r1,#MODULE_HASH_H
 strh r0,[r1,#0]
 lsls r0,r7,#24
 str r0,[r4,#MODULE_ID]
 IF SC_USE_FRIENDS == 1
 lsrs r0,r7,#5
 movs r1,#31
 ands r1,r1,r7
 lsls r0,r0,#2
 adds r0,r0,#MODULE_FRIENDSET
 movs r2,#1
 lsls r2,r2,r1
 str r2,[r4,r0]
 ENDIF
 movs r0,r4
 adds r0,r0,#SIZEOF_MODULE_CB
 str r0,[r4,#MODULE_PCBS]
 ldr r1,[r4,#MODULE_MAX_PROCESS]
 lsls r1,r1,#2
 adds r0,r0,r1
 str r0,[r4,#MODULE_POOL]
 ldr r1,[r4,#MODULE_MAX_POOLS]
 lsls r1,r1,#2
 adds r0,r0,r1
 str r0,[r4,#MODULE_DATA]
 str r0,[r4,#MODULE_DATAPTR]
 ldr r1,[r4,#MODULE_TEXT]
 subs r1,r1,r0
 ldr r0,[sp,#PARA_SIZE]
 adds r0,r0,r1
 str r0,[r4,#MODULE_FREESIZE]
 str r4,[r5,#0]
 pop {r1-r2} 
 movs r0,r4
 adds r0,r0,#MODULE_NAME
 movs r6,#SC_PROCPRIOCREATESTATIC>>24
 lsls r6,r6,#24
 movs r5,#0 
 movs r3,#0 
 push {r3,r5-r6}
 ldr r6,[r4,#MODULE_ID]
 movs r5,#1 
 push {r3,r5-r6}
 bl sc_sysProcCreate
 add sp,sp,#6*4
 lsls r0,r7,#24
 RET r4-r7
smc_too_small:
 movs r1,r4
 ldr r0,smc_too_small_lit
 b smc_err_cont
smc_no_module:
 mov r1,r12
 ldr r0,smc_no_module_lit
 b smc_err_cont
 LALIGN
smc_sciopta:
 LONG sciopta
smc_modulevirgin:
 LONG 0xaaaaaaaa
smc_ill_name_lit:
 LONG KERNEL_EILL_MODULE_NAME|SC_MODULECREATE|SC_ERR_SYSTEM_FATAL
smc_ill_value_lit:
 LONG (SC_MODULECREATE|KERNEL_EILL_VALUE|SC_ERR_SYSTEM_FATAL)
smc_too_small_lit:
 LONG (KERNEL_EMODULE_TOO_SMALL|SC_MODULECREATE|SC_ERR_SYSTEM_FATAL)
smc_no_module_lit:
 LONG (KERNEL_ENO_MORE_MODULE|SC_MODULECREATE|SC_ERR_SYSTEM_FATAL)
 SC_ENDFUNC sc_sysModuleCreate
#undef PARA_INIT
#undef PARA_STACKSIZE
#undef PARA_MODSTART
#undef PARA_SIZE
#undef PARA_TEXTSIZE
#undef PARA_MAX_POOLS
#undef PARA_MAX_PROCS
 IF SC_USE_FRIENDS == 1
 XSC_TFUNC sc_moduleFriendAdd,-12
 SC_TFUNC sc_moduleFriendAdd
 push {lr}
 lsrs r1,r0,#24
 cmp r1,#SC_MAX_MODULE
 bhs mfa_err
 movs r0,#31
 ands r0,r0,r1
 lsrs r1,r1,#5
 lsls r1,r1,#2
 ldr r2,mfa_lit1
 adds r1,r1,#MODULE_FRIENDSET
 ldr r2,[r2,#SC_CURMODULE]
 movs r3,#1
 lsls r3,r3,r0
 LOCK
 ldr r0,[r2,r1]
 orrs r0,r0,r3
 str r0,[r2,r1]
 UNLOCK
 EXIT
mfa_err:
 movs r1,r0
 ldr r0,mfa_lit0
 bl sc_sysError
 LALIGN
mfa_lit0:
 LONG (KERNEL_EILL_MODULE|SC_MODULEFRIENDADD|SC_ERR_SYSTEM_FATAL)
mfa_lit1:
 LONG sciopta
 SC_ENDFUNC sc_moduleFriendAdd
 XSC_TFUNC sc_moduleFriendRm,-12
 SC_TFUNC sc_moduleFriendRm
 push {lr}
 ldr r2,mfr_lit1
 lsrs r1,r0,#24
 cmp r1,#SC_MAX_MODULE
 bhs mfr_err
 ldr r2,[r2,#SC_CURMODULE]
 ldr r3,[r2,#MODULE_ID]
 lsrs r3,r3,#24
 cmp r3,r0 
 beq mfr_err
 movs r0,#31
 ands r0,r0,r1
 lsrs r1,r1,#5
 lsls r1,r1,#2
 adds r1,r1,#MODULE_FRIENDSET
 movs r3,#1
 lsls r3,r3,r0
 LOCK
 ldr r0,[r2,r1]
 bics r0,r0,r3
 str r0,[r2,r1]
 UNLOCK
 EXIT
mfr_err:
 movs r1,r0
 ldr r0,mfr_lit0
 bl sc_sysError
 LALIGN
mfr_lit0:
 LONG (KERNEL_EILL_MODULE|SC_MODULEFRIENDRM|SC_ERR_SYSTEM_FATAL)
mfr_lit1:
 LONG sciopta
 SC_ENDFUNC sc_moduleFriendRm
 XSC_TFUNC sc_moduleFriendGet,-12
 SC_TFUNC sc_moduleFriendGet
 lsrs r1,r0,#24
 cmp r1,#SC_MAX_MODULE
 bhs mfg_err
 movs r0,#31
 ands r0,r0,r1
 lsrs r1,r1,#5
 lsls r1,r1,#2
 ldr r2,mfg_lit1
 adds r1,r1,#MODULE_FRIENDSET
 ldr r2,[r2,#SC_CURMODULE]
 movs r3,#1
 lsls r3,r3,r0
 ldr r0,[r2,r1]
 ands r0,r0,r3
 bx lr
mfg_err:
 movs r1,r0
 ldr r0,mfg_lit0
 bl sc_sysError
 LALIGN
mfg_lit0:
 LONG (KERNEL_EILL_MODULE|SC_MODULEFRIENDGET|SC_ERR_SYSTEM_FATAL)
mfg_lit1:
 LONG sciopta
 SC_ENDFUNC sc_moduleFriendGet
 XSC_TFUNC sc_moduleFriendNone,-16
 SC_TFUNC sc_moduleFriendNone
 push {lr}
 ldr r3,mfn_lit1
 ldr r2,[r3,#SC_CURMODULE]
 ldr r1,[r3,#MODULE_ID]
 lsrs r1,r1,#24
 movs r0,#31
 ands r0,r0,r1
 lsrs r1,r1,#5
 lsls r1,r1,#2
 adds r1,r1,#MODULE_FRIENDSET
 movs r3,#1
 lsls r3,r3,r0 
 movs r0,#0
 LOCK
 str r0,[r2,#MODULE_FRIENDSET]
 str r0,[r2,#MODULE_FRIENDSET+4]
 str r0,[r2,#MODULE_FRIENDSET+8]
 str r0,[r2,#MODULE_FRIENDSET+12]
 str r3,[r2,r1]
 UNLOCK
 EXIT 
 LALIGN
mfn_lit1:
 LONG sciopta
 SC_ENDFUNC sc_moduleFriendNone
 XSC_TFUNC sc_moduleFriendAll,-16
 SC_TFUNC sc_moduleFriendAll
 push {lr}
 ldr r3,mfall_lit1
 movs r0,#0
 ldr r2,[r3,#SC_CURMODULE]
 subs r0,r0,#1
 LOCK
 str r0,[r2,#MODULE_FRIENDSET]
 str r0,[r2,#MODULE_FRIENDSET+4]
 str r0,[r2,#MODULE_FRIENDSET+8]
 str r0,[r2,#MODULE_FRIENDSET+12]
 UNLOCK
 EXIT 
 LALIGN
mfall_lit1:
 LONG sciopta
 SC_ENDFUNC sc_moduleFriendAll
 ENDIF
#if 0
 XSC_TFUNC sc_miscState,-16
 SC_TFUNC sc_miscState
 add r0,pc,#0
 bx r0 
 CODE32
 ldr r3,ms_sciopta
 ldr r0,[r3,#SC_CONF]
 cmp r0,#0
 bxeq lr
 movs r0,#1
 ldr r1,[r3,#SC_CURMODULE]
 cmp r1,#0
 bxeq lr
 movs r0,#3
 ldr r1,[r3,#SC_SCHEDULER_LOCK]
 cmp r1,#0
 orreq r0,r0,#4
 mrs r1,cpsr
 ands r1,r1,#PSR_I_BIT
 orrne r0,r0,#8
 bx lr
 LALIGN
ms_sciopta:
 LONG sciopta
 SC_ENDFUNC sc_miscState
#endif
 XSC_TFUNC sc_miscKerneldRegister,-16
 SC_TFUNC sc_miscKerneldRegister
 push {lr}
 movs r0,#0
 ldr r3,mkr_sciopta
 ldr r2,[r3,#SC_CURPCB]
 ldr r1,[r3,#SC_CURPID]
 lsrs r1,r1,#24
 ldrh r1,[r2,#PCB_FLAGS_H]
 bne mkr_err0
 lsrs r1,r1,#PCB_FLG_IS_PRIO_POS+1
 bcc mkr_err0
 ldr r3,mkr_SC_KERNELD
 ldr r2,[r2,#PCB_PID]
 LOCK
 ldr r1,[r3,#0]
 cmp r1,#0
 bne mkr_err1
 str r2,[r3,#0]
 movs r0,#1
mkr_err1:
 UNLOCK 
mkr_err0:
 EXIT
 LALIGN
mkr_sciopta:
 LONG sciopta
mkr_SC_KERNELD:
 LONG sciopta+SC_KERNELD
 SC_ENDFUNC sc_miscKerneldRegister
 XSC_TFUNC sc_miscErrnoGet,-16
 SC_TFUNC sc_miscErrnoGet
 ldr r3,meg_sciopta
 ldr r3,[r3,#SC_CURPCB]
 ldr r0,[r3,#PCB_ERRORCODE]
 bx lr
 LALIGN
meg_sciopta:
 LONG sciopta
 SC_ENDFUNC sc_miscErrnoGet
 XSC_TFUNC sc_miscErrnoSet,-12
 SC_TFUNC sc_miscErrnoSet
 ldr r3,mes_sciopta
 ldr r3,[r3,#SC_CURPCB]
 str r0,[r3,#PCB_ERRORCODE]
 bx lr
 LALIGN
mes_sciopta:
 LONG sciopta
 SC_ENDFUNC sc_miscErrnoSet
 IF SC_ERR_HOOK == 1
 XSC_TFUNC sc_miscErrorHookRegister,-12
 SC_TFUNC sc_miscErrorHookRegister 
 ldr r3,mehr_sciopta
 ldr r2,[r3,#SC_CURMODULE]
 cmp r2,#0
 bne mehr1
 ldr r2,mehr_SC_ERRHOOK
 adds r2,r2,r3
mehr1:
 ldr r1,[r2,#MODULE_ERRHOOK]
 str r0,[r2,#MODULE_ERRHOOK]
 movs r0,r1
 bx lr
 LALIGN
mehr_sciopta:
 LONG sciopta
mehr_SC_ERRHOOK:
 LONG SC_ERRHOOK-MODULE_ERRHOOK
 SC_ENDFUNC sc_miscErrorHookRegister
 ENDIF
 XSC_TFUNC sc_miscError,-8
 SC_TFUNC sc_miscError
 movs r2,#1
 b merr_cnt
 SC_TPUBLIC sc_sysError
 movs r2,#0
merr_cnt:
 push {r4-r7,lr}
 LOCK
 ldr r6,merr_sciopta
 ldr r3,[r6,#SC_CURPCB]
 movs r4,#SC_ERR_SYSTEM_FATAL 
 cmp r3,#0
 beq se0
 movs r4,#0 
 ldrh r7,[r3,#PCB_FLAGS_H]
 movs r5,#PCB_FLG_IS_TIMER|PCB_FLG_IS_IRQ 
 tst r7,r5
 beq se00 
 movs r5,#SC_ERR_FATAL 
 tst r0,r5
 beq se00 
 movs r4,r5 
se00:
 orrs r0,r0,r4
 str r0,[r3,#PCB_ERRORCODE]
se0:
 orrs r0,r0,r4
 ldr r7,merr_ERR_MSG
 ldr r4,[r6,#SC_CURPID]
 movs r5,r7 
 stmia r7!,{r0-r4}
 IF SC_ERR_HOOK == 1
 push {r0} 
 movs r7,#0 
 ldr r4,[r6,#SC_CURMODULE]
 cmp r4,#0 
 beq se1 
 ldr r4,[r4,#MODULE_ERRHOOK] 
 cmp r4,#0
 beq se1 
 bl via_r4 
 movs r7,#4 
 cmp r0,#0
 beq se1
 adds r7,r7,#1 
se1:
 ldmia r5!,{r0-r3} 
 ldr r4,merr_lit2
 ldr r4,[r6,r4] 
 cmp r4,#0 
 beq se2 
 bl via_r4
 adds r7,r7,#8 
 cmp r0,#0
 beq se2
 adds r7,r7,#2 
se2:
 pop {r2} 
 cmp r7,#15 
 beq se2a
 cmp r7,#10 
 beq se2a
 cmp r7,#5 
 bne sc_fatal
se2a: 
 movs r3,#SC_ERR_FATAL
 tst r2,r3
 bne se3 
 UNLOCK 
 RET r4-r7
se3:
 ldr r0,[r6,#SC_CURPID]
 movs r1,#0
 lsrs r2,r2,#1
 bcs sc_fatal 
 lsrs r2,r2,#1 
 bcs se_module 
 cmp r0,#0 
 beq sc_fatal
 bl sc_sysProcKill
 b se5
se_module:
 lsrs r2,r0,#24 
 beq sc_fatal
 bl sc_sysModuleKill
se5: 
 LOCK
 ldr r4,p_sc_sysProcDispatcher
via_r4:
 bx r4
 ENDIF
 SC_TPUBLIC sc_fatal
 mov r8,r8 
 bl sc_fatal
 LALIGN
merr_ERR_MSG: 
 LONG ERR_MSG
merr_sciopta:
 LONG sciopta
 IF SC_ERR_HOOK == 1
merr_lit2:
 LONG SC_ERRHOOK
p_sc_sysProcDispatcher:
 LONG sc_sysProcDispatcher
 ENDIF
 SC_ENDFUNC sc_miscError
 XSC_TFUNC sc_miscCrcString,-12
 SC_TFUNC sc_miscCrcString
 movs r3,r0
 movs r0,#0
 subs r0,r0,#1
 lsrs r0,r0,#16
 ldr r2,mcs_lit0
 b mcs1
mcs0:
 adds r3,r3,#1
 eors r1,r1,r0 
 lsls r1,r1,#24 
 lsrs r1,r1,#23
 ldrh r1,[r2,r1]
 lsrs r0,r0,#8
 eors r0,r0,r1
mcs1: 
 ldrb r1,[r3,#0]
 cmp r1,#0
 bne mcs0
 bx lr
 LALIGN
mcs_lit0:
 LONG sc_miscCrcTab
 SC_ENDFUNC sc_miscCrcString
 XSC_TFUNC sc_miscCrc,-8
 XSC_TFUNC sc_miscCrcContd,-4
 SC_TFUNC sc_miscCrc
 movs r2,#0
 subs r2,r2,#1
 SC_TPUBLIC sc_miscCrcContd
 push {r4,lr}
 movs r4,r0 
 lsls r0,r2,#16
 lsrs r0,r0,#16 
 cmp r1,#0 
 beq mcc9
 ldr r3,mcc_lit0 
mcc0:
 ldrb r2,[r4,#0]
 adds r4,r4,#1
 eors r2,r2,r0 
 lsls r2,r2,#24 
 lsrs r2,r2,#23
 ldrh r2,[r3,r2]
 lsrs r0,r0,#8
 eors r0,r0,r2
 subs r1,r1,#1
 bne mcc0
mcc9:
 RET r4
 LALIGN
mcc_lit0:
 LONG sc_miscCrcTab
 SC_ENDFUNC sc_miscCrc
 SC_FUNC sc_miscCrcTab
#ifndef __ADS__
 DATA
#endif
 WORD 0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf
 WORD 0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7
 WORD 0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e
 WORD 0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876
 WORD 0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd
 WORD 0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5
 WORD 0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c
 WORD 0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974
 WORD 0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb
 WORD 0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3
 WORD 0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a
 WORD 0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72
 WORD 0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9
 WORD 0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1
 WORD 0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738
 WORD 0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70
 WORD 0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7
 WORD 0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff
 WORD 0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036
 WORD 0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e
 WORD 0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5
 WORD 0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd
 WORD 0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134
 WORD 0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c
 WORD 0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3
 WORD 0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb
 WORD 0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232
 WORD 0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a
 WORD 0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1
 WORD 0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9
 WORD 0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330
 WORD 0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
 SC_ENDFUNC sc_miscCrcTab
 XSC_TFUNC sc_msgAcquire,-12
 SC_TFUNC sc_msgAcquire
 push {r0,r4-r7,lr} 
 IF SC_MSG_PARA_CHECK == 1
 cmp r0,#0 
 beq mac_err0 
 ldr r3,[r0,#0]
 cmp r3,#0 
 beq mac_err0 
 ELSE
 ldr r3,[r0,#0]
 ENDIF
 ldr r7,mac_sciopta
 ldr r6,mac_sc_modules
 subs r3,r3,#MSG_HEAD 
 ldr r1,[r3,#MSG_OWNER] 
 ldr r5,[r7,#SC_CURPID] 
 cmp r1,#0 
 beq mac_err1
 cmp r5,r1 
 beq mac_exit 
 IF SC_NEVER_COPY == 1
 ELSE
 lsrs r0,r5,#24 
 ldr r1,[r3,#MSG_PLID]
 lsls r1,r1,#1
 lsrs r1,r1,#25 
 cmp r0,r1 
 beq mac_nocopy 
 IF SC_USE_FRIENDS == 1
 lsls r1,r1,#2
 ldr r4,[r6,r1] 
 movs r1,#31
 ands r1,r1,r0 
 lsrs r0,r0,#5 
 lsls r0,r0,#2 
 adds r0,r0,#MODULE_FRIENDSET
 ldr r0,[r4,r0] 
 adds r1,r1,#1 
 lsrs r0,r0,r1 
 bcs mac_nocopy 
 ENDIF
 movs r4,r3 
 ldr r5,[r4,#MSG_SIZE] 
 movs r2,#1 
 subs r3,r2,#1+(-SC_FATAL_IF_TMO) 
 adds r2,r2,#SC_DEFAULT_POOL-1
 movs r0,r5 
 bl sc_msgAlloc
 push {r0,r4} 
 subs r0,r0,#MSG_HEAD 
 ldr r1,[r4,#MSG_SENDER] 
 ldr r2,[r4,#MSG_ADDRESSEE] 
 str r1,[r0,#MSG_SENDER] 
 str r2,[r0,#MSG_ADDRESSEE]
 adds r0,r0,#MSG_HEAD 
 adds r4,r4,#MSG_HEAD 
 lsrs r2,r5,#3 
 beq mac1 
mac0:
 ldmia r4!,{r1,r3} 
 subs r2,r2,#1
 stmia r0!,{r1,r3}
 bne mac0
mac1:
 lsrs r2,r5,#3 
 bcc mac12 
 ldmia r4!,{r3}
 stmia r0!,{r3}
mac12: 
 lsrs r2,r5,#2 
 bcc mac2 
 ldrh r3,[r4,#0]
 adds r4,r4,#2
 strh r3,[r0,#0]
 adds r0,r0,#2
mac2:
 lsrs r2,r5,#1 
 bcc mac3
 ldrb r3,[r4,#0]
 strb r3,[r0,#0]
mac3:
 pop {r4} 
 pop {r0} 
 bl sc_sysMsgFree 
 pop {r0} 
 str r4,[r0,#0] 
 RET r4-r7
 ENDIF
mac_nocopy:
 ldr r0,[r3,#MSG_OWNER] 
 lsrs r1,r0,#24 
 lsls r1,r1,#2
 ldr r1,[r6,r1] 
 ldr r1,[r1,#MODULE_PCBS]
 lsls r0,r0,#18 
 lsrs r0,r0,#16 
 ldr r2,[r1,r0] 
 LOCK 
 dbl_del(r3,r0,r1) 
 ldr r0,[r2,#PCB_NALLOC] 
 subs r0,r0,#1
 str r0,[r2,#PCB_NALLOC]
 ldr r0,[r7,#SC_CURPCB] 
 movs r1,r0 
 adds r1,r1,#PCB_ALLOCED 
 dbl_tailAdd(r1,r3,r2)
 ldr r1,[r0,#PCB_NALLOC] 
 str r5,[r3,#MSG_OWNER] 
 adds r1,r1,#1
 str r1,[r0,#PCB_NALLOC]
 UNLOCK
mac_exit:
 add sp,sp,#4 
 RET r4-r7
 IF SC_MSG_PARA_CHECK == 1
mac_err0:
 movs r1,r0 
 ldr r0,mac_lit0
 b mac_err_cont
 ENDIF
mac_err1:
 movs r1,r3
 ldr r0,mac_lit1
mac_err_cont: 
 bl sc_sysError
 LALIGN
mac_sciopta:
 LONG sciopta
mac_sc_modules:
 LONG sc_modules
 IF SC_MSG_PARA_CHECK == 1
mac_lit0:
 LONG (KERNEL_ENIL_PTR|SC_MSGACQUIRE|SC_ERR_MODULE_FATAL)
 ENDIF
mac_lit1:
 LONG (KERNEL_EMSG_HD_CORRUPT|SC_MSGACQUIRE|SC_ERR_MODULE_FATAL)
 SC_ENDFUNC sc_msgAcquire
 IF SC_MSG_HOOK == 1
 XSC_TFUNC sc_msgHookRegister,-8
 SC_TFUNC sc_msgHookRegister
 ldr r3,mhr_sciopta
 ldr r2,[r3,#SC_CURMODULE] 
 cmp r2,#0 
 beq mhr1
 IF SC_MSGTX_HOOK == 1
 movs r3,#MODULE_MSGTXHOOK
 cmp r0,#SC_SET_MSGTX_HOOK
 beq mhr2
 ENDIF
 IF SC_MSGRX_HOOK == 1
 movs r3,#MODULE_MSGRXHOOK
 cmp r0,#SC_SET_MSGRX_HOOK
 beq mhr2
 ENDIF
 IF SC_MSG_PARA_CHECK == 1
mhr_err:
 movs r1,r0
 ldr r0,mhr_illval0
 b mhr_err_cont 
 ENDIF
mhr1: 
 IF SC_MSGTX_HOOK == 1
 ldr r2,mhr_SC_MSGTXHOOK
 cmp r0,#SC_SET_MSGTX_HOOK
 beq mhr2
 ENDIF
 IF SC_MSGRX_HOOK == 1
 ldr r2,mhr_SC_MSGRXHOOK
 cmp r0,#SC_SET_MSGRX_HOOK
 beq mhr2
 ENDIF
 IF SC_MSG_PARA_CHECK == 1
 movs r1,r0
 ldr r0,mhr_illval1
mhr_err_cont: 
 bl sc_sysError
 ELSE
 movs r0,#0
 bx lr
 ENDIF
mhr2:
 ldr r0,[r2,r3]
 str r1,[r2,r3]
 bx lr
 LALIGN
mhr_sciopta:
 LONG sciopta
 IF SC_MSGTX_HOOK == 1
mhr_SC_MSGTXHOOK:
 LONG SC_MSGTXHOOK
 ENDIF
 IF SC_MSGRX_HOOK == 1
mhr_SC_MSGRXHOOK:
 LONG SC_MSGRXHOOK
 ENDIF
 IF SC_MSG_PARA_CHECK == 1
mhr_illval0:
 LONG SC_MSGHOOKREGISTER|KERNEL_EILL_VALUE|SC_ERR_MODULE_FATAL
mhr_illval1:
 LONG SC_MSGHOOKREGISTER|KERNEL_EILL_VALUE|SC_ERR_SYSTEM_FATAL
 ENDIF
 SC_ENDFUNC sc_msgHookRegister
 ENDIF
 XSC_TFUNC sc_msgAllocClr,0
 SC_TFUNC sc_msgAllocClr
 push {r0,lr} 
 bl sc_msgAlloc 
 pop {r2} 
 cmp r0,#0 
 beq smac10 
 subs r2,r2,#4 
 beq smac10 
 mov lr,r0 
 adds r0,r0,#4 
 movs r3,#0
 lsrs r1,r2,#2 
 beq smac1
smac0: 
 stmia r0!,{r3} 
 subs r1,r1,#1
 bne smac0
smac1:
 lsls r2,r2,#31
 bcc smac2 
 strh r3,[r0,#0]
 adds r0,r0,#2
smac2:
 lsls r2,r2,#1
 bcc smac9 
 strb r3,[r0,#0]
smac9:
 mov r0,lr 
smac10:
 EXIT
 SC_ENDFUNC sc_msgAllocClr
 XSC_TFUNC sc_msgAlloc,0
#if 1
#define PARA_SIZE 0
#define PARA_ID 4
#define PARA_TMO 12
 SC_TFUNC sc_sysMsgAlloc
 push {r0-r7,lr}
 b sma0
 SC_TPUBLIC sc_msgAlloc
 push {r0-r7,lr}
 ldr r7,sma_sciopta
 ldr r6,[r7,#SC_CURMODULE] 
sma0:
 ldr r5,[r7,#SC_CURPCB] 
 IF SC_MSG_PARA_CHECK == 1
 cmp r6,#0 
 beq sma_err6 
 movs r4,r3 
 beq sma02 
 adds r4,r4,#1
 beq sma01 
 adds r4,r4,#1
 beq sma02 
 lsrs r4,r3,#24
 beq sma01
 movs r1,r3
 ldr r0,sma_illValue
 b sma_err_cont 
sma_err6:
 movs r1,r6
 ldr r0,sma_illModule
 b sma_err_cont 
sma_err5:
 movs r1,#0
 ldr r0,sma_notPrio
 b sma_err_cont
sma01:
 ldrh r4,[r5,#PCB_FLAGS_H]
 lsrs r4,r4,#PCB_FLG_IS_PRIO_POS+1
 bcc sma_err5
sma02: 
 cmp r0,#4 
 blo sma_err0 
 ENDIF
 subs r4,r2,#1 
 cmp r4,#255
 bne sma1 
 ldrh r2,[r5,#PCB_DEFAULTPOOL_H] 
 lsls r4,r2,#2 
 ldr r7,[r6,#MODULE_POOL] 
 ldr r4,[r7,r4] 
 cmp r4,#1 
 bhi sma1 
 ldr r0,sma_illDefPool 
 movs r1,r2 
sma_err_cont: 
 bl sc_sysError
 mov r4,sp
 ldmia r4!,{r0-r3} 
 movs r2,#0 
 strh r2,[r5,#PCB_DEFAULTPOOL_H]
 b sma1
sma_err0:
 movs r2,#KERNEL_EILL_BUFSIZE>>12 
 movs r1,r0 
sma_cont:
 lsls r2,r2,#12 
 movs r0,#SC_MSGALLOC>>24
 lsls r0,r0,#24 
 orrs r0,r0,r2 
 bl sc_sysError
 add sp,sp,#4*4 
 movs r0,#0
 RET r4-r7
sma_err2:
 movs r2,#KERNEL_EILL_BUFSIZE>>12
 movs r1,r0
 LOCK
sma_cont2:
 ldr r3,[r7,#POOL_LOCK]
 subs r3,r3,#1
 str r3,[r7,#POOL_LOCK]
 IF SC_MSG_PARA_CHECK == 1
 b sma_cont1
sma_err1:
 movs r1,r2
 movs r2,#KERNEL_EILL_POOL_ID>>12
 ENDIF
sma_cont1:
 UNLOCK
sma_cont3:
 lsls r2,r2,#12
 movs r0,#SC_MSGALLOC>>24
 lsls r0,r0,#24
 orrs r0,r0,r2
 adds r0,r0,#SC_ERR_MODULE_FATAL
 b sma_err_cont 
sma1:
 LOCK
 IF SC_MSG_PARA_CHECK == 1
 ldr r4,[r6,#MODULE_MAX_POOLS] 
 cmp r2,r4
 bhs sma_err1
 ENDIF
 lsls r4,r2,#2
 ldr r7,[r6,#MODULE_POOL]
 ldr r7,[r7,r4] 
 IF SC_MSG_PARA_CHECK == 1
 cmp r7,#1 
 bls sma_err1 
 ENDIF
 ldr r4,[r7,#POOL_LOCK] 
 adds r4,r4,#1 
 str r4,[r7,#POOL_LOCK]
 UNLOCK
 IF CPUTYPE <= Xscale 
 movs r4,#POOL_BUFSIZE
 adds r4,r4,r7 
 IF SC_MAX_NUM_BUFFERSIZES == 16
 ldr r6,[r4,#7*4] 
 cmp r0,r6
 bls sma2
 adds r4,r4,#8*4
sma2:
 ENDIF
 IF SC_MAX_NUM_BUFFERSIZES >= 8
 ldr r6,[r4,#3*4]
 cmp r0,r6
 bls sma3
 adds r4,r4,#4*4
sma3:
 ENDIF
 ldr r6,[r4,#1*4]
 cmp r0,r6
 bls sma4
 adds r4,r4,#2*4
sma4:
 ldr r6,[r4,#0]
 cmp r0,r6
 bls sma5
 ldr r6,[r4,#4]
 adds r4,r4,#4
 cmp r0,r6
 bhi sma_err2
sma5:
 subs r4,r4,r7
 subs r4,r4,#POOL_BUFSIZE 
 IF SC_MSG_STAT == 1
 movs r1,r7
 IF SC_MAX_NUM_BUFFERSIZES > 8
 adds r1,r1,#POOL_STAT_REQ-255
 adds r1,r1,#255
 ELSE
 adds r1,r1,#POOL_STAT_REQ
 ENDIF
 ldr r2,[r1,r4] 
 adds r2,r2,#1
 str r2,[r1,r4]
 IF SC_MAX_NUM_BUFFERSIZES > 8
 adds r1,r1,#POOL_STAT_MAX-POOL_STAT_REQ-1
 adds r1,r1,#1
 ELSE
 adds r1,r1,#POOL_STAT_MAX-POOL_STAT_REQ
 ENDIF
 ldr r2,[r1,r4]
 cmp r0,r2
 bls sma6
 str r0,[r1,r4] 
sma6:
 ENDIF
 ELSE 
 movs r4,r7 
 IF SC_MAX_NUM_BUFFERSIZES == 16
 ldr r6,[r4,#POOL_BUFSIZE+7*4] 
 cmp r0,r6
 it hi
 addhi r4,r4,#8*4
 ENDIF
 IF SC_MAX_NUM_BUFFERSIZES >= 8
 ldr r6,[r4,#POOL_BUFSIZE+3*4]
 cmp r0,r6
 it hi
 addhi r4,r4,#4*4
 ENDIF
 ldr r6,[r4,#POOL_BUFSIZE+1*4]
 cmp r0,r6
 it hi
 addhi r4,r4,#2*4
 ldr r6,[r4,#POOL_BUFSIZE+0]
 cmp r0,r6
 bls sma5
 ldr r6,[r4,#POOL_BUFSIZE+4]
 adds r4,r4,#4
 cmp r0,r6
 bhi sma_err2
sma5:
 subs r4,r4,r7 
 IF SC_MSG_STAT == 1
 add r1,r7,#POOL_STAT_REQ
 ldr r2,[r1,r4] 
 adds r2,r2,#1
 str r2,[r1,r4]
 adds r1,r1,#POOL_STAT_MAX-POOL_STAT_REQ
 ldr r2,[r1,r4]
 cmp r0,r2
 it hi
 strhi r0,[r1,r4] 
 ENDIF 
 ENDIF 
 adds r4,r4,r4 
 LOCK
sma7:
 movs r1,r7
 adds r1,r1,#POOL_FREED 
 ldr r0,[r1,r4] 
 adds r1,r1,r4
 cmp r0,r1 
 beq sma10 
 ldr r2,[r0,#dbl_prev] 
 str r2,[r1,#dbl_prev] 
 str r1,[r2,#dbl_next]
sma8:
 movs r2,r5 
 adds r2,r2,#PCB_ALLOCED
 dbl_tailAdd(r2,r0,r1)
 ldr r1,[r5,#PCB_NALLOC] 
 adds r1,r1,#1
 str r1,[r5,#PCB_NALLOC]
 ldr r2,[r7,#POOL_ID] 
 ldr r1,[sp,#PARA_SIZE+4] 
 str r4,[r0,#MSG_BUFSIZE_IDX]
 str r2,[r0,#MSG_PLID]
 str r1,[r0,#MSG_SIZE]
 IF SC_MSG_CHECK == 1
 adds r1,r1,r0 
 adds r1,r1,#4 
 movs r2,#0xee 
 strb r2,[r1,#MSG_ID-4] 
 strb r2,[r1,#MSG_ID-3]
 strb r2,[r1,#MSG_ID-2]
 strb r2,[r1,#MSG_ID-1]
 ENDIF
 ldr r2,[r5,#PCB_PID]
 str r2,[r0,#MSG_OWNER] 
 ldrh r2,[r5,#PCB_STATE_H] 
 movs r1,#ST_WAIT_TMO
 tst r2,r1
 beq sma9
 removeTimer(r5,r2,r1,r3) 
sma9:
 movs r1,#ST_TMO
 bics r2,r2,r1
 strh r2,[r5,#PCB_STATE_H]
 UNLOCK
 ldr r2,[sp,#PARA_ID]
 movs r1,#0
 str r1,[r0,#MSG_SENDER] 
 str r1,[r0,#MSG_ADDRESSEE] 
 str r2,[r0,#MSG_ID] 
 adds r0,r0,#MSG_ID 
 add sp,sp,#4*4 
 IF SC_MSG_STAT == 1
 IF SC_MAX_NUM_BUFFERSIZES > 8
 adds r7,r7,#255
 adds r7,r7,#POOL_STAT_ALLOC-124-255
 ELSE
 adds r7,r7,#POOL_STAT_ALLOC-124
 ENDIF
 lsrs r2,r4,#1
 adds r7,r7,r2
 ldr r2,[r7,#124] 
 adds r2,r2,#1
 str r2,[r7,#124]
 ENDIF
 RET r4-r7
sma10:
 ldr r0,[r7,#POOL_CUR] 
 ldr r2,[r7,#POOL_END] 
 IF SC_MSG_CHECK == 1
 adds r1,r0,#4 
 ELSE
 movs r1,r0
 ENDIF
 adds r1,r1,r6 
 adds r1,r1,#MSG_HEAD 
 cmp r1,r2 
 bhs sma12 
 str r1,[r7,#POOL_CUR] 
 IF SC_MSG_CHECK == 1
 movs r2,#0xee 
 lsls r3,r2,#8
 orrs r2,r2,r3
 lsls r3,r2,#16
 orrs r2,r2,r3
 subs r1,r1,#4
 str r2,[r1,#0]
 ENDIF
 b sma8 
sma7_cont:
 b sma7
sma12:
 adds r0,r3,#-SC_FATAL_IF_TMO 
 beq sma_err3 
 cmp r3,#SC_NO_TMO 
 beq sma18 
 ldrh r0,[r5,#PCB_STATE_H] 
 movs r2,#ST_TMO
 tst r0,r2
 bne sma17 
 ldr r2,sma_sciopta 
 makeNotReady(r5,r0,r1,r2,sma)
 adds r0,r7,r4 
 adds r0,r0,#POOL_WAITER 
 adds r5,r5,#PCB_PCBLIST
 dbl_tailAdd(r0,r5,r1)
 subs r5,r5,#PCB_PCBLIST
 adds r0,r3,#-SC_ENDLESS_TMO 
 beq sma13 
 IF CPUTYPE <= Xscale
 enterTimer(r5,r3,r0,r1,r2,sma) 
 ELSE
 enterTimer_T2(r5,r3,r0,r1,r2,sma)
 ENDIF
 movs r0,#ST_WAIT_TMO
sma13:
 ldrh r1,[r5,#PCB_STATE_H]
 movs r2,#ST_WAIT_MSGALLOC
 orrs r1,r1,r0
 orrs r1,r1,r2
 strh r1,[r5,#PCB_STATE_H] 
 UNLOCK
 SWAP_OUT_SAVE {r4-r7} 
 movs r3,#0
 subs r3,r3,#-SC_ENDLESS_TMO 
 movs r1,#ST_WAIT_MSGALLOC
 movs r2,#ST_TMO
 LOCK
 cmp r0,#0 
 ldrh r0,[r5,#PCB_STATE_H] 
 beq sma_err4 
 tst r0,r1 
 beq sma7_cont 
 bics r0,r0,r1 
 tst r0,r2 
 beq sma_ill_state 
sma17: 
 bics r0,r0,r2
 strh r0,[r5,#PCB_STATE_H] 
sma18:
 ldr r0,[r7,#POOL_LOCK] 
 subs r0,r0,#1
 str r0,[r7,#POOL_LOCK]
 UNLOCK
 movs r0,#0 
 add sp,sp,#4*4 
 RET r4-r7
sma_err3:
 movs r2,#KERNEL_EOUT_OF_MEMORY>>12
 ldr r1,[sp,#PARA_SIZE+4]
 b sma_cont2
sma_err4:
 movs r1,#ST_WAIT_TMO
 tst r0,r1
 beq sma_err4a
 removeTimer(r5,r0,r1,r3) 
 strh r0,[r5,#PCB_STATE_H]
sma_err4a:
 UNLOCK
 movs r2,#KERNEL_ELOCKED>>12
 movs r1,#0
 b sma_cont3
sma_ill_state: 
 movs r1,r0
 ldr r0,sma_illValue
 b sma_err_cont 
 LALIGN
sma_sciopta:
 LONG sciopta
sma_illDefPool:
 LONG (KERNEL_EILL_DEFPOOL_ID|SC_MSGALLOC)
sma_illValue:
 LONG (KERNEL_EILL_VALUE|SC_MSGALLOC|SC_ERR_MODULE_FATAL)
 IF SC_MSG_PARA_CHECK == 1
sma_illModule:
 LONG (KERNEL_EILL_MODULE|SC_MSGALLOC|SC_ERR_SYSTEM_FATAL)
sma_notPrio:
 LONG (KERNEL_EPROC_NOT_PRIO|SC_MSGALLOC|SC_ERR_MODULE_FATAL)
 ENDIF
 SC_ENDFUNC sc_sysMsgAlloc
#undef PARA_SIZE
#undef PARA_ID
#undef PARA_TMO
#endif
 XSC_TFUNC sc_msgFree,-12
 SC_TFUNC sc_msgFreex
 IF SC_MSG_PARA_CHECK == 1
smf_err0:
 ldr r0,smf_err
 bl sc_sysError
 ENDIF
 SC_TPUBLIC sc_msgFree
 IF SC_MSG_PARA_CHECK == 1
 movs r1,r0 
 beq smf_err0 
 ldr r0,[r1,#0] 
 cmp r0,#0 
 beq smf_err0 
 movs r2,#0
 str r2,[r1,#0] 
 ELSE
 ldr r3,[r0,#0]
 movs r2,#0
 str r2,[r0,#0]
 movs r0,r3
 ENDIF
 subs r0,r0,#MSG_HEAD 
 IF SC_MSG_CHECK == 1
 push {r4-r7,lr}
 movs r1,#SC_MSGFREE>>24
 lsls r1,r1,#24
 bl sc_msgCheck 
 b smf0
 ENDIF
 SC_TPUBLIC sc_sysMsgFree
 push {r4-r7,lr}
smf0:
 ldr r3,[r0,#MSG_PLID] 
 ldr r6,smf_sc_modules
 lsls r4,r3,#1 
 lsrs r4,r4,#24-1 
 ldr r5,[r6,r4] 
 ldr r4,smf_sciopta
 ldr r1,[r5,#MODULE_POOL] 
 lsls r3,r3,#8 
 lsrs r3,r3,#6 
 ldr r3,[r1,r3] 
 ldr r5,[r0,#MSG_BUFSIZE_IDX] 
 IF SC_MSG_STAT == 1
 lsrs r1,r5,#1 
 adds r1,r1,#255
 IF SC_MAX_NUM_BUFFERSIZES > 8
 adds r1,r1,#255
 adds r1,r1,#POOL_STAT_FREE-510
 ELSE
 adds r1,r1,#POOL_STAT_FREE-255
 ENDIF
 ldr r2,[r3,r1] 
 adds r2,r2,#1
 str r2,[r3,r1]
 ENDIF
 movs r1,#0
 LOCK
 ldr r2,[r0,#MSG_OWNER] 
 str r1,[r0,#MSG_OWNER] 
 str r1,[r0,#MSG_SENDER] 
 str r2,[r0,#MSG_ADDRESSEE] 
 cmp r2,#0 
 beq smf1 
 lsrs r1,r2,#24 
 lsls r1,r1,#2
 ldr r1,[r6,r1] 
 lsls r6,r2,#18 
 lsrs r6,r6,#18-2
 cmp r1,#0 
 beq smf1
 ldr r1,[r1,#MODULE_PCBS] 
 ldr r6,[r1,r6] 
 cmp r6,#0
 beq smf1 
 ldr r1,[r6,#PCB_PID] 
 cmp r1,r2
 bne smf1 
 ldr r1,[r6,#PCB_NALLOC]
 subs r1,r1,#1
 str r1,[r6,#PCB_NALLOC]
smf1:
 dbl_del(r0,r1,r2) 
 adds r1,r5,r3 
 adds r1,r1,#POOL_FREED
 dbl_tailAdd(r1,r0,r2) 
 ldr r1,[r3,#POOL_LOCK] 
 subs r1,r1,#1
 str r1,[r3,#POOL_LOCK]
 adds r5,r5,r3 
 adds r5,r5,#POOL_WAITER
 ldr r0,[r5,#dbl_next]
 cmp r0,r5 
 bne smf3
smf2:
 UNLOCK
 RET r4-r7
smf3:
 ldr r2,[r0,#dbl_next] 
 str r2,[r5,#dbl_next]
 str r5,[r2,#dbl_prev]
 subs r0,r0,#PCB_PCBLIST 
 ldrh r1,[r0,#PCB_STATE_H] 
 movs r2,#ST_WAIT_MSGALLOC 
 bics r1,r1,r2
 strh r1,[r0,#PCB_STATE_H]
 lsrs r1,r1,#ST_STOPPED_POS+1 
 bcs smf2 
 makeReady(r0,r1,r2,r3,r4,smf) 
 UNLOCK
 ldrb r1,[r0,#PCB_PRIO_B] 
 ldr r2,[r4,#SC_CURPRIOPCB]
 ldrb r2,[r2,#PCB_PRIO_B]
 cmp r1,r2
 bhs smf5 
 SWAP_OUT
smf5:
 RET r4-r7
 LALIGN
smf_sciopta:
 LONG sciopta
smf_sc_modules:
 LONG sc_modules
 IF SC_MSG_PARA_CHECK == 1
smf_err:
 LONG KERNEL_ENIL_PTR|SC_MSGFREE|SC_ERR_MODULE_FATAL
 ENDIF
 SC_ENDFUNC sc_msgFreex
#define PARA_TYPE 0
#define PARA_MSGPTR 4
#define PARA_ADDR 8
#define PARA_FLAGS 12
#define PARA_ALIAS 16
#define PARA_TYPE0 40
 XSC_TFUNC sc_msgTx,-4
 XSC_TFUNC sc_msgTxAlias,0
 SC_TFUNC sc_msgTx
 movs r3,#0 
 push {r0-r7,lr} 
 movs r4,#SC_MSGTX>>24 
 lsls r4,r4,#24
 b mtx1 
 SC_TPUBLIC sc_msgTxAlias
 push {r0-r7,lr} 
 movs r4,#SC_MSGTXALIAS>>24 
 lsls r4,r4,#24
 b mtx1 
 IF SC_MSG_PARA_CHECK == 1
mtx_err0:
 movs r1,r0
 ldr r0,mtx_lit1
 pop {r2}
 orrs r0,r0,r2
 bl sc_sysError
 ENDIF
 SC_TPUBLIC sc_sysMsgTx
 push {r0-r7,lr} 
mtx1:
 push {r4} 
 IF SC_MSG_PARA_CHECK == 1
 cmp r0,#0 
 beq mtx_err0 
 movs r3,#0
 ldr r4,[r0,#0]
 cmp r4,#0 
 beq mtx_err0 
 str r3,[r0,#0] 
 ELSE
 ldr r4,[r0,#0]
 movs r3,#0
 str r3,[r0,#0]
 ENDIF
 subs r4,r4,#MSG_HEAD 
 IF SC_MSG_CHECK == 1
 movs r0,r4
 push {r1,r4} 
 ldr r1,[sp,#8] 
 bl sc_msgCheck
 pop {r1,r4}
 ENDIF
 ldr r7,mtx_sciopta
 adds r0,r1,#-SC_CURRENT_PID 
 bne mtx2
 ldr r1,[r7,#SC_CURPID] 
mtx2:
 LOCK
 IF SC_PROC_PARA_CHECK == 1
 ldr r2,mtx_lit6
 cmp r1,r2 
 beq mtx_err3 
 ENDIF
 movs r5,#0 
 cmp r1,#0 
 IF SC_PROC_PARA_CHECK == 1
 beq mtx_err3 
 ENDIF 
 bpl mtx3 
 IF SC_MAX_CONNECTOR > 0
 movs r5,r1 
 lsls r0,r1,#1
 lsrs r0,r0,#25 
 cmp r0,#SC_MAX_CONNECTOR 
 bhs mtx_err2 
 ldr r2,mtx_sc_connectors
 lsls r0,r0,#2
 ldr r0,[r2,r0] 
 cmp r1,#1 
 bls mtx_err2 
 ldr r1,[r0,#PCB_PID] 
 b mtx3 
mtx_err2:
 ENDIF
mtx_err3:
 ldr r0,mtx_lit4 
mtx_err_cont1:
 UNLOCK
 pop {r2}
 orrs r0,r0,r2
 bl sc_sysError
mtx4:
 IF SC_MAX_CONNECTOR > 0
 ldr r0,mtx_sc_connectors
 ldr r7,[r0,#0] 
 cmp r7,#0 
 beq mtx5 
 movs r5,r1
 ldr r1,[r7,#PCB_PID] 
 ldr r6,mtx_sc_modules
 lsrs r0,r1,#24
 lsls r0,r0,#2
 ldr r6,[r6,r0] 
 b mtx6
 ENDIF
mtx5:
 UNLOCK
 movs r0,r4
 bl sc_sysMsgFree
 add sp,sp,#5*4
 RET r4-r7
mtx3:
 lsrs r0,r1,#24 
 IF SC_PROC_PARA_CHECK == 1
 cmp r0,#SC_MAX_MODULE 
 bhs mtx_err3 
 ENDIF
 ldr r6,mtx_sc_modules
 lsls r0,r0,#2
 ldr r6,[r6,r0] 
 IF SC_PROC_PARA_CHECK == 1
 cmp r6,#0 
 beq mtx_err3 
 ENDIF
 lsls r0,r1,#18
 lsrs r0,r0,#18-2 
 IF SC_PROC_PARA_CHECK == 1
 ldr r2,[r6,#MODULE_MAX_PROCESS]
 lsrs r7,r0,#2 
 cmp r7,r2
 bhs mtx_err3
 ENDIF 
 ldr r2,[r6,#MODULE_PCBS]
 ldr r7,[r2,r0] 
 cmp r7,#0 
 beq mtx4 
 ldr r0,[r7,#PCB_PID] 
 cmp r0,r1 
mtx4_cont: 
 bne mtx4 
mtx6:
 str r1,[sp,#PARA_ADDR+4] 
 IF SC_NEVER_COPY == 1
 ELSE
 ldr r2,mtx_sciopta
 ldr r2,[r2,#SC_CURMODULE]
 cmp r2,r6 
 beq mtx11 
 IF SC_USE_RP == 1
 ldrh r2,[r6,#MODULE_RPIDX_H] 
 cmp #0,r2 
 beq mtx6a 
 adds r2,r2,#MODULE_PCBS]
 ldr r2,[r6,r2]
 ldr r2,[r2,#PCB_PID]
mtx6a:
 ENDIF
 IF SC_USE_FRIENDS == 1
 movs r2,#31
 lsrs r0,r1,#24 
 ands r2,r2,r0 
 lsrs r0,r0,#5 
 ldr r3,[r4,#MSG_PLID] 
 ldr r1,mtx_sc_modules 
 lsls r3,r3,#1 
 lsrs r3,r3,#25 
 lsls r3,r3,#2 
 ldr r1,[r1,r3] 
 lsls r0,r0,#2 
 adds r0,r0,#MODULE_FRIENDSET 
 ldr r3,[r1,r0] 
 adds r2,r2,#1 
 lsrs r3,r3,r2 
 bcs mtx11 
 ENDIF 
mtx7:
 UNLOCK
 push {r7} 
 movs r3,#-SC_FATAL_IF_TMO 
 NEG(r3) 
 ldrh r2,[r7,#PCB_DEFAULTPOOL_H]
 ldr r1,[r4,#MSG_ID]
 ldr r0,[r4,#MSG_SIZE]
 ldr r7,mtx_sciopta
 bl sc_sysMsgAlloc
 pop {r7} 
 ldr r1,[r4,#MSG_SIZE] 
 push {r0,r4}
 adds r4,r4,#MSG_HEAD
 lsrs r2,r1,#2
mtx8:
 ldmia r4!,{r3}
 subs r2,r2,#1
 stmia r0!,{r3}
 bne mtx8
 lsrs r2,r1,#2
 bcc mtx9
 ldrh r3,[r4,#0]
 strh r3,[r0,#0]
 adds r4,r4,#2
 adds r0,r0,#2
mtx9:
 lsrs r2,r1,#1
 bcc mtx10
 ldrb r3,[r4,#0]
 strb r3,[r0,#0]
mtx10:
 pop {r0,r1}
 movs r4,r0
 movs r0,r1
 bl sc_sysMsgFree 
 subs r4,r4,#MSG_HEAD
 LOCK
 ENDIF 
mtx11:
 ldr r6,mtx_sciopta 
 dbl_del(r4,r0,r1)
 ldr r0,[r6,#SC_CURPCB] 
 ldr r1,[r0,#PCB_NALLOC] 
 subs r1,r1,#1
 str r1,[r0,#PCB_NALLOC]
 UNLOCK
 ldr r0,[r4,#MSG_OWNER] 
 ldr r3,[sp,#PARA_ALIAS] 
 cmp r3,#0 
 beq mtx13 
 movs r0,r3
mtx13:
 str r0,[r4,#MSG_SENDER] 
 ldr r0,[sp,#PARA_ADDR] 
 cmp r5,#0 
 bne mtx14 
 movs r5,r0 
mtx14:
 str r5,[r4,#MSG_ADDRESSEE] 
 movs r5,#0
 str r5,[r4,#MSG_OWNER] 
 LOCK 
 ldr r1,[r7,#PCB_PID] 
 cmp r1,r0 
 bne mtx4_cont 
 ldr r1,[sp,#PARA_FLAGS+4] 
 cmp r1,#0 
 bne mtx20 
 IF SC_MSG_HOOK == 1 
 IF SC_MSGTX_HOOK == 1
 ldr r3,[r6,#SC_CURMODULE]
 ldr r2,[r3,#MODULE_MSGTXHOOK]
 cmp r2,#0
 beq mtx15
 movs r0,r4
 BLX_R2_t 
mtx15:
 ldr r1,mtx_lit8
 ldr r2,[r1,#0]
 cmp r2,#0
 beq mtx16
 movs r0,r4
 BLX_R2_t 
mtx16: 
 ENDIF
 ENDIF
 movs r0,#PCB_MSGQUEUE 
 adds r0,r0,r7
 dbl_tailAdd(r0,r4,r1)
 ldr r0,[r7,#PCB_NQUEUE] 
 adds r0,r0,#1
 str r0,[r7,#PCB_NQUEUE]
 ldr r1,[r7,#PCB_FLAGS_H] 
 IF LITTLE_ENDIAN == 1
 lsrs r0,r1,#PCB_FLG_IS_PRIO_POS+1
 ELSE
 lsrs r0,r1,#PCB_FLG_IS_PRIO_POS+1+16
 ENDIF
 bcc mtx18 
 IF LITTLE_ENDIAN == 0
 lsls r1,r1,#16 
 ENDIF
 lsrs r1,r1,#16 
 movs r0,#ST_WAIT_MSGRX 
 tst r1,r0
 beq mtx_exit 
 bics r1,r1,r0 
 strh r1,[r7,#PCB_STATE_H] 
 lsrs r1,r1,#ST_STOPPED_POS+1 
 bcs mtx_exit 
 makeReady(r7,r0,r1,r2,r6,mtx) 
 UNLOCK
 add sp,sp,#5*4 
 ldr r0,[r6,#SC_CURPRIOPCB]
 ldrb r4,[r7,#PCB_PRIO_B] 
 ldrb r1,[r0,#PCB_PRIO_B]
 cmp r4,r1 
 bhs mtx17c 
 SWAP_OUT
mtx17c:
 RET r4-r7
mtx18:
 lsrs r0,r1,#PCB_FLG_IS_IRQ_POS+1
 bcs mtx18a 
 lsrs r0,r1,#PCB_FLG_IS_TIMER_POS+1
 bcc mtx_err4 
mtx18a: 
 UNLOCK
 add sp,sp,#5*4 
 IF LITTLE_ENDIAN == 1
 lsrs r0,r1,#PCB_FLG_WAKEUP_DIS_POS+1
 ELSE
 lsrs r0,r1,#PCB_FLG_WAKEUP_DIS+1+16
 ENDIF
 bcs mtx_exit1
 movs r0,#1 
 movs r1,r7 
 FAKE_INT
 ldr r0,[r6,#SC_SCHEDULE_REQ] 
 cmp r0,#0
 beq mtx_exit1 
 SWAP_OUT
mtx_exit1:
 RET r4-r7
mtx19:
 b mtx_err_cont1
mtx20:
 lsrs r0,r1,#SC_MSGTX_OBSERVE_POS+1 
 ldr r0,mtx_lit7
 bcc mtx19 
 movs r0,#PCB_CONNECTED 
 adds r0,r0,r7
 dbl_tailAdd(r0,r4,r1)
 ldr r0,[r7,#PCB_NALLOC] 
 adds r0,r0,#1
 str r0,[r7,#PCB_NALLOC]
mtx_exit:
 UNLOCK
 add sp,sp,#5*4
 RET r4-r7
mtx_err4:
 ldr r0,mtx_illproc_type
 ldr r1,[r7,#PCB_PID] 
 b mtx_err_cont1
 LALIGN
 IF SC_MSG_PARA_CHECK == 1
mtx_lit1:
 LONG KERNEL_ENIL_PTR|SC_ERR_MODULE_FATAL
 ENDIF
mtx_sciopta:
 LONG sciopta
 IF SC_MAX_CONNECTOR > 0
mtx_sc_connectors:
 LONG sc_connectors
 ENDIF
mtx_lit4:
 LONG KERNEL_EILL_PID|SC_ERR_MODULE_FATAL
mtx_sc_modules:
 LONG sc_modules
mtx_lit6:
 LONG SC_ILLEGAL_PID
mtx_lit7:
 LONG KERNEL_EILL_VALUE|SC_ERR_MODULE_FATAL
 IF SC_MSG_HOOK == 1 
 IF SC_MSGTX_HOOK == 1
mtx_lit8:
 LONG sciopta+SC_MSGTXHOOK
 ENDIF
 ENDIF
mtx_illproc_type: 
 LONG KERNEL_EILL_PROCTYPE|SC_ERR_MODULE_FATAL
 SC_ENDFUNC sc_msgTx
#undef PARA_MSGPTR
#undef PARA_ADDR
#undef PARA_FLAGS
#undef PARA_ALIAS
#undef PARA_TYPE
#define PARA_TMO 0
#define PARA_WANTED 4
#define PARA_FLAGS 8
 XSC_TFUNC sc_msgRx,-4
 SC_TFUNC sc_msgRx
 push {r0-r2,r4-r7,lr} 
 ldr r7,mrx_sciopta 
 ldr r6,[r7,#SC_CURPCB]
 IF SC_MSG_PARA_CHECK == 1
 ldrh r5,[r6,#PCB_FLAGS_H] 
 cmp r0,#SC_NO_TMO
 beq mrx0
 lsrs r5,r5,#PCB_FLG_IS_PRIO_POS+1 
 bcc mrx_err2 
 adds r3,r0,#-(SC_ENDLESS_TMO) 
 beq mrx0 
 lsrs r3,r0,#24 
 bne mrx_err0 
mrx0:
 movs r3,#SC_MSGRX_BOTH
 ands r3,r3,r2
 bne mrx1 
 cmp r1,#0 
 beq mrx1 
 ldr r3,[r1,#0] 
 cmp r3,#0
 bne mrx_err1 
mrx1:
 ENDIF
 adds r6,r6,#PCB_MSGQUEUE 
 cmp r1,#0 
 bne rx_w_select 
rx_wo_select:
 LOCK
 b mrx3a
mrx3:
 bl mrx_empty
mrx3a:
 ldr r4,[r6,#dbl_next] 
 cmp r6,r4 
 beq mrx3 
mrx_found_msg:
 dbl_del(r4,r0,r1) 
 subs r6,r6,#PCB_MSGQUEUE
 ldr r0,[r6,#PCB_NQUEUE] 
 ldr r1,[r6,#PCB_NALLOC]
 subs r0,r0,#1
 adds r1,r1,#1
 str r0,[r6,#PCB_NQUEUE]
 str r1,[r6,#PCB_NALLOC]
 movs r0,r6
 adds r0,r0,#PCB_ALLOCED 
 dbl_tailAdd(r0,r4,r1)
 ldr r0,[r6,#PCB_PID] 
 str r0,[r4,#MSG_OWNER] 
 ldr r1,[r6,#PCB_FLAGS_H] 
 IF LITTLE_ENDIAN == 1
 lsrs r0,r1,#PCB_FLG_IS_PRIO_POS+1
 bcc mrx42 
 lsrs r1,r1,#16 
 ELSE
 lsrs r0,r1,#PCB_FLG_IS_PRIO_POS+1+16
 bcc mrx42 
 ENDIF
 movs r0,#ST_WAIT_TMO 
 tst r1,r0 
 beq mrx41
 removeTimer(r6,r1,r0,r2) 
mrx41:
 movs r0,#ST_TMO 
 bics r1,r1,r0
 strh r1,[r6,#PCB_STATE_H]
mrx42: 
 UNLOCK
 movs r0,r4
 IF SC_MSG_HOOK == 1
 IF SC_MSGRX_HOOK == 1
 ldr r1,[r7,#SC_CURMODULE]
 ldr r2,[r1,#MODULE_MSGRXHOOK]
 cmp r2,#0
 beq mrx4
 BLX_R2_t 
 movs r0,r4
mrx4:
 ldr r2,mrx_SC_MSGRXHOOK
 ldr r2,[r7,r2]
 cmp r2,#0
 beq mrx5
 BLX_R2_t 
 movs r0,r4
mrx5: 
 ENDIF
 ENDIF
 adds r0,r0,#MSG_HEAD 
 add sp,sp,#3*4 
 RET r4-r7
mrx_err0:
 movs r1,r0
 ldr r0,mrx_err0_const
 IF SC_MSG_PARA_CHECK == 1
 b mrx_err_cont
mrx_err1:
 movs r1,#0
 ldr r0,mrx_err1_const
 ENDIF
mrx_err_cont: 
 bl sc_sysError
 IF SC_MSG_PARA_CHECK == 1
 movs r2,#SC_MSGRX_MSGID
 str r2,[sp,#PARA_FLAGS]
 ldr r1,[sp,#PARA_WANTED]
 ldr r0,[sp,#PARA_TMO]
 b mrx1
mrx_err2:
 movs r1,#0
 ldr r0,mrx_err2_const
 b mrx_err_cont
 ENDIF
rx_w_select:
 ldr r3,[r1,#0]
 cmp r3,#0 
 beq rx_wo_select
 movs r4,r6 
 lsrs r2,r2,#1 
 bcs rx_pid 
 lsrs r2,r2,#2 
 bcs rx_notmsgid 
mrx9:
 movs r5,r4 
 LOCK
 bl rx_msgid 
rx_msgid:
 ldr r4,[r5,#dbl_next] 
 cmp r6,r4 
 beq mrx_empty 
 UNLOCK
 ldr r0,[sp,#PARA_WANTED] 
 ldr r1,[r4,#MSG_ID] 
mrx10:
 ldmia r0!,{r2} 
 cmp r2,#0 
 beq mrx9 
 cmp r2,r1 
 bne mrx10 
mrx_found_msg2: 
 LOCK 
 b mrx_found_msg 
rx_notmsgid:
 movs r5,r4 
 LOCK
 bl mrx11 
mrx11:
 ldr r4,[r5,#dbl_next] 
 cmp r6,r4 
 beq mrx_empty 
 UNLOCK
 ldr r0,[sp,#PARA_WANTED]
 ldr r1,[r4,#MSG_ID] 
mrx12:
 ldmia r0!,{r2} 
 cmp r2,#0 
 beq mrx_found_msg2 
 cmp r2,r1 
 beq rx_notmsgid 
 b mrx12 
rx_pid:
 lsrs r2,r2,#1 
 bcs rx_both 
 lsrs r2,r2,#1 
 bcs rx_notpid 
mrx13: 
 movs r5,r4
 LOCK
 bl mrx14
mrx14: 
 ldr r4,[r5,#dbl_next]
 cmp r6,r4
 beq mrx_empty
 UNLOCK
 ldr r0,[sp,#PARA_WANTED]
 ldr r1,[r4,#MSG_SENDER]
mrx15:
 ldmia r0!,{r2}
 cmp r2,#0
 beq mrx13
 cmp r2,r1
 bne mrx15
 LOCK
 b mrx_found_msg
rx_notpid:
 movs r5,r4
 LOCK
 bl mrx16
mrx16:
 ldr r4,[r5,#dbl_next]
 cmp r6,r4
 beq mrx_empty
 UNLOCK
 ldr r0,[sp,#PARA_WANTED]
 ldr r1,[r4,#MSG_SENDER]
mrx17:
 ldmia r0!,{r2}
 cmp r2,#0
 beq mrx_found_msg2
 cmp r2,r1
 beq rx_notpid
 b mrx17
rx_both:
 lsrs r2,r2,#1
 bcs rx_notboth
mrx18:
 movs r5,r4
 LOCK
 bl mrx19
mrx19:
 ldr r4,[r5,#dbl_next]
 cmp r6,r4
 beq mrx_empty
 UNLOCK
 ldr r0,[sp,#PARA_WANTED]
 ldr r1,[r4,#MSG_ID]
 ldr r2,[r4,#MSG_SENDER]
mrx20:
 ldmia r0!,{r3,r5} 
 cmp r3,#0 
 beq mrx18 
 cmp r5,#0 
 beq mrx18 
 cmp r1,r5
 bne mrx20
 cmp r2,r3
 bne mrx20
 LOCK
 b mrx_found_msg
rx_notboth:
 movs r5,r4
 LOCK
 bl mrx21
mrx21:
 ldr r4,[r5,#dbl_next]
 cmp r6,r4
 beq mrx_empty
 UNLOCK
 ldr r0,[sp,#PARA_WANTED]
 ldr r1,[r4,#MSG_ID]
 ldr r2,[r4,#MSG_SENDER]
mrx22: 
 ldmia r0!,{r3,r5}
 cmp r3,#0
 IF CPUTYPE > ARMv5TE
 beq.n mrx_found_msg2 
 cmp r5,#0
 beq.n mrx_found_msg2 
 ELSE
 beq mrx_found_msg2 
 cmp r5,#0
 beq mrx_found_msg2 
 ENDIF
 cmp r1,r5
 bne mrx22
 cmp r2,r3
 bne mrx22
 b rx_notboth
mrx_err0_cont:
 b mrx_err0
mrx_empty:
 ldr r0,[sp,#PARA_TMO+4] 
 subs r6,r6,#PCB_MSGQUEUE 
 cmp r0,#SC_NO_TMO 
 beq mrx_ret0 
 adds r2,r0,#-SC_ENDLESS_TMO 
 beq mrx50 
 IF CPUTYPE <= Xscale
 enterTimer(r6,r0,r1,r2,r7,mrx) 
 ELSE
 enterTimer_T2(r6,r0,r1,r2,r7,mrx)
 ENDIF
 movs r1,#0 
 subs r1,r1,#-SC_ENDLESS_TMO
 str r1,[sp,#PARA_TMO+4]
 movs r2,#ST_WAIT_TMO 
mrx50:
 ldrh r1,[r6,#PCB_STATE_H]
 movs r0,#ST_WAIT_MSGRX
 orrs r0,r0,r1 
 orrs r0,r0,r2 
 movs r2,#ST_TMO 
 tst r1,r2
 bne mrx_ret0a 
 strh r0,[r6,#PCB_STATE_H] 
 makeNotReady(r6,r0,r1,r7,mrx) 
 mov r4,lr 
 UNLOCK
 SWAP_OUT_SAVE {r4-r5} 
 movs r3,#ST_WAIT_MSGRX
 movs r2,#ST_TMO 
 LOCK
 ldrh r1,[r6,#PCB_STATE_H] 
 cmp r0,#0 
 beq mrx_err3 
 tst r1,r3 
 beq mrx_cont 
 bics r1,r1,r3 
 tst r1,r2 
 beq mrx_err0_cont 
mrx_ret0a: 
 bics r1,r1,r2 
 strh r1,[r6,#PCB_STATE_H] 
 movs r0,#0 
mrx_ret0:
 UNLOCK
 add sp,sp,#3*4
 RET r4-r7
mrx_cont: 
 mov lr,r4 
 adds r6,r6,#PCB_MSGQUEUE 
 bx r4 
mrx_err3:
 movs r0,#ST_WAIT_TMO 
 tst r1,r0 
 beq mrx51
 removeTimer(r6,r1,r0,r2) 
 strh r1,[r6,#PCB_STATE_H]
mrx51:
 UNLOCK
 movs r1,#0 
 ldr r0,mrx_KERNEL_ELOCKED
 bl sc_sysError
 LALIGN
mrx_err0_const:
 LONG (KERNEL_EILL_VALUE|SC_MSGRX|SC_ERR_MODULE_FATAL)
 IF SC_MSG_PARA_CHECK == 1
mrx_err1_const:
 LONG (KERNEL_EILL_PARAMETER|SC_MSGRX)
mrx_err2_const:
 LONG (KERNEL_EPROC_NOT_PRIO|SC_MSGRX|SC_ERR_MODULE_FATAL)
 ENDIF
mrx_sciopta:
 LONG sciopta
 IF SC_MSG_HOOK == 1
 IF SC_MSGRX_HOOK == 1
mrx_SC_MSGRXHOOK:
 LONG SC_MSGRXHOOK
 ENDIF
 ENDIF
mrx_KERNEL_ELOCKED:
 LONG (KERNEL_ELOCKED|SC_MSGRX|SC_ERR_MODULE_FATAL)
 SC_ENDFUNC sc_msgRx
#undef PARA_TMO
#undef PARA_WANTED
#undef PARA_FLAGS
 SC_TFUNC sc_msgCheck
 ldr r4,mchk_sciopta
 ldr r3,[r0,#MSG_OWNER] 
 ldr r2,[r4,#SC_CURPID] 
 cmp r2,r3 
 bne mchk_err0 
 SC_TPUBLIC sc_msgCheck2
 ldr r4,mchk_sciopta 
 push {r1} 
 ldr r1,[r0,#MSG_PLID] 
 movs r2,#0x7f
 lsrs r1,r1,#24 
 ands r1,r1,r2 
 ldr r2,mchk_sc_modules
 cmp r1,#SC_MAX_MODULE 
 bhs mchk_err1 
 lsls r1,r1,#2
 ldr r2,[r2,r1] 
 IF SC_NEVER_COPY == 1
 ELSE
 ldr r1,[r4,#SC_CURMODULE]
 cmp r1,r2 
 IF SC_USE_FRIENDS == 1
 beq mchk2 
 ldr r1,[r1,#MODULE_ID] 
 lsrs r1,r1,#24 
 movs r3,#31
 ands r3,r3,r1 
 adds r3,r3,#1 
 lsrs r1,r1,#5 
 lsls r1,r1,#2 
 adds r1,r1,r2
 ldr r1,[r1,#MODULE_FRIENDSET]
 lsrs r1,r1,r3
 bcc mchk_err2 
 ELSE
 bne mchk_err2 
 ENDIF 
 ENDIF 
mchk2:
 IF LITTLE_ENDIAN == 1
 ldrb r1,[r0,#MSG_PLID] 
 ELSE
 ldrb r1,[r0,#MSG_PLID+3]
 ENDIF
 ldr r3,[r2,#MODULE_MAX_POOLS]
 cmp r1,r3 
 bge mchk_err2 
 ldr r3,[r2,#MODULE_POOL] 
 lsls r1,r1,#2
 ldr r3,[r3,r1] 
 cmp r3,#1 
 bls mchk_err2 
 IF SC_ASYNC_TMO == 1
 ldr r1,[r0,#MSG_PLID]
 ldr r2,mchk_MSG_TMO_MAGIC
 lsls r1,r1,#8
 lsrs r1,r1,#16
 beq mchk3 
 cmp r1,r2 
 bne mchk_err3 
 ldr r1,[r0,#MSG_SIZE] 
 lsrs r1,r1,#28 
 lsls r1,r1,#2 
 b mchk4
mchk3:
 ENDIF 
 ldr r1,[r0,#MSG_BUFSIZE_IDX] 
 lsrs r1,r1,#1 
mchk4:
 ldr r2,[r3,#POOL_NBUFSIZES]
 lsls r2,r2,#2
 cmp r1,r2 
 bhs mchk_err3 
 adds r1,r1,#POOL_BUFSIZE
 ldr r1,[r3,r1] 
 ldr r2,[r3,#POOL_START]
 cmp r0,r2 
 blo mchk_err4 
 ldr r2,mchk_msgmagic
 subs r0,r0,#4
 ldr r4,[r0,#0] 
 adds r0,r0,#4 
 cmp r2,r4 
 bne mchk_err5 
 adds r1,r1,#MSG_HEAD 
 adds r1,r1,r0 
 ldr r4,[r3,#POOL_END]
 cmp r1,r4 
 bhs mchk_err4 
 ldr r4,[r1,#0] 
 cmp r2,r4
 bne mchk_err6 
 ldr r1,[r0,#MSG_SIZE] 
 IF SC_ASYNC_TMO == 1
 lsls r1,r1,#4
 lsrs r1,r1,#4 
 ENDIF
 ldr r2,[r3,#POOL_NBUFSIZES]
 lsls r2,r2,#2
 adds r2,r2,#POOL_BUFSIZE-4
 ldr r2,[r3,r2] 
 cmp r1,r2
 bhi mchk_err3 
 adds r1,r1,r0
 adds r1,r1,#MSG_HEAD 
 ldrb r2,[r1,#0] 
 ldrb r3,[r1,#1]
 ldrb r4,[r1,#2]
 ldrb r1,[r1,#3]
 cmp r2,#0xee
 bne mchk_err6 
 cmp r3,#0xee
 bne mchk_err6 
 cmp r4,#0xee
 bne mchk_err6 
 cmp r1,#0xee
 bne mchk_err6 
 pop {r1}
 bx lr
mchk_err6:
 ldr r2,mchk_lit8
 b mchk_err_cont
mchk_err5:
 pop {r1} 
 movs r5,r0 
 ldr r0,[r3,#POOL_START]
 movs r6,r0 
 movs r7,r3
 adds r7,r7,#POOL_BUFSIZE 
 ldmia r0!,{r4} 
 cmp r2,r4 
 bne mchk_err5a 
mchk5: 
 bl sc_msgCheck2 
 ldr r2,[r0,#MSG_BUFSIZE_IDX]
 movs r6,r0
 lsrs r2,r2,#1 
 ldr r2,[r7,r2] 
 adds r0,r0,r2
 adds r0,r0,#MSG_HEAD+4 
 cmp r0,r5 
 blo mchk5 
mchk_err5a:
 movs r1,r6
 ldr r2,mchk_lit7 
 b mchk_err_cont2
mchk_err4:
 ldr r2,mchk_lit5
 b mchk_err_cont 
mchk_err3:
 ldr r2,mchk_lit4
 b mchk_err_cont
mchk_err2:
 ldr r2,mchk_lit3
 b mchk_err_cont
mchk_err1:
 ldr r2,mchk_lit2
 b mchk_err_cont
mchk_err0:
 ldr r2,mchk_lit1
 push {r1}
mchk_err_cont:
 movs r1,r0
mchk_err_cont2:
 pop {r0}
 orrs r0,r0,r2
 bl sc_sysError
 LALIGN
mchk_sciopta:
 LONG sciopta
mchk_sc_modules:
 LONG sc_modules
mchk_lit1:
 LONG KERNEL_ENOT_OWNER|SC_ERR_MODULE_FATAL
mchk_lit2:
 LONG KERNEL_EILL_MODULE|SC_ERR_MODULE_FATAL
mchk_lit3:
 LONG KERNEL_EILL_POOL_ID|SC_ERR_MODULE_FATAL
mchk_lit4:
 LONG KERNEL_EMSG_HD_CORRUPT|SC_ERR_MODULE_FATAL
mchk_lit5:
 LONG KERNEL_EOUTSIDE_POOL|SC_ERR_MODULE_FATAL
mchk_msgmagic:
 LONG 0xeeeeeeee
mchk_lit7:
 LONG KERNEL_EMSG_PREV_ENDMARK_CORRUPT|SC_ERR_MODULE_FATAL
mchk_lit8:
 LONG KERNEL_EMSG_ENDMARK_CORRUPT|SC_ERR_MODULE_FATAL
mchk_MSG_TMO_MAGIC:
 LONG MSG_TMO_MAGIC>>8
 SC_ENDFUNC sc_msgCheck
#ifdef THUMB_MODE
 XSC_TFUNC sc_msgAddrGet,-12
 SC_TFUNC sc_msgAddrGet
 IF CPUTYPE <= Xscale
 IF SC_MSG_PARA_CHECK == 1
 adds r1,r0,#0
 beq mag1
 ldr r2,[r1,#0]
 cmp r2,#0
 beq mag1
 ELSE
 ldr r2,[r0,#0]
 ENDIF
 subs r2,r2,#MSG_HEAD
 ldr r0,[r2,#MSG_ADDRESSEE]
 bx lr
 IF SC_MSG_PARA_CHECK == 1
mag1:
 ldr r0,mag_lit0
 bl sc_sysError
 LALIGN
mag_lit0:
 LONG SC_MSGADDRGET|KERNEL_ENIL_PTR|SC_ERR_MODULE_FATAL
 ENDIF
 ELSE
 IF SC_MSG_PARA_CHECK == 1
 cbz r0,mag1
 ldr r2,[r0,#0]
 cbz r2,mag1
 ELSE
 ldr r2,[r0,#0]
 ENDIF
 ldr r0,[r2,#MSG_ADDRESSEE-MSG_HEAD]
 bx lr
 IF SC_MSG_PARA_CHECK == 1
mag1:
 ldr r0,mag_lit0
 bl sc_sysError
 LALIGN
mag_lit0:
 LONG SC_MSGADDRGET|KERNEL_ENIL_PTR|SC_ERR_MODULE_FATAL
 ENDIF
 ENDIF
 SC_ENDFUNC sc_msgAddrGet
#else
 SC_FUNC sc_msgAddrGet
 IF SC_MSG_PARA_CHECK == 1
 adds r1,r0,#0
 ldrne r2,[r1,#0]
 cmpne r2,#0
 ldrne r0,[r2,#MSG_ADDRESSEE-MSG_HEAD]
 bxne lr
 ldr r0,mag_lit0
 b sc_sysError
mag_lit0:
 LONG SC_MSGADDRGET|KERNEL_ENIL_PTR|SC_ERR_MODULE_FATAL
 ELSE
 ldr r2,[r0,#0]
 ldr r0,[r2,#MSG_ADDRESSEE-MSG_HEAD]
 bx lr
 ENDIF
 SC_ENDFUNC sc_msgAddrGet
#endif 
#ifdef THUMB_MODE
 XSC_TFUNC sc_msgSizeGet,-12
 SC_TFUNC sc_msgSizeGet
 IF CPUTYPE <= Xscale
 IF SC_MSG_PARA_CHECK == 1
 adds r1,r0,#0 
 beq msig1 
 ldr r2,[r1,#0] 
 cmp r2,#0 
 beq msig1 
 ELSE
 ldr r2,[r0,#0] 
 ENDIF
 subs r2,r2,#MSG_HEAD 
 ldr r0,[r2,#MSG_SIZE] 
 bx lr 
 IF SC_MSG_PARA_CHECK == 1
msig1:
 ldr r0,msig_lit0 
 bl sc_sysError
 LALIGN
msig_lit0:
 LONG SC_MSGSIZEGET|KERNEL_ENIL_PTR|SC_ERR_MODULE_FATAL
 ENDIF
 ELSE
 IF SC_MSG_PARA_CHECK == 1
 cbz r0,msig1 
 ldr r2,[r0,#0] 
 cbz r2,msig1 
 ELSE
 ldr r2,[r0,#0] 
 ENDIF
 ldr r0,[r2,#MSG_SIZE-MSG_HEAD] 
 bx lr 
 IF SC_MSG_PARA_CHECK == 1
msig1:
 ldr r0,msig_lit0 
 bl sc_sysError
 LALIGN
msig_lit0:
 LONG SC_MSGSIZEGET|KERNEL_ENIL_PTR|SC_ERR_MODULE_FATAL
 ENDIF
 ENDIF
 SC_ENDFUNC sc_msgSizeGet
#else
 SC_FUNC sc_msgSizeGet
 IF SC_MSG_PARA_CHECK == 1
 adds r1,r0,#0 
 ldrne r2,[r1,#0] 
 cmpne r2,#0 
 ldrne r0,[r2,#MSG_SIZE-MSG_HEAD] 
 bxne lr 
 ldr r0,msig_lit0 
 b sc_sysError
msig_lit0:
 LONG SC_MSGSIZEGET|KERNEL_ENIL_PTR|SC_ERR_MODULE_FATAL
 ELSE
 ldr r2,[r0,#0] 
 ldr r0,[r2,#MSG_SIZE-MSG_HEAD] 
 bx lr
 ENDIF
 SC_ENDFUNC sc_msgSizeGet
#endif 
#ifdef THUMB_MODE
 XSC_TFUNC sc_msgSizeSet,-12
 SC_TFUNC sc_msgSizeSet
 IF CPUTYPE <= Xscale
 IF SC_MSG_PARA_CHECK == 1
 cmp r0,#0 
 beq mss_err0
 ldr r2,[r0,#0]
 cmp r2,#0
 beq mss_err0
 ELSE
 ldr r2,[r0,#0] 
 ENDIF
 movs r3,r2
 subs r3,r3,#MSG_HEAD
 ldr r0,[r3,#MSG_SIZE] 
 IF SC_MSG_PARA_CHECK == 1
 cmp r1,#4 
 blo mss_err1 
 cmp r1,r0 
 bgt mss_err2
 ENDIF
 str r1,[r3,#MSG_SIZE] 
 IF SC_MSG_CHECK == 1
 movs r0,#0xee 
 adds r3,r2,r1
 strb r0,[r3,#0]
 strb r0,[r3,#1]
 strb r0,[r3,#2]
 strb r0,[r3,#3]
 ENDIF
 movs r0,r1 
 bx lr
 IF SC_MSG_PARA_CHECK == 1
mss_err1:
 ldr r0,mss_lit0
 b mss_err_cont
mss_err2:
 ldr r0,mss_lit2
 b mss_err_cont
mss_err0:
 movs r1,r0
 ldr r0,mss_lit1
mss_err_cont: 
 bl sc_sysError
 LALIGN
mss_lit0:
 LONG SC_MSGSIZESET|KERNEL_EILL_VALUE|SC_ERR_MODULE_FATAL
mss_lit1:
 LONG SC_MSGSIZESET|KERNEL_ENIL_PTR|SC_ERR_MODULE_FATAL
mss_lit2:
 LONG SC_MSGSIZESET|KERNEL_EENLARGE_MSG|SC_ERR_MODULE_FATAL
 ENDIF 
 ELSE
 IF SC_MSG_PARA_CHECK == 1
 cbz r0,mss_err0 
 ldr r2,[r0,#0]
 cbz r2,mss_err0a
 ELSE
 ldr r2,[r0,#0] 
 ENDIF
 ldr r0,[r2,#MSG_SIZE-MSG_HEAD] 
 IF SC_MSG_PARA_CHECK == 1
 cmp r1,#4 
 blo mss_err1 
 cmp r1,r0 
 bgt mss_err2
 ENDIF
 str r1,[r2,#MSG_SIZE-MSG_HEAD]
 IF SC_MSG_CHECK == 1
#ifdef __IAR5__
 ldr r0,=0xeeeeeeee 
#else
 movw r0,#0xeeee 
 movt r0,#0xeeee
#endif
 str r0,[r2,r1]
 ENDIF
 movs r0,r1 
 bx lr
 IF SC_MSG_PARA_CHECK == 1
mss_err1:
 movs r1,r0
 ldr r0,mss_lit0
 b mss_err_cont
mss_err2:
 ldr r0,mss_lit2
 b mss_err_cont
mss_err0a:
 movs r0,r2
mss_err0:
 movs r1,r0
 ldr r0,mss_lit1
mss_err_cont: 
 bl sc_sysError
 LALIGN
mss_lit0:
 LONG SC_MSGSIZESET|KERNEL_EILL_VALUE|SC_ERR_MODULE_FATAL
mss_lit1:
 LONG SC_MSGSIZESET|KERNEL_ENIL_PTR|SC_ERR_MODULE_FATAL
mss_lit2:
 LONG SC_MSGSIZESET|KERNEL_EENLARGE_MSG|SC_ERR_MODULE_FATAL
 ENDIF 
 ENDIF
 SC_ENDFUNC sc_msgSizeSet
#else 
 SC_FUNC sc_msgSizeSet
 IF SC_MSG_PARA_CHECK == 1
 cmp r0,#0 
 ldrne r2,[r0,#0] 
 cmpne r2,#0 
 moveq r1,r0 
 ldreq r0,mss_lit3 
 beq sc_sysError 
 ldr r3,[r2,#MSG_SIZE-MSG_HEAD] 
 cmp r1,#4 
 blt mss_err1 
 cmp r1,r3 
 bgt mss_err2 
 ELSE
 ldr r2,[r0,#0]
 ENDIF
 str r1,[r2,#MSG_SIZE-MSG_HEAD] 
 IF SC_MSG_CHECK == 1
 movs r0,#0xee 
 strb r0,[r2,+r1]!
 strb r0,[r2],#1
 strb r0,[r2],#1
 strb r0,[r2],#1
 ENDIF
 movs r0,r1 
 bx lr
 IF SC_MSG_PARA_CHECK == 1
mss_err1:
 ldr r0,mss_lit1
 bl sc_sysError
mss_err2:
 ldr r0,mss_lit2
 bl sc_sysError
mss_lit1:
 LONG SC_MSGSIZESET|KERNEL_EILL_VALUE|SC_ERR_MODULE_FATAL
mss_lit2:
 LONG SC_MSGSIZESET|KERNEL_EENLARGE_MSG|SC_ERR_MODULE_FATAL
mss_lit3:
 LONG SC_MSGSIZESET|KERNEL_ENIL_PTR|SC_ERR_MODULE_FATAL
 ENDIF
 SC_ENDFUNC sc_msgSizeSet
#endif 
#ifdef THUMB_MODE
 XSC_TFUNC sc_msgSndGet,-12
 SC_TFUNC sc_msgSndGet
 IF CPUTYPE <= Xscale
 IF SC_MSG_PARA_CHECK == 1
 adds r1,r0,#0
 beq msg1
 ldr r2,[r1,#0]
 cmp r2,#0
 beq msg1
 ELSE
 ldr r2,[r0,#0]
 ENDIF
 subs r2,r2,#MSG_HEAD
 ldr r0,[r2,#MSG_SENDER]
 bx lr
 IF SC_MSG_PARA_CHECK == 1
msg1:
 ldr r0,msg_lit0
 bl sc_sysError
 LALIGN
msg_lit0:
 LONG SC_MSGSNDGET|KERNEL_ENIL_PTR|SC_ERR_MODULE_FATAL
 ENDIF
 ELSE 
 IF SC_MSG_PARA_CHECK == 1
 cbz r0,msg1
 ldr r2,[r0,#0]
 cbz r2,msg1
 ELSE
 ldr r2,[r0,#0]
 ENDIF
 ldr r0,[r2,#MSG_SENDER-MSG_HEAD]
 bx lr
 IF SC_MSG_PARA_CHECK == 1
msg1:
 mov r1,r0
 ldr r0,msg_lit0
 bl sc_sysError
 LALIGN
msg_lit0:
 LONG SC_MSGSNDGET|KERNEL_ENIL_PTR|SC_ERR_MODULE_FATAL
 ENDIF
 ENDIF
 SC_ENDFUNC sc_msgSndGet
#else
 SC_FUNC sc_msgSndGet
 IF SC_MSG_PARA_CHECK == 1
 adds r1,r0,#0
 ldrne r2,[r1,#0]
 cmpne r2,#0
 ldrne r0,[r2,#MSG_SENDER-MSG_HEAD]
 bxne lr
 ldr r0,msg_lit0
 b sc_sysError
msg_lit0:
 LONG SC_MSGSNDGET|KERNEL_ENIL_PTR|SC_ERR_MODULE_FATAL
 ELSE
 ldr r2,[r0,#0]
 ldr r0,[r2,#MSG_SENDER-MSG_HEAD]
 bx lr
 ENDIF
 SC_ENDFUNC sc_msgSndGet
#endif 
#ifdef THUMB_MODE
 XSC_TFUNC sc_msgOwnerGet,-12
 SC_TFUNC sc_msgOwnerGet
 IF CPUTYPE <= Xscale
 IF SC_MSG_PARA_CHECK == 1
 adds r1,r0,#0
 beq mog1
 ldr r2,[r1,#0]
 cmp r2,#0
 beq mog1
 ELSE
 ldr r2,[r0,#0]
 ENDIF
 subs r2,r2,#MSG_HEAD
 ldr r0,[r2,#MSG_OWNER]
 bx lr
 IF SC_MSG_PARA_CHECK == 1
mog1:
 ldr r0,mog_lit0
 bl sc_sysError
 LALIGN
mog_lit0:
 LONG SC_MSGOWNERGET|KERNEL_ENIL_PTR|SC_ERR_MODULE_FATAL
 ENDIF
 ELSE
 IF SC_MSG_PARA_CHECK == 1
 cbz r0,mog1
 ldr r2,[r0,#0]
 cbz r2,mog1
 ELSE
 ldr r2,[r0,#0]
 ENDIF
 ldr r0,[r2,#MSG_OWNER-MSG_HEAD]
 bx lr
 IF SC_MSG_PARA_CHECK == 1
mog1:
 mov r1,r0
 ldr r0,mog_lit0
 bl sc_sysError
 LALIGN
mog_lit0:
 LONG SC_MSGOWNERGET|KERNEL_ENIL_PTR|SC_ERR_MODULE_FATAL
 ENDIF
 ENDIF
 SC_ENDFUNC sc_msgOwnerGet
#else
 SC_FUNC sc_msgOwnerGet
 IF SC_MSG_PARA_CHECK == 1
 adds r1,r0,#0
 ldrne r2,[r1,#0]
 cmpne r2,#0
 ldrne r0,[r2,#MSG_OWNER-MSG_HEAD]
 bxne lr
 ldr r0,mog_lit0
 b sc_sysError
mog_lit0:
 LONG SC_MSGOWNERGET|KERNEL_ENIL_PTR|SC_ERR_MODULE_FATAL
 ELSE
 ldr r2,[r0,#0]
 ldr r0,[r2,#MSG_OWNER-MSG_HEAD]
 bx lr
 ENDIF
 SC_ENDFUNC sc_msgOwnerGet
#endif 
#ifdef THUMB_MODE
 XSC_TFUNC sc_msgPoolIdGet,-12
 SC_TFUNC sc_msgPoolIdGet
 IF SC_MSG_PARA_CHECK == 1
 adds r1,r0,#0 
 beq mpig1 
 ldr r2,[r1,#0] 
 cmp r2,#0 
 beq mpig1 
 ELSE
 ldr r2,[r0,#0] 
 ENDIF
 subs r2,r2,#MSG_HEAD 
 ldr r0,[r2,#MSG_PLID] 
 IF (SC_NEVER_COPY == 1) || (SC_USE_FRIENDS == 1)
 ldr r1,mpig_sciopta 
 ldr r1,[r1,#SC_CURMODULE] 
 ldr r1,[r1,#MODULE_ID] 
 lsls r2,r0,#1 
 lsrs r2,r2,#25 
 lsrs r1,r1,#24
 cmp r2,r1
 beq mpig2
 movs r0,#SC_DEFAULT_POOL-1
 adds r0,r0,#1
 bx lr
mpig2:
 ENDIF
 lsls r0,r0,#25 
 lsrs r0,r0,#25
 bx lr
 IF SC_MSG_PARA_CHECK == 1
mpig1:
 ldr r0,mpig_lit0
 bl sc_sysError
 LALIGN
mpig_lit0:
 LONG SC_MSGPOOLIDGET|KERNEL_ENIL_PTR|SC_ERR_MODULE_FATAL
 ENDIF
 IF (SC_NEVER_COPY == 1) || (SC_USE_FRIENDS == 1)
 LALIGN
mpig_sciopta:
 LONG sciopta
 ENDIF
 SC_ENDFUNC sc_msgPoolIdGet
#else 
 SC_FUNC sc_msgPoolIdGet
 IF SC_MSG_PARA_CHECK == 1
 adds r1,r0,#0
 ldrne r2,[r1,#0]
 cmpne r2,#0
 IF SC_USE_FRIENDS == 1
 ENDIF 
 IF LITTLE_ENDIAN == 1
 ldrneb r0,[r2,#MSG_PLID-MSG_HEAD]
 ELSE
 ldrneb r0,[r2,#MSG_PLID-MSG_HEAD+3]
 ENDIF
 bxne lr
 ldr r0,mpig_lit0
 b sc_sysError
mpig_lit0:
 LONG SC_MSGPOOLIDGET|KERNEL_ENIL_PTR|SC_ERR_MODULE_FATAL
 ELSE
 ldr r2,[r0,#0]
 IF LITTLE_ENDIAN == 1
 ldrb r0,[r2,#MSG_PLID-MSG_HEAD]
 ELSE
 ldrb r0,[r2,#MSG_PLID-MSG_HEAD+3]
 ENDIF
 bx lr
 ENDIF
 SC_ENDFUNC sc_msgPoolIdGet
#endif 
#if SC_PATH_MAX != 511
#error
#endif
 XSC_TFUNC sc_procWakeupEnable,-16
 SC_TFUNC sc_procWakeupEnable
 ldr r3,pwe_sciopta 
 ldr r2,[r3,#SC_CURPCB] 
 ldrh r1,[r2,#PCB_FLAGS_H] 
 IF SC_PROC_PARA_CHECK == 1
 movs r0,#PCB_FLG_IS_TIMER|PCB_FLG_IS_IRQ
 tst r1,r0 
 beq pwe_error 
 ENDIF
 movs r0,#PCB_FLG_WAKEUP_DIS
 bics r1,r1,r0 
 strh r1,[r2,#PCB_FLAGS_H]
 bx lr 
 IF SC_PROC_PARA_CHECK == 1
pwe_error:
 ldr r0,pwe_lit
 movs r1,#0
 bl sc_sysError
 LALIGN
pwe_lit:
 LONG SC_PROCWAKEUPENABLE|SC_ERR_PROCESS_FATAL|KERNEL_EILL_PROCTYPE
 ENDIF
 LALIGN
pwe_sciopta:
 LONG sciopta
 SC_ENDFUNC sc_procWakeupEnable
 XSC_TFUNC sc_procWakeupDisable,-16
 SC_TFUNC sc_procWakeupDisable
 ldr r3,pwd_sciopta 
 ldr r2,[r3,#SC_CURPCB] 
 ldrh r1,[r2,#PCB_FLAGS_H] 
 IF SC_PROC_PARA_CHECK == 1
 movs r0,#PCB_FLG_IS_TIMER|PCB_FLG_IS_IRQ
 tst r1,r0 
 beq pwd_error 
 ENDIF
 movs r0,#PCB_FLG_WAKEUP_DIS
 orrs r1,r1,r0 
 strh r1,[r2,#PCB_FLAGS_H]
 bx lr
 IF SC_PROC_PARA_CHECK == 1
pwd_error:
 ldr r0,pwd_lit
 movs r1,#0
 bl sc_sysError
 LALIGN
pwd_lit:
 LONG SC_PROCWAKEUPDISABLE|SC_ERR_PROCESS_FATAL|KERNEL_EILL_PROCTYPE
 ENDIF
 LALIGN
pwd_sciopta:
 LONG sciopta
 SC_ENDFUNC sc_procWakeupDisable
 XSC_TFUNC sc_procVarDel,-12
 SC_TFUNC sc_procVarDel
 ldr r3,pvd_sciopta
 mov r12,r0 
 ldr r0,pvd_KERNEL_ENIL_PTR 
 ldr r3,[r3,#SC_CURPCB] 
 ldr r3,[r3,#PCB_LOCALS] 
 cmp r3,#0 
 beq pvd3 
 ldr r2,[r3,#4] 
 movs r0,#0
pvd1:
 ldr r1,[r3,#8] 
 adds r3,r3,#8 
 cmp r1,r12 
 beq pvd2 
 subs r2,r2,#1 
 bne pvd1 
 bx lr 
pvd2:
 str r0,[r3,#0] 
 str r0,[r3,#4] 
 movs r0,#1 
 bx lr
 LALIGN
pvd_sciopta:
 LONG sciopta
pvd_KERNEL_ENIL_PTR:
 LONG SC_PROCVARDEL|KERNEL_ENIL_PTR|SC_ERR_PROCESS_FATAL
pvd3:
 movs r1,#0
 bl sc_sysError
 SC_ENDFUNC sc_procVarDel
 XSC_TFUNC sc_procVarSet,-8
 SC_TFUNC sc_procVarSet
 push {r4-r5} 
 ldr r3,pvs_sciopta
 ldr r3,[r3,#SC_CURPCB] 
 ldr r3,[r3,#PCB_LOCALS] 
 cmp r3,#0 
 beq pvs5 
 adds r3,r3,#4 
 ldmia r3!,{r2} 
 mov r12,r0 
 movs r5,#0
pvs1:
 ldr r0,[r3,#0] 
 cmp r0,r12 
 beq pvs3 
 orrs r0,r0,r5 
 bne pvs2 
 movs r5,r3 
pvs2:
 adds r3,r3,#8 
 subs r2,r2,#1 
 bne pvs1 
 movs r0,#0 
 movs r3,r5 
 beq pvs4 
pvs3:
 mov r0,r12 
 stmia r3!,{r0-r1} 
pvs3a:
 movs r0,#1 
pvs4:
 pop {r4-r5}
 bx lr
pvs5:
 movs r1,#0
 ldr r0,pvs_lit0
 bl sc_sysError
 LALIGN
pvs_sciopta:
 LONG sciopta
pvs_lit0:
 LONG SC_PROCVARSET|KERNEL_ENIL_PTR|SC_ERR_PROCESS_FATAL
 SC_ENDFUNC sc_procVarSet
 XSC_TFUNC sc_procVarGet,-8
 SC_TFUNC sc_procVarGet
 cmp r1,#0 
 beq pvg5 
 push {r4} 
 ldr r3,pvg_sciopta
 ldr r3,[r3,#SC_CURPCB]
 ldr r3,[r3,#PCB_LOCALS] 
 cmp r3,#0 
 beq pvg5 
 adds r3,r3,#4 
 ldmia r3!,{r2} 
 mov r12,r0 
pvg1:
 ldmia r3!,{r0,r4} 
 cmp r0,r12 
 beq pvg4 
 subs r2,r2,#1 
 bne pvg1
pvg2:
 movs r0,#0 
pvg3:
 pop {r4}
 bx lr
pvg5:
 movs r1,#0 
 ldr r0,pvg_lit0 
 bl sc_sysError
 LALIGN
pvg_sciopta:
 LONG sciopta
pvg_lit0:
 LONG SC_PROCVARGET|KERNEL_ENIL_PTR|SC_ERR_PROCESS_FATAL
pvg4:
 str r4,[r1,#0] 
 movs r0,#1 
 b pvg3
 SC_ENDFUNC sc_procVarGet
 XSC_TFUNC sc_procVarInit,-8
 SC_TFUNC sc_procVarInit
 ldr r3,pvi_sciopta
 ldr r3,[r3,#SC_CURPCB]
 ldr r2,[r3,#PCB_LOCALS] 
 cmp r2,#0 
 bne pvi_err 
 cmp r0,#0 
 beq pvi_err1 
 ldr r2,[r0,#0] 
 cmp r2,#0 
 beq pvi_err1 
 mov r12,r2 
 movs r2,#0 
 str r2,[r0,#0] 
 mov r2,r12
 subs r2,r2,#MSG_HEAD 
 cmp r1,#2 
 blt pvi_err2 
 ldr r0,[r2,#MSG_SIZE] 
 lsrs r0,r0,#3 
 cmp r1,r0
 bgt pvi_err2 
 mov r2,r12 
 str r2,[r3,#PCB_LOCALS] 
 ldr r0,pvi_lit3
 subs r1,r1,#1
 stmia r2!,{r0-r1} 
 movs r0,#0 
 movs r3,#0 
pvi1:
 subs r1,r1,#1
 stmia r2!,{r0,r3}
 bne pvi1
 bx lr
pvi_err1:
 movs r1,#0
 ldr r0,pvi_lit2
 b pvi_err_cont
pvi_err:
 movs r1,r2
 ldr r0,pvi_alreadyDefined
 b pvi_err_cont
pvi_err2:
 ldr r0,pvi_lit1
pvi_err_cont:
 bl sc_sysError
 LALIGN
pvi_sciopta:
 LONG sciopta
pvi_lit1:
 LONG (SC_PROCVARINIT|KERNEL_EILL_VALUE|SC_ERR_PROCESS_FATAL)
pvi_lit2:
 LONG (SC_PROCVARINIT|KERNEL_ENIL_PTR|SC_ERR_PROCESS_FATAL)
pvi_lit3:
 LONG SC_VARPOOL_ID
pvi_alreadyDefined:
 LONG (SC_PROCVARINIT|KERNEL_EALREADY_DEFINED|SC_ERR_PROCESS_FATAL)
 SC_ENDFUNC sc_procVarInit
 XSC_TFUNC sc_procVarRm,-16
 SC_TFUNC sc_procVarRm
 ldr r3,pvr_sciopta
 ldr r3,[r3,#SC_CURPCB]
 ldr r0,[r3,#PCB_LOCALS] 
 movs r1,#0
 str r1,[r3,#PCB_LOCALS] 
 cmp r0,#0 
 beq pvr_warn 
 bx lr 
pvr_warn:
 ldr r0,pvr_lit0 
 bl sc_sysError
 LALIGN
pvr_sciopta:
 LONG sciopta
pvr_lit0:
 LONG SC_PROCVARRM|KERNEL_ENIL_PTR|SC_ERR_PROCESS_FATAL
 SC_ENDFUNC sc_procVarRm
 XSC_TFUNC sc_procIdGet,-8
 SC_TFUNC sc_procIdGet
 push {r4-r7,lr}
 ldr r7,pig_sciopta
 movs r5,r1 
 beq pig0
 lsrs r1,r5,#24 
 beq pig0 
 adds r1,r5,#-SC_FATAL_IF_TMO 
 beq pig0
 adds r1,r5,#-SC_ENDLESS_TMO
 bne pig_err_illtmo
pig0: 
 movs r4,r0 
 ldr r0,[r7,#SC_CURPID] 
 beq pig9 
 ldrb r2,[r4,#0] 
 cmp r2,#0
 beq pig9 
 IF SC_PROC_PARA_CHECK == 1
 movs r2,#(SC_PATH_MAX+1)>>2 
 lsls r2,r2,#2 
 ENDIF
 movs r6,#0
 movs r0,r4 
pig1:
 ldrb r1,[r0,#0]
 adds r0,r0,#1
 IF SC_PROC_PARA_CHECK == 1
 subs r2,r2,#1
 beq pig_err
 ENDIF
 cmp r1,#0
 bne pig1
 subs r6,r0,r4 
 ldr r2,pig_SC_PROCD
 ldr r1,pig_lit1 
 ldr r0,[r2,#0] 
 movs r2,#0 
 movs r3,#0 
 cmp r0,#0 
 beq pig_ill 
 push {r0-r3} 
 movs r0,r6
 adds r0,r0,#SC_PROCIDGETMSG_SZ-1 
 subs r1,r1,#1 
 movs r2,#SC_DEFAULT_POOL-1
 adds r2,r2,#1
 movs r3,#-SC_FATAL_IF_TMO
 NEG(r3)
 bl sc_msgAlloc 
 push {r0} 
 str r5,[r0,#SC_PROCIDGETMSG_TMO] 
 adds r0,r0,#SC_PROCIDGETMSG_PATH-1 
 subs r1,r4,#1 
pig2: 
 ldrb r2,[r1,#1]
 adds r1,r1,#1
 adds r0,r0,#1
 strb r2,[r0,#0]
 cmp r2,#0
 bne pig2
 mov r0,sp 
 ldr r1,[sp,#4] 
 movs r2,#0 
 bl sc_msgTx
 ldr r3,[r7,#SC_CURPCB] 
 ldrh r3,[r3,#PCB_FLAGS_H]
 lsrs r3,r3,#PCB_FLG_IS_PRIO_POS+1
 bcc pig_ill0 
 add r1,sp,#4 
 movs r2,#SC_MSGRX_BOTH 
 subs r0,r2,#SC_MSGRX_BOTH+(-SC_ENDLESS_TMO)
 bl sc_msgRx 
 str r0,[sp,#0] 
 ldr r5,[r0,#SC_PROCIDGETMSG_REPLY_PID]
 ldr r6,[r0,#SC_PROCIDGETMSG_REPLY_ERROR]
 mov r0,sp
 bl sc_msgFree 
 movs r0,r6 
 bne pig_err_cont 
 movs r0,r5 
 add sp,sp,#5*4 
pig9:
 RET r4-r7
pig_ill0:
 add sp,sp,#5*4 
pig_ill:
 ldr r0,pig_lit2
 RET r4-r7
pig_err_illtmo:
 movs r1,r5
 ldr r0,pig_illtmo
 bl sc_sysError
 IF SC_PROC_PARA_CHECK == 1
pig_err:
 ldr r0,pig_lit3
 ENDIF
pig_err_cont:
 movs r1,#SC_ERR_MODULE_FATAL
 orrs r0,r0,r1 
 movs r1,r4
 bl sc_sysError
 LALIGN
pig_sciopta:
 LONG sciopta
pig_SC_PROCD:
 LONG sciopta+SC_PROCD
pig_lit1:
 LONG SC_PROCIDGETMSG_REPLY
pig_lit2:
 LONG SC_ILLEGAL_PID
 IF SC_PROC_PARA_CHECK == 1
pig_lit3:
 LONG SC_PROCIDGET|KERNEL_EILL_PROC_NAME|SC_ERR_MODULE_FATAL
 ENDIF
pig_illtmo:
 LONG SC_PROCIDGET|KERNEL_EILL_SLICE|SC_ERR_PROCESS_FATAL
 SC_ENDFUNC sc_procIdGet
 SC_TFUNC sc_sysProcIdGet
 push {r4-r7,lr} 
 ldr r5,spig_sc_modules 
 movs r4,r0 
 IF SC_PROC_PARA_CHECK == 1
 beq spig_err0 
 lsrs r3,r1,#24 
 cmp r3,#SC_MAX_MODULE 
 bge spig_err1 
 mov r12,r1 
 movs r2,#SC_PROC_NAME_SIZE+1 
spig1:
 ldrb r1,[r0,#0]
 subs r2,r2,#1
 beq spig_err0 
 adds r0,r0,#1
 cmp r1,#0
 bne spig1
 mov r1,r12 
 ELSE
 lsrs r3,r1,#24 
 ENDIF
 lsls r3,r3,#2
 ldr r5,[r5,r3] 
 cmp r5,#1 
 bls spig_ill 
 movs r0,r4 
 bl sc_miscCrcString
 mov r12,r0 
 ldr r1,[r5,#MODULE_MAX_PROCESS] 
 ldr r5,[r5,#MODULE_PCBS] 
spig2: 
 ldmia r5!,{r0} 
 cmp r0,#0 
 beq spig4 
 ldrh r2,[r0,#PCB_HASH_H] 
 cmp r2,r12 
 bne spig4 
 ldr r2,[r0,#PCB_NAME] 
 movs r3,r4 
spig3: 
 ldrb r6,[r2,#0]
 adds r2,r2,#1
 ldrb r7,[r3,#0]
 adds r3,r3,#1
 cmp r6,r7 
 bne spig4 
 cmp r6,#0 
 bne spig3 
 ldr r0,[r0,#PCB_PID] 
 RET r4-r7
spig4:
 subs r1,r1,#1 
 bne spig2
spig_ill:
 ldr r0,spig_SC_ILLEGAL_PID
 RET r4-r7
 IF SC_PROC_PARA_CHECK == 1
spig_err0:
 movs r1,#0
 ldr r0,spig_lit0
spig_err_cont:
 bl sc_sysError
spig_err1:
 ldr r0,spig_lit1
 b spig_err_cont
 ENDIF 
 LALIGN
spig_sciopta:
 LONG sciopta
spig_sc_modules:
 LONG sc_modules
 IF SC_PROC_PARA_CHECK == 1
spig_lit0:
 LONG (SC_PROCIDGET|KERNEL_EILL_PROC_NAME|SC_ERR_MODULE_FATAL)
spig_lit1:
 LONG (SC_PROCIDGET|KERNEL_EILL_PID|SC_ERR_MODULE_FATAL)
 ENDIF
spig_SC_ILLEGAL_PID: 
 LONG SC_ILLEGAL_PID 
 SC_ENDFUNC sc_sysProcIdGet
 XSC_TFUNC sc_procKill,-8
 SC_TFUNC sc_procKill
 push {r4,lr}
 ldr r2,pk_SC_KERNELD
 ldr r2,[r2,#0] 
 cmp r2,#0 
 beq pk_err 
 mov r12,r2 
 adds r2,r0,#-SC_CURRENT_PID 
 ldr r4,pk_sciopta
 bne pk1 
 ldr r0,[r4,#SC_CURPID] 
 b pk2 
pk1:
 lsrs r2,r0,#24 
 cmp r2,#SC_MAX_MODULE 
 ldr r3,pk_sc_modules 
 ldr r4,[r4,#SC_CURMODULE] 
 bge pk_err1 
 lsls r2,r2,#2
 ldr r3,[r3,r2] 
 cmp r3,r4 
 beq pk2 
 lsrs r2,r1,#SC_PROCKILL_KILL_POS+1 
 bcc pk_err1 
pk2: 
 push {r0-r1} 
 mov r0,r12 
 ldr r1,pk_SC_PROCKILLMSG_REPLY
 movs r2,#0
 movs r3,#0
 push {r0-r3} 
 movs r0,#SC_PROCKILLMSG_SZ
 subs r1,r1,#1 
 movs r2,#SC_DEFAULT_POOL-1
 adds r2,r2,#1
 movs r3,#-SC_ENDLESS_TMO
 NEG(r3)
 bl sc_msgAlloc 
 push {r0} 
 adds r1,r0,#4 
 ldr r2,[sp,#20] 
 ldr r3,[sp,#24] 
 stmia r1!,{r2-r3} 
 mov r0,sp
 ldr r1,[sp,#4]
 movs r2,#0
 bl sc_msgTx 
 add r1,sp,#4 
 movs r2,#SC_MSGRX_BOTH
 subs r0,r2,#SC_MSGRX_BOTH+(-SC_ENDLESS_TMO) 
 bl sc_msgRx 
 str r0,[sp,#0]
 mov r0,sp
 bl sc_msgFree 
 add sp,sp,#4*4+4+2*4 
 RET r4
pk_err:
 movs r1,#0
 ldr r0,pk_lit1
pk_err_cont:
 bl sc_sysError
pk_err1:
 movs r1,r0 
 ldr r0,pk_lit2
 b pk_err_cont
 LALIGN
pk_sc_modules:
 LONG sc_modules
pk_sciopta:
 LONG sciopta
pk_SC_KERNELD:
 LONG sciopta+SC_KERNELD
pk_SC_PROCKILLMSG_REPLY:
 LONG SC_PROCKILLMSG_REPLY
pk_lit1:
 LONG SC_PROCKILL|KERNEL_ENO_KERNELD|SC_ERR_SYSTEM_FATAL
pk_lit2:
 LONG SC_PROCKILL|KERNEL_EILL_PID|SC_ERR_MODULE_FATAL
 SC_ENDFUNC sc_procKill
#define PARA_NAME 0
#define PARA_ENTRY 4
#define PARA_STACKSIZE 8
#define PARA_PERIOD 12
#define PARA_INITDELAY 24
#define PARA_STATE 28
#define PARA_PLID 32
 XSC_TFUNC sc_procTimCreate,12
 XSC_TFUNC sc_procPrioCreate,12
 XSC_TFUNC sc_procIntCreate,12
 XSC_TFUNC sc_procUsrIntCreate,12
 SC_TFUNC sc_procTimCreate
 push {r0-r4,lr} 
 adr r2,ptc_lit0 
 bl sc_procXCreate 
 LALIGN
ptc_lit0:
 LONG SC_PROCTIMCREATEMSG_REPLY
 LONG sciopta+SC_KERNELD
 LONG SC_PROCTIMCREATE
 SC_ENDFUNC sc_procTimCreate
 SC_TFUNC sc_procPrioCreate
 push {r0-r4,lr}
 adr r2,ppric_lit0
 bl sc_procXCreate
 LALIGN
ppric_lit0:
 LONG SC_PROCPRIOCREATEMSG_REPLY
 LONG sciopta+SC_KERNELD
 LONG SC_PROCPRIOCREATE
 SC_ENDFUNC sc_procPrioCreate
 SC_TFUNC sc_procIntCreate
 push {r0-r4,lr}
 adr r2,pic_lit0
 bl sc_procXCreate
 LALIGN
pic_lit0:
 LONG SC_PROCINTCREATEMSG_REPLY
 LONG sciopta+SC_KERNELD
 LONG SC_PROCINTCREATE
 SC_ENDFUNC sc_procIntCreate
 SC_TFUNC sc_procUsrIntCreate
 push {r0-r4,lr}
 adr r2,puic_lit0
 bl sc_procXCreate
 LALIGN
puic_lit0:
 LONG SC_PROCUSRINTCREATEMSG_REPLY
 LONG sciopta+SC_KERNELD
 LONG SC_PROCUSRINTCREATE
 SC_ENDFUNC sc_procUsrIntCreate
 SC_TFUNC sc_procXCreate
 IF SC_PROC_PARA_CHECK == 1
 movs r3,#SC_PROC_NAME_SIZE+1 
 movs r4,#0xdf
pxc1:
 ldrb r1,[r0,#0]
 adds r0,r0,#1
 subs r3,r3,#1
 beq pxc_err0 
 cmp r1,#0
 beq pxc2
 cmp r1,#'_'
 beq pxc1
 cmp r1,#'0'
 blo pxc_err0
 cmp r1,#'9'
 bls pxc1 
 ands r1,r1,r4 
 cmp r1,#'A'
 blo pxc_err0
 cmp r1,#'Z'
 bls pxc1
pxc_err0:
 ldr r0,pxc_lit3
 ldr r1,[sp,#0]
 b pxc_err_cont
pxc2:
 ENDIF
 ldmia r2!,{r1,r3,r4} 
 ldr r0,[r3,#0] 
 cmp r0,#0 
 beq pxc_err1 
 movs r2,#0 
 movs r3,#0
 push {r0-r3} 
 movs r0,#SC_PROCCREATEMSG_SZ
 subs r1,r1,#1 
 ldr r2,[sp,#PARA_PLID+16]
 subs r3,r2,#1
 ble pxc2a 
 cmp r3,#SC_DEFAULT_POOL-1 
 bne pxc2a 
 push {r0,r1} 
 movs r0,#0 
 subs r0,r0,#1
 bl sc_poolDefault 
 movs r2,r0
 pop {r0,r1}
 str r2,[sp,#PARA_PLID+16] 
pxc2a:
 movs r3,#-SC_ENDLESS_TMO
 NEG(r3)
 bl sc_msgAlloc 
 push {r0} 
 adds r1,r0,#3 
 ldr r2,[sp,#20+PARA_NAME]
pxc3:
 ldrb r3,[r2,#0] 
 adds r2,r2,#1
 adds r1,r1,#1
 strb r3,[r1,#0]
 cmp r3,#0
 bne pxc3
 adds r0,r0,#SC_PROCCREATEMSG_ENTRY 
 add r1,sp,#20+PARA_ENTRY
 ldmia r1!,{r2-r4} 
 stmia r0!,{r2-r4}
 adds r1,r1,#2*4
 ldmia r1!,{r2-r4} 
 stmia r0!,{r2-r4}
 mov r0,sp
 ldr r1,[sp,#4] 
 movs r2,#0
 bl sc_msgTx
 add r1,sp,#4
 movs r2,#SC_MSGRX_BOTH
 subs r0,r2,#SC_MSGRX_BOTH+(-SC_ENDLESS_TMO)
 bl sc_msgRx
 str r0,[sp,#0]
 ldr r0,[r0,#SC_PROCCREATEMSG_REPLY_PID]
 push {r0}
 add r0,sp,#4
 bl sc_msgFree
 pop {r0}
 add sp,sp,#4*4+4+4*4
 RET r4
pxc_err1:
 ldr r0,pxc_lit2
 movs r1,#0
pxc_err_cont:
 orrs r0,r0,r4
 bl sc_sysError
 LALIGN
pxc_lit2:
 LONG KERNEL_ENO_KERNELD|SC_ERR_SYSTEM_FATAL
pxc_lit3:
 LONG KERNEL_EILL_PROC_NAME|SC_ERR_MODULE_FATAL
 SC_ENDFUNC sc_procXCreate
#undef PARA_NAME
#undef PARA_ENTRY
#undef PARA_STACKSIZE
#undef PARA_PERIOD
#undef PARA_INITDELAY
#undef PARA_STATE
#undef PARA_PLID
 XSC_TFUNC sc_procUnobserve,-8
 SC_TFUNC sc_procUnobserve
 push {r4-r7,lr}
 ldr r7,puo_sciopta
 adds r2,r1,#-SC_CURRENT_PID
 ldr r3,[r7,#SC_CURPID]
 bne puo0
 movs r1,r3
puo0:
 IF SC_PROC_PARA_CHECK == 1
 adds r2,r0,#-SC_CURRENT_PID
 beq puo_err0
 cmp r0,r3
 beq puo_err0
 ENDIF
 cmp r0,#0
 IF SC_MAX_CONNECTOR > 0
 bmi puo10
 ELSE
 bmi puo_err0
 ENDIF
 movs r6,r1
 LOCK
 lsrs r2,r0,#24
 cmp r2,#SC_MAX_MODULE
 ldr r3,puo_sc_modules
 bge puo_err1
 lsls r2,r2,#2
 ldr r3,[r3,r2]
 cmp r3,#1
 bls puo_err1
 ldr r4,[r3,#MODULE_MAX_PROCESS]
 lsls r2,r0,#18
 lsrs r2,r2,#18
 cmp r2,r4
 ldr r3,[r3,#MODULE_PCBS]
 bge puo_err1
 lsls r2,r2,#2
 ldr r4,[r3,r2]
 cmp r4,#0
 beq puo9
 ldr r3,[r4,#PCB_PID]
 cmp r3,r0
 bne puo9
 ldrh r7,[r4,#PCB_FLAGS_H]
 movs r0,#1
 lsls r0,r0,#PCB_FLG_PCB_LOCKED_POS
 orrs r0,r0,r3
 strh r0,[r4,#PCB_FLAGS_H]
 adds r4,r4,#PCB_CONNECTED
 movs r0,r4
 b puo2
puo1:
 LOCK
puo2:
 movs r5,r0
 ldr r0,[r5,#dbl_next]
 cmp r0,r4
 beq puo8
 UNLOCK
 ldr r2,[r0,#MSG_SENDER]
 cmp r6,r2
 bne puo1
 ldr r2,[r0,#MSG_OWNER]
 str r2,[r0,#MSG_ADDRESSEE]
 bl sc_sysMsgFree
 movs r0,r5
 b puo1
puo8:
 ldrh r2,[r4,#PCB_FLAGS_H]
 lsrs r7,r7,#PCB_FLG_PCB_LOCKED_POS
 beq puo9
 lsls r7,r7,#PCB_FLG_PCB_LOCKED_POS
 orrs r2,r2,r7
 strh r2,[r4,#PCB_FLAGS_H]
puo9:
 UNLOCK
 RET r4-r7
puo10:
 IF SC_MAX_CONNECTOR > 0
 LOCK
 lsls r2,r0,#1
 lsrs r2,r0,#25
 cmp r2,#SC_MAX_CONNECTOR
 bge puo_err1
 ldr r4,puo_sc_connectors
 lsls r2,r2,#2
 ldr r4,[r4,r2]
 cmp r4,#1
 bls puo_err1
 ldr r2,[r4,#PCB_PID]
 UNLOCK
 push {r0-r2}
 movs r0,#SC_CONNPROCUNOBSERVEMSG_SZ
 ldr r1,puo_lit3
 movs r2,#SC_DEFAULT_POOL-1
 adds r2,r2,#1
 movs r3,#-SC_FATAL_IF_TMO
 NEG(r3)
 bl sc_msgAlloc
 pop {r1-r3}
 push {r0}
 str r2,[r0,#SC_CONNPROCUNOBSERVEMSG_PID]
 ldr r2,[r1,#0]
 str r2,[r0,#SC_CONNPROCUNOBSERVEMSG_OBSERVER]
 movs r2,#0
 str r2,[r1,#0]
 mov r0,sp
 movs r1,r3
 movs r2,#0
 bl sc_msgTx
 add sp,sp,#4
 RET r4-r7
 ENDIF
puo_err1:
 UNLOCK
puo_err0:
 movs r1,r0
 ldr r0,puo_lit0
puo_err_cont:
 bl sc_sysError
 LALIGN
puo_sciopta:
 LONG sciopta
puo_sc_modules:
 LONG sc_modules
 IF SC_MAX_CONNECTOR > 0
puo_sc_connectors: 
 LONG sc_connectors
puo_lit3: 
 LONG SC_CONNPROCUNOBSERVEMSG
 ENDIF
puo_lit0:
 LONG (KERNEL_EILL_PID|SC_PROCUNOBSERVE|SC_ERR_MODULE_FATAL)
 SC_ENDFUNC sc_procUnobserve
 XSC_TFUNC sc_procObserve,-8
 SC_TFUNC sc_procObserve
 push {r4,lr}
 ldr r3,po_sciopta
 IF SC_PROC_PARA_CHECK == 1 
 adds r2,r1,#-SC_CURRENT_PID
 ldr r2,[r3,#SC_CURPID]
 beq po_err0
 cmp r2,r1
 beq po_err0
 cmp r0,#0
 beq po_err1
 ldr r2,[r0,#0]
 cmp r2,#0
 beq po_err1
 ELSE
 ldr r2,[r0,#0]
 ENDIF
 cmp r1,#0
 bmi po1
 movs r2,#SC_MSGTX_OBSERVE
 movs r3,#0
 movs r4,#SC_PROCOBSERVE>>24
 lsls r4,r4,#24
 bl sc_sysMsgTx
 RET r4
po1:
 IF SC_MAX_CONNECTOR > 0
 LOCK
 lsls r2,r1,#1
 lsrs r2,r2,#25
 cmp r2,#SC_MAX_CONNECTOR
 bge po_err2
 ldr r4,po_sc_connectors
 lsls r2,r2,#2
 ldr r4,[r4,r2]
 cmp r4,#1
 bls po_err2
 ldr r2,[r4,#PCB_PID]
 UNLOCK
 push {r0-r2}
 movs r0,#SC_CONNPROCOBSERVEMSG_SZ
 ldr r1,po_lit3
 movs r2,#SC_DEFAULT_POOL-1
 adds r2,r2,#1
 movs r3,#-SC_FATAL_IF_TMO
 NEG(r3)
 bl sc_msgAlloc
 pop {r1-r3}
 push {r0}
 str r2,[r0,#SC_CONNPROCOBSERVEMSG_PID]
 ldr r2,[r1,#0]
 str r2,[r0,#SC_CONNPROCOBSERVEMSG_MSG]
 movs r2,#0
 str r2,[r1,#0]
 mov r0,sp
 movs r1,r3
 movs r2,#0
 bl sc_msgTx
 add sp,sp,#4
 RET r4
po_err2:
 UNLOCK
 ENDIF
po_err0:
 ldr r0,po_lit2
po_err_cont:
 bl sc_sysError
 IF SC_PROC_PARA_CHECK == 1
po_err1:
 movs r1,#0
 ldr r0,po_lit1
 b po_err_cont
 ENDIF
 LALIGN
po_sciopta:
 LONG sciopta
 IF SC_PROC_PARA_CHECK == 1
po_lit1:
 LONG (KERNEL_ENIL_PTR|SC_PROCOBSERVE|SC_ERR_MODULE_FATAL)
 ENDIF
po_lit2:
 LONG (KERNEL_EILL_PID|SC_PROCOBSERVE|SC_ERR_MODULE_FATAL)
 IF SC_MAX_CONNECTOR > 0
po_lit3:
 LONG SC_CONNPROCOBSERVEMSG
po_sc_connectors:
 LONG sc_connectors
 ENDIF
 SC_ENDFUNC sc_procObserve
 XSC_TFUNC sc_procPathCheck,-12
 SC_TFUNC sc_procPathCheck
 adds r1,r0,#0
 IF SC_PROC_PARA_CHECK == 1
 beq ppc_err0
 ENDIF
 movs r2,#(SC_PATH_MAX+1)>>2
 lsls r2,r2,#2
 mov r12,r1
 movs r1,#0xdf
ppc1:
 subs r2,r2,#1
 beq ppc_ret0
 ldrb r3,[r0,#0]
 adds r0,r0,#1
 cmp r3,#0
 beq ppc2
 cmp r3,#'_'
 beq ppc1
 cmp r3,#'/'
 beq ppc1
 cmp r3,#'0'
 blt ppc_ret0
 cmp r3,#'9'
 bls ppc1
 ands r3,r3,r1
 cmp r3,#'A'
 blo ppc_ret0
 cmp r3,#'Z'
 bls ppc1
ppc_ret0:
 movs r0,#0
 bx lr
ppc2:
 mov r0,r12
 ldrb r1,[r0,#0]
ppc3:
 movs r3,#SC_PROC_NAME_SIZE+1
 cmp r1,#'/'
 bne ppc6
 ldrb r2,[r0,#1]
 adds r0,r0,#1
 cmp r1,r2
 bne ppc5
ppc4:
 subs r3,r3,#1
 bmi ppc_ret0
 ldrb r1,[r0,#1]
 adds r0,r0,#1
 cmp r1,#'/'
 bne ppc4
 cmp r3,#SC_PROC_NAME_SIZE
 beq ppc_ret0
 b ppc3
ppc5:
 subs r3,r3,#1
 bmi ppc_ret0
 ldrb r1,[r0,#0]
 adds r0,r0,#1
 cmp r1,#0
 beq ppc_ret1
 cmp r1,#'/'
 bne ppc5
 cmp r3,#SC_PROC_NAME_SIZE
 beq ppc_ret0
 movs r3,#SC_PROC_NAME_SIZE+1
ppc6:
 subs r3,r3,#1
 bmi ppc_ret0
 ldrb r1,[r0,#0]
 adds r0,r0,#1
 cmp r1,#'/'
 beq ppc_ret0
 cmp r1,#0
 bne ppc6
ppc_ret1: 
 cmp r3,#SC_PROC_NAME_SIZE
 beq ppc_ret0
 movs r0,#1
 bx lr
 IF SC_PROC_PARA_CHECK == 1
ppc_err0:
 ldr r0,ppc_lit0
 bl sc_sysError
 ENDIF
 LALIGN
 IF SC_PROC_PARA_CHECK == 1
ppc_lit0:
 LONG SC_PROCPATHCHECK|KERNEL_ENIL_PTR|SC_ERR_SYSTEM_FATAL
 ENDIF
 SC_ENDFUNC sc_procPathCheck
 XSC_TFUNC sc_procDaemonUnregister,-16
 SC_TFUNC sc_procDaemonUnregister
 ldr r3,pdu_sciopta
 ldr r2,pdu_SC_PROCD
 ldr r3,[r3,#SC_CURPID]
 ldr r1,[r2,#0]
 cmp r1,r3
 bne pdu1
 movs r0,#0
 str r0,[r2,#0]
pdu1:
 bx lr
 LALIGN
pdu_sciopta:
 LONG sciopta
pdu_SC_PROCD:
 LONG sciopta+SC_PROCD
 SC_ENDFUNC sc_procDaemonUnregister
 XSC_TFUNC sc_procDaemonRegister,-16
 SC_TFUNC sc_procDaemonRegister
 push {lr}
 movs r0,#0
 ldr r3,pdr_sciopta
 ldr r2,[r3,#SC_CURPID]
 lsrs r1,r2,#24
 ldr r1,[r3,#SC_CURPCB]
 bne pdr_ret0
 ldrh r1,[r1,#PCB_FLAGS_H]
 lsrs r1,r1,#PCB_FLG_IS_PRIO_POS+1
 bcc pdr_ret0
 ldr r3,pdr_SC_PROCD
 LOCK
 ldr r1,[r3,#0]
 cmp r1,#0
 bne pdr1
 str r2,[r3,#0]
 movs r0,#1
pdr1:
 UNLOCK
pdr_ret0: 
 EXIT
 LALIGN
pdr_sciopta:
 LONG sciopta
pdr_SC_PROCD:
 LONG sciopta+SC_PROCD
 SC_ENDFUNC sc_procDaemonRegister
 IF SC_PROC_HOOK == 1
 XSC_TFUNC sc_procHookRegister,-8
 SC_TFUNC sc_procHookRegister
 ldr r3,phr_sciopta
 ldr r2,[r3,#SC_CURMODULE]
 cmp r2,#0
 beq phr1
 IF SC_PROCKILL_HOOK == 1
 movs r3,#MODULE_PROCKILLHOOK
 cmp r0,#SC_SET_PROCKILL_HOOK
 beq phr2
 ENDIF
 IF SC_PROCCREATE_HOOK == 1
 movs r3,#MODULE_PROCCREATEHOOK
 cmp r0,#SC_SET_PROCCREATE_HOOK
 beq phr2
 ENDIF
#if 0 
 IF SC_PROCSWAP_HOOK == 1
 movs r3,#MODULE_PROCSWAPHOOK
 cmp r0,#SC_SET_PROCSWAP_HOOK
 beq phr2
 ENDIF
#endif
 IF SC_PROC_PARA_CHECK == 1
 movs r1,r0
 ldr r0,phr_illval1
 b phr_err_cont 
 ENDIF
phr1: 
 IF SC_PROCKILL_HOOK == 1
 ldr r2,phr_SC_PROCKILLHOOK
 cmp r0,#SC_SET_PROCKILL_HOOK
 beq phr2
 ENDIF
 IF SC_PROCCREATE_HOOK == 1
 ldr r2,phr_SC_PROCCREATEHOOK
 cmp r0,#SC_SET_PROCCREATE_HOOK
 beq phr2
 ENDIF
 IF SC_PROCSWAP_HOOK == 1
 ldr r2,phr_SC_PROCSWAPHOOK
 cmp r0,#SC_SET_PROCSWAP_HOOK
 beq phr2
 ENDIF
 IF SC_PROCMMU_HOOK == 1
 ldr r2,phr_SC_PROCMMUHOOK
 cmp r0,#SC_SET_PROCMMU_HOOK
 beq phr2
 ENDIF
 IF SC_PROC_PARA_CHECK == 1
 movs r1,r0
 ldr r0,phr_illval2
phr_err_cont: 
 bl sc_sysError
 ELSE
 movs r0,#0
 bx lr
 ENDIF
phr2:
 ldr r0,[r2,r3]
 str r1,[r2,r3]
 bx lr
 LALIGN
phr_sciopta:
 LONG sciopta
 IF SC_PROCKILL_HOOK == 1
phr_SC_PROCKILLHOOK:
 LONG SC_PROCKILLHOOK
 ENDIF
 IF SC_PROCCREATE_HOOK == 1
phr_SC_PROCCREATEHOOK:
 LONG SC_PROCCREATEHOOK
 ENDIF
 IF SC_PROCSWAP_HOOK == 1
phr_SC_PROCSWAPHOOK:
 LONG SC_PROCSWAPHOOK
 ENDIF
 IF SC_PROCMMU_HOOK == 1
phr_SC_PROCMMUHOOK:
 LONG SC_PROCMMUHOOK
 ENDIF
 IF SC_PROC_PARA_CHECK == 1
phr_illval1:
 LONG SC_PROCHOOKREGISTER|KERNEL_EILL_VALUE|SC_ERR_MODULE_FATAL
phr_illval2:
 LONG SC_PROCHOOKREGISTER|KERNEL_EILL_VALUE|SC_ERR_SYSTEM_FATAL
 ENDIF
 SC_ENDFUNC sc_procHookRegister
 ENDIF
 XSC_TFUNC sc_procStop,-12
 SC_TFUNC sc_procStop
 push {r7,lr} 
 LOCK 
 ldr r7,pst_sciopta 
 adds r1,r0,#-SC_CURRENT_PID 
 ldr r2,[r7,#SC_CURPCB] 
 beq pst1 
 cmp r0,#0 
 ble pst_err 
 lsrs r1,r0,#24 
 cmp r1,#SC_MAX_MODULE 
 ldr r2,pst_sc_modules 
 bge pst_err 
 lsls r1,r1,#2 
 ldr r2,[r2,r1] 
 cmp r2,#1 
 bls pst_warn 
 ldr r3,[r2,#MODULE_MAX_PROCESS]
 lsls r1,r0,#18
 lsrs r1,r1,#18 
 cmp r1,r3 
 ldr r3,[r2,#MODULE_PCBS] 
 bge pst_err 
 lsls r1,r1,#2 
 ldr r2,[r3,r1] 
 cmp r2,#0 
 beq pst_warn 
 ldr r3,[r2,#PCB_PID] 
 cmp r3,r0 
 bne pst_warn 
pst1: 
 ldrh r1,[r2,#PCB_FLAGS_H] 
 lsrs r0,r1,#PCB_FLG_IS_PRIO_POS+1
 bcs pst2 
 lsrs r0,r1,#PCB_FLG_IS_TIMER_POS+1
 bcc pst_err1 
pst2: 
 ldrh r3,[r2,#PCB_STOPPED_H] 
 adds r3,r3,#1 
 lsrs r0,r3,#16 
 bne pst_err2 
 strh r3,[r2,#PCB_STOPPED_H] 
 ldrh r3,[r2,#PCB_STATE_H] 
 movs r0,#ST_STOPPED>>2
 lsls r0,r0,#2
 orrs r3,r3,r0 
 strh r3,[r2,#PCB_STATE_H]
 lsrs r0,r1,#PCB_FLG_IS_PRIO_POS+1
 bcs pst3 
 movs r0,#PCB_FLG_READY 
 removeTimer(r2,r1,r0,r3) 
 strh r1,[r2,#PCB_FLAGS_H]
 UNLOCK
 RET r7
pst3:
 makeNotReady(r2,r0,r1,r7,pst3) 
 UNLOCK
 ldr r0,[r7,#SC_CURPRIOPCB] 
 cmp r0,r2 
 bne pst_exit 
 push {r4-r6} 
 SWAP_OUT 
 RET r4-r7
pst_err:
 ldr r2,pst_lit0
 b pst_err_cont
pst_err1:
 ldr r2,pst_lit2
 b pst_err_cont
pst_err2:
 ldr r2,pst_lit3
 b pst_err_cont
pst_warn:
 ldr r2,pst_lit1
pst_err_cont:
 movs r1,r0
 movs r0,r2
 UNLOCK
 bl sc_sysError
pst_exit: 
 RET r7
 LALIGN
pst_sciopta:
 LONG sciopta
pst_sc_modules:
 LONG sc_modules
pst_lit0:
 LONG KERNEL_EILL_PID|SC_PROCSTOP|SC_ERR_SYSTEM_FATAL
pst_lit1:
 LONG KERNEL_EILL_PID|SC_PROCSTOP
pst_lit2:
 LONG KERNEL_EILL_PROC|SC_PROCSTOP|SC_ERR_MODULE_FATAL
pst_lit3:
 LONG KERNEL_EILL_VALUE|SC_PROCSTOP|SC_ERR_MODULE_FATAL
 SC_ENDFUNC sc_procStop
 XSC_TFUNC sc_procStart,-12
 SC_TFUNC sc_procStart
 push {r4-r7,lr} 
 ldr r7,ps_sciopta 
 movs r1,r0 
 ble ps_err0 
 lsrs r0,r1,#24 
 cmp r0,#SC_MAX_MODULE 
 ldr r6,ps_sc_modules 
 bge ps_err0 
 lsls r0,r0,#2 
 LOCK
 ldr r6,[r6,r0] 
 cmp r6,#1 
 bls ps_warn0 
 ldr r5,[r6,#MODULE_PCBS] 
 ldr r4,[r6,#MODULE_MAX_PROCESS] 
 lsls r0,r1,#18
 lsrs r0,r0,#18 
 cmp r0,r4 
 bge ps_err1 
 lsls r0,r0,#2 
 ldr r5,[r5,r0] 
 cmp r5,#0 
 beq ps_warn0 
 ldr r0,[r5,#PCB_PID] 
 cmp r0,r1
 bne ps_warn0 
 ldrh r2,[r5,#PCB_FLAGS_H] 
 lsrs r0,r2,#PCB_FLG_IS_TIMER_POS+1
 bcs ps1 
 lsrs r0,r2,#PCB_FLG_IS_PRIO_POS+1
 bcc ps_err2 
ps1:
 ldrh r0,[r5,#PCB_STOPPED_H] 
 ldrh r1,[r5,#PCB_STATE_H] 
 cmp r0,#0 
 beq ps_warn1 
 subs r0,r0,#1 
 strh r0,[r5,#PCB_STOPPED_H] 
 bne ps_exit 
 movs r0,#ST_STOPPED>>2
 lsls r0,r0,#2
 bics r1,r1,r0 
 strh r1,[r5,#PCB_STATE_H]
 lsrs r0,r2,#PCB_FLG_IS_PRIO_POS+1
 bcc ps5 
 lsrs r0,r1,#ST_READY_POS 
 bne ps_exit 
 makeReady(r5,r0,r1,r2,r7,ps1) 
 UNLOCK
 ldr r0,[r7,#SC_CURPRIOPCB]
 ldrb r1,[r5,#PCB_PRIO_B]
 ldrb r0,[r0,#PCB_PRIO_B]
 cmp r1,r0
 bhs ps3
 SWAP_OUT
ps3:
 RET r4-r7
ps_err1:
 UNLOCK
ps_err0:
 ldr r0,ps_lit0
 b ps_err_cont
ps_warn1:
 ldr r0,ps_lit3
 b ps_err_cont1 
ps_warn0:
 ldr r0,ps_lit1
ps_err_cont1:
 UNLOCK
ps_err_cont:
 bl sc_sysError
 RET r4-r7
ps_err2:
 ldr r0,ps_lit2
 b ps_err_cont1
ps5:
 ldr r1,[r5,#PCB_SLICE]
 movs r0,#PCB_FLG_READY
 orrs r2,r2,r0
 strh r2,[r5,#PCB_FLAGS_H]
 ldr r0,[r5,#PCB_SLICE]
 enterTimer(r5,r0,r1,r2,r7,ps5)
 ldrh r1,[r5,#PCB_STATE_H]
 movs r0,#ST_WAIT_TMO
 orrs r1,r1,r0
 strh r1,[r5,#PCB_STATE_H]
ps_exit:
 UNLOCK
 RET r4-r7
 LALIGN
ps_sciopta:
 LONG sciopta
ps_sc_modules:
 LONG sc_modules
ps_lit0:
 LONG KERNEL_EILL_PID|SC_PROCSTART|SC_ERR_MODULE_FATAL
ps_lit1:
 LONG KERNEL_EILL_PID|SC_PROCSTART
ps_lit2:
 LONG KERNEL_EILL_PROC|SC_PROCSTART|SC_ERR_MODULE_FATAL
ps_lit3:
 LONG KERNEL_ESTART_NOT_STOPPED|SC_PROCSTART
 SC_ENDFUNC sc_procStart
 XSC_TFUNC sc_procSliceSet,-8
 SC_TFUNC sc_procSliceSet
 ldr r3,pss_sciopta 
 adds r2,r0,#-SC_CURRENT_PID 
 ldr r2,[r3,#SC_CURPCB] 
 beq pss1 
 cmp r0,#0 
 bmi pss_err 
 mov r12,r1 
 lsrs r1,r0,#24 
 cmp r1,#SC_MAX_MODULE
 ldr r2,pss_sc_modules
 bhs pss_err 
 lsls r1,r1,#2
 ldr r2,[r2,r1] 
 cmp r2,#1
 bls pss_warnpid 
 ldr r3,[r2,#MODULE_MAX_PROCESS]
 lsls r1,r0,#18
 lsrs r1,r1,#18 
 cmp r1,r3 
 ldr r3,[r2,#MODULE_PCBS] 
 bge pss_err 
 lsls r1,r1,#2
 ldr r2,[r3,r1] 
 cmp r2,#0 
 beq pss_warnpid 
 ldr r3,[r2,#PCB_PID]
 cmp r3,r0 
 bne pss_warnpid 
 mov r1,r12 
pss1:
 lsrs r0,r1,#24
 bne pss_err_illslice
 ldrh r0,[r2,#PCB_FLAGS_H]
 lsrs r3,r0,#PCB_FLG_IS_TIMER_POS+1
 bcs pss2
 lsrs r0,r0,#PCB_FLG_IS_PRIO_POS+1
 bcc pss_err2
 ldr r0,[r2,#PCB_CURSLICE]
 cmp r0,#0
 bne pss3
 mov r3,lr
 LOCK
 str r1,[r2,#PCB_SLICE]
 str r1,[r2,#PCB_CURSLICE]
 UNLOCK
 bx r3
pss2:
 cmp r1,#SC_NO_TMO 
 beq pss_err_illslice
pss3: 
 str r1,[r2,#PCB_SLICE]
 bx lr
pss_err:
 movs r1,r0
 ldr r0,pss_lit0
 b pss_err_cont0
pss_err_illslice: 
 ldr r0,pss_errillslice
 b pss_err_cont0
pss_err2:
 ldr r0,pss_lit4
 ldr r1,[r2,#PCB_PID]
pss_err_cont0: 
 bl sc_sysError
pss_warnpid:
 movs r1,r0
 ldr r0,pss_lit1
pss_err_cont:
 push {lr}
 bl sc_sysError
 EXIT
 LALIGN
pss_sciopta:
 LONG sciopta
pss_sc_modules:
 LONG sc_modules
pss_lit0:
 LONG KERNEL_EILL_PID|SC_PROCSLICESET|SC_ERR_PROCESS_FATAL
pss_lit1:
 LONG KERNEL_EILL_PID|SC_PROCSLICESET|SC_ERR_PROCESS_WARNING
pss_errillslice:
 LONG KERNEL_EILL_SLICE|SC_PROCSLICESET|SC_ERR_PROCESS_FATAL
pss_lit4:
 LONG KERNEL_EILL_PROCTYPE|SC_PROCSLICESET|SC_ERR_MODULE_FATAL
 SC_ENDFUNC sc_procSliceSet
 XSC_TFUNC sc_procSliceGet,-12
 SC_TFUNC sc_procSliceGet
 ldr r3,pslg_sciopta
 adds r1,r0,#-SC_CURRENT_PID 
 ldr r2,[r3,#SC_CURPCB] 
 beq pslg_exit 
 cmp r0,#0 
 bmi pslg9 
 lsrs r1,r0,#24 
 cmp r1,#SC_MAX_MODULE 
 ldr r2,pslg_sc_modules
 bge pslg_err 
 lsls r1,r1,#2
 ldr r2,[r2,r1] 
 cmp r2,#1 
 bls pslg_warn 
 ldr r3,[r2,#MODULE_MAX_PROCESS]
 lsls r1,r0,#18
 lsrs r1,r1,#18
 cmp r1,r3 
 ldr r3,[r2,#MODULE_PCBS]
 bge pslg_err 
 lsls r1,r1,#2
 ldr r2,[r3,r1] 
 cmp r2,#0
 beq pslg_warn 
 ldr r3,[r2,#PCB_PID]
 cmp r3,r0 
 bne pslg_warn 
pslg_exit: 
 ldr r0,[r2,#PCB_SLICE]
 bx lr
pslg9:
 IF SC_MAX_CONNECTOR > 0
 movs r0,#-SC_ENDLESS_TMO
 NEG(r0)
 bx lr
 ENDIF
pslg_err:
 movs r1,r0
 ldr r0,pslg_lit0
 bl pslg_err_cont
pslg_warn:
 movs r1,r0
 ldr r0,pslg_lit1
pslg_err_cont:
 push {lr}
 bl sc_sysError
 movs r0,#-SC_ENDLESS_TMO
 NEG(r0)
 EXIT
 LALIGN
pslg_sciopta:
 LONG sciopta
pslg_sc_modules:
 LONG sc_modules
pslg_lit0:
 LONG KERNEL_EILL_PID|SC_PROCSLICEGET|SC_ERR_SYSTEM_FATAL
pslg_lit1:
 LONG KERNEL_EILL_PID|SC_PROCSLICEGET
 SC_ENDFUNC sc_procSliceGet
 XSC_TFUNC sc_procPpidGet,-12
 SC_TFUNC sc_procPpidGet
 ldr r3,ppidg_sciopta
 adds r1,r0,#-SC_CURRENT_PID
 ldr r2,[r3,#SC_CURPCB]
 beq ppidg_exit
 cmp r0,#0
 bmi ppidg9
 lsrs r1,r0,#24
 cmp r1,#SC_MAX_MODULE
 ldr r2,ppidg_sc_modules
 bge ppidg_err 
 lsls r1,r1,#2
 ldr r2,[r2,r1] 
 cmp r2,#1
 bls ppidg_warn
 ldr r3,[r2,#MODULE_MAX_PROCESS]
 lsls r1,r0,#18
 lsrs r1,r1,#18
 cmp r1,r3
 ldr r3,[r2,#MODULE_PCBS]
 bge ppidg_err
 lsls r1,r1,#2
 ldr r2,[r3,r1] 
 cmp r2,#0
 beq ppidg_warn
 ldr r3,[r2,#PCB_PID]
 cmp r3,r0
 bne ppidg_warn
ppidg_exit:
 ldrh r0,[r2,#PCB_FLAGS_H]
 lsrs r0,r0,#PCB_FLG_IS_STATIC_POS+1
 movs r0,#0
 bcs ppidg8
 subs r2,r2,#MSG_HEAD
 ldr r0,[r2,#MSG_SENDER]
ppidg8:
 bx lr
ppidg9:
 IF SC_MAX_CONNECTOR > 0
 ldr r0,ppidg_SC_ILLEGAL_PID
 bx lr
 ENDIF
ppidg_err:
 movs r1,r0
 ldr r0,ppidg_lit0
 bl ppidg_err_cont
ppidg_warn:
 movs r1,r0
 ldr r0,ppidg_lit1
ppidg_err_cont:
 push {lr}
 bl sc_sysError
 ldr r0,ppidg_SC_ILLEGAL_PID
 EXIT
 LALIGN
ppidg_sciopta:
 LONG sciopta
ppidg_sc_modules:
 LONG sc_modules
ppidg_lit0:
 LONG KERNEL_EILL_PID|SC_PROCPPIDGET|SC_ERR_SYSTEM_FATAL
ppidg_lit1:
 LONG KERNEL_EILL_PID|SC_PROCPPIDGET
ppidg_SC_ILLEGAL_PID:
 LONG SC_ILLEGAL_PID
 SC_ENDFUNC sc_procPpidGet
 XSC_TFUNC sc_procPrioGet,-12
 SC_TFUNC sc_procPrioGet
 ldr r3,ppg_sciopta
 adds r1,r0,#-SC_CURRENT_PID
 ldr r2,[r3,#SC_CURPCB]
 beq ppg_exit
 cmp r0,#0
 bmi ppg9
 lsrs r1,r0,#24
 cmp r1,#SC_MAX_MODULE
 ldr r2,ppg_sc_modules
 bge ppg_err 
 lsls r1,r1,#2
 ldr r2,[r2,r1]
 cmp r2,#1
 bls ppg_warn
 ldr r3,[r2,#MODULE_MAX_PROCESS]
 lsls r1,r0,#18
 lsrs r1,r1,#18
 cmp r1,r3
 ldr r3,[r2,#MODULE_PCBS]
 bge ppg_err
 lsls r1,r1,#2
 ldr r2,[r3,r1]
 cmp r2,#0
 beq ppg_warn
 ldr r3,[r2,#PCB_PID]
 cmp r3,r0
 bne ppg_warn
ppg_exit: 
 ldrb r0,[r2,#PCB_PRIO0_B]
 bx lr
ppg9:
 IF SC_MAX_CONNECTOR > 0
 movs r0,#32
 bx lr
 ENDIF
ppg_err:
 movs r1,r0
 ldr r0,ppg_lit0
 bl ppg_err_cont
ppg_warn:
 movs r1,r0
 ldr r0,ppg_lit1
ppg_err_cont:
 push {lr}
 bl sc_sysError
 movs r0,#32
 EXIT
 LALIGN
ppg_sciopta:
 LONG sciopta
ppg_sc_modules:
 LONG sc_modules
ppg_lit0:
 LONG KERNEL_EILL_PID|SC_PROCPRIOGET|SC_ERR_SYSTEM_FATAL
ppg_lit1:
 LONG KERNEL_EILL_PID|SC_PROCPRIOGET
 SC_ENDFUNC sc_procPrioGet
 XSC_TFUNC sc_procNameGet,-12
 SC_TFUNC sc_procNameGet
 push {r4-r7,lr}
 ldr r7,nag_sc_modules 
 ldr r3,nag_sciopta 
 ldr r6,[r3,#SC_CURMODULE] 
 ldr r5,[r3,#SC_CURPCB] 
 adds r1,r0,#-SC_CURRENT_PID 
 beq nag0 
 movs r4,r0 
 IF SC_MAX_CONNECTOR > 0
 bmi nag9 
 ELSE
 bmi nag_err 
 ENDIF
 lsrs r1,r0,#24 
 cmp r1,#SC_MAX_MODULE 
 bge nag_err 
 lsls r1,r1,#2
 ldr r6,[r7,r1] 
 cmp r6,#1 
 bls nag_warn 
 ldr r2,[r6,#MODULE_MAX_PROCESS] 
 ldr r5,[r6,#MODULE_PCBS] 
 lsls r1,r0,#18
 lsrs r1,r1,#18 
 cmp r1,r2 
 bge nag_err 
 lsls r1,r1,#2
 ldr r5,[r5,r1] 
 cmp r5,#0 
 beq nag_warn 
 ldr r1,[r5,#PCB_PID] 
 cmp r1,r0
 bne nag_warn 
nag0:
 movs r0,#SC_PROCNAMEGETMSG_REPLY_SZ
 ldr r1,nag_SC_PROCNAMEGETMSG_REPLY
 movs r2,#SC_DEFAULT_POOL-1
 adds r2,r2,#1
 movs r3,#-SC_FATAL_IF_TMO
 NEG(r3)
 bl sc_msgAllocClr
 adds r1,r0,#SC_PROCNAMEGETMSG_REPLY_TARGET-1
 ldr r7,[r7,#0] 
 adds r7,r7,#MODULE_NAME 
nag1:
 ldrb r2,[r7,#0]
 adds r1,r1,#1
 adds r7,r7,#1
 strb r2,[r1,#0]
 cmp r2,#0
 bne nag1
 movs r1,r0 
 adds r1,r1,#SC_PROCNAMEGETMSG_REPLY_MODULE-1
 adds r6,r6,#MODULE_NAME
nag2:
 ldrb r2,[r6,#0]
 adds r1,r1,#1
 adds r6,r6,#1
 strb r2,[r1,#0]
 cmp r2,#0
 bne nag2
 movs r1,r0
 adds r1,r1,#SC_PROCNAMEGETMSG_REPLY_PROCESS-1 
 ldr r5,[r5,#PCB_NAME]
nag3:
 ldrb r2,[r5,#0]
 adds r1,r1,#1
 adds r5,r5,#1
 strb r2,[r1,#0]
 cmp r2,#0
 bne nag3
nag_exit:
 RET r4-r7
nag_warn:
 movs r1,r0
 ldr r0,nag_lit1
nag_err_cont:
 bl sc_sysError
 movs r0,#0
 b nag_exit
nag_err:
 movs r1,r0
 ldr r0,nag_lit0
 bl sc_sysError
 IF SC_MAX_CONNECTOR > 0
nag9:
 ldr r6,nag_sc_connectors 
 lsls r1,r4,#1
 lsrs r1,r1,#25 
 cmp r1,#SC_MAX_CONNECTOR 
 bge nag_err 
 lsls r1,r1,#2
 ldr r6,[r6,r1] 
 cmp r6,#1 
 bls nag_err 
 ldr r0,[r6,#PCB_PID] 
 ldr r1,nag_SC_PROCNAMEGETMSG_REPLY
 movs r2,#0
 movs r3,#0
 push {r0-r3} 
 movs r0,#SC_PROCNAMEGETMSG_SZ
 subs r1,r1,#1 
 movs r2,#SC_DEFAULT_POOL-1
 adds r2,r2,#1
 movs r3,#-SC_FATAL_IF_TMO
 NEG(r3)
 bl sc_msgAlloc
 push {r0} 
 str r4,[r0,#SC_PROCNAMEGETMSG_PID]
 mov r0,sp 
 ldr r1,[sp,#4] 
 movs r2,#0
 bl sc_msgTx
 add r1,sp,#4 
 movs r2,#SC_MSGRX_BOTH
 subs r0,r2,#SC_MSGRX_BOTH+(-SC_ENDLESS_TMO) 
 bl sc_msgRx
 add sp,sp,#5*4 
 ldr r1,[r0,#SC_PROCNAMEGETMSG_REPLY_ERROR] 
 cmp r1,#0
 beq nag_exit 
 push {r0} 
 mov sp,r0
 bl sc_msgFree
 pop {r0} 
 b nag_exit
 ENDIF 
 LALIGN
nag_sciopta:
 LONG sciopta
nag_sc_modules:
 LONG sc_modules
nag_lit0:
 LONG KERNEL_EILL_PID|SC_PROCNAMEGET|SC_ERR_MODULE_FATAL
nag_lit1:
 LONG KERNEL_EILL_PID|SC_PROCNAMEGET
nag_SC_PROCNAMEGETMSG_REPLY:
 LONG SC_PROCNAMEGETMSG_REPLY
 IF SC_MAX_CONNECTOR > 0
nag_sc_connectors:
 LONG sc_connectors
 ENDIF
 SC_ENDFUNC sc_procNameGet
 XSC_TFUNC sc_procPathGet,-8
 SC_TFUNC sc_procPathGet
 push {r4-r7,lr}
 ldr r3,pag_sciopta
 ldr r7,pag_sc_modules
 ldr r6,[r3,#SC_CURMODULE]
 movs r5,r1
 ldr r4,[r3,#SC_CURPCB]
 adds r1,r0,#-SC_CURRENT_PID
 beq pag0
 movs r4,r0
 IF SC_MAX_CONNECTOR > 0
 bmi pag9
 ELSE
 bmi pag_err
 ENDIF
 lsrs r1,r0,#24
 cmp r1,#SC_MAX_MODULE
 bge pag_err
 lsls r1,r1,#2
 ldr r6,[r7,r1]
 cmp r6,#1
 bls pag_warn
 ldr r2,[r6,#MODULE_MAX_PROCESS]
 ldr r4,[r6,#MODULE_PCBS]
 lsls r1,r0,#18
 lsrs r1,r1,#18
 cmp r1,r2
 bge pag_err
 lsls r1,r1,#2
 ldr r4,[r4,r1]
 cmp r4,#0
 beq pag_warn
 ldr r1,[r4,#PCB_PID]
 cmp r1,r0
 bne pag_warn
pag0: 
 movs r0,#SC_PROCPATHGETMSG_REPLY_SZ
 movs r1,r6
 adds r1,r1,#MODULE_NAME
pag1:
 ldrb r2,[r1,#0]
 adds r0,r0,#1
 adds r1,r1,#1
 cmp r2,#0
 bne pag1
 cmp r5,#0
 beq pag3
 ldr r1,[r7,#0]
 adds r1,r1,#MODULE_NAME
pag2: 
 ldrb r2,[r1,#0]
 adds r0,r0,#1
 adds r1,r1,#1
 cmp r2,#0
 bne pag2
 adds r0,r0,#1
pag3:
 ldr r1,[r4,#PCB_NAME]
pag4: 
 ldrb r2,[r1,#0]
 adds r0,r0,#1
 adds r1,r1,#1
 cmp r2,#0
 bne pag4
 ldr r1,pag_lit2
 movs r2,#SC_DEFAULT_POOL-1
 adds r2,r2,#1
 movs r3,#-SC_FATAL_IF_TMO
 NEG(r3)
 bl sc_msgAlloc
 adds r1,r0,#4
 ldr r2,[r4,#PCB_PID]
 movs r3,#0
 stmia r1!,{r2-r3}
 movs r2,#'/'
 strb r2,[r1,#0]
 adds r6,r6,#MODULE_NAME
 cmp r5,#0
 beq pag6
 strb r2,[r1,#1]
 adds r1,r1,#1
 ldr r3,[r7,#0]
 adds r3,r3,#MODULE_NAME
pag5: 
 ldrb r5,[r3,#0]
 adds r1,r1,#1
 adds r3,r3,#1
 strb r5,[r1,#0]
 cmp r5,#0
 bne pag5
 strb r2,[r1,#0]
pag6: 
 ldrb r5,[r6,#0]
 adds r1,r1,#1
 adds r6,r6,#1
 strb r5,[r1,#0]
 cmp r5,#0
 bne pag6
 strb r2,[r1,#0]
 ldr r4,[r4,#PCB_NAME]
pag7: 
 ldrb r5,[r4,#0]
 adds r1,r1,#1
 adds r4,r4,#1
 strb r5,[r1,#0]
 cmp r5,#0
 bne pag7
pag_exit: 
 RET r4-r7
pag_warn:
 movs r1,r0
 ldr r0,pag_lit1
pag_err_cont:
 bl sc_sysError
 movs r0,#0
 b pag_exit
pag_err:
 movs r1,r0
 ldr r0,pag_lit0
 bl sc_sysError
 IF SC_MAX_CONNECTOR > 0
pag9:
 ldr r6,pag_sc_connectors
 lsls r1,r4,#1
 lsrs r1,r1,#25
 cmp r1,#SC_MAX_CONNECTOR
 bge pag_err
 lsls r1,r1,#2
 ldr r6,[r6,r1]
 cmp r6,#1
 bls pag_err
 ldr r0,[r6,#PCB_PID]
 ldr r1,pag_lit2
 movs r2,#0
 movs r3,#0
 push {r0-r3}
 movs r0,#SC_PROCPATHGETMSG_SZ
 subs r1,r1,#1
 movs r2,#SC_DEFAULT_POOL-1
 adds r2,r2,#1
 movs r3,#-SC_FATAL_IF_TMO
 NEG(r3)
 bl sc_msgAlloc
 push {r0}
 adds r1,r0,#4
 stmia r1!,{r4-r5}
 mov r0,sp
 ldr r1,[sp,#4]
 movs r2,#0
 bl sc_msgTx
 add r1,sp,#4
 movs r2,#SC_MSGRX_BOTH
 subs r0,r2,#SC_MSGRX_BOTH+(-SC_ENDLESS_TMO)
 bl sc_msgRx
 add sp,sp,#5*4
 ldr r1,[r0,#SC_PROCPATHGETMSG_REPLY_ERROR]
 cmp r1,#0
 beq pag_exit
 push {r0}
 mov sp,r0
 bl sc_msgFree
 pop {r0}
 b pag_exit
 ENDIF 
 LALIGN
pag_sciopta:
 LONG sciopta
pag_sc_modules:
 LONG sc_modules
pag_lit0:
 LONG KERNEL_EILL_PID|SC_PROCPATHGET|SC_ERR_MODULE_FATAL
pag_lit1:
 LONG KERNEL_EILL_PID|SC_PROCPATHGET
pag_lit2:
 LONG SC_PROCPATHGETMSG_REPLY
 IF SC_MAX_CONNECTOR > 0
pag_sc_connectors:
 LONG sc_connectors
 ENDIF
 SC_ENDFUNC sc_procPathGet
 XSC_TFUNC sc_sysProcPcbGet,-12
 SC_TFUNC sc_sysProcPcbGet
 ldr r3,sppg_sciopta
 movs r1,r0
 adds r0,r0,#-SC_CURRENT_PID
 ldr r0,[r3,#SC_CURPCB]
 beq sppg_exit
 cmp r1,#0
 bmi sppg_ret0
 lsrs r0,r1,#24
 cmp r0,#SC_MAX_MODULE
 bge sppg_ret0
 ldr r3,sppg_sc_modules
 lsls r0,r0,#2
 ldr r3,[r3,r0]
 cmp r3,#1
 bls sppg_ret0
 ldr r2,[r3,#MODULE_PCBS]
 ldr r0,[r3,#MODULE_MAX_PROCESS]
 lsls r3,r1,#18
 lsrs r3,r3,#18
 cmp r3,r0
 bge sppg_ret0
 lsls r3,r3,#2
 ldr r0,[r2,r3]
 cmp r0,#0
 beq sppg_exit
 ldr r2,[r0,#PCB_PID]
 cmp r1,r2
 beq sppg_exit 
sppg_ret0:
 movs r0,#0
sppg_exit:
 bx lr
 LALIGN
sppg_sciopta:
 LONG sciopta
sppg_sc_modules:
 LONG sc_modules
 SC_ENDFUNC sc_sysProcPcbGet
 XSC_TFUNC sc_sysProcCurrentName,-16
 SC_TFUNC sc_sysProcCurrentName
 ldr r3,spcn_sciopta
 ldr r0,[r3,#SC_CURPCB]
 cmp r0,#0
 beq spcn1
 ldr r0,[r0,#PCB_NAME]
spcn1:
 bx lr
 LALIGN
spcn_sciopta:
 LONG sciopta
 SC_ENDFUNC sc_sysProcCurrentName
 XSC_TFUNC sc_procYield,-16
 SC_TFUNC sc_procYield
 push {lr}
 ldr r3,py_sciopta
 ldr r2,[r3,#SC_CURPCB]
 ldrh r0,[r2,#PCB_FLAGS_H]
 lsrs r0,r0,#PCB_FLG_IS_PRIO_POS+1
 bcc py_err0
 adds r2,r2,#PCB_PCBLIST
 ldr r3,[r2,#dbl_prev]
 ldr r0,[r2,#dbl_next]
 cmp r3,r0
 beq py_exit 
 LOCK
 dbl_del(r2,r0,r1)
 dbl_tailAdd(r3,r2,r1)
 UNLOCK
 SWAP_OUT_SAVE {r4-r7}
 EXIT
py_err0:
 ldr r0,py_lit2
 bl sc_sysError
py_exit:
 EXIT
 LALIGN
py_sciopta:
 LONG sciopta
py_lit2:
 LONG SC_PROCYIELD|KERNEL_EPROC_NOT_PRIO
 SC_ENDFUNC sc_procYield
 XSC_TFUNC sc_procPrioSet,-12
 SC_TFUNC sc_procPrioSet
 push {r4-r7,lr}
 ldr r3,pps_lit1
 ldr r2,[r3,#SC_CURPCB]
 ldrh r1,[r2,#PCB_FLAGS_H]
 lsrs r1,r1,#PCB_FLG_IS_PRIO_POS+1
 bcc pps_err0
 cmp r0,#31
 bls pps10
 cmp r0,#32
 bhi pps_err1
 ldr r0,[r2,#PCB_PID]
 lsls r0,r0,#18
 lsrs r0,r0,#18
 bne pps_err0
 LOCK
 makeNotReady(r2,r0,r7,r3,pps)
 movs r0,#32
 strb r0,[r2,#PCB_PRIO0_B]
 strb r0,[r2,#PCB_PRIO_B]
 ldrh r0,[r2,#PCB_FLAGS_H]
 movs r1,#PCB_FLG_IS_IDLE>>8
 lsls r1,r1,#8
 orrs r0,r0,r1
 movs r1,#PCB_FLG_IS_PRIO
 bics r0,r0,r1
 strh r0,[r2,#PCB_FLAGS_H]
 UNLOCK
 SWAP_OUT
 RET r4-r7
pps_err0:
 ldr r0,pps_lit2
 movs r1,#0
 bl sc_sysError
pps_err1:
 movs r1,r0
 ldr r0,pps_lit3
 bl sc_sysError
 movs r0,#31
 ldr r3,pps_lit1
 b pps10
pps10:
 strb r0,[r2,#PCB_PRIO0_B]
 ldr r1,[r3,#SC_CURMODULE]
 ldr r1,[r1,#MODULE_PRIO]
 adds r1,r1,r0
 cmp r1,#31
 ldrb r0,[r2,#PCB_PRIO_B]
 bls pps11
 movs r1,#31
pps11:
 cmp r1,r0
 beq pps_exit
 LOCK
 makeNotReady(r2,r0,r7,r3,pps2)
 prioMask(r2,r1,r0,r3)
 makeReady(r2,r0,r1,r7,r3,pps)
 UNLOCK
 SWAP_OUT
pps_exit:
 RET r4-r7
 LALIGN
pps_lit1:
 LONG sciopta
pps_lit2:
 LONG (SC_PROCPRIOSET|KERNEL_EPROC_NOT_PRIO|SC_ERR_MODULE_FATAL)
pps_lit3:
 LONG (SC_PROCPRIOSET|KERNEL_EILL_PRIORITY)
 SC_ENDFUNC sc_procPrioSet
 XSC_TFUNC sc_procSchedLock,-16
 SC_TFUNC sc_procSchedLock
 push {lr}
 ldr r3,psl_lit1
 ldr r0,[r3,#SC_CURPCB]
 ldrh r1,[r0,#PCB_FLAGS_H]
 lsrs r1,r1,#PCB_FLG_IS_PRIO_POS+1
 bcc psl_err0
 LOCK
 ldr r0,[r3,#SC_SCHEDULER_LOCK]
 adds r0,r0,#1
 str r0,[r3,#SC_SCHEDULER_LOCK] 
 UNLOCK
 EXIT
psl_err0:
 ldr r1,[r0,#PCB_PID]
 ldr r0,psl_lit0
 bl sc_sysError
 LALIGN
psl_lit0:
 LONG (SC_PROCSCHEDLOCK|KERNEL_EPROC_NOT_PRIO|SC_ERR_MODULE_FATAL)
psl_lit1:
 LONG sciopta
 SC_ENDFUNC sc_procSchedLock
 XSC_TFUNC sc_procSchedUnlock,-16
 SC_TFUNC sc_procSchedUnlock
 ldr r3,psul_lit1
 ldr r0,[r3,#SC_CURPCB]
 ldrh r1,[r0,#PCB_FLAGS_H]
 lsrs r1,r1,#PCB_FLG_IS_PRIO_POS+1
 bcc psul_err0
 mov r2,lr
 LOCK
 ldr r1,[r3,#SC_SCHEDULER_LOCK]
 subs r0,r1,#1
 bmi psul_err1
 str r0,[r3,#SC_SCHEDULER_LOCK]
 UNLOCK
 ldr r0,[r3,#SC_SCHEDULE_REQ]
 cmp r0,#0
 beq psul1
 mov lr,r2
 push {r4-r7,lr}
 SWAP_OUT
 RET r4-r7
psul1:
 bx r2
psul_err0:
 ldr r1,[r0,#PCB_PID]
 ldr r0,psul_lit0
 bl sc_sysError
psul_err1:
 UNLOCK
 ldr r0,psul_lit2
 bl sc_sysError
 LALIGN
psul_lit0:
 LONG (SC_PROCSCHEDUNLOCK|KERNEL_EPROC_NOT_PRIO|SC_ERR_MODULE_FATAL)
psul_lit1:
 LONG sciopta
psul_lit2:
 LONG (KERNEL_EUNLOCK_WO_LOCK|SC_ERR_MODULE_FATAL|SC_PROCSCHEDUNLOCK)
 SC_ENDFUNC sc_procSchedUnlock
#define PARA_NAME 0
#define PARA_ENTRY 4
#define PARA_STACKSIZE 8
#define PARA_SLICE 12
#define PARA_PRIO 36
#define PARA_STATE 40
#define PARA_MODULEID 44
#define PARA_PCB 48
#define PARA_STACK 52
#define PARA_TYPE 56
 XSC_TFUNC sc_sysProcCreate,24
 SC_TFUNC sc_sysProcCreate
 push {r0-r7,lr}
 IF SC_PROC_PARA_CHECK == 1
 ldr r0,[sp,#PARA_TYPE]
 lsrs r0,r0,#24
 cmp r0,#SC_PROCPRIOCREATE>>24
 beq spc1
 cmp r0,#SC_PROCPRIOCREATESTATIC>>24
 bne spc2
spc1:
 ldr r1,[sp,#PARA_PRIO]
 cmp r1,#32
 blo spc6
 movs r2,#KERNEL_EILL_PRIORITY>>12
spc_cont:
 ldr r0,[sp,#PARA_TYPE]
 lsls r2,r2,#12
 orrs r0,r0,r2
 adds r0,r0,#SC_ERR_MODULE_FATAL
 bl sc_sysError
spc2: 
 cmp r0,#SC_PROCINTCREATE>>24
 beq spc3
 cmp r0,#SC_PROCINTCREATESTATIC>>24
 bne spc4
#if 0 
 beq spc3
 cmp r0,#SC_PROCUSRINTCREATE>>24
 beq spc3
 cmp r0,#SC_PROCUSRINTCREATESTATIC>>24
 bne spc4
#endif
spc3:
 cmp r3,#SC_MAX_INT_VECTOR
 bls spc6
 movs r1,r3
 movs r2,#KERNEL_EILL_VECTOR>>12
 b spc_cont
spc4:
 cmp r0,#SC_PROCTIMCREATE>>24
 beq spc5
 cmp r0,#SC_PROCTIMCREATESTATIC>>24
 bne spc6
spc5:
 lsrs r1,r3,#24 
 bne spc5a
 adds r1,r3,#0 
 bne spc6
spc5a: 
 movs r2,#KERNEL_EILL_SLICE>>12
 b spc_cont
spc6:
 cmp r2,#16*4 
 bhi spc7
 movs r1,r2
 movs r2,#KERNEL_EILL_STACKSIZE>>12
 b spc_cont
spc7:
 ldr r1,[sp,#PARA_STACK]
 cmp r1,#0
 beq spc9
 ldr r1,[sp,#PARA_PCB]
 cmp r1,#0
 bne spc10 
spc8:
 movs r2,#KERNEL_EILL_PROCTYPE>>12
 b spc_cont
spc9:
 cmp r0,#SC_PROCINTCREATE>>24
 beq spc8
 cmp r0,#SC_PROCPRIOCREATE>>24
 beq spc8
 cmp r0,#SC_PROCTIMCREATE>>24
 beq spc8
#if 0 
 cmp r0,#SC_PROCUSRINTCREATE>>24
 beq spc8
#endif
spc10:
 ENDIF
 ldr r7,=sc_modules
 ldr r1,[sp,#PARA_MODULEID]
 lsrs r6,r1,#24-2
 ldr r6,[r7,r6]
 ldr r7,=sciopta
 IF SC_PROC_PARA_CHECK == 1
 cmp r6,#1
 bhi spc11 
 movs r2,#KERNEL_EILL_MODULE>>12
 b spc_cont
spc11:
 ENDIF
 ldr r1,[r6,#MODULE_NPROCESS]
 ldr r0,[r6,#MODULE_MAX_PROCESS]
 cmp r0,r1
 bne spc12
 movs r2,#KERNEL_ENO_MORE_PROC>>12
 IF SC_PROC_PARA_CHECK == 1
 b spc_cont
 ELSE
spc_cont:
 ldr r0,[sp,#PARA_TYPE]
 lsls r2,r2,#12
 orrs r0,r0,r2
 adds r0,r0,#SC_ERR_MODULE_FATAL
 bl sc_sysError
 ENDIF
spc12:
 ldr r0,[sp,#PARA_NAME]
 movs r1,#SC_PROC_NAME_SIZE+2
spc13:
 subs r1,r1,#1
 ldrb r4,[r0,#0]
 adds r0,r0,#1
 cmp r4,#0
 bne spc13
spc14:
 IF SC_PROC_PARA_CHECK == 1
 cmp r1,#SC_PROC_NAME_SIZE+1
 beq spc15
 cmp r1,#0
 bgt spc16
spc15:
 movs r2,#KERNEL_EILL_PROC_NAME>>12
spc15a: 
 ldr r1,[sp,#PARA_NAME]
 b spc_cont
spc16:
 ENDIF
 ldr r0,[sp,#PARA_NAME]
 bl sc_miscCrcString
 IF SC_PROC_PARA_CHECK == 1
 mov r12,r6
 ldr r4,[r6,#MODULE_NPROCESS]
 ldr r1,[r6,#MODULE_PCBS]
 cmp r4,#0
 beq spc20
spc17:
 ldr r2,[r1,#0]
 adds r1,r1,#4
 cmp r2,#0
 beq spc17
 ldrh r3,[r2,#PCB_HASH_H]
 cmp r3,r0
 bne spc19
 ldr r2,[r2,#PCB_NAME]
 ldr r3,[sp,#PARA_NAME]
spc18:
 ldrb r5,[r2,#0]
 adds r2,r2,#1
 ldrb r6,[r3,#0]
 adds r3,r3,#1
 cmp r5,r6
 bne spc19 
 cmp r5,#0
 bne spc18
 ldr r1,[sp,#PARA_NAME]
 movs r2,#KERNEL_EILL_PROC_NAME>>12
 b spc_cont
spc19:
 subs r4,r4,#1
 bne spc17
spc20:
 mov r6,r12
 ENDIF
 ldr r5,[sp,#PARA_PCB]
 ldr r1,[r6,#MODULE_FREESIZE]
 ldr r2,[r6,#MODULE_DATAPTR]
 cmp r5,#0
 bne spc22
 cmp r1,#SIZEOF_PCB
 bhs spc21
spc_oom:
 movs r2,#KERNEL_EOUT_OF_MEMORY>>12
 b spc_cont
spc21:
 movs r5,r2
 subs r1,r1,#SIZEOF_PCB
 str r1,[r6,#MODULE_FREESIZE]
 adds r2,r2,#SIZEOF_PCB
 str r2,[r6,#MODULE_DATAPTR]
spc22:
 ldr r4,[sp,#PARA_STACK]
 cmp r4,#0
 ldr r3,[sp,#PARA_STACKSIZE]
 bne spc23
 cmp r3,r1
 bhi spc_oom
 movs r4,r2
 adds r2,r2,r3
 str r2,[r6,#MODULE_DATAPTR]
 subs r1,r1,r3
 str r1,[r6,#MODULE_FREESIZE]
spc23:
 movs r2,#0
 movs r1,#SIZEOF_PCB-4
spc24:
 str r2,[r5,r1]
 subs r1,r1,#4
 bpl spc24
 strh r0,[r5,#PCB_HASH_H]
 mov r12,r4 
 ldr r3,[r6,#MODULE_NPROCESS]
 adds r3,r3,#1
 str r3,[r6,#MODULE_NPROCESS]
 ldr r4,[r6,#MODULE_PID_NXT]
 lsls r1,r4,#2
 ldr r2,[r6,#MODULE_PCBS] 
 str r5,[r2,r1] 
 movs r0,r4
 ldr r1,[r6,#MODULE_ID]
 orrs r0,r0,r1
 ldr r1,[r6,#MODULE_PID_CNT]
 lsls r1,r1,#14 
 orrs r0,r0,r1 
 str r0,[r5,#PCB_PID]
 ldr r0,[r6,#MODULE_MAX_PROCESS]
 lsrs r1,r1,#14 
 cmp r0,r3 
 beq spc24c
spc24a:
 adds r4,r4,#1 
 cmp r4,r0 
 bne spc24b 
 movs r4,#1 
 adds r1,r1,#1 
 lsls r1,r1,#8+14 
 lsrs r1,r1,#8+14
spc24b:
 lsls r3,r4,#2
 ldr r3,[r2,r3] 
 cmp r3,#0
 bne spc24a 
 str r4,[r6,#MODULE_PID_NXT] 
 str r1,[r6,#MODULE_PID_CNT] 
spc24c:
 mov r4,r12 
 ldr r0,spc_SC_PCB_MAGIC_STATIC
 ldr r1,[sp,#PARA_TYPE]
 movs r2,#PCB_FLG_IS_STATIC
 lsrs r1,r1,#24
 movs r3,#PCB_FLG_IS_IRQ|PCB_FLG_READY
 cmp r1,#SC_PROCINTCREATE>>24
 beq spc25
 cmp r1,#SC_PROCINTCREATESTATIC>>24
 beq spc26
 movs r3,#PCB_FLG_IS_PRIO
 cmp r1,#SC_PROCPRIOCREATE>>24
 beq spc25
 cmp r1,#SC_PROCPRIOCREATESTATIC>>24
 beq spc26
 movs r3,#PCB_FLG_IS_TIMER
 cmp r1,#SC_PROCTIMCREATE>>24
 beq spc25
 cmp r1,#SC_PROCTIMCREATESTATIC>>24
 beq spc26
#if 0
 movs r3,#PCB_FLG_IS_UIRQ 
 cmp r1,#SC_PROCUSRINTCREATESTATIC>>24
 beq spc26 
 cmp r1,#SC_PROCUSRINTCREATE>>24
 beq spc25
#endif
 movs r0,#SC_SYSPROCCREATE>>24
 lsls r0,r0,#24
 adds r0,r0,#SC_ERR_MODULE_FATAL
 ldr r1,[sp,#PARA_TYPE]
spc_err_cont: 
 bl sc_sysError
spc25:
 movs r2,#0
 ldr r0,spc_SC_PCB_MAGIC
spc26:
 orrs r2,r2,r3
 strh r2,[r5,#PCB_FLAGS_H]
 str r0,[r5,#PCB_MAGIC]
 ldr r0,[sp,#PARA_NAME]
 str r0,[r5,#PCB_NAME]
 movs r0,r5
 adds r0,r0,#PCB_MSGQUEUE
 str r0,[r0,#0] 
 str r0,[r0,#4]
 adds r0,r0,#8 
 str r0,[r0,#0] 
 str r0,[r0,#4]
 adds r0,r0,#8 
 str r0,[r0,#0] 
 str r0,[r0,#4]
 ldr r0,[sp,#PARA_ENTRY]
 str r0,[r5,#PCB_ENTRY]
 ldr r1,[sp,#PARA_PRIO]
 ldr r0,[r6,#MODULE_PRIO]
 str r1,[r5,#PCB_INITIAL]
 strb r1,[r5,#PCB_PRIO0_B]
 adds r1,r1,r0
 cmp r1,#32
 blo spc27
 movs r1,#31
spc27:
 movs r0,#1
 str r0,[r5,#PCB_TRIGGER_VAL]
 prioMask(r5,r1,r0,r7)
 ldr r1,[sp,#PARA_SLICE]
 ldr r0,[sp,#PARA_STACKSIZE]
 str r1,[r5,#PCB_SLICE]
 adds r4,r4,#3
 lsrs r4,r4,#2
 lsls r4,r4,#2
 str r4,[r5,#PCB_STACKBTM]
 adds r0,r0,r4
 lsrs r0,r0,#3 
 lsls r0,r0,#3
 IF SC_STACK_CHECK == 1
 subs r1,r0,#7
 subs r1,r1,#1
 ELSE
 movs r1,r0
 ENDIF
 str r1,[r5,#PCB_STACKTOP]
 ldr r1,spc_stackvirgin
spc28:
 subs r0,r0,#4
 cmp r0,r4
 str r1,[r0,#0]
 bne spc28
 ldr r0,[sp,#PARA_STATE]
 cmp r0,#0
 bne spc29
 movs r0,#1
 strh r0,[r5,#PCB_STOPPED_H]
 lsls r0,r0,#ST_STOPPED_POS
 strh r0,[r5,#PCB_STATE_H] 
spc29:
 IF SC_PROC_HOOK == 1
 IF SC_PROCCREATE_HOOK == 1
 ldr r2,spc_SC_PROCCREATEHOOK
 ldr r2,[r7,r2]
 cmp r2,#0
 beq spc30
 movs r0,r5
 movs r1,#0
 BLX_R2_t
spc30: 
 ldr r2,[r6,#MODULE_PROCCREATEHOOK]
 cmp r2,#0
 beq spc31
 movs r0,r5
 movs r1,#0
 BLX_R2_t
spc31:
 ENDIF
 ENDIF
 ldr r4,spc_SC_PROCD
 ldr r4,[r7,r4]
 cmp r4,#0
 beq spc33
 movs r0,#SC_PROCCREATENOTIFYMSG_SZ
 movs r1,#1 
 lsls r1,r1,#31
 movs r2,#3
 lsls r2,r2,#8
 orrs r1,r1,r2 
 movs r2,#SC_DEFAULT_POOL-1
 adds r2,r2,#1 
 movs r3,#SC_ENDLESS_TMO+1
 subs r3,r3,#1
 bl sc_msgAlloc
 push {r0}
 ldr r1,[r5,#PCB_PID]
 str r1,[r0,#SC_PROCCREATENOTIFYMSG_PID]
 movs r2,#SC_PROCCREATENOTIFYMSG_PHASH
 ldrh r1,[r5,#PCB_HASH_H]
 strh r1,[r0,r2]
 subs r2,r2,#2
 movs r1,#MODULE_HASH_H
 ldrh r3,[r6,r1]
 strh r3,[r0,r2]
 subs r2,r2,#2
 ldr r3,=sc_modules
 ldr r3,[r3,#0]
 ldrh r1,[r3,r1]
 strh r1,[r0,r2]
 ldr r1,[r5,#PCB_NAME] 
 adds r6,r6,#MODULE_NAME 
 adds r3,r3,#MODULE_NAME 
 movs r2,#0
spc32:
 ldr r4,[r1,r2]
 str r4,[r0,#SC_PROCCREATENOTIFYMSG_PROCESS]
 ldr r4,[r6,r2]
 str r4,[r0,#SC_PROCCREATENOTIFYMSG_MODULE]
 ldr r4,[r3,r2]
 str r4,[r0,#SC_PROCCREATENOTIFYMSG_TARGET]
 adds r0,r0,#4
 adds r2,r2,#4
 cmp r2,#32
 bne spc32
 subs r6,r6,#MODULE_NAME
 mov r0,sp
 ldr r1,spc_SC_PROCD
 ldr r1,[r7,r1]
 movs r2,#0
 bl sc_msgTx
 add sp,sp,#4
spc33:
 ldr r0,[sp,#PARA_TYPE]
 lsrs r0,r0,#24
 cmp r0,#SC_PROCPRIOCREATE>>24
 beq spc_prio
 cmp r0,#SC_PROCPRIOCREATESTATIC>>24
 beq spc_prio
 cmp r0,#SC_PROCINTCREATE>>24
 beq spc_int
 cmp r0,#SC_PROCINTCREATESTATIC>>24
 beq spc_int
 cmp r0,#SC_PROCTIMCREATE>>24
 beq spc_tim
 cmp r0,#SC_PROCTIMCREATESTATIC>>24
 beq spc_tim
#if 0
 cmp r0,#SC_PROCUSRINTCREATE>>24
 beq spc_uint
 cmp r0,#SC_PROCUSRINTCREATESTATIC>>24
 beq spc_uint
#endif
spc34:
 b spc34
spc_prio:
 IF CPUTYPE <= Xscale
 ldr r0,[r5,#PCB_STACKTOP]
 subs r0,r0,#6*4
 adr r1,process_exit
 str r1,[r0,#5*4] 
 movs r1,#0xcc
 str r1,[r0,#4*4] 
 movs r1,#0x33
 str r1,[r0,#3*4] 
 movs r1,#0x22
 str r1,[r0,#2*4] 
 movs r1,#0x11
 str r1,[r0,#1*4] 
 movs r1,#0
 str r1,[r0,#0*4] 
 IF SC_PROC_HOOK == 1
 IF SC_PROCMMU_HOOK == 1
 sub r0,#4
 ldr r2,[r5,#PCB_PID]
 ldr r3,=0x40000001 
 lsr r2,r2,#23 
 bne spc36a2
 ldr r3,=0x55555555
spc36a2: 
 mov r1,#3
 lsl r1,r2 
 orr r1,r3
 str r1,[r0,#0] 
 ENDIF
 ENDIF
 IF CPUTYPE == Xscale
 subs r0,r0,#2*4
 movs r1,#0xff
 str r1,[r0,#1*4] 
 movs r1,#0xee
 str r1,[r0,#0*4] 
 ENDIF
 movs r1,#0xbb
spc36a1:
 subs r0,r0,#4
 str r1,[r0,#0]
 subs r1,r1,#0x11
 cmp r1,#0x33
 bne spc36a1
 ldr r1,[r5,#PCB_ENTRY]
 subs r0,r0,#4
 str r1,[r0,#0*4] 
 lsls r1,r1,#31 
 lsrs r1,r1,#31-5
 ldr r3,[sp,#PARA_TYPE] 
 movs r4,#SC_PROC_TYPE_EXT_USR
 movs r2,#PSR_SYS_MODE
 tst r3,r4
 beq spc36a
 movs r2,#PSR_USR_MODE
spc36a: 
 orrs r1,r1,r2
 subs r0,r0,#4
 str r1,[r0,#0*4] 
 adds r0,r0,#1
 str r0,[r5,#PCB_STACKPTR] 
 ELSE
 IF CPUTYPE == ARMv6
 ldr r0,[r5,#PCB_STACKTOP]
 subs r0,r0,#8*4
 ldr r1,[r5,#PCB_ENTRY]
 str r1,[r0,#6*4] 
 lsls r1,r1,#31 
 lsrs r1,r1,#31-5
 ldr r3,[sp,#PARA_TYPE] 
 movs r4,#SC_PROC_TYPE_EXT_USR
 movs r2,#PSR_SYS_MODE
 tst r3,r4
 beq spc36b
 movs r2,#PSR_USR_MODE
spc36b: 
 orrs r1,r1,r2
 subs r0,r0,#4
 str r1,[r0,#7*4] 
 adr r1,process_exit
 str r1,[r0,#5*4] 
 movs r1,#0xcc
 str r1,[r0,#4*4] 
 movs r1,#0x33
 str r1,[r0,#3*4] 
 movs r1,#0x22
 str r1,[r0,#2*4] 
 movs r1,#0x11
 str r1,[r0,#1*4] 
 movs r1,#0
 str r1,[r0,#0*4] 
 IF SC_PROC_HOOK == 1
 IF SC_PROCMMU_HOOK == 1
 sub r0,#4
 ldr r2,[r5,#PCB_PID]
 ldr r3,=0x40000001 
 lsr r2,r2,#23 
 bne spc36b2
 ldr r3,=0x55555555
spc36b2: 
 mov r1,#3
 lsl r1,r2 
 orr r1,r3
 str r1,[r0,#0] 
 ENDIF
 ENDIF
 movs r1,#0xbb
spc36b1:
 subs r0,r0,#4
 str r1,[r0,#0]
 subs r1,r1,#0x11
 cmp r1,#0x33
 bne spc36b1
 adds r0,r0,#1
 str r0,[r5,#PCB_STACKPTR] 
 ELSE
 ldr r0,[r5,#PCB_STACKTOP]
 mov r3,#1<<24 
 ldr r2,[r5,#PCB_ENTRY]
 adr r1,process_exit
 stmfd r0!,{r1-r3}
 movs r3,#0xcc 
 movs r2,#0x33
 movs r1,#0x22
 stmfd r0!,{r1-r3}
 movs r2,#0x11
 movs r1,#0
 stmfd r0!,{r1-r2}
 movs r3,#0
 sub r3,r3,#7 
 movs r2,#0xbb
 movs r1,#0xaa
 stmfd r0!,{r1-r3} 
 movs r3,#0x99
 movs r2,#0x88
 movs r1,#0x77
 stmfd r0!,{r1-r3}
 movs r3,#0x66
 movs r2,#0x55
 movs r1,#0x44
 stmfd r0!,{r1-r3}
 str r0,[r5,#PCB_STACKPTR] 
 ENDIF 
 ENDIF 
 ldr r0,[sp,#PARA_STATE]
 cmp r0,#0
 beq spc_exit
 LOCK
 makeReady(r5,r0,r1,r2,r7,spc)
 UNLOCK
 ldr r0,[r7,#SC_CURPRIOPCB]
 cmp r0,#0
 beq spc37
 ldrb r1,[r0,#PCB_PRIO_B]
 ldrb r2,[r5,#PCB_PRIO_B]
 cmp r2,r1
 bhs spc_exit 
spc37:
 push {r5}
 SWAP_OUT
 pop {r5}
spc_exit:
 add sp,sp,#4*4
 ldr r0,[r5,#PCB_PID]
 RET r4-r7
 LTORG
spc_uint: 
spc_int:
 ldr r1,[r5,#PCB_STACKTOP]
 str r1,[r5,#PCB_STACKPTR]
 ldr r0,spc_irq_vectors
 ldr r1,[r5,#PCB_VECTOR]
 lsls r1,r1,#2
 IF SC_PROC_PARA_CHECK == 1
 ldr r2,[r0,r1]
 cmp r2,#0
 bne spc_intinuse_err
 str r5,[r0,r1]
 movs r1,#0
 subs r1,r1,#1
 strh r1,[r5,#PCB_NESTED_H]
 ELSE
 str r5,[r0,r1]
 ENDIF
 ldr r2,[r5,#PCB_ENTRY]
 movs r0,#0
 subs r0,r0,#1
 movs r1,r5
 FAKE_INT 
 b spc_exit
 IF SC_PROC_PARA_CHECK == 1
spc_intinuse_err:
 movs r2,#KERNEL_EALREADY_DEFINED>>12
 b spc_cont 
 ENDIF
spc_tim:
 ldr r1,[r5,#PCB_STACKTOP]
#if 0
 IF SC_CDEBUG == 1
 subs r1,r1,#4
 ENDIF
#endif
 str r1,[r5,#PCB_STACKPTR]
 movs r0,#0
 subs r0,r0,#1
 movs r1,r5
 FAKE_INT 
 ldr r0,[sp,#PARA_STATE]
 cmp r0,#0
 beq spc_exit
 ldrh r0,[r5,#PCB_FLAGS_H]
 movs r1,#PCB_FLG_READY
 orrs r0,r0,r1
 strh r0,[r5,#PCB_FLAGS_H] 
 ldr r0,[r5,#PCB_INITIAL]
 ldr r1,[r5,#PCB_SLICE]
 cmp r0,#0
 bne spc38
 movs r0,r1
spc38:
 mov r6,sp
 LOCK
 enterTimer(r5,r0,r1,r2,r7,spc38)
 ldrh r1,[r5,#PCB_STATE_H]
 movs r0,#ST_WAIT_TMO
 orrs r1,r1,r0
 strh r1,[r5,#PCB_STATE_H]
 UNLOCK
 b spc_exit
 LALIGN
spc_stackvirgin:
 LONG 0x17171717
spc_SC_PROCD:
 LONG SC_PROCD
spc_SC_PCB_MAGIC_STATIC:
 LONG SC_PCB_MAGIC_STATIC
spc_SC_PCB_MAGIC:
 LONG SC_PCB_MAGIC
spc_irq_vectors:
 LONG sc_irq_vectors
 IF SC_PROC_HOOK == 1
 IF SC_PROCCREATE_HOOK == 1
spc_SC_PROCCREATEHOOK:
 LONG SC_PROCCREATEHOOK
 ENDIF
 ENDIF
#if 0
spc_virgin1:
 LONG 0xdddddddd
spc_virgin2:
 LONG 0x11111111
#endif
process_exit:
 ldr r0,spc_lit0
 b spc_err_cont
 LALIGN
spc_lit0:
 LONG KERNEL_EPROC_TERMINATE|SC_ERR_MODULE_FATAL
 SC_ENDFUNC sc_sysProcCreate
#undef PARA_NAME
#undef PARA_ENTRY
#undef PARA_STACKSIZE
#undef PARA_SLICE
#undef PARA_PRIO
#undef PARA_STATE
#undef PARA_PCB
#undef PARA_STACk
#undef PARA_TYPE
 SC_TFUNC sc_sysProcKill
 push {r1,r4-r7,lr}
 ldr r7,spk_sciopta
 adds r1,r0,#-SC_CURRENT_PID
 ldr r6,[r7,#SC_CURMODULE]
 ldr r5,[r7,#SC_CURPCB]
 beq spk1
 cmp r0,#0 
 ble spk_err 
 lsrs r1,r0,#24 
 cmp r1,#SC_MAX_MODULE 
 ldr r6,spk_sc_modules
 bge spk_err 
 lsls r1,r1,#2
 ldr r6,[r6,r1] 
 cmp r6,#1 
 bls spk_err 
 ldr r3,[r6,#MODULE_MAX_PROCESS]
 lsls r2,r0,#18
 lsrs r2,r2,#18 
 cmp r2,r3 
 ldr r5,[r6,#MODULE_PCBS]
 bge spk_err 
 lsls r2,r2,#2
 ldr r5,[r5,r2] 
 cmp r5,#0 
 beq spk_err 
 ldr r3,[r5,#PCB_PID] 
 cmp r3,r0
 bne spk_err 
spk1:
 ldrh r3,[r5,#PCB_FLAGS_H]
 LOCK
 lsrs r0,r3,#PCB_FLG_IS_PRIO_POS+1
 bcc spk2
 makeNotReady(r5,r0,r1,r7,spk1)
spk2:
 ldrh r2,[r5,#PCB_STATE_H]
 movs r0,#ST_WAIT_TMO
 tst r2,r0
 beq spk3
 removeTimer(r5,r2,r0,r4)
spk3:
 movs r0,#ST_WAIT_MSGALLOC
 tst r2,r0
 beq spk4
 bics r2,r2,r0
 movs r0,r5
 adds r0,r0,#PCB_PCBLIST
 dbl_del(r0,r1,r4)
spk4:
 strh r2,[r5,#PCB_STATE_H]
 movs r0,#1
 lsls r0,r0,#PCB_FLG_IS_CONNECTOR_POS
 tst r3,r0
 IF SC_MAX_CONNECTOR > 0
 beq spk7
 bics r3,r3,r0
 movs r0,#SC_MAX_CONNECTOR
 ldr r1,spk_sc_connectors
spk5:
 ldmia r1!,{r2}
 cmp r2,r5
 beq spk6
 subs r0,r0,#1
 bpl spk5
 b spk_err1
spk6:
 subs r1,r1,#4
 movs r0,#0
 str r0,[r1,#0]
 ELSE
 bne spk_err1
 ENDIF
spk7:
 strh r3,[r5,#PCB_FLAGS_H]
 IF SC_PROC_HOOK == 1
 IF SC_PROCKILL_HOOK == 1
 ldr r2,[r6,#MODULE_PROCKILLHOOK]
 movs r0,r5
 movs r1,#0
 cmp r2,#0
 beq spk8
 BLX_R2_t
spk8: 
 ldr r2,spk_SC_PROCKILLHOOK
 ldr r2,[r7,r2]
 movs r0,r5
 movs r1,#0
 cmp r2,#0
 beq spk9
 BLX_R2_t
spk9:
 ENDIF
 ENDIF
 ldr r4,[r5,#PCB_PID]
 movs r0,#0
 str r0,[r5,#PCB_PID]
spk10: 
 lsrs r0,r3,#PCB_FLG_PCB_LOCKED_POS+1
 bcc spk11 
 UNLOCK
 movs r0,#1
 bl sc_sleep
 LOCK
 ldrh r3,[r5,#PCB_FLAGS_H]
 b spk10
spk_err_cont:
 movs r1,r0
 movs r0,r2
 bl sc_sysError
 RET r4-r7
spk_err:
 ldr r2,spk_lit1
 b spk_err_cont
spk_err1:
 ldr r2,spk_lit2
 b spk_err_cont 
spk11:
 UNLOCK
 ldr r1,[sp,#0]
 lsrs r0,r1,#SC_PROCKILL_KILL_POS+1 
 bcs spk20 
 push {r4} 
 adds r5,r5,#PCB_ALLOCED
spk12:
 ldr r0,[r5,#dbl_next]
 cmp r0,r5
 beq spk15
 LOCK
 ldr r1,[r0,#MSG_PLID]
 lsls r2,r1,#8
 lsrs r2,r2,#16
 beq spk13 
 lsls r2,r2,#8
 bics r1,r1,r2
 str r1,[r0,#MSG_PLID]
 adds r0,r0,#TMOMSG_TMOLIST
 dbl_del(r0,r1,r2)
 UNLOCK
 ldr r2,[r7,#SC_CURPID]
 subs r0,r0,#TMOMSG_TMOLIST
 movs r1,#0
 str r1,[r0,#MSG_SENDER]
 str r1,[r0,#MSG_ADDRESSEE]
 str r2,[r0,#MSG_OWNER]
 ldr r2,[r0,#MSG_SIZE]
 lsrs r1,r2,#28
 lsls r1,r1,#3
 str r1,[r0,#MSG_BUFSIZE_IDX]
 lsls r2,r2,#4
 lsrs r2,r2,#4
 str r2,[r0,#MSG_SIZE]
 b spk14
spk13:
 UNLOCK
spk14: 
 bl sc_sysMsgFree
 b spk12
spk15: 
 subs r5,r5,#PCB_ALLOCED-PCB_MSGQUEUE
spk16:
 ldr r0,[r5,#dbl_next]
 cmp r0,r5
 beq spk17
 bl sc_sysMsgFree
 b spk16
spk17:
 subs r5,r5,#PCB_MSGQUEUE
 pop {r4}
spk20:
 adds r5,#PCB_CONNECTED
spk21: 
 ldr r0,[r5,#dbl_next]
 cmp r0,r5
 beq spk22
 ldr r1,[r5,#PCB_ERRORCODE-PCB_CONNECTED]
 str r4,[r0,#MSG_OWNER]
 str r1,[r0,#MSG_HEAD+4]
 ldr r7,[r0,#MSG_SENDER]
 adds r0,r0,#MSG_HEAD
 push {r0}
 mov r0,sp
 bl sc_msgAcquire
 mov r0,sp
 movs r1,r7
 movs r2,#0
 movs r3,r4
 bl sc_msgTxAlias
 add sp,sp,#4
 b spk21
spk22:
 subs r5,r5,#PCB_CONNECTED
 ldrh r1,[r5,#PCB_FLAGS_H]
 movs r0,#PCB_FLG_IS_IRQ|PCB_FLG_IS_TIMER|PCB_FLG_IS_UIRQ
 ands r1,r1,r0
 beq spk22a
 lsrs r1,r1,#PCB_FLG_IS_TIMER_POS+1
 bcs spk22b
 ldr r0,spk_irq_vectors
 movs r2,#0
 ldr r1,[r5,#PCB_VECTOR]
 lsls r1,r1,#2
 str r2,[r0,r1]
spk22b:
 movs r0,#0
 subs r0,r0,#2 
 movs r1,r5
 FAKE_INT
spk22a: 
 lsls r0,r4,#18
 lsrs r0,r0,#16
 ldr r1,[r6,#MODULE_PCBS]
 movs r2,#0
 LOCK
 str r2,[r1,r0]
 ldr r2,[r6,#MODULE_NPROCESS]
 ldr r1,[r6,#MODULE_MAX_PROCESS]
 cmp r2,r1
 bne spk23
 ldr r1,[r6,#MODULE_PID_CNT]
 lsrs r0,r0,#2
 str r0,[r6,#MODULE_PID_NXT]
 adds r1,r1,#1
 lsls r1,r1,#14+8 
 lsrs r1,r1,#14+8
 str r1,[r6,#MODULE_PID_CNT]
spk23:
 subs r2,r2,#1
 str r2,[r6,#MODULE_NPROCESS]
 UNLOCK
 ldr r0,spk_magic
 ldr r1,[r5,#PCB_MAGIC]
 pop {r2} 
 ldr r3,spk_sciopta
 ldr r6,[r3,#SC_CURPID]
 cmp r6,r4 
 bne spk25 
 cmp r1,r0 
 bne spk24 
 lsrs r2,r2,#SC_PROCKILL_KILL_POS+1 
 bcs spk24 
 ldr r0,spk_sc_svc_stack 
 mov sp,r0
 movs r0,r5
 subs r0,r0,#MSG_HEAD
 bl sc_sysMsgFree 
spk24:
 ldr r3,spk_dispatcher
 LOCK
 bx r3
spk25:
 cmp r1,r0 
 bne spk26 
 lsrs r2,r2,#SC_PROCKILL_KILL_POS+1 
 bcs spk26 
 movs r0,r5 
 subs r0,r0,#MSG_HEAD
 bl sc_sysMsgFree
spk26: 
 RET r4-r7
 LALIGN
spk_sciopta:
 LONG sciopta
spk_sc_modules:
 LONG sc_modules
spk_irq_vectors:
 LONG sc_irq_vectors
spk_dispatcher:
 LONG sc_sysProcDispatcher
 IF SC_MAX_CONNECTOR > 0
spk_sc_connectors:
 LONG sc_connectors
 ENDIF
 IF SC_PROC_HOOK == 1
 IF SC_PROCKILL_HOOK == 1
spk_SC_PROCKILLHOOK:
 LONG SC_PROCKILLHOOK
 ENDIF
 ENDIF
spk_lit1:
 LONG KERNEL_EILL_PID|SC_PROCKILL|SC_ERR_SYSTEM_FATAL
spk_lit2:
 LONG SC_PROCKILL|SC_ERR_SYSTEM_FATAL
spk_magic:
 LONG SC_PCB_MAGIC
spk_sc_svc_stack:
 LONG sc_svc_stack
 SC_ENDFUNC sc_sysProcKill
 IF CPUTYPE <= Xscale
 SC_TFUNC swap_out
 add r0,pc,#0
 BX R0 
 CODE32
swap_out32: 
 mrs r1,cpsr
 ldr r3,so_sciopta
 ands r1,r1,#PSR_I_BIT
 mov r0,#0
 strne r1,[r3,#SC_SCHEDULE_REQ]
 ldreq r1,[r3,#SC_SCHEDULER_LOCK]
 bxne lr
 ldr r2,[r3,#SC_CURPCB]
 cmp r1,#0
 strne r1,[r3,#SC_SCHEDULE_REQ]
 ldrheq r1,[r2,#PCB_FLAGS_H]
 bxne lr
 ands r1,r1,#PCB_FLG_IS_PRIO|PCB_FLG_IS_IDLE
 streq r3,[r3,#SC_SCHEDULE_REQ]
 bxeq lr
 IF SC_STACK_CHECK == 1
 SC_PUBLIC dp_stackcheck
 ldr r1,dp_stackmagic 
 ldr r0,[r2,#PCB_STACKBTM]
 ldr r3,[r2,#PCB_STACKTOP]
 ldr r4,[r0,#0] 
 cmp r4,r1 
 bne dp_err
 ldr r4,[r3,#0] 
 cmp r4,r1
 bne dp_err
 cmp sp,r0 
 blt dp_err 
 cmp sp,r3 
 bgt dp_err2 
 ENDIF
 IF CPUTYPE == Xscale
 mra r0,r1,acc0
 IF SC_PROC_HOOK == 1 && SC_PROCMMU_HOOK == 1
 mrc p15,0,r3,c3,c0,0
 stmfd sp!,{r0-r1,r3,r8-r11,lr}
 ELSE
 stmfd sp!,{r0-r1,r8-r11,lr}
 ENDIF
 ELSE
 IF SC_PROC_HOOK == 1 && SC_PROCMMU_HOOK == 1
 mrc p15,0,r0,c3,c0,0
 stmfd sp!,{r0,r8-r11,lr}
 ELSE
 stmfd sp!,{r8-r11,lr}
 ENDIF
 ENDIF
 msr cpsr_c,#PSR_SYS_MODE|PSR_I_BIT
 str sp,[r2,#PCB_STACKPTR]
 SC_PUBLIC sc_sysProcDispatcher
 IF SC_PROC_HOOK == 1
 IF SC_PROCMMU_HOOK == 1
 mov r0,#-1
 mcr p15,0,r0,c3,c0,0
 ENDIF
 ENDIF
 ldr r7,so_sciopta
 mov r1,#0
 LDR R0,[R7,#SC_PRIOMASK]
 str r1,[r7,#SC_SCHEDULE_REQ]
 IF CPUTYPE >= ARMv5T
 cmp r0,#0
 beq dp_idle
 clz r1,r0
 ELSE
 rsbs r2, r0, #0 
 and r2, r0, r2 
 beq dp_idle
 movs r1, r2, lsr #16 
 movne r1, #16 
 movne r2, r2, lsr #16 
 tst r2, #0xff00 
 addne r1, r1,#8 
 movne r2, r2, lsr #8 
 tst r2, #0xf0 
 addne r1, r1, #4 
 movne r2, r2, lsr #4 
 add r1, r1, r2, lsr #1 
 sub r1, r1, r2, lsr #3 
 ENDIF
 add r2,r7,#SC_READYLIST+dbl_next
 ldr r6,[r2,r1,lsl #3]
#ifdef EVAL_VERSION
 ldr r0,[r7,#-12]
 subs r0,r0,#1
 str r0,[r7,#-12]
 bmi ev_err
#endif
dp2:
 IF SC_PROC_HOOK == 1
 IF SC_PROCSWAP_HOOK == 1
 ldr r13,so_sc_irq_stack
 ldr r2,so_SC_PROCSWAPHOOK
 ldr r2,[r7,r2]
 sub r0,r6,#PCB_PCBLIST
 ldr r1,[r7,#SC_CURPCB]
 cmp r2,#0
 BLXNE_R2_a
 ENDIF
 ENDIF
 ldr r1,[r7,#SC_CURMODULE]
 ldr r0,[r6,#PCB_PID-PCB_PCBLIST]
 sub r2,r6,#PCB_PCBLIST
 sub r6,r6,#PCB_PCBLIST
 stmia r7,{r0,r2,r6} 
 ldr r2,so_sc_modules
 movs r0,r0,lsr #24
 ldr r5,[r2,r0,lsl #2]
 str r5,[r7,#SC_CURMODULE]
 ldr sp,[r6,#PCB_STACKPTR] 
 tst r13,#1 
 bne full_swapin 
 msr cpsr_c,#PSR_SYS_MODE 
 IF CPUTYPE == Xscale
 IF SC_PROC_HOOK == 1 && SC_PROCMMU_HOOK == 1
 ldmfd sp!,{r0-r2,r8-r11,lr} 
 mcr p15,0,r2,c3,c0,0
 ELSE
 ldmfd sp!,{r0-r1,r8-r11,lr} 
 ENDIF
 mar acc0,r0,r1 
 ELSE
 IF SC_PROC_HOOK == 1 && SC_PROCMMU_HOOK == 1
 ldmfd sp!,{r0,r8-r11,lr}
 mcr p15,0,r0,c3,c0,0
 ELSE
 ldmfd sp!,{r8-r11,lr}
 ENDIF
 ENDIF
 mov r0,#1 
 bx lr 
 SC_PUBLIC full_swapin
 bic r12,r13,#1 
 IF CPUTYPE == Xscale
 ldmfd r12!,{r2-r11,r13-r14} 
 mar acc0,r13,r14 
 IF SC_PROC_HOOK == 1 && SC_PROCMMU_HOOK == 1
 ldmfd r12!,{r0}
 mcr p15,0,r0,c3,c0,0
 ENDIF
 ELSE
 IF SC_PROC_HOOK == 1 && SC_PROCMMU_HOOK == 1
 ldmfd r12!,{r2-r11,r13} 
 mcr p15,0,r13,c3,c0,0
 ELSE
 ldmfd r12!,{r2-r11} 
 ENDIF
 ENDIF
 add sp,r12,#6*4 
 msr cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT 
 msr spsr_cxsf,r2 
 mov r14,r3 
 ldmia r12,{r0-r3,r12,lr}^ 
 nop 
 movs pc,lr 
dp_idle: 
 ldr r4,[r7,#SC_CURMODULE]
 cmp r4,#0
 beq dp_idle3
dp_idle2: 
 ldr r4,[r4,#MODULE_PCBS]
 ldr r6,[r4,#0]
 ldrh r0,[r6,#PCB_FLAGS_H]
 tst r0,#PCB_FLG_IS_IDLE
 addne r6,r6,#PCB_PCBLIST
 bne dp2
dp_idle3:
 ldr r4,so_sc_modules
 ldr r4,[r4,#0]
 b dp_idle2
#ifdef EVAL_VERSION
ev_err:
 ldr r0,dp_overrun
 mov r1,#0
 ldr r2,=sc_sysError
 bx r2
#endif
 IF SC_STACK_CHECK == 1
dp_err:
 ldr r0,dp_overrun
dp_err_cont:
 movs r1,r2
 ldr r2,dp_sc_sysError
 bx r2
dp_err2: 
 ldr r0,dp_underrun
 b dp_err_cont
 ENDIF
 LALIGN
 IF SC_PROC_HOOK == 1
 IF SC_PROCMMU_HOOK == 1
dp_55555555:
 LONG 0x55555555
 ENDIF
 ENDIF
 IF SC_STACK_CHECK == 1
dp_sc_sysError:
 LONG sc_sysError
dp_stackmagic: 
 LONG 0x17171717
dp_overrun:
 LONG SC_DISPATCHER|KERNEL_ESTACK_OVERFLOW|SC_ERR_MODULE_FATAL
dp_underrun:
 LONG SC_DISPATCHER|KERNEL_ESTACK_UNDERFLOW|SC_ERR_MODULE_FATAL
 ENDIF
so_sciopta:
 LONG sciopta
so_sc_modules:
 LONG sc_modules
 IF SC_PROC_HOOK == 1
so_sc_irq_stack:
 LONG sc_irq_stack
 IF SC_PROCSWAP_HOOK == 1
so_SC_PROCSWAPHOOK:
 LONG SC_PROCSWAPHOOK
 ENDIF
 IF SC_PROCMMU_HOOK == 1
so_SC_PROCMMUHOOK:
 LONG SC_PROCMMUHOOK
 ENDIF 
 ENDIF
 SC_ENDFUNC swap_out
 ELSE
 SC_TFUNC swap_out
 mrs r1,PRIMASK 
 ldr r3,so_sciopta
 movs r0,#0
 tst r1,#1
 itt ne
 strne r1,[r3,#SC_SCHEDULE_REQ]
 bxne lr
 ldr r1,[r3,#SC_SCHEDULER_LOCK]
 cmp r1,#0
 itt ne
 strne r1,[r3,#SC_SCHEDULE_REQ]
 bxne lr
 ldr r2,[r3,#SC_CURPCB]
 ldrh r1,[r2,#PCB_FLAGS_H]
 tst r1,#PCB_FLG_IS_PRIO|PCB_FLG_IS_IDLE
 itt eq
 streq r3,[r3,#SC_SCHEDULE_REQ]
 bxeq lr
 IF SC_STACK_CHECK == 1
 SC_PUBLIC dp_stackcheck
 ldr r1,dp_stackmagic 
 ldr r0,[r2,#PCB_STACKBTM]
 ldr r3,[r2,#PCB_STACKTOP]
 ldr r4,[r0,#0] 
 cmp r4,r1 
 bne dp_err
 ldr r4,[r3,#0] 
 cmp r4,r1
 bne dp_err
 cmp sp,r0 
 blt dp_err 
 cmp sp,r3 
 bgt dp_err2 
 ENDIF
#ifdef __IAR5__
 ldr r0,=0xe000ed04 
#else
 movw r0,#0xed04 
 movt r0,#0xe000
#endif
 mov r1,#0x10000000
 str r1,[r0,#0] 
 nop
 movs r0,#1
 bx lr
 SC_PUBLIC sc_sysPendSV
 cpsid i
 cbz r2,no_save
 push.w {r4-r11,lr} 
 mov r0,sp
 str r0,[r2,#PCB_STACKPTR]
 SC_PUBLIC sc_sysProcDispatcher
no_save:
 ldr r7,so_sciopta
 movs r1,#0
 LDR R0,[R7,#SC_PRIOMASK]
 str r1,[r7,#SC_SCHEDULE_REQ]
 cbz r0,dp_idle
 clz r1,r0
 adds r2,r7,#SC_READYLIST+dbl_next
 ldr r6,[r2,r1,lsl #3]
dp2:
 IF SC_PROC_HOOK == 1
 IF SC_PROCSWAP_HOOK == 1
 movw r3,#SC_PROCSWAPHOOK
 ldr r2,[r7,r3]
 subs r0,r6,#PCB_PCBLIST
 cbz r2,dp2a
 ldr r3,=sc_svc_stack
 ldr r1,[r7,#SC_CURPCB]
 mov sp,r3
 blx r2
dp2a: 
 ENDIF
 ENDIF
 ldr r1,[r7,#SC_CURMODULE]
 ldr r0,[r6,#PCB_PID-PCB_PCBLIST]
 subs r2,r6,#PCB_PCBLIST
 subs r6,r6,#PCB_PCBLIST
 stmia r7,{r0,r2,r6} 
 ldr r2,so_sc_modules
 lsrs r0,r0,#24
 ldr r5,[r2,r0,lsl #2]
 str r5,[r7,#SC_CURMODULE]
 movs r0,#1 
 ldr r1,[r6,#PCB_STACKPTR] 
 mov sp,r1
 pop.w {r4-r11,lr}
 cpsie i 
 bx lr 
dp_idle: 
 ldr r4,[r7,#SC_CURMODULE]
 cbz r4,dp_idle3
dp_idle2: 
 ldr r4,[r4,#MODULE_PCBS]
 ldr r6,[r4,#0]
 ldrh r0,[r6,#PCB_FLAGS_H]
 tst r0,#PCB_FLG_IS_IDLE
 it ne
 addne r6,r6,#PCB_PCBLIST
 bne dp2
dp_idle3:
 ldr r4,so_sc_modules
 ldr r4,[r4,#0]
 b dp_idle2
 IF SC_STACK_CHECK == 1
dp_err:
 ldr r0,dp_overrun
dp_err_cont:
 movs r1,r2
 ldr r2,dp_sc_sysError
 bx r2
dp_err2: 
 ldr r0,dp_underrun
 b dp_err_cont
 ENDIF
 LALIGN
 IF SC_STACK_CHECK == 1
dp_sc_sysError:
 LONG sc_sysError
dp_stackmagic: 
 LONG 0x17171717
dp_overrun:
 LONG SC_DISPATCHER|KERNEL_ESTACK_OVERFLOW|SC_ERR_MODULE_FATAL
dp_underrun:
 LONG SC_DISPATCHER|KERNEL_ESTACK_UNDERFLOW|SC_ERR_MODULE_FATAL
 ENDIF
so_sciopta:
 LONG sciopta
so_sc_modules:
 LONG sc_modules
 SC_ENDFUNC swap_out
 ENDIF
 IF SC_POOL_HOOK == 1
 XSC_TFUNC sc_poolHookRegister,-8
 SC_TFUNC sc_poolHookRegister
 ldr r3,plhr_sciopta
 ldr r2,[r3,#SC_CURMODULE]
 cmp r2,#0 
 beq plhr1 
 IF SC_POOLKILL_HOOK == 1
 movs r3,#MODULE_POOLKILLHOOK
 cmp r0,#SC_SET_POOLKILL_HOOK
 beq plhr2
 ENDIF
 IF SC_POOLCREATE_HOOK == 1
 movs r3,#MODULE_POOLCREATEHOOK
 cmp r0,#SC_SET_POOLCREATE_HOOK
 beq plhr2
 ENDIF
 IF SC_POOL_PARA_CHECK == 1
 movs r1,r0
 ldr r0,plhr_illval1
 b plhr_err_cont 
 ENDIF
plhr1: 
 IF SC_POOLKILL_HOOK == 1
 ldr r2,plhr_SC_POOLKILLHOOK
 cmp r0,#SC_SET_POOLKILL_HOOK
 beq plhr2
 ENDIF
 IF SC_POOLCREATE_HOOK == 1
 ldr r2,plhr_SC_POOLCREATEHOOK
 cmp r0,#SC_SET_POOLCREATE_HOOK
 beq plhr2
 ENDIF
 IF SC_POOL_PARA_CHECK == 1
 movs r1,r0
 ldr r0,plhr_illval2
plhr_err_cont: 
 bl sc_sysError
 ELSE
 movs r0,#0
 bx lr
 ENDIF
plhr2:
 ldr r0,[r2,r3]
 str r1,[r2,r3]
 bx lr
 LALIGN
plhr_sciopta:
 LONG sciopta
 IF SC_POOLKILL_HOOK == 1
plhr_SC_POOLKILLHOOK:
 LONG SC_POOLKILLHOOK
 ENDIF
 IF SC_POOLCREATE_HOOK == 1
plhr_SC_POOLCREATEHOOK:
 LONG SC_POOLCREATEHOOK
 ENDIF
 IF SC_POOL_PARA_CHECK == 1
plhr_illval1:
 LONG SC_POOLHOOKREGISTER|KERNEL_EILL_VALUE|SC_ERR_MODULE_FATAL
plhr_illval2:
 LONG SC_POOLHOOKREGISTER|KERNEL_EILL_VALUE|SC_ERR_SYSTEM_FATAL
 ENDIF
 SC_ENDFUNC sc_poolHookRegister
 ENDIF
 XSC_TFUNC sc_poolDefault,-12
 SC_TFUNC sc_poolDefault
 ldr r3,pd_sciopta 
 adds r1,r0,#1 
 ldr r2,[r3,#SC_CURPCB] 
 ldr r3,[r3,#SC_CURMODULE] 
 beq pd9 
 bmi pd_err 
 ldr r1,[r3,#MODULE_MAX_POOLS] 
 cmp r0,r1 
 bge pd_err 
 ldr r1,[r3,#MODULE_POOL] 
 lsls r0,r0,#2 
 ldr r1,[r1,r0] 
 lsrs r0,r0,#2 
 cmp r1,#1 
 bls pd_err 
 ldrh r1,[r2,#PCB_DEFAULTPOOL_H] 
 strh r0,[r2,#PCB_DEFAULTPOOL_H] 
 movs r0,r1
 bx lr
pd9:
 ldrh r0,[r2,#PCB_DEFAULTPOOL_H] 
 bx lr
pd_err:
 movs r1,r0 
 ldrh r0,[r2,#PCB_DEFAULTPOOL_H]
 push {r0,lr} 
 ldr r0,poold_lit0 
 bl sc_sysError 
 RET r0 
 LALIGN
pd_sciopta:
 LONG sciopta
poold_lit0:
 LONG SC_POOLDEFAULT|KERNEL_EILL_POOL_ID
 SC_ENDFUNC sc_poolDefault
 XSC_TFUNC sc_poolInfo,-4
 SC_TFUNC sc_poolInfo
 IF SC_POOL_PARA_CHECK == 1
 lsrs r0,r0,#24 
 cmp r2,#0
 beq pi_err 
 lsrs r3,r1,#7 
 beq pi0 
 lsls r3,r1,#1 
 lsrs r3,r3,#25
 cmp r0,r3
 bne pi_ret0
pi0:
 ELSE
 lsrs r0,r0,#24 
 ENDIF
 mov r12,r2 
 ldr r3,pi_sciopta 
 cmp r0,#SC_MAX_MODULE 
 bge pi_err1 
 ldr r2,pi_sc_modules 
 lsls r0,r0,#2 
 ldr r2,[r2,r0] 
 cmp r2,#1 
 bls pi_ret0 
 ldr r3,[r2,#MODULE_MAX_POOLS] 
 lsls r0,r1,#8 
 lsrs r0,r0,#8
 cmp r0,r3
 bge pi_err2 
 ldr r1,[r2,#MODULE_POOL] 
 lsls r0,r0,#2 
 ldr r1,[r1,r0] 
 cmp r1,#1 
 bls pi_ret0 
 mov r0,r12 
 movs r2,#SIZEOF_POOL_CB>>2 
pi1:
 subs r2,r2,#1 
 ldmia r1!,{r3}
 stmia r0!,{r3}
 bne pi1
 movs r0,#1 
 bx lr
pi_ret0:
 movs r0,#0
 bx lr
pi_err2:
 ldr r0,pi_lit2
 b pi_err_cont
pi_err1:
 lsls r1,r0,#24
 ldr r0,pi_lit1
pi_err_cont:
 bl sc_sysError
 IF SC_POOL_PARA_CHECK == 1
pi_err:
 movs r1,#0
 ldr r0,pi_lit0
 b pi_err_cont
 ENDIF
 LALIGN
pi_sciopta:
 LONG sciopta
pi_sc_modules:
 LONG sc_modules
pi_lit2:
 LONG KERNEL_EILL_POOL_ID|SC_POOLINFO|SC_ERR_MODULE_FATAL
pi_lit1:
 LONG KERNEL_EILL_MODULE|SC_POOLINFO|SC_ERR_MODULE_FATAL
 IF SC_POOL_PARA_CHECK == 1
pi_lit0:
 LONG KERNEL_ENIL_PTR|SC_POOLINFO|SC_ERR_PROCESS_FATAL
 ENDIF
 SC_ENDFUNC sc_poolInfo
 XSC_TFUNC sc_poolIdGet,-12
 SC_TFUNC sc_poolIdGet
 ldr r3,plig_sciopta
 cmp r0,#0
 bne plig1
plig0:
 ldr r0,[r3,#SC_CURPCB]
 ldrh r0,[r0,#PCB_DEFAULTPOOL_H]
 bx lr
plig1:
 ldrb r1,[r0,#0]
 cmp r1,#0
 beq plig0
 IF SC_POOL_PARA_CHECK == 1
 movs r1,#SC_POOL_NAME_SIZE
plig2:
 ldrb r2,[r0,r1]
 cmp r2,#0
 beq plig3
 subs r1,r1,#1
 bpl plig2
 movs r1,r0
 ldr r0,plig_lit0
 bl sc_sysError
 LALIGN
plig_lit0:
 LONG SC_POOLIDGET|KERNEL_EILL_NAME|SC_ERR_MODULE_FATAL
 ENDIF
plig3:
 ldr r3,[r3,#SC_CURMODULE]
 ldr r2,[r3,#MODULE_MAX_POOLS]
 ldr r3,[r3,#MODULE_POOL]
 push {r4-r6}
 mov r12,r0
plig4:
 ldmia r3!,{r1}
 cmp r1,#1
 bls plig7
 ldr r0,[r1,#POOL_ID]
 IF POOL_NAME > 255
 adds r1,r1,#POOL_NAME-255
 adds r1,r1,#255
 ELSE
 adds r1,r1,#POOL_NAME
 ENDIF
 lsls r0,r0,#8
 lsrs r0,r0,#8
 mov r4,r12
plig5:
 ldrb r5,[r1,#0]
 adds r1,r1,#1
 ldrb r6,[r4,#0]
 adds r4,r4,#1
 cmp r5,r6
 bne plig7
 cmp r5,#0
 bne plig5
plig6: 
 pop {r4-r6}
 bx lr
plig7:
 subs r2,r2,#1
 bne plig4
 movs r0,#-SC_ILLEGAL_POOLID
 NEG(r0)
 b plig6
 LALIGN
plig_sciopta:
 LONG sciopta
plig_sc_modules:
 LONG sc_modules
 SC_ENDFUNC sc_poolIdGet
 XSC_TFUNC sc_poolKill,-12
 SC_TFUNC sc_poolKill
 ldr r1,sspok_sciopta
 ldr r1,[r1,#SC_CURMODULE]
 ldr r1,[r1,#MODULE_ID]
 SC_TPUBLIC sc_sysPoolKill
 push {r4-r5,lr}
 ldr r3,sspok_sciopta
 ldr r4,sspok_sc_modules
 lsrs r2,r1,#24
 cmp r2,#SC_MAX_MODULE
 bge sspok_err
 lsls r2,r2,#2
 LOCK
 ldr r4,[r4,r2]
 cmp r4,#0
 beq sspok_err0
 ldr r3,[r4,#MODULE_MAX_POOLS]
 ldr r2,[r4,#MODULE_POOL]
 cmp r0,r3
 bge sspok_err1
 lsls r3,r0,#2
 ldr r5,[r2,r3]
 cmp r5,#1
 bls sspok_err1
 ldr r1,[r5,#POOL_LOCK]
 cmp r1,#0
 bne sspok_err2
 movs r1,#1
 str r1,[r2,r3]
 UNLOCK
 bl sysPoolKill
 RET r4-r5
sspok_err2:
 movs r1,r0
 ldr r0,sspok_lit2
 b sspok_err_cont1
sspok_err1:
 movs r1,r0
 ldr r0,sspok_lit1
sspok_err_cont1: 
 UNLOCK
 b sspok_err_cont
sspok_err0:
 UNLOCK
sspok_err:
 ldr r0,sspok_lit0
sspok_err_cont: 
 bl sc_sysError
 LALIGN
sspok_sciopta:
 LONG sciopta
sspok_sc_modules:
 LONG sc_modules
sspok_lit0:
 LONG KERNEL_EILL_MODULE|SC_SYSPOOLKILL|SC_ERR_MODULE_FATAL
sspok_lit1:
 LONG KERNEL_EILL_POOL_ID|SC_POOLKILL|SC_ERR_MODULE_FATAL
sspok_lit2:
 LONG KERNEL_EPOOL_IN_USE|SC_POOLKILL|SC_ERR_MODULE_FATAL
 SC_ENDFUNC sc_poolKill
 SC_TFUNC sysPoolKill
 push {lr}
 cmp r5,#1
 bls spok9
 IF SC_POOL_HOOK == 1
 IF SC_POOLKILL_HOOK == 1
 ldr r2,[r4,#MODULE_POOLKILLHOOK]
 cmp r2,#0
 beq spok1
 movs r0,r5
 BLX_R2_t
spok1:
 ldr r2,spok_SC_POOLKILLHOOK
 ldr r2,[r2,#0]
 cmp r2,#0
 beq spok2
 movs r0,r5
 BLX_R2_t
spok2:
 ENDIF
 ENDIF
 ldr r1,[r5,#POOL_ID]
 IF SC_CDEBUG == 1
 ldr r3,spok_lit0
 ldr r0,[r5,#POOL_SIZE]
 lsrs r0,r0,#2
 adds r0,r0,#SIZEOF_POOL_CB>>2
spok4:
 subs r0,r0,#1
 stmia r5!,{r3}
 bne spok4
 ELSE
 IF SC_MSG_STAT == 1
 adds r5,r5,#POOL_STAT
 movs r3,#0
 movs r0,#SIZEOF_SC_POOL_STAT>>2
spok3:
 subs r0,r0,#1
 stmia r5!,{r3}
 bne spok3
 ENDIF
 ENDIF
 ldr r2,[r4,#MODULE_POOL]
 movs r3,#0
 lsls r1,r1,#24
 lsrs r1,r1,#22
 LOCK
 str r3,[r2,r1]
 ldr r0,[r4,#MODULE_NPOOLS]
 subs r0,r0,#1
 str r0,[r4,#MODULE_NPOOLS]
 UNLOCK
spok9:
 EXIT
 LALIGN
 IF SC_POOL_HOOK == 1
 IF SC_POOLKILL_HOOK == 1
spok_SC_POOLKILLHOOK:
 LONG sciopta+SC_POOLKILLHOOK
 ENDIF
 ENDIF
 IF SC_CDEBUG == 1
spok_lit0:
 LONG 0xdddddddd
 ENDIF
 SC_ENDFUNC sysPoolKill
#define PARA_START 0
#define PARA_SIZE 4
#define PARA_NBUFSIZE 8
#define PARA_BUFSIZE 12
#define PARA_NAME 36
#define PARA_MID 40
 XSC_TFUNC sc_sysPoolCreate,8
 XSC_TFUNC sc_poolCreate,0
 SC_TFUNC sc_poolCreate
 mov r12,r0
 ldr r0,spoc_sciopta
 ldr r0,[r0,#SC_CURMODULE]
 ldr r0,[r0,#MODULE_ID]
 push {r0,lr}
 ldr r0,[sp,#8]
 push {r0}
 mov r0,r12
 bl sc_sysPoolCreate
 add sp,sp,#8
 EXIT
 SC_TPUBLIC sc_sysPoolCreate
 push {r0-r7,lr}
 cmp r2,#SC_MAX_NUM_BUFFERSIZES
 bhi spoc_err0
 cmp r2,#16
 beq spoc1
 cmp r2,#8
 beq spoc1
 cmp r2,#4
 beq spoc1 
spoc_err0:
 movs r1,r2
 movs r2,#KERNEL_EILL_NUM_SIZES>>12
spoc_cont:
 lsls r2,r2,#12
 movs r0,#SC_ERR_MODULE_FATAL
 orrs r0,r0,r2
 movs r2,#SC_POOLCREATE>>24
 lsls r2,r2,#24
 orrs r0,r0,r2
 bl sc_sysError
spoc_err1a:
 movs r6,r7
spoc_err1:
 movs r2,#KERNEL_EILL_BUF_SIZES>>12
 movs r1,r6
 b spoc_cont
spoc_err2:
 movs r2,#KERNEL_EILL_POOL_SIZE>>12
 b spoc_cont
spoc_err3:
 movs r2,#KERNEL_EILL_NAME>>12
 ldr r1,[sp,#PARA_NAME]
 b spoc_cont
spoc_err4:
 movs r2,#KERNEL_ENO_MORE_POOL>>12
spoc_cont1:
 UNLOCK
 b spoc_cont
spoc_err5:
 movs r2,#KERNEL_EOUT_OF_MEMORY>>12
 b spoc_cont1
spoc_err6:
 movs r1,r5
 movs r2,#KERNEL_EILL_VALUE>>12
 b spoc_cont1
spoc_err7:
 movs r2,#KERNEL_EILL_VALUE>>12
 b spoc_cont
spoc_ill_module:
 movs r2,#KERNEL_EILL_MODULE>>12
 b spoc_cont
spoc1:
 movs r4,r3
 subs r5,r2,#1
 ldr r6,[r4,#0]
spoc2:
 ldr r7,[r4,#4]
 cmp r6,r7
 bhi spoc_err1
 lsls r0,r6,#30 
 bne spoc_err1
 movs r6,r7
 adds r4,r4,#4
 subs r5,r5,#1
 bne spoc2
 lsls r4,r7,#30 
 bne spoc_err1a
 lsls r4,r1,#30
 bne spoc_err7
 subs r6,r1,r7 
 IF SIZEOF_POOL_CB > 255
 IF SIZEOF_POOL_CB > 510
 subs r6,r6,#SIZEOF_POOL_CB-510
 subs r6,r6,#255
 ELSE 
 subs r6,r6,#SIZEOF_POOL_CB-255
 ENDIF
 subs r6,r6,#255
 ELSE
 subs r6,r6,#SIZEOF_POOL_CB
 ENDIF
 subs r6,r6,#MSG_HEAD
 IF SC_MSG_CHECK == 1
 subs r6,r6,#4
 ENDIF
 bmi spoc_err2
 ldr r4,[sp,#PARA_NAME]
 cmp r4,#0
 beq spoc_err3
 movs r5,#SC_POOL_NAME_SIZE+1
 NEG(r5)
spoc3:
 ldrb r6,[r4,#0]
 adds r4,r4,#1
 cmp r6,#0
 beq spoc4
 adds r5,r5,#1
 bne spoc3
 b spoc_err3
spoc4:
 adds r5,r5,#SC_POOL_NAME_SIZE+1
 beq spoc_err3
 ldr r1,[sp,#PARA_MID]
 lsrs r6,r1,#24
 cmp r6,#SC_MAX_MODULE
 bhs spoc_ill_module
 ldr r7,spoc_sc_modules
 lsls r6,r6,#2 
 ldr r6,[r7,r6]
 cmp r6,#0
 beq spoc_ill_module 
 ldr r5,[r6,#MODULE_NPOOLS]
 cmp r5,#0
 beq spoc8
 ldr r4,[r6,#MODULE_POOL]
spoc5:
 ldr r7,[r4,#0]
 adds r4,r4,#4
 cmp r7,#0
 beq spoc5
 cmp r7,#1
 beq spoc7
 IF POOL_NAME > 255
 adds r7,r7,#POOL_NAME-255
 adds r7,r7,#255
 ELSE
 adds r7,r7,#POOL_NAME
 ENDIF
 ldr r3,[sp,#PARA_NAME]
spoc6:
 ldrb r2,[r7,#0]
 ldrb r1,[r3,#0]
 adds r7,r7,#1
 adds r3,r3,#1
 cmp r2,r1
 bne spoc7
 cmp r2,#0
 bne spoc6
 b spoc_err3 
spoc7:
 subs r5,r5,#1
 bne spoc5
spoc8:
 LOCK
 ldr r1,[r6,#MODULE_NPOOLS]
 ldr r4,[r6,#MODULE_MAX_POOLS]
 cmp r1,r4
 beq spoc_err4
 adds r1,r1,#1
 ldr r4,[r6,#MODULE_POOL]
 str r1,[r6,#MODULE_NPOOLS]
 subs r7,r4,#4
spoc9:
 ldr r0,[r7,#4]
 adds r7,r7,#4
 cmp r0,#0
 beq spoc10
 subs r1,r1,#1
 bne spoc9
spoc10:
 subs r4,r4,r7
 NEG(r4)
 lsrs r4,r4,#2 
 ldr r5,[sp,#PARA_START+4]
 cmp r5,#0
 bne spoc10a
 ldr r0,[r6,#MODULE_FREESIZE]
 ldr r5,[r6,#MODULE_DATAPTR]
 ldr r1,[sp,#PARA_SIZE+4]
 cmp r1,r0
 IF CPUTYPE > ARMv5TE
 bhi.n spoc_err5 
 ELSE
 bhi spoc_err5 
 ENDIF
 subs r0,r0,r1
 str r0,[r6,#MODULE_FREESIZE]
 adds r1,r1,r5
 str r1,[r6,#MODULE_DATAPTR]
 b spoc11
spoc10a:
 lsls r0,r5,#30
 IF CPUTYPE > ARMv5TE
 bne.n spoc_err6 
 ELSE
 bne spoc_err6 
 ENDIF
spoc11:
 movs r0,#1
 str r0,[r7,#0]
 UNLOCK
 movs r0,r5
 movs r1,#SIZEOF_POOL_CB>>2
 movs r2,#0
spoc12:
 str r2,[r0,#0]
 adds r0,r0,#4
 subs r1,r1,#1
 bne spoc12
 ldr r0,[r6,#MODULE_ID]
 lsrs r0,r0,#24
 bne spoc13
 movs r0,#0x80
spoc13:
 lsls r0,r0,#24
 orrs r4,r4,r0
 str r4,[r5,#POOL_ID]
 ldr r0,spoc_sciopta
 ldr r0,[r0,#SC_CURPID]
 str r0,[r5,#POOL_CREATOR]
 ldr r0,[sp,#PARA_SIZE]
 adds r2,r5,r0 
 IF SIZEOF_POOL_CB > 255
 IF SIZEOF_POOL_CB > 510
 movs r1,#SIZEOF_POOL_CB-510
 adds r1,r1,#255
 ELSE
 movs r1,#SIZEOF_POOL_CB-255
 ENDIF 
 adds r1,r1,#255
 ELSE
 movs r1,#SIZEOF_POOL_CB
 ENDIF 
 subs r0,r0,r1
 str r0,[r5,#POOL_SIZE]
 adds r1,r1,r5
 str r1,[r5,#POOL_START]
 str r1,[r5,#POOL_CUR]
 str r2,[r5,#POOL_END]
 ldr r4,[sp,#PARA_NBUFSIZE]
 movs r0,r4
 str r4,[r5,#POOL_NBUFSIZES]
 ldr r1,[sp,#PARA_BUFSIZE]
 movs r2,r5
 adds r2,r2,#POOL_BUFSIZE
spoc14:
 ldmia r1!,{r3}
 stmia r2!,{r3}
 subs r0,r0,#1
 bne spoc14
 IF SC_MAX_NUM_BUFFERSIZES == 16
 cmp r4,#4
 bne spoc14b
 str r3,[r2,#12]
spoc14b:
 ENDIF
 movs r0,r4
 movs r1,r5
 adds r1,r1,#POOL_FREED
 movs r2,r5
 adds r2,r2,#POOL_WAITER
spoc15:
 str r1,[r1,#0]
 str r1,[r1,#4]
 str r2,[r2,#0]
 str r2,[r2,#4]
 adds r1,r1,#8
 adds r2,r2,#8
 subs r0,r0,#1
 bne spoc15
 movs r3,r5
 IF POOL_NAME > 255
 adds r3,r3,#POOL_NAME-255
 adds r3,r3,#254
 ELSE
 adds r3,r3,#POOL_NAME-1
 ENDIF
 ldr r0,[sp,#PARA_NAME]
spoc15a:
 ldrb r2,[r0,#0]
 adds r0,r0,#1
 adds r3,r3,#1
 strb r2,[r3,#0]
 cmp r2,#0
 bne spoc15a
 IF SC_MSG_CHECK == 1
 ldr r0,[r5,#POOL_START]
 movs r2,#0xcc
 lsls r1,r2,#8
 orrs r2,r2,r1
 lsls r1,r2,#16
 orrs r2,r2,r1
 ldr r1,[r5,#POOL_SIZE]
spoc16:
 str r2,[r0,#0]
 adds r0,r0,#4
 subs r1,r1,#4
 bne spoc16
 ldr r0,[r5,#POOL_CUR]
 movs r2,#0xee
 lsls r1,r2,#8
 orrs r2,r2,r1
 lsls r1,r2,#16
 orrs r2,r2,r1
 str r2,[r0,#0]
 adds r0,r0,#4
 str r0,[r5,#POOL_CUR]
 ENDIF
 IF SC_POOL_HOOK == 1
 IF SC_POOLCREATE_HOOK == 1
 ldr r2,[r6,#MODULE_POOLCREATEHOOK]
 cmp r2,#0
 beq spoc17
 movs r0,r5
 BLX_R2_t
spoc17:
 ldr r0,spoc_SC_POOLCREATEHOOK
 ldr r2,[r0,#0]
 cmp r2,#0
 beq spoc18
 movs r0,r5
 BLX_R2_t
spoc18: 
 ENDIF
 ENDIF
 str r5,[r7,#0]
 add sp,sp,#4*4
 ldr r0,[r5,#POOL_ID]
 lsls r0,r0,#8
 lsrs r0,r0,#8
 RET r4-r7
 LALIGN
spoc_sciopta:
 LONG sciopta
spoc_sc_modules:
 LONG sc_modules
 IF SC_POOL_HOOK == 1
 IF SC_POOLCREATE_HOOK == 1
spoc_SC_POOLCREATEHOOK:
 LONG sciopta+SC_POOLCREATEHOOK
 ENDIF
 ENDIF
 SC_ENDFUNC sc_poolCreate
#undef PARA_START
#undef PARA_SIZE
#undef PARA_NBUFSIZE
#undef PARA_BUFSIZE
#undef PARA_NAME
#undef PARA_MID
 XSC_TFUNC sc_poolReset,-12
 SC_TFUNC sc_poolReset
 push {lr}
 ldr r3,por_sciopta 
 ldr r3,[r3,#SC_CURMODULE] 
 ldr r2,[r3,#MODULE_MAX_POOLS] 
 ldr r1,[r3,#MODULE_POOL] 
 cmp r0,r2 
 bge por_err 
 lsls r2,r0,#2 
 adds r2,r2,r1 
 LOCK
 ldr r3,[r2,#0] 
 cmp r3,#1 
 bls por_err0 
 ldr r1,[r3,#POOL_LOCK] 
 cmp r1,#0 
 bne por_err1 
 movs r1,#1
 str r1,[r2,#0] 
 UNLOCK
 mov r12,r2 
 IF SC_MSG_STAT == 1
 movs r0,#POOL_STAT>>2 
 lsls r0,r0,#2
 adds r0,r0,r3
 movs r1,#0
 movs r2,#SIZEOF_SC_POOL_STAT>>2
por1:
 subs r2,r2,#1
 stmia r0!,{r1}
 bne por1
 ENDIF
 IF SC_CDEBUG == 1
 ldr r0,[r3,#POOL_START] 
 ldr r2,[r3,#POOL_SIZE]
 ldr r1,por_virgin
por2:
 subs r2,r2,#4
 stmia r0!,{r1}
 bne por2
 ENDIF
 ldr r2,[r3,#POOL_NBUFSIZES] 
 movs r0,r3
 adds r0,r0,#POOL_FREED
 movs r1,r3
 adds r1,r1,#POOL_WAITER
por3: 
 str r0,[r0,#0]
 str r0,[r0,#4]
 str r1,[r1,#0]
 str r1,[r1,#4]
 adds r0,r0,#8
 adds r1,r1,#8
 subs r2,r2,#1
 bne por3
 ldr r0,[r3,#POOL_START]
 mov r2,r12
 IF SC_MSG_CHECK == 1
 ldr r1,por_endmark
 stmia r0!,{r1}
 ENDIF
 str r0,[r3,#POOL_CUR]
 str r3,[r2,#0]
 EXIT
por_err1:
 UNLOCK
 ldr r0,por_lit1
 ldr r1,[r3,#POOL_ID]
 b por_err_cont1
por_err0:
 UNLOCK
por_err:
 movs r1,r0
 ldr r0,por_lit0
por_err_cont1:
 bl sc_sysError
 LALIGN
por_sciopta:
 LONG sciopta
por_lit0:
 LONG KERNEL_EILL_POOL_ID|SC_POOLRESET|SC_ERR_MODULE_FATAL
por_lit1:
 LONG KERNEL_EPOOL_IN_USE|SC_POOLRESET|SC_ERR_MODULE_FATAL
 IF SC_CDEBUG == 1
por_virgin:
 LONG 0xcccccccc
 ENDIF
 IF SC_MSG_CHECK == 1
por_endmark:
 LONG 0xeeeeeeee
 ENDIF
 SC_ENDFUNC sc_poolReset
#define U32_MAX_BY_1000 4294967
 XSC_TFUNC sc_tickTick2Ms,-12
 SC_TFUNC sc_tickTick2Ms
 IF CPUTYPE <= Xscale
 bx pc
 nop
 CODE32
 ldr r12,tt2m_sciopta
 ldr r2,[r12,#SC_TCKLENGTH-256*8] 
 cmp r2,#0
 beq tt2m_err
 cmp r2,#1000 
 bxeq lr 
 cmp r0,#0 
 bxeq lr 
 ldr r12,[r12,#SC_RECI_TCKLENGTH-256*8] 
 movs r1,#-1
 umull r3,r1,r12,r1 
 cmp r0,r1
 bls tt2m0
 ldr r3,tt2m_by_1000 
 adds r0,r0,#500 
 subcs r0,r0,#500 
 umull r12,r0,r3,r0 
 cmp r0,r1
 movhi r0,#-1
 bxhi lr
 mul r0,r2,r0
 bx lr
tt2m0:
 ldr r3,tt2m_by_1000 
 mul r1,r0,r2 
 adds r1,r1,#500 
 umull r1,r0,r3,r1 
 cmp r0,#0
 moveq r0,#1
 bx lr
tt2m_err:
 movs r1,#0
 ldr r0,tt2m_illval
 ldr r3,tt2m_sc_sysError
 bx r3
 LALIGN
tt2m_by_1000:
 LONG 0x00418937 
tt2m_sciopta:
 LONG sciopta+256*8
tt2m_illval:
 LONG SC_TICKTICK2MS|SC_ERR_SYSTEM_FATAL|KERNEL_EILL_VALUE
tt2m_sc_sysError:
 LONG sc_sysError
 ELSE
 ldr r3,=sciopta
 ldr r2,[r3,#SC_TCKLENGTH] 
 cbz r2,tt2m_err0 
 ldr r1,=1000
 cmp r1,r2 
 it eq
 bxeq lr 
 mov r3,#-1
 ldr r1,=1000
 udiv r3,r3,r2 
 cmp r0,r3
 bls tt2m0
 udiv r0,r0,r1
 cmp r0,r3
 ite hi
 movhi r0,#-1
 mulls r0,r0,r2
 b tt2m_exit
tt2m0:
 muls r0,r2,r0
 udiv r0,r0,r1
tt2m_exit: 
 cmp r0,#0
 it eq
 moveq r0,#1
 bx lr
tt2m_err0:
 ldr r0,=SC_TICKTICK2MS|SC_ERR_SYSTEM_FATAL|KERNEL_EILL_VALUE
 movs r1,#0
 bl sc_sysError
 ENDIF
 SC_ENDFUNC sc_tickTick2Ms
 XSC_TFUNC sc_tickMs2Tick,-12
 SC_TFUNC sc_tickMs2Tick
 IF CPUTYPE <= Xscale
 bx pc
 nop
 CODE32
 ldr r3,tm2t_sciopta
 ldr r2,[r3,#SC_RECI_TCKLENGTH-256*8] 
 ldr r12,[r3,#SC_TCKLENGTH-256*8] 
 cmp r12,#0 
 beq tm2t_err0 
 cmp r0,#0 
 bxeq lr 
 cmp r12,#1000 
 bxeq lr 
 ldr r1,tm2t_limit
 cmp r0,r1 
 bls tm2t1
 adds r0,r0,r12,lsr #1 
 umull r0,r3,r2,r0 
 cmp r3,r1 
 movhi r0,#-1 
 bxhi lr
 movs r1,#1000
 mul r0,r3,r1 
 bx lr
tm2t1: 
 movs r1,#1000
 mul r3,r0,r1
 adds r3,r3,r12,lsr #1 
 umull r1,r0,r3,r2 
 cmp r0,#0
 moveq r0,#1
 bx lr
tm2t_err0:
 movs r1,#0
 ldr r0,tm2t_illval
 ldr r3,tm2t_sc_sysError
 bx r3
 LALIGN
tm2t_sciopta:
 LONG sciopta+256*8
tm2t_illval:
 LONG SC_TICKMS2TICK|SC_ERR_SYSTEM_FATAL|KERNEL_EILL_VALUE
tm2t_limit:
 LONG 4294967 
tm2t_sc_sysError:
 LONG sc_sysError
 ELSE
 ldr r3,=sciopta
 ldr r2,[r3,#SC_TCKLENGTH] 
 cbz r2,tm2t_err0 
 ldr r1,=1000
 cmp r1,r2 
 it eq
 bxeq lr 
 ldr r3,=U32_MAX_BY_1000
 cmp r0,r3 
 bls tm2t0 
 udiv r0,r0,r2
 cmp r0,r3 
 ite hi
 movhi r0,#-1
 mulls r0,r1,r0
 b tm2t_exit
tm2t0:
 muls r0,r1,r0
 udiv r0,r0,r2
tm2t_exit:
 cmp r0,#0
 it eq
 moveq r0,#1
 bx lr
tm2t_err0:
 movs r1,#0
 ldr r0,=SC_TICKMS2TICK|SC_ERR_SYSTEM_FATAL|KERNEL_EILL_VALUE
 bl sc_sysError
 ENDIF
 SC_ENDFUNC sc_tickMs2Tick
 XSC_TFUNC sc_tickLength,-12
 SC_TFUNC sc_tickLength
 IF CPUTYPE <= Xscale
 bx pc
 nop
 CODE32
 ldr r3,tl_SC_TCKLENGTH
 movs r1,r0 
 ldr r0,[r3,#0] 
 bxeq lr 
 str r1,[r3,#0] 
 movs r2,r1
 cmp r2,#0x00010000
 movlo r2,r2,lsl #16
 cmp r2,#0x01000000
 movlo r2,r2,lsl #8
 cmp r2,#0x10000000
 movlo r2,r2,lsl #4
 cmp r2,#0x40000000
 movlo r2,r2,lsl #2
 cmp r2,#0x80000000
 movlo r2,r2,lsl #1
 movs r3,#1
 rsc r0,r2,#0
 movs r2,r2,lsr #1
tl1:
 cmp r0,r2
 adc r3,r3,r3
 subcs r0,r0,r2
 cmp r1,r2 
 movne r2,r2,lsr #1
 bne tl1
 ldr r1,tl_sciopta
 cmp r0,#0
 addne r3,r3,#1
 str r3,[r1,#SC_RECI_TCKLENGTH]
 bx lr
 LALIGN
tl_sciopta:
 LONG sciopta
tl_SC_TCKLENGTH:
 LONG sciopta+SC_TCKLENGTH
 ELSE
 ldr r1,=sciopta
 ldr r2,[r1,#SC_TCKLENGTH]
 cbz r0,tl_get
 str r0,[r1,#SC_TCKLENGTH]
tl_get:
 movs r0,r2
 bx lr
 ENDIF
 SC_ENDFUNC sc_tickLength
 XSC_TFUNC sc_tickGet,-16
 SC_TFUNC sc_tickGet
 ldr r3,tg_sciopta
 ldr r0,[r3,#SC_TCK]
 bx lr
 nop
 LALIGN
tg_sciopta:
 LONG sciopta
 SC_ENDFUNC sc_tickGet
 XSC_TFUNC sc_tick,-16
 SC_TFUNC sc_tickx
 IF CPUTYPE <= Xscale
 IF SC_STACK_CHECK == 1
tick_err:
 ldr r0,tick_overrun
tick_err_cont:
 movs r1,r4
 bl sc_sysError
tick_err2: 
 ldr r0,tick_underrun
 b tick_err_cont
 ENDIF
 SC_TPUBLIC sc_tick
 mov r0,r8
 mov r1,r9
 mov r2,r10
 mov r3,r11 
 push {r0-r7,lr}
 ldr r7,tick_sciopta
 ldr r5,[r7,#SC_TCK] 
 adds r5,r5,#1
 str r5,[r7,#SC_TCK]
 IF SC_STACK_CHECK == 1
 ldr r0,[r7,#SC_CURPRIOPCB] 
 ldr r1,tick_stackmagic 
 ldr r2,[r0,#PCB_STACKBTM]
 ldr r3,[r0,#PCB_STACKTOP]
 ldr r4,[r0,#PCB_STACKPTR]
 ldr r0,[r2,#0] 
 cmp r0,r1 
 bne tick_err
 ldr r0,[r3,#0] 
 cmp r0,r1
 bne tick_err
 cmp r4,r2 
 blt tick_err 
 cmp r4,r3 
 bgt tick_err2 
 ENDIF
 ldr r0,[r7,#SC_CURPCB]
 ldr r1,[r7,#SC_CURMODULE]
 push {r0,r1}
 movs r2,#0
 mov r11,r2 
 movs r0,#64
 lsls r0,r0,#3
 mov r10,r0 
 movs r0,#SC_SLEEPERLISTS-255
 adds r0,r0,#255
 adds r0,r0,r7 
 mov r9,r0 
 movs r6,#4
tick0:
 movs r0,#0x3f
 ands r0,r0,r5 
 lsls r0,r0,#3 
 add r0,r9 
tick1:
 LOCK
 ldr r1,[r0,#dbl_next] 
 cmp r0,r1 
 beq tick9 
 dbl_del(r1,r2,r3) 
 subs r1,r1,#PCB_SLEEPERS 
 ldr r2,[r1,#PCB_TMO] 
 cmp r2,#0 
 bne tick6 
 ldrh r2,[r1,#PCB_STATE_H] 
 subs r2,r2,#ST_WAIT_TMO 
 ldrh r3,[r1,#PCB_FLAGS_H] 
 lsrs r3,r3,#PCB_FLG_IS_TIMER_POS+1 
 bcs tick5 
 adds r2,r2,#ST_TMO 
 strh r2,[r1,#PCB_STATE_H] 
 lsrs r4,r2,#ST_WAIT_MSGALLOC_POS+1 
 bcc tick3 
 adds r1,r1,#PCB_PCBLIST 
 dbl_del(r1,r3,r4)
 subs r1,r1,#PCB_PCBLIST
tick3:
 lsrs r2,r2,#ST_STOPPED_POS+1 
 bne tick4 
 makeReady(r1,r2,r3,r4,r7,tick1) 
 ldr r2,[r7,#SC_CURPRIOPCB] 
 ldrb r1,[r1,#PCB_PRIO_B] 
 ldrb r2,[r2,#PCB_PRIO_B] 
 cmp r1,r2
 bhs tick4
 mov r11,r2 
tick4:
 UNLOCK
 b tick1
tick9:
 UNLOCK
 lsls r0,r5,#26 
 bne tick10 
 lsrs r5,r5,#6 
 subs r6,r6,#1
 beq tick10 
 add r9,r10 
 b tick0
 LALIGN
tick5:
 bx pc
 nop
 SC_PUBLIC tick5_a
 strh r2,[r1,#PCB_STATE_H] 
 ldr r3,[r1,#PCB_PID] 
 str r1,[r7,#SC_CURPCB]
 str r3,[r7,#SC_CURPID]
 ldr r2,tick_sc_modules
 lsrs r3,r3,#24
 ldr r2,[r2,r3,lsl #2]
 str r2,[r7,#SC_CURMODULE]
 IF SC_PROC_HOOK == 1
 IF SC_PROCMMU_HOOK == 1
 movs r2,#3
 adds r3,r3,r3
 movs r2,r2,lsl r3
 movne r3,#0x40000001 
 ldreq r3,tick_55555555 
 orr r2,r2,r3
 mrc p15,0,r3,c3,c0,0 
 mcr p15,0,r2,c3,c0,0 
 ENDIF
 ENDIF
 bl cpu_unlock_r12_a 
 mov r2,sp 
 ldr sp,[r1,#PCB_STACKTOP] 
 stmfd sp!,{r0-r3} 
 movs r0,#0
 ldr r2,[r1,#PCB_ENTRY]
 BLX_R2_a 
 ldmfd sp!,{r0-r3}
 mov sp,r2 
 bl cpu_lock_r12_a 
 IF SC_PROC_HOOK == 1
 IF SC_PROCMMU_HOOK == 1
 mcr p15,0,r3,c3,c0,0 
 ENDIF
 ENDIF
 add r3,pc,#1
 bx r3
 SC_TPUBLIC tick_t
 ldr r2,[r1,#PCB_SLICE] 
 ldrh r3,[r1,#PCB_FLAGS_H] 
 lsrs r3,r3,#PCB_FLG_READY_POS+1 
 bcc tick7 
tick6:
 enterTimer(r1,r2,r3,r4,r7,tick5) 
 ldrh r3,[r1,#PCB_STATE_H] 
 movs r2,#ST_WAIT_TMO
 orrs r3,r3,r2
 strh r3,[r1,#PCB_STATE_H]
tick7:
 UNLOCK
 b tick1 
tick10:
 pop {r0,r1} 
 LOCK
 ldr r2,[r0,#PCB_PID]
 str r1,[r7,#SC_CURMODULE]
 str r0,[r7,#SC_CURPCB]
 str r2,[r7,#SC_CURPID]
 UNLOCK
 ldr r0,[r7,#SC_CURPRIOPCB] 
 ldr r3,[r0,#PCB_SLICE] 
 ldr r1,[r0,#PCB_CURSLICE] 
 cmp r3,#0 
 beq tick20 
 subs r1,r1,#1 
 bne tick19 
 movs r1,r3 
 LOCK
 ldr r2,[r0,#PCB_PCBLIST+dbl_prev] 
 ldr r3,[r0,#PCB_PCBLIST+dbl_next] 
 cmp r2,r3 
 beq tick18 
 str r3,[r2,#dbl_next] 
 str r2,[r3,#dbl_prev]
 movs r4,r0 
 adds r4,r4,#PCB_PCBLIST
 dbl_tailAdd(r2,r4,r3)
 mov r11,r2 
tick18:
 UNLOCK
tick19:
 str r1,[r0,#PCB_CURSLICE] 
tick20:
 IF SC_ASYNC_TMO == 1
 ldr r5,[r7,#SC_TCK] 
 ldr r0,tick_SC_TMOLISTS
 mov r9,r0 
 movs r6,#4
tick21: 
 movs r0,#0x3f
 ands r0,r0,r5 
 lsls r0,r0,#3 
 add r0,r9 
 b tick21a 
tick25:
 enterAsyncTmo(r1,r2,r3,r4,r7,tick_SC_TMOLISTS,tick25)
tick21a:
 ldr r1,[r0,#dbl_next] 
 cmp r0,r1 
 beq tick29 
 dbl_del(r1,r2,r3) 
 subs r1,r1,#TMOMSG_TMOLIST 
 ldr r2,[r1,#TMOMSG_TMO] 
 cmp r2,#0 
 bne tick25 
 mov r8,r5 
 ldr r2,[r1,#MSG_PLID] 
 movs r3,#255
 lsls r3,r3,#24
 adds r3,r3,#255
 ands r2,r2,r3
 str r2,[r1,#MSG_PLID] 
 ldr r2,[r1,#MSG_SIZE] 
 lsrs r3,r2,#28 
 lsls r3,r3,#3 
 lsls r2,r2,#4 
 lsrs r2,r2,#4 
 str r3,[r1,#MSG_BUFSIZE_IDX] 
 str r2,[r1,#MSG_SIZE] 
 movs r2,#0
 str r2,[r1,#MSG_OWNER] 
 ldr r2,[r1,#MSG_SENDER] 
 str r2,[r1,#MSG_ADDRESSEE] 
 lsrs r3,r2,#24 
 lsls r3,r3,#2 
 ldr r4,tick_sc_modules
 ldr r3,[r4,r3] 
 lsls r2,r2,#18
 lsrs r2,r2,#16 
 ldr r4,[r3,#MODULE_PCBS]
 ldr r2,[r4,r2] 
 dbl_del(r1,r4,r5) 
 ldr r4,[r2,#PCB_NALLOC] 
 subs r4,r4,#1
 str r4,[r2,#PCB_NALLOC]
 IF SC_MSG_HOOK == 1
 IF SC_MSGTX_HOOK == 1
 push {r0-r2}
 ldr r2,[r3,#MODULE_MSGTXHOOK]
 cmp r2,#0
 beq tick22
 movs r0,r1
 BLX_R2_t
tick22:
 ldr r2,tick_sc_msgtxhook
 ldr r2,[r2,#0]
 cmp r2,#0
 beq tick23
 ldr r0,[sp,#4]
 BLX_R2_t
tick23:
 pop {r0-r2}
 ENDIF
 ENDIF
 mov r5,r8 
 movs r3,r2 
 adds r3,r3,#PCB_MSGQUEUE
 LOCK
 dbl_tailAdd(r3,r1,r4)
 ldr r4,[r2,#PCB_NQUEUE] 
 adds r4,r4,#1
 str r4,[r2,#PCB_NQUEUE]
 ldrh r3,[r2,#PCB_STATE_H] 
 movs r4,#ST_WAIT_MSGRX
 tst r3,r4
 beq tick26 
 bics r3,r3,r4 
 strh r3,[r2,#PCB_STATE_H] 
 lsrs r3,r3,#ST_STOPPED_POS+1 
 bne tick26 
 makeReady(r2,r1,r3,r4,r7,tick24) 
 ldr r4,[r7,#SC_CURPRIOPCB] 
 ldrb r3,[r2,#PCB_PRIO_B] 
 ldrb r4,[r4,#PCB_PRIO_B] 
 cmp r3,r4
 bhs tick26
 mov r11,r4 
tick26:
 UNLOCK
 b tick21a
tick29:
 lsls r0,r5,#26 
 bne tick30 
 lsrs r5,r5,#6 
 subs r6,r6,#1
 beq tick30
 add r9,r10 
 b tick21
tick30:
 ENDIF 
tick99:
 LOCK
 ldr r0,[r7,#SC_SCHEDULE_REQ] 
 mov r1,r11
 orrs r0,r0,r1 
 str r0,[r7,#SC_SCHEDULE_REQ] 
 UNLOCK
 pop {r0-r7} 
 mov r8,r0
 mov r9,r1
 mov r10,r2
 mov r11,r3
 pop {r3}
 bx r3
 LALIGN
 IF SC_PROC_HOOK == 1
 IF SC_PROCMMU_HOOK == 1
tick_55555555:
 LONG 0x55555555
 ENDIF
 ENDIF
 IF SC_STACK_CHECK == 1
tick_stackmagic:
 LONG 0x17171717
tick_overrun:
 LONG SC_TICK|KERNEL_ESTACK_OVERFLOW|SC_ERR_MODULE_FATAL
tick_underrun:
 LONG SC_TICK|KERNEL_ESTACK_UNDERFLOW|SC_ERR_MODULE_FATAL
 ENDIF
 IF SC_PROC_PARA_CHECK == 1
tick_ill_proc:
 LONG SC_TICK|KERNEL_EILL_PROC|SC_ERR_SYSTEM_FATAL
 ENDIF
tick_sciopta:
 LONG sciopta
tick_sc_modules:
 LONG sc_modules
tick_SC_TMOLISTS:
 LONG sciopta+SC_TMOLISTS
 IF SC_MSG_HOOK == 1
 IF SC_MSGTX_HOOK == 1
tick_sc_msgtxhook:
 LONG sciopta+SC_MSGTXHOOK
 ENDIF
 ENDIF
 ELSE 
 IF SC_STACK_CHECK == 1
tick_err:
 ldr r0,tick_overrun
tick_err_cont:
 movs r1,r4
 bl sc_sysError
tick_err2: 
 ldr r0,tick_underrun
 b tick_err_cont
 ENDIF
 SC_TPUBLIC sc_tick
 push.w {r4-r11,lr}
 ldr r7,tick_sciopta
 ldr r5,[r7,#SC_TCK] 
 adds r5,r5,#1
 str r5,[r7,#SC_TCK]
 IF SC_STACK_CHECK == 1
#ifdef __IAR5__
 ldr r1,=0x17171717
#else
 movw r1,#0x1717
 movt r1,#0x1717
#endif
 ldr r0,[r7,#SC_CURPRIOPCB] 
 ldr r2,[r0,#PCB_STACKBTM]
 ldr r3,[r0,#PCB_STACKTOP]
 ldr r4,[r0,#PCB_STACKPTR]
 ldr r0,[r2,#0] 
 cmp r0,r1 
 bne tick_err
 ldr r0,[r3,#0] 
 cmp r0,r1
 bne tick_err
 cmp r4,r2 
 blt tick_err 
 cmp r4,r3 
 bgt tick_err2 
 ENDIF
 ldr r0,[r7,#SC_CURPCB]
 ldr r1,[r7,#SC_CURMODULE]
 push {r0,r1}
 mov r11,#0 
 mov r10,#64*8 
 addw r9,r7,#SC_SLEEPERLISTS 
 mov r8,r5 
 movs r6,#4
 b tick0 
tick4:
 UNLOCK 
tick1:
 LOCK
 ldr r1,[r0,#dbl_next] 
 cmp r0,r1 
 beq tick9 
 dbl_del(r1,r2,r3) 
 subs r1,r1,#PCB_SLEEPERS 
 ldr r2,[r1,#PCB_TMO] 
 cmp r2,#0
 bne tick6 
 ldrh r2,[r1,#PCB_STATE_H] 
 subs r2,r2,#ST_WAIT_TMO 
 ldrh r3,[r1,#PCB_FLAGS_H] 
 lsrs r3,r3,#PCB_FLG_IS_TIMER_POS+1 
 bcs tick5 
 adds r2,r2,#ST_TMO 
 strh r2,[r1,#PCB_STATE_H] 
 lsrs r4,r2,#ST_WAIT_MSGALLOC_POS+1 
 bcc tick3 
 adds r5,r1,#PCB_PCBLIST 
 dbl_del(r5,r3,r4)
tick3:
 lsrs r2,r2,#ST_STOPPED_POS+1 
 bcs tick4 
 makeReady(r1,r2,r3,r4,r7,tick1) 
 ldr r2,[r7,#SC_CURPRIOPCB] 
 ldrb r1,[r1,#PCB_PRIO_B] 
 ldrb r2,[r2,#PCB_PRIO_B] 
 cmp r1,r2
 bhs tick4
 mov r11,r2 
 b tick4
tick9:
 UNLOCK
 lsls r0,r8,#26 
 bne tick10 
 lsrs r8,r8,#6 
 subs r6,r6,#1
 beq tick10 
 add r9,r10 
tick0:
 and r0,r8,#0x3f 
 add r0,r9,r0,lsl #3 
 b tick1
 LALIGN
tick5:
 strh r2,[r1,#PCB_STATE_H] 
 ldr r3,[r1,#PCB_PID] 
 str r1,[r7,#SC_CURPCB]
 str r3,[r7,#SC_CURPID]
 ldr r2,tick_sc_modules
 lsrs r3,r3,#24
 ldr r2,[r2,r3,lsl #2]
 str r2,[r7,#SC_CURMODULE]
 IF SC_PROC_HOOK == 1
 IF SC_PROCMMU_HOOK == 1
 ENDIF
 ENDIF
 mrs r2,msp 
 ldr r12,[r1,#PCB_STACKTOP] 
 msr msp,r12
 cpsie i
 push {r0-r3} 
 movs r0,#0
 ldr r2,[r1,#PCB_ENTRY]
 BLX_R2_a 
 cpsid i
 pop {r0-r3}
 msr msp,r2 
 IF SC_PROC_HOOK == 1
 IF SC_PROCMMU_HOOK == 1
 ENDIF
 ENDIF
 ldr r2,[r1,#PCB_SLICE] 
 ldrh r3,[r1,#PCB_FLAGS_H] 
 lsrs r3,r3,#PCB_FLG_READY_POS+1 
 bcc tick4 
tick6:
 enterTimer_T2(r1,r2,r3,r4,r7,tick5) 
 ldrh r3,[r1,#PCB_STATE_H] 
 orr r3,r3,#ST_WAIT_TMO
 strh r3,[r1,#PCB_STATE_H]
 b tick4
tick10:
 pop {r0,r1} 
 LOCK
 ldr r2,[r0,#PCB_PID]
 str r1,[r7,#SC_CURMODULE]
 str r0,[r7,#SC_CURPCB]
 str r2,[r7,#SC_CURPID]
 UNLOCK
 ldr r0,[r7,#SC_CURPRIOPCB] 
 ldr r3,[r0,#PCB_SLICE] 
 ldr r1,[r0,#PCB_CURSLICE] 
 cmp r3,#0 
 beq tick20 
 subs r1,r1,#1 
 bne tick19 
 movs r1,r3 
 LOCK
 ldr r2,[r0,#PCB_PCBLIST+dbl_prev] 
 ldr r3,[r0,#PCB_PCBLIST+dbl_next] 
 cmp r2,r3 
 beq tick18 
 str r3,[r2,#dbl_next] 
 str r2,[r3,#dbl_prev]
 movs r4,r0 
 adds r4,r4,#PCB_PCBLIST
 dbl_tailAdd(r2,r4,r3)
 mov r11,r2 
tick18:
 UNLOCK
tick19:
 str r1,[r0,#PCB_CURSLICE] 
tick20:
 IF SC_ASYNC_TMO == 1
 ldr r8,[r7,#SC_TCK] 
 add r9,r7,#SC_TMOLISTS 
 movs r6,#4
 b tick29
tick21:
 enterAsyncTmo_T2(r1,r2,r3,r4,r7,tick21)
tick22:
 ldr r1,[r0,#dbl_next] 
 cmp r0,r1 
 beq tick28 
 dbl_del(r1,r2,r3) 
 subs r1,r1,#TMOMSG_TMOLIST 
 ldr r2,[r1,#TMOMSG_TMO] 
 cmp r2,#0 
 bne tick21 
 ldr r2,[r1,#MSG_PLID] 
 bfc r2,#8,#16 
 str r2,[r1,#MSG_PLID] 
 ldr r2,[r1,#MSG_SIZE] 
 lsrs r3,r2,#28 
 lsls r3,r3,#3 
 bic r2,r2,#0xf0000000 
 str r3,[r1,#MSG_BUFSIZE_IDX] 
 str r2,[r1,#MSG_SIZE] 
 movs r2,#0
 str r2,[r1,#MSG_OWNER] 
 ldr r2,[r1,#MSG_SENDER] 
 str r2,[r1,#MSG_ADDRESSEE] 
 lsrs r3,r2,#24 
 ldr r4,tick_sc_modules
 ldr r3,[r4,r3,lsl #2] 
 bfc r2,#14,#18 
 ldr r4,[r3,#MODULE_PCBS]
 ldr r2,[r4,r2,lsl #2] 
 dbl_del(r1,r4,r5) 
 ldr r4,[r2,#PCB_NALLOC] 
 subs r4,r4,#1
 str r4,[r2,#PCB_NALLOC]
 IF SC_MSG_HOOK == 1
 IF SC_MSGTX_HOOK == 1
 push {r0-r2}
 ldr r2,[r3,#MODULE_MSGTXHOOK]
 cmp r2,#0
 beq tick24
 movs r0,r1
 BLX_R2_t
tick24:
 ldr r2,tick_sc_msgtxhook
 ldr r2,[r2,#0]
 cmp r2,#0
 beq tick25
 ldr r0,[sp,#4]
 BLX_R2_t
tick25:
 pop {r0-r2}
 ENDIF
 ENDIF
 adds r3,r2,#PCB_MSGQUEUE 
 LOCK
 dbl_tailAdd(r3,r1,r4)
 ldr r4,[r2,#PCB_NQUEUE] 
 adds r4,r4,#1
 str r4,[r2,#PCB_NQUEUE]
 ldrh r3,[r2,#PCB_STATE_H] 
 movs r4,#ST_WAIT_MSGRX
 tst r3,r4
 beq tick27 
 bics r3,r3,r4 
 strh r3,[r2,#PCB_STATE_H] 
 lsrs r3,r3,#ST_STOPPED_POS+1 
 bcs tick27 
 makeReady(r2,r1,r3,r4,r7,tick24) 
 ldr r4,[r7,#SC_CURPRIOPCB] 
 ldrb r3,[r2,#PCB_PRIO_B] 
 ldrb r4,[r4,#PCB_PRIO_B] 
 cmp r3,r4
 bhs tick27
 mov r11,r4 
tick27:
 UNLOCK
 b tick22
tick28:
 lsls r0,r8,#26 
 bne tick30 
 lsrs r8,r8,#6 
 subs r6,r6,#1
 beq tick30
 add r9,r10 
tick29: 
 and r0,r8,#0x3f 
 add r0,r9,r0,lsl #3 
 b tick22
tick30:
 ENDIF 
tick99:
 LOCK
 ldr r0,[r7,#SC_SCHEDULE_REQ] 
 orr r0,r0,r11 
 str r0,[r7,#SC_SCHEDULE_REQ] 
 UNLOCK
 pop.w {r4-r11,pc} 
 LALIGN
 IF SC_STACK_CHECK == 1
tick_overrun:
 LONG SC_TICK|KERNEL_ESTACK_OVERFLOW|SC_ERR_MODULE_FATAL
tick_underrun:
 LONG SC_TICK|KERNEL_ESTACK_UNDERFLOW|SC_ERR_MODULE_FATAL
 ENDIF
 IF SC_PROC_PARA_CHECK == 1
tick_ill_proc:
 LONG SC_TICK|KERNEL_EILL_PROC|SC_ERR_SYSTEM_FATAL
 ENDIF
tick_sciopta:
 LONG sciopta
tick_sc_modules:
 LONG sc_modules
 IF SC_MSG_HOOK == 1
 IF SC_MSGTX_HOOK == 1
tick_sc_msgtxhook:
 LONG sciopta+SC_MSGTXHOOK
 ENDIF
 ENDIF
 ENDIF
 SC_ENDFUNC sc_tickx
 XSC_TFUNC sc_sleep,-12
 SC_TFUNC sc_sleep
 push {r4-r7,lr} 
 ldr r7,sleep_sciopta
 ldr r6,[r7,#SC_CURPCB]
 ldrh r1,[r6,#PCB_FLAGS_H] 
 lsrs r1,r1,#PCB_FLG_IS_PRIO_POS+1 
 bcc sleep_err1 
 cmp r0,#SC_NO_TMO 
 beq sleep99 
 lsrs r1,r0,#24 
 bne sleep_err2 
 LOCK
 makeNotReady(r6,r1,r2,r7,sleep) 
 IF CPUTYPE <= Xscale
 enterTimer(r6,r0,r1,r2,r7,sleep) 
 ELSE
 enterTimer_T2(r6,r0,r1,r2,r7,sleep) 
 ENDIF
 ldrh r1,[r6,#PCB_STATE_H] 
 movs r0,#ST_WAIT_TMO 
 orrs r1,r1,r0
 strh r1,[r6,#PCB_STATE_H] 
 UNLOCK
 SWAP_OUT
 LOCK
 ldrh r1,[r6,#PCB_STATE_H] 
 cmp r0,#0 
 beq sleep_err3 
 movs r0,#ST_TMO 
 bics r1,r1,r0
 strh r1,[r6,#PCB_STATE_H] 
 UNLOCK
sleep99:
 RET r4-r7
sleep_err1:
 movs r1,#0
 ldr r0,sleep_err1_lit
 b sleep_err_cont
sleep_err2:
 movs r1,r0
 ldr r0,sleep_err2_lit
sleep_err_cont:
 bl sc_sysError
 b sleep99
sleep_err3:
 movs r0,#ST_WAIT_TMO 
 tst r1,r0
 beq sleep1 
 removeTimer(r6,r1,r0,r2) 
 bics r1,r1,r0
 strh r1,[r6,#PCB_STATE_H] 
sleep1:
 UNLOCK
 movs r1,#0
 ldr r0,sleep_err3_lit
 b sleep_err_cont
 LALIGN
sleep_sciopta:
 LONG sciopta
sleep_err1_lit:
 LONG (SC_SLEEP|KERNEL_EPROC_NOT_PRIO|SC_ERR_MODULE_FATAL)
sleep_err2_lit:
 LONG (SC_SLEEP|KERNEL_EILL_VALUE)
sleep_err3_lit:
 LONG (KERNEL_ELOCKED|SC_SLEEP|SC_ERR_MODULE_FATAL)
 SC_ENDFUNC sc_sleep
 IF SC_ASYNC_TMO == 1
 XSC_TFUNC sc_tmoAdd,-8
 SC_TFUNC sc_tmoAdd
 push {r4-r5,lr}
 IF SC_MSG_PARA_CHECK == 1
 cmp r1,#0
 beq tma_err0
 ldr r4,[r1,#0]
 cmp r4,#0
 beq tma_err0
 ELSE
 ldr r4,[r1,#0]
 ENDIF
 ldr r5,tma_sciopta
 IF SC_PROC_PARA_CHECK == 1
 ldr r3,[r5,#SC_CURPCB]
 ldrh r3,[r3,#PCB_FLAGS_H]
 lsrs r3,r3,#PCB_FLG_IS_PRIO_POS+1
 bcc tma_err1
 lsrs r2,r0,#24
 bne tma_err2
 ENDIF
 subs r4,r4,#MSG_HEAD
 ldr r3,[r4,#MSG_OWNER]
 IF SC_MSG_PARA_CHECK == 1
 mov r12,r5
 ldr r5,[r5,#SC_CURPCB]
 ldr r5,[r5,#PCB_PID]
 cmp r3,r5
 bne tma_err3
 mov r5,r12
 ENDIF
 str r3,[r4,#MSG_SENDER]
 cmp r0,#0
 bne tma1
 movs r4,#SC_TMOADD>>24
 lsls r4,r4,#24
 movs r0,r1
 movs r1,r3
 movs r2,#0
 bl sc_sysMsgTx
 RET r4-r5
tma1:
 movs r2,#0
 str r2,[r1,#0]
 ldr r2,tma_magic
 LOCK
 ldr r1,[r4,#MSG_PLID]
 orrs r1,r1,r2
 str r1,[r4,#MSG_PLID]
 ldr r2,[r4,#MSG_BUFSIZE_IDX]
 ldr r1,[r4,#MSG_SIZE]
 lsls r2,r2,#28-3
 orrs r1,r1,r2
 str r1,[r4,#MSG_SIZE]
 enterAsyncTmo(r4,r0,r1,r2,r5,tma_tmolist,tma)
 UNLOCK
 movs r0,r4
 RET r4-r5
 IF SC_PROC_PARA_CHECK == 1
tma_err2:
 ldr r0,tma_lit2
tma_err_cont:
 bl sc_sysError
 add sp,sp,#2*4
 RET r4-r5
tma_err1:
 ldr r1,[r2,#SC_CURPID]
 ldr r0,tma_lit1
 b tma_err_cont
 ENDIF
 IF SC_MSG_PARA_CHECK == 1
tma_err3:
 movs r5,r1
 ldr r0,tma_lit3
 b tma_err_cont 
tma_err0:
 ldr r0,tma_lit0
 IF SC_PROC_PARA_CHECK == 1
 b tma_err_cont
 ELSE
tma_err_cont: 
 bl sc_sysError
 ENDIF
 ENDIF
 LALIGN
 IF SC_MSG_PARA_CHECK == 1
tma_lit3:
 LONG (KERNEL_ENOT_OWNER|SC_TMOADD|SC_ERR_MODULE_FATAL)
tma_lit0:
 LONG (KERNEL_ENIL_PTR|SC_TMOADD|SC_ERR_MODULE_FATAL)
 ENDIF
 IF SC_PROC_PARA_CHECK == 1
tma_lit1:
 LONG (KERNEL_EILL_PROCTYPE|SC_TMOADD|SC_ERR_MODULE_FATAL)
tma_lit2:
 LONG (SC_TMOADD|KERNEL_EILL_VALUE)
 ENDIF
tma_sciopta:
 LONG sciopta
tma_magic:
 LONG MSG_TMO_MAGIC
tma_tmolist:
 LONG sciopta+SC_TMOLISTS
 SC_ENDFUNC sc_tmoAdd
 XSC_TFUNC sc_tmoRm,-12
 SC_TFUNC sc_tmoRm
 push {r4-r5,lr}
 movs r1,#0
 IF SC_MSG_PARA_CHECK == 1
 cmp r0,#0 
 beq tmr_err3
 ENDIF
 ldr r3,[r0,#0] 
 cmp r3,#0 
 beq tmr_err0 
 str r1,[r0,#0] 
 ldr r4,tmr_sciopta
 ldr r4,[r4,#SC_CURPCB] 
 ldr r5,[r4,#PCB_PID] 
 ldr r1,[r3,#MSG_SENDER] 
 cmp r1,r5 
 bne tmr_err1 
 ldr r1,tmr_magic
 LOCK
 ldr r0,[r3,#MSG_PLID]
 lsls r2,r0,#8
 lsrs r2,r2,#16
 cmp r2,r1 
 beq tmr10 
 ldr r0,[r3,#MSG_OWNER] 
 ldr r1,[r3,#MSG_ADDRESSEE] 
 cmp r0,#0 
 bne tmr_err2
 cmp r5,r1 
 bne tmr_err2 
 ldr r1,[r4,#PCB_NQUEUE] 
 subs r1,r1,#1
 str r1,[r4,#PCB_NQUEUE]
 dbl_del(r3,r1,r2) 
 ldr r1,[r4,#PCB_NALLOC] 
 adds r1,r1,#1
 str r1,[r4,#PCB_NALLOC]
 adds r4,r4,#PCB_ALLOCED
 dbl_tailAdd(r4,r3,r1) 
tmr_exit:
 movs r0,#0 
 str r0,[r3,#MSG_SENDER] 
 str r0,[r3,#MSG_ADDRESSEE] 
 str r5,[r3,#MSG_OWNER] 
 UNLOCK
 movs r0,r3
 adds r0,r0,#MSG_HEAD
 RET r4-r5
tmr10:
 movs r4,r3 
 adds r4,r4,#TMOMSG_TMOLIST 
 dbl_del(r4,r1,r2) 
 movs r1,#255
 lsls r1,r1,#24
 adds r1,r1,#255
 ands r0,r0,r1 
 str r0,[r3,#MSG_PLID]
 ldr r0,[r3,#MSG_SIZE]
 lsrs r1,r0,#28 
 lsls r1,r1,#3
 str r1,[r3,#MSG_BUFSIZE_IDX] 
 lsls r0,r0,#4
 lsrs r0,r0,#4 
 str r0,[r3,#MSG_SIZE]
 b tmr_exit
tmr_err2:
 UNLOCK
tmr_err1:
 ldr r0,tmr_lit1
 b tmr_err_cont
 IF SC_MSG_PARA_CHECK == 1
tmr_err3:
 ldr r0,tmr_nilptr
 b tmr_err_cont
 ENDIF
tmr_err0:
 ldr r0,tmr_lit0
tmr_err_cont: 
 bl sc_sysError
 movs r0,#0
 RET r4-r5
 LALIGN
tmr_lit0:
 LONG (KERNEL_EILL_VALUE|SC_TMORM)
tmr_lit1:
 LONG (KERNEL_ENOT_OWNER|SC_TMORM|SC_ERR_MODULE_FATAL)
 IF SC_MSG_PARA_CHECK == 1
tmr_nilptr: 
 LONG (KERNEL_ENIL_PTR|SC_TMORM|SC_ERR_MODULE_FATAL)
 ENDIF
tmr_sciopta:
 LONG sciopta
tmr_magic:
 LONG MSG_TMO_MAGIC>>8
 SC_ENDFUNC sc_tmoRm
 ENDIF
 XSC_TFUNC sc_triggerWait,-8
 SC_TFUNC sc_triggerWaitx
tw_err:
 movs r1,r0
tw_err2:
 ldr r0,tw_lit1
tw_err_cont: 
 bl sc_sysError
 movs r0,#0
 RET r4-r7
 SC_TPUBLIC sc_triggerWait
 push {r4-r7,lr}
 ldr r7,tw_sciopta 
 adds r5,r0,#0 
 ble tw_err 
 cmp r1,#SC_NO_TMO 
 beq tw_err2 
 ldr r6,[r7,#SC_CURPCB] 
 ldrh r2,[r6,#PCB_FLAGS_H] 
 lsrs r2,r2,#PCB_FLG_IS_PRIO_POS+1
 bcc tw_err0 
 movs r0,#0 
 LOCK
 ldr r2,[r6,#PCB_TRIGGER_VAL]
 subs r2,r2,r5
 str r2,[r6,#PCB_TRIGGER_VAL] 
 bgt tw_exit 
 adds r2,r1,#-SC_ENDLESS_TMO 
 beq tw1 
 IF CPUTYPE <= Xscale
 enterTimer(r6,r1,r0,r2,r7,tw) 
 ELSE
 enterTimer_T2(r6,r1,r0,r2,r7,tw) 
 ENDIF
 movs r2,#ST_WAIT_TMO 
tw1:
 ldrh r0,[r6,#PCB_STATE_H] 
 movs r1,#ST_WAIT_TRIGGER
 orrs r0,r0,r2
 orrs r0,r0,r1
 strh r0,[r6,#PCB_STATE_H] 
 makeNotReady(r6,r1,r2,r7,tw1) 
 UNLOCK
 SWAP_OUT_SAVE {r5} 
 LOCK
 ldrh r1,[r6,#PCB_STATE_H]
 cmp r0,#0 
 beq tw_err1 
 movs r0,#1 
 movs r2,#ST_TMO
 tst r1,r2 
 beq tw_exit 
 adds r2,#ST_WAIT_TRIGGER
 bics r1,r1,r2 
 strh r1,[r6,#PCB_STATE_H] 
 subs r0,r0,#2 
 ldr r1,[r6,#PCB_TRIGGER_VAL] 
 adds r1,r1,r5
 str r1,[r6,#PCB_TRIGGER_VAL] 
tw_exit: 
 UNLOCK
 RET r4-r7
tw_err1:
 movs r0,#ST_WAIT_TMO 
 tst r1,r0 
 beq tw5
 removeTimer(r4,r1,r0,r2) 
 strh r1,[r4,#PCB_STATE_H]
tw5:
 UNLOCK
 ldr r0,tw_lit2
 movs r1,#0
 b tw_err_cont
tw_err0:
 movs r1,r3
 ldr r0,tw_lit2
 b tw_err_cont
 LALIGN
tw_sciopta:
 LONG sciopta
tw_lit1:
 LONG SC_TRIGGERWAIT|KERNEL_EILL_VALUE|SC_ERR_PROCESS_WARNING
tw_lit2:
 LONG SC_TRIGGERWAIT|KERNEL_EPROC_NOT_PRIO|SC_ERR_PROCESS_FATAL
 SC_ENDFUNC sc_triggerWaitx
 XSC_TFUNC sc_trigger,-12
 SC_TFUNC sc_trigger
 push {r4-r7,lr}
 ldr r7,t_sciopta 
 ldr r6,t_sc_modules 
 ldr r5,[r7,#SC_CURPCB] 
 LOCK
 adds r1,r0,#-SC_CURRENT_PID 
 beq t1 
 adds r1,r0,#0 
 ble t_err 
 lsrs r2,r0,#24 
 cmp r2,#SC_MAX_MODULE 
 bhs t_err 
 lsls r2,r2,#2
 ldr r2,[r6,r2] 
 cmp r2,#1 
 bls t_err 
 ldr r3,[r2,#MODULE_MAX_PROCESS] 
 lsls r0,r0,#18 
 lsrs r0,r0,#18 
 cmp r0,r3 
 ldr r3,[r2,#MODULE_PCBS] 
 bhs t_err 
 lsls r0,r0,#2
 ldr r5,[r3,r0] 
 cmp r5,#0 
 beq t_err 
 ldr r2,[r5,#PCB_PID] 
 cmp r2,r1
 bne t_warn 
t1:
 ldrh r4,[r5,#PCB_FLAGS_H] 
 ldr r3,[r5,#PCB_TRIGGER_VAL] 
 movs r2,#PCB_FLG_IS_PRIO|PCB_FLG_IS_TIMER|PCB_FLG_IS_IRQ
 tst r4,r2 
 beq t_err 
 adds r3,r3,#1 
 str r3,[r5,#PCB_TRIGGER_VAL] 
 ble t_exit 
 ldrh r3,[r5,#PCB_STATE_H]
 lsrs r0,r4,#PCB_FLG_IS_PRIO_POS+1
 bcc t3 
 movs r1,#ST_WAIT_TRIGGER
 tst r3,r1 
 beq t_exit 
 bics r3,r3,r1 
 movs r1,#ST_WAIT_TMO
 tst r3,r1 
 beq t2 
 removeTimer(r5,r3,r1,r0) 
t2: 
 strh r3,[r5,#PCB_STATE_H] 
 lsrs r3,r3,#ST_READY_POS+1 
 bne t_exit 
 makeReady(r5,r0,r1,r2,r7,t2) 
 UNLOCK
 ldr r0,[r7,#SC_CURPRIOPCB] 
 ldrb r1,[r5,#PCB_PRIO_B]
 ldrb r0,[r0,#PCB_PRIO_B]
 cmp r1,r0
 bhs t_exit1 
t5:
 SWAP_OUT
 RET r4-r7
t_exit:
 UNLOCK
 RET r4-r7
t3:
 UNLOCK
 movs r0,#PCB_FLG_WAKEUP_DIS
 tst r4,r0 
 bne t_exit1
 movs r0,#2 
 movs r1,r5 
 FAKE_INT
 ldr r0,[r7,#SC_SCHEDULE_REQ] 
 cmp r0,#0
 bne t5 
t_exit1:
 RET r4-r7
t_warn:
 ldr r0,t_lit1
 b t_err_cont
t_err:
 ldr r0,t_lit0
t_err_cont: 
 UNLOCK
 bl sc_sysError
 RET r4-r7
 LALIGN
t_sciopta:
 LONG sciopta
t_sc_modules:
 LONG sc_modules
t_lit0:
 LONG KERNEL_EILL_PID|SC_TRIGGER|SC_ERR_PROCESS_FATAL
t_lit1:
 LONG KERNEL_EILL_PID|SC_TRIGGER
 SC_ENDFUNC sc_trigger
 XSC_TFUNC sc_triggerValueSet,-12
 SC_TFUNC sc_triggerValueSet
 adds r1,r0,#0
 ble tvs_err
 ldr r3,tvs_sciopta
 ldr r2,[r3,#SC_CURPCB]
 str r0,[r2,#PCB_TRIGGER_VAL]
 bx lr
tvs_err:
 push {lr}
 ldr r0,tvs_lit0
 bl sc_sysError
 EXIT
 LALIGN
tvs_sciopta:
 LONG sciopta
tvs_lit0:
 LONG SC_TRIGGERVALUESET|KERNEL_EILL_VALUE
 SC_ENDFUNC sc_triggerValueSet
 XSC_TFUNC sc_triggerValueGet,-12
 SC_TFUNC sc_triggerValueGet
 ldr r3,tvg_sciopta
 ldr r2,[r3,#SC_CURPCB]
 adds r1,r0,#-SC_CURRENT_PID
 bne tvg1
 ldr r0,[r2,#PCB_TRIGGER_VAL]
 bx lr
tvg1: 
 push {lr}
 cmp r0,#0
 ble tvg_err
 lsrs r1,r0,#24
 cmp r1,#SC_MAX_MODULE
 ldr r3,tvg_sc_modules
 bhs tvg_err
 LOCK
 lsls r1,r1,#2
 ldr r3,[r3,r1]
 lsls r1,r0,#18
 lsrs r1,r1,#18
 cmp r3,#1
 bls tvg_err0
 ldr r2,[r3,#MODULE_MAX_PROCESS]
 cmp r1,r2
 ldr r2,[r3,#MODULE_PCBS]
 bhs tvg_err0
 lsls r1,r1,#2
 ldr r2,[r2,r1]
 cmp r2,#0
 beq tvg_err0
 ldr r0,[r2,#PCB_TRIGGER_VAL]
 UNLOCK
 EXIT 
tvg_err0:
 UNLOCK
tvg_err:
 movs r1,r0
 ldr r0,tvg_lit1
 bl sc_sysError
 LALIGN
tvg_sciopta:
 LONG sciopta
tvg_sc_modules:
 LONG sc_modules
tvg_lit1:
 LONG KERNEL_EILL_PID|SC_TRIGGERVALUEGET|SC_ERR_PROCESS_FATAL
 SC_ENDFUNC sc_triggerValueGet
#define REQUEST 48
#define REPLY 44
#define MSG 40
#define HOOK 32
#define TARGET 0
#define REQUEST_ID r11
 SC_TFUNC sc_procdx
pd_idget:
 movs r0,#SC_PROCDREQUESTMSG_SZ
 ldr r1,pd_SC_PROCDREQUESTMSG
 movs r2,#0
 subs r3,r2,#-SC_FATAL_IF_TMO
 bl sc_msgAlloc
 movs r4,r0
 mov r5,r8
 subs r5,r5,#MSG_HEAD
 ldr r1,[r5,#MSG_SENDER]
 movs r0,#0
 str r0,[r4,#SC_PROCDREQUESTMSG_REQUEST_ID]
 str r0,[r4,#SC_PROCDREQUESTMSG_TMOID]
 str r1,[r4,#SC_PROCDREQUESTMSG_REQUESTOR]
 adds r5,r5,#MSG_HEAD+SC_PROCIDGETMSG_PATH
 movs r0,r5
 bl sc_procPathCheck
 cmp r0,#0
 beq pd_error_cont
 ldrb r1,[r5,#0]
 cmp r1,#'/'
 bne pd_no_module
pd_again:
 ldrb r2,[r5,#1]
 adds r5,r5,#1
 cmp r1,r2
 bne pd_no_target
 mov r2,r10
 b pd12
pd_error_cont:
 b pd_error
pd11:
 strb r1,[r2,#0]
pd12:
 ldrb r1,[r5,#1]
 adds r5,r5,#1
 adds r2,r2,#1
 cmp r1,#'/'
 bne pd11
 movs r1,#0
 strb r1,[r2,#0]
 add r0,sp,#TARGET
 bl sc_moduleIdGet
 movs r1,#'/'
 cmp r0,#0
 beq pd_again
 IF SC_MAX_CONNECTOR > 0
 add r0,sp,#TARGET
 bl sc_miscCrcString
 mov r12,r0
 ldr r7,pd_sc_connectors
 movs r6,#SC_MAX_CONNECTOR
pd13:
 ldmia r7!,{r3}
 subs r6,r6,#1
 bmi pd_error_cont
 cmp r3,#1
 bls pd13
 ldrh r0,[r3,#PCB_HASH_H]
 cmp r0,r12
 bne pd13
 ldr r0,[r3,#PCB_PID]
 mov r11,r0
 add r0,sp,#TARGET
 ldr r3,[r3,#PCB_NAME]
pd14:
 ldrb r1,[r0,#0]
 adds r0,r0,#1
 ldrb r2,[r3,#0]
 adds r3,r3,#1
 cmp r2,r1
 bne pd13
 cmp r2,#0
 bne pd14
 movs r1,r5
 movs r0,#SC_CONNPROCIDGETMSG_SZ-1
pd15:
 ldrb r2,[r1,#0]
 adds r0,r0,#1
 adds r1,r1,#1
 cmp r2,#0
 bne pd15
 ldr r1,pd_lit1
 movs r2,#0
 subs r3,r2,#-SC_FATAL_IF_TMO
 bl sc_msgAlloc
 str r0,[sp,#REPLY]
 mov r1,r8
 ldr r2,[r1,#SC_PROCIDGETMSG_TMO]
 str r2,[r0,#SC_CONNPROCIDGETMSG_TMO]
 mov r2,REQUEST_ID
 str r2,[r0,#SC_CONNPROCIDGETMSG_REQUEST]
 adds r0,r0,#SC_CONNPROCIDGETMSG_PATH-1
 movs r1,r5
pd16:
 ldrb r2,[r1,#0]
 adds r0,r0,#1
 adds r1,r1,#1
 strb r2,[r0,#0]
 cmp r2,#0
 bne pd16
 add r0,sp,#REPLY
 mov r1,r11
 movs r2,#0
 bl sc_msgTx
 mov r0,REQUEST_ID
 str r0,[r4,#SC_PROCDREQUESTMSG_REQUEST_ID]
 adds r0,r0,#1
 mov REQUEST_ID,r0
 ldr r0,pd_lit2
 str r0,[r4,#0]
 b pd_loopadd
 ELSE
 b pd_error
 ENDIF
pd_no_module:
 ldr r0,[r4,#SC_PROCDREQUESTMSG_REQUESTOR]
 bl sc_moduleNameGet
 movs r1,r4
 adds r1,r1,#SC_PROCDREQUESTMSG_MODULE
 ldmia r0!,{r2-r3,r6-r7}
 stmia r1!,{r2-r3,r6-r7}
 ldmia r0!,{r2-r3,r6-r7}
 stmia r1!,{r2-r3,r6-r7}
 ldr r7,[r4,#SC_PROCDREQUESTMSG_REQUESTOR]
 b pd_idget_proc
pd_no_target:
 movs r1,r4
 adds r1,r1,#SC_PROCDREQUESTMSG_MODULE
 movs r0,r1
pd17:
 strb r2,[r1,#0]
pd18:
 ldrb r2,[r5,#1]
 adds r5,r5,#1
 adds r1,r1,#1
 cmp r2,#0
 beq pd_target_proc
 cmp r2,#'/'
 bne pd17
 movs r2,#0
 strb r2,[r1,#0]
 bl sc_moduleIdGet
 movs r7,r0 
 adds r5,r5,#1
 b pd_idget_proc
pd_target_proc:
 strb r2,[r1,#0]
 movs r1,r4
 adds r1,r1,#SC_PROCDREQUESTMSG_PROCESS
 ldmia r0!,{r2-r3,r5-r6}
 stmia r1!,{r2-r3,r5-r6}
 ldmia r0!,{r2-r3,r5-r6}
 stmia r1!,{r2-r3,r5-r6}
 movs r7,#0
 movs r0,r7
 bl sc_moduleNameGet
 movs r1,r4
 adds r1,r1,#SC_PROCDREQUESTMSG_MODULE
 ldmia r0!,{r2-r3,r5-r6}
 stmia r1!,{r2-r3,r5-r6}
 ldmia r0!,{r2-r3,r5-r6}
 stmia r1!,{r2-r3,r5-r6}
 b pd20
pd_idget_proc:
 movs r1,r4
 adds r1,r1,#SC_PROCDREQUESTMSG_PROCESS-1
pd19:
 ldrb r2,[r5,#0]
 adds r5,r5,#1
 adds r1,r1,#1
 strb r2,[r1,#0]
 cmp r2,#0
 bne pd19
pd20:
 movs r6,#0
 adds r0,r7,#-SC_ILLEGAL_MID
 beq pd21
 movs r0,r4
 adds r0,r0,#SC_PROCDREQUESTMSG_PROCESS
 movs r1,r7
 bl sc_sysProcIdGet
 ldr r1,pd_SC_ILLEGAL_PID
 movs r7,r0
 cmp r0,r1
 beq pd21
 b pd_txreply
pd21:
 mov r0,r8
 ldr r5,[r0,#SC_PROCIDGETMSG_TMO]
 adds r2,r5,#-SC_FATAL_IF_TMO
 beq pd_error
 cmp r5,#SC_NO_TMO
 beq pd_tmoreply
 movs r0,r4
 adds r0,r0,#SC_PROCDREQUESTMSG_MODULE
 bl sc_miscCrcString
 movs r6,#SC_PROCDREQUESTMSG_MHASH
 strh r0,[r4,r6]
 adds r6,r6,#SC_PROCDREQUESTMSG_PHASH-SC_PROCDREQUESTMSG_MHASH
 movs r0,r4
 adds r0,r0,#SC_PROCDREQUESTMSG_PROCESS
 bl sc_miscCrcString
 strh r0,[r4,r6]
 adds r0,r5,#-SC_ENDLESS_TMO
 beq pd_loopadd
 IF SC_ASYNC_TMO == 1
 movs r0,#SC_PROCDTMOREQMSG_SZ
 ldr r1,pd_SC_PROCDTMOREQMSG
 movs r2,#0
 subs r3,r2,#-SC_ENDLESS_TMO
 bl sc_msgAlloc
 str r4,[r0,#SC_PROCDTMOREQMSG_REQUEST]
 str r0,[sp,#REPLY]
 movs r0,r5
 add r1,sp,#REPLY
 bl sc_tmoAdd
 str r0,[r4,#SC_PROCDREQUESTMSG_TMOID]
 b pd_loopadd
 ELSE
 b pd_error
 ENDIF
 SC_TPUBLIC sc_procd
 bl sc_procDaemonRegister
 cmp r0,#0
 bne pd1
 ldr r0,pd_lit0
 movs r1,#0
 bl sc_procKill
pd1: 
 movs r0,#42
 mov REQUEST_ID,r0
 sub sp,sp,#SC_PROC_NAME_SIZE+1+12+8
 add r0,sp,#HOOK
 str r0,[sp,#HOOK]
 str r0,[sp,#HOOK+4]
 mov r0,sp 
 subs r0,r0,#1
 mov r10,r0
 b pd10
pd_error:
 ldr r6,pd_lit3 
pd_tmoreply:
 ldr r7,pd_SC_ILLEGAL_PID 
pd_txreply:
 movs r0,#SC_PROCIDGETMSG_REPLY_SZ
 ldr r1,pd_SC_PROCIDGETMSG_REPLY
 movs r2,#0
 subs r3,r2,#-SC_FATAL_IF_TMO
 bl sc_msgAlloc
 str r0,[sp,#REPLY]
 str r6,[r0,#SC_PROCIDGETMSG_REPLY_ERROR]
 str r7,[r0,#SC_PROCIDGETMSG_REPLY_PID]
 add r0,sp,#REPLY
 ldr r1,[r4,#SC_PROCDREQUESTMSG_REQUESTOR]
 movs r2,#0
 bl sc_msgTx
 str r4,[sp,#REQUEST]
 add r0,sp,#REQUEST
 bl sc_msgFree
 b pd_loop
pd_loopadd:
 add r0,sp,#HOOK
 adds r4,r4,#SC_PROCDREQUESTMSG_LIST
 dbl_tailAdd(r0,r4,r1)
pd_loop:
 add r0,sp,#MSG
 bl sc_msgFree
pd10:
 movs r0,#-SC_ENDLESS_TMO
 NEG(r0)
 adr r4,pd_select
 movs r1,r4
 movs r2,#SC_MSGRX_MSGID
 bl sc_msgRx
 str r0,[sp,#MSG]
 mov r8,r0
 ldr r5,[r0,#0] 
 IF SC_MAX_CONNECTOR > 0 
 ldmia r4!,{r1-r3}
 ELSE
 ldmia r4!,{r1-r2}
 ENDIF
 cmp r5,r1
 beq pd_notify
 IF SC_ASYNC_TMO == 1
 cmp r5,r2
 beq pd_tmoreq
 ENDIF
 IF SC_MAX_CONNECTOR > 0 
 cmp r5,r3
 beq pd_connidget
 ENDIF
 b pd_idget 
 IF SC_ASYNC_TMO == 1
pd_tmoreq:
 ldr r0,[r0,#SC_PROCDTMOREQMSG_REQUEST]
 movs r4,r0
 adds r0,r0,#SC_PROCDREQUESTMSG_LIST
 dbl_del(r0,r1,r2)
 movs r6,#0
 b pd_tmoreply
 ENDIF
pd_notify: 
 add r5,sp,#HOOK
 mov r9,r5
pd30:
 ldr r5,[r5,#dbl_next]
 cmp r5,r9
 beq pd_loop
 movs r4,r5
 subs r4,r4,#SC_PROCDREQUESTMSG_LIST
 ldr r0,[r4,#SC_PROCDREQUESTMSG_ID]
 ldr r1,pd_SC_PROCDREQUESTMSG
 cmp r1,r0
 bne pd30
 movs r2,r4
 adds r2,r2,#SC_PROCDREQUESTMSG_MHASH
 mov r1,r8
 adds r1,r1,#SC_PROCCREATENOTIFYMSG_MHASH
 ldrh r1,[r1,#0]
 ldrh r3,[r2,#0]
 cmp r1,r3
 bne pd30
 mov r1,r8
 adds r1,r1,#SC_PROCCREATENOTIFYMSG_PHASH
 ldrh r1,[r1,#0]
 ldrh r2,[r2,#SC_PROCDREQUESTMSG_PHASH-SC_PROCDREQUESTMSG_MHASH]
 cmp r1,r2
 bne pd30
 movs r2,r4
 adds r2,r2,#SC_PROCDREQUESTMSG_MODULE
 mov r3,r8
 adds r3,r3,#SC_PROCCREATENOTIFYMSG_MODULE
pd31:
 ldrb r0,[r2,#0]
 ldrb r1,[r3,#0]
 adds r2,r2,#1
 adds r3,r3,#1
 cmp r0,r1
 bne pd30
 cmp r0,#0
 bne pd31
 movs r2,r4
 adds r2,r2,#SC_PROCDREQUESTMSG_PROCESS
 mov r3,r8
 adds r3,r3,#SC_PROCCREATENOTIFYMSG_PROCESS
pd32:
 ldrb r0,[r2,#0]
 ldrb r1,[r3,#0]
 adds r2,r2,#1
 adds r3,r3,#1
 cmp r0,r1
 bne pd30
 cmp r0,#0
 bne pd32
 dbl_del(r5,r0,r1)
 IF SC_ASYNC_TMO == 1
 ldr r0,[r4,#SC_PROCDREQUESTMSG_TMOID]
 cmp r0,#0
 beq pd_txreply2a
 movs r0,r4
 adds r0,r0,#SC_PROCDREQUESTMSG_TMOID
 bl sc_tmoRm
 str r0,[sp,#REPLY]
 add r0,sp,#REPLY
 bl sc_msgFree
 ENDIF
pd_txreply2a:
 mov r0,r8
 movs r6,#0
 ldr r7,[r0,#SC_PROCCREATENOTIFYMSG_PID]
 bl pd_txreply2
 b pd30
 IF SC_MAX_CONNECTOR > 0
pd_connidget:
 mov r7,r8
 add r5,sp,#HOOK
 mov r9,r5
pd40:
 ldr r5,[r5,#dbl_next]
 cmp r5,r9
 beq pd_loop
 movs r4,r5
 subs r4,r4,#SC_PROCDREQUESTMSG_LIST
 ldr r0,[r4,#SC_PROCDREQUESTMSG_ID]
 ldr r1,pd_SC_PROCDCONNREQUESTMSG
 cmp r0,r1
 bne pd40
 ldr r0,[r4,#SC_PROCDREQUESTMSG_REQUEST_ID]
 ldr r1,[r7,#SC_CONNPROCIDGETMSG_REPLY_REQUEST]
 cmp r0,r1
 bne pd40
 dbl_del(r5,r0,r1)
 ldr r6,[r7,#SC_CONNPROCIDGETMSG_REPLY_ERROR]
 ldr r7,[r7,#SC_CONNPROCIDGETMSG_REPLY_PID]
 cmp r6,#0
 beq pd41
 ldr r6,pd_lit3
pd41:
 bl pd_txreply2
 b pd40
 ENDIF
pd_txreply2:
 push {lr}
 movs r0,#SC_PROCIDGETMSG_REPLY_SZ
 ldr r1,pd_SC_PROCIDGETMSG_REPLY
 movs r2,#0
 subs r3,r2,#-SC_FATAL_IF_TMO
 bl sc_msgAlloc
 str r0,[sp,#REPLY+4]
 str r6,[r0,#SC_PROCIDGETMSG_REPLY_ERROR]
 str r7,[r0,#SC_PROCIDGETMSG_REPLY_PID]
 add r0,sp,#REPLY+4
 ldr r1,[r4,#SC_PROCDREQUESTMSG_REQUESTOR]
 movs r2,#0
 bl sc_msgTx
 str r4,[sp,#REQUEST+4]
 add r0,sp,#REQUEST+4
 bl sc_msgFree
 pop {pc}
 LALIGN
pd_SC_PROCDCONNREQUESTMSG:
 LONG SC_PROCDCONNREQUESTMSG
pd_SC_PROCDREQUESTMSG:
 LONG SC_PROCDREQUESTMSG
pd_SC_PROCDTMOREQMSG:
 LONG SC_PROCDTMOREQMSG
pd_SC_ILLEGAL_PID:
 LONG SC_ILLEGAL_PID
pd_SC_PROCIDGETMSG_REPLY:
 LONG SC_PROCIDGETMSG_REPLY
pd_lit3:
 LONG (SC_PROCIDGET|KERNEL_EILL_PROC_NAME|SC_ERR_PROCESS_FATAL)
 IF SC_MAX_CONNECTOR > 0
pd_sc_connectors:
 LONG sc_connectors
pd_lit2:
 LONG SC_PROCDCONNREQUESTMSG
pd_lit1:
 LONG SC_CONNPROCIDGETMSG
 ENDIF
pd_lit0: 
 LONG SC_CURRENT_PID
pd_select:
 LONG SC_PROCCREATENOTIFYMSG
 LONG SC_PROCDTMOREQMSG
 IF SC_MAX_CONNECTOR > 0
 LONG SC_CONNPROCIDGETMSG_REPLY
 ENDIF
 LONG SC_PROCIDGETMSG
 LONG 0
 SC_ENDFUNC sc_procdx
#undef TARGET
#undef REQUEST
#undef REPLY
#undef MSG
#undef HOOK
#undef REQUEST_ID
#define REPLY 4
#define MSG 0
 SC_TFUNC sc_kerneld
 sub sp,sp,#8
 bl sc_miscKerneldRegister
 cmp r0,#0
 bne k_loop
 movs r0,#-SC_CURRENT_PID
 NEG(r0)
 movs r1,#0
 bl sc_procKill
k_reply:
 ldr r0,[r2,#0]
 adds r0,r0,#1
 str r0,[r2,#0]
 add r0,sp,#MSG 
 bl sc_msgSizeSet
 add r0,sp,#MSG
 mov r1,r10
 movs r2,#0
 bl sc_msgTx
k_loop:
 movs r0,#-SC_ENDLESS_TMO
 NEG(r0)
 adr r7,k_select
 movs r1,r7
 movs r2,#SC_MSGRX_MSGID
 bl sc_msgRx
 str r0,[sp,#MSG]
 mov r8,r0
 subs r0,r0,#MSG_HEAD
 ldr r1,[r0,#MSG_SENDER]
 mov r10,r1 
 ldr r0,[r0,#MSG_HEAD]
 adr r6,k_lit0
 ldmia r7!,{r1-r4}
 ldmia r6!,{r5}
 cmp r0,r1
 beq k_create
 ldmia r6!,{r5}
 cmp r0,r2
 beq k_create
 ldmia r6!,{r5}
 cmp r0,r3
 beq k_create
 ldmia r6!,{r5}
 cmp r0,r4
 beq k_create
 ldmia r7!,{r1-r2}
 cmp r0,r1
 beq k_kill
 cmp r0,r2
 beq k_mcreate
k_mkill:
 mov r4,r8
 ldr r0,[r4,#SC_MODULEKILLMSG_MID]
 ldr r1,[r4,#SC_MODULEKILLMSG_FLAGS]
 adds r2,r0,#-SC_CURRENT_MID
 bne k_mk1
 mov r0,r10
k_mk1:
 movs r5,r0
 bl sc_sysModuleKill
 movs r1,#SC_MODULEKILLMSG_REPLY_SZ
 mov r2,r8
 cmp r5,r10
 str r5,[r2,#SC_MODULEKILLMSG_REPLY_MID]
 bne k_reply
k_free: 
 add r0,sp,#MSG
 bl sc_msgFree
 b k_loop
k_mcreate:
 mov r0,r8
 adds r0,r0,#SC_MODULECREATEMSG_STACKSIZE
 ldmia r0!,{r1-r7}
 subs r0,r0,#SC_MODULECREATEMSG_SZ-SC_MODULECREATEMSG_MODULE
 push {r3-r7}
 movs r3,r2 
 movs r2,r1 
 ldr r1,[r0,#SC_MODULECREATEMSG_ENTRY-SC_MODULECREATEMSG_MODULE]
 bl sc_sysModuleCreate
 add sp,sp,#5*4
 mov r2,r8
 str r0,[r2,#SC_MODULECREATEMSG_REPLY_MID]
 movs r1,#SC_MODULECREATEMSG_REPLY_SZ 
 b k_reply
k_kill:
 mov r3,r8
 ldr r0,[r3,#SC_PROCKILLMSG_PID]
 ldr r1,[r3,#SC_PROCKILLMSG_FLAG]
 movs r4,r0
 bl sc_sysProcKill
 movs r1,#SC_PROCKILLMSG_REPLY_SZ
 mov r2,r8
 cmp r4,r10
 bne k_reply
 b k_free
k_create:
 mov r3,r8
 mov r1,r10
 ldr r6,k_sc_modules
 lsrs r2,r1,#24
 lsls r2,r2,#2
 ldr r6,[r6,r2] 
 ldr r0,[r3,#SC_PROCCREATEMSG_STACKSIZE]
 IF CPUTYPE == Xscale
 lsrs r0,r0,#3
 lsls r0,r0,#3
 ELSE
 lsrs r0,r0,#2
 lsls r0,r0,#2
 ENDIF
 adds r0,r0,#SIZEOF_PCB+SC_PROC_NAME_SIZE+1
 movs r4,r0 
 ldr r1,k_lit1
 ldr r2,[r3,#SC_PROCCREATEMSG_PLID]
 movs r3,#SC_NO_TMO
 ldr r7,k_sciopta
 bl sc_sysMsgAlloc
 cmp r0,#0
 beq k_err
 str r0,[sp,#REPLY]
 mov r9,r0
 movs r4,r0 
 adds r4,r4,#SIZEOF_PCB+SC_PROC_NAME_SIZE+1
 movs r3,r0 
 ldr r2,[r6,#MODULE_ID]
 mov r7,r8 
 ldr r1,[r7,#SC_PROCCREATEMSG_STATE]
 ldr r0,[r7,#SC_PROCCREATEMSG_PRIO]
 push {r0-r5}
 adds r5,r7,#SC_PROCCREATEMSG_NAME
 mov r1,r9
 adds r1,r1,#SIZEOF_PCB
 movs r0,r1
 ldmia r5!,{r2-r4,r6}
 stmia r1!,{r2-r4,r6}
 ldmia r5!,{r2-r4,r6}
 stmia r1!,{r2-r4,r6}
 adds r7,r7,#SC_PROCCREATEMSG_ENTRY
 ldmia r7!,{r1-r3}
 bl sc_sysProcCreate
 add sp,sp,#6*4
 mov r1,r10 
 mov r2,r9 
 subs r2,r2,#MSG_HEAD
 str r1,[r2,#MSG_SENDER]
 mov r2,r8
 str r0,[r2,#SC_PROCCREATEMSG_REPLY_PID]
 movs r1,#SC_PROCCREATEMSG_REPLY_SZ
 b k_reply
k_err:
 ldr r0,k_lit2
 movs r1,r4
 orrs r0,r0,r5
 bl sc_sysError
 LALIGN
k_sciopta:
 LONG sciopta
k_lit2:
 LONG KERNEL_EOUT_OF_MEMORY|SC_ERR_MODULE_FATAL
k_lit1:
 LONG SC_PCB_MAGIC
k_lit0:
 LONG SC_PROCPRIOCREATE
 LONG SC_PROCINTCREATE
 LONG SC_PROCUSRINTCREATE
 LONG SC_PROCTIMCREATE
k_sc_modules:
 LONG sc_modules
k_select:
 LONG SC_PROCPRIOCREATEMSG
 LONG SC_PROCINTCREATEMSG
 LONG SC_PROCUSRINTCREATEMSG
 LONG SC_PROCTIMCREATEMSG
 LONG SC_PROCKILLMSG
 LONG SC_MODULECREATEMSG
 LONG SC_MODULEKILLMSG
#ifdef SC_POOLCREATEMSG
 LONG SC_POOLCREATEMSG
 LONG SC_POOLRESETMSG
 LONG SC_POOLKILLMSG
#endif
 LONG 0
 SC_ENDFUNC sc_kerneld
#undef MSG
#undef REPLY
 IF SC_MAX_CONNECTOR > 0
 XSC_TFUNC sc_connectorRegister,-12
 SC_TFUNC sc_connectorRegister
 push {r7,lr}
 ldr r3,cr_sciopta
 ldr r2,[r3,#SC_CURPCB]
 ldrh r1,[r2,#PCB_FLAGS_H]
 lsrs r1,r1,#PCB_FLG_IS_CONNECTOR_POS+1
 bcs cr_err
 ldr r3,cr_sc_connectors
 cmp r0,#0
 beq cr_nodef
 LOCK
 ldr r0,[r3,#0]
 cmp r0,#0
 bne cr_err0
 str r2,[r3,#0]
 ldrh r3,[r2,#PCB_FLAGS_H]
 movs r1,#1
 lsls r1,r1,#PCB_FLG_IS_CONNECTOR_POS
 orrs r3,r3,r1
 strh r3,[r2,#PCB_FLAGS_H]
 UNLOCK
 movs r0,#0x80
 lsls r0,r0,#24
 RET r7
cr_nodef:
 movs r0,#0
 movs r7,#0
 LOCK
cr1:
 adds r0,r0,#4
 cmp r0,#SC_MAX_CONNECTOR*4
 beq cr2
 ldr r1,[r3,r0] 
 cmp r1,#1 
 blo cr3 
 bne cr1 
 cmp r7,#0 
 bne cr1 
 movs r7,r0 
 b cr1
cr2:
 adds r0,r7,#0 
 beq cr_err1 
cr3: 
 str r2,[r3,r0] 
 ldrh r3,[r2,#PCB_FLAGS_H]
 movs r1,#1
 lsls r1,r1,#PCB_FLG_IS_CONNECTOR_POS
 orrs r3,r3,r1
 strh r3,[r2,#PCB_FLAGS_H]
 UNLOCK
 lsrs r0,r0,#2 
 adds r0,#0x80
 lsls r0,r0,#24
 RET r7
cr_err1:
 UNLOCK
 ldr r0,cr_lit1
 b cr_err_cont
cr_err0:
 UNLOCK
cr_err:
 movs r1,r2
 ldr r0,cr_lit0
cr_err_cont: 
 bl sc_sysError
 LALIGN
cr_sc_connectors:
 LONG sc_connectors
cr_sciopta:
 LONG sciopta
cr_lit0:
 LONG SC_CONNECTORREGISTER|KERNEL_EALREADY_DEFINED|SC_ERR_SYSTEM_FATAL
cr_lit1:
 LONG SC_CONNECTORREGISTER|KERNEL_ENO_MORE_CONNECTOR|SC_ERR_SYSTEM_FATAL
 SC_ENDFUNC sc_connectorRegister
 XSC_TFUNC sc_connectorUnregister,-16
 SC_TFUNC sc_connectorUnregister
 ldr r3,cur_sciopta
 ldr r2,[r3,#SC_CURPCB]
 ldrh r1,[r2,#PCB_FLAGS_H]
 lsrs r0,r1,#PCB_FLG_IS_CONNECTOR_POS+1
 bcc cur_err
 ldr r3,cur_sc_connectors
 movs r1,#SC_MAX_CONNECTOR
cur0:
 ldr r0,[r3,#4]
 adds r3,r3,#4
 subs r1,r1,#1
 bmi cur_err
 cmp r0,r2
 bne cur0
 movs r0,#1
 str r0,[r3,#0]
 lsls r0,r0,#PCB_FLG_IS_CONNECTOR_POS
 mov r3,lr
 LOCK
 ldrh r1,[r2,#PCB_FLAGS_H]
 bics r1,r1,r0
 strh r1,[r2,#PCB_FLAGS_H]
 UNLOCK
 bx r3
cur_err:
 push {lr}
 ldr r0,cur_lit0
 movs r1,#0
 bl sc_sysError
 EXIT
 LALIGN
cur_sciopta:
 LONG sciopta
cur_sc_connectors:
 LONG sc_connectors-4
cur_lit0:
 LONG SC_CONNECTORUNREGISTER|SC_ERR_SYSTEM_FATAL
 SC_ENDFUNC sc_connectorUnregister
 ENDIF
 IF SC_USE_TRAP == 1
 SC_FUNC sc_sysCall
syscall_tab:
 LONG 0
 LONG sc_msgAlloc
 LONG sc_msgFree
 LONG sc_msgAddrGet
 LONG sc_msgSndGet
 LONG sc_msgSizeGet
 LONG sc_msgSizeSet
 LONG sc_msgOwnerGet
 LONG sc_msgTx
 LONG sc_msgTxAlias
 LONG sc_msgRx
 LONG sc_msgPoolIdGet
 LONG sc_msgAcquire
 LONG sc_msgAllocClr
 IF SC_MSG_HOOK == 1 
 LONG sc_msgHookRegister
 ELSE
 LONG 0
 ENDIF
 LONG 0 
 LONG 0 
 LONG sc_poolReset
 LONG sc_poolKill
 LONG sc_poolInfo
 LONG sc_poolDefault
 LONG sc_poolIdGet
 LONG 0 
 IF SC_POOL_HOOK == 1
 LONG sc_poolHookRegister
 ELSE
 LONG 0
 ENDIF
 IF SC_ERR_HOOK == 1
 LONG sc_miscErrorHookRegister
 ELSE 
 LONG 0 
 ENDIF
 LONG sc_miscKerneldRegister
 LONG sc_miscCrcContd
 LONG sc_miscCrc
 LONG sc_miscErrnoSet
 LONG sc_miscErrnoGet
 LONG sc_procWakeupEnable
 LONG sc_procWakeupDisable
 LONG sc_procPrioGet
 LONG sc_procPrioSet
 LONG sc_procSliceGet
 LONG sc_procSliceSet
 LONG sc_procIdGet
 LONG sc_procPpidGet
 LONG sc_procNameGet
 LONG sc_procStart
 LONG sc_procStop
 LONG sc_procVarInit
 LONG sc_procSchedUnlock
 LONG 0 
 LONG 0 
 LONG 0 
 LONG 0 
 LONG 0 
 LONG 0 
 LONG 0 
 LONG 0 
 LONG sc_procKill
 LONG sc_procYield
 LONG sc_procObserve
 LONG 0 
 LONG sc_procSchedLock
 LONG sc_procVarGet
 LONG sc_procVarSet
 LONG sc_procVarDel
 LONG sc_procVarRm
 LONG sc_procUnobserve
 LONG sc_procPathGet
 LONG sc_procPathCheck
 IF SC_PROC_HOOK == 1
 LONG sc_procHookRegister
 ELSE
 LONG 0 
 ENDIF 
 LONG 0 
 LONG sc_moduleKill
 LONG sc_moduleNameGet
 LONG sc_moduleIdGet
 LONG sc_moduleInfo
 LONG sc_sysModulePrioSet
 LONG sc_sysModulePrioGet
 IF SC_USE_FRIENDS == 1
 LONG sc_moduleFriendAdd
 LONG sc_moduleFriendRm
 LONG sc_moduleFriendGet
 LONG sc_moduleFriendNone
 LONG sc_moduleFriendAll
 ELSE
 LONG 0
 LONG 0
 LONG 0
 LONG 0
 LONG 0
 ENDIF 
 LONG 0 
 LONG 0 
 LONG sc_procDaemonUnregister
 LONG sc_procDaemonRegister
 LONG sc_triggerValueSet
 LONG sc_triggerValueGet
 LONG sc_trigger
 LONG sc_triggerWait
 LONG 0 
 LONG sc_miscError
 LONG 0 
 LONG sc_tick
 IF SC_ASYNC_TMO == 1
 LONG sc_tmoAdd
 ELSE
 LONG 0
 ENDIF
 LONG 0
 LONG sc_sleep
 IF SC_ASYNC_TMO == 1
 LONG sc_tmoRm
 ELSE
 LONG 0
 ENDIF
 LONG sc_tickGet
 LONG sc_tickLength
 LONG sc_tickMs2Tick
 LONG sc_tickTick2Ms
 IF SC_MAX_CONNECTOR > 0
 LONG sc_connectorRegister
 LONG sc_connectorUnregister
 ELSE
 LONG 0
 LONG 0
 ENDIF
 LONG 0 
 LONG 0
 IF SC_CDEBUG == 1
xsyscall_tab:
 LONG 0
 LONG xxsc_msgAlloc
 LONG xsc_msgFree
 LONG xsc_msgAddrGet
 LONG xsc_msgSndGet
 LONG xsc_msgSizeGet
 LONG xsc_msgSizeSet
 LONG xsc_msgOwnerGet
 LONG xsc_msgTx
 LONG xxsc_msgTxAlias
 LONG xsc_msgRx
 LONG xsc_msgPoolIdGet
 LONG xsc_msgAcquire
 LONG xxsc_msgAllocClr
 IF SC_MSG_HOOK == 1 
 LONG xsc_msgHookRegister
 ELSE
 LONG 0
 ENDIF
 LONG 0 
 LONG 0 
 LONG xsc_poolReset
 LONG xsc_poolKill
 LONG xsc_poolInfo
 LONG xsc_poolDefault
 LONG xsc_poolIdGet
 LONG 0 
 IF SC_POOL_HOOK == 1
 LONG xsc_poolHookRegister
 ELSE
 LONG 0
 ENDIF
 IF SC_ERR_HOOK == 1
 LONG xsc_miscErrorHookRegister
 ELSE 
 LONG 0 
 ENDIF
 LONG xsc_miscKerneldRegister
 LONG xsc_miscCrcContd
 LONG xsc_miscCrc
 LONG xsc_miscErrnoSet
 LONG xsc_miscErrnoGet
 LONG xsc_procWakeupEnable
 LONG xsc_procWakeupDisable
 LONG xsc_procPrioGet
 LONG xsc_procPrioSet
 LONG xsc_procSliceGet
 LONG xsc_procSliceSet
 LONG xsc_procIdGet
 LONG xsc_procPpidGet
 LONG xsc_procNameGet
 LONG xsc_procStart
 LONG xsc_procStop
 LONG xsc_procVarInit
 LONG xsc_procSchedUnlock
 LONG 0 
 LONG 0 
 LONG 0 
 LONG 0 
 LONG 0 
 LONG 0 
 LONG 0 
 LONG 0 
 LONG xsc_procKill
 LONG xsc_procYield
 LONG xsc_procObserve
 LONG 0 
 LONG xsc_procSchedLock
 LONG xsc_procVarGet
 LONG xsc_procVarSet
 LONG xsc_procVarDel
 LONG xsc_procVarRm
 LONG xsc_procUnobserve
 LONG xsc_procPathGet
 LONG xsc_procPathCheck
 IF SC_PROC_HOOK == 1
 LONG xsc_procHookRegister
 ELSE
 LONG 0 
 ENDIF 
 LONG 0 
 LONG xsc_moduleKill
 LONG xsc_moduleNameGet
 LONG xsc_moduleIdGet
 LONG xsc_moduleInfo
 LONG xsc_sysModulePrioSet
 LONG xsc_sysModulePrioGet
 IF SC_USE_FRIENDS == 1
 LONG xsc_moduleFriendAdd
 LONG xsc_moduleFriendRm
 LONG xsc_moduleFriendGet
 LONG xsc_moduleFriendNone
 LONG xsc_moduleFriendAll
 ELSE
 LONG 0
 LONG 0
 LONG 0
 LONG 0
 LONG 0
 ENDIF 
 LONG 0 
 LONG 0 
 LONG xsc_procDaemonUnregister
 LONG xsc_procDaemonRegister
 LONG xsc_triggerValueSet
 LONG xsc_triggerValueGet
 LONG xsc_trigger
 LONG xsc_triggerWait
 LONG 0 
 LONG xsc_miscError
 LONG 0 
 LONG xsc_tick
 IF SC_ASYNC_TMO == 1
 LONG xsc_tmoAdd
 ELSE
 LONG 0
 ENDIF
 LONG 0
 LONG xsc_sleep
 IF SC_ASYNC_TMO == 1
 LONG xsc_tmoRm
 ELSE
 LONG 0
 ENDIF
 LONG xsc_tickGet
 LONG xsc_tickLength
 LONG xsc_tickMs2Tick
 LONG xsc_tickTick2Ms
 IF SC_MAX_CONNECTOR > 0
 LONG xsc_connectorRegister
 LONG xsc_connectorUnregister
 ELSE
 LONG 0
 LONG 0
 ENDIF
 LONG 0 
 LONG 0
 ENDIF
 SC_ENDFUNC sc_sysCall
 IF SC_CDEBUG == 1
 XXSC_TFUNC sc_msgAlloc,0
 XXSC_TFUNC sc_msgAllocClr,0
 XXSC_TFUNC sc_msgTxAlias,0
 ENDIF
 ENDIF
#if defined __IAR5__
 END
#endif
#if defined __IAR__
 ENDMOD 
 MODULE resethook
 RSEG ICODE:CODE:NOROOT(2)
 EXPORT reset_hook
 CODE16
reset_hook:
 mov r0,#0
 bx lr
 ENDMOD
 MODULE starthook
 RSEG CODE:CODE:NOROOT(2)
 EXPORT start_hook
 CODE16
start_hook:
 bx lr
 ENDMOD
 MODULE user_swi_handler
 RSEG CODE:CODE:NOROOT(2)
 EXPORT user_swi_handler
 IMPORT sc_sysError
 CODE32
user_swi_handler:
 ldr r0,=(0x05000000|KERNEL_EILL_EXCEPTION|SC_ERR_FATAL)
 mov r1,lr
 ldr lr,=sc_sysError+1
 bx lr
 ENDMOD
#endif
