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
** ID: S03070BS1                                                    **
** +Revision: 1.20 +                                                 **
** +Date: 2008/01/16 13:02:10 +                                     **
** doubly linked list primitives                                    **
**********************************************************************
**
** General function parameters:
**
** head - Pointer to head of list.
** item - Pointer to dbl_t inside listmember
**        e.g.: 
**            struct pcb_s {
**               dbl_t readylist;
**               dbl_t sleeplist;
**            }pcb;
**        =>  Item is either &pcb->readlist or &pcb->sleeplist;
**
** To access the start of the real item use the macro DBL_ITEM:
** DBL_ITEM(type of dbl_t containing struct,item pointer,list hook)
** e.g: struct pcb_s pcb = DBL_ITEM(struct pcb_s,ptr,sleeplist)
**
************************************************
EOC*/

#ifndef _UTIL_CONTAINER_DBL_LIST_H_
#define _UTIL_CONTAINER_DBL_LIST_H_
#ifndef __ASM_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif
#include <sciopta.h>
#include <stddef.h>

#ifdef __IAR_SYSTEMS_ICC__
/* disable MISRA C Rule 45 again*/
/* The rule complains the (dbl_t NEARPTR)0 casts which we need. */
#pragma diag_suppress=Pm038
#endif

struct dbl_s {
  struct dbl_s NEARPTR prev;
  struct dbl_s NEARPTR next;
};
typedef struct dbl_s dbl_t;

#define DBL_HEAD(head) dbl_t head = { &head,&head }
#define DBL_HEAD_INIT(ptr) do { (ptr)->next = (ptr);\
                                (ptr)->prev = (ptr);\
                           }while(0)

/*
** Macro to access the list member
*/
#define DBL_ITEM(type,ptr,list) ((type NEARPTR)(((char *)ptr) - offsetof(type,list)))

#define HEAD next
#define TAIL prev

STATIC_INLINE void dbl_tailAdd (dbl_t NEARPTR head, dbl_t NEARPTR item);
STATIC_INLINE dbl_t NEARPTR dbl_tailDel (dbl_t NEARPTR head);
STATIC_INLINE void dbl_add (dbl_t NEARPTR pos, dbl_t NEARPTR item);
STATIC_INLINE void dbl_headAdd (dbl_t NEARPTR head, dbl_t NEARPTR item);
STATIC_INLINE void dbl_del (dbl_t NEARPTR pos);
STATIC_INLINE dbl_t NEARPTR dbl_headDel (dbl_t NEARPTR head);
STATIC_INLINE dbl_t NEARPTR dbl_headGet (dbl_t NEARPTR head);
STATIC_INLINE dbl_t NEARPTR dbl_tailGet (dbl_t NEARPTR head);
STATIC_INLINE int dbl_listEmpty (dbl_t NEARPTR head);
STATIC_INLINE dbl_t NEARPTR dbl_forAll (dbl_t NEARPTR head, int (*func) (void NEARPTR));
STATIC_INLINE dbl_t NEARPTR dbl_get (dbl_t NEARPTR head, int (*func) (const void NEARPTR,const void NEARPTR),const void NEARPTR ref);


#define DBL_GET(name,type,listmember)        \
type NEARPTR name(dbl_t NEARPTR head,type *ref)    \
{                                            \
  dbl_t NEARPTR ptr = head->HEAD;            \
  while (ptr != head) {                      \
    type NEARPTR s = DBL_ITEM(type,ptr,listmember); \
    if (s == ref) {                          \
      return s;                              \
    }                                        \
    ptr = ptr->next;                         \
  }                                          \
  return (type NEARPTR) 0;                         \
}
  
