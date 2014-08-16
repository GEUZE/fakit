/****************************************************************************
 *
 *            Copyright (c) 2003-2009 by HCC Embedded
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
#include <hcc/eusbh/midi/midi.h>


/* SCIOPTA ****/
union sc_msg {
  midi_event_msg_t     midi_event;
  hcc_usbhConnect_t    connect;
  hcc_usbhDisconnect_t disconnect;
};
/**************/

#define MIDI_MAX_JACKS_PER_EP		16
#define MIDI_MAX_INPUT_PINS		16


/*************************************************************/
/******************* MIDI SPECIFIC VALUES ********************/
/*************************************************************/
#define AUDIO_CLASS			1
#define AUDIO_SCLASS_MIDI		3	/* MIDI subclass */
#define AUDIO_PROTOCOL			0

/* Audio specific interface descriptor type and subtypes */
#define CS_INTERFACE	0x24
#define MS_HEADER				0x01
#define MIDI_IN_JACK				0x02
#define MIDI_OUT_JACK				0x03
#define ELEMENT				0x04

#define JACK_TYPE_EMBEDDED		0x01
#define JACK_TYPE_EXTERNAL		0x02

#define CS_ENDPOINT	0x25
#define MS_GENERAL	0x01


/***********************************************************************************/

/* MIDI IN Jack descriptor */
typedef struct {
  hcc_u8 id;		/* ID */
  hcc_u8 type;		/* embedded / external */
} t_midi_in_jack;

/* MIDI OUT Jack descriptor */
typedef struct {
  hcc_u8 src_id;	/* input pin source ID */
  hcc_u8 src_pin;	/* source pin */
} t_input_pin;

typedef struct {
  hcc_u8 id;		/* ID */
  hcc_u8 type;		/* embedded / external */
  hcc_u8 input_pins;    /* no. input pins */
  t_input_pin input_pin[MIDI_MAX_INPUT_PINS];
} t_midi_out_jack;


#define EP0		0
#define IN		1
#define OUT		2
static const t_cd_ep_dsc midi_cd_ep_dsc[4] = {
  3,
  USBH_EP_DSC(USBH_TYPE_CONTROL,0),			/* control endpoint */
  USBH_EP_DSC(USBH_TYPE_BULK,USBH_DIR_IN),	/* IN */
  USBH_EP_DSC(USBH_TYPE_BULK,USBH_DIR_OUT)	/* OUT */
};


/* MIDI event buffer */
typedef struct {
  hcc_uint r,w,f;				/* read,write,full */
  t_midi_event event[MIDI_EVENT_BUFFER_SIZE];	/* events */
  hcc_u16 ep_size;				/* EP size */
  hcc_u8 ep_buf[64];				/* EP buffer */
  t_usbh_transfer uht;				/* transfer */
} t_midi_event_buf;

/* MIDI device */
typedef struct {
  void *ep_hdl[3];				/* EP handles */
  hcc_u8 in_cn_cnt;				/* no. IN cables */
  hcc_u8 out_cn_cnt;				/* no. OUT cables */
  t_midi_in_jack in_jack[MIDI_MAX_JACKS_PER_EP];/* IN jack descriptors */
  t_midi_out_jack out_jack[MIDI_MAX_JACKS_PER_EP];/* OUT jack descriptors */
  t_midi_event_buf out_ev_buf;		       	/* OUT event buffer */
  t_midi_event_buf in_ev_buf;			/* IN event buffer */
} t_midi;


/****************************************************************************/

static t_midi midi[MIDI_MAX_DEVICE];		/* MIDI devices */
static OS_MUTEX_TYPE *midi_mutex;		/* MIDI mutex */
/** SCIOPTA **/
static sc_pid_t driver;

/***********************************************************************************/

static int midi_start_receive (t_midi *md);

/***********************************************************************************/


