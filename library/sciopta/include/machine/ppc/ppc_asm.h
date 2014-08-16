/* -*-c++-*-
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
** ID: S03076BS13                                                   **
** +Revision: 1.25.4.2 +                                                **
** +Date: 2009/10/01 12:52:59 +                                     ** 
** PowerPC register definitions                                     **
**********************************************************************
*/
#ifdef __ASM_INCLUDED__
#define SAVED_XER	0
#define SAVED_CTR	4
#define SAVED_FULL_FLG  8
#define SAVED_CR	8
#define SAVED_LR	12
#define SAVED_MSR	16
#define SAVED_PC	20
#else
#define SAVED_XER	0
#define SAVED_CTR	4
#define SAVED_FULL_FLG  8
#define SAVED_CR	12
#define SAVED_LR	16
#define SAVED_MSR	20
#define SAVED_PC	24
#endif
#define SAVED_R2	(SAVED_PC+4)
#define SAVED_R3	(SAVED_PC+2*4)
#define SAVED_R4	(SAVED_PC+3*4)
#define SAVED_R5	(SAVED_PC+4*4)
#define SAVED_R6	(SAVED_PC+5*4)
#define SAVED_R7	(SAVED_PC+6*4)
#define SAVED_R8	(SAVED_PC+7*4)
#define SAVED_R9	(SAVED_PC+8*4)
#define SAVED_R10	(SAVED_PC+9*4)
#define SAVED_R11	(SAVED_PC+10*4)
#define SAVED_R12	(SAVED_PC+11*4)
#define SAVED_R13	(SAVED_PC+12*4)
#define SAVED_R14	(SAVED_PC+13*4)
#define SAVED_R15	(SAVED_PC+14*4)
#define SAVED_R16	(SAVED_PC+15*4)
#define SAVED_R17	(SAVED_PC+16*4)
#define SAVED_R18	(SAVED_PC+17*4)
#define SAVED_R19	(SAVED_PC+18*4)
#define SAVED_R20	(SAVED_PC+19*4)
#define SAVED_R21	(SAVED_PC+20*4)
#define SAVED_R22	(SAVED_PC+21*4)
#define SAVED_R23	(SAVED_PC+22*4)
#define SAVED_R24	(SAVED_PC+23*4)
#define SAVED_R25	(SAVED_PC+24*4)
#define SAVED_R26	(SAVED_PC+25*4)
#define SAVED_R27	(SAVED_PC+26*4)
#define SAVED_R28	(SAVED_PC+27*4)
#define SAVED_R29	(SAVED_PC+28*4)
#define SAVED_R30	(SAVED_PC+29*4)
#define SAVED_R31       (SAVED_PC+30*4)
#define SAVED_R0	(SAVED_PC+31*4)

#define SAVED_FR0	(SAVED_R0+4)
#define SAVED_FR1	(SAVED_FR0+1*8)
#define SAVED_FR2	(SAVED_FR0+2*8)
#define SAVED_FR3	(SAVED_FR0+3*8)
#define SAVED_FR4	(SAVED_FR0+4*8)
#define SAVED_FR5	(SAVED_FR0+5*8)
#define SAVED_FR6	(SAVED_FR0+6*8)
#define SAVED_FR7	(SAVED_FR0+7*8)
#define SAVED_FR8	(SAVED_FR0+8*8)
#define SAVED_FR9	(SAVED_FR0+9*8)
#define SAVED_FR10	(SAVED_FR0+10*8)
#define SAVED_FR11	(SAVED_FR0+11*8)
#define SAVED_FR12	(SAVED_FR0+12*8)
#define SAVED_FR13	(SAVED_FR0+13*8)
#define SAVED_FR14	(SAVED_FR0+14*8)
#define SAVED_FR15	(SAVED_FR0+15*8)
#define SAVED_FR16	(SAVED_FR0+16*8)
#define SAVED_FR17	(SAVED_FR0+17*8)
#define SAVED_FR18	(SAVED_FR0+18*8)
#define SAVED_FR19	(SAVED_FR0+19*8)
#define SAVED_FR20	(SAVED_FR0+20*8)
#define SAVED_FR21	(SAVED_FR0+21*8)
#define SAVED_FR22	(SAVED_FR0+22*8)
#define SAVED_FR23	(SAVED_FR0+23*8)
#define SAVED_FR24	(SAVED_FR0+24*8)
#define SAVED_FR25	(SAVED_FR0+25*8)
#define SAVED_FR26	(SAVED_FR0+26*8)
#define SAVED_FR27	(SAVED_FR0+27*8)
#define SAVED_FR28	(SAVED_FR0+28*8)
#define SAVED_FR29	(SAVED_FR0+29*8)
#define SAVED_FR30	(SAVED_FR0+30*8)
#define SAVED_FR31      (SAVED_FR0+31*8)


