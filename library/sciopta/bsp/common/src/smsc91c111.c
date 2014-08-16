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
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S05018BS2                                                    **
** +Revision: 1.11.4.1 +                                                 **
** +Date: 2009/08/11 10:05:25 +                                     **
** Lowlevel part of the driver for SMSC91C111                       **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <string.h>
#include <sdd/sdd.msg>
#include <sdd/sdd.h>
#include <logd/logd.h>

#include "smsc91c111_p.h"
#include "smsc91c111.h"

struct context_s {
  __u16 ptr;
  __u8 pnr;
  __u8 bsel;
};

static void smsc91c111_read(sdd_netbuf_t *buf);
static void smsc91c111_send(sdd_netbuf_t NEARPTR netbuf);
static void smsc91c111_reset(void);

/*
** Save/Restore chip-context
**
** These functions must be called with int's disabled !
*/
STATIC_INLINE void saveContext(struct context_s *context)
{
  context->bsel = SMSC_GETBANK();
  context->ptr = get_reg16(SMSC_PTR);   // set bank and read
  context->pnr = SMSC_GET8(SMSC_PNR);   // bank already set
}

STATIC_INLINE void restoreContext(struct context_s *context)
{
  put_reg16(SMSC_PTR,context->ptr);
  SMSC_PUT8(SMSC_PNR,context->pnr);
  SMSC_SELBANK(context->bsel);
}

/*
** Set base address
*/
void smsc91c111_setBase(unsigned int base)
{
  *(__vu8 *)(CNF_SMSC91C111_BASE+0x30e) = 1;
  *(__vu16 *)(CNF_SMSC91C111_BASE+0x300+(SMSC_BAR & 0xf)) = base | 1;
}

/*
** smsc91c111_reset
**
** Reset the chip
*/
STATIC_INLINE void smsc91c111_reset()
{
  /* wait min. 50ms after power up (APPN 9.6)*/
  sc_sleep(sc_tickMs2Tick(50)+1);

  /* Reset chip */
  put_reg16( SMSC_RCR, SMSC_RCR_SOFT_RST);
  put_reg16( SMSC_RCR, 0);

  /* wait min. 50ms after soft reset (APPN 9.6)*/
  sc_sleep(sc_tickMs2Tick(50)+1);

  /* Set config register (not touched by soft-reset */
  put_reg16( SMSC_CR, (SMSC_CR_EPH_POWER_EN | 
		       SMSC_CR_NO_WAIT | 
		       SMSC_CR_MASK));

  /* wait some time */
  sc_sleep(sc_tickMs2Tick(1)+1);

  put_reg16( SMSC_TCR, 0);

  put_reg16(SMSC_CTR,(get_reg16(SMSC_CTR) 
		      | SMSC_CTR_MASK
		      | SMSC_CTR_AUTO_RELEASE));

  /* reset MMU */
  put_reg16( SMSC_MMUCR, SMSC_MMUCR_RESET_MMU);

  smsc91c111_disableInt();
}

/*
** smsc91c111_init
**
** Check if an 91C111 is present and if
** initialise it and start autonegotiation.
**
** Returns: -1 : no chip found
**           0 : auto neg not succeeded
**           1 : auto neg succeeded
*/
int smsc91c111_init(__u8 *eth_mac)
{
  __u16 val;

  /* probe chip by reading the signature in BS register */
  val = get_reg16(SMSC_BS);

  if ((val & 0xFF00) != 0x3300) return -1;

  smsc91c111_reset();

  val = get_reg16(SMSC_EPHSR);

  /* write the MAC address */
  put_reg16( SMSC_IAR01, *(__u16 *)&eth_mac[0]);
  put_reg16( SMSC_IAR23, *(__u16 *)&eth_mac[2]);
  put_reg16( SMSC_IAR45, *(__u16 *)&eth_mac[4]);

  /*
  ** Clear multicast-table
  */
  put_reg16( SMSC_MT01, 0 );
  put_reg16( SMSC_MT23, 0 );
  put_reg16( SMSC_MT45, 0 );
  put_reg16( SMSC_MT67, 0 );
  
  /* configure the controller */
  put_reg16( SMSC_RCR,( 0
			| SMSC_RCR_STRIP_CRC
			| SMSC_RCR_RXEN 
//->			| SMSC_RCR_ALMUL 
//->			| SMSC_RCR_PRMS
	     ));
	   

  return 0;
}

/*
** smsc91c111_enableInt
**
** Enable most interrupts.
*/
#define ENABLED_INTS (SMSC_MSK_RCV_INT \
		      | SMSC_MSK_RX_OVRN_INT \
		      | SMSC_MSK_MDINT \
		      | SMSC_MSK_EPH_INT)

void smsc91c111_enableInt()
{
  /* ack old interrupts */
  put_reg8( SMSC_ACK, 0xff);
  /* enable interrupts */
  put_reg8( SMSC_MSK, ENABLED_INTS);
}

