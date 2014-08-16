/****************************************************************************
 *
 *            Copyright (c) 2003-2008 by HCC Embedded
 *
 * This software is copyrighted by and is the sole property of
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1133
 * Vaci Ut 110
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/

#include <sciopta.h>
#include <sdd/sdd.msg>
#include <logd/logd.h>
#include <sys/errno.h>
#include <hcc/eusbh/usbh.msg>

#include <hcc/hcc_types.h>
#include <hcc/eusbh/usb_driver/usb_host.h>
#include <hcc/eusbh/usb_driver/usb_utils.h>
#include <hcc/eusbh/usb_driver/pmgr/pmgr.h>
#include <hcc/eusbh/cdcser/cdcser.h>

/* SCIOPTA ****/
union sc_msg {
  sdd_baseMessage_t    base;
  sdd_devWrite_t       write;
  sdd_devRead_t        read;
  hcc_usbhConnect_t    connect;
  hcc_usbhDisconnect_t disconnect;
};
/**************/

#define CDCSER_DATA_CLASS	0x0a	/* CDC serial class */
#define CDCSER_ALTCLASS		0xff	/* alternate class */
#define CDCSER_SCLASS		0x00
#define CDCSER_PROTOCOL		0x00

#define CDC_SETUP_REQUEST	0x21

#define SEND_ENCAPSULATED_COMMAND	0x00
#define GET_ENCAPSULATED_RESPONSE	0x01
#define SET_COMM_FEATURE		0x02
#define GET_COMM_FEATURE		0x03
#define CLEAR_COMM_FEATURE		0x04
#define SET_LINE_CODING			0x20
#define GET_LINE_CODING	       		0x21
#define SET_CONTROL_LINE_STATE		0x22
#define SEND_BREAK			0x23

#define EP0		0
#define IN		1
#define OUT		2


static const t_cd_ep_dsc cdcser_cd_ep_dsc[4] = {
  3,
  USBH_EP_DSC(USBH_TYPE_CONTROL,0),
  USBH_EP_DSC(USBH_TYPE_BULK,USBH_DIR_IN),
  USBH_EP_DSC(USBH_TYPE_BULK,USBH_DIR_OUT)
};
#if 0 /* moved to cdcser.h */
typedef struct {
  hcc_u8 buf[CDCSER_RXBUF_SIZE];
  hcc_u32 pos;
  hcc_u32 size;
  int rc;
} t_rxbuf;

typedef struct {
  hcc_u8 cdcser_device;

/***** usb host access *****/
  t_usbh_transfer rx_uht;
  t_usbh_transfer tx_uht;

/***** buffering *****/
  hcc_u8 rxbuf_r;
  hcc_u8 rxbuf_w;
  t_rxbuf rxbuf[CDCSER_RXBUF_COUNT];

/***** Serial settings *****/
  hcc_u32 br;
  hcc_u8 b;
  hcc_u8 p;
  hcc_u8 s;
  hcc_u8 control;

/***** Interface *****/
  void *ep_hdl[3];
  void *ifc_hdl;
  hcc_u8 ifc_ndx;

#if OS_TASK_POLL_MODE==0
/***** mutex *****/
  OS_MUTEX_TYPE *rx_mutex;
#endif
  
/***** events *****/
  sc_pid_t rx_ev;
  sc_msg_t tx_ev;
} t_cdcser;				       /* CDC serial device variables */
#endif /* moved to cdcser.h */ 
static t_cdcser cdcser[CDCSER_MAX_DEVICE];

static int cdcser_receive_init (hcc_u8 dev);


