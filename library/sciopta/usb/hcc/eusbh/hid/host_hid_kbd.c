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
#include <hcc/eusbh/hid/host_hid.h>
#include <hcc/eusbh/hid/host_hid_kbd.h>
#include <hcc/eusbh/hid/hid_parser.h>
#include <hcc/eusbh/hid/hid_usage.h>

/*****************************************************************************
 * External references.
 *****************************************************************************/
/* none */
/*****************************************************************************
 * Local types.
 *****************************************************************************/
/* none */
/*****************************************************************************
 * Macro definitions.
 *****************************************************************************/
/* KBD states */
#define HKST_INVALID    0u
#define HKST_ACTIVE     1u
/*****************************************************************************
 * Module variables.
 *****************************************************************************/
struct {
  hid_info_t hid_info;

  hcc_u8 hid_kbd_device;

  t_hid_cb cb;

  t_usbh_transfer uht; 
  hcc_u8 in_report_data[8]; /* Only 8 bytes are used. */
  hcc_u8 out_report_data[1];

  hcc_u8 state;
  hcc_u8 lctrl;
  hcc_u8 lshift;
  hcc_u8 lalt;
  hcc_u8 lgui;
  hcc_u8 rctrl;
  hcc_u8 rshift;
  hcc_u8 ralt;
  hcc_u8 rgui;
  hcc_u8 keys[6];
} static hid_kbd_info;

static const report_t in_report = {
  rpt_in,
  0,
  64,
  { /* report items */
    {
      0,     /*min vale device can report*/
      1,     /*max value device can report*/
      0,     /*min value read can return*/
      1,     /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_KEYB,
      KEY_LEFTCONTROL,     /* usage min  */
      KEY_LEFTCONTROL,     /* usage max  */
      0,     /*ary size*/
      0,     /* offset */
      0,     /*shift*/
      1,     /*size*/
      0      /*signed?*/  		
    },
    {
      0,     /*min vale device can report*/
      1,     /*max value device can report*/
      0,     /*min value read can return*/
      1,     /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_KEYB,
      KEY_LEFTSHIFT,     /* usage min  */
      KEY_LEFTSHIFT,     /* usage max  */
      0,     /*ary size*/
      0,     /* offset */
      1,     /*shift*/
      1,     /*size*/
      0      /*signed?*/  		
    },
    {
      0,     /*min vale device can report*/
      1,     /*max value device can report*/
      0,     /*min value read can return*/
      1,     /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_KEYB,
      KEY_LEFTALT,     /* usage min  */
      KEY_LEFTALT,     /* usage max  */
      0,     /*ary size*/
      0,     /* offset */
      2,     /*shift*/
      1,     /*size*/
      0      /*signed?*/  		
    },
    {
      0,     /*min vale device can report*/
      1,     /*max value device can report*/
      0,     /*min value read can return*/
      1,     /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_KEYB,
      KEY_LEFT_GUI,     /* usage min  */
      KEY_LEFT_GUI,     /* usage max  */
      0,     /*ary size*/
      0,     /* offset */
      3,     /*shift*/
      1,     /*size*/
      0      /*signed?*/  		
    },
    {
      0,     /*min vale device can report*/
      1,     /*max value device can report*/
      0,     /*min value read can return*/
      1,     /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_KEYB,
      KEY_RIGHTCONTROL,     /* usage min  */
      KEY_RIGHTCONTROL,     /* usage max  */
      0,     /*ary size*/
      0,     /* offset */
      4,     /*shift*/
      1,     /*size*/
      0      /*signed?*/  		
    },
    {
      0,     /*min vale device can report*/
      1,     /*max value device can report*/
      0,     /*min value read can return*/
      1,     /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_KEYB,
      KEY_RIGHTSHIFT,     /* usage min  */
      KEY_RIGHTSHIFT,     /* usage max  */
      0,     /*ary size*/
      0,     /* offset */
      5,     /*shift*/
      1,     /*size*/
      0      /*signed?*/  		
    },
    {
      0,     /*min vale device can report*/
      1,     /*max value device can report*/
      0,     /*min value read can return*/
      1,     /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_KEYB,
      KEY_RIGHTALT,     /* usage min  */
      KEY_RIGHTALT,     /* usage max  */
      0,     /*ary size*/
      0,     /* offset */
      6,     /*shift*/
      1,     /*size*/
      0      /*signed?*/  		
    },
    {
      0,     /*min vale device can report*/
      1,     /*max value device can report*/
      0,     /*min value read can return*/
      1,     /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_KEYB,
      KEY_RIGHT_GUI,     /* usage min  */
      KEY_RIGHT_GUI,     /* usage max  */
      0,     /*ary size*/
      0,     /* offset */
      7,     /*shift*/
      1,     /*size*/
      0      /*signed?*/  		
    },
    {
      0,     /*min vale device can report*/
      0,     /*max value device can report*/
      0,     /*min value read can return*/
      0,     /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_UNDEFINED,
      0,     /* usage min  */
      0,     /* usage max  */
      0,     /*ary size*/
      1,     /* offset */
      0,     /*shift*/
      8,     /*size*/
      0      /*signed?*/  		
    },
    {
      0,     /*min vale device can report*/
      0xff,  /*max value device can report*/
      0,     /*min value read can return*/
      0xff,  /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_KEYB,
      KEY_NONE,     /* usage min  */
      0xff,     /* usage max  */
      6,     /*ary size*/
      2,     /* offset */
      0,     /*shift*/
      8,     /*size*/
      0      /*signed?*/  		
    },  	
  }	
};


