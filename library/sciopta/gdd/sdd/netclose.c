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
** ID: S04147BS31                                                   **
** +Revision: 1.8 +                                                 **
** +Date: 2008/01/16 13:02:00 +                                     **
** Basic net functions: sdd_netClose                                **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <errno.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_netClose_t close;
};

int
sdd_netClose (const sdd_obj_t NEARPTR self)
{
  sc_msg_t msg, msg2;

  static const sc_msgid_t closeRpl[2] = {
    SDD_NET_CLOSE_REPLY, 0
  };
  static const sc_msgid_t errorRpl[2] = {
    SDD_ERROR, 0
  };
  int error;

  msg = sc_msgAlloc (sizeof (sdd_netClose_t), 
		     SDD_NET_CLOSE,
		     sc_msgPoolIdGet ((sc_msgptr_t) &self), 
		     SC_FATAL_IF_TMO);
  msg->close.base.error = 0;
  msg->close.base.handle = self->base.handle;
  sc_msgTx (&msg, self->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  closeRpl, SC_MSGRX_MSGID);

  /*
  ** Poll for pending SDD_ERROR (async socket)
  */
  msg2 = sc_msgRx (SC_NO_TMO,  errorRpl, SC_MSGRX_MSGID);

  error = msg->base.error;
  sc_miscErrnoSet (msg->base.error);    

  sc_msgFree (&msg);
    
  if (msg2 && msg2->base.handle == self->base.handle) {
    sc_msgFree (&msg2);
  }
  else if (msg2) {
    /*
    ** Not for the closed connection, re-queue it
    */
    sc_msgTxAlias (&msg2, SC_CURRENT_PID, 0, sc_msgSndGet(&msg2) );
  }

  return ( error ? -1 : 0 );
}

