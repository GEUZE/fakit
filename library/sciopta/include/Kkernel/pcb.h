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
** ID: S01103BS49                                                   **
** +Revision: 1.17 +                                                **
** +Date: 2005/07/01 11:18:25 +                                     **
** Process control block definitions                                **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_PCB_H_
#define _KERNEL_PCB_H_ 1
/* SOCNV */
#define SC_PROC_NAME_SIZE 31

/*
****************************************
** Process-states
****************************************
*/
#define ST_TMO                0x01  /* a timeout occured */
#define ST_TRIGGER            0x02  /* process has been triggered */
#define ST_READY              0x03
#define ST_WAIT_MSGALLOC      0x10  /* process waits for a free msg */
#define ST_WAIT_MSGRX         0x20
#define ST_WAIT_TRIGGER       0x40

#define ST_TMO_MASK           0x87

#define ST_WAIT_TMO           0x08  /* process wait for an timeout */
#define ST_STOPPED            0x80  /* process is stopped */
#define ST_STOPPED_POS        7
#define ST_NOT_READY_MASK     0xfc
/*
****************************************
** Process-flags
****************************************
*/
#define PCB_FLG_READY            0x0001  /* process is ready */
#define PCB_FLG_IS_TIMER         0x0002  /* process is a timer */
#define PCB_FLG_IS_IRQ           0x0004  /* process is an interrupt */
#define PCB_FLG_IS_UIRQ          0x0008  /* process is a user interrupt */
#define PCB_FLG_IS_PRIO          0x0010  /* process is a prio. process */
#define PCB_FLG_IS_STATIC        0x0020  /* process is a static process */
#define PCB_FLG_ALLOCCLR         0x0040
#define PCB_FLG_IS_CONNECTOR     0x0080  /* process is a connector */

#define PCB_NOT_FLG_IS_SCPROC    0x00E9
/* EOCNV */
#define SC_PCB_MAGIC (SC_MSG_BASE + 0x2207)
#define SC_PCB_MAGIC_STATIC (SC_MSG_BASE + 0x3008)

#define SC_PDB_STATE_RUN 1
/*
****************************************
** offsets
****************************************
*/
/*
** Possible tuning:
** combine PCB_PRIO_B with PCB_DEFAULTPOOL_B  => 1 byte
** w/o stackcheck => remove STACKBTM,STACKTOP => 4 bytes
** interrupts => remove PRIOMASK              => 4 bytes
*/
#if SIZEOF_PTR == 2
/* SOCNV */
#define PCB_MSGQUEUE     0
#define PCB_FLAGS_B      4
#define PCB_STATE_B      5
#define PCB_STOPPED_B    6
#define PCB_PRIO_B       7
#define PCB_PCBLIST      8
#define PCB_SLEEPERS     9
#define PCB_PID_B        10
#define PCB_TMO          11
#define PCB_TMOIDX       12
#define PCB_DEFAULTPOOL_B  13
#define PCB_SLICE        14
#define PCB_VECTOR       14
#define PCB_LISTHEAD     14
#define PCB_STACKPTR     16
#define PCB_NAME         18
#if CPU == M68HC12
#define PCB_ERRORCODE    20
#define PCB_ENTRY        24
#define PCB_TRIGGER_VAL  26+BANK_OFF
#define PCB_LOCALS       28+BANK_OFF
#define PCB_STACKTOP     30+BANK_OFF
#define PCB_STACKBTM     32+BANK_OFF
#if SC_CDEBUG == 1
#define PCB_CLINE        34+BANK_OFF
#define PCB_CFILE        36+BANK_OFF
#define PCB_PRIOMASK     38+BANK_OFF
#define SIZEOF_PCB       42+BANK_OFF
#else
#define PCB_PRIOMASK     34+BANK_OFF
#define SIZEOF_PCB       38+BANK_OFF
#endif
#else
#define PCB_ERRORCODE    20+BANK_OFF
#define PCB_ENTRY        24+BANK_OFF
#define PCB_TRIGGER_VAL  26+(2*BANK_OFF)
#define PCB_LOCALS       28+(2*BANK_OFF)
#define PCB_STACKTOP     30+(2*BANK_OFF)
#define PCB_STACKBTM     32+(2*BANK_OFF)
#if SC_CDEBUG == 1
#define PCB_CLINE        34+(2*BANK_OFF)
#define PCB_CFILE        36+(2*BANK_OFF)
#define PCB_PRIOMASK     38+(2*BANK_OFF)
#define SIZEOF_PCB       42+(2*BANK_OFF)
#else
#define PCB_PRIOMASK     34+(2*BANK_OFF)
#define SIZEOF_PCB       38+(2*BANK_OFF)
#endif
#endif
/* EOCNV */
#else /* SIZEOF_PTR == 2*/
#define PCB_MSGQUEUE     0 

