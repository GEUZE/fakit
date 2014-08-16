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
** ID: S03030CL1                                                    **
** +Revision: 1.11 +                                                 **
** +Date: 2008/01/16 13:02:10 +                                     **
** Trace Rx/Tx within a shell.                                      **
**********************************************************************
EOC*/
#include <sciopta.h>
#undef __STRICT_ANSI__
#include <scio.h>
#include <string.h>
#include <ossys/libsc.h>
#include <sh/sh.h>

#define TRACE_MAX		512	
#define TRACE_DATA_MAX	        16	

#ifdef NC30
#define memcpy _n_n_memcpy
#endif

typedef struct trace_info_s {
  sc_time_t tstamp; 
  sc_pid_t from; 
  sc_pid_t to; 
  sc_msgid_t id; 
  char data[TRACE_DATA_MAX];
} trace_info_t;

typedef struct trace_conf_s {
  int trace_max, data_max, in, out, count;
  int data;
  sc_time_t start;
  trace_info_t trace[TRACE_MAX];
} trace_conf_t;

trace_conf_t trace_rxConf = { TRACE_MAX, TRACE_DATA_MAX, 0 };

trace_conf_t trace_txConf = { TRACE_MAX, TRACE_DATA_MAX, 0 };

static void
traceHook (trace_conf_t NEARPTR dump, const sc_msg_hd_t NEARPTR mh)
{
  if (dump->in != dump->out || dump->count == 0) {
    dump->trace[dump->in].tstamp = sc_tickGet () - dump->start;
    dump->trace[dump->in].from = mh->sender;
    dump->trace[dump->in].to = mh->addressee;
    dump->trace[dump->in].id = mh->id;
    memcpy (dump->trace[dump->in].data, (char NEARPTR)mh+36, dump->data);
    dump->in = (dump->in + 1) & (dump->trace_max - 1);
    ++dump->count;
  }
}
static void
trace_usage (const char *name)
{
  fdprintf (scio_stdout, "Usage: %s [OPTION]... [STRING]...\n", name);
  fdprintf (scio_stdout, "  -b     start trace\n");
  fdprintf (scio_stdout, "  -e     stop trace\n");
  fdprintf (scio_stdout, "  -p<no> print <no> lines of trace\n");
  fdprintf (scio_stdout, "  -h     display this help text\n");
}

void
trace_rxHook (const sc_msg_hd_t NEARPTR mh) 
{
  traceHook (&trace_rxConf, mh);
}

void
trace_txHook (const sc_msg_hd_t NEARPTR mh) 
{
  traceHook (&trace_txConf, mh);
}

int
sh_trace (int argc, char *argv[])
{
  int i, c;
  int no;
  opt_r_t options;
  sc_msgHook_t *oldTx;
  sc_msgHook_t *oldRx;

  initopt_r (&options);
  while ((c = getopt_r (&options, argc, argv, "bep:h")) != -1) {
    switch (c) {
    case 'b':
      sc_msgHookRegister (SC_SET_MSGTX_HOOK, trace_txHook);
      sc_msgHookRegister (SC_SET_MSGRX_HOOK, trace_rxHook);
      trace_txConf.start = trace_rxConf.start = sc_tickGet ();
      return 0;
    case 'e':
      sc_msgHookRegister (SC_SET_MSGTX_HOOK, 0);
      sc_msgHookRegister (SC_SET_MSGRX_HOOK, 0);
      return 0;
    case 'p':
      no = dectoi (options.optarg);
      if (!no) {
	no = 20;
      }
      /* stop trace if running */
      oldTx = sc_msgHookRegister (SC_SET_MSGTX_HOOK, 0);
      oldRx = sc_msgHookRegister (SC_SET_MSGRX_HOOK, 0);
      /* print entries */
      for (i = 0; 
	  i < no && (trace_rxConf.count || trace_txConf.count);
	  i++) {
	if (trace_txConf.count) {
	  fdprintf (scio_stdout, "[%10d] Tx; snd:%08x addr:%08x id:%08x\n", 
                    trace_txConf.trace[trace_txConf.out].tstamp,
                    trace_txConf.trace[trace_txConf.out].from, 
		    trace_txConf.trace[trace_txConf.out].to, 
		    trace_txConf.trace[trace_txConf.out].id);
          trace_txConf.out = (trace_txConf.out + 1) 
                           & (trace_txConf.trace_max - 1);
          --trace_txConf.count;
	}
	if (trace_rxConf.count) {
	  fdprintf (scio_stdout, "[%10d] Rx; snd:%08x addr:%08x id:%08x\n", 
                    trace_txConf.trace[trace_txConf.out].tstamp,
                    trace_rxConf.trace[trace_rxConf.out].from, 
		    trace_rxConf.trace[trace_rxConf.out].to, 
		    trace_rxConf.trace[trace_rxConf.out].id);
          trace_rxConf.out = (trace_rxConf.out + 1) 
                           & (trace_rxConf.trace_max - 1);
          --trace_rxConf.count;
	}
      }
      /* start trace if it was running before */
      sc_msgHookRegister (SC_SET_MSGTX_HOOK, oldTx);
      sc_msgHookRegister (SC_SET_MSGTX_HOOK, oldRx);
      return 0;
    case 'h':
      trace_usage (argv[0]);
      return 0;
    default:
      break;
    }
  }
  trace_usage (argv[0]);
  return 0;
}
