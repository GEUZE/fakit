/*SOC -*-c++-*-
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
** ID: S03070CL64                                                   **
** +Revision: 1.100 +                                                 **
** +Date: 2008/01/16 13:02:00 +                                     **
** SDD messages                                                     **
**********************************************************************
EOC*/

#ifndef _GDD_SDD_MSG_
#define _GDD_SDD_MSG_

#include <sciopta.h>

EXTERN_C_BEGIN

#define SDD_FIRST_FREE (SDD_MSG_BASE + 71)

typedef struct sdd_baseMessage_s {
  sc_msgid_t id;
  sc_errcode_t error;
  void NEARPTR handle;
} sdd_baseMessage_t;

typedef struct sdd_name_s {
  sdd_baseMessage_t base;
  char string[SC_NAME_MAX + 1];
} sdd_name_t;

typedef struct sdd_obj_s {
  sdd_baseMessage_t base;
  char name[SC_NAME_MAX + 1];
  sc_msgid_t type;
  sc_pid_t controller;
  sc_pid_t sender;
  sc_pid_t receiver;
  void NEARPTR manager;
} sdd_obj_t;

#define SDD_IS_A(object, checkType) \
  (((sdd_obj_t NEARPTR ) object)->type & checkType)

#define SDD_ERROR			(SDD_MSG_BASE)
typedef struct sdd_error_s {
  sdd_baseMessage_t base;
} sdd_error_t;

#define SDD_OBJ_RELEASE			(SDD_MSG_BASE + 1)
#define SDD_OBJ_RELEASE_REPLY		(SDD_OBJ_RELEASE + 1)
typedef struct sdd_objRelease_s {
  sdd_baseMessage_t base;
} sdd_objRelease_t;

#define SDD_OBJ_DUP			(SDD_MSG_BASE + 3)
#define SDD_OBJ_DUP_REPLY		(SDD_OBJ_DUP + 1)
typedef struct sdd_objDup_s {
  sdd_baseMessage_t base;
} sdd_objDup_t;

#define SDD_OBJ_TIME_GET		(SDD_MSG_BASE + 7)
#define SDD_OBJ_TIME_GET_REPLY		(SDD_OBJ_TIME_GET + 1)
#define SDD_OBJ_TIME_SET		(SDD_MSG_BASE + 9)
#define SDD_OBJ_TIME_SET_REPLY		(SDD_OBJ_TIME_SET + 1)
typedef struct sdd_objTime_s {
  sdd_baseMessage_t base;
  __u32 data;
} sdd_objTime_t;
 
#define SDD_OBJ_SIZE_GET		(SDD_MSG_BASE + 11)
#define SDD_OBJ_SIZE_GET_REPLY		(SDD_OBJ_SIZE_GET + 1)
typedef struct sdd_size_s {
  size_t total;
  size_t free;
  size_t used;
  size_t bad;
} sdd_size_t;

typedef struct sdd_objSize_s {
  sdd_baseMessage_t base;
  size_t total;
  size_t free;
  size_t bad;
} sdd_objSize_t;

#define SDD_OBJ_DESC_GET		(SDD_MSG_BASE + 13)
#define SDD_OBJ_DESC_GET_REPLY		(SDD_OBJ_DESC_GET + 1)
typedef struct sdd_objDescGet_s {
  sdd_obj_t object;
} sdd_objDescGet_t;
  
#define SDD_MAN_ADD			(SDD_MSG_BASE + 15)
#define SDD_MAN_ADD_REPLY		(SDD_MAN_ADD + 1)
typedef struct sdd_manAdd_s {
  sdd_obj_t object;
} sdd_manAdd_t;

#define SDD_MAN_RM			(SDD_MSG_BASE + 17)
#define SDD_MAN_RM_REPLY		(SDD_MAN_RM + 1)
typedef struct sdd_manRm_s {
  sdd_obj_t object;
} sdd_manRm_t;

#define SDD_MAN_GET			(SDD_MSG_BASE + 19)
#define SDD_MAN_GET_REPLY		(SDD_MAN_GET + 1)
typedef struct sdd_manGet_s {
  sdd_obj_t object;
} sdd_manGet_t;