#define PCB_FLAGS_B      8
#define PCB_STATE_B      9
#define PCB_STOPPED_B    10
#define PCB_PRIO_B       11

#define PCB_PCBLIST_B    12
#define PCB_SLEEPERS_B   13
#define PCB_PID_B        14
#define PCB_SPARE        15          
#define PCB_TMO_H        16
#define PCB_TMOIDX_B     18
#define PCB_DEFAULTPOOL_B  19
#define PCB_SLICE        20
#define PCB_VECTOR       20
#define PCB_LISTHEAD     20
#define PCB_STACKPTR     24
#define PCB_STACKBTM     28
#define PCB_STACKTOP     32
#define PCB_NAME         36
#define PCB_ERRORCODE    40
#define PCB_ENTRY        44
#define PCB_TRIGGER_VAL  48
#define PCB_LOCALS       52
#if SC_CDEBUG == 1
#define PCB_CLINE        56
#define PCB_CFILE        60

#define SIZEOF_PCB       64
#else
#define SIZEOF_PCB       56
#endif
#endif /* SIZEOF_PTR */

#ifndef __ASM_INCLUDED__
#define PCB(ptr,member) SGL_ITEM(sc_pcb_t,ptr,member)

struct sc_pcb_s {
  sgl_head_t msgQueue;          /* root of msgQueue */
  __u8 flags;                   /* misc flags */
  __u8 state;                   /* process state */
  __u8 stopped;
  __u8 prio;                    /* process' priority */

  __u8 pcblist;             /* next process in list */
  __u8 sleepers;            /* next in list of sleeping process */
  __u8 pid;                 /* process' own ID */
#if SIZEOF_PTR == 2
  __u8 tmo;
#else
  __u8 spare;
  __u16 tmo;                    /* remaining tmo */
#endif
  __u8 tmoidx;                  /* tmo-list currently in */
  __u8 defaultPool;
  union {
    sc_ticks_t slice;             /* period */
    sc_ticks_t vector;
    __u8 NEARPTR listhead;
  } u1;
  char NEARPTR stackptr;               /* current stack-pointer */
  const char FARPTR name;             /* process' name */
  sc_errcode_t errorcode;
  void (* entry)(int);
  sc_triggerval_t triggerValue;
  sc_local_t NEARPTR locals;          /* pointer to an array with local vars */
  char NEARPTR stacktop;               /* first byte above stack */
  char NEARPTR stackbtm;               /* first byte of stack */
#if SC_CDEBUG == 1
  int cline;
  const char *cfile;
#endif
#if SIZEOF_PTR == 2
  __u32 priomask;
#endif
  /* 
  ** host-system specific
  */
#if SCIOPTA == LINUX_HOSTED
  int linuxPid;
#endif
};

typedef struct sc_pcb_s sc_pcb_t;

#define PID2IDX(pid) ((pid) & SC_PID_MSK)
#define PID_IS_EXTERN(pid)  (pid & PID_EXTERN_FLAG)
#endif /* __ASM_INCLUDED__ */
#endif /* _KERNEL_PCB_H_ */

/*
****************************************************************************
** $Log: pcb.h,v $
** Revision 1.17  2005/07/01 11:18:25  bastian
** merge from branch-1.7.2
**
** Revision 1.16.4.1  2005/05/23 14:40:03  bastian
** fix: iar does not calculate correctly
**
** Revision 1.16  2004/12/17 15:24:43  bastian
** fixes for hc12
**
** Revision 1.15  2004/10/07 10:33:37  bastian
** fixes
**
** Revision 1.14  2004/10/07 07:37:47  bastian
** fixed NEAR * to NEARPTR
**
** Revision 1.13  2004/10/06 07:42:24  bastian
** introduced NEAR
**
** Revision 1.12  2004/09/30 08:33:56  bastian
** fixes for m16c
**
** Revision 1.11  2004/06/28 12:18:59  bastian
** moved priomask at end
**
** Revision 1.10  2004/05/07 05:30:53  bastian
** added bit-pos for stop-state
** fixed PID2IDX
**
** Revision 1.9  2004/01/23 08:00:39  bastian
** added mask
**
** Revision 1.8  2004/01/13 14:27:28  bastian
** SC_PROC_NAME_SIZE tagged to be moved to kernelsource
**
** Revision 1.7  2003/12/17 11:08:09  bastian
** added banked-rom support
**
** Revision 1.6  2003/07/22 12:31:45  bastian
** added flag for allocClr
**
** Revision 1.5  2003/07/14 10:09:15  bastian
** removed observer-vars
**
** Revision 1.4  2003/07/04 06:27:47  bastian
** various
**
** Revision 1.3  2003/06/10 06:54:57  bastian
** added priomask if 1<<prio is expensive
**
** Revision 1.2  2003/05/23 14:30:02  bastian
** week-end
**
** Revision 1.1  2003/05/19 12:06:05  bastian
** 1st
****************************************************************************
*/
