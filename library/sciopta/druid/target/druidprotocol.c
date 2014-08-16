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
** ID: S04281CL2                                                    **
** +Revision: 1.88 +                                                **
** +Date: 2008/11/11 10:09:41 +                                     **
** Druid stop command                                               **
**********************************************************************
EOC*/

#include "sconf.h"
#include "druidcnf.h"
#include <sciopta.h>
#include <arch/druid.h>
#include <ossys/swap.h>
#include <druid/device.h>
#include <druid/commands.h>
#include <string.h>
#include <stdarg.h>

DRUID_DECLARATIONS

#define DRUID_MSG_HOOKS \
  SC_MSG_HOOK == 1 && \
  (SC_MSGTX_HOOK == 1 || SC_MSGRX_HOOK == 1)
  
#define DRUID_PROC_HOOKS \
  SC_PROC_HOOK == 1 && \
  (SC_PROCCREATE_HOOK == 1 || SC_PROCKILL_HOOK == 1 || SC_PROCSWAP_HOOK == 1)

#define DRUID_POOL_HOOKS \
  SC_POOL_HOOK == 1 && \
  (SC_POOLCREATE_HOOK == 1 || SC_POOLKILL_HOOK == 1)

/**
 * forward declarations
 */
static void druid_main(int autoGo_init);

/* commands */
static void druid_none (void);
static void druid_stop (void);
static void druid_start (void);
static void druid_moduleGet (void);
static void druid_processGet (void);
static void druid_stackGet (void);
static void druid_poolGet (void);
static void druid_msgGet (void);
#if DRUID_MSG_HOOKS || DRUID_PROC_HOOKS || DRUID_POOL_HOOKS 
static void druid_optionEnter (void);
static void druid_optionDisplay (void);
static void druid_optionClear (void);
static void druid_traceDisplay (void);
#endif

/* hooks */
#if SC_MSGTX_HOOK == 1
static void druid_msgTxHook (const sc_msg_hd_t *);
#endif
#if SC_MSGRX_HOOK == 1
static void druid_msgRxHook (const sc_msg_hd_t *);
#endif
#if SC_PROCCREATE_HOOK == 1
static void druid_procCreateHook (const sc_pcb_t *, const sc_pcb_t *);
#endif
#if SC_PROCKILL_HOOK == 1
static void druid_procKillHook (const sc_pcb_t *,const sc_pcb_t *);
#endif
#if SC_PROCSWAP_HOOK == 1
static void druid_procSwapHook (const sc_pcb_t *, const sc_pcb_t *);
#endif
#if SC_POOLCREATE_HOOK == 1
static void druid_poolCreateHook (const sc_pool_cb_t *);
#endif
#if SC_POOLKILL_HOOK == 1
static void druid_poolKillHook (const sc_pool_cb_t *);
#endif
#if SC_ERR_HOOK == 1
static void druid_errorDisplay (void);
#endif

/*
** local data
*/
static __u8 writeBuf[SC_NAME_MAX * 2 + CNF_DRUID_MSG_DATA_MAX + 256];

typedef struct readreq_s {
  int len;
  int idx;
  __u8 *p;
  __u8 buf[64];
} readreq_t;

static readreq_t readreq;
static int autoGo;

/** 
 * command array
 */
static const druid_cmd_f druid_commands[] = {
  druid_none,
  druid_stop,
  druid_start,
  druid_moduleGet,
  druid_processGet,
  druid_stackGet,
  druid_poolGet,
  druid_msgGet,
#if DRUID_MSG_HOOKS || DRUID_PROC_HOOKS || DRUID_POOL_HOOKS 
  druid_optionEnter,
  druid_optionDisplay,
  druid_optionClear,
  druid_traceDisplay
#else
  druid_none,
  druid_none,
  druid_none,
  druid_none
#endif
};
#define NO_CMDS (int)(sizeof (druid_commands) / sizeof (druid_cmd_f))

static const int cmdlength[12] = {
  0,
  1, /* stop */
  1, /* start */
  2, /* module get */
  6, /* process get */
  6, /* stack get */
  6, /* pool get */
  12,/* msg get */
  27,/* option enter */
  2, /* option display */
  2, /* option clear */
  1  /* display trace */
};

/**
 * trace info
 */
typedef struct druid_ti_s {
  int in;
  int out;
  int count;
  __u8 nma;
#define DRUID_TRACE_OFF 0
#define DRUID_TRACE_ON 1
#define DRUID_TRACE_DISPLAY 2
  __u8 trace;
} druid_ti_t;

/**
 * trace frame 
 */
typedef struct druid_tf_s {
  __u32 timestamp;
  __u32 from;
  __u32 to;
  __u8 hookType;
  __u8 size;
} druid_tf_t;

/**
 * trace option 
 */
typedef struct druid_option_s {
  __u8 hookType;
  __u32 from;
  __u32 fromMask;
  __u32 to;
  __u32 toMask;
  __u32 id;
  __u32 idMask;
  void (*call) (void * h, __u8 hookType);
} druid_option_t;

