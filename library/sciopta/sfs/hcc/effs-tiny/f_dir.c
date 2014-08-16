/****************************************************************************
 *
 *            Copyright (c) 2005 by HCC Embedded 
 *
 * This software is copyrighted by and is the sole property of 
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,  
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1132
 * Victor Hugo Utca 11-15
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/
#include <hcc/effs-tiny/tiny.h>
#include <hcc/effs-tiny/f_volume.h>
#include <hcc/effs-tiny/f_file.h>
#include <hcc/effs-tiny/port.h>

/**************************************************************************/
/************************* INTERNAL FUNCTIONS *****************************/
/**************************************************************************/

#if F_CHECKNAME
char _f_toupper(char ch) 
{
  return ((ch>='a'&&ch<='z')?(ch-'a'+'A'):ch);
}
#endif

/*
** Checks if a name conatins directory delimiter '/' or '\\'
** RETURN: position if delimiter found, 0 if not found
*/
#if F_CHECKNAME
unsigned char _check_dir_delimiter (const char *name)
{
  unsigned char i;
  for (i=0;name[i] && name[i]!='/' && name[i]!='\\'; i++);
  if (name[i]) return i;
  return (unsigned char)F_INVALID;
}
#endif

/*
** Checks a portion of flash if it is equal to the required string or wildcard name
** INPUT: name - name to look for
**        addr - address in flash
**        fname - where to store real file name
**        wca_on - 1-wildcard allowed
** RETURN: 0-matches / 1-doesn't match
*/
#if F_WILDCARD
static unsigned char _f_cmp_wname (const char *wname, unsigned long addr, char *fname, char wca_on)
#else
static unsigned char _f_cmp_wname (const char *wname, unsigned long addr, char *fname)
#endif
{
  char act;
  char wact;
  unsigned char rc;
  unsigned char fpos=0;
#if F_WILDCARD
  char wild;
  char wild_end=0;
  unsigned char wild_fpos=0;
  const char *wild_wname=NULL;
  char *wild_fname=NULL;
  unsigned long wild_addr;


  wild=0;
  wild_addr=0;
#endif

  for (;;)
  {
    rc=f_flash_read(&act,addr++,1);
    if (rc) return 1;			/* get next character of the original file name */

    if (fpos++==F_MAX_FILE_NAME_LENGTH+1) return 1;	/* something is wrong, zero terminator missing */
    
    if (fname) *fname++=act;		/* set return string if needed */
#if F_CHECKNAME 
    act=_f_toupper(act);
    wact=_f_toupper(*wname);
#else
    wact=*wname;
#endif
    

    if (wname==NULL)			/* return if we are just checking for anything */
    {
      if (act==(char)F_INVALID) return 1;	/* invalid name */
                           else return 0;	/* valid name */
    }
    
    if (act==(char)F_INVALID) return 1;	/* return if invalid file */
    if (act==0) 			/* if end of file name reached */
    {
      while (wact=='?' || wact=='*') wact=*(++wname);	/* skip '?'/'*' in wildcard name */
      if (wact) return 1;		/* if anything left in wildcard name, the compare failed */
      break;				/* otherwise it was successfull */
    }

#if F_WILDCARD
    if (wca_on)		/* if compare is a wildcard compare */
    {
      if (wild) 	/* if found * earlier */
      {
__f_cmp_wname_check_end:
        if (act==wild_end)	/* check if current character is the last character of the wildcard name */
        {
          wild=0;
          wild_fpos=fpos;
          wild_wname=wname++;
          wild_fname=fname;
          wild_addr=addr;	/* if yes, store current positions */
        }
        continue;		/* get next character */
      }
                         
      if (wact=='*') 		/* if wildcard name is * */
      { 
        wild=1; 		/* set * found */
#if F_CHECKNAME 
        while (*wname=='*') ++wname;	/* skip *-s */
        wild_end=_f_toupper(*wname);
#else
        wild_end=*(++wname); 		/* get end character for * */
#endif
        goto __f_cmp_wname_check_end;	/* check whether the current character is an end character */
      }
      else if (*wname!='?' && wact!=act)	/* if wildcard name compare failes and it is not ? */
      {
        if (wild_addr) 		/* check if we had earlier a * in the wildcard name */
        {
          wild=1;
          fpos=wild_fpos;
          wname=wild_wname;
          fname=wild_fname;
          addr=wild_addr;	/* if yes, the go back to that state and try to rematch again */
        }
        else return 1;		/* otherwise compare failed */
      }
      else 
      {
        ++wname;
      }

      continue;
    }
#endif

    if (wact!=act) return 1; 
    ++wname;
  }

  return 0;
}


