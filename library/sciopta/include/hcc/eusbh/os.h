/****************************************************************************
 *
 *            Copyright (c) 2007 by HCC Embedded
 *
 * This software is copyrighted by and is the sole property of
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1133
 * Vaci ut 110
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/
#ifndef _OS_H
#define _OS_H

#ifdef __cplusplus
extern "C" {
#endif


#include <hcc/hcc_types.h>
#include <sdd/sdd.msg>

#define OS_ISR_FN(fn)		void fn (void)
#define OS_TASK_FN(fn)		void fn (void)

#define OS_MUTEX_TYPE		sdd_obj_t
#define OS_EVENT_BIT_TYPE       int
#define OS_INTERRUPT_ENABLE		1
#define OS_TASK_POLL_MODE		0

enum {
  OS_SUCCESS,
  OS_ERR_MUTEX,
  OS_ERR_EVENT,
  OS_ERR
};

int os_init (void);
int os_kill (void);
void os_idle (void);

int os_isr_init (hcc_u32,void(*ih)(void));
int os_isr_enable (hcc_u32);
int os_isr_disable (hcc_u32);
int os_isr_kill (hcc_u32);

int os_mutex_create(OS_MUTEX_TYPE **pmutex);
int os_mutex_get(OS_MUTEX_TYPE *pmutex);
int os_mutex_put(OS_MUTEX_TYPE *pmutex);

void os_requestDoorbell();

#define ehci_hw_unlock_buffer os_unlock_buffer
#define ehci_hw_lock_buffer os_lock_buffer

int os_buffer_init (void);
void *os_lock_buffer (void *ptr, hcc_u32 *size);
void os_unlock_buffer (void *ptr, hcc_u32 size);

#ifdef __cplusplus
}
#endif

#endif

