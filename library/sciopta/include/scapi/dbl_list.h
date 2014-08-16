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
** +Revision: 1.3 +                                                 **
** +Date: 2006/03/09 08:19:35 +                                     **
** doubly linked list primitives (scapi version)                    **
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

#define STATIC __inline

#include <stddef.h>

struct dbl_s {
  struct dbl_s * prev;
  struct dbl_s * next;
};
typedef struct dbl_s dbl_t;

#define DBL_HEAD(head) dbl_t head = { &head,&head }
#define DBL_HEAD_INIT(ptr) do { (ptr)->next = (ptr);\
                                (ptr)->prev = (ptr);\
                           }while(0)

/*
** Macro to access the list member
*/
#define DBL_ITEM(type,ptr,list) ((type *)(((char *)ptr) - offsetof(type,list)))

#define HEAD next
#define TAIL prev

STATIC void dbl_tailAdd (dbl_t * head, dbl_t * item);
STATIC dbl_t * dbl_tailDel (dbl_t * head);
STATIC void dbl_add (dbl_t * pos, dbl_t * item);
STATIC void dbl_headAdd (dbl_t * head, dbl_t * item);
STATIC void dbl_del (dbl_t * pos);
STATIC dbl_t * dbl_headDel (dbl_t * head);
STATIC dbl_t * dbl_headGet (dbl_t * head);
STATIC dbl_t * dbl_tailGet (dbl_t * head);
STATIC int dbl_listEmpty (dbl_t * head);
STATIC dbl_t * dbl_forAll (dbl_t * head, int (*func) (void *));
STATIC dbl_t * dbl_get (dbl_t * head, int (*func) (void *,void *),const void * ref);

#define DBL_GET(name,type,listmember)        \
type * name(dbl_t * head,type *ref)    \
{                                            \
  dbl_t * ptr = head->HEAD;            \
  while (ptr != head) {                      \
    type * s = DBL_ITEM(type,ptr,listmember); \
    if (s == ref) {                          \
      return s;                              \
    }                                        \
    ptr = ptr->next;                         \
  }                                          \
  return (type *) 0;                         \
}
  
/*
** Add item at the end of the list.
**
** E.g.:  dbl_add_tail(&xx.head,&pcb->ready);
*/
STATIC void dbl_tailAdd (dbl_t * head, dbl_t * item)
{
  item->next = head;
  item->prev = head->TAIL;
  
  head->TAIL->next = item;
  head->TAIL = item;
}
/*
** Remove item from the end of the list
*/ 
STATIC dbl_t * dbl_tailDel (dbl_t * head)
{
  dbl_t * item = head->TAIL;
  if (item == head) {
    return (dbl_t * ) 0;
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
STATIC void dbl_add (dbl_t * pos, dbl_t * item)
{
  item->prev = pos;
  item->next = pos->next;
  pos->next->prev = item;
  pos->next = item;
}

/*
** Add an item at the beginning of the list.
*/
 
STATIC void dbl_headAdd (dbl_t * head, dbl_t * item)
{
  item->prev = head;
  item->next = head->HEAD;
  
  head->HEAD->prev = item;
  head->HEAD = item;
}

/*
** Remove an item from the  list.
*/
STATIC void dbl_del (dbl_t * pos)
{
  pos->prev->next = pos->next;
  pos->next->prev = pos->prev;
}

/*
** Remove an item from the beginning of the list.
*/
STATIC dbl_t * dbl_headDel (dbl_t * head)
{
  dbl_t * item = head->HEAD;
  if (item == head) {
    return (dbl_t * ) 0;
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
STATIC dbl_t * dbl_headGet (dbl_t * head)
{
  dbl_t * item = head->HEAD;
  if (item == head) {
    return (dbl_t * ) 0;
  }
  else {
    return item;
  }
}

/*
** Get an item from the end of the list.
*/
STATIC dbl_t * dbl_tailGet (dbl_t * head)
{
  dbl_t * item = head->TAIL;
  if (item == head) {
    return (dbl_t * ) 0;
  }
  else {
    return item;
  }
}

/*
** Call func for every list-member or until func returns true.
** If true, returns touched item.
*/

STATIC dbl_t * dbl_forAll (dbl_t * head, int (*func) (void *))
{
  dbl_t * ptr = head->HEAD;
  while (ptr != head) {
    if (func (ptr)) {
      return ptr;
    }
    ptr = ptr->next;
  }
  return (dbl_t * ) 0;
}

STATIC dbl_t * dbl_get (dbl_t * head, int (*func) (const void *,const void *),const void * ref)
{
  dbl_t * ptr = head->HEAD;
  while (ptr != head) {
    if (func (ptr,ref)) {
      return ptr;
    }
    ptr = ptr->next;
  }
  return (dbl_t * ) 0;
}

STATIC int dbl_listEmpty (dbl_t * head) 
{
  return (head->next == head);
}
#undef STATIC
#endif
