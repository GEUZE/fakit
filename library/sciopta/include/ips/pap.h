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
** ID:  S02010CL5                                                   **
** +Revision: 1.7 +                                                **
** +Date: 2004/07/01 10:18:07 +                                     **
** password authentication protocol                                 ** 
**********************************************************************
EOC*/


#ifndef _GDD_PAP_H_
#define _GDD_PAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sciopta.h>
#include <ips/ppp.msg>
#include <ips/ppp.h>
  
#define PAP_PROT	0xc023  

  /* PPP_CONFIG/PPP_CONFIG_REPLY */
  typedef struct pap_config_s {
    ppp_config_t config;
    int max_authreq;
    int restart;
    int timeout;
  } pap_config_t; 
  

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: pap.h,v $
** Revision 1.7  2004/07/01 10:18:07  bastian
** fixed castings
** fixed LOGD handling
**
** Revision 1.6  2003/06/23 11:28:29  christian
** Added missing header files with in header files.
**
** Revision 1.5  2003/03/18 15:04:26  christian
** Cleaned.
**
** Revision 1.4  2003/03/14 14:51:01  bastian
** fixes for ADS
**
** Revision 1.3  2003/03/04 08:19:31  christian
** A clean up and down of the stack.
**
** Revision 1.2  2002/12/06 12:21:38  christian
** Wrong includes and wrong paths.
**
** Revision 1.1  2002/11/18 15:49:42  christian
** Started with pap.
** Linux like log.
**
**
**/
