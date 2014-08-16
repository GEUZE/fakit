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
** ID: S07064BS3                                                    **
** +Revision: 1.2 +                                                **
** +Date: 2008/01/16 13:01:18 +                                     **
** Common Ethernet PHY functions                                    **
**********************************************************************
EOC*/
#include <sciopta.h>
#include <eth_phy.h>

/* CPU-independant part */

/* test ability, set link, wait for link up */
static int phy_testLink (__uint phy, int speed, int duplex, sc_ticks_t tmo);

void
phy_reset (__uint phy)
{
  __uint cr;
  sc_time_t tick, tmo;
  
  cr = phy_regGet (phy, PHY_REG_CONTROL);
  cr |= PHY_REG_CONTROL_RESET;
  phy_regSet (phy, PHY_REG_CONTROL, cr);
  /* "The reset process shall be completed withing 0.5 s from the setting of bit 0.15." */
  tick = sc_tickGet ();
  sc_sleep (2); /* "The process will be waiting for at least the requested time minus one system tick." */
  tmo = tick + sc_tickMs2Tick (500);
  while (phy_regGet (phy, PHY_REG_CONTROL) & PHY_REG_CONTROL_RESET)
  {
    tick = sc_tickGet ();
    if (tick > tmo) {
      sc_miscError (SC_ERR_PROCESS_FATAL, (sc_extra_t) "tmo");
    }
    sc_sleep (2);
  }
  
  cr = phy_regGet (phy, PHY_REG_CONTROL);
  cr &= ~(0
	  | PHY_REG_CONTROL_ISOLATE
	  | PHY_REG_CONTROL_LOOPBACK
	  | PHY_REG_CONTROL_PWRDN);
  cr |= (0
	 | PHY_REG_CONTROL_AUTONEG_EN);

  phy_regSet(phy, PHY_REG_CONTROL, cr);
}

/* only supports 10Mb/s and 100Mb/s */
int
phy_doAutoNeg (__uint phy, sc_ticks_t tmo)
{
  __uint cr;
  __uint adv;

  cr = phy_regGet (phy, PHY_REG_CONTROL);
  /* check if AutoNeg is disabled */
  if ((cr & PHY_REG_CONTROL_AUTONEG_EN) == 0) {
    cr |= PHY_REG_CONTROL_AUTONEG_EN;
    phy_regSet (phy, PHY_REG_CONTROL, cr);
    /* check if the bit was really set */
    cr = phy_regGet (phy, PHY_REG_CONTROL);
    if ((cr & PHY_REG_CONTROL_AUTONEG_EN) == 0) {
      sc_miscError (SC_ERR_PROCESS_FATAL, cr);
    }
  }
  /* clear Next Page ability (needed for 1Gig ethernet) */
  adv = phy_regGet (phy, PHY_REG_AUTONEG_ADV);
  if (adv & PHY_REG_AUTONEG_BASE_NP) {
    adv ^= PHY_REG_AUTONEG_BASE_NP;
    phy_regSet (phy, PHY_REG_AUTONEG_ADV, adv);
  }
  /* disable MII isolate */
  cr &= ~PHY_REG_CONTROL_ISOLATE;
  /* restart AutoNeg (if not already started) */
  cr |= PHY_REG_CONTROL_AUTONEG_RS;
  phy_regSet (phy, PHY_REG_CONTROL, cr);

  if (tmo == SC_ENDLESS_TMO) {
    /* wait for AutoNeg to complete */
    while ((phy_regGet (phy, PHY_REG_STATUS) & PHY_REG_STATUS_AUTONEG_CP) == 0){
      sc_sleep (2);
    }
    return 1;
  }
  if (tmo == SC_NO_TMO || tmo == SC_FATAL_IF_TMO) {
    return 0;
  }
  tmo += sc_tickGet ();
  sc_sleep (2);
  /* wait for AutoNeg to complete */
  while ((phy_regGet (phy, PHY_REG_STATUS) & PHY_REG_STATUS_AUTONEG_CP) == 0)
  {
    if (sc_tickGet () > tmo) {
      return 0;
    }
    sc_sleep (2);
  }
  return 1;
}

macAddr_t
phy_idMacAddrGet (__uint phy)
{
  __uint phyId;
  __u32 btm;
  __uint top;
  macAddr_t addr;

  phyId = phy_regGet (phy, PHY_REG_ID1);
  btm  = ((__u32)  phyId          ) <<  2; /* first part of OUI */

  phyId = phy_regGet (phy, PHY_REG_ID2);
  btm |= ((__u32) (phyId & 0xfc00)) <<  8; /* second part of OUI */
  btm |= ((__u32) (phyId & 0x03f0)) << 20; /* model number */
  btm |= ((__u32) (phyId & 0x000c)) << 28; /* first part of revision number */
  addr.btm = btm;

  top  = ((__u32) (phyId & 0x0003)) << 14; /* second part of revision number */
  addr.top = top;

  return addr;
}

