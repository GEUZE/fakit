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
**                  (c) 2007 Sciopta Systems GmbH                   **
**                                                                  **
**********************************************************************
** ID: S04062CL1 
** +Revision: 1.17 +
** +Date: 2008/02/14 14:15:47 +
** Semaphore emulation for sciopta 
**********************************************************************
EOC*/

#include <sciopta.h>
#include <dev/sem.h>
#include <dev/sem.msg>
#include <sys/errno.h>
#include <container/list.h>
#include <container/fifo.h>
#include <ossys/libsc.h>
#include <string.h>

#if defined __ARMCC_VERSION
/* suppress: assignment in condition warning */
#pragma diag_suppress 1293
#endif

/*
** Private message structures 
*/
typedef struct sem_handle_s {
  int value;
  fifo_t NEARPTR waiters;
  sc_tmoid_t tmoid;
} sem_handle_t;

typedef struct sem_tmo_s {
  sc_msgid_t id;
  sem_handle_t NEARPTR handle;
  sc_msg_t request;
} sem_tmo_t;

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_obj_t sem;
  sem_set_t set;
  sem_incr_t incr;
  sem_decr_t decr;
  sem_decr_wtmo_t decr_wtmo;
  sem_read_t read;
  sem_tmo_t  tmo;
};


static int
entryPidCmp (const void NEARPTR left, const void NEARPTR right)
{
  return sc_msgSndGet ((sc_msgptr_t) &left) -
    sc_msgSndGet ((sc_msgptr_t) &right);
}

static int
entryNameOrderCmp (const void NEARPTR left, const void NEARPTR right)
{
  return 
    strncmp ((const char *) ((sdd_obj_t NEARPTR) left)->name,
	     (const char *) ((sdd_obj_t NEARPTR) right)->name, SC_NAME_MAX);
}

static void
entryDel (void NEARPTR e)
{
  if (e)
    sc_msgFree ((sc_msgptr_t) &e);
}

