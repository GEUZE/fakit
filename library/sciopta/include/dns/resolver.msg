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
** ID: S01332CL2                                                    **
** +Revision: 2.5 +                                                 **
** +Date: 2005/01/19 11:06:00 +                                     **
** Signal Interface for the resolver process                        **
**********************************************************************
EOC*/

#ifndef _DNS_RESOLVER_MSG_
#define _DNS_RESOLVER_MSG_



/** @name Includes
*/
/*@{*/
#include <sciopta.h>
#include <dns/parser.h>
/*@}*/

#define DNS_IP_REQUEST_MSG	(DNS_MSG_BASE + 1)	
/* dns_names_t */

#define DNS_IP_REPLY_MSG 	(DNS_IP_REQUEST_MSG + 1)	
/* dns_ip_t */

#define DNS_NAME_REQUEST_MSG	(DNS_MSG_BASE + 3)
/* dns_ip_t */

#define DNS_NAME_REPLY_MSG	(DNS_NAME_REQUEST_MSG + 1)
/* dns_name_t */

#endif

