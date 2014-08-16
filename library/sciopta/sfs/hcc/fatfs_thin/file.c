/****************************************************************************
 *
 *            Copyright (c) 2003-2008 by HCC Embedded
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
 * Budapest 1133
 * Vaci ut 110
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/
#include "thin_usr.h"

#include <hcc/fatfs_thin/file.h>
#include <hcc/fatfs_thin/util.h>
#include <hcc/fatfs_thin/volume.h>
#include <hcc/fatfs_thin/fat.h>
#include "drv.h"
#if F_LONGFILENAME
#include <hcc/fatfs_thin/dir_lfn.h>
#else
#include <hcc/fatfs_thin/dir.h>
#endif


/****************************************************************************
 *
 * _f_getintfile
 *
 * allocate an internal file handler
 *
 * RETURNS
 *
 * internal file pointer if successful
 * zero if there was no free file pointer (see F_MAXFILES)
 *
 ***************************************************************************/
F_FILE *_f_getintfile(void) {
unsigned char a;
	for (a=0; a<F_MAXFILES; a++) {
		if (gl_files[a].mode==F_FILE_CLOSE) {
			__memset(&gl_files[a],0,22);
			return &gl_files[a];
		}
	}
	return 0;		/*no more file could be opened */
}




/****************************************************************************
 *
 * _f_checklocked
 *
 * check if a given file is locked or not
 *
 * INPUTS
 * pos - pos structure contains directory entry pos
 *
 * RETURNS
 * zero - if not locked
 * F_ERR_LOCKED - if locked
 *
 ***************************************************************************/
#if F_WRITING
unsigned char _f_checklocked(F_POS *pos) {
unsigned char a;
	for (a=0; a<F_MAXFILES; a++) {
		F_FILE *f=&gl_files[a];
		if (f->mode!=F_FILE_CLOSE) {
			if (f->dirpos.sector==pos->sector && f->dirpos.pos==pos->pos) return F_ERR_LOCKED;
		}
	}
	return F_NO_ERROR;
}
#endif




/****************************************************************************
 *
 * fn_rename
 *
 * Rename file or directory
 *
 * INPUTS
 *
 * filename - filename or directory name (with or without path)
 * newname - new name of the file or directory (without path)
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/
#if (F_RENAME & F_WRITING)

#if F_LONGFILENAME
unsigned char fn_rename(const char *filename, const char *newname) {
F_LFNINT lfn;
F_POS posdir;
F_POS pos;
F_DIRENTRY *de;
F_DIRENTRY deold;
F_NAME fsname;
F_NAME fsnewname;
unsigned char ret;

	if (_f_setfsname(filename,&fsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsname.lname,1)) return F_ERR_INVALIDNAME;/*invalid name*/
	if (fsname.lname[0]=='.') return F_ERR_NOTFOUND;
#endif
	ret=_f_getvolume();
	if (ret) return ret;

	if (!(_f_findpath(&fsname,&posdir))) return F_ERR_INVALIDDIR;

	pos=posdir;

#if F_FINDING
	if (!_f_findlfilewc(fsname.lname,&pos,&de,&lfn,0)) return F_ERR_NOTFOUND;
#else
	if (!_f_findlfilewc(fsname.lname,&pos,&de,&lfn)) return F_ERR_NOTFOUND;
#endif

	if (de->attr & F_ATTR_READONLY) return F_ERR_ACCESSDENIED;      /*readonly*/

	if (_f_checklocked(&pos)) return F_ERR_LOCKED;

	__memcpy(&deold,de,sizeof(F_DIRENTRY)); /*store all properties*/

#if F_CHECKNAME
	if (_f_checknamewc(newname,1)) return F_ERR_INVALIDNAME;/*invalid name no drive,no path*/
#endif
	if (_f_setfsname(newname,&fsnewname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsnewname.lname,1)) return F_ERR_INVALIDNAME;/*invalid name*/
	if (fsnewname.lname[0]=='.') return F_ERR_INVALIDNAME;
#endif

	__memcpy(fsnewname.path,fsname.path,sizeof(fsname.path));

	pos=posdir;

#if F_FINDING
	if (_f_findlfilewc(fsnewname.lname,&pos,0,&lfn,0)) return F_ERR_DUPLICATED;
#else
	if (_f_findlfilewc(fsnewname.lname,&pos,0,&lfn)) return F_ERR_DUPLICATED;
#endif

	pos=posdir;

	ret=_f_addentry(&fsnewname,&pos,&de);
	if (ret) return ret;

	__memcpy(&deold,de,F_MAXNAME+F_MAXEXT);	/* update name and ext */
	__memcpy(de,&deold,sizeof(F_DIRENTRY)); /*restore all properties*/

	ret = _f_writedirsector();
	if (ret) return ret;

	pos=posdir;

#if F_FINDING
	if (!_f_findlfilewc(fsname.lname,&pos,&de,&lfn,0)) return F_ERR_NOTFOUND; /*try to find again to remove*/
