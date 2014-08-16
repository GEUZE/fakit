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
** (c) 2002,2003 Sciopta GmbH, Litronic AG/ Schweiz                 **
**                                                                  **
**********************************************************************
** ID: S03076BS3                                                    **
** +Revision: 1.4 +                                                 **
** +Date: 2003/08/05 11:04:31 +                                     ** 
** PowerPC MMU definitions                                          **
**********************************************************************
*/

/* These definitions are partly taken from Linux sources */

/* Block size masks */
#define BL_128K	0x000
#define BL_256K 0x001
#define BL_512K 0x003
#define BL_1M   0x007
#define BL_2M   0x00F
#define BL_4M   0x01F
#define BL_8M   0x03F
#define BL_16M  0x07F
#define BL_32M  0x0FF
#define BL_64M  0x1FF
#define BL_128M 0x3FF
#define BL_256M 0x7FF

/* BAT Access Protection */
#define BPP_XX	0x00		/* No access */
#define BPP_RX	0x01		/* Read only */
#define BPP_RW	0x02		/* Read/write */

/* Control/status registers for the MPC8xx.
 * A write operation to these registers causes serialized access.
 * During software tablewalk, the registers used perform mask/shift-add
 * operations when written/read.  A TLB entry is created when the Mx_RPN
 * is written, and the contents of several registers are used to
 * create the entry.
 */
#define MI_CTR		784	/* Instruction TLB control register */
#define MI_GPM		0x80000000	/* Set domain manager mode */
#define MI_PPM		0x40000000	/* Set subpage protection */
#define MI_CIDEF	0x20000000	/* Set cache inhibit when MMU dis */
#define MI_RSV4I	0x08000000	/* Reserve 4 TLB entries */
#define MI_PPCS		0x02000000	/* Use MI_RPN prob/priv state */
#define MI_IDXMASK	0x00001f00	/* TLB index to be loaded */
#define MI_RESETVAL	0x00000000	/* Value of register at reset */

/* These are the Ks and Kp from the PowerPC books.  For proper operation,
 * Ks = 0, Kp = 1.
 */
#define MI_AP		786
#define MI_Ks		0x80000000	/* Should not be set */
#define MI_Kp		0x40000000	/* Should always be set */

/* The effective page number register.  When read, contains the information
 * about the last instruction TLB miss.  When MI_RPN is written, bits in
 * this register are used to create the TLB entry.
 */
#define MI_EPN		787
#define MI_EPNMASK	0xfffff000	/* Effective page number for entry */
#define MI_EVALID	0x00000200	/* Entry is valid */
#define MI_ASIDMASK	0x0000000f	/* ASID match value */
					/* Reset value is undefined */

/* A "level 1" or "segment" or whatever you want to call it register.
 * For the instruction TLB, it contains bits that get loaded into the
 * TLB entry when the MI_RPN is written.
 */
#define MI_TWC		789
#define MI_APG		0x000001e0	/* Access protection group (0) */
#define MI_GUARDED	0x00000010	/* Guarded storage */
#define MI_PSMASK	0x0000000c	/* Mask of page size bits */
#define MI_PS8MEG	0x0000000c	/* 8M page size */
#define MI_PS512K	0x00000004	/* 512K page size */
#define MI_PS4K_16K	0x00000000	/* 4K or 16K page size */
#define MI_SVALID	0x00000001	/* Segment entry is valid */
					/* Reset value is undefined */

/* Real page number.  Defined by the pte.  Writing this register
 * causes a TLB entry to be created for the instruction TLB, using
 * additional information from the MI_EPN, and MI_TWC registers.
 */
#define MI_RPN		790
#define MI_RPN_MASK     0xfffff000
/*     supervisor  user             */
/*             \  /                 */
#define MI_RPN_RORO     0x00000C00
#define MI_RPN_RWRW     0x00000800
#define MI_RPN_RWRO     0x00000400
#define MI_RPN_RWNO     0x00000000
#define MI_RPN_PPC_MODE 0x00000200
#define MI_RPN_SPS      0x00000008
#define MI_RPN_SH       0x00000004
#define MI_RPN_CI       0x00000002
#define MI_RPN_VALID    0x00000001


/* Define an RPN value for mapping kernel memory to large virtual
 * pages for boot initialization.  This has real page number of 0,
 * large page size, shared page, cache enabled, and valid.
 * Also mark all subpages valid and write access.
 */

#define MI_BOOTINIT	(0x000001f0|MI_RPN_RWRW|MI_RPN_SH|MI_RPN_VALID|MI_RPN_SPS)

#define MD_CTR		792	/* Data TLB control register */
#define MD_GPM		0x80000000	/* Set domain manager mode */
#define MD_PPM		0x40000000	/* Set subpage protection */
#define MD_CIDEF	0x20000000	/* Set cache inhibit when MMU dis */
#define MD_WTDEF	0x10000000	/* Set writethrough when MMU dis */
#define MD_RSV4I	0x08000000	/* Reserve 4 TLB entries */
#define MD_TWAM		0x04000000	/* Use 4K page hardware assist */
#define MD_PPCS		0x02000000	/* Use MI_RPN prob/priv state */
#define MD_IDXMASK	0x00001f00	/* TLB index to be loaded */
#define MD_RESETVAL	0x04000000	/* Value of register at reset */

