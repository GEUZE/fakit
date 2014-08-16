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
#ifndef _HOST_HID_KBD_H_
#define _HOST_HID_KBD_H_

#include <hcc/eusbh/hid/host_hid.h>

int hid_kbd_init(t_hid_cb cb);
void *hid_kbd_get_hdl (void);
int hid_kbd_connect(void *dev_hdl);
int hid_kbd_disconnect(void *dev_hdl);
void hid_kbd_set_leds(hcc_u8 num, hcc_u8 caps, hcc_u8 scroll, hcc_u8 compo, hcc_u8 kana);
int hid_kbd_process(void);
hcc_u8 hid_kbd_get_lshift(void);
hcc_u8 hid_kbd_get_rshift(void);
hcc_u8 hid_kbd_get_ralt(void);
hcc_u8 hid_kbd_get_lalt(void);
hcc_u8 hid_kbd_get_rctrl(void);
hcc_u8 hid_kbd_get_lctrl(void);
hcc_u8 hid_kbd_get_rgui(void);
hcc_u8 hid_kbd_get_lgui(void);
hcc_u8 hid_kbd_get_nkey(void);
hcc_u8 hid_kbd_get_key(hcc_u8 ndx);

#endif
/****************************** END OF FILE **********************************/
