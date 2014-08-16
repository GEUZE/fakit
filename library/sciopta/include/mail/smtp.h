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
** ID: S04117CL2 
** +Revision: 1.11 + 
** +Date: 2005/07/01 12:28:32 + 
** SMTP API. 
**********************************************************************
EOC*/

#ifndef _MAIL_SMTP_H_ 
#define _MAIL_SMTP_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <ips/addr.h>
#include <ossys/types.h>

  /**
   * Opaque hook type for smtp usage
   */
  typedef struct smtp_s smtp_t;
  /**
   * connect to a smtp server
   * \param addr    Address of the smtp server
   * \param port    Port normaly 25!
   * \param domain  Hello I am domain
   * \param plid    Pool from which the smtp functions should alloc memory
   * \param tmo     How long a smtp functions should wait for reply
   * \return        A new smtp instance, called the self hook or NULL on error
   */
  smtp_t NEARPTR smtp_connect (ips_addr_t * addr, 
			       __u16 port, 
			       const char *domain,
			       sc_poolid_t plid, unsigned int tmo);
  /**
   * close a smtp session
   * \param self    Free the smtp hook and set it to NULL 
   */
  void smtp_close (smtp_t NEARPTR NEARPTR self);
  /**
   * Specify the sender
   * \param self    Self hook
   * \param from    Email address
   * \return        -1 on Error or the smtp Ok Code 
   */
  int smtp_from (smtp_t NEARPTR self, const char *from);
  /**
   * Specify the addresse, if there are more than one just call this function
   * for every addresse
   * \param self    Self hook
   * \param to      Email address
   * \return        -1 on Error or the smtp Ok Code 
   */
  int smtp_to (smtp_t NEARPTR self, const char *to);
  /**
   * Specify the carbon copy, if there are more than one just call this
   * function for every carbon copy 
   * \param self    Self hook
   * \param cc      Email address
   * \return        -1 on Error or the smtp Ok Code 
   */
  int smtp_cc (smtp_t NEARPTR self, const char *cc);
  /**
   * Specify the blind carbon copy, if there are more than one just call this
   * function for every blind carbon copy
   * \param self    Self hook
   * \param bcc     Email address
   * \return        -1 on Error or the smtp Ok Code 
   */
  int smtp_bcc (smtp_t NEARPTR self, const char *bcc);
  /**
   * Start a email
   * \param self    Self hook
   * \return        -1 on Error or the smtp Ok Code 
   */
  int smtp_begin (smtp_t NEARPTR self);
  /**
   * The smtp printf function, works like the well known printf function
   * \param self    Self hook
   * \param fmt     Formated text
   * \param ...     All variables listed in the formated text.
   * \return        -1 on Error or 0 
   */
  int smtp_printf (smtp_t NEARPTR self, const char *fmt, ...);
  /**
   * Flush the netbuf.
   * \param self    Self hook
   * \return        -1 on Error or 0 
   */
  int smpt_flush (smtp_t NEARPTR self);
  /** 
   * Marks the end of a mail text
   * \param self    Self hook
   * \return        -1 on Error or the smtp Ok Code 
   */
  int smtp_end (smtp_t NEARPTR self);
  /**
   * If a command is set by the smtp_printf (what is of course possible) you
   * have to wait for the "ok" of the smtp server
   * \param self    Self hook
   * \return        -1 on Error or the smtp Ok Code 
   */
  int smtp_waitOk (smtp_t NEARPTR self);
  /**
   * Get the status from mail host in plain text.
   * \param self   Self hook
   * return        The last status from mail host in plain text
   */
  char * smtp_status (smtp_t NEARPTR self);

#ifdef __cplusplus
}
#endif

#endif
