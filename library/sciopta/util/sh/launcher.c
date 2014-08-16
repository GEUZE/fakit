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
** ID: S03070CL71                                                   **
** +Revision: 1.34 +                                                **
** +Date: 2008/01/16 13:02:10 +                                     **
** shell: launcher                                                  **
**********************************************************************
EOC*/
#include <sciopta.h>
#include <ossys/libsc.h>
#include <sh/sh.msg>
#include <sh/sh.h>

#include <string.h>
#include <unistd.h>

#if defined __ARMCC_VERSION
/* suppress: assignment in condition warning */
#pragma diag_suppress 1293
#endif

union sc_msg {
  sc_msgid_t id;
  sh_inherit_t inherit;
  sh_dead_t dead;
};

/* add sh_builtins statical */
extern sh_process_t sh_processes[];

static const sh_process_t errorCmd = {
  "error", "", SH_NOT_BE_PROC, 0, sh_error
};

int
sh_launcher (sc_msg_t msg, int bg, sc_pid_t * pid)
{
  fd_tab_t NEARPTR fd_tab;
  int i, ret;
  const sh_process_t *cmd;
  static const sc_msgid_t sel[2] = { SH_END, 0 };
  /* conservativ */
  ret = 0;
  /* santity check */
  if (msg->id != SH_BEGIN) {
    sc_msgFree (&msg);
    PRINTF ("this is not a arg message\n");
    return -1;
  }
  if (!msg->inherit.argc) {
    PRINTF ("no command\n");
    sc_msgFree (&msg);
    return -1;
  }
  /* get process function*/
  i = 0;
  while (sh_processes[i].main != NULL
	 && strncmp (sh_processes[i].name, msg->inherit.argv[0],
		     SC_NAME_MAX) != 0) {
    ++i;
  }
  if (!sh_processes[i].main) {
    /* load process from filesystem */
  }

  if (sh_processes[i].main) {
    cmd = &sh_processes[i];
  }
  else {
    cmd = &errorCmd;
  }
  if (cmd->flags == SH_NOT_BE_PROC || (cmd->flags == SH_CAN_BE_PROC && !bg)) {
    ret = cmd->main (msg->inherit.argc, msg->inherit.argv);
    for (i = 0; i < msg->inherit.argc; i++) {
      if (msg->inherit.argv[i]) {
	sc_msgFree ((sc_msgptr_t) &msg->inherit.argv[i]);
      }
    }
    sc_msgFree (&msg);
    return ret;
  }
  else if ((cmd->flags == SH_CAN_BE_PROC && bg)
	   || cmd->flags == SH_MUST_BE_PROC
	   || cmd->flags == SH_MUST_BE_ZOMBIE) {
    /* connect main to message */
    msg->inherit.main = cmd->main;
    /* get a copy of the fd_tab */
    if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
      /* copy the fd_tab here, target should acquire the copied fd_tab */
      msg->inherit.fd_tab = fdTabDuplicate ();
      if (bg) {
	msg->inherit.fd_tab->flags |= SCIO_FLAGS_BACKGROUND;
      }
    }
    else {
      msg->inherit.fd_tab = (fd_tab_t NEARPTR)0;
    }
    if ((*pid = sc_procTmpCreate (msg->inherit.argv[0], sh_process,
	                          cmd->stacksz, SC_DEFAULT_POOL))) {
      sc_msgTx (&msg, *pid, 0);

      if (cmd->flags != SH_MUST_BE_ZOMBIE) {
	msg = sc_msgAlloc (sizeof (sh_dead_t),
			   SH_END,
			   SC_DEFAULT_POOL,
			   SC_ENDLESS_TMO);

	sc_procObserve (&msg, *pid);
      }
      if (!bg && cmd->flags != SH_MUST_BE_ZOMBIE) {
	msg = sc_msgRx (SC_ENDLESS_TMO, sel, SC_MSGRX_MSGID);
	PRINTF1 ("SH_END received: %d\n", msg->dead.err);
	ret = (int)msg->dead.err;
	sc_msgFree (&msg);
      }
    }
    else {
      write (2, "scshell: not able to launch\n",
	     strlen ("scshell: not able to launch\n"));
    }
    return ret;
  }
  return ret;
}
