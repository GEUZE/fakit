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
** ID: S02253CL1                                                    **
** +Revision: 1.9 +                                                 **
** +Date: 2005/07/01 12:28:32 +                                     **
** IPS protocol registry.                                           **
**********************************************************************
EOC*/

#ifndef _IPS_LINK_H_
#define _IPS_LINK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sciopta.h>
#include <ips/addr.h>
#include <ips/link.msg>

  /**
   * Link process declaration
   */
  void link_process (int noOfProtos);
  /**
   * Register a protocol family like ipv4, ipv6.
   * \param[in]  protocol Protocol message, will be set to NULL on execution 
   * \return              -1 on error
   */
  int ips_linkProtocolRegister (ips_protocol_t NEARPTR NEARPTR protocol);
  /**
   * Unregister a previously registered protocol family.
   * \param[in]  protocol Protocol reference
   * \return              -1 on error
   */
  int ips_linkProtocolUnregister (ips_protocol_t NEARPTR protocol);

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: link.h,v $
** Revision 1.9  2005/07/01 12:28:32  christian
** Merged SCIOPTA_1_7_2 to MAIN
**
** Revision 1.8.2.1  2005/06/10 06:25:59  bastian
** fixes for Tasking M16C
**
** Revision 1.8  2005/01/20 14:23:06  christian
** Replace list_t with array_t for protocol holding, because this is more or
** less statical.
**
** Revision 1.7  2004/05/26 06:05:23  christian
** Header files documented and spell checks.
**
** Revision 1.6  2003/03/18 15:04:26  christian
** Cleaned.
**
** Revision 1.5  2002/09/26 15:13:03  christian
** intermediate.
**
** Revision 1.4  2002/09/17 09:56:46  christian
** Bios interface has changed
**
** Revision 1.3  2002/09/12 13:17:34  christian
** Bugfix
**
** Revision 1.2  2002/09/11 12:24:14  christian
** Moved the protocol types over to device/include/ips/device.h.
**
** Revision 1.1.1.1  2002/09/10 14:51:20  christian
** link layer
**
**
**
**/
