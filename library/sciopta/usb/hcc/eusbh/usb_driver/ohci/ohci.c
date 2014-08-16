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
/*+++ SCIOPTA */
#include <sciopta.h>
#include <hcc/eusbh/usb_driver/ohcib/ohci_reg.h>
#include <hcc/eusbh/usb_driver/ohcib/ohci_hub.h>
#include "usb_host_def.h"
#include <hcc/eusbh/usbh.msg>
union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  hcc_usbhIrq_t irq;
};
/*--- SCIOPTA */

#if OHCI_BUFFER_ADDRESS==0
#define MAX_BUFFERS			MAX_TEMP_BUFFERS	/* if no OHCI area can be anywhere then only MAX_TEMP_BUFFERS needed */
#else
#define MAX_BUFFERS			MAX_TRANSFERS		/* if OHCI has to be in a fixed area then every transfer needs temp. area */
#endif


#define CONTROL_EP			(MAX_DEVICE+1)		/* 1 for every EP and a general 0 address EP */
#define TOTAL_EP			(MAX_EP+CONTROL_EP)	/* total number of endpoints */

#define ED_SIZE				(TOTAL_EP*16)
#define TD_SIZE				(MAX_TRANSFERS*16)
#define HCCA_SIZE			(256)

#define OHCI_BUFFER_SIZE	(ED_SIZE+TD_SIZE+HCCA_SIZE+(MAX_BUFFERS*MAX_PACKET_SIZE))

#if OHCI_BUFFER_ADDRESS && (OHCI_BUFFER_SIZE>OHCI_BUFFER_MAX_SIZE)
  #error Required OHCI buffer size is bigger than available (try decreasing MAX_TRANSFERS)
#endif

/* Completion Codes */
#define C_NoError				0x0
#define C_CRC					0x1
#define C_BitStuffing			0x2
#define C_DataToggleMismatch	0x3
#define C_Stall					0x4
#define C_DeviceNotResponding	0x5
#define C_PIDCheckFailure		0x6
#define C_UnexpectedPID			0x7
#define C_DataOverrun			0x8
#define C_DataUnderrun			0x9
#define C_BufferOverrun			0xc
#define C_BufferUnderrun		0xd

#define _d_setup				0
#define _d_out					1
#define _d_in					2
#define _d_from_td				3

#define OHCI_TRST_STOPPED		0
#define OHCI_TRST_RUNNING		1
#define OHCI_TRST_SHORT_PACKET	2
#define OHCI_TRST_CB			3
#define OHCI_TRST_ERROR			4
#define OHCI_TRST_SUCCESS		5


/*
** Temporary buffer area, used when the requested amount of data is less than
** the maximum packet size on the endpoint
*/
typedef struct {
  hcc_u8 *buf;					/* source pointer */
  hcc_u8 *dptr;					/* destination pointer */
  hcc_u8 used;					/* used buffer */
} t_buf_dsc;


/*
** ED descriptor for additional information for the endpoint
*/
typedef struct {
  hcc_u8 used;					/* used EP descriptor */

  hcc_u8 type;					/* EP type */
  hcc_u8 dir;					/* EP direction */
  hcc_u8 interval;				/* request interval */
  hcc_u16 psize;				/* maximum packet size */

  hcc_u16 td_cnt;				/* number of TDs attached */
  hcc_u32 csize;				/* completed size */
  t_usbh_transfer *uht;			/* tranfer descriptor */
  volatile hcc_u8 tstate;		/* transfer state */
} t_ed_dsc;


/*
** TD descriptor, stores temporary information for the TD.
*/
typedef struct {
  t_ed_dsc *ed_dsc;				/* ED dsc this TD is attached to */

#if OHCI_BUFFER_ADDRESS==0
  void *taddr;					/* temporary address for transactions */
#endif
  t_buf_dsc *tbuf_dsc;			/* temporary buffer, if used */

  hcc_u16 timeout;				/* timeout of TD */
  hcc_u32 frame;				/* last frame number (for timeout) */

  hcc_u16 req_size;				/* originally requested size */
  hcc_u8 dir;
  hcc_u8 used;					/* used TD descriptor */
} t_td_dsc;


/************************************************************************************************************************/
/********************************************** Local variables *********************************************************/
/************************************************************************************************************************/
/***** OHCI variables *****/
#if OHCI_BUFFER_ADDRESS==0											/* allocate OHCI area if it can be anywhere */
static volatile hcc_u8 _ohci_buffer[OHCI_BUFFER_SIZE+0xff];			/* +255 to ensure 256 byte alignment */
static hcc_u8 *ohci_buffer;
#endif
static t_hcca *hcca;												/* HCCA area */
static t_ed *ed;													/* ED area */
static t_td *td;													/* TD area */
static hcc_u8 *tbuf;												/* temporary buffer area */

static t_ed_dsc ed_dsc[TOTAL_EP];									/* endpoint descriptor area */
static t_td_dsc td_dsc[MAX_TRANSFERS];								/* transfer descriptor area */
static t_buf_dsc buf_dsc[MAX_BUFFERS];								/* maximum temporary buffers */

