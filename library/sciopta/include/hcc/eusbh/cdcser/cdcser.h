/****************************************************************************
 *
 *            Copyright (c) 2003-2008 by HCC Embedded
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
#ifndef _CDCSER_H
#define _CDCSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <hcc/hcc_types.h>
#include <hcc/eusbh/os.h>
#include "cdcser_def.h"

#if OS_TASK_POLL_MODE
  #define CDCSER_MUTEX_COUNT	0
#else
  #define CDCSER_MUTEX_COUNT	CDCSER_MAX_DEVICE
#endif
#define CDCSER_EVENT_COUNT	0

#define CDCSER_BITS_5		5
#define CDCSER_BITS_6		6
#define CDCSER_BITS_7		7
#define CDCSER_BITS_8		8
#define CDCSER_BITS_16		16

#define CDCSER_PARITY_NONE	0
#define CDCSER_PARITY_ODD	1
#define CDCSER_PARITY_EVEN	2
#define CDCSER_PARITY_MARK	3
#define CDCSER_PARITY_SPACE	4

#define CDCSER_STOP_1		0
#define CDCSER_STOP_1_5		1
#define CDCSER_STOP_2		2


/* moved from cdcser.c */
typedef struct {
  hcc_u8 buf[CDCSER_RXBUF_SIZE];
  hcc_u32 pos;
  hcc_u32 size;
  int rc;
} t_rxbuf;

typedef struct {
  hcc_u8 cdcser_device;

/***** usb host access *****/
  t_usbh_transfer rx_uht;
  t_usbh_transfer tx_uht;

/***** buffering *****/
  hcc_u8 rxbuf_r;
  hcc_u8 rxbuf_w;
  t_rxbuf rxbuf[CDCSER_RXBUF_COUNT];

/***** Serial settings *****/
  hcc_u32 br;
  hcc_u8 b;
  hcc_u8 p;
  hcc_u8 s;
  hcc_u8 control;

/***** Interface *****/
  void *ep_hdl[3];
  void *ifc_hdl;
  hcc_u8 ifc_ndx;

#if OS_TASK_POLL_MODE==0
/***** mutex *****/
  OS_MUTEX_TYPE *rx_mutex;
#endif
  
/***** events *****/
  sc_pid_t rx_ev;
  sc_msg_t tx_ev;
/****** Sciopta ************/
  int device;
  sc_pid_t driver;

} t_cdcser;				       /* CDC serial device variables */
/************************/

int cdcser_init (void);
int cdcser_set_events (hcc_u8 dev, OS_EVENT_BIT_TYPE *rx_ev, OS_EVENT_BIT_TYPE *tx_ev);
int cdcser_send (hcc_u8 dev, hcc_u8 *buf, hcc_u32 length);
int cdcser_get_send_state (hcc_u8 dev);
int cdcser_receive (hcc_u8 dev, hcc_u8 *buf, hcc_u32 max_length, hcc_u32 *rlength);
hcc_u32 cdcser_rx_chars (hcc_u8 dev);
int cdcser_get_receive_state (hcc_u8 dev);
hcc_u8 cdcser_present (hcc_u8 dev);

int cdcser_get_line_coding (hcc_u8 dev, hcc_u32 *br, hcc_u8 *b, hcc_u8 *p, hcc_u8 *s);
int cdcser_set_line_coding (hcc_u8 dev, hcc_u32 br, hcc_u8 b, hcc_u8 p, hcc_u8 s);
int cdcser_set_control_line_state (hcc_u8 dev, hcc_u8 rts, hcc_u8 dtr);

int cdcser_suspend (hcc_u8 dev);
int cdcser_resume (hcc_u8 dev);




#ifdef __cplusplus
}
#endif


#endif


