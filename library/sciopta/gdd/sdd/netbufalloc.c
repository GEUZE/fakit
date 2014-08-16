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
** ID: S04147BS20                                                   **
** +Revision: 1.5 +                                                 **
** +Date: 2004/12/10 07:28:58 +                                     **
** Basic netbuf functions: sdd_netbufAlloc                          **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#ifdef SC_DEBUG
#include <string.h> /* memset */
#endif

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
};

sdd_netbuf_t NEARPTR 
sdd_netbufAlloc (size_t head, size_t data, size_t tail,
		    sc_poolid_t plid, sc_ticks_t tmo)
{
  sdd_netbuf_t NEARPTR tmp = NEAR_NULL;

  PRINTF ("sdd_netbufAlloc\n");
  tmp = (sdd_netbuf_t NEARPTR) sc_msgAlloc(sizeof (sdd_netbuf_t)+head+data+tail,
					  SDD_NETBUF,
					  plid, 
					  tmo);

  if (tmp) {
#ifdef SC_DEBUG
    memset (tmp, 0xe, sizeof (sdd_netbuf_t) + head + data + tail);
#endif
    tmp->base.error = 0;
    tmp->base.handle = NEAR_NULL;
    tmp->doBeforeSend = NULL;
    tmp->returnTo = SC_ILLEGAL_PID;
    tmp->pkttype = SDD_HOST_PKT;
    tmp->head = 0;
    tmp->cur = tmp->data = tmp->head + head;
    tmp->tail = tmp->data + data;
    tmp->end = tmp->tail + tail;
  }

  return tmp;
}

