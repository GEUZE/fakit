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
** ID: S01103BS13                                                   **
** +Revision: 2.49.12.1 +                                                **
** +Date: 2009/10/04 09:53:43 +                                     **
** Module control block definitions and API                         **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_MODULE_H_
#define _KERNEL_MODULE_H_ 1

/*
** Assure module and process names have equal sizes.
*/
/* SOCNV */
#ifdef SC_PROC_NAME_SIZE
#define SC_MODULE_NAME_SIZE (SC_PROC_NAME_SIZE)
#else
#define SC_MODULE_NAME_SIZE 31
#endif
/*
** Flags
*/
#ifdef SC_PROCKILL_KILL
#define SC_MODULEKILL_KILL (SC_PROCKILL_KILL)
#else
#define SC_MODULEKILL_KILL (0x4000)
#endif
/* EOCNV */
#ifndef __ASM_INCLUDED__
typedef struct sc_module_addr_s {
  char *start;
  __u32 size;
  __u32 initsize;
} sc_module_addr_t;

struct sc_mdb_s {
  void (*init)(void);
  sc_bufsize_t stacksize;
  sc_prio_t initprio;
  char *modstart;
  sc_modulesize_t size;
  sc_modulesize_t textsize;
  unsigned int max_pools;
  unsigned int max_procs;
  char name[SC_MODULE_NAME_SIZE+1];
};
typedef struct sc_mdb_s sc_mdb_t;
#ifdef _SC_CONF_H_
struct sc_module_cb_s {
  sc_moduleid_t id;                        /* modules id */
  char *text;                              /* real module start */
  char *data;                              /* start of free RAM */
  sc_modulesize_t size;                    /* complete module size */
  char *dataptr;                           /* current start of free RAM */
  sc_modulesize_t freesize;                /* free data space */
  
  sc_poolid_t npools;                      /* current No. pools */
  sc_poolid_t max_pools;                   /* max No. pools */
  sc_pool_cb_t **pool;                     /* array of pool_cb pointers */
  
  sc_prio_t prio;                          /* module priority */
  sc_pid_t pid_cnt;                        /* unique id counter */
  sc_pid_t pid_nxt;                        /* next free pid index */
  sc_pid_t nprocess;                       /* current No. processes */
  sc_pid_t max_process;                    /* max. No. processes */
  sc_pcb_t **pcbs;                         /* array of PCB pointers */
  
#if SC_USE_FRIENDS == 1
  __u32 friendset[4];                      /* set of friend-modules */
#endif
  /* module level hooks */
#if SC_MSG_HOOK == 1
#if SC_MSGTX_HOOK == 1
  sc_msgHook_t *msgTxHook;
#endif
#if SC_MSGRX_HOOK == 1
  sc_msgHook_t *msgRxHook;
#endif
#endif
#if SC_PROC_HOOK == 1
#if SC_PROCCREATE_HOOK == 1
  sc_procHook_t *procCreateHook;
#endif
#if SC_PROCKILL_HOOK == 1
  sc_procHook_t *procKillHook;
#endif
#if SC_PROCSWAP_HOOK == 1
  sc_procHook_t *procSwapHook;
#endif
#endif
#if SC_ERR_HOOK == 1
  sc_errHook_t *errHook;
#endif
#if SC_POOL_HOOK == 1
#if SC_POOLKILL_HOOK == 1
  sc_poolHook_t *poolKillHook;
#endif
#if SC_POOLCREATE_HOOK == 1
  sc_poolHook_t *poolCreateHook;
#endif
#endif
  __u16 hash;                              /* hash value of the mod.name */
#if SIZEOF_PTR == 4
  __u16 spare_h;
#endif
  char name[SC_MODULE_NAME_SIZE+1];        /* module's name */
#if CPU == PPC
  __u32 r2_save;
  __u32 r13_save;
#endif

};
#ifndef SC_MODULE_CB_T
#define SC_MODULE_CB_T 1
typedef struct sc_module_cb_s sc_module_cb_t;
#endif
#endif /* _SC_CONF_H_ */
/*
** Info structure filled by sc_moduleInfo
*/
typedef struct sc_moduleInfo_s {
  sc_moduleid_t mid;
  char name[SC_MODULE_NAME_SIZE+1];
  char *text;
  sc_modulesize_t textsize;
  char *data;
  sc_modulesize_t datasize;
  unsigned int max_process;
  unsigned int nprocess;
  unsigned int max_pools;
  unsigned int npools;
} sc_moduleInfo_t;


