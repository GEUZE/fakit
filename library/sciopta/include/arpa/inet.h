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
** ID: S03070CL52                                                   **
** +Revision: 1.4 +                                                 **
** +Date: 2003/08/05 09:05:50 +                                     **
** IP translation functions                                         **
**********************************************************************
EOC*/

#ifndef	_ARPA_INET_H
#define	_ARPA_INET_H	1

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/types.h>
#include <netinet/in.h>

  int inet_aton (const char *cp, struct in_addr *inp);
  in_addr_t inet_addr (const char *cp);
  in_addr_t inet_network (const char *cp);
  char *inet_ntoa (struct in_addr in);
  struct in_addr inet_makeaddr (int net, int host);
  in_addr_t inet_lnaof (struct in_addr in);
  in_addr_t inet_netof (struct in_addr in);

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: inet.h,v $
** Revision 1.4  2003/08/05 09:05:50  bastian
** fixed description
**
** Revision 1.3  2003/06/23 11:29:46  christian
** Included now everything i need for this header file, so it can work
** standalone.
**
** Revision 1.2  2003/03/11 14:58:30  bastian
** added header
**
** Revision 1.1  2002/10/07 15:32:32  christian
** Add include/arpa/inet.h include/netinet/in.h and src/inet.c
**
**
**/
