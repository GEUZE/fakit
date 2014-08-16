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
#ifndef _HOST_HID_EXPLORE_H_
#define _HOST_HID_EXPLORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hcc/eusbh/host_hid/host_hid.h>
#include "host_hid_explore_def.h"
#if 0 /* moved to host_hid_explore_def.h */
#define HID_REPORT_DESCRIPTOR_MAX_SIZE	128
#define MAX_NO_OF_REPORTS				1
#define MAX_EXPLORE_CB					4
#endif

typedef void (*t_hid_explore_cb) (hcc_u16 usage, hcc_s32 value);

int hid_explore_init(void);
void *hid_explore_get_hdl (void);
int hid_explore_connect(void *dev_hdl);
int hid_explore_disconnect(void *dev_hdl);
int hid_explore_report_size (void *dev_hdl, hcc_u16 report_size);
int hid_explore_get_value (hcc_u8 usage_page, hcc_u8 usage, hcc_s32 *value);

#ifdef __cplusplus
}
#endif

#endif