/*
** cdcser_check
**
** Check whether current interface is a CDC serial
**
** Input: dev_inf - device info
**        port_hdl - port handler
**        ifc_info - interface info
** Output: dev_hdl - device handler
** Return: USBH_SUCCESS - if found
**         USBH_ERR_NOT_AVAILABLE - not found
*/
static void **cdcser_check (void **dev_hdl, void *port_hdl, device_info_t *dev_inf, ifc_info_t *ifc_inf)
{
  (void)port_hdl;
  (void)dev_inf;
  if (  (ifc_inf->_class==CDCSER_DATA_CLASS || ifc_inf->_class==CDCSER_ALTCLASS)
      && ifc_inf->sclass==CDCSER_SCLASS && ifc_inf->protocol==CDCSER_PROTOCOL)
  {
    hcc_uint i;
    for (i=0;i<CDCSER_MAX_DEVICE && cdcser[i].cdcser_device;i++)
      ;
    if (i<CDCSER_MAX_DEVICE)
    {
      cdcser[i].ifc_ndx=ifc_inf->ndx;
      *dev_hdl=cdcser+i;
      return cdcser[i].ep_hdl;
    }
  }
  return NULL;
}



/*
** cdcser_connect
**
** Connect CDC serial device
** Callback function for port manager class driver handler.
**
** Input: dev_hdl - device handler
**        ifc_hdl - port handler
** Return: USBH_SUCCESS - if found
**         USBH_ERR_NOT_AVAILABLE - not found
*/
static int cdcser_connect (void *dev_hdl, void *ifc_hdl)
{
  int rc=USBH_ERR_NOT_AVAILABLE;
  t_cdcser *ci=(t_cdcser *)dev_hdl;
  hcc_u8 dev=(hcc_u8)(ci-cdcser);
  if (ci->ep_hdl[EP0] && ci->ep_hdl[IN] && ci->ep_hdl[OUT])
  {
    hcc_uint i;

    for (i=0;i<CDCSER_RXBUF_COUNT;i++)
    {
      cdcser[dev].rxbuf[i].pos=0;
      cdcser[dev].rxbuf[i].size=0;
      cdcser[dev].rxbuf[i].rc = 0;
    }
    cdcser[dev].rxbuf_r=0;
    cdcser[dev].rxbuf_w=0;

    ci->ifc_hdl=ifc_hdl;
    ci->cdcser_device=1;
    rc=cdcser_set_line_coding(dev,0,0,0,0);
    if (rc==USBH_SUCCESS) rc=cdcser_set_control_line_state(dev,0xff,0);
    if (rc==USBH_SUCCESS) rc=cdcser_receive_init(dev);
    if ( rc ){
      ci->cdcser_device=0;
    } else {
      sc_msg_t msg;
      msg = sc_msgAlloc(sizeof(hcc_usbhConnect_t),
			HCC_USBH_CONNECT,
			SC_DEFAULT_POOL,
			SC_FATAL_IF_TMO);
      
      msg->connect.hdl = dev_hdl;
      sc_msgTx(&msg, ci->driver, 0);
    }

  }

  return rc;
}


/*
** cdcser_disconnect
**
** Input: dev_hdl - device handler
**
** Callback function for usb host class driver handler.
*/
static int cdcser_disconnect (void *dev_hdl)
{
  t_cdcser *ci=(t_cdcser *)dev_hdl;
  ci->cdcser_device=0;
  {
    sc_msg_t msg;
    msg = sc_msgAlloc(sizeof(hcc_usbhDisconnect_t),
		      HCC_USBH_DISCONNECT,
		      SC_DEFAULT_POOL,
		      SC_FATAL_IF_TMO);
    msg->disconnect.hdl = dev_hdl;
    sc_msgTx(&msg, ci->driver, 0);
  }
  return USBH_SUCCESS;
}



/*
** cdcser_init
**
** Initialize CDC serial module
**
** Return: 0 - success / other if error
*/
int cdcser_init (void)
{
  hcc_u8 i;
  for (i=0;i<CDCSER_MAX_DEVICE;i++)
  {
    cdcser[i].cdcser_device=0;
    cdcser[i].br=0;
    cdcser[i].control=0xff;
    cdcser[i].rx_ev = SC_ILLEGAL_PID;
    cdcser[i].tx_ev=NULL;
#if OS_TASK_POLL_MODE==0    
    if (os_mutex_create(&cdcser[i].rx_mutex)) return USBH_ERR_RESOURCE;
#endif    
    cdcser[i].driver = sc_procIdGet(NULL, SC_NO_TMO);
    cdcser[i].device = i;
  }
  return pmgr_add_cd(cdcser_check,cdcser_connect,cdcser_disconnect,NULL,(t_cd_ep_dsc *)&cdcser_cd_ep_dsc);
}


