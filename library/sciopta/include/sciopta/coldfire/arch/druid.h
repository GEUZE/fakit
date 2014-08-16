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
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S04281CL4                                                    **
** +Revision: 1.9 +                                                 **
** +Date: 2006/01/19 17:30:36 +                                     **
** Druid device macros                                              **
**********************************************************************
EOC*/


/**
 * Global definitions
 */
#if SC_MAX_CONNECTOR > 0 
  #define DRUID_DECLARATIONS \
    extern sciopta_t sciopta; \
    extern sc_module_cb_t *sc_modules[]; \
    extern sc_pcb_t *sc_connectors[]; 
#else
  #define DRUID_DECLARATIONS \
  extern sciopta_t sciopta; \
  extern sc_module_cb_t *sc_modules[];
#endif 

/**
 * Module access definition
 */
#define DRUID_MODULE_MAX \
  SC_MAX_MODULE 

#define DRUID_MODULE_CUR \
  ((__u32 ) sciopta.curModule)

#define DRUID_MODULE_CHK_INDEX(module) \
  ((__u32 ) sc_modules[module] != 0 && \
   (__u32 ) sc_modules[module] != 1)

#define DRUID_MODULE_CB(module) \
  ((__u32 ) sc_modules[module])

#define DRUID_MODULE_NAME(module) \
  ((char *) sc_modules[module]->name)
  
#define DRUID_MODULE_PRIO(module) \
  (sc_modules[module]->prio)

#define DRUID_MODULE_NPROCESS(module) \
  (sc_modules[module]->nprocess)

#define DRUID_MODULE_NPOOLS(module) \
  (sc_modules[module]->npools)

#define DRUID_MODULE_TEXT(module) \
  ((__u32 ) sc_modules[module]->text)

#define DRUID_MODULE_DATA(module) \
  ((__u32 ) sc_modules[module]->data)

#define DRUID_MODULE_SIZE(module) \
  (sc_modules[module]->size)

#define DRUID_MODULE_FREESIZE(module) \
  (sc_modules[module]->freesize)



/**
 * Process access definition
 */
#define DRUID_PROC_MAX(module) \
  (sc_modules[module]->max_process)

#define DRUID_PROC_CUR \
  ((__u32 ) ((__u8 *) sciopta.curPcb - PCB_PCBLIST))

#define DRUID_PROC_CHK_INDEX(module, process) \
  ((__u32 ) sc_modules[module]->pcbs[process] != 0 && \
   (__u32 ) sc_modules[module]->pcbs[process] != 1)

#define DRUID_PROC_CB(module, process) \
  ((__u32 ) ((__u8 *) sc_modules[module]->pcbs[process] - PCB_PCBLIST))

#define DRUID_PROC_ID(module, process) \
  (((sc_pcb_t *) DRUID_PROC_CB (module, process))->pid)
  
#define DRUID_PROC_NAME(module, process) \
  ((char *) ((sc_pcb_t *) DRUID_PROC_CB (module, process))->name)
  
#define DRUID_PROC_FLAGS(module, process) \
  (((sc_pcb_t *) DRUID_PROC_CB (module, process))->flags)
  
#define DRUID_PROC_STATE(module, process) \
  (((sc_pcb_t *) DRUID_PROC_CB (module, process))->state)
  
#define DRUID_PROC_PRIO(module, process) \
  (((sc_pcb_t *) DRUID_PROC_CB (module, process))->u1.priproc.prio)
  
#define DRUID_PROC_TMO(module, process) \
  (((sc_pcb_t *) DRUID_PROC_CB (module, process))->flags & PCB_FLG_IS_TIMER) ? \
  ((sc_pcb_t *) DRUID_PROC_CB (module, process))->u1.timproc.tmo : 0

#define DRUID_PROC_NALLOCED(module, process) \
    ((__u32 ) ((sc_pcb_t *) DRUID_PROC_CB (module, process))->nAlloc)

#define DRUID_PROC_NMSGS(module, process) \
     ((__u32 ) ((sc_pcb_t *) DRUID_PROC_CB (module, process))->nQueue)

INLINE static __u32 nObserve(sc_pcb_t * pcb)
{
  __u32 n = 0;
  dbl_t *ptr = pcb->connected.next;
  while( ptr != &pcb->connected ){
    ptr = ptr->next;
    ++n;
  }
  return n;
}
  
#define DRUID_PROC_NCONNECTED(module, process) \
  nObserve((sc_pcb_t *) DRUID_PROC_CB (module, process))

#define DRUID_PROC_STACKBTM(module, process) \
  ((__u32 ) ((sc_pcb_t *) DRUID_PROC_CB (module, process))->stackbtm)
  
#define DRUID_PROC_STACKTOP(module, process) \
  ((__u32 ) ((sc_pcb_t *) DRUID_PROC_CB (module, process))->stacktop)
  
#define DRUID_PROC_STACKPTR(module, process) \
  ((__u32 ) ((sc_pcb_t *) DRUID_PROC_CB (module, process))->stackptr)
  
  

