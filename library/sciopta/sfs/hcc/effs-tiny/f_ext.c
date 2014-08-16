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

#include <<hcc/effs-tiny/tiny.h>
#include <hcc/effs-tiny/f_volume.h>

/*
** Find changed files/directories
*/
#if F_FIND_ENABLE && F_CHANGE_NOTIFY
unsigned char f_findchanged (F_FIND *find)
{
  unsigned char i;
  F_FILE_ID_TYPE j;
  unsigned char rc;

  for (i=0;i<_CHANGE_TABLE_SIZE && f_volume.changed[i]==0;i++);
  if (i<_CHANGE_TABLE_SIZE)
  {
    for (i=0;i<_CHANGE_TABLE_SIZE && f_volume.changed[i]==0xff;i++);
    if (i<_CHANGE_TABLE_SIZE)
    {
      for (j=0;j<F_MAX_FILE;j++)
      {
        if (_GET_BIT(f_volume.changed,j))
        {
          unsigned long addr;
#if F_DIRECTORY
          if (j<F_MAX_DIR)
            addr=_DIR_DSC_ADDR(j);
          else
            addr=_FILE_ID_MGM_DSC_ADDR(j-F_MAX_DIR);
#else
          addr=_FILE_ID_MGM_DSC_ADDR(j);
#endif
          rc=f_flash_read(find->filename,addr+_SADDR(F_FILE_DSC,name[0]),F_MAX_FILE_NAME_LENGTH+1);
          if (rc) return rc;
          rc=f_flash_read((char *)find+_SADDR(F_FIND,attr),addr+_SADDR(F_FILE_DSC,par),sizeof(F_FILE_PAR));
          if (rc) return rc;
#if F_DIRECTORY
          rc=f_flash_read(&find->dirid,addr+_SADDR(F_FILE_DSC,dirid),sizeof(F_DIR_ID_TYPE));
          if (rc) break;
          if (j<F_MAX_DIR) 
          {
            find->length=0;
          }
#endif
          _CLEAR_BIT(f_volume.changed,j);
          find->findpos=j+1;
          return F_NOERR;
        }
      }
    }
    else
    {
      for (i=0;i<_CHANGE_TABLE_SIZE;f_volume.changed[i++]=0);
      memset(find,0,sizeof(F_FIND));
      find->length=(F_LENGTH_TYPE)F_INVALID;
      return F_NOERR;
    }
  }

  return F_ERR_NOTFOUND;
}
#endif


/*
** Decide whether an address points to a used area
*/
#if TINY_USED_ADDR_FN
unsigned char tiny_used_addr(unsigned long addr)
{
  if (addr>=F_FIRST_CLUSTER_ADDR)
  {
    F_CLUSTER_COUNT_TYPE cluster=(addr-F_FIRST_CLUSTER_ADDR)/F_CLUSTER_SIZE;
    if (cluster>=F_CLUSTER_COUNT) return 0;
    if (_GET_BIT(f_volume.cluster_table,cluster)==0) return 0;
  }

  return 1;
}
#endif

