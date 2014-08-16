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
** ID: S04147BS8                                                    **
** +Revision: 1.53 +                                                 **
** +Date: 2008/01/16 13:02:00 +                                     **
** SCP_manager process                                              **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <string.h>
#include <errno.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#include <container/list.h>
#include <logd/logd.h>

#if defined __ARMCC_VERSION
/* suppress: assignment in condition warning */
#pragma diag_suppress 1293
#endif

#ifndef LOGD_LEVEL_MAX
#define LOGD_LEVEL_MAX LOGD_SEVERE
#endif

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_manNotify_t notify;
  sdd_manNotifyTmo_t notifyTmo;
  sdd_obj_t object;
  sc_procPathGetMsgReply_t nameGet;
};

typedef struct __pidEntry_s {
  sc_msgid_t id;
  int ref;
  sc_pid_t pid;
} __pidEntry_t;

static int
entryNameOrderCmp (const void NEARPTR left, const void NEARPTR right)
{
  return strncmp ((const char *)((sdd_name_t NEARPTR) left)->string,
		  (const char *)((sdd_name_t NEARPTR) right)->string, 
		  SC_NAME_MAX);
}

static void
entryDel (void NEARPTR e)
{
  if (e)
    sc_msgFree ((sc_msgptr_t) &e);
}

static void
manager_notfiy (list_t *delays, const char *name)
{
  sc_msg_t old;
  sc_msg_t cur;
  sc_msg_t tmo;

  cur = (sc_msg_t)delays->ops->getFirst (delays);
  while (cur) {
    old = cur;
    cur = (sc_msg_t)delays->ops->getNext (delays, cur);
    if (strncmp (name, old->object.name, SC_NAME_MAX) == 0) {
      delays->ops->rm (delays, old);
      if (old->notify.tmoid != NULL) {
        tmo = sc_tmoRm (&old->notify.tmoid);
	sc_msgFree (&tmo);
      }
      old->notify.name.base.error = 0;
      old->id++;
      sc_msgTx (&old, sc_msgSndGet (&old), 0);
    }
  }
}

