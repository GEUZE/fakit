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
** ID: S05315CL5 
** +Revision: 1.3 + 
** +Date: 2006/01/19 13:05:58 + 
** FTP NVT API. 
**********************************************************************
EOC*/

#ifndef _FTP_NVT_H_ 
#define _FTP_NVT_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <sciopta.h>
#include <sdd/sdd.msg>

  /**
   * Opaque hook type for ftp nvt usage
   */
  typedef struct ftp_nvt_s ftp_nvt_t;
  /**
   * Get a ftp nvt stream
   * \param conn    IPS connection object 
   * \return        A new ftp nvt instance, NULL on error
   */
  ftp_nvt_t NEARPTR ftp_nvtStreamNew (sdd_obj_t NEARPTR conn);
  /**
   * \param self    Self hook
   * Free ftp nvt stream
   */
  void ftp_nvtStreamDestroy (ftp_nvt_t NEARPTR NEARPTR self);
  /**
   * The ftp nvt printf function, works like the well known printf function
   * \param self    Self hook
   * \param fmt     Formated text
   * \param ...     All variables listed in the formated text.
   * \return        -1 on Error or 0 
   */
  int ftp_nvtPrintf (ftp_nvt_t NEARPTR self, const char *fmt, ...);
  /**
   * Flush the netbuf.
   * \param self    Self hook
   * \return        -1 on Error or 0 
   */
  int ftp_nvtFlush (ftp_nvt_t NEARPTR self);
  /**
   * If a command is set by the ftp_nvt_printf (what is of course possible) you
   * have to wait for the "ok" of the ftp server
   * \param self    Self hook
   * \return        -1 on Error or the ftp Ok Code 
   */
  int ftp_nvtWaitOk (ftp_nvt_t NEARPTR self, sc_ticks_t tmo);
  /**
   * Get the status from ftp nvt host in plain text.
   * \param self   Self hook
   * return        The last status from ftp nvt host in plain text
   */
  char * ftp_nvtStatus (ftp_nvt_t NEARPTR self);


#ifdef __cplusplus
}
#endif

#endif