/*
** cdcser_set_events
**
** Set RX/TX events for a CDC channel
**
** Input: rx_ev - receive event
**        tx_ev - transmit event
** Return: 0 - success / other if error
*/
int cdcser_set_events (hcc_u8 dev, OS_EVENT_BIT_TYPE *rx_ev, OS_EVENT_BIT_TYPE *tx_ev)
{
  sc_msg_t msg;
  if ( (int)tx_ev && !cdcser[dev].tx_ev){ 
    msg = sc_msgAllocClr(sizeof(sdd_devWrite_t),
			 SDD_DEV_WRITE_REPLY,
			 SC_DEFAULT_POOL,
			 SC_FATAL_IF_TMO);
    msg->base.handle = (void *)sc_procIdGet(NULL,SC_NO_TMO);
    cdcser[dev].tx_ev = msg;
  }    
  if ( (int)rx_ev ){ 
    cdcser[dev].rx_ev = sc_procIdGet(NULL,SC_NO_TMO);
  }    

  return 0;
}


/*
** cdcser_get_line_config
**
** Get line configuration.
** Input: device number
** Output: br - baudrate
**         b - data bits
**         p - parity
**         s - stop bits
** Return: 0 - success / other if error
*/
int cdcser_get_line_coding (hcc_u8 dev, hcc_u32 *br, hcc_u8 *b, hcc_u8 *p, hcc_u8 *s)
{
  hcc_u8 setup[8]={CDC_SETUP_REQUEST|STP_DIR_IN,GET_LINE_CODING,0,0,0,0,7,0};
  hcc_u8 lc[7];
  int rc=USBH_ERR_TRANSFER;

  if (cdcser[dev].cdcser_device)
  {
    setup[4]=cdcser[dev].ifc_ndx;
    if (cdcser[dev].ifc_ndx) --setup[4];	/* address comm interface */
    rc=usbh_receive_control_b(cdcser[dev].ep_hdl[EP0],setup,lc,7,NULL);
    if (rc==USBH_SUCCESS)
    {
      *br=RD_LE32(lc);
      *s=lc[4];
      *p=lc[5];
      *b=lc[6];
    }
  }
  return rc;
}


/*
** cdcser_set_line_config
**
** Set line configuration
**
** Input: dev - device number
**        br - baudrate
**        b - data bits
**        p - parity
**        s - stop bits
** Return: 0 - success / other if error
*/
int cdcser_set_line_coding (hcc_u8 dev, hcc_u32 br, hcc_u8 b, hcc_u8 p, hcc_u8 s)
{
  hcc_u8 setup[8]={CDC_SETUP_REQUEST|STP_DIR_OUT,SET_LINE_CODING,0,0,0,0,7,0};
  hcc_u8 lc[7];

  if (br)
  {
    cdcser[dev].br=br;
    cdcser[dev].b=b;
    cdcser[dev].p=p;
    cdcser[dev].s=s;
  }

  if (cdcser[dev].cdcser_device && cdcser[dev].br)
  {
    setup[4]=cdcser[dev].ifc_ndx;
    if (cdcser[dev].ifc_ndx) --setup[4];		/* address comm interface */
    WR_LE32(lc,cdcser[dev].br)
      ;
    lc[4]=cdcser[dev].s;
    lc[5]=cdcser[dev].p;
    lc[6]=cdcser[dev].b;
    return usbh_send_control_b(cdcser[dev].ep_hdl[EP0],setup,lc,7,NULL);
  }

  return USBH_SUCCESS;
}


/*
** cdcser_set_control_line_state
**
** Set line configuration
**
** Input: dev - device number
**        rts - RTS state
**        dtr - DTR state
** Return: 0 - success / other if error
*/
int cdcser_set_control_line_state (hcc_u8 dev, hcc_u8 rts, hcc_u8 dtr)
{
  hcc_u8 setup[8]={CDC_SETUP_REQUEST|STP_DIR_OUT,SET_CONTROL_LINE_STATE,0,0,0,0,0,0};

  if (rts!=0xff) cdcser[dev].control=(hcc_u8)((rts<<1)|dtr);
  if (cdcser[dev].cdcser_device && cdcser[dev].control!=0xff)
  {
    setup[4]=cdcser[dev].ifc_ndx;
    if (cdcser[dev].ifc_ndx) --setup[4];		/* address comm interface */
    WR_LE16(setup+2,cdcser[dev].control)
      ;
    return usbh_send_control_b(cdcser[dev].ep_hdl[EP0],setup,NULL,0,NULL);
  }
  return USBH_SUCCESS;
}