/*
** smsc91c111_disableInt()
**
** Disable and clear all MAC interrupts
*/
void smsc91c111_disableInt()
{
  /* disable interrupts */
  put_reg8( SMSC_MSK, 0);
  /* ack old interrupts */
  put_reg8( SMSC_ACK, 0xff);
}
  
/*
** smsc91c111_alloc
**
** Try to allocate a send-buffer. If succefull directly
** store the netbuf and enable TX_EMPTY_INT. Else
** deferre sending and activate ALLOC_INT to send the buffer
** in the interrupt service routine.
*/
int smsc91c111_alloc(sdd_netbuf_t NEARPTR netbuf)
{
  sc_cpuflags_t oldmask;
  int tmo;

  LOCK(oldmask);
  SMSC_SELBANK(2);
  while( SMSC_GET16(SMSC_MMUCR) & SMSC_MMUCR_BUSY){
    /* wait */
  }	
  SMSC_PUT16(SMSC_MMUCR, SMSC_MMUCR_ALLOC_FOR_TX);
  UNLOCK(oldmask);
  for( tmo = 16; tmo > 0 ; --tmo){
    if ( SMSC_GET8(SMSC_IST) & SMSC_IST_ALLOC_INT ){
      break;
    }
  }
  if ( !tmo ){
    SMSC_PUT8(SMSC_MSK, SMSC_GET8(SMSC_MSK) | SMSC_MSK_ALLOC_INT);
  } else {
    tmo = 1;
    SMSC_PUT8(SMSC_ACK, SMSC_ACK_TX_EMPTY_INT);
    smsc91c111_send(netbuf);
    SMSC_SELBANK(2);
    SMSC_PUT8(SMSC_MSK, 
	      SMSC_GET8(SMSC_MSK) | SMSC_MSK_TX_INT | SMSC_MSK_TX_EMPTY_INT);
  }
  return tmo;
}
/*
** smsc91c111_isr
**
** Handle all interrupts.
*/
int smsc91c111_isr(sdd_netbuf_t NEARPTR NEARPTR txbuf,sdd_netbuf_t NEARPTR netbuf)
{
  sc_msg_t msg;
  struct context_s context;
  __u16 irq;
  __u16 mask;
  __u16 val;

  saveContext(&context);

  // mask irqs
  SMSC_SELBANK(2);
  mask = SMSC_GET8(SMSC_MSK);
  SMSC_PUT8(SMSC_MSK,0);

  irq = SMSC_GET8(SMSC_IST) & mask;

  if( irq ){
    if ( (irq & SMSC_IST_ALLOC_INT) ){
      if ( *txbuf ){
	smsc91c111_send(*txbuf);
	sc_msgAcquire((sc_msgptr_t)txbuf);
	if ( (*txbuf)->returnTo && (*txbuf)->returnTo != SC_ILLEGAL_PID ){	
	  (*txbuf)->base.id = SDD_NET_SEND_REPLY;
	  sc_msgTx((sc_msgptr_t)txbuf,(*txbuf)->returnTo,0);
	} else {
	  sc_msgFree((sc_msgptr_t)txbuf);
	}
	mask |= SMSC_MSK_TX_INT;
      } else {
	while( SMSC_GET16(SMSC_MMUCR) & SMSC_MMUCR_BUSY){
	  /* wait */
	}	
	put_reg16( SMSC_MMUCR, SMSC_MMUCR_RESET_MMU);
      }
      mask &= ~SMSC_MSK_ALLOC_INT;
    }

    if ( irq & SMSC_IST_EPH_INT ){
      kprintf(1,"EPH\n");
      SMSC_SELBANK(0);
      SMSC_PUT8(SMSC_TCR, SMSC_GET8(SMSC_TCR) | SMSC_TCR_TXENA);
      val = get_reg16(SMSC_CTR);

    }
    
    if ( irq & SMSC_IST_MDINT ){
      /*
      ** Clear and enable Phy interrupts. (Read twice, please. Dont't ask.)
      */
      val = smsc91c111_read_phy(0,SMSC_PHY_STATUS_OUT);
      val = smsc91c111_read_phy(0,SMSC_PHY_STATUS_OUT);
      
      put_reg8(SMSC_ACK,SMSC_ACK_MDINT);

      val = (SMSC_PHY_BASE_MASK & 
	     ~(SMSC_PHY_STATUS_OUT_INT |
//->	       SMSC_PHY_STATUS_OUT_SPDDET |
//->	       SMSC_PHY_STATUS_OUT_DPLXDET |
	       SMSC_PHY_STATUS_OUT_LINK_FAIL));
      
      smsc91c111_write_phy(0,SMSC_PHY_MASK,val);
     

      msg = sc_msgAlloc(sizeof(sc_msgid_t),
			SMSC91C111_LINK_MSG,
			SC_DEFAULT_POOL,
			SC_FATAL_IF_TMO);

      sc_msgTx(&msg,sc_procPpidGet(SC_CURRENT_PID),0);

    } 
    
    if ( irq & SMSC_IST_RX_OVRN_INT ){
      /*
      ** Receiver overrun
      */
      put_reg8( SMSC_ACK, SMSC_ACK_RX_OVRN_INT);
      val = get_reg16(SMSC_RCR);
      val |= SMSC_RCR_RXEN;
      val &= ~SMSC_RCR_ABORT_ENB;
      SMSC_PUT16(SMSC_RCR,val);

      while( SMSC_GET16(SMSC_MMUCR) & SMSC_MMUCR_BUSY){
	/* wait */
      }	     
      put_reg16( SMSC_MMUCR, SMSC_MMUCR_RESET_MMU);
    }
    
    if ( irq & SMSC_IST_TX_INT ){
      /*
      ** Transmit interrupt.
      ** Since we use AUTO RELEASE, this is likely due to an error
      */
      SMSC_SELBANK(2);
      val = SMSC_GET16(SMSC_FIFO);
      if ( !(val & SMSC_FIFO_TEMPTY) ){       	
	SMSC_PUT8(SMSC_PNR,val & 0xff);
	SMSC_PUT16(SMSC_PTR,SMSC_PTR_READ|SMSC_PTR_AUTO_INCR);
	val = SMSC_GET16(SMSC_DATA);

	SMSC_PUT16(SMSC_MMUCR, SMSC_MMU_REL_PACKET);

	while( SMSC_GET16(SMSC_MMUCR) & SMSC_MMUCR_BUSY){
	  /* wait */
	}

	SMSC_SELBANK(0);
	SMSC_PUT16(SMSC_TCR, SMSC_GET16(SMSC_TCR)| SMSC_TCR_TXENA | SMSC_TCR_PAD_EN);
      }
      mask &= ~(SMSC_MSK_TX_INT|SMSC_MSK_TX_EMPTY_INT);      
      put_reg8( SMSC_ACK, SMSC_ACK_TX_INT);

    } 
    
    if ( irq & SMSC_IST_TX_EMPTY_INT ){
      /*
      ** Last sequence sent.
      */

      /* clear counter */
      val = get_reg16(SMSC_ECR);

      put_reg8( SMSC_ACK, SMSC_ACK_TX_EMPTY_INT);
      mask &= ~(SMSC_MSK_TX_EMPTY_INT|SMSC_MSK_TX_INT);

    } 

    if ( irq & SMSC_IST_RCV_INT ){
      smsc91c111_read(netbuf); 
    }
    
    irq = get_reg8(SMSC_IST) & mask;
  }
  
  if ( !(get_reg8(SMSC_TCR) & SMSC_TCR_TXENA) ){
    put_reg16(SMSC_TCR,SMSC_TCR_TXENA | SMSC_TCR_PAD_EN);
  }

  put_reg8(SMSC_MSK,mask|ENABLED_INTS);  
  restoreContext(&context);
  return irq;
}