#else
	if (!_f_findlfilewc(fsname.lname,&pos,&de,&lfn)) return F_ERR_NOTFOUND; /*try to find again to remove*/
#endif

	return _f_removelfn(posdir.cluster,&lfn);
}

#else

unsigned char fn_rename(const char *filename, const char *newname) {
F_POS posdir;
F_POS pos;
F_DIRENTRY *de;
F_NAME fsname;
char name[F_MAXNAME];
char ext[F_MAXEXT];
unsigned char a;
unsigned char ret;

	if (_f_setfsname(filename,&fsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/*invalid name*/
#endif
	ret=_f_getvolume();
	if (ret) return ret;

	if (!(_f_findpath(&fsname,&posdir))) return F_ERR_INVALIDDIR;

	pos=posdir;

	if (fsname.filename[0]=='.') return F_ERR_NOTFOUND;

#if F_FINDING
	if (!_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de,0)) return F_ERR_NOTFOUND;
#else
	if (!_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de)) return F_ERR_NOTFOUND;
#endif

	if (de->attr & F_ATTR_READONLY) return F_ERR_ACCESSDENIED;      /*readonly*/

	if (_f_checklocked(&pos)) return F_ERR_LOCKED;

	if (!_f_setnameext((char *)newname,name,ext)) return 0; /*no length invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(name,ext)) return 0;/*invalid name*/
	if (name[0]=='.' || name[0]==' ') return F_ERR_INVALIDNAME;
#endif
#if F_FINDING
	if (_f_findfilewc(name,ext,&posdir,0,0)) return F_ERR_DUPLICATED;
#else
	if (_f_findfilewc(name,ext,&posdir,0)) return F_ERR_DUPLICATED;
#endif

	ret=_f_getdirsector(pos.sector);
	if (ret) return ret;

	for (a=0;a<F_MAXNAME; a++) de->name[a]=name[a];
	for (a=0;a<F_MAXEXT; a++) de->ext[a]=ext[a];

	return _f_writedirsector();
}

#endif

#endif



/****************************************************************************
 *
 * fn_filelength
 *
 * Get a file length
 *
 * INPUTS
 *
 * filename - file whose length is needed
 *
 * RETURNS
 *
 * length of the file
 *
 ***************************************************************************/
#if F_FILELENGTH

long fn_filelength(const char *filename) {
#if F_LONGFILENAME
F_LFNINT lfn;
#endif
F_POS pos;
F_DIRENTRY *de;
F_NAME fsname;

	if (_f_setfsname(filename,&fsname)) return 0; /*invalid name*/
#if F_CHECKNAME
#if F_LONGFILENAME
	if (_f_checknamewc(fsname.lname,1)) return 0;/*invalid name*/
#else
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return 0;/*invalid name*/
#endif
#endif
	if (_f_getvolume()) return 0; /*can't get the size*/

	if (!_f_findpath(&fsname,&pos)) return 0;
#if F_LONGFILENAME
#if F_FINDING
	if (!_f_findlfilewc(fsname.lname,&pos,&de,&lfn,0)) return 0;
#else
	if (!_f_findlfilewc(fsname.lname,&pos,&de,&lfn)) return 0;
#endif
#else
#if F_FINDING
	if (!_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de,0)) return 0;
#else
	if (!_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de)) return 0;
#endif
#endif

	if (de->attr & F_ATTR_DIR) return 0;				/*directory*/

	return (long)_f_getlong(&de->filesize);
}

#endif




/****************************************************************************
 *
 * _f_emptywritebuffer
 *
 * empty write buffer if it contains unwritten data
 *
 * INPUTS
 *
 * f - internal file pointer
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

#if F_WRITING

static unsigned char _f_emptywritebuffer(F_FILE *f, void *ptr) {
unsigned char ret;
	if (!f->modified) return F_NO_ERROR; /*nothing to write*/
	if (f->pos.sector>=f->pos.sectorend) {
		if (!f->startcluster) {
			ret=_f_alloccluster(&f->startcluster);
			if (ret) return ret;
			_f_clustertopos(f->startcluster,&f->pos);

			ret=_f_setclustervalue(f->startcluster,F_CLUSTER_LAST);
			if (ret) return ret;
		}
		else {
			unsigned long value;
			ret=_f_getclustervalue(f->pos.cluster,&value);
			if (ret) return ret;
					
			if (value<F_CLUSTER_RESERVED) {  /*we are in chain*/
				f->prevcluster=f->pos.cluster;
				_f_clustertopos(value,&f->pos); /*go to next cluster*/
			}
			else {
				unsigned long nextcluster;
			
				ret=_f_alloccluster(&nextcluster);
				if (ret) return ret;

				ret=_f_setclustervalue(nextcluster,F_CLUSTER_LAST);
				if (ret) return ret;

				ret=_f_setclustervalue(f->pos.cluster,nextcluster);
				if (ret) return ret;

				f->prevcluster=f->pos.cluster;

				_f_clustertopos(nextcluster,&f->pos);
			}
		}
	}

        if (ptr==NULL) ptr=f->_tdata;
