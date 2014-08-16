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
** (c) 2007 Sciopta Systems GmbH Deutschland, Sciopta AG/ Schweiz   **
**                                                                  **
**********************************************************************
** ID: S07260BS2                                                    **
** +Revision: 1.2 +                                            **
** +Date: 2008/01/16 13:01:17 +                                     **
** DM9000 definitions                                               **
**********************************************************************
EOC*/

#ifndef _DM9000_H_
#define _DM9000_H_
#define NCR    0x00 /* network control register */
#define NSR    0x01 /* network status register */
#define TCR    0x02 /* TX control register */
#define TSR1   0x03 /* TX status register 1 */
#define TSR2   0x04 /* TX status register 2 */
#define RCR    0x05 /* RX control register */
#define RSR    0x06 /* RX status register */
#define ROCR   0x07 /* RX overflow counter */
#define BPTR   0x08 /* back pressure control */
#define FCTR   0x09 /* Flow control threshold */
#define FCR    0x0a /* RX flow control */
#define EPCR   0x0b /* EEPROM & PHY control */
#define EPAR   0x0c /* EEPROM & PHY Address */
#define EPDRL  0x0d /* EEPROM & PHY low byte data */
#define EPDRH  0x0e /* EEPROM & PHY high byte data */
#define WCR    0x0f /* wakeup control */
#define PAR    0x10 /* MAC address */
#define MAR    0x16 /* Multicast address */
#define GPCR   0x1e /* General purpose control */
#define GPR    0x1f /* General purpose */
#define TRPAL  0x22 /* TX SRAM Read Address low */
#define TRPAH  0x23 /* TX SRAM Read Address high */
#define RWPAL  0x24 /* RX SRAM Write Address low */
#define RWPAH  0x25 /* RX SRAM Write Adress high */
#define VIDL   0x28 /* Vendor ID low */
#define VIDH   0x29 /* Vendor ID high */
#define PIDL   0x2a /* Product ID low */
#define PIDH   0x2b /* Product ID high */
#define CHIPR  0x2c /* Chip revision */
#define SMCR   0x2f /* Special mode control */
#define MRCMDX 0xf0  /* read w/o increment from RX RAM*/
#define MRCMD  0xf2  /* reaw w/  increment from RX RAM */
#define MRRL   0xf4  /* memory read address low */
#define MRRH   0xf5  /* memory read address high */
#define MWCMDX 0xf6  /* memory write w/o increment to TX RAM */
#define MWCMD  0xf8  /* memory write w/  increment to TX RAM */
#define MWRL   0xfa  /* memory write address low */
#define MWRH   0xfb  /* memory write address high */
#define TXPLL  0xfc  /* TX packet length low byte */
#define TXPLH  0xfd  /* TX packet length high byte */
#define ISR    0xfe  /* interrupt status register */
#define IMR    0xff  /* interrupt mask register */

#define NCR_EXT_PHY  0x80
#define NCR_WAKEEN   0x40
#define NCR_FCOL     0x10
#define NCR_FDX      0x08
#define NCR_LBK_NRM  0x00
#define NCR_LBK_PHY  0x04
#define NCR_LBK_MAC  0x02
#define NCR_RST      0x01

#define NSR_SPEED    0x80
#define NSR_LINKST   0x40
#define NSR_WAKEST   0x20
#define NSR_TX2END   0x08
#define NSR_TX1END   0x04
#define NSR_RXOV     0x02

#define TCR_TJDIS    0x40
#define TCR_EXCEM    0x20
#define TCR_PAD_DIS2 0x10
#define TCR_CRC_DIS2 0x08
#define TCR_PAD_DIS1 0x04
#define TCR_CRC_DIS1 0x02
#define TCR_TXREQ    0x01

#define TSR_TJTO     0x80
#define TSR_LC       0x40
#define TSR_NC       0x20
#define TSR_LCOL     0x10
#define TSR_COL      0x08
#define TSR_EC       0x04

#define RCR_WTDIS    0x40
#define RCR_DIS_LONG 0x20
#define RCR_DIS_CRC  0x10
#define RCR_ALL      0x08
#define RCR_RUNT     0x04
#define RCR_PRMSC    0x02
#define RCR_RXEN     0x01

#define RSR_RF       0x80
#define RSR_MF       0x40
#define RSR_LCS      0x20
#define RSR_RWTO     0x10
#define RSR_PLE      0x08
#define RSR_AE       0x04
#define RSR_CE       0x02
#define RSR_FOE      0x01

#define RSR_ERROR    (RSR_RF|RSR_LCS|RSR_RWTO|RSR_PLE|RSR_AE|RSR_CE|RSR_FOE)

#define ROCR_RXFU    0x80
#define ROCR_ROC     0x7f

#define FCR_TXP0     0x80
#define FCR_TCPF     0x40
#define FCR_TXPEN    0x20
#define FCR_BKPA     0x10
#define FCR_BKPM     0x08
#define FCR_RXPS     0x04
#define FCR_RXPCS    0x02
#define FCR_FLCE     0x01

#define EPCR_REEP    0x20
#define EPCR_WEP     0x10
#define EPCR_EPOS    0x08
#define EPCR_ERPRR   0x04
#define EPCR_ERPRW   0x02
#define EPCR_ERRE    0x01

