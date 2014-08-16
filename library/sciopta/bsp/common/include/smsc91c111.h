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
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S05018BS2                                                    **
** +Revision: 1.8.2.1 +                                                 **
** +Date: 2009/04/08 06:19:13 +                                     **
** Defines for SMSC91C111                                           **
**********************************************************************
EOC*/


#ifndef __SMSC_LAN91C111_H__
#define __SMSC_LAN91C111_H__

#define SMSC_TCR		0x00	/* Transmit Control Register */
#define SMSC_EPHSR	        0x02
#define SMSC_RCR		0x04	/* Receive Control Register */
#define SMSC_ECR		0x06
#define SMSC_MIR		0x08
#define SMSC_RPCR		0x0A
#define SMSC_RESERVED_0		0x0C
#define SMSC_BS			0x0E

#define SMSC_CR			0x10
#define SMSC_BAR		0x12
#define SMSC_IAR01		0x14
#define SMSC_IAR23		0x16
#define SMSC_IAR45		0x18
#define SMSC_GPR		0x1A
#define SMSC_CTR		0x1C
#define SMSC_BS1		0x1E

#define SMSC_MMUCR		0x20
#define SMSC_PNR		0x22
#define SMSC_ARR		0x23
#define SMSC_FIFO		0x24
#define SMSC_PTR		0x26
#define SMSC_DATA_HIGH		0x28
#define SMSC_DATA		0x2a
#define SMSC_IST		0x2c
#define SMSC_ACK		0x2c
#define SMSC_MSK		0x2d
#define SMSC_BS2		0x2e

#define SMSC_MT01		0x30
#define SMSC_MT23		0x32
#define SMSC_MT45		0x34
#define SMSC_MT67		0x36
#define SMSC_MGMT		0x38
#define SMSC_REV		0x3a
#define SMSC_ERCV		0x3c
#define SMSC_BS3		0x3e

/* bits in Transmit Control Register (bank 0,0) */
#define SMSC_TCR_SWFDUP		0x8000	/* Switched Full Duplex mode */
#define SMSC_TCR_RESERVED	0x4000	
#define SMSC_TCR_EPH_LOOP	0x2000	/* loopback mode */
#define SMSC_TCR_STP_SQET	0x1000	/* Stop transmission on SQET error */
#define SMSC_TCR_FDUPLX		0x0800	/* full duplex */
#define SMSC_TCR_MON_CSN	0x0400	/* monitor carrier during tx (91C96) */
#define SMSC_TCR_NOCRC		0x0100	/* does not append CRC to frames */
#define SMSC_TCR_PAD_EN		0x0080	/* pads frames with 00 to min length */
#define SMSC_TCR_FORCOL		0x0004	/* force collision */
#define SMSC_TCR_LOOP		0x0002	/* GP pin */
#define SMSC_TCR_TXENA		0x0001	/* enable */

/* bits in EPH Status Register (bank 0,2) */
#define SMSC_EPHSR_TX_UNRN	0x8000	/* Tx Underrun */
#define SMSC_EPHSR_LINK_OK	0x4000	/* Driven by inverted value of nLNK pin */
#define SMSC_EPHSR_CTR_ROL	0x1000	/* Counter Roll Over indication */
#define SMSC_EPHSR_EXC_DEF	0x0800	/* Excessive Deferral */
#define SMSC_EPHSR_LOST_CARR	0x0400	/* Lost Carrier Sense */
#define SMSC_EPHSR_LATCOL	0x0200	/* Late collision detected on last tx */
#define SMSC_EPHSR_WAKEUP	0x0100
#define SMSC_EPHSR_TX_DEFR	0x0080	/* Transmit Deferred */
#define SMSC_EPHSR_LTX_BRD	0x0040	/* Last tx was a broadcast */
#define SMSC_EPHSR_SQET		0x0020	/* Signal Quality Error Test */
#define SMSC_EPHSR_16COL	0x0010	/* 16 Collisions Reached */
#define SMSC_EPHSR_LTX_MULT	0x0008	/* Last tx was a multicast */
#define SMSC_EPHSR_MUL_COL	0x0004	/* Multiple coll. detected for last tx*/
#define SMSC_EPHSR_SNGL_COL	0x0002	/* Single coll. detected for last tx */
#define SMSC_EPHSR_TX_SUC	0x0001	/* Last TX was successful */