write_again:
	ret=_f_writesector(ptr,f->pos.sector);
	if (ret) {
	 	if (gl_volume.state!=F_STATE_WORKING) return ret; /*check if still working volume*/
		
	 	ret=_f_dobadblock(f);
	 	if (ret) return ret;

		if (!f->prevcluster) {
			f->startcluster=f->pos.cluster;
		}
		else {
			ret=_f_setclustervalue(f->prevcluster,f->pos.cluster);
			if (ret) return ret;
		}

	 	goto write_again;
	}

	f->modified=0;
	return F_NO_ERROR;
}

#endif



/****************************************************************************
 *
 * _f_extend
 *
 * Extend file to a certain size
 *
 ***************************************************************************/
#if (F_WRITING & (F_SEEK|F_SETEOF))
static unsigned char _f_extend (F_FILE *f, long size)
{
  unsigned long _size;
  unsigned char rc;
  unsigned char ms=0;

  size-=f->filesize;
  _size=(unsigned long)size;
  if (f->relpos==F_SECTOR_SIZE)
  {
    f->modified=1;
    rc=_f_emptywritebuffer(f,NULL);
    if (rc) return rc;
  }

  __memset(f->_tdata+f->relpos,0,(__size_t)(F_SECTOR_SIZE-f->relpos));
  if (f->relpos+_size>F_SECTOR_SIZE)
  {
    _size-=(F_SECTOR_SIZE-f->relpos);
    while (_size)
    {
      f->modified=1;
      rc=_f_emptywritebuffer(f,NULL);
      if (rc) return rc;
      ++f->pos.sector;

      if (!ms)
      {
        __memset(f->_tdata,0,F_SECTOR_SIZE);
        ms=1;
      }

      _size-=(_size>F_SECTOR_SIZE)?F_SECTOR_SIZE:_size;
    }
  }
  else
  {
    _size+=f->relpos;
  }


  f->filesize+=size;
  f->abspos=f->filesize&(~(F_SECTOR_SIZE-1));
  f->relpos=_size;
  f->modified=1;

  return F_NO_ERROR;
}
#endif



/****************************************************************************
 *
 * _f_fseek
 *
 * subfunction for f_seek it moves position into given offset and read
 * the current sector
 *
 * INPUTS
 * f - F_FILEINT structure which file position needed to be modified
 * offset - position from start
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
static unsigned char _f_fseek(F_FILE *f,long offset)
{
  unsigned long cluster;
  unsigned long tmp;
  unsigned char ret=F_NO_ERROR;
  long remain;

  if (offset<0) offset=0;
#if (F_WRITING==0 || F_SEEK==0)
  if ((unsigned long)offset>f->filesize)
  {
    offset=(long)f->filesize;
  }
#endif

  if (offset<=f->filesize && offset>=f->abspos && offset<f->abspos+F_SECTOR_SIZE)
  {
    f->relpos=offset-f->abspos;
  }
  else
  {
#if F_WRITING
  	if (f->modified)
  	{
      ret=_f_emptywritebuffer(f,NULL);
      if (ret)
      {
        f->mode=F_FILE_CLOSE; /*cant accessed any more*/
        return ret;
      }

      ret=_f_writefatsector();
      if (ret)
      {
        f->mode=F_FILE_CLOSE;
        return ret;
      }
  		
    }
#endif
    if (f->startcluster)
    {
      f->abspos=0;
      f->relpos=0;
      f->prevcluster=0;
      f->pos.cluster=f->startcluster;
      remain=f->filesize;

      tmp=gl_volume.bootrecord.sector_per_cluster;
      tmp*=F_SECTOR_SIZE;	/* set to cluster size */

      /*calc cluster*/
      while ((unsigned long)offset>=tmp)
      {
        ret=_f_getclustervalue(f->pos.cluster,&cluster);
        if (ret)
        {
          f->mode=F_FILE_CLOSE;
          return ret;
        }
        if (tmp>=remain) break;
        remain-=tmp;
        offset-=tmp;
        f->abspos+=tmp;
        if (cluster>=F_CLUSTER_RESERVED) break;
        f->prevcluster=f->pos.cluster;
        f->pos.cluster=cluster;
      }

      _f_clustertopos(f->pos.cluster,&f->pos);
      if (remain && offset)
      {
        while (offset>F_SECTOR_SIZE && remain>F_SECTOR_SIZE)
        {
          f->pos.sector++;
          offset-=F_SECTOR_SIZE;
          remain-=F_SECTOR_SIZE;
          f->abspos+=F_SECTOR_SIZE;
        }
      }

      ret=_f_getcurrsector(f,NULL);
      if (ret) return ret;

      if (remain<offset)
      {
        f->relpos=(unsigned long)remain;
        ret=_f_extend(f,f->filesize+offset-remain);
      }
      else
      {
        f->relpos=(unsigned long)offset;
      }
    }
  }

  return ret;
}