#define r0 0
#define r1 1
#define r2 2
#define r3 3
#define r4 4
#define r5 5
#define r6 6
#define r7 7
#define r8 8
#define r9 9
#define r10 10
#define r11 11
#define r12 12
#define r13 13
#define r14 14
#define r15 15
#define r16 16
#define r17 17
#define r18 18
#define r19 19
#define r20 20
#define r21 21
#define r22 22
#define r23 23
#define r24 24
#define r25 25
#define r26 26
#define r27 27
#define r28 28
#define r29 29
#define r30 30
#define r31 31

#define fr0 0
#define fr1 1
#define fr2 2
#define fr3 3
#define fr4 4
#define fr5 5
#define fr6 6
#define fr7 7
#define fr8 8
#define fr9 9
#define fr10 10
#define fr11 11
#define fr12 12
#define fr13 13
#define fr14 14
#define fr15 15
#define fr16 16
#define fr17 17
#define fr18 18
#define fr19 19
#define fr20 20
#define fr21 21
#define fr22 22
#define fr23 23
#define fr24 24
#define fr25 25
#define fr26 26
#define fr27 27
#define fr28 28
#define fr29 29
#define fr30 30
#define fr31 31


#define sr0 0
#define sr1 1
#define sr2 2
#define sr3 3
#define sr4 4
#define sr5 5
#define sr6 6
#define sr7 7
#define sr8 8
#define sr9 9
#define sr10 10
#define sr11 11
#define sr12 12
#define sr13 13
#define sr14 14
#define sr15 15

#define dar  19
#define dec  22
#define srr0 26
#define srr1 27
#define der  149

#define pid   48
#define decar 54

/* exection address and sysndrom (BookE) */
#define dear 61
#define DEAR dear
#define esr  62
#define ESR  esr

/* criticial interrupt (BookE) */
#define csrr0 58
#define csrr1 59

/* timebase control (BookE) */
#define tsr      336
#define TSR_ENW        0x80000000
#define TSR_WIS        0x40000000
#define TSR_WRS_NONE   0x00000000
#define TSR_WRS_CHKSTP 0x10000000
#define TSR_WRS_RESET  0x20000000
#define TSR_DIS        0x08000000
#define TSR_FIS        0x04000000

#define tcr      340
#define TCR_WRC_NONE   0x00000000
#define TCR_WRC_CHKSTP 0x10000000
#define TCR_WRC_RESET  0x20000000
#define TCR_WIE        0x08000000
#define TCR_DIE        0x04000000
#define TCR_FIE        0x00800000
#define TCR_ARE        0x00400000

/* Interrupt vector registers (BookE) */
#define IPVR     63
#define IVOR0    400
#define IVOR1    401
#define IVOR2    402
#define IVOR3    403
#define IVOR4    404
#define IVOR5    405
#define IVOR6    406
#define IVOR7    407
#define IVOR8    408
#define IVOR9    409
#define IVOR10   410
#define IVOR11   411
#define IVOR12   412
#define IVOR13   413
#define IVOR14   414
#define IVOR15   415
#define IVOR32   528
#define IVOR33   529
#define IVOR34   530

