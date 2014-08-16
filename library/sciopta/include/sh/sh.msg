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
** ID: S03070CL90                                                   **
** +Revision: 1.12 +                                                **
** +Date: 2004/10/07 07:37:41 +                                     **
** shell messages                                                   **
**********************************************************************
EOC*/

#ifndef _UTIL_SH_MSG_
#define _UTIL_SH_MSG_

#ifdef __cplusplus
extern "C" {
#endif

#include <scio.h>
#include <ossys/types.h>

#define SH_ARG_MAX		31
#define SH_BEGIN		(SH_MSG_BASE)
  typedef struct sh_inherit {
    sc_msgid_t 	msgId;
    fd_tab_t	NEARPTR fd_tab;
    int 	(*main) (int argc, char *argv[]);
    int 	argc;
    char	*argv[1];
  } sh_inherit_t;

#define SH_END			(SH_BEGIN + 1)
  typedef struct sh_dead_s{
      sc_msgid_t id;
      sc_errcode_t err;
  } sh_dead_t;
 
#define SH_FG			(SH_MSG_BASE + 2)
  typedef struct sh_fg_s {
    sc_msgid_t id;
  } sh_fg_t;
  
#ifdef __cplusplus
}
#endif
#endif
