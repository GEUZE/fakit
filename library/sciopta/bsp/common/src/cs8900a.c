
#define _CS8900A_C_
#include <sciopta.h>
#include "config.h"
#include <cs8900a.h>
#include <cs8900a_p.h>

#define USE_BUF_EVENT 1

#if CS8900A_IO_MODE == 1

int cs8900a_init(char *mac)
{
  __u16 *p;
  __u16 val;
  unsigned int i;
  /* check for chip */
  if ( CS8900A_READ(PP_MANID) != 0x630e ){
    return -1;
  }

  /* reset chip */
  CS8900A_WRITE(PP_SELF_CTL,POWER_ON_RESET);

  do{
    sc_sleep(sc_tickMs2Tick(10));
    val = CS8900A_READ(PP_SELF_CTL);
  }while( (val & 0x1f) != 0x15 || (val & POWER_ON_RESET) );

  CS8900A_SET_PP_NOINC(PP_SELFST);
  while( !(CS8900A_READ_PP_DATA() & INIT_DONE) ){
    sc_sleep(sc_tickMs2Tick(10));
  }
  
  CS8900A_WRITE(PP_LINE_CTL, TENBASET);

  CS8900A_WRITE(PP_RX_CTL, RX_OK_ACCEPT|RX_BROADCAST_ACCEPT|RX_IA_ACCEPT);

  p = (__u16 *)mac;
  CS8900A_SET_PP(PP_IA);
  CS8900A_WRITE_PP_DATA(*p); ++p;
  CS8900A_WRITE_PP_DATA(*p); ++p;
  CS8900A_WRITE_PP_DATA(*p);

  CS8900A_WRITE(PP_ISAINT,0);

#if USE_BUF_EVENT == 1
  CS8900A_WRITE(PP_BUF_CFG, READY_FOR_TX_ENBL);
#endif
//->  CS8900A_WRITE(PP_TX_CFG, TX_OK_ENBL);

  CS8900A_WRITE(PP_RX_CFG, RX_OK_ENBL);

  CS8900A_WRITE(PP_LINE_CTL, SERIAL_RX_ON|SERIAL_TX_ON);

  for( i = 400; i ; --i ){
    if ( (CS8900A_READ(PP_LINEST) & LINK_OK) ){
      return  1;
    }
    sc_sleep(sc_tickMs2Tick(10));
  }
  return 0;
}

void cs8900a_ei()
{
  CS8900A_WRITE(PP_BUS_CTL, ENABLE_IRQ);
}

void cs8900a_di()
{
  CS8900A_WRITE(PP_BUS_CTL, 0);
}

int cs8900a_alloc(int length)
{
  CS8900A_WRITE_TX_CMD(0);
  CS8900A_WRITE_TX_LENGTH(length);
#if USE_BUF_EVENT == 1
  return (CS8900A_READ(PP_BUSST) & READY_FOR_TX_NOW);
#else
  while( !(CS8900A_READ(PP_BUSST) & READY_FOR_TX_NOW) ){
    /* empty */
  }
  return 1;
#endif
}
#endif
