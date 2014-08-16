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
** ID: S03070CL15                                                   **
** +Revision: 1.35 +                                                **
** +Date: 2006/06/09 14:15:10 +                                     **
** opendir                                                          **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>
#include <errno.h>
#include <sdd/sdd.h>

DIR NEARPTR 
opendir (const char *dir)
{
  fd_tab_t NEARPTR fd_tab;
  sdd_obj_t NEARPTR man;
  const char *last;
  
  DIR NEARPTR dirDesc = (DIR NEARPTR)0;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC) {
      man = sdd_objResolve (fd_tab->manager, dir, &last);
      if (!man || *last) {
	return dirDesc;
      }	
      else if (!SDD_IS_A(man, SDD_MAN_TYPE)) {
	sc_miscErrnoSet (ENOTDIR);
	sdd_objFree (&man);
      }
      else {
	dirDesc =
	  (DIR NEARPTR) sc_msgAlloc (sizeof (DIR), 
				    0,
				    sc_msgPoolIdGet ((sc_msgptr_t) &man),
				    SC_FATAL_IF_TMO);
	dirDesc->man = man;
	dirDesc->cur = (sdd_obj_t NEARPTR)0;
      }
    }
  }

  return dirDesc;
}

