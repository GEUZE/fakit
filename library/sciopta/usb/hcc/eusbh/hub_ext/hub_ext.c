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
#include <hcc/eusbh/usbh.msg>

#include <hcc/hcc_types.h>
#include <hcc/eusbh/usb_driver/usb_host.h>
#include <hcc/eusbh/usb_driver/usb_utils.h>
#include <hcc/eusbh/usb_driver/pmgr/pmgr.h>
#include <hcc/eusbh/hub_ext/hub_ext.h>

static sc_pid_t pmgr_pid = SC_ILLEGAL_PID;
union sc_msg {
  sc_msgid_t id;
};

#define HUB_EXT_CLASS		0x09
#define HUB_EXT_SCLASS		0x00


/* Request types */
#define ClearHubFeature		0x20
#define ClearPortFeature	0x23
#define ClearTTBuffer		0x23
#define GetHubDescriptor	0xa0
#define GetHubStatus		0xa0
#define GetPortStatus		0xa3
#define ResetTT				0x23
#define SetHubDescriptor	0x20
#define SetHubFeature		0x20
#define SetPortFeature		0x23
#define GetTTState			0xa3
#define StopTT				0x23

/* Request codes */
#define GET_STATUS			0
#define CLEAR_FEATURE		1
#define SET_FEATURE			3
#define GET_DESCRIPTOR		6
#define SET_DESCRIPTOR		7
#define CLEAR_TT_BUFFER		8
#define RESET_TT			9
#define GET_TT_STATE		10
#define STOP_TT				11

/* Feature selectors */
#define F_C_HUB_LOCAL_POWER		0
#define F_C_HUB_OVER_CURRENT	1
#define F_PORT_CONNECTION		0
#define F_PORT_ENABLE			1
#define F_PORT_SUSPEND			2
#define F_PORT_OVER_CURRENT		3
#define F_PORT_RESET			4
#define F_PORT_POWER			8
#define F_PORT_LOW_SPEED		9
#define F_PORT_HIGH_SPEED		10
#define F_C_PORT_CONNECTION		16
#define F_C_PORT_ENABLE			17
#define F_C_PORT_SUSPEND		18
#define F_C_PORT_OVER_CURRENT	19
#define F_C_PORT_RESET			20
#define F_PORT_TEST				21
#define F_PORT_INDICATOR		22


#define EP0		0
#define IN_IT	1
static const t_cd_ep_dsc hub_ext_cd_ep_dsc[3] = {
  2,
  USBH_EP_DSC(USBH_TYPE_CONTROL,0),
  USBH_EP_DSC(USBH_TYPE_INT,USBH_DIR_IN)
};

typedef struct {
  hcc_u8 hub_ext_device;/* device present */

  void *ep_hdl[2];
  hcc_u16 in_it_psize;	/* interrupt IN packet size */

  t_usbh_transfer uht;	/* host transfer */
  hcc_u8 buf[64];		/* temporary read buffer */

  hcc_u8 tt;			/* no TT / single / multiple */
  hcc_u8 ports;			/* number of downstream ports */
  hcc_u16 pptime;		/* poer on to power good time */

  void *hub_handler;	/* HUB handler assigned */
} t_hub_ext_info;
t_hub_ext_info hub_ext_info[HUB_EXT_MAX_DEVICE];





/*
** Execute setup transfer
** INPUT: rt - request type
**        r - request
**        v - value
**        i - index;
**        l - length
**        buf - tranfer buffer (if needed)
** RETURN: 0-on success / 1-if error
*/
static int hub_ext_ep0_transfer (t_hub_ext_info *p_hi, hcc_u8 rt, hcc_u8 r, hcc_u16 v, hcc_u16 i,
                                 hcc_u16 l, hcc_u8 *buf)
{
  hcc_u8 setup[8];
  int rc;

  setup[0]=rt;
  setup[1]=r;
  WR_LE16(setup+2,v)
    ;
  WR_LE16(setup+4,i)
    ;
  WR_LE16(setup+6,l)
    ;
  if (rt&0x80) rc=usbh_receive_control_b(p_hi->ep_hdl[EP0],setup,buf,l,NULL);
          else rc=usbh_send_control_b(p_hi->ep_hdl[EP0],setup,buf,l,NULL);
  if (rc==USBH_SUCCESS)
  {
    return 0;
  }
  return 1;
}



