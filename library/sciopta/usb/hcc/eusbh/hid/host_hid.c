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
#include <hcc/eusbh/usb_driver/usb_host.h>
#include <hcc/eusbh/usb_driver/usb_utils.h>
#include <hcc/eusbh/usb_driver/pmgr/pmgr.h>
#include <hcc/eusbh/hid/host_hid.h>
#include <hcc/eusbh/hid/hid_parser.h>
#if HID_MOUSE_ENABLE
#include <hcc/eusbh/hid/host_hid_mouse.h>
#endif
#if HID_KBD_ENABLE
#include <hcc/eusbh/hid/host_hid_kbd.h>
#endif
#if HID_EXPLORE_ENABLE
#include <hcc/eusbh/hid/host_hid_explore.h>
#endif


#define HID_CLASS			0x03


#define HIDDESC_HID       0x21
#define HIDDESC_REPORT    0x22
#define HIDDESC_PHYSICAL  0x23

#define HIDRQ_GET_REPORT    0x1
#define HIDRQ_GET_IDLE      0x2
#define HIDRQ_GET_PROTOCOL  0x3

#define HIDRQ_SET_REPORT    0x9
#define HIDRQ_SET_IDLE      0xa
#define HIDRQ_SET_PROTOCOL  0xb


static const t_cd_ep_dsc hid_cd_ep_dsc[] = {
  3,
  USBH_EP_DSC(USBH_TYPE_CONTROL,0),
  USBH_EP_DSC(USBH_TYPE_INT,USBH_DIR_IN),
  USBH_EP_DSC(USBH_TYPE_INT,USBH_DIR_OUT)
};


/*
** hid_check
**
** Check whether current interface is HID
**
** Input: dev_inf - device info
**        port_hdl - port handler
**        ifc_info - interface info
** Output: dev_hdl - device handler
** Return: USBH_SUCCESS - if found
**         USBH_ERR_NOT_AVAILABLE - not found
*/
static void **hid_check (void **dev_hdl, void *port_hdl, device_info_t *dev_inf, ifc_info_t *ifc_info)
{
  hcc_u8 boot,type;
  hid_info_t *hid_info=NULL;

  if (ifc_info->_class==HID_CLASS)
  {
    type=HIDTYPE_UNKNOWN;
    if (ifc_info->sclass==0x00)
    { /* non boot device */
      boot=0;
      if (dev_inf->vid==0x06d6 && dev_inf->pid==0x002f && dev_inf->rev==0x0120)
      {
        type=HIDTYPE_JOY;
      }  else {
         type=ifc_info->protocol;
      }
    }
    else if (ifc_info->sclass==0x01)
    { /* boot device */
      if (ifc_info->protocol<3)
      {
        boot=1;
        type=ifc_info->protocol;
      }
      else
      { /* bad protocol value, skip interface */
        return NULL;
      }
    }

#if HID_MOUSE_ENABLE
    if (type==HIDTYPE_MOUSE)
    {
      hid_info=(hid_info_t *)hid_mouse_get_hdl();
    }
#endif
#if HID_KBD_ENABLE
    if (type==HIDTYPE_KBD)
    {
      hid_info=(hid_info_t *)hid_kbd_get_hdl();
    }
#endif
#if HID_EXPLORE_ENABLE
    if (type==HIDTYPE_UNKNOWN)
    {
      hid_info=(hid_info_t *)hid_explore_get_hdl();
    }
#endif
    if (hid_info)
    {
      hid_info->boot=boot;
      hid_info->type=type;
      hid_info->ifc_ndx=ifc_info->ndx;
      *dev_hdl=hid_info;
      return hid_info->ep_hdl;
    }
  }
  return NULL;
}



/*
** hid_connect
**
** Connect CDC serial device
** Callback function for port manager class driver handler.
**
** Input: dev_hdl - device handler
**        ifc_hdl - port handler
** Return: USBH_SUCCESS - if found
**         USBH_ERR_NOT_AVAILABLE - not found
*/
static int hid_connect (void *dev_hdl, void *ifc_hdl)
{
  int rc=USBH_ERR_NOT_AVAILABLE;
  hid_info_t *hid_info=(hid_info_t *)dev_hdl;

  if (hid_info->ep_hdl[EP0] && hid_info->ep_hdl[IN_IT])
  {
#if HID_MOUSE_ENABLE
    if (hid_info->type==HIDTYPE_MOUSE)
    {
      rc=hid_mouse_connect(dev_hdl);
    }
#endif
#if HID_KBD_ENABLE
    if (hid_info->type==HIDTYPE_KBD)
    {
      rc=hid_kbd_connect(dev_hdl);
    }
#endif
#if HID_EXPLORE_ENABLE
    if (hid_info->type==HIDTYPE_UNKNOWN)
    {
      rc=hid_explore_connect(dev_hdl);
    }
#endif
  }
  return rc;
}


