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
** ID: S04196CL1                                                    **
** +Revision: 1.10 +                                                 **
** +Date: 2005/05/25 07:34:31 +                                     **
** IPS printf lib                                                   **
**********************************************************************
EOC*/

#ifndef _IPS_STREAM_H_
#define _IPS_STREAM_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <sciopta.h>
#include <stdarg.h>
#include <sdd/sdd.msg>

#define IPS_STREAM_BUF_SIZE 512
  
  typedef struct ips_stream_s ips_stream_t;

  /**
   * User defined putc function type.
   * \param[in]  self     the hooking selfpointer which holds the session data.
   * \param[in]  data     user defined hook.
   * \param[in]  c        the char which shall be put to netbuf.
   * \return              character written as an unsigned char cast to an int 
   *                      or EOF on error
   */
  typedef int (*ips_putc_f) (ips_stream_t NEARPTR self, void *data, 
                             const char c);
 
  /**
   * Hook definition for the session data.
   */
  struct ips_stream_s {
    sc_msgid_t id;
    sdd_obj_t NEARPTR conn;
    int out;
    sdd_netbuf_t NEARPTR outbuf;
    int in;
    sdd_netbuf_t NEARPTR inbuf;
    void *data;
    ips_putc_f putc;
  };

  /**
   * Get a new instance for ips printf lib.
   * \param[in]  conn     established connection in async and ret ack mode.
   * \param[in]  data     user data.
   * \param[in]  putc     User defined putc could itself call ips_putc but
   *                      never call ips_vprintf, because it will call this 
   *                      subscribet putc function this will lead into a
   *                      endless recursion!
   *                      This could be used for example for character 
   *                      escaping/conversions.
   *                      Could be NULL -> default putc will be taken.
   * \param[in]  plid     pool for allocations.
   * \param[in]  tmo      tmo strategy for allocations.
   * \return              NULL on error else a ips printf lib instance.
   */
  ips_stream_t NEARPTR ips_streamNew (sdd_obj_t NEARPTR  conn, void *data, 
                                      ips_putc_f putc, sc_poolid_t plid, 
				      sc_ticks_t tmo);
  
  /**  
   * gives the telnet hook free, but does not close the connection and does
   * not free the user data handle.
   * \param[in]  self     the hooking selfpointer which holds the session data.
   */
  void ips_streamDestroy (ips_stream_t NEARPTR  NEARPTR  self);

  /**
   * put one charakter into netbuf.
   * \param[in]  self     the hooking selfpointer which holds the session data.
   * \return              -1 on error.
   */
  int ips_streamNetbufAlloc (ips_stream_t NEARPTR self);

  /**
   * put one charakter into netbuf.
   * \param[in]  self     the hooking selfpointer which holds the session data.
   * \param[in]  c        the char which shall be put to netbuf.
   * \return              character written as an unsigned char cast to an int 
   *                      or EOF on error
   */
  int ips_putc (ips_stream_t NEARPTR  self, const char c);

  /**
   * put one charakter into netbuf.
   * \param[in]  self     the hooking selfpointer which holds the session data.
   * \param[in]  buf      data buffer.
   * \param[in]  size     data size.
   * \return              number of written bytes else -1 on error.
   */
  ssize_t ips_write (ips_stream_t NEARPTR self, const void *buf, size_t size);

  /**
   * Many protocols need a printf like function. To write such a function
   * only once and that this function can be wrapped, we design a vprintf for
   * ips in general. We suppose that the underlying protocol hast allways the
   * option SO_SC_RET_ACK.
   * \param[in]  self   Printf sesstion hook.
   * \param[in]  fmt    Formated string.
   * \param[in]  ap     va_list instead of  a variable number of arguments.
   * \return            -1 on error else the written number of bytes.
   */
  int ips_vprintf (ips_stream_t NEARPTR  self, const char *fmt, va_list ap);
  /**
   * Many protocols need a printf like function. To write such a function
   * only once and that this function can be wrapped, we design a vprintf for
   * ips in general. We suppose that the underlying protocol hast allways the
   * option SO_SC_RET_ACK.
   * \param[in]  self   Printf sesstion hook.
   * \param[in]  fmt    Formated string.
   * \param[in]  ...    a variable number of arguments.
   * \return            -1 on error else the written number of bytes.
   */
  int ips_printf (ips_stream_t NEARPTR  self, const char *fmt, ...);
  /**
   * flush the netbuf even if it is not full yet.
   * \param[in]  self     the hooking selfpointer which holds the session data.
   * \return              -1 on error else 0.
   */
  int ips_flush (ips_stream_t NEARPTR  self);

#ifdef __cplusplus
}
#endif
#endif
