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
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S04108CL1 
** +Revision: 1.7.24.1 + 
** +Date: 2009/02/20 09:58:06 + 
** IPS printf because many services based on ips using plain text 
** protocols. 
** This methode only works on allready established connections.
**********************************************************************
EOC*/

#ifndef _HTTP_PRINTF_H_
#define _HTTP_PRINTF_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

  /**
   * Opaque hook type for http usage
   */
  typedef struct http_s http_t;
  /**
   * To get a new instance for a connection described in a sdd_obj_t instance.
   * \param[in]  conn    Connection descriptor: For cgi the sender must be the 
   *                     webserver pid, all other entries are not needed.
   * \param[in]  pild    Poolid for memory alloction (netbufs)
   * \return             A new instance for the http function usage.
   */
  http_t NEARPTR http_new (sdd_obj_t NEARPTR  conn, sc_poolid_t plid);
  /**
   * Copy a buffer to a stream.
   * \param[in]  self    Self hook
   * \param[in]  buf     buffer
   * \param[in]  size    number of bytes to copy
   */
  void
  http_write(http_t NEARPTR self, const char *buf, size_t size);

  /**
   * For put a single character to the stream.
   * \param[in]  self    Self hook
   * \param[in]  c       Character which shall be printed
   */
  void http_putc (http_t NEARPTR self, int c);
  /**
   * Formated printer for http, works like the wellknown printf function.
   * \param[in]  self    Self hook
   * \param[in]  fmt     Formated string
   * \param[in]  ...     All parameters requested in the fmt
   */
  void http_printf (http_t NEARPTR self, const char *fmt, ...);
  /**
   * Flush the not yet sent netbufs of the last http_printf
   * \param[in]  self    Self hook
   */
  void http_flush (http_t NEARPTR self);
  /**
   * Send a eof (netbuf with no data) to the webserver
   * \param[in]  self    Self hook
   */
  void http_eof (http_t NEARPTR self);
  /**
   * Get the cookie from http
   * \param[in]  self    Self hook
   * \param[out] cookie  Cookie string holder
   * \param[in]  len     String len of cookie
   * \return             -1 on error
   */
  int http_cookie (http_t NEARPTR self, char *cookie, int len);
  /**
   * Frees up the http instance (self hook)
   * \param[in]  self    Self hook, will be set to NULL after it is freed
   */
  void http_free (http_t NEARPTR NEARPTR self);

#ifdef __cplusplus
}
#endif

#endif

/*
 * $Log: printf.h,v $
 * Revision 1.7.24.1  2009/02/20 09:58:06  bastian
 * added http_write
 *
 * Revision 1.7  2004/12/17 15:19:25  christian
 * NEARPTR.
 *
 * Revision 1.6  2004/10/07 07:37:42  bastian
 * fixed NEAR * to NEARPTR
 *
 * Revision 1.5  2004/10/04 12:27:27  bastian
 * started to change for m16c
 *
 * Revision 1.4  2004/08/17 13:15:24  christian
 * New version of http. Have now much more info in the CGI and the CGI
 * is now able to provide http content with in the reply.
 *
 * Revision 1.3  2004/06/02 06:00:29  christian
 * Added splitted the hook specification from http printf file.
 *
 *
 */
