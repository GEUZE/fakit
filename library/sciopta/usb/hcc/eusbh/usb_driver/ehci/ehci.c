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
#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#include <hcc/eusbh/usb_driver/ehci/ehci_reg.h>
#include <hcc/eusbh/usb_driver/ehci/ehci_hub.h>
#include <hcc/eusbh/usb_driver/pmgr/pmgr.h>
#include <hcc/eusbh/os.h>
#include <hcc/eusbh/usbh.msg>
union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  hcc_usbhIrq_t irq;
};
/*--- SCIOPTA */

#define CONTROL_EP			(MAX_DEVICE+1)			/* 1 for every EP and a general 0 address EP */
#define TOTAL_EP			(MAX_EP+CONTROL_EP)		/* total number of endpoints */

#define QH_CNT				TOTAL_EP
#define QTD_CNT				MAX_TRANSFERS

#define PFL_SIZE			(EHCI_PERIODIC_LIST_SIZE*sizeof(hcc_u32))
#define QH_SIZE				(QH_CNT*ALIGN32(sizeof(t_qh)))
#define QTD_SIZE			(QTD_CNT*ALIGN32(sizeof(t_qtd)))
#define QH_DSC_SIZE			(QH_CNT*sizeof(t_qh_dsc))
#define QTD_DSC_SIZE		(QTD_CNT*sizeof(t_qtd_dsc))

/* buffer size +4095 if peridic enabled to be able to align to 4K boundary otherwise 32 byte alignment is enough */
#if EHCI_PERIODIC_LIST_SIZE
#define EHCI_BUFFER_SIZE	(PFL_SIZE+QH_SIZE+QTD_SIZE+4095)
#else
#define EHCI_BUFFER_SIZE	(QH_SIZE+QTD_SIZE+31)
#endif


#define EHCI_TRST_STOPPED		0
#define EHCI_TRST_RUNNING		1
#define EHCI_TRST_SHORT_PACKET	2
#define EHCI_TRST_CB			3
#define EHCI_TRST_ERROR			4
#define EHCI_TRST_SUCCESS		5

#define _d_out			0
#define _d_in			1
#define _d_setup		2
#define _d_from_td		3


/*
** driver area structure for t_usbh_transfer
*/
typedef struct {
  hcc_u32 ctsize;			/* current transfer size */
  void *qh_dsc;				/* QH_DSC of this descriptor */
} t_ehci_transfer_area;

 
/*
** QH transfer descriptor
*/
typedef struct {
  t_qtd *fqtd;				/* first qTD */
  t_usbh_transfer *uht;		/* tranfer descriptor */
  hcc_u32 timeout;			/* timeout value */
  hcc_u32 cframe;			/* last frame */
  volatile hcc_u8 state;	/* transfer state */
} t_qh_transfer;


/*
** QH descriptor
*/
typedef struct {
  void *pqh;				/* previous QH */
  hcc_u8 used;				/* QH used */
  hcc_u8 dir;				/* direction */
  hcc_u8 type;				/* type */
  hcc_u8 toggle;			/* toggle value (bug in 5329) */
  hcc_u8 psize_sh;			/* maximum packet size shift */
  hcc_u16 interval;			/* poll interval for INTR EP */

  t_qh_transfer transfer;	/* transfer descriptor */
} t_qh_dsc;


/*
** qTD descriptor
*/
typedef struct {
  void *qh;
  hcc_u32 ctsize;			/* current QTD transfer size */
  void *ctbuf;				/* current transfer buffer pointer */
} t_qtd_dsc;


/************************************************************************************************************************/
/********************************************** Local variables *********************************************************/
/************************************************************************************************************************/
/***** EHCI variables *****/
static hcc_u8 ehci_buffer[EHCI_BUFFER_SIZE];						/* EHCI buffer for QH and qTD */

static hcc_u32 *pfl;												/* periodic frame list */

static t_qh *qh;													/* QH area */
static t_qh_dsc qh_dsc[QH_CNT];

static t_qtd *qtd;													/* qTD area */
static t_qtd_dsc qtd_dsc[QTD_CNT];

/***** mutexes and events *****/
#define EHCI_MUTEX_TRANSFER		0
/*+++ SCIOPTA */
#if 0 /* not used */
#define EHCI_EVENT_TRANSFER		0
#define EHCI_EVENT_IOAA		1
#endif
/*--- SCIOPTA */
static OS_MUTEX_TYPE *ehci_mutex[EHCI_MUTEX_COUNT];
/*+++ SCIOPTA */
#if 0 /* not used */
static OS_EVENT_BIT_TYPE ehci_event_bit[EHCI_EVENT_COUNT];
#endif
/*--- SCIOPTA */
/************************************************************************************************************************/

/*
** EHCI interrupt handler
*/
/*+++ SCIOPTA */
#if 0 /* not used with SCIOPTA */
OS_ISR_FN(ehci_isr_handler)
{
  hcc_u32 sts;

  _r32(sts,USBSTS);
  sts&=(PCD|USBINT|USBERRINT|IOAA);
  _w32(USBSTS,sts);
  if (sts&PCD) (void)os_event_set_int(pmgr_event_bit);
  if (sts&(USBINT|USBERRINT)) (void)os_event_set_int(ehci_event_bit[EHCI_EVENT_TRANSFER]);
  if (sts&IOAA) (void)os_event_set_int(ehci_event_bit[EHCI_EVENT_IOAA]);
}
#endif
/*--- SCIOPTA */
/*
** Return current frame number
*/
hcc_u32 ehci_get_frame_number (void)
{
  hcc_u32 rc;
  _r32(rc,FRINDEX);
  return rc&~7;
}