/*
** smsc91c111_send
**
** Send one packet.
*/
static void smsc91c111_send(sdd_netbuf_t NEARPTR netbuf)
{
  unsigned int packet;
  unsigned int control;

#if SMSC91C111_32BIT_TX == 1
  __u32 *sdata;
#else
  __u16 *sdata;
#endif
  unsigned int len;
  __u8 * buf;

  len = SDD_NET_DATA_SIZE(netbuf);
  buf = SDD_NET_DATA(netbuf);

  SMSC_SELBANK(2);
  packet = SMSC_GET8(SMSC_ARR);
  if ( packet & SMSC_ARR_FAILED ){
    return;
  }

  SMSC_PUT8( SMSC_PNR, packet);

  SMSC_PUT16(SMSC_PTR,SMSC_PTR_AUTO_INCR);

  /* Pointer is now set, and the proper bank is selected for data writes. */
  /* Prepare header: */
#if SMSC91C111_ALWAYS_EVEN_TX == 1
  len = ( len + 1 ) & ~1;
#endif

#if SMSC91C111_32BIT_TX == 1
  SMSC_PUT32(SMSC_DATA,(len+6)<<16);
  sdata = (__u32 *)buf;
#if CPU == ARM
  if ( (int)buf & 2 ){
    SMSC_PUT16(SMSC_DATA,*(__u16 *)buf);
    sdata = (__u32 *)(buf+2);
    len -= 2;
  } 
#endif
  /* Put data into buffer */
  while( len >= 4 ){
    SMSC_PUT32(SMSC_DATA_HIGH,*sdata);
    ++sdata;
    len -= 4;
  }
  if ( len & 2 ){
    SMSC_PUT16(SMSC_DATA,*(__u16 *)sdata);
    sdata = (__u32 *)((int)sdata+2);
  }
#else
  sdata = (__u16 *)buf;
  SMSC_PUT16(SMSC_DATA,0);	   /* reserve space for status word */
  /* packet length (includes status, byte-count and control shorts) */
  SMSC_PUT16(SMSC_DATA,0x7FE & (len + 6));
  /* Put data into buffer */
  while( len >= 2 ){
    SMSC_PUT16(SMSC_DATA,*sdata);
    ++sdata;
    len -= 2;
  }
#endif
  control = SMSC_CONTROLBYTE_CRC;
#if SMSC91C111_ALWAYS_EVEN_TX == 0  
  if (len & 1) {
    control |= *(__u8 *)sdata;
    control |= SMSC_CONTROLBYTE_ODD;
  }
#endif
  SMSC_PUT16(SMSC_DATA,control);

  SMSC_PUT8(SMSC_ACK, SMSC_ACK_TX_EMPTY_INT);

  while( SMSC_GET16(SMSC_MMUCR) & 1 ){
    /* wait */
  }

  /* Enqueue the packet */
  SMSC_PUT16(SMSC_MMUCR,SMSC_MMU_ENQ_PACKET);
}

