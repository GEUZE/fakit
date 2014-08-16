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
#include <hcc/eusbh/hid/hid_parser.h>
#include <string.h>

/* Meximun depth of global state stack */
#define MAX_GSTACK_DEPTH 3


/*****************************************************************************
 ************************* Macro definitions *********************************
 *****************************************************************************/
/* Access filed of the first byte of a report descriptor item. */
#define RPDITEM_TAG(val)  (((hcc_u8)((val)&0xf0)>>4))
#define RPDITEM_TYPE(val) ((rpdi_type_t)(((val)&0x0C)>>2))
#define RPDITEM_SIZE(val) ((hcc_u8)(((val)&0x03)>>0))

/* Main item flags (mask the data filed of rp_item_data_t with these). */
#define RPDITEM_IS_CONST(i)         ((i)->data & (1<<0))
#define RPDITEM_IS_DATA(i)          (!RPDITEM_IS_CONST(i))
#define RPDITEM_IS_VARIABLE(i)      ((i)->data & (1<<1))
#define RPDITEM_IS_ARRAY(i)         (!RPDITEM_IS_VARIABLE(i))
#define RPDITEM_IS_RELATIVE(i)      ((i)->data & (1<<2))
#define RPDITEM_IS_ABSOLUTE(i)      (!RPDITEM_IS_RELATIVE(i))
#define RPDITEM_IS_WARP(i)          ((i)->data & (1<<3))
#define RPDITEM_IS_NOWARP(i)        (!RPDITEM_IS_WARP(i))
#define RPDITEM_IS_NONLINEAR(i)     ((i)->data & (1<<4))
#define RPDITEM_IS_LINEAR(i)        (!RPDITEM_IS_NONLINEAR(i))
#define RPDITEM_IS_NOPREFST(i)      ((i)->data & (1<<5))
#define RPDITEM_IS_PREFST(i)        (!RPDITEM_IS_PREFST(i))
#define RPDITEM_IS_NONULLPOS(i)     ((i)->data & (1<<6))
#define RPDITEM_IS_NULLPOS(i)       (!RPDITEM_IS_NONULLPOS(i))
#define RPDITEM_IS_VOLATILE(i)      ((i)->data & (1<<7))
#define RPDITEM_IS_NONVOLATILE(i)   (!RPDITEM_IS_VOLATILE(i))
#define RPDITEM_IS_BUFFEREDBYTES(i) ((i)->data & (1<<8))
#define RPDITEM_IS_BITFIELD(i)      (!RPDITEM_IS_BUFFEREDBYTES(i))
#define RPDITEM_IS_UNKNOWN(i)       ((i)->data & ~((1<<9)-1))

/*****************************************************************************
 ***************************** Local types ***********************************
 *****************************************************************************/
/* Report desctiptor item type values. */
typedef enum {
	rpdit_main,
	rpdit_global,
	rpdit_local
} rpdi_type_t;

/* Report descriptor rpdit_global item tag values */
typedef enum {
	rpditgg_usagepage,
	rpditgg_logicalmin,
	rpditgg_logicalmax,
	rpditgg_physicalmin,
	rpditgg_physicalmax,
	rpditgg_unitexponent,
	rpditgg_unit,
	rpditgg_reportsize,
	rpditgg_reportid,
	rpditgg_reportcount,
	rpditgg_push,
	rpditgg_pop
} rpdi_tag_global_t;

/* Report descriptor rpdit_local item tag values */
typedef enum {
	rpditgl_usage,
	rpditgl_usagemin,
	rpditgl_usagemax,
	rpditgl_designatorindex,
	rpditgl_designatormin,
	rpditgl_designatormax,
	rpditgl_stringindex =7,
	rpditgl_stringmin,
	rpditgl_stringmax,
	rpditgl_delimiter
} rpdi_tag_local_t;

/* Report descriptor rpdit_main item tag values */
typedef enum {
	rpditgm_input =8,
	rpditgm_output,
	rpditgm_start_collection,
	rpditgm_feature,
	rpditgm_end_collection
} rpdi_tag_main_t;

