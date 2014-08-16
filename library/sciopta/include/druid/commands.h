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
** ID: S04281CL3                                                    **
** +Revision: 1.36 +                                                 **
** +Date: 2006/02/08 10:11:32 +                                     **
** Druid commands                                                   **
**********************************************************************
EOC*/


#ifndef _DRUID_COMMANDS_H_ 
#define _DRUID_COMMANDS_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#define DRUID_NONE                      0

#define DRUID_CMD_STOP                  1
#define DRUID_CMD_START                 2
#define DRUID_CMD_MODULE_GET            3
#define DRUID_CMD_PROCESS_GET           4
#define DRUID_CMD_STACK_GET             5
#define DRUID_CMD_POOL_GET              6
#define DRUID_CMD_MSG_GET               7
#define DRUID_CMD_OPTION_ENTER          8
#define DRUID_CMD_OPTION_DISPLAY        9
#define DRUID_CMD_OPTION_CLEAR         10
#define DRUID_CMD_TRACE_DISPLAY        11 

#define DRUID_CMD_TEXT_DISPLAY        254
#define DRUID_CMD_ERROR_DISPLAY       255

  
#define DRUID_OPTION_DUMP               1
#define DRUID_OPTION_STOP               2
#define DRUID_OPTION_START              3
#define DRUID_OPTION_BREAK              4

#define DRUID_MSG_HOOK_TX               1
#define DRUID_MSG_HOOK_RX               2
#define DRUID_PROC_HOOK_CREATE          3
#define DRUID_PROC_HOOK_KILL            4
#define DRUID_PROC_HOOK_SWAP            5
#define DRUID_POOL_HOOK_CREATE          6
#define DRUID_POOL_HOOK_KILL            7

#define DRUID_TIME_UNIT_SEC		1
#define DRUID_TIME_UNIT_MSEC		2
#define DRUID_TIME_UNIT_USEC		3

  
#define DRUID_MARK_END                  1 
#define DRUID_MARK_MORE                 2 
#define DRUID_MARK_ERROR                3 
#define DRUID_MARK_MAGIC       0x12345678 

#ifdef __cplusplus
}
#endif

#endif
