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
** ID: S08290BS1                                                    **
** +Revision: 1.4.2.3 +                                                 **
** +Date: 2009/11/19 11:55:08 +                                     **
** Ethernet driver for Stellaris                                    **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <ossys/swap.h>
#include <sdd/sdd.msg>
#include <sdd/sdd.h>
#include <ips/device.h>
#include <ips/device.msg>
#include <logd/logd.h>

#include "config.h"
/* LMI Driver Lib */
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_ethernet.h"
#include "inc/hw_gpio.h"
#include "driverlib/ethernet.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/flash.h"
#include "driverlib/gpio.h"

#include <string.h>


#define NIFTAG 0x4e494630       /* NIF0 */
/*
** Internal messages
*/
#define ETH_LINK_UP   0x1234001
#define ETH_LINK_DOWN 0x1234002

typedef struct eth_s {
  int ref;
  sc_pid_t controllerPid;
  sc_pid_t isrPid;
  sc_pid_t linkPid;
  int haveLink;
  logd_t *logd;
  sdd_netbuf_t *tx;
  sdd_netbuf_t *rx;
  char mac[8];
  unsigned int mtu;
} eth_t;

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_netbuf_t netbuf;
};

static eth_t eth;

static void setup(eth_t *eth)
{
  __u32 temp;
  __u32 mac[2];
  //
  // Enable and Reset the Ethernet Controller.
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH);
  SysCtlPeripheralReset(SYSCTL_PERIPH_ETH);
  
  //
  // Enable Port F for Ethernet LEDs.
  //  LED0        Bit 3   Output
  //  LED1        Bit 2   Output
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3, GPIO_DIR_MODE_HW);
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3,
		   GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
  
  //
  // Configure the hardware MAC address for Ethernet Controller filtering of
  // incoming packets.
  //
  // For the LM3S6965 Evaluation Kit, the MAC address will be stored in the
  // non-volatile USER0 and USER1 registers.  These registers can be read
  // using the FlashUserGet function, as illustrated below.
  //
  FlashUserGet(&mac[0], &mac[1]);
  if( (mac[0] == 0xffffffff) || (mac[1] == 0xffffffff) ){
    strcpy(eth->mac,CNF_ETH_MAC);
  } else {
    //
    // Convert the 24/24 split MAC address from NV ram into a 32/16 split MAC
    // address needed to program the hardware registers, then program the MAC
    // address into the Ethernet Controller registers.
    //
    eth->mac[0] = ((mac[0] >>  0) & 0xff);
    eth->mac[1] = ((mac[0] >>  8) & 0xff);
    eth->mac[2] = ((mac[0] >> 16) & 0xff);
    eth->mac[3] = ((mac[1] >>  0) & 0xff);
    eth->mac[4] = ((mac[1] >>  8) & 0xff);
    eth->mac[5] = ((mac[1] >> 16) & 0xff);
  }
 
  /* set MAC hardware address */
  EthernetMACAddrSet(ETH_BASE, (__u8 *)&eth->mac[0]);

  /* maximum transfer unit */
  eth->mtu = CNF_ETH_MTU;

  /* Do whatever else is needed to initialize interface. */
  /* Disable all Ethernet Interrupts. */
  EthernetIntDisable(ETH_BASE, (ETH_INT_PHY | 
				ETH_INT_MDIO |
				ETH_INT_RXER |
				ETH_INT_RXOF | 
				ETH_INT_TX | 
				ETH_INT_TXER | 
				ETH_INT_RX));

  temp = EthernetIntStatus(ETH_BASE, false);
  EthernetIntClear(ETH_BASE, temp);

  /* Initialize the Ethernet Controller. */
  EthernetInitExpClk(ETH_BASE, SysCtlClockGet());

  /*
   * Configure the Ethernet Controller for normal operation.
   * - Enable TX Duplex Mode
   * - Enable TX Padding
   * - Enable TX CRC Generation
   * - Enable RX Multicast Reception
   */
  EthernetConfigSet(ETH_BASE, (ETH_CFG_TX_DPLXEN |
			       ETH_CFG_TX_CRCEN |
			       ETH_CFG_RX_AMULEN |   /* enable for Multicast */
			       ETH_CFG_TX_PADEN ));

  /* read and clear PHY status */
  temp = EthernetPHYRead(ETH_BASE,PHY_MR17);
  
  /* enable interrupt on link up/down */
  EthernetPHYWrite(ETH_BASE,PHY_MR17,(PHY_MR17_ANEGCOMP_IE |
				      PHY_MR17_LSCHG_IE));
}

static void eth_transmit(eth_t *eth)
{
  int len;

  len = SDD_NET_DATA_SIZE (eth->tx);

  if ( len > eth->mtu || len <= 0 ){
    sdd_netbufFree(&eth->tx);
  } else {
    len = len < 64 ? 64 : len;
    EthernetPacketPutNonBlocking(ETH_BASE,
				 SDD_NET_DATA(eth->tx),
				 len);    
  }
}

#define BC_ADDR "\xff\xff\xff\xff\xff\xff"