static void smsc91c111_read(sdd_netbuf_t *netbuf)
{
  __u16 stat;
  unsigned int len;
#if SMSC91C111_32BIT_RX == 1
  __u32  val;
  __u32 *sdata;
#else
  __u16  val;
  __u16 *sdata;
#endif /* SMSC91C111_32BIT_RX */

  netbuf->base.error = (sc_errcode_t)-1;
  netbuf->pkttype = SDD_HOST_PKT;

  SMSC_SELBANK(2);
  stat = SMSC_GET16(SMSC_FIFO);
  if ( stat & SMSC_FIFO_REMPTY ){
    return;
  }
    
  SMSC_PUT16(SMSC_PTR,SMSC_PTR_READ|SMSC_PTR_RCV|SMSC_PTR_AUTO_INCR);

  /* Read status and (word) length */
#if SMSC91C111_32BIT_RX == 1
  val  = SMSC_GET32(SMSC_DATA_HIGH);
  stat = val & 0xffff;
  len  = val >> 16;
#else
  stat = SMSC_GET16(SMSC_DATA);
  len  = SMSC_GET16(SMSC_DATA);
#endif /* SMSC91C111_32BIT_RX */

  len -= 6;             /* minus header/footer words */

  if ((stat & SMSC_RX_STATUS_BAD) == 0) {
    if (stat & SMSC_RX_STATUS_ODDFRM) {
      ++len;
    }

    if (stat & SMSC_RX_STATUS_BCAST){
      netbuf->pkttype = SDD_BROADCAST_PKT;
    }

    /* check for buffer size */
#if CPU == ARM
    len += 2;
    if (len > SDD_NET_DATA_SIZE(netbuf)){
      len = SDD_NET_DATA_SIZE(netbuf);
    }
#else
    if (len > SDD_NET_DATA_SIZE(netbuf)){
      len = SDD_NET_DATA_SIZE(netbuf);
    }
#endif

#if CPU == ARM
    /* In order to align IP header on a 4 byte boundary we read the
    ** 2 extra bytes in front of the ETH header
    */
    while( SMSC_GET16( SMSC_PTR ) & SMSC_PTR_NOT_EMPTY ){
      /* empty */
    }
    
    SMSC_PUT16(SMSC_PTR,SMSC_PTR_READ|SMSC_PTR_RCV|SMSC_PTR_AUTO_INCR|2);
#endif

#if SMSC91C111_32BIT_RX == 1
    sdata = (__u32 *)SDD_NET_DATA(netbuf);
#else
    sdata = (__u16 *)SDD_NET_DATA(netbuf);
#endif
    netbuf->tail = netbuf->data + len;

    while (len >= sizeof(*sdata) ) {
#if SMSC91C111_32BIT_RX == 1
      *sdata++ = SMSC_GET32(SMSC_DATA_HIGH);
#else
      *sdata++ = SMSC_GET16(SMSC_DATA);
#endif
      len -=sizeof(*sdata);
    }
    /* Read control word (and potential data) unconditionally */
#if SMSC91C111_32BIT_RX == 1
    val = SMSC_GET32(SMSC_DATA_HIGH);
    if (len & 2) {
      *(__u16 *)sdata = val & 0xFFFF;
      sdata = (__u32 *)((int)sdata + 2);
      len  -= 2;
      val >>= 16;
    }
#else
    val = SMSC_GET16(SMSC_DATA);
#endif /* SMSC91C111_32BIT_RX */
    if (val & SMSC_CONTROLBYTE_ODD) {
      /* last byte contains data */
      *(__u8 *)sdata = (__u8)(val & 0x00FF);
    }
#if CPU == ARM
    /* skip superfluos bytes */
    netbuf->data += 2;
#endif
    netbuf->base.error = 0;
  }

  /* Free packet */
  SMSC_PUT16(SMSC_MMUCR, SMSC_MMU_RMREL_RX_FRAME);

  while( SMSC_GET16(SMSC_MMUCR) & SMSC_MMUCR_BUSY ){
    /* wait */
  }
}