/***** mutexes and events *****/
static OS_MUTEX_TYPE *ohci_mutex_transfer;
//->static OS_EVENT_BIT_TYPE ohci_event_bit_transfer;
/************************************************************************************************************************/


/*
** Add all TD-s described in t_ohci_transfer
** INPUT: t_usbh_transfer - described in ohci.h
** RETURN: USBH_ERR_TRANSFER_FULL - no more entries to complete whole transfer,
**           later retry needed, check ot->csize after calling ohci_transfer_status()
**         USBH_ERR_NOT_AVAILABLE - transfer not possible, endpoint doesn't exist
**         USBH_SUCCESS - all transfers queued successfully
*/
static int ohci_add_td (t_usbh_transfer *uht)
{
  int rc=USBH_SUCCESS;
  int i,be;
  hcc_u32 tsize;
  hcc_u32 asize;
  hcc_u8 *abuf=uht->buf;
  t_ed *aed=NULL;
  t_ed_dsc *aed_dsc;
  t_td *atd, *ptd=NULL;
  t_td_dsc *atd_dsc;
  t_buf_dsc *abuf_dsc=buf_dsc;
  hcc_u8 dir = 0;

  if (uht->eph==NULL) uht->eph=(void *)ed;
  if ((t_ed *)(uht->eph)>=ed && (t_ed *)(uht->eph)<(ed+TOTAL_EP))
  {
    aed=(t_ed *)uht->eph;
    aed_dsc=ed_dsc+(aed-ed);
    *((t_ed_dsc **)uht->driver_area)=aed_dsc;
    if (aed_dsc->dir==_d_from_td)
    {
      switch (uht->dir)
      {
        case USBH_DIR_SETUP: dir=_d_setup; break;
        case USBH_DIR_IN: dir=_d_in; break;
        case USBH_DIR_OUT: dir=_d_out; break;
        default: rc=USBH_ERR_DIR;
      }
    }
    else dir=aed_dsc->dir;

    if (uht->size==USBH_SIZE_DEFAULT)
    {
      uht->size=aed_dsc->psize;
    }
  }
  else rc=USBH_ERR_NOT_AVAILABLE;

  if (rc==USBH_SUCCESS)
  {
    if (os_mutex_get(ohci_mutex_transfer)==OS_SUCCESS)
    {
      hcc_u16 mps=_rED(aed->ctrl,MPS);	/* get maximum packet size for this endpoint */

      if (aed_dsc->tstate==OHCI_TRST_STOPPED)
      {
        /* Set skip bit for this EP if not set */
        if ((aed->ctrl&_mED(K))==0)
        {
          aed->ctrl|=_mED(K);				/* set skip bit */
          HcInterruptStatus=SF;
          while (!(HcInterruptStatus&SF));	/* wait start of frame */
        }

        tsize=uht->size-uht->csize;
        aed_dsc->td_cnt=0;					/* set TD count to zero */
        i=0;
        be=0;
        atd_dsc=td_dsc;
        atd=td;
        do {
          if (i++==MAX_TRANSFERS)
          {
            rc=USBH_ERR_TRANSFER_FULL;
            break;
          }

          if (atd_dsc->used==0)				/* check if found */
          {
            hcc_u32 tc=0;

            if (tsize)
            {
#if OHCI_BUFFER_ADDRESS==0
              hcc_u32 len=tsize;
              atd_dsc->taddr=ohci_lock_buffer((void *)abuf,&len);	/* get an uncached or invalidated buffer */
              if (atd_dsc->taddr==NULL)
              {
                rc=USBH_ERR_TRANSFER_FULL;
                break;
              }
              asize=0x2000-((hcc_u32)(atd_dsc->taddr)&0xfff);
              if (asize>len) asize=len;
              if (asize!=len && (asize&(mps-1))) asize&=~(mps-1);
#else
              asize=mps;
              if (asize>tsize) asize=tsize;
#endif
            }
            else
            {
              atd_dsc->taddr=NULL;
              asize=0;
            }

            /* Fill new TD (delay interrupt,ec and cc is zero) */
            _wTD(tc,DP,dir);				/* set direction */
            if (uht->toggle!=USBH_TOGGLE_PRESERVE)
            {
              _wTD(tc,T,0x2|(uht->toggle));		/* use predefined toggle value */
              uht->toggle=USBH_TOGGLE_PRESERVE;	/* predefined value valid just for the 1st transfer */
            }

            atd->ctrl=tc;
            atd->nexttd=0;						/* no next TD */


            /* replace transfer buffer to a packet size buffer if IN transfer is initiated and the size of
               it is less than packet size (and not zero). Replace the buffer in case the OHCI HW is using
               a dedicated area for transfers */
#if OHCI_BUFFER_ADDRESS==0
            if (asize && atd_dsc->taddr!=abuf && mps>asize && dir==_d_in) 	/* if requested size < max. packet size */
#else
            if ( asize &&
                (((hcc_u32)abuf<OHCI_BUFFER_ADDRESS || (hcc_u32)abuf>(OHCI_BUFFER_ADDRESS+OHCI_BUFFER_MAX_SIZE))
                || (mps>asize && dir==_d_in)) )
#endif
            {
              for (;be<MAX_BUFFERS && abuf_dsc->used;be++, abuf_dsc++);
              if (be<MAX_TRANSFERS)				/* find temporary buffer */
              {
                atd->cbp=(hcc_u32)ohci_cached_addr((void *)tbuf)+(be*MAX_PACKET_SIZE);
#if OHCI_BUFFER_ADDRESS
                if (dir!=_d_in)
                {
                  _memcpy(ohci_uncached_addr((void *)atd->cbp),abuf,asize);
                  abuf_dsc->buf=NULL;
                }
                else
#endif
                {
                  abuf_dsc->buf=(hcc_u8 *)(atd->cbp);
                }
                abuf_dsc->dptr=abuf;
                abuf_dsc->used=1;
                atd_dsc->tbuf_dsc=abuf_dsc;		/* assign buffer */
              }
              else
              {
                rc=USBH_ERR_TRANSFER_FULL;
                break;							/* currently there are no free buffers */
              }
            }
            else
            {
#if OHCI_BUFFER_ADDRESS==0
              atd->cbp=(hcc_u32)atd_dsc->taddr;
              if (atd_dsc->taddr!=abuf && (dir==_d_setup || dir==_d_out))
              {
                _memcpy(atd_dsc->taddr,abuf,asize);
              }
#else
              atd->cbp=(hcc_u32)abuf;
#endif
              atd_dsc->tbuf_dsc=NULL;			/* if req. size == max. packet size use user buffer */
            }

            if (asize) atd->be=(atd->cbp)+asize-1;
                  else atd->be=0;

            atd_dsc->ed_dsc=aed_dsc;			/* save ED dsc belonging to this transfer */
            atd_dsc->req_size=asize;			/* store request length */
            atd_dsc->frame=0;					/* reset timeout frame */
            atd_dsc->used=1;					/* set used flag */
            atd_dsc->timeout=uht->timeout;		/* save timeout */
            aed_dsc->csize=0;					/* clear completed size for this EP */
            atd_dsc->dir=dir;					/* save direction of the transfer */

            /* Add new TD to TD list */
            {
              void *patd=ohci_cached_addr((void *)atd);
              if (ptd) _wADDR(ptd->nexttd,patd);
                  else _wADDR(aed->headp,patd);
            }

            ptd=atd;							/* store current TD */
            tsize-=asize;						/* calculate remaining transfer size */
            abuf+=asize;						/* calculate new buffer pointer */
            ++aed_dsc->td_cnt;					/* increase number of TD-s for this ED */

            rc=USBH_SUCCESS;
          }

          ++atd_dsc;
          ++atd;
        } while ((tsize || aed_dsc->td_cnt==0) && rc==USBH_SUCCESS);

        /* start tranfer if successful TD-s were added or if zero length packet */
        if (uht->size!=tsize || (uht->size==0 && rc==USBH_SUCCESS))
        {
          aed_dsc->uht=uht;
          aed_dsc->tstate=OHCI_TRST_RUNNING;

          aed->ctrl&=~_mED(K);		/* clear skip bit */
          aed->headp&=~_mED(H);		/* clear halt bit */
          if (aed_dsc->type==USBH_TYPE_CONTROL) HcCommandStatus|=CLF;
          if (aed_dsc->type==USBH_TYPE_BULK) HcCommandStatus|=BLF;
        }
      }
      else rc=USBH_ERR_ONGOING_TRANSFER;
      (void)os_mutex_put(ohci_mutex_transfer);
    }
    else rc=USBH_ERR_RESOURCE;
  }

  return rc;
}