#define sprg0 272
#define sprg1 273
#define sprg2 274
#define sprg3 275
/* GE2_LE and BookE only */
#define sprg4 276
#define sprg5 277
#define sprg6 278
#define sprg7 279

#define sprg4_usr 260
#define sprg5_usr 261
#define sprg6_usr 262
#define sprg7_usr 263

#define usprg0 256

/* common registers */
#define TBL_rd 268
#define TBU_rd 269

#define TBL_wr 284
#define TBU_wr 285

#define cr0      0
#define cr1      4
#define cr2      8
#define cr3      12
#define cr4      16
#define cr5      20
#define cr6      24
#define cr7      28

#define CR0_LT 0
#define CR0_GT 1
#define CR0_EQ 2
#define CR0_SO 3

#define CR1_LT 4
#define CR1_GT 5
#define CR1_EQ 6
#define CR1_SO 7

#define CR2_LT 8
#define CR2_GT 9
#define CR2_EQ 10
#define CR2_SO 11

#define CR3_LT 12
#define CR3_GT 13
#define CR3_EQ 14
#define CR3_SO 15

#define CR3_LT 12
#define CR3_GT 13
#define CR3_EQ 14
#define CR3_SO 15

#define CR4_LT 16
#define CR4_GT 17
#define CR4_EQ 18
#define CR4_SO 19

#define CR5_LT 20
#define CR5_GT 21
#define CR5_EQ 22
#define CR5_SO 23

#define CR6_LT 24
#define CR6_GT 25
#define CR6_EQ 26
#define CR6_SO 27

#define CR7_LT 28
#define CR7_GT 29
#define CR7_EQ 30
#define CR7_SO 31

#define IMMR 638

#if CPUTYPE == MPC8xx || CPUTYPE == MPC5xx
#define EIE   80
#define EID   81
#define NRI   82
#endif

#if CPUTYPE == MPC5xx
#define MI_GRA   528
#define EIBADR   529
#define L2U_GRA  536
#define BBC_MCR  560
#define L2U_MCR  568

#define MI_RBA0  784
#define MI_RBA1  785
#define MI_RBA2  786
#define MI_RBA3  787

#define L2U_RBA0 792
#define L2U_RBA1 793
#define L2U_RBA2 794
#define L2U_RBA3 795

#define MI_RA0   816
#define MI_RA1   817
#define MI_RA2   818
#define MI_RA3   819

#define L2U_RA0  824
#define L2U_RA1  825
#define L2U_RA2  826
#define L2U_RA3  827

#define FPECR    1022

#endif /* CPUTYPE == MPC5xx */


#if CPUTYPE == MPC8xx
#define IC_CST 560
#define IC_ADR 561
#define IC_DAT 562
#define DC_CST 568
#define DC_ADR 569
#define DC_DAT 570

#define XC_CMD_EN 0x02000000
#define XC_CMD_DI 0x04000000
#define XC_CMD_LL 0x06000000
#define XC_CMD_UL 0x08000000
#define XC_CMD_UA 0x0A000000
#define XC_CMD_IA 0x0C000000
#define XC_CMD_XX 0x0E000000
#endif

/* Instruction BAT register */
#define ibat0u   528
#define ibat0l   529
#define ibat1u   530
#define ibat1l   531
#define ibat2u   532
#define ibat2l   533
#define ibat3u   534
#define ibat3l   535

/* Data BAT register */
#define dbat0u   536
#define dbat0l   537
#define dbat1u   538
#define dbat1l   539
#define dbat2u   540
#define dbat2l   541
#define dbat3u   542
#define dbat3l   543

// BAT bit definitions
#define BATU_BEPI	0xFFFE0000
#define BATU_BL_128K	0x00000000
#define BATU_BL_256K	0x00000004
#define BATU_BL_512K	0x0000000C
#define BATU_BL_1M	0x0000001C
#define BATU_BL_2M	0x0000003C
#define BATU_BL_4M	0x0000007C
#define BATU_BL_8M	0x000000FC
#define BATU_BL_16M	0x000001FC
#define BATU_BL_32M	0x000003FC
#define BATU_BL_64M	0x000007FC
#define BATU_BL_128M	0x00000FFC
#define BATU_BL_256M	0x00001FFC
#define BATU_SUPERVISOR	0x00000002
#define BATU_USER	0x00000001
#define BATU_INVALID	0x00000000

