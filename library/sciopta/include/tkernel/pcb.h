/*SOC
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
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S05216BS2                                                    **
** +Revision: 1.6 +                                                 **
** +Date: 2005/11/08 06:31:06 +                                     **
** Process control block definition                                 **
**********************************************************************
EOC*/
#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_PCB_H_
#define _KERNEL_PCB_H_
/* SOCNV */
#define PCB_PCBLIST_MASK     0x0f
#define PCB_SLEEPERLIST_MASK 0xf0
#define PCB_PRIO_FULL_RESTORE   0x80
#define PCB_PRIO_PRIO_MASK   0x0f
#define PCB_PRIO_IDLE        0x08
#define PCB_STATE_STOPPED    0x80
#define PCB_STATE_STOPPED_POS 7
#define PCB_STATE_READY      0x40
#define PCB_STATE_READY_POS  6
#define PCB_STATE_TYPE_MASK  0x30
#define PCB_STATE_PRIO       0x00
#define PCB_STATE_INT        0x10
#define PCB_STATE_TIM        0x20
#define PCB_STATE_TIM_POS    5
#define PCB_STATE_UINT       0x30
#define PCB_STATE_UINT_POS   5
#define PCB_STATE_TMO        0x01
#define PCB_STATE_MSGRX      0x02
#define PCB_STATE_WAITTMO    0x04

#define NOT_PCB_STATE_TMO    0xfe


#if SIZEOF_PTR == 2
#define PCB_MSGQUEUE 0
#define PCB_STACKPTR 4
#define PCB_LISTS_B  6
#define PCB_STATE_B  7
#define PCB_PRIO_B   8
#define PCB_SLICE_B  8
#define PCB_VECTOR_B 8
#define PCB_TMO_B    9
#define PCB_PID_B    9

#define SIZEOF_PCB   10
#else
#define PCB_MSGQUEUE 0
#define PCB_STACKPTR 8
#define PCB_LISTS_B  12
#define PCB_STATE_B  13
#define PCB_PRIO_B   14
#define PCB_SLICE_B  14
#define PCB_VECTOR_B 14
#define PCB_TMO_B    15
#define PCB_PID_B    15

#define SIZEOF_PCB   16
#endif


/* EOCNV */
#ifndef __ASM_INCLUDED__

struct sc_pcb_s {
  sgl_head_t msgQueue;
  char NEARPTR stackptr;
  struct {
    unsigned char pcbs:4;    /* next process in list */
    unsigned char sleepers:4;/* next in list of sleeping process */
  }  PACKED lists;
  struct {
    unsigned char state:4;
    unsigned char type:2;
    unsigned char ready:1;
    unsigned char stop:1;
  } PACKED state;
  union {
    __u8 prio;
    __u8 slice;
    __u8 vector;
  } PACKED a;
  union {
    __u8 tmo;
    __u8 pid;
  } PACKED b;
} PACKED;

typedef struct sc_pcb_s sc_pcb_t;

#endif /* #ifndef __ASM_INCLUDED__ */

/* SOCNV */
#if SIZEOF_PTR == 2
#define PDB_NAME      0x00
#define PDB_STACKBTM  0x02
#define PDB_STACKTOP  0x04
#define PDB_FUNC      0x06
#define PDB_TYPE      0x08
#define PDB_PRIO      0x09
#define PDB_SLICE     0x09
#define PDB_VECTOR    0x09
#define PDB_INITIAL   0x0a
#define PDB_STOPPED   0x0b
#define SIZEOF_PDB    0x0c
#else
#define PDB_NAME      0x00
#define PDB_STACKBTM  0x04
#define PDB_STACKTOP  0x08
#define PDB_FUNC      0x0c
#define PDB_TYPE      0x10
#define PDB_PRIO      0x11
#define PDB_SLICE     0x11
#define PDB_VECTOR    0x11
#define PDB_INITIAL   0x12
#define PDB_STOPPED   0x13
#define SIZEOF_PDB    0x14
#endif

#define PDB_TYPE_PRIO      PCB_STATE_PRIO
#define PDB_TYPE_INT       PCB_STATE_INT
#define PDB_TYPE_TIM       PCB_STATE_TIM
#define PDB_TYPE_UINT      PCB_STATE_UINT

/* EOCNV */
#ifndef __ASM_INCLUDED__
struct sc_pdb_s {
  char FARPTR name;
  __u8 NEARPTR stackbtm;
  __u8 NEARPTR stacktop;
  void * func;
  __u8 type;
  union {
    __u8 prio;
    __u8 slice;
    __u8 vector;
  } PACKED;
  __u8 initial;
  __u8 stopped;
} PACKED;
typedef struct sc_pdb_s sc_pdb_t;
#endif /* #ifndef __ASM_INCLUDED__ */
#endif /* _KERNEL_PCB_H_ */
