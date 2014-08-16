/*SOC -*-c++-*-
**********************************************************************
**  _______  _______ _________ _______  _______ _________ _______   **
** (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
** | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
** | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
** (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
**       ) || |         | |   | |   | || (         | |   | (   ) |  **
** /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
** \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
**                                                                  **
** (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
**                                                                  **
**********************************************************************
** ID: S01103BS46                                                   **
** +Revision: 1.13 +                                                **
** +Date: 2008/01/16 13:02:02 +                                     **
** Message pool API and definitions                                 **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif
#ifndef _KERNEL_POOL_H_
#define _KERNEL_POOL_H_
/* SOCNV */
#define SC_POOL_NAME_SIZE 31
/* EOCNV */
#ifdef _SC_CONF_H_
/* SOCNV */
#define SC_SET_POOLCREATE_HOOK (1)
#define SC_SET_POOLKILL_HOOK (2)
/* EOCNV */
#endif

#ifndef __ASM_INCLUDED__

#ifdef _SC_CONF_H_
#if SIZEOF_PTR == 2
struct sc_pool_stat_s {
  __u16 cnt_req[SC_MAX_NUM_BUFFERSIZES];   /* No. requests for a spec. size */
  __u16 cnt_alloc[SC_MAX_NUM_BUFFERSIZES]; /* No. allocation of a spec. size */
  __u16 cnt_free[SC_MAX_NUM_BUFFERSIZES];  /* No. releases of a spec. size */
  __u16 cnt_wait[SC_MAX_NUM_BUFFERSIZES];  /* No. unfullfilled allocations */
  sc_bufsize_t maxalloc[SC_MAX_NUM_BUFFERSIZES]; /* largest wanted size */
};
typedef struct sc_pool_stat_s sc_pool_stat_t;
#else
struct sc_pool_stat_s {
  __u32 cnt_req[SC_MAX_NUM_BUFFERSIZES];   /* No. requests for a spec. size */
  __u32 cnt_alloc[SC_MAX_NUM_BUFFERSIZES]; /* No. allocation of a spec. size */
  __u32 cnt_free[SC_MAX_NUM_BUFFERSIZES];  /* No. releases of a spec. size */
  __u32 cnt_wait[SC_MAX_NUM_BUFFERSIZES];  /* No. unfullfilled allocations */
  sc_bufsize_t maxalloc[SC_MAX_NUM_BUFFERSIZES]; /* largest wanted size */
};
typedef struct sc_pool_stat_s sc_pool_stat_t;
#endif

struct sc_pool_cb_s{
  sc_bufsize_t bufsize[SC_MAX_NUM_BUFFERSIZES];
  sgl_t freed[SC_MAX_NUM_BUFFERSIZES];  /* list of free'd buffers */
  /* lst of process waiting for a buffer */
  __u8 waiterHead[SC_MAX_NUM_BUFFERSIZES]; 
  __u8 waiterTail[SC_MAX_NUM_BUFFERSIZES];
  sc_poolid_t id;                   /* id of this pool */
  sc_plsize_t size;                 /* complete size */

#if SIZEOF_PTR == 2 && CPU != MSP430
  __u8 nbufsizes;
#else
  int nbufsizes;
#endif
  int lock;                         /* == 0 => not locked */
  char *start;                      /* start of pool */
  char *end;                        /* end of pool (first byte, not in pool) */
  char *cur;                        /* first free byte inside pool */
  const char FARPTR name;
#if SC_MSG_STAT == 1
  sc_pool_stat_t stat;
#endif
};
typedef struct sc_pool_cb_s sc_pool_cb_t;
#endif /* _SC_CONF_H_ */
#define POOL_MASK ((sc_poolid_t)(0x7fffffff))
#define POOLID2IDX(plid) ((plid) & (sc_poolid_t)(0x000000ff))
#define SC_DEFAULT_POOL ((sc_poolid_t)(0x100))
#define SC_ILLEGAL_POOLID ((sc_poolid_t)(-1))

