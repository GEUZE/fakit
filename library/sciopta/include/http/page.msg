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
** ID: S03350CL1 
** +Revision: 1.6 + 
** +Date: 2005/01/19 11:20:16 +
** PAGE message interface
**********************************************************************
EOC*/

#ifndef _HTTP_PAGE_MSG_
#define _HTTP_PAGE_MSG_ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/defines.h>
#include <sdd/sdd.msg>

  /* start the page process - synchronisation */
#define HTTP_PAGE_START			(HTTP_MSG_BASE + 2)
  /* authentication type */
#define HTTP_PAGE_AUTH_TYPE		(HTTP_MSG_BASE + 3)
  /* content length of the upload */
#define HTTP_PAGE_CONTENT_LENGTH	(HTTP_MSG_BASE + 4)
  /* content type */
#define HTTP_PAGE_CONTENT_TYPE		(HTTP_MSG_BASE + 5)
  /* cgi version */
#define HTTP_PAGE_GATEWAY_INTERFACE	(HTTP_MSG_BASE + 6)
  /* path without protocol, host, port and script path */
#define HTTP_PAGE_PATH_INFO		(HTTP_MSG_BASE + 7)
  /* all behind ? in the uri */
#define HTTP_PAGE_QUERY_STRING		(HTTP_MSG_BASE + 8)
  /* the ip address of the client */
#define HTTP_PAGE_REMOTE_ADDR		(HTTP_MSG_BASE + 9)
  /* request method - GET, POST, HEAD, OPTIONS, ... */
#define HTTP_PAGE_REQUEST_METHOD	(HTTP_MSG_BASE + 10)
  /* absolute path to the script */
#define HTTP_PAGE_SCRIPT_NAME		(HTTP_MSG_BASE + 11)
  /* name of the server */
#define HTTP_PAGE_SERVER_NAME		(HTTP_MSG_BASE + 12)
  /* listen port */
#define HTTP_PAGE_SERVER_PORT		(HTTP_MSG_BASE + 13)
  /* requested protocol - http, webdav, ... */
#define HTTP_PAGE_SERVER_PROTOCOL	(HTTP_MSG_BASE + 14)
  /* servers software */
#define HTTP_PAGE_SERVER_SOFTWARE	(HTTP_MSG_BASE + 15)
  /* http entries */
#define HTTP_PAGE_HTTP_ENTRY		(HTTP_MSG_BASE + 16)
  /* entier http header - not yet supported */
#define HTTP_PAGE_HTTP_HEADER		(HTTP_MSG_BASE + 17)
  /**
   * A message holder for various data types which are specified in the CGI 1.2
   * draft.
   * \param[in]  id         Message id
   * \param[in]  data       Null terminated string with the requested data 
   */
  typedef struct http_pageData_s {
    sc_msgid_t id;
    char data[1];
  } http_pageData_t;

#define HTTP_PAGE_REPLY			(HTTP_MSG_BASE + 18)
  /**
   * The reply to message defined above. 
   * \param[in]  id         Message id
   * \param[in]  more       == message defined above or 0 if no more infos 
   * \param[in]  data       additional data to get next information. 
   */
  typedef struct http_pageReply {
    sc_msgid_t id;
    sc_msgid_t more;
    char data[1];
  } http_pageReply_t;

#ifdef __cplusplus
}
#endif
#endif

