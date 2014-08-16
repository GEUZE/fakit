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
** ID: S03070CL76                                                   **
** +Revision: 1.24 +                                                 **
** +Date: 2008/01/16 13:02:10 +                                     **
** shell process                                                    **
**********************************************************************
EOC*/
#include <sciopta.h>
#include <scio.h>
#include <sh/sh.msg>
#include <sh/sh.h>
#if defined(__IAR_SYSTEMS_ICC__) || \
    (defined(__CM16C__) && defined(__TASKING__))
#include <sys/unistd.h>
#else
#include <unistd.h>
#endif

union sc_msg {
  sc_msgid_t id;
  sh_inherit_t inherit;
};

SC_PROCESS (sh_process)
{
  sc_msg_t msg;
  static const sc_msgid_t sel[2] = { SH_BEGIN, 0 };
  sc_errcode_t err;
  int i;
  PRINTF ("init the procVAR stuff\n");
  msg = sc_msgAlloc (SC_TAG_SIZE * 2, 0, SC_DEFAULT_POOL, SC_ENDLESS_TMO);
  sc_procVarInit (&msg, 2);
  PRINTF ("wait on the SH_BEGIN message\n");
  msg = sc_msgRx (SC_ENDLESS_TMO, sel, SC_MSGRX_MSGID);
  switch (msg->id) {
  case SH_BEGIN:
    PRINTF ("SH_INHERIT message\n");
    /* acquire everything, so if I get killed the pool is cleaned up */
    for (i = 0; i < msg->inherit.argc; i++) {
      if (msg->inherit.argv[i]) {
	PRINTF1 ("acquire arg %d\n", i);
	sc_msgAcquire ((sc_msgptr_t) &msg->inherit.argv[i]);
      }
    }
    if (msg->inherit.fd_tab) {
      PRINTF ("acquire fd array\n");
      sc_msgAcquire ((sc_msgptr_t) &msg->inherit.fd_tab);
      sc_msgAcquire ((sc_msgptr_t) &msg->inherit.fd_tab->manager);
      for( i = 0; i < msg->inherit.fd_tab->max_fd; ++i){
	if (msg->inherit.fd_tab->bf[i]) {
	  sc_msgAcquire ((sc_msgptr_t) &msg->inherit.fd_tab->bf[i]);
	}
      }
      sc_procVarSet (SCIO_PROCVAR_ID, (sc_var_t) msg->inherit.fd_tab);
    }
    else {
      PRINTF ("no fds\n");
      err = (sc_errcode_t)-1;
      break;
    }
    PRINTF ("start main programm\n");
    err = msg->inherit.main (msg->inherit.argc, msg->inherit.argv);
    closeall();
    sdd_objFree (&msg->inherit.fd_tab->manager);
    break;
  default:
    PRINTF1 ("do not understand this message: %x\n", msg->id);
    err = (sc_errcode_t)-1;
    break;
  }
  PRINTF ("programm terminates\n");
  sc_msgFree (&msg);
  sc_miscErrnoSet (err);
  sc_procKill (SC_CURRENT_PID, 0);
}