SCFUNC5(sc_poolid_t,sc_poolCreate,char NEARPTR,sc_plsize_t,unsigned int,sc_bufsize_t FARPTR,const char FARPTR);
SCFUNC1(void,sc_poolReset,sc_poolid_t);
SCFUNC1(sc_poolid_t,sc_poolDefault,int idx);
SCFUNC1(sc_poolid_t,sc_poolIdGet,const char FARPTR);
SCFUNC1(void,sc_poolKill,sc_poolid_t);
#ifdef _SC_CONF_H_
typedef void (sc_poolHook_t)(const sc_pool_cb_t NEARPTR);
SCFUNC2(sc_poolHook_t *,sc_poolHookRegister,int,sc_poolHook_t *);
SCFUNC3(int,sc_poolInfo,int,sc_poolid_t,sc_pool_cb_t NEARPTR);
#endif /* _SC_CONF_H_ */

#ifndef __KERNEL__
#define sc_poolCreate(a,b,c,d,e) _FUNC5(sc_poolCreate,a,b,c,d,e)
#define sc_poolReset(a)          _FUNC1(sc_poolReset,a)
#define sc_poolDefault(a)        _FUNC1(sc_poolDefault,a)
#define sc_poolIdGet(a)          _FUNC1(sc_poolIdGet,a)
#define sc_poolKill(a)           _FUNC1(sc_poolKill,a)
#ifdef _SC_CONF_H_
#define sc_poolHookRegister(a,b) _FUNC2(sc_poolHookRegister,a,b)
#define sc_poolInfo(a,b,c)       _FUNC3(sc_poolInfo,a,b,c)
#endif
#endif /* __KERNEL__ */

#else  /* __ASM_INCLUDED__ */
#ifdef _SC_CONF_H_
/* SOCNV */
#define SC_DEFAULT_POOL (0x100)
#define SC_ILLEGAL_POOLID (-1)
/* EOCNV */
#if SIZEOF_PTR == 2
/* SOCNV */
#define POOL_BUFSIZE    0x00
#define POOL_FREED      (POOL_BUFSIZE+(2*SC_MAX_NUM_BUFFERSIZES))
#define POOL_WAITER     (POOL_FREED+(2*SC_MAX_NUM_BUFFERSIZES))
#define POOL_ID         (POOL_WAITER+(2*SC_MAX_NUM_BUFFERSIZES))
#define POOL_SIZE       (POOL_ID+2)
#if CPU == MSP430
#define POOL_NBUFSIZES  (POOL_SIZE+2)
#define POOL_LOCK       (POOL_NBUFSIZES_B+2)
#else
#define POOL_NBUFSIZES_B  (POOL_SIZE+2)
#define POOL_LOCK       (POOL_NBUFSIZES_B+1)
#endif
#define POOL_START      (POOL_LOCK+2)
#define POOL_END        (POOL_START+2)
#define POOL_CUR        (POOL_END+2)
#define POOL_NAME       (POOL_CUR+2)
#if CPU == M16C
#define POOL_STAT       (POOL_NAME + 2 + BANK_OFF)
#else
#define POOL_STAT       (POOL_NAME + 2)
#endif
#define POOL_STAT_REQ   (POOL_STAT)
#define POOL_STAT_ALLOC (POOL_STAT_REQ+(2*SC_MAX_NUM_BUFFERSIZES))
#define POOL_STAT_FREE  (POOL_STAT_ALLOC+(2*SC_MAX_NUM_BUFFERSIZES))
#define POOL_STAT_WAIT  (POOL_STAT_FREE+(2*SC_MAX_NUM_BUFFERSIZES))
#define POOL_STAT_MAX   (POOL_STAT_WAIT+(2*SC_MAX_NUM_BUFFERSIZES))
#if SC_MSG_STAT == 1
#define SIZEOF_POOL_CB (POOL_STAT_MAX+(2*SC_MAX_NUM_BUFFERSIZES))
#define SIZEOF_SC_POOL_STAT (SC_MAX_NUM_BUFFERSIZES*2*5)
#else
#define SIZEOF_POOL_CB (POOL_NAME+2)
#endif /* SC_MSG_STAT */
/* EOCNV */
#else /* SIZEOF_PTR */
#define POOL_BUFSIZE    0x00
#define POOL_FREED      (POOL_BUFSIZE+4*SC_MAX_NUM_BUFFERSIZES)
#define POOL_WAITER     (POOL_FREED+8*SC_MAX_NUM_BUFFERSIZES)

