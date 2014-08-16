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


F_VOLUME f_volume;		/* volume structure */
F_FILE files[F_MAX_OPEN_FILE];	/* file structure */


/**************************************************************************/
/************************* INTERNAL FUNCTIONS *****************************/
/**************************************************************************/

/*
** Get a free directory entry
*/
#if F_DIRECTORY
F_DIR_ID_TYPE _f_get_dir_entry (void)
{
  F_DIR_ID_TYPE i,j;
  unsigned char tmp;

  for (i=0;i<F_MAX_DIR;i++)
  {
    j=f_volume.dir_empty_pos++;
    if (j+1==F_MAX_DIR) f_volume.dir_empty_pos=0;
    if (f_flash_read(&tmp,_DIR_DSC_ADDR(j)+_SADDR(F_DIR_DSC,name[0]),1)) break;
    if (tmp==(unsigned char)F_INVALID) return j;
  }

  return (F_DIR_ID_TYPE)F_INVALID;
}
#endif

/*
** Get empty data cluster
*/
F_CLUSTER_COUNT_TYPE _f_get_empty_cluster (void)
{
  F_CLUSTER_COUNT_TYPE i,j;

  for (i=0;i<F_CLUSTER_COUNT;i++)
  {
    j=f_volume.cluster_empty_pos++;
    if (j+1==F_CLUSTER_COUNT) f_volume.cluster_empty_pos=0;
    if (_GET_BIT(f_volume.cluster_table,j)==0) 
    {
      _SET_BIT(f_volume.cluster_table,j);
      return j; 
    }
  }

  return (F_CLUSTER_COUNT_TYPE)F_INVALID;
}



