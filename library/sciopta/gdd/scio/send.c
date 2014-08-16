/*SOC -*-c++-*-
**********************************************************************
**  _______  _______ _________ _______	_______ _________ _______   **
** (  ____ \(  ____ \\__   __/(	 ___  )(  ____ )\__   __/(  ___	 )  **
** | (	  \/| (	   \/	) (   | (   ) || (    )|   ) (	 | (   ) |  **
** | (_____ | |		| |   | |   | || (____)|   | |	 | (___) |  **
** (_____  )| |		| |   | |   | ||  _____)   | |	 |  ___	 |  **
**	 ) || |		| |   | |   | || (	   | |	 | (   ) |  **
** /\____) || (____/\___) (___| (___) || )	   | |	 | )   ( |  **
** \_______)(_______/\_______/(_______)|/	   )_(	 |/	\|  **
**								    **
** (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz	    **
**								    **
**********************************************************************
** ID: S03070CL18						    **
** +Revision: 1.19 +						     **
** +Date: 2008/01/16 13:02:00 +					    **
** send								    **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <scio.h>
#include <scio.msg>
#include <ips/addr.h>
#include <ips/connect.h>
#include <sdd/sdd.h>

union sc_msg {
  sc_msgid_t id;
  scio_send_t send;
};

ssize_t
send (int sd, const void *buf, size_t n, int flags)
{
  sc_msg_t msg;
  int ret;
  fd_tab_t NEARPTR fd_tab;

  static const sc_msgid_t select[3] = {
    SDD_ERROR, SCIO_SEND_REPLY, 0
  };

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC &&
	sd >= 0 && sd <= fd_tab->max_fd && fd_tab->bf[sd]) {
      msg =
	sc_msgAlloc (sizeof (scio_send_t), SCIO_SEND,
		     sc_msgPoolIdGet ((sc_msgptr_t) &fd_tab->bf[sd]),
		     SC_FATAL_IF_TMO);
      msg->send.write.base.error = 0;
      msg->send.write.base.handle = fd_tab->bf[sd]->base.handle;
      msg->send.write.size = n;
      msg->send.write.curpos = 0;
      msg->send.write.outlineBuf = buf;
      msg->send.flags = flags;
      sc_msgTx (&msg, fd_tab->bf[sd]->sender, 0);
      msg = sc_msgRx (SC_ENDLESS_TMO,  select, SC_MSGRX_MSGID);
      if (msg->send.write.base.error) {
	sc_miscErrnoSet (msg->send.write.base.error);
	sc_msgFree (&msg);
	return -1;
      }
      else {
	ret = msg->send.write.size;
	sc_msgFree (&msg);
	return ret;
      }
    }
    else {
      sc_miscErrnoSet (EBADF);
      return -1;
    }
  }
  sc_miscErrnoSet (EMFILE);
  return -1;
}

