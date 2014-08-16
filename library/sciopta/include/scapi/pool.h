/*SOC
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
** (c) 2006 Sciopta Systems GmbH Deutschland, Sciopta AG/ Schweiz   **
**                                                                  **
**********************************************************************
** ID: S06055BS5                                                    **
** +Revision: 1.7 +                                                 **
** +Date: 2006/10/11 14:01:40 +                                     **
** Pool definitions and API                                         **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_POOL_H_
#define _KERNEL_POOL_H_ 2

#define SC_POOL_NAME_SIZE 31

#ifdef _SC_CONF_H_
struct sc_pool_stat_s {
  __u32 cnt_req[SC_MAX_NUM_BUFFERSIZES];   /* # requests for a spec. size */
  __u32 cnt_alloc[SC_MAX_NUM_BUFFERSIZES]; /* # allocation of a spec. size */
  __u32 cnt_free[SC_MAX_NUM_BUFFERSIZES];  /* # releases of a spec. size */
  __u32 cnt_wait[SC_MAX_NUM_BUFFERSIZES];  /* # unfullfilled allocations */
  sc_bufsize_t maxalloc[SC_MAX_NUM_BUFFERSIZES]; /* largest wanted size */
};
typedef struct sc_pool_stat_s sc_pool_stat_t;

struct sc_pool_cb_s{
  sc_poolid_t id;                   /* id of this pool */
  sc_pid_t creator;                 /* id of creating process */
  sc_plsize_t size;                 /* complete size */
  sc_bufsize_t bufsize[SC_MAX_NUM_BUFFERSIZES];
  unsigned int nbufsizes;
  long int lock;                /* == 0 => not locked */
  char *start;                      /* start of pool */
  char *end;                        /* end of pool (first byte, not in pool) */
  char *cur;                        /* first free byte inside pool */
  dbl_t freed[SC_MAX_NUM_BUFFERSIZES];  /* list of free'd buffers */
  dbl_t waiter[SC_MAX_NUM_BUFFERSIZES]; /* list of processes waiting for a buffer */
  sc_flags_t flags;
  char name[SC_POOL_NAME_SIZE+1];
#if SC_MSG_STAT == 1
  sc_pool_stat_t stat;
#endif
#ifdef WIN32
  /*
  ** win32 specific
  */
  CRITICAL_SECTION poolLock;
#else
  pthread_mutex_t poolLock;
#endif
};
typedef struct sc_pool_cb_s sc_pool_cb_t;
#endif /* _SCONF_H_ */

#define SC_DEFAULT_POOL ((sc_poolid_t)(0x100))
#define SC_ILLEGAL_POOLID ((sc_poolid_t)(-1))

#if SC_USE_SYSCALL == 0
SCFUNC6(sc_poolid_t,sc_sysPoolCreate,
	char *,
	sc_plsize_t ,
	unsigned int,
	const sc_bufsize_t *,
	const char *,
	sc_moduleid_t );
#endif
SCFUNC5(DLLEXP sc_poolid_t,sc_poolCreate,
	char *,
	sc_plsize_t,
	unsigned int,
	const sc_bufsize_t *,
	const char *);


SCFUNC1(DLLEXP sc_poolid_t,sc_poolIdGet,const char *);
SCFUNC1(DLLEXP void,sc_sysPoolCBdump,sc_poolid_t);
SCFUNC3(DLLEXP int,sc_poolInfo,int,sc_poolid_t,sc_pool_cb_t NEARPTR);
#ifndef __KERNEL__
#define sc_sysPoolCreate(start,size,nbufsize,bufsize,name,module)\
   _FUNC6(sc_sysPoolCreate,start,size,nbufsize,bufsize,name,module)
#define sc_poolCreate(start,size,nbufsize,bufsize,name) _FUNC5(sc_poolCreate,start,size,nbufsize,bufsize,name)
#define sc_poolIdGet(name) _FUNC1(sc_poolIdGet,name)
#define sc_sysPoolCBdump(poolid) _FUNC1(sc_sysPoolCBdump,poolid)
#define sc_poolInfoGet(mid,poolid,poolcb) _FUNC3(sc_poolInfoGet,mid,poolid,poolcb)

#endif /* __KERNEL__ */

#endif /*_KERNEL_POOL_H_*/