/*
** _midi_check
**
** Check whether current interface is MIDI
**
** Input: dev_inf - device info
**        port_hdl - port handler
**        ifc_info - interface info
** Output: dev_hdl - device handler
** Return: pointer to EP list / NULL if not required
*/
static void **_midi_check (void **dev_hdl, void *port_hdl, device_info_t *dev_inf, ifc_info_t *ifc_inf)
{
  (void)dev_inf;
  (void)port_hdl;
  if ( ifc_inf->_class==AUDIO_CLASS && ifc_inf->sclass==AUDIO_SCLASS_MIDI && ifc_inf->protocol==AUDIO_PROTOCOL )
  {								/* is this a MIDI interface? */
    hcc_uint i;
    for (i=0;i<MIDI_MAX_DEVICE && midi[i].ep_hdl[EP0];i++)
      ;							/* look for an unused descriptor */
    if (i<MIDI_MAX_DEVICE)			/* if found */
    {
      *dev_hdl=midi+i;			/* set device handler */
      return midi[i].ep_hdl;	/* and return EP handle structure */
    }
  }
  return NULL;
}


/*
** _midi_connect
**
** Connect midi device.
** Callback function for port manager class driver handler.
**
** Input: dev_hdl - device handler
**        ifc_hdl - interface handler
** Return: USBH_SUCCESS - if found
**         USBH_ERR_NOT_AVAILABLE - not found
*/
static int _midi_connect (void *dev_hdl, void *ifc_hdl)
{
  t_midi *md=(t_midi *)dev_hdl;
  int rc;
  
  (void)ifc_hdl;
  if (md->ep_hdl[EP0] && (md->ep_hdl[IN] || md->ep_hdl[OUT]))
  {
    if (md->ep_hdl[OUT])
    {						/* get the endpoint size of the OUT endpoint */
      rc=usbh_get_ep_inf(md->ep_hdl[OUT],NULL,NULL,NULL,NULL,&md->out_ev_buf.ep_size);
      if (rc) return rc;
    }

    if (md->ep_hdl[IN])
    {						/* get the endpoint size of the IN endpoint */
      rc=usbh_get_ep_inf(md->ep_hdl[IN],NULL,NULL,NULL,NULL,&md->in_ev_buf.ep_size);
      if (rc) return rc;

      rc=midi_start_receive(md);
      if (rc) return rc;	/* start reception on the IN channel */
    }

    {
      sc_msg_t msg;
      msg = sc_msgAlloc(sizeof(hcc_usbhConnect_t),
			HCC_USBH_CONNECT,
			SC_DEFAULT_POOL,
			SC_FATAL_IF_TMO);
      
      msg->connect.hdl = (void *)(md-midi);
      sc_msgTx(&msg, driver, 0);
    }
    return USBH_SUCCESS;
  }
  return USBH_ERR_NOT_AVAILABLE;
}


/*
** _midi_disconnect
**
** Input: dev_hdl - device handler
**
** Callback function for usb host class driver handler.
*/
static int _midi_disconnect (void *dev_hdl)
{
  t_midi *md=(t_midi *)dev_hdl;
  {
    sc_msg_t msg;
    msg = sc_msgAlloc(sizeof(hcc_usbhDisconnect_t),
		      HCC_USBH_DISCONNECT,
		      SC_DEFAULT_POOL,
		      SC_FATAL_IF_TMO);
    msg->disconnect.hdl = (void *)(md-midi);
    sc_msgTx(&msg, driver, 0);
  }
  _memset(md,0,sizeof(t_midi));	/* reset all variables */
  return USBH_SUCCESS;
}