/*
** cdcser_tx_cb
**
** Callback function for transmit
*/

static void cdcser_tx_cb (void *_dev, int rc)
{
  t_cdcser *ci=(t_cdcser *)_dev;
  sc_pid_t drv = (sc_pid_t)ci->tx_ev->base.handle;

  if ( rc == USBH_SUCCESS ){
    ci->tx_ev->base.error = 0;
  } else {
    ci->tx_ev->write.size = 0;
    ci->tx_ev->base.error = EIO;
  }
  sc_msgAcquire(&ci->tx_ev);
  sc_msgTx(&ci->tx_ev,drv,0);
  ci->tx_ev = sc_msgAllocClr(sizeof(sdd_devWrite_t),
			       SDD_DEV_WRITE_REPLY,
			       SC_DEFAULT_POOL,
			       SC_FATAL_IF_TMO);
  ci->tx_ev->base.handle = (void *)drv;
}

/*
** Get send state.
** Input: dev - device number
** Returns the completion code of the last send
*/
int cdcser_get_send_state (hcc_u8 dev)
{
  return cdcser[dev].tx_uht.rc;
}



/*
** cdcser_send
**
** Send data over CDC serial channel
**
** Input: dev - device number
**        buf - buffer
**        length - length of data to send
** Return: error code
*/
int cdcser_send (hcc_u8 dev, hcc_u8 *buf, hcc_u32 length)
{
  int rc=USBH_ERR_NOT_AVAILABLE;
  if (cdcser[dev].cdcser_device)
  {
    if (cdcser[dev].tx_ev)
    {
      /* SCIOPTA {*/
      sc_msg_t msg = (sc_msg_t)cdcser[dev].tx_ev;
      msg->write.size = length;
      msg->base.handle = (void *)sc_procIdGet(NULL,SC_NO_TMO);
      /* }        */
      cdcser[dev].tx_uht.eph=cdcser[dev].ep_hdl[OUT];
      cdcser[dev].tx_uht.buf=buf;
      cdcser[dev].tx_uht.size=length;
      cdcser[dev].tx_uht.toggle=USBH_TOGGLE_PRESERVE;
      cdcser[dev].tx_uht.timeout=USBH_TIMEOUT_BULK;
      cdcser[dev].tx_uht.cb_fn=cdcser_tx_cb;
      cdcser[dev].tx_uht.cb_param=cdcser+dev;
      rc=usbh_transfer(&cdcser[dev].tx_uht,USBH_MODE_NON_BLOCK);
    }
    else
    {
      rc=usbh_send_b(cdcser[dev].ep_hdl[OUT],buf,length,NULL,USBH_TIMEOUT_BULK);
    }
  }
  return rc;
}



/*
** cdcser_rx_cb
**
** Callback function for receive
*/
static void cdcser_rx_cb (void *_dev, int rc)
{
  t_cdcser *ci=(t_cdcser *)_dev;
  int skip=0;

#if OS_TASK_POLL_MODE==0
  (void)os_mutex_get(ci->rx_mutex);
#endif  

  ci->rxbuf[ci->rxbuf_w].pos=0;
  ci->rxbuf[ci->rxbuf_w].rc=ci->rx_uht.rc;

  if (rc==USBH_SUCCESS || rc==USBH_SHORT_PACKET)
  {
    ci->rxbuf[ci->rxbuf_w].size=ci->rx_uht.csize;
    if (ci->rx_uht.csize==0) skip=1;
  }
  else
  {
    ci->rxbuf[ci->rxbuf_w].size=1;
  }

  if (skip==0)
  {
    if (++(ci->rxbuf_w)==CDCSER_RXBUF_COUNT) ci->rxbuf_w=0;

    if ( ci->rx_ev != SC_ILLEGAL_PID ){
      sc_msg_t msg;
      msg = sc_msgAlloc(sizeof(sdd_devRead_t),
			SDD_DEV_READ_REPLY,
			SC_DEFAULT_POOL,
			SC_FATAL_IF_TMO);

      msg->read.size = ci->rxbuf[ci->rxbuf_w].size;

      sc_msgTx(&msg, ci->rx_ev, 0);
    }
  }

#if OS_TASK_POLL_MODE==0
  (void)os_mutex_put(ci->rx_mutex);
#endif  
  
  (void)cdcser_receive_init((hcc_u8)(ci-cdcser));
}