SC_PROCESS (SCP_sem)
{
  sc_msg_t msg;
  sc_msg_t dup;
  sc_msg_t tmoMsg;
  sc_pid_t to;
  list_t NEARPTR names;
  sem_handle_t NEARPTR handle;
  sc_bufsize_t msize;
  sc_bufsize_t dsize;
  __s32 semCount;

  names =
    list_new (entryNameOrderCmp, 
	      entryDel, 
	      SC_DEFAULT_POOL, 
	      SC_FATAL_IF_TMO);

  semCount = 0;
  for (;;) {
    msg = sc_msgRx (SC_ENDLESS_TMO, SC_MSGRX_ALL, SC_MSGRX_MSGID);
    to = sc_msgSndGet (&msg);
    switch (msg->id) {

    case SDD_MAN_ADD:
      /* create a new semaphore by copy the sdd_obj_t and create a semaphore 
       * handle */
      if ( !msg->sem.name[0] ){
	strcpy(msg->sem.name,"Xsem");
	sc_miscInt2Str(semCount,&msg->sem.name[4],8);
	msg->sem.name[12] = 0;
	++semCount;
      }
      if (!names->ops->add (names, msg)) {
	msg->base.error = EEXIST;
	goto txReply;
      }
      msize = sc_msgSizeGet (&msg);

      dup = sc_msgAlloc (msize,
			 msg->id, 
			 sc_msgPoolIdGet (&msg),
			 SC_NO_TMO);
      if (!dup) {
        msg->base.error = ENOMEM;
        goto txReply;
      }
      handle =
	(sem_handle_t NEARPTR) sc_msgAlloc (sizeof (sem_handle_t), 
					    SDD_OBJ,
					    SC_DEFAULT_POOL, 
					    SC_NO_TMO);
      if (!handle) {
        sc_msgFree ((sc_msgptr_t) &dup);
        msg->base.error = ENOMEM;
        goto txReply;
      }
      handle->waiters = fifo_new (entryDel, SC_DEFAULT_POOL, SC_NO_TMO);

      if (!handle->waiters) {
        sc_msgFree ((sc_msgptr_t) &handle);
        sc_msgFree (&dup);
        msg->base.error = ENOMEM;
        goto txReply;
      }

      msg->base.handle = handle;
      memcpy (dup, msg, msize);
      msg = dup;
      break;

    case SDD_MAN_RM:
      /* XXX: Must check if no waiters, else error */
      if (!(dup = (sc_msg_t)names->ops->rm (names, msg))) {
	msg->base.error = ENOENT;
	goto txReply;
      }
      handle = (sem_handle_t NEARPTR)dup->base.handle;
      handle->waiters->ops->destroy (&handle->waiters);
      sc_msgFree ((sc_msgptr_t) &handle);
      sc_msgFree (&dup);
      break;

    case SDD_MAN_GET:
      if (!(dup = (sc_msg_t)names->ops->get (names, msg))) {
	msg->base.error = ENOENT;
	goto txReply;
      }
      msize = sc_msgSizeGet (&msg);
      dsize = sc_msgSizeGet (&dup);
      if ( dsize < msize ) {
	msize = dsize;
      }
      memcpy (msg, dup, msize);
      msg->id = SDD_MAN_GET; 
      break;

    case SDD_MAN_GET_FIRST:
      if (!(dup = (sc_msg_t)names->ops->getFirst (names))) {
	msg->base.error = ENOENT;
	goto txReply;
      }
      msize = sc_msgSizeGet (&msg);
      dsize = sc_msgSizeGet (&dup);
      if ( dsize < msize ) {
	msize = dsize;
      }
      memcpy (msg, dup, msize);
      msg->id = SDD_MAN_GET_FIRST; 
      break;

    case SDD_MAN_GET_NEXT:
      if (!(dup = (sc_msg_t)names->ops->getNext (names, msg))) {
	msg->base.error = ENOENT;
	goto txReply;
      }
      msize = sc_msgSizeGet (&msg);
      dsize = sc_msgSizeGet (&dup);
      if ( dsize < msize ) {
	msize = dsize;
      }
      memcpy (msg, dup, msize);
      msg->id = SDD_MAN_GET_NEXT; 
      break;

    case SEM_SET:
      handle = (sem_handle_t NEARPTR)msg->base.handle;
      if (!handle) {
	msg->base.error = EFAULT;
	goto txReply;
      }

      if ( handle->waiters->ops->noOfItems(handle->waiters) > 0 ){       
	msg->base.error = EFAULT;
	goto txReply;
      }
	
      handle->value = msg->set.value;
      break;

    case SEM_INCR:
      handle = (sem_handle_t NEARPTR)msg->base.handle;
      if (!handle) {
	msg->base.error = EFAULT;
	goto txReply;
      }
      if (msg->incr.value < 0) {
	msg->base.error = EINVAL;
	goto txReply;
      }
      handle->value += msg->incr.value;
      while (handle->waiters->ops->noOfItems(handle->waiters) > 0 &&
	     handle->value >= 0) 
      {
	dup = (sc_msg_t)handle->waiters->ops->getFirst(handle->waiters);
	handle->value -= dup->decr.value;

	if ( handle->value >= 0 ){
	  handle->waiters->ops->rm (handle->waiters, dup);
	  if ( handle->tmoid ){
	    tmoMsg = sc_tmoRm(&handle->tmoid);
	    sc_msgFree(&tmoMsg);
	  }
	  /* wakeup waiter */
	  ++dup->id;
	  sc_msgTx (&dup, sc_msgSndGet (&dup), 0);
	} else {
	  dup->decr.value = 0;
	}	  
      }
      break;
    case SEM_TMO:
      handle = msg->tmo.handle;
      handle->tmoid = 0;

      dup = handle->waiters->ops->rm (handle->waiters, msg->tmo.request);

      sc_msgFree(&msg);

      msg = dup;
      msg->base.error = ETIME;
      /*
      ** Timeout expired, restore semaphore value
      */
      handle->value += msg->decr.valueSave;
      ++msg->id;
      sc_msgTx (&msg, to, 0);     
      break;
    case SEM_DECR_W_TMO:
    case SEM_DECR:
      /* Check parameters */
      handle = (sem_handle_t NEARPTR)msg->base.handle;
      if (!handle) {
	msg->base.error = EFAULT;
	goto txReply;
      }

      if (msg->decr.value < 0) {
	msg->base.error = EINVAL;
	goto txReply;
      }

      if ( msg->id == SEM_DECR_W_TMO && msg->decr_wtmo.tmo > SC_TMO_MAX ){
	msg->base.error = EINVAL;
	goto txReply;
      }

      dup = handle->waiters->ops->getCmp (handle->waiters, msg, entryPidCmp );
      if ( dup ){
	msg->base.error = EINVAL;
	goto txReply;
      }

      msg->decr.valueSave = msg->decr.value; /* save for restoring */
      if (handle->value >= 0) {
	handle->value -= msg->decr.value; 
	msg->decr.value = 0; // Flag: Don't decrement again
      }
      if ( handle->value >= 0 ){
	goto txReply;
      }

      handle->tmoid = 0;
      if ( msg->id == SEM_DECR_W_TMO ){
	if ( msg->decr_wtmo.tmo == 0 ){
	  /* Just polling, restore semaphore */
	  msg->base.error = ETIME;	  
	  handle->value += msg->decr.valueSave;
	  goto txReply;
	}

	tmoMsg = sc_msgAlloc(sizeof(sem_tmo_t),
			     SEM_TMO,
			     SC_DEFAULT_POOL,
			     SC_FATAL_IF_TMO);
	tmoMsg->tmo.handle = handle;
	tmoMsg->tmo.request = msg;
	handle->tmoid = sc_tmoAdd(msg->decr_wtmo.tmo, &tmoMsg);
      }
      if (!(handle->waiters->ops->add (handle->waiters, msg))) {
	/* nested lock, this pid did already lock */
	/* this situation should be handled as an system error */
	sc_miscError (SEM_ERR_BASE + SC_EBADBAD, sc_msgSndGet (&msg));
      }
      continue;
      
    case SEM_READ:
      handle = (sem_handle_t NEARPTR)msg->base.handle;
      if (!handle) {
	msg->base.error = EFAULT;
      }
      msg->read.value = handle->value;
      break;

    default:
      msg->base.error = SC_ENOTSUPP;
      break;
    }
    
  txReply:
    if ( msg ){
      ++msg->id;
      sc_msgTx (&msg, to, 0);
    }

  }
}

