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
** ID: S05216BS1                                                    **
** +Revision: 1.7 +                                                 **
** +Date: 2005/12/06 13:54:38 +                                     **
** Message header structure for TinySciopta                         **
**********************************************************************
EOC*/
#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef __BYTE_ORDER
#include <arch/endian.h>
#endif

#ifndef _KERNEL_MSG_H_
#define _KERNEL_MSG_H_

/*SOCNV*/
#define SC_MSGRX_PID       0x01
#define SC_MSGRX_PID_POS   0
#define SC_MSGRX_MSGID     0x02
#define SC_MSGRX_MSGID_POS 1
#define SC_MSGRX_BOTH      (SC_MSGRX_PID+SC_MSGRX_MSGID)
#define SC_MSGRX_NOT       0x04
#define SC_MSGRX_NOT_POS   2
/* EOCNV */
#define SC_MSGRX_ALL  ((const void *)0)
#define NULL_MSG ((sc_msg_t)0)

/*SOCNV*/
#if SIZEOF_PTR == 2
#define MSG_LIST    0
#define MSG_SIZE_H  2
#if CPU == MSP430
#define MSG_PBOAS_H 4
#define MSG_AS_B    4
#define MSG_PBO_B   5
#else
#define MSG_PBSAO_H 4
#define MSG_PBS_B   4
#define MSG_AO_B    5
#endif
#define MSG_ID      6
#define MSG_HEAD    MSG_ID
#else
#define MSG_LIST    0
#define MSG_SIZE_H  4
#define MSG_PBSAO_H 6
#if __BYTE_ORDER == __BIGENDIAN
#define MSG_PBS_B   6
#define MSG_AO_B    7
#else
#define MSG_PBS_B   7
#define MSG_AO_B    6
#endif
#define MSG_ID      8
#define MSG_HEAD    MSG_ID
#endif

#define MSG_PLIDX_POS       15
#define MSG_BUFSIZE_IDX_POS 12
#define MSG_SENDER_POS       8
#define MSG_ADDRESSEE_POS    4
#define MSG_OWNER_POS        0

#define MSG_SENDER_MASK      0x0f
#define MSG_ADDRESSEE_MASK   0xf0
#define MSG_OWNER_MASK       0x0f

/* EOCNV */
#ifndef __ASM_INCLUDED__
struct sc_msg_hd_s{
  sgl_t list;
  sc_bufsize_t size;                   /* allocated size */
  struct {
    unsigned int plidx:1;              /* pool this message is from */
    unsigned int bufsize_idx:3;        /* index to fixed buffer size */
    unsigned int sender:4;             /* last sender */
    unsigned int addressee:4;          /* last addressee */
    unsigned int owner:4;              /* current owner */
  } PACKED pbsao;
  sc_msgid_t id;
} PACKED;
typedef struct sc_msg_hd_s sc_msg_hd_t;
#define MSGHEAD (offsetof(sc_msg_hd_t,id))
#define MSG(ptr,member) SGL_ITEM(sc_msg_hd_t,ptr,member)

/* Function prototypes */
SCFUNC4(sc_msg_t,sc_msgAlloc,sc_bufsize_t,sc_msgid_t,sc_poolid_t,sc_ticks_t);
SCFUNC1(void,sc_msgFree,sc_msgptr_t);
SCFUNC3(void,sc_msgTx,sc_msgptr_t,sc_pid_t,flags_t);
SCFUNC3(sc_msg_t,sc_msgRx,sc_ticks_t,const void *,flags_t);
SCFUNC1(sc_pid_t,sc_msgSndGet,sc_msgptr_t);

SCFUNC1(sc_pid_t,sc_msgAddrGet,sc_msgptr_t);
SCFUNC1(sc_pid_t,sc_msgOwnerGet,sc_msgptr_t);
SCFUNC1(sc_bufsize_t,sc_msgSizeGet,sc_msgptr_t);
//->SCFUNC1(sc_poolid_t,sc_msgPoolIdGet,sc_msgptr_t);
#ifndef __KERNEL__
#define sc_msgAlloc(a,b,c,d)     _FUNC4(sc_msgAlloc,a,b,c,d)
#define sc_msgFree(a)            _FUNC1(sc_msgFree,a)
#define sc_msgTx(a,b,c)          _FUNC3(sc_msgTx,a,b,c)
#define sc_msgRx(a,b,c)          _FUNC3(sc_msgRx,a,b,c)
#define sc_msgSndGet(a)          _FUNC1(sc_msgSndGet,a)

#define sc_msgAddrGet(a)         _FUNC1(sc_msgAddrGet,a)
#define sc_msgOwnerGet(a)        _FUNC1(sc_msgOwnerGet,a)
#define sc_msgSizeGet(a)         _FUNC1(sc_msgSizeGet,a)
//->#define sc_msgPoolIdGet(a)       _FUNC1(sc_msgPoolIdGet,a)
#endif /* __KERNEL__ */


#endif /* #ifndef __ASM_INCLUDED__ */



#endif /*_KERNEL_MSG_H_*/
