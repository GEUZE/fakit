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
** (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
**                                                                  **
**********************************************************************
** ID:  S02010CL7                                                   **
** +Revision: 1.4 +                                                **
** +Date: 2003/03/18 15:04:26 +                                     **
** ppp configuration definitions                                    ** 
**********************************************************************
EOC*/

#ifndef _GDD_PPP_CONF_H_
#define _GDD_PPP_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sciopta.h>

typedef struct ppp_process_s {
  sc_msgid_t id;
  char name [SC_PROC_NAME_SIZE + 1];
  void (*entry) (void);
  sc_bufsize_t stacksize;
  sc_ticks_t slice;
  sc_prio_t prio;
  int state;
  sc_poolid_t plid;
} ppp_process_t;

#ifdef __cplusplus
}
#endif

#endif

/*
** $Log: pppconf.h,v $
** Revision 1.4  2003/03/18 15:04:26  christian
** Cleaned.
**
** Revision 1.3  2003/03/14 14:51:01  bastian
** fixes for ADS
**
** Revision 1.2  2002/08/13 15:21:07  christian
** A cleaned up new version with a pppd
**
** Revision 1.1  2002/08/09 15:14:36  christian
** Every Protocol Layer will be started from pppconf. If it is a statical
** system the neccessary processes musst be started in the scipota.con.
**
** 
**/
