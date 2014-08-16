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
#include <hcc/eusbh/usb_driver/ehci/ehci_hub.h>
#include <hcc/eusbh/usb_driver/ehci/ehci_reg.h>
#include <hcc/eusbh/usb_driver/pmgr/pmgr.h>
#include "ehci_hw.h"


static void *hub_handler;


/*
** Suspend port
*/
static int _ehci_hub_port_suspend (hcc_u8 port)
{
  _sb32(PORTSC(port),SUSP);			/* suspend port */
  return USBH_SUCCESS;
}

/*
** Resume port
*/
static int _ehci_hub_port_resume (hcc_u8 port)
{
  _sb32(PORTSC(port),FPR);			/* force port resume*/
  ehci_delay(20);					/* delay 20 ms */
  _cb32(PORTSC(port),FPR);			/* force port resume*/
  while (_gb32(PORTSC(port),SUSP))	/* wait while in suspended state */
    ;
  return USBH_SUCCESS;
}


/*
** Reset EHCI port
*/
static int ehci_hub_port_reset (void *dev_hdl, hcc_u8 port)
{
  hcc_u32 sp;
  ehci_delay(200);					/* wait 200ms to stabilize power */
  _cb32(PORTSC(port),PE);			/* disable port */
  _sb32(PORTSC(port),PR);			/* reset port */
  while (_gb32(PORTSC(port),PR))	/* wait reset to complete */
    ;
  _r32(sp,PORTSC(port));
  if (sp&PE)						/* port has to be enabled after reset */
  {
    ehci_delay(100);				/* wait 100ms after reset */
    _r32(sp,PORTSC(port));
    sp=_rF(sp,LS);
    if (sp==1) return USBH_LOW_SPEED;
    else if (sp==2) return USBH_FULL_SPEED;
    else if (sp==0) return USBH_HIGH_SPEED;
  }
  return USBH_SPEED_ERROR;
}

/*
** Disable EHCI port
*/
static int ehci_hub_port_disable (void *dev_hdl, hcc_u8 port)
{
  _cb32(PORTSC(port),PE);			/* disable port */
  return USBH_SUCCESS;
}

/*
** Suspend port
*/
static int ehci_hub_suspend (void *dev_hdl, hcc_u8 port)
{
  int rc=0;
  if (port==(hcc_u8)-1)
  {
    hcc_u8 i;
    for (i=1;i<_rF(_EHCI_V32(HCSPARAMS),N_PORTS)+1;i++)
    {
      rc=_ehci_hub_port_suspend(i);
      if (rc) return rc;
    }
    _cb32(USBCMD,RS);									/* clear run bit */
    ehci_hw_suspend();
  }
  else
  {
    rc=_ehci_hub_port_suspend(port);
  }
  return rc;
}

/*
** Resume port
*/
static int ehci_hub_resume (void *dev_hdl, hcc_u8 port)
{
  int rc=0;
  if (port==(hcc_u8)-1)
  {
    hcc_u8 i;
    ehci_hw_resume();
    _sb32(USBCMD,RS);									/* set run bit */
    for (i=1;i<_rF(_EHCI_V32(HCSPARAMS),N_PORTS)+1;i++)
    {
      rc=_ehci_hub_port_resume(i);
      if (rc) return rc;
    }
  }
  else
  {
    rc=_ehci_hub_port_resume(port);
  }
  return rc;
}

/*
** Idle routine which needs to be called sequencially or should be an interrupt.
** Check device connect/disconnect.
*/
static int ehci_hub_scan (void *dev_hdl, t_hub_scan_inf *hsi)
{
  int rc=0;
  hcc_u8 i;

  for (i=1;i<_rF(_EHCI_V32(HCSPARAMS),N_PORTS)+1;i++)
  {
    hcc_u32 st;
    _r32(st,PORTSC(i));		/* get port state */
    if (st&(CSC|OCC))
    {
      hsi->state=0;
      hsi->port=i;

      if (st&CSC)			/* connection status changed */
      {
        if (st&CCS) hsi->state=USBH_STATE_CONNECTED;
               else hsi->state=USBH_STATE_DISCONNECTED;
      }

      if (st&OCC)			/* overcurrent indication */
      {
        hsi->state=USBH_STATE_OVERCURRENT;
      }

      ++rc;
      ++hsi;

      _w32(PORTSC(i),st);
    }
    else if (st&FPR)		/* check resume condition */
    {
      (void)ehci_hub_resume(dev_hdl,i);		/* resume port */
    }
  }

  if (ehci_hw_state()==USBH_ERR_SUSPENDED)	/* restart EHCI module if in suspended state */
  {
    ehci_hw_resume();
    _sb32(USBCMD,RS);						/* set run bit */
  }

  return rc;
}


int ehci_hub_init (void)
{
  hcc_u8 i;
  for (i=1;i<_rF(_EHCI_V32(HCSPARAMS),N_PORTS)+1;i++)
  {
    /*_cb32(PORTSC(i),(1<<28));*/
    _cb32(PORTSC(i),PP);
  }
  hub_handler=pmgr_add_hub(NULL, ehci_hub_scan, ehci_hub_port_reset, ehci_hub_port_disable, ehci_hub_suspend, ehci_hub_resume);
  if (hub_handler==NULL) return USBH_ERR_HOST;
  return USBH_SUCCESS;
}


int ehci_hub_start (void)
{
  hcc_u8 i;
  for (i=1;i<_rF(_EHCI_V32(HCSPARAMS),N_PORTS)+1;i++)
  {
    _sb32(PORTSC(i),PP);
  }
  return USBH_SUCCESS;
}


int ehci_hub_stop (void)
{
  hcc_u8 i;
  for (i=1;i<_rF(_EHCI_V32(HCSPARAMS),N_PORTS)+1;i++)
  {
    _cb32(PORTSC(i),PP);
  }
  return USBH_SUCCESS;
}

