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
** ID: S04042CL1                                                    **
** +Revision: 1.8 +                                                **
** +Date: 2008/02/18 10:09:21 +                                     **
** IGMP header definition                                           **
**********************************************************************
EOC*/

#ifndef _IPS_IGMP_H_
#define _IPS_IGMP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/types.h>
#include <ips/link.msg>

  /**
   * IGMP process definition
   */ 
  void igmp_process (unsigned int clientMax);

/** IGMP Types */

/** IGMP Header */
  typedef struct ips_igmpHeader_s {
    __u8 type;
    __u8 max_respTime;
    __u16 chksum;
    __u32 group;
  } ips_igmpHeader_t;

  typedef struct ips_igmpV3QueryHeader_s {
    __u8 type;
    __u8 max_respTime;
    __u16 chksum;
    __u32 group;
    __u8 qrv;
    __u8 qqic;
    __u16 noSrc;
  } ips_igmpV3QueryHeader_t;

  typedef struct ips_igmpV3ReportHeader_s {
    __u8 type;
    __u8 spare1;
    __u16 chksum;
    __u16 spare2;
    __u16 noGroups;
  } ips_igmpV3ReportHeader_t;

  typedef struct ips_igmpV3Record_s {
    __u8 type;
#define IGMP_RECORD_MODE_IS_INCLUDE        1
#define IGMP_RECORD_MODE_IS_EXCLUDE        2
#define IGMP_RECORD_MODE_CHANGE_TO_INCLUDE 3
#define IGMP_RECORD_MODE_CHANGE_TO_EXCLUDE 4
#define IGMP_RECORD_ALLOW_NEW_SOURCES      5
#define IGMP_RECORD_BLOCK_OLD_SOURCES      6
    __u8 auxData;
    __u16 noSrc;
    __u32 mcAddr;
  } ips_igmpV3Record_t;


#define IGMP_TYPE_QUERY    0x11
#define IGMP_TYPE_v2REPORT 0x16
#define IGMP_TYPE_LEAVE    0x17

#define IGMP_TYPE_v1REPORT 0x12
#define IGMP_TYPE_v3REPORT 0x22

/** MIB structure */

#ifdef __cplusplus
}
#endif
#endif
/*
 * $Log: igmp.h,v $
 * Revision 1.8  2008/02/18 10:09:21  bastian
 * Fixed rules
 * Fixed type
 *
 * Revision 1.7  2008/02/15 15:17:58  bastian
 * we
 *
 * Revision 1.6  2008/02/06 13:40:14  bastian
 * fixed memory leak
 *
 * Revision 1.5  2008/01/27 08:11:21  bastian
 * support for query added
 *
 * Revision 1.4  2008/01/24 00:20:34  bastian
 * first working MC version
 *
 * Revision 1.3  2004/12/09 09:46:30  christian
 * Removed SDD_OBJ_INFO, will replace this with more specific messages.
 *
 * Revision 1.2  2004/04/23 06:13:56  christian
 * Did rename to make source more readable.
 *
 * Revision 1.1  2004/02/11 15:05:39  christian
 * Started with the IGMP protocol.
 *
 *
 */