/*
** functions for accessing PHY
*/

__u16 smsc91c111_read_phy( __u8 phyaddr, __u8 phyreg)
{
  unsigned int i;
  unsigned int mask;  
  unsigned int input_idx, clk_idx;
  unsigned int value;
  __u16 mii_reg;
  __u8 bits[64];

  /* 32 consecutive ones on MDO to establish sync */
  for(clk_idx = 0; clk_idx < 32; ++clk_idx ){
    bits[clk_idx] = SMSC_MGMT_MDOE | SMSC_MGMT_MDO;
  }

  /* Start code <01> */
  bits[clk_idx++] = SMSC_MGMT_MDOE;
  bits[clk_idx++] = SMSC_MGMT_MDOE | SMSC_MGMT_MDO;

  /* Read command <10> */
  bits[clk_idx++] = SMSC_MGMT_MDOE | SMSC_MGMT_MDO;
  bits[clk_idx++] = SMSC_MGMT_MDOE;

  /* Output the PHY address, msb first */
  for (mask = 0x10; mask; mask >>= 1){
    if (phyaddr & mask)
      bits[clk_idx++] = SMSC_MGMT_MDOE | SMSC_MGMT_MDO;
    else
      bits[clk_idx++] = SMSC_MGMT_MDOE;
  }

  /* Output the phy register number, msb first */
  for (mask = 0x10; mask; mask >>= 1){
    if (phyreg & mask)
      bits[clk_idx++] = SMSC_MGMT_MDOE | SMSC_MGMT_MDO;
    else
      bits[clk_idx++] = SMSC_MGMT_MDOE;
  }

  /* Tristate and turnaround (1 bit times) */
  bits[clk_idx++] = 0;

  /* Input starts at this bit time */
  input_idx = clk_idx;

  /* Will input 16 bits */
  for (i = 0; i < 16; ++i){
    bits[clk_idx++] = 0;
  }

  /* last clock */
  bits[clk_idx++] = 0;

  /* Get the current MII register value */
  SMSC_SELBANK(3);
  mii_reg = SMSC_GET16(SMSC_MGMT);

  /* Turn off all MII Interface bits */
  mii_reg &= ~(SMSC_MGMT_MDOE | SMSC_MGMT_MCLK |
	       SMSC_MGMT_MDI | SMSC_MGMT_MDO);

  /* Clock all 64 cycles */ 
  for (i = 0; i < sizeof(bits); ++i){
    /* Clock Low - output data */
    SMSC_PUT16( SMSC_MGMT, mii_reg | bits[i]);
    UDELAY(PHY_DELAY);

    /* Clock Hi - input data */
    SMSC_PUT16( SMSC_MGMT, mii_reg | bits[i] | SMSC_MGMT_MCLK);
    UDELAY(PHY_DELAY);

    bits[i] |= get_reg16(SMSC_MGMT) & SMSC_MGMT_MDI;
  }

  /* Return to idle state */
  SMSC_PUT16( SMSC_MGMT, mii_reg);
  UDELAY(PHY_DELAY);

  /* Recover input data */
  for (value = 0, i = 0; i < 16; ++i){
    value <<= 1;
#if SIZEOF_INT >= 4
    value |= (bits[input_idx++] & SMSC_MGMT_MDI);
#else
    if (bits[input_idx++] & SMSC_MGMT_MDI)
      value |= 1;
#endif
  }
#if SIZEOF_INT >= 4
  value >>= 1;
#endif
  return value;
}