/****************************************************************************
 *
 * fn_open
 *
 * open a file for reading/writing/appending
 *
 * INPUTS
 *
 * filename - which file need to be opened
 * mode - string how to open ("r"-read, "w"-write, "w+"-overwrite, "a"-append
 *
 * RETURNS
 *
 * F_FILE pointer if successfully
 * 0 - if any error
 *
 ***************************************************************************/

F_FILE *fn_open(const char *filename,const char *mode) {
#if F_LONGFILENAME
F_LFNINT lfn;
#endif
F_DIRENTRY *de;
F_NAME fsname;
unsigned char m_mode=F_FILE_CLOSE;
F_FILE *f=0;

	if (mode[1]==0)
	{
	  if (mode[0]=='r') m_mode=F_FILE_RD;
#if F_WRITING
	  if (mode[0]=='w') m_mode=F_FILE_WR;
	  if (mode[0]=='a') m_mode=F_FILE_A;
	}
	if (mode[1]=='+' && mode[2]==0)
	{
	  if (mode[0]=='r') m_mode=F_FILE_RDP;
	  if (mode[0]=='w') m_mode=F_FILE_WRP;
	  if (mode[0]=='a') m_mode=F_FILE_AP;
#endif
	}

	if (m_mode==F_FILE_CLOSE) return 0; 			/*invalid mode*/

	if (_f_setfsname(filename,&fsname)) return 0; /*invalid name*/
#if F_CHECKNAME
#if F_LONGFILENAME
	if (_f_checknamewc(fsname.lname,1)) return 0;/*invalid name*/
	if (fsname.lname[0]=='.') return 0;
#else
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return 0;/*invalid name*/
	if (fsname.filename[0]=='.') return 0;
#endif
#endif
	if (_f_getvolume()) return 0; /*cant open any*/

	f=_f_getintfile();
	if (!f) return 0; /*no more file could be opened */

	if (!_f_findpath(&fsname,&f->dirpos)) return 0;
   switch (m_mode) {
#if F_WRITING
      case F_FILE_RDP: /*r*/
#endif
      case F_FILE_RD: /*r*/
#if F_LONGFILENAME
#if F_FINDING
		if (!_f_findlfilewc(fsname.lname,&f->dirpos,&de,&lfn,0)) return 0;
#else
		if (!_f_findlfilewc(fsname.lname,&f->dirpos,&de,&lfn)) return 0;
#endif
#else
#if F_FINDING
		if (!_f_findfilewc(fsname.filename,fsname.fileext,&f->dirpos,&de,0)) return 0;
#else
		if (!_f_findfilewc(fsname.filename,fsname.fileext,&f->dirpos,&de)) return 0;
#endif
#endif

   		if (de->attr & F_ATTR_DIR) return 0;		   /*directory*/

		f->startcluster=_f_getdecluster(de);

		if (f->startcluster)
		{
		  _f_clustertopos(f->startcluster,&f->pos);
		  f->filesize=_f_getlong(&de->filesize);
		  f->abspos=(unsigned long)-F_SECTOR_SIZE;
		  if (_f_fseek(f,0)) return 0;
		}
		else
		{
		  f->pos.sector=0;
		  f->pos.sectorend=0;
		}

#if F_WRITING
		if ((m_mode|F_FILE_RDP) && _f_checklocked(&f->dirpos)) return 0;
#endif

   		break;

#if F_WRITING
   	case F_FILE_AP:
   	case F_FILE_A: /*a*/
		 __memcpy(&(f->pos),&(f->dirpos),sizeof(F_POS));
#if F_LONGFILENAME
#if F_FINDING
		 if (_f_findlfilewc(fsname.lname,&f->dirpos,&de,&lfn,0))
#else
		 if (_f_findlfilewc(fsname.lname,&f->dirpos,&de,&lfn))
#endif
#else
#if F_FINDING
		 if (_f_findfilewc(fsname.filename,fsname.fileext,&f->dirpos,&de,0))
#else
		 if (_f_findfilewc(fsname.filename,fsname.fileext,&f->dirpos,&de))
#endif
#endif
		 {

		 	if (de->attr & (F_ATTR_DIR|F_ATTR_READONLY)) return 0;

		 	if (_f_checklocked(&f->dirpos)) return 0; /*locked*/


		 	f->startcluster=_f_getdecluster(de);
			f->filesize=_f_getlong(&de->filesize);
			if (f->startcluster) {
		 		_f_clustertopos(f->startcluster,&f->pos);
				f->abspos=(unsigned long)-F_SECTOR_SIZE; /*forcing seek to read 1st sector! abspos=0;*/
				if (_f_fseek(f,(long)f->filesize)) {
					f->mode=F_FILE_CLOSE;
					return 0;
				}
			}
			else {
				f->pos.sector=0;
				f->pos.sectorend=0;
				__memset(f,0,21);
			}
		 }
		 else
		 {
		 	__memcpy(&(f->dirpos),&(f->pos),sizeof(F_POS));

			_f_clustertopos(f->dirpos.cluster,&f->pos);

	   		if (_f_addentry(&fsname,&f->dirpos,&de)) return 0;  /*couldnt be added*/

			de->attr |= F_ATTR_ARC;		/*set as archiv*/
			if (_f_writedirsector()) return 0;

			f->pos.sector=0;
			f->pos.sectorend=0;
			__memset(f,0,21);
		 }
   		break;


   	case F_FILE_WR: /*w*/
   	case F_FILE_WRP: /*w+*/
		_f_clustertopos(f->dirpos.cluster,&f->pos);
#if F_LONGFILENAME
#if F_FINDING
		if (_f_findlfilewc(fsname.lname,&f->pos,&de,&lfn,0))
#else
		if (_f_findlfilewc(fsname.lname,&f->pos,&de,&lfn))
#endif
#else
#if F_FINDING
		if (_f_findfilewc(fsname.filename,fsname.fileext,&f->pos,&de,0))
#else
		if (_f_findfilewc(fsname.filename,fsname.fileext,&f->pos,&de))
#endif
#endif
                {
			unsigned long cluster=_f_getdecluster(de);	   /*exist*/

			if (de->attr & (F_ATTR_DIR|F_ATTR_READONLY)) return 0;

			__memcpy(&(f->dirpos),&(f->pos),sizeof(F_POS));

			if (_f_checklocked(&f->dirpos)) return 0; /*locked*/

			_f_setlong(de->filesize,0);					/*reset size;*/
			de->attr |= F_ATTR_ARC;		/*set as archiv*/
			_f_setword(de->clusterlo,0);					/*no points anywhere*/
			_f_setword(de->clusterhi,0);					/*no points anywhere*/

#if F_FAT32
			if (gl_volume.mediatype==F_FAT32_MEDIA) {
				_f_setword(&de->crtdate,f_getdate());  /*if there is realtime clock then creation date could be set from*/
				_f_setword(&de->crttime,f_gettime());  /*if there is realtime clock then creation time could be set from*/
				_f_setword(&de->lastaccessdate,f_getdate());  /*if there is realtime clock then creation date could be set from*/
			}
#endif

			if (_f_writedirsector()) return 0;

			if (_f_removechain(cluster)) return 0; /*remove */
		}
		else {
	   		if (_f_addentry(&fsname,&f->dirpos,&de)) return 0;  /*couldnt be added*/
			__memset(f,0,21);
			de->attr |= F_ATTR_ARC;		/*set as archiv*/
			if (_f_writedirsector()) return 0;
		}


		f->startcluster=0;
		f->pos.sector=0;
		f->pos.sectorend=0;

   		break;
#endif

	default: return 0; /*invalid mode*/
	}

	f->mode=m_mode; /* lock it */
	return f;
}


