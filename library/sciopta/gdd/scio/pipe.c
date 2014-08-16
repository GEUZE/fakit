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
** ID: S03070CL30                                                   **
** +Revision: 1.19 +                                                 **
** +Date: 2006/05/23 14:29:14 +                                     **
** pipe                                                             **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>
#include <errno.h>
#include <dev/pipedev.h>
#include <sdd/sdd.h>

union sc_msg {
  sc_msgid_t id;
  sdd_devDualopen_t open;
};

int
pipe (int fd[2])
{
  fd_tab_t NEARPTR fd_tab;

#ifdef SIMPLE
  char dev[] = "pipe";
#else
  char dev[] = "/dev/pipe";
#endif

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC) {
      sdd_obj_t NEARPTR man, *h;
      const char *devName;
      sc_msg_t msg;
      static const sc_msgid_t select[3] = {
	SDD_ERROR, SDD_DEV_OPEN_REPLY, 0
      };
      fd[0] = 0;
      while (fd[0] < fd_tab->max_fd && fd_tab->bf[fd[0]]) {
        ++fd[0];
      }
      fd[1] = 0;
      while (fd[1] < fd_tab->max_fd && fd_tab->bf[fd[1]]) {
        ++fd[1];
      }
      if (fd[0] >= fd_tab->max_fd && fd[1] >= fd_tab->max_fd) {
	sc_miscErrnoSet (EMFILE);
	return -1;
      }
#ifdef SIMPLE
      man = fd_tab->manager;
      devName = dev;
#else
      man = sdd_objResolve (fd_tab->manager, dev, &devName);
      if (!devName) {
	sc_miscErrnoSet (EISDIR);
	return -1;
      }
#endif
      h = sdd_manGetByName (man, devName);
      msg =
	sc_msgAlloc (sizeof (sdd_devDualopen_t), SDD_DEV_DUALOPEN,
		     SC_DEFAULT_POOL, SC_FATAL_IF_TMO);
      msg->open.base.error = 0;
      msg->open.base.handle = h->base.handle;
      sc_msgTx (&msg, h->controller, 0);
      msg = sc_msgRx (SC_ENDLESS_TMO,  select, SC_MSGRX_MSGID);
      if (msg->open.base.error) {
	sc_miscErrnoSet (msg->open.base.error);
	sc_msgFree (&msg);
	return -1;
      }
      fd_tab->bf[fd[0]] = (sdd_obj_t NEARPTR)msg->open.base.handle;
      fd_tab->bf[fd[1]] = (sdd_obj_t NEARPTR)msg->open.handle2;
      return 0;
    }
  }
  sc_miscErrnoSet (EMFILE);
  return -1;
}

