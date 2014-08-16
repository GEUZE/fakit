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

#ifndef _EHCI_H
#define _EHCI_H

#ifdef __cplusplus
extern "C" {
#endif


#include <hcc/hcc_types.h>
#include <hcc/eusbh/usb_driver/usb_host.h>

#include <hcc/eusbh/os.h>
#include <ehci_hw.h>

#define EHCI_DENDIAN   0 /* opposite endianness between EHCI and the system */

#define MAX_DEVICE     3 /* maximum number of devices supported */
#define MAX_EP	       6 /* max. number of EPs bulk, isochronous, interrupt */
#define MAX_TRANSFERS  6 /* number of maximum simultaneous transfers */
#define EHCI_PERIODIC_LIST_SIZE	256 /* periodic list size, 0 - means INT transfers are not allowed */

#define EHCI_MUTEX_COUNT 1		/* bulk/control transfer */
#define EHCI_EVENT_COUNT 2		/* transfer complete/error */

#define EHCI_HOST_ISR			0x12001200

int ehci_init (void);
int ehci_start (hcc_u32 itc);
int ehci_stop (void);

void *ehci_add_ep (hcc_u8 daddr, hcc_u8 ep, hcc_u8 type, hcc_u8 dir, hcc_u16 psize, hcc_u8 interval);
int ehci_del_daddr (hcc_u8 daddr);
int ehci_clear_toggle (void *ep_handle);

int ehci_transfer (t_usbh_transfer *uht, hcc_u8 mode);
int ehci_transfer_status (t_usbh_transfer *uht, hcc_u8 mode);

int ehci_set_ep0_speed_hub (void *ep0_handle, hcc_u8 speed, hcc_u8 hub_daddr, hcc_u8 hub_speed);
void ehci_delay (hcc_u32 ms);
int ehci_get_ep_inf (void *ep_handle, hcc_u8 *daddr, hcc_u8 *ep, hcc_u8 *type, hcc_u8 *dir, hcc_u16 *psize);

OS_TASK_FN(ehci_transfer_task);
OS_ISR_FN(ehci_isr_handler);

#ifdef __cplusplus
}
#endif

#endif