/*
** Remove transfer descriptors from and ED
** INPUT: aed - ED pointer
**        m - need mutex
** RETURN: USBH_SUCCESS
*/
static int ohci_del_td (t_ed *aed, hcc_u8 m)
{
  int rc=USBH_SUCCESS;
  unsigned int i;
  t_td_dsc *atd_dsc;
  t_ed_dsc *aed_dsc;

  aed=ohci_uncached_addr((void *)aed);
  aed_dsc=ed_dsc+(aed-ed);

  if (m)
  {
    rc=os_mutex_get(ohci_mutex_transfer);
  }

  if (rc==USBH_SUCCESS)
  {
    aed->ctrl|=_mED(K);					/* set skip bit */
    for (i=0,atd_dsc=td_dsc;i<MAX_TRANSFERS;i++,atd_dsc++)
    {
      if (atd_dsc->used && atd_dsc->ed_dsc==aed_dsc)
      {
        if (atd_dsc->tbuf_dsc) atd_dsc->tbuf_dsc->used=0;
#if OHCI_BUFFER_ADDRESS==0
        if (atd_dsc->taddr) ohci_unlock_buffer(atd_dsc->taddr,0);
#endif
        atd_dsc->used=0;
      }
    }
    _wADDR(aed->headp,0);
    _wADDR(aed->tailp,0);

    if (m)
    {
      (void)os_mutex_put(ohci_mutex_transfer);
    }
  }

  return rc;
}


