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
** ID: S03094CL1                                                    **
** +Revision: 1.6 +                                                 **
** +Date: 2005/07/01 12:28:32 +                                     **
** Move one file to another location (prepared with create/get)     **
**********************************************************************
EOC*/

/** @name Description
* Start the right shell. 
*/

#include <sciopta.h>
#include <sys/fcntl.h>
#include <scio.h>

#include <errno.h>

#include <sdd/sdd.h>
#include <sdd/sdd.msg>
#include <sfs/sfs.h>
#include <sfs/sfs.msg>

#include <string.h>

#if defined __ARMCC_VERSION
/* suppress: assignment in condition warning */
#pragma diag_suppress 1293
#endif

#define SFS_DIR_CPY_MAX_DEPTH 5

union sc_msg {
  sc_msgid_t id;
  sdd_obj_t object;
  sdd_devRead_t read;
  sdd_devWrite_t write;
};

static int 
sfs_fileObjCpyDel (sdd_obj_t NEARPTR  olddir, sdd_obj_t NEARPTR  oldfile,
                   sdd_obj_t NEARPTR  target, const char *newname, int del)
{
  static const sc_msgid_t selread[2] = { SDD_DEV_READ_REPLY, 0 };
  static const sc_msgid_t selwrite[2] = { SDD_DEV_WRITE_REPLY, 0 };
  sc_msg_t msg;
  sdd_obj_t NEARPTR newfile;

  /* get newfile if exist or create it */
  sfs_create (target, newname, SFS_ATTR_FILE, 0);

  /* open oldfile for reading */
  if (sdd_devOpen (oldfile, O_RDONLY) == -1) {
    return -1;
  }
  /* open newfile for reading */
  if (!(newfile = sdd_manGetByName (target, newname))) {
    return -1;
  }
  if (sdd_devOpen (newfile, O_WRONLY | O_TRUNC) == -1) {
    return -1;
  }
  /* read from oldfile and write to newfile */
  /* set a read request */
  if (sdd_devAread (oldfile, 512) == -1) {
    return -1;
  }
  msg = sc_msgRx (SC_ENDLESS_TMO, (void *) selread, SC_MSGRX_MSGID);
  while (!msg->read.base.error && msg->read.size) {
    msg->id = SDD_DEV_WRITE;
    sc_msgTx (&msg, newfile->sender, 0);
    msg = sc_msgRx (SC_ENDLESS_TMO, (void *) selwrite, SC_MSGRX_MSGID);
    if (msg->read.base.error) {
      sc_msgFree (&msg);
      return -1;
    }
    if (sdd_devAread (oldfile, 512) == -1) {
      return -1;
    }
    msg = sc_msgRx (SC_ENDLESS_TMO, (void *) selread, SC_MSGRX_MSGID);
  }
  sc_msgFree (&msg);

  /* close files */
  sdd_devClose (oldfile);
  sdd_devClose (newfile);
  
  /* remove oldfile */
  if (del) {
    return sdd_manRm (olddir, oldfile, sizeof (sdd_obj_t));
  }
  return 0;
}

/* attention this is a recursive function cut down to SFS_DIR_CPY_MAX_DEPTH
 * recursions!
 */
static int 
sfs_dirObjCpyDel (sdd_obj_t NEARPTR  olddir, sdd_obj_t NEARPTR  target,
                  const char *path, char *curpath, int del, int level)
{
  sdd_obj_t NEARPTR cur, *tmpdir, *old;
  int len;

  if (level >= SFS_DIR_CPY_MAX_DEPTH) {
    sc_miscErrnoSet (EMLINK);
    return -1;
  }

  cur = sdd_manGetFirst (olddir, sizeof (sdd_obj_t));
  while (cur) {
    if (strncmp (cur->name, ".", 2) && strncmp (cur->name, "..", 3)) {
      if (cur->type == SFS_ATTR_FILE) {
        if (sfs_fileObjCpyDel (olddir, cur, target, cur->name, del) == -1) {
	  return -1;
	}
      }
      else if (cur->type == SFS_ATTR_DIR) {
        if (sfs_create (target, cur->name, SFS_ATTR_DIR, 0) == -1) {
	  return -1;
	}
        tmpdir = target;
	if ((target = sdd_manGetByName (tmpdir, cur->name)) == NULL) {
	  return -1;
	}
        len = strlen (curpath);
        strcat (curpath, cur->name);
        strcat (curpath, "/");
        if (sfs_dirObjCpyDel (cur, target, path, curpath, del, level + 1) == -1) {
	  return -1;
	}
        curpath[len] = '\0';
        sdd_objFree (&target);
        target = tmpdir;
        if (del) {
          if (sdd_manRm (olddir, cur, sizeof (sdd_obj_t)) == -1) {
	    return -1;
	  }
        }
      }
    }
    old = cur;
    cur = sdd_manGetNext (olddir, old, sizeof (sdd_obj_t));
    sdd_objFree (&old);
  }
  return 0;
}

