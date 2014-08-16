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
** ID: S04147BS21                                                   **
** +Revision: 1.6 +                                                 **
** +Date: 2006/03/17 06:35:54 +                                     **
** Basic netbuf functions: sdd_netbufCopy                           **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#include <errno.h>
#include <string.h> /* memcpy */

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
};

sdd_netbuf_t NEARPTR 
sdd_netbufCopy (const sdd_netbuf_t NEARPTR buffer, 
		ssize_t newHeadRoom,
		ssize_t newTailRoom, 
		sc_poolid_t plid, 
		sc_ticks_t tmo)
{
  size_t size;
  sdd_netbuf_t NEARPTR tmp;

  if (!buffer) {
    sc_miscError (SDD_ERR_BASE + EFAULT, 0);
  }
  size = SDD_NET_DATA_SIZE (buffer);
  tmp = sdd_netbufAlloc (newHeadRoom, size, newTailRoom, plid, tmo);
  if (tmp) {
    (void)memcpy (tmp, buffer, offsetof (sdd_netbuf_t, pkttype));
    (void)memcpy (SDD_NET_DATA (tmp), SDD_NET_DATA (buffer), size);
  }
  return tmp;
}

