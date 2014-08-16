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
** ID: S04147BS24                                                   **
** +Revision: 1.7 +                                                 **
** +Date: 2006/05/10 09:04:19 +                                     **
** Basic netbuf functions: sdd_netbufDup                            **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <errno.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#include <string.h> /* memcpy */

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_obj_t object;
};

sdd_netbuf_t NEARPTR 
sdd_netbufDup(const sdd_netbuf_t NEARPTR buffer,sc_poolid_t plid,sc_ticks_t tmo)
{
  sdd_netbuf_t NEARPTR tmp;
  sc_bufsize_t size;

  if (!buffer) {
    sc_miscError (SDD_ERR_BASE + EFAULT, 0);
  }

  size = sc_msgSizeGet((sc_msgptr_t)&buffer);

  if ( plid == SC_ILLEGAL_POOLID ){
    plid = sc_msgPoolIdGet((sc_msgptr_t)&buffer);
  }

  tmp = (sdd_netbuf_t NEARPTR)sc_msgAlloc(size,SDD_NETBUF,plid,tmo);

  if ( tmp ){
    (void)memcpy(tmp,buffer,size);
  }
  return tmp;
}

