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
#include <hcc/hcc_types.h>
#include <hcc/eusbh/usb_driver/ohcib/ohci_reg.h>
#include <hcc/eusbh/usb_driver/pmgr/pmgr.h>
static void *hub_handler;


/*
** Reset OHCI port
*/
static int ohci_hub_port_reset (void *dev_hdl, hcc_u8 port)
{
  ohci_delay(100);			/* wait 100ms to stabilize power */
  HcRhPortStatus(port)=PRS;	/* reset port */
  while ((HcRhPortStatus(port)&PRS));
  HcRhPortStatus(port)=PRSC;
  ohci_delay(400);
  if (HcRhPortStatus(port)&LSDA) return USBH_LOW_SPEED;
  return USBH_FULL_SPEED;
}

/*
** Disable OHCI port
*/
static int ohci_hub_port_disable (void *dev_hdl, hcc_u8 port)
{
  HcRhPortStatus(port)=CCS;	/* power off and disable port */
  HcRhPortStatus(port)=PESC;
  return 0;
}

/*
** Idle routine which needs to be called sequencially or should be an interrupt.
** Check device connect/disconnect.
*/
static int ohci_hub_scan (void *dev_hdl, t_hub_scan_inf *hsi)
{
  int rc=0;
  hcc_u8 i;

  for (i=1;i<(HcRhDescriptorA&NDP)+1;i++)
  {
    hcc_u32 st=HcRhPortStatus(i);	/* get port state */
    if (st&(CSC|POCIC))
    {
      hsi->state=0;
      hsi->port=i;
      if (st&CSC)					/* connection status changed */
      {
        if (st&CCS) hsi->state=USBH_STATE_CONNECTED;
               else hsi->state=USBH_STATE_DISCONNECTED;
        HcRhPortStatus(i)=CSC;
      }

      if (st&POCIC)	/* overcurrent indication */
      {
        hsi->state=USBH_STATE_OVERCURRENT;
        HcRhPortStatus(i)=POCIC;
      }

      ++rc;
      ++hsi;
    }
  }

  return rc;
}


int ohci_hub_init (void)
{
  HcRhDescriptorA=(NOCP|OCPM|NPS|_POTPGT(50));
  HcRhDescriptorB=0;								/* per port switching to all ports */
  HcInterruptStatus=RHSC;

  hub_handler=pmgr_add_hub(NULL, ohci_hub_scan, ohci_hub_port_reset, ohci_hub_port_disable, NULL, NULL);
  if (hub_handler==NULL) return 1;
  return 0;
}