#define WCR_LINKEN   0x20
#define WCR_SAMPLEEN 0x10
#define WCR_MAGICEN  0x08
#define WCR_LINKST   0x04
#define WCR_SAMPLEST 0x02
#define WCR_MAGICST  0x01

#define GPCR_CNTL(n) ((n) & 0xf)
#define GPR_CNTL(n)  ((n) & 0xf)

#define ISR_IOMODE16 0x00
#define ISR_IOMODE32 0x40
#define ISR_IOMODE8  0x80
#define ISR_ROOS     0x08
#define ISR_ROS      0x04
#define ISR_PTS      0x02
#define ISR_PRS      0x01

#define IMR_PAR      0x80
#define IMR_ROOM     0x08
#define IMR_ROM      0x04
#define IMR_PTM      0x02
#define IMR_PRM      0x01


#define DM9000_PHY_BMCR	          ( 0)
#define BMCR_RESET                (0x8000)
#define BMCR_LOOPBACK             (0x4000)
#define BMCR_SPEED                (0x2000)
#define BMCR_AUTONEG              (0x1000)
#define BMCR_POWERDOWN            (0x0800)
#define BMCR_ISOLATE              (0x0400)
#define BMCR_RESTART_AUTO         (0x0200)
#define BMCR_DPLX                 (0x0100)
#define BMCR_COLL_TEST            (0x0080)

#define DM9000_PHY_BMSR	          ( 1)
#define BMSR_100BASET4            (0x8000)
#define BMSR_100BASETXFD          (0x4000)
#define BMSR_100BASETXHD          (0x2000)
#define BMSR_10BASEFD             (0x1000)
#define BMSR_10BASEHD             (0x0800)
#define BMSR_MIISUP               (0x0040)
#define BMSR_AUTONEG_DONE         (0x0020)
#define BMSR_REMOTE_FAULT         (0x0010)
#define BMSR_AUTONEG_ABLE         (0x0008)
#define BMSR_LINK_STATUS          (0x0004)
#define BMSR_JABBER               (0x0002)
#define BMSR_EXTENDED             (0x0001)

#define DM9000_PHY_PHYID1         ( 2)
#define DM9000_PHY_OUI_MSB	  (0xffff)  /* 16 bits */

#define DM9000_PHY_PHYID2         ( 3)
#define DM9000_PHY_OUI_LSB	  (0xfc00)  /*  6 bits */
#define DM9000_PHY_VNDR_MDL       (0x03f0)  /*  6 bits */
#define DM9000_PHY_MDL_REV	  (0x000f)  /*  4 bits */

#define DM9000_PHY_ANAR	          ( 4)
#define DM9000_PHY_ANLPAR         ( 5)
#define DM9000_PHY_NP	          (0x8000)
#define DM9000_PHY_ACK	          (0x4000)
#define DM9000_PHY_RF	          (0x2000)
#define DM9000_PHY_FCS	          (0x0400)
#define DM9000_PHY_T4	          (0x0200)
#define DM9000_PHY_TX_FDX	  (0x0100)
#define DM9000_PHY_TX_HDX	  (0x0080)
#define DM9000_PHY_10_FDX	  (0x0040)
#define DM9000_PHY_10_HDX	  (0x0020)
#define DM9000_PHY_SEL	          (0x001f)  /*  5 bits */

#define DM9000_PHY_ANER	          ( 6)
#define DM9000_PHY_PDF	          (0x0010)
#define DM9000_PHY_LP_NP_ABLE     (0x0008)
#define DM9000_PHY_NP_ABLE	  (0x0004)
#define DM9000_PHY_PAGE_RX	  (0x0002)
#define DM9000_PHY_LP_AN_ABLE     (0x0001)

#define DM9000_PHY_DSCR	          (16)
#define DM9000_PHY_BP_4B5B	  (0x8000)
#define DM9000_PHY_BP_SCR	  (0x4000)
#define DM9000_PHY_BP_ALIGN       (0x2000)
#define DM9000_PHY_BP_ADPOK       (0x1000)
#define DM9000_PHY_REPEATER       (0x0800)
#define DM9000_PHY_TX	          (0x0400)
#define DM9000_PHY_F_LINK_100     (0x0080)
#define DM9000_PHY_RPDCTR_EN      (0x0010)
#define DM9000_PHY_SMRST	  (0x0008)
#define DM9000_PHY_MFPSC	  (0x0004)
#define DM9000_PHY_SLEEP	  (0x0002)
#define DM9000_PHY_RLOUT	  (0x0001)

#define DM9000_PHY_DSCSR	  (17)
#define DM9000_PHY_100FDX	  (0x8000)
#define DM9000_PHY_100HDX	  (0x4000)
#define DM9000_PHY_10FDX	  (0x2000)
#define DM9000_PHY_10HDX	  (0x1000)
#define DM9000_PHY_PHYADR	  (0x01f0)	/*  5 bits */
#define DM9000_PHY_ANMD	          (0x000f)	/*  4 bits */

#define DM9000_PHY_10BTCSR	  (18)
#define DM9000_PHY_LP_EN	  (0x4000)
#define DM9000_PHY_HBE	          (0x2000)
#define DM9000_PHY_SQUELCH	  (0x1000)
#define DM9000_PHY_JABEN	  (0x0800)
#define DM9000_PHY_10BT_SER       (0x0400)
#define DM9000_PHY_POLR	          (0x0001)

#define DM9000_PHY                (0x40)

#endif