/****************************************************************************
 * _f_updatefileentry
 * Updated a file directory entry or removes the entry
 * and the fat chain belonging to it.
 ***************************************************************************/
#if F_WRITING
static unsigned char _f_updatefileentry (F_FILE *f, int remove)
{
  F_DIRENTRY *de;

  de=(F_DIRENTRY*)(gl_volume.direntry+sizeof(F_DIRENTRY)*f->dirpos.pos);

  if (_f_getdirsector(f->dirpos.sector) || remove)
  {
    (void)_f_removechain(f->startcluster);
    _f_setdecluster(de,0);
    _f_setlong(&de->filesize,0);
    (void)_f_writedirsector();
    return F_ERR_WRITE;
  }

  _f_setdecluster(de,f->startcluster);
  _f_setlong(&de->filesize,f->filesize);
  _f_setword(&de->cdate,f_getdate());  /*if there is realtime clock then creation date could be set from*/
  _f_setword(&de->ctime,f_gettime());  /*if there is realtime clock then creation time could be set from*/

#if F_FAT32
  if (gl_volume.mediatype==F_FAT32_MEDIA) {
    _f_setword(&de->lastaccessdate,f_getdate());  /*if there is realtime clock then creation date could be set from*/
  }
#endif
  return _f_writedirsector();
}
#endif


