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
** ID: S02021CL2                                                    **
** +Revision: 1.22 +                                                **
** +Date: 2004/11/24 13:57:12 +                                     **
** IPS device manager                                               **
**********************************************************************
EOC*/

#ifndef _IPS_DEV_MSG_
#define _IPS_DEV_MSG_

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/defines.h>
#include <sdd/sdd.msg> 
#include <ips/addr.h>
  
  typedef struct ips_dev_s {
    sdd_obj_t object;
    unsigned int type;
    __u16 mtu, mru;
    ips_addr_t hwaddr, broadcast;
  } ips_dev_t;

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: device.msg,v $
** Revision 1.22  2004/11/24 13:57:12  bastian
** fixes for Cosmic
**
** Revision 1.21  2004/07/01 11:21:33  bastian
** added castings
**
** Revision 1.20  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 1.19  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 1.18  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 1.17  2003/07/08 12:52:19  christian
** Bugfixed the error strategy.
**
** Revision 1.16  2003/04/15 13:42:49  gilbert
** Bugfixes.
**
** Revision 1.15  2003/03/18 15:04:25  christian
** Cleaned.
**
** Revision 1.14  2003/03/18 10:50:22  christian
** Cleaned.
**
** Revision 1.13  2002/11/01 12:50:13  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 1.12  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 1.11  2002/10/23 14:58:36  christian
** Intermediate.
**
** Revision 1.10  2002/09/16 12:24:13  christian
** Rename sdd_handle_t to sdd_obj_t (prepare for future changes)
**
** Revision 1.9  2002/09/13 14:11:09  christian
** No idea what the protocol entry in the device struct is good for
**
** Revision 1.8  2002/09/11 07:47:02  christian
** Bufixes
**
** Revision 1.7  2002/09/10 12:11:12  christian
** Running net device manager
**
** Revision 1.6  2002/09/09 14:57:59  christian
** Added a net device manager.
**
** Revision 1.5  2002/05/24 14:31:09  christian
** Removed start and stop methode.
** -> replaced them with incr and decr
**
** Revision 1.4  2002/03/12 12:23:35  christian
** Added a START message id for driver construction.
**
** Revision 1.3  2002/01/23 16:04:45  christian
** Fixed message bases.
**
** Revision 1.2  2002/01/23 15:02:23  christian
** Use arp functions instead of own hacket arp packets.
**
** Revision 1.1.1.1  2002/01/21 16:45:49  christian
** first revision
**
**
**
**/