sdd_obj_t NEARPTR 
sem_new (const char *name, int value, const char *semProc, sc_poolid_t plid)
{
  sc_msg_t msg;
  sc_pid_t pid;

  static const sc_msgid_t sel[2] = { SDD_MAN_ADD_REPLY, 0 };

  if ((pid = sc_procIdGet (semProc, 0)) == SC_ILLEGAL_PID) {
    sc_miscErrnoSet (SC_ENOPROC);
    return NEAR_NULL;
  }
  
  msg = sc_msgAllocClr (sizeof (sdd_obj_t), SDD_MAN_ADD, plid, SC_FATAL_IF_TMO);
  msg->sem.sender = msg->sem.receiver = msg->sem.controller = pid;
//  msg->sem.name[0] = 0;  allocClr does this

  if (name && name[0]) {
    strncpy (msg->sem.name, name, SC_NAME_MAX);
  }

  sc_msgTx (&msg, pid, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  sel, SC_MSGRX_MSGID);

  if (msg->base.error) {
    sc_miscErrnoSet (msg->base.error);
    sc_msgFree (&msg);
    return NEAR_NULL;
  }

  sem_set (&msg->sem, value);

  return &msg->sem;
}

sdd_obj_t NEARPTR 
sem_get (const char *name, const char *semProc, sc_poolid_t plid)
{
  sc_msg_t msg;
  sc_pid_t pid;

  static const sc_msgid_t sel[2] = { SDD_MAN_GET_REPLY, 0 };

  if ((pid = sc_procIdGet (semProc, 0)) == SC_ILLEGAL_PID) {
    sc_miscErrnoSet (SC_ENOPROC);
    return NEAR_NULL;
  }

  msg = sc_msgAllocClr (sizeof (sdd_obj_t), SDD_MAN_GET, plid, SC_FATAL_IF_TMO);
  msg->sem.sender = msg->sem.receiver = msg->sem.controller = pid;
  strncpy (msg->sem.name, name, SC_NAME_MAX);
  sc_msgTx (&msg, msg->sem.controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  sel, SC_MSGRX_MSGID);

  if (msg->base.error) {
    sc_miscErrnoSet (msg->base.error);
    sc_msgFree (&msg);
  }

  return (sdd_obj_t NEARPTR) msg;
}

int
sem_rm (sdd_obj_t NEARPTR NEARPTR self)
{
  sc_msg_t msg;

  static const sc_msgid_t sel[2] = { SDD_MAN_RM_REPLY, 0 };

  (*self)->base.id = SDD_MAN_RM;
  sc_msgTx ((sc_msgptr_t) self, (*self)->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  sel, SC_MSGRX_MSGID);

  if (msg->base.error) {
    sc_miscErrnoSet (msg->base.error);
    sc_msgFree (&msg);
    return -1;
  }

  sc_msgFree (&msg);
  return 0;
}