/****************************************************************************
 *
 * fn_close
 *
 * close a previously opened file
 *
 * INPUTS
 *
 * filehandle - which file needs to be closed
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

unsigned char fn_close(F_FILE *f) {
unsigned char ret;

	if (!f) return F_ERR_NOTOPEN;

	ret=_f_getvolume();
	if (ret) return ret;

	if (f->mode==F_FILE_CLOSE)
	{
	  return F_ERR_NOTOPEN;
	}
#if F_WRITING
	else if (f->mode==F_FILE_RD)
#else
    else
#endif
	{
	  f->mode=F_FILE_CLOSE;
	  return F_NO_ERROR;
	}
#if F_WRITING
	else
	{
				f->mode=F_FILE_CLOSE;

				if (_f_emptywritebuffer(f,NULL)) {
					(void)_f_updatefileentry(f,1);
					return F_ERR_WRITE;
				}

				if (_f_writefatsector()) {
					(void)_f_updatefileentry(f,1);
					return F_ERR_WRITE;
				}

				return _f_updatefileentry(f,0);
	}
#endif
}

/****************************************************************************
 *
 * fn_flush
 *
 * flush a previously opened file
 *
 * INPUTS
 *
 * filehandle - which file needs to be closed
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if (F_WRITING && F_WRITE && F_FLUSH)
unsigned char fn_flush(F_FILE *f) {
unsigned char ret;

	if (!f) return F_ERR_NOTOPEN;

	ret=_f_getvolume();
	if (ret) return ret;

	if (f->mode==F_FILE_CLOSE)
	{
	  return F_ERR_NOTOPEN;
	}
	else if (f->mode!=F_FILE_RD)
	{
		if (_f_emptywritebuffer(f,NULL)) {
			(void)_f_updatefileentry(f,1);
			return F_ERR_WRITE;
		}

		if (_f_writefatsector()) {
			(void)_f_updatefileentry(f,1);
			return F_ERR_WRITE;
		}

		return _f_updatefileentry(f,0);
	}
	return F_NO_ERROR;
}
#endif

/****************************************************************************
 *
 * fn_read
 *
 * read data from file
 *
 * INPUTS
 *
 * buf - where the store data
 * size - size of items to be read
 * _size_t - number of items need to be read
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * with the number of read bytes
 *
 ***************************************************************************/

long fn_read(void *buf,long size,long _size_st,F_FILE *f) {
char *buffer=(char*)buf;
long retsize;
unsigned char direct=0;

	if (!f) return 0;

	if ((f->mode&(F_FILE_RD|F_FILE_RDP|F_FILE_WRP|F_FILE_AP))==0) return 0;

	retsize=size;
	size*=_size_st;
	_size_st=retsize;
	retsize=0;

	if (_f_getvolume()) return 0; /*cant read any*/
	if (size+f->relpos+f->abspos >= f->filesize) {	/*read len longer than the file*/
		size=(long)((f->filesize)-(f->relpos)-(f->abspos));  /*calculate new size*/
	}

	if (size<=0) return 0;

	while (size) {
		unsigned long rdsize=(unsigned long)size;

		if (f->relpos==F_SECTOR_SIZE) {
			unsigned char ret;

			f->abspos+=f->relpos;
			f->relpos=0;

#if F_WRITING
			if (f->modified) {
				ret=_f_emptywritebuffer(f,NULL); /*empty write buffer automatically goes to next sector*/
				if (ret) {
					f->mode=F_FILE_CLOSE; /*no more read allowed*/
					return retsize;
				}

				if (_f_writefatsector()) {
					f->mode=F_FILE_CLOSE;
					return retsize;
				}

			}
#endif
			f->pos.sector++;	/*goto next*/

			if (size>=F_SECTOR_SIZE)
			{
			  direct=1;
			  ret=_f_getcurrsector(f,buffer);
			}
			else
			{
			  direct=0;
			  ret=_f_getcurrsector(f,NULL);
			}
			if (ret==F_ERR_EOF && (!size)) return retsize;

			if (ret) {
				f->mode=F_FILE_CLOSE; /*no more read allowed*/
				return retsize;
			}
		}
		
		if (!size) break;

		if (rdsize>=F_SECTOR_SIZE-f->relpos)
		{
			rdsize=F_SECTOR_SIZE-f->relpos;
		}

		if (!direct) __memcpy(buffer,f->_tdata+f->relpos,(__size_t)rdsize); /*always less than 512*/

		buffer+=rdsize;
		f->relpos+=rdsize;
		size-=rdsize;
		retsize+=rdsize;

	}

	return retsize/_size_st;

}


/****************************************************************************
 *
 * fn_write
 *
 * write data into file
 *
 * INPUTS
 *
 * buf - where the store data
 * size - size of items to be read
 * size_t - number of items need to be read
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * with the number of read bytes
 *
 ***************************************************************************/

