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
** ID: S04034CL1
** +Revision: 1.6 +  
** +Date: 2004/11/10 08:15:52 + 
** IOCTL tags definition 
**********************************************************************
EOC*/

#ifndef _GDD_SDD_IOCTL_H_
#define _GDD_SDD_IOCTL_H_ 1
#ifdef __cplusplus
extern "C" {
#endif

#include <sciopta.h>

#define IOCTL_TAG_END			0

/* Connector device driver tags
*/
#define IOCTL_TAG_CON_BAUD			1
#define IOCTL_TAG_CON_RCVMAX		2
#define IOCTL_TAG_CON_POOLID		3
#define IOCTL_TAG_CON_HEADERSIZE    4

/* Ethernet device driver tags
 */
#define IOCTL_TAG_ETH_PROMISCUOS 	1
  typedef int sdd_ioctlTagEthPromiscuos_t;

#define IOCTL_TAG_ETH_HWADDR 		2
  typedef struct sdd_ioctlTagEthHwaddr_s {
    __u8 addr[6];
  } sdd_ioctlTagEthHwaddr_t;

#define IOCTL_TAG_ETH_ADD_HWADDR_FILTER	3
  typedef struct sdd_ioctlTagEthAddHwaddrFilter_s {
    __u8 addr[6];
  } sdd_ioctlTagEthAddHwaddrFilter_t;

#define IOCTL_TAG_ETH_RM_HWADDR_FILTER 	4
  typedef struct sdd_ioctlTagEthRmHwaddrFilter_s {
    __u8 addr[6];
  } sdd_ioctlTagEthRmHwaddrFilter_t;

/* HDLC device driver tags
 */
#define IOCTL_TAG_HDLC_ASYNCMAP		1
  typedef int sdd_ioctlTagHdlcAsyncmap_t;

#define	IOCTL_TAG_HDLC_MRU		2
  typedef int sdd_ioctlTagHdlcMru_t;

#define	IOCTL_TAG_HDLC_MTU		3
  typedef int sdd_ioctlTagHdlcMtu_t;

#define IOCTL_TAG_HDLC_XACCM		4 	
  typedef int sdd_ioctlTagHdlcXaccm_t;

#define IOCTL_TAG_HDLC_IDLE		5
  typedef struct sdd_ioctlTagHdlcIdle {
    time_t xmitIdle;            /* time since last packet sent */
    time_t recvIdle;            /* time since last packet received */
  } sdd_ioctlTagHdlcIdle_t;

    
#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: ioctl.h,v $
** Revision 1.6  2004/11/10 08:15:52  gilbert
** new connector tag
**
** Revision 1.5  2004/02/13 13:30:13  christian
** Started with hdlc tags
**
** Revision 1.4  2004/02/13 10:54:04  christian
** An example how to name tags.
**
** Revision 1.3  2004/02/03 15:47:38  gilbert
** some tag definition for async
**
** Revision 1.2  2004/02/03 07:31:23  christian
** Fixed comment header and log tail.
**
** Revision 1.1  2004/02/03 07:26:08  christian
** Start with ioctl tagging system. This is the central tag definition file.
**
**/
