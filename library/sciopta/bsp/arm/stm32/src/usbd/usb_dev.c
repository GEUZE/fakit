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
**        (c) 2007 Sciopta Systems GmbH, Sciopta AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S07051BS3                                                    **
** +Revision: 1.1.2.1 +                                                 **
** +Date: 2009/08/20 09:09:02 +                                     **
** USB low level driver for mcf532x                                 **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <dev/usb.h>
#include "usbd.h"
#include "usbd.msg"
#include <string.h>

#include <mcf532x.h>

void d(void *);

#define DEBUG_PRINT


union sc_msg {
  sc_msgid_t id;
  usb_setup_req_t  setup_req;
  usb_ctrlPacket_t ctrl;
  usb_setDesc_t    setDesc;
  usb_epConfig_t   epConfig;
  usb_epComplete_t epComplete;
  usb_epReset_t    epReset;
  usb_sendData_t   sendData;
  usb_recvData_t   recvData;
  usb_suspend_t    suspend;
};

#define SWAP16(a) (((a & 0xff)<<8)|((a>>8) & 0xff))

/*
** Public processes
*/
SC_INT_PROCESS(SCI_usbd,src);

/*
** Internal function prototypes
*/
static void usbd_isr(usbd_t *usbd);
static void reset_irq(usbd_t *usbd);
static void dtd_complete_irq(usbd_t *usbd);

static void usb_get_ep0_setup(usbd_t *usbd, __u32 setup[2]);
static int usb_handle_setup(usbd_t *usbd,usb_deviceRequest_t* setup);
static void usb_handle_device_setup(usbd_t *usbd,usb_deviceRequest_t* setup);
static void reset_ep_queue(usbd_t *usbd,__uint ep_num);

static void usb_device_send_cntrl_packet(usbd_t *usbd, 
				  __u32 epnum, 
				  const __u8* buf, 
				  __u32 size);
static void Ep0Stall();
static void usb_send_data(usbd_t *usbd,__uint ep_num,usb_dtd_t *dtd);
static void usb_recv_data(usbd_t *usbd,__uint ep_num,usb_dtd_t *dtd);
static void reset_queues(usbd_t *usbd);

static void usb_ep_qh_init(usbd_t *usbd, 
			   __u32 ep_num, 
			   __u32 dir,
			   __u32 mult, 
			   __u32 max_packet, 
			   __u32 ios, 
			   usb_dtd_t * next_dtd);

static int get_port_speed(int);
static ep_queue_head_t *usb_device_ep0_init(void);
static void usb_device_init(usbd_t *usbd);
/********************************************************************/
#if 0
static inline void __flush_cache_all(void)
{
	__asm__ __volatile__ (
        	"movel	#0x81000200, %%d0\n\t"
        	"movec	%%d0, %%CACR\n\t"
		"nop\n\t"
		: : : "d0" );
}
#endif
static inline int swap32(int data)
{
  __u32 swapped = 0;

  swapped |= data & 0x000000ff;
  swapped <<= 8;
  data >>= 8;
  swapped |= data & 0x000000ff;
  swapped <<= 8;
  data >>= 8;
  swapped |= data & 0x000000ff;
  swapped <<= 8;
  data >>= 8;
  swapped |= data & 0x000000ff;

  return(swapped);
}

