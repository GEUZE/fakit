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
#ifndef _HOST_HID_MOUSE_H_
#define _HOST_HID_MOUSE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hcc/eusbh/hid/host_hid.h>

int hid_mouse_init(t_hid_cb cb);
void *hid_mouse_get_hdl (void);
int hid_mouse_connect(void *dev_hdl);
int hid_mouse_disconnect(void *dev_hdl);
hcc_u16 hid_mouse_get_x(void);
hcc_u16 hid_mouse_get_y(void);
hcc_u8 hid_mouse_get_button(hcc_u8 b);

#ifdef __cplusplus
}
#endif

#endif

