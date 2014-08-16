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
** ID: S02038CL2                                                    **
** +Revision: 1.12 +                                                 **
** +Date: 2006/03/16 15:57:35 +                                     **
** Server interface (integratable into shell)                       **
**********************************************************************
EOC*/

#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/defines.h>
#include <ips/addr.h>

  typedef struct http_config_s {
    sc_msgid_t id;
    ips_addr_t addr;
    __u16 port;
    char *documentRoot;
    char *indexHtml;
    int httpURIMax;
    int httpREQMax;
    __uint connMax;
  } http_config_t;

  void http_daemon (const http_config_t *defaultConfig);

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: server.h,v $
** Revision 1.12  2006/03/16 15:57:35  bastian
** config const
**
** Revision 1.11  2006/03/16 15:55:43  bastian
** added connMax option
**
** Revision 1.10  2005/10/19 12:13:15  christian
** F #43: configurable name for index html file.
** F #44: Return IPS_ACK to sender not to page pid.
** F #45: Adjust page pid on every request.
**
** Revision 1.9  2004/09/24 13:54:44  christian
** Rename config with defaultConfig. Will extent configuration in future.
**
** Revision 1.8  2004/09/02 13:50:59  christian
** The idea is working. Can now access all specified CGI Meta variables.
** Not all Meta variables do have contents at the moment.
**
** Revision 1.7  2004/09/01 14:09:53  christian
** Webserver is now more RFC compilant.
**
** Revision 1.6  2004/08/11 05:44:59  christian
** Http next generation.
**
** Revision 1.5  2004/07/23 10:12:52  christian
** Replaced httpd with http_daemon like telnet.
**
** Revision 1.4  2004/05/12 06:05:32  christian
** Structured the http project. Did comment all functions directly in the
** header files.
**
** Revision 1.3  2004/04/16 10:27:58  christian
** There is now a printf function to make it very easy to program CGIs.
**
** Revision 1.2  2003/03/18 15:04:25  christian
** Cleaned.
**
** Revision 1.1  2002/11/01 09:07:31  christian
** Add server header file.
**
** Revision 1.7  2002/10/29 14:27:06  christian
** Fixed various bugs. Better error handling.
**
** Revision 1.6  2002/10/25 14:02:56  christian
** Port the httpd example to the new IPS.
**
** Revision 1.5  2002/02/21 12:33:24  christian
** Simpler commends with the new kprintf.
**
** Revision 1.4  2002/02/19 12:54:45  christian
** Changed the http hook (made it dynamic).
**
** Revision 1.3  2002/02/18 16:11:55  christian
** Fixes.
**
** Revision 1.2  2002/02/18 15:34:54  christian
** Successfully started with http parser.
**
** Revision 1.1.1.1  2002/02/07 13:04:00  christian
** first revision
**
**
**
**/
