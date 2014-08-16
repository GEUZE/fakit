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
** ID: S05315CL6
** +Revision: 1.5 + 
** +Date: 2006/01/19 13:05:58 + 
** FTP NVT API. 
**********************************************************************
EOC*/

#ifndef _FTP_SERVER_H_ 
#define _FTP_SERVER_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <sciopta.h>
#include <sdd/sdd.msg>
#include <ips/addr.h>

  /**
   * Upcall prototype for new connection.
   * \param[in] data        User data installed with ftp_daemon_t.
   * \param[in] msg         SDD object (connection descriptor)
   */
  typedef void (*ftp_accept_f) (void * data, sc_msgptr_t msg);

  /**
   * Daemon configuration information.
   */ 
  typedef struct ftp_daemon_s {
    sc_msgid_t id;
    ips_addr_t ip;              /*> FTP host address */
    __u16 port;                 /*> FTP host port */
    ftp_accept_f accept;        /*> Upcall for new connection */
    void *data;                 /*> User data for upcall */
  } ftp_daemon_t;

  /**
   * FTP daemon process which does schedul the servers on accepted
   * connections.
   * \param[in]  conf       General configurations.
   * \param[in]  statServer Statical server process list, NULL terminated.
   */
  void ftp_daemon (ftp_daemon_t *conf);

  /**
   * Prototype for server process.
   * \param[in]  conn       Connection object
   * \param[in]  login      Login Process 
   * \param[in]  root       Root directory of fs 
   */ 
  void ftp_server (sdd_obj_t * conn, sc_pid_t login, sdd_obj_t * root);

#ifdef __cplusplus
}
#endif

#endif
