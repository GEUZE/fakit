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
** ID: S03070CL66                                                   **
** +Revision: 1.49 +                                                **
** +Date: 2008/01/16 13:02:10 +                                     **
** queue primitives                                                 **
**********************************************************************
EOC*/
#include <sciopta.h>
#include <sys/errno.h>

#include <container/swqueue.h>
#include <container/dbl_list.h>

#if defined __ARMCC_VERSION
/* suppress: assignment in condition warning */
#pragma diag_suppress 1293
#endif

#if defined __HIWARE__
/* disable : CONDITION ALWAYS FALSE for DBL_HEAD_INIT */
#pragma MESSAGE DISABLE C4001
#endif

typedef struct __entry {
  swq_entry_t parent;
  dbl_t list;
  swqueue_t NEARPTR swq;
  size_t index;
} __entry_t;

struct swqueue {
  sc_msgid_t id;
  sc_poolid_t plid;
  sc_ticks_t tmo;
  __u32 start;
  __u32 entrySeq;
  __u32 get;
  __u32 window;
  __u32 ack;
  __entry_t NEARPTR cur;
  dbl_t waitlist;
};

static void
__swqDel (void NEARPTR entry)
{
  if (entry && ((swq_entry_t NEARPTR) entry)->msg) {
    sc_msgFree (&((swq_entry_t NEARPTR) entry)->msg);
  }
  if (entry && ((swq_entry_t NEARPTR) entry)->id) {
    sc_msgFree ((sc_msgptr_t) &entry);
  }
}

swqueue_t NEARPTR 
swq_new (__u32 start, sc_poolid_t plid, sc_ticks_t tmo)
{
  swqueue_t NEARPTR tmp;

  tmp =
    (swqueue_t NEARPTR) sc_msgAllocClr (sizeof (struct swqueue), CONTAINER_SWQ_HOOK, plid, tmo);
  if (tmp) {
    tmp->plid = plid;
    tmp->tmo = tmo;
    tmp->start = start;
    tmp->entrySeq = start;
    tmp->get = start;
    tmp->ack = start;
    tmp->window = 2;
    tmp->cur = NEAR_NULL;
    DBL_HEAD_INIT (&tmp->waitlist);
  }
  return tmp;
}

void
swq_destroy (swqueue_t NEARPTR NEARPTR hook)
{
  dbl_t *e;

  while ((e = dbl_tailDel (&(*hook)->waitlist))) {
    __swqDel (DBL_ITEM (__entry_t, e, list));
  }

  sc_msgFree ((sc_msgptr_t) hook);
}

swq_entry_t NEARPTR 
swq_entryInit (swq_entry_t NEARPTR e, 
	       swqueue_t NEARPTR self, 
	       char *buf, 
	       size_t len,
               sc_msg_t msg)
{
  __entry_t NEARPTR tmp = (__entry_t NEARPTR) e;

  tmp->parent.id = 0;
  tmp->parent.buf = buf;
  tmp->parent.len = len;
  tmp->parent.seq = 0;
  tmp->parent.msg = msg;
  tmp->swq = self;
  tmp->index = 0;
  return (swq_entry_t NEARPTR) tmp;
}

swq_entry_t NEARPTR 
swq_entryNew (swqueue_t NEARPTR self, char *buf, size_t len, sc_msg_t msg)
{
  __entry_t NEARPTR tmp;
  tmp =
    (__entry_t NEARPTR) sc_msgAlloc (sizeof (__entry_t), CONTAINER_SWQ_ENTRY, self->plid,
                               self->tmo);
  if (tmp) {
    tmp->parent.buf = buf;
    tmp->parent.len = len;
    tmp->parent.seq = 0;
    tmp->parent.msg = msg;
    tmp->swq = self;
    tmp->index = 0;
  }
  return (swq_entry_t NEARPTR) tmp;
}

void
swq_entryFree (swq_entry_t NEARPTR NEARPTR e)
{
  sc_msgFree ((sc_msgptr_t) e);
}

int
swq_add (swqueue_t NEARPTR hook, swq_entry_t NEARPTR entry)
{
  if (!hook) {
    return -1;
  }

  if (!entry) {
    return -1;
  }
  
  entry->seq = hook->entrySeq;
  ((__entry_t NEARPTR) entry)->index = 0;
  dbl_tailAdd (&hook->waitlist, &((__entry_t NEARPTR) entry)->list);
  PRINTF1 ("could add: %u\n", entry->len);
  hook->entrySeq += entry->len;

  return 0;
}

