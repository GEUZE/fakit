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
** ID: S01332CL18                                                   **
** +Revision: 2.24 +                                                **
** +Date: 2004/06/15 11:17:24 +                                     **
** UDP header definition                                            **
**********************************************************************
EOC*/

#ifndef _UDP_H
#define _UDP_H	1

#ifdef __cplusplus
extern "C" {
#endif

#include <ossys/defines.h>
#include <ips/addr.h>

#include <sdd/sdd.h>

  /**
   * Read/(write)able tags
   */
#define UDP_TAG_IN_DATAGRAMS 1
#define UDP_TAG_NO_PORTS 2
#define UDP_TAG_IN_ERRORS 3
#define UDP_TAG_OUT_DATAGRAMS 4

  /**
   * For direct access to the UDP header 
   */

#define UDP_HEADER_SIZE	8
  typedef struct udp_header {
    __u16 srcPort;
    __u16 dstPort;
    __u16 len;
    __u16 chksum;
  } udp_header_t;

  void udp_process (int maxConn, int maxPktQueue);

#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: udp.h,v $
** Revision 2.24  2004/06/15 11:17:24  christian
** All needed tags are implemented.
**
** Revision 2.23  2004/06/11 14:02:36  christian
** Switch over to new ioctl command (tagging system).
**
** Revision 2.22  2004/06/01 12:55:29  christian
** Do string to binary translation family specific.
**
** Revision 2.21  2004/04/23 06:13:56  christian
** Did rename to make source more readable.
**
** Revision 2.20  2004/01/27 14:33:53  christian
** Tcp uses now sc_miscChksumContd.
** Tcp does now correctly handle the maxConn (max number of connections).
** Did wrongly renamed struct member chksum to sc_miscChksum :-((
**
** Revision 2.19  2004/01/27 13:32:22  christian
** Changed namings of some libsc functions to sc_miscXxxYyy
**
** Revision 2.18  2004/01/22 07:59:12  christian
** Remove all __inline
**
** Revision 2.17  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 2.16  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 2.15  2003/08/18 12:54:29  christian
** Removed ips/util.h is now all in netinet/in.h
**
** Revision 2.14  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 2.13  2003/03/18 15:04:26  christian
** Cleaned.
**
** Revision 2.12  2003/03/14 14:51:02  bastian
** fixes for ADS
**
** Revision 2.11  2003/03/11 14:59:28  christian
** Bugfixes.
**
** Revision 2.10  2003/03/10 15:39:49  christian
** Udp has now connection and rcv queue limitation.
** Telnet has a outcommented get peer name solution.
**
** Revision 2.9  2003/01/15 16:18:08  christian
** Added statistical info (handle == NULL, else you will get socket infos)
**
** Revision 2.8  2003/01/15 14:56:30  christian
** MIB statistics added.
**
** Revision 2.7  2002/10/04 12:40:10  christian
** Adjusted ips toward the new scipota specific system call ips_alloc.
**
** Revision 2.6  2002/10/03 13:03:03  christian
** Finished the socket interface (message based interface).
**
** Revision 2.5  2002/09/20 14:56:49  christian
** Do not Incr/Decr protocol entities, because of possible resource
** dead locks.
**
** Revision 2.4  2002/09/20 09:55:55  christian
** Connect/Disconnect implemented
**
** Revision 2.3  2002/02/12 14:35:36  christian
** A new object oriented like device structur.
**
** Revision 2.2  2002/02/05 12:27:34  christian
** Make the socket init funktion available.
**
** Revision 2.1  2002/01/04 13:30:43  christian
** generic view to net device.
**
** Revision 2.0  2002/01/03 15:54:46  christian
** Changed the freighter structure.
** This makes the net devices interface IP independent. It is now possible to
** add IPv6 and/or other protocol families.
**
** Revision 1.2  2001/12/11 13:25:33  christian
** Removed ips/data.h cause do not need any more.
** Add a des pointer (have to set it on receive) and let more pointer untouched.
**
** Revision 1.1.1.1  2001/11/28 12:43:04  christian
** emigrate to cvs
**
 * 
 *
**/
