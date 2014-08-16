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
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S05315CL7
** +Revision: 1.5 + 
** +Date: 2006/01/19 13:05:58 + 
** FTP NVT API. 
**********************************************************************
EOC*/

#ifndef _FTP_SERVER_MSG_ 
#define _FTP_SERVER_MSG_ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <sciopta.h>
#include <sdd/sdd.msg>

#define FTP_LOGIN_MAX		31
#define FTP_LOGIN		(FTP_MSG_BASE)
#define FTP_LOGIN_REPLY	        (FTP_LOGIN + 1)	
  typedef struct ftp_login_s {
    sc_msgid_t id;
    sc_errcode_t error; 
    char user[FTP_LOGIN_MAX + 1];
    char pass[FTP_LOGIN_MAX + 1];
  } ftp_login_t;
  
#ifdef __cplusplus
}
#endif

#endif