/*
** Get empty file management page
** INPUT: fileid - file ID
**        state - state stored in header (if 0 skip storing of the header)
**        seq - sequence number
*/
F_FILE_MGM_PAGE_COUNT_TYPE _f_get_file_mgm_empty_page (F_FILE_ID_TYPE fileid, unsigned char state, unsigned char seq)
{
  F_FILE_MGM_HEADER hdr;
  F_FILE_MGM_PAGE_COUNT_TYPE i,j;
  unsigned char rc;

  for (i=0;i<F_FILE_MGM_PAGE_COUNT;i++)
  {
    j=f_volume.file_mgm_empty_pos++;
    if (j+1==F_FILE_MGM_PAGE_COUNT) f_volume.file_mgm_empty_pos=0;
    if (_GET_BIT(f_volume.file_mgm_table,j)==0)
    {
      if (f_flash_erase(_FILE_MGM_ADDR(j),F_MGM_PAGE_SIZE)) break; 
      if (state)
      {
        hdr.next=(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID;
        hdr.fileid=fileid;
        hdr.seq=seq;
        hdr.state=state;
        rc=f_flash_write(_FILE_MGM_ADDR(j),&hdr,sizeof(F_FILE_MGM_HEADER));
      }
      if (state==0 || rc==F_NOERR)
      {
        _SET_BIT(f_volume.file_mgm_table,j);
        return j;
      }
    }
  }
  
  return (F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID;
}


/*
** Get cluster value for a file.
*/
unsigned char _f_get_cluster_value (F_FILE *f)
{
  F_FILE_MGM_CLUSTER_PER_PAGE_COUNT_TYPE cl_per_page;
  unsigned char rc;

 
  if (f->first_mgm_page==f->act_mgm_page) cl_per_page=F_FILE_MGM_CLUSTER_PER_PAGE0;
                                     else cl_per_page=F_FILE_MGM_CLUSTER_PER_PAGE;

  if (f->page_cl_pos==cl_per_page)
  {
    F_FILE_MGM_HEADER mgm_hdr;
    rc=f_flash_read(&mgm_hdr,_FILE_MGM_HEADER_ADDR(f->act_mgm_page),sizeof(F_FILE_MGM_HEADER));
    if (rc) return rc;
    if (mgm_hdr.next!=(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID) 
    {
      f->act_mgm_page=mgm_hdr.next;
      f->page_cl_pos=0;
      ++f->page_cnt;
    }
    else
    {
      return (unsigned char)F_INVALID;
    }
  }

  rc=f_flash_read(&f->cluster,_FILE_MGM_PAGE_CLUSTER_ADDR(f->act_mgm_page,f->page_cl_pos),sizeof(F_CLUSTER_COUNT_TYPE));
  if (rc) return rc;

  if (f->cluster==(F_CLUSTER_COUNT_TYPE)F_INVALID) return (unsigned char)F_INVALID;
                                              else ++f->page_cl_pos;

  return F_NOERR;
}


/*
** Set a cluster value
*/
unsigned char _f_set_cluster_value (F_FILE *f)
{
  F_FILE_MGM_CLUSTER_PER_PAGE_COUNT_TYPE cl_per_page;
  unsigned char rc;

 
  if (f->first_mgm_page==f->act_mgm_page) cl_per_page=F_FILE_MGM_CLUSTER_PER_PAGE0;
                                     else cl_per_page=F_FILE_MGM_CLUSTER_PER_PAGE;

  if (f->page_cl_pos==cl_per_page)
  {
    F_FILE_MGM_HEADER mgm_hdr;

    rc=f_flash_read(&mgm_hdr,_FILE_MGM_HEADER_ADDR(f->act_mgm_page),sizeof(F_FILE_MGM_HEADER));
    if (rc) return rc;

    if (mgm_hdr.next==(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID)
    {
      F_FILE_MGM_PAGE_COUNT_TYPE next;

      next=_f_get_file_mgm_empty_page(0,0,0);
      if (next==(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID) return (unsigned char)F_INVALID;

      mgm_hdr.next=next;
      rc=f_flash_write(_FILE_MGM_HEADER_ADDR(f->act_mgm_page),&mgm_hdr,sizeof(F_FILE_MGM_HEADER));
      if (rc) return rc;

      mgm_hdr.next=(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID;
      mgm_hdr.state=_FM_STATE_EXT;
      rc=f_flash_write(_FILE_MGM_HEADER_ADDR(next),&mgm_hdr,sizeof(F_FILE_MGM_HEADER));
      if (rc) return rc;

      f->act_mgm_page=next;
    }
    else
    {
      f->act_mgm_page=mgm_hdr.next;
    }
    ++f->page_cnt;
    f->page_cl_pos=0;
  }

  rc=f_flash_write(_FILE_MGM_PAGE_CLUSTER_ADDR(f->act_mgm_page,f->page_cl_pos++),&f->cluster,sizeof(F_CLUSTER_COUNT_TYPE));
  if (rc) return rc;

  return F_NOERR;
}


/*
** use orig_first_mgm_page otherwise us first_mgm_page
*/
F_CLUSTER_COUNT_TYPE _f_get_orig_cluster_value (F_FILE *f)
{
  F_FILE_MGM_PAGE_COUNT_TYPE act=_FILE_ID_MGM_PAGE(f->fileid);
  F_FILE_MGM_PAGE_COUNT_TYPE page_cnt=f->page_cnt;
  F_FILE_MGM_HEADER mgm_hdr;
  F_CLUSTER_TYPE rc;
  unsigned char rc2;
  
  while (act!=(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID && page_cnt)
  {
    rc2=f_flash_read(&mgm_hdr,_FILE_MGM_HEADER_ADDR(act),sizeof(F_FILE_MGM_HEADER));
    if (rc2) return (F_CLUSTER_COUNT_TYPE)F_INVALID;
    act=mgm_hdr.next;
    --page_cnt;
  }

  if (act!=(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID)
  {
    rc2=f_flash_read(&rc,_FILE_MGM_PAGE_CLUSTER_ADDR(act,f->page_cl_pos-1),sizeof(F_CLUSTER_COUNT_TYPE));
    if (!rc2) return rc;
  }

  return (F_CLUSTER_COUNT_TYPE)F_INVALID;
}



/*
** Remove file management chain.
*/
unsigned char _f_remove_file_mgm (F_FILE_MGM_PAGE_COUNT_TYPE first, char mode)
{
  F_FILE_MGM_PAGE_COUNT_TYPE act_page;
  F_FILE_MGM_HEADER mgm_hdr;
  unsigned char rc;

  if (mode)	/* remove clusters from cluster table */
  {
    F_FILE_MGM_CLUSTER_PER_PAGE_COUNT_TYPE cnt=F_FILE_MGM_CLUSTER_PER_PAGE0;
    F_CLUSTER_COUNT_TYPE act,i;

    act_page=first;
    for (i=0;;i++)
    {
      if (i==cnt)
      {
        rc=f_flash_read(&mgm_hdr,_FILE_MGM_HEADER_ADDR(act_page),sizeof(F_FILE_MGM_HEADER));
        if (rc) return rc;
        if (mgm_hdr.next==(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID) break;
        act_page=mgm_hdr.next;
        cnt=F_FILE_MGM_CLUSTER_PER_PAGE;
        i=0;
      }

      rc=f_flash_read(&act,_FILE_MGM_PAGE_CLUSTER_ADDR(act_page,i),sizeof(F_CLUSTER_COUNT_TYPE));
      if (rc) return rc;
      if (act==(F_CLUSTER_COUNT_TYPE)F_INVALID) break;

      _CLEAR_BIT(f_volume.cluster_table,act);
    }
  }

  act_page=first;
  while (act_page!=(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID)
  {
    rc=f_flash_read(&mgm_hdr,_FILE_MGM_HEADER_ADDR(act_page),sizeof(F_FILE_MGM_HEADER));
    if (rc) return rc;
    _CLEAR_BIT(f_volume.file_mgm_table,act_page);
    act_page=mgm_hdr.next;
  }

  rc=f_flash_erase_safe(_FILE_MGM_ADDR(first),F_MGM_PAGE_SIZE);
  if (rc) return rc;

  return 0;
}


/*
** copy file managment chain to a new place
*/
F_FILE_MGM_PAGE_COUNT_TYPE _f_copy_mgm (F_FILE_MGM_PAGE_COUNT_TYPE oact)
{
  F_FILE_MGM_HEADER hdr;
  F_FILE_MGM_PAGE_COUNT_TYPE rc;
  F_FILE_MGM_PAGE_COUNT_TYPE nact,nprev;
  unsigned char frc=0;
  
  nprev=rc=(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID;
  while (oact!=(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID)
  {
    nact=_f_get_file_mgm_empty_page(0,0,0);		/* get an empty file management page */
    if (nact==(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID)
    {
      frc=1;
      break;
    }

    if (nprev!=(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID)	/* write previous page header if current new page is not the first */
    {
      hdr.next=nact;
      frc=f_flash_write(_FILE_MGM_HEADER_ADDR(nprev),&hdr,sizeof(F_FILE_MGM_HEADER));
      if (frc) break; 
      hdr.state=_FM_STATE_EXT;
    }
     
    if (rc==(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID)	/* if first page then read original first page header */
    {							/* change state to OPEN and increase sequence number */
      frc=f_flash_read(&hdr,_FILE_MGM_HEADER_ADDR(oact),sizeof(F_FILE_MGM_HEADER));
      if (frc) break;
      hdr.next=(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID;
      hdr.state=_FM_STATE_OPEN;
      ++hdr.seq;
      rc=nact;
    }

    frc=f_flash_copy(_FILE_MGM_PAGE_CLUSTER_ADDR(nact,0),_FILE_MGM_PAGE_CLUSTER_ADDR(oact,0),F_MGM_PAGE_SIZE);
    if (frc) break;	/* copy everything but the header from old management page to the new */
    frc=f_flash_read(&oact,_FILE_MGM_HEADER_ADDR(oact)+_SADDR(F_FILE_MGM_HEADER,next),sizeof(F_FILE_MGM_PAGE_COUNT_TYPE));
    if (frc) break; 	/* get next page in the original management area  */

    nprev=nact;
  }

  if (frc==0)		/* write last header */
  {
    hdr.next=(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID;
    frc=f_flash_write(_FILE_MGM_HEADER_ADDR(nprev),&hdr,sizeof(F_FILE_MGM_HEADER));
  }

  if (frc)		/* if there was a problem remove everything and return error */
  {
    if (rc!=(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID) (void)_f_remove_file_mgm(rc,0);
    return (F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID;
  }

  return rc;
}





/**************************************************************************/
/*************************** USER FUNCTIONS *******************************/
/**************************************************************************/

/*
** f_initvolume
**
** Initialize volume and fix inconsistencies on the volume.
**
** RETURN: F_NOERR on success, other if error.
*/
unsigned char f_initvolume (void)
{
  unsigned char rc;
  F_FILE_MGM_PAGE_COUNT_TYPE fi;
#if F_DIRECTORY
  F_DIR_ID_TYPE di;
#endif
  F_FILE_MGM_HEADER mgm_hdr;

  f_volume.state=F_STATE_NOTFORMATTED;

  rc=f_flash_init();
  if (rc) return rc;

#if F_CHECKMEDIA
  {
    F_FS_ID fs_id;
    rc=f_flash_read(&fs_id,F_FS_ID_ADDR,sizeof(F_FS_ID));
    if (rc) return rc;
    if (_strcmp(fs_id.tinystr,_TINYSTR)) return F_ERR_NOTFORMATTED;
    if (fs_id.version!=_TINY_VERSION || fs_id.f_cluster_size!=F_CLUSTER_SIZE
        || fs_id.f_mgm_page_size!=F_MGM_PAGE_SIZE   
#if F_DIRECTORY
        || fs_id.f_dir_page_count!=F_DIR_PAGE_COUNT
#else
        || fs_id.f_dir_page_count
#endif
        || fs_id.f_file_mgm_page_count!=F_FILE_MGM_PAGE_COUNT 
        || fs_id.f_cluster_count!=F_CLUSTER_COUNT) return F_ERR_DIFFMEDIA;
  }
#endif

  (void)_memset(&files[0],0,sizeof(files));
  (void)_memset(&f_volume,0,sizeof(f_volume));
  (void)_memset(f_volume.file_id_page,0xff,sizeof(f_volume.file_id_page));

#if F_DIRECTORY
  f_volume.current_dir=F_DIR_ROOT;	/* change to root directory */
  for (di=0;di<F_MAX_DIR;di++)
  {
    unsigned char tmp;
    rc=f_flash_read(&tmp,_DIR_DSC_ADDR(di)+_SADDR(F_DIR_DSC,name[0]),1);
    if (rc) return rc;
    if (tmp!=(unsigned char)F_INVALID)
    { 
      rc=f_flash_read(&tmp,_DIR_DSC_ADDR(di)+_SADDR(F_DIR_DSC,par)+_SADDR(F_DIR_PAR,attr),1);
      if (rc) return rc;
      if ((tmp&F_ATTR_DIR)==0) return F_ERR_NOTFORMATTED;
    }
  }
#endif

  for (fi=0;fi<F_FILE_MGM_PAGE_COUNT;fi++)
  {
    rc=f_flash_read(&mgm_hdr,_FILE_MGM_HEADER_ADDR(fi),sizeof(F_FILE_MGM_HEADER));
    if (rc) return rc;

    if (mgm_hdr.fileid!=(F_FILE_ID_TYPE)F_INVALID)
    {
      switch (mgm_hdr.state)
      {
        case _FM_STATE_CLOSE:
                      {
                        F_FILE *f=&files[0];
                        F_FILE_MGM_HEADER mgm_hdr2;
                        F_FILE_MGM_PAGE_COUNT_TYPE prev_mgm_page=(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID;
  
                        if (_FILE_ID_MGM_PAGE(mgm_hdr.fileid)!=(F_FILE_MGM_PAGE_COUNT_TYPE)F_INVALID)
                        {
  
                          rc=f_flash_read(&mgm_hdr2,_FILE_ID_MGM_HEADER_ADDR(mgm_hdr.fileid),sizeof(F_FILE_MGM_HEADER));
                          if (rc) return rc;
  
                          if (mgm_hdr2.seq+1==mgm_hdr.seq)
                          {
                            rc=_f_remove_file_mgm(_FILE_ID_MGM_PAGE(mgm_hdr.fileid),1);
                            if (rc) return rc;
                          }
                          else
                          {
                            rc=_f_remove_file_mgm(fi,0);
                            if (rc) return rc;
                            break;
                          }
                        }
  
                        f->page_cl_pos=0;
                        f->page_cnt=0;
                        f->first_mgm_page=fi;
                        f->act_mgm_page=fi;
                        for (;;)
                        {
                          if (prev_mgm_page!=f->act_mgm_page)
                          {
                            _SET_BIT(f_volume.file_mgm_table,f->act_mgm_page);
                            prev_mgm_page=f->act_mgm_page;
                          }
                          rc=_f_get_cluster_value(f);
                          if (rc) break;
                          _SET_BIT(f_volume.cluster_table,f->cluster);
                        }
                        _FILE_ID_MGM_PAGE(mgm_hdr.fileid)=fi;
                      }
                      break;
                     
        case _FM_STATE_OPEN: 	/* no need to do anything */ 
                      break;
 
        case _FM_STATE_EXT:  	/* no need to do anything */
                      break;

        default:
                      return F_ERR_NOTFORMATTED;
  
      }
    }
  }
  
  f_volume.state=F_STATE_WORKING;

  return F_NOERR;  
}


/*
** Format the device
*/
unsigned char f_format (void)
{
  unsigned char rc;
  
  rc=f_flash_init();
  if (rc) return rc;
  rc=f_flash_format();
  if (rc) return rc;
  rc=f_flash_erase(F_BEGIN_ADDR,F_FS_ID_SIZE+F_DIR_SIZE+F_FILE_MGM_SIZE);
  if (rc) return rc;
#if F_CHECKMEDIA
  {
    F_FS_ID fs_id;
    fs_id.version=_TINY_VERSION;
    fs_id.f_cluster_size=F_CLUSTER_SIZE;
    fs_id.f_mgm_page_size=F_MGM_PAGE_SIZE;
#if F_DIRECTORY
    fs_id.f_dir_page_count=F_DIR_PAGE_COUNT;
#else
    fs_id.f_dir_page_count=0;
#endif
    fs_id.f_file_mgm_page_count=F_FILE_MGM_PAGE_COUNT;
    fs_id.f_cluster_count=F_CLUSTER_COUNT;
    (void)_memcpy(fs_id.tinystr,_TINYSTR,sizeof(_TINYSTR));
    rc=f_flash_write_safe(F_FS_ID_ADDR,&fs_id,sizeof(F_FS_ID));
    if (rc) return rc;
  }
#endif
  return f_initvolume();
}



