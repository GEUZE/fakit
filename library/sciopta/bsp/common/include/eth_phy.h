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
** ID: S07064BS2                                                    **
** +Revision: 1.2 +                                                 **
** +Date: 2008/01/16 13:01:18 +                                     **
** Common Ethernet Phy register-description                         **
**********************************************************************
EOC*/
#ifndef _ETH_PHY_H_
#define _ETH_PHY_H_

#include <sciopta.h>
#include <eth_mac.h>	/* macAddr_t */

/* MMI Management register set */

#define PHY_REG_CONTROL	      	   ( 0)
#define PHY_REG_CONTROL_RESET	  	(0x8000)
#define PHY_REG_CONTROL_LOOPBACK  	(0x4000)
#define PHY_REG_CONTROL_SPEED_SEL 	(0x2040)  /* 2 bits */
#define PHY_REG_CONTROL_AUTONEG_EN	(0x1000)
#define PHY_REG_CONTROL_PWRDN		(0x0800)
#define PHY_REG_CONTROL_ISOLATE   	(0x0400)
#define PHY_REG_CONTROL_AUTONEG_RS	(0x0200)
#define PHY_REG_CONTROL_DUPLEX	  	(0x0100)
#define PHY_REG_CONTROL_COLTEST	  	(0x0080)
#define PHY_REG_CONTROL_UNIDIR_EN 	(0x0020)

/* possible values for PHY_REG_CONTROL_SPEED_SEL */
#define PHY_REG_CONTROL_SPEED_10M 	(0x0000)
#define PHY_REG_CONTROL_SPEED_100M 	(0x2000)
#define PHY_REG_CONTROL_SPEED_1000M 	(0x0040)

#define PHY_REG_CONTROL_LINK		(0x2140)  /* 3 bits */
/* possible values for PHY_REG_CONTROL_LINK */
#define PHY_REG_CONTROL_LINK_10M_HD 	(0x0000)
#define PHY_REG_CONTROL_LINK_10M_FD	(0x0100)
#define PHY_REG_CONTROL_LINK_100M_HD	(0x2000)
#define PHY_REG_CONTROL_LINK_100M_FD 	(0x2100)
#define PHY_REG_CONTROL_LINK_1000M_HD	(0x0040)
#define PHY_REG_CONTROL_LINK_1000M_FD 	(0x0140)

#define PHY_REG_STATUS	      	   ( 1)
#define PHY_REG_STATUS_ABLE_100BASE_T4	  (0x8000)
#define PHY_REG_STATUS_ABLE_100BASE_X_FD  (0x4000)
#define PHY_REG_STATUS_ABLE_100BASE_X_HD  (0x2000)
#define PHY_REG_STATUS_ABLE_10M_FD	  (0x1000)
#define PHY_REG_STATUS_ABLE_10M_HD	  (0x0800)
#define PHY_REG_STATUS_ABLE_100BASE_T2_FD (0x0400)
#define PHY_REG_STATUS_ABLE_100BASE_T2_HD (0x0200)
#define PHY_REG_STATUS_EXT_STATUS	  (0x0100)
#define PHY_REG_STATUS_ABLE_UNIDIR	  (0x0080)
#define PHY_REG_STATUS_MFPS		  (0x0040)
#define PHY_REG_STATUS_AUTONEG_CP	  (0x0020)
#define PHY_REG_STATUS_REMOTE_FAULT	  (0x0010)
#define PHY_REG_STATUS_ABLE_AUTONEG	  (0x0008)
#define PHY_REG_STATUS_LINK_UP		  (0x0004)
#define PHY_REG_STATUS_JABBER		  (0x0002)
#define PHY_REG_STATUS_EXT_REG		  (0x0001)

/* some shortcuts for the ability bits */
#define PHY_REG_STATUS_ABLE_10M		  (0x1800)    /* 2 bits */
#define PHY_REG_STATUS_ABLE_100M	  (0xe600)    /* 5 bits */
#define PHY_REG_STATUS_ABLE_100BASE_X	  (0x6000)    /* 2 bits */
#define PHY_REG_STATUS_ABLE_100BASE_T2	  (0x0600)    /* 2 bits */
#define PHY_REG_STATUS_ABLE_100M_HD	  (0x2200)    /* 2 bits */
#define PHY_REG_STATUS_ABLE_100M_FD	  (0xc400)    /* 3 bits */

#define PHY_REG_ID1	      	   ( 2)
#define PHY_REG_ID2	      	   ( 3)

#define PHY_REG_AUTONEG_ADV   	   ( 4)
#define PHY_REG_AUTONEG_LPBPA 	   ( 5)
#define PHY_REG_AUTONEG_NPT   	   ( 7)
#define PHY_REG_AUTONEG_LPRNP 	   ( 8)

/* applies to above four registers */
#define PHY_REG_AUTONEG_BASE_NP	      (0x8000)
#define PHY_REG_AUTONEG_BASE_ACK      (0x4000)
#define PHY_REG_AUTONEG_BASE_RF	      (0x2000)
#define PHY_REG_AUTONEG_BASE_TAF      (0x1fe0)	/*  8 bits */
#define PHY_REG_AUTONEG_BASE_TA_10BASE_T      (0x0020)
#define PHY_REG_AUTONEG_BASE_TA_10BASE_T_FD   (0x0040)
#define PHY_REG_AUTONEG_BASE_TA_100BASE_TX    (0x0080)
#define PHY_REG_AUTONEG_BASE_TA_100BASE_TX_FD (0x0100)
#define PHY_REG_AUTONEG_BASE_TA_100BASE_T4    (0x0200)
#define PHY_REG_AUTONEG_BASE_TA_PAUSE	      (0x0400)
#define PHY_REG_AUTONEG_BASE_TA_PAUSE_ASYM    (0x0800)

