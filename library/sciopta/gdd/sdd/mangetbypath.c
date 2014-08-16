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
** ID: S04343CL1                                                    **
** +Revision: 1.5 +                                                 **
** +Date: 2008/01/16 13:02:00 +                                     **
** Get an object by its path                                        **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <errno.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#include <string.h>

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_obj_t object;
  sdd_manGetPath_t getPath;
};

sdd_obj_t NEARPTR 
sdd_manGetByPath (const sdd_obj_t NEARPTR root, const char *path)
{
  const char *name;
  sdd_obj_t NEARPTR folder;
  sdd_obj_t NEARPTR obj = NEAR_NULL;

  if (!root) {
    sc_miscErrnoSet (EINVAL);
    return NEAR_NULL;
  }

  folder = sdd_objResolve (root, path, &name);
  if (!folder) {
    sc_miscErrnoSet (ENOENT);
    return NEAR_NULL;
  }
    
  if (name && name[0] && !strchr(name,'/')) {    
    obj = sdd_manGetByName (folder, name);
    if ( folder != root ){
      sdd_objFree(&folder);
    }
    return obj;
  } else if ( !name || name[0] == 0 ){
    return folder;
  } else {
    sc_miscErrnoSet(EINVAL);
    if ( folder != root ){
      sdd_objFree(&folder);
    }
  }
  return NEAR_NULL;
}

sdd_obj_t NEARPTR 
sdd_manGetByRelPath (const sdd_obj_t NEARPTR self, const char *path)
{
  // XXX: select for both msgid and self->controller !
  static const sc_msgid_t select[2] = {
    SDD_MAN_GET_PATH_REPLY, 
    0
  };

  sc_msg_t msg;
  int len;

  len = strlen(path);
  if ( len > SC_PATH_MAX ){
    sc_miscErrnoSet(EINVAL);
    return NEAR_NULL;
  }

  msg = sc_msgAllocClr (sizeof (sdd_manGetPath_t)+len, 
			SDD_MAN_GET_PATH,
			sc_msgPoolIdGet ((sc_msgptr_t) &self), 
			SC_FATAL_IF_TMO);

  msg->object.manager = self->base.handle;
  (void)strcpy (msg->getPath.path, path);
  sc_msgTx (&msg, self->controller, 0);

  do {
    if (msg) {
      sc_msgFree (&msg);
    }
    msg = sc_msgRx (SC_ENDLESS_TMO,  select, SC_MSGRX_MSGID);
  }
  /* could be an early error which is still in the queue */
  while (msg->object.manager != self->base.handle);

  if (msg->base.error) {
    sc_miscErrnoSet (msg->base.error);
    sc_msgFree (&msg);
    return NEAR_NULL;
  }
  else {
    return &msg->object;
  }
}
