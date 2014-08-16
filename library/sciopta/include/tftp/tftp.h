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
** ID: S03101CL1                                                    **
** +Revision: 1.14 +                                                 **
** +Date: 2008/01/16 13:02:07 +                                     **
** tftp - trivial file transfer program                             **
**********************************************************************
EOC*/

#ifndef _TFTP_TFTP_H_
#define _TFTP_TFTP_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/defines.h>
#include <ips/addr.h>

#define TFTP_HANDLE    (TFTP_MSG_BASE)

#define TFTP_RRQ	1
#define TFTP_WRQ	2
#define TFTP_DATA	3
#define TFTP_ACK	4
#define TFTP_ERROR	5

#define TFTP_ENOTDEF		0
#define TFTP_EFILE_NOT_FOUND	1
#define TFTP_EACCESS_VIOL	2
#define TFTP_EDISK_FULL		3
#define TFTP_EILLEGAL_OP	4
#define TFTP_EUNKNOWN		5
#define TFTP_EFILE_EXIST	6
#define TFTP_ENO_SUCH_USER	7

  /**
   * Opaque hook type for tftp usage
   */
  typedef struct tftp_handle_s tftp_handle_t;

  /**
   * Close a connection and free the self hook.
   * \param self    Self hook, will be set to NULL
   */
  void tftp_close (tftp_handle_t NEARPTR NEARPTR self);

  /***********************************************/
  /************** Client only API ****************/
  /***********************************************/

  /**
   * Connect to a tftp server.
   * \param addr    Address of the tftp server
   * \param port    Port of the tftp server
   * \param plid    Pool for memory allocations 
   * \param tmo     Timeout waiting for pkts 
   * \return        A new instance for the tftp connection
   */
  tftp_handle_t NEARPTR tftp_connect (ips_addr_t *addr, 
				      int port, 
				      sc_poolid_t plid,
				      sc_ticks_t tmo);

  /**
   * Set tftp mode.
   * \param self    Self hook
   * \param mode    Tftp mode: "ascii", "binary"
   */
  int tftp_mode (tftp_handle_t NEARPTR self, const char *mode);

  /**
   * Put the content of a memory area to the server.
   * \param self    Self hook
   * \param start   Memory start
   * \param len     Length of the memory to transmit
   * \param to      File name on the tftp server
   * \return        -1 on Error
   */
  int tftp_putMem (tftp_handle_t NEARPTR self, const __u8 * start, __u32 len,
                   const char *to);
  /**
   * Put a file from the sciopta filesystem to the server.
   * \param self    Self hook
   * \param dev     An allready opened device 
   * \param to      Filename on the tftp server
   */
  int tftp_putFile (tftp_handle_t NEARPTR self, 
		    sdd_obj_t NEARPTR dev, 
		    const char *to);

  /**
   * Get a file from tftp server to a memory area.
   * \param self    Self hook
   * \param start   start of the memory
   * \param len     Length of the memory
   * \param from    Filename on the tftp server
   */
  int tftp_get2Mem (tftp_handle_t NEARPTR self, 
		    __u8 * start,
		    __u32 len,
                    const char *from);
  /**
   * Get a file from tftp server to a file on the sciopta filesystem.
   * \param self    Self hook
   * \param dev     An allready opened device 
   * \param from    File on the tftp server
   */
  int tftp_get2File (tftp_handle_t NEARPTR self, 
		     sdd_obj_t NEARPTR dev, 
		     const char *from);

  /***********************************************/
  /************** Server only API ****************/
  /***********************************************/

  /**
   * Listen on a spezified port for a tftp connection (first pkt).
   * \param port    Port on which tftp should listen for connection (first pkt)
   * \param plid    Pool for memory allocations 
   * \param tmo     Timeout waiting for pkts on the established connections
   * \return        A new instance for tftp, called the self hook
   */
  tftp_handle_t NEARPTR tftp_listen (int port, 
				     sc_poolid_t plid, 
				     sc_ticks_t tmo);
  /**
   * Accept a connection (first pkt arrived).
   * 
   * \param self    Self hook
   * \param opcode  Request code from the other side: TFTP_RRQ, TFTP_WRQ
   * \param file    requested file name
   * \param len     Length of file string to avoid range checks 
   * \return        A new instance for the accepted tftp connection
   * \todo          Functions to handle a accepted connections
   */
  tftp_handle_t NEARPTR tftp_accept (tftp_handle_t NEARPTR self, 
				     __u8 * opcode, char *file,
				     int len);

  /**
   *  Receive data from tftp client and store to a memory area.
   * \param self    Self hook
   * \param start   start of the memory
   * \param len     Length of the memory
   */
  int tftp_recv2Mem (tftp_handle_t NEARPTR self, __u8 * mem, __u32 len);

  /**
   *  Receive data from tftp client and store to a SDD file.
   * \param self    Self hook
   * \param dev     SDD device
   */
  int tftp_recv2File (tftp_handle_t NEARPTR self, sdd_obj_t NEARPTR dev);

  /**
   * Put the content of a memory area to the client.
   * \param self    Self hook
   * \param start   Memory start
   * \param len     Length of the memory to transmit
   * \return        sent length -1 on Error
   */
  int tftp_sendMem (tftp_handle_t NEARPTR self, const __u8 * mem, __u32 len);

  /**
   * Send a file to the client. Only works as server.
   * \param self    Self hook
   * \param dev     SDD device
   * \return        sent length or -1 on Error
   */
  int tftp_sendFile (tftp_handle_t NEARPTR self, sdd_obj_t NEARPTR dev);

#ifdef __cplusplus
}
#endif
#endif