SC_INT_PROCESS(SCI_usbd,src)
{
  static usbd_t usbd;
  sc_msg_t msg;
  __uint ep_num;
  __u32 epcr;

  switch ( src ){
  case -2:
    break;
  case -1:
    memset((char *)&usbd,0,sizeof(usbd));
    
    usb_device_init(&usbd);
    
    MCF_USB_USBINTR(USB_OTG) = (0
				| MCF_USB_USBINTR_SLE
				| MCF_USB_USBINTR_URE
				| MCF_USB_USBINTR_UE
				| MCF_USB_USBINTR_PCE
				| 0);
    
    MCF_INTC1_ICR47 = 2;
    MCF_INTC1_IMRH &= ~MCF_INTC_IPRH_INT47; /* => vector 175 */
    break;
  case 0:
    usbd_isr(&usbd);
    break;
  case 1:
    msg = sc_msgRx(SC_NO_TMO,NULL, SC_MSGRX_MSGID);
    if ( msg ){
      switch( msg->id ){
      case USB_SET_DEVICE_DESC_MSG:
	usbd.epClient[0] = sc_msgSndGet(&msg);
	usbd.deviceDesc = msg->setDesc.desc;
	usbd.deviceDescSize = msg->setDesc.size;
	break;
      case USB_SET_CONFIG_DESC_MSG:
	usbd.configDesc = msg->setDesc.desc;
	usbd.configDescSize = msg->setDesc.size;
	break;
      case USB_EP_CONFIG_MSG:
	epcr = 0;
	ep_num = msg->epConfig.ep_num;

	if ( msg->epConfig.enabled ){
	  if ( msg->epConfig.in_mode ){
	    epcr = (msg->epConfig.in_mode & 3)<<18;
	    epcr |= MCF_USB_EPCR_TXE;

	    /* Initialize IN endpoint queue head */
	    usb_ep_qh_init(&usbd, 
			   ep_num, 
			   EP_DIR_IN, 
			   0, 
			   msg->epConfig.in_size+1, /* +1 for win2k */
			   1,
			   NULL);
	
	  } else {
	    epcr = MCF_USB_EPCR_TXT_BULK;
	  }
	  if ( msg->epConfig.out_mode ){
	    epcr |= (msg->epConfig.out_mode & 3)<<2;
	    epcr |= MCF_USB_EPCR_RXE;
 
	    /* Initialize IN endpoint queue head */

	    /* XXX: Possible bug in mcf532x: If dtd.size bytes
	    ** are received, the active-bit is cleared, but
	    ** no complete interrupt comes.
	    */

	    usb_ep_qh_init(&usbd, 
			   ep_num, 
			   EP_DIR_OUT, 
			   0, 
			   msg->epConfig.out_size+1,
			   1,
			   NULL);

	  } else {
	    epcr |= MCF_USB_EPCR_RXT_BULK;
	  }
	  
	  usbd.epClient[ep_num] = sc_msgSndGet(&msg);
//->	  kprintf(0,"DCD:EP%d setup: %08x\n",ep_num,epcr);
	} else {
	  reset_ep_queue(&usbd,ep_num);
	  usb_ep_qh_init(&usbd, 
			 ep_num, 
			 EP_DIR_OUT, 
			 0, 
			 0,
			 0,
			 NULL);

	  usb_ep_qh_init(&usbd, 
			 ep_num, 
			 EP_DIR_IN, 
			 0, 
			 0,
			 0,
			 NULL);
	  usbd.epClient[ep_num] = SC_ILLEGAL_PID;
	}
	MCF_USB0_EPCR(ep_num) = epcr;
	break;
      case USB_CTRL_PACKET_MSG:
	usb_device_send_cntrl_packet(&usbd,
				     msg->ctrl.ep_num,
				     msg->ctrl.data,
				     msg->ctrl.size);
	break;	

      case USB_SEND_DATA_MSG(1):
      case USB_SEND_DATA_MSG(2):
      case USB_SEND_DATA_MSG(3):
	usb_send_data(&usbd,msg->sendData.ep_num,msg->sendData.dtd);
	break;
      case USB_RECV_DATA_MSG(0):
      case USB_RECV_DATA_MSG(1):
      case USB_RECV_DATA_MSG(2):
      case USB_RECV_DATA_MSG(3):
	usb_recv_data(&usbd,msg->recvData.ep_num,msg->recvData.dtd);

	break;
      }
      sc_msgFree(&msg);
    }
    break;
  case 2:
    break;
  default:
    sc_miscError(SC_ERR_FATAL,src);
  }
}

/*
** Low-level driver
*/
static void usbd_isr(usbd_t *usbd)
{
  __u32 sts;
  int i;
  sc_msg_t msg;

  __flush_cache_all();

  sts = MCF_USB0_USBSTS & MCF_USB0_USBINTR;
  
  MCF_USB0_USBSTS &= sts;

//->  kprintf(0,"------------------\n");
//->  kprintf(0,"DCD:-> STS: %08x\n",sts);

  if ( sts & MCF_USB_USBSTS_PCI ){ 
    get_port_speed(USB_OTG);
  }

  if ( sts & MCF_USB_USBSTS_URI ){
    reset_irq(usbd);
  }

  if ( sts & MCF_USB_USBSTS_SLI ){ 
    usbd->resume_state = usbd->state;
    usbd->state = USB_SUSPENDED_STATE;
    for(i = 1; i < USB_MAX_EP; ++i ){
      if ( usbd->epClient[i] && usbd->epClient[i] != SC_ILLEGAL_PID ){
	msg = sc_msgAlloc(sizeof(usb_suspend_t),
			  USB_SUSPEND_MSG,
			  SC_DEFAULT_POOL,
			  SC_FATAL_IF_TMO);
	msg->suspend.ep_num = i;
	sc_msgTx(&msg,usbd->epClient[i],0);
      }
    }
  } else if ( usbd->resume_state ){
    usbd->state = usbd->resume_state;
    usbd->resume_state = 0;
  }
  
  if ( sts & MCF_USB_USBSTS_UI ){ 
    /* Setup package, we only support ep0 as control ep */
    if (MCF_USB0_EPSETUPSR & MCF_USB_EPSETUPSR_EPSETUPSTAT(1)) {
      usb_get_ep0_setup(usbd,(__u32 *)&usbd->setup_buf);
      if ( usb_handle_setup(usbd,&usbd->setup_buf) == 0 ){
	usb_handle_device_setup(usbd,&usbd->setup_buf);
      }
    } 
    /* completion of dtd */
    dtd_complete_irq(usbd);   
  }
}


