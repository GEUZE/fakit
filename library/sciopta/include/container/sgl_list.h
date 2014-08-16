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
** ID: S03070CL83                                                   **
** +Revision: 1.14 +                                                 **
** +Date: 2008/01/16 13:02:10 +                                     **
** single linked list primitives                                    **
**********************************************************************
** General function parameters:
**
** head - Pointer to head of list.
** item - Pointer to sgl_t inside listmember
**        e.g.: 
**            struct pcb_s {
**               sgl_t readylist;
**               sgl_t sleeplist;
**            }pcb;
**        =>  Item is either &pcb->readlist or &pcb->sleeplist;
**
** To access the start of the real item use the macro SGL_ITEM:
** SGL_ITEM(type of sgl_t containing struct,item pointer,list hook)
** e.g: struct pcb_s pcb = ITEM(struct pcb_s,ptr,sleeplist)
**********************************************************************
EOC*/
#ifndef _UTIL_CONTAINER_SGL_LIST_H_
#define _UTIL_CONTAINER_SGL_LIST_H_

#ifndef __ASM_INCLUDED__
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined __KKERNEL__
#define STATIC
#elif defined __GNUC__  || defined __MWERKS__
#define STATIC static inline
#elif defined __DCC__
#define STATIC  static __inline__
#elif defined __ARMCC_VERSION
#define STATIC  static __inline
#else
#define STATIC static
#endif

#ifdef __IAR_SYSTEMS_ICC__
/* disable MISRA C Rule 45 again*/
/* The rule complains the (dbl_t NEARPTR)0 casts which we need. */
#pragma diag_suppress=Pm038
#endif

struct sgl_s {
  struct sgl_s NEARPTR next;
};
typedef struct sgl_s sgl_t;

struct sgl_head_s {
  sgl_t NEARPTR head;
  sgl_t NEARPTR tail;
};
typedef struct sgl_head_s sgl_head_t;

#define SGL_HEAD(head) sgl_head_t head ={(sgl_t NEARPTR)0,(sgl_t NEARPTR)&head}
#define SGL_HEAD_INIT(ptr) do { (ptr)->head = (sgl_t NEARPTR)(0); \
                                (ptr)->tail = (sgl_t NEARPTR)(ptr); }while(0)

/*
** Macro to access the list member
*/
#define SGL_ITEM(type,ptr,list) (type *)(((char *)ptr) - offsetof(type,list))


STATIC void sgl_tailAdd (sgl_head_t NEARPTR head, sgl_t NEARPTR item);
STATIC sgl_t NEARPTR sgl_tailDel(sgl_head_t NEARPTR head);
STATIC void sgl_headAdd (sgl_head_t NEARPTR head, sgl_t NEARPTR item);
STATIC sgl_t NEARPTR sgl_headDel (sgl_head_t NEARPTR head);
STATIC void sgl_del(sgl_head_t NEARPTR head, sgl_t NEARPTR item);
STATIC void sgl_del2(sgl_head_t NEARPTR head, sgl_t NEARPTR prev,sgl_t NEARPTR item);
STATIC sgl_t NEARPTR sgl_headGet(sgl_head_t NEARPTR head);
STATIC sgl_t NEARPTR sgl_for_all (sgl_head_t NEARPTR head, int (*func) (void NEARPTR));

#if !defined __KKERNEL__ || defined __LIBSGL_LIST__

/*
** Add item at the end of the list.
**
** E.g.:  sgl_add_tail(&xx.head,&pcb->ready);
*/
STATIC void sgl_tailAdd (sgl_head_t NEARPTR head, sgl_t NEARPTR item) 
{
  item->next = (sgl_t NEARPTR) 0;
  head->tail->next = item;
  head->tail = item;
}
/*
** Remove item from the end of the list
*/ 
STATIC sgl_t NEARPTR sgl_tailDel(sgl_head_t NEARPTR head) 
{
  sgl_t NEARPTR item = head->tail;
  sgl_t NEARPTR ptr = head->head;
  
  if (ptr == (sgl_t NEARPTR) 0) {
    return (sgl_t NEARPTR) 0;
  }
  
  while ((ptr != (sgl_t NEARPTR) 0) && (ptr->next != item)) {
    ptr = ptr->next;
  }
  if (ptr != (sgl_t NEARPTR) 0) {
    head->tail = ptr;
    ptr->next = (sgl_t NEARPTR) 0;
  }
  else {
    head->tail = (sgl_t NEARPTR) head;
    head->head = (sgl_t NEARPTR) 0;
  }
  return item;
}

/*
** Add an item after pos.
*/
#if 0 /* not used */
void sgl_add(sgl_t NEARPTRpos,sgl_t NEARPTRitem)
{
  item->prev = pos;
  item->next = pos->next;
  pos->next->prev = item;
  pos->next = item;
}
#endif
/*
** Add an item at the beginning of the list.
*/
STATIC void sgl_headAdd (sgl_head_t NEARPTR head, sgl_t NEARPTR item) 
{
  item->next = head->head;
  head->head = item;

}

/*
** Remove an item from the beginning of the list.
*/
STATIC sgl_t NEARPTR sgl_headDel (sgl_head_t NEARPTR head)
{
  sgl_t NEARPTR item = head->head;
  if ( head->head ){
    head->head = item->next;
    if (head->head == (sgl_t NEARPTR) 0) {
      head->tail = (sgl_t NEARPTR) head;
    }
  }
  return item;
}

STATIC void sgl_del(sgl_head_t NEARPTR head, sgl_t NEARPTR item)
{
  sgl_t NEARPTR x = (sgl_t NEARPTR)head;
  while( x->next != item ){
    x = x->next;
  }
  x->next = item->next;
  if ( item == head->tail ){
    head->tail = x;
  }
}

STATIC void sgl_del2(sgl_head_t NEARPTR head, sgl_t NEARPTR prev,sgl_t NEARPTR item)
{
  prev->next = item->next;
  if ( prev->next == (sgl_t NEARPTR)0 ){
    head->tail = prev;
  }
}

STATIC sgl_t NEARPTR sgl_headGet(sgl_head_t NEARPTR head)
{
  return head->head;
}
/*
** Call func for every list-member or until func returns true.
** If true, returns touched item.
*/
STATIC sgl_t NEARPTR sgl_for_all (sgl_head_t NEARPTR head, int (*func) (void NEARPTR))
{
  sgl_t NEARPTR ptr = head->head;
  while (ptr != (sgl_t NEARPTR) 0) {
    if (func ((void NEARPTR)ptr)) {
      return ptr;
    }
    ptr = ptr->next;
  }
  return (sgl_t NEARPTR) 0;
}

#ifdef __IAR_SYSTEMS_ICC__
/* enable MISRA C Rule 45 again*/
#pragma diag_default=Pm038
#endif
#ifdef __cplusplus
}
#endif
#undef STATIC
#endif /* !defined __KKERNEL__ || defined __LIBSGL_LIST__ */
#endif /* __ASM_INCLUDED__ */
#endif /* _UTIL_CONTAINER_SGL_LIST_H_ */