/*
** Add all qTD-s described in t_ehci_transfer
** INPUT: t_ehci_transfer - described in ehci.h
** RETURN: USBH_ERR_TRANSFER_FULL - no more entries to complete whole transfer,
**           later retry needed, check ot->csize after calling ohci_td_status()
**         USBH_ERR_NOT_AVAILABLE - transfer not possible, endpoint doesn't exist
**         USBH_SUCCESS - all transfers queued successfully
*/
static int ehci_add_qtd (t_usbh_transfer *uht)
{
  int rc=USBH_SUCCESS;
  t_qtd *fqtd,*aqtd,*pqtd;		/* first/actual/previous QTD */
  t_qtd_dsc *aqtd_dsc;			/* actual QTD descriptor */
  t_qh *aqh;					/* actual QH */
  t_qh_dsc *aqh_dsc;			/* aqctual QH descriptor */
  hcc_u32 tmp;
  hcc_u32 size,remain,max;
  hcc_u32 salign=0;
  hcc_u32 buf=0;
  hcc_u8 unalign=0;
  hcc_u8 dir;
  hcc_u8 *abuf,*ctbuf;
  t_ehci_transfer_area *uht_et=(t_ehci_transfer_area *)(uht->driver_area);

  aqtd=NULL;
  if (uht->eph==NULL) uht->eph=qh;
  if ((t_qh *)(uht->eph)>=qh && (t_qh *)(uht->eph)<(qh+QH_CNT)) 
  {
    pqtd=NULL;
    fqtd=NULL;
    aqh=(t_qh *)uht->eph;
    aqh_dsc=qh_dsc+(aqh-qh);
  
    if (aqh_dsc->dir==_d_from_td)
    {
      switch (uht->dir)
      {
        case USBH_DIR_SETUP: dir=_d_setup; break;
        case USBH_DIR_IN: dir=_d_in; break;
        case USBH_DIR_OUT: dir=_d_out; break;
        default: rc=USBH_ERR_DIR; 
      }
    }
    else dir=aqh_dsc->dir;
  }
  else rc=USBH_ERR_NOT_AVAILABLE;

  if (rc==USBH_SUCCESS)
  {
    if (os_mutex_get(ehci_mutex[EHCI_MUTEX_TRANSFER])==OS_SUCCESS)
    {
      if (aqh_dsc->transfer.state==EHCI_TRST_STOPPED)
      {
        uht_et->ctsize=0;
        if (uht->size==USBH_SIZE_DEFAULT)
        {
          uht->size=(hcc_u32)(1<<(aqh_dsc->psize_sh));
        }
        aqtd=qtd;
        aqtd_dsc=qtd_dsc;
        remain=uht->size-uht->csize;								/* calculate remaining size to complete the transfer */
        abuf=uht->buf;
        do {
          for (;aqtd_dsc->qh && aqtd_dsc<qtd_dsc+QTD_CNT;aqtd++,aqtd_dsc++)	/* search for empty qTD */
            ;
          if (aqtd_dsc<qtd_dsc+QTD_CNT)								/* empty qTD available? */
          {
            aqtd_dsc->qh=aqh;										/* mark qTD as used */
    
            if (abuf)
            {
              /* lock buffer */
              size=remain;
#ifdef ehci_hw_lock_buffer              
              ctbuf=ehci_hw_lock_buffer(abuf,&size)
                ;
#else
              ctbuf=abuf;
#endif
              if (ctbuf==NULL)
              {
                if (fqtd) rc=USBH_ERR_TRANSFER_FULL;				/* no more space for another qTD */
                     else rc=USBH_ERR_TRANSFER;
                break;
              }

              salign=ALIGN_NEXT_PAGE(ctbuf)-(hcc_u32)(ctbuf);		/* get remaining space in PAGE */
              if (size>salign)										/* remaining space is less than remaining data? */
              {
                max=salign+((EHCI_PAGE_COUNT-1)*EHCI_PAGE_SIZE);	/* yes, calculate max. size of transfer in qTD */
                if (size>max)
                {
                  size=max;											/* use all data in qTD (remaining data>max. size) */
                  if ((hcc_u32)(ctbuf)&((1<<(aqh_dsc->psize_sh))-1))
                  {
                    size&=~((1<<(aqh_dsc->psize_sh))-1);			/* align transfer to max. packet size if */
                    unalign=1;										/* in this case transfer can't be done in linked list */
                  }
                }
                else if (size<max && remain>size)					/* can't handle a linked list transfer if size available */
                {													/* in the current buffer can't fill the maximum transfer */
                  unalign=1;										/* size in one descriptor */
                }
              }
            }
            else 
            {
              ctbuf=NULL;
              size=0;
            }

            if (fqtd==NULL) fqtd=aqtd;								/* store first qTD of the transfer */
 
            if (ctbuf)
            {
              buf=(hcc_u32)ctbuf;
              aqtd_dsc->ctbuf=ctbuf;
              aqtd_dsc->ctsize=size;
              if (ctbuf!=abuf && (dir==_d_out || dir==_d_setup))
              {
                _memcpy(ctbuf,abuf,size);
              }
            }
            else
            {
              aqtd_dsc->ctbuf=NULL;
            }
    
            if (pqtd) _wADDR(pqtd->nqtd,aqtd);						/* link qTD to previous qTD if exists */
            _w32(aqtd->nqtd,_vQTD(T,1));
            _w32(aqtd->anqtd,_vQTD(T,1));							/* nqTD=aqTD=1 */
            if (uht->toggle!=USBH_TOGGLE_PRESERVE)					/* predefined toggle bit */
            {
              tmp=(hcc_u32)_vQTD(DT,uht->toggle);
            }
            else
            {
              tmp=(hcc_u32)_vQTD(DT,aqh_dsc->toggle);				/* use stored toggle value (this will be updated */
              uht->toggle=aqh_dsc->toggle;
            }														/* when the tranfer is finished) */
    
            if (size<remain)
            {
              if ((size>>(aqh_dsc->psize_sh))&1) uht->toggle^=1;	/* calculate next toggle value if requested transfer */
            }														/* size can't fit in the current qTD */

            tmp|=_vQTD(TB,size)|_vQTD(PID,dir)|_vQTD(STATUS,ST_ACTIVE)|_vQTD(CERR,1);
            _w32(aqtd->ctrl0,tmp);
    
            remain-=size;											/* substract qTD transfer size from requested tranfer */
            uht_et->ctsize+=size;									/* current transfer size+=qTD transfer size */
            abuf+=size;												/* set new buffer pointer */
            for (tmp=0;tmp<EHCI_PAGE_COUNT;tmp++)
            {
              if (size)
              {
                _w32(aqtd->bptr[tmp],buf);
                buf=ALIGN_NEXT_PAGE(buf);
                if (size>salign) size-=salign;
                            else size=0;
                salign=EHCI_PAGE_SIZE;
              }
              else _w32(aqtd->bptr[tmp],0);
            }														/* fill in page pointers */

            pqtd=aqtd;												/* save previous qTD */
          }
          else
          {
            if (fqtd) rc=USBH_ERR_TRANSFER_FULL;					/* no more space for another qTD */
                 else rc=USBH_ERR_TRANSFER;
            break;
          }
          break;
        } while (remain && unalign==0);								/* add qTD-s while req. tranfer size is not 0 and aligned */
      }
      else rc=USBH_ERR_ONGOING_TRANSFER;
    
      uht->toggle=USBH_TOGGLE_PRESERVE;								/* preserve toggle bit in the future (obtained from */
      if (fqtd)
      {
        aqh_dsc->transfer.fqtd=fqtd;
        aqh_dsc->transfer.uht=uht;
        aqh_dsc->transfer.cframe=ehci_get_frame_number();
        aqh_dsc->transfer.timeout=uht->timeout;
        aqh_dsc->transfer.state=EHCI_TRST_RUNNING;
        uht_et->qh_dsc=aqh_dsc;

        _sb32(aqtd->ctrl0,_vQTD(IOC,1));							/* set interrupt on complete for the last qTD */	
        tmp=_rQTD(aqh->qtd.ctrl0,STATUS)&ST_PERR;
        _wQTD(aqh->qtd.ctrl0,STATUS,ST_HALTED);
        _wADDR(aqh->qtd.nqtd,fqtd);									/* add first qTD pointer to QH */
        _w32(aqh->qtd.ctrl0,_vQTD(STATUS,tmp));
      }

      (void)os_mutex_put(ehci_mutex[EHCI_MUTEX_TRANSFER]);
    }
    else rc=USBH_ERR_RESOURCE;
  }

  return rc;
}