#define BATL_BRPN	0xFFFE0000
#define BATL_WRITE_THR	0x00000040
#define BATL_CACHE_INH	0x00000020
#define BATL_COHERENCY	0x00000010
#define BATL_GUARDED	0x00000008
#define BATL_READ_WRITE	0x00000002
#define BATL_READ_ONLY	0x00000001
#define BATL_NO_ACCESS	0x00000000


#if CPUTYPE == MPC5200 || CPUTYPE == MPC82XX
#define iabr     1010

/* Software table search */
#define DMISS    976
#define DCMP     977
#define HASH1    978
#define HASH2    979
#define IMISS    980
#define ICMP     981
#define RPA      982
#endif

#if CPUTYPE == MPC5200
#define hid0     1008
#define hid1     1009
#define hid2     1011

#define ear      282
#define mbar     311
#define iabr2    1018
#define dabr     1013
#define dabr2    317
#define dsisr    18
#define svr      286
/* Instruction BAT register (GE2_LE)*/
#define ibat4u   560
#define ibat4l   561
#define ibat5u   562
#define ibat5l   563
#define ibat6u   564
#define ibat6l   565
#define ibat7u   566
#define ibat7l   567

/* Data BAT register (GE2_LE) */
#define dbat4u   568
#define dbat4l   569
#define dbat5u   570
#define dbat5l   571
#define dbat6u   572
#define dbat6l   573
#define dbat7u   574
#define dbat7l   575

#define HID0_EMCP   0x80000000
#define HID0_EBA    0x20000000
#define HID0_EBD    0x10000000
#define HID0_SBCLK  0x08000000
#define HID0_ECLK   0x02000000
#define HID0_PAR    0x01000000
#define HID0_DOZE   0x00800000
#define HID0_NAP    0x00400000
#define HID0_SLEEP  0x00200000
#define HID0_DPM    0x00100000
#define HID0_ICE    0x00008000
#define HID0_DCE    0x00004000
#define HID0_ILOCK  0x00002000
#define HID0_DLOCK  0x00001000
#define HID0_ICFI   0x00000800
#define HID0_DCFI   0x00000400
#define HID0_IFEM   0x00000080
#define HID0_FIOB   0x00000010
#define HID0_ABE    0x00000008
#define HID0_NOOPTI 0x00000001

#define HID1_PC0    0x80000000
#define HID1_PC1    0x40000000
#define HID1_PC2    0x20000000
#define HID1_PC3    0x10000000
#define HID1_PC4    0x08000000

#define HID2_LET        0x08000000
#define HID2_HBE        0x00040000
#define HID2_IWLCK_NONE 0x00000000
#define HID2_IWLCK_W0   0x00002000
#define HID2_IWLCK_W1   0x00004000
#define HID2_IWLCK_W2   0x00006000
#define HID2_IWLCK_W3   0x00008000
#define HID2_IWLCK_W4   0x0000A000
#define HID2_IWLCK_W5   0x0000C000

#define HID2_DWLCK_NONE 0x00000000
#define HID2_DWLCK_W0   0x00000020
#define HID2_DWLCK_W1   0x00000040
#define HID2_DWLCK_W2   0x00000060
#define HID2_DWLCK_W3   0x00000080
#define HID2_DWLCK_W4   0x000000A0
#define HID2_DWLCK_W5   0x000000C0

#endif /* MPC5200 */

#if CPUTYPE == MPC5500
#define hid0     1008
#define hid1     1009
#define HID0     hid0
#define HID1     hid1
	
#define HID0_TBE 0x00004000

#define l1csr0 1010
#define L1CSR0 l1csr0