/* bits in Receive Control Register (bank 0,4) */
#define SMSC_RCR_SOFT_RST	0x8000	/* soft reset */
#define SMSC_RCR_FILT_CAR	0x4000	/* filter carrier */
#define SMSC_RCR_ABORT_ENB	0x2000	/* abort on collision */
#define SMSC_RCR_STRIP_CRC	0x0200	/* strip CRC */
#define SMSC_RCR_RXEN		0x0100	/* enable RX */
#define SMSC_RCR_ALMUL		0x0004	/* receive all muticasts */
#define SMSC_RCR_PRMS		0x0002	/* promiscuous */
#define SMSC_RCR_RX_ABORT	0x0001	/* set when abort due to long frame */

/* bits in Receice/Phy Control Register (bank 0,8) */
#define SMSC_RPCR_LEDB_LINK	(0 << 2)
#define SMSC_RPCR_LEDB_TXRX	(4 << 2)
#define SMSC_RPCR_LEDB_RX	(6 << 2)
#define SMSC_RPCR_LEDB_TX	(7 << 2)
#define SMSC_RPCR_LEDA_LINK	(0 << 5)
#define SMSC_RPCR_LEDA_TXRX	(4 << 5)
#define SMSC_RPCR_LEDA_RX	(6 << 5)
#define SMSC_RPCR_LEDA_TX	(7 << 5)
#define SMSC_RPCR_ANEG		(1 << 11)
#define SMSC_RPCR_DPLX		(1 << 12)
#define SMSC_RPCR_SPEED		(1 << 13)

/* bits in Configuration Register (bank 1,0) */
#define SMSC_CR_EPH_POWER_EN 	0x8000	/* disable low power mode. */
#define SMSC_CR_NO_WAIT		0x1000	/* no extra wait states on ISA bus */
#define SMSC_CR_GPCNTRL		0x0400	/* Inverse value drives pin nCNTRL */
#define SMSC_CR_EXT_PHY		0x0200	/* 1=external MII, 0=internal Phy */

#define SMSC_CR_MASK            0x20b1

#define SMSC_BAR_MASK            0x0001

/* control register bits (bank 1,c) */
#define SMSC_CTR_RCV_BAD	0x4000
#define SMSC_CTR_AUTO_RELEASE	0x0800
#define SMSC_CTR_LE_ENABLE	0x0080
#define SMSC_CTR_CR_ENABLE	0x0040
#define SMSC_CTR_TE_ENABLE	0x0020
#define SMSC_CTR_MASK           0x1210
/* These are for setting the MAC address in the 91C96 serial EEPROM */
#define SMSC_CTR_EEPROM_SELECT	0x0004
#define SMSC_CTR_RELOAD		0x0002
#define SMSC_CTR_STORE		0x0001
#define SMSC_CTR_EEPROM_BUSY	0x0003
#define SMSC_ESA_EEPROM_OFFSET	0x0020

/* bits in MMU Command Register (bank 2,0) */
#define SMSC_MMUCR_NOOP		0x0000
#define SMSC_MMUCR_ALLOC_FOR_TX	0x0020
#define SMSC_MMUCR_RESET_MMU	0x0040
#define SMSC_MMUCR_RM_RX_FRAME	0x0060
#define SMSC_MMUCR_RM_TX_FRAME	0x0070	/* (91C96) only when TX stopped */
#define SMSC_MMU_RMREL_RX_FRAME 0x0080
#define SMSC_MMU_REL_PACKET	0x00A0
#define SMSC_MMU_ENQ_PACKET	0x00C0
#define SMSC_MMU_RESET_TX_FIFO	0x00E0
#define SMSC_MMUCR_BUSY         0x0001

/* bits in Allocation result register (bank 2,3) */
#define SMSC_ARR_FAILED         0x80