SC_PROCESS (SCP_manager)
{
  sc_msg_t msg;
  sc_pid_t to;
  logd_t NEARPTR logd;
  list_t NEARPTR names;
  int s1, s2;
  sc_msg_t e;
  sc_poolid_t pool = SC_DEFAULT_POOL;
  sc_ticks_t timeout = SC_FATAL_IF_TMO;

  list_t NEARPTR delays;
  sc_msg_t tmp;
  
  logd = logd_new ("/SCP_logd", LOGD_LEVEL_MAX, "manager", SC_DEFAULT_POOL,
		   SC_FATAL_IF_TMO);
  msg = sc_procPathGet (SC_CURRENT_PID,0);
  logd_printf (logd, LOGD_INFO, "manager %s start\n",msg->nameGet.path);
  sc_msgFree(&msg);
  
  names = list_new (entryNameOrderCmp, entryDel, pool, timeout);
  delays = list_new (NULL, entryDel, pool, SC_NO_TMO);

  for (;;) {
    msg = sc_msgRx (SC_ENDLESS_TMO, SC_MSGRX_ALL, SC_MSGRX_MSGID);
    to = sc_msgSndGet (&msg);
    switch (msg->id) {
      
    case SDD_ERROR:
      /* ignore */
      logd_printf (logd, LOGD_INFO, "rev'd SDD_ERROR !\n");
      sc_msgFree(&msg);
      continue;

    case SDD_MAN_ADD:
      {
	if (names->ops->add (names, msg)) {

	  /* send all notifies */
          manager_notfiy (delays, msg->object.name); 

	  /* alloc reply */
	  msg = sc_msgAllocClr (sizeof (sdd_obj_t), SDD_MAN_ADD, pool,
				timeout);
	}
	else {
	  /* this means this name allready exist. Names musst be unique in
	     given namespace (namespace is the name of the manager).
	   */
	  msg->base.error = EEXIST;
	  /* set the wrong entry to 0 */
	  msg->object.name[0] = 0;
	}
      }
      break;

    case SDD_MAN_RM:
      if ((e = (sc_msg_t)names->ops->rm (names, msg))) {

	/* send all notifies */
        manager_notfiy (delays, msg->object.name); 

	/* prepare reply */
	sc_msgFree (&msg);
	msg = e;
	msg->id = SDD_MAN_RM;
      }
      else {
	msg->base.error = ENOENT;
      }
      break;
      
    case SDD_MAN_GET:
      if ((e = (sc_msg_t)names->ops->get (names, msg))) {
	s1 = sc_msgSizeGet (&e);
	s2 = sc_msgSizeGet (&msg); 
	if (s1 < s2) {
	  msg->base.error = EFAULT;
	}
	else {
	  memcpy (msg, e, s2);
	  msg->base.error = 0;
	}
	msg->id = SDD_MAN_GET;
      }
      else {
	msg->base.error = ENOENT;
      }
      break;

    case SDD_MAN_GET_FIRST:
      if ((e = (sc_msg_t)names->ops->getFirst (names))) {	
	s1 = sc_msgSizeGet (&e);
	s2 = sc_msgSizeGet (&msg); 
	if (s1 < s2) {
	  msg->base.error = EFAULT;
	}
	else {
	  memcpy (msg, e, s2);
	  msg->base.error = 0;
	}
	msg->id = SDD_MAN_GET_FIRST;
      }
      else {
	msg->base.error = ENOENT;
      }
      break;
      
    case SDD_MAN_GET_NEXT:
      if ((e = (sc_msg_t)names->ops->getNext (names, msg))) {
	s1 = sc_msgSizeGet (&e);
	s2 = sc_msgSizeGet (&msg); 
	if (s1 < s2) {
	  msg->base.error = EFAULT;
	}
	else {
	  memcpy (msg, e, s2);
	  msg->base.error = 0;
	}
	msg->id = SDD_MAN_GET_NEXT;
      }
      else {
	msg->base.error = ENOENT;
      }
      break;

    case SDD_MAN_NOTIFY_RM:
    case SDD_MAN_NOTIFY_ADD:
      /* check if the name is allready there */
      if ((msg->id == SDD_MAN_NOTIFY_ADD && names->ops->get (names, msg)) ||
	  (msg->id == SDD_MAN_NOTIFY_RM && !names->ops->get (names, msg))) {
	msg->base.error = 0;
      }
      /* if not try to add to the delay list */
      else if (!delays->ops->add (delays, msg)) {
	msg->base.error = ENOSPC;
      }
      /* tmo handling */
      else {
	if (msg->notify.tmo != SC_ENDLESS_TMO) {
	  /* set error to timedout and clear it if it is not the case :) */
	  msg->base.error = ETIMEDOUT;

	  /* start tmo */
          tmp = sc_msgAlloc (sizeof (sdd_manNotifyTmo_t), SDD_MAN_NOTIFY_TMO,
	                     pool, 0);
	  if (tmp) {
	    tmp->notifyTmo.delay = msg;
	    msg->notify.tmoid = sc_tmoAdd (msg->notify.tmo, &tmp);
	    msg = NULL;
	  }
	  else {
	    delays->ops->rm (delays, msg);
	    msg->base.error = ENOMEM;
	  }
        }
        else {
	  msg->notify.tmoid = NULL;
	  msg = NULL;
        }
      }
      break;

    /* timeout of notfify add */ 
    case SDD_MAN_NOTIFY_TMO:
      /* remove the delay from the delays list */
      tmp = (sc_msg_t)delays->ops->rm (delays, msg->notifyTmo.delay); 
      sc_msgFree (&msg);
      ++tmp->id;
      sc_msgTx (&tmp, sc_msgSndGet (&tmp), 0);
      break;

    case SDD_OBJ_DUP:
      msg->base.error = 0;
      break;

    case SDD_OBJ_RELEASE:
      msg->base.error = 0;
      break;

    default:
      msg->base.error = SC_ENOTSUPP;
      break;
    }

    if (msg) {
      ++msg->id;
      sc_msgTx (&msg, to, 0);
    }
  }
}

