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

#ifndef _OHCI_H
#define _OHCI_H

#include <hcc/hcc_types.h>
#include <hcc/eusbh/usb_driver/pmgr/pmgr.h>
#include "ohci_hw.h"
#include <hcc/eusbh/os.h>

#define OHCI_BUFFER_ADDRESS		0
#define OHCI_BUFFER_MAX_SIZE	0

#define OHCI_MAX_PORTS			2		/* maximum number of ports in OHCI */

// defined in usb_host_def.h
#define MAX_DEVICE				6		/* maximum number of devices supported */
#define MAX_EP					12		/* max. number of endpoints bulk, isochronous, interrupt */
#define MAX_TRANSFERS			8		/* number of maximum simultaneous transfers */
#define MAX_PACKET_SIZE			64		/* maximum size of a packet */

#define MAX_TEMP_BUFFERS		MAX_DEVICE		/* maximum number of temporary buffers (invalid if OHCI_BUFFER_ADDRESS is not 0) */

#define OHCI_MUTEX_COUNT		1		/* bulk/control transfer */
#define OHCI_EVENT_COUNT		1		/* transfer complete */

#define OHCI_HOST_ISR			0x06C106C1

int ohci_init (void);
int ohci_start (hcc_u32 itc);
int ohci_stop (void);

void *ohci_add_ep (hcc_u8 daddr, hcc_u8 ep, hcc_u8 type, hcc_u8 dir, hcc_u16 psize, hcc_u8 interval);
int ohci_del_daddr (hcc_u8 daddr);
int ohci_clear_toggle (void *ep_handle);

int ohci_transfer (t_usbh_transfer *uht, hcc_u8 mode);
int ohci_transfer_status (t_usbh_transfer *uht, hcc_u8 mode);

int ohci_set_ep0_speed_hub (void *ep0_handle, hcc_u8 speed, hcc_u8 hub_daddr, hcc_u8 hub_speed);
void ohci_delay (hcc_u32 ms);
int ohci_get_ep_inf (void *ep_handle, hcc_u8 *daddr, hcc_u8 *ep, hcc_u8 *type, hcc_u8 *dir, hcc_u16 *psize);

OS_TASK_FN(ohci_transfer_task);
OS_ISR_FN(ohci_isr_handler);

#endif