/*
** Add endpoint.
** INPUT: daddr - device address
**        ep - endpoint number
**        type - USBH_TYPE_CONTROL / USBH_TYPE_BULK / USBH_TYPE_INT / USBH_TYPE_ISO
**        dir - USBH_DIR_ANY / USBH_DIR_SETUP / USBH_DIR_OUT / USBH_DIR_IN
**        psize - maximum packet size
**        interval - poll interval for periodic endpoints
** RETURN: NULL - error / on success pointer to the endpoint
*/
void *ohci_add_ep (hcc_u8 daddr, hcc_u8 ep, hcc_u8 type, hcc_u8 dir, hcc_u16 psize, hcc_u8 interval)
{
  void *rc=NULL;
  int i;
  hcc_u32 lbit,ec;
  t_ed *aed,*led,*paed;
  t_ed_dsc *aed_dsc, *fed_dsc=NULL;
  hcc_u8 speed;

  switch (dir)
  {
    case USBH_DIR_ANY: dir=_d_from_td; break;
    case USBH_DIR_SETUP: dir=_d_setup; break;
    case USBH_DIR_OUT: dir=_d_out; break;
    case USBH_DIR_IN: dir=_d_in; break;
    default: return NULL;
  }

  if (os_mutex_get(ohci_mutex_transfer)==OS_SUCCESS)
  {
    aed_dsc=ed_dsc;
    aed=ed;
    speed=0;
    for (i=0;i<TOTAL_EP;i++)		/* check for an empty place and if new descriptor */
    {								/* exists in ED area */
      ec=aed->ctrl;
      if (aed_dsc->used && _rED(ec,FA)==daddr)
      {
        if (ep && _rED(ec,EN)==0)
        {
          if (ec&_mED(S)) speed=1;
        }
        if (_rED(ec,EN)==ep && _rED(ec,D)==dir)
        {
          rc=(void *)aed;
          break;
        }
      }
      if (aed_dsc->used==0 && fed_dsc==NULL) fed_dsc=aed_dsc;
      ++aed_dsc;
      ++aed;
    }

    if (i==TOTAL_EP)
    {
      if (fed_dsc)
      {
        lbit=0;
        switch (type)
        {
          case USBH_TYPE_CONTROL: lbit=CLE; break;
          case USBH_TYPE_BULK:    lbit=BLE; break;
          case USBH_TYPE_INT:     lbit=PLE; break;
        }

        if (lbit)
        {
          HcControl&=~lbit;					/* disable type list */

          aed=ed+(fed_dsc-ed_dsc);
          rc=(void *)aed;
          paed=ohci_cached_addr((void *)aed);
		
          ec=0;
          _wED(ec,FA,daddr);
          _wED(ec,EN,ep);
          _wED(ec,D,dir);
          _wED(ec,S,speed);
          ec|=_mED(K);
          if (type==USBH_TYPE_ISO) ec|=_mED(F);
          _wED(ec,MPS,psize);
          aed->ctrl=ec;

          aed->tailp=aed->headp=aed->nexted=0;
          if (type!=USBH_TYPE_INT)
          {
            led=NULL;
            switch (type)
            {
              case USBH_TYPE_CONTROL:
                if (HcControlHeadED) led=(t_ed *)HcControlHeadED;
                                else HcControlHeadED=(hcc_u32)paed;
                break;
              case USBH_TYPE_BULK:
                if (HcBulkHeadED) led=(t_ed *)HcBulkHeadED;
                             else HcBulkHeadED=(hcc_u32)paed;
                break;
            }

            if (led)
            {
              t_ed *pled=ohci_uncached_addr((void *)led);
              while (_rADDR(pled->nexted))
              {
                led=_rADDR(pled->nexted);
                if (led) pled=ohci_uncached_addr((void *)led);
              }
              _wADDR(pled->nexted,paed);
            }
          }
          else
          {
            hcc_u8 fp,ap;
            if (interval>32) interval=32;
            for (fp=0;fp<32 && hcca->int_table[fp];fp++);
            if (fp<32)
            {
              hcca->int_table[fp]=(hcc_u32)paed;
              for (ap=fp+interval;ap<32 && fp+31-ap>=interval;ap++)
              {
                if (hcca->int_table[ap]==0)
                {
                  hcca->int_table[ap]=(hcc_u32)paed;
                  ap+=interval-1;
                }
              }
            }
          }

          fed_dsc->used=1;
          fed_dsc->type=type;
          fed_dsc->interval=interval;
          fed_dsc->dir=dir;
          fed_dsc->psize=psize;
          fed_dsc->tstate=OHCI_TRST_STOPPED;

          HcControl|=lbit;					/* reenable list */
        }
      }
    }
    (void)os_mutex_put(ohci_mutex_transfer);
  }

  return rc;
}