/**********************************************************************************************************/
/******************** HUB driver functions (to handle devices attached to EXT HUB) ************************/
/**********************************************************************************************************/
static void hub_ext_int_cb (void *param, int rc)
{
  if (rc==USBH_SUCCESS)
  {
   sc_msg_t msg = sc_msgAlloc(sizeof(hcc_usbhIrq_t),
		      HCC_USBH_IRQ1,
		      SC_DEFAULT_POOL,
		      SC_FATAL_IF_TMO);
    sc_msgTx(&msg,pmgr_pid,0);
//->    sc_trigger(pmgr_pid);
//->    (void)os_event_set(pmgr_event_bit);
  }
  else
  {
    t_hub_ext_info *p_hi=(t_hub_ext_info *)param;
    p_hi->uht.csize=0;
  }
}


/*
** Start transfer on the interrupt channel.
** Status change transfer.
*/
static int hub_ext_start_int_transfer (t_hub_ext_info *p_hi)
{
  t_usbh_transfer *uht=&(p_hi->uht);
  int rc;
  uht->eph=p_hi->ep_hdl[IN_IT];
  uht->buf=p_hi->buf;
  uht->size=USBH_SIZE_DEFAULT;
  uht->toggle=USBH_TOGGLE_PRESERVE;
  uht->cb_fn=hub_ext_int_cb;
  uht->cb_param=p_hi;
  do {
    rc=usbh_transfer(uht,USBH_MODE_NON_BLOCK);
  } while (rc==USBH_ERR_TRANSFER_FULL);

  return rc;
}


/*
** Reset external HUB port
*/
static int hub_ext_port_reset (void *dev_hdl, hcc_u8 port)
{
  t_hub_ext_info *p_hi=(t_hub_ext_info *)dev_hdl;
  hcc_u8 st[4];
  hcc_u16 pstc,pst;

  usbh_delay(p_hi->pptime);

  if (hub_ext_ep0_transfer(p_hi,ClearPortFeature,CLEAR_FEATURE,F_PORT_ENABLE,port,0,NULL)) return USBH_SPEED_ERROR;
  if (hub_ext_ep0_transfer(p_hi,SetPortFeature,SET_FEATURE,F_PORT_RESET,port,0,NULL)) return USBH_SPEED_ERROR;

  do {
    if (hub_ext_ep0_transfer(p_hi,GetPortStatus,GET_STATUS,0,port,4,st)) return USBH_SPEED_ERROR;
    pstc=RD_LE16(st+2);
  } while ((pstc&(1<<F_PORT_RESET))==0);

  if (hub_ext_ep0_transfer(p_hi,ClearPortFeature,CLEAR_FEATURE,F_C_PORT_RESET,port,0,NULL)) return USBH_SPEED_ERROR;
  usbh_delay(200);

  if (hub_ext_ep0_transfer(p_hi,GetPortStatus,GET_STATUS,0,port,4,st)) return USBH_SPEED_ERROR;
  pst=RD_LE16(st);
  if (pst&(1<<F_PORT_ENABLE))
  {
    if (pst&(1<<F_PORT_LOW_SPEED)) return USBH_LOW_SPEED;
    else if (pst&(1<<F_PORT_HIGH_SPEED)) return USBH_HIGH_SPEED;
    else return USBH_FULL_SPEED;
  }

  return USBH_SPEED_ERROR;
}


/*
** Disable external HUB port
*/
static int hub_ext_port_disable (void *dev_hdl, hcc_u8 port)
{
  t_hub_ext_info *p_hi=(t_hub_ext_info *)dev_hdl;
  if (hub_ext_ep0_transfer(p_hi,ClearPortFeature,CLEAR_FEATURE,F_PORT_ENABLE,port,0,NULL)) return 1;
  return 0;
}


