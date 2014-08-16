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

#include <hcc/fatfs_sthin/dir.h>
#include <hcc/fatfs_sthin/util.h>
#include <hcc/fatfs_sthin/volume.h>
#include <hcc/fatfs_sthin/fat.h>

#include "drv.h"


/****************************************************************************
 *
 * _f_findfilewc
 *
 * internal function to finding file in directory entry with or without
 * wildcard
 *
 * INPUTS
 *
 * name - filename
 * ext - fileextension
 * pos - where to start searching, and contains current position
 * pde - store back the directory entry pointer
 * wc - wildcard checking?
 *
 * RETURNS
 *
 * 0 - if file was not found
 * 1 - if file was found
 *
 ***************************************************************************/
#if F_FINDING
unsigned char _f_findfilewc(char *name,char *ext,F_POS *pos, F_DIRENTRY **pde, unsigned char wc)
#else
unsigned char _f_findfilewc(char *name,char *ext,F_POS *pos, F_DIRENTRY **pde) 
#endif
{
#if F_DIRECTORIES || F_FAT32
	while (pos->cluster<F_CLUSTER_RESERVED) 
	{
#endif
		for (;pos->sector<pos->sectorend; pos->sector++) {
			F_DIRENTRY *de=(F_DIRENTRY*)(gl_sector+sizeof(F_DIRENTRY)*pos->pos);

			if (_f_readglsector(pos->sector)) return 0; /*not found*/

			for (; pos->pos<F_SECTOR_SIZE/sizeof(F_DIRENTRY);de++,pos->pos++) {
				unsigned char b,ok;

				if (!de->name[0]) return 0;						 /*empty*/
				if ((unsigned char)(de->name[0])==0xe5) continue; /*deleted*/
				if (de->attr & F_ATTR_VOLUME) continue;

#if F_FINDING
				if (wc)
				{
				  for (b=0,ok=1; b<sizeof(de->name); b++) {
					if (name[b]=='*') break;

					if (name[b]!='?') {
						if (de->name[b]!=name[b]) {
							ok=0;
							break;
						}
					}
				  }
				
				  if (ok)
				  {
				    for (b=0,ok=1; b<sizeof(de->ext); b++) {
					if (ext[b]=='*') {
						if (pde) *pde=de;
						return 1;
					}
				
					if (ext[b]!='?') {
						if (de->ext[b]!=ext[b]) {
							ok=0;
							break;
						}
					}
				
				    }

				    if (ok) {
					if (pde) *pde=de;
					return 1;
				    }
				  }
				}
				else
#endif
				{
				  for (b=0,ok=1; b<sizeof(de->name); b++) {
					if (de->name[b]!=name[b]) {
						ok=0;
						break;
					}
				  }

				  if (ok)
				  {
				    for (b=0,ok=1; b<sizeof(de->ext); b++) {
					if (de->ext[b]!=ext[b]) {
						ok=0;
						break;
					}
				    }

				    if (ok) {
					if (pde) *pde=de;
					return 1;
				    }
				  }
				}
			}

			pos->pos=0;
		}

#if F_DIRECTORIES || F_FAT32
 		if (!pos->cluster) 
 		{
#if F_FAT32
 		  if (gl_volume.mediatype==F_FAT32_MEDIA) pos->cluster=gl_volume.bootrecord.rootcluster;
 		  else
#endif
 		  return 0;
 		}
 		
		{
			unsigned long nextcluster;
			gl_volume.fatsector=(unsigned long)-1;
			if (_f_getclustervalue(pos->cluster,&nextcluster)) return 0; /*not found*/

	   		if (nextcluster>=F_CLUSTER_RESERVED) return 0; /*eof*/

			_f_clustertopos(nextcluster,pos);
		}
	}
#endif

	return 0;
}


/****************************************************************************
 *
 * _f_getfilename
 *
 * create a complete filename from name and extension
 *
 * INPUTS
 *
 * dest - where to store filename
 * name - name of the file
 * ext - extension of the file
 *
 ***************************************************************************/
static void _f_getfilename(char *dest, char *name,char *ext) {
unsigned char a,len;

	for (len=a=F_MAXNAME; a; a--,len--) {
		if (name[a-1]!=' ') break;
	}
	for (a=0; a<len; a++) *dest++=*name++;


	for (len=a=F_MAXEXT; a; a--,len--) {
		if (ext[a-1]!=' ') break;
	}
	if (len) *dest++='.';

	for (a=0; a<len; a++) *dest++=*ext++;

	*dest=0; /*terminateit*/
}