/*
** Remove all endpoints belonging to a device address
** INPUT: daddr - device address
** RERURN: USBH_ERR_RESOURCE / USBH_ERR_NOT_AVAILABLE / USBH_SUCCESS
*/
int ohci_del_daddr (hcc_u8 daddr)
{
  int rc=USBH_ERR_NOT_AVAILABLE;
  t_ed_dsc *aed_dsc;
  t_ed *aed,*ped,*ned;
  hcc_u32 lsave;
  hcc_u32 ssave;
  hcc_u8 ap;

  if (os_mutex_get(ohci_mutex_transfer)==OS_SUCCESS)
  {
    ssave=HcCommandStatus&(CLF|BLF);
    HcCommandStatus&=~(CLF|BLF);
    HcInterruptStatus=SF;
    while (!(HcInterruptStatus&SF));	/* wait start of frame */

    lsave=HcControl&(PLE|IE|CLE|BLE);
    HcControl&=~(PLE|IE|CLE|BLE);		/* disable control and bulk list */

    ped=aed=(t_ed *)HcControlHeadED;
    while (aed)
    {
      ned=_rADDR(aed->nexted);
      if (_rED(aed->ctrl,FA)==daddr)
      {
        rc=USBH_SUCCESS;
        aed_dsc=ed_dsc+((t_ed *)(ohci_uncached_addr((void *)aed))-ed);
        if (aed_dsc->tstate!=OHCI_TRST_STOPPED)
        {
          ohci_del_td(aed,0);			/* remove TD-s belonging to this ED */
          aed_dsc->tstate=OHCI_TRST_STOPPED;
        }
        if (ped==aed)
        {
          HcControlHeadED=(hcc_u32)ned;
          ped=ned;
        }
        else
        {
          t_ed *pped=ohci_uncached_addr((void *)ped);
          _wADDR(pped->nexted,ned);
        }

        if (HcControlCurrentED==(hcc_u32)aed)
        {
          if (ned) HcControlCurrentED=(hcc_u32)ned;
              else HcControlCurrentED=HcControlHeadED;
        }
        aed_dsc->used=0;
      }
      else ped=aed;
      aed=ned;
    }

    ped=aed=(t_ed *)HcBulkHeadED;
    while (aed)
    {
      ned=_rADDR(aed->nexted);
      if (_rED(aed->ctrl,FA)==daddr)
      {
        rc=USBH_SUCCESS;
        aed_dsc=ed_dsc+((t_ed *)(ohci_uncached_addr((void *)aed))-ed);
        if (aed_dsc->tstate!=OHCI_TRST_STOPPED)
        {
          ohci_del_td(aed,0);			/* remove TD-s belonging to this ED */
          aed_dsc->tstate=OHCI_TRST_STOPPED;
        }
        if (ped==aed)
        {
          HcBulkHeadED=(hcc_u32)ned;
          ped=ned;
        }
        else
        {
          t_ed *pped=ohci_uncached_addr((void *)ped);
          _wADDR(pped->nexted,ned);
        }

        if (HcBulkCurrentED==(hcc_u32)aed)
        {
          if (ned) HcBulkCurrentED=(hcc_u32)ned;
              else HcBulkCurrentED=HcControlHeadED;
        }
        aed_dsc->used=0;
      }
      else ped=aed;
      aed=ned;
    }

    for (ap=0;ap<32;ap++)
    {
      aed=(t_ed *)hcca->int_table[ap];
      if (_rED(aed->ctrl,FA)==daddr)
      {
        rc=USBH_SUCCESS;
        aed_dsc=ed_dsc+((t_ed *)(ohci_uncached_addr((void *)aed))-ed);
        if (aed_dsc->tstate!=OHCI_TRST_STOPPED)
        {
          ohci_del_td(aed,0);			/* remove TD-s belonging to this ED */
          aed_dsc->tstate=OHCI_TRST_STOPPED;
        }
        aed_dsc->used=0;
        hcca->int_table[ap]=0;
        for (ap++;ap<32;ap++)
        {
          if (hcca->int_table[ap]==(hcc_u32)aed) hcca->int_table[ap]=0;
        }
        break;
      }
    }

    if (HcControlHeadED==0)
    {
      lsave&=~CLE;
      ssave&=~CLF;
    }
    if (HcBulkHeadED==0)
    {
      lsave&=~BLE;
      ssave&=~BLF;
    }

    HcControl|=lsave;					/* reenable lists */
    HcCommandStatus|=ssave;

    (void)os_mutex_put(ohci_mutex_transfer);
  }
  else rc=USBH_ERR_RESOURCE;

  return rc;
}


/*
** Clear EP toggle value
** INPUT: ep_handle - endpoint handle
** RETURN: USBH_ERR_NOT_AVAILABLE - endpoint not found
**         USBH_SUCCESS
*/
int ohci_clear_toggle (void *ep_handle)
{
  int rc=USBH_ERR_NOT_AVAILABLE;

  if ((t_ed *)ep_handle>=ed && (t_ed *)ep_handle<(ed+TOTAL_EP))
  {
    if (os_mutex_get(ohci_mutex_transfer)==OS_SUCCESS)
    {
      ((t_ed *)ep_handle)->headp&=~_mED(C);
      (void)os_mutex_put(ohci_mutex_transfer);
      rc=USBH_SUCCESS;
    }
    else rc=USBH_ERR_RESOURCE;
  }

  return rc;
}


/*
** Transfer task
*/