/*
** _midi_descriptor
**
** Try to precess unknown descriptors.
**
** Input: dev_hdl - device handler
**        dsc - descriptor
*/
static int _midi_descriptor (void *dev_hdl, hcc_u8 *dsc)
{
  t_midi *md=(t_midi *)dev_hdl;
  hcc_uint i;

  if (dsc[1]==CS_INTERFACE)
  {
    switch (dsc[2])
    {
      case MIDI_IN_JACK:
        for (i=0;i<MIDI_MAX_JACKS_PER_EP && md->in_jack[i].id;i++)
          ;
        if (i<MIDI_MAX_JACKS_PER_EP)
        {
          md->in_jack[i].id=dsc[4];
          md->in_jack[i].type=dsc[3];
        }
        break;

      case MIDI_OUT_JACK:
        for (i=0;i<MIDI_MAX_JACKS_PER_EP && md->out_jack[i].id;i++)
          ;
        if (i<MIDI_MAX_JACKS_PER_EP)
        {
          hcc_uint j;
        
          md->out_jack[i].id=dsc[4];
          md->out_jack[i].type=dsc[3];
          md->out_jack[i].input_pins=(hcc_u8)(dsc[5]>MIDI_MAX_INPUT_PINS?MIDI_MAX_INPUT_PINS:dsc[5]);
          for (j=0;j<md->out_jack[i].input_pins;j++)
          {
            md->out_jack[i].input_pin[j].src_id=dsc[6+2*j];
            md->out_jack[i].input_pin[j].src_pin=dsc[6+2*j+1];
          }
        }
        break;

      case ELEMENT:
        break;

      default:
        break;
    }
  }
  else if (dsc[1]==CS_ENDPOINT)
  {
    if (dsc[2]==MS_GENERAL)
    {
      for (i=0;i<MIDI_MAX_JACKS_PER_EP && md->in_jack[i].id!=dsc[4];i++)
        ;		/* check if the 1st source ID belongs to an IN jack */
      if (i<MIDI_MAX_JACKS_PER_EP)
      {			/* external OUT jack (embedded IN jack belongs to an external OUT jack) */
        if (md->out_cn_cnt==0) md->out_cn_cnt=dsc[3];
      }
      else
      {			/* external IN jack */
        if (md->in_cn_cnt==0) md->in_cn_cnt=dsc[3];
      }
    }
  }

  return USBH_SUCCESS;
}



/*
** midi_receive_cb
**
** Callback function for receive.
**
** Input: param - parameter
**        rc - return code of the transfer
*/
static void midi_receive_cb (void *param, int rc)
{
  t_midi *md=(t_midi *)param;
  
  if (rc==USBH_SUCCESS || rc==USBH_SHORT_PACKET)    /* if data is available */
  {
    if (md->in_ev_buf.uht.csize)
    {
      hcc_uint i;
      t_midi_event_buf *ev_buf=&md->in_ev_buf;
      t_midi_event *ev;
      hcc_u8 *bptr=ev_buf->ep_buf;
      sc_msg_t msg;
      int n = ((ev_buf->uht.csize)>>2);
      int cin;
      
      msg = sc_msgAlloc(sizeof(midi_event_msg_t)+
			n*sizeof(t_midi_event),
			HCC_USBH_MIDI_EVENT,
			SC_DEFAULT_POOL,
			SC_FATAL_IF_TMO);
      
      msg->midi_event.device = (md-midi);
      msg->midi_event.time = sc_tickGet();
      ev = &msg->midi_event.event[0];

      msg->midi_event.n = 0;
      for (i = 0; i < n; i++){	/* no. events = receive size/4 */
	cin = (hcc_u8)(*bptr&0xf);	/* save CIN */
	if ( cin > 1 ){
	  ev->cn = (hcc_u8)(*bptr>>4);		/* save CN */
	  ++bptr;
	  ev->cin = cin; /* save CIN */
	  ev->event[0] = *bptr++;
	  ev->event[1] = *bptr++;
	  ev->event[2] = *bptr++;			/* and save data */
	  ++ev;
	  ++msg->midi_event.n;	    
	} else {
	  bptr += 4;
	}
      }
      if ( msg->midi_event.n ){
	sc_msgTx(&msg,driver,0);
      } else {
	sc_msgFree(&msg);
      }
    }
    (void)midi_start_receive(md);	/* start next reception */
  }
}


