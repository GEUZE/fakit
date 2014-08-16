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
** +Revision: 1.8 +                                                **
** +Date: 2008/01/16 13:02:10 +                                     **
** array pimitives                                                  **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <string.h> // memset
#include <sys/errno.h>
#include <container/array.h>

#ifdef NC30
#define memset _n_memset
#endif

typedef struct __array_s {
  array_t parent;
  void NEARPTR array[1];
} __array_t;

static int
cmpDefault (const void NEARPTR left, const void NEARPTR right)
{
  return (__ptrsize_t) left - (__ptrsize_t) right;
}

static void
delDefault (void NEARPTR e)
{
  /* nothing */
  (void)e;
}
static void
sort (__array_t NEARPTR self)
{
  __uint i, j;
  void NEARPTR entry;

  for (i = 1; i < self->parent.size && self->array[i]; i++) {
    entry = self->array[i];
    j = i;
    while (j > 0 && self->parent.cmp (self->array[j - 1], entry) > 0) {
      self->array[j] = self->array[j - 1];
      j--;
    }
    self->array[j] = entry;
  }
}

static void
stuff (__array_t NEARPTR self, __uint start)
{
  __uint i;

  for (i = start + 1; i < self->parent.size && self->array[i]; i++) {
    self->array[i - 1] = self->array[i];
  }
}

/* destructor */
static void
array_destroy (array_t NEARPTR NEARPTR self)
{
  __uint i;

  if ((*self)) {
    for (i = 0;
         i < (*self)->size && ((__array_t NEARPTR) (*self))->array[i]; i++) {
      (*self)->del (((__array_t NEARPTR) (*self))->array[i]);
      ((__array_t NEARPTR) (*self))->array[i] = 0;
    }
  }
  sc_msgFree ((sc_msgptr_t) self);
}

static void NEARPTR
array_add (array_t NEARPTR self, void NEARPTR entry)
{
  __uint i;

  sc_miscErrnoSet (0);

  if (self) {
    for (i = 0; i < self->size && ((__array_t NEARPTR) self)->array[i]
         && self->cmp (((__array_t NEARPTR) self)->array[i], entry) != 0;
         i++);
    if (i < self->size && !((__array_t NEARPTR) self)->array[i]) {
      /* found a free place */
      ((__array_t NEARPTR) self)->array[i] = entry;
      sort ((__array_t NEARPTR) self);
      return entry;
    }
    else {
      sc_miscErrnoSet (EEXIST);
      return NEAR_NULL;
    }
  }
  else {
    sc_miscErrnoSet (EFAULT);
    return NEAR_NULL;
  }
}

static void NEARPTR
array_rm (array_t NEARPTR self, const void NEARPTR entry)
{
  void *ret;
  __uint i;

  sc_miscErrnoSet (0);

  if (self) {
    for (i = 0; i < self->size && ((__array_t NEARPTR) self)->array[i]
         && self->cmp (((__array_t NEARPTR) self)->array[i], entry) != 0;
         i++);
    if (i < self->size) {
      ret = ((__array_t NEARPTR) self)->array[i];
      ((__array_t NEARPTR) self)->array[i] = 0;
      stuff ((__array_t NEARPTR) self, i);
      sort ((__array_t NEARPTR) self);
      return ret;
    }
    else {
      sc_miscErrnoSet (ENOENT);
      return NEAR_NULL;
    }
  }
  else {
    sc_miscErrnoSet (EFAULT);
    return NEAR_NULL;
  }
}

/* read only  */
static void NEARPTR
array_getCmp (const array_t NEARPTR self,
              const void NEARPTR entry, 
	      int (*cmp) (const void NEARPTR, const void NEARPTR))
{
  __uint i;

  sc_miscErrnoSet (0);

  if (self) {
    for (i = 0; i < self->size && ((__array_t NEARPTR) self)->array[i]
         && cmp (((__array_t NEARPTR) self)->array[i], entry) != 0; i++);
    if (i < self->size && ((__array_t NEARPTR) self)->array[i]) {
      return ((__array_t NEARPTR) self)->array[i];
    }
    else {
      sc_miscErrnoSet (ENOENT);
      return NEAR_NULL;
    }
  }
  else {
    sc_miscErrnoSet (EFAULT);
    return NEAR_NULL;
  }
}

