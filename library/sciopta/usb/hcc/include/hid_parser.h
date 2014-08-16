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
#ifndef _HID_PARSER_H_
#define _HID_PARSER_H_

#include <hcc/hcc_types.h>
#include "hid_parser_def.h"
#ifdef __cplusplus
extern "C" {
#endif

#if 0 /* moved to hid_parser_pref.h */
/* Number of report items a report may have. */
#define MAX_REPORT_ITEMS  16
#endif

#define INVALID_OFFSET  0xff

/* Report data item. */
typedef struct {
  hcc_s32 logical_min;  /*min value device can return */
  hcc_s32 logical_max;  /*max value device can return */
  hcc_s32 physical_min; /*min vale read can report */
  hcc_s32 physical_max; /*max value read can report */
  hcc_u32 resolution;   /* ratio between logical and physical value */
  hcc_u16 usage_page;
  hcc_u16 usage_min;
  hcc_u16 usage_max;    /* only used for arrays */
  hcc_u8 ary_size;
  hcc_u8 offset;        /* offset of first byte in buffer */  
  hcc_u8 shift;         /* offset of firs bit in LSB */
  hcc_u8 size;          /* size in bits */
  hcc_u8 sign;          /* 1 if item is signed. */
} rp_item_t;


/* Possible types for a report. */
typedef enum {
  rpt_invalid,
  rpt_in,
  rpt_out,
  rpt_feature
} report_type_t;

typedef struct {
  report_type_t type;
  int id;
  int bit_ofs;
  rp_item_t items[MAX_REPORT_ITEMS];
  /* The content of this buffer is transferred over the USB. Any report read/write modifyes 
     this buffer. */
  hcc_u8 *buffer;
} report_t;

extern int find_collection(hcc_u8 *data, hcc_u16 length, 
                           hcc_u16 upage, hcc_u16 usage, 
                           hcc_u8 **start, hcc_u16 *size);
extern int get_reports(report_t *reports, hcc_u16 reports_size, hcc_u8 *data, hcc_u16 length);

extern int read_item(rp_item_t *ri, hcc_u8 *buffer, hcc_u8 index, hcc_s32 *value);
extern int write_item(rp_item_t *ri, hcc_u8 *buffer, hcc_u8 index, hcc_s32 value);


#ifdef __cplusplus
}
#endif

#endif