/*
** midi_start_receive
**
** Start reception on the IN endpoint
**
** Input: md - pointer to the midi device
*/
static int midi_start_receive (t_midi *md)
{
  t_usbh_transfer *uht=&(md->in_ev_buf.uht);
  int rc;

  uht->eph=md->ep_hdl[IN];
  uht->buf=md->in_ev_buf.ep_buf;
  memset(md->in_ev_buf.ep_buf,0,md->in_ev_buf.ep_size);
  uht->size=USBH_SIZE_DEFAULT;
  uht->toggle=USBH_TOGGLE_PRESERVE;
  uht->cb_fn=midi_receive_cb;
  uht->cb_param=md;
  do {
    rc=usbh_transfer(uht,USBH_MODE_NON_BLOCK);
  } while (rc==USBH_ERR_TRANSFER_FULL);

  return rc;
}


/*
** Send all queued events.
**
** Input: dev - device number
** Return: MIDI_SUCCESS
**         MIDI_ERR_NOT_PRESENT
**         MIDI_ERR_RESOURCE
**         MIDI_ERROR
*/
int midi_send (int dev)
{
  t_midi *md=midi+dev;
  t_midi_event_buf *ev_buf;
  t_midi_event *ev;
  hcc_uint i;
  hcc_u8 *bptr;
  int rc=MIDI_SUCCESS;

  if (dev<MIDI_MAX_DEVICE && md->ep_hdl[EP0])	/* check if device is available */
  {
    ev_buf=&md->out_ev_buf;
    if (os_mutex_get(midi_mutex)==OS_SUCCESS)	/* get mutex */
    {
      while (ev_buf->r!=ev_buf->w || ev_buf->f)	/* while unsent events are available */
      {
        bptr=ev_buf->ep_buf;
        for (i=0;i<(ev_buf->ep_size>>2);i++)	/* put EP size/4 no. events to the output buffer */
        {
          ev=ev_buf->event+ev_buf->r;
          *bptr++=(hcc_u8)((ev->cn<<4)|ev->cin);/* set CN and CIN */
          *bptr++=ev->event[0];
          *bptr++=ev->event[1];
          *bptr++=ev->event[2];						/* set data */
          
          if (++ev_buf->r==MIDI_EVENT_BUFFER_SIZE) ev_buf->r=0;	/* set new R position */
          if (ev_buf->r==ev_buf->w) {		/* if all events processed */
            ++i;				/* increase it to indicate the right amount of events */
            ev_buf->f=0;			/* set F to 0 */
            break;				/* and stop */
          }
        }

        if (usbh_send_b(md->ep_hdl[OUT],ev_buf->ep_buf,i<<2,NULL,USBH_TIMEOUT_BULK)!=USBH_SUCCESS) 
        {					/* send data */
          rc=MIDI_ERROR;
        }
      }
      (void)os_mutex_put(midi_mutex);				/* release mutex */
    }
    else rc=MIDI_ERR_RESOURCE;
  }
  else rc=MIDI_ERR_NOT_PRESENT;

  return rc;
}



/*
** midi_add_event
**
** Add an event to MIDI device
**
** Input: dev - device number
**        event - pointer to the event
** Return: MIDI_SUCCESS
**         MIDI_ERR_NOT_PRESENT
**         MIDI_ERR_RESOURCE
**         MIDI_FULL
*/
int midi_add_event (int dev, t_midi_event *event)
{
  t_midi *md=midi+dev;
  t_midi_event_buf *ev_buf;
  int rc=MIDI_ERR_NOT_PRESENT;

  if (dev<MIDI_MAX_DEVICE && md->ep_hdl[EP0])	/* check if device available */
  {
    ev_buf=&md->out_ev_buf;
    _memcpy(&ev_buf->event[ev_buf->w],event,sizeof(t_midi_event));	/* save event */
    if (os_mutex_get(midi_mutex)==OS_SUCCESS)		/* get mutex */
    {
      /* set R poistion and F flag, if more data received than the buffer size then drop the oldest */
      if (ev_buf->f && ++ev_buf->r == MIDI_EVENT_BUFFER_SIZE) ev_buf->r=0;
      if (++ev_buf->w == MIDI_EVENT_BUFFER_SIZE) ev_buf->w=0;
      if (ev_buf->w == ev_buf->r) 	/* if buffer is full set F and indicate it to the caller */
      {
        ev_buf->f=1;
        rc=MIDI_FULL;
      }
      else
      {
        rc=MIDI_SUCCESS;
      }
      (void)os_mutex_put(midi_mutex);				/* release mutex */
    }
    else rc=MIDI_ERR_RESOURCE;
  }

  return rc;
}