swq_entry_t NEARPTR 
swq_free (swqueue_t NEARPTR hook, __u32 ack)
{
  dbl_t *e;
  __entry_t NEARPTR cur = NULL;

  if (!hook) {
    return NEAR_NULL;
  }

  if ((ack -= hook->start) >= hook->window) {
    PRINTF3 ("ack outside window: (%u -= %u) >= %u\n", ack, hook->start,
             hook->window);
    return NEAR_NULL;
  }
  e = dbl_headGet (&hook->waitlist);
  if (e) {
    cur = DBL_ITEM (__entry_t, e, list);
    if (ack >= (cur->parent.seq - hook->start) + cur->parent.len) {
      PRINTF4 ("could free: %u >= (%u - %u) + %u\n", ack, cur->parent.seq,
               hook->start, cur->parent.len);
      dbl_del (&cur->list);
      if (cur == hook->cur) {
        hook->cur = (__entry_t NEARPTR)0;
      }
      hook->start += cur->parent.len;
      return (swq_entry_t NEARPTR) cur;
    }
    else {
      PRINTF4 ("could not free: %u >= (%u - %u) + %u\n", ack, cur->parent.seq,
               hook->start, cur->parent.len);
    }
  }
  else {
    PRINTF ("could not free: NULL\n");
  }
  return NEAR_NULL;
}

swq_entry_t NEARPTR 
swq_get (swqueue_t NEARPTR hook, __u32 window, __u32 mss)
{
  dbl_t *e;
  swq_entry_t NEARPTR tmp;
  __entry_t NEARPTR cur;

  if (!hook) {
    return NEAR_NULL;
  }

  PRINTF ("get\n");
  hook->window = window;
  
  /* lets look if we are still in the window */
  if (window <= hook->get - hook->start) {
    PRINTF3 ("not in window: %u <= %u - %u\n", window, hook->get,
             hook->start);
    return NEAR_NULL;
  }
  /* test if we have an entry in the hand */
  
  cur = hook->cur;
  if (!cur) {
    /* no? get the first */
    e = dbl_headGet (&hook->waitlist);
    if (e) {
      cur = hook->cur = DBL_ITEM (__entry_t, e, list);
      /* initialize this entry */
      cur->index = 0;
    }
    else {
      cur = hook->cur = NULL; 
      /* there are no entries */
      PRINTF ("no entries\n");
      return NEAR_NULL;
    }
  }
  /* if there is an entry lets look if there is data left to send */
  
  while (cur->parent.len == cur->index || !cur->parent.buf) {
    /* no? so get the next one */
    if (!cur->parent.buf) {
      hook->get += cur->parent.len;
    }
    e = cur->list.next;
    if (e != &hook->waitlist) {
      cur = DBL_ITEM (__entry_t, e, list);
      /* initialize this entry */
      hook->cur = cur;
      cur->index = 0;
    }
    else {
      /* don't touch hook->cur else we will send all from the first as new data
         :-P */
      PRINTF ("no entries\n");
      return NEAR_NULL;
    }
  }
  
  /* now adjust mss */
  if (mss > cur->parent.len - cur->index) {
    mss = cur->parent.len - cur->index;
  }
  
  /* now all work is done just create a tmp entry for the requestor */
  if (window < hook->get - hook->start + mss) {
    /* hope this will work */
    mss = window - (hook->get - hook->start);
    PRINTF4 ("adjust mss: %u = %u - (%u - %u)\n", mss, window, hook->get,
             hook->start);
  }
  
  tmp = swq_entryNew (hook, 0, 0, (sc_msg_t)0);
  tmp->buf = &cur->parent.buf[cur->index];
  tmp->seq = hook->get;
  tmp->len = (size_t)mss;
  cur->index += (size_t)mss;
  hook->get += (size_t)mss;
  return tmp;
}

swq_entry_t NEARPTR 
swq_getLast (swqueue_t NEARPTR hook)
{
  dbl_t *e;
  __entry_t NEARPTR entry = NEAR_NULL;

  if ( !hook ) {
    return NEAR_NULL;
  }

  e = dbl_tailGet (&hook->waitlist);
  if (e) {
    entry = DBL_ITEM (__entry_t, e, list);
  }
  return (swq_entry_t NEARPTR) entry;
}

void
swq_incrSeq (swqueue_t NEARPTR hook, size_t size)
{
  if ( hook ) {
    hook->entrySeq += size;
  }
}

int
swq_checkAck (swqueue_t NEARPTR hook, __u32 ack)
{
  if (hook && hook->ack - hook->start < ack - hook->start
      && ack - hook->start < hook->window) 
  {
    hook->ack = ack;
    return 1;
  } else {
    return 0;
  }
}

int
swq_lastAckEqSeq (swqueue_t NEARPTR hook, __u32 seq)
{
  if ( !hook ) {
    return 0;
  }

  return (hook->ack == seq);
}

int
swq_lastAckInRange (swqueue_t NEARPTR hook, __u32 lower, __u32 upper)
{
  if ( !hook ) {
    return -1;
  }

  return (hook->ack - hook->start >= lower - hook->start
          && hook->ack - hook->start < upper - hook->start);
}

__s32
swq_reset (swqueue_t NEARPTR hook, __u32 * seq)
{
  dbl_t *e;
  __s32 ret;

  if (!hook) {
    return -1;
  }

  ret = (*seq - hook->start) - (hook->ack - hook->start);
  *seq = hook->get = hook->ack;
 
  e = dbl_headGet (&hook->waitlist); 
  if (e) {
    hook->cur = DBL_ITEM (__entry_t, e, list);
    hook->cur->index = (size_t)(hook->get - hook->cur->parent.seq);
  }
  else {
    hook->cur = NEAR_NULL;
  }
  return ret;
}