/****************************************************************************
 *
 * _f_getdecluster
 *
 * get a directory entry structure start cluster value
 *
 * INPUTS
 *
 * de - directory entry
 *
 * RETURNS
 *
 * directory entry cluster value
 *
 ***************************************************************************/
unsigned long _f_getdecluster(F_DIRENTRY *de) 
{
#if F_FAT32
	unsigned long cluster;
	if (gl_volume.mediatype==F_FAT32_MEDIA) {
		cluster=_f_getword(&de->clusterhi);
		cluster<<=16;
		cluster|=_f_getword(&de->clusterlo);
		return cluster;
	}
#endif
	return _f_getword(&de->clusterlo);
}

/****************************************************************************
 *
 * _f_setdecluster
 *
 * set a directory entry structure start cluster value
 *
 * INPUTS
 *
 * de - directory entry
 * cluster - value of the start cluster
 *
 ***************************************************************************/
#if F_WRITING
void _f_setdecluster(F_DIRENTRY *de,unsigned long cluster) {
	_f_setword(&de->clusterlo,(unsigned short)(cluster&0xffff));
#if F_FAT32
	if (gl_volume.mediatype==F_FAT32_MEDIA) {
		_f_setword(&de->clusterhi,(unsigned short)(cluster>>16));
	}
	else
#endif
	{
		_f_setword(&de->clusterhi,(unsigned short)0);
	}
}
#endif

/****************************************************************************
 *
 * _f_findpath
 *
 * finding out if path is valid in F_NAME and
 * correct path info with absolute path (removes relatives)
 *
 * INPUTS
 *
 * fsname - filled structure with path,drive
 * pos - where to start searching, and contains current position
 *
 * RETURNS
 *
 * 0 - if path was not found or invalid
 * 1 - if path was found
 *
 ***************************************************************************/

unsigned char _f_findpath(F_NAME *fsname,F_POS *pos) {
#if F_DIRECTORIES
char *path=fsname->path;
char *mpath=path;
F_DIRENTRY *de;

	_f_clustertopos(0,pos);

	for (;*path;) {
		char name[F_MAXNAME];
		char ext[F_MAXEXT];

		unsigned char len=_f_setnameext(path,name,ext);

		if (pos->cluster==0 && len==1 && name[0]=='.') {
			_f_clustertopos(0,pos);
		}
		else {
#if F_FINDING
			if (!_f_findfilewc(name,ext,pos,&de,0)) return 0;
#else
			if (!_f_findfilewc(name,ext,pos,&de)) return 0;
#endif
			if (!(de->attr & F_ATTR_DIR ) ) return 0;

			_f_clustertopos(_f_getdecluster(de),pos);
		}


		if (name[0]=='.') {
			if (len==1) {
				path+=len;

				if (!(*path)) {
					if (mpath!=fsname->path) mpath--; /*if we are now at the top*/
					break;
				}
				path++;
				continue;
			}

			if (name[1]!='.') return 0; /*invalid name*/
			if (len!=2) return 0; /*invalid name !*/

			path+=len;

			if (mpath==fsname->path) return 0; /*we are in the top*/
			
			mpath--; /*no on separator*/
			for (;;) {
				if (mpath==fsname->path) break; /*we are now at the top*/
				mpath--;
				if (*mpath=='/') {
					mpath++;
					break;
				}
			}

			if (!(*path)) {
				if (mpath!=fsname->path) mpath--; /*if we are now at the top*/
				break;
			}
			path++;
			continue;
			
		}
		else {
			if (path==mpath) {							/*if no was dots just step*/
				path+=len;
				mpath+=len;
			}
			else {
				unsigned char a;
				for (a=0; a<len;a++) *mpath++=*path++;	 /*copy if in different pos*/
			}
		}

		if (!(*path)) break;
		path++;
		*mpath++='/'; /*add separator*/
	}

	*mpath=0; /*terminate it*/
	return 1;
#else
	_f_clustertopos(0,pos);
   return 1;
#endif
}


