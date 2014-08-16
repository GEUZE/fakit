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
**              (c) 2008 SCIOPTA Systems AG/ Schweiz                **
**                                                                  **
**********************************************************************
** ID: S08023BS1                                                    **
** +Revision: 1.1 +                                                 **
** +Date: 2008/02/14 13:58:04 +                                     **
** IGMP messages                                                    **
**********************************************************************
EOC*/

#ifndef _IGMP_MSG_
#define _IGMP_MSG_
#include <netinet/in.h>
#include <ips/igmp.h>

#define IGMP_JOIN_GROUP       (IGMP_MSG_BASE+1)
#define IGMP_JOIN_GROUP_REPLY (IGMP_MSG_BASE+2)
typedef struct igmp_join_s {
  sc_msgid_t id;
  __u32 mcAddr; /* host order */
  __u32 ifAddr; /* host order */
  sc_msgid_t appl;
} igmp_join_t;

  
#define IGMP_LEAVE_GROUP       (IGMP_MSG_BASE+3)
#define IGMP_LEAVE_GROUP_REPLY (IGMP_MSG_BASE+4)
typedef struct igmp_leave_s {
  sc_msgid_t id;
  __u32 mcAddr; /* host order */
  __u32 ifAddr; /* host order */
  sc_msgid_t appl;
} igmp_leave_t;

#define IGMP_SOCKET            (IGMP_MSG_BASE+5)
#define IGMP_REPORT_TMO        (IGMP_MSG_BASE+6)
#define IGMP_LEAVE_TMO         (IGMP_MSG_BASE+7)

#define IGMP_ADD_SRC           (IGMP_MSG_BASE+8)
#define IGMP_ADD_SRC_REPLY     (IGMP_MSG_BASE+9)
typedef struct igmp_add_src_s {
  sc_msgid_t id;
  __u32 mcAddr;  /* host order */
  __u32 ifAddr;  /* host order */
  __u32 srcAddr; /* host order */
  sc_msgid_t appl;
} igmp_add_src_t;

/* private struct */
#endif

  