/*
** Delete all qTD chain
** INPUT: aqtd - first qTD in the list to remove
** RETURN: USBH_SUCCESS
*/
static int ehci_del_qtd (t_qtd *aqtd)
{
  t_qtd_dsc *aqtd_dsc;
  if (os_mutex_get(ehci_mutex[EHCI_MUTEX_TRANSFER])==OS_SUCCESS)
  {
    while (aqtd)
    {
      aqtd_dsc=qtd_dsc+(aqtd-qtd);
      aqtd_dsc->qh=NULL;
#ifdef ehci_hw_unlock_buffer
      if (aqtd_dsc->ctbuf)
      {
        ehci_hw_unlock_buffer(aqtd_dsc->ctbuf,0)
          ;
      }
#endif
      aqtd=_rADDR(aqtd->nqtd);
    }
    (void)os_mutex_put(ehci_mutex[EHCI_MUTEX_TRANSFER]);
  }
  else return USBH_ERR_RESOURCE;
  return USBH_SUCCESS;
}


/*
** Add endpoint (adds QueueHead to the list).
** INPUT: daddr - device address
**        ep - endpoint number
**        type - USBH_TYPE_CONTROL / USBH_TYPE_BULK / USBH_TYPE_INT / USBH_TYPE_ISO
**        dir - USBH_DIR_ANY / USBH_DIR_SETUP / USBH_DIR_OUT / USBH_DIR_IN
**        psize - maximum packet size
**        interval - poll interval for periodic endpoints
** RETURN: NULL - error / on success pointer to the endpoint
*/
void *ehci_add_ep (hcc_u8 daddr, hcc_u8 ep, hcc_u8 type, hcc_u8 dir, hcc_u16 psize, hcc_u8 interval)
{
  void *rc=NULL;
  hcc_uint f;
  hcc_u32 c0;
  t_qh *lqh,*aqh;									/* last, actual QH */
  t_qh_dsc *aqh_dsc=NULL;
  hcc_u8 hub_daddr;									/* HUB address */
  hcc_u8 speed;

  if (ehci_hw_state()==USBH_ERR_SUSPENDED) return NULL;
  if (interval==1) interval=2;

#if EHCI_PERIODIC_LIST_SIZE
  if (type==USBH_TYPE_ISO || (type==USBH_TYPE_INT && pfl==NULL)) return NULL;
#else
  if (type==USBH_TYPE_ISO || type==USBH_TYPE_INT) return NULL;
#endif

  switch (dir)
  {
    case USBH_DIR_ANY: dir=_d_from_td; break;
    case USBH_DIR_SETUP: dir=_d_setup; break;
    case USBH_DIR_OUT: dir=_d_out; break;
    case USBH_DIR_IN: dir=_d_in; break;
    default: return NULL;
  }

  if (os_mutex_get(ehci_mutex[EHCI_MUTEX_TRANSFER])==OS_SUCCESS)
  {
    aqh=lqh=NULL;
    hub_daddr=0;
    speed=0;
    for (f=0;f<TOTAL_EP;f++)							/* find an empty QH position */
    {
      if (qh_dsc[f].used)
      {
        c0=(qh+f)->ctrl0;								/* check if DA/EP is already available */
        if (_rQH(c0,DA)==daddr)
        {
          if (ep && _rQH(c0,ENDPT)==0)					/* save HUB address and TT for this device */
          {
            speed=(hcc_u8)_rQH((qh+f)->ctrl0,EPS);
            hub_daddr=(hcc_u8)_rQH((qh+f)->ctrl1,HUBADDR);
          }
          if (_rQH(c0,ENDPT)==ep && qh_dsc[f].dir==dir)
          {
            rc=qh+f;
            break;
          }
        }
        if (_rADDR((qh+f)->nqh)==qh) lqh=qh+f;			/* store last QH pointer */
      }
      else if (aqh==NULL)
      {
        aqh=qh+f;
        aqh_dsc=qh_dsc+f;
      }
    }
  
    if (f==TOTAL_EP)									/* enpoint not available */
    {
      if (aqh)
      {
        rc=aqh;
#if EHCI_PERIODIC_LIST_SIZE
        if (type==USBH_TYPE_INT)
        {
          _w32(aqh->nqh,_vQH(TYP,th_qh)|_vQH(T,1));		/* type is QH and last in periodic list */
        }
        else
#endif
        {
          _w32(aqh->nqh,_vQH(TYP,th_qh));				/* type is QH */
          _wADDR(aqh->nqh,qh);							/* next QH is always the first QH in ASYNC list */
        }
        c0=_vQH(DTC,1);									/* always use toggle value from qTD. BUG in MCF5329!!! (?) */
        c0|=_vQH(DA,daddr)|_vQH(ENDPT,ep)|_vQH(MPL,psize);	/* set DA/EN/packet size */
        c0|=_vQH(EPS,speed);							/* set speed */
    
        if (type==USBH_TYPE_CONTROL) c0|=_vQH(C,1);		/* set control flag if needed */
        _w32(aqh->ctrl0,c0);
        
        c0=_vQH(MULT,1);
        if (hub_daddr) 
        {
          c0|=_vQH(HUBADDR,hub_daddr);
          c0|=_vQH(PORTNUM,pmgr_get_port_number(hub_daddr));
        }
#if EHCI_PERIODIC_LIST_SIZE
        if (type==USBH_TYPE_INT) 
        {
          c0|=_vQH(UFRAME_S,0x1);			/* set uframe S-mask if interrupt EP */
          if (hub_daddr) c0|=_vQH(UFRAME_C,0x0c);
        }
#endif
        _w32(aqh->ctrl1,c0);
        _w32(aqh->cqtd,0);
        _memset((void *)&aqh->qtd,0,sizeof(t_qtd));		/* clear qTD area */
        _w32(aqh->qtd.nqtd,_vQTD(T,1));					/* set nqTD to 1 */
    
        aqh_dsc->type=type;
        aqh_dsc->dir=dir;
        aqh_dsc->toggle=0;
        for (c0=0;psize!=(1<<c0) && c0<10;c0++)
          ;
        aqh_dsc->psize_sh=(hcc_u8)c0;
        aqh_dsc->used=1;
    
#if EHCI_PERIODIC_LIST_SIZE
        if (type==USBH_TYPE_INT)						/* PERIODIC QH */
        {
          hcc_u32 fle;
          hcc_u16 apos;
          hcc_u16 fpos;
    
          if (speed==EPS_HIGH) aqh_dsc->interval=(hcc_u16)(1<<(interval-1-3));
                          else aqh_dsc->interval=interval;

          _cb32(USBCMD,PSE);							/* temporarily disable periodic list */
          for (apos=0;apos<EHCI_PERIODIC_LIST_SIZE && *(pfl+apos)!=_EHCI_V32(_vQH(T,1));apos++)
            ;
          if (apos<EHCI_PERIODIC_LIST_SIZE)				/* empty entry available in PFL */
          {
            _w32(fle,_vQH(TYP,th_qh));
            _wADDR(fle,aqh);							/* set entry value */
            *(pfl+apos)=fle;							/* store entry on the 1st empty position */
            fpos=apos;									/* save first position */
            do {
              apos+=aqh_dsc->interval;
              for (;*(pfl+apos)!=_EHCI_V32(_vQH(T,1)) && apos<EHCI_PERIODIC_LIST_SIZE;apos++)
                ;
              if (apos<EHCI_PERIODIC_LIST_SIZE)
              {
                if (fpos+EHCI_PERIODIC_LIST_SIZE-apos<aqh_dsc->interval) break;
                *(pfl+apos)=fle;
              }
            } while (apos<EHCI_PERIODIC_LIST_SIZE);
            _sb32(USBCMD,PSE);							/* enable periodic list */
          }
          else
          {
            _sb32(USBCMD,PSE);							/* enable periodic list */
            aqh_dsc->used=0;
            rc=NULL;									/* no more entries in periodic list */
          }
        }
        else											/* ASYNC QH */
#endif
        {
          aqh_dsc->pqh=lqh;								/* store information in descriptor area */
          if (lqh==NULL)								/* if this will be the 1st QH */
          {
            _wADDR(ASYNCLISTADDR,aqh);					/* set ASYNCLISTADDR */
            _sb32(aqh->ctrl0,_vQH(H,1));				/* set head bit */
            _sb32(USBCMD,ASE);							/* and enable process of the list */
          }
          else
          {
            _wADDR(lqh->nqh,aqh);						/* otherwise link new QH to last QH */
          }
        }
      }
    }

    (void)os_mutex_put(ehci_mutex[EHCI_MUTEX_TRANSFER]);
  }
  
  return rc;
}