/****************************************************************************
 *
 * fn_getcwd
 *
 * getting a current working directory of current drive
 *
 * INPUTS
 *
 * buffer - where to store current working folder
 * maxlen - buffer length (possible size is F_MAXPATH)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

#if F_DIRECTORIES

unsigned char fn_getcwd(char *buffer, unsigned char maxlen, char root ) {
unsigned char a;

	if (!maxlen) return F_NO_ERROR;

	maxlen--;	/*need for termination*/
	if (root && maxlen)
	{
	  *buffer++='/';
	  maxlen--;
	}

	for (a=0; a<maxlen; a++) {
		char ch=gl_volume.cwd[a];
		buffer[a]=ch;
		if (!ch) break;
	}

	buffer[a]=0;	/*add terminator at the end*/

	return F_NO_ERROR;
}

#endif

/****************************************************************************
 *
 * fn_findfirst
 *
 * find a file(s) or directory(s) in directory
 *
 * INPUTS
 *
 * filename - filename (with or without wildcards)
 * find - where to store found file information
 *
 * RETURNS
 *																			 
 * error code or zero if successful
 *																			 
 ***************************************************************************/

#if F_FINDING

unsigned char fn_findfirst(const char *filename,F_FIND *find) {
unsigned char ret;
	if (_f_setfsname(filename,&find->findfsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checkname(find->findfsname.filename,find->findfsname.fileext)) return F_ERR_INVALIDNAME;/*invalid name, wildcard is ok*/
#endif

	ret=_f_getvolume();
	if (ret) return ret;

	if (!_f_findpath(&find->findfsname,&find->pos)) return F_ERR_INVALIDDIR; /*search for path*/

	return fn_findnext(find);
}

#endif 

/****************************************************************************
 *
 * fn_findnext
 *
 * find further file(s) or directory(s) in directory
 *
 * INPUTS
 *
 * find - where to store found file information (findfirst should call 1st)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

#if F_FINDING

unsigned char fn_findnext(F_FIND *find) {
F_DIRENTRY *de;
unsigned char a;
unsigned char ret;

	ret=_f_getvolume();
	if (ret) return ret;

	if (!_f_findfilewc(find->findfsname.filename,find->findfsname.fileext,&find->pos,&de,1)) return F_ERR_NOTFOUND;
	
	for (a=0;a<F_MAXNAME; a++) find->name[a]=de->name[a];
	for (a=0;a<F_MAXEXT; a++) find->ext[a]=de->ext[a];

	_f_getfilename(find->filename,(char *)de->name,(char *)de->ext);

	find->attr=de->attr;
	find->cdate=_f_getword(&de->cdate);
	find->ctime=_f_getword(&de->ctime);
	find->filesize=(long)_f_getlong(&de->filesize);
	find->cluster=_f_getdecluster(de);
	find->pos.pos++; /*goto next position*/

	return 0;
}

#endif

/****************************************************************************
 *
 * fn_chdir
 *
 * change current working directory
 *
 * INPUTS
 *
 * dirname - new working directory name
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error 
 *
 ***************************************************************************/
#if (F_CHDIR & F_DIRECTORIES)
unsigned char fn_chdir(const char *dirname) {
F_POS pos;
F_NAME fsname;
unsigned char len;
unsigned char a;
unsigned char ret;

	ret=_f_setfsname(dirname,&fsname);

	if (ret==1) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/*invalid name*/
#endif
	ret=_f_getvolume();
	if (ret) return ret;

	for (len=0;fsname.path[len];) len++;
	if (len && (fsname.filename[0]!=32 || fsname.fileext[0]!=32)) fsname.path[len++]='/';

	_f_getfilename(fsname.path+len,fsname.filename,fsname.fileext);

	if (!(_f_findpath(&fsname,&pos))) return F_ERR_NOTFOUND;

	for (a=0; a<F_MAXPATH;a++) gl_volume.cwd[a]=fsname.path[a];

	return F_NO_ERROR;
}
#endif



/****************************************************************************
 *
 * _f_initentry
 *
 * init directory entry, this function is called if a new entry is coming
 *
 * INPUTS
 *
 * de - directory entry which needs to be initialized
 * name - fil ename  (8)
 * ext - file extension (3)
 *
 ***************************************************************************/
#if F_WRITING
static void _f_initentry(F_DIRENTRY *de,char *name, char *ext) {

   __memset (de,0,sizeof(F_DIRENTRY)); /*reset all entries*/

	__memcpy (de->name,name,sizeof(de->name));
	__memcpy (de->ext,ext,sizeof(de->ext));

	_f_setword(&de->cdate,f_getdate());  /*if there is realtime clock then creation date could be set from*/
	_f_setword(&de->ctime,f_gettime());  /*if there is realtime clock then creation time could be set from*/

}
#endif