static int
sfs_pathCpyDel (sdd_obj_t NEARPTR  fs, const char *oldpath, const char *newpath, 
                int del)
{
  sdd_obj_t NEARPTR oldman = NULL, *newman = NULL, *tmp;
  const char *name, *file;
  char curpath[SC_PATH_MAX + 1];
  int i, ret = 0;

  sc_miscErrnoSet (0);

  /* get oldpath object */
  oldman = sdd_objResolve (fs, oldpath, &file);
  if (file[0] && strstr (file, "/")) {
    sc_miscErrnoSet (ENOTDIR);
    ret = -1;
    goto fin;
  }

  /* get newpath object */
  newman = sdd_objResolve (fs, newpath, &name);
  if (!newman || strstr (name, "/")) {
    sc_miscErrnoSet (ENOENT);
    ret = -1;
    goto fin;
  }

  /* oldpath is a directory and if newpath dir does not exist */
  if (!file[0] && name[0]) {
    /* create missing newman */
    if (sfs_create (newman, name, SFS_ATTR_DIR, 0) == -1) {
      ret = -1;
      goto fin;
    }
    /* get it */
    if ((tmp = sdd_manGetByName (newman, name)) == NULL ) {
      ret = -1;
      goto fin;
    }
    /* release newman */
    sdd_objFree (&newman);
    /* and make tmp to newman */
    newman = tmp;
  }
  
  /* prepare paths to avoid recursivly copy/move of target directory */
  if (oldpath[0] == '/') {
    ++oldpath;
  }
  if (newpath[0] == '/') {
    ++newpath;
  }

  /* prepare curpath as empty string */
  curpath[0] = '\0';

  /* remove parts which are equal from newpath and oldpath */
  i = 0;
  while (!i) {
    /* compare until the next "/" */
    while (i < SC_PATH_MAX && oldpath[i] != '/' && oldpath[i] == newpath[i]) {
      ++i;
    }
    if (oldpath[i] == '/' && oldpath[i] == newpath[i]) {
      oldpath += i + 1;
      newpath += i + 1;
      i = 0;
    }
    else {
      i = 1;
    }
  }
  /* not on the same fs controller, have to copy oldobj to newobj */
  if (file[0]) {
    tmp = sdd_manGetByName (oldman, file);
    ret = sfs_fileObjCpyDel (oldman, tmp, newman, file, del);
    sdd_objFree (&tmp);
  }
  else {
    ret = sfs_dirObjCpyDel (oldman, newman, newpath, curpath, del, 0);
  }

fin:
  if (oldman) {
    sdd_objFree (&oldman);
  }
  if (newman) {
    sdd_objFree (&newman);
  }

  return ret;
}

int
sfs_move (sdd_obj_t NEARPTR  fs, const char *oldpath, const char *newpath)
{
  sc_miscErrnoSet (0);
  return sfs_pathCpyDel (fs, oldpath, newpath, 1);
}

int
sfs_copy (sdd_obj_t NEARPTR  fs, const char *oldpath, const char *newpath)
{
  sc_miscErrnoSet (0);
  return sfs_pathCpyDel (fs, oldpath, newpath, 0);
}

/*
** $Log: sfs_move.c,v $
** Revision 1.6  2005/07/01 12:28:32  christian
** Merged SCIOPTA_1_7_2 to MAIN
**
** Revision 1.5.4.1  2005/05/12 06:01:19  bastian
** fixes for RVCS
**
** Revision 1.5  2004/12/14 09:10:12  christian
** Remove __GNUC__ for nonexisting includes, because this includes no exist.
**
** Revision 1.4  2004/10/07 09:25:28  bastian
** NEAR * => NEARPTR
**
** Revision 1.3  2004/10/04 13:33:58  bastian
** fix for NEAR/FAR
**
** Revision 1.2  2004/07/29 06:00:01  christian
** Bugfix hcc stuff and correct the error handling for move.
**
** Revision 1.1  2004/04/15 11:47:11  christian
** Split up files into single files to have a better scalability on code size
**
** Revision 1.11  2004/04/15 07:38:11  christian
** Clearup the chaos with release/free duple.
**
** Revision 1.10  2004/04/14 13:26:24  christian
** Clean up of helper functions.
**
** Revision 1.9  2004/04/14 09:12:32  christian
** Move directories and files is working :-)
**
** Revision 1.8  2004/04/13 09:43:07  christian
** There is no move message anymore only renaming of a file is possible.
** The move function is finished and is working on a copy base.
**
** Revision 1.7  2004/04/13 05:29:19  christian
** hccffs does only have a rename, means no movement!
**
** Revision 1.6  2004/04/07 14:06:54  christian
** Move a file and/or directory function.
**
** Revision 1.5  2003/11/14 15:14:04  christian
** Integration of hcc's effs 1.20
**
** Revision 1.4  2003/09/08 07:57:16  christian
** ips_device -> ips_dev.
**
** Revision 1.3  2003/09/08 07:07:30  christian
** sdd_device -> sdd_dev.
** a simpler ppp.
** adjusted examples as far as possible.
**
** Revision 1.2  2003/07/31 12:00:36  christian
** sdd_object -> sdd_obj
** sdd_dev -> sdd_dev
** ips_dev -> ips_dev
** SDD_OBJECT -> SDD_OBJ
** SDD_DEVICE -> SDD_DEV
** IPS_DEVICE -> IPS_DEV
**
** Revision 1.1  2003/04/04 07:41:32  christian
** Building up a testable move command.
**
**
**/