#define MODULE(ptr,element) SC_DBL_ITEM(sc_module_cb_t,ptr,element)
/* Sciopta-internal function. __DO_NOT_USE__ !! */
#if SC_USE_SYSCALL == 0
SCFUNCn(sc_moduleid_t,sc_sysModuleCreate,
	const char *,
	void (*)(void),
	sc_bufsize_t,
	sc_prio_t,
	char *,
	sc_modulesize_t,
	sc_modulesize_t,
	__uint,
	__uint);
#endif

#if CPU == PPC
externC void sc_sysModuleSDASet(__u32 _r2,__u32 _r13);
#endif

/* public functions */
SCFUNCn(void,sc_sysModulePrioSet,sc_prio_t,sc_moduleid_t);
SCFUNCn(sc_prio_t,sc_sysModulePrioGet,sc_moduleid_t);
SCFUNCn(sc_moduleid_t,sc_moduleCreate,
	const char *,
	void_f,
	sc_bufsize_t,
	sc_prio_t,char *,
	sc_modulesize_t,
	sc_modulesize_t,
	unsigned int,
	unsigned int);

SCFUNCn(void,sc_moduleKill,sc_moduleid_t,flags_t);
SCFUNCn(const char *,sc_moduleNameGet,sc_moduleid_t);
SCFUNCn(sc_moduleid_t,sc_moduleIdGet,const char *);
#define MODULE_IS_FRIEND(idx) \
        (SC.curModule->friendset[(idx)>>5] & (1<<((idx) & 0x1f)))

#define FRIEND_IS_MODULE(module) \
        (module->friendset[(MID2IDX(SC.curModule->id))>>5] & (1<<((MID2IDX(SC.curModule->id)) & 0x1f)))
#if SC_USE_FRIENDS == 1
SCFUNCn(void,sc_moduleFriendAdd,sc_moduleid_t);
SCFUNCn(void,sc_moduleFriendRm,sc_moduleid_t);
SCFUNCn(int,sc_moduleFriendGet,sc_moduleid_t);
SCFUNC0(void,sc_moduleFriendNone);
SCFUNC0(void,sc_moduleFriendAll);
#endif
SCFUNCn(int,sc_moduleInfo,sc_moduleid_t,sc_moduleInfo_t *);

#ifndef __KERNEL__
#define sc_sysModulePrioSet(prio,moduleid) _FUNCn(sc_sysModulePrioSet,prio,moduleid)
#define sc_sysModulePrioGet(moduleid)      _FUNCn(sc_sysModulePrioGet,moduleid)
#define sc_sysModulePPCprivate() _FUNC0(sc_sysModulePPCprivate)
#define sc_moduleKill(moduleid,killflag)   _FUNCn(sc_moduleKill,moduleid,killflag)
#define sc_moduleCreate(name,initfunc,stacksize,prio,start,initsize,modulesize,maxpool,maxprocess) \
  _FUNCn(sc_moduleCreate,name,initfunc,stacksize,prio,start,initsize,modulesize,maxpool,maxprocess)
#define sc_sysModuleCreate(name,initfunc,stacksize,prio,start,initsize,modulesize,maxpool,maxprocess) \
  _FUNCn(sc_sysModuleCreate,name,initfunc,stacksize,prio,start,initsize,modulesize,maxpool,maxprocess)

#define sc_moduleIdGet(name)            _FUNCn(sc_moduleIdGet,name)
#define sc_moduleNameGet(moduleid)      _FUNCn(sc_moduleNameGet,moduleid)
#define sc_moduleInfo(moduleid,infoptr) _FUNCn(sc_moduleInfo,moduleid,infoptr)
#define sc_moduleFriendAdd(moduleid)    _FUNCn(sc_moduleFriendAdd,moduleid)
#define sc_moduleFriendRm(moduleid)     _FUNCn(sc_moduleFriendRm,moduleid)
#define sc_moduleFriendGet(moduleid)    _FUNCn(sc_moduleFriendGet,moduleid)
#define sc_moduleFriendNone()           _FUNC0(sc_moduleFriendNone)
#define sc_moduleFriendAll()            _FUNC0(sc_moduleFriendAll)
#endif /* __KERNEL__ */

#define MID(x) ((x) & MODULE_MASK)
#define MID2IDX(x) ( ((x) >> MODULE_SHIFT) & (MODULE_MASK>>MODULE_SHIFT) )
#define SC_ILLEGAL_MID ((sc_moduleid_t)-2)
#define SC_CURRENT_MID ((sc_moduleid_t)-1)

#define MODULE_IS_TARGET(m) ((m)->id == 0)