/*
** cdcser_receive_init
**
** Initialize receive. Starts a non-blocking receive transfer on the CDC bulk IN channel.
**
** Input: dev - device number
** Return: 0 - success / 1 - error
*/
static int cdcser_receive_init (hcc_u8 dev)
{
  int rc=USBH_ERR_ONGOING_TRANSFER;

#if OS_TASK_POLL_MODE==0
  (void)os_mutex_get(cdcser[dev].rx_mutex);
#endif

  if ( cdcser[dev].rxbuf[cdcser[dev].rxbuf_w].rc!=USBH_ERR_ONGOING_TRANSFER
       && cdcser[dev].rxbuf[cdcser[dev].rxbuf_w].pos==cdcser[dev].rxbuf[cdcser[dev].rxbuf_w].size )
  {
    cdcser[dev].rx_uht.eph=cdcser[dev].ep_hdl[IN];
    cdcser[dev].rx_uht.buf=cdcser[dev].rxbuf[cdcser[dev].rxbuf_w].buf;
    cdcser[dev].rx_uht.size=CDCSER_RXBUF_SIZE;
    cdcser[dev].rx_uht.toggle=USBH_TOGGLE_PRESERVE;
    cdcser[dev].rx_uht.timeout=USBH_WAIT_FOREVER;

//->    if (cdcser[dev].rx_ev)
    {
      cdcser[dev].rx_uht.cb_fn=cdcser_rx_cb;
      cdcser[dev].rx_uht.cb_param=cdcser+dev;
    }
//->    else
//->    {
//->      cdcser[dev].rx_uht.cb_fn=(void *)0;
//->    }
    rc=usbh_transfer(&cdcser[dev].rx_uht,USBH_MODE_NON_BLOCK);
    if (rc==USBH_SUCCESS) 
    {
      cdcser[dev].rxbuf[cdcser[dev].rxbuf_w].rc=USBH_ERR_ONGOING_TRANSFER;
    }
    else 
    {
      cdcser[dev].rxbuf[cdcser[dev].rxbuf_w].rc=rc;
    }
  }
  
#if OS_TASK_POLL_MODE==0
  (void)os_mutex_put(cdcser[dev].rx_mutex);
#endif

  return rc;
}


/*
** Get receive state.
** Input:  dev - device number
** Returns the completion code of the last receive.
*/
int cdcser_get_receive_state (hcc_u8 dev)
{
  int rc=cdcser[dev].rxbuf[cdcser[dev].rxbuf_r].rc;
  if (rc==USBH_SHORT_PACKET) rc=USBH_SUCCESS;
  if (cdcser[dev].rx_ev)
  {
    if (rc!=USBH_ERR_ONGOING_TRANSFER && rc!=USBH_SUCCESS)
    {
      cdcser[dev].rxbuf[cdcser[dev].rxbuf_r].pos=0;
      cdcser[dev].rxbuf[cdcser[dev].rxbuf_r].size=0;
      if (cdcser[dev].rxbuf_r==cdcser[dev].rxbuf_w) (void)cdcser_receive_init(dev);
      if (++cdcser[dev].rxbuf_r==CDCSER_RXBUF_COUNT) cdcser[dev].rxbuf_r=0;
    }
  }
  return rc;
}