/****************************************************************************
 *
 * _f_addentry
 *
 * Add a new directory entry into driectory list
 *
 * INPUTS
 *
 * fs_name - filled structure what to add into directory list
 * pos - where directory cluster chains starts
 * pde - F_DIRENTRY pointer where to store the entry where it was added
 *
 * RETURNS
 *
 * 0 - if successfully added
 * other - if any error (see FS_xxx errorcodes)
 *
 ***************************************************************************/
#if F_WRITING
unsigned char _f_addentry(F_NAME *fsname,F_POS *pos,F_DIRENTRY **pde) {
unsigned char ret;

	if (!fsname->filename[0]) return F_ERR_INVALIDNAME;
	if (fsname->filename[0]=='.') return F_ERR_INVALIDNAME;

#if F_DIRECTORIES || F_FAT32
	while (pos->cluster<F_CLUSTER_RESERVED) {
#endif
		for (;pos->sector<pos->sectorend; pos->sector++) {
			F_DIRENTRY *de=(F_DIRENTRY*)(gl_sector+sizeof(F_DIRENTRY)*pos->pos);

			ret=_f_readglsector(pos->sector);
			if (ret) return ret;

			for (; pos->pos<F_SECTOR_SIZE/sizeof(F_DIRENTRY);de++,pos->pos++) {
				if ((!de->name[0]) || ((unsigned char)(de->name[0])==0xe5)) {
					_f_initentry(de,fsname->filename,fsname->fileext);
#if F_FAT32
					if (gl_volume.mediatype==F_FAT32_MEDIA) {
						_f_setword(&de->crtdate,f_getdate());  /*if there is realtime clock then creation date could be set from*/
						_f_setword(&de->crttime,f_gettime());  /*if there is realtime clock then creation time could be set from*/
						_f_setword(&de->lastaccessdate,f_getdate());  /*if there is realtime clock then creation date could be set from*/
					}
#endif
					if (pde) *pde=de;
					return F_NO_ERROR;
				}
			}

			pos->pos=0;
		}

#if F_DIRECTORIES || F_FAT32
		if (!pos->cluster) 
		{
#if F_FAT32
		  if (gl_volume.mediatype==F_FAT32_MEDIA) pos->cluster=gl_volume.bootrecord.rootcluster;
		  else
#endif
		  return F_ERR_NOMOREENTRY;
		}

		{
			unsigned long cluster;
			
			gl_volume.fatsector=(unsigned long)-1;
			ret=_f_getclustervalue(pos->cluster,&cluster); /*try to get next cluster*/
			if (ret) return ret;

			if (cluster<F_CLUSTER_RESERVED) {
				_f_clustertopos(cluster,pos);
			}
			else {
				ret=_f_alloccluster(&cluster);	/*get a new one*/
				if (ret) return ret;

				if (cluster<F_CLUSTER_RESERVED) {
					if (gl_file.mode!=F_FILE_CLOSE) return F_ERR_NOMOREENTRY;
					_f_clustertopos(cluster,&gl_file.pos);

					ret=_f_setclustervalue(gl_file.pos.cluster,F_CLUSTER_LAST);
					if (ret) return ret;

					ret=_f_setclustervalue(pos->cluster,gl_file.pos.cluster);
					if (ret) return ret;
				  
					ret=_f_writefatsector();
					if (ret) return ret;

					gl_volume.fatsector=(unsigned long)-1;
					__memset(gl_sector,0,F_SECTOR_SIZE);
					while(gl_file.pos.sector<gl_file.pos.sectorend) 
					{
					  ret=_f_writeglsector(gl_file.pos.sector);
					  if (ret)  return ret;
					  gl_file.pos.sector++;
					}
		
					_f_clustertopos(gl_file.pos.cluster,pos);
				}
				else return F_ERR_NOMOREENTRY;

			}
		}
	}
#endif

	return F_ERR_NOMOREENTRY;
}
#endif



/****************************************************************************
 *
 * fn_mkdir
 *
 * making a new directory
 *
 * INPUTS
 *
 * dirname - new directory name
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if (F_MKDIR & F_DIRECTORIES & F_WRITING)
unsigned char fn_mkdir(const char *dirname) {
F_POS posdir;
F_POS pos;
F_DIRENTRY *de;
F_NAME fsname;
unsigned long cluster;
unsigned char ret;

	if (_f_setfsname(dirname,&fsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/*invalid name*/