/*
** Remove all endpoints belonging to a device address
** INPUT: daddr - device address
** RERURN: USBH_SUCCESS
*/
int ehci_del_daddr (hcc_u8 daddr)
{
  int rc=USBH_ERR_NOT_AVAILABLE;
  hcc_uint i;
  t_qh *aqh;
  t_qh_dsc *aqh_dsc;
  hcc_u32 nqh_pos;

  if (ehci_hw_state()==USBH_ERR_SUSPENDED) return USBH_ERR_SUSPENDED;
  if (os_mutex_get(ehci_mutex[EHCI_MUTEX_TRANSFER])==OS_SUCCESS)
  {
#if EHCI_PERIODIC_LIST_SIZE
    _cb32(USBCMD,ASE);											/* disable ASYNC list */
    _cb32(USBCMD,PSE);											/* disable PERIODIC list */
#else
    USBCMD&=~ASE;
#endif
    while (_gb32(USBSTS,ASS))									/* wait ASYNC list to be diabled */
      ;
    for (i=0;i<TOTAL_EP;i++)
    {
      aqh=qh+i;
      aqh_dsc=qh_dsc+i;

      if (aqh_dsc->used && _rQH(aqh->ctrl0,DA)==daddr)			/* if QH needs to be deleted */
      {
        if (aqh!=qh)											/* delete not allowed for global DA0/EP0 QH */
        {
          if (aqh_dsc->transfer.state!=EHCI_TRST_STOPPED)		/* ongoing transfers */
          {
            t_qtd *aqtd;
            t_qtd_dsc *aqtd_dsc;
            aqtd=aqh_dsc->transfer.fqtd;						/* delete all qTD's belonging to this EP */
            while (aqtd)
            {
              aqtd_dsc=qtd_dsc+(aqtd-qtd);
              aqtd_dsc->qh=NULL;
#ifdef ehci_hw_unlock_buffer
              if (aqtd_dsc->ctbuf)
              {
                ehci_hw_unlock_buffer(aqtd_dsc->ctbuf,0)
                  ;
              }
#endif
              aqtd=_rADDR(aqtd->nqtd);
            }
            aqh_dsc->transfer.state=EHCI_TRST_STOPPED;
          }

          aqh_dsc->used=0;
#if EHCI_PERIODIC_LIST_SIZE
          if (aqh_dsc->type==USBH_TYPE_INT)
          {
            hcc_u16 i;
            for (i=0;i<EHCI_PERIODIC_LIST_SIZE;i++)
            {
              if (_rADDR(*(pfl+i))==aqh) _w32(*(pfl+i),_vQH(T,1));
            }
          }
          else
#endif
          {
            _wADDR(((t_qh *)(aqh_dsc->pqh))->nqh,_rADDR(aqh->nqh));	/* set previous next to current next pointer */
            nqh_pos=(hcc_u32)(((t_qh *)_rADDR(aqh->nqh))-qh);
            qh_dsc[nqh_pos].pqh=aqh_dsc->pqh;						/* set next QH prev pointer to previous QH */
          }
          rc=USBH_SUCCESS;
        }
      }
    }
#if EHCI_PERIODIC_LIST_SIZE
    _sb32(USBCMD,PSE);											/* reenable PERIDOC list */
#endif
    _sb32(USBCMD,ASE);											/* reenable ASYNC list */

    (void)os_mutex_put(ehci_mutex[EHCI_MUTEX_TRANSFER]);
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
int ehci_clear_toggle (void *ep_handle)
{
  int rc=USBH_ERR_NOT_AVAILABLE;
  hcc_uint f;

  if ((t_qh *)ep_handle>=qh && (t_qh *)ep_handle<(qh+QH_CNT))
  {
    if (os_mutex_get(ehci_mutex[EHCI_MUTEX_TRANSFER])==OS_SUCCESS)
    {
      f=(hcc_uint)((t_qh *)ep_handle-qh);
      (qh_dsc+f)->toggle=0;
      (void)os_mutex_put(ehci_mutex[EHCI_MUTEX_TRANSFER]);
      rc=USBH_SUCCESS;
    }
    else rc=USBH_ERR_RESOURCE;
  }
  
  return rc;
}



/*
** Transfer task
*/
OS_TASK_FN(ehci_transfer_task)
{
#if OS_TASK_POLL_MODE
  if (os_event_get(ehci_event_bit[EHCI_EVENT_TRANSFER])==OS_SUCCESS)
  {
#else
  for (;;)
  {
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
#if 0
    if (os_event_get(ehci_event_bit[EHCI_EVENT_TRANSFER])!=OS_SUCCESS) continue;
#endif
/*--- SCIOPTA */
#endif
    {
      t_qh *aqh;
      t_qh_dsc *aqh_dsc;
      t_qtd *aqtd;
      t_qtd_dsc *aqtd_dsc;
      t_usbh_transfer *uht;
      t_ehci_transfer_area *uht_et;
      hcc_u8 cb=0;

      if (os_mutex_get(ehci_mutex[EHCI_MUTEX_TRANSFER])==OS_SUCCESS)
      {
        for (aqh=qh,aqh_dsc=qh_dsc;aqh<qh+QH_CNT;aqh++,aqh_dsc++)
        {
          if (aqh_dsc->transfer.state==EHCI_TRST_RUNNING)	/* valid transfer found */
          {
            hcc_u32 tmp;
  
            aqtd=(t_qtd *)aqh_dsc->transfer.fqtd;
            uht=aqh_dsc->transfer.uht;
            uht_et=(t_ehci_transfer_area *)(uht->driver_area);

            tmp=ST_SPLITXSTATE;
            if (_rQH(aqh->ctrl0,EPS)==EPS_HIGH) tmp|=ST_PERR;
            if (_EHCI_V32(aqh->cqtd) && (_rQTD(aqh->qtd.ctrl0,STATUS)&~tmp)!=ST_ACTIVE)
            {
              aqh_dsc->transfer.state=EHCI_TRST_SUCCESS;
              /* check transfer descriptor states */
              do {
                aqtd_dsc=qtd_dsc+(aqtd-qtd);
                
                _r32(tmp,aqtd->ctrl0);
                tmp&=~_vQTD(STATUS,ST_DATA_BUFFER_ERROR);	/* not considered as transaction error */
                if (_rQTDv(tmp,STATUS)==0 || (_rQH(aqh->ctrl0,EPS)==EPS_HIGH && _rQTDv(tmp,STATUS)==ST_PERR))
                {											/* successful transfer */
                  if (_rQTDv(tmp,TB))
                  {
                    uht_et->ctsize-=_rQTDv(tmp,TB);
                    aqtd_dsc->ctsize-=_rQTDv(tmp,TB);
                    aqh_dsc->transfer.state=EHCI_TRST_SHORT_PACKET;	/* short packet, transfer finished */
                  }
                  
                  if (_rQTDv(tmp,PID)==_d_in)
                  {
                    if (aqtd_dsc->ctbuf)
                    {
#ifdef ehci_hw_unlock_buffer
                      ehci_hw_unlock_buffer(aqtd_dsc->ctbuf,aqtd_dsc->ctsize)
                        ;
#endif
                      aqtd_dsc->ctbuf=NULL;
                    }
                  }

                  if (tmp&_mQTD(DT)) aqh_dsc->toggle=1;
                                else aqh_dsc->toggle=0;		/* store toggle value */

                  if (_rQTDv(tmp,TB)) break;
                }
                else
                {
                  aqh_dsc->transfer.state=EHCI_TRST_ERROR;	/* transfer error */
                  break;
                }
                aqtd=_rADDR(aqtd->nqtd);
              } while (aqtd);

              uht->buf+=uht_et->ctsize;
              uht->csize+=uht_et->ctsize;
  
              _w32(aqh->qtd.nqtd,_vQH(T,1));
              aqh->cqtd=0;
              
              if (uht->cb_fn!=NULL)							/* callback function available */
              {
                if (aqh_dsc->transfer.state==EHCI_TRST_ERROR) uht->rc=USBH_ERR_TRANSFER;
                else if (aqh_dsc->transfer.state==EHCI_TRST_SHORT_PACKET) uht->rc=USBH_SHORT_PACKET;
                else uht->rc=USBH_SUCCESS;
                aqh_dsc->transfer.state=EHCI_TRST_CB;		/* set callback state */
                cb=1;
              }
            }
          }
        }
        (void)os_mutex_put(ehci_mutex[EHCI_MUTEX_TRANSFER]);

        if (cb)												/* call all callback functions */
        {
          for (aqh_dsc=qh_dsc;aqh_dsc<qh_dsc+QH_CNT;aqh_dsc++)
          {
            if (aqh_dsc->transfer.state==EHCI_TRST_CB)		/* call back transfer found */
            {
              ehci_del_qtd(aqh_dsc->transfer.fqtd);			/* delete all qTD's belonging to this EP */
              aqh_dsc->transfer.state=EHCI_TRST_STOPPED;	/* set stopped state */
              (aqh_dsc->transfer.uht->cb_fn)(aqh_dsc->transfer.uht->cb_param,aqh_dsc->transfer.uht->rc);
            }
          }
        }
      }
    }
  }
}



/*
** Get status of a transfer 
** INPUT: uht - transfer descriptor
**        mode - USBH_MODE_BLOCK/USBH_MODE_NON_BLOCK
** RETURN:USBH_SUCCESS
**        USBH_SHORT_PACKET
**        USBH_ERR_TRANSFER
**        USBH_ERR_TIMEOUT
**        USBH_ERR_NOT_COMPLETE
*/
int ehci_transfer_status (t_usbh_transfer *uht, hcc_u8 mode)
{
  int rc=USBH_SUCCESS;
  hcc_u32 cframe;
  t_ehci_transfer_area *uht_et=(t_ehci_transfer_area *)(uht->driver_area);
  t_qh_dsc *aqh_dsc=(t_qh_dsc *)uht_et->qh_dsc;
  t_qh *aqh=(t_qh *)uht->eph;

  for (;;)
  {
#if OS_TASK_POLL_MODE
    ehci_transfer_task();
#endif
    if (aqh_dsc->transfer.state!=EHCI_TRST_RUNNING)		/* transfer finished */
    {
      ehci_del_qtd(aqh_dsc->transfer.fqtd);
      if (aqh_dsc->transfer.state==EHCI_TRST_ERROR) rc=USBH_ERR_TRANSFER;
      else if (aqh_dsc->transfer.state==EHCI_TRST_STOPPED) rc=USBH_ERR_TRANSFER;
      else if (aqh_dsc->transfer.state==EHCI_TRST_SHORT_PACKET) rc=USBH_SHORT_PACKET;
      aqh_dsc->transfer.state=EHCI_TRST_STOPPED;
      if (rc==USBH_SUCCESS && uht->size!=uht->csize)
      {
        rc=ehci_add_qtd(uht);							/* build next qTD chain if transfer not finished */
        if (rc!=USBH_SUCCESS && rc!=USBH_ERR_TRANSFER_FULL) break;
      }
      else
      {
        break;
      }
    }

    if (uht->timeout!=USBH_WAIT_FOREVER)				/*check timeout if needed */
    {
/*+++ SCIOPTA */
      /* Needed to allow transfer-task to run */
      sc_sleep(2);
/*--- SCIOPTA */
      
      if (os_mutex_get(ehci_mutex[EHCI_MUTEX_TRANSFER])==OS_SUCCESS)
      {
        cframe=ehci_get_frame_number();
        if (cframe!=(aqh_dsc->transfer.cframe))
        {
          hcc_u32 d;
          if (cframe<aqh_dsc->transfer.cframe) cframe|=(_mREG(FI)+1);
          d=(cframe-aqh_dsc->transfer.cframe)>>3;
          if (aqh_dsc->transfer.timeout<d)				/* timed out */
          {
            _wQTD(aqh->qtd.ctrl0,STATUS,ST_HALTED);
            _sb32(aqh->qtd.nqtd,_vQTD(T,1));			/* suspend transfer */
            aqh->cqtd=0;
/*+++ SCIOPTA */
	    os_requestDoorbell(); /* request and wait for doorbell */
#if 0
            _sb32(USBCMD,IOAAD);			/* request doorbell */
            /* wait doorbell request to finish */
            kprintf(1,"wait for doorbell\n");
#if OS_TASK_POLL_MODE
            while (os_event_get(ehci_event_bit[EHCI_EVENT_IOAA])!=OS_SUCCESS)
              ;
#else
            if (os_event_get(ehci_event_bit[EHCI_EVENT_IOAA])==OS_SUCCESS)
#endif
#endif
/*--- SCIOPTA */

            {
              (void)os_mutex_put(ehci_mutex[EHCI_MUTEX_TRANSFER]);	/* release mutex */
#if OS_TASK_POLL_MODE
              ehci_transfer_task();
#endif
              /* recheck tranfer in case it has been completed while the tranfer was suspended */
              if (aqh_dsc->transfer.state!=EHCI_TRST_RUNNING) continue; /* completed */
              aqh_dsc->transfer.state=EHCI_TRST_STOPPED;
              ehci_del_qtd(aqh_dsc->transfer.fqtd);
              rc=USBH_ERR_TIMEOUT;
              break;
            }
          }
          else
          {
            aqh_dsc->transfer.timeout-=d;
            aqh_dsc->transfer.cframe=cframe&_mREG(FI);
          }
        }
        (void)os_mutex_put(ehci_mutex[EHCI_MUTEX_TRANSFER]);	/* release mutex */
      }
      else 
      {
        rc=USBH_ERR_RESOURCE;
        break;
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
int ehci_transfer (t_usbh_transfer *uht, hcc_u8 mode)
{
  int rc;

  uht->csize=0;
  if (mode==USBH_MODE_BLOCK)
  {
    if (ehci_hw_state()==USBH_ERR_SUSPENDED) return USBH_ERR_SUSPENDED;
    rc=ehci_add_qtd(uht);									/* build qTD chain */
    if (rc==USBH_SUCCESS || rc==USBH_ERR_TRANSFER_FULL) 
    {
      rc=ehci_transfer_status(uht,mode);
    }
  }
  else
  {
    rc=ehci_add_qtd(uht);									/* build qTD chain */
  }

  return rc;
}


/*
** Delay ms number of milliseconds
*/
void ehci_delay (hcc_u32 ms)
{
/*+++ SCIOPTA */
  sc_sleep(sc_tickMs2Tick(ms));
#if 0
  hcc_u32 frame,pframe;
  ++ms;
  _r32(pframe,FRINDEX);
  pframe&=~7;
  while (ms)
  {
    _r32(frame,FRINDEX);
    frame&=~7;
    if (frame!=pframe)
    {
      --ms;
      pframe=frame;
    }
  }
#endif
/*--- SCIOPTA */
}


/*
** Set speed,HUB address and HUB speed for endpoint0
*/
int ehci_set_ep0_speed_hub (void *ep0_handle, hcc_u8 speed, hcc_u8 hub_daddr, hcc_u8 hub_speed)
{
  int rc=USBH_ERR_NOT_AVAILABLE;
  hcc_uint f;
  hcc_u32 c;

  if (ehci_hw_state()==USBH_ERR_SUSPENDED) return USBH_ERR_SUSPENDED;
  if (ep0_handle==NULL) ep0_handle=qh;
  if ((t_qh *)ep0_handle>=qh && (t_qh *)ep0_handle<(qh+QH_CNT))
  {
    if (os_mutex_get(ehci_mutex[EHCI_MUTEX_TRANSFER])==OS_SUCCESS)
    {
      f=(hcc_uint)((t_qh *)ep0_handle-qh);

      _cb32((qh+f)->ctrl0,_mQH(EPS));
      if (speed==USBH_LOW_SPEED) _sb32((qh+f)->ctrl0,_vQH(EPS,EPS_LOW));
      else if (speed==USBH_HIGH_SPEED) _sb32((qh+f)->ctrl0,_vQH(EPS,EPS_HIGH));
      /* EPF_FULL is 0, so don't do anything when port is full speed */

      if (hub_speed==USBH_HIGH_SPEED && hub_daddr)
      {
        _r32(c,(qh+f)->ctrl1);
        c|=(hcc_u32)_vQH(HUBADDR,hub_daddr);
        c|=_vQH(PORTNUM,pmgr_get_port_number(hub_daddr));
        _w32((qh+f)->ctrl1,c);
      }
      
      (void)os_mutex_put(ehci_mutex[EHCI_MUTEX_TRANSFER]);
      rc=USBH_SUCCESS;
    }
    else rc=USBH_ERR_RESOURCE;
  }
  
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
int ehci_get_ep_inf (void *ep_handle, hcc_u8 *daddr, hcc_u8 *ep, hcc_u8 *type, hcc_u8 *dir, hcc_u16 *psize)
{
  int rc=USBH_SUCCESS;
  hcc_uint f;

  if ((t_qh *)ep_handle>=qh && (t_qh *)ep_handle<(qh+QH_CNT))
  {
    f=(hcc_uint)((t_qh *)ep_handle-qh);
    if (daddr) *daddr=(hcc_u8)(_rQH((qh+f)->ctrl0,DA));
    if (ep) *ep=(hcc_u8)(_rQH((qh+f)->ctrl0,ENDPT));
    if (type) *type=(qh_dsc+f)->type;
    if (psize) *psize=(hcc_u16)(1<<((qh_dsc+f)->psize_sh));
    if (dir)
    {
      switch ((qh_dsc+f)->dir)
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
** Init EHCI
*/
int ehci_init (void)
{
  int rc;

  rc=ehci_hw_init();/* init EHCI HW */
  if (rc) return rc;
  
  if (os_mutex_create(&ehci_mutex[EHCI_MUTEX_TRANSFER])) return USBH_ERR_RESOURCE;
/*+++ SCIOPTA */
#if 0
  if (os_event_create(&ehci_event_bit[EHCI_EVENT_TRANSFER])) return USBH_ERR_RESOURCE;
  if (os_event_create(&ehci_event_bit[EHCI_EVENT_IOAA])) return USBH_ERR_RESOURCE;
#endif
/*--- SCIOPTA */
  
  rc=ehci_hub_init();
  if (rc) return rc;

  return USBH_SUCCESS;
}



/*
** Start EHCI controller
*/
int ehci_start (hcc_u32 itc)
{
  int rc;
  unsigned int i;
  hcc_u32 pfls=FLS_1024;

  rc=ehci_hw_start();	/* init EHCI HW */
  if (rc) return rc;

#if EHCI_PERIODIC_LIST_SIZE
  pfl=(hcc_u32 *)ALIGN4096(ehci_buffer);
  qh=(t_qh *)((hcc_u8 *)pfl+PFL_SIZE);
#else
  pfl=NULL;
  qh=(t_qh *)ALIGN32(ehci_buffer);
#endif
  qtd=(t_qtd *)((hcc_u8 *)qh+QH_SIZE);

#if EHCI_PERIODIC_LIST_SIZE
  if (EHCI_PERIODIC_LIST_SIZE!=1024)
  {
    if (EHCI_PERIODIC_LIST_SIZE==256) pfls=FLS_256;
                                 else pfls=FLS_512;

    if (_gb32(HCCPARAMS,PFLF)==0) return USBH_ERR_PERIODIC_LIST;
  }
#endif

  _memset(ehci_buffer,0,EHCI_BUFFER_SIZE);				/* assign and clear QH and qTD buffers */
  _memset(qh_dsc,0,QH_DSC_SIZE);
  _memset(qtd_dsc,0,QTD_DSC_SIZE);

  
  /* reset controller */
  /*
  _w32(USBCMD,HCRESET);
  while (_gb32(USBCMD,HCRESET))
    ;
  */
  _w32(USBCMD,_vF(ASPMC,3)|_vF(ITC,itc)|_vF(FLS,pfls));	/* FLS=pfls, ASPMC=3, ITC=itc */

#if EHCI_PERIODIC_LIST_SIZE
  for (i=0;i<EHCI_PERIODIC_LIST_SIZE;*(pfl+i++)=_EHCI_V32(_vQH(T,1)))
    ;
  _wADDR(PERIODICLISTBASE,pfl);							/* set periodic list base */
#endif

#if OS_INTERRUPT_ENABLE
  _w32(USBINTR,UIE|UEIE|PCIE|IOAAE);					/* enable EHCI intrerupts */
#endif
  USBSTS=USBSTS;
  _sb32(USBCMD,RS);										/* set run bit */

  _w32(CONFIGFLAG,1);									/* route all ports to EHCI by default */

  if (ehci_add_ep(0,0,USBH_TYPE_CONTROL,USBH_DIR_ANY,8,0)==NULL)
  {														/* add general control endpoint */
    return USBH_ERR_HOST;
  }
  
  rc=ehci_hub_start();									/* start root HUB */
  if (rc) return rc;
 
  /* os_isr_init is intentionally here (not in ehci_init) because it may happen that the same IRQ
     vector is shared. E.g.: between USB host and device */
/*+++ SCIOPTA */
#if 0 /* not used with SCIOPTA */
  rc=os_isr_init(EHCI_HOST_ISR, ehci_isr_handler);
  if (rc) return rc;
#endif
/*--- SCIOPTA */
  rc=os_isr_enable(EHCI_HOST_ISR);
  if (rc) return rc;

  return USBH_SUCCESS;
}



/*
** Stop EHCI controller
*/
int ehci_stop (void)
{
  int rc;

  rc=os_isr_disable(EHCI_HOST_ISR);					/* disable interrupts */
  if (rc) return rc;

  rc=ehci_hub_stop();								/* stop HUB */
  if (rc) return rc;

  _w32(USBCMD,HCRESET);								/* reset controller */
  while (_gb32(USBCMD,HCRESET))
    ;

  rc=ehci_hw_stop();
  if (rc) return rc;

  return USBH_SUCCESS;
}

