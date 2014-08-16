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
** +Revision: 1.41 +                                                **
** +Date: 2008/01/16 13:02:10 +                                     **
** list pimitives                                                   **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sys/errno.h>
#include <container/list.h>

static int
cmpDefault (const void NEARPTR left, const void NEARPTR right)
{
  return (__ptrsize_t) left - (__ptrsize_t) right;
}

static void
delDefault (void NEARPTR e)
{
  (void)e;
  /* nothing */
}

/* destructor */
static void
list_destroy (list_t NEARPTR NEARPTR self)
{
  if ((*self)) {
    list_entry_t NEARPTR cur = (*self)->first->next;
    while (cur != (*self)->first) {
      (*self)->del (cur->entry);
      cur->entry = NEAR_NULL;   // be sure we don't kill it twice !!
      cur = cur->next;
      sc_msgFree ((sc_msgptr_t) & cur->prev);
    }
    sc_msgFree ((sc_msgptr_t) & (*self)->first);
  }
  /* call the supers destroy */
  sc_msgFree ((sc_msgptr_t) self);
}

/* manipulators */
static list_entry_t NEARPTR
_cntEntryNew (list_t NEARPTR self, void NEARPTR entry)
{
/* var */
  list_entry_t NEARPTR tmp;
/* begin */
  sc_miscErrnoSet (0);
  tmp =
    (list_entry_t NEARPTR) sc_msgAllocClr (sizeof (list_entry_t),
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
list_add (list_t NEARPTR self, void NEARPTR entry)
{
/* var */
  list_entry_t NEARPTR cur;
  void NEARPTR ret;
/* begin */
  sc_miscErrnoSet (0);
  ret = NEAR_NULL;
  if (self && self->first) {
    cur = self->first;
    while (cur->next != self->first
           && self->cmp (cur->next->entry, entry) < 0) {
      cur = cur->next;
    }
    if (cur->next == self->first || self->cmp (cur->next->entry, entry) != 0) {
      list_entry_t NEARPTR newE = _cntEntryNew (self, entry);
      if (newE) {
        ret = entry;
        newE->prev = cur->prev;
        newE->next = cur;
        cur->prev->next = newE;
        cur->prev = newE;
        ++self->noOfItems;
      }
    }
    else {
      sc_miscErrnoSet (EEXIST);
    }
  }
  else {
    sc_miscErrnoSet (EFAULT);
  }
  return ret;
}

static void NEARPTR
list_rm (list_t NEARPTR self, const void NEARPTR entry)
{
/* var */
  list_entry_t NEARPTR cur;
  list_entry_t NEARPTR old;
  void NEARPTR ret;
/* begin */
  sc_miscErrnoSet (0);
  ret = NEAR_NULL;
  if (self && self->first) {
    cur = self->first;
    if (cur->next != cur) {
      /* there are elems */
      while (cur->next != self->first
             && self->cmp (cur->next->entry, entry) != 0) {
        cur = cur->next;
      }
      /* found place */
      if (cur->next != self->first
          && self->cmp (cur->next->entry, entry) == 0) {
        /* found one */
        old = cur->next;
        cur->next = old->next;
        old->next->prev = cur;
        ret = old->entry;
        sc_msgFree ((sc_msgptr_t) & old);
        --self->noOfItems;
        return ret;
      }
    }
    sc_miscErrnoSet (ENOENT);
  }
  else {
    sc_miscErrnoSet (EFAULT);
  }
  return ret;
}

/* read only  */
static void NEARPTR
list_getCmp (const list_t NEARPTR self,
             const void NEARPTR entry, 
	     int (*cmp) (const void NEARPTR, const void NEARPTR))
{
/* var */
  list_entry_t NEARPTR cur;
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
list_get (const list_t NEARPTR self, const void NEARPTR entry)
{
  return list_getCmp (self, entry, self->cmp);
}

static void NEARPTR
list_getFirst (const list_t NEARPTR self)
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
list_getNext (const list_t NEARPTR self, const void NEARPTR reference)
{
/* var */
  list_entry_t NEARPTR cur;
  void NEARPTR ret;
/* begin */
  sc_miscErrnoSet (0);
  ret = NEAR_NULL;
  if (self && self->first) {
    cur = self->first;
    while (cur->next != self->first
           && self->cmp (cur->next->entry, reference) != 0) {
      cur = cur->next;
    }
    ret = cur->next->next->entry;
  }
  else {
    sc_miscErrnoSet (EFAULT);
  }
  return ret;
}

static void NEARPTR
list_getPrev (const list_t NEARPTR self, const void NEARPTR reference)
{
/* var */
  list_entry_t NEARPTR cur;
  void NEARPTR ret;
/* begin */
  sc_miscErrnoSet (0);
  ret = NEAR_NULL;
  if (self && self->first) {
    cur = self->first;
    if (cur->next != cur) {
      /* there are elems */
      while (cur->next != self->first
             && self->cmp (cur->next->entry, reference) != 0) {
        cur = cur->next;
      }
      ret = cur->prev->entry;
    }
  }
  else {
    sc_miscErrnoSet (EFAULT);
  }
  return ret;
}

static void NEARPTR
list_getLast (const list_t NEARPTR self)
{
  if (self && self->first) {
    sc_miscErrnoSet (0);
    return self->first->prev->entry;
  }
  sc_miscErrnoSet (EFAULT);
  return NEAR_NULL;
}

static int
list_noOfItems (const list_t NEARPTR self)
{
  if (self) {
    sc_miscErrnoSet (0);
    return self->noOfItems;
  }
  sc_miscErrnoSet (EFAULT);
  return -1;
}

static const list_ops_t listOps = {
  0,
  list_destroy,
  list_add,
  list_rm,
  list_get,
  list_getFirst,
  list_getNext,
  list_getPrev,
  list_getLast,
  list_noOfItems,
  list_getCmp
};

/* constructor */
static int
list_init (list_t NEARPTR hook,
           list_hdlCmp cmp_handler,
           list_hdlDel del_handler, sc_poolid_t plid, sc_ticks_t tmo)
{
  hook->noOfItems = 0;
  if (cmp_handler) {
    hook->cmp = cmp_handler;
  }
  else {
    hook->cmp = cmpDefault;
  }
  if (del_handler) {
    hook->del = del_handler;
  }
  else {
    hook->del = delDefault;
  }
  hook->plid = plid;
  hook->tmo = tmo;
  hook->first = _cntEntryNew (hook, NEAR_NULL);
  if (!hook->first) {
    sc_miscErrnoSet (ENOMEM);
    return 0;
  }
  hook->first->next = hook->first;
  hook->first->prev = hook->first;
//->  hook->ops = (list_ops_t *) & listOps;
  hook->ops = & listOps;

  return 1;
}

list_t NEARPTR
list_new (list_hdlCmp cmp_handler,
          list_hdlDel del_handler, sc_poolid_t plid, sc_ticks_t tmo)
{
  list_t NEARPTR tmp;
  tmp =
    (list_t NEARPTR) sc_msgAlloc (sizeof (list_t), CONTAINER_LIST_HOOK, plid,
                                  tmo);
  sc_miscErrnoSet (0);
  if (tmp) {
    if (!list_init (tmp, cmp_handler, del_handler, plid, tmo)) {
      sc_msgFree ((sc_msgptr_t) & tmp);
    }
  }
  else {
    sc_miscErrnoSet (ENOMEM);
  }
  return tmp;
}