#define SDD_MAN_GET_FIRST		(SDD_MSG_BASE + 21)
#define SDD_MAN_GET_FIRST_REPLY		(SDD_MAN_GET_FIRST + 1)
typedef struct sdd_manGetFirst_s {
  sdd_obj_t object;
} sdd_manGetFirst_t;

#define SDD_MAN_GET_NEXT		(SDD_MSG_BASE + 23)
#define SDD_MAN_GET_NEXT_REPLY		(SDD_MAN_GET_NEXT + 1)
typedef struct sdd_manGetNext_s {
  sdd_obj_t object;
} sdd_manGetNext_t;

#define SDD_MAN_NOTIFY_ADD		(SDD_MSG_BASE + 25)
#define SDD_MAN_NOTIFY_ADD_REPLY	(SDD_MAN_NOTIFY_ADD + 1)
#define SDD_MAN_NOTIFY_RM		(SDD_MSG_BASE + 27)
#define SDD_MAN_NOTIFY_RM_REPLY		(SDD_MAN_NOTIFY_RM + 1)
typedef struct sdd_manNotify_s {
  sdd_name_t name;
  sc_ticks_t tmo;
  sc_tmoid_t tmoid;
} sdd_manNotify_t;

#define SDD_MAN_NOTIFY_TMO		(SDD_MSG_BASE + 29)
typedef struct sdd_manNotifyTmo_s {
  sc_msgid_t id;
  sc_msg_t delay;
} sdd_manNotifyTmo_t;

#define SDD_DEV_OPEN			(SDD_MSG_BASE + 31)
#define SDD_DEV_OPEN_REPLY		(SDD_DEV_OPEN + 1)
typedef struct sdd_devOpen_s {
  sdd_baseMessage_t base;
  flags_t flags;
} sdd_devOpen_t;

#define SDD_DEV_DUALOPEN		(SDD_MSG_BASE + 33)
#define SDD_DEV_DUALOPEN_REPLY		(SDD_DEV_DUALOPEN + 1)
typedef struct sdd_devDualopen_s {
  sdd_baseMessage_t base;
  void NEARPTR handle2;
} sdd_devDualopen_t;

#define SDD_DEV_CLOSE			(SDD_MSG_BASE + 35)
#define SDD_DEV_CLOSE_REPLY		(SDD_DEV_CLOSE + 1)
typedef struct sdd_devClose_s {
  sdd_baseMessage_t base;
} sdd_devClose_t;

#define SDD_DEV_READ			(SDD_MSG_BASE + 37)
#define SDD_DEV_READ_REPLY		(SDD_DEV_READ + 1)
typedef struct sdd_devRead_s {
  sdd_baseMessage_t base;
  ssize_t size;
  ssize_t curpos;
  __u8 *outlineBuf;
  __u8 inlineBuf[1];
} sdd_devRead_t;

#define SDD_DEV_WRITE			(SDD_MSG_BASE + 39)
#define SDD_DEV_WRITE_REPLY		(SDD_DEV_WRITE + 1)
typedef struct sdd_devWrite_s {
  sdd_baseMessage_t base;
  ssize_t size;
  ssize_t curpos;
  const __u8 *outlineBuf;
  __u8 inlineBuf[1];
} sdd_devWrite_t;

/* XXX: should not be used anymore !! use the new ioctl instead */
#define SDD_DEV_IOCTL			(SDD_MSG_BASE + 41)
#define SDD_DEV_IOCTL_REPLY		(SDD_DEV_IOCTL + 1)
typedef struct sdd_devIoctl_s {
  sdd_baseMessage_t base;
  unsigned int cmd;
  int ret;
  unsigned long outlineArg;
  unsigned char inlineArg[1];
} sdd_devIoctl_t;

#define SDD_FILE_SEEK			(SDD_MSG_BASE + 43)
#define SDD_FILE_SEEK_REPLY		(SDD_FILE_SEEK + 1)
typedef struct sdd_fileSeek_s {
  sdd_baseMessage_t base;
  off_t offset;
  int whence;
} sdd_fileSeek_t;

#define SDD_FILE_RESIZE			(SDD_MSG_BASE + 45)
#define SDD_FILE_RESIZE_REPLY		(SDD_FILE_RESIZE + 1)
typedef struct sdd_fileResize_s {
  sdd_baseMessage_t base;
  ssize_t size;
} sdd_fileResize_t;

