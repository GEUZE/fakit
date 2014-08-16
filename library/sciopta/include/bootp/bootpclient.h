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
** ID: S03175CL2                                                    **
** +Revision: 1.5 +                                                **
** +Date: 2005/07/01 12:28:31 +                                     **
** BOOTP definition                                                 **
**********************************************************************
EOC*/

#ifndef _BOOTPLIENT_H
#define _BOOTPLIENT_H	1

#ifdef __cplusplus
extern "C" {
#endif

#include <bootp/bootp.h>

  /** 
   * Send a bootp request.
   * \param[in]  etherdev   Bootp over which ethernet device
   * \param[in]  clientIp   Your IP if known else 0.0.0.0
   * \param[in]  serverIp   Servers IP if known else 255.255.255.255
   * \param[in]  serverName Optional server name, NULL terminated
   * \param[in]  bootFile   Optional, Logical boot file name, NULL terminated
   * \return                NULL on error or the reply from bootp server see
   *                        <bootp/bootp.h> for the structure of the reply.
   */
  bootp_reply_t NEARPTR bootp_request (const char *etherdev, 
				       const char *clientIp,
				       const char *serverIp,
				       const char *serverName,
				       const char *bootFile);

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: bootpclient.h,v $
** Revision 1.5  2005/07/01 12:28:31  christian
** Merged SCIOPTA_1_7_2 to MAIN
**
** Revision 1.4.4.1  2005/05/25 09:09:41  bastian
** fix for tasking
**
** Revision 1.4  2004/07/27 12:04:13  christian
** Release bugfix: Remove refresh IP.
**
** Revision 1.3  2004/07/26 13:27:16  christian
** Renamed ips_bootpXxxYyy to bootp_xxxYyy.
**
** Revision 1.2  2004/05/26 06:05:23  christian
** Header files documented and spell checks.
**
** Revision 1.1  2003/07/01 13:03:17  christian
** Added bootpclient and dhcpclient header files.
**
**
**/
