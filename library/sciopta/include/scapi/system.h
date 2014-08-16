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
** ID: S06059BS1                                                    **
** +Revision: 1.9 +                                                 **
** +Date: 2006/05/12 13:48:05 +                                     **
** Global system data                                               **
**********************************************************************
EOC*/
#ifndef _KERNEL_SYSTEM_H_
#define _KERNEL_SYSTEM_H_ 2

/*
** Sciopta on win32 management structure
*/
#define SCIOPTA_MAGIC 0x20102001

#if SC_PROC_STAT == 1
#define PROC_STAT(a) a
#else
#define PROC_STAT(a)
#endif

typedef void (*sys_putchar)(int);

/*
** API configuration
** (Will be copied.)
*/
typedef struct sciopta_config_s
{
  char systemName[32];
  sys_putchar putchar;
  __u32 maxConnector;
  __u32 maxPool;
} sciopta_config_t;

/*
** API global data
*/
typedef struct sciopta_s {
  __u32 magic;                  /* a magic number */
  char systemName[32];          /* needed to identify the system */
  dbl_t pcbs;                   /* list of PCBs */
  sc_pcb_t **connectors;        /* array of connectors (runtime allocated) */
  sc_pool_cb_t **pools;         /* array of pools (runtime allocated) */
  PROC_STAT(__u32 swaps);       /* number of swaps sincs API start */
  sys_putchar putchar;          /* putchar for kprintf */
  __u32 nConnectors;            /* number of currently registered connectors */
  __u32 maxConnector;           /* maximum number of connectors */
  __u32 nPool;                  /* number of currently created pools */
  __u32 maxPool;                /* maximum number of pools */
  long int nPcb;                /* number of processes in list */
  __u32 nextPid;                /* next PID to choose */
  sc_errHook_t *errHook;         /* ? */
  __u32 prioCount[32];
  __u32 prioMask;
#ifdef WIN32
  DWORD startTime;              /* API init time in ms */
  HANDLE globalLock;            /* global locking (not used ATM) */
  CRITICAL_SECTION poolMutex;   /* mutex for pool-arry */
#else
  pthread_mutex_t poolMutex;
  pthread_mutex_t globalLock;
  __u32 startTime;
#endif
} sciopta_t;

/*
** API calls with no distinct place...
*/

SCFUNC1(DLLEXP int,sc_sysApiInit,const sciopta_config_t *);
SCFUNC0(DLLEXP void,sc_sysApiExit);
SCFUNC0(DLLEXP __u32,sc_sysApiVersionGet);
SCFUNC3(DLLEXP void,kprintf,int,const char *,...);

// scapi internal function
void makeReady(sc_pcb_t *pcb);
void makeNotReady(sc_pcb_t *pcb);
int isHighestPrioProc(sc_pcb_t *pcb);
void waitTilHighest(sc_pcb_t *pcb);
/*
** Some handy macros
*/
#ifdef WIN32
#define INIT_MUTEX(mutex) InitializeCriticalSection(mutex)
#define LOCK_MUTEX(mutex) EnterCriticalSection(mutex)
#define UNLOCK_MUTEX(mutex) LeaveCriticalSection(mutex);
#define INCR_ATOMIC(var) InterlockedIncrement(&(var))
#define DECR_ATOMIC(var) InterlockedDecrement(&(var))
#define NOTIFY_COND(var) SetEvent(var)
#define COND_WAIT(var) 
#else
#define INIT_MUTEX(mutex) pthread_mutex_init(mutex, NULL)
#define LOCK_MUTEX(mutex) pthread_mutex_lock(mutex)
#define UNLOCK_MUTEX(mutex) pthread_mutex_unlock(mutex)
#define INCR_ATOMIC(var) ++(var)
#define DECR_ATOMIC(var) --(var)
#define NOTIFY_COND(var) pthread_cond_signal(&var)
#define COND_WAIT(var)   pthread_cond_wait(&var)
#endif

#define DBG(s) kprintf(1,"%d:%s\n",__LINE__,s)
#define TMO(tmo) (tmo == SC_ENDLESS_TMO ? INFINITE : tmo)


#endif /* _KERNEL_SYSTEM_H_ */