/**
 * SDD_NET_OPEN message will activate the network driver. A network
 * driver could be a Ethernet driver, PPP or a protocol.
 */
#define SDD_NET_OPEN			(SDD_MSG_BASE + 47)
#define SDD_NET_OPEN_REPLY		(SDD_NET_OPEN + 1)
/**
 * \param base.id            Message ID
 * \param base.error         Error message on reply
 * \param base.handle        Device handle you got with SDD_MAN_GET
 * \param proto              Usualy 0, else the subprotocol mainly used
 *                           within ICMP
 * \param proxy              Process id to where the SDD_NET_RECEIVE must be
 *                           sent to
 */
typedef struct sdd_netOpen_s {
  sdd_baseMessage_t base;
  __u32 proto;
  sc_pid_t proxy;
} sdd_netOpen_t;

/**
 * SDD_NET_CLOSE message will deactivate the network driver.
 */
#define SDD_NET_CLOSE			(SDD_MSG_BASE + 49)
#define SDD_NET_CLOSE_REPLY		(SDD_NET_CLOSE + 1)
/**
 * \param base.id            Message ID
 * \param base.error         Error message on reply
 * \param base.handle        Device handle you got with SDD_MAN_GET
 */
typedef struct sdd_netClose_s {
  sdd_baseMessage_t base;
} sdd_netClose_t;

#define SDD_NETBUF                      (SDD_MSG_BASE + 51)
/**
 * SDD_NET_RECEIVE message will be sent up the layers on every received
 * packet. Direction is upper protocol layer.
 */
#define SDD_NET_RECEIVE			(SDD_MSG_BASE + 53)
#define SDD_NET_RECEIVE_REPLY		(SDD_NET_RECEIVE + 1)
/**
 * SDD_NET_RECEIVE_2 message is for internal use.
 */
#define SDD_NET_RECEIVE_2		(SDD_MSG_BASE + 55)
#define SDD_NET_RECEIVE_2_REPLY		(SDD_NET_RECEIVE_2 + 1)
/**
 * SDD_NET_RECEIVE_URGENT message declares a urgent packet. For future use. 
 */
#define SDD_NET_RECEIVE_URGENT		(SDD_MSG_BASE + 57)
#define SDD_NET_RECEIVE_URGENT_REPLY	(SDD_NET_RECEIVE_URGENT + 1)
/**
 * SDD_NET_SEND message will be send down to the layer. Direction is next
 * lower protocol layer or the driver at the end.
 */
#define SDD_NET_SEND			(SDD_MSG_BASE + 59)
#define SDD_NET_SEND_REPLY		(SDD_NET_SEND + 1)
/**
 * \param base.id            Message ID
 * \param base.error         Error message on reply
 * \param base.handle        Device handle you got with SDD_MAN_GET
 * \param doBeforeSend       Upcall function which must be called before
 *                           The layer do send it. Used to get a clean
 *                           layer separation in case of TCP|UDP pseudo
 *                           header stuff. NULL if not used.
 * \param returnTo           Information for the lowest layer (driver) to
 *                           send the packet back to returnTo pid if done
 *                           With the message id SDD_NET_SEND_REPLY
 * \param protocol           IP or ARP protocol depends on what your driver
 *                           did receive. Protocol will be found on the
 *                           received net data on the position 12:
 *                           = ntohs (*(__u16 *) &recvBuf[12])
 * \param pkttype            You also have to specify within the driver if
 *                           the received packet is broadcase, multicast,
 *                           host or other host packet. Broadcast and host 
 *                           type is very easy to detect. If the MAC address
 *                           is yours it is a SDD_HOST_PKT packet. if MAC
 *                           address is ff:ff:ff:ff:ff:ff it is a broadcast
 *                           address. Multicast is not supported yet. Other
 *                           host packet are all packet which are not
 *                           broadcast or not host packet and will only be
 *                           received in promiscous mode.
 * \param cur                cur points to the head of the lower or upper
 *                           layer, depends on the direction. If the packet
 *                           is received in a user application it points to
 *                           to ipv4, because user application does not count
 *                           as a layer.
 * \param head               head does point to the start of the buffer. This
 *                           space can be used for internal use. Can only be
 *                           used process local. On receive this are 0 bytes.
 *                           On send it must be at least 68 bytes for tcp,
 *                           udp and ipv4 headers. 
 * \param data               does point to the user data.
 * \param tail               Tail is used to add data at the end of the
 *                           netbuf. For TCP it would be very performant if
 *                           data + tail are about 512 bytes. 
 * \param end                marks the absolut end of the netbuffer.
 */