static void eth_receive(eth_t *eth)
{  
  long size;
  
  if ( !eth->rx ){
    eth->rx = sdd_netbufAlloc (0,
			       eth->mtu,
			       4, 
			       SC_DEFAULT_POOL,
			       SC_NO_TMO);
  }
  if ( eth->rx ){
    size = EthernetPacketGetNonBlocking(ETH_BASE, 
					SDD_NET_DATA(eth->rx),
					eth->mtu);
    if ( size > 0 ){
      if ( !memcmp(SDD_NET_DATA(eth->rx),BC_ADDR,6) ){
	eth->rx->pkttype = SDD_BROADCAST_PKT;
	memcpy(SDD_NET_DATA(eth->rx),eth->mac,6);
      } else if ( *SDD_NET_DATA(eth->rx) == 0x01 ){
         eth->rx->pkttype = SDD_MULTICAST_PKT;
      } else {
	eth->rx->pkttype = SDD_HOST_PKT;
      }
      eth->rx->protocol = ntohs (*(__u16 *) & (SDD_NET_DATA(eth->rx)[12]));
      eth->rx->tail = eth->rx->data + size;
      eth->rx->data += 14;
      eth->rx->base.id = SDD_NET_RECEIVE;
      sc_msgTx((sc_msgptr_t )&eth->rx,eth->linkPid,0);
    }
  } else {
    __u8 buf[4];
    kprintf(1,"No mem\n");
    /* do a dummy read to clear fifo */
    size = EthernetPacketGetNonBlocking(ETH_BASE, 
					buf,
					4);
  }
}

void eth_phyint(eth_t *eth)
{
  __u32 mii;
  sc_msg_t msg;

  mii = EthernetPHYRead(ETH_BASE,PHY_MR17);
  
  if ( (mii & (PHY_MR17_LSCHG_INT|PHY_MR17_ANEGCOMP_INT)) == 
       (PHY_MR17_LSCHG_INT|PHY_MR17_ANEGCOMP_INT) ) {

    /* notify controller */
    msg = sc_msgAlloc(sizeof(sc_msgid_t),
		      ETH_LINK_UP,
		      SC_DEFAULT_POOL,
		      SC_FATAL_IF_TMO);
    sc_msgTx(&msg, eth->controllerPid, 0);

    /* Enable the Ethernet Controller transmitter and receiver. */
    EthernetEnable(ETH_BASE);

    /* remember link state */
    eth->haveLink = 1;

  } else if ( mii & PHY_MR17_LSCHG_INT ){

    /* remember link state */
    eth->haveLink = 0;

    /* notify controller */
    msg = sc_msgAlloc(sizeof(sc_msgid_t),
		      ETH_LINK_DOWN,
		      SC_DEFAULT_POOL,
		      SC_FATAL_IF_TMO);
    sc_msgTx(&msg, eth->controllerPid, 0);

    /* shut down ethernet */
    EthernetDisable(ETH_BASE);
  }
}
  

SC_INT_PROCESS(SCI_eth,src)
{
  static const sc_msgid_t sel[2] = {
    SDD_NET_SEND,
    0
  };
  int stat;

  if ( src == 0 ){
    //
    // Read and Clear the interrupt.
    //
    stat = EthernetIntStatus(ETH_BASE, false);
    EthernetIntClear(ETH_BASE, stat);

    if ( stat & MAC_RIS_RXINT ){
      /* handle all packets in the fifo */
      while( (HWREG(ETH_BASE + MAC_O_NP) & MAC_NP_NPR_M) ){
//->	kprintf(0,"RX:%d\n", (HWREG(ETH_BASE + MAC_O_NP) & MAC_NP_NPR_M));
	eth_receive(&eth);
      }
    }
    
    if ( stat & MAC_RIS_TXEMP ){
      if ( eth.tx ){
	sc_msgFree((sc_msgptr_t)&eth.tx);
      }
      eth.tx = (sdd_netbuf_t *)sc_msgRx(SC_NO_TMO, sel, SC_MSGRX_MSGID);
      if ( eth.tx && eth.haveLink ){
	eth_transmit(&eth);
      } else if ( eth.tx ){
	sc_msgFree((sc_msgptr_t)&eth.tx);
      }
    }    
    
    if ( stat & MAC_RIS_PHYINT ){
      eth_phyint(&eth);
    }

  } else if ( src == 1 && eth.tx == NULL ){
    eth.tx = (sdd_netbuf_t *)sc_msgRx(SC_NO_TMO, sel, SC_MSGRX_MSGID);
    if ( eth.tx && eth.haveLink ){
      eth_transmit(&eth);
    } else if ( eth.tx ){
      sc_msgFree((sc_msgptr_t)&eth.tx);
    }
  } else if ( src == 2 ){
    /* Enable the Ethernet Interrupt handler. */
    IntEnable(INT_ETH);
    
    /* Enable Ethernet Interrupts: RX,TX and PHY */
    EthernetIntEnable(ETH_BASE, (ETH_INT_RX | 
				 ETH_INT_TX |
				 ETH_INT_PHY) );
    
  } else {
    kprintf(0,"ISR:tx not ready\n");
  }
}