/* Hid report descriptor item. Used by the parser while processind the
   report descriptor. */
typedef struct {
  hcc_u8 tag;
  rpdi_type_t type;
  hcc_u8  size;
  hcc_u32 data;
} rpd_item_t;

/* Report descriptor collection type values. */
typedef enum {
	coty_physical,
	coty_application,
	coty_logical,
	coty_report,
	coty_namedarray,
	coty_usageswitch,
	coty_usagemodifier
} rpdi_coltyp_t;

typedef struct {
  hcc_u16 upage;
  hcc_s32 lmin;
  hcc_s32 lmax;
  hcc_s32 pmin;
  hcc_s32 pmax;
  hcc_u32 unint_exponent;
  hcc_u32 unit;
  hcc_u8  rsize;
  hcc_u8  report_id;
  hcc_u8  rcount;
} parser_global_state_t;

/*****************************************************************************
 ************************* Module variables **********************************
 *****************************************************************************/
static struct {
  parser_global_state_t gstate[MAX_GSTACK_DEPTH];
  int nextndx;
} gstate_stack;

/*****************************************************************************
 ************************* Global variables **********************************
 *****************************************************************************/

/*****************************************************************************
 ************************* Function prototypes *******************************
 *****************************************************************************/

/*****************************************************************************
 ************************* Function declarations******************************
 *****************************************************************************/
static report_t *allocate_report(report_t *reports, hcc_u16 reports_size, report_type_t type, int id)
{
  int x;
  for(x=0; x < reports_size; x++)
  {
    if (reports[x].type == rpt_invalid)
	{
      int r;
	  reports[x].type=type;
	  reports[x].id=id;
	  reports[x].bit_ofs=0;
      for (r=0; r<sizeof(reports[x].items)/sizeof(reports[x].items[0]); r++)
      {
        reports[x].items[r].offset=INVALID_OFFSET;
      }
	  return(reports+x);
	}
  }
  return(0);
}

static rp_item_t *alloc_rp_item(report_t *report, parser_global_state_t *gs, hcc_u32 usage_min, hcc_u32 usage_max)
{
  int rp_ndx;
  /* Look for an empty place. */
  for(rp_ndx=0; rp_ndx<sizeof(report->items)/sizeof(report->items[0]); rp_ndx++)
  {
    rp_item_t *items=report->items;
    if (items[rp_ndx].offset==INVALID_OFFSET)
    {
	    /* Fill new item with values. */
      if (usage_max==usage_min)
      {
        items[rp_ndx].size=(hcc_u8)gs->rsize;
        items[rp_ndx].ary_size=0;
        items[rp_ndx].sign=(hcc_u8)(gs->lmin<0 ? 1 : 0);
      }
      else
      {
        items[rp_ndx].size=(hcc_u8)(gs->rsize*gs->rcount);
        items[rp_ndx].ary_size=(hcc_u8)(gs->rcount);
        items[rp_ndx].sign=0;
      }

	  items[rp_ndx].offset=(hcc_u8)(report->bit_ofs/8);
      items[rp_ndx].shift=(hcc_u8)(report->bit_ofs%8);
	
	  /* Update last free bit offset in report. */
	  report->bit_ofs+=items[rp_ndx].size;

      /* TODO: handle extended usages */
      items[rp_ndx].logical_min=gs->lmin;
      items[rp_ndx].logical_max=gs->lmax;
      items[rp_ndx].physical_min=gs->pmin;
      items[rp_ndx].physical_max=gs->pmax;
      items[rp_ndx].usage_page=gs->upage;
      items[rp_ndx].usage_min=(hcc_u16)usage_min;
      items[rp_ndx].usage_max=(hcc_u16)usage_max;

      items[rp_ndx].resolution=0;

        /* Avoid div0.*/
      if (gs->lmax!=gs->lmin)
      {
        items[rp_ndx].resolution=(hcc_u32)((gs->pmax-gs->pmin)/(gs->lmax-gs->lmin));
        /* it is useless to multiply by one when reading or writing
           the element. So resolution 1 is disabled. */
        if (items[rp_ndx].resolution==1)
        {
          items[rp_ndx].resolution=0;
        }
      }
      return(items+rp_ndx);
    }
  }
  return(0);
}