#else /* __ASM_INCLUDED__ */
#ifdef _SC_CONF_H_
#if SIZEOF_PTR == 2
#define MDB_INIT       0
#define MDB_STACKSIZE  2
#define MDB_INITPRIO   4
#define MDB_MODSTART   5
#define MDB_SIZE       7
#define MDB_TEXTSIZE   9
#define MDB_MAX_POOLS 11
#define MDB_MAX_PROCS 12
#define MDB_NAME      13
#define SIZEOF_MDB    55

#define MODULE_ID          0x00
#define MODULE_TEXT        0x02
#define MODULE_DATA        0x04
#define MODULE_SIZE        0x06
#define MODULE_DATAPTR     0x08
#define MODULE_FREESIZE    0x0a
#define MODULE_NPOOLS      0x0c
#define MODULE_MAX_POOLS   0x0d
#define MODULE_MAX_PROCESS 0x0e
#define MODULE_PRIO        0x0f
#define MODULE_POOL        0x10
#define MODULE_PCBS        0x12
#define MODULE_PID_CNT     0x14
#define MODULE_PID_NXT     0x15
#define MODULE_NPROCESS    0x16
#if SC_USE_FRIENDS == 1
#define MODULE_FRIENDSET   0x17
#else
#define MODULE_FRIENDSET  0x16
#endif

#if SC_MSG_HOOK == 1
#if SC_MSGTX_HOOK == 1
#define MODULE_MSGTXHOOK   (MODULE_FRIENDSET+1)
#else
#define MODULE_MSGTXHOOK   (MODULE_FRIENDSET)
#endif
#if SC_MSGRX_HOOK == 1
#define MODULE_MSGRXHOOK  (MODULE_MSGTXHOOK+2)
#else
#define MODULE_MSGRXHOOK  (MODULE_MSGTXHOOK)
#endif
#else
#define MODULE_MSGRXHOOK  (MODULE_FRIENDSETE)
#endif
#if SC_PROC_HOOK == 1
#  if SC_PROCCREATE_HOOK == 1
#    define MODULE_PROCCREATEHOOK (MODULE_MSGRXHOOK+2)
#  else
#    define MODULE_PROCCREATEHOOK (MODULE_MSGRXHOOK)
#  endif
#  if SC_PROCKILL_HOOK == 1
#    define MODULE_PROCKILLHOOK (MODULE_PROCCREATEHOOK+2)
#  else
#    define MODULE_PROCKILLHOOK (MODULE_PROCCREATEHOOK)
#  endif
#  if SC_PROCSWAP_HOOK == 1
#    define MODULE_PROCSWAPHOOK (MODULE_PROCKILLHOOK+2)
#  else
#    define MODULE_PROCSWAPHOOK (MODULE_PROCKILLHOOK)
#  endif
#else
#  define MODULE_PROCSWAPHOOK (MODULE_MSGRXHOOK)
#endif
#if SC_ERR_HOOK == 1
#define MODULE_ERRHOOK (MODULE_PROCSWAPHOOK+2)
#else
#define MODULE_ERRHOOK (MODULE_PROCSWAPHOOK)
#endif

#if SC_POOL_HOOK == 1
#if SC_POOLKILL_HOOK == 1
#define MODULE_POOLKILLHOOK (MODULE_ERRHOOK+2)
#else
#define MODULE_POOLKILLHOOK (MODULE_ERRHOOK)
#endif
#if SC_POOLCREATE_HOOK == 1
#define MODULE_POOLCREATEHOOK (MODULE_POOLKILLHOOK+2)
#else
#define MODULE_POOLCREATEHOOK (MODULE_POOLKILLHOOK)
#endif
#else
#define MODULE_POOLCREATEHOOK (MODULE_ERRHOOK)
#endif
#define MODULE_HASH_H (MODULE_POOLCREATEHOOK+2)
#define MODULE_NAME (MODULE_HASH_H+2)

#define SIZEOF_MODULE_CB (MODULE_NAME+SC_MODULE_NAME_SIZE+1)

#else /* SIZEOF_PTR == 2 */
/* SOCNV */
#define MDB_INIT       0
#define MDB_STACKSIZE  4
#define MDB_INITPRIO   8
#define MDB_MODSTART  12
#define MDB_SIZE      16
#define MDB_TEXTSIZE  20
#define MDB_MAX_POOLS 24
#define MDB_MAX_PROCS 28
#define MDB_NAME      32
#define SIZEOF_MDB    64

