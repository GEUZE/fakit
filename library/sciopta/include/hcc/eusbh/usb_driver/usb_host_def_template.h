/***************************************************************************
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
#ifndef _USB_HOST_DEF_H_
#define _USB_HOST_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/* #define	OHCI */
/* #define	OHCIB */
/* #define	EHCI */
/* #define	ISP1561 */
/* #define	ISP1161 */
/* #define	ISP1362 */
/* #define	ISP176x */
/* #define	MAX3421 */
/* #define	LM3S3768 */

#include "pmgr_def.h"

#if defined OHCI || defined OHCIB

  #ifdef OHCI
    #include  <hcc/eusbh/usb_driver/ohci/ohci.h>
  #else
    #include  <hcc/eusbh/usb_driver/ohcib/ohci.h>
  #endif

  #define USBH_MUTEX_COUNT			OHCI_MUTEX_COUNT+PMGR_MUTEX_COUNT
  #define USBH_EVENT_COUNT			OHCI_EVENT_COUNT+PMGR_EVENT_COUNT

  #define _usbhd_init				ohci_init
  #define _usbhd_start				ohci_start
  #define _usbhd_stop				ohci_stop
  #define _usbhd_add_ep				ohci_add_ep
  #define _usbhd_del_daddr			ohci_del_daddr
  #define _usbhd_clear_toggle		ohci_clear_toggle
  #define _usbhd_transfer			ohci_transfer
  #define _usbhd_transfer_status	ohci_transfer_status
  #define _usbhd_set_ep0_speed_hub	ohci_set_ep0_speed_hub
  #define _usbhd_delay				ohci_delay
  #define _usbhd_get_ep_inf			ohci_get_ep_inf
  #define _usbhd_transfer_task		ohci_transfer_task

#elif defined EHCI || defined ISP1561

  #ifdef EHCI
    #include  <hcc/eusbh/usb_driver/ehci/ehci.h>
  #else
    #include  <hcc/eusbh/usb_driver/isp1561/ehci.h>
  #endif

  #define USBH_MUTEX_COUNT			EHCI_MUTEX_COUNT+PMGR_MUTEX_COUNT
  #define USBH_EVENT_COUNT			EHCI_EVENT_COUNT+PMGR_EVENT_COUNT

  #define _usbhd_init				ehci_init
  #define _usbhd_start				ehci_start
  #define _usbhd_stop				ehci_stop
  #define _usbhd_add_ep				ehci_add_ep
  #define _usbhd_del_daddr			ehci_del_daddr
  #define _usbhd_clear_toggle		ehci_clear_toggle
  #define _usbhd_transfer			ehci_transfer
  #define _usbhd_transfer_status	ehci_transfer_status
  #define _usbhd_set_ep0_speed_hub	ehci_set_ep0_speed_hub
  #define _usbhd_delay				ehci_delay
  #define _usbhd_get_ep_inf			ehci_get_ep_inf
  #define _usbhd_transfer_task		ehci_transfer_task

#elif defined ISP1161 || defined ISP1362 || defined ISP176x

  #ifdef ISP1161
    #include  <hcc/eusbh/usb_driver/isp1161/isp1161.h>
    #define USBH_MUTEX_COUNT		ISP1161_MUTEX_COUNT+PMGR_MUTEX_COUNT
    #define USBH_EVENT_COUNT		ISP1161_EVENT_COUNT+PMGR_EVENT_COUNT
  #elif defined ISP1362
    #include  <hcc/eusbh/isp1362/isp1362.h>
    #define USBH_MUTEX_COUNT		ISP1362_MUTEX_COUNT+PMGR_MUTEX_COUNT
    #define USBH_EVENT_COUNT		ISP1362_EVENT_COUNT+PMGR_EVENT_COUNT
  #else
    #include  <hcc/eusbh/usb_driver/isp176x/isp176x.h>
    #define USBH_MUTEX_COUNT		ISP176x_MUTEX_COUNT+PMGR_MUTEX_COUNT
    #define USBH_EVENT_COUNT		ISP176x_EVENT_COUNT+PMGR_EVENT_COUNT
  #endif

  #define _usbhd_init				isp_init
  #define _usbhd_start				isp_start
  #define _usbhd_stop				isp_stop
  #define _usbhd_add_ep				isp_add_ep
  #define _usbhd_del_daddr			isp_del_daddr
  #define _usbhd_clear_toggle		isp_clear_toggle
  #define _usbhd_transfer			isp_transfer
  #define _usbhd_transfer_status	isp_transfer_status
  #define _usbhd_set_ep0_speed_hub	isp_set_ep0_speed_hub
  #define _usbhd_delay				isp_delay
  #define _usbhd_get_ep_inf			isp_get_ep_inf
  #define _usbhd_transfer_task		isp_transfer_task

#elif defined MAX3421

  #include  <hcc/eusbh/usb_driver/max3421/max3421.h>

  #define USBH_MUTEX_COUNT			MAX3421_MUTEX_COUNT+PMGR_MUTEX_COUNT
  #define USBH_EVENT_COUNT			MAX3421_EVENT_COUNT+PMGR_EVENT_COUNT

  #define _usbhd_init				max3421_init
  #define _usbhd_start				max3421_start
  #define _usbhd_stop				max3421_stop
  #define _usbhd_add_ep				max3421_add_ep
  #define _usbhd_del_daddr			max3421_del_daddr
  #define _usbhd_clear_toggle		max3421_clear_toggle
  #define _usbhd_transfer			max3421_transfer
  #define _usbhd_transfer_status	max3421_transfer_status
  #define _usbhd_set_ep0_speed_hub	max3421_set_ep0_speed_hub
  #define _usbhd_delay				max3421_delay
  #define _usbhd_get_ep_inf			max3421_get_ep_inf
  #define _usbhd_transfer_task		max3421_transfer_task

#elif defined LM3S3768

  #include  <hcc/eusbh/usb_driver/lm3s3768/lms.h>
  
  #define USBH_MUTEX_COUNT			LMS_MUTEX_COUNT+PMGR_MUTEX_COUNT
  #define USBH_EVENT_COUNT			LMS_EVENT_COUNT+PMGR_EVENT_COUNT
  
  #define _usbhd_init				lms_init
  #define _usbhd_start				lms_start
  #define _usbhd_stop				lms_stop
  #define _usbhd_add_ep				lms_add_ep
  #define _usbhd_del_daddr			lms_del_daddr
  #define _usbhd_clear_toggle		lms_clear_toggle
  #define _usbhd_transfer			lms_transfer
  #define _usbhd_transfer_status	lms_transfer_status
  #define _usbhd_set_ep0_speed_hub	lms_set_ep0_speed_hub
  #define _usbhd_delay				lms_delay
  #define _usbhd_get_ep_inf			lms_get_ep_inf
  #define _usbhd_transfer_task		lms_transfer_task

#else

  #error NO HOST CONTROLLER SELECTED!

#endif

#ifdef __cplusplus
}
#endif

#endif