/*
** midi_get_event
**
** Get event received on a MIDI device
**
** Input: dev - device number
** Output: event - pointer to event structure where event needs to be stored
** Return: MIDI_SUCCESS
**         MIDI_ERR_NOT_PRESENT
**         MIDI_ERR_RESOURCE
**         MIDI_NO_EVENT
*/
int midi_get_event (int dev, t_midi_event *event)
{
  t_midi *md=midi+dev;
  t_midi_event_buf *ev_buf;
  int rc=MIDI_ERR_NOT_PRESENT;

  if (dev<MIDI_MAX_DEVICE && md->ep_hdl[EP0])		/* check if device is available */
  {
    ev_buf=&md->in_ev_buf;
    if (ev_buf->r!=ev_buf->w || ev_buf->f)			/* if event is available */
    {
      _memcpy(event,&ev_buf->event[ev_buf->r],sizeof(t_midi_event));	/* store event */
      if (os_mutex_get(midi_mutex)==OS_SUCCESS)		/* get mutex */
      {
        /* set R and F flags */
        if (++ev_buf->r==MIDI_EVENT_BUFFER_SIZE) ev_buf->r=0;
        ev_buf->f=0;
        (void)os_mutex_put(midi_mutex);				/* release mutex */
        rc=MIDI_SUCCESS;
      }
      else rc=MIDI_ERR_RESOURCE;
    }
    else rc=MIDI_NO_EVENT;
  }
  
  return rc;
}


/*
** midi_state
**
** Returns the state of a MIDI device
**
** Input: dev - device number
** Return: MIDI_SUCCESS - device present
**         MIDI_ERR_NOT_PRESENT - device not available
*/
int midi_state (int dev)
{
  t_midi *md=midi+dev;
  if (dev<MIDI_MAX_DEVICE && md->ep_hdl[EP0]) return MIDI_SUCCESS;
  return MIDI_ERR_NOT_PRESENT;
}


/*
** midi_get_info
**
** Get no. IN and OUT cables on a MIDI device
**
** Input: dev - device number
** Output: n_in - where to store no. IN cables
**         n_out - where to store no. OUT cables
** Return: MIDI_SUCCESS - device present
**         MIDI_ERR_NOT_PRESENT - device not available
*/
int midi_get_info (int dev, hcc_u8 *n_in, hcc_u8 *n_out)
{
  t_midi *md=midi+dev;
  if (dev<MIDI_MAX_DEVICE && md->ep_hdl[EP0])
  {
    *n_in=md->in_cn_cnt;
    *n_out=md->out_cn_cnt;
    return MIDI_SUCCESS;
  }
  return MIDI_ERR_NOT_PRESENT;
}


/*
** midi_init
**
** Initialize midi module
**
** Return: MIDI_SUCCESS
**         MIDI_ERR_RESOURCE
**         MIDI_ERROR
*/
int midi_init (void)
{
  int rc;
  _memset(midi,0,sizeof(midi));		/* reset midi structure */

  driver = sc_procIdGet(NULL,SC_NO_TMO);
      
  rc=os_mutex_create(&midi_mutex);	/* create mutex */
  if (rc) return MIDI_ERR_RESOURCE;

  									/* add class driver to PMGR */
  rc=pmgr_add_cd(_midi_check,_midi_connect,_midi_disconnect,_midi_descriptor,(t_cd_ep_dsc *)&midi_cd_ep_dsc);
  if (rc) return MIDI_ERROR;

  return MIDI_SUCCESS;
}