/* bits in FIFO Ports register (bank 2,4) */
#define SMSC_FIFO_REMPTY        0x8000
#define SMSC_FIFO_TEMPTY        0x0080

/* bits in Pointer register (bank 2,6) */
#define SMSC_PTR_RCV		0x8000
#define SMSC_PTR_AUTO_INCR	0x4000
#define SMSC_PTR_READ		0x2000
#define SMSC_PTR_ETEN		0x1000
#define SMSC_PTR_NOT_EMPTY 	0x0800

/* bits in Interrupt Status register (bank 2,c) */
#define SMSC_IST_MDINT_M	0x8000
#define SMSC_IST_ERCV_INT_M	0x4000
#define SMSC_IST_EPH_INT_M	0x2000
#define SMSC_IST_RX_OVRN_INT_M	0x1000
#define SMSC_IST_ALLOC_INT_M	0x0800
#define SMSC_IST_TX_EMPTY_INT_M	0x0400
#define SMSC_IST_TX_INT_M	0x0200
#define SMSC_IST_RCV_INT_M	0x0100

#define SMSC_IST_MDINT		0x0080
#define SMSC_IST_ERCV_INT	0x0040
#define SMSC_IST_EPH_INT	0x0020
#define SMSC_IST_RX_OVRN_INT	0x0010
#define SMSC_IST_ALLOC_INT	0x0008
#define SMSC_IST_TX_EMPTY_INT	0x0004
#define SMSC_IST_TX_INT		0x0002
#define SMSC_IST_RCV_INT	0x0001

/* bits in Interrupt Acknowledge Register (bank 2,c) */

#define SMSC_ACK_MDINT		0x0080
#define SMSC_ACK_ERCV_INT	0x0040
#define SMSC_ACK_RX_OVRN_INT	0x0010
#define SMSC_ACK_TX_EMPTY_INT	0x0004
#define SMSC_ACK_TX_INT		0x0002

/* bits in Interrupt Mask register (bank 2,d) */

#define SMSC_MSK_MDINT		0x80
#define SMSC_MSK_ERCV_INT	0x40
#define SMSC_MSK_EPH_INT	0x20
#define SMSC_MSK_RX_OVRN_INT	0x10
#define SMSC_MSK_ALLOC_INT	0x08
#define SMSC_MSK_TX_EMPTY_INT	0x04
#define SMSC_MSK_TX_INT		0x02
#define SMSC_MSK_RCV_INT	0x01


#define SMSC_IST_TX_MSK         (SMSC_IST_TX_INT_M+SMSC_IST_TX_EMTY_M)
#define SMSC_IST_TX_ACK		(SMSC_IST_TX_INT+SMSC_IST_TX_EMPTY_INT)

#define SMSC_MSK_TX             (SMSC_MSK_TX_INT+SMSC_MSK_TX_EMTY)
#define SMSC_ACK_TX             (SMSC_ACK_TX_INT+SMSC_ACK_EMPTY_INT)

/* Bits in control byte */
#define SMSC_CONTROLBYTE_CRC	0x1000
#define SMSC_CONTROLBYTE_ODD	0x2000
#define SMSC_CONTROLBYTE_RX	0x4000

#define SMSC_RX_STATUS_ALIGNERR	0x8000
#define SMSC_RX_STATUS_BCAST	0x4000
#define SMSC_RX_STATUS_BADCRC	0x2000
#define SMSC_RX_STATUS_ODDFRM	0x1000
#define SMSC_RX_STATUS_TOOLONG	0x0800
#define SMSC_RX_STATUS_TOOSHORT	0x0400
#define SMSC_RX_STATUS_HASHVALMASK 0x007E	/* MASK */
#define SMSC_RX_STATUS_MCAST	0x0001
#define SMSC_RX_STATUS_BAD		\
	(SMSC_RX_STATUS_ALIGNERR |	\
	 SMSC_RX_STATUS_BADCRC   |	\
	 SMSC_RX_STATUS_TOOLONG  |	\
	 SMSC_RX_STATUS_TOOSHORT)

