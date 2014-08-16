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
** ID: S04147BS30                                                   **
** +Revision: 1.6 +                                                 **
** +Date: 2008/01/16 13:02:00 +                                     **
** Basic netbuf functions: sdd_netbufTrim                           **
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
sdd_netbufTrim (sdd_netbuf_t NEARPTR buffer, size_t len)
{
  if (!buffer) {
    sc_miscError (SDD_ERR_BASE + EFAULT, 0);
  }

  if (buffer->data + len > buffer->end) {
    /* no more data room */
    PRINTF3 ("buffer->data: %d, buffer->end: %d, size: %d\n",
	     buffer->data, buffer->end, buffer->size);
    PRINTF1 ("requested len: %d\n", len);
    PRINTF2 ("sender: %x, receiver: %x\n",
	     sc_msgSndGet ((sc_msgptr_t) &buffer),
	     sc_msgOwnerGet ((sc_msgptr_t) &buffer));
    sc_miscError (SDD_ERR_BASE + EINVAL, 0);
  }

  buffer->tail = buffer->data + len;
}