OS_TASK_FN(ohci_transfer_task)
{
  int rc;
  os_buffer_init();
  
  rc = ohci_init();

  if (rc) {
    sc_miscError(7,rc);
  }
  
  for (;;)
  {
    t_ed_dsc *aed_dsc;
    hcc_u8 cb=0;
/*+++ SCIOPTA */
    {
      static const sc_msgid_t sel[3] = {
	SDD_OBJ_RELEASE,
	HCC_USBH_IRQ,
	0
      };
      sc_msg_t msg;
      msg = sc_msgRx(SC_ENDLESS_TMO,sel,SC_MSGRX_MSGID);
      if ( msg->id == SDD_OBJ_RELEASE ){
	msg->base.error = 0;
	++msg->id;
	sc_msgTx(&msg,sc_msgSndGet(&msg),0);
	continue;
      }
      sc_msgFree(&msg);
    }    
//->    sc_triggerWait(1,SC_ENDLESS_TMO);
/*--- SCIOPTA */    
    if (os_mutex_get(ohci_mutex_transfer)==OS_SUCCESS)
    {
        t_td_dsc *atd_dsc;
        t_td *atd=_rADDR(hcca->done_head);		/* get first TD address in done list */
        t_td *ptd=NULL;
        hcc_u32 csize;
        hcc_u8 cc;

        while (atd && ptd!=atd)					/* while TD available and no loopback */
        {
          atd=ohci_uncached_addr((void *)atd);
          atd_dsc=td_dsc+(atd-td);				/* get TD descriptor address */
          aed_dsc=atd_dsc->ed_dsc;

          cc=(hcc_u8)_rTD(atd->ctrl,CC);
          if (cc==0 || cc==C_DataUnderrun)		/* if TD completed successfully or short packet */
          {
            csize=atd_dsc->req_size;			/* calculate completed size */
            if (atd->cbp)
            {
              hcc_u32 n=atd->be+1;
              n-=atd->cbp;
              csize-=n;
            }

#if OHCI_BUFFER_ADDRESS==0
            if (atd_dsc->dir==_d_in)
            {
              ohci_unlock_buffer(atd_dsc->taddr,csize);
              atd_dsc->taddr=NULL;
            }
#endif

            aed_dsc->uht->buf+=csize;
            aed_dsc->uht->csize+=csize;			/* add TD completed size to transfer completed size */

            if (atd_dsc->tbuf_dsc)				/* if temporary buffer associated */
            {									/* then copy back data to user buffer if needed */
              if (atd_dsc->tbuf_dsc->buf && csize)
              {
                _memcpy(atd_dsc->tbuf_dsc->dptr,ohci_uncached_addr((void *)atd_dsc->tbuf_dsc->buf),csize);
              }
            }
          }
          if (atd_dsc->tbuf_dsc) atd_dsc->tbuf_dsc->used=0;
          --aed_dsc->td_cnt;					/* decrease number of TD-s for this ED */

          if (aed_dsc->tstate==OHCI_TRST_RUNNING)
          {
            if (cc)
            {
              if (cc==C_DataUnderrun) aed_dsc->tstate=OHCI_TRST_SHORT_PACKET;
								 else aed_dsc->tstate=OHCI_TRST_ERROR;
            }
            else if (aed_dsc->td_cnt==0) aed_dsc->tstate=OHCI_TRST_SUCCESS;

            if (aed_dsc->tstate!=OHCI_TRST_RUNNING)
            {
              if (aed_dsc->uht->cb_fn)			/* callback function available */
              {
                if (aed_dsc->tstate==OHCI_TRST_ERROR) aed_dsc->uht->rc=USBH_ERR_TRANSFER;
                else if (aed_dsc->tstate==OHCI_TRST_SHORT_PACKET) aed_dsc->uht->rc=USBH_SHORT_PACKET;
                else aed_dsc->uht->rc=USBH_SUCCESS;
                aed_dsc->tstate=OHCI_TRST_CB;	/* set callback state */
                cb=1;
              }
            }
          }

          ptd=atd;
          atd=_rADDR(atd->nexttd);
        }
        hcca->done_head=0;						/* clear done head */
        HcInterruptStatus=WDH;					/* clear HccaDoneHead interrupt */
        HcInterruptEnable=WDH;					/* reenable DoneHead interrupt */

        (void)os_mutex_put(ohci_mutex_transfer);

        if (cb)									/* call all callback functions */
        {
          for (aed_dsc=ed_dsc;aed_dsc<ed_dsc+TOTAL_EP;aed_dsc++)
          {
            if (aed_dsc->tstate==OHCI_TRST_CB)	/* call back transfer found */
            {
              (void)ohci_del_td(ed+(aed_dsc-ed_dsc),1);
              aed_dsc->tstate=OHCI_TRST_STOPPED;/* set stopped state */
              (aed_dsc->uht->cb_fn)(aed_dsc->uht->cb_param,aed_dsc->uht->rc);
            }
          }
        }
      }
    }
  
}



