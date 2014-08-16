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
** ID:  S02010CL3                                                   **
** +Revision: 1.17 +                                                **
** +Date: 2004/07/01 10:18:07 +                                     **
** point to point protocol defintions                               **
**********************************************************************
EOC*/


#ifndef _GDD_NET_HDLC_H_
#define _GDD_NET_HDLC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sciopta.h>


#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: hdlc.h,v $
** Revision 1.17  2004/07/01 10:18:07  bastian
** fixed castings
** fixed LOGD handling
**
** Revision 1.16  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 1.15  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 1.14  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 1.13  2003/07/30 05:59:05  christian
** sdd_netBuffer -> sdd_netbuf
**
** Revision 1.12  2003/03/18 15:04:26  christian
** Cleaned.
**
** Revision 1.11  2003/03/14 14:51:01  bastian
** fixes for ADS
**
** Revision 1.10  2002/11/20 16:14:38  christian
** In sdd_obj struct changed device to controller.
** Started with tty.c
** Fixed mtu in ppp for default interface.
**
** Revision 1.9  2002/11/01 12:50:13  christian
** renamed bios.h and bios.msg to sdd.h and sdd.msg directly on the server :-(
** Could be a problem if we need an older version than this one.
**
** Revision 1.8  2002/11/01 12:08:23  christian
** Renamed bios_ to sdd_ and BIOS_ to SDD_
** (renamed also the gdd/sdd to gdd/sdd)
**
** Revision 1.7  2002/09/06 15:03:58  christian
** Rewritten based now completly on bios net messages and methodes.
** --> using sdd_netbuf_t instead of the ugly ips_freighter_t
**
** Revision 1.6  2002/09/03 15:10:11  christian
** Adjust to the new bios concept.
**
** Revision 1.5  2002/08/23 15:03:41  christian
** Added a const char * data to the parameter list of ech ctrl protocol
** process (i.e. hdlc_process, lcp_process, ipcp_process).
**
** Revision 1.4  2002/08/21 15:35:14  christian
** On shutdown -> clear the message queue of each control protocol layer.
**
** Revision 1.3  2002/08/15 15:03:46  christian
** Work on a clean shutdown. Intermediate steps.
**
** Revision 1.2  2002/08/13 15:21:07  christian
** A cleaned up new version with a pppd
**
** Revision 1.1  2002/08/07 14:57:32  christian
** Split up ppp in a generic ppp part and a hdlc part.
** Started with a ppp configurator. Conception phase.
** Removed pppd this will be a shell part later on.
**
**
**/
