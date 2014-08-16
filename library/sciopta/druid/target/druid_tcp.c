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
** ID: S05026CL1                                                    **
** +Revision: 1.3.20.1 +                                                 **
** +Date: 2009/10/09 13:16:16 +                                     **
** Druid device implementation                                      **
**********************************************************************
EOC*/

#include "sconf.h"
#include "config.h"
#include <sciopta.h>
#include <ossys/errtxt.h>
#include <druid/device.h>
#include <sdd/sdd.msg>
#include <sdd/sdd.h>
#include <ips/addr.h>
#include <netinet/in.h>
#include <ips/connect.msg>
#include <ips/connect.h>

#define RET_ACK 1

union sc_msg {
  sc_msgid_t id;
  sdd_error_t error;
  sdd_baseMessage_t base;
  sdd_netbuf_t netbuf;
};

static druid_isr_f rxisr;

typedef struct druid_stream_s {
  sdd_obj_t *listener;
  sdd_obj_t *tcp;
  sc_msg_t rxbuf;
  char *dp;
  int rxCnt;
} druid_stream_t;

druid_stream_t druid_stream;


int druid_getc()
{
  static const sc_msgid_t select[3] = { SDD_NET_RECEIVE, SDD_ERROR, 0 };
  ips_addr_t dstAddr;
  __u16 dstPort;
  int c;
  int on;
  sc_msg_t msg;

nextAccept:
  if ( !druid_stream.tcp ){

    druid_stream.tcp = ips_accept (druid_stream.listener, &dstAddr, &dstPort);
    
    if ( !druid_stream.tcp ) {
      ips_close(&druid_stream.listener);
      return -1;
    }
//->    kprintf(0,"New conn\n");
    /* set async queue */
    on = 1;
    if (ips_setOption (druid_stream.tcp, SOL_SOCKET, SO_SC_ASYNC, &on, 
	               sizeof (on)) == -1) 
    {
      kprintf (1, "Can not switch to async\n");
      sc_miscError(2,sc_miscErrnoGet());
    }
#if RET_ACK == 1
    /* Setting SO_SC_RET_ACK must be _after_ SO_SC_ASYNC ! */
    on = 1;
    if (ips_setOption (druid_stream.tcp, SOL_SOCKET, SO_SC_RET_ACK, &on, sizeof (on)) ==
	-1) {
      kprintf (1, "Can not switch to async\n");
      sc_miscError(2,sc_miscErrnoGet());
    }
#endif
  }
  while ( druid_stream.rxCnt == 0 ){
    msg = sc_msgRx (SC_ENDLESS_TMO, select, SC_MSGRX_MSGID);
    switch (msg->id) {
    case SDD_ERROR:
      if (msg->base.handle == druid_stream.tcp->base.handle) {
	/* close accepted connection */
	ips_close (&druid_stream.tcp);	
	goto nextAccept;
      }
      sc_msgFree (&msg);
      break;
    case SDD_NET_RECEIVE:
      if (msg->base.handle != druid_stream.tcp->base.handle) {
	sc_msgFree (&msg);
      } else if (msg->netbuf.base.error == 0 && 
		 SDD_NET_DATA_SIZE (&msg->netbuf) > 0) {
	/* every received netbuf must be acked */
	ips_ack (druid_stream.tcp, &msg->netbuf);
	druid_stream.rxbuf = msg;
	druid_stream.rxCnt = SDD_NET_DATA_SIZE (&msg->netbuf);
	druid_stream.dp = (char *)SDD_NET_DATA(&msg->netbuf);
//->	kprintf(0,"<- %d\n",druid_stream.rxCnt);
      } else {
	ips_close (&druid_stream.tcp);	
	goto nextAccept;
      }
      break;
    default:
      break;	
    }
  }   
  
  c = *druid_stream.dp++;
  --druid_stream.rxCnt;
  if ( druid_stream.rxCnt == 0 ){
    sc_msgFree(&druid_stream.rxbuf);
  }
  return c;
}

/**
 * Druid interrupt process.
 */
