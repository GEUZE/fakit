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
** ID: S02253CL2                                                    **
** +Revision: 1.12 +                                                 **
** +Date: 2005/05/19 07:51:20 +                                     **
** IPS protocol registry message interface.                         **
**********************************************************************
EOC*/

#ifndef _IPS_LINK_MSG_
#define _IPS_LINK_MSG_

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/defines.h>
#include <sdd/sdd.msg> 
  
  typedef struct ips_protocol_s {
    sdd_obj_t object;
    __u32 type;
  } ips_protocol_t;

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: link.msg,v $
** Revision 1.12  2005/05/19 07:51:20  christian
** Changed net open api slightly.
**
** Revision 1.11  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 1.10  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 1.9  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 1.8  2003/06/12 12:41:20  christian
** Removed deviceMother and deviceHandle form Protocol and netBuffer. Made it
** too complicated. Can readd it if requested.
**
** Revision 1.7  2003/03/18 15:04:26  christian
** Cleaned.
**
** Revision 1.6  2003/03/18 10:50:22  christian
** Cleaned.
**
** Revision 1.5  2002/11/01 12:50:13  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 1.4  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 1.3  2002/09/16 12:24:13  christian
** Rename sdd_handle_t to sdd_obj_t (prepare for future changes)
**
** Revision 1.2  2002/09/11 12:25:37  christian
** It is now possible to add a protocol to one or more specific devices.
**
** Revision 1.1.1.1  2002/09/10 14:51:20  christian
** link layer
**
**
**
**/