#define MODULE_ID          0x00
#define MODULE_TEXT        0x04
#define MODULE_DATA        0x08
#define MODULE_SIZE        0x0c
#define MODULE_DATAPTR     0x10
#define MODULE_FREESIZE    0x14
#define MODULE_NPOOLS      0x18
#define MODULE_MAX_POOLS   0x1c
#define MODULE_POOL        0x20
#define MODULE_PRIO        0x24
#define MODULE_PID_CNT     0x28
#define MODULE_PID_NXT     0x2c
#define MODULE_NPROCESS    0x30
#define MODULE_MAX_PROCESS 0x34
#define MODULE_PCBS        0x38
#if SC_USE_FRIENDS == 1
#define MODULE_FRIENDSET   0x3c
#define MODULE_FRIENDSETE  0x48
#else
#define MODULE_FRIENDSETE  0x38
#endif
#if SC_MSG_HOOK == 1
#if SC_MSGTX_HOOK == 1
#define MODULE_MSGTXHOOK   (MODULE_FRIENDSETE+4)
#else
#define MODULE_MSGTXHOOK   (MODULE_FRIENDSETE)
#endif
#if SC_MSGRX_HOOK == 1
#define MODULE_MSGRXHOOK  (MODULE_MSGTXHOOK+4)
#else
#define MODULE_MSGRXHOOK  (MODULE_MSGTXHOOK)
#endif
#else
#define MODULE_MSGRXHOOK  (MODULE_FRIENDSETE)
#endif
#if SC_PROC_HOOK == 1
#if SC_PROCCREATE_HOOK == 1
#define MODULE_PROCCREATEHOOK (MODULE_MSGRXHOOK+4)
#else
#define MODULE_PROCCREATEHOOK (MODULE_MSGRXHOOK)
#endif
#if SC_PROCKILL_HOOK == 1
#define MODULE_PROCKILLHOOK (MODULE_PROCCREATEHOOK+4)
#else
#define MODULE_PROCKILLHOOK (MODULE_PROCCREATEHOOK)
#endif
#if SC_PROCSWAP_HOOK == 1
#define MODULE_PROCSWAPHOOK (MODULE_PROCKILLHOOK+4)
#else
#define MODULE_PROCSWAPHOOK (MODULE_PROCKILLHOOK)
#endif
#else
#define MODULE_PROCSWAPHOOK (MODULE_MSGRXHOOK)
#endif
#if SC_ERR_HOOK == 1
#define MODULE_ERRHOOK (MODULE_PROCSWAPHOOK+4)
#else
#define MODULE_ERRHOOK (MODULE_PROCSWAPHOOK)
#endif
#if SC_POOL_HOOK == 1
#if SC_POOLKILL_HOOK == 1
#define MODULE_POOLKILLHOOK (MODULE_ERRHOOK+4)
#else
#define MODULE_POOLKILLHOOK (MODULE_ERRHOOK)
#endif
#if SC_POOLCREATE_HOOK == 1
#define MODULE_POOLCREATEHOOK (MODULE_POOLKILLHOOK+4)
#else
#define MODULE_POOLCREATEHOOK (MODULE_POOLKILLHOOK)
#endif
#else
#define MODULE_POOLCREATEHOOK (MODULE_ERRHOOK)
#endif
#define MODULE_HASH_H (MODULE_POOLCREATEHOOK+4)
#define MODULE_SPARE_H (MODULE_HASH_H+2)
#define MODULE_NAME (MODULE_SPARE_H+2)
#if CPU == PPC
#define MODULE_R2_SAVE (MODULE_NAME+32)
#define MODULE_R13_SAVE (MODULE_R2_SAVE+4)
#else
#define MODULE_R13_SAVE (MODULE_NAME+28)
#endif
#define SIZEOF_MODULE_CB (MODULE_R13_SAVE+4)
/* EOCNV */
#endif /* SIZEOF_PTR == 2 */
#endif /* _SC_CONF_H_ */
/* SOCNV */
#define MODULE_INFO_MID 0
#define MODULE_INFO_NAME 4
#define MODULE_INFO_TEXT (MODULE_INFO_NAME+SC_MODULE_NAME_SIZE+1)
#define MODULE_INFO_TEXTSIZE (MODULE_INFO_TEXT+4)
#define MODULE_INFO_DATA (MODULE_INFO_TEXTSIZE+4)
#define MODULE_INFO_DATASIZE (MODULE_INFO_DATA+4)
#define MODULE_INFO_MAX_PROCESS (MODULE_INFO_DATASIZE+4)
#define MODULE_INFO_NPROCESS (MODULE_INFO_MAX_PROCESS+4)
#define MODULE_INFO_MAX_POOLS (MODULE_INFO_NPROCESS+4)
#define MODULE_INFO_NPOOLS (MODULE_INFO_MAX_POOLS+4)
#define SIZEOF_MODULE_INFO (MODULE_INFO_NPOOLS+4)

#define SC_ILLEGAL_MID (-2)
#define SC_CURRENT_MID (-1)
/* EOCNV */
#endif /* __ASM_INCLUDED__ */
#endif







