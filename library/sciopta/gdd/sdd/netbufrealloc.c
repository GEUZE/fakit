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
** ID: S04147BS29                                                   **
** +Revision: 1.5 +                                                 **
** +Date: 2004/12/14 09:10:12 +                                     **
** Basic netbuf functions: sdd_netbufRealloc                        **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <errno.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_obj_t object;
};

void
sdd_netbufRealloc (sdd_netbuf_t NEARPTR NEARPTR buffer, ssize_t newDataRoom,
		      sc_ticks_t tmo)
{
  sdd_netbuf_t NEARPTR tmp;

  if (!(*buffer)) {
    sc_miscError (SDD_ERR_BASE + EFAULT, 0);
  }

  tmp =
    sdd_netbufCopy ((*buffer), 0, newDataRoom,
		       sc_msgPoolIdGet ((sc_msgptr_t) buffer), tmo);

  if (tmp) {
    sc_msgFree ((sc_msgptr_t) buffer);
    sdd_netbufTrim (tmp, SDD_NET_DATA_SIZE (tmp) + newDataRoom);
    *buffer = tmp;
  }
  else {
    /* error ?! */
  }
}

