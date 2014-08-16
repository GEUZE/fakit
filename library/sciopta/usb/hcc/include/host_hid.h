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
#ifndef _HOST_HID_H_
#define _HOST_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hcc/hcc_types.h>
#include <hcc/eusbh/usb_driver/pmgr/pmgr.h>
#include "host_hid_def.h"
#if 0 /* moved to host_hid_def.h */
#define HID_MOUSE_ENABLE	1	/* set to 0 if mouse is not required */
#define HID_KBD_ENABLE		1	/* set to 0 if keyboard is not required */
#define HID_EXPLORE_ENABLE	0	/* set to 0 if explore disable */ 
#endif

#define HIDTYPE_UNKNOWN	0
#define HIDTYPE_KBD		1
#define HIDTYPE_MOUSE	2
#define HIDTYPE_JOY		3


#define EP0		0
#define IN_IT	1
#define OUT_IT	2
typedef struct {
  hcc_u8 boot;         /* type of boot-protocol device talks */
  hcc_u8 type;         /* HID type */
  hcc_u8 ifc_ndx;      /* interface index */
  void *ep_hdl[3];
} hid_info_t;

typedef void (*t_hid_cb) (void);

int hid_init (void);
hcc_uint hid_get_report_descriptor(hid_info_t *hid_info, hcc_u8 *buffer, hcc_u16 length);
hcc_uint hid_set_report(hid_info_t *hid_info, hcc_u8 report_id, hcc_u8 *buffer, hcc_u16 length);
hcc_uint hid_get_report(hid_info_t *hid_info, hcc_u8 report_id, hcc_u8 *buffer, hcc_u16 length);
int hid_set_protocol(hid_info_t *hid_info, hcc_u8 boot);
int hid_set_idle(hid_info_t *hid_info, hcc_u8 dur, hcc_u8 report_id);

#ifdef __cplusplus
}
#endif

#endif