#if (F_WRITING && F_WRITE)
long fn_write(const void *buf,long size,long _size_st,F_FILE *f) {
char *buffer=(char*)buf;
long retsize;
long ret=0;

	if (!f) return 0;

	if ((f->mode&(F_FILE_WR|F_FILE_A|F_FILE_RDP|F_FILE_WRP|F_FILE_AP))==0) return 0;

        retsize=size;
	size*=_size_st;
	_size_st=retsize;
	retsize=0;

	if (_f_getvolume()) return 0; /*can't write*/

	if (f->mode&(F_FILE_A|F_FILE_AP))
	{
	  if (_f_fseek(f,(long)f->filesize))
	  {
	    f->mode=F_FILE_CLOSE;
	    return 0;
	  }
	}

	while (size) {
		unsigned long wrsize=(unsigned long)size;

		if (f->relpos==F_SECTOR_SIZE)
		{ /*now full*/
			if (f->modified)
			{
			  if (_f_emptywritebuffer(f,NULL)) {
				f->mode=F_FILE_CLOSE;

				if (f->startcluster)
				{
				  if (_f_updatefileentry(f,0)==0) return retsize;
				}
				else
				{
				  (void)_f_updatefileentry(f,1);
				}

				return 0;
			  }
			}

			f->abspos+=f->relpos;
			f->relpos=0;

			f->pos.sector++;	/*goto next*/

			if (wrsize<F_SECTOR_SIZE)
			{
			  ret=_f_getcurrsector(f,NULL);

			  if (ret) {
				if (ret!=F_ERR_EOF) {
					f->mode=F_FILE_CLOSE; /*no more read allowed*/
					return retsize;
				}
			  }
			}
		}
		
		if (wrsize>=F_SECTOR_SIZE-f->relpos)
		{
			wrsize=F_SECTOR_SIZE-f->relpos;
		}

		if ( wrsize==F_SECTOR_SIZE )
		{
                  f->modified=1;
		  if (_f_emptywritebuffer(f,buffer)) {
			f->mode=F_FILE_CLOSE;
			(void)_f_removechain(f->startcluster);
			/*dont care with return value of _f_removechain*/
			return 0;
		  }
		}
		else
		{
		  __memcpy(f->_tdata+f->relpos,buffer,(__size_t)wrsize);
		  f->modified=1;	 /*sector is modified*/
		}
		
		buffer+=wrsize;
		f->relpos+=wrsize;
		size-=wrsize;
		retsize+=wrsize;

		if (f->filesize<f->abspos+f->relpos) {
			f->filesize=f->abspos+f->relpos;
		}

	}

	return retsize/_size_st;
}

#endif

/****************************************************************************
 *
 * fn_seek
 *
 * moves position into given offset in given file
 *
 * INPUTS
 *
 * filehandle - F_FILE structure which file position needed to be modified
 * offset - relative position
 * whence - where to calculate position (F_SEEK_SET,F_SEEK_CUR,F_SEEK_END)
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

#if (F_SEEK || F_REWIND)

unsigned char fn_seek(F_FILE *f,long offset,unsigned char whence) {
unsigned char ret;

	if (!f) return F_ERR_NOTOPEN;

	if ((f->mode&(F_FILE_RD|F_FILE_WR|F_FILE_A|F_FILE_RDP|F_FILE_WRP|F_FILE_AP))==0) return F_ERR_NOTOPEN;

	ret=_f_getvolume();
	if (ret) return ret;

	if (whence==F_SEEK_CUR)  return _f_fseek(f,(long)(f->abspos+f->relpos+offset));
	else if (whence==F_SEEK_END)  return _f_fseek(f,(long)(f->filesize+offset));
	else if (whence==F_SEEK_SET)  return _f_fseek(f,offset);

	return F_ERR_NOTUSEABLE;
}

#endif

/****************************************************************************
 *
 * fn_tell
 *
 * Tells the current position of opened file
 *
 * INPUTS
 *
 * filehandle - which file needs the position
 *
 * RETURNS
 *
 * position in the file from start
 *
 ***************************************************************************/
#if F_TELL

long fn_tell(F_FILE *f) {

	if (!f) return 0;

	if ((f->mode&(F_FILE_RD|F_FILE_WR|F_FILE_A|F_FILE_RDP|F_FILE_WRP|F_FILE_AP))==0) return 0;

	return (long)(f->abspos+f->relpos);
}

#endif


/****************************************************************************
 *
 * fn_eof
 *
 * Tells if the current position is end of file or not
 *
 * INPUTS
 *
 * filehandle - which file needs the checking
 *
 * RETURNS
 *
 * 0 - if not EOF
 * other - if EOF or invalid file handle
 *
 ***************************************************************************/
#if F_EOF

unsigned char fn_eof(F_FILE *f) {
		
	if (!f) return F_ERR_NOTOPEN;  /*if error*/
	if (f->abspos+f->relpos<f->filesize) return 0;
	return F_ERR_EOF;/*EOF*/
}

#endif



/****************************************************************************
 *
 * fn_rewind
 *
 * set the fileposition in the opened file to the begining
 *
 * INPUTS
 *
 * filehandle - which file needs to be rewinded
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if F_REWIND

unsigned char fn_rewind(F_FILE *filehandle) {
	return fn_seek( filehandle, 0L, F_SEEK_SET);
}

#endif


/****************************************************************************
 *
 * fn_putc
 *
 * write a character into file
 *
 * INPUTS
 *
 * ch - what to write into file
 * filehandle - file where to write
 *
 * RETURNS
 *
 * with the number of written bytes (1-success, 0-not successfully)
 *
 ***************************************************************************/
#if (F_PUTC & F_WRITING)

int fn_putc(int ch,F_FILE *filehandle)
{
  unsigned char tmpch=(unsigned char)ch;
  if (fn_write(&tmpch,1,1,filehandle)) return ch;
                                  else return -1;
}

#endif


