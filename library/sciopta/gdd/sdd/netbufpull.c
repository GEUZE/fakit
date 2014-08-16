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
** ID: S04147BS26                                                   **
** +Revision: 1.5 +                                                 **
** +Date: 2004/12/14 09:10:12 +                                     **
** Basic netbuf functions: sdd_netbufPull                           **
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
sdd_netbufPull (sdd_netbuf_t NEARPTR buffer, ssize_t len)
{
  if (!buffer) {
    sc_miscError (SDD_ERR_BASE + EFAULT, 0);
  }

  if (len < 0) {
    /* we have to realloc the buffer */
    sc_miscError (SDD_ERR_BASE + EINVAL, 0);
  }

  if (buffer->data + len > buffer->tail) {
    /* no more data room */
    sc_miscError (SDD_ERR_BASE + EINVAL, 0);
  }

  buffer->data += len;
}