int
sem_set (const sdd_obj_t NEARPTR self, int value)
{
  sc_msg_t msg;

  static const sc_msgid_t sel[2] = { SEM_SET_REPLY, 0 };

  if (self == NULL) {
    sc_miscError (SEM_ERR_BASE + EFAULT, __LINE__);
  }

  msg = sc_msgAllocClr (sizeof (sem_set_t), 
			SEM_SET,
			sc_msgPoolIdGet ((sc_msgptr_t) &self),
			SC_FATAL_IF_TMO);

  msg->set.base.handle = self->base.handle;
  msg->set.value = value;
  sc_msgTx (&msg, self->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  sel, SC_MSGRX_MSGID);

  if (msg->base.error) {
    sc_miscErrnoSet (msg->base.error);
    sc_msgFree (&msg);
    return -1;
  }

  sc_msgFree (&msg);
  return 0;
}

int
sem_incr (const sdd_obj_t NEARPTR self, int value)
{
  sc_msg_t msg;

  static const sc_msgid_t sel[2] = { SEM_INCR_REPLY, 0 };

  if (self == NULL) {
    sc_miscError (SEM_ERR_BASE + EFAULT, __LINE__);
  }

  msg = sc_msgAllocClr (sizeof (sem_incr_t), 
			SEM_INCR,
			sc_msgPoolIdGet ((sc_msgptr_t) &self),
			SC_FATAL_IF_TMO);

  msg->incr.base.handle = self->base.handle;
  msg->incr.value = value;
  sc_msgTx (&msg, self->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  sel, SC_MSGRX_MSGID);

  if (msg->base.error) {
    sc_miscErrnoSet (msg->base.error);
    sc_msgFree (&msg);
    return -1;
  }

  sc_msgFree (&msg);
  return 0;
}

int
sem_decr (const sdd_obj_t NEARPTR self, int value)
{
  sc_msg_t msg;

  static const sc_msgid_t sel[2] = { SEM_DECR_REPLY, 0 };

  if (self == NULL) {
    sc_miscError (SEM_ERR_BASE + EFAULT, __LINE__);
  }

  msg = sc_msgAllocClr (sizeof (sem_decr_t), 
			SEM_DECR,
			sc_msgPoolIdGet ((sc_msgptr_t) &self),
			SC_FATAL_IF_TMO);

  msg->decr.base.handle = self->base.handle;
  msg->decr.value = value;
  sc_msgTx (&msg, self->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  sel, SC_MSGRX_MSGID);

  if (msg->base.error) {
    sc_miscErrnoSet (msg->base.error);
    sc_msgFree (&msg);
    return -1;
  }

  sc_msgFree (&msg);
  return 0;
}

int
sem_decr_w_tmo (const sdd_obj_t NEARPTR self, int value, sc_ticks_t tmo)
{
  sc_msg_t msg;

  static const sc_msgid_t sel[2] = { SEM_DECR_W_TMO_REPLY, 0 };

  if (self == NULL) {
    sc_miscError (SEM_ERR_BASE + EFAULT, __LINE__);
  }

  msg = sc_msgAllocClr (sizeof (sem_decr_wtmo_t), 
			SEM_DECR_W_TMO,
			sc_msgPoolIdGet ((sc_msgptr_t) &self),
			SC_FATAL_IF_TMO);

  msg->decr_wtmo.base.handle = self->base.handle;
  msg->decr_wtmo.value = value;
  msg->decr_wtmo.tmo = tmo;
  sc_msgTx (&msg, self->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  sel, SC_MSGRX_MSGID);

  if (msg->base.error) {
    sc_miscErrnoSet (msg->base.error);
    sc_msgFree (&msg);
    return -1;
  }

  sc_msgFree (&msg);
  return 0;
}

int
sem_read (const sdd_obj_t NEARPTR self, int *value)
{
  sc_msg_t msg;

  static const sc_msgid_t sel[2] = { SEM_READ_REPLY, 0 };

  if (self == NULL) {
    sc_miscError (SEM_ERR_BASE + EFAULT, __LINE__);
  }

  msg = sc_msgAllocClr (sizeof (sem_read_t), 
			SEM_READ,
			sc_msgPoolIdGet ((sc_msgptr_t) &self),
			SC_FATAL_IF_TMO);

  msg->read.base.handle = self->base.handle;
  sc_msgTx (&msg, self->controller, 0);
  msg = sc_msgRx (SC_ENDLESS_TMO,  sel, SC_MSGRX_MSGID);

  if (msg->base.error) {
    sc_miscErrnoSet (msg->base.error);
    sc_msgFree (&msg);
    return -1;
  }

  *value = msg->read.value;
  sc_msgFree (&msg);
  return 0;
}

