/***************************************************************************
 *
 *            Copyright (c) 2003-2007 by HCC Embedded
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
 * Vaci Ut 110
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/

/****************************************************************************
* Because there is no invalidatation possibility just for a memory
* area, we allocate a buffer (rx_buffer) and make it uncached. All
* USB (or other) transfers will first lock a buffer from the uncached
* region and unlock it when there is no more need for it.
* This is more efficient (and quicker) than to invalidate L1DCache
* and L2Cache every time a buffer needs to be accessed by a DMA module
* (USB host/device).
****************************************************************************/
#include "sconf.h"
#include <sciopta.h>
#include <hcc/hcc_types.h>
#include <hcc/eusbh/os.h>
#if CPU == ARM
#include "memcopy.h"
#endif

#define RX_BUF_ALIGN	16	/* align buffer */
#define RX_BUF_SIZE	(16*1024)
#define RX_SEG_SHIFT	12	/* shift -> 1<<12 = 4096 */

#define RX_SEG_CNT		(RX_BUF_SIZE>>RX_SEG_SHIFT)

#if RX_SEG_CNT>4
#define RX_SEG_MAX_CNT	(RX_SEG_CNT/4)
#elif RX_SEG_CNT>2
#define RX_SEG_MAX_CNT	(RX_SEG_CNT/2)
#else
#define RX_SEG_MAX_CNT	(RX_SEG_CNT)
#endif

hcc_u8 _rx_buffer[RX_BUF_SIZE+RX_BUF_ALIGN-1] = {0};
static hcc_u8 *rx_buffer;	/* temporary receive buffer */
static void *rx_buffer_ptr[RX_SEG_CNT];

static OS_MUTEX_TYPE *mutex_buf;


/*
** Try to allocate required amount of segments.
**
** Input: size - size of the buffer
** Output: pos - start position
**         cnt - number of segments
** Return: size of buffer available
*/
static hcc_u32 _os_get_buffer (unsigned int *pos, unsigned int *cnt, hcc_u32 size)
{
  unsigned int i,pa,ca;

  *cnt = 0;
  pa = (unsigned int)-1;
  ca = 0;
  for (i = 0;i<RX_SEG_CNT;i++)
  {
    if (rx_buffer_ptr[i] == NULL)
    {
      if (pa == (unsigned int)-1) pa=i;
      ++ca;
      if ((ca<<RX_SEG_SHIFT) >= size || ca == RX_SEG_MAX_CNT) break;
    }
    else
    {
      if (ca > *cnt)
      {
        *pos = pa;
        *cnt = ca;
      }
      pa = (unsigned int)-1;
      ca = 0;
    }
  }
  if (ca > *cnt)
  {
    *pos = pa;
    *cnt = ca;
  }

  return ((*cnt)<<RX_SEG_SHIFT)>size?size:((*cnt)<<RX_SEG_SHIFT);
}


/*
** Checks if buffer fits properly in the cache, if not get a proper buffer.
**
** Input: ptr - buffer pointer
** Input/Output: size - size of the buffer / size available
** Return: new pointer
*/
void *os_lock_buffer (void *ptr, hcc_u32 *size)
{
  void *rc=ptr;
  unsigned int p = 0,c;

  if (os_mutex_get(mutex_buf)==OS_SUCCESS)
  {
   // kprintf(0,"Alloc:%d",*size);

    *size=_os_get_buffer(&p,&c,*size);
    if (*size)      
    {    
      rc=rx_buffer+(p<<RX_SEG_SHIFT);			/* get new pointer */
      while (c--) rx_buffer_ptr[p++]=ptr;		/* mark segments as used */
     // kprintf(0,"  Alloc:%d %0x8\n",*size,rc);
    }
    else rc=NULL;
    (void)os_mutex_put(mutex_buf);
  }
  return rc;
}


/*
** Release the buffer (if used)
**
** Input: ptr - buffer pointer
**        size - size
*/
void os_unlock_buffer (void *ptr, hcc_u32 size)
{
  if ( ptr!=NULL ) {
    unsigned int p;
    void *optr;
    p =(unsigned int)(((unsigned long)((hcc_u8 *)ptr-rx_buffer))>>RX_SEG_SHIFT);
    optr = rx_buffer_ptr[p];
    if (os_mutex_get(mutex_buf) == OS_SUCCESS) {
      while (rx_buffer_ptr[p] == optr && p<RX_SEG_CNT) {
	rx_buffer_ptr[p++]=NULL;
      }
      (void)os_mutex_put(mutex_buf);
      if ( size ){
#if CPU == ARM
	if ( size < 16 ){
	  _memcpy(optr,ptr,size);
	} else {
	  switch( (__u32)optr & 3 ){
	  case 0:
	    MEMCOPY_L2L_BY4(optr,ptr,size);
	    break;
	  case 1:
	  case 3:
	    _memcpy(optr,ptr,size);
	    break;
	  case 2:
	    MEMCOPY_L2S_BY4(optr,ptr,size);
	    break;
	  }
	}
#else
	_memcpy(optr,ptr,size);
#endif
      }
    }
  }
}


/*
** Initialize cache buffer handling system
*/
int os_buffer_init (void)
{
  rx_buffer = (hcc_u8 *)(((hcc_u32)_rx_buffer+(RX_BUF_ALIGN-1))&~(RX_BUF_ALIGN-1));
  _memset(rx_buffer_ptr,0,sizeof(void *)*RX_SEG_CNT);
  if (os_mutex_create(&mutex_buf)!=OS_SUCCESS) return 1;
  return OS_SUCCESS;
}

