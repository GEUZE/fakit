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
#include <hcc/effs-tiny/f_dir.h>
#include <hcc/effs-tiny/port.h>

/**************************************************************************/
/************************* INTERNAL FUNCTIONS *****************************/
/**************************************************************************/

/*
** _f_checkopen
**
** Check if file is already opened or not
**
** INPUT : depos - directory position
** RETURN: 1-opened / 0-not opened
*/
unsigned char _f_checkopen (F_FILE_ID_TYPE fileid)
{
  unsigned char i;
  for (i=0;i<F_MAX_OPEN_FILE;i++)
  {
    if (files[i].mode!=F_MODE_CLOSE && files[i].fileid==fileid) return 1;
  }
  return 0;
}


/*
** _f_checkopendir
**
** Check if file is already opened with define directory ID 
**
** INPUT : depos - directory position
** RETURN: 1-opened / 0-not opened
*/
#if F_DIRECTORY
unsigned char _f_checkopendir (F_DIR_ID_TYPE dirid)
{
  unsigned char i;
  for (i=0;i<F_MAX_OPEN_FILE;i++)
  {
    if (files[i].mode!=F_MODE_CLOSE && files[i].dirid==dirid) return 1;
  }
  return 0;
}
#endif


/*
** _f_getfreefile
**
** Gets a free file descriptor if available
**
** RETURN: file descriptor of F_INVALID if not available
*/
F_FILE *_f_getfreefile (void)
{
  unsigned char i;
  F_FILE *file=NULL;

  /* get free file entry */
  for (i=0;i<F_MAX_OPEN_FILE;i++)
  {
    if (files[i].mode==F_MODE_CLOSE)
    {
      file=&files[i];
      (void)_memset(file,0,sizeof(F_FILE));
#if (F_SEEK_WRITE)
      file->orig_cluster=(F_CLUSTER_COUNT_TYPE)F_INVALID;
#endif
      file->cluster_pos=F_CLUSTER_SIZE;
      break;
    }
  }

  return file;
}

/**************************************************************************/
/*************************** USER FUNCTIONS *******************************/
/**************************************************************************/

