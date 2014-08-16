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
#include <hcc/eusbh/hid/hid_usage.h>
#include <hcc/eusbh/hid/host_hid.h>
#include <hcc/eusbh/hid/host_hid_mouse.h>
#include <hcc/eusbh/hid/hid_parser.h>

/*****************************************************************************
 * Module variables.
 *****************************************************************************/
/* Mouse state info. All mouses connected to the system shall modify this
   structure. Currently only one mouse is supported. */
static struct {
  hid_info_t hid_info;

  hcc_u8 hid_mouse_device;

  t_hid_cb cb;

  t_usbh_transfer uht;
  hcc_u8 report_data[64];

  hcc_u16 x;
  hcc_u16 y;
  hcc_u8 buttons[3];
} hid_mouse_info;


/* These structures define how to access items in a HID mouse report, if the
   mouse talks the boot protocol. Most mouse is compatibile with this
   protocol. */
static const report_t in_report = {
  rpt_in,  /* in report */
  0,       /* id is not used */
  24,      /* bit ofs (size in bits) */
  {        /* report items */
  	{ /*button 0*/
      0,     /*min vale device can report*/
      1,     /*max value device can report*/
      0,     /*min value read can return*/
      1,     /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_BUTTON,
      1,     /* usage min (button 1) */
      1,     /* usage max  */
      0,     /*ary size*/
      0,     /* offset */
      0,     /*shift*/
      1,     /*size*/
      0      /*signed?*/
  	},
  	{ /*button 2*/
      0,     /*min vale device can report*/
      1,     /*max value device can report*/
      0,     /*min value read can return*/
      1,     /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_BUTTON,
      2,     /* usage min (button 2) */
      2,     /* usage max */
      0,     /*ary size*/
      0,     /* offset */
      1,     /*shift*/
      1,     /*size*/
      0      /*signed?*/
  	},
  	{ /*button 3*/
      0,     /*min vale device can report*/
      1,     /*max value device can report*/
      0,     /*min value read can return*/
      1,     /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_BUTTON,
      3,     /* usage min (button 3) */
      3,     /* usage max  */
      0,     /*ary size*/
      0,     /* offset */
      2,     /*shift*/
      1,     /*size*/
      0      /*signed?*/
  	},
  	{ /* x coord */
      -127,  /*min vale device can report*/
      127,   /*max value device can report*/
      -127,  /*min value read can return*/
      127,   /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_GEN_DES,
      HIDU_X, /* usage min (not used) */
      HIDU_X, /* usage max (not used) */
      0,     /*ary size*/
      1,     /* offset */
      0,     /*shift*/
      8,     /*size*/
      1      /*signed?*/
  	},
  	{ /* y coord */
      -127,  /*min vale device can report*/
      127,   /*max value device can report*/
      -127,  /*min value read can return*/
      127,   /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_GEN_DES,
      HIDU_Y, /* usage min (not used) */
      HIDU_Y, /* usage max (not used) */
      0,     /*ary size*/
      2,     /* offset */
      0,     /*shift*/
      8,     /*size*/
      1      /*signed?*/
  	},
  },
  hid_mouse_info.report_data
};

/*****************************************************************************
 * Function predefinitions.
 *****************************************************************************/
static int hid_mouse_receive_init (void);
static void hid_mouse_convert (void);

/*****************************************************************************
 * Function definitions.
 *****************************************************************************/
