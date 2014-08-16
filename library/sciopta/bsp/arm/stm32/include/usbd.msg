/* -*-c++-*- */

#include <sciopta.h>

#define USB_SETUP_REQ_MSG (USB_MSG_BASE+0x201)
typedef struct usb_setup_req_s {
  sc_msgid_t id;
  //usb_deviceRequest_t setup;
} usb_setup_req_t;

#define USB_CTRL_PACKET_MSG (USB_MSG_BASE+0x202)
typedef struct usb_ctrlPacket_s {
  sc_msgid_t id;
  __uint ep_num;
  const __u8 *data;
  __uint size;
} usb_ctrlPacket_t;
		      
		      
#define USB_SET_DEVICE_DESC_MSG (USB_MSG_BASE+0x203)
#define USB_SET_CONFIG_DESC_MSG (USB_MSG_BASE+0x204)
typedef struct usb_setDesc_s {
  sc_msgid_t id;
  const __u8 * desc;
  __uint size;
} usb_setDesc_t;

#define USB_CHANGE_CONFIG_MSG (USB_MSG_BASE+0x205)
typedef struct usb_changeConfig_s {
  sc_msgid_t id;
  __uint configuration;
} usb_chgConfig;

#define USB_EP_CONFIG_MSG (USB_MSG_BASE+0x206)
typedef struct usb_epConfig_s {
  sc_msgid_t id;
  int enabled;
  __uint ep_num;
  __uint in_mode;
  __uint in_size;
  __uint out_mode;
  __uint out_size;
} usb_epConfig_t;

#define USB_SUSPEND_MSG (USB_MSG_BASE + 0x207)
typedef struct usb_suspend_s {
  sc_msgid_t id;
  __uint ep_num;
} usb_suspend_t;

#define USB_SEND_DATA_MSG(ep_num) (USB_MSG_BASE + 0x20 + (ep_num))
typedef struct usb_sendData_s {
  sc_msgid_t id;
  __u32 txAddr;
  int size;
  __u8 data[0];
} usb_sendData_t;

#define USB_RECV_DATA_MSG(ep_num) (USB_MSG_BASE + 0x30 + (ep_num))
typedef struct usb_recvData_s {
  sc_msgid_t id;
  __u32 rxAddr;
  int size;
  __u8 data[0];
} usb_recvData_t;

#define USB_EP_COMPLETE_MSG(ep_num) (USB_MSG_BASE + 0x40 + (ep_num))
typedef struct usb_epComplete_s {
  sc_msgid_t id;
  int rcvSnd;      /* 0 - recv / != 0 - send */
  int size;
  __u8 data[0];
} usb_epComplete_t;


#define USB_EP_RESET_MSG(ep_num)    (USB_MSG_BASE + 0x50 + (ep_num))
typedef struct usb_epReset_s {
  sc_msgid_t id;
} usb_epReset_t;