#if DRUID_MSG_HOOKS 
static druid_ti_t druid_ti = { 0, 0, 0, 0, 0 };
static druid_option_t druid_option[CNF_DRUID_OPTION_MAX + 1] = { {0}, };
static __u8 druid_traceMem [CNF_DRUID_TRACE_MEM_MAX] = { 0, };
#endif

/**
 * error information
 */
typedef struct druid_err_s {
  __u32 valid;
  __u32 code;
  __u32 extra;
  __u32 user;
  __u32 pcb;
} druid_err_t;

static druid_err_t druid_err = { 0, 0, 0, 0, 0 };

/**
 * System load information
 */
#if 0 /* NOT YET */
typedef struct druid_load_s {
  __u32 start;
  __u32 idle;
  __u32 process;
} druid_load_t;

static druid_load_t druid_load = { 0, 0, 0 };
#endif


/**
 * helper functions
 */
static __u32
druid_cntList (dbl_t * head)
{
  dbl_t *p = head->next;
  unsigned int cnt = 0;

  while (p != head) {
    ++cnt;
    p = p->next;
  }
  return cnt;
}

STATIC_INLINE __u8 *
druid_cpyU8 (__u8 * buf, __u8 c)
{
  *buf++ = c;
  return buf;
}

#if 0 /* not used */
STATIC_INLINE __u8 *
druid_cpyU16 (__u8 * buf, __u16 c)
{
  /* BE protocol */
  *buf++ = c >> 8;
  *buf++ = c & 0xff;
  return buf;
}
#endif

STATIC_INLINE __u8 *
druid_cpyU32 (__u8 * buf, __u32 c)
{
  /* BE protocol */
  *buf++ = (c >> 24) & 0xff;
  *buf++ = (c >> 16) & 0xff;
  *buf++ = (c >> 8) & 0xff;
  *buf++ = (c >> 0) & 0xff;
  return buf;
}

STATIC_INLINE __u8 *
druid_cpyStr (__u8 * buf, const char *str)
{
  __u8 len = strlen (str);
  *buf++= len;
  memcpy(buf,str,len);
  return buf+len;
}

