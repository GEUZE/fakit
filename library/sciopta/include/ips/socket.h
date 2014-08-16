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
** ID: S04008CL2
** +Revision: 1.9 +
** +Date: 2008/01/24 00:20:34 +
** Socket base structur 
**********************************************************************
EOC*/

#ifndef _IPS_SOCKET_H_
#define _IPS_SOCKET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sciopta.h>
#include <ips/addr.h>

  /**
   * General socket structure for tcp and udp
   */
  typedef struct ips_socket_s {
    sc_msgid_t id;
    __u16 srcPort;
    __u16 dstPort;
    ips_addr_t srcIp;
    ips_addr_t dstIp;
    dbl_t list;
  } ips_socket_t;

  /** 
   * socket hash object structur
   */
  typedef struct ips_socketHash_s ips_socketHash_t;

  /** 
   * Create a new socket hash instance.
   * \param[in]  cmpHdl  Compare handler
   * \param[in]  delHdl  Delete handler
   * \param[in]  plid    Poolid for memory allocation
   * \param[in]  size    Size
   * \return             NULL on error else a new instance
   */
  ips_socketHash_t NEARPTR ips_socketHashNew (int (*cmpHdl) (ips_socket_t NEARPTR , 
							     ips_socket_t NEARPTR ),
					      void (*delHdl) (void NEARPTR ),
					      int size);
  /**
   * Add a socket.
   * \param[in]  hash    Hash instance
   * \param[in]  socket  Socket
   */
  void ips_socketHashAdd (ips_socketHash_t NEARPTR hash,
			  ips_socket_t NEARPTR socket);
  /**
   * Remove a socket.
   * \param[in]  hash    Hash instance
   * \param[in]  ref     Reference socket
   */
  void ips_socketHashRm (ips_socketHash_t NEARPTR hash,
			 ips_socket_t NEARPTR ref);
  /**
   * Get a socket.
   * \param[in]  hash    Hash instance
   * \param[in]  ref     Reference socket
   */
  ips_socket_t NEARPTR ips_socketHashGet (ips_socketHash_t NEARPTR hash,
					ips_socket_t NEARPTR ref);
  /**
   * Get a free ephemeral port.
   * \param[in]  hash    Hash instance
   * \param[in]  ref     Reference socket which defines from where the search 
   *                     shall be started
   */
  int ips_socketGetEphPort (ips_socketHash_t NEARPTR hash, ips_socket_t NEARPTR ref);


  /**
   * Search an ips_socket
   * \param [in] list   Socket-list
   * \param [in] ref    Compare value
   *
   */
  ips_socket_t NEARPTR ips_socketGet(dbl_t NEARPTR list,ips_socket_t NEARPTR ref);

  /**
   * Search an ips_socket
   * \param [in] list   Socket-list
   * \param [in] socket    Compare value
   * \param [in] cmp    Comparefunction
   *
   */
 
  ips_socket_t NEARPTR 
  ips_socketGetCmp(dbl_t NEARPTR list,
		   ips_socket_t NEARPTR socket,
		   int (*cmp)(ips_socket_t NEARPTR l,ips_socket_t NEARPTR r));
    

  /**
   * Search next ips_socket
   * \param [in] list   Socket-list
   * \param [in] start  last socket
   * \param [in] socket Compare value
   * \param [in] cmp    Comparefunction
   *
   */
  
ips_socket_t NEARPTR 
ips_socketGetNextCmp(
  dbl_t NEARPTR list,
  ips_socket_t NEARPTR start,
  ips_socket_t NEARPTR socket,
  int (*cmp)(ips_socket_t NEARPTR l,ips_socket_t NEARPTR r)
  );

  /**
   * Return the first socket in the list.
   * \param [in] list   Socket-list
   *
   */
  ips_socket_t NEARPTR ips_socketGetFirst(dbl_t NEARPTR list);
  
  /**
   * Search the next socket starting from ref
   * \param [in] list   Socket-list
   * \param [in] ref    Start socket
   *
   */
  ips_socket_t NEARPTR ips_socketGetNext(dbl_t NEARPTR list,ips_socket_t NEARPTR ref);
  
  /**
   * Search socket and remove if found
   * \param [in] list   Socket-list
   * \param [in] ref    socket
   *
   */
  void ips_socketRm(dbl_t  NEARPTR list,ips_socket_t NEARPTR ref);
  
#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: socket.h,v $
** Revision 1.9  2008/01/24 00:20:34  bastian
** first working MC version
**
** Revision 1.8  2004/12/16 10:32:11  bastian
** added socketRm
**
** Revision 1.7  2004/12/09 10:10:56  bastian
** fixed empty lines + log
**
** Revision 1.6  2004/12/09 08:15:04  bastian
** big bang
**
** Revision 1.5  2004/10/07 07:37:42  bastian
** fixed NEAR * to NEARPTR
**
** Revision 1.4  2004/10/04 12:27:27  bastian
** started to change for m16c
**
** Revision 1.3  2004/05/26 06:05:23  christian
** Header files documented and spell checks.
**
** Revision 1.2  2004/01/12 07:26:32  christian
** Replace the linear lists for connections and listener with a more
** efficient hash.
**
** Revision 1.1  2004/01/08 15:09:14  christian
** Do general socket operations in out of tcp and udp (and others).
**
**
*/
