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
** ID: S04147BS19                                                   **
** +Revision: 1.7 +                                                 **
** +Date: 2006/05/23 14:29:14 +                                     **
** Basic object functions: sdd_objResolve                           **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <sciopta.msg>
#include <errno.h>
#include <sdd/sdd.h>
#include <sdd/sdd.msg>

union sc_msg {
  sc_msgid_t id;
  sdd_baseMessage_t base;
  sdd_obj_t object;
};

sdd_obj_t NEARPTR 
sdd_objResolve (const sdd_obj_t NEARPTR self, 
		const char *pathname,
		const char **last)
{
  const char *p;
  char *pt;
  char name[SC_NAME_MAX];
  unsigned int i;
  sdd_obj_t NEARPTR h;
  sdd_obj_t NEARPTR cur;

  /* exception */
  if (pathname[0] == '/' && pathname[1] == '\0') {
    p = pathname;
    ++p;
    *last = p;
    return (sdd_obj_t NEARPTR)self;
  }

  cur = (sdd_obj_t NEARPTR)self;
  p = pathname;

  if (*p == '/') {
    ++p;
  }
  else {
    /* 
    ** get pwd from procVar
    */
  }

  for(;;) {
    /* return start of current part */
    *last = p;

    /* copy over next part of the path*/
    pt = name;
    for (i = 0; *p && *p != '/' && i <= SC_NAME_MAX; ++i) {
      *pt++ = *p++;
    }
    *pt = 0;
    /* if '/', skip it */
    if (*p) {
      ++p;
    }

    if (!i && !*p) {
      return cur;
    }
    /* Check if the current part is a manager. 
    ** If not, return last manager.
    */
    else if (i) {
      h = sdd_manGetByName (cur, name);
      /*
      ** If we got no object or the object is not a manager
      ** destroy the on-the-fly object and return last manager.
      */
      if (!h || !SDD_IS_A (h, SDD_MAN_TYPE)) {
	if (h && h != self) {
	  /* do never destroy the self pointer!! */
	  sdd_objFree (&h);
	}
	return cur;
      }
      /* ok, got a manager, release previous */
      if (cur != self) {
	sdd_objFree (&cur);
      }
      /* new startpoint */
      cur = h;
    }
  }
}