void 
druid_process (ips_addr_t *source, __u16 port)
{
  sc_sleep(100);

  for(;;){
    if ( !druid_stream.listener ){
      /* setup a listener socket */
      druid_stream.listener = ips_open ("ipv4", "tcp", 0);
      
      /* setup a listener server, accept one by one */
      if (ips_bind (druid_stream.listener, source, htons(port)) == -1) {
	sc_miscError (1, 0);
      }
      
      if (ips_listen (druid_stream.listener, 1) == -1) {
	sc_miscError (2, 0);
      }
    }

    rxisr(druid_getc());
  }
}


/**
 * Druid time get
 */
static __u32 druid_time = 0;

int 
druid_timeUsPerTick (void)
{
  int tickLen = sc_tickLength (0);
  
  if (tickLen) {
    return tickLen;
  }
  else {
    return CNF_TICKLENGTH;
  }
}

void 
druid_timeReset (void)
{
  druid_time = sc_tickGet ();
}

__u32
druid_timeGet (void)
{
  /* simplest impl is using sc_tickGet () */
  return sc_tickGet () - druid_time; 
}
/*
****************************************************************************
** druid internal stuff, leave untouched !
****************************************************************************
*/

void druid_init (druid_isr_f druid_rxisr, druid_isr_f druid_txisr)
{
  (void)druid_txisr; /* not yet used */
  
  memset((char *)&druid_stream,0,sizeof(druid_stream_t));

  rxisr = druid_rxisr;
}


void druid_write(const __u8 *buf, int len)
{
  if ( druid_stream.tcp && buf && len ){
#if RET_ACK == 1
    static const sc_msgid_t selAck[3] = { IPS_ACK, SDD_ERROR, 0 };
    sc_msg_t msg;
#endif
    sdd_netbuf_t *nbuf;
    int xlen;
//->    kprintf(0,"Write %d\n",len);
    do{
      xlen = len > 1460 ? 1460 : len;      
      nbuf = sdd_netbufAlloc (68, xlen, 0,
			    SC_DEFAULT_POOL,
			    SC_FATAL_IF_TMO);
      memcpy(SDD_NET_DATA(nbuf),buf,len);
#if RET_ACK == 1
      ips_send(druid_stream.tcp,&nbuf);
      msg = sc_msgRx(SC_ENDLESS_TMO,selAck,SC_MSGRX_MSGID);

      if ( msg->id == SDD_ERROR ){
	sc_msgFree(&msg);
	druid_stream.tcp = NULL;
	return;
      } 
      sc_msgFree(&msg);
#endif
      len -= xlen;
    }while ( len );    
  }
}


int druid_read(void)
{
  return (int) druid_getc();          // command
}

#if SC_ERR_HOOK == 1
int tcpErrorHook(sc_errcode_t err,sc_extra_t extra,int user,const sc_pcb_t *pcb)
{
  if ( druid_stream.tcp ){
    if (user) {
      druid_errorHook (err, extra, user, pcb);
      return 1;
    }
    else {
      return druid_errorHook (err, extra, user, pcb);
    }
  } else {
    kprintf(9,"%s-Error\n"
	    " %08lx(%s,line %d in %s)\n"
	    " pcb = %08lx err = %08lx extra = %08lx\n",
	    user ? "User" : "System",
	    (__uint)pcb>1 ? pcb->pid:0,
	    (__uint)pcb>1 ? pcb->name:"xx",
	    (__uint)pcb>1 ?pcb->cline:0,
	    (__uint)pcb>1 ?pcb->cfile:"xx",
	    pcb,
	    err,
	    extra);
    if ( user != 1 && 
	 ((err>>12)&0xfff) <= SC_MAXERR &&
	 (err>>24) <= SC_MAXFUNC )
    {	   
      kprintf(0,"Function: %s\nError: %s\n",
	      func_txt[err>>24],
	      err_txt[(err>>12)&0xfff]);
    }
    kprintf(0,"<stopped>\n");
    return 0;
  }
}
#endif
