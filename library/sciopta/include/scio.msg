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
** ID: S04026CL2 
** +Revision: 1.4 + 
** +Date: 2006/06/14 14:53:05 +
** SCIO for legacy code 
**********************************************************************
EOC*/

#ifndef _SCIO_MSG_MSG
#define _SCIO_MSG_MSG	1

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/defines.h>
#include <sdd/sdd.msg>
#include <ips/addr.h>

#define SCIO_SOCKET		(SCIO_MSG_BASE + 3)
#define SCIO_SOCKET_REPLY	(SCIO_SOCKET + 1)
  typedef struct scio_socket_s {
    sdd_obj_t obj;
  } scio_socket_t;
  
#define SCIO_SEND		(SCIO_MSG_BASE + 5)
#define SCIO_SEND_REPLY		(SCIO_SEND + 1)
  typedef struct scio_send_s {
    sdd_devWrite_t write;
    int flags;
  } scio_send_t;
  
#define SCIO_RECV		(SCIO_MSG_BASE + 7)
#define SCIO_RECV_REPLY		(SCIO_RECV + 1)
  typedef struct scio_recv_s {
    sdd_devRead_t read;
    int flags;
    ips_addr_t fromAddr;
    __u16 fromPort;
  } scio_recv_t;
  
#define SCIO_DEAD		(SCIO_MSG_BASE + 9)
  typedef struct bsd_dead_s {
    sc_msgid_t id;
    sc_errcode_t error;
  } bsd_dead_t;
  
#ifdef __cplusplus
}
#endif

#endif

/*
** $Log: scio.msg,v $
** Revision 1.4  2006/06/14 14:53:05  bastian
** xxx
**
** Revision 1.3  2004/05/27 09:15:54  christian
** Working sendto and recvfrom.
**
** Revision 1.2  2004/01/29 14:36:10  christian
** Read/write and recv/send are implemented but not tested.
**
** Revision 1.1  2004/01/27 12:52:17  christian
** Startet a new implementation to handle socket descriptors.
**
**
*/
