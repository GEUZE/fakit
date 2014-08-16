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
** ID: S03070CL64                                                   **
** +Revision: 1.3 +                                                **
** +Date: 2008/01/16 13:02:10 +                                     **
** fifo pimitives                                                   **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sys/errno.h>
#include <container/fifo.h>

static void
delDefault (void NEARPTR e)
{
  (void)e;
  /* nothing */
}

/* destructor */
static void
fifo_destroy (fifo_t NEARPTR NEARPTR self)
{
  fifo_entry_t NEARPTR old;
  
  if ((*self)) {
    fifo_entry_t NEARPTR cur = (*self)->first->next;
    while (cur != (*self)->first) {
      (*self)->del (cur->entry);
      cur->entry = NEAR_NULL;   // be sure we don't kill it twice !!
      old = cur;
      cur = cur->next;
      sc_msgFree ((sc_msgptr_t) & old);
    }
    sc_msgFree ((sc_msgptr_t) & (*self)->first);
  }
  /* call the supers destroy */
  sc_msgFree ((sc_msgptr_t) self);
}

/* manipulators */
static fifo_entry_t NEARPTR
_cntEntryNew (fifo_t NEARPTR self, void NEARPTR entry)
{
/* var */
  fifo_entry_t NEARPTR tmp;
/* begin */
  sc_miscErrnoSet (0);
  tmp =
    (fifo_entry_t NEARPTR) sc_msgAllocClr (sizeof (fifo_entry_t),
                                           CONTAINER_LIST_ENTRY,
                                           self->plid, self->tmo);

  if (tmp) {
    tmp->entry = entry;
  }
  else {
    sc_miscErrnoSet (ENOMEM);
  }
  return tmp;
}

static void NEARPTR
fifo_add (fifo_t NEARPTR self, void NEARPTR entry)
{
/* var */
  fifo_entry_t NEARPTR newE;
  void NEARPTR ret;
/* begin */
  sc_miscErrnoSet (0);
  ret = NEAR_NULL;
  if (self && self->first && self->last) {
    ++self->noOfItems;
    newE = _cntEntryNew (self, entry);
    newE->next = self->first;
    self->last->next = newE; 
    self->last = newE;
    ret = entry;
  }
  else {
    sc_miscErrnoSet (EFAULT);
  }
  return ret;
}

static void NEARPTR
fifo_rm (fifo_t NEARPTR self, const void NEARPTR entry)
{
/* var */
  fifo_entry_t NEARPTR old;
  void NEARPTR ret;
/* begin */
  (void)entry;

  sc_miscErrnoSet (0);
  ret = NEAR_NULL;
  if (self && self->first && self->last) {
    if (self->last == self->first) {
      sc_miscErrnoSet (ENOENT);
      return NEAR_NULL;
    }
    else {
      --self->noOfItems;
      old = self->first->next;
      self->first->next = old->next;
      if (self->last == old) {
	self->last = self->first;
      }
      ret = old->entry;
      sc_msgFree ((sc_msgptr_t) &old);
    }
  }
  else {
    sc_miscErrnoSet (EFAULT);
  }
  return ret;
}

/* read only  */
static void NEARPTR
fifo_getCmp (const fifo_t NEARPTR self,
             const void NEARPTR entry, 
	     int (*cmp) (const void NEARPTR, const void NEARPTR))
{
/* var */
  fifo_entry_t NEARPTR cur;
/* begin */
  sc_miscErrnoSet (0);
  if (self && self->first) {
    cur = self->first;
    if (cur->next != cur) {
      /* there are elems */
      while (cur->next != self->first && cmp (cur->next->entry, entry) != 0) {
        cur = cur->next;
      }
      /* found place */
      if (cur->next != self->first && cmp (cur->next->entry, entry) == 0) {
        return cur->next->entry;
      }
    }
    sc_miscErrnoSet (ENOENT);
  }
  else {
    sc_miscErrnoSet (EFAULT);
  }
  return NEAR_NULL;
}

static void NEARPTR
fifo_get (const fifo_t NEARPTR self, const void NEARPTR entry)
{
  (void)self;
  (void)entry;
  return NEAR_NULL;
}

static void NEARPTR
fifo_getFirst (const fifo_t NEARPTR self)
{
/* begin */
  if (self && self->first) {
    sc_miscErrnoSet (0);
    return self->first->next->entry;
  }
  sc_miscErrnoSet (EFAULT);
  return NEAR_NULL;
}

static void NEARPTR
fifo_getNext (const fifo_t NEARPTR self, const void NEARPTR reference)
{
  (void) self;
  (void) reference;
  sc_miscErrnoSet (ENOENT);
  return NEAR_NULL;
}

static void NEARPTR
fifo_getPrev (const fifo_t NEARPTR self, const void NEARPTR reference)
{
  (void) self;
  (void) reference;

  sc_miscErrnoSet (ENOENT);
  return NEAR_NULL;
}

static void NEARPTR
fifo_getLast (const fifo_t NEARPTR self)
{
  if (self && self->first) {
    sc_miscErrnoSet (0);
    return self->last->entry;
  }
  sc_miscErrnoSet (EFAULT);
  return NEAR_NULL;
}

static int
fifo_noOfItems (const fifo_t NEARPTR self)
{
  if (self) {
    sc_miscErrnoSet (0);
    return self->noOfItems;
  }
  sc_miscErrnoSet (EFAULT);
  return -1;
}

static const fifo_ops_t fifoOps = {
  0,
  fifo_destroy,
  fifo_add,
  fifo_rm,
  fifo_get,
  fifo_getFirst,
  fifo_getNext,
  fifo_getPrev,
  fifo_getLast,
  fifo_noOfItems,
  fifo_getCmp
};

/* constructor */
static int
fifo_init (fifo_t NEARPTR hook,
           fifo_hdlDel del_handler, sc_poolid_t plid, sc_ticks_t tmo)
{
  hook->noOfItems = 0;
  if (del_handler) {
    hook->del = del_handler;
  }
  else {
    hook->del = delDefault;
  }
  hook->plid = plid;
  hook->tmo = tmo;
  hook->last = hook->first = _cntEntryNew (hook, NEAR_NULL);
  if (!hook->first) {
    sc_miscErrnoSet (ENOMEM);
    return 0;
  }
  hook->first->next = hook->first;
//->  hook->ops = (fifo_ops_t *) & fifoOps;
  hook->ops =  & fifoOps;
  return 1;
}

fifo_t NEARPTR
fifo_new (fifo_hdlDel del_handler, sc_poolid_t plid, sc_ticks_t tmo)
{
  fifo_t NEARPTR tmp;
  tmp =
    (fifo_t NEARPTR) sc_msgAlloc (sizeof (fifo_t), CONTAINER_LIST_HOOK, plid,
                                  tmo);
  sc_miscErrnoSet (0);
  if (tmp) {
    if (!fifo_init (tmp, del_handler, plid, tmo)) {
      sc_msgFree ((sc_msgptr_t) & tmp);
    }
  }
  else {
    sc_miscErrnoSet (ENOMEM);
  }
  return tmp;
}
