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
** +Revision: 2.36.4.1 +                                                **
** +Date: 2009/10/04 09:53:43 +                                     **
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
#ifdef _SC_CONF_H_
#define SC_SET_POOLCREATE_HOOK (1)
#define SC_SET_POOLKILL_HOOK (2)
#endif
/* EOCNV */
#ifndef __ASM_INCLUDED__

#ifdef _SC_CONF_H_
struct sc_pool_stat_s {
  __u32 cnt_req[SC_MAX_NUM_BUFFERSIZES];     /* No. requests for a spec. size */
  __u32 cnt_alloc[SC_MAX_NUM_BUFFERSIZES];   /* No. allocation of a spec. size */ 
  __u32 cnt_free[SC_MAX_NUM_BUFFERSIZES];    /* No. releases of a spec. size */
  __u32 cnt_wait[SC_MAX_NUM_BUFFERSIZES];    /* No. unfullfilled allocations */
  sc_bufsize_t maxalloc[SC_MAX_NUM_BUFFERSIZES]; /* largest wanted size */
};
typedef struct sc_pool_stat_s sc_pool_stat_t;

struct sc_pool_cb_s{
  sc_poolid_t id;                   /* id of this pool */
  sc_pid_t creator;                 /* id of creating process */
  sc_plsize_t size;                 /* complete size */
  sc_bufsize_t bufsize[SC_MAX_NUM_BUFFERSIZES];
  unsigned int nbufsizes;
  unsigned int lock;                /* == 0 => not locked */
  char *start;                      /* start of pool */
  char *end;                        /* end of pool (first byte, not in pool) */
  char *cur;                        /* first free byte inside pool */
  dbl_t freed[SC_MAX_NUM_BUFFERSIZES];  /* list of free'd buffers */
  dbl_t waiter[SC_MAX_NUM_BUFFERSIZES]; /* list of processes waiting for a buffer */
  flags_t flags;
  char name[SC_POOL_NAME_SIZE+1];
#if SC_MSG_STAT == 1
  sc_pool_stat_t stat;
#endif
};
typedef struct sc_pool_cb_s sc_pool_cb_t;
#endif /* _SC_CONF_H_ */
#define POOL_IS_TARGET(plid) ((plid) & 0x80000000)
#define POOL_MASK ((sc_poolid_t)(0x7fffffff))
#define POOL_SET_TARGET(plid) ((plid) | 0x80000000)
#define POOLID2IDX(plid) ((plid) & (sc_poolid_t)(0x000000ff))
#define SC_DEFAULT_POOL ((sc_poolid_t)(0x100))
#define SC_ILLEGAL_POOLID ((sc_poolid_t)(-1))

#if SC_USE_SYSCALL == 0
SCFUNCn(sc_poolid_t,sc_sysPoolCreate,
	char *,
	sc_plsize_t ,
	unsigned int,
	const sc_bufsize_t *,
	const char *,
	sc_moduleid_t );
#endif

SCFUNCn(sc_poolid_t,sc_poolCreate,
	char *,
	sc_plsize_t,
	unsigned int,
	const sc_bufsize_t *,
	const char *);
SCFUNCn(void,sc_poolReset,sc_poolid_t);
SCFUNCn(void,sc_poolKill,sc_poolid_t);
SCFUNCn(sc_poolid_t,sc_poolDefault,int);
SCFUNCn(sc_poolid_t,sc_poolIdGet,const char *);
#ifdef _SC_CONF_H_
typedef void (sc_poolHook_t)(const sc_pool_cb_t *);
SCFUNCn(sc_poolHook_t *,sc_poolHookRegister,int,sc_poolHook_t *);
SCFUNCn(int,sc_poolInfo,sc_moduleid_t,sc_poolid_t,sc_pool_cb_t *);
#endif /* _SC_CONF_H_ */

#ifndef __KERNEL__
#define sc_sysPoolCreate(start,size,nbufsize,bufsizes,name,moduleid) \
                                           _FUNCn(sc_sysPoolCreate,start,size,nbufsize,bufsizes,name,moduleid)
#define sc_poolCreate(start,size,nbufsize,bufsizes,name) \
                                           _FUNCn(sc_poolCreate,start,size,nbufsize,bufsizes,name)
#define sc_poolReset(poolidx)              _FUNCn(sc_poolReset,poolidx)
#define sc_poolKill(poolidx)               _FUNCn(sc_poolKill,poolidx)
#define sc_poolDefault(poolidx)            _FUNCn(sc_poolDefault,poolidx)
#define sc_poolIdGet(name)                 _FUNCn(sc_poolIdGet,name)
#ifdef _SC_CONF_H_
#define sc_poolHookRegister(hook,flag)      _FUNCn(sc_poolHookRegister,hook,flag)
#define sc_poolInfo(moduleid,poolid,poolcb) _FUNCn(sc_poolInfo,moduleid,poolid,poolcb)
#endif
#endif /* __KERNEL__ */

#else  /* __ASM_INCLUDED__ */
#ifdef _SC_CONF_H_
/* SOCNV */
#define SC_DEFAULT_POOL (0x100)
#define SC_ILLEGAL_POOLID (-1)
#define POOL_ID         0x00
#define POOL_CREATOR    0x04
#define POOL_SIZE       0x08
#define POOL_BUFSIZE    0x0c
#define POOL_NBUFSIZES  (POOL_BUFSIZE+4*SC_MAX_NUM_BUFFERSIZES)
#define POOL_LOCK       (POOL_NBUFSIZES+4)
#define POOL_START      (POOL_LOCK+4)
#define POOL_END        (POOL_START+4)
#define POOL_CUR        (POOL_END+4)
#define POOL_FREED      (POOL_CUR+4)
#define POOL_WAITER     (POOL_FREED+8*SC_MAX_NUM_BUFFERSIZES)
#define POOL_FLAGS      (POOL_WAITER+8*SC_MAX_NUM_BUFFERSIZES)
#define POOL_NAME       (POOL_FLAGS+4)
#define POOL_STAT       (POOL_NAME + SC_POOL_NAME_SIZE+1)
#define POOL_STAT_REQ   (POOL_STAT)
#define POOL_STAT_ALLOC (POOL_STAT_REQ+4*SC_MAX_NUM_BUFFERSIZES)
#define POOL_STAT_FREE  (POOL_STAT_ALLOC+4*SC_MAX_NUM_BUFFERSIZES)
#define POOL_STAT_WAIT  (POOL_STAT_FREE+4*SC_MAX_NUM_BUFFERSIZES)
#define POOL_STAT_MAX   (POOL_STAT_WAIT+4*SC_MAX_NUM_BUFFERSIZES)
#if SC_MSG_STAT == 1
#define SIZEOF_POOL_CB (POOL_STAT_MAX+4*SC_MAX_NUM_BUFFERSIZES)
#define SIZEOF_SC_POOL_STAT (SC_MAX_NUM_BUFFERSIZES*4*5)
#else
#define SIZEOF_POOL_CB (POOL_STAT)
#endif /* SC_MSG_STAT */
/* EOCNV */
#endif /* _SC_CONF_H_ */
#endif /* __ASM_INCLUDED__ */
#endif /* _KERNEL_POOL_H_ */