#define POOL_ID         (POOL_WAITER+8*SC_MAX_NUM_BUFFERSIZES)
#define POOL_SIZE       (POOL_ID+4)
#define POOL_NBUFSIZES  (POOL_SIZE+4)
#define POOL_LOCK       (POOL_NBUFSIZES_B+4)
#define POOL_START      (POOL_LOCK+4)
#define POOL_END        (POOL_START+4)
#define POOL_CUR        (POOL_END+4)
#define POOL_NAME       (POOL_CUR+4)
#define POOL_STAT       (POOL_NAME+4)
#define POOL_STAT_REQ   (POOL_STAT)
#define POOL_STAT_ALLOC (POOL_STAT_REQ+4*SC_MAX_NUM_BUFFERSIZES)
#define POOL_STAT_FREE  (POOL_STAT_ALLOC+4*SC_MAX_NUM_BUFFERSIZES)
#define POOL_STAT_WAIT  (POOL_STAT_FREE+4*SC_MAX_NUM_BUFFERSIZES)
#define POOL_STAT_MAX   (POOL_STAT_WAIT+4*SC_MAX_NUM_BUFFERSIZES)
#if SC_MSG_STAT == 1
#define SIZEOF_POOL_CB (POOL_STAT_MAX+4*SC_MAX_NUM_BUFFERSIZES)
#define SIZEOF_SC_POOL_STAT (SC_MAX_NUM_BUFFERSIZES*4*5)
#else
#define SIZEOF_POOL_CB (POOL_NAME+4)
#endif /* SC_MSG_STAT */
#endif /* SIZEOF_PTR */
#endif /* _SC_CONF_H_ */
#endif /* __ASM_INCLUDED__ */
#endif /* _KERNEL_POOL_H_ */
/*
****************************************************************************
** $Log: pool.h,v $
** Revision 1.13  2008/01/16 13:02:02  bastian
** merge from 1.9.1.12
**
** Revision 1.12.6.1  2007/09/14 07:40:13  bastian
** fixes for nc30
**
** Revision 1.12  2005/09/20 09:43:15  bastian
** added const
**
** Revision 1.11  2004/11/05 14:19:45  bastian
** fix
**
** Revision 1.10  2004/10/19 07:59:02  bastian
** fixes
**
** Revision 1.9  2004/10/07 07:37:47  bastian
** fixed NEAR * to NEARPTR
**
** Revision 1.8  2004/09/30 08:33:56  bastian
** fixes for m16c
**
** Revision 1.7  2004/05/25 05:02:54  bastian
** fix for MSP430
**
** Revision 1.6  2004/01/12 10:54:49  bastian
** POOL_NAME not taken into .inc
**
** Revision 1.5  2003/10/23 11:36:58  bastian
** added sc_poolKill
**
** Revision 1.4  2003/08/04 08:36:20  bastian
** renamne sysPoolCreate => poolCreate
**
** Revision 1.3  2003/07/04 06:27:47  bastian
** various
**
** Revision 1.2  2003/06/10 06:53:34  bastian
** free list needs no tail
**
** Revision 1.1  2003/05/19 12:06:05  bastian
** 1st
**
****************************************************************************
*/
