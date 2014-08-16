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
** ID: S05217BS1                                                    **
** +Revision: 1.7 +                                                 **
** +Date: 2005/12/06 13:54:38 +                                     **
** System control structur                                          **
**********************************************************************
EOC*/
#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_SYSTEM_H_
#define _KERNEL_SYSTEM_H_

/* SOCNV */
/* EOCNV */
#ifndef __ASM_INCLUDED__
struct sc_err_msg_s {
  sc_errcode_t error;
  sc_extra_t extra;
  sc_pcb_t *pcb;
  sc_pid_t pid;
  int user;
};
typedef struct sc_err_msg_s sc_err_msg_t;
#endif

/* SOCNV */
#if SIZEOF_PTR == 2
#define SC_CURPCB          0
#define SC_CURPRIOPCB      2
#define SC_CURPID_B        4
#define SC_FREE_B          5
#define SC_NESTED_IRQ_B    6
#define SC_SCHEDULE_REQ_B  7
#define SC_SLEEPERS_B      8
#define SC_PRIOMASK_B      9
#define SC_TCK            10
#define SC_READYLIST      14

#define SIZEOF_SCIOPTA    22

#else
#define SC_CURPCB          0
#define SC_CURPRIOPCB      4
#define SC_CURPID_B        8
#define SC_FREE_B          9
#define SC_NESTED_IRQ_B   10
#define SC_SCHEDULE_REQ_B 11
#define SC_SLEEPERS_B     12
#define SC_PRIOMASK_B     13
#define SC_FREE_H         14
#define SC_TCK            16
#define SC_READYLIST      20
#define SIZEOF_SCIOPTA    28
#endif

/* EOCNV */
#ifndef __ASM_INCLUDED__
struct sciopta_s {
  sc_pcb_t *curPcb;
  sc_pcb_t *curPrioPcb;
  __u8 curPid;
  __u8 freeb;
  __u8 nested_irq;
  __u8 schedule_req;

  __u8 sleepers;
  __u8 prioMask;
#if SIZEOF_PTR != 2
  __u16 freeh;
#endif
  __u32 tick;
  __u8 readyList[8];
} PACKED;

typedef struct sciopta_s sciopta_t;
#endif /* __ASM_INCLUDED__ */

#endif /*  _KERNEL_SYSTEM_H_ */