void 
smsc91c111_write_phy(__u8 phyaddr, __u8 phyreg, __u16 value)
{
  unsigned int i;
  unsigned int mask;
  unsigned int clk_idx;
  __u16 mii_reg;
  __u8 bits[64];

  /* 32 consecutive ones on MDO to establish sync */
  for(clk_idx = 0; clk_idx < 32; ++clk_idx ){
    bits[clk_idx] = SMSC_MGMT_MDOE | SMSC_MGMT_MDO;
  }

  /* Start code <01> */
  bits[clk_idx++] = SMSC_MGMT_MDOE;
  bits[clk_idx++] = SMSC_MGMT_MDOE | SMSC_MGMT_MDO;

  /* Write command <01> */
  bits[clk_idx++] = SMSC_MGMT_MDOE;
  bits[clk_idx++] = SMSC_MGMT_MDOE | SMSC_MGMT_MDO;

  /* Output the PHY address, msb first */
  for (mask = 0x10; mask; mask >>= 1) {
    if (phyaddr & mask)
      bits[clk_idx++] = SMSC_MGMT_MDOE | SMSC_MGMT_MDO;
    else
      bits[clk_idx++] = SMSC_MGMT_MDOE;
  }

  /* Output the phy register number, msb first */
  for (mask = 0x10; mask; mask >>= 1) {
    if (phyreg & mask)
      bits[clk_idx++] = SMSC_MGMT_MDOE | SMSC_MGMT_MDO;
    else
      bits[clk_idx++] = SMSC_MGMT_MDOE;
  }

  /* Tristate and turnaround (2 bit times) */
  bits[clk_idx++] = SMSC_MGMT_MDOE | SMSC_MGMT_MDO;;
  bits[clk_idx++] = SMSC_MGMT_MDOE;

  /* Write out 16 bits of data, msb first */
  for (mask = 0x8000; mask; mask >>= 1) {
    if (value & mask)
      bits[clk_idx++] = SMSC_MGMT_MDOE | SMSC_MGMT_MDO;
    else
      bits[clk_idx++] = SMSC_MGMT_MDOE;
  }

  /* Get the current MII register value */
  SMSC_SELBANK(3);
  mii_reg = SMSC_GET16(SMSC_MGMT);

  /* Turn off all MII Interface bits */
  mii_reg &= ~(SMSC_MGMT_MDOE | SMSC_MGMT_MCLK |
	       SMSC_MGMT_MDI | SMSC_MGMT_MDO);

  /* Clock all cycles */
  for (i = 0; i < sizeof(bits); ++i) {
    /* Clock Low - output data */
    SMSC_PUT16( SMSC_MGMT, mii_reg | bits[i]);
    UDELAY(PHY_DELAY);

    /* Clock Hi - input data */
    SMSC_PUT16( SMSC_MGMT, mii_reg | bits[i] | SMSC_MGMT_MCLK);
    UDELAY(PHY_DELAY);
  }

  /* Return to idle state */
  SMSC_PUT16( SMSC_MGMT, mii_reg);
  UDELAY(PHY_DELAY);
}

