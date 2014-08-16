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
**              (c) 2009 SCIOPTA Systems AG/ Schweiz                **
**                                                                  **
**********************************************************************
** ID: S09142BS1                                                    **
** +Revision: 1.1.2.2 +                                                 **
** +Date: 2009/08/06 08:56:04 +                                     **
** USBH messages                                                    **
**********************************************************************
EOC*/

#ifndef _USBH_MSG_
#define _USBH_MSG_

#define HCC_USBH_BASE    (HCC_MSG_BASE+0x1000)
#define HCC_USBH_ERR_BASE (HCC_MSG_BASE+0x1000)

/* Messages */
#ifdef USBH_DRIVER
#define HCC_USBH_ADD_HUB (HCC_USBH_BASE+1)
#define HCC_USBH_ADD_HUB_REPLY (HCC_USBH_BASE+2)
typedef struct hcc_usbhAddHub_s {
  sc_msgid_t id;
  sc_errcode_t err;
  int idx;
  void *port_hdl;
  hub_scan_fn scan;
  hub_fn reset;
  hub_fn disable;
} hcc_usbhAddHub_t;

#define HCC_USBH_DEL_HUB (HCC_USBH_BASE+3)
#define HCC_USBH_DEL_HUB_REPLY (HCC_USBH_BASE+4)
typedef struct hcc_usbhDelHub_s {
  sc_msgid_t id;
  sc_errcode_t err;
  int idx;
  void *port_hdl;
} hcc_usbhDelHub_t;
#endif

#define HCC_USBH_CONNECT (HCC_USBH_BASE+5)
typedef struct hcc_usbhConnect_s {
  sc_msgid_t id;
  void *hdl;
}hcc_usbhConnect_t;

#define HCC_USBH_DISCONNECT (HCC_USBH_BASE+6)
typedef struct hcc_usbhDisconnect_s {
  sc_msgid_t id;
  void *hdl;
}hcc_usbhDisconnect_t;

#define HCC_USBH_IRQ     (HCC_USBH_BASE+7)
#define HCC_USBH_IRQ1    (HCC_USBH_BASE+8)
typedef struct hcc_usbhIrq_s {
  sc_msgid_t id;
  int misc;
}hcc_usbhIrq_t;

#define HCC_USBH_ALLOC   (HCC_USBH_BASE+9)

#define HCC_USBH_DOORBELL    (HCC_USBH_BASE+10)
#define HCC_USBH_DOORBELL_REPLY    (HCC_USBH_BASE+11)
typedef struct hcc_usbhDoorBell_s {
  sc_msgid_t id;
}hcc_usbhDoorBell_t;

/* MIDI */
#define HCC_USBH_MIDI_EVENT (HCC_USBH_BASE+12)
/* message defined in midi.h */

/* MSC */
#define HCC_USBH_LUN_POLL (HCC_USBH_BASE+14)
typedef struct hcc_usbhLunPollMsg_s {
  sc_msgid_t id;
  unsigned int volume;
} hcc_usbhLunPollMsg_t;


/* Errors */
#define HCC_USBH_OHIC_NRDY (HCC_USBH_ERR_BASE+0x10+SC_ERR_MODULE_FATAL)

#endif /* _USBH_MSG_ */