/* Convert the next item in the report descriptor to a structure. */
static hcc_u8 *get_next_rpd_item(hcc_u8 *start, rpd_item_t *item)
{
  int x;

  item->tag=(hcc_u8)RPDITEM_TAG(*start);
  item->type=RPDITEM_TYPE(*start);
  item->size=RPDITEM_SIZE(*start);
  if (item->size==3)
  {
    item->size=4;
  }
  item->data=0;
  start++;

  /* up to 4 data bytes in big endian format.
     Bytes not present default to value 0. */

  for(x=0; x<item->size; x++)
  {
    item->data |= *start<<(8*x);
    start++;
  }
  return(start);
}

static int push_parser_gstate(parser_global_state_t *s)
{
  if (gstate_stack.nextndx>=sizeof(gstate_stack.gstate)/sizeof(gstate_stack.gstate[0]))
  {
    return(1);
  }
  gstate_stack.nextndx++;
  memcpy(&gstate_stack.gstate[gstate_stack.nextndx], s, sizeof(parser_global_state_t));
  return(0);
}

static int pop_parser_gstate(parser_global_state_t *s)
{
  if (gstate_stack.nextndx < 0)
  {
	  return(1);
  }
  memcpy(s, &gstate_stack.gstate[gstate_stack.nextndx], sizeof(parser_global_state_t));
  gstate_stack.nextndx--;
  return(0);
}

static void init_gstate(parser_global_state_t *gs)
{
  /* TODO: default values? */
  memset(gs, 0, sizeof(parser_global_state_t));
}

/* Return the next value of the specified tag type. */
static int get_local_tag(rpdi_tag_local_t type, int ndx, hcc_u8* start, hcc_u8* end
				  , hcc_u32 *value)
{
  /* Info about the current report descriptor item. */
  rpd_item_t rpdi;
  /* Current offset in the report descriptor. */
  hcc_u8 *curr=start;
  /* Index of last seen report descriptor item having the type searching. */
  int curr_ndx=-1;
  unsigned int curr_val=-1u;

  /* hold info about min/max tags. */
  struct  {
      hcc_u32 min;
      hcc_u32 max;
      int min_seen;
      int max_seen;
  } range = { 0, 0, 0, 0};

  /* Reset rnage status (no range start or stop seen) */
  range.min_seen=0;
  range.max_seen=0;

  /* Loop to process report descriptor. */
  while(curr<end)
  {
	/* Clear the number of found items. */
    unsigned int size=0;

    hcc_u8* next=get_next_rpd_item(curr, &rpdi);

	  /* skipp report item if it is not a local tag */
	  if (rpdi.type == rpdit_local)
	  {
		  /* See if type matches */
		  if (type==rpditgl_usage)
		  {
		    switch ((rpdi_tag_local_t)rpdi.tag)
		    {
		    /* if this is the start of a range */
		    case rpditgl_usagemin:
			  range.min_seen=1;
			  range.min=rpdi.data;
			  break;
        /* this is the end of a range */
		    case rpditgl_usagemax:
			  range.max_seen=1;
			  range.max=rpdi.data;
			  break;
		    /* this is a single item */
		    case rpditgl_usage:
			    size=1;
			  break;
			default:
			  break;
		    }
		  }
  		
		  /* If both the start and the end of the range is known, then we found a range.
         Calculate range length. */
		  if (range.min_seen && range.max_seen)
		  {
		    size=range.max-range.min+1;
		  }

		  /* If the type matches (number of found items is greater than zero*/
		  if (size)
		  {
		    /* Advance index to reflect index of last found item */
		    curr_ndx+=size;
		    /* If this is a single item */
		    if (size==1)
		    {
			    /* see if index of found item equals to the requested index */
			    if (curr_ndx == ndx)
			    {
			      *value=rpdi.data;
			      return(0);
			    }
			    /* save last known value of tag type */
			    curr_val=rpdi.data;
		    }
		    /* this is a range */
		    else
		    {
			    /* see if requested index is in range */
			    if (curr_ndx >= ndx)
			    {
			      *value=range.min+(ndx-(curr_ndx-size+1));
			      return(0);
			    }
 			    /* save last known value of tag type */
  			  curr_val=range.max;
	        /* Prepare for next range. */
			    range.max_seen=range.min_seen=0;
		    }
		  }
	  }
    curr=next;
  }

  /* reached end of report descriptor and did not found an element with the
     requested type.
	   Return last found vale if we had a hit */
  if (curr_ndx != -1)
  {
    *value=curr_val;
    return(0);
  }
   /* not found status */
   return(1);
}