/*
** hid_disconnect
**
** Input: dev_hdl - device handler
**
** Callback function for usb host class driver handler.
*/
static int hid_disconnect (void *dev_hdl)
{
  int rc=USBH_SUCCESS;
  hid_info_t *hid_info=(hid_info_t *)dev_hdl;

#if HID_MOUSE_ENABLE
  if (hid_info->type==HIDTYPE_MOUSE)
  {
    rc=hid_mouse_disconnect(dev_hdl);
  }
#endif
#if HID_KBD_ENABLE
  if (hid_info->type==HIDTYPE_KBD)
  {
    rc=hid_kbd_disconnect(dev_hdl);
  }
#endif
#if HID_EXPLORE_ENABLE
  if (hid_info->type==HIDTYPE_UNKNOWN)
  {
    rc=hid_explore_disconnect(dev_hdl);
  }
#endif
  return rc;
}


/*
** hid_descriptor
**
** Try to precess unknown descriptors.
*/
static int hid_descriptor (void *dev_hdl, hcc_u8 *dsc)
{
  int rc=USBH_SUCCESS;
#if HID_EXPLORE_ENABLE
  hid_info_t *hid_info=(hid_info_t *)dev_hdl;

  if (hid_info->type==HIDTYPE_UNKNOWN)
  {
    if (USBDSC_TYPE(dsc)==HIDDESC_HID)
    {
      hcc_u8 ndesc=*(dsc+5);
      if (ndesc)
      {
        if (*(dsc+6)==HIDDESC_REPORT)
        {
          rc=hid_explore_report_size(dev_hdl,RD_LE16(dsc+7));
        }
      }
    }
  }

#endif
  return rc;
}



/*
** hid_init
**
** Initialize HID module
** Return: 0 - success / other if error
*/
int hid_init (void)
{
  return pmgr_add_cd(hid_check,hid_connect,hid_disconnect,hid_descriptor,(t_cd_ep_dsc *)&hid_cd_ep_dsc);
}



/* get report descriptor */
hcc_uint hid_get_report_descriptor(hid_info_t *hid_info, hcc_u8 *buffer, hcc_u16 length)
{
  hcc_u8 setup[8];
  fill_setup_packet(setup, STP_DIR_IN, STP_TYPE_STD, STP_RECIPIENT_IFC,
     STDRQ_GET_DESCRIPTOR, HIDDESC_REPORT<<8, hid_info->ifc_ndx, length);

  if (usbh_receive_control_b(hid_info->ep_hdl[EP0],setup,buffer,length,NULL)!=USBH_SUCCESS)
  {
    return 1;
  }
  return 0;
}

/* get the sepcified report over the control channel */
hcc_uint hid_set_report(hid_info_t *hid_info, hcc_u8 report_id, hcc_u8 *buffer, hcc_u16 length)
{
  hcc_u8 setup[8];
  fill_setup_packet(setup, STP_DIR_OUT, STP_TYPE_CLASS, STP_RECIPIENT_IFC,
     HIDRQ_SET_REPORT, (hcc_u16)((0x02<<8) | report_id), hid_info->ifc_ndx,
     length);

  if (usbh_send_control_b(hid_info->ep_hdl[EP0],setup,buffer,length,NULL)!=USBH_SUCCESS)
  {
    return 1;
  }
  return 0;
}


/* get the sepcified report over the control channel */
hcc_uint hid_get_report(hid_info_t *hid_info, hcc_u8 report_id, hcc_u8 *buffer, hcc_u16 length)
{
  hcc_u8 setup[8];
  fill_setup_packet(setup, STP_DIR_IN, STP_TYPE_CLASS, STP_RECIPIENT_IFC,
     HIDRQ_GET_REPORT, (hcc_u16)((0x01<<8) | report_id), hid_info->ifc_ndx,
     length);

  if (usbh_receive_control_b(hid_info->ep_hdl[EP0],setup,buffer,length,NULL)!=USBH_SUCCESS)
  {
    return 1;
  }
  return 0;
}

/* set protocol to boot/nonboot version */
int hid_set_protocol(hid_info_t *hid_info, hcc_u8 boot)
{
  hcc_u8 setup[8];
  fill_setup_packet(setup, STP_DIR_OUT, STP_TYPE_CLASS, STP_RECIPIENT_IFC,
     HIDRQ_SET_PROTOCOL, (hcc_u8)(boot ? 0 : 1), hid_info->ifc_ndx, 0);
  return usbh_send_control_b(hid_info->ep_hdl[EP0],setup,NULL,0,NULL);
}

/* set idle time of device */
int hid_set_idle(hid_info_t *hid_info, hcc_u8 dur, hcc_u8 report_id)
{
  hcc_u8 setup[8];
  fill_setup_packet(setup, STP_DIR_OUT, STP_TYPE_CLASS, STP_RECIPIENT_IFC,
     HIDRQ_SET_IDLE, (hcc_u16)((dur<<8) | report_id), hid_info->ifc_ndx, 0);
  return usbh_send_control_b(hid_info->ep_hdl[EP0],setup,NULL,0,NULL);
}