static const report_t out_report = {
  rpt_out,
  0,
  8,
  {/* report items */
    {
      0,     /*min vale device can report*/
      1,     /*max value device can report*/
      0,     /*min value read can return*/
      1,     /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_LED,
      NUM_LOCK,     /* usage min  */
      NUM_LOCK,     /* usage max  */
      0,     /*ary size*/
      0,     /* offset */
      0,     /*shift*/
      1,     /*size*/
      0      /*signed?*/  		  	  
    },
    {
      0,     /*min vale device can report*/
      1,     /*max value device can report*/
      0,     /*min value read can return*/
      1,     /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_LED,
      CAPS_LOCK,     /* usage min  */
      CAPS_LOCK,     /* usage max  */
      0,     /*ary size*/
      0,     /* offset */
      1,     /*shift*/
      1,     /*size*/
      0      /*signed?*/  		  	  
    },
    {
      0,     /*min vale device can report*/
      1,     /*max value device can report*/
      0,     /*min value read can return*/
      1,     /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_LED,
      SCROLL_LOCK,     /* usage min  */
      SCROLL_LOCK,     /* usage max  */
      0,     /*ary size*/
      0,     /* offset */
      2,     /*shift*/
      1,     /*size*/
      0      /*signed?*/  		  	  
    },
    {
      0,     /*min vale device can report*/
      1,     /*max value device can report*/
      0,     /*min value read can return*/
      1,     /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_LED,
      COMPOSE,     /* usage min  */
      COMPOSE,     /* usage max  */
      0,     /*ary size*/
      0,     /* offset */
      3,     /*shift*/
      1,     /*size*/
      0      /*signed?*/  		  	  
    },
    {
      0,     /*min vale device can report*/
      1,     /*max value device can report*/
      0,     /*min value read can return*/
      1,     /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_LED,
      KANA,     /* usage min  */
      KANA,     /* usage max  */
      0,     /*ary size*/
      0,     /* offset */
      4,     /*shift*/
      1,     /*size*/
      0      /*signed?*/  		  	  
    },
    {
      0,     /*min vale device can report*/
      0,     /*max value device can report*/
      0,     /*min value read can return*/
      0,     /*max value read can return*/
      0,     /*resolution*/
      HIDUPG_LED,
      0,     /* usage min  */
      0,     /* usage max  */
      0,     /*ary size*/
      0,     /* offset */
      5,     /*shift*/
      3,     /*size*/
      0      /*signed?*/  		  	  
    }      	
  }
};


/*****************************************************************************
 * Function predefinitions.
 *****************************************************************************/
/* none */
/*****************************************************************************
 * Function definitions.
 *****************************************************************************/


static int hid_kbd_receive_init (void);
static void hid_kbd_in_convert (void);