/*
** Get status of a transfer
** INPUT: uht - transfer descriptor
**        mode - OHCI_MODE_BLOCK/OHCI_MODE_NON_BLOCK
** RETURN:USBH_SUCCESS
**        USBH_SHORT_PACKET
**        USBH_ERR_TRANSFER
**        USBH_ERR_TIMEOUT
**        USBH_ERR_NOT_COMPLETE
*/
int ohci_transfer_status (t_usbh_transfer *uht, hcc_u8 mode)
{
  int rc;
  t_td *ctd,*pctd;
  t_ed_dsc *aed_dsc=*((t_ed_dsc **)uht->driver_area);
  t_ed *aed=ed+(aed_dsc-ed_dsc);


  pctd=_rADDR(aed->headp);
  for (;;)
  {
  transfer_complete:
    if (aed_dsc->tstate!=OHCI_TRST_RUNNING)			/* transfer finished */
    {
      (void)ohci_del_td(aed,1);
      if (aed_dsc->tstate==OHCI_TRST_ERROR) rc=USBH_ERR_TRANSFER;
      else if (aed_dsc->tstate==OHCI_TRST_STOPPED) rc=USBH_ERR_TRANSFER;
      else if (aed_dsc->tstate==OHCI_TRST_SHORT_PACKET) rc=USBH_SHORT_PACKET;
      else rc=USBH_SUCCESS;
      aed_dsc->tstate=OHCI_TRST_STOPPED;
      if (rc==USBH_SUCCESS && uht->size!=uht->csize)
      {
        rc=ohci_add_td(uht);							/* build next qTD chain if transfer not finished */
        if (rc!=USBH_SUCCESS && rc!=USBH_ERR_TRANSFER_FULL) break;
      }
      else
      {
        break;
      }
    }

    ctd=_rADDR(aed->headp);							/* save current TD for transfers ED */
    if (aed_dsc->uht->timeout!=USBH_WAIT_FOREVER)	/* check timeout if needed */
    {
/*+++ SCIOPTA */
      /* Needed to allow transfer-task to run */
      //sc_sleep(2);
/*--- SCIOPTA */
      if (ctd)										/* if TD is available for this tranfer ED */
      {
        if (pctd==ctd)								/* check if TD changed in ED head pointer */
        {
          hcc_u32 cframe=HcFmNumber&FN;				/* if yes then check timeout */
          hcc_u32 d;
          t_td *_ctd=ohci_uncached_addr((void *)ctd);
          t_td_dsc *ctd_dsc=td_dsc+(_ctd-td);		/* get TD descriptor */
		
          if (os_mutex_get(ohci_mutex_transfer)==OS_SUCCESS)
          {
            if (ctd_dsc->frame)
            {
              if (cframe<ctd_dsc->frame) cframe|=(FN+1);/* set FN+1 bit if overflow happened */
              d=cframe-(ctd_dsc->frame);
              if (ctd_dsc->timeout<=d)				/* timeout occured */
              {
                aed->ctrl|=_mED(K);					/* set skip bit */
                HcInterruptStatus=SF;
                while (!(HcInterruptStatus&(SF|UE)));	/* wait start of frame */
                aed->headp|=_mED(H);					/* set halt bit */
                (void)os_mutex_put(ohci_mutex_transfer);

                if (aed_dsc->tstate!=OHCI_TRST_RUNNING) goto transfer_complete;
                aed_dsc->tstate=OHCI_TRST_STOPPED;
                ohci_del_td(aed,1);
                rc=USBH_ERR_TIMEOUT;
                break;
              }
              else ctd_dsc->timeout-=d;
            }
            ctd_dsc->frame=cframe&FN;				/* save current frame */

            (void)os_mutex_put(ohci_mutex_transfer);
          }
        }
        pctd=ctd;
      }
    }

    if (mode==USBH_MODE_NON_BLOCK)
    {
      rc=USBH_ERR_NOT_COMPLETE;
      break;
    }
  }

  return rc;
}


/*
** Start transfer
** INPUT: uht - transfer descriptor
**        mode - USBH_MODE_BLOCK/USBH_MODE_NON_BLOCK
** RETURN:
*/
int ohci_transfer (t_usbh_transfer *uht, hcc_u8 mode)
{
  int rc;

  uht->csize=0;
  rc=ohci_add_td(uht);									/* build TD chain */
  if (mode==USBH_MODE_BLOCK)
  {
    if (rc==USBH_SUCCESS || rc==USBH_ERR_TRANSFER_FULL)
    {
      rc=ohci_transfer_status(uht,mode);
    }
  }

  return rc;
}


/*
** Delay ms number of milliseconds
*/
void ohci_delay (hcc_u32 ms)
{
  sc_sleep(sc_tickMs2Tick(ms));
#if 0
  hcc_u32 cframe=HcFmNumber&FN;
  while (ms)
  {
    if (cframe!=(HcFmNumber&FN))
    {
      --ms;
      cframe=HcFmNumber&FN;
    }
  }
#endif
}


/*
** Set speed,HUB address and HUB speed for endpoint0
*/
int ohci_set_ep0_speed_hub (void *ep0_handle, hcc_u8 speed, hcc_u8 hub_daddr, hcc_u8 hub_speed)
{
  int rc=USBH_SUCCESS;
  t_ed *aed;

  if (ep0_handle==NULL) ep0_handle=(void *)ed;
  if ((t_ed *)ep0_handle>=ed && (t_ed *)ep0_handle<(ed+TOTAL_EP))
  {
    if (os_mutex_get(ohci_mutex_transfer)==OS_SUCCESS)
    {
      aed=(t_ed *)ep0_handle;
      if (speed==USBH_LOW_SPEED) aed->ctrl|=_mED(S);
      else if (speed==USBH_FULL_SPEED) aed->ctrl&=~_mED(S);
      else rc=USBH_SPEED_ERROR;
      (void)os_mutex_put(ohci_mutex_transfer);
    }
    else rc=USBH_ERR_RESOURCE;
  }
  else rc=USBH_ERR_NOT_AVAILABLE;

  return rc;
}


