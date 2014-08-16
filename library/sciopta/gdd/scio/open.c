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
** ID: S03070CL33                                                   **
** +Revision: 1.65 +                                                **
** +Date: 2006/05/24 12:54:42 +                                     **
** open fdTabDublicate                                              **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <fcntl.h>
#include <errno.h>
#include <scio.h>
#include <sdd/sdd.h>

#include <string.h>

fd_tab_t NEARPTR
fdTabDuplicate (void)
{
  fd_tab_t NEARPTR fd_tab;
  fd_tab_t NEARPTR dup;
  int i;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC) {
      size_t size = sc_msgSizeGet ((sc_msgptr_t) &fd_tab);
      sc_poolid_t plid = sc_msgPoolIdGet ((sc_msgptr_t) &fd_tab);
      dup = (fd_tab_t NEARPTR) sc_msgAlloc (size, 0, plid, SC_FATAL_IF_TMO);
      memset (dup, 0, size);
      dup->max_fd = fd_tab->max_fd;
      dup->magic = SCIO_MAGIC;
      dup->flags = fd_tab->flags;
      dup->manager = sdd_objDup (fd_tab->manager);
      i = 0;
      while (fd_tab->bf[i] && i < fd_tab->max_fd) {
	dup->bf[i] = sdd_objDup (fd_tab->bf[i]);
	++i;
      }
      return dup;
    }
  }
  return (fd_tab_t NEARPTR)0;
}

int
open (const char *path, int flags, ...)
{
  fd_tab_t NEARPTR fd_tab;
  sdd_obj_t NEARPTR h;
  sdd_obj_t NEARPTR dir;
  const char *devName;
  int fd = 0;

  if (sc_procVarGet (SCIO_PROCVAR_ID, (sc_var_t NEARPTR) & fd_tab)) {
    if (fd_tab && fd_tab->magic == SCIO_MAGIC) {

      sc_miscErrnoSet (0);
      /* resolve path */
      dir = sdd_objResolve (fd_tab->manager, path, &devName);
      
      /*
      ** If the rest is empty, the path points to a directory.
      */ 
      if (!devName || devName[0] == 0) {
	sc_miscErrnoSet (EISDIR);
	return -1;
      }
      /*
      ** The remaining name must not have a path separator
      */
      if ( strchr(devName,'/') ){
	if (dir != fd_tab->manager) {
	  sdd_objFree (&dir);
	}	  
	sc_miscErrnoSet( EINVAL );
	return -1;
      }
      
      h = sdd_manGetByName (dir, devName);

      if (!h ){
	if ( (flags & O_CREAT) && SDD_IS_A (dir, SFS_DIR_TYPE) ) {
	  
	  h = (sdd_obj_t NEARPTR) sc_msgAlloc (sizeof (sdd_obj_t), 
					       SDD_OBJ,
					       SC_DEFAULT_POOL, 
					       SC_FATAL_IF_TMO);
	  
	  strncpy (h->name, devName, SC_NAME_MAX);
	  h->type = SDD_OBJ_TYPE | SDD_DEV_TYPE | SDD_FILE_TYPE;
	  h->base.handle = NEAR_NULL;
	  h->base.error = 0;
	  flags &= (int)~O_CREAT;
	  
	  if (sdd_manAdd (dir, &h) == -1) {
	    if (dir != fd_tab->manager) {
	      sdd_objFree (&dir);
	    }	  
	    return -1;
	  }
	  
	  h = sdd_manGetByName (dir, devName);
	} else {
	  if (dir != fd_tab->manager) {
	    sdd_objFree (&dir);
	  }	  
	  sc_miscErrnoSet( EINVAL );
	  return -1;
	}
      }     
      
      if (dir != fd_tab->manager) {
	sdd_objFree (&dir);
      }
      
      if (h && SDD_IS_A (h, SDD_DEV_TYPE)) {
	while (fd < fd_tab->max_fd && fd_tab->bf[fd]) {
	  ++fd;
	}
	
	if (fd != fd_tab->max_fd) {
	  if ((sdd_devOpen (h, flags)) != -1) {
	    fd_tab->bf[fd] = h;
	    return fd;
	  }
	  else {
	    sdd_objFree (&h);
	    return -1;
	  }
	}
      }
      else {
	if (h) {
	  sdd_objFree (&h);
	}
	sc_miscErrnoSet (ENODEV);
	return -1;
      }
    }
  }
  
  sc_miscErrnoSet (EMFILE);
  return -1;
}

int
creat (const char *path, mode_t mode)
{
  return open(path,O_CREAT|O_WRONLY|O_TRUNC,mode);
}