static int create_report_item(report_t *rep, parser_global_state_t *gs, rpd_item_t *rpdi, hcc_u8 *sblock, hcc_u8 *eblock)
{
  int x;
  rp_item_t *rpi;

  /* If the item is a variable, add a report item for each variable. */
  if (RPDITEM_IS_VARIABLE(rpdi))
  {
    for (x=0; x<gs->rcount; x++)
    {
      hcc_u32 usage;
      if (get_local_tag(rpditgl_usage, x, sblock, eblock, &usage))
      {
        /* this means an item to be ignored. We still need to add it
           to advance bit offset. */
        usage=0;
      }
      rpi=alloc_rp_item(rep, gs, usage, usage);
      if (rpi == 0)
      {
        return(1);
      }
    }
  }
  /* If the item is an array, then add one array report item. */
  else if (RPDITEM_IS_ARRAY(rpdi))
  {
    /* Note: an array item must define one usage for each control in
       the array. The spec requires (not explicitly) the usages to form a
       continous filed. This means an array item should be preceeded by
       usage_min and usage_max tags. This defines the length of the array too.
       Unfortunately usage_min and usage_max is not allways used, so we use
       the following workaround.
       Try to detect the array size using the logical_min and logical_max
       tags. If set, then get usage index 0 and usage index (logical range
       length).
       Get usage index 0 and usage index maximum possible logical max based
       on report size.
       */
    hcc_u32 usage_min, usage_max=0;
    if (get_local_tag(rpditgl_usage, 0, sblock, eblock, &usage_min))
    {
      hcc_u8 rsize, rcount;
      /* this means an item to be ignored. We still need to add it
         to advance bit offset. Instead of an array item allocate a
         variable item with the same bit size. */
      usage_min=usage_max=0;
      rsize=gs->rsize;
      rcount=gs->rcount;
      gs->rsize=(hcc_u8)(gs->rsize*gs->rcount);
      gs->rcount=1;
      rpi=alloc_rp_item(rep, gs, usage_min, usage_max);
      gs->rcount=rcount;
      gs->rsize=rsize;
    }
    else
    {
      /* Since previously we already found an usage, this tyme we must
         have a hit. So return value can not be an error. */
      rpi=alloc_rp_item(rep, gs, usage_min, usage_max);
    }

    if (rpi==0)
    {
        return(1);
    }
  }
  return(0);
}

static hcc_s32 cvt_rpd_item_value(rpd_item_t *rpdi)
{
  hcc_s32 r=0;
  /* check if the value is signed */
  if (rpdi->data & (1<<(rpdi->size-1)))
  {
    switch(rpdi->size)
    {
    case 1:
      r=(hcc_s8)rpdi->data;
      break;
    case 2:
      r=(hcc_s16)rpdi->data;
      break;
    case 4:
      r=(hcc_s32)rpdi->data;
      break;
    }
  }
  else
  {
    switch(rpdi->size)
    {
    case 1:
      r=(hcc_u8)rpdi->data;
      break;
    case 2:
      r=(hcc_u16)rpdi->data;
      break;
    case 4:
      /* If value if out of range set the nearest possible. */
      if (rpdi->data>=(1u<<31))
      {
        r=0x7ffffff;
      }
      else
      {
        r=(hcc_s32)rpdi->data;
      }
      break;
    }
  }
  return(r);
}