/*
** Idle routine which needs to be called sequencially or should be an interrupt.
** Check device connect/disconnect.
*/
static int hub_ext_scan (void *dev_hdl, t_hub_scan_inf *hsi)
{
  t_hub_ext_info *p_hi=(t_hub_ext_info *)dev_hdl;
  int rc=0;

  if (p_hi->uht.csize)			/* status change obtained */
  {
    hcc_u16 val;
    hcc_u16 pst,pstc;
    hcc_u8 st[4];
    hcc_u8 i=0;

    /* HUB STATUS CHANGE */
    if (p_hi->buf[0]&1)
    {
      if (hub_ext_ep0_transfer(p_hi,GetHubStatus,GET_STATUS,0,0,4,st)) return 0;
      pst=RD_LE16(st);
      pstc=RD_LE16(st+2);

      /* clear all hub change bits */
      for (val=0;val<2;val++)
      {
        if (pstc&(1<<val))
        {
          if (hub_ext_ep0_transfer(p_hi,ClearHubFeature,CLEAR_FEATURE,(hcc_u16)(val+16),i,0,NULL)) return 0;
        }
      }

      if (pstc&(1<<F_C_HUB_LOCAL_POWER)) return 0;

      if (pstc&(1<<F_C_HUB_OVER_CURRENT))
      {
        do {
          for (i=1;i<(p_hi->ports+1);i++)				/* enable power for all ports */
          {
            if (hub_ext_ep0_transfer(p_hi,SetPortFeature,SET_FEATURE,F_PORT_POWER,(hcc_u16)i,0,NULL)) return 1;
          }

          if (hub_ext_ep0_transfer(p_hi,GetHubStatus,GET_STATUS,0,0,4,st)) return 0;
          pst=RD_LE16(st);
          pstc=RD_LE16(st+2);							/* and recheck overcurrent on HUB basis */
        } while ((pstc&(1<<F_C_HUB_OVER_CURRENT))==0);	/* if all powered on don't do anything else */
      }													/* the next HUB status request will start reenumeration */

    }

    /* PORT STATUS CHANGE */
    for (i=1;i<p_hi->ports+1;i++)
    {
      if (p_hi->buf[i>>3]&(1<<i))
      {
        if (hub_ext_ep0_transfer(p_hi,GetPortStatus,GET_STATUS,0,(hcc_u16)i,4,st)) return 0;
        pst=RD_LE16(st);
        pstc=RD_LE16(st+2);

        if (pstc&((1<<F_PORT_CONNECTION)|(1<<F_PORT_OVER_CURRENT)))
        {
          hsi->state=0;
          hsi->port=i;

          if (pstc&(1<<F_PORT_CONNECTION))		/* connection status changed */
          {
            if (pst&(1<<F_PORT_CONNECTION)) hsi->state=USBH_STATE_CONNECTED;
                                       else hsi->state=USBH_STATE_DISCONNECTED;
          }

          if (pstc&(1<<F_PORT_OVER_CURRENT))	/* overcurrent indication */
          {
            do {								/* try to reenable power while overcurrent persists */
              if (hub_ext_ep0_transfer(p_hi,SetPortFeature,SET_FEATURE,F_PORT_POWER,(hcc_u16)i,0,NULL)) return 0;
              if (hub_ext_ep0_transfer(p_hi,GetPortStatus,GET_STATUS,0,(hcc_u16)i,4,st)) return 0;
              pst=RD_LE16(st);
              pstc|=RD_LE16(st+2);
            } while ((pst&(1<<F_PORT_POWER))==0);
												/* check connection status after power remains on */
            if (pst&(1<<F_PORT_CONNECTION)) hsi->state=USBH_STATE_CONNECTED;
                                       else hsi->state=USBH_STATE_DISCONNECTED;
          }

          ++rc;
          ++hsi;
        }

        /* clear all port change bits */
        for (val=0;val<5;val++)
        {
          if (pstc&(1<<val))
          {
            if (hub_ext_ep0_transfer(p_hi,ClearPortFeature,CLEAR_FEATURE,(hcc_u16)(val+16),i,0,NULL)) return 0;
          }
        }
      }
    }

    if (hub_ext_start_int_transfer(p_hi)) return 0;		/* start next transfer */
  }

  return rc;
}



/**********************************************************************************************************/
/********************************* EXT HUB class driver functions *****************************************/
/**********************************************************************************************************/

