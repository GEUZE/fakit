#ifndef _SC_CONF_H_
#define _SC_CONF_H_         0x42

#define SCIOPTA_CNF         1
#define SCIOPTA             NATIVE_HOSTED

#define CPU                 ARM
#define CPUTYPE             ARMv7M

#define SC_HAS_FPU          0
#define SC_HAS_MMU          0

#define SC_MAX_MODULE       1
#define SC_MAX_CONNECTOR    0
#define SC_MAX_NUM_BUFFERSIZES 8
#define SC_USE_FRIENDS      0
#define SC_USE_TRAP         0
#define SC_NEVER_COPY       0

#define SC_CDEBUG           1
#define SC_ASYNC_TMO        1

#define SC_PROC_HOOK        1
#define SC_PROCCREATE_HOOK  1
#define SC_PROCKILL_HOOK    1
#define SC_PROCSWAP_HOOK    1
#define SC_PROCMMU_HOOK     0
#define SC_PROC_STAT        1
#define SC_ERR_HOOK         1

#define SC_POOL_HOOK        1
#define SC_POOLKILL_HOOK    1
#define SC_POOLCREATE_HOOK  1

#define SC_STACK_CHECK      1
#define SC_PROC_PARA_CHECK  1
#define SC_POOL_PARA_CHECK  1

#define SC_MSG_HOOK         1
#define SC_MSGRX_HOOK       1
#define SC_MSGTX_HOOK       1
#define SC_MSG_PARA_CHECK   1
#define SC_MSG_CHECK        1
#define SC_MSG_STAT         1

#define SC_KERNELSTACK_SIZE 512
#define SC_IRQSTACK_SIZE    1024

#define SC_MAX_INT_VECTORS  60
#define SC_MAX_INT_VECTOR   (60-1)

#endif