static void usb_get_ep0_setup(usbd_t *usbd, __u32 setup[2])
{
  
  ep_queue_head_t * qh;

  qh = &usbd->ep_queue_head[0];

  /* Clear bit in ENDPTSETUPSTAT */
  MCF_USB0_EPSETUPSR |= MCF_USB_EPSETUPSR_EPSETUPSTAT(1);
  /* while a hazard exists when setup package arrives */
  do {
    /* Set Setup Tripwire */
    MCF_USB0_USBCMD|=MCF_USB_USBCMD_SUTW;

    /* Copy the setup packet to local buffer */
    setup[0] = swap32(qh->setup_buffer[0]);
    setup[1] = swap32(qh->setup_buffer[1]);
  } while (!(MCF_USB0_USBCMD & MCF_USB_USBCMD_SUTW));

  /* Clear Setup Tripwire */
  MCF_USB0_USBCMD &=~MCF_USB_USBCMD_SUTW;
}



static void Ep0Stall()
{
  MCF_USB0_EPCR(0)|=MCF_USB_EPCR_TXS|MCF_USB_EPCR_RXS;
}


static void usb_handle_device_setup(usbd_t *usbd,usb_deviceRequest_t* setup)
{ 
  sc_msg_t msg;
  
  if ( usbd->epClient[0] && usbd->epClient[0] != SC_ILLEGAL_PID ){
//->  kprintf(0,"DCD:type %02x req %02x val %04x idx %04x len %04x\n",
//->	  setup->bmRequestType,setup->bRequest,
//->	  setup->wValue,setup->wIndex,setup->wLength);

    msg = sc_msgAlloc(sizeof(usb_setup_req_t),
		      USB_SETUP_REQ_MSG,
		      SC_DEFAULT_POOL,
		      SC_FATAL_IF_TMO);
    
    memcpy((char *)&msg->setup_req.setup,
	   (char *)setup,
	   sizeof(usb_deviceRequest_t));
    
    sc_msgTx(&msg,usbd->epClient[0],0);  
  }
}

static int usb_handle_setup(usbd_t *usbd,usb_deviceRequest_t* setup)
{
  __u16 w_value,w_index,w_length;
  int size;

  w_value  = SWAP16(setup->wValue);
  w_index  = SWAP16(setup->wIndex);
  w_length = SWAP16(setup->wLength);

  setup->wValue = w_value;
  setup->wIndex = w_index;
  setup->wLength = w_length;

  reset_ep_queue(usbd,0);

  if ( !usbd->deviceDesc) {
    Ep0Stall();
    return 1;
  }

  if ((setup->bmRequestType & USB_DIR_IN)||(w_length==0))
    usbd->ep0_dir = USB_DIR_IN;
  else
    usbd->ep0_dir = USB_DIR_OUT;

  if((setup->bmRequestType&USB_TYPE_MASK)!=USB_TYPE_STANDARD){
    return 0;
  }

//->  kprintf(0,"DCD:type %02x req %02x val %04x idx %04x len %04x\n",
//->	  setup->bmRequestType,setup->bRequest,w_value,w_index,w_length);

  switch( setup->bRequest ){
  case USB_GET_DESCRIPTOR:
    if((setup->bmRequestType&USB_TYPE_MASK)!=USB_TYPE_STANDARD){
      return 0;
    }

    switch( w_value >> 8 ){
    case 1:
#ifdef DEBUG_PRINT
      kprintf(0,"DCD:Get device descriptor:%d\n",w_length);
#endif
      if ( usbd->state != USB_ADDRESSED_STATE ){
	size = 8;
      } else {
	size = usbd->deviceDescSize;
      }       
      usb_device_send_cntrl_packet(usbd,
				   0, 
				   usbd->deviceDesc,
				   size);     
      break;
    case 2:
#ifdef DEBUG_PRINT
      kprintf(0,"DCD:Get config descriptor:%d(%d)\n",
	      w_length,usbd->configDescSize);
#endif
      /* If requested length exceeds the actual, 
      ** use the actual length of descriptor 
      */
      if ( w_length > usbd->configDescSize ){
	w_length = usbd->configDescSize;
      }
      
      usb_device_send_cntrl_packet(usbd,
				   0,
				   usbd->configDesc,
				   w_length);
      break;
    case USB_DEVICE_QUALIFIER:
      /* USB 2.0 */
#ifdef DEBUG_PRINT
      kprintf(0,"DCD:Device qualifier\n");
#endif
      Ep0Stall();
      break;
    default:
      /* Application must handle this request.*/
      return 0;
    }
    break;
  case USB_SET_CONFIGURATION:
    /* Switch to configured state, actual configuration
    ** must be send by the application
    */
    usbd->state = USB_CONFIGURED_STATE;
    return 0;
    break;

  case USB_SET_ADDRESS:
#ifdef DEBUG_PRINT
    kprintf(0,"DCD:Set Addr\n");
#endif
    if(((setup->bmRequestType&USB_RECIP_MASK) != USB_RECIP_DEV)
       || (w_length!=0)
       || (w_index!=0) )
    {
      Ep0Stall();
      return 1;
    }
    /* send status */
    usb_device_send_cntrl_packet(usbd, 0, NULL, 0);  

    usbd->device_addr = w_value;
    /* Set addressed state, actual address is set _after_ status phase */
    usbd->state = USB_ADDRESSED_STATE;
    break;
  case USB_CLEAR_FEATURE:
  case USB_SET_FEATURE:    
    /* send status */
    usb_device_send_cntrl_packet(usbd, 0, NULL, 0);  
    break;
  default:
#ifdef DEBUG_PRINT
    kprintf(0,"DCD:Error %02x\n",setup->bRequest);
#endif
    Ep0Stall();
    return 0;
    break;
  }
  
  return 1;
}

