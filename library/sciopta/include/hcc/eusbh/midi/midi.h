/****************************************************************************
 *
 *            Copyright (c) 2003-2009 by HCC Embedded
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
#ifndef _MIDI_H
#define _MIDI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <hcc/hcc_types.h>
#include <hcc/eusbh/os.h>
#include "midi_def.h"

#define MIDI_MUTEX_COUNT		1
#define MIDI_EVENT_COUNT		0

/* return codes */
enum {
  MIDI_SUCCESS=0,
  MIDI_NO_EVENT,
  MIDI_FULL,
  MIDI_ERR_RESOURCE,
  MIDI_ERR_NOT_PRESENT,
  MIDI_ERROR
};

typedef struct {
  hcc_u8 cn;		/* cable number */
  hcc_u8 cin;		/* code index number */
  hcc_u8 event[3];	/* midi event */
} PACKED t_midi_event;

int midi_init (void);
int midi_add_event (int dev, t_midi_event *event);
int midi_send (int dev);
int midi_get_event (int dev, t_midi_event *event);
int midi_state (int dev);
int midi_get_info (int dev, hcc_u8 *n_in, hcc_u8 *n_out);

/*+++SCIOPTA+++*/
typedef struct {
  sc_msgid_t id;
  sc_time_t time;
  int device;
  int n;
  t_midi_event event[0];
} midi_event_msg_t;

#ifdef __cplusplus
}
#endif

#endif /* _MIDI_H */