static int
phy_testLink (__uint phy, int speed, int duplex, sc_ticks_t tmo)
{
  int sri;
  __uint able;
  __uint link;
  __uint sr;
  __uint cr;

  if (duplex) {
    speed++;
  }
  switch (speed)
  {
    case 10: 
      sri = PHY_REG_STATUS;
      able = PHY_REG_STATUS_ABLE_10M_HD;
      link = PHY_REG_CONTROL_LINK_10M_HD;
      break;
    case 11: 
      sri = PHY_REG_STATUS;
      able = PHY_REG_STATUS_ABLE_10M_FD;
      link = PHY_REG_CONTROL_LINK_10M_FD;
      break;
    case 100: 
      sri = PHY_REG_STATUS;
      able = PHY_REG_STATUS_ABLE_100M_HD;
      link = PHY_REG_CONTROL_LINK_100M_HD;
      break;
    case 101: 
      sri = PHY_REG_STATUS;
      able = PHY_REG_STATUS_ABLE_100M_FD;
      link = PHY_REG_CONTROL_LINK_100M_FD;
      break;
    case 1000: 
      sri = PHY_REG_EXT_STATUS;
      able = PHY_REG_EXT_STATUS_ABLE_1000M_HD;
      link = PHY_REG_CONTROL_LINK_1000M_HD;
      break;
    case 1001: 
      sri = PHY_REG_EXT_STATUS;
      able = PHY_REG_EXT_STATUS_ABLE_1000M_FD;
      link = PHY_REG_CONTROL_LINK_1000M_FD;
      break;
    default:
      return -1;
  }
  /* check ability */
  sr = phy_regGet (phy, sri);
  if (sr & able) {
    /* set link */
    cr = phy_regGet (phy, PHY_REG_CONTROL);
    cr = (cr & ~PHY_REG_CONTROL_LINK) | link;
    phy_regSet (phy, PHY_REG_CONTROL, cr);
  } else {
    return -1;
  }
  /* wait for link up */
  if (tmo == SC_ENDLESS_TMO) {
    do {
      sc_sleep (2);
      sr = phy_regGet (phy, PHY_REG_STATUS);
    } while ((sr & PHY_REG_STATUS_LINK_UP) == 0);
    return 0;
  }
  if (tmo == SC_NO_TMO || tmo == SC_FATAL_IF_TMO) {
    sc_miscError (SC_ERR_PROCESS_FATAL, (sc_extra_t) "tmo");
    return -1;
  }
  tmo += sc_tickGet ();
  sr = phy_regGet (phy, PHY_REG_STATUS);
  if (sr & PHY_REG_STATUS_LINK_UP) {
    return 0;
  }
  sc_sleep (2);
  sr = phy_regGet (phy, PHY_REG_STATUS);
  while (!(sr & PHY_REG_STATUS_LINK_UP)) {
    if (sc_tickGet () > tmo) {
      return -1;
    }
    sc_sleep (2);
    sr = phy_regGet (phy, PHY_REG_STATUS);
  }
  return 0;
}

void
phy_doManNeg (__uint phy, __uint maxSpeed, sc_ticks_t tmo_per_try)
{
  static const __uint speed[3] = {10, 100, 1000};	/* in 1Mb/s */
  int res;
  int i;

  switch (maxSpeed)
  {
    case 1000: i = 5; break;
    case  100: i = 3; break;
    case   10: i = 1; break;
    default:
      sc_miscError (7, maxSpeed);
      return;
  }
  /* check if AutoNeg is enabled */
  __u16 cr = phy_regGet (phy, PHY_REG_CONTROL);
  if (cr & PHY_REG_CONTROL_AUTONEG_EN) {
    cr &= ~PHY_REG_CONTROL_AUTONEG_EN;
    phy_regSet (phy, PHY_REG_CONTROL, cr);
    /* check if the bit was really cleared */
    cr = phy_regGet (phy, PHY_REG_CONTROL);
    if (cr & PHY_REG_CONTROL_AUTONEG_EN) {
      sc_miscError (SC_ERR_PROCESS_FATAL, cr);
    }
  }

  for (; i >= 0; i--) {
    res = phy_testLink (phy, speed[i >> 1] , i & 1, tmo_per_try);
    if (res == 0) {
      return;
    }
  }
}

