/*SOC
**********************************************************************
**  ______  _______ _________ _______  _______ _________ _______    **
** (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
** | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
** | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
** (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
**       ) || |         | |   | |   | || (         | |   | (   ) |  **
** /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
** \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
**                                                                  **
** (c) 2002 Sciopta GmbH, Litronic AG/ Schweiz                      **
**                                                                  **
**********************************************************************
** ID: S01332CL15                                                   **
** +Revision: 1.12 +                                                 **
** +Date: 2004/10/07 07:37:43 +                                     **
** Telnet interface (integratable into shell)                       **
**********************************************************************
EOC*/

#ifndef _IPS_TELNET_H_
#define _IPS_TELNET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/defines.h>
#include <ips/addr.h>
#include <ips/stream.h>
#include <sdd/sdd.msg>

  /**
   */
  void telnet_daemon (ips_addr_t *ip, __u16 port, sc_pid_t *server);

  /**
   */
  void telnet_device (const char *name);

  /**
   * telnet hook, needed for any telnet function
   */
  typedef struct telnet_s telnet_t;
  struct telnet_s {
    sc_msgid_t id;
    ips_stream_t *stream;
    int last;
  };

  /**
   * creates a new telnet handle to track this session.
   * \param[in]  conn     established and ready to receive connection (ASYNC).
   * \param[in]  plid     pool from which the hook shall be allocated.
   * \param[in]  tmo      how long should telnet_new try to get the memory. 
   * \return              NULL on error else a hook for the telnet session.
   */
  telnet_t *telnet_new (sdd_obj_t NEARPTR  conn, sc_poolid_t plid, sc_ticks_t tmo);

  /**  
   * gives the telnet hook free, but does not close the connection and does
   * not free the user data handle.
   * \param[in]  self     the hooking selfpointer which holds the session data.
   */
  void telnet_destroy (telnet_t ** self);

  /**
   * put one charakter into netbuf.
   * \param[in]  self     the hooking selfpointer which holds the session data.
   * \param[in]  c        the char which shall be put to netbuf.
   * \return              character written as an unsigned char cast to an int 
   *                      or EOF on error
   */
  int telnet_putc (telnet_t * self, const char c);

  /**
   * print a fromated string to netbuf.
   * \param[in]  self     the hooking selfpointer which holds the session data.
   * \param[in]  fmt      formated string, like in wellknown printf.
   * \return              -1 on error else number of bytes written.
   */
  int telnet_printf (telnet_t * self, const char *fmt, ...);

  /**
   * flush the netbuf even if it is not full yet.
   * \param[in]  self     the hooking selfpointer which holds the session data.
   * \return              -1 on error else 0.
   */
  int telnet_flush (telnet_t * self);


#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: telnet.h,v $
** Revision 1.12  2004/10/07 07:37:43  bastian
** fixed NEAR * to NEARPTR
**
** Revision 1.11  2004/10/04 12:27:28  bastian
** started to change for m16c
**
** Revision 1.10  2004/07/22 06:45:49  christian
** Telent is now a sciopta character device like serial or keyboard/screen.
**
** Revision 1.9  2004/07/19 09:48:02  christian
** Did the newline conversion in a way, that it does no conversion if
** allready done by customer and or shell/shellcomand.
**
** Revision 1.8  2004/07/16 13:25:41  christian
** Telnet also needs a data pointer for customer putc function.
**
** Revision 1.7  2004/07/15 11:31:02  christian
** Added newline conversion.
**
** Revision 1.6  2004/07/15 08:29:33  christian
** telnet_putc integration.
**
** Revision 1.5  2004/07/14 14:03:44  christian
** Printf functionality is now globaly solved in the ips stream lib.
**
** Revision 1.4  2004/07/13 13:49:08  christian
** Implemented putc and filled the other functions in compilable way.
**
** Revision 1.3  2004/07/13 10:08:20  christian
** starting function library for telnet usage.
**
** Revision 1.2  2003/03/18 15:04:26  christian
** Cleaned.
**
** Revision 1.1  2002/12/20 16:16:37  christian
** Intermediate.
**
** Revision 2.1  2002/01/04 13:30:43  christian
** generic view to net device.
**
** Revision 2.0  2002/01/03 15:54:46  christian
** Changed the freighter structure.
** This makes the net devices interface IP independent. It is now possible to
** add IPv6 and/or other protocol families.
**
** Revision 1.1.1.1  2001/11/28 12:43:04  christian
** emigrate to cvs
**
**/