/*
** Send a controll packet (normaly EP0).
** If size is 0 send a ZLP
*/
static void usb_device_send_cntrl_packet(usbd_t *usbd, 
					 __u32 epnum, 
					 const __u8* buf, 
					 __u32 size)
{
  static __u8 recv_buf[MAX_USB_DESC_SIZE];
  usb_dtd_t * usb_dtd1;
  usb_dtd_t * usb_dtd2;
  ep_queue_head_t * qh;

  qh = &usbd->ep_queue_head[epnum*2];

//->  kprintf(0,"DCD:Send control packet..\n");
  /* Initialize dTD to send device descriptor */
  if ( usbd->tx_dtd[epnum] || usbd->rx_dtd[epnum] ) for(;;);

  usbd->tx_dtd[epnum] = 
    usb_dtd1 = usb_dtd_init(size, 1, 0, (__u8 *)buf);	

  /* Point the endpoint IN QH to the dTD */
  (qh+1)->next_dtd = (__u32)usb_dtd1;
    
  /* Prime Tx buffer for control endpoint */
  MCF_USB_EPPRIME(USB_OTG) |= MCF_USB_EPPRIME_PETB(1<<epnum);
    
  /* Wait for prime to complete */
  while (MCF_USB_EPPRIME(USB_OTG) & MCF_USB_EPPRIME_PETB(1<<epnum));
  
  if ( size ){
    /* Initialize dTD for receive. The host won't actually send anything,
       but a dTD needs to be setup to correctly deal with the 0 byte OUT
       packet the host sends after receiving the IN data. */
    usbd->rx_dtd[epnum] = 
      usb_dtd2 = usb_dtd_init(0x40, 1, 0, recv_buf);	
    
    /* Point the endpoint OUT QH to the dTD */
    qh->next_dtd = (__u32)usb_dtd2;
    
    /* Prime Rx buffer for control endpoint */
    MCF_USB_EPPRIME(USB_OTG) |= MCF_USB_EPPRIME_PERB(1<<epnum);
    
    /* Wait for prime to complete */
    while (MCF_USB_EPPRIME(USB_OTG) & MCF_USB_EPPRIME_PERB(1<<epnum));
  }
}

/*
** Prepare an EP to send data using a given DTD
*/
static void usb_send_data(usbd_t *usbd,__uint ep_num,usb_dtd_t *dtd)
{
  ep_queue_head_t * qh;

  qh = &usbd->ep_queue_head[ep_num*2];
  
  if ( usbd->tx_dtd[ep_num] ){
    return;
  }
  
  usbd->tx_dtd[ep_num] = dtd;

  /* Point QH to new dTDs */
  (qh+1)->next_dtd = (__u32)dtd;
  
  /* Prime Tx buffer for EP1 */
  MCF_USB0_EPPRIME |= MCF_USB_EPPRIME_PETB(1<<ep_num);
}
/*
** Prepare an EP to receive data using a given DTD
*/
static void usb_recv_data(usbd_t *usbd,__uint ep_num,usb_dtd_t *dtd)
{
  ep_queue_head_t * qh;

  qh = &usbd->ep_queue_head[ep_num*2];
  
  if ( usbd->rx_dtd[ep_num] ){
    return;
  }
  
  usbd->rx_dtd[ep_num] = dtd;

  /* Point QH to new dTDs */
  qh->next_dtd = (__u32)dtd;
  
  /* Prime Tx buffer for EP1 */
  MCF_USB0_EPPRIME |= MCF_USB_EPPRIME_PERB(1<<ep_num);
}