/*
** Get info of an ep_handle
**
** Input: ep_handle - endpoint handler
** Output: daddr - device address
**         ep - endpoint number
**         type - endpoint type
**         dir - endpoint dir
**         psize - default packet size
** Return: USBH_SUCCESS / USB_ERR_NOT_AVAILABLE / USBH_ERR_DIR
*/
int ohci_get_ep_inf (void *ep_handle, hcc_u8 *daddr, hcc_u8 *ep, hcc_u8 *type, hcc_u8 *dir, hcc_u16 *psize)
{
  int rc=USBH_SUCCESS;
  hcc_uint f;

  if ((t_ed *)ep_handle>=ed && (t_ed *)ep_handle<(ed+TOTAL_EP))
  {
    f=(hcc_uint)((t_ed *)ep_handle-ed);
    if (daddr) *daddr=(hcc_u8)(_rED((ed+f)->ctrl,FA));
    if (ep) *ep=(hcc_u8)(_rED((ed+f)->ctrl,EN));
    if (type) *type=(ed_dsc+f)->type;
    if (psize) *psize=(ed_dsc+f)->psize;
    if (dir)
    {
      switch ((ed_dsc+f)->dir)
      {
        case _d_from_td: *dir=USBH_DIR_ANY; break;
        case _d_setup: *dir=USBH_DIR_SETUP; break;
        case _d_out: *dir=USBH_DIR_OUT; break;
        case _d_in: *dir=USBH_DIR_IN; break;
        default: rc=USBH_ERR_DIR;
      }
    }
  }
  else rc=USBH_ERR_NOT_AVAILABLE;

  return rc;
}


/*
** Init OHCI
*/
int ohci_init (void)
{
  ohci_buffer=ohci_uncached_addr((void *)_ohci_buffer);
#if OHCI_BUFFER_ADDRESS==0
  _memset((void *)ohci_buffer,0,sizeof(ohci_buffer));
  hcca=(t_hcca *)ALIGN256((hcc_u32)ohci_buffer);	/* HCCA buffer address, 256 byte aligned */
#else
  _memset((void *)OHCI_BUFFER_ADDRESS,0,OHCI_BUFFER_SIZE);
  hcca=(t_hcca *)ALIGN256(OHCI_BUFFER_ADDRESS);		/* HCCA buffer address, 256 byte aligned */
#endif
  ed=(t_ed *)ALIGN16((hcc_u32)hcca+HCCA_SIZE);		/* ED area, 16 byte aligned */
  td=(t_td *)ALIGN16((hcc_u32)ed+ED_SIZE);			/* TD area, 16 byte aligned */
  tbuf=(hcc_u8 *)((hcc_u32)td+TD_SIZE);				/* temporary buffer area */

  if (os_mutex_create(&ohci_mutex_transfer)) return USBH_ERR_RESOURCE;

  return USBH_SUCCESS;
}


/*
** Start OHCI
*/
int ohci_start (hcc_u32 p)
{
  int rc;
  hcc_u32 fminterval;

  rc=ohci_hw_init();
  if (rc) return rc;

  rc=ohci_hub_init();
  if (rc) return rc;

  HcCommandStatus|=HCR;
  while (HcCommandStatus&HCR);						/* reset host controller */

  HcControl=HCFS_RESET;

  _memset(ed_dsc,0,sizeof(ed_dsc));
  _memset(td_dsc,0,sizeof(td_dsc));

  HcHCCA=(hcc_u32)ohci_cached_addr((void *)hcca);

  fminterval=0x2edf;
  HcPeriodStart=(fminterval*9)/10;
  fminterval|=((((fminterval-210)*6)/7)<<16);
  HcFmInterval=fminterval;
  HcLSThreshold=0x628;

  HcControl=(CBSR_1_1|HCFS_OPERATIONAL);

  if (ohci_add_ep(0,0,USBH_TYPE_CONTROL,USBH_DIR_ANY,8,0)==NULL)
  {
    return USBH_ERR_HOST;
  }

#if OS_INTERRUPT_ENABLE
  HcInterruptEnable=MIE|WDH|RHSC;					/* enable WriteDoneHead and RHSC interrupts */
#endif
#if 0
  rc=os_isr_init(OHCI_HOST_ISR, ohci_isr_handler);
  if (rc) return rc;

  rc=os_isr_enable(OHCI_HOST_ISR);
  if (rc) return rc;
#endif
  return USBH_SUCCESS;
}


/*
** Stop OHCI
*/
int ohci_stop (void)
{
#if 0
  int rc;
  rc=os_isr_disable(OHCI_HOST_ISR);
  if (rc) return rc;
#endif
#if OS_INTERRUPT_ENABLE
  HcInterruptEnable=0;			
#endif

  HcControl=0;

  return USBH_SUCCESS;
}