/**
 * Pool access definition
 */
#define DRUID_POOL_MAX(module) \
  (sc_modules[module]->max_pools)

#define DRUID_POOL_CHK_INDEX(module, pooli) \
  ((__u32 ) sc_modules[module]->pool[pooli] != 0 && \
   (__u32 ) sc_modules[module]->pool[pooli] != 1)

#define DRUID_POOL_CB(module, pooli) \
  ((__u32 ) sc_modules[module]->pool[pooli])

#define DRUID_POOL_ID(module, pooli) \
  (((sc_pool_cb_t *) DRUID_POOL_CB (module, pooli))->id)
  
#define DRUID_POOL_NAME(module, pooli) \
  ((char *) ((sc_pool_cb_t *) DRUID_POOL_CB (module, pooli))->name)
 
#define DRUID_POOL_CREATOR(module, pooli) \
  ((__u32 ) ((sc_pool_cb_t *) DRUID_POOL_CB (module, pooli))->creator)
 
#define DRUID_POOL_START(module, pooli) \
  ((__u32 ) ((sc_pool_cb_t *) DRUID_POOL_CB (module, pooli))->start)
  
#define DRUID_POOL_CUR(module, pooli) \
  ((__u32 ) ((sc_pool_cb_t *) DRUID_POOL_CB (module, pooli))->cur)
  
#define DRUID_POOL_SIZE(module, pooli) \
  ((__u32 ) ((sc_pool_cb_t *) DRUID_POOL_CB (module, pooli))->size)
  
#define DRUID_POOL_END(module, pooli) \
  ((__u32 ) ((sc_pool_cb_t *) DRUID_POOL_CB (module, pooli))->end)
  
#define DRUID_POOL_NBUFSIZES(module, pooli) \
  ((__u32 ) ((sc_pool_cb_t *) DRUID_POOL_CB (module, pooli))->nbufsizes)
  
#define DRUID_POOL_BUFSIZES(module, pooli, i) \
  ((__u32 )((sc_pool_cb_t *) DRUID_POOL_CB (module, pooli))->bufsize[i])
  
#define DRUID_POOL_FREED(module, pooli, i) \
  ((dbl_t *) &((sc_pool_cb_t *) DRUID_POOL_CB (module, pooli))->freed[i])
 
/**
 * Message access functions
 */

STATIC_INLINE __u32 bufsizeIdx(sc_msg_hd_t *msghd)
{
  if ( (msghd->plid & MSG_TMO_MASK) == MSG_TMO_MAGIC ){
    return msghd->size >> (24+2);
  } else {
    return msghd->bufsize_idx >> 2;
  }
}

STATIC_INLINE __u32 msgOwner(sc_msg_hd_t *msghd)
{
  if ( (msghd->plid & MSG_TMO_MASK) == MSG_TMO_MAGIC ){
    return 0xAFFEAFFE;
  } else {
    return msghd->owner;
  }
}

STATIC_INLINE __u32 msgAddr(sc_msg_hd_t *msghd)
{
  if ( (msghd->plid & MSG_TMO_MASK) == MSG_TMO_MAGIC ){
    return 0;
  } else {
    return msghd->addressee;
  }
}

#define DRUID_MSG_GET_NEXT(module, pool, msg) \
  do { \
    if (msg == (__u32 ) -1) { \
      msg = DRUID_POOL_START(module, pool); \
    } \
    else if (msg < DRUID_POOL_CUR (module, pool)) { \
      msg += MSGHEAD + \
	     DRUID_POOL_BUFSIZES (module, pool, DRUID_MSG_BUFSIZE_IDX (msg)); \
    } \
    else { \
      msg = 0; \
    } \
  } while (0) 
  
#define DRUID_MSG_CHK_END(module, pool, msg) \
  (msg < DRUID_POOL_CUR (module, pool)) 

#define DRUID_MSG_BUFSIZE_IDX(msg) bufsizeIdx(DRUID_MSG_HEADER (msg))

#define DRUID_MSG_HEADER(msg) \
  ((sc_msg_hd_t *) msg) 

#define DRUID_MSG(msg) \
  (msg + MSGHEAD) 

#define DRUID_MSG_ID(msg) \
  DRUID_MSG_HEADER (msg)->id 

#define DRUID_MSG_ID_SIZE(msg) \
  4

#define DRUID_MSG_SIZE(msg) \
  (DRUID_MSG_HEADER (msg)->size & 0x00ffffff)

#define DRUID_MSG_PLID(msg) \
  (DRUID_MSG_HEADER (msg)->plid & (~MSG_TMO_MASK))

#define DRUID_MSG_OWNER(msg) \
  msgOwner(DRUID_MSG_HEADER (msg))

#define DRUID_MSG_SENDER(msg) \
  DRUID_MSG_HEADER (msg)->sender 

#define DRUID_MSG_ADDRESSEE(msg) \
  msgAddr(DRUID_MSG_HEADER (msg))