/*
** Handle USB bus reset
*/
static void reset_irq(usbd_t *usbd)
{
  /* Clear the device address */
  MCF_USB0_DEVICEADDR = 0;
  usbd->device_addr = 0;

  /* Clear usb state */
  usbd->state = USB_DEFAULT_STATE;

  /* Clear all the setup token semaphores */
  MCF_USB0_EPSETUPSR = MCF_USB0_EPSETUPSR;
  /* Clear all the endpoint complete status bits */
  MCF_USB0_EPCOMPLETE = MCF_USB0_EPCOMPLETE;

  /* Flush all EPs */
  MCF_USB0_EPFLUSH = 0xFFFFFFFF;

  if (MCF_USB0_PORTSC & MCF_USB_PORTSC_PR) {
    /* Bus is reseting */
    reset_queues(usbd);
  } else {
    reset_queues(usbd);
    usb_device_init(usbd);
    MCF_USB_USBINTR(USB_OTG) = (0
				| MCF_USB_USBINTR_SLE
				| MCF_USB_USBINTR_URE
				| MCF_USB_USBINTR_UE
				| MCF_USB_USBINTR_PCE
				| 0);
  }
}


static void dtd_complete_irq(usbd_t *usbd)
{
  sc_msg_t msg;
  __u32 bit_pos;
  int i;

  /* Clear the bits in the register */
  bit_pos = MCF_USB0_EPCOMPLETE;
  MCF_USB0_EPCOMPLETE = bit_pos;

  if (!bit_pos)
    return;

//->  kprintf(0,"DCD:BM %08x\n",bit_pos);

  /* handle EP0 special */
  if ( (bit_pos & 1) && 
       usbd->rx_dtd[0] && 
       !(usbd->rx_dtd[0]->dtd_token & 0x80))
  {
    if ( usbd->epClient[0] &&
	 usbd->epClient[0] != SC_ILLEGAL_PID &&
	 sc_msgOwnerGet((sc_msgptr_t)&usbd->rx_dtd[0]->malloc_ptr) == usbd->epClient[0] )
    {
      msg = sc_msgAlloc(sizeof(usb_epComplete_t),
			USB_EP_COMPLETE_MSG(0),
			SC_DEFAULT_POOL,
			SC_FATAL_IF_TMO);

      msg->epComplete.dtd_in = NULL;
      msg->epComplete.dtd_out = usbd->rx_dtd[0];	  
      sc_msgTx(&msg,usbd->epClient[0],0);
      
    } else {
      usb_dtd_free(usbd->rx_dtd[0]);
    }
    usbd->rx_dtd[0] = NULL;	
  }
  
  if ( (bit_pos & 0x10000) && 
       usbd->tx_dtd[0] && 
       !(usbd->tx_dtd[0]->dtd_token & 0x80)) 
  {
    if ( usbd->state == USB_ADDRESSED_STATE) {
      /* Set the new address */
      MCF_USB0_DEVICEADDR=MCF_USB_DEVICEADDR_USBADR(usbd->device_addr);
    }
    
    usb_dtd_free(usbd->tx_dtd[0]);
    usbd->tx_dtd[0] = NULL;	
  }
  
  bit_pos &= 0x000e000e;
  for( i = 1; bit_pos && i < USB_MAX_EP; ++i ){
    bit_pos >>= 1; 
    bit_pos &= 0x70007;
    if ( bit_pos & 0x10001 && (usbd->tx_dtd[i] || usbd->rx_dtd[i]) ){

      msg = sc_msgAllocClr(sizeof(usb_epComplete_t),
			   USB_EP_COMPLETE_MSG(i),
			   SC_DEFAULT_POOL,
			   SC_FATAL_IF_TMO);
      
      if ( bit_pos & 0x10000 ){
	msg->epComplete.dtd_in = usbd->tx_dtd[i];
	usbd->tx_dtd[i] = NULL;
      } 
      
      if ( bit_pos & 1 ){
	msg->epComplete.dtd_out = usbd->rx_dtd[i];	  
	usbd->rx_dtd[i] = NULL;
      }
      
      sc_msgTx(&msg,usbd->epClient[i],0);
    }
  }
}


static void reset_ep_queue(usbd_t *usbd,__uint ep_num)
{
  usb_dtd_t *nextDTD;
  usb_dtd_t *dTD;
  sc_msg_t msg;

  if ( usbd->epClient[ep_num] && 
       usbd->epClient[ep_num] != SC_ILLEGAL_PID  &&
       (usbd->tx_dtd[ep_num] || usbd->rx_dtd[ep_num]) )
  {      
    msg = sc_msgAlloc(sizeof(usb_epReset_t),
		      USB_EP_RESET_MSG(ep_num),
		      SC_DEFAULT_POOL,
		      SC_FATAL_IF_TMO);
    
    msg->epReset.dtd_in = usbd->tx_dtd[ep_num];
    msg->epReset.dtd_out = usbd->rx_dtd[ep_num];
    usbd->tx_dtd[ep_num] = NULL;
    usbd->rx_dtd[ep_num] = NULL;
    
    sc_msgTx(&msg,usbd->epClient[ep_num],0);
    return;
  }
  
  if ( usbd->tx_dtd[ep_num] ){
    dTD = usbd->tx_dtd[ep_num];
    while( !((__u32)dTD & 1)){
      nextDTD = dTD->next_dtd;
      sc_msgAcquire((sc_msgptr_t)&dTD->malloc_ptr);
      sc_msgFree((sc_msgptr_t)&dTD->malloc_ptr);
      dTD = nextDTD;
    }
    usbd->tx_dtd[ep_num] = NULL;	
  }
  if ( usbd->rx_dtd[ep_num] ){
    dTD = usbd->rx_dtd[ep_num];
    while( !((__u32)dTD & 1)){
      nextDTD = dTD->next_dtd;
      sc_msgAcquire((sc_msgptr_t)&dTD->malloc_ptr);
      sc_msgFree((sc_msgptr_t)&dTD->malloc_ptr);
      dTD = nextDTD;
    }
    usbd->rx_dtd[ep_num] = NULL;	
  }
}