/* Attribute memory registers in PCMCIA mode */
#define SMSC_ECOR		0x8000
#define SMSC_ECOR_RESET		(1<<7)
#define SMSC_ECOR_LEVIRQ	(1<<6)
#define SMSC_ECOR_ATTWR		(1<<2)
#define SMSC_ECOR_ENABLE	(1<<0)

#define SMSC_ECSR		0x8002
#define SMSC_ECSR_IOIS8		(1<<5)
#define SMSC_ECSR_PWRDWN	(1<<2)
#define SMSC_ECSR_INTR		(1<<1)

/* These are for manipulating the MII interface */
#define SMSC_MGMT_MDO		0x0001	/* MII Data Output */
#define SMSC_MGMT_MDI		0x0002	/* MII Data Input */
#define SMSC_MGMT_MCLK		0x0004	/* MII Clock */
#define SMSC_MGMT_MDOE		0x0008	/* MII Data Output Enable */

/* Internal PHY registers (91c111) */
#define SMSC_PHY_CTRL		 0 /* PHY Control Register */
#define SMSC_PHY_STAT		 1 /* PHY Status Register */
#define SMSC_PHY_ID1		 2 /* PHY Identifier 1 */
#define SMSC_PHY_ID2		 3 /* PHY Identifier 2 */
#define SMSC_PHY_AUTO_AD	 4 /* PHY Auto-Neg. Advertisement Register */
#define SMSC_PHY_AUTO_CAP	 5
#define SMSC_PHY_CONFIG1	16
#define SMSC_PHY_CONFIG2	17
#define SMSC_PHY_STATUS_OUT	18 /* PHY Status Output Register */
#define SMSC_PHY_MASK		19 /* PHY Interrupt/Status Mask Register */

/* PHY control bits */
#define SMSC_PHY_CTRL_COLTST	(1 << 7)  /* 1=MII Colision Test */
#define SMSC_PHY_CTRL_DPLX	(1 << 8)  /* 1=Full Duplex, 0=Half Duplex */
#define SMSC_PHY_CTRL_ANEG_RST	(1 << 9)  /* 1=Reset Auto negotiate */
#define SMSC_PHY_CTRL_MII_DIS	(1 << 10) /* 1=MII 4 bit interface disabled */
#define SMSC_PHY_CTRL_PDN	(1 << 11) /* 1=PHY Power Down mode */
#define SMSC_PHY_CTRL_ANEG_EN	(1 << 12) /* 1=Enable Auto negotiation */
#define SMSC_PHY_CTRL_SPEED	(1 << 13) /* 1=100Mbps, 0=10Mpbs */
#define SMSC_PHY_CTRL_LPBK	(1 << 14) /* 1=PHY Loopback */
#define SMSC_PHY_CTRL_RST	(1 << 15) /* 1=PHY Reset */

/* PHY status bits */
#define SMSC_PHY_STAT_CAP_T4	(1 << 15) /* 1=100Base-T4 capable */
#define SMSC_PHY_STAT_CAP_TXF	(1 << 14) /* 1=100Base-X full duplex capable */
#define SMSC_PHY_STAT_CAP_TXH	(1 << 13) /* 1=100Base-X half duplex capable */
#define SMSC_PHY_STAT_CAP_TF	(1 << 12) /* 1=10Mbps full duplex capable */
#define SMSC_PHY_STAT_CAP_TH	(1 << 11) /* 1=10Mbps half duplex capable */
#define SMSC_PHY_STAT_CAP_SUPR	(1 << 6) /* 1=recv mgmt frames with not preamble */
#define SMSC_PHY_STAT_ANEG_ACK	(1 << 5) /* 1=ANEG has completed */
#define SMSC_PHY_STAT_REM_FLT	(1 << 4) /* 1=Remote Fault detected */
#define SMSC_PHY_STAT_CAP_ANEG	(1 << 3) /* 1=Auto negotiate capable */
#define SMSC_PHY_STAT_LINK	(1 << 2) /* 1=valid link */
#define SMSC_PHY_STAT_JAB	(1 << 1) /* 1=10Mbps jabber condition */
#define SMSC_PHY_STAT_EXREG	(1 << 0) /* 1=extended registers implemented */

