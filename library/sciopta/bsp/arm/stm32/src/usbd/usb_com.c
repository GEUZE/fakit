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
** ID: S07051BS1                                                    **
** +Revision: 1.1.2.1 +                                                 **
** +Date: 2009/08/20 09:09:02 +                                     **
** USB CDC UART driver  
**********************************************************************
EOC*/
#include <sciopta.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#include <dev/usb.h>
#include <ossys/swap.h>

#include "usbd.msg"

#define DEBUG_PRINT

typedef struct hook_s {
  int enumCompleted;
  int lineState;
  int opened;

  sc_msg_t txMsg;
  int txSize;
  const __u8 *txPtr;
  usb_dtd_t *txDtd;

  sc_msg_t rxMsg;
  int rxSize;
  __u8 *rxPtr;
  usb_dtd_t *rxDtd;
} hook_t;
  
union sc_msg {
  sc_msgid_t id;
  usb_setup_req_t  setup_req;
  usb_setDesc_t    setDesc;
  usb_epConfig_t   epConfig;
  usb_epComplete_t epComplete;
  usb_epReset_t    epReset;
  usb_suspend_t    suspend;
  /* SDD */
  sdd_baseMessage_t base;
  sdd_devRead_t     read;
  sdd_devWrite_t    write;
  sdd_devOpen_t     open;
};

/********************************************************************/

#define EP_BULK_OUT 1
#define EP_BULK_IN  2
#define EP_INT_IN   3

#define RXBUF_SIZE 64
#define TXBUF_SIZE 64

#include "desc_com.h"


static __u8 linecoding[7] = {
  0xb0,4,0,0,        /* baud rate (Little endian !!)*/
  0,                 /* stop bits-1*/
  0,                 /* parity - none*/
  8		     /* no. of bits 8*/
};