/*
** hub_ext_check
**
** Check whether current interface is external HUB
**
** Input: dev_inf - device info
**        port_hdl - port handler
**        ifc_info - interface info
** Output: dev_hdl - device handler
** Return: NULL - not found
**         otherwise pointer to ep_hdl array
*/
static void **hub_ext_check (void **dev_hdl, void *port_hdl, device_info_t *dev_inf, ifc_info_t *ifc_inf)
{
  if (ifc_inf->_class==HUB_EXT_CLASS && ifc_inf->sclass==HUB_EXT_SCLASS)
  {
    hcc_uint i;
    for (i=0;i<HUB_EXT_MAX_DEVICE && hub_ext_info[i].hub_ext_device;i++)
      ;
    if (i<HUB_EXT_MAX_DEVICE)
    {
      hub_ext_info[i].tt=dev_inf->protocol;
      *dev_hdl=hub_ext_info+i;
      return hub_ext_info[i].ep_hdl;
    }
  }
  return NULL;
}


/*
** hub_ext_connect
**
** Connect external HUB
** Callback function for port manager class driver handler.
**
** Input:  dev_hdl - device handler
**         ifc_hdl - port handler
** Return: USBH_SUCCESS - if found
**         USBH_ERR_NOT_AVAILABLE - not found
*/
static int hub_ext_connect (void *dev_hdl, void *ifc_hdl)
{
  t_hub_ext_info *p_hi=(t_hub_ext_info *)dev_hdl;
  if (p_hi->ep_hdl[EP0] && p_hi->ep_hdl[IN_IT])
  {
    if (usbh_get_ep_inf (p_hi->ep_hdl[IN_IT], NULL, NULL, NULL, NULL, &p_hi->in_it_psize)==USBH_SUCCESS)
    {
      hcc_u8 hub_ext_buf[16];
      hcc_u8 i;

      /* Get HUB descriptor */
      if (hub_ext_ep0_transfer(p_hi,GetHubDescriptor,GET_DESCRIPTOR,0,0,(hcc_u16)(p_hi->in_it_psize+8),hub_ext_buf)==0)
      {
        p_hi->ports=hub_ext_buf[2];						/* get number of ports */
        p_hi->pptime=(hcc_u16)(hub_ext_buf[5]*2);		/* get power on to power good time */
        for (i=1;i<(p_hi->ports+1);i++)					/* enable power for all ports */
        {
          if (hub_ext_ep0_transfer(p_hi,SetPortFeature,SET_FEATURE,F_PORT_POWER,(hcc_u16)i,0,NULL)) return 1;
        }
        /* Register new HUB driver */
        p_hi->hub_handler=pmgr_add_hub(ifc_hdl,hub_ext_scan,hub_ext_port_reset,hub_ext_port_disable,NULL,NULL);
        if (p_hi->hub_handler)
        {
          if (hub_ext_start_int_transfer(p_hi)==USBH_SUCCESS)
          {
            p_hi->hub_ext_device=1;
            return USBH_SUCCESS;
          }
        }
      }
    }
  }
  return USBH_ERR_NOT_AVAILABLE;
}


/*
** hub_ext_disconnect
**
** Input:  dev_hdl - device handler
**
** Disconnect external HUB device.
** Callback function for port manager class driver handler.
**
*/
static int hub_ext_disconnect (void *dev_hdl)
{
  t_hub_ext_info *p_hi=(t_hub_ext_info *)dev_hdl;
  p_hi->hub_ext_device=0;
  pmgr_delete_hub(p_hi->hub_handler);
  return USBH_SUCCESS;
}


/*
** Initialize external HUB
*/
int hub_ext_init (sc_pid_t pmgr)
{
  hcc_uint i;
  for (i=0;i<HUB_EXT_MAX_DEVICE;hub_ext_info[i++].hub_ext_device=0)
    ;
  pmgr_pid = pmgr;
  /* add class driver */
  return pmgr_add_cd(hub_ext_check,hub_ext_connect,hub_ext_disconnect,NULL,(t_cd_ep_dsc *)&hub_ext_cd_ep_dsc);
}