typedef struct sdd_netbuf_s sdd_netbuf_t;
struct sdd_netbuf_s {
  sdd_baseMessage_t base;
  void (*doBeforeSend) (sdd_netbuf_t NEARPTR netbuf);
  sc_pid_t returnTo;
  __u16 protocol; 
  __u16 pkttype; 
#define SDD_BROADCAST_PKT	0x8000
#define SDD_MULTICAST_PKT	0x4000
#define SDD_HOST_PKT		0x2000
#define SDD_OTHERHOST_PKT	0x1000
  size_t cur;
  size_t head;
  size_t data;
  size_t tail;
  size_t end;
  __u8 inlineBuf[1];
};

/**
 * This will get the data pointer from a given netbuf
 */
#define SDD_NET_DATA(netbuf) ((__u8 *)&(netbuf)->inlineBuf[(netbuf)->data])
/**
 * This will get the head pointer from a given netbuf
 */
#define SDD_NET_HEAD(netbuf) ((__u8 *)&(netbuf)->inlineBuf[(netbuf)->head])
/**
 * This will get the tail pointer from a given netbuf
 */
#define SDD_NET_TAIL(netbuf) ((__u8 *)&(netbuf)->inlineBuf[(netbuf)->tail])
/**
 * This will get the current pointer from a given netbuf
 */
#define SDD_NET_CUR(netbuf) ((__u8 *)&(netbuf)->inlineBuf[(netbuf)->cur])

/**
 * Header size == data - head
 */
#define SDD_NET_HEAD_SIZE(netbuf) ((netbuf)->data - (netbuf)->head)
/**
 * Data size = tail - data
 */
#define SDD_NET_DATA_SIZE(netbuf) ((netbuf)->tail - (netbuf)->data)
/**
 * Tail size = end - tail
 */
#define SDD_NET_TAIL_SIZE(netbuf) ((netbuf)->end - (netbuf)->tail)

typedef struct sdd_netReceive_s {
  sdd_netbuf_t buffer;
} sdd_netReceive_t;

typedef struct sdd_netSend_s {
  sdd_netbuf_t buffer;
} sdd_netSend_t;

#define SDD_OBJ_TAG_SET		        (SDD_MSG_BASE + 61)
#define SDD_OBJ_TAG_SET_REPLY		(SDD_OBJ_TAG_SET + 1)
#define SDD_OBJ_TAG_GET		        (SDD_MSG_BASE + 63)
#define SDD_OBJ_TAG_GET_REPLY		(SDD_OBJ_TAG_GET + 1)

typedef struct sc_tab_s{
  sc_tag_t tag;
  __u32 value;
} sc_tab_t;

typedef struct sc_ioctl_s {
  sc_tag_t tag;
  char data[1];
} sc_ioctl_t;

typedef struct sdd_objIoctl_s {
  sdd_baseMessage_t base;
  union {
    sc_tab_t entity[1];
    sc_ioctl_t ioctl;
  } u;
} sdd_objIoctl_t;

#define SDD_MAN_GET_ROOT              (SDD_MSG_BASE + 65)
#define SDD_OBJ                       (SDD_MSG_BASE + 66)

#define SDD_MAN_GET_PATH              (SDD_MSG_BASE + 67)
#define SDD_MAN_GET_PATH_REPLY        (SDD_MSG_BASE + 68)
typedef struct sdd_manGetPath_s {
  sdd_obj_t obj;
  char path[1];
} sdd_manGetPath_t;
  
#define SDD_3RD_PARTY                 (SDD_MSG_BASE + 69)
#define SDD_3RD_PARTY_REPLY           (SDD_3RD_PARTY + 1)
  typedef struct sdd_3rdParty_s {
    sdd_baseMessage_t base;
    __u32 magic;
    __u32 len;
    __u8 data[1];
  } sdd_3rdParty_t;


  EXTERN_C_END
#endif