/****************************************************************************
 *
 * fn_getc
 *
 * get a character from file
 *
 * INPUTS
 *
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * with the read character or -1 if read was not successfully
 *
 ***************************************************************************/
#if F_GETC
int fn_getc(F_FILE *filehandle) {
unsigned char ch;
	if (!fn_read(&ch,1,1,filehandle)) return -1;
	return (int)ch;
}

#endif



/****************************************************************************
 *
 * fn_delete
 *
 * delete a file
 *
 * INPUTS
 *
 * filename - file which wanted to be deleted (with or without path)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if (F_DELETE & F_WRITING)
unsigned char fn_delete(const char *filename) {
#if F_LONGFILENAME
F_LFNINT lfn;
F_POS dirpos;
#endif
F_POS pos;
F_DIRENTRY *de;
F_NAME fsname;
unsigned char ret;

	if (_f_setfsname(filename,&fsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
#if F_LONGFILENAME
	if (_f_checknamewc(fsname.lname,1)) return F_ERR_INVALIDNAME;/*invalid name*/
	if (fsname.lname[0]=='.') return F_ERR_NOTFOUND;
#else
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/*invalid name*/
	if (fsname.filename[0]=='.') return F_ERR_NOTFOUND;
#endif
#endif
	ret=_f_getvolume();
	if (ret) return ret;

#if F_LONGFILENAME
	if (!(_f_findpath(&fsname,&dirpos))) return F_ERR_INVALIDDIR;
	__memcpy(&pos,&dirpos,sizeof(F_POS)); /*copy directory pos*/
#if F_FINDING
	if (!_f_findlfilewc(fsname.lname,&pos,&de,&lfn,0)) return F_ERR_NOTFOUND;
#else
	if (!_f_findlfilewc(fsname.lname,&pos,&de,&lfn)) return F_ERR_NOTFOUND;
#endif
#else
	if (!(_f_findpath(&fsname,&pos))) return F_ERR_INVALIDDIR;
#if F_FINDING
	if (!_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de,0)) return F_ERR_NOTFOUND;
#else
	if (!_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de)) return F_ERR_NOTFOUND;
#endif
#endif

	if (de->attr & F_ATTR_DIR) return F_ERR_INVALIDDIR;		   /*directory*/
	if (de->attr & F_ATTR_READONLY) return F_ERR_ACCESSDENIED;      /*readonly*/
	
	if (_f_checklocked(&pos)) return F_ERR_LOCKED;

#if F_LONGFILENAME
	ret=_f_removelfn(dirpos.cluster,&lfn);
#else
	de->name[0]=(char)0xe5;	/*removes it*/
	ret=_f_writedirsector();
#endif

	if (ret) return ret;

	return _f_removechain(_f_getdecluster(de));
}
#endif



/****************************************************************************
 *
 * fn_seteof
 *
 * Set end of file
 *
 * INPUT:	f - file pointer
 *		filesize - required new size
 * RETURN:	F_NO_ERROR - on success
 *		other if error
 *
 ***************************************************************************/
#if (F_WRITING & F_SETEOF)
unsigned char fn_seteof (F_FILE *f, long filesize)
{
  unsigned char rc=F_NO_ERROR;

  if (!f) return F_ERR_NOTOPEN;  /*if error*/
  if (filesize<f->filesize)
  {
    rc=_f_emptywritebuffer(f,NULL);
    if (rc==F_NO_ERROR)
    {
      rc=_f_fseek(f,filesize);
      if (rc==F_NO_ERROR)
      {
        unsigned long cluster;
        rc=_f_getclustervalue(f->pos.cluster,&cluster);
        if (rc==F_NO_ERROR)
        {
          if (cluster!=F_CLUSTER_LAST)
          {
            rc=_f_removechain(cluster);
            if (rc) return rc;
            rc=_f_setclustervalue(f->pos.cluster,F_CLUSTER_LAST);
            if (rc) return rc;
          }
          f->filesize=(unsigned long)filesize;
        }
      }
    }
  }
  else if (filesize>f->filesize)
  {
    rc=_f_emptywritebuffer(f,NULL);
    if (rc==F_NO_ERROR) rc=_f_fseek(f,filesize);
  }
  return rc;
}
#endif



/****************************************************************************
 *
 * fn_truncate
 *
 * Open a file and set end of file
 *
 * INPUT:	filename - name of the file
 *		filesize - required new size
 * RETURN:	NULL on error, otherwise file pointer
 *
 ***************************************************************************/
#if (F_WRITING & F_TRUNCATE & F_SETEOF)
F_FILE *fn_truncate (const char *filename, long filesize)
{
  F_FILE *f=fn_open(filename,"r+");
  unsigned char rc;
  if (f!=NULL)
  {
    rc=_f_fseek(f,(long)f->filesize);
    if (rc==F_NO_ERROR)
    {
      rc=fn_seteof(f,filesize);
    }
    if (rc)
    {
      fn_close(f);
      f=NULL;
    }
  }
  return f;
}
#endif