int smsc91c111_establishLink()
{
  int timeout;
  int ok = 1;
  __u16 myPhyCaps;
  __u16 myAdCaps;
  __u16 val16;
  __u16 mode;

  smsc91c111_write_phy(0,SMSC_PHY_CTRL,SMSC_PHY_CTRL_RST);

  for( timeout = 10; timeout ; --timeout ){
    sc_sleep(sc_tickMs2Tick(50));
    if ( !(smsc91c111_read_phy(0,SMSC_PHY_CTRL) & SMSC_PHY_CTRL_RST) ){
      break;
    }
  }
  if ( !timeout ){
    kprintf(1,"Error reseting PHY\n");
    return 0;
  }
  /*
  ** Disable Auto-negotiation; Enable internal Phy
  */
  smsc91c111_write_phy(0,SMSC_PHY_CTRL, 0);
  
  /*
  ** Copy our capabilities from PHY_STAT_REG to PHY_AD_REG
  */
  myPhyCaps = smsc91c111_read_phy(0,SMSC_PHY_STAT);

  myAdCaps = smsc91c111_read_phy(0,SMSC_PHY_AUTO_AD);
  myAdCaps &= 0xFC1F;

  if (myPhyCaps & SMSC_PHY_STAT_CAP_T4)
    myAdCaps |= SMSC_PHY_AUTO_AD_T4;
#if SMSC91C111_CAN_100MB == 1
  if (myPhyCaps & SMSC_PHY_STAT_CAP_TXF)
    myAdCaps |= SMSC_PHY_AUTO_AD_TX_FDX;
  
  if (myPhyCaps & SMSC_PHY_STAT_CAP_TXH)
    myAdCaps |= SMSC_PHY_AUTO_AD_TX_HDX;
#endif  
  if (myPhyCaps & SMSC_PHY_STAT_CAP_TF)
    myAdCaps |= SMSC_PHY_AUTO_AD_10_FDX;
  
  if (myPhyCaps & SMSC_PHY_STAT_CAP_TH)
    myAdCaps |= SMSC_PHY_AUTO_AD_10_HDX;
  
  myAdCaps |= SMSC_PHY_AUTO_AD_CSMA;

  /*
  ** Update our Auto-Neg Advertisement Register
  */
  smsc91c111_write_phy(0,SMSC_PHY_AUTO_AD, myAdCaps);

  /*
  ** Enable the autonegotiation mode (Set the MAC Register) 
  */
  put_reg16(SMSC_RPCR,SMSC_RPCR_ANEG);
  
  /* 
  ** Clear Phy Status Output Register
  */
  (void)smsc91c111_read_phy(0,SMSC_PHY_STATUS_OUT);

  /*
  ** Restart auto-negotiation process in order to advertise my caps
  */
  smsc91c111_write_phy(0,
		      SMSC_PHY_CTRL,
		      SMSC_PHY_CTRL_ANEG_RST | SMSC_PHY_CTRL_ANEG_EN);

//->  for( i = 0 ; i < 10; ++ i ){
//->    kprintf(0,".");
//->    sc_sleep(sc_tickMs2Tick(200));
//->  }

  /*
  ** Wait for the auto-negotiation to complete.  This may take from
  ** 2 to 3 seconds.
  ** Wait for completion, or time out
  */
  
  for( timeout = 16; timeout; --timeout){
    val16 = smsc91c111_read_phy(0,SMSC_PHY_STAT);
//->    kprintf(1,"phy-stat: %04x\n",val16);
    if ( (val16 & SMSC_PHY_STAT_ANEG_ACK) &&
	 (val16 & SMSC_PHY_STAT_LINK )){
      break;
    }
    if ( val16 & SMSC_PHY_STAT_REM_FLT ){
      kprintf(1,"Remote failed to autonegotiate, restarting ...\n");
      smsc91c111_write_phy(0,
			  SMSC_PHY_CTRL,
			  SMSC_PHY_CTRL_ANEG_RST | SMSC_PHY_CTRL_ANEG_EN |
			  SMSC_PHY_CTRL_SPEED | SMSC_PHY_CTRL_DPLX);
      /* reset timeout */
      timeout += 4;
      continue;
    }      
    sc_sleep(sc_tickMs2Tick(125));
  }

  if ( !timeout || (val16 & SMSC_PHY_STAT_REM_FLT) ){
    ok = 0;
    goto error_exit;
  }
  
  /*
  ** Read PHY Register 18, Status Output
  */
  val16 = smsc91c111_read_phy(0,SMSC_PHY_STATUS_OUT);

  /*
  ** Set TCR register to match auto-negotiation results
  */
  mode = SMSC_TCR_TXENA | SMSC_TCR_PAD_EN;
#if 0 // ignored if ANEG == 1
  if ( val16 & SMSC_PHY_STATUS_OUT_DPLXDET ){
    mode |= SMSC_TCR_SWFDUP;
  }
#endif
  put_reg16(SMSC_TCR,mode);

  /*
  ** Set RPCR register to match auto-negotiation results
  */
  mode = get_reg16(SMSC_RPCR) & SMSC_RPCR_ANEG;
  mode |= SMSC_RPCR_LEDA_RX | SMSC_RPCR_LEDB_LINK;

#if 0 // ignored if ANEG == 1
  if ( val16 & SMSC_PHY_STATUS_OUT_SPDDET ){
    mode |= SMSC_RPCR_SPEED;
  }

  if ( val16 & SMSC_PHY_STATUS_OUT_DPLXDET ){
    mode |= SMSC_RPCR_DPLX;
  }
#endif

  put_reg16(SMSC_RPCR,mode);

error_exit:
  /* 
  ** Enable auto release
  */
  put_reg16(SMSC_CTR,SMSC_CTR_AUTO_RELEASE|SMSC_CTR_MASK);

  /*
  ** Clear and enable Phy interrupts. (Read twice, please. Dont't ask.)
  */
  val16 = smsc91c111_read_phy(0,SMSC_PHY_STATUS_OUT);
  val16 = smsc91c111_read_phy(0,SMSC_PHY_STATUS_OUT);

  val16 = (SMSC_PHY_BASE_MASK & 
	   ~(SMSC_PHY_STATUS_OUT_INT|
	     SMSC_PHY_STATUS_OUT_LINK_FAIL));

  smsc91c111_write_phy(0,SMSC_PHY_MASK,val16);

  return ok;
}

void smsc91c111_dumpPhy(logd_t NEARPTR logd)
{
  __u16 phy[10];
  unsigned int i;
  for(i = 0; i < 10 ; ++i ){
    if ( i < 6 ){
      phy[i] = smsc91c111_read_phy(0,i);
    } else {
      phy[i] = smsc91c111_read_phy(0,i+10);
    }
  }

  logd_printf(logd, LOGD_INFO,
	      "SMSC91C111 Phy 0:\n"
	      "( 0, 1) : %04X%04X\n"
	      "( 2, 3) : %04X%04X (should be 0016F840)\n"
	      "( 4, 5) : %04X%04X\n"
	      "(16,17) : %04X%04X\n"
	      "(18,19) : %04X%04X\n",
	      phy[0],phy[1],
	      phy[2],phy[3],
	      phy[4],phy[5],
	      phy[16-10],phy[17-10],
	      phy[18-10],phy[19-10]);
}

