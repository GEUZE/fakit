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
** ID: S03070CL40                                                   **
** +Revision: 1.30 +                                                 **
** +Date: 2006/12/06 11:41:39 +                                     **
** readdir                                                          **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <scio.h>
#include <sdd/sdd.h>
#include <string.h>
#include <sys/dirent.h>

struct dirent NEARPTR 
readdir (DIR NEARPTR dir)
{
  sdd_obj_t NEARPTR old;

  if (!dir) {
    return (struct dirent NEARPTR)0;
  }
  else {  
    if ( !dir->cur ){
      dir->cur = sdd_manGetFirst (dir->man, sizeof (sdd_obj_t));  
    } else {
      old = dir->cur;
      dir->cur = sdd_manGetNext (dir->man, dir->cur, sizeof (sdd_obj_t));
      sdd_objFree (&old);
    }      
    if ( dir->cur ){
      strncpy (dir->dirent.d_name,(const char *) dir->cur->name, SC_NAME_MAX);
      if ( (dir->cur->type == SFS_ATTR_DIR) 
	   || (dir->cur->type & (SDD_MAN_TYPE | SFS_DIR_TYPE))){
	dir->dirent.d_type = DT_DIR;
      } else {
	dir->dirent.d_type = DT_CHR;
      }
    } else {
      return (struct dirent NEARPTR)0;
    }
  }
  return &dir->dirent;
}