#define L1CSR0_CWM  0x00100000
#define	L1CSR0_CINV 0x00000002
#define L1CSR0_CE   0x00000001

#define MAS0    0x270
#define MAS1    0x271
#define MAS2    0x272
#define MAS3    0x273
#define MAS4    0x274
#define MAS6    0x276

#define MAS1_VALID       0x80000000
#define MAS1_IPROT       0x40000000
#define MAS1_TS          0x00001000
#define MAS1_TSIZE_1KB   (0<<8)
#define MAS1_TSIZE_4KB   (1<<8)
#define MAS1_TSIZE_16KB  (2<<8)
#define MAS1_TSIZE_64KB  (3<<8)
#define MAS1_TSIZE_256KB (4<<8)
#define MAS1_TSIZE_1MB   (5<<8)
#define MAS1_TSIZE_4MB   (6<<8)
#define MAS1_TSIZE_16MB  (7<<8)
#define MAS1_TSIZE_64MB  (8<<8)
#define MAS1_TSIZE_256MB (9<<8)

#define SZ_1K	    0x00000000
#define SZ_4K	    0x00000010
#define SZ_16K	    0x00000020
#define SZ_64K	    0x00000030
#define SZ_256K	    0x00000040
#define SZ_1M	    0x00000050
#define SZ_8M       0x00000060
#define SZ_16M	    0x00000070
#define SZ_256M	    0x00000090

#define MAS2_W 0x10
#define MAS2_I 0x08
#define MAS2_M 0x04
#define MAS2_G 0x02
#define MAS2_E 0x01
#define MAS2_EPN(n) ((n) & 0xfffff000)

#define MAS3_UX 0x20
#define MAS3_SX 0x10
#define MAS3_UW 0x08
#define MAS3_SW 0x04
#define MAS3_UR 0x02
#define MAS3_SR 0x01
#define MAS3_RPN(n) ((n) & 0xfffff000)

#endif

/* e500 and e200 */
#define spefscr 512
#define SPEFSCR 512


#define MSR_LE   0x00000001
#define MSR_RI   0x00000002
#define MSR_DS   0x00000010      /* BOOKE */
#define MSR_IS   0x00000020      /* BOOKE */
#define MSR_DR   0x00000010
#define MSR_IR   0x00000020
#define MSR_IP   0x00000040
#define MSR_CE   0x00000080      /* BOOKE */
#define MSR_FE1  0x00000100
#define MSR_BE   0x00000200
#define MSR_SE   0x00000400
#define MSR_FE0  0x00000800
#define MSR_ME   0x00001000
#define MSR_FP   0x00002000
#define MSR_PR   0x00004000
#define MSR_EE   0x00008000
#define MSR_ILE  0x00010000
#define MSR_TGPR 0x00020000     /* GE2_LE */
#define MSR_POW  0x00040000
#define MSR_SPE  0x02000000     /* e500 and e200 */
#define MSR_UCLE 0x04000000     /* e500 and e200 */

#define MSR_SPE_POS   6
#define MSR_FP_POS   18
#define MSR_PR_POS   17
#define MSR_EE_POS   16
#define MSR_ME_POS   19
#define MSR_IS_POS   26
#define MSR_DS_POS   27
#define MSR_RI_POS   30

#if CPUTYPE == AMCC440GP || CPUTYPE == AMCC440EP || CPUTYPE == AMCC460EX
#define rstcfg 0x39b
#define mmucr  0x3b2
#define ccr0   0x3b3
#endif /* CPUTYPE == AMCC440GP */

	/* helper macros */
	
#ifdef __GNUC__
#define TEXT(name) .section name,"ax",@progbits
#elif defined __MWERKS__
#define TEXT(name) .section name,4,PROGBITS
#elif defined __DCC__
#define TEXT(name) .section name,,c
#else
#error Unsupported Compiler
#endif

#define FUNC(name) \
        .align  2               ; \
	.globl	name 		; \
	.type	name,@function	;\
name:

#define END_FUNC(name)		\
        .align 2               ;\
	.size	name,.-name

	