/* PHY Auto-Negotiation Advertisement bits */
#define SMSC_PHY_AUTO_AD_NP	(1 << 15) /* 1=PHY requests exc. of Next Page */
#define SMSC_PHY_AUTO_AD_ACK	(1 << 14) /* 1=got link code word from remote */
#define SMSC_PHY_AUTO_AD_RF	(1 << 13) /* 1=advertise remote fault */
#define SMSC_PHY_AUTO_AD_T4	(1 << 9) /* 1=PHY is capable of 100Base-T4 */
#define SMSC_PHY_AUTO_AD_TX_FDX	(1 << 8) /* 1=PHY is cap. of 100Base-TX FDPLX */
#define SMSC_PHY_AUTO_AD_TX_HDX	(1 << 7) /* 1=PHY is cap. of 100Base-TX HDPLX */
#define SMSC_PHY_AUTO_AD_10_FDX	(1 << 6) /* 1=PHY is capable of 10Base-T FDPLX */
#define SMSC_PHY_AUTO_AD_10_HDX	(1 << 5) /* 1=PHY is capable of 10Base-T HDPLX */
#define SMSC_PHY_AUTO_AD_CSMA	(1 << 0) /* 1=PHY is capable of 802.3 CMSA */

/* PHY Status Output bits */
#define SMSC_PHY_STATUS_OUT_INT	(1 << 15) /* Interrupt detected */
#define SMSC_PHY_STATUS_OUT_LINK_FAIL (1 << 14) /* Linkfail detected */
#define SMSC_PHY_STATUS_OUT_LOSSYNC (1 << 13) /* Descrambler loss of Sync detect */
#define SMSC_PHY_STATUS_OUT_CWRD (1 << 12) /* Codeword error */
#define SMSC_PHY_STATUS_OUT_SSD	 (1 << 11) /* Start of stream error */
#define SMSC_PHY_STATUS_OUT_ESD	 (1 << 10) /* End of stream error */
#define SMSC_PHY_STATUS_OUT_RPOL (1 << 9) /* Reverse polarity detected */
#define SMSC_PHY_STATUS_OUT_JAB	 (1 << 8) /* Jabber detected */
#define SMSC_PHY_STATUS_OUT_SPDDET (1 << 7) /* 1=100Mbps, 0=10 Mbps */
#define SMSC_PHY_STATUS_OUT_DPLXDET (1 << 6) /* 1=full duplex, 0=half duplex */


#define SMSC_PHY_BASE_MASK 0xffc0

void smsc91c111_setBase(unsigned int base);
int smsc91c111_init(__u8 *eth_mac);

int smsc91c111_alloc(sdd_netbuf_t NEARPTR netbuf);
void smsc91c111_allocInt(void);
int smsc91c111_isr(sdd_netbuf_t NEARPTR NEARPTR tx,sdd_netbuf_t NEARPTR rx);

void smsc91c111_disableInt(void);
void smsc91c111_enableInt(void);

/* Phy functions */
void smsc91c111_dumpPhy(logd_t NEARPTR logd);
__u16 smsc91c111_read_phy(__u8 phyaddr, __u8 phyreg);
void smsc91c111_write_phy(__u8 phyaddr, __u8 phyreg, __u16 value);
int smsc91c111_establishLink(void);

#if (CNF_SMSC91C111_EEPROM == 1)
/* EEPROM functions */
void smsc91c111_eepromRead (__u16 start, char *buf, size_t len);
void smsc91c111_eepromWrite (__u16 start, const char *buf, size_t len);
#endif

#ifndef SMSC91C111_LINK_MSG
#define SMSC91C111_LINK_MSG (0x1234000A)
typedef struct smsc91c111_link_s {
  sc_msgid_t id;
} smsc91c111_link_t;
#endif

#endif /* __SMSC_SMSC_H__ */