static void update_global_state(parser_global_state_t *gs, rpd_item_t *rpdi)
{
  switch((rpdi_tag_global_t)rpdi->tag)
  {
  case rpditgg_usagepage:
    gs->upage=(hcc_u16)rpdi->data;
    break;
  case rpditgg_logicalmin:
    gs->lmin=cvt_rpd_item_value(rpdi);
    break;
  case rpditgg_logicalmax:
    gs->lmax=cvt_rpd_item_value(rpdi);
    break;
  case rpditgg_physicalmin:
    gs->pmin=cvt_rpd_item_value(rpdi);
    break;
  case rpditgg_physicalmax:
    gs->pmax=cvt_rpd_item_value(rpdi);
    break;
  case rpditgg_unitexponent:
    break;
  case rpditgg_unit:
    break;
  case rpditgg_reportsize:
    gs->rsize=(hcc_u8)rpdi->data;
    break;
  case rpditgg_reportid:
    gs->report_id=(hcc_u8)rpdi->data;
    break;
  case rpditgg_reportcount:
    gs->rcount=(hcc_u8)rpdi->data;
    break;
  case rpditgg_push:
    push_parser_gstate(gs);
    break;
  case rpditgg_pop:
    pop_parser_gstate(gs);
    break;
  default:
    break;
  }
}

int find_collection(hcc_u8 *data, hcc_u16 length, hcc_u16 upage, hcc_u16 usage, hcc_u8 **start, hcc_u16 *size)
{
  rpd_item_t rpdi;
  parser_global_state_t g_state;

  /* the start of the report descriptor */
  hcc_u8 *curr=data;
  /* end of report descriptor */
  hcc_u8 *end=data+length;
  hcc_u8 level=0;
  hcc_u8 found=0;
  /* start of current main item */
  hcc_u8 *smain=data;

  *start=data;
  *size=length;

  /* init global state */
  init_gstate(&g_state);

  while(curr < end)
  {
    hcc_u8* next=get_next_rpd_item(curr, &rpdi);
    switch(rpdi.type)
    {
    case rpdit_main:
      if ((rpdi_tag_main_t)rpdi.tag == rpditgm_start_collection)
      {
        if (g_state.upage == upage)
        {
          if (usage !=0)
          {
            hcc_u32 lusage;
            if (0==get_local_tag(rpditgl_usage, -1, smain, curr, &lusage)
               && lusage==usage)
            {
              *start=next;
              found=1;
            }
          }
        }
        if (found)
        {
          level++;
        }
      }
      else if ((rpdi_tag_main_t)rpdi.tag == rpditgm_end_collection)
      {
        if (found)
        {
          level--;
          if (level==0)
          {
            *size=(hcc_u16)(curr-*start);
            return(0);
          }
        }
      }
      smain=next;
      break;
    case rpdit_global:
      update_global_state(&g_state, &rpdi);
      break;
    default:
      break;
    }
    curr=next;
  }
  return(1);
}

int get_reports(report_t *reports, hcc_u16 reports_size, hcc_u8 *data, hcc_u16 length)
{
  parser_global_state_t g_state;
  rpd_item_t rpdi;
  int x;

  /* the start of the report descriptor */
  hcc_u8 *curr=data;
  /* end of report descriptor */
  hcc_u8 *end=data+length;
  /* start of current main item */
  hcc_u8 *smain=data;

  /* the current input report */
  report_t *in_report=0;

  /* init global state */
  init_gstate(&g_state);
  gstate_stack.nextndx=0;
  for(x=0; x<reports_size; x++)
  {
    reports[x].type=rpt_invalid;
  }

  while(curr < end)
  {
    /* find the next main item and store values for global items */
    hcc_u8 *next=get_next_rpd_item(curr, &rpdi);
    switch(rpdi.type)
    {
    case rpdit_main:
      switch((rpdi_tag_main_t)rpdi.tag)
      {
      case rpditgm_input: /* allocate some new input items. */
      {
	    if (!in_report || in_report->id != g_state.report_id)
	    {
          in_report=allocate_report(reports, reports_size, rpt_in, g_state.report_id);
          if (in_report==0)
          {
            return(1);
          }
	    }
        if (create_report_item(in_report, &g_state, &rpdi, smain, curr))
        {
          return(1);
        }
      }
      break;
      case rpditgm_output:
      case rpditgm_feature:
        break;
      case rpditgm_start_collection:
      case rpditgm_end_collection:
        break;
      default:
        break;
      }
      /* Local items are reset after each rpdit_main item is read. */
      smain=next;
      break;

    case rpdit_global:
      update_global_state(&g_state, &rpdi);
      break;
    case rpdit_local:
	    /* ignore local tags. we pharse them later when a min item has been found */
	    break;
	  default:
	    break;
    }
    curr=next;
  }
  return(0);
}

