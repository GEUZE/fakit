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
** (c) 2007 Sciopta Systems GmbH Deutschland, Sciopta AG/ Schweiz   **
**                                                                  **
**********************************************************************
** ID: S07180BS1                                                    **
** +Revision: 1.2 +                                                 **
** +Date: 2008/01/16 13:01:18 +                                     **
** Header file for tl16c2550                                        **
**********************************************************************
EOC*/

#ifndef __TL16C2550_H__
#define __TL16C2550_H__ 1
#include <sciopta.h>

typedef struct uart16550reg_s {
  union {
    __u8 rbr;
    __u8 thr;
    __u8 dll;
  }u1;
  union {
    __u8 ier;
    __u8 dlm;
  }u2;
  union {
    __u8 iir;
    __u8 fcr;
  }u3;
  __u8 lcr;
  __u8 mcr;
  __u8 lsr;
  __u8 msr;
  __u8 scr;
} uart16550reg_t;

#define UART16550_IER_ERBI  0x01
#define UART16550_IER_ETBEI 0x02
#define UART16550_IER_ELSI  0x04
#define UART16550_IER_EDSSI 0x08

#define UART16550_FCR_EN    0x01
#define UART16550_FCR_RX_RS 0x02
#define UART16550_FCR_TX_RS 0x04
#define UART16550_FCR_DMA   0x08
#define UART16550_FCR_TRG_1 0x00
#define UART16550_FCR_TRG_4 0x40
#define UART16550_FCR_TRG_8 0x80
#define UART16550_FCR_TRG_14 0xc0

#define UART16550_LCR_5BIT  0x00
#define UART16550_LCR_6BIT  0x01
#define UART16550_LCR_7BIT  0x02
#define UART16550_LCR_8BIT  0x03

#define UART16550_LCR_1STP  0x00
#define UART16550_LCR_2STP  0x04
#define UART16550_LCR_PE    0x08
#define UART16550_LCR_EVEN  0x10
#define UART16550_LCR_SP    0x20
#define UART16550_LCR_BREAK 0x40
#define UART16550_LCR_DLAB  0x80

#define UART16550_LSR_DR    0x01
#define UART16550_LSR_OE    0x02
#define UART16550_LSR_PE    0x04
#define UART16550_LSR_FE    0x08
#define UART16550_LSR_BI    0x10
#define UART16550_LSR_THRE  0x20
#define UART16550_LSR_TEMT  0x40
#define UART16550_LSR_ERROR 0x80

#define UART16550_MCR_nDTR  0x01
#define UART16550_MCR_nRTS  0x02
#define UART16550_MCR_OUT1  0x04
#define UART16550_MCR_OUT2  0x08
#define UART16550_MCR_LOOP  0x10
#define UART16550_MCR_AFE   0x20

#define UART16550_MSR_dCTS  0x01
#define UART16550_MSR_dDSR  0x02
#define UART16550_MSR_TERI  0x04
#define UART16550_MSR_dDCD  0x08
#define UART16550_MSR_CTS   0x10
#define UART16550_MSR_DSR   0x20
#define UART16550_MSR_RI    0x40
#define UART16550_MSR_DCD   0x80


#define TTY_BREAK 3
#define TTY_EOF 4
#define TTY_BS 8
#define TTY_TAB 9
#define TTY_NL 10
#define TTY_CR 13

typedef struct serial_conf_s {
  __u32 baud;
  char eof,eol;
  __u16 unit;
  sc_poolid_t pool;
  int ref;
  /* variable data */
  int flags;
#define SERIAL_FLAGS_NONE	0x00000000UL
#define SERIAL_FLAGS_TMO	0x20000000UL
#define SERIAL_FLAGS_ECHO	0x40000000UL
#define SERIAL_FLAGS_XONXOFF	0x80000000UL
#define SERIAL_FLAGS_RTSCTS	0x01000000UL
#define SERIAL_FLAGS_TTY	0x02000000UL

  sc_ticks_t rtmo;
  sc_tmoid_t rtmoId;

  sc_msg_t rmsg;
  int rmax;
  int rcur;
  int rin,rout;                      /* rx in/out index */
  int rcnt;                          /* # bytes in rx-buffer */
  char rbuf[CNF_UART_RXBUF_SIZE];    /* rx buffer            */

  sc_msg_t tmsg;
  int tmax;
  int tcur;
  int tin,tout;                      /* tx in/out index */
  int tcnt;                          /* # bytes in tx-buffer */
  char tbuf[CNF_UART_TXBUF_SIZE];    /* tx buffer            */
} serial_conf_t;

#endif /* __TL16C2550_H__ */