STATIC_INLINE __u32
druid_read32(__u8 *buf)
{
  return (buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|buf[3];
}

STATIC_INLINE __u16
druid_read16(__u8 *buf)
{
  return (buf[0]<<8)|buf[1];
}


static void
druid_ringPut (__u8 *source, size_t in)
{
  size_t i;
  __uint size;

  for (i = 0; i < in; i++) {
    druid_traceMem[druid_ti.in] = source[i];

    if (druid_ti.in != druid_ti.out || druid_ti.count == 0) {
      druid_ti.in = (druid_ti.in + 1) & (CNF_DRUID_TRACE_MEM_MAX - 1);
      ++druid_ti.count;
    }
    else {
      size = druid_traceMem[(druid_ti.out + offsetof (druid_tf_t, size)) & 
	                    (CNF_DRUID_TRACE_MEM_MAX - 1)];
      druid_ti.out = (druid_ti.out + sizeof (druid_tf_t) + size) & 
	             (CNF_DRUID_TRACE_MEM_MAX - 1);
      druid_ti.count -= sizeof (druid_tf_t) + size;
      druid_ti.in = (druid_ti.in + 1) & (CNF_DRUID_TRACE_MEM_MAX - 1);
      ++druid_ti.count;
    }
  }
}

static void
druid_ringGet (__u8 *dest, size_t out)
{
  size_t i;

  for (i = 0; druid_ti.count && i < out; i++) {
    dest[i] = druid_traceMem[druid_ti.out];

    druid_ti.out = (druid_ti.out + 1) & (CNF_DRUID_TRACE_MEM_MAX - 1);
    --druid_ti.count;
  }
}


static void druid_rxisr(int c)
{
  if ( readreq.len ){
    *readreq.p++ = c;
  } else {
    if ( c < NO_CMDS ){
      readreq.p = readreq.buf;
      *readreq.p++ = c;
      readreq.len = cmdlength[c];
      readreq.idx = 0;
    }
  }    
  readreq.idx++;
  if ( readreq.idx == readreq.len ){
    druid_main(1);
  }
}

static void readCmdline()
{
  int c;
  while( readreq.len == 0 || readreq.len != readreq.idx ){
    c = druid_read();
    if ( c < 0 ){
      readreq.p = readreq.buf;
      readreq.len = 0;
      continue;
    } 
    if ( readreq.len ){
      *readreq.p++ = c;
      readreq.idx++;
    } else {
      if ( c < NO_CMDS ){
	readreq.p = readreq.buf;
	*readreq.p++ = c;
	readreq.len = cmdlength[c];
	readreq.idx = 1;
      }
    }
  }  
}
  
static void druid_main(int autoGo_init)
{
  autoGo = autoGo_init;

  do{
    readCmdline();
    
    druid_commands[readreq.buf[0]]();
        
    readreq.len = 0;
    readreq.p = readreq.buf;
  
  }while( autoGo == 0);

  druid_timeReset ();
}
  
void
druid_startup (int stopMode)
{
  readreq.len = 0;
  readreq.p = readreq.buf;

  /* setup the druid */
  druid_init (druid_rxisr,NULL);

  /* insert hooks */
#if SC_MSGTX_HOOK == 1
  sc_msgHookRegister (SC_SET_MSGTX_HOOK, druid_msgTxHook);
#endif
#if SC_MSGRX_HOOK == 1
  sc_msgHookRegister (SC_SET_MSGRX_HOOK, druid_msgRxHook);
#endif
#if SC_PROCCREATE_HOOK == 1
  sc_procHookRegister (SC_SET_PROCCREATE_HOOK, druid_procCreateHook);
#endif
#if SC_PROCKILL_HOOK == 1
  sc_procHookRegister (SC_SET_PROCKILL_HOOK, druid_procKillHook);
#endif
#if SC_PROCSWAP_HOOK == 1
  sc_procHookRegister (SC_SET_PROCSWAP_HOOK, druid_procSwapHook);
#endif
#if SC_POOLCREATE_HOOK == 1
  sc_poolHookRegister (SC_SET_POOLCREATE_HOOK, druid_poolCreateHook);
#endif
#if SC_POOLKILL_HOOK == 1
  sc_poolHookRegister (SC_SET_POOLKILL_HOOK, druid_poolKillHook);
#endif

  if ( stopMode ){
    druid_main(0);
  }
}


static void
druid_none(void)
{
  /* reply on the unused command */
  __u8 * buf = writeBuf;

  buf = druid_cpyU8  (buf, readreq.buf[0]);
  buf = druid_cpyU8  (buf, DRUID_MARK_END);

  druid_write (writeBuf, 2);
}
  
/**
 * Stop call
 */

static void
druid_stop (void)
{
  /* reply on the stop command */
  __u8 * buf = writeBuf;

  buf = druid_cpyU8  (buf, DRUID_CMD_STOP);
  buf = druid_cpyU8  (buf, DRUID_MARK_END);
  buf = druid_cpyU32 (buf, druid_timeUsPerTick ());

  druid_write (writeBuf, buf - writeBuf);
  autoGo = 0;
}

    
/**
 * Start call
 */
static void
druid_start (void)
{
  __u8 * buf = writeBuf;
  buf = druid_cpyU8  (buf, DRUID_CMD_START);
  buf = druid_cpyU8  (buf, DRUID_MARK_END);
  buf = druid_cpyU32 (buf, druid_timeUsPerTick ());

  druid_write (writeBuf, buf - writeBuf);

  autoGo = 1;
}

/**
 * Module get call
 */
static void
druid_moduleGet ()
{
  __u8 module;
  __u32 curMcb;

  module = readreq.buf[1];
  curMcb = DRUID_MODULE_CUR;
  do {
    ++module;
  } while (module < DRUID_MODULE_MAX && !DRUID_MODULE_CHK_INDEX (module));

  if (module < DRUID_MODULE_MAX && DRUID_MODULE_CHK_INDEX (module)) {
    __u8 * buf = writeBuf;

    buf = druid_cpyU8  (buf, DRUID_CMD_MODULE_GET);
    buf = druid_cpyU8  (buf, DRUID_MARK_MORE);
    buf = druid_cpyU8  (buf, module);
    buf = druid_cpyU32 (buf, curMcb);
    buf = druid_cpyU32 (buf, DRUID_MODULE_CB (module));
    buf = druid_cpyU32 (buf, DRUID_MODULE_PRIO (module));
    buf = druid_cpyU32 (buf, DRUID_PROC_MAX (module));
    buf = druid_cpyU32 (buf, DRUID_MODULE_NPROCESS (module));
    buf = druid_cpyU32 (buf, DRUID_POOL_MAX (module));
    buf = druid_cpyU32 (buf, DRUID_MODULE_NPOOLS (module));
    buf = druid_cpyU32 (buf, DRUID_MODULE_TEXT (module));
    buf = druid_cpyU32 (buf, DRUID_MODULE_DATA (module));
    buf = druid_cpyU32 (buf, DRUID_MODULE_SIZE (module));
    buf = druid_cpyU32 (buf, DRUID_MODULE_FREESIZE (module));
    buf = druid_cpyStr (buf, DRUID_MODULE_NAME (module));

    druid_write (writeBuf, buf - writeBuf);
  }
  else {
    writeBuf[0] = DRUID_CMD_MODULE_GET;
    writeBuf[1] = DRUID_MARK_END;
    druid_write (writeBuf, 2);
  }
}


/**
 * Process get call
 */
static void
druid_processGet ()
{
  __u8 module;
  __u32 process;
  __u32 curPcb;

  curPcb = DRUID_PROC_CUR;

  module = readreq.buf[1];
  process = druid_read32(&readreq.buf[2]);

  if (module < DRUID_MODULE_MAX && DRUID_MODULE_CHK_INDEX (module)) {
    do {
      ++process;
    } while (process < DRUID_PROC_MAX (module) &&
             !DRUID_PROC_CHK_INDEX (module, process));

    if (process < DRUID_PROC_MAX (module) &&
        DRUID_PROC_CHK_INDEX (module, process)) {
      __u32 nalloc;
      __u32 nobserve;
      __u8 * buf = writeBuf;

      buf = druid_cpyU8  (buf, DRUID_CMD_PROCESS_GET);
      buf = druid_cpyU8  (buf, DRUID_MARK_MORE);
      buf = druid_cpyU32 (buf, process);
      buf = druid_cpyU32 (buf, DRUID_PROC_CB (module, process));
      buf = druid_cpyU32 (buf, curPcb);
      buf = druid_cpyU32 (buf, DRUID_PROC_FLAGS (module, process));
      buf = druid_cpyU32 (buf, DRUID_PROC_STATE (module, process));
      buf = druid_cpyU8  (buf, DRUID_PROC_PRIO (module, process));
      buf = druid_cpyU32 (buf, DRUID_PROC_TMO (module, process));
     
      nobserve = DRUID_PROC_NCONNECTED (module, process);
      nalloc = DRUID_PROC_NALLOCED (module, process) - nobserve;
      buf = druid_cpyU32 (buf, nalloc);
      buf = druid_cpyU32 (buf, DRUID_PROC_NMSGS (module, process));
      buf = druid_cpyU32 (buf, nobserve );
      buf = druid_cpyU32 (buf, DRUID_PROC_ID (module, process));
      buf = druid_cpyStr (buf, DRUID_PROC_NAME (module, process));

      druid_write (writeBuf, buf - writeBuf);
    }
    else {
      writeBuf[0] = DRUID_CMD_PROCESS_GET;
      writeBuf[1] = DRUID_MARK_END;     
      druid_write (writeBuf, 2); 
    }
  }
  else {
    writeBuf[0] = DRUID_CMD_PROCESS_GET;
    writeBuf[1] = DRUID_MARK_END;
    druid_write (writeBuf, 2); 
  }
}
/**
 * Stack get call
 */
STATIC_INLINE __u32
stackUsage (__u8 module, __u32 process)
{
  __u32 *e;
  __u32 *s;

  s = (__u32 *) DRUID_PROC_STACKBTM (module, process);
  e = (__u32 *) DRUID_PROC_STACKTOP (module, process);

  while (*s++ == 0x17171717 && s < e ){
    /* empty */
  }
  --s;

  return (__u32) e - (__u32) s;
}

static void
druid_stackGet()
{
  __u8 module;
  __u32 process;
  __u32 unused;

  module = readreq.buf[1];
  process = druid_read32(&readreq.buf[2]);

  if (module < DRUID_MODULE_MAX && DRUID_MODULE_CHK_INDEX (module)) {
    do {
      ++process;
    } while (process < DRUID_PROC_MAX (module) &&
             !DRUID_PROC_CHK_INDEX (module, process));

    if (process < DRUID_PROC_MAX (module) &&
        DRUID_PROC_CHK_INDEX (module, process)) {
      __u8 * buf = writeBuf;
      unused = stackUsage (module, process);

      buf = druid_cpyU8 (buf, DRUID_CMD_STACK_GET);
      buf = druid_cpyU8 (buf, DRUID_MARK_MORE);
      buf = druid_cpyU32 (buf, process);
      buf = druid_cpyU32 (buf, DRUID_PROC_ID (module, process));
      buf = druid_cpyU32 (buf, DRUID_PROC_STACKBTM (module, process));
      buf = druid_cpyU32 (buf, DRUID_PROC_STACKTOP (module, process));
      buf = druid_cpyU32 (buf, DRUID_PROC_STACKPTR (module, process));
      buf = druid_cpyU32 (buf, unused);
      buf = druid_cpyStr (buf, DRUID_PROC_NAME (module, process));

      druid_write (writeBuf, buf - writeBuf);
    }
    else {
      writeBuf[0] = DRUID_CMD_STACK_GET;
      writeBuf[1] = DRUID_MARK_END;
      druid_write (writeBuf, 2);
    }
  }
  else {
    writeBuf[0] = DRUID_CMD_STACK_GET;
    writeBuf[1] = DRUID_MARK_END;
    druid_write (writeBuf, 2);
  }
}
/**
 * Pool get
 */
static void
druid_poolGet()
{
  __u8 o;
  __u8 nbufsizes;
  __u8 module;
  __u32 pool;
  __u32 creator;

  module = readreq.buf[1];
  pool = druid_read32(&readreq.buf[2]);

  if (module < DRUID_MODULE_MAX && DRUID_MODULE_CHK_INDEX (module)) {
    do {
      ++pool;
    } while (pool < DRUID_POOL_MAX (module) &&
             !DRUID_POOL_CHK_INDEX (module, pool));
    if (pool < DRUID_POOL_MAX (module)
        && DRUID_POOL_CHK_INDEX (module, pool)) {
      __u8 * buf = writeBuf;

      buf = druid_cpyU8 (buf, DRUID_CMD_POOL_GET);
      buf = druid_cpyU8 (buf, DRUID_MARK_MORE);
      buf = druid_cpyU32 (buf, pool);
      buf = druid_cpyU32 (buf, DRUID_POOL_ID (module, pool));
      buf = druid_cpyU32 (buf, DRUID_POOL_START (module, pool));
      buf = druid_cpyU32 (buf, DRUID_POOL_CUR (module, pool));
      buf = druid_cpyU32 (buf, DRUID_POOL_END (module, pool));
      buf = druid_cpyU32 (buf, DRUID_POOL_SIZE (module, pool));
      nbufsizes = DRUID_POOL_NBUFSIZES (module, pool);
      buf = druid_cpyU8 (buf, nbufsizes);
      for (o = 0; o < nbufsizes; ++o) {
        buf = druid_cpyU32 (buf, DRUID_POOL_BUFSIZES (module, pool, o));
        buf = druid_cpyU32 (buf,
                      druid_cntList (DRUID_POOL_FREED (module, pool, o)));
      }
      buf = druid_cpyStr (buf, DRUID_POOL_NAME (module, pool));
      creator = DRUID_POOL_CREATOR (module, pool);
      if (creator < DRUID_PROC_MAX (module)
          && DRUID_PROC_CHK_INDEX (module, creator)) {
        buf = druid_cpyStr (buf, DRUID_PROC_NAME (module, creator));
      }
      else {
        buf = druid_cpyU8 (buf, 0);
      }

      druid_write (writeBuf, buf - writeBuf);
    }
    else {
      writeBuf[0] = DRUID_CMD_POOL_GET;
      writeBuf[1] = DRUID_MARK_END;
      druid_write (writeBuf, 2);
    }
  }
  else {
    writeBuf[0] = DRUID_CMD_POOL_GET;
    writeBuf[1] = DRUID_MARK_END;
    druid_write (writeBuf, 2);
  }
}
/**
 * Messages get 
 */
static void
druid_msgGet()
{
  int o;
  int tmp;
  __u8 module;
  __u32 pool;
  __u32 msg;
  __u16 msgsize;
  int size;

  module = readreq.buf[1];
  pool = druid_read32(&readreq.buf[2]);
  msg = druid_read32(&readreq.buf[6]);
  msgsize = druid_read16(&readreq.buf[10]);

  if (module < DRUID_MODULE_MAX && 
      DRUID_MODULE_CHK_INDEX (module) &&
      pool < DRUID_POOL_MAX (module) && 
      DRUID_POOL_CHK_INDEX (module, pool)) 
  {
    DRUID_MSG_GET_NEXT (module, pool, msg);
#if (SC_MSG_CHECK == 1)
    msg += 4;
#endif
    if (DRUID_MSG_CHK_END (module, pool, msg)) {
      __u8 * buf = writeBuf;

      buf = druid_cpyU8 (buf, DRUID_CMD_MSG_GET);
      buf = druid_cpyU8 (buf, DRUID_MARK_MORE);
      buf = druid_cpyU32 (buf, msg);
      buf = druid_cpyU32 (buf, DRUID_MSG (msg));
      buf = druid_cpyU32 (buf, DRUID_MSG_ID (msg));
      buf = druid_cpyU32 (buf, DRUID_MSG_PLID (msg));
      buf = druid_cpyU32 (buf, DRUID_MSG_SIZE (msg));
      buf = druid_cpyU32 (buf, DRUID_MSG_OWNER (msg));
      buf = druid_cpyU32 (buf, DRUID_MSG_SENDER (msg));
      buf = druid_cpyU32 (buf, DRUID_MSG_ADDRESSEE (msg));
      tmp = buf - writeBuf;
      size = DRUID_MSG_SIZE (msg) - DRUID_MSG_ID_SIZE (msg) < msgsize ?
        DRUID_MSG_SIZE (msg) - DRUID_MSG_ID_SIZE (msg) : msgsize;
      buf = druid_cpyU8 (buf, size);
      for (o = 0; o < size; o++) {
        buf = druid_cpyU8 (buf,
                     *(__u8 *) (DRUID_MSG (msg) +
                                DRUID_MSG_ID_SIZE (msg) + o));
      }
      writeBuf[tmp] = o;

      druid_write (writeBuf, buf - writeBuf);
    }
    else {
      writeBuf[0] = DRUID_CMD_MSG_GET;
      writeBuf[1] = DRUID_MARK_END;
      druid_write (writeBuf, 2);
    }
  }
  else {
    writeBuf[0] = DRUID_CMD_MSG_GET;
    writeBuf[1] = DRUID_MARK_END;
    druid_write (writeBuf, 2);
  }
}
#if DRUID_MSG_HOOKS 
/**
 * Message trace
 */
static void
druid_msgTraceDump (void * h, __u8 hookType)
{
  sc_msg_hd_t * mh = (sc_msg_hd_t *)h;
  druid_tf_t frame;

  if (druid_ti.trace & DRUID_TRACE_ON) {
    frame.timestamp = druid_timeGet ();
    frame.hookType = hookType;
    frame.from = mh->sender;
    frame.to = mh->addressee;
    frame.size = mh->size < CNF_DRUID_MSG_DATA_MAX ? mh->size : 
                   CNF_DRUID_MSG_DATA_MAX;
    druid_ringPut ((__u8 *) &frame, sizeof (druid_tf_t));
    druid_ringPut ((__u8 *) mh + 32, frame.size);
  }
}

#if SC_MSGTX_HOOK == 1 || SC_MSGRX_HOOK == 1
static void
druid_msgHook (const sc_msg_hd_t * mh, __u8 hookType)
{
  int i, j;

  if (druid_ti.trace & DRUID_TRACE_DISPLAY) {
    return;
  }
  
  for (i = 0, j = 0; i < CNF_DRUID_OPTION_MAX && j < druid_ti.nma; i++) {
    if (druid_option[i].hookType) {
      ++j;
      if (druid_option[i].call && 
	  druid_option[i].hookType == hookType &&
          (druid_option[i].from & druid_option[i].fromMask) == (mh->sender & druid_option[i].fromMask) &&
          (druid_option[i].to & druid_option[i].toMask) == (mh->addressee & druid_option[i].toMask) &&
          (druid_option[i].id & druid_option[i].idMask) == (mh->id & druid_option[i].idMask))
      {
        druid_option[i].call ((void *)mh, hookType);
      }
    }
  }
}
#endif

#if SC_MSGTX_HOOK == 1
static void
druid_msgTxHook (const sc_msg_hd_t * mh)
{
  druid_msgHook (mh, DRUID_MSG_HOOK_TX);
}
#endif

#if SC_MSGRX_HOOK == 1
static void
druid_msgRxHook (const sc_msg_hd_t * mh)
{
  druid_msgHook (mh, DRUID_MSG_HOOK_RX);
}
#endif
#endif

#if DRUID_PROC_HOOKS 
/**
 * Process trace
 */
typedef struct druid_pt_s {
  const sc_pcb_t * from;
  const sc_pcb_t * to;
} druid_pt_t;

static void
druid_procTraceDump (void * h, __u8 hookType)
{
  druid_pt_t * ph = (druid_pt_t *)h;
  druid_tf_t frame;

  if (druid_ti.trace & DRUID_TRACE_ON) {
    frame.timestamp = druid_timeGet ();
    frame.hookType = hookType;
    frame.from = ph->from ? ph->from->pid : SC_ILLEGAL_PID;
    frame.to = ph->to ? ph->to->pid : SC_ILLEGAL_PID;
    frame.size = 0; 
    druid_ringPut ((__u8 *) &frame, sizeof (druid_tf_t));
  }
}

#if SC_PROCCREATE_HOOK == 1 || SC_PROCKILL_HOOK == 1 || SC_PROCSWAP_HOOK == 1  
static void
druid_procHook (const sc_pcb_t * from, const sc_pcb_t * to, __u8 hookType)
{
  int i, j;
  druid_pt_t pt;

  if (druid_ti.trace & DRUID_TRACE_DISPLAY) {
    return;
  }
  
  for (i = 0, j = 0; i < CNF_DRUID_OPTION_MAX && j < druid_ti.nma; i++) {
    if (druid_option[i].hookType) {
      ++j;
      if (druid_option[i].call && druid_option[i].hookType == hookType &&
          (druid_option[i].from & druid_option[i].fromMask) ==
          (from->pid & druid_option[i].fromMask) &&
          (druid_option[i].to & druid_option[i].toMask) ==
          (to->pid & druid_option[i].toMask)) {
	pt.from = from;
	pt.to = to;
        druid_option[i].call (&pt, hookType);
      }
    }
  }
}
#endif

#if SC_PROCCREATE_HOOK == 1
static void
druid_procCreateHook (const sc_pcb_t * from, const sc_pcb_t * to)
{
  druid_procHook (from, to, DRUID_PROC_HOOK_CREATE);
}
#endif

#if SC_PROCKILL_HOOK == 1
static void
druid_procKillHook (const sc_pcb_t * from, const sc_pcb_t * to)
{
  druid_procHook (from, to, DRUID_PROC_HOOK_KILL);
}
#endif

#if SC_PROCSWAP_HOOK == 1
static void
druid_procSwapHook (const sc_pcb_t * from, const sc_pcb_t * to)
{
  druid_procHook (from, to, DRUID_PROC_HOOK_SWAP);
}
#endif
#endif

#if DRUID_POOL_HOOKS 
/**
 * Pool trace
 */
typedef struct druid_lt_s {
  const sc_pool_cb_t *plcb;
  sc_pid_t causer;
} druid_lt_t;

static void
druid_poolTraceDump (void * h, __u8 hookType)
{
  druid_lt_t *lt = (druid_lt_t *)h;
  druid_tf_t frame;

  if (druid_ti.trace & DRUID_TRACE_ON) {
    frame.timestamp = druid_timeGet ();
    frame.hookType = hookType;
    frame.from = lt->causer;
    frame.to = lt->plcb->id;
    frame.size = 0; 
    druid_ringPut ((__u8 *) &frame, sizeof (druid_tf_t));
  }
}

#if SC_POOLCREATE_HOOK == 1 || SC_POOLKILL_HOOK == 1  
static void
druid_poolHook (const sc_pool_cb_t * plcb, sc_pid_t causer, __u8 hookType)
{
  int i, j;
  druid_lt_t lt;

  if (druid_ti.trace & DRUID_TRACE_DISPLAY) {
    return;
  }
  
  for (i = 0, j = 0; i < CNF_DRUID_OPTION_MAX && j < druid_ti.nma; i++) {
    if (druid_option[i].hookType) {
      ++j;
      if (druid_option[i].call && druid_option[i].hookType == hookType &&
          (druid_option[i].from & druid_option[i].fromMask) ==
          (causer & druid_option[i].fromMask) &&
          (druid_option[i].to & druid_option[i].toMask) ==
          (plcb->id & druid_option[i].toMask)) {
	lt.plcb = plcb;
	lt.causer = causer;
        druid_option[i].call (&lt, hookType);
      }
    }
  }
}
#endif

#if SC_POOLCREATE_HOOK == 1
static void
druid_poolCreateHook (const sc_pool_cb_t * plcb)
{
  druid_poolHook (plcb, plcb->creator, DRUID_POOL_HOOK_CREATE);
}
#endif

#if SC_POOLKILL_HOOK == 1
static void
druid_poolKillHook (const sc_pool_cb_t * plcb)
{
  druid_poolHook (plcb, 0, DRUID_POOL_HOOK_KILL);
}
#endif
#endif


#if DRUID_MSG_HOOKS || DRUID_PROC_HOOKS || DRUID_POOL_HOOKS 
/**
 * Start/Stop/Break functions 
 */
static void
druid_traceStart (void * h, __u8 hookType)
{
  druid_ti.trace = DRUID_TRACE_ON;
}

static void
druid_traceStop (void * h, __u8 hookType)
{
  druid_ti.trace = DRUID_TRACE_OFF;
}

static void
druid_traceBreak (void * h, __u8 hookType)
{
  druid_stop ();
}

/**
 * Enter message action
 */
static void
druid_optionEnter()
{
  int i;

  /* look for a free place */
  for (i = 0; i < CNF_DRUID_OPTION_MAX && druid_option[i].hookType; i++){
    /* empty */
  }
  if (i < CNF_DRUID_OPTION_MAX) {
    ++druid_ti.nma;
    druid_option[i].hookType = readreq.buf[1];
    switch (readreq.buf[2]) {
    case DRUID_OPTION_STOP:
      druid_option[i].call = druid_traceStop;
      break;
    case DRUID_OPTION_START:
      druid_option[i].call = druid_traceStart;
      break;
    case DRUID_OPTION_BREAK:
      druid_option[i].call = druid_traceBreak;
      break;
    default:
      if (druid_option[i].hookType == DRUID_MSG_HOOK_TX ||
	  druid_option[i].hookType == DRUID_MSG_HOOK_RX) { 
        druid_option[i].call = druid_msgTraceDump;
      }
      else if (druid_option[i].hookType == DRUID_PROC_HOOK_CREATE ||
	       druid_option[i].hookType == DRUID_PROC_HOOK_KILL ||
	       druid_option[i].hookType == DRUID_PROC_HOOK_SWAP) {
        druid_option[i].call = druid_procTraceDump;
      }	
      else if (druid_option[i].hookType == DRUID_POOL_HOOK_CREATE ||
	       druid_option[i].hookType == DRUID_POOL_HOOK_KILL) {
        druid_option[i].call = druid_poolTraceDump;
      }	
      else {
      }
      break;
    }
    druid_option[i].from = druid_read32(&readreq.buf[3]);
    druid_option[i].fromMask = druid_read32(&readreq.buf[7]);
    druid_option[i].to = druid_read32(&readreq.buf[11]);
    druid_option[i].toMask = druid_read32(&readreq.buf[15]);
    druid_option[i].id = druid_read32(&readreq.buf[19]);
    druid_option[i].idMask = druid_read32(&readreq.buf[23]);

  }
  writeBuf[0] = DRUID_CMD_OPTION_ENTER;
  writeBuf[1] = DRUID_MARK_END;
  writeBuf[2] = CNF_DRUID_OPTION_MAX - druid_ti.nma;
  druid_write (writeBuf, 3);
}

/**
 * Display message action
 */
static void
druid_optionDisplay()
{
  __u8 num = readreq.buf[1];

  do {
    ++num;
  } while (num < CNF_DRUID_OPTION_MAX && !druid_option[num].hookType);

  if (num < CNF_DRUID_OPTION_MAX) {
    __u8 * buf = writeBuf;

    buf = druid_cpyU8 (buf, DRUID_CMD_OPTION_DISPLAY);
    buf = druid_cpyU8 (buf, DRUID_MARK_MORE);
    buf = druid_cpyU8 (buf, num);
    buf = druid_cpyU8 (buf, druid_option[num].hookType);
    if (druid_option[num].call == druid_traceStop) {
      buf = druid_cpyU8 (buf, DRUID_OPTION_STOP);
    }
    else if (druid_option[num].call == druid_traceStart) {
      buf = druid_cpyU8 (buf, DRUID_OPTION_START);
    }
    else if (druid_option[num].call == druid_traceBreak) {
      buf = druid_cpyU8 (buf, DRUID_OPTION_BREAK);
    }
    else {
      buf = druid_cpyU8 (buf, DRUID_OPTION_DUMP);
    }
    buf = druid_cpyU32 (buf, druid_option[num].from);
    buf = druid_cpyU32 (buf, druid_option[num].fromMask);
    buf = druid_cpyU32 (buf, druid_option[num].to);
    buf = druid_cpyU32 (buf, druid_option[num].toMask);
    buf = druid_cpyU32 (buf, druid_option[num].id);
    buf = druid_cpyU32 (buf, druid_option[num].idMask);

    druid_write (writeBuf, buf - writeBuf);
  }
  else {
    writeBuf[0] = DRUID_CMD_OPTION_DISPLAY;
    writeBuf[1] = DRUID_MARK_END;
    druid_write (writeBuf,2);
  }
}

/**
 * Clear message action
 */
static void
druid_optionClear()
{
  __u8 num = readreq.buf[1];

  druid_ti.trace = DRUID_TRACE_OFF;

  do {
    ++num;
  } while (num < CNF_DRUID_OPTION_MAX && !druid_option[num].hookType);

  if (num < CNF_DRUID_OPTION_MAX) {
    --druid_ti.nma;
    memset (&druid_option[num], 0, sizeof (druid_option_t));

    writeBuf[0] = DRUID_CMD_OPTION_CLEAR;
    writeBuf[1] = DRUID_MARK_MORE;
    writeBuf[2] = num;
    druid_write (writeBuf,3);
  }
  else {
    memset (&druid_option[num], 0, sizeof (druid_option_t));

    writeBuf[0] = DRUID_CMD_OPTION_CLEAR;
    writeBuf[1] = DRUID_MARK_END;
    druid_write (writeBuf,2);
  }
}

/**
 * Display message trace 
 */
static void
druid_traceDisplay (void)
{
#ifdef MSGID_16BIT
  __u16 id16;
#else
  __u32 id32;
#endif
  __u32 id = 0;
  druid_tf_t frame;

  druid_ti.trace |= DRUID_TRACE_DISPLAY;
  
  if (druid_ti.count) {
    __u8 * buf = writeBuf;
    druid_ringGet ((__u8 *) &frame, sizeof (druid_tf_t));
    buf = druid_cpyU8 (buf, DRUID_CMD_TRACE_DISPLAY);
    buf = druid_cpyU8 (buf, DRUID_MARK_MORE);
    buf = druid_cpyU32 (buf, frame.timestamp);
    buf = druid_cpyU8 (buf, frame.hookType);
    buf = druid_cpyU32 (buf, frame.from);
    buf = druid_cpyU32 (buf, frame.to);
    if (frame.hookType == DRUID_MSG_HOOK_TX || 
	frame.hookType == DRUID_MSG_HOOK_RX) {
      /* first 2 or 4 Bytes are the msg id only for msg trace frames */
#ifdef MSGID_16BIT
      druid_ringGet ((__u8 *) &id16, 2);
      frame.size -= 2;
      id = id16;
#else 
      druid_ringGet ((__u8 *) &id32, 4);
      frame.size -= 4;
      id = id32;
#endif
    }
    buf = druid_cpyU32 (buf, id);
    buf = druid_cpyU8 (buf, frame.size);
    druid_ringGet (buf, frame.size);
    buf += frame.size;

    druid_write (writeBuf,buf - writeBuf);
  }
  else {
    druid_ti.trace &= ~DRUID_TRACE_DISPLAY;
    writeBuf[0] = DRUID_CMD_TRACE_DISPLAY;
    writeBuf[1] = DRUID_MARK_END;
    druid_write (writeBuf,2);
  }
}
#endif

/**
 * Text display methode
 */
void
druid_textDisplay (const char *text)
{
  __u8 * buf = writeBuf;
  buf = druid_cpyU8 (buf, DRUID_CMD_TEXT_DISPLAY);
  buf = druid_cpyU8 (buf, DRUID_MARK_END);
  buf = druid_cpyStr (buf, text);

  druid_write (writeBuf,buf - writeBuf);
}

#if SC_ERR_HOOK == 1
/**
 * Error get
 */
static void
druid_errorDisplay (void)
{
  sc_pcb_t *pcb = (sc_pcb_t *) druid_err.pcb;
  __u8 * buf = writeBuf;

  buf = druid_cpyU8 (buf, DRUID_CMD_ERROR_DISPLAY);
  buf = druid_cpyU8 (buf, DRUID_MARK_END);
  buf = druid_cpyU32 (buf, (int) pcb > 1 ? pcb->pid : 0);
  buf = druid_cpyStr (buf, (int) pcb > 1 ? pcb->name : "xx");
  buf = druid_cpyU32 (buf, (int) pcb > 1 ? pcb->cline : 0);
  buf = druid_cpyStr (buf, (int) pcb > 1 ? pcb->cfile : "xx");
  buf = druid_cpyU32 (buf, druid_err.pcb);
  buf = druid_cpyU32 (buf, druid_err.code);
  buf = druid_cpyU32 (buf, druid_err.extra);
  buf = druid_cpyU32 (buf, druid_err.user);

  druid_write (writeBuf,buf - writeBuf);
  druid_err.valid = 0;
}

/**
 * Error condition
 */
int
druid_errorHook (sc_errcode_t err, 
		 sc_extra_t extra, 
		 int user, 
		 const sc_pcb_t * pcb)
{
  druid_err.valid = 1;
  druid_err.code = (__u32) err;
  druid_err.extra = (__u32) extra;
  druid_err.user = (__u32) user;
  druid_err.pcb = (__u32) pcb;

  druid_errorDisplay ();

  druid_main(0);

  druid_err.valid = 0;
  return 0;
}
#endif