static hcc_u32 read_bits(hcc_u8 *buf, hcc_u8 shift, hcc_u8 size)
{
  hcc_u32 val=0;
  int x;
  /* read data bytes in little endian order */
  for(x=0; x < ((size & 0x7) ? (size/8)+1 : (size/8)); x++)
  {
    val |= buf[x] << (x*8);
  }
  val=(val >> shift) & ((1<<size)-1);
  return(val);
}

int read_item(rp_item_t *ri, hcc_u8 *buffer, hcc_u8 index, hcc_s32 *value)
{
  hcc_u32 val;
  hcc_s32 vs;
  char *data=(char*)buffer+ri->offset;
  hcc_u8 shift=ri->shift;

  /* If the item is an array, calcuaate item offset. */
  if (ri->ary_size)
  {
    hcc_u32 bofs;
    /* if requested inted is out of bounds return error */
    if (index > ri->ary_size)
    {
      return(1);
    }
    bofs=(hcc_u32)((ri->size*index)+shift);
    /* calculate new base pointer and shift value */
    data+=bofs/8;
    shift=(hcc_u8)(bofs%8);
  }

  /* read out the value */
  val=read_bits((hcc_u8*)data, ri->shift, ri->size);

  /* See if the number is signed and negative */
  if ((ri->sign) && (val & (1<<(ri->size-1))))
  {
    /* yes, so sign extend value to 32 bits. */
    vs=(int)((-1 & ~((1<<(ri->size))-1)) | val);
  }
  else
  {
    /* no sign extend value to 32 bits */
    vs=(hcc_s32)val;
  }

  /* if value is out of range */
  if (vs<ri->logical_min || vs > ri->logical_max)
  {
    return(1);
  }

  /* convert logical value to physical value */
  if(ri->resolution == 0)
  {
    *value=vs;
  }
  else
  {
    *value=(hcc_s32)(vs*ri->resolution);
  }
  return(0);
}

static void write_bits(hcc_u8 *buf, hcc_u8 shift, hcc_u8 size, hcc_u32 value)
{
  int x;
  hcc_u32 mask;

  /* Write logical value to report in little endian order. */
  mask=(hcc_u32)(((1<<size)-1)<<shift);
  value = value << shift;

  for(x=0; x < ((size & 0x7) ? (size/8)+1 : (size/8)); x++)
  {
    *(buf+x)=(hcc_u8)((*(buf+x) & ~(mask>>(x*8))) | ((value>>(x*8)) & (mask>>(x*8))));
  }
}

int write_item(rp_item_t *ri, hcc_u8* buffer, hcc_u8 index, hcc_s32 value)
{
  hcc_u8 *data=buffer+ri->offset;
  hcc_u8 shift=ri->shift;

  /* Note: out of range physical values are transmitted. It is up to the device to
     ignore them. */

  /* If the item is an array, calcuaate item offset. */
  if (ri->ary_size)
  {
    index++;
    return(1);
  }

  /* Convert physical value to logical value. */
  if (ri->resolution != 0)
  {
    value/=ri->resolution;
  }
  write_bits(data, shift, ri->size, (hcc_u32)value);

  return(0);
}