#endif
	ret=_f_getvolume();
	if (ret) return ret;

	if (!_f_findpath(&fsname,&posdir)) return F_ERR_INVALIDDIR;

	pos=posdir;

	if (fsname.filename[0]=='.') return F_ERR_NOTFOUND;
#if F_FINDING
	if (_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de,0)) return F_ERR_DUPLICATED;
#else
	if (_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de)) return F_ERR_DUPLICATED;
#endif

	pos=posdir;

	gl_volume.fatsector=(unsigned long)-1;
	ret=_f_alloccluster(&cluster);
	if (ret) return ret;

	ret=_f_addentry(&fsname,&pos,&de);
	if (ret) return ret;

	de->attr |= F_ATTR_DIR;		/*set as directory*/

        if (gl_file.mode!=F_FILE_CLOSE) return F_ERR_LOCKED;
	_f_clustertopos(cluster,&gl_file.pos);
	_f_setdecluster(de,cluster); /*new dir*/

	(void)_f_writeglsector((unsigned long)-1);	/*write actual directory sector*/


	de=(F_DIRENTRY *)gl_sector;

	_f_initentry(de,".       ","   ");
	de->attr = F_ATTR_DIR;		/*set as directory*/
	_f_setdecluster(de,cluster);	 /*current*/
	de++;

	_f_initentry(de,"..      ","   ");
	de->attr = F_ATTR_DIR;		/*set as directory*/
	_f_setdecluster(de,posdir.cluster); /*parent*/
	de++;

	__memset(de,0,(__size_t)(F_SECTOR_SIZE-2*sizeof(F_DIRENTRY)));
	

	ret=_f_writeglsector(gl_file.pos.sector);
	if (ret) return ret;

	gl_file.pos.sector++;
	__memset(gl_sector,0,(__size_t)(2*sizeof(F_DIRENTRY)));
	while(gl_file.pos.sector<gl_file.pos.sectorend) 
	{
	  ret=_f_writeglsector(gl_file.pos.sector);
	  if (ret) return ret;
	  gl_file.pos.sector++;
	}

	gl_volume.fatsector=(unsigned long)-1;
	ret=_f_setclustervalue(gl_file.pos.cluster,F_CLUSTER_LAST);
	if (ret) return ret;

	return _f_writefatsector();
}
#endif



/****************************************************************************
 *
 * fn_rmdir
 *
 * Remove directory, only could be removed if empty
 *
 * INPUTS
 *
 * dirname - which directory needed to be removed
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if (F_RMDIR & F_DIRECTORIES & F_WRITING)
unsigned char fn_rmdir(const char *dirname) {
unsigned char ret;
F_POS pos;
F_DIRENTRY *de;
F_NAME fsname;
unsigned long dirsector;
unsigned char a;

	if (_f_setfsname(dirname,&fsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/*invalid name*/
	if (fsname.filename[0]=='.') return F_ERR_NOTFOUND;
#endif
	ret=_f_getvolume();
	if (ret) return ret;

	if (!(_f_findpath(&fsname,&pos))) return F_ERR_INVALIDDIR;
#if F_FINDING
	if (!_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de,0)) return F_ERR_NOTFOUND;
#else
	if (!_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de)) return F_ERR_NOTFOUND;
#endif

	if (!(de->attr & F_ATTR_DIR)) return F_ERR_INVALIDDIR;		   /*not a directory*/
        dirsector=gl_volume.actsector;
        
        if (gl_file.mode!=F_FILE_CLOSE) return F_ERR_LOCKED;
	_f_clustertopos(_f_getdecluster(de),&gl_file.pos);

	while (1) {
		F_DIRENTRY *de2;
		char ch;

		ret=_f_getcurrsector();
		if (ret==F_ERR_EOF) break;
		if (ret) return ret;

		de2=(F_DIRENTRY *)gl_sector;
		for (a=0; a<(F_SECTOR_SIZE/sizeof(F_DIRENTRY)); a++,de2++) {
			ch=de2->name[0];
			if (!ch) break;
			if ((unsigned char)ch==0xe5) continue;
			if (ch=='.') continue;
			return F_ERR_NOTEMPTY; /*something is there*/
		}
		if (!ch) break;
		gl_file.pos.sector++;
	}

        ret=_f_readglsector(dirsector);
        if (ret) return ret;
	de->name[0]=(char)0xe5;

	ret=_f_writeglsector(dirsector);
	if (ret) return ret;

	gl_volume.fatsector=(unsigned long)-1; 
	return _f_removechain(_f_getdecluster(de));
}
#endif