static void reset_queues(usbd_t *usbd)
{
  int i;
  for(i = 0; i < USB_MAX_EP; ++i ){
    reset_ep_queue(usbd,i);
  }
}


/********************************************************************/
/*
 * Return the speed of the USB port.
 *
 * Parameters:
 *  port      USB module to send reset
 */
static int 
get_port_speed(int port)
{
  int speed;
	
  /* Wait for connect */
  while(!( MCF_USB_PORTSC(USB_OTG) & MCF_USB_PORTSC_CCS));

  /* Determine the speed we are connected at. */
  speed = (MCF_USB_PORTSC(USB_OTG) & MCF_USB_PORTSC_PSPD(0x3));
#ifdef DEBUG_PRINT
  switch (speed)
  { 
  case MCF_USB_PORTSC_PSPD_FULL:
    kprintf(0,"DCD:Full-speed\n");
    break;
  case MCF_USB_PORTSC_PSPD_LOW:
    kprintf(0,"DCD:Low-speed\n");
    break;
  case MCF_USB_PORTSC_PSPD_HIGH:
    kprintf(0,"DCD:High-speed\n");
    break;
  default:
    kprintf(0,"DCD:Invalid speed\n");
    break;
  }
#endif

  return speed;    
}
/********************************************************************/
/*
 * Initialize an endpoint QH. The space for the endpoint queue heads is 
 * allocated when the endpoint list is created, so this function does not
 * call malloc. 
 *
 * Parameters:
 *  usbd	global usb device data
 *  ep_num      EndPoint
 *  dir         direction (USB_DIR_IN or USB_DIR_OUT)
 *  mult	number of packets per transcation, 
 *                should be 0 for non-ISO endpoints
 * max_packet	maximum packet length for the endpoint			
 * ios		flag to enable interrupts on incoming setup packets
 *next_dtd	pointer to the first dtd for the ep_qh
 */
static void usb_ep_qh_init(usbd_t *usbd,
			   __u32 ep_num,
			   __u32 dir,
			   __u32 mult, 
			   __u32 max_packet, 
			   __u32 ios, 
			   usb_dtd_t * next_dtd)
{

  ep_queue_head_t * qh;
  __u32 token;
	
  /* Create a pointer to the endpoint queue head being initialized */
  ep_num = ep_num * 2 + (dir ? 1 : 0 );

  qh = &usbd->ep_queue_head[ep_num];
	
  if (max_packet > MAX_QH_PACKET_SIZE ){
#ifdef DEBUG_PRINT
    kprintf(0,"DCD:ERR!! Invalid packet size.\n");
#endif
  }

  if ( !next_dtd ){
    next_dtd = (usb_dtd_t *)1;
  }

  token = 0;
  if (ios){
    token = USB_EP_QH_EP_CHAR_IOS;
  }

     			  		
  qh->ep_char = ( token
		  | USB_EP_QH_EP_CHAR_MULT(mult)
		  | USB_EP_QH_EP_CHAR_MAX_PACKET(max_packet));    
  
  qh->curr_dtd = 0;
  qh->next_dtd = (__u32)next_dtd;
  qh->dtd_token = 0;
  qh->dtd_buf0 = 0;
  qh->dtd_buf1 = 0;
  qh->dtd_buf2 = 0;
  qh->dtd_buf3 = 0;
  qh->dtd_buf4 = 0;
}
/********************************************************************/
/*
 * This function will initialize endpoint 0 so that
 * it is ready to respond to the host for enumeration. 
 * Since currently only one USB port supports device operation, the port
 * for the device init is set at the beginning of this function. If future 
 * devices implement more than one USB device port then this can be changed
 * to a passed in parameter.
 *
 * Parameters:
 *  eplistaddr      return a pointer to the device endpoint list
 */
static ep_queue_head_t *
usb_device_ep0_init(void)
{
  extern char ep_list_base[]; /* Defined in the linker script !*/

  /* Set the device endpoint list address */
  MCF_USB_EPLISTADDR(USB_OTG) = (__u32)ep_list_base;
  	
  /* Clear the entire ep list */
  memset(ep_list_base,0,USB_MAX_EP*64*2);

  /* 
  ** Configure EP0. Only the required EP0 for control traffic 
  ** is initialized at this time. 
  ** => Enable TX/RX of EP0
  */
  MCF_USB_EPCR0(USB_OTG) |= (MCF_USB_EPCR_TXE | MCF_USB_EPCR_RXE);

  return (ep_queue_head_t *)ep_list_base;

}