#define PHY_REG_AUTONEG_BASE_TA_FD	      (0x0340)	/* 3 bits */
#define PHY_REG_AUTONEG_BASE_TA_10M	      (0x0060)	/* 2 bits */
#define PHY_REG_AUTONEG_BASE_TA_100M	      (0x0380)	/* 3 bits */

#define PHY_REG_AUTONEG_BASE_SF	      (0x001f)	/*  5 bits */
#define PHY_REG_AUTONEG_BASE_S_802_3	      (0x0001)
#define PHY_REG_AUTONEG_BASE_S_802_9	      (0x0002)
#define PHY_REG_AUTONEG_BASE_S_802_5	      (0x0003)
#define PHY_REG_AUTONEG_BASE_S_1394	      (0x0004)

#define PHY_REG_AUTONEG_NEXT_NP	      (PHY_REG_AUTONEG_BASE_NP)
#define PHY_REG_AUTONEG_NEXT_ACK      (PHY_REG_AUTONEG_BASE_ACK)
#define PHY_REG_AUTONEG_NEXT_MP	      (0x2000)
#define PHY_REG_AUTONEG_NEXT_ACK2     (0x1000)
#define PHY_REG_AUTONEG_NEXT_TOGGLE   (0x0800)
#define PHY_REG_AUTONEG_NEXT_MSG      (0x07ff)	/* 11 bits */

#define PHY_REG_AUTONEG_EX    	   ( 6)
#define PHY_REG_AUTONEG_EX_RNPLA      (0x0040)
#define PHY_REG_AUTONEG_EX_RNPSL      (0x0020)
#define PHY_REG_AUTONEG_EX_PDF	      (0x0010)
#define PHY_REG_AUTONEG_EX_LPNPA      (0x0008)
#define PHY_REG_AUTONEG_EX_NPA	      (0x0004)
#define PHY_REG_AUTONEG_EX_PRX	      (0x0002)
#define PHY_REG_AUTONEG_EX_LPANA      (0x0001)

#define PHY_REG_MASL_CONTROL  	   ( 9)
#define PHY_REG_MASL_STATUS   	   (10)
#define PHY_REG_PSE_CONTROL   	   (11)
#define PHY_REG_PSE_STATUS    	   (12)
#define PHY_REG_MMDA_CONTROL  	   (13)
#define PHY_REG_MMDA_ADDR_DATA	   (14)

#define PHY_REG_EXT_STATUS     	   (15)
#define PHY_REG_EXT_STATUS_ABLE_1000BASE_X_FD	(0x8000)
#define PHY_REG_EXT_STATUS_ABLE_1000BASE_X_HD	(0x4000)
#define PHY_REG_EXT_STATUS_ABLE_1000BASE_T_FD	(0x2000)
#define PHY_REG_EXT_STATUS_ABLE_1000BASE_T_HD	(0x1000)

/* some shortcuts for the extended ability bits */
#define PHY_REG_EXT_STATUS_ABLE_1000M		(0xf000)
#define PHY_REG_EXT_STATUS_ABLE_1000BASE_X	(0xc000)
#define PHY_REG_EXT_STATUS_ABLE_1000BASE_T	(0x3000)
#define PHY_REG_EXT_STATUS_ABLE_1000M_HD	(0x5000)
#define PHY_REG_EXT_STATUS_ABLE_1000M_FD	(0xa000)

#define PHY_REG_VENDOR_SPECIFIC_0  (16)
#define PHY_REG_VENDOR_SPECIFIC_1  (17)
#define PHY_REG_VENDOR_SPECIFIC_2  (18)
#define PHY_REG_VENDOR_SPECIFIC_3  (19)
#define PHY_REG_VENDOR_SPECIFIC_4  (20)
#define PHY_REG_VENDOR_SPECIFIC_5  (21)
#define PHY_REG_VENDOR_SPECIFIC_6  (22)
#define PHY_REG_VENDOR_SPECIFIC_7  (23)
#define PHY_REG_VENDOR_SPECIFIC_8  (24)
#define PHY_REG_VENDOR_SPECIFIC_9  (25)
#define PHY_REG_VENDOR_SPECIFIC_10 (26)
#define PHY_REG_VENDOR_SPECIFIC_11 (27)
#define PHY_REG_VENDOR_SPECIFIC_12 (28)
#define PHY_REG_VENDOR_SPECIFIC_13 (29)
#define PHY_REG_VENDOR_SPECIFIC_14 (30)
#define PHY_REG_VENDOR_SPECIFIC_15 (31)

/* MAC-specific functions */

__uint phy_regGet (__uint phy, __uint idx);

void phy_regSet (__uint phy, __uint idx, __uint val);


/* MAC-independent interface */

void phy_reset (__uint phy);

//void phy_setLoopbackMode (void);

/* perform auto-negotiation */
int phy_doAutoNeg (__uint phy, sc_ticks_t tmo);

/* perform manual negotiation */
void phy_doManNeg (__uint phy, __uint maxSpeed, sc_ticks_t tmo_per_try);

/* 
 * get PHY identifier as MAC address
 * last 14 bits are free for network allocation
 */
macAddr_t phy_idMacAddrGet (__uint phy);

#endif /* _ETH_PHY_H_ */