/*****************************************************************************
 * Name:
 *    hid_kbd_receive_cb
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
static void hid_kbd_receive_cb (void *param, int rc)
{
  if (rc==USBH_SUCCESS || rc==USBH_SHORT_PACKET)
  {
    hid_kbd_in_convert();
    (hid_kbd_info.cb)();
    hid_kbd_receive_init();
  }
}



/*****************************************************************************
 * Name:
 *    hid_kbd_receive_init
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
static int hid_kbd_receive_init (void)
{
  hid_kbd_info.uht.eph=hid_kbd_info.hid_info.ep_hdl[IN_IT];
  hid_kbd_info.uht.buf=hid_kbd_info.in_report_data;
  hid_kbd_info.uht.size=USBH_SIZE_DEFAULT;
  hid_kbd_info.uht.toggle=USBH_TOGGLE_PRESERVE;
  hid_kbd_info.uht.cb_fn=hid_kbd_receive_cb;
  return usbh_transfer(&hid_kbd_info.uht,USBH_MODE_NON_BLOCK);
}




/*****************************************************************************
 * Name:
 *    hid_kbd_init
 * In:
 *    pointer to the callback function
 * Out:
 *    USBH_SUCCESS
 *
 * Description:
 *    Initialize keyboard driver.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
int hid_kbd_init (t_hid_cb cb)
{
  hid_kbd_info.hid_kbd_device=0;
  hid_kbd_info.cb=cb;
  return USBH_SUCCESS;
}


/*****************************************************************************
 * Name:
 *    hid_kbd_get_hdl
 * In:
 *    -
 * Out:
 *    NULL / pointer to info structure
 *
 * Description:
 *    Get kbd information handler if available.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
void *hid_kbd_get_hdl (void)
{
  if (hid_kbd_info.hid_kbd_device==0)
  {
    return &hid_kbd_info;
  }
  return NULL;
}


/*****************************************************************************
 * Name:
 *    hid_kbd_connect
 * In:
 *    Device handler
 * Out:
 *    USBH_*
 *
 * Description:
 *    Iitialize keyboard driver.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
int hid_kbd_connect(void *dev_hdl)
{
  int x;

  if (hid_kbd_info.hid_kbd_device) return 0;

  hid_kbd_info.lctrl=hid_kbd_info.lshift=0;
  hid_kbd_info.lalt=hid_kbd_info.lgui=0;
  hid_kbd_info.rctrl=hid_kbd_info.rshift=0;
  hid_kbd_info.ralt=hid_kbd_info.rgui=0;
  hid_kbd_info.state=HKST_INVALID;
  for(x=0; x< sizeof(hid_kbd_info.in_report_data); x++)
  {
    hid_kbd_info.in_report_data[x]=0;
  }
  for(x=0; x< sizeof(hid_kbd_info.out_report_data); x++)
  {
    hid_kbd_info.out_report_data[x]=0;
  }

  if (hid_set_idle(&hid_kbd_info.hid_info, 0,0)==USBH_SUCCESS)
  {
    if (hid_get_report(&hid_kbd_info.hid_info, 0, hid_kbd_info.in_report_data, sizeof(hid_kbd_info.in_report_data))==0)
    {
      hid_kbd_in_convert();
      hid_kbd_info.state=HKST_ACTIVE;
      hid_kbd_info.hid_kbd_device=1;
      return hid_kbd_receive_init();
    }
  }
  return USBH_ERR_NOT_AVAILABLE;
}


/*****************************************************************************
 * Name:
 *    hid_kbd_disconnect
 * In:
 *    Device handler
 * Out:
 *    erorr code
 *
 * Description:
 *    Disconnect mouse driver.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
int hid_kbd_disconnect (void *dev_hdl)
{
  hid_kbd_info.hid_kbd_device=0;
  return 0;
}


/*****************************************************************************
 * Name:
 *    hid_kbd_set_leds
 * In:
 *    num    - required status of num lock LED (0 off , 1 on)
 *    caps   - required status of CAPS lock LED (0 off , 1 on)
 *    scroll - required status of scroll lock LED (0 off , 1 on)
 *    compo  - required status of compose lock LED (0 off , 1 on)
 *    kana   - required status of kana lock LED (0 off , 1 on)
 * Out:
 *    -
 *
 * Description:
 *    Set status of LEDS on the keyboard.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
void hid_kbd_set_leds(hcc_u8 num, hcc_u8 caps, hcc_u8 scroll, hcc_u8 compo, hcc_u8 kana)
{
  /* update report buffer */ 
  write_item((void*)(out_report.items+0), hid_kbd_info.out_report_data, 0, (hcc_s32)(num ? 1 : 0));
  write_item((void*)(out_report.items+1), hid_kbd_info.out_report_data, 0, (hcc_s32)(caps ? 1 : 0));
  write_item((void*)(out_report.items+2), hid_kbd_info.out_report_data, 0, (hcc_s32)(scroll ? 1 : 0));
  write_item((void*)(out_report.items+3), hid_kbd_info.out_report_data, 0, (hcc_s32)(compo ? 1 : 0));
  write_item((void*)(out_report.items+4), hid_kbd_info.out_report_data, 0, (hcc_s32)(kana ? 1 : 0));
  
  /* send report to device */
  hid_set_report(&hid_kbd_info.hid_info, 0, hid_kbd_info.out_report_data, 1);

}
/*****************************************************************************
 * Name:
 *    hid_kbd_in_convert
 * In:
 *    -
 * Out:
 *    -
 *
 * Description:
 *    Update stored values with values got in the last report.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
static void hid_kbd_in_convert(void)
{
  hcc_u8 *modifiers[]= {
      &hid_kbd_info.lctrl, &hid_kbd_info.lshift, &hid_kbd_info.lalt
      ,&hid_kbd_info.lgui, &hid_kbd_info.rctrl, &hid_kbd_info.rshift
      , &hid_kbd_info.ralt, &hid_kbd_info.rgui  
  };
  int x;
  
  for(x=0; x<sizeof(modifiers)/sizeof(modifiers[0]); x++)
  {
    hcc_s32 tmp;
    if (!read_item((void*)(in_report.items+x), hid_kbd_info.in_report_data, 0, &tmp))
    {
      *modifiers[x]=(hcc_u8)(tmp ? 1 : 0);
    }
  }

  for(x=0; x < sizeof(hid_kbd_info.keys); x++)
  {
    hcc_s32 tmp;
    if (!read_item((void*)(in_report.items+9), hid_kbd_info.in_report_data, (hcc_u8)x, &tmp))
    {
      hid_kbd_info.keys[x]=(hcc_u8)tmp;
    }    
  }
}

/*****************************************************************************
 * Name:
 *    hid_kbd_get_lshift
 * In:
 *    -
 * Out:
 *    0 - released
 *    1 - pressed
 *
 * Description:
 *    Get status of left shift.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_lshift(void)
{
  return(hid_kbd_info.lshift);
}
/*****************************************************************************
 * Name:
 *    hid_kbd_get_rshift
 * In:
 *    -
 * Out:
 *    0 - released
 *    1 - pressed
 *
 * Description:
 *    Get status of right shift.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_rshift(void)
{
  return(hid_kbd_info.rshift);
}
/*****************************************************************************
 * Name:
 *    hid_kbd_get_ralt
 * In:
 *    -
 * Out:
 *    0 - released
 *    1 - pressed
 *
 * Description:
 *    Get status of rigth alt.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_ralt(void)
{
  return(hid_kbd_info.ralt);
}
/*****************************************************************************
 * Name:
 *    hid_kbd_get_lalt
 * In:
 *    -
 * Out:
 *    0 - released
 *    1 - pressed
 *
 * Description:
 *    Get status of left alt.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_lalt(void)
{
  return(hid_kbd_info.lalt);
}
/*****************************************************************************
 * Name:
 *    hid_kbd_get_rctrl
 * In:
 *    -
 * Out:
 *    0 - released
 *    1 - pressed
 *
 * Description:
 *    Get status of right control.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_rctrl(void)
{
  return(hid_kbd_info.rctrl);
}
/*****************************************************************************
 * Name:
 *    hid_kbd_get_lctrl
 * In:
 *    -
 * Out:
 *    0 - released
 *    1 - pressed
 *
 * Description:
 *    Get status of left control.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_lctrl(void)
{
  return(hid_kbd_info.lctrl);
}
/*****************************************************************************
 * Name:
 *    hid_kbd_get_rgui
 * In:
 *    -
 * Out:
 *    0 - released
 *    1 - pressed
 *
 * Description:
 *    Get status of right gui.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_rgui(void)
{
  return(hid_kbd_info.rgui);
}
/*****************************************************************************
 * Name:
 *    hid_kbd_get_lgui
 * In:
 *    -
 * Out:
 *    0 - released
 *    1 - pressed
 *
 * Description:
 *    Get status of left gui.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_lgui(void)
{
  return(hid_kbd_info.lgui);
}
/*****************************************************************************
 * Name:
 *    hid_kbd_get_nkey
 * In:
 *    -
 * Out:
 *    Number of keys pressed.
 *
 * Description:
 *    Return the number of pressed keys.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_nkey(void)
{
  hcc_u8 nkey;
  
  for(nkey=0; nkey < sizeof(hid_kbd_info.keys)/sizeof(hid_kbd_info.keys[0]); nkey++)
  {
    if (hid_kbd_info.keys[nkey] == 0
        || hid_kbd_info.keys[nkey] == 1)
    {        
      break;
    }
  }
  return(nkey);
}
/*****************************************************************************
 * Name:
 *    hid_kbd_get_key
 * In:
 *    Index of pressed key.
 * Out:
 *    Scan code.
 *
 * Description:
 *    Return the scan code of ndx th pressed key.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_key(hcc_u8 ndx)
{
  return(hid_kbd_info.keys[ndx]);
}