static void NEARPTR
array_get (const array_t NEARPTR self, const void NEARPTR entry)
{
  return array_getCmp (self, entry, self->cmp);
}

static void NEARPTR
array_getFirst (const array_t NEARPTR self)
{
  if (self) {
    if (!((__array_t NEARPTR) self)->array[0]) {
      sc_miscErrnoSet (ENOENT);
    }
    return ((__array_t NEARPTR) self)->array[0];
  }
  else {
    sc_miscErrnoSet (EFAULT);
    return NEAR_NULL;
  }
}

static void NEARPTR
array_getNext (const array_t NEARPTR self, const void NEARPTR reference)
{
  __uint i;

  if (self) {
    for (i = 0; i < self->size && ((__array_t NEARPTR) self)->array[i]
         && self->cmp (((__array_t NEARPTR) self)->array[i], reference) != 0;
         i++);
    if (i + 1 < self->size && ((__array_t NEARPTR) self)->array[i + 1]) {
      return ((__array_t NEARPTR) self)->array[i + 1];
    }
    else {
      sc_miscErrnoSet (ENOENT);
      return NEAR_NULL;
    }
  }
  else {
    sc_miscErrnoSet (EFAULT);
    return NEAR_NULL;
  }
}

static void NEARPTR
array_getPrev (const array_t NEARPTR self, const void NEARPTR reference)
{
  __uint i;

  if (self) {
    for (i = 0; i < self->size && ((__array_t NEARPTR) self)->array[i]
         && self->cmp (((__array_t NEARPTR) self)->array[i], reference) != 0;
         i++);
    if (i >= 1 && ((__array_t NEARPTR) self)->array[i - 1]) {
      return ((__array_t NEARPTR) self)->array[i - 1];
    }
    else {
      sc_miscErrnoSet (ENOENT);
      return NEAR_NULL;
    }
  }
  else {
    sc_miscErrnoSet (EFAULT);
    return NEAR_NULL;
  }
}

static void NEARPTR
array_getLast (const array_t NEARPTR self)
{
  __uint i;

  if (self) {
    for (i = 1; i < self->size && ((__array_t NEARPTR) self)->array[i]; i++);
    if (((__array_t NEARPTR) self)->array[i - 1]) {
      sc_miscErrnoSet (ENOENT);
    }
    return ((__array_t NEARPTR) self)->array[i - 1];
  }
  else {
    sc_miscErrnoSet (EFAULT);
    return NEAR_NULL;
  }
}

static int
array_noOfItems (const array_t NEARPTR self)
{
  if (self) {
    sc_miscErrnoSet (0);
    return -1;
  }
  sc_miscErrnoSet (EFAULT);
  return -1;
}

static const array_ops_t arrayOps = {
  0,
  array_destroy,
  array_add,
  array_rm,
  array_get,
  array_getFirst,
  array_getNext,
  array_getPrev,
  array_getLast,
  array_noOfItems,
  array_getCmp
};

/* constructor */
void
array_init (array_t NEARPTR self, array_hdlCmp cmp_handler,
            array_hdlDel del_handler, size_t size)
{
  if (cmp_handler) {
    self->cmp = cmp_handler;
  }
  else {
    self->cmp = cmpDefault;
  }
  if (del_handler) {
    self->del = del_handler;
  }
  else {
    self->del = delDefault;
  }
  self->size = size;
  memset (((__array_t NEARPTR) self)->array,0,(int)(size*sizeof(void NEARPTR)));
//->  self->ops = (array_ops_t *) & arrayOps;
  self->ops = &arrayOps;
}

array_t NEARPTR
array_new (array_hdlCmp cmp_handler, array_hdlDel del_handler, size_t size,
           sc_poolid_t plid, sc_ticks_t tmo)
{
  array_t NEARPTR tmp;
  tmp =
    (array_t NEARPTR) sc_msgAlloc (sizeof (array_t) +
                                   (sc_bufsize_t)size * sizeof (void NEARPTR),
                                   CONTAINER_ARRAY_HOOK, plid, tmo);
  sc_miscErrnoSet (0);
  if (tmp) {
    array_init (tmp, cmp_handler, del_handler, size);
  }
  else {
    sc_miscErrnoSet (ENOMEM);
  }
  return tmp;
}
