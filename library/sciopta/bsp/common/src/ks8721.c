/*
 * File:        ks8721.c
 * Purpose:     Driver for the Micrel KS8721 10/100 Ethernet PHY
 *
 * Notes:       
 */

#include <ossys/types.h>
#include <ks8721.h>

int mii_write(__u8 ch, __u8 phy_addr, __u8 reg_addr, __u16 data);
int mii_read(__u8 ch, __u8 phy_addr, __u8 reg_addr, __u16 *data);

//->#define DEBUG_PRINT
/********************************************************************/
/* Initialize the KS8721 PHY
 *
 * This function sets up the Auto-Negotiate Advertisement register
 * within the PHY and then forces the PHY to auto-negotiate for
 * it's settings.
 * 
 * Params:
 *  fec_ch      FEC channel
 *  phy_addr    Address of the PHY.
 *  speed       Desired speed (10BaseT or 100BaseTX)
 *  duplex      Desired duplex (Full or Half)
 *
 * Return Value:
 *  0 if MII commands fail
 *  1 otherwise
 */
int
ks8721_init(__u8 fec_ch, __u8 phy_addr, __u8 speed, __u8 duplex)
{
  __u16 settings;

  /* Disable Auto-Negotiation */
  if (!mii_write(fec_ch, phy_addr, KS8721_CTRL, 0)){
    return -1;
  }
  /* Reset the PHY */
  if (!mii_write(fec_ch, phy_addr, KS8721_CTRL, KS8721_CTRL_RESET)){
    return -1;
  }

  /* 
   * Read back the contents of the CTRL register and verify
   * that RESET is not set - this is a sanity check to ensure
   * that we are talking to the PHY correctly. RESET should
   * always be cleared.
   */
  if (!mii_read(fec_ch, phy_addr, KS8721_CTRL, &settings)){
    return -1;
  }

  if (settings & KS8721_CTRL_RESET){
    return -1;
  }

  if (speed == MII_10BASE_T)
  {
    settings = (__u16)((duplex == MII_FULL_DUPLEX) 
		       ? (KS8721_AN_ADV_10BT_FDX | KS8721_AN_ADV_10BT) 
		       : KS8721_AN_ADV_10BT);
  }
  else /* (speed == MII_100BASE_TX) */
  {
    settings = (__u16)((duplex == MII_FULL_DUPLEX)  
		       ? (KS8721_AN_ADV_100BTX_FDX | KS8721_AN_ADV_100BTX
			  | KS8721_AN_ADV_10BT_FDX   | KS8721_AN_ADV_10BT) 
		       : (KS8721_AN_ADV_100BTX | KS8721_AN_ADV_10BT));
  }
     
  /* Set the Auto-Negotiation Advertisement Register */
  if (!mii_write(fec_ch, phy_addr, KS8721_AN_ADV, settings)){
    return -1;
  }

  /* Enable Auto-Negotiation */
  if (!mii_write(fec_ch, phy_addr, KS8721_CTRL, (KS8721_CTRL_ANE
						 | KS8721_CTRL_RESTART_AN))){
    return -1;
  }

  return 0;
}

int ks8721_checkLink(int fec_ch, int phy_addr)
{
  __u16 settings;
  int ret;

  /* Read PHY status register */
  if (!mii_read(fec_ch, phy_addr, KS8721_STAT, &settings)){
    return -1;
  }
  if (!(settings & KS8721_STAT_AN_COMPLETE)){
    return -1;
  }

  /* 
   * Read 100BaseTX PHY Control Register 
   */
  if (!mii_read(fec_ch, phy_addr, KS8721_100TX_CTRL, &settings)){
    return -1;
  }

  /*
   * Set the proper duplex in the FEC now that we have auto-negotiated
   */
  if (((settings & KS8721_100TX_CTRL_MODE_MASK) == KS8721_100TX_CTRL_MODE_10HDX) ||
      ((settings & KS8721_100TX_CTRL_MODE_MASK) == KS8721_100TX_CTRL_MODE_100HDX))
  {
    ret = MII_HALF_DUPLEX;
  } else {
    ret = MII_FULL_DUPLEX;
  }

#ifdef DEBUG_PRINT
  kprintf(0,"\nPHY Mode: ");
  switch (settings & KS8721_100TX_CTRL_MODE_MASK)
  {
  case KS8721_100TX_CTRL_MODE_AN : 
    kprintf(0,"Auto-negotiating\n");
    break;
  case KS8721_100TX_CTRL_MODE_10HDX :    
    kprintf(0,"10Mbps Half-duplex\n");
    break;
  case KS8721_100TX_CTRL_MODE_100HDX : 
    kprintf(0,"100Mbps Half-duplex\n");
    break;
  case KS8721_100TX_CTRL_MODE_DEFAULT : 
    kprintf(0,"Default\n");
    break;
  case KS8721_100TX_CTRL_MODE_10FDX : 
    kprintf(0,"10Mbps Full-duplex\n");
    break;
  case KS8721_100TX_CTRL_MODE_100FDX : 
    kprintf(0,"100Mbps Full-duplex\n");
    break;
  case KS8721_100TX_CTRL_MODE_ISOLATE : 
    kprintf(0,"Isolation\n");
    break;
  default:
    kprintf(0,"Unknown\n");
  }
#endif

  return ret;
}
/********************************************************************/