static void registerDev(eth_t *hook)
{
  ips_dev_t * dev;

  logd_printf (hook->logd, LOGD_INFO, "Register device...");
  /* register ips device */
  dev =(ips_dev_t *) sc_msgAllocClr (sizeof (ips_dev_t), 
				     0, 
				     SC_DEFAULT_POOL,
				     SC_FATAL_IF_TMO);
  /* 
  ** object data
  */
  dev->object.base.error = 0;
  dev->object.base.handle = hook;
  dev->object.type = SDD_OBJ_TYPE | SDD_NET_TYPE;
  strncpy (dev->object.name, "eth0", SC_NAME_MAX);
  /* 
  ** set controller process id
  */
  dev->object.controller = hook->controllerPid;
  /* 
  ** set rx intr process 
  */
  dev->object.receiver = hook->isrPid;
  /* 
  ** set tx intr process 
  */
  dev->object.sender = hook->isrPid;
  /* 
  ** additional ips device datas
  */
  dev->type = IPS_DEV_TYPE_ETHER;
  dev->mtu = dev->mru = hook->mtu - 18;
  dev->hwaddr.len = 6;
  memcpy (dev->hwaddr.addr, hook->mac, 6);
  
  if (ips_devRegister (&dev) == -1) {
    logd_printf (hook->logd, LOGD_INFO, "failt: Device register error %d\n",
		 sc_miscErrnoGet ());
    sc_procKill (hook->isrPid,0);
    sc_procKill (hook->controllerPid, 0);
  }
  logd_printf (hook->logd, LOGD_INFO, "ok\n");  
}

static void unregisterDev(logd_t *logd)
{
  ips_dev_t * dev;
  dev =(ips_dev_t *) sc_msgAllocClr (sizeof (ips_dev_t), 
				     0, 
				     SC_DEFAULT_POOL,
				     SC_FATAL_IF_TMO);
  /* 
  ** object data (device name is enough)
  */
  strcpy (dev->object.name, "eth0");

  /* unregister ips device */
  logd_printf (logd, LOGD_INFO, "Link down. UnRegister device...");
  if (ips_devUnregister (dev) == -1) {
    logd_printf (logd, LOGD_INFO, "failt: Device unregister error %d\n",
		 sc_miscErrnoGet ());
    sc_miscError(0x1000|SC_ERR_PROCESS_FATAL,NIFTAG);
  }
  logd_printf (logd, LOGD_INFO, "ok\n");
  sc_msgFree((sc_msgptr_t)&dev);
}

SC_PROCESS(SCP_eth)
{
  sc_msg_t msg;

  memset((char *)&eth,0,sizeof(eth_t));

  eth.controllerPid = sc_procIdGet(NULL, SC_NO_TMO);
  eth.isrPid = sc_procIdGet("SCI_eth",sc_tickMs2Tick(100));  
  eth.linkPid = sc_procIdGet ("/SCP_link", sc_tickMs2Tick(100));
  eth.logd = logd_new ("/SCP_logd", LOGD_INFO, "eth0", 0, SC_FATAL_IF_TMO);
  logd_printf (eth.logd, LOGD_INFO, "Sciopta Ethernet driver\n");
  if ( eth.isrPid == SC_ILLEGAL_PID || eth.linkPid == SC_ILLEGAL_PID ){
    logd_printf(eth.logd, LOGD_SEVERE, 
		"Could not find SCI_eth and/or SCP_link\n");
    sc_procKill(SC_CURRENT_PID,0);
  }

  logd_printf(eth.logd, LOGD_INFO, "Setting up HW...");

  setup(&eth);

  logd_printf(eth.logd, LOGD_INFO, "done\n");
  
  /* start ETH-ISR */
  sc_trigger(eth.isrPid);

  for(;;){
    static const sc_msgid_t sel[6] = {
      ETH_LINK_UP,
      ETH_LINK_DOWN,
      SDD_NET_OPEN,
      SDD_NET_CLOSE,
      SDD_OBJ_RELEASE,
      0
    };
    msg = sc_msgRx(SC_ENDLESS_TMO, sel, SC_MSGRX_MSGID );
    switch( msg->id ){
    case ETH_LINK_UP:
      sc_msgFree(&msg);
      /* wait another 100ms after link is up */
      sc_sleep(sc_tickMs2Tick(100));
      registerDev(&eth);
      break;
    case ETH_LINK_DOWN:
      sc_msgFree(&msg);
      unregisterDev(eth.logd);
      break;
    case SDD_NET_OPEN:
      eth.ref++;
      msg->base.error = 0;
      msg->id++;
      break;
    case SDD_NET_CLOSE:
      --eth.ref;
      msg->base.error = 0;
      msg->id++;
      break;
    case SDD_OBJ_RELEASE:
      msg->base.error = SC_ENOTSUPP;
      msg->id++;
      break;
    }
    if ( msg ){
      sc_msgTx(&msg,sc_msgSndGet(&msg),0);
    }
  }
}