SC_PROCESS(SCP_com)
{
  static const sc_msgid_t sel[] = {
    USB_SETUP_REQ_MSG,
    USB_SUSPEND_MSG,
    USB_EP_RESET_MSG(1),
    USB_EP_RESET_MSG(2),
    USB_EP_COMPLETE_MSG(1),
    USB_EP_COMPLETE_MSG(2),
    SDD_DEV_READ,
    SDD_DEV_WRITE,
    SDD_DEV_OPEN,
    SDD_DEV_CLOSE
  };

  static const sc_msgid_t selEP0[2] = {
    USB_EP_COMPLETE_MSG(0),
    0
  };

  __uint baud;
  sc_pid_t usbd_pid;
  __uint ep_num;
  sc_msg_t msg;
  int size;
  usb_dtd_t *dtd;

  hook_t hook = {0};

  usbd_pid = sc_procIdGet("SCI_usbd",SC_ENDLESS_TMO);

  /*
  ** Send DCD the basic configuration and
  ** register also as receiver for EP0 setup packets.
  */
  msg = sc_msgAlloc(sizeof(usb_setDesc_t),
		    USB_SET_DEVICE_DESC_MSG,
		    SC_DEFAULT_POOL,
		    SC_FATAL_IF_TMO);

  msg->setDesc.desc = (__u8 *)&commDeviceDesc;
  msg->setDesc.size = sizeof(commDeviceDesc);
  sc_msgTx(&msg,usbd_pid,0);

  /*
  ** Configuration Descriptor for the device is also handled
  ** by the DCD.
  */
  msg = sc_msgAlloc(sizeof(usb_setDesc_t),
		    USB_SET_CONFIG_DESC_MSG,
		    SC_DEFAULT_POOL,
		    SC_FATAL_IF_TMO);

  msg->setDesc.desc = commConfigDesc;
  msg->setDesc.size = sizeof(commConfigDesc);
  sc_msgTx(&msg,usbd_pid,0);

  for(;;){
    msg = sc_msgRx(SC_ENDLESS_TMO,sel,SC_MSGRX_MSGID);

    switch( msg->id ){
    case USB_SUSPEND_MSG:
//->      kprintf(0,"Suspended\n");
      ep_num = msg->suspend.ep_num;
      sc_msgFree(&msg);
      msg = sc_msgAlloc(sizeof(usb_epConfig_t),
			USB_EP_CONFIG_MSG,
			SC_DEFAULT_POOL,
			SC_FATAL_IF_TMO);
      
      msg->epConfig.ep_num = ep_num;
      msg->epConfig.enabled = 0;
      sc_msgTx(&msg,usbd_pid,0);
      break;

    case USB_EP_RESET_MSG(EP_BULK_IN):
    case USB_EP_RESET_MSG(EP_BULK_OUT):
//->      kprintf(0,"EP%d reset\n",msg->id & 0xf);
      if ( (msg->id & 15) == 1 ){
	hook.rxDtd = NULL;
      } else if ( (msg->id & 15) == 2 ){
	hook.txDtd = NULL;
      }

      if ( msg->epReset.dtd_in ){
	usb_dtd_free(msg->epReset.dtd_in);
      }
      if ( msg->epReset.dtd_out ){
	usb_dtd_free(msg->epReset.dtd_out);
      }
      sc_msgFree(&msg);
      
      if ( hook.rxMsg ){
	hook.rxMsg->base.error = -1;
	hook.rxMsg->read.size = hook.rxPtr - hook.rxMsg->read.outlineBuf;
	sc_msgTx(&hook.rxMsg,sc_msgSndGet(&hook.rxMsg),0);
      }
      if ( hook.txMsg ){
	hook.txMsg->base.error = -1;
	hook.txMsg->write.size = hook.txPtr - hook.txMsg->write.outlineBuf;
	sc_msgTx(&hook.txMsg,sc_msgSndGet(&hook.txMsg),0);
      }

      hook.enumCompleted = 0;
      hook.lineState = 0;
      hook.opened = 0;
      break;
    case USB_EP_COMPLETE_MSG(EP_BULK_OUT):
//->      kprintf(0,"EP%d complete\n",msg->id & 0xf);
      if ( (dtd = msg->epComplete.dtd_out) ){
	size = RXBUF_SIZE;
	if ( hook.rxSize < size ){
	  size = hook.rxSize;
	}
	size -= (dtd->dtd_token>>16);
//->	kprintf(0,"Got %d %08x\n",size,dtd->dtd_token);

	hook.rxSize -= size;
	hook.rxPtr += size;

	if ( hook.rxSize <= 0 ||
	     size < RXBUF_SIZE )
	{
	  if ( hook.rxMsg ){
	    hook.rxMsg->read.size = hook.rxPtr - hook.rxMsg->read.outlineBuf;
	    sc_msgTx(&hook.rxMsg,sc_msgSndGet(&hook.rxMsg),0);
	  }
	  break;
	} 
	  
	size = hook.rxSize;
	if ( size > RXBUF_SIZE ){
	  size = RXBUF_SIZE;
	}
	usb_dtd_reinit(size,hook.rxDtd,hook.rxPtr);
	
	usb_recvData(usbd_pid,1,hook.rxDtd);
      }
      break;
    case USB_EP_COMPLETE_MSG(EP_BULK_IN):
//->      kprintf(0,"EP%d complete\n",msg->id & 0xf);
//->      kprintf(0,"-> %08x\n",msg->epComplete.dtd_in->dtd_token);
      if ( msg->epComplete.dtd_in ){
	if ( hook.txSize ){	  	  
	  size = hook.txSize;
	  if ( size > TXBUF_SIZE ){
	    size = TXBUF_SIZE;
	  }
	  usb_dtd_reinit(size,hook.txDtd, (__u8 *)hook.txPtr);
	  hook.txSize -= size;
	  hook.txPtr += size;
	  /* Send the data */    
	  usb_sendData(usbd_pid,2,hook.txDtd);
	} else {
	  if ( hook.txMsg ){
	    hook.txMsg->base.error = 0;
	    hook.txMsg->write.size = hook.txPtr - hook.txMsg->write.outlineBuf;
	    sc_msgTx(&hook.txMsg,sc_msgSndGet(&hook.txMsg),0);
	  }
	}
      }
      break;

    case USB_SETUP_REQ_MSG:
      /*
      ** Handle class setup requests
      */
//->	  kprintf(0,"type %02x req %02x val %04x idx %04x len %04x\n",
//->		  msg->setup_req.setup.bmRequestType,
//->		  msg->setup_req.setup.bRequest,
//->		  msg->setup_req.setup.wValue,
//->		  msg->setup_req.setup.wIndex,
//->		  msg->setup_req.setup.wLength);

      switch( msg->setup_req.setup.bRequest ){
      case USB_SET_CONFIGURATION:
//->	kprintf(0,"Set configuration:%d\n",msg->setup_req.setup.wValue);
	usbd_sendStatusPacket(usbd_pid,0);
	sc_msgFree(&msg);

	msg = sc_msgAlloc(sizeof(usb_epConfig_t),
			  USB_EP_CONFIG_MSG,
			  SC_DEFAULT_POOL,
			  SC_FATAL_IF_TMO);
	
	msg->epConfig.ep_num = EP_INT_IN;
	msg->epConfig.enabled = 1;
	msg->epConfig.in_mode = USB_ENDPOINT_INT;
	msg->epConfig.in_size = 8;
	msg->epConfig.out_mode = 0;
		
	sc_msgTx(&msg,usbd_pid,0);
	

	msg = sc_msgAlloc(sizeof(usb_epConfig_t),
			  USB_EP_CONFIG_MSG,
			  SC_DEFAULT_POOL,
			  SC_FATAL_IF_TMO);
	
	msg->epConfig.ep_num = EP_BULK_OUT;
	msg->epConfig.enabled = 1;
	msg->epConfig.in_mode = 0;
	msg->epConfig.out_mode = USB_ENDPOINT_BULK;
	msg->epConfig.out_size = RXBUF_SIZE;
	
	sc_msgTx(&msg,usbd_pid,0);
	
	msg = sc_msgAlloc(sizeof(usb_epConfig_t),
			  USB_EP_CONFIG_MSG,
			  SC_DEFAULT_POOL,
			  SC_FATAL_IF_TMO);
	
	msg->epConfig.ep_num = EP_BULK_IN;
	msg->epConfig.enabled = 1;
	msg->epConfig.in_mode = USB_ENDPOINT_BULK;
	msg->epConfig.in_size = TXBUF_SIZE;
	msg->epConfig.out_mode = 0;
	       
	sc_msgTx(&msg,usbd_pid,0);

	hook.enumCompleted = 1;
#ifdef DEBUG_PRINT			
	kprintf(0,"Enumeration completed\n");
#endif
	break;
      case USB_GET_DESCRIPTOR:
#ifdef DEBUG_PRINT			
	kprintf(0,"Get descriptor: %02x\n",
		(msg->setup_req.setup.wValue >> 8));
	
	kprintf(0,"type %02x req %02x val %04x idx %04x len %04x\n",
		msg->setup_req.setup.bmRequestType,
		msg->setup_req.setup.bRequest,
		msg->setup_req.setup.wValue,
		msg->setup_req.setup.wIndex,
		msg->setup_req.setup.wLength);
#endif	
	usbd_sendRequestError(usbd_pid,0);
	break;
/******************************************************/
/* CDC specific                                       */
/******************************************************/
      case 0x20: /* Set line coding */
	sc_msgFree(&msg);
	dtd = usb_dtd_init(0x40, 1, 0, linecoding);	
	usb_recvData(usbd_pid,0,dtd);
	msg = sc_msgRx(SC_ENDLESS_TMO,selEP0,SC_MSGRX_MSGID);

	baud = (linecoding[0] |
		(linecoding[1]<<8) |
		(linecoding[2]<<16) |
		(linecoding[3]<<24));
	
//->	kprintf(0,"Set line coding: %d baud\n",baud);
	
	usbd_sendStatusPacket(usbd_pid,0);
	if ( msg->epComplete.dtd_out ){
	  usb_dtd_free(msg->epComplete.dtd_out);
	}
      break;
      case 0x21: /* get line coding */
//->	kprintf(0,"Get line coding\n");
	usbd_sendCtrlPacket(usbd_pid,
			    0, 
			    (const __u8 *)&linecoding,
			    sizeof(linecoding));

	break;
      case 0x22: /* set control line state */
	hook.lineState = msg->setup_req.setup.wValue;
	kprintf(0,"Set line state: %d\n",hook.lineState);
	usbd_sendStatusPacket(usbd_pid,0);
	if ( hook.lineState < 3 ){
	  if ( hook.rxMsg ){
	    hook.rxMsg->read.size = hook.rxPtr - hook.rxMsg->read.outlineBuf;
	    sc_msgTx(&hook.rxMsg,sc_msgSndGet(&hook.rxMsg),0);
	  }
	  if ( hook.txMsg ){
	    hook.txMsg->write.size = hook.txPtr - hook.txMsg->write.outlineBuf;
	    sc_msgTx(&hook.txMsg,sc_msgSndGet(&hook.txMsg),0);
	  }
	} 
	break;
      case 0x23: /* set control line state */
	kprintf(0,"send break %s\n", msg->setup_req.setup.wValue?"on" : "off");
	usbd_sendStatusPacket(usbd_pid,0);
	break;
      default:
#ifdef DEBUG_PRINT			
	kprintf(0,"type %02x req %02x val %04x idx %04x len %04x\n",
		msg->setup_req.setup.bmRequestType,
		msg->setup_req.setup.bRequest,
		msg->setup_req.setup.wValue,
		msg->setup_req.setup.wIndex,
		msg->setup_req.setup.wLength);
#endif
	usbd_sendRequestError(usbd_pid,0);
      } /* bRequest */
      break;
/******************************************************/
/* SDD messages                                       */
/******************************************************/
    case SDD_DEV_OPEN:
      msg->id = SDD_DEV_OPEN_REPLY;
      msg->base.error = 0;
      if ( !hook.enumCompleted || hook.opened){
	msg->base.error = -1;
      } else {
	hook.opened = 1;
      }
      sc_msgTx(&msg, sc_msgSndGet(&msg), 0);
      break;
      
    case SDD_DEV_CLOSE:
      msg->id = SDD_DEV_CLOSE;
      if ( hook.opened ){
	hook.opened = 0;
	msg->base.error = 0;
      } else {
	msg->base.error = -1;
      }
      sc_msgTx(&msg, sc_msgSndGet(&msg), 0);
      break;

    case SDD_DEV_READ:
      msg->id = SDD_DEV_READ_REPLY;
      msg->base.error = 0;
      if ( !hook.enumCompleted || !hook.opened){
	msg->base.error = -1;
	sc_msgTx(&msg, sc_msgSndGet(&msg), 0);
      } 
      if ( msg->read.size == 0 ){
	sc_msgTx(&msg, sc_msgSndGet(&msg), 0);
      } else {
	if ( !msg->read.outlineBuf ){
	  msg->read.outlineBuf = msg->read.inlineBuf;
	}


	hook.rxPtr = msg->read.outlineBuf;
	hook.rxSize = msg->read.size;

	size = hook.rxSize;
	if ( size > RXBUF_SIZE ){
	  size = RXBUF_SIZE;
	}
	
	if ( hook.rxDtd ){
	  usb_dtd_reinit(size,hook.rxDtd,hook.rxPtr);
	} else {
	  hook.rxDtd = usb_dtd_init(size,1, 0, hook.rxPtr);
	}	
	usb_recvData(usbd_pid,1,hook.rxDtd);
      }
      hook.rxMsg = msg;      
      msg = NULL;
      break;

    case SDD_DEV_WRITE:
      msg->id = SDD_DEV_WRITE_REPLY;
      msg->base.error = 0;
      if ( !hook.enumCompleted || !hook.opened){
	msg->base.error = -1;
	sc_msgTx(&msg, sc_msgSndGet(&msg), 0);
      } 
      if ( msg->write.size == 0 ){
	sc_msgTx(&msg, sc_msgSndGet(&msg), 0);
      } else {
	if ( !msg->write.outlineBuf ){
	  msg->write.outlineBuf = msg->write.inlineBuf;
	}

	hook.txPtr = msg->write.outlineBuf;
	hook.txSize = msg->write.size;

	size = hook.txSize;
	if ( size > TXBUF_SIZE ){
	  size = TXBUF_SIZE;
	}
	
	if ( hook.txDtd ){
	  usb_dtd_reinit(size,hook.txDtd,(__u8 *)hook.txPtr);
	} else {
	  hook.txDtd = usb_dtd_init(size,1, 0, (__u8 *)hook.txPtr);
	}	
	usb_sendData(usbd_pid,2,hook.txDtd);
	hook.txSize -= size;
	hook.txPtr += size;

      }
      hook.txMsg = msg;      
      msg = NULL;
      break;

/******************************************************/
    default:
      kprintf(0,"Unknown msg: %08x from %08x\n",msg->id,sc_msgSndGet(&msg));
    } /* msg->id */

    if ( msg ) {
      sc_msgFree(&msg);
    }
  }
}