#if (CNF_SMSC91C111_EEPROM == 1)
static void
smsc91c111_eepromEnter (struct context_s *context, __u8 *mask)
{
  sc_cpuflags_t oldmask;
  
  /* Save the context */
  LOCK(oldmask);
  saveContext(context);
  UNLOCK(oldmask);
  /* and disable smsc interrups */
  *mask = get_reg8(SMSC_MSK);
  put_reg8( SMSC_MSK, 0);
}

static void
smsc91c111_eepromLeave (struct context_s *context, __u8 mask)
{
  sc_cpuflags_t oldmask;

  /* Restore interrupts */
  put_reg8( SMSC_MSK, mask);
  /* and restore the context. */
  LOCK(oldmask);
  restoreContext(context);
  UNLOCK(oldmask);
}

static void 
smsc91c111_eepromReadWord (__u16 pos, __u16 *data)
{
  /* Wait until eeprom is ready */
  SMSC_SELBANK (1);
  while (SMSC_GET16 (SMSC_CTR) & SMSC_CTR_EEPROM_BUSY);
  /* Set pointer register. */
  put_reg16 (SMSC_PTR, pos);
  /* Set control register. */
  put_reg16 (SMSC_CTR, (SMSC_CTR_EEPROM_SELECT | 
			SMSC_CTR_RELOAD | 
			SMSC_CTR_MASK)); 
  /* Wait until reload */
  SMSC_SELBANK (1);
  while (SMSC_GET16 (SMSC_CTR) & SMSC_CTR_RELOAD);
  /* Fill the general purpose register with data */
  *data = get_reg16 (SMSC_GPR); 
}

static void 
smsc91c111_eepromWriteWord (__u16 pos, __u16 data)
{
  /* Wait until eeprom is ready */
  SMSC_SELBANK (1);
  while (SMSC_GET16 (SMSC_CTR) & SMSC_CTR_EEPROM_BUSY);
  /* Set pointer register. */
  put_reg16 (SMSC_PTR, pos);
  /* to general purpose register. */
  put_reg16 (SMSC_GPR, data); 
  /* Set control register. */
  put_reg16 (SMSC_CTR, (SMSC_CTR_EEPROM_SELECT | 
			SMSC_CTR_STORE |
			SMSC_CTR_MASK)); 
  /* wait until finished write word */
  while (SMSC_GET16 (SMSC_CTR) & SMSC_CTR_STORE);
}

void 
smsc91c111_eepromRead (__u16 start, char *buf, size_t len)
{
  struct context_s context;
  __u8 mask;
  __u16 wordStart;
  int words, before, after;
  int i;
  __u16 data;

  smsc91c111_eepromEnter (&context, &mask); 

  wordStart = start / 2;
  before = start - wordStart * 2;
  words = len / 2;
  after = len - words * 2;
 
  i = 0; 

  /* Bytes before word boundary */
  if (before) {
    /* Get word */
    smsc91c111_eepromReadWord (wordStart, &data);
    /* fill it to buf */
    memcpy (buf, &((__u8 *) &data)[before], 2 - before);
    i++;
  }
  
  /* Data on word boundary */
  for (; i < words; i++) { 
    /* Get word */
    smsc91c111_eepromReadWord (wordStart + i, &data);
    /* fill it to buf */
    memcpy (&buf[i * 2], &data, 2);
  }

  /* Rest data counted in after */
  if (after) {
    ++i;
    /* Get word */
    smsc91c111_eepromReadWord (wordStart + i, &data);
    /* fill it to buf */
    memcpy (&buf[i * 2], &data, after);
  }
  
  smsc91c111_eepromLeave (&context, mask); 
}

void
smsc91c111_eepromWrite (__u16 start, const char *buf, size_t len)
{
  struct context_s context;
  __u8 mask;
  __u16 wordStart;
  int words, before, after;
  int i;
  __u16 data;

  smsc91c111_eepromEnter (&context, &mask); 
 
  wordStart = start / 2;
  before = start - wordStart * 2;
  words = len / 2;
  after = len - words * 2;
 
  i = 0; 

  /* Bytes before word boundary */
  if (before) {
    /* Fill it to data */
    smsc91c111_eepromReadWord (wordStart, &data);
    memcpy (&((__u8 *) &data)[before], buf, 2 - before);
    /* Get word */
    smsc91c111_eepromWriteWord (wordStart, data);
    i++;
  }
  
  /* Data on word boundary */
  for (; i < words; i++) { 
    /* Fill it to data */
    memcpy (&data, &buf[i * 2], 2);
    /* Get word */
    smsc91c111_eepromWriteWord (wordStart + i, data);
  }

  /* Rest data counted in after */
  if (after) {
    ++i;
    /* fill it to buf */
    memcpy (&data, &buf[i * 2], after);
    /* Get word */
    smsc91c111_eepromWriteWord (wordStart + i, data);
  }
  
  smsc91c111_eepromLeave (&context, mask); 
}
#endif
