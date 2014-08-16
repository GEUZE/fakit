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
** ID: S03070CL36                                                   **
** +Revision: 1.15 +                                                 **
** +Date: 2008/01/16 13:02:00 +                                     **
** inet_aton inet_ntoa                                              **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>
#include <ossys/libsc.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int
inet_aton (const char *cp, struct in_addr *inp)
{
  const char *p;
  __u8 *ip;

  p = cp;
  ip = (__u8 *) inp;
  ip[0] = (__u8)dectoi (p);
  
  p = strstr (p, ".");
  if (!p) {
    sc_miscErrnoSet (EINVAL);
    return 0;
  }
  
  ip[1] = (__u8)dectoi (++p);
  p = strstr (p, ".");
  if (!p) {
    sc_miscErrnoSet (EINVAL);
    return 0;
  }

  ip[2] = (__u8)dectoi (++p);
  p = strstr (p, ".");
  if (!p) {
    sc_miscErrnoSet (EINVAL);
    return 0;
  }

  ip[3] = (__u8)dectoi (++p);

  return 1;
}

in_addr_t inet_addr (const char *cp);
in_addr_t inet_network (const char *cp);

char *
inet_ntoa (struct in_addr in)
{
  fd_tab_t NEARPTR fd_tab;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    int i,pos = 0;
    __u8 *ip = (__u8 *) &in.s_addr;

    for (i = 0; i < 4; i++) {
      pos += sc_miscInt2Str (*ip++, &fd_tab->ipNoNDots[pos], 3);
      if (i < 3) {
        fd_tab->ipNoNDots[pos++] = '.';
      }
    }
    fd_tab->ipNoNDots[pos] = '\0';
    return fd_tab->ipNoNDots;
  }
  return NULL;
}

struct in_addr inet_makeaddr (int net, int host);
in_addr_t inet_lnaof (struct in_addr in);
in_addr_t inet_netof (struct in_addr in);