#if !defined __KKERNEL__ || defined __LIBDBL_LIST__
/*
** Add item at the end of the list.
**
** E.g.:  dbl_add_tail(&xx.head,&pcb->ready);
*/
STATIC_INLINE void dbl_tailAdd (dbl_t NEARPTR head, dbl_t NEARPTR item)
{
  item->next = head;
  item->prev = head->TAIL;
  
  head->TAIL->next = item;
  head->TAIL = item;
}
/*
** Remove item from the end of the list
*/ 
STATIC_INLINE dbl_t NEARPTR dbl_tailDel (dbl_t NEARPTR head)
{
  dbl_t NEARPTR item = head->TAIL;
  if (item == head) {
    return (dbl_t NEARPTR ) 0;
  }
  
  head->TAIL = item->prev;
  head->TAIL->next = head;
  
  item->next = 0;
  item->prev = 0;
  return item;
}

/*
** Add an item after pos.
*/
STATIC_INLINE void dbl_add (dbl_t NEARPTR pos, dbl_t NEARPTR item)
{
  item->prev = pos;
  item->next = pos->next;
  pos->next->prev = item;
  pos->next = item;
}

/*
** Add an item at the beginning of the list.
*/
 
STATIC_INLINE void dbl_headAdd (dbl_t NEARPTR head, dbl_t NEARPTR item)
{
  item->prev = head;
  item->next = head->HEAD;
  
  head->HEAD->prev = item;
  head->HEAD = item;
}

/*
** Remove an item from the  list.
*/
STATIC_INLINE void dbl_del (dbl_t NEARPTR pos)
{
  pos->prev->next = pos->next;
  pos->next->prev = pos->prev;
}

/*
** Remove an item from the beginning of the list.
*/
STATIC_INLINE dbl_t NEARPTR dbl_headDel (dbl_t NEARPTR head)
{
  dbl_t NEARPTR item = head->HEAD;
  if (item == head) {
    return (dbl_t NEARPTR ) 0;
  }
  head->HEAD = item->next;
  head->HEAD->prev = head;
  item->next = 0;
  item->prev = 0;
  return item;
}

/*
** Get an item from the beginning of the list.
*/
STATIC_INLINE dbl_t NEARPTR dbl_headGet (dbl_t NEARPTR head)
{
  dbl_t NEARPTR item = head->HEAD;
  if (item == head) {
    return (dbl_t NEARPTR ) 0;
  }
  else {
    return item;
  }
}

/*
** Get an item from the end of the list.
*/
STATIC_INLINE dbl_t NEARPTR dbl_tailGet (dbl_t NEARPTR head)
{
  dbl_t NEARPTR item = head->TAIL;
  if (item == head) {
    return (dbl_t NEARPTR ) 0;
  }
  else {
    return item;
  }
}

/*
** Call func for every list-member or until func returns true.
** If true, returns touched item.
*/

STATIC_INLINE dbl_t NEARPTR dbl_forAll (dbl_t NEARPTR head, int (*func) (void NEARPTR))
{
  dbl_t NEARPTR ptr = head->HEAD;
  while (ptr != head) {
    if (func (ptr)) {
      return ptr;
    }
    ptr = ptr->next;
  }
  return (dbl_t NEARPTR ) 0;
}

STATIC_INLINE dbl_t NEARPTR 
dbl_get (dbl_t NEARPTR head, 
         int (*func) (const void NEARPTR,const void NEARPTR),
	 const void NEARPTR ref)
{
  dbl_t NEARPTR ptr = head->HEAD;
  while (ptr != head) {
    if (func ((const void NEARPTR)ptr,(const void NEARPTR)ref)) {
      return ptr;
    }
    ptr = ptr->next;
  }
  return (dbl_t NEARPTR ) 0;
}

STATIC_INLINE int dbl_listEmpty (dbl_t NEARPTR head) 
{
  return (head->next == head);
}
#ifdef __IAR_SYSTEMS_ICC__
/* enable MISRA C Rule 45 again*/
#pragma diag_default=Pm038
#endif

#ifdef __cplusplus
}
#endif

#endif /* !defined __KKERNEL__ || defined __LIBDBL_LIST__ */
#endif /* __ASM_INCLUDED__ */
#endif