/*****************************************************************************
 * Name:
 *    hid_mouse_receive_cb
 * In:
 *    param - parameter
 *    rc - return code
 *
 * Description:
 *    Callback function for the receive transfer.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
static void hid_mouse_receive_cb (void *param, int rc)
{
  if (rc==USBH_SUCCESS || rc==USBH_SHORT_PACKET)
  {
    if (hid_mouse_info.uht.csize>=4)
    {
      hid_mouse_convert();
      (hid_mouse_info.cb)();
      hid_mouse_receive_init();
    }
  }
}


/*****************************************************************************
 * Name:
 *    hid_mouse_receive_init
 * In:
 *    -
 * Out:
 *    transfer start return code
 *
 * Description:
 *    Initialize receive.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
static int hid_mouse_receive_init (void)
{
  hid_mouse_info.uht.eph=hid_mouse_info.hid_info.ep_hdl[IN_IT];
  hid_mouse_info.uht.buf=hid_mouse_info.report_data;
  hid_mouse_info.uht.size=USBH_SIZE_DEFAULT;
  hid_mouse_info.uht.toggle=USBH_TOGGLE_PRESERVE;
  hid_mouse_info.uht.cb_fn=hid_mouse_receive_cb;
  return usbh_transfer(&hid_mouse_info.uht,USBH_MODE_NON_BLOCK);
}


/*****************************************************************************
 * Name:
 *    hid_mouse_init
 * In:
 *    pointer to the callback function
 * Out:
 *    USBH_SUCCESS
 *
 * Description:
 *    Initialize mouse driver.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
int hid_mouse_init (t_hid_cb cb)
{
  hid_mouse_info.hid_mouse_device=0;
  hid_mouse_info.cb=cb;
  return USBH_SUCCESS;
}


/*****************************************************************************
 * Name:
 *    hid_mouse_get_hdl
 * In:
 *    -
 * Out:
 *    NULL / pointer to info structure
 *
 * Description:
 *    Get mouse information handler if available.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
void *hid_mouse_get_hdl (void)
{
  if (hid_mouse_info.hid_mouse_device==0)
  {
    return &hid_mouse_info;
  }
  return NULL;
}


/*****************************************************************************
 * Name:
 *    hid_mouse_connect
 * In:
 *    device handler
 * Out:
 *    USBH_*
 *
 * Description:
 *    Connect mouse driver.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
int hid_mouse_connect (void *dev_hdl)
{
  if (hid_mouse_info.hid_mouse_device) return 0;

  hid_mouse_info.x=0;
  hid_mouse_info.y=0;
  hid_mouse_info.buttons[0]=0;
  hid_mouse_info.buttons[1]=0;
  hid_mouse_info.buttons[2]=0;
  hid_mouse_info.report_data[0]=0;
  hid_mouse_info.report_data[1]=0;
  hid_mouse_info.report_data[2]=0;
  hid_mouse_info.report_data[3]=0;

  /* Get default values from device. */
  if (hid_get_report(&hid_mouse_info.hid_info, 0, hid_mouse_info.report_data, 4)==0)
  {
    /* Convert report data. */
    hid_mouse_convert();
    hid_mouse_info.hid_mouse_device=1;
    return hid_mouse_receive_init();
  }

  return USBH_ERR_NOT_AVAILABLE;
}


/*****************************************************************************
 * Name:
 *    hid_mouse_disconnect
 * In:
 *    Device handler
 * Out:
 *    error code
 *
 * Description:
 *    Disconnect mouse driver.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
int hid_mouse_disconnect (void *dev_hdl)
{
  hid_mouse_info.hid_mouse_device=0;
  return 0;
}

/*****************************************************************************
 * Name:
 *    hid_mouse_get_x
 * In:
 *    -
 * Out:
 *    Current value of X offset.
 *
 * Description:
 *    Returns X offset.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u16 hid_mouse_get_x(void)
{
  return(hid_mouse_info.x);
}
/*****************************************************************************
 * Name:
 *    hid_mouse_get_y
 * In:
 *    -
 * Out:
 *    Current value of Y offset.
 *
 * Description:
 *    Returns Y offset.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u16 hid_mouse_get_y(void)
{
  return(hid_mouse_info.y);
}
/*****************************************************************************
 * Name:
 *    hid_mouse_get_button
 * In:
 *    b - index of button.
 * Out:
 *    0 - button released
 *    1 - button pressed
 *
 * Description:
 *    Return state os a button.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_mouse_get_button(hcc_u8 b)
{
  if (b>2)
  {
    return(0);
  }
  return(hid_mouse_info.buttons[b]);
}
/*****************************************************************************
 * Name:
 *    hid_mouse_convert
 * In:
 *    -
 * Out:
 *    -
 *
 * Description:
 *    Update stored values with values got in the last report. Some values
 *    contain relative data and thus instead of overwriting we add the cnahge.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
static void hid_mouse_convert(void)
{
  hcc_s32 tmp;
  int x;
  hcc_u16 *axe_data[2];
  axe_data[0]=&hid_mouse_info.x;
  axe_data[1]=&hid_mouse_info.y;

  /* Read two axes. */
  for (x=0; x<sizeof(axe_data)/sizeof(axe_data[0]); x++)
  {
    if (!read_item((void*)(in_report.items+3+x)
         , hid_mouse_info.report_data
         , 0, &tmp))
    {
      tmp+=(signed int)(*axe_data[x]);
      if (tmp < 0)
      {
        tmp=0;
      }
      if (tmp > 0xffff)
      {
        tmp=0xffff;
      }
      *axe_data[x]=(hcc_u16)tmp;
    }  	
  }

  /* Read three buttons. */
  for (x=0; x<3; x++)
  {
    if (!read_item((void*)(in_report.items+x)
         , hid_mouse_info.report_data
         , 0, &tmp))
    {
      
      hid_mouse_info.buttons[x]=(hcc_u8)(tmp ? 1 : 0);
    } 	
  }
}