/********************************************************************/
/*
 * Initialize the USB device for operation. This initialization performs
 * basic configuration to prepare the device for connection to a host.
 * Since currently only one USB port supports device operation, the port
 * for the device init is set at the beginning of this function. If future 
 * devices implement more than one USB device port then this can be changed
 * to a passed in parameter.
 *
 * Parameters:
 */
static void usb_device_init(usbd_t *usbd)
{
  /* Initialize the clock divider for the USB */
  if  ((MCF_CCM_CCR & MCF_CCM_CCR_PLL_MODE) == 3 ){
    MCF_CCM_MISCCR |= MCF_CCM_MISCCR_USBDIV;
  } else {
    MCF_CCM_MISCCR &= ~MCF_CCM_MISCCR_USBDIV;
  }
  // internal CLK
  MCF_CCM_MISCCR |= MCF_CCM_MISCCR_USBSRC;

//->  MCF_CCM_MISCCR |= MCF_CCM_MISCCR_USBDIV;
		
  /* Stop and reset the usb controller */
  MCF_USB0_USBCMD &= ~MCF_USB_USBCMD_RS;
  
  MCF_USB0_USBCMD = MCF_USB_USBCMD_RST;
  
  /* Wait reset completed */
  int timeout = 500;
  while (timeout-- && (MCF_USB0_USBCMD&0x2));

  if ( !timeout ) return;
  /*
   * Set "Device mode","Big endian byte ordering" and "Setup lockouts off":
   */
  MCF_USB0_USBMODE = ( 0 
		       | MCF_USB_USBMODE_SLOM
		       | MCF_USB_USBMODE_ES
		       | MCF_USB_USBMODE_CM_DEVICE);
  
  /* Clear the setup status */
  MCF_USB0_USBSTS = 0;
  
  /*--- Intial Configuration ---*/
  // Set interrupt threshold control = 0
  MCF_USB_USBCMD(USB_OTG) &= ~( MCF_USB_USBCMD_ITC(0xFF));
      
  /* Initialize EP0 to handle enumeration. This function will also allocate
   * memory to be used for the device endpoint list. */
  usbd->ep_queue_head = usb_device_ep0_init();

  /* Initialize queue head for EP0 IN (device to host)  */
  usb_ep_qh_init(usbd, 0, EP_DIR_IN, 0, 0x40, 1, NULL); 
	
  /* Initialize queue head for EP0 OUT (host to device)  */
  usb_ep_qh_init(usbd, 0, EP_DIR_OUT, 0, 0x40, 1, NULL); 

  MCF_USB_USBCMD(USB_OTG) |= MCF_USB_USBCMD_RS;	// RUN controller

  /* Enable  a valid B session to allow device to connect to a host. */
  MCF_CCM_UOCSR = ( MCF_CCM_UOCSR_BVLD );

#ifdef DEBUG_PRINT			
  kprintf(0,"DCD:USB device initialized.\n");
#endif
}

/*
** Public function interface 
*/
void usbd_sendCtrlPacket(sc_pid_t usbd_pid,
			 __uint ep_num,
			 const __u8 *data,
			 __uint size)
{
  sc_msg_t msg;

  msg = sc_msgAlloc(sizeof(usb_ctrlPacket_t),
		    USB_CTRL_PACKET_MSG,
		    SC_DEFAULT_POOL,
		    SC_FATAL_IF_TMO);
  msg->ctrl.data = data;
  msg->ctrl.ep_num = ep_num;
  msg->ctrl.size = size;
  
  sc_msgTx(&msg,usbd_pid,0);
}

void usbd_sendStatusPacket(sc_pid_t usbd_pid,__uint ep_num)
{
  sc_msg_t msg;

  msg = sc_msgAlloc(sizeof(usb_ctrlPacket_t),
		    USB_CTRL_PACKET_MSG,
		    SC_DEFAULT_POOL,
		    SC_FATAL_IF_TMO);

  msg->ctrl.data = NULL;
  msg->ctrl.ep_num = ep_num;
  msg->ctrl.size = 0;
  
  sc_msgTx(&msg,usbd_pid,0);
}

void usb_sendData(sc_pid_t usbd_pid,__uint ep_num, usb_dtd_t *dTD)
{
  sc_msg_t msg;
  
  msg = sc_msgAlloc(sizeof(usb_sendData_t),
		    USB_SEND_DATA_MSG(ep_num),
		    SC_DEFAULT_POOL,
		    SC_FATAL_IF_TMO);
  
  msg->sendData.ep_num = ep_num;
  msg->sendData.dtd = dTD;
 
  sc_msgTx(&msg,usbd_pid,0);
}