/*
** cdcser_rx_chars
**
** Get the number of characters in the receive buffer
**
** Input:  dev - device number
**
** Return: number of characters in the receive buffer
*/
hcc_u32 cdcser_rx_chars (hcc_u8 dev)
{
  if (cdcser[dev].cdcser_device)
  {
    return cdcser[dev].rxbuf[cdcser[dev].rxbuf_r].size-cdcser[dev].rxbuf[cdcser[dev].rxbuf_r].pos;
  }
  return 0;
}


/*
** cdcser_receive
**
** Receive data on the CDC serial channel
**
** Input:  dev - device number
**         max_length - maximum length of receive buffer
** Output: buf - buffer to put incoming data
**         rlength - receive length
** Return: error code
*/
int cdcser_receive (hcc_u8 dev, hcc_u8 *buf, hcc_u32 max_length, hcc_u32 *rlength)
{
  int orc;
  int rc=USBH_SUCCESS;

  *rlength=0;
  if (cdcser[dev].cdcser_device)
  {
    if (cdcser[dev].rx_ev==SC_ILLEGAL_PID)		/* only enter this section if not in RTOS mode */
    {
      if (cdcser[dev].rxbuf[cdcser[dev].rxbuf_w].rc==USBH_ERR_ONGOING_TRANSFER)
      {
        orc=usbh_transfer_status(&cdcser[dev].rx_uht,USBH_MODE_NON_BLOCK);
        if (orc==USBH_SUCCESS || orc==USBH_SHORT_PACKET)
        {
          if (cdcser[dev].rx_uht.csize)
          {
            cdcser[dev].rxbuf[cdcser[dev].rxbuf_w].pos=0;
            cdcser[dev].rxbuf[cdcser[dev].rxbuf_w].size=cdcser[dev].rx_uht.csize;
            cdcser[dev].rxbuf[cdcser[dev].rxbuf_w].rc=orc;
            if (++cdcser[dev].rxbuf_w==CDCSER_RXBUF_COUNT) cdcser[dev].rxbuf_w=0;
          }
          (void)cdcser_receive_init(dev);
        }
        else if (orc!=USBH_ERR_NOT_COMPLETE)
        {
          rc=orc;
          (void)cdcser_receive_init(dev);
        }
      }
    }

    if (cdcser[dev].rxbuf[cdcser[dev].rxbuf_r].pos!=cdcser[dev].rxbuf[cdcser[dev].rxbuf_r].size)
    {
      hcc_u32 l;

      if (cdcser[dev].rx_ev==SC_ILLEGAL_PID)
      {
        rc=cdcser_get_receive_state(dev);
      }

      if (rc==USBH_SUCCESS)
      {
        l=cdcser[dev].rxbuf[cdcser[dev].rxbuf_r].size-cdcser[dev].rxbuf[cdcser[dev].rxbuf_r].pos;
        if (l>max_length) l=max_length;
        *rlength=l;

        _memcpy(buf,cdcser[dev].rxbuf[cdcser[dev].rxbuf_r].buf+cdcser[dev].rxbuf[cdcser[dev].rxbuf_r].pos,l);
        cdcser[dev].rxbuf[cdcser[dev].rxbuf_r].pos+=l;

        if (cdcser[dev].rxbuf[cdcser[dev].rxbuf_r].pos==cdcser[dev].rxbuf[cdcser[dev].rxbuf_r].size)
        {
          if (cdcser[dev].rxbuf_r==cdcser[dev].rxbuf_w) (void)cdcser_receive_init(dev);
          if (++cdcser[dev].rxbuf_r==CDCSER_RXBUF_COUNT) cdcser[dev].rxbuf_r=0;
        }
      }
    }
  }

  return rc;
}


/*
** cdcser_present
** Input:  dev - device number
*/
hcc_u8 cdcser_present (hcc_u8 dev)
{
  return cdcser[dev].cdcser_device;
}


/*
** Suspend CDC serial device
** Input:  dev - device number
*/
int cdcser_suspend (hcc_u8 dev)
{
  return pmgr_suspend(cdcser[dev].ifc_hdl);
}


/*
** Resume CDC serial device
** Input:  dev - device number
*/
int cdcser_resume (hcc_u8 dev)
{
  return pmgr_resume(cdcser[dev].ifc_hdl);
}