/*
** _f_addentry
**
** Adds an entry to the directory structure. If no more space left in the directory and
** there is a deleted item, calls mergedir and adds the new entry.
**
** fileid -> F_INVALID - allocate new fileid
**           other - put thta to the new page
*/
F_FILE_ID_TYPE _f_add_entry (const char *filename, F_FILE_ID_TYPE fileid, F_FILE_MGM_PAGE_COUNT_TYPE *page)
{
  F_FILE_MGM_PAGE_COUNT_TYPE mgm_page; 
  F_FILE_ID_TYPE i;
  unsigned char rc,len;

  len=_strlen(filename);
#if F_CHECKNAME
  if (len>F_MAX_FILE_NAME_LENGTH) return (F_FILE_ID_TYPE)F_INVALID;	/* invalid name */
#endif
  ++len;						/* calculate length of the filename */

#if F_CHECKNAME
  if (_check_dir_delimiter(filename)!=(unsigned char)F_INVALID) return (F_FILE_ID_TYPE)F_INVALID;
#endif

  if (fileid==(F_FILE_ID_TYPE)F_INVALID)
  {
    for (i=0;i<F_MAX_FILE && f_volume.file_id_page[i]!=(F_FILE_ID_TYPE)F_INVALID;i++);
    if (i==F_MAX_FILE) return (F_FILE_ID_TYPE)F_INVALID;		/* no more files allowed */
    mgm_page=_f_get_file_mgm_empty_page(i,_FM_STATE_OPEN,0);
  }
  else
  {
    unsigned char seq;
    rc=f_flash_read(&seq,_FILE_ID_MGM_HEADER_ADDR(fileid)+_SADDR(F_FILE_MGM_HEADER,seq),1);
    mgm_page=_f_get_file_mgm_empty_page(fileid,_FM_STATE_OPEN,(unsigned char)(seq+1));
    i=fileid;
  }
  if (mgm_page==(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID) return (F_FILE_ID_TYPE)F_INVALID;

  rc=f_flash_write(_FILE_MGM_DSC_ADDR(mgm_page)+_SADDR(F_FILE_DSC,name[0]),(char *)filename,len);
  if (rc) return (F_FILE_ID_TYPE)F_INVALID;

  if (i!=fileid) f_volume.file_id_page[i]=mgm_page;
  *page=mgm_page;
  return i;
}



/*
** Find a directory with a defined parent directory
** INPUT: dirname - name of the directory
**        id - parent directory ID
** RETURN:ID of the directory, F_INVALID if not available
*/
#if F_DIRECTORY
F_DIR_ID_TYPE _f_find_dir (const char *dirname, F_DIR_ID_TYPE id)
{
  F_DIR_ID_TYPE i,tmp;
  unsigned char rc;

  /* dirname = . | .. */
  if (_strcmp(dirname,_DIR_ACT)==0) return id;
  if (_strcmp(dirname,_DIR_TOP)==0)
  {
    if (id==F_DIR_ROOT) return F_DIR_ROOT;
    rc=f_flash_read(&tmp,_DIR_DSC_ADDR(id)+_SADDR(F_DIR_DSC,dirid),sizeof(F_DIR_ID_TYPE));
    if (rc) return (F_DIR_ID_TYPE)F_INVALID;
    return tmp;
  }

  for (i=0;i<F_MAX_DIR;i++)
  {
    if (i==id) continue;
#if F_WILDCARD
    rc=_f_cmp_wname(dirname,_DIR_DSC_ADDR(i)+_SADDR(F_DIR_DSC,name[0]),NULL,0);
#else
    rc=_f_cmp_wname(dirname,_DIR_DSC_ADDR(i)+_SADDR(F_DIR_DSC,name[0]),NULL);
#endif
    if (rc==0)
    {
      rc=f_flash_read(&tmp,_DIR_DSC_ADDR(i)+_SADDR(F_DIR_DSC,dirid),sizeof(F_DIR_ID_TYPE));
      if (rc) return (F_DIR_ID_TYPE)F_INVALID;
      if (tmp==id) return i; 
    }
  }

  return (F_DIR_ID_TYPE)F_INVALID;
}
#endif



/*
** Find a file with a defined parent directory (if directories are allowed)
** INPUT: filename - name of the file
**        id - parent directory ID (optional)
** RETURN:ID of the file, F_INVALID if not available
*/
#if F_DIRECTORY
F_FILE_ID_TYPE _f_find_file (const char *filename, F_DIR_ID_TYPE dirid)
#else
F_FILE_ID_TYPE _f_find_file (const char *filename)
#endif
{
  F_FILE_ID_TYPE i;
  unsigned char rc;

  for (i=0;i<F_MAX_FILE;i++)
  {
    if (f_volume.file_id_page[i]==(F_FILE_ID_TYPE)F_INVALID) continue;

#if F_DIRECTORY
    {
      F_DIR_ID_TYPE tmp;
      rc=f_flash_read(&tmp,_FILE_ID_MGM_DSC_ADDR(i)+_SADDR(F_FILE_DSC,dirid),sizeof(F_FILE_ID_TYPE));
      if (rc) return (F_FILE_ID_TYPE)F_INVALID;
      if (tmp!=dirid) continue;
    }
#endif
    
#if F_WILDCARD
    rc=_f_cmp_wname(filename,_FILE_MGM_DSC_ADDR(f_volume.file_id_page[i])+_SADDR(F_FILE_DSC,name[0]),NULL,0);
#else
    rc=_f_cmp_wname(filename,_FILE_MGM_DSC_ADDR(f_volume.file_id_page[i])+_SADDR(F_FILE_DSC,name[0]),NULL);
#endif
    if (rc==0) return i;
  }

  return (F_FILE_ID_TYPE)F_INVALID;
}



#if F_DIRECTORY
/*
** find a directory or a file with filename, with parent directory..
** INPUT:  filename - name of the file or directory
**         odirid - parent directory ID
** OUTPUT: par - if not NULL puts the parameter block there
** RETURN: the ID of the file or directory matched, F_INVALID if not available
*/
F_FIND_ID_TYPE _f_find (const char *filename, F_FILE_PAR *par, F_DIR_ID_TYPE odirid)
{
  F_FILE_ID_TYPE fileid;
  F_DIR_ID_TYPE dirid;
  unsigned char rc;

  fileid=_f_find_file(filename,odirid);
  if (fileid==(F_FILE_ID_TYPE)F_INVALID)
  {
    dirid=_f_find_dir(filename,odirid);
    if (dirid!=(F_DIR_ID_TYPE)F_INVALID)
    {
      if (par)
      {
        rc=f_flash_read(par,_DIR_DSC_ADDR(dirid)+_SADDR(F_DIR_DSC,par),sizeof(F_DIR_PAR));
        if (rc) return rc;
        par->length=0;
      }
      return (F_FIND_ID_TYPE)dirid;
    }
    else return (F_FIND_ID_TYPE)F_INVALID;
  }
  else
  {
    if (par)
    {
      rc=f_flash_read(par,_FILE_ID_MGM_DSC_ADDR(fileid)+_SADDR(F_FILE_DSC,par),sizeof(F_FILE_PAR));
      if (rc) return rc;
    }
  }

  return (F_FIND_ID_TYPE)fileid;
}
#else
/*
** find a file with filename, with parent directory..
** INPUT:  filename - name of the file or directory
** OUTPUT: par - if not NULL puts the parameter block there
** RETURN: the ID of the file matched, F_INVALID if not available
*/
F_FIND_ID_TYPE _f_find (const char *filename, F_FILE_PAR *par)
{
  F_FILE_ID_TYPE fileid;
  unsigned char rc;

  fileid=_f_find_file(filename);
  if (fileid!=(F_FILE_ID_TYPE)F_INVALID)
  {
    if (par)
    {
      rc=f_flash_read(par,_FILE_ID_MGM_DSC_ADDR(fileid)+_SADDR(F_FILE_DSC,par),sizeof(F_FILE_PAR));
      if (rc) return rc;
    }
    return (F_FIND_ID_TYPE)fileid;
  }

  return (F_FIND_ID_TYPE)F_INVALID;
}
#endif




/*
** Return current dir obtained from name.
** INPUT:  path - path to check
** OUTPUT: if filename present name is handled as dir/filename and returns file name start position
**         in filename
** RETURN: directory ID to path, F_INVALID if doesn't exist
*/
#if F_DIRECTORY
F_DIR_ID_TYPE _f_check_path (const char *path, char **filename)
{
  F_DIR_ID_TYPE rc;
  const char *start=NULL;
  char name[F_MAX_FILE_NAME_LENGTH+1];

  if (filename) *filename=0;
  if (*path=='/' || *path=='\\') 
  {
    rc=F_DIR_ROOT;
#if F_CHECKNAME 
    while (*path && (*path=='/' || *path=='\\')) ++path;
#else
    ++path;
#endif
  }
  else 
  {
    rc=f_volume.current_dir;
  }
  
  if (*path)
  {                         
    for (start=path;;path++)
    {
      /* if actual char is / or \ or form was a/b and b is supposed to be a directory */
      if (*path=='/' || *path=='\\' || (*path==0 && filename==NULL))
      { 
        (void)_memcpy(name,start,path-start);
        name[path-start]=0;
#if F_CHECKNAME 
        while (*path && (*(path+1)=='/' || *(path+1)=='\\')) ++path;
#endif
        if (filename && *(path+1)==0) break;	/* if form was a/b/ and filename not zero -> filename="b/" */
        rc=_f_find_dir(name,rc);		/* find directory */
        if (rc==(F_DIR_ID_TYPE)F_INVALID) return rc;
        if (*(path+1)==0) break;		/* if form was a/b/ and b/ directory found stop */
        start=path+1;				/* set next name start pointer */
      }

      if (*path==0) break;			/* stop if end of path reached (form: a/b) */
    }
  }

  if (filename) *filename=(char *)start;
  return rc;
}
#endif




/*
** _f_find_file_wcard
** 
** Find a file using wild card (*,?). If a file is found using find->findname then the
** parameters of it are setted in the find structure, otherwise find->curpos is set to
** invalid.
**
** INPUT : *find - pointer to F_FIND structure
*/
#if F_FIND_ENABLE
static void _f_find_file_wcard(F_FIND *find)
{
  unsigned long addr;
  unsigned char rc;

#if F_DIRECTORY
  F_DIR_ID_TYPE dirid;

  for (;find->findpos<(F_MAX_DIR+F_MAX_FILE);find->findpos++)
#else
  for (;find->findpos<F_MAX_FILE;find->findpos++)
#endif
  {
#if F_DIRECTORY
    if (find->findpos<F_MAX_DIR)
    {
      addr=_DIR_DSC_ADDR(find->findpos);
    }
    else
    {
      if (_FILE_ID_MGM_PAGE(find->findpos-F_MAX_DIR)==(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID) continue;
      addr=_FILE_ID_MGM_DSC_ADDR(find->findpos-F_MAX_DIR);
    }
#else
    if (f_volume.file_id_page[find->findpos]==(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID) continue;
    addr=_FILE_ID_MGM_DSC_ADDR(find->findpos);
#endif

#if F_DIRECTORY
    rc=f_flash_read(&dirid,addr+_SADDR(F_FILE_DSC,dirid),sizeof(F_DIR_ID_TYPE));
    if (rc) break;
    if (dirid==find->dirid)
#endif
    {
#if F_WILDCARD
      if (_f_cmp_wname(find->findname,addr+_SADDR(F_FILE_DSC,name[0]),find->filename,1)==0)
#else
      if (_f_cmp_wname(find->findname,addr+_SADDR(F_FILE_DSC,name[0]),find->filename)==0)
#endif
      {
#if F_DIRECTORY
        if (find->findpos<F_MAX_DIR)
        {
          F_DIR_PAR dp;
          rc=f_flash_read(&dp,addr+_SADDR(F_DIR_DSC,par),sizeof(F_DIR_PAR));
          if (rc) break;
          find->attr=dp.attr;
          find->ctime=dp.ctime;
          find->cdate=dp.cdate;
          find->length=0;
        }
        else
#endif
        {
          F_FILE_PAR fp;
          rc=f_flash_read(&fp,addr+_SADDR(F_FILE_DSC,par),sizeof(F_FILE_PAR));
          if (rc) break;
          find->attr=fp.attr;
          find->ctime=fp.ctime;
          find->cdate=fp.cdate;
          find->length=fp.length;
        }

        ++find->findpos;
        return;
      }
    }
  }

  find->findpos=(F_FIND_ID_TYPE)F_INVALID;
}
#endif


/**************************************************************************/
/*************************** USER FUNCTIONS *******************************/
/**************************************************************************/
/*
** f_findfirst
** 
** find first time a file using wildcards
**
** INPUT : filename - name of the file 
**         *find - pointer to a pre-define F_FIND structure
** RETURN: F_NOERR - on success
**         F_ERR_NOTFOUND - if not found
*/
#if F_FIND_ENABLE
unsigned char f_findfirst(const char *filename,F_FIND *find) 
{
  if (f_volume.state==F_STATE_NOTFORMATTED) return F_ERR_NOTFOUND;
#if F_DIRECTORY
  find->dirid=_f_check_path(filename,&(find->findname));
  if (find->dirid==(F_DIR_ID_TYPE)F_INVALID) return F_ERR_NOTFOUND;
#else
  find->findname=(char *)filename;
#endif
  find->findpos=0;
  return f_findnext(find);
}
#endif



/*
** f_findnext
** 
** find next time a file using wildcards
**
** INPUT : *find - pointer to a pre-define F_FIND structure
** RETURN: F_NOERR - on success
**         F_ERR_NOTFOUND - if not found
*/
#if F_FIND_ENABLE
unsigned char f_findnext(F_FIND *find) 
{
  if (f_volume.state==F_STATE_NOTFORMATTED) return F_ERR_NOTFOUND;
  _f_find_file_wcard(find);
  if (find->findpos==(F_FIND_ID_TYPE)F_INVALID) return F_ERR_NOTFOUND;
  return F_NOERR;
}
#endif


/*
** f_filelength
**
** Get the length of a file
**
** INPUT : filename - name of the file
** RETURN: size of the file or F_ERR_INVALID if not exists or volume not working
*/
#if F_FILELENGTH
unsigned long f_filelength (const char *filename) 
{
  F_FILE_PAR par;

  if (f_volume.state==F_STATE_NOTFORMATTED) return (unsigned long)F_ERR_INVALID;
#if F_DIRECTORY
  {
    F_DIR_ID_TYPE dirid;
    char *tmp;
    dirid=_f_check_path(filename,&tmp);
    if (_f_find(tmp,&par,dirid)==(F_FIND_ID_TYPE)F_INVALID) return (unsigned long)F_ERR_INVALID;
  }
#else
  if (_f_find(filename,&par)==(F_FIND_ID_TYPE)F_INVALID) return (unsigned long)F_ERR_INVALID;
#endif
  return par.length;
}
#endif



/*
** f_gettimedate
**
** Gets time and date for a file.
**
** INPUT : filename - name of the file
** OUTPUT: *pctime - where to strore time
**         *pcdata - where to store date
** RETURN: F_NOERR on success, other if error
*/
#if F_GETTIMEDATE
int f_gettimedate(const char *filename,unsigned short *pctime,unsigned short *pcdate) 
{
  F_FILE_PAR par;

  if (f_volume.state==F_STATE_NOTFORMATTED) return (unsigned long)F_ERR_NOTFORMATTED;
#if F_DIRECTORY
  {
    F_DIR_ID_TYPE dirid;
    char *tmp;
    dirid=_f_check_path(filename,&tmp);
    if (_f_find(tmp,&par,dirid)==(F_FIND_ID_TYPE)F_INVALID) return (int)F_ERR_NOTFOUND;
  }
#else
  if (_f_find(filename,&par)==(F_FIND_ID_TYPE)F_INVALID) return (int)F_ERR_NOTFOUND;
#endif
  if (pctime) *pctime=par.ctime;
  if (pcdate) *pcdate=par.cdate;
	
  return F_NOERR;
}
#endif


/*
** f_getfreespace
**
** Get free space on the volume
**
** OUTPUT: *sp - pre-defined F_SPACE structure, where information will be stored
** RETURN: F_NOERR - on success
**         F_ERR_NOTFORMATTED - if volume is not formatted
*/
#if F_GETFREESPACE
unsigned char f_getfreespace (F_SPACE *sp)
{
  F_CLUSTER_COUNT_TYPE i;

  if (f_volume.state==F_STATE_NOTFORMATTED) return F_ERR_NOTFORMATTED;
  sp->total=F_SIZE;
  sp->free=0;
  for (i=0;i<F_CLUSTER_COUNT;i++)
  {
    if (_GET_BIT(f_volume.cluster_table,i)==0) ++sp->free;
  }
  sp->free*=F_CLUSTER_SIZE;
  return F_NOERR;
}
#endif



/*
** f_delete
**
** Delete a file. Removes the chain that belongs to the file and inserts a new descriptor
** to the directory with first_cluster set to 0.
**
** INPUT : filename - name of the file to delete
** RETURN: F_NOERR on success, other if error.
*/
#if F_DELETE
int f_delete(const char *filename)
{
  F_FILE_ID_TYPE fileid;
  unsigned char rc;
#if F_DIRECTORY
    F_DIR_ID_TYPE dirid;
#endif

  if (f_volume.state==F_STATE_NOTFORMATTED) return F_ERR_NOTFORMATTED;
#if F_DIRECTORY
  {
    char *tmp;
    dirid=_f_check_path(filename,&tmp);
    fileid=_f_find_file(tmp,dirid);
  }
#else
  fileid=_f_find_file(filename);
#endif
  if (fileid==(F_FILE_ID_TYPE)F_INVALID) return F_ERR_NOTFOUND;
  if (_f_checkopen(fileid)) return F_ERR_OPENED;
  rc=_f_remove_file_mgm(_FILE_ID_MGM_PAGE(fileid),1);
  if (rc) return rc;
  _FILE_ID_MGM_PAGE(fileid)=(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID;

#if F_FIND_ENABLE && F_CHANGE_NOTIFY
  {
    F_FIND_ID_TYPE i;
    for (i=0;i<_CHANGE_TABLE_SIZE;f_volume.changed[i++]=0xff);
  }
#endif

  return F_NOERR;
}
#endif


/*
** f_rename
**
** Rename a file.
**
** INPUT : oldname - old name of the file
**         newname - new name of the file
** RETURN: F_NOERR on success, other if error
*/
#if F_RENAME
int f_rename(const char *oldname, const char *newname)
{
  unsigned char rc;
#if F_DIRECTORY
  F_DIR_ID_TYPE dirid,fdirid;
#endif
  char *filename;
  unsigned long addr=(unsigned long)F_INVALID;

  if (f_volume.state==F_STATE_NOTFORMATTED) return F_ERR_NOTFORMATTED;
#if F_DIRECTORY
#if F_CHECKNAME
  if (_check_dir_delimiter(newname)!=(unsigned char)F_INVALID) return F_ERR_INVNAME;
  if (_strlen(newname)>F_MAX_FILE_NAME_LENGTH) return F_ERR_INVNAME;
#endif
  dirid=_f_check_path(oldname,&filename);
  if (dirid==(F_DIR_ID_TYPE)F_INVALID) return F_ERR_NOTFOUND;
  if (_f_find(newname,NULL,dirid)!=(F_FIND_ID_TYPE)F_INVALID) return F_ERR_DUPLICATED;
#else
  if (_f_find(newname,NULL)!=(F_FIND_ID_TYPE)F_INVALID) return F_ERR_DUPLICATED;
  filename=(char *)oldname;
#endif

#if F_DIRECTORY
  fdirid=_f_find_dir(filename,dirid);
  if (fdirid!=(F_DIR_ID_TYPE)F_INVALID)
  {
    dirid=f_volume.current_dir;
    while (dirid!=F_DIR_ROOT)
    {
      if (dirid==fdirid) return F_ERR_OPENED;
      rc=f_flash_read(&dirid,_DIR_DSC_ADDR(dirid)+_SADDR(F_DIR_DSC,dirid),sizeof(F_DIR_ID_TYPE));
      if (rc) return rc;
    }
    if (_f_checkopendir(fdirid)) return F_ERR_OPENED;
    addr=_DIR_DSC_ADDR(fdirid)+_SADDR(F_DIR_DSC,name[0]);
  }
  else
#endif
  {
    F_FILE_ID_TYPE fileid;
#if F_DIRECTORY
    fileid=_f_find_file(filename,dirid);
#else
    fileid=_f_find_file(filename);
#endif
    if (fileid!=(F_FILE_ID_TYPE)F_INVALID)
    {
      if (_f_checkopen(fileid)) return F_ERR_OPENED;
      addr=_FILE_ID_MGM_DSC_ADDR(fileid)+_SADDR(F_FILE_DSC,name[0]);
    }
  }

  if (addr!=(unsigned long)F_INVALID)
  {
    rc=f_flash_write_safe(addr,(char *)newname,F_MAX_FILE_NAME_LENGTH+1);
    if (rc) return rc;
  }
  else return F_ERR_NOTFOUND;

  return F_NOERR;
}
#endif



/*
** f_mkdir
**
** Create a directory
**
** INPUT:  path - new directory path
** RETURN: 0 - on success, other if error
*/ 
#if (F_DIRECTORY && F_MKDIR)
int f_mkdir (const char *path)
{
  F_DIR_DSC dsc;
  F_DIR_ID_TYPE dirid,ndirid;
  char *name;
  unsigned char rc;

  if (f_volume.state==F_STATE_NOTFORMATTED) return F_ERR_NOTFORMATTED;
  dirid=_f_check_path(path,&name);
  if (name && *name)
  {
    (void)_memcpy(dsc.name,name,F_MAX_FILE_NAME_LENGTH+1);
#if F_CHECKNAME
    {
      unsigned char pos;
      if (_strlen(name)>F_MAX_FILE_NAME_LENGTH) return F_ERR_INVNAME;
      pos=_check_dir_delimiter(dsc.name);
      if (pos!=(unsigned char)F_INVALID) dsc.name[pos]=0;
    }
#endif

    if (_f_find(dsc.name,NULL,dirid)==(F_FIND_ID_TYPE)F_INVALID)
    {
      ndirid=_f_get_dir_entry();
      if (ndirid!=(F_DIR_ID_TYPE)F_INVALID)
      {
        dsc.par.attr=F_ATTR_DIR;
        dsc.par.ctime=dsc.par.cdate=0;
        rc=get_timedate(&(dsc.par.ctime),&(dsc.par.cdate));
        if (rc) return rc;
        dsc.dirid=dirid;
        rc=f_flash_write_safe(_DIR_DSC_ADDR(ndirid),&dsc,sizeof(F_DIR_DSC));
#if F_FIND_ENABLE && F_CHANGE_NOTIFY
        if (rc==0)
        {
          _SET_BIT(f_volume.changed,ndirid);
        }
#endif
        return rc;
      }
      return F_ERR_NOMOREENTRY;
    }
    return F_ERR_DUPLICATED;
  }
  
  return F_ERR_INVDIR;
}
#endif



/*
** f_rmdir
**
** Removes a directory
**
** INPUT:  path - path to remove
** RETURN: 0 - on success, other if error
*/
#if (F_DIRECTORY && F_RMDIR)
int f_rmdir (const char *path)
{
  F_DIR_ID_TYPE dirid;
  char del_flag=(char)F_INVALID;

  if (f_volume.state==F_STATE_NOTFORMATTED) return F_ERR_NOTFORMATTED;
  dirid=_f_check_path(path,NULL);
  if (dirid!=(F_DIR_ID_TYPE)F_INVALID)
  {
    if (f_volume.current_dir!=dirid)
    {
      if (_f_find(NULL,NULL,dirid)==(F_FIND_ID_TYPE)F_INVALID)
      {
        unsigned char rc;
        rc=f_flash_write_safe(_DIR_DSC_ADDR(dirid)+_SADDR(F_DIR_DSC,name[0]),&del_flag,1);
#if F_FIND_ENABLE && F_CHANGE_NOTIFY
        if (rc==0)
        {
          F_FIND_ID_TYPE i;
          for (i=0;i<_CHANGE_TABLE_SIZE;f_volume.changed[i++]=0xff);
        }
#endif
        return rc;
      }
    }
    else return F_ERR_ACCESSDENIED;
  }

  return F_ERR_NOTFOUND;
}
#endif



/*
** f_chdir
**
** Change to a directory
**
** INPUT:  path - path to the dircetory
** RETURN: 0 - on success, other if error
*/
#if (F_DIRECTORY && F_CHDIR)
int f_chdir (const char *path)
{
  F_DIR_ID_TYPE dirid;
  if (f_volume.state==F_STATE_NOTFORMATTED) return F_ERR_NOTFORMATTED;
  dirid=_f_check_path(path,NULL);
  if (dirid!=(F_DIR_ID_TYPE)F_INVALID)
  {
    f_volume.current_dir=dirid;
    return F_NOERR;
  }

  return F_ERR_NOTFOUND;
}
#endif



/*
** f_getcwd
**
** Get current working directory
**
** INPUT:  maxlen - maximum length allowed
** OUTPUT: path - current working directory
** RETURN: 0 - on success, other if error
*/
#if (F_DIRECTORY && F_GETCWD)
int f_getcwd (char *path, int maxlen)
{
  char tmp[F_MAX_FILE_NAME_LENGTH+1];
  F_DIR_ID_TYPE dirid;
  unsigned char rc;
  unsigned char len;

  if (maxlen<2) return F_ERR_INVSIZE;
  *path++='/';
  *path=0;
  maxlen-=2;
  
  dirid=f_volume.current_dir;
  while (dirid!=F_DIR_ROOT)
  {
    rc=f_flash_read(tmp,_DIR_DSC_ADDR(dirid)+_SADDR(F_DIR_DSC,name[0]),F_MAX_FILE_NAME_LENGTH+1);
    if (rc) return rc;
    len=_strlen(tmp);
    if (maxlen<len+1) return F_ERR_INVSIZE;
    (void)_memmove(path+len+1,path,_strlen(path)+1);
    (void)_memcpy(path,tmp,len);
    *(path+len)='/';
    maxlen-=(len+1);
    rc=f_flash_read(&dirid,_DIR_DSC_ADDR(dirid)+_SADDR(F_DIR_DSC,dirid),sizeof(F_DIR_ID_TYPE));
    if (rc) return rc;
  }

  return F_NOERR;
}
#endif