void usb_recvData(sc_pid_t usbd_pid,__uint ep_num, usb_dtd_t *dTD)
{
  sc_msg_t msg;
  
  msg = sc_msgAlloc(sizeof(usb_recvData_t),
		    USB_RECV_DATA_MSG(ep_num),
		    SC_DEFAULT_POOL,
		    SC_FATAL_IF_TMO);
  
  msg->recvData.ep_num = ep_num;
  msg->recvData.dtd = dTD;
 
  sc_msgTx(&msg,usbd_pid,0);
}

void usbd_sendRequestError(sc_pid_t usbd_pid,__uint ep_num)
{
  MCF_USB0_EPCR(ep_num)|=MCF_USB_EPCR_TXS|MCF_USB_EPCR_RXS;
}
/********************************************************************/
/*
 * Allocate memory for a device transfer descriptor (dTD) and initialize
 * the dTD. This function assumes the dTD is the last in the list so 
 * the next dTD pointer is marked as invalid. 
 *
 * Parameters:
 * 	trans_sz		number of bytes to be transferred
 *	ioc			interrupt on complete flag
 *	pid			PID code for the transfer (
 *	buffer_ptr		pointer to the data buffer 
 */
usb_dtd_t *
usb_dtd_init(__u32 trans_sz, __u32 ioc, __u32 multo, __u8 * buffer_ptr)
{
  usb_dtd_t *usb_dtd;
  __u32 token;
  __u32 malloc_addr;
  /*
   * The USB requires dTDs to be aligned on a 32 byte boundary. 
   * In order to accomplish this, the data is over-allocated and 
   * adjusted. 
   */
  malloc_addr = (__u32)sc_msgAlloc(96,
				   USB_DTD_MSG,
				   SC_DEFAULT_POOL,
				   SC_FATAL_IF_TMO);
  
  
  usb_dtd = (usb_dtd_t *)((malloc_addr + 31) & 0xFFFFFFE0);

  usb_dtd->next_dtd = (usb_dtd_t *)(USB_DTD_MSG | 1);
		
  if (trans_sz > MAX_DTD_TRANS_SIZE){
#ifdef DEBUG_PRINT
    kprintf(0,"DCD:ERR!! Invalid transfer size.\n");
#endif
    return NULL;
  }

  token = ioc ? USB_DTD_TOKEN_IOC : 0;
  
  usb_dtd->dtd_token = (token
			| USB_DTD_TOKEN_TOTAL_BYTES(trans_sz)
			| USB_DTD_TOKEN_MULTO(multo)
			| USB_DTD_TOKEN_STAT_ACTIVE );
  							
  usb_dtd->dtd_buf0 = (__u32)buffer_ptr;
  usb_dtd->dtd_buf1 = (__u32)buffer_ptr+0x1000;
  usb_dtd->dtd_buf2 = (__u32)buffer_ptr+0x2000;
  usb_dtd->dtd_buf3 = (__u32)buffer_ptr+0x3000;
  usb_dtd->dtd_buf4 = (__u32)buffer_ptr+0x4000;
  usb_dtd->malloc_ptr = (void *)malloc_addr;

  return usb_dtd;
}

void usb_dtd_reinit(__u32 trans_sz, usb_dtd_t *dtd,__u8 *buffer)
{
  __u32 token;

  dtd->next_dtd = (usb_dtd_t *)(USB_DTD_MSG | 1);
		
  if (trans_sz > MAX_DTD_TRANS_SIZE){
#ifdef DEBUG_PRINT
    kprintf(0,"DCD:ERR!! Invalid transfer size.\n");
#endif
  }

  /* remove size */
  token = dtd->dtd_token & 0x0000ff80;

  dtd->dtd_token = (token
		    | USB_DTD_TOKEN_TOTAL_BYTES(trans_sz)
		    | USB_DTD_TOKEN_STAT_ACTIVE );
  
  dtd->dtd_buf0 = (__u32)buffer;
  dtd->dtd_buf1 = (__u32)buffer+0x1000;
  dtd->dtd_buf2 = (__u32)buffer+0x2000;
  dtd->dtd_buf3 = (__u32)buffer+0x3000;
  dtd->dtd_buf4 = (__u32)buffer+0x4000;
}

void usb_dtd_free(usb_dtd_t *dTD)
{
  usb_dtd_t *nextDTD;

  if ( !dTD ) return;

  while( !((__u32)dTD & 1)){
    nextDTD = dTD->next_dtd;
    sc_msgFree((sc_msgptr_t)&dTD->malloc_ptr);
    dTD = nextDTD;
  }
}


/*
** Helper
*/
void d(void *x)
{
  __u8 *p = (__u8 *)(((__u32)x) & ~1);
  int i;
  for(i = 0; i < 8 ; ++i ){
    kprintf(0,"%02x ",*p++);
  }
  kprintf(0,"\n");
}
