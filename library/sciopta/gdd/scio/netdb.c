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
** ID: S03070CL27                                                   **
** +Revision: 1.26 +                                                 **
** +Date: 2005/09/16 11:36:21 +                                     **
** gethostbyname gethostbyaddr getprotobyname                       **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <dns/resolver.msg>
#include <dns/parser.h>

extern char *resolver;

union sc_msg {
  sc_msgid_t id;
  dns_name_t ipRequest;
  dns_ip_t nameRequest;
  dns_ip_t ipReply;
  dns_name_t nameReply;
};

static struct hostent *
getHostent (void)
{
  fd_tab_t NEARPTR fd_tab;
  int i;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (!fd_tab->hostent) {
      struct hostent *hostent =
	(struct hostent *) sc_msgAlloc (sizeof (struct hostent), 0,
					SC_DEFAULT_POOL, SC_ENDLESS_TMO);

      hostent->h_name =
	(char *) sc_msgAlloc (DNS_NAME_MAX + 1, 0, SC_DEFAULT_POOL,
			      SC_ENDLESS_TMO);
      hostent->h_addrtype = AF_INET;
      hostent->h_aliases =
	(char **) sc_msgAlloc (sizeof (sc_msgid_t), 0, SC_DEFAULT_POOL,
			       SC_ENDLESS_TMO);
      hostent->h_aliases[0] = NULL;
      hostent->h_addr_list =
	(char **) sc_msgAlloc (10 * sizeof (char *), 0, SC_DEFAULT_POOL,
			       SC_ENDLESS_TMO);

      for (i = 0; i < 10; i++) {
	*hostent->h_addr_list =
	  (char *) sc_msgAlloc (DNS_NAME_MAX + 1, 0, SC_DEFAULT_POOL,
				SC_ENDLESS_TMO);
      }

      fd_tab->hostent = hostent;
    }
    return fd_tab->hostent;
  }
  else {
    sc_miscErrnoSet (ENOENT);
    return NULL;
  }
}

struct hostent *
gethostbyname (const char *name)
{
  struct hostent *hostent;
  sc_msg_t msg;

  static const sc_msgid_t select[3] = {
    SDD_ERROR, DNS_IP_REPLY_MSG, 0
  };
  sc_pid_t to = sc_procIdGet (resolver, SC_NO_TMO);
  
  if (to != SC_ILLEGAL_PID) {
    int error;
    msg =
      sc_msgAlloc (sizeof (dns_name_t), DNS_IP_REQUEST_MSG,
		   SC_DEFAULT_POOL, SC_ENDLESS_TMO);
    msg->ipRequest.noOf = 1;
    strncpy ((char *) msg->ipRequest.entry[0].name, name, DNS_NAME_MAX);
    sc_msgTx (&msg, to, 0);
    msg = sc_msgRx (SC_ENDLESS_TMO,  select, SC_MSGRX_MSGID);
    error = (int)msg->ipReply.parent.error;

    if (!error && msg->ipReply.noOf) {
      int i;
      hostent = getHostent ();

      if (hostent) {
	hostent->h_length = msg->ipReply.entry[0].ip.len;
	for (i = 0; i < msg->ipReply.noOf; i++) {
	  memcpy (hostent->h_addr_list[i], msg->ipReply.entry[0].ip.addr,
		  hostent->h_length);
	}
      }
      return hostent;
    }
    else {
      sc_msgFree (&msg);
      if (error) {
	sc_miscErrnoSet (error);
      }
      else {
	sc_miscErrnoSet (ENOENT);
      }
      return NULL;
    }
  }
  else {
    sc_miscErrnoSet (ENOENT);
    return NULL;
  }
}

struct hostent *
gethostbyaddr (const char *addr, int len, int type)
{
  struct hostent *hostent;
  sc_msg_t msg;
  int i;

  static const sc_msgid_t select[3] = {
    SDD_ERROR, DNS_NAME_REPLY_MSG, 0
  };
  sc_pid_t to = sc_procIdGet (resolver, SC_NO_TMO);

  if (len <= 16 && type == AF_INET && to != SC_ILLEGAL_PID) {
    int error;
    msg =
      sc_msgAlloc (sizeof (dns_ip_t), DNS_NAME_REQUEST_MSG,
		   SC_DEFAULT_POOL, SC_ENDLESS_TMO);
    msg->nameRequest.noOf = 1;
    msg->nameRequest.entry[0].ip.len = 4;

    for (i = 0; i < 4; i++) {
      msg->nameRequest.entry[0].ip.addr[i] =(__u8) atoi (addr);
      addr = strstr (addr, ".") + 1;
    }

    sc_msgTx (&msg, to, 0);
    msg = sc_msgRx (SC_ENDLESS_TMO,  select, SC_MSGRX_MSGID);
    error =(int) msg->ipReply.parent.error;
    if (!error && msg->nameReply.noOf) {
      hostent = getHostent ();
      if (hostent) {
	hostent->h_length = len;
	strncpy ((char *) hostent->h_name,
		 (const char *) msg->nameReply.entry[0].name, DNS_NAME_MAX);
	hostent->h_name[DNS_NAME_MAX] = 0;
      }
      return hostent;
    }
    else {
      sc_msgFree (&msg);
      if (error) {
	sc_miscErrnoSet (error);
      }
      else {
	sc_miscErrnoSet (ENOENT);
      }
      return NULL;
    }
  }
  else {
    return NULL;
  }
}

void sethostent (int stayopen);
void endhostent (void);

struct protoent *
getprotobyname (const char *name)
{
  fd_tab_t NEARPTR fd_tab;
  struct protoent *protoent;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (!fd_tab->protoent) {
      protoent =
	(struct protoent *) sc_msgAlloc (sizeof (struct hostent), 0,
					 SC_DEFAULT_POOL, SC_ENDLESS_TMO);
      protoent->p_name =
	(char *) sc_msgAlloc (SC_NAME_MAX, 0, SC_DEFAULT_POOL,
			      SC_ENDLESS_TMO);
      protoent->p_aliases =
	(char **) sc_msgAlloc (sizeof (sc_msgid_t), 0, SC_DEFAULT_POOL,
			       SC_ENDLESS_TMO);
      protoent->p_aliases[0] = NULL;
      fd_tab->protoent = protoent;
    }
    else {
      protoent = fd_tab->protoent;
    }
    if (strncmp (name, "ip", 3) == 0) {
      protoent->p_proto = SOL_IP;
    }
    else if (strncmp (name, "icmp", 5) == 0) {
      protoent->p_proto = SOL_ICMP;
    }
    else if (strncmp (name, "tcp", 4) == 0) {
      protoent->p_proto = SOL_TCP;
    }
    else if (strncmp (name, "udp", 4) == 0) {
      protoent->p_proto = SOL_UDP;
    }
    else if (strncmp (name, "raw", 4) == 0) {
      protoent->p_proto = SOL_RAW;
    }
    else {
      return NULL;
    }
    return protoent;
  }
  else {
    return NULL;
  }
}