#define M_CASID		793	/* Address space ID (context) to match */
#define MC_ASIDMASK	0x0000000f	/* Bits used for ASID value */


/* These are the Ks and Kp from the PowerPC books.  For proper operation,
 * Ks = 0, Kp = 1.
 */
#define MD_AP		794
#define MD_Ks		0x80000000	/* Should not be set */
#define MD_Kp		0x40000000	/* Should always be set */

/* The effective page number register.  When read, contains the information
 * about the last instruction TLB miss.  When MD_RPN is written, bits in
 * this register are used to create the TLB entry.
 */
#define MD_EPN		795
#define MD_EPNMASK	0xfffff000	/* Effective page number for entry */
#define MD_EVALID	0x00000200	/* Entry is valid */
#define MD_ASIDMASK	0x0000000f	/* ASID match value */
					/* Reset value is undefined */

/* The pointer to the base address of the first level page table.
 * During a software tablewalk, reading this register provides the address
 * of the entry associated with MD_EPN.
 */
#define M_TWB		796
#define	M_L1TB		0xfffff000	/* Level 1 table base address */
#define M_L1INDX	0x00000ffc	/* Level 1 index, when read */
					/* Reset value is undefined */

/* A "level 1" or "segment" or whatever you want to call it register.
 * For the data TLB, it contains bits that get loaded into the TLB entry
 * when the MD_RPN is written.  It is also provides the hardware assist
 * for finding the PTE address during software tablewalk.
 */
#define MD_TWC		797
#define MD_L2TB		0xfffff000	/* Level 2 table base address */
#define MD_L2INDX	0xfffffe00	/* Level 2 index (*pte), when read */
#define MD_APG		0x000001e0	/* Access protection group (0) */
#define MD_GUARDED	0x00000010	/* Guarded storage */
#define MD_PSMASK	0x0000000c	/* Mask of page size bits */
#define MD_PS8MEG	0x0000000c	/* 8M page size */
#define MD_PS512K	0x00000004	/* 512K page size */
#define MD_PS4K_16K	0x00000000	/* 4K or 16K page size */
#define MD_WT		0x00000002	/* Use writethrough page attribute */
#define MD_SVALID	0x00000001	/* Segment entry is valid */
					/* Reset value is undefined */


/* Real page number.  Defined by the pte.  Writing this register
 * causes a TLB entry to be created for the data TLB, using
 * additional information from the MD_EPN, and MD_TWC registers.
 */
#define MD_RPN		798
#define MD_RPN_MASK     0xfffff000
/*     supervisor  user             */
/*             \  /                 */
#define MD_RPN_RORO     0x00000C00
#define MD_RPN_RWRW     0x00000800
#define MD_RPN_RWRO     0x00000400
#define MD_RPN_RWNO     0x00000000
#define MD_RPN_PPC_MODE 0x00000200
#define MD_RPN_SPS      0x00000008
#define MD_RPN_SH       0x00000004
#define MD_RPN_CI       0x00000002
#define MD_RPN_VALID    0x00000001

/* This is a temporary storage register that could be used to save
 * a processor working register during a tablewalk.
 */
#define M_TW		799

/*
 * At present, all PowerPC 400-class processors share a similar TLB
 * architecture. The instruction and data sides share a unified,
 * 64-entry, fully-associative TLB which is maintained totally under
 * software control. In addition, the instruction side has a
 * hardware-managed, 4-entry, fully- associative TLB which serves as a
 * first level to the shared TLB. These two TLBs are known as the UTLB
 * and ITLB, respectively.
 */

#define        PPC4XX_TLB_SIZE 64

/*
 * TLB entries are defined by a "high" tag portion and a "low" data
 * portion.  On all architectures, the data portion is 32-bits.
 *
 * TLB entries are managed entirely under software control by reading,
 * writing, and searchoing using the 4xx-specific tlbre, tlbwr, and tlbsx
 * instructions.
 */

#define	TLB_LO          1
#define	TLB_HI          0
       
#define	TLB_DATA        TLB_LO
#define	TLB_TAG         TLB_HI

/* Tag portion */

#define TLB_EPN_MASK    0xFFFFFC00      /* Effective Page Number */
#define TLB_PAGESZ_MASK 0x00000380
#define TLB_PAGESZ(x)   (((x) & 0x7) << 7)
#define   PAGESZ_1K		0
#define   PAGESZ_4K             1
#define   PAGESZ_16K            2
#define   PAGESZ_64K            3
#define   PAGESZ_256K           4
#define   PAGESZ_1M             5
#define   PAGESZ_4M             6
#define   PAGESZ_16M            7
#define TLB_VALID       0x00000040      /* Entry is valid */

/* Data portion */
                 
#define TLB_RPN_MASK    0xFFFFFC00      /* Real Page Number */
#define TLB_PERM_MASK   0x00000300
#define TLB_EX          0x00000200      /* Instruction execution allowed */
#define TLB_WR          0x00000100      /* Writes permitted */
#define TLB_ZSEL_MASK   0x000000F0
#define TLB_ZSEL(x)     (((x) & 0xF) << 4)
#define TLB_ATTR_MASK   0x0000000F
#define TLB_W           0x00000008      /* Caching is write-through */
#define TLB_I           0x00000004      /* Caching is inhibited */
#define TLB_M           0x00000002      /* Memory is coherent */
#define TLB_G           0x00000001      /* Memory is guarded from prefetch */