/*
** f_open
**
** open a file 
**
** INPUT : filename - file to be opened
**         mode - open method (r,w,a,r+,w+,a+)
** RETURN: pointer to a file descriptor or 0 on error
*/
F_FILE *f_open(const char *path,const char *mode) 
{
  F_FILE *file=NULL;
  F_FILE_PAR par;
  F_FIND_ID_TYPE id;
  unsigned char reqmode=F_MODE_CLOSE;
  unsigned char rc=0;
  char *filename;


  if (f_volume.state==F_STATE_NOTFORMATTED) return NULL;

  if (mode[1]==0 || (mode[1]=='+' && mode[2]==0))
  {
    if (mode[0]=='r') reqmode|=F_MODE_READ;
    if (mode[0]=='w') reqmode|=F_MODE_WRITE;
    if (mode[0]=='a') reqmode|=F_MODE_APPEND;
    if (mode[1]=='+') reqmode|=F_MODE_PLUS;	/* set open mode */
  }
  else return NULL;

  if ((reqmode&(F_MODE_READ|F_MODE_WRITE|F_MODE_APPEND))==0) return NULL;

  file=_f_getfreefile();
  if (!file) return NULL;	/* no more files available */

  /* check if filename is available */
#if F_DIRECTORY
  file->dirid=_f_check_path(path,&filename);
  if (file->dirid==(F_DIR_ID_TYPE)F_INVALID) return NULL;
  if (filename==NULL || *filename==0) return NULL;
  id=_f_find(filename,&par,file->dirid);
#else
  filename=(char *)path;
  id=_f_find(filename,&par);
#endif
  if (id!=(F_FIND_ID_TYPE)F_INVALID)
  {
#if F_DIRECTORY
    if (par.attr&F_ATTR_DIR) return NULL;
#endif
    file->fileid=(F_FILE_ID_TYPE)id;
    file->first_mgm_page=_FILE_ID_MGM_PAGE((F_FILE_ID_TYPE)id);
    file->length=par.length;
  }

  switch (reqmode&(~F_MODE_PLUS))
  {
    case F_MODE_READ:
        if (id==(F_FIND_ID_TYPE)F_INVALID) return NULL;	/* file not available */
        if ((reqmode&F_MODE_PLUS)==0) break;

    case F_MODE_APPEND:
        if (id!=(F_FIND_ID_TYPE)F_INVALID)
        {
          if (_f_checkopen((F_FILE_ID_TYPE)id)) return NULL;
          file->first_mgm_page=_f_copy_mgm(file->first_mgm_page);	/* copy management area of the original file */
          if (file->first_mgm_page==(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID) return NULL;
          file->act_mgm_page=file->first_mgm_page;
          if (reqmode&F_MODE_APPEND) (void)f_seek(file,0,F_SEEK_END);	/* seek to the end of the file if append (not r+) */
          break;
        }

    case F_MODE_WRITE:
        if (id==(F_FIND_ID_TYPE)F_INVALID)
        {
          file->fileid=_f_add_entry(filename,(F_FILE_ID_TYPE)F_INVALID,&file->first_mgm_page);
          if (file->fileid==(F_FILE_ID_TYPE)F_INVALID) return NULL;
          (void)_memset(&par,0,sizeof(F_FILE_PAR));
          (void)get_timedate(&par.ctime,&par.cdate);
          rc=f_flash_write(_FILE_MGM_DSC_ADDR(file->first_mgm_page)+_SADDR(F_FILE_DSC,par),&par,sizeof(F_FILE_PAR));
        }
        else
        {
          if (_f_checkopen(id)) return NULL;
          if (_f_add_entry(filename,file->fileid,&file->first_mgm_page)==(F_FILE_ID_TYPE)F_INVALID) return NULL;
          file->length=0;
        }
        
#if F_DIRECTORY
        if (rc==F_NOERR)
        {
          rc=f_flash_write(_FILE_MGM_DSC_ADDR(file->first_mgm_page)+_SADDR(F_FILE_DSC,dirid),&file->dirid,sizeof(F_DIR_ID_TYPE));
        }
#endif
        if (rc)
        {
          _CLEAR_BIT(f_volume.file_mgm_table,file->first_mgm_page);
          return NULL;
        }
        break;
  }
  
  file->act_mgm_page=file->first_mgm_page;
  file->mode=reqmode;
  return file;
}



/*
** f_close
**
** Close a previously opened file.
**
** INPUT : *filehandle - pointer to the file descriptor
** RETURN: F_NOERR on success, other if error
*/
int f_close(F_FILE *filehandle) 
{
  unsigned char rc=F_NOERR;
  unsigned char tmp;
  F_FILE_PAR par;
  
  if (f_volume.state==F_STATE_NOTFORMATTED) return F_ERR_INVHANDLE;
  if (!filehandle) return F_ERR_INVHANDLE; 

  switch (filehandle->mode&(F_MODE_READ|F_MODE_WRITE|F_MODE_APPEND))
  {
    case F_MODE_READ:	/* when opened for read, there is nothing to do */
        if ((filehandle->mode&F_MODE_PLUS)==0) break;

    case F_MODE_WRITE:
    case F_MODE_APPEND:
        if ((filehandle->mode&(F_MODE_READ|F_MODE_APPEND)) && ((filehandle->mode&F_MODE_CHANGED)==0)
            && _FILE_ID_MGM_PAGE(filehandle->fileid)!=filehandle->first_mgm_page)
        {
          rc=_f_remove_file_mgm(filehandle->first_mgm_page,0);	/* remove new file */
        }
        else
        {
#if RTC_PRESENT==0
          rc=f_flash_read(&par,_FILE_ID_MGM_DSC_ADDR(filehandle->fileid)+_SADDR(F_FILE_DSC,par),sizeof(F_FILE_PAR));
          if (rc==F_NOERR)
#endif
          {
              /* store parameter block */
            (void)get_timedate(&par.ctime,&par.cdate);
            par.attr=0;
            par.length=filehandle->length;
            rc=f_flash_write(_FILE_MGM_DSC_ADDR(filehandle->first_mgm_page)+_SADDR(F_FILE_DSC,par),&par,sizeof(F_FILE_PAR));
            if (rc==F_NOERR)
            {
              /* set closed state on the first file management block */
              tmp=_FM_STATE_CLOSE;
              rc=f_flash_write_safe(_FILE_MGM_HEADER_ADDR(filehandle->first_mgm_page)+_SADDR(F_FILE_MGM_HEADER,state),&tmp,sizeof(unsigned char));
              if (rc==F_NOERR)
              {
                /* if current file is a modification of a previous file */
                if (_FILE_ID_MGM_PAGE(filehandle->fileid)!=filehandle->first_mgm_page)
                {
                  rc=_f_remove_file_mgm(_FILE_ID_MGM_PAGE(filehandle->fileid),1);	/* remove old file */
                  if (rc) break;
                  filehandle->act_mgm_page=filehandle->first_mgm_page;
                  filehandle->page_cnt=0;
                  for (filehandle->page_cl_pos=0;;)
                  {
                    rc=_f_get_cluster_value(filehandle);
                    if (rc) break;
                    _SET_BIT(f_volume.cluster_table,filehandle->cluster);		/* and mark new file clusters */
                  }
   
                  /* change fileid first mgm page */
                  if (rc==(unsigned char)F_INVALID)
                  {
                    F_FILE_ID_TYPE i;
                    for (i=0;i<F_MAX_FILE && _FILE_ID_MGM_PAGE(i)!=_FILE_ID_MGM_PAGE(filehandle->fileid);i++);
                    if (i<F_MAX_FILE) _FILE_ID_MGM_PAGE(i)=filehandle->first_mgm_page;
                    rc=0;
                  }
                }
              }
            }
          }
      
          if (rc)
          {
            rc=_f_remove_file_mgm(filehandle->first_mgm_page,1);
          }
  
#if F_FIND_ENABLE && F_CHANGE_NOTIFY
          if (rc==0)
          {
  #if F_DIRECTORY
            _SET_BIT(f_volume.changed,filehandle->fileid+F_MAX_DIR);
  #else
            _SET_BIT(f_volume.changed,filehandle->fileid);
  #endif
          }
#endif
        }
        break;

    default: return F_ERR_NOTOPENED;
  }

  filehandle->mode=F_MODE_CLOSE;
  return rc;
}



/*
** f_read
**
** Read from a file.
**
** INPUT : buf - buffer to read data
**         size - number of unique
**         size_st - size of unique
**         *filehandle - pointer to file descriptor
** OUTPUT: number of read bytes
*/
long f_read(void *bbuf,long size,long size_st,F_FILE *filehandle) 
{
  F_CLUSTER_TYPE pos,rsize;
  char *buf=(char *)bbuf;
  long retsize;

  if (f_volume.state==F_STATE_NOTFORMATTED) return 0;
  if (!filehandle) return 0;			/* no file pointer */
  if ((filehandle->mode&(F_MODE_READ|F_MODE_PLUS))==0) return 0;	/* read not allowed */ 
  retsize=size;
  size*=size_st;				/* size to read */
  size_st=retsize;				/* size_st stores the item size from now */
  retsize=0;

  if (size<=0) return 0;
  if (size>(long)(filehandle->length-filehandle->abs_pos)) size=(long)(filehandle->length-filehandle->abs_pos);  
  	/* change read size, if less bytes available than requested */

  pos=filehandle->cluster_pos;
  while (size)
  {
    if (pos==F_CLUSTER_SIZE)
    {
      if (_f_get_cluster_value(filehandle)) break;
      pos=0;		/* change actual block to next block if data is left */
    }
  
    if (size>=(long)(F_CLUSTER_SIZE-pos)) rsize=F_CLUSTER_SIZE-pos;
                                     else rsize=(F_CLUSTER_TYPE)size;	
    	/* rsize contains size read left or the amount of bytes left in the current cluster */
    if (f_flash_read(buf,_CLUSTER_ADDR(filehandle->cluster)+pos,rsize))	break; /* put data from file system to user data */

    pos+=rsize;
    filehandle->abs_pos+=rsize;
    buf+=rsize;
    size-=rsize;
    retsize+=rsize;
  }
  filehandle->cluster_pos=pos;

  return retsize/size_st;
}



/* 
** f_write
** 
** INPUT : bbuf - buffer to write from
**         size - number of unique
**         size_st - size of unique
**         *filehandle - pointer to the file descriptor
** RETURN: number of written bytes
*/
long f_write(const void *bbuf,long size,long size_st,F_FILE *filehandle)
{
  F_CLUSTER_TYPE pos,wsize;
  char *buf=(char *)bbuf;
  unsigned char rc=F_NOERR;
  long retsize;

  if (f_volume.state==F_STATE_NOTFORMATTED) return 0;
  if (!filehandle) return 0;			/* no file pointer */
  if ((filehandle->mode&(F_MODE_WRITE|F_MODE_APPEND|F_MODE_PLUS))==0) return 0;	/* write not allowed */
  retsize=size;
  size*=size_st;				/* size to write */
  size_st=retsize;				/* size_st stores the size of an item */
  retsize=0;

  if (size<=0) return 0;

  if ( filehandle->mode&F_MODE_APPEND
#if (F_SEEK_WRITE==0)
      || filehandle->abs_pos!=filehandle->length
#endif
     )
  {
    if (f_seek(filehandle,0,F_SEEK_END)) return 0;
  }

  pos=filehandle->cluster_pos;
  while (size)
  {
    /* if no more space left in current cluster or first time after open*/
    if (pos==F_CLUSTER_SIZE)	
    {
      if (filehandle->abs_pos==filehandle->length)
      {
        filehandle->cluster=_f_get_empty_cluster();
        if (filehandle->cluster==(F_CLUSTER_COUNT_TYPE)F_INVALID) break;
        if (_f_set_cluster_value(filehandle)) break; 
#if (F_SEEK_WRITE)
        filehandle->orig_cluster=filehandle->cluster;
#endif
      }
      else
      {
        if (_f_get_cluster_value(filehandle)) break;
#if (F_SEEK_WRITE)
        filehandle->orig_cluster=(F_CLUSTER_COUNT_TYPE)F_INVALID;
#endif
      }
      pos=0;
    }

#if (F_SEEK_WRITE)
    /* allocate new cluster and copy old file cluster content to it if modification will be done to */
    /* a cluster present in a previous file */
    if (filehandle->orig_cluster==(F_CLUSTER_COUNT_TYPE)F_INVALID)
    {
      if (_FILE_ID_MGM_PAGE(filehandle->fileid)!=filehandle->first_mgm_page)
      {
        F_CLUSTER_COUNT_TYPE ocl;
        ocl=_f_get_orig_cluster_value(filehandle);
        if (ocl!=(F_CLUSTER_COUNT_TYPE)F_INVALID && ocl==filehandle->cluster)
        {
          filehandle->cluster=_f_get_empty_cluster();
          if (filehandle->cluster!=(F_CLUSTER_COUNT_TYPE)F_INVALID)
          {
            rc=f_flash_copy(_CLUSTER_ADDR(filehandle->cluster),_CLUSTER_ADDR(ocl),F_CLUSTER_SIZE);
            if (rc==F_NOERR)
            {
              --filehandle->page_cl_pos;
              rc=_f_set_cluster_value(filehandle);
            }
          }
          else break;
        }
      }
      if (rc) break;
      filehandle->orig_cluster=filehandle->cluster;
    }
#endif

    if (size>=(long)(F_CLUSTER_SIZE-pos)) wsize=F_CLUSTER_SIZE-pos;
                                     else wsize=(F_CLUSTER_TYPE)size;	
    	/* wsize contains number of bytes left to write or number of bytes left in the current cluster*/

    if (f_flash_write(_CLUSTER_ADDR(filehandle->cluster)+pos,buf,wsize)) break;

    pos+=wsize;
    filehandle->abs_pos+=wsize;
    buf+=wsize;
    size-=wsize;
    retsize+=wsize;
    
    if (filehandle->abs_pos>filehandle->length) filehandle->length=filehandle->abs_pos;
  }
  filehandle->cluster_pos=pos;
  filehandle->mode|=F_MODE_CHANGED;

  return retsize/size_st;
}



/*
** f_seek
**
** Seek in a file.
**
** INPUT : *filehandle - pointer to a file descriptor
**         offset - offset  
**         whence - F_SEEK_SET: position = offset
**                  F_SEEK_CUR: position = position + offset
**                  F_SEEK_END: position = end of file (offset=0)
** RETURN: F_NOERR on succes, other if error.
*/
int f_seek(F_FILE *filehandle,long offset,long whence) 
{
  F_LENGTH_TYPE abs_pos;
  F_CLUSTER_COUNT_TYPE cco,ccn;

  if (!filehandle) return F_ERR_INVHANDLE;

  abs_pos=filehandle->abs_pos;
  switch (whence)	/* calculate new position */ 
  {
    case F_SEEK_SET: break;
    case F_SEEK_CUR: offset=abs_pos+offset; break;
    case F_SEEK_END: offset=filehandle->length+offset; break;
    default: return F_ERR_INVOFFSET;
  }

  if (offset<0) return F_ERR_INVOFFSET;
  if (offset>(long)(filehandle->length)) return F_ERR_INVOFFSET;	/* new position is invalid */

  if ((long)abs_pos==offset) return F_NOERR;

  ccn=(F_CLUSTER_COUNT_TYPE)(((unsigned long)offset)/F_CLUSTER_SIZE);
  cco=(F_CLUSTER_COUNT_TYPE)(abs_pos/F_CLUSTER_SIZE);
  if (filehandle->page_cnt==0 && filehandle->page_cl_pos==0) (void)_f_get_cluster_value(filehandle);
  if (cco!=ccn)
  {
    /* calculate new block and block position */ 
    if (cco>ccn)
    {
      filehandle->act_mgm_page=filehandle->first_mgm_page;
      filehandle->page_cnt=0;
      filehandle->page_cl_pos=0;
      ++ccn;
    }
    else
    {
      ccn-=cco;
    }

    while (ccn--) (void)_f_get_cluster_value(filehandle);

#if (F_SEEK_WRITE)
    filehandle->orig_cluster=(F_CLUSTER_COUNT_TYPE)F_INVALID;
#endif 
  }

  filehandle->cluster_pos=(F_CLUSTER_TYPE)(((unsigned long)offset)%F_CLUSTER_SIZE);
  filehandle->abs_pos=offset;

  return F_NOERR;
}



/*
** f_getc
**
** read one byte from a file
**
** INPUT : *filehandle - pointer to a file descriptor
** RETURN: 0xff00 if error, otherwise the read character.
*/
int f_getc(F_FILE *filehandle) 
{
  unsigned char ch;
  if (f_read(&ch,1,1,filehandle)==1) return ch;
                                else return (0xff00);	
}



/*
** f_putc
**
** write one byte to a file
**
** INPUT : ch - character to write
**         *filehandle - pointer to a file handler
** RETURN: ch on success, -1 on error
*/
int f_putc(int ch,F_FILE *filehandle) 
{
  unsigned char tmpch=(unsigned char)(ch);
  if (f_write(&tmpch,1,1,filehandle)==1) return tmpch;
  return -1;
}



/*
** f_tell
**
** get current position in the file
**
** INPUT : *filehandle - pointer to a file descriptor
** RETURN: F_ERR_INVALID on error or current position.
*/
long f_tell(F_FILE *filehandle) 
{
  if (f_volume.state==F_STATE_NOTFORMATTED) return (long)F_ERR_INVALID;
  if (filehandle->mode==F_MODE_CLOSE) return (long)F_ERR_INVALID;
  return (long)filehandle->abs_pos;
}



/*
** f_eof
**
** check if current position is at the end of the file.
**
** INPUT : *filehandle - pointer to a file descriptor
** RETURN: F_ERR_EOF - at the end of the file
**         F_NOERR - no error, end of the file not reached
**         other - on error
*/
int f_eof(F_FILE *filehandle) 
{
  if (f_volume.state==F_STATE_NOTFORMATTED) return F_ERR_INVHANDLE;
  if (!filehandle) return F_ERR_INVHANDLE;
  if (filehandle->mode==F_MODE_CLOSE) return F_ERR_INVMODE;
  if (filehandle->abs_pos<filehandle->length) return F_NOERR;
  return F_ERR_EOF;
}



/*
** f_rewind
**
** set current position in the file to the beginning
**
** INPUT : *filehandle - pointer to a file descriptor
** RETURN: F_NOERR on succes, other if error.
*/
int f_rewind(F_FILE *filehandle) 
{
  return f_seek(filehandle,0,F_SEEK_SET);
}

