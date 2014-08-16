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

#include <hcc/fatfs_thin/dir_lfn.h>
#include <hcc/fatfs_thin/util.h>
#include <hcc/fatfs_thin/volume.h>
#include <hcc/fatfs_thin/file.h>
#include <hcc/fatfs_thin/fat.h>

#include "drv.h"

#if F_LONGFILENAME
/****************************************************************************
 *
 * _f_getnextstring
 *
 * finding in a string a string until / or \ termination
 *
 * INPUTS
 *
 * dest - where to store next string
 * sou - original text
 *
 * RETURNS
 *
 * new start position in the original
 *
 ***************************************************************************/
#if F_DIRECTORIES
static char *_f_getnextstring(char *dest, char *sou) {
	for (;;) {
		char ch=*sou;
		if (!ch) break;
		sou++;
		if (ch=='/' || ch=='\\') break;
		
		*dest++=ch;
	}
	*dest=0;
	return sou;
}
#endif



/****************************************************************************
 *
 * _f_getdirsector
 *
 * read a directory sector
 *
 * INPUTS
 *
 * sector - which sector is needed
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

unsigned char _f_getdirsector(unsigned long sector) {
unsigned char ret=F_NO_ERROR;
	ret=_f_readsector(gl_volume.direntry,sector);
	if (!ret) gl_volume.direntrysector=sector;
	     else gl_volume.direntrysector=(unsigned long)(-1);
	return ret;
}

/****************************************************************************
 *
 * _f_writedirsector
 *
 * Write directory sector, which was last read
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

#if F_WRITING

unsigned char _f_writedirsector(void) {
   if (gl_volume.direntrysector!=(unsigned long)(-1)) {
   	return _f_writesector(gl_volume.direntry,gl_volume.direntrysector);
   }
   return F_ERR_INVALIDSECTOR;
}

#endif







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

static void _f_getfilename(char *dest, char *name, char *ext)
{
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
 * _f_addlfn
 *
 * add next directory entry as long filename entry, this routine will build
 * the next file name, even if its long filename or short filename
 *
 * INPUTS
 *
 * de - directory entry to add
 * plfn - pointer to lfn structure where to build
 * num - number of directory entry
 *
 ***************************************************************************/
static void _f_addlfn(F_DIRENTRY *de,F_LFNINT *plfn, unsigned long num) {
F_LFN *lfn=(F_LFN *)de;
long pos;

   if ((!de->name[0]) || ((unsigned char)(de->name[0])==0xe5)) {
      plfn->state=F_LFNSTATE_INV;
      return;
   }

   if (de->attr == F_ATTR_LFN) {

      if (_f_getword(lfn->clusterlo)) {  /*this is always zero in lfn!!*/
         plfn->state=F_LFNSTATE_INV;
         return;
      }

      pos=(long)(lfn->ord & 0x3f);

      if (pos<1 || pos>20) {
         plfn->state=F_LFNSTATE_INV;
         return;
      }

      pos--;
      pos*=13; /*get current char pos;*/

      if (lfn->ord & 0x40) {  /*if its the end of lfn (reverse order!)*/
         plfn->ord=lfn->ord;
         plfn->state=F_LFNSTATE_NEXT;
         plfn->name[pos+13]=0; /*set zero terminate*/
         plfn->chksum=lfn->chksum;
         plfn->start=num;
         plfn->end=num;
      }
      else {

         if (plfn->chksum!=lfn->chksum) {
            plfn->state=F_LFNSTATE_INV;
            return;
         }

         if (plfn->state==F_LFNSTATE_NEXT) {
            if (lfn->ord != ((plfn->ord & 0x3f)-1)) { /*if not the next*/
               plfn->state=F_LFNSTATE_INV;
               return;
            }

            plfn->ord=lfn->ord;
         }
         else {
            plfn->state=F_LFNSTATE_INV;
            return;
         }
      }

      {
        char *b=&(plfn->name[pos]);
        char *c=(char *)lfn;
	unsigned char cpos[13] = {1,3,5,7,9,14,16,18,20,22,24,28,30};
	unsigned char i;
	for (i=0;i<13;*b++=*(c+cpos[i++]))
	  ;
      }
   }
   else
   { /*check sum check*/
      if (plfn->state==F_LFNSTATE_NEXT)
      {
         if ((plfn->ord & 0x3f) == 1)
         {
            unsigned char chksum=0;
            unsigned char *ptr;
            unsigned char a;

            ptr=(unsigned char *)de;
	    for (a=0;a<11;a++)
	    {
               chksum = (unsigned char)((((chksum&1)<<7) | (( chksum & 0xfe) >>1)) + ((*ptr++)&0xff));
            }

            if ((chksum&0xff) == plfn->chksum) {
               plfn->state=F_LFNSTATE_LFN;
               plfn->end=num;
               return;
            }
         }
      }

      plfn->start=plfn->end=num; /*only short name*/

      _f_getfilename(plfn->name,(char *)de->name,(char *)de->ext);

      if (de->ntres & NTRES_LOW) _f_strtolower((char*)plfn->name);

      plfn->state=F_LFNSTATE_SFN;
   }
}











#if (F_DELETE || F_RMDIR || F_WRITING || F_DIRECTORIES || F_GETLABEL || F_SETLABEL)
/****************************************************************************
 *
 * _f_opendir
 *
 * open a directory as file
 *
 * INPUTS
 * cluster - directory start cluster
 *
 * RETURNS
 * file pointer or zero if any error
 *
 ***************************************************************************/
F_FILE *_f_opendir(unsigned long cluster)
{
  F_FILE *f=0;

	f=&gl_files[F_MAXFILES];
	__memset((char *)f,0,22);
	if (!f) return 0; /*no more file could be opened */

	f->startcluster=cluster;

	_f_clustertopos(f->startcluster,&f->pos);

	if (_f_readsector(f->_tdata,f->pos.sector)) return 0;

	f->mode=F_FILE_RD;
	return f;
}


/****************************************************************************
 *
 * _f_nextdirsector
 *
 * check and go to next directory sector
 *
 * INPUTS
 * f - filepointer
 *
 * RETURNS
 * error code or zero if successfull
 *
 ***************************************************************************/
static unsigned char _f_nextdirsector (F_FILE *f)
{
unsigned char ret;

	if (f->relpos==F_SECTOR_SIZE)
	{
#if F_WRITING
		if (f->modified)
		{
			ret=_f_writesector(f->_tdata,f->pos.sector);
			if (ret) return ret;
			f->modified=0;
		}
#endif
		if ((f->pos.sector+1)==f->pos.sectorend)
		{
			if (f->pos.cluster>=F_CLUSTER_RESERVED) return F_ERR_EOF;

	  		if (!f->pos.cluster) {
#if F_FAT32
		   		if (gl_volume.mediatype==F_FAT32_MEDIA)
		   		{
					f->pos.cluster=gl_volume.bootrecord.rootcluster;
				}
				else
#endif
				return F_ERR_EOF; /*root directory*/
			}

			{
				unsigned long nextcluster;
				if (_f_getclustervalue(f->pos.cluster,&nextcluster)) return F_ERR_EOF; /*not found*/
				if (nextcluster>=F_CLUSTER_RESERVED) return F_ERR_EOF;
				_f_clustertopos(nextcluster,&f->pos);
			}
		}
		else
		{
			f->pos.sector++;	
		}

		ret=_f_readsector(f->_tdata,f->pos.sector);
		if (ret) return ret;

		f->relpos=0;
		f->abspos+=F_SECTOR_SIZE;
	}

	return F_NO_ERROR;
}



/****************************************************************************
 *
 * _f_seekdir
 *
 * seek into directory
 *
 * INPUTS
 *
 * file - filepointer
 * offset - number of directory entries
 * whence - how to seek
 *
 * RETURNS
 *
 * error code or zero if successfull
 *
 ***************************************************************************/
unsigned char _f_seekdir(F_FILE *f,long offset, unsigned char whence)
{
unsigned char ret;

	if (!f) return F_ERR_NOTOPEN;

	if (offset==-1 && whence==F_SEEK_CUR && f->relpos) {
		f->relpos-=sizeof (F_DIRENTRY);
		return F_NO_ERROR;
	}

	offset*=sizeof(F_DIRENTRY);

	switch (whence) {
	case F_SEEK_CUR:  offset=(long)(f->abspos+f->relpos+offset); break;
	case F_SEEK_SET:  break;
	default: return F_ERR_NOTUSEABLE;
	}

#if F_WRITING
	if (f->modified) {
		ret=_f_writesector(f->_tdata,f->pos.sector);
		if (ret) return ret;
		f->modified=0;
	}
#endif
	f->relpos=0;
	f->abspos=0;

	_f_clustertopos(f->startcluster,&f->pos);

	while (1)
	{
		if (offset<F_SECTOR_SIZE) {
			f->relpos=(unsigned long)offset;

			ret=_f_readsector(f->_tdata,f->pos.sector);
			if (ret) return ret;
			return F_NO_ERROR;
		}

		offset-=F_SECTOR_SIZE;
		f->abspos+=F_SECTOR_SIZE;

		if ((f->pos.sector+1)==f->pos.sectorend) {

			if (f->pos.cluster>=F_CLUSTER_RESERVED) return F_ERR_EOF;

	  		if (!f->pos.cluster) {
#if F_FAT32
				if (gl_volume.mediatype==F_FAT32_MEDIA)
				{
					f->pos.cluster=gl_volume.bootrecord.rootcluster;
				}
				else
#endif
				return F_ERR_EOF; /*root directory*/
			}

			{
				unsigned long nextcluster;
				if (_f_getclustervalue(f->pos.cluster,&nextcluster)) return F_ERR_EOF; /*not found*/
				if (nextcluster>=F_CLUSTER_RESERVED) return F_ERR_EOF;
				_f_clustertopos(nextcluster,&f->pos);
			}
		}
		else {
			f->pos.sector++;	
		}
	}
}


/****************************************************************************
 *
 * _f_writedir
 *
 * write directory entry into directory
 *
 * INPUTS
 * de - directory entry needs written
 * file - filepointer
 *
 * RETURNS
 * error code or zero if successfull
 *
 ***************************************************************************/

unsigned char _f_writedir(F_DIRENTRY *de,F_FILE *f)
{
unsigned char ret;

	if (!f) return F_ERR_NOTOPEN;

	ret=_f_nextdirsector(f);
	if (ret) return ret;

	__memcpy(f->_tdata+f->relpos,de,sizeof(F_DIRENTRY));
	f->relpos+=sizeof (F_DIRENTRY);

	f->modified=1;

	return F_NO_ERROR;
}

/****************************************************************************
 *
 * _f_readdir
 *
 * read next directory entry
 *
 * INPUTS
 * de - where to store information
 * file - filepointer
 *
 * RETURNS
 * error code or zero if successfull
 *
 ***************************************************************************/
unsigned char _f_readdir(F_DIRENTRY *de,F_FILE *f)
{
unsigned char ret;

	if (!f) return F_ERR_NOTOPEN;

	ret=_f_nextdirsector(f);
	if (ret) return ret;

	__memcpy(de,f->_tdata+f->relpos,sizeof(F_DIRENTRY));
	f->relpos+=sizeof (F_DIRENTRY);

	return F_NO_ERROR;
}


/****************************************************************************
 *
 * _f_closedir
 *
 * close a directory, and carry on error code
 *
 * INPUTS
 * file - file pointer
 * errcode - errorcode
 *
 * RETURNS
 * error code or zero if successfull
 *
 ***************************************************************************/
unsigned char _f_closedir(F_FILE *f, unsigned char errcode)
{

	if (!f) {
		if (errcode) return errcode;
		return F_ERR_NOTOPEN;
	}

	f->mode=F_FILE_CLOSE;

#if F_WRITING
	if (f->modified) {
		int ret;
		ret=_f_writesector(f->_tdata,f->pos.sector);
		if (ret) {
			if (errcode) return errcode;
			return (unsigned char)ret;
		}
		f->modified=0;
	}
#endif
	if (errcode) return errcode;
	return F_NO_ERROR;
}
#endif




#if ((F_DELETE || F_RMDIR) && F_WRITING)
/****************************************************************************
 *
 * _f_removelfn
 *
 * remove lfn entry from directory
 *
 * INPUTS
 * cluster - cluster of directory
 * lfn - long file name to remove (findfile fills starts and ends)
 *
 * RETURNS
 * 0 - if successfully
 * or error code
 *
 ***************************************************************************/
unsigned char _f_removelfn(unsigned long cluster, F_LFNINT *lfn)
{
F_DIRENTRY de;
unsigned long a;
F_FILE *file;
unsigned char ret;

	if (lfn->state!=F_LFNSTATE_LFN && lfn->state!=F_LFNSTATE_SFN) {
		return F_ERR_INVALIDNAME;
	}

	file=_f_opendir(cluster);
	if (!file) return F_ERR_INVALIDDIR;

	ret=_f_seekdir(file,(long)lfn->start,F_SEEK_SET);
	if (ret) return	_f_closedir(file,ret);

	for (a=lfn->start; a<=lfn->end; a++)
	{
		ret=_f_readdir(&de,file);		
		if (ret) return	_f_closedir(file,ret);

		ret=_f_seekdir(file,-1,F_SEEK_CUR);
		if (ret) return	_f_closedir(file,ret);

		de.name[0]=(char)0xe5;

		ret=_f_writedir(&de,file);		
		if (ret) return	_f_closedir(file,ret);
	}

   return _f_closedir(file,F_NO_ERROR);
}
#endif






/****************************************************************************
 *
 * _fs_findlfilewc
 *
 * internal function to finding file in directory entry with or
 * without wildcard.
 *
 * INPUTS
 * lname - filename
 * pos - where to start searching, and contains current position
 * pde - store back the directory entry pointer
 * lfn - long file name structure
 * wc - use wildcard checking?
 *
 * RETURNS
 * 0 - if file was not found
 * 1 - if file was found
 *
 ***************************************************************************/
#if F_FINDING
unsigned char _f_findlfilewc(char *lname,F_POS *pos, F_DIRENTRY **pde, F_LFNINT *lfn, unsigned char wc)
#else
unsigned char _f_findlfilewc(char *lname,F_POS *pos, F_DIRENTRY **pde, F_LFNINT *lfn)
#endif
{
unsigned long denum=0;
char sname[8+3+2];

   lfn->state=F_LFNSTATE_INV;

	while (pos->cluster<F_CLUSTER_RESERVED) {

		for (;pos->sector<pos->sectorend; pos->sector++) {
			F_DIRENTRY *de=(F_DIRENTRY*)(gl_volume.direntry+sizeof(F_DIRENTRY)*pos->pos);

			if (_f_getdirsector(pos->sector)) return 0; /*not found*/

			for (; pos->pos<F_SECTOR_SIZE/sizeof(F_DIRENTRY);de++,pos->pos++)
			{

				_f_addlfn(de,lfn,denum);
				denum++;

				if (!de->name[0]) return 0;						 /*empty*/
				if ((unsigned char)(de->name[0])==0xe5) continue; /*deleted*/
				if (de->attr & F_ATTR_VOLUME) continue;

				_f_getfilename(sname,(char *)de->name,(char *)de->ext);
#if F_FINDING
				if (wc)
				{
				  char *buf;
				  unsigned short b,c;

				  for (buf=lfn->name;buf!=sname;buf=sname)
				  {
				    for (b=0,c=0;;b++,c++) {
					char ch=_f_toupper(buf[c]);
					char chfind=lname[b];

					if (chfind=='*') {    /*try to get next extension*/
						for (;;b++) {
							ch=lname[b];
							if (!ch) {
								if (pde) *pde=de;
								return 1;
							}
							if (ch=='.') break;
						}

						for (;;c++) {
							ch=buf[c];
							if (!ch) {
								c--;
								break;
							}
							if (ch=='.') break;
						}
						continue;
					}

					if (chfind!='?') {
						if (ch != _f_toupper(chfind)) break; /*differs*/
					}
					if (!ch) {
						if (pde) *pde=de;
						return 1;
					}
				    }
				  }
				}
				else
#endif
				{
				  unsigned short b;
				  char *buf;

				  for (buf=lfn->name;buf!=sname;buf=sname)
				  {
				    for (b=0;;b++) {
					char ch=_f_toupper(buf[b]);
					if (ch != _f_toupper(lname[b])) break; /*differs*/
					if (!ch) {
						if (pde) *pde=de;
						return 1;
					}
				    }
				  }
				}
			}

			pos->pos=0;
		}

 		if (!pos->cluster) {
#if F_FAT32
   			if (gl_volume.mediatype==F_FAT32_MEDIA) {
				pos->cluster=gl_volume.bootrecord.rootcluster;
			}
			else
#endif
			return 0; /*root directory*/
		}

		{
			unsigned long nextcluster;
			if (_f_getclustervalue(pos->cluster,&nextcluster)) return 0; /*not found*/
			if (nextcluster>=F_CLUSTER_RESERVED) return 0; /*not found*/
			_f_clustertopos(nextcluster,pos);
		}
	}

	return 0;
}



/****************************************************************************
 *
 * _f_getdecluster
 *
 * get a directory entry structure start cluster value
 *
 * INPUTS
 * de - directory entry
 *
 * RETURNS
 *
 * directory entry cluster value
 *
 ***************************************************************************/

unsigned long _f_getdecluster(F_DIRENTRY *de) {
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
 * fsname - filled structure with path,drive
 * pos - where to start searching, and contains current position
 *
 * RETURNS
 * 0 - if path was not found or invalid
 * 1 - if path was found
 *
 ***************************************************************************/
unsigned char _f_findpath(F_NAME *fsname,F_POS *pos)
{
#if F_DIRECTORIES
F_LFNINT lfn;
F_DIRENTRY *de;
char *path=fsname->path;
char currname[F_MAXPATH];
char oripath[F_MAXPATH];
unsigned short pathpos=0;
unsigned short a;

	oripath[0]=0;

	_f_clustertopos(0,pos);
	for (;*path;) {
		path=_f_getnextstring(currname,path);

		if ((pos->cluster==0) && (currname[0]=='.') && (!currname[1])) {
			_f_clustertopos(0,pos);
			continue;
		}
		else {
#if F_FINDING
			if (!_f_findlfilewc(currname,pos,&de,&lfn,0)) return 0;
#else
			if (!_f_findlfilewc(currname,pos,&de,&lfn)) return 0;
#endif
			if (!(de->attr & F_ATTR_DIR ) ) return 0;
			_f_clustertopos(_f_getdecluster(de),pos);
		}
		
		if (lfn.name[0]=='.') {
			if (!lfn.name[1]) {
				continue; /*nothing to modify*/
			}
			else if (lfn.name[1]=='.') {
				if (lfn.name[2]) return 0; /*more than 2 dots*/

				if (!pathpos) return 0; /*we are on the top*/
				for (;pathpos;) {		/*set back with one level*/
					pathpos--;
					if (oripath[pathpos]=='/') break;
				}
			}
		}
		else {
			if (pathpos) oripath[pathpos++]='/';
			for (a=0;;) {
				char ch=lfn.name[a++];
				if (!ch) break;
				oripath[pathpos++]=ch;
			}
		}
	}
	oripath[pathpos]=0; /*terminates it;*/
	__memcpy(fsname->path,oripath,F_MAXPATH);
	return 1; /*found*/
#else	
	_f_clustertopos(0,pos);
	return 1;
#endif
}





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
void _f_initentry(F_DIRENTRY *de,char *name, char *ext) {

   __memset (de,0,sizeof(F_DIRENTRY)); /*reset all entries*/

	__memcpy (de->name,name,sizeof(de->name));
	__memcpy (de->ext,ext,sizeof(de->ext));

	_f_setword(&de->cdate,f_getdate());  /*if there is realtime clock then creation date could be set from*/
	_f_setword(&de->ctime,f_gettime());  /*if there is realtime clock then creation time could be set from*/

}
#endif





/****************************************************************************
 *
 * _f_createlfn
 *
 * Create a raw lfn entry and short name,extension from long filename
 *
 * INPUTS
 *
 * lname - original long file name
 * lfn - lfn structure where to create raw lfn entry
 * name - where to create 8 bytes short name
 * ext - where to create 3 bytes short extension
 *
 ***************************************************************************/
#if F_WRITING
static void _f_createlfn(char *lname, F_LFNINT *lfn, char *name,char *ext) {
unsigned short len;
unsigned short a,b;

	lfn->state=F_LFNSTATE_INV;

        len=0;
        while (lname[len++])
          ;

	lfn->end=len;

	__memset(name,' ',8);
	__memset(ext,' ',3);

	for (a=len;a>0; a--) {
		if (lname[a-1]=='.') {
			for (b=0;;a++) {
				char ch=_f_toupper(lname[a]);
				if (!ch) break;
				if (ch==32) continue;
				if (ch=='[' || ch==']') ch='_';
				ext[b++]=ch;
				if (b==3) break;
			}
			break;
		}
	}

	for (a=b=0;; a++) {
		char ch=_f_toupper(lname[a]);
		if (!ch) break;
		if (ch==32) continue;
		if (ch=='.') break;
		if (ch=='[' || ch==']') ch='_';
		name[b++]=ch;
		if (b==8) break;
	}

    _f_getfilename(lfn->name,name,ext);
	for (a=0; ;a++) {
		if (lname[a] != lfn->name[a]) {

			for (a=8; a; a--) {
				if (name[a-1]!=32) break;
			}

			if (a)
			{
			  if (a>6) a=6;
			  lfn->start=a; /*start counter of name*/
			  lfn->state=F_LFNSTATE_LFN;
			  lfn->ord=(unsigned char)((len+12)/13);
			}
			return;
		}
		if (!lfn->name[a]) break;
	}
	
 	lfn->state=F_LFNSTATE_SFN;
	lfn->ord=0; /*no lfn needed*/
}
#endif


/****************************************************************************
 * num2str
 *
 * Converts a number to string and puts '~' before it.
 * Needed to generate short filename that belongs to a long filename.
 *
 * INPUTS
 * a - the number to convert
 * str - pointer to the string where to store the result
 *
 * OUTPUT
 * length of the string
 ***************************************************************************/
#if F_WRITING
static unsigned char num2str (unsigned short a, char *str)
{
  unsigned char b,rc;

  *str++='~';
  for (rc=2,b=100;b>1;b/=10)
  {
    unsigned char r=(unsigned char)(a/b);
    if (r)
    {
      *str++=(char)(r+'0');
      ++rc;
    }
  }
  *str=(char)((a%10)+'0');
  return rc;
}
#endif

/****************************************************************************
 *
 * _f_addentry
 *
 * Add a new directory entry into driectory list
 *
 * INPUTS
 * fsname - filled F_NAME structure
 * pos - where directory cluster chains starts
 * pde - F_DIRENTRY pointer where to store the entry where it was added
 *
 * RETURNS				
 * 0 - if successfully added
 * other - if any error (see FS_xxx errorcodes)
 *
 ***************************************************************************/
#if F_WRITING
unsigned char _f_addentry(F_NAME *fsname,F_POS *pos,F_DIRENTRY **pde) {
#if F_FAT32
unsigned char fat32root=0;
#endif
char name[8];
char ext[3];
F_LFNINT lfn;
unsigned char ret;
F_FILE *f;
long num=0;
unsigned char len=0;
long start=-1;
F_DIRENTRY de;

	if (!fsname->lname[0]) return F_ERR_INVALIDNAME;
	if (fsname->lname[0]=='.') return F_ERR_INVALIDNAME;

	_f_createlfn(fsname->lname,&lfn,name,ext);

	if (lfn.state==F_LFNSTATE_INV) return F_ERR_INVALIDNAME;

#if F_FAT32
	if ((!pos->cluster) && (gl_volume.mediatype==F_FAT32_MEDIA))	fat32root=1;
#endif

	f=_f_opendir(pos->cluster);
	if (!f) return F_ERR_INVALIDDIR;

	while (1)
	{
		ret=_f_readdir(&de,f);
		if (ret==F_ERR_EOF)
		{
			unsigned short a;
			unsigned long cluster;

			if (!f || (!f->pos.cluster) || (f->pos.cluster>=F_CLUSTER_RESERVED))
			{
				return _f_closedir(f,F_ERR_NOMOREENTRY); /*EOF!! root FAT16,FAT12;*/
			}

			ret=_f_alloccluster(&cluster);	/*get a new one*/
			if (ret) return _f_closedir(f,ret);

			if (cluster<F_CLUSTER_RESERVED) {
#if F_FAT32			
				if (fat32root)
				  ret=_f_setclustervalue(cluster,F_CLUSTER_LASTF32R);
				else
#endif
				  ret=_f_setclustervalue(cluster,F_CLUSTER_LAST);

				if (ret) return _f_closedir(f,ret);

				ret=_f_setclustervalue(f->pos.cluster,cluster);
				if (ret) return _f_closedir(f,ret);
				
				ret=_f_writefatsector();
				if (ret) return _f_closedir(f,ret);
			}
			else return _f_closedir(f,F_ERR_NOMOREENTRY);

			__memset(&de,0,sizeof(F_DIRENTRY));

			a=gl_volume.bootrecord.sector_per_cluster;
			a*=F_SECTOR_SIZE/sizeof(F_DIRENTRY);
			while (a--) {
				ret=_f_writedir(&de,f);
				if (ret) return	_f_closedir(f,ret);
			}

			ret=_f_seekdir(f,num,F_SEEK_SET);
			if (ret) return _f_closedir(f,ret);

			ret=_f_readdir(&de,f); /*read new entry*/
		}

		if (ret) return _f_closedir(f,ret);

		if ((!de.name[0]) || ((unsigned char)(de.name[0])==0xe5)) {
			if (start==-1) {
				start=num;
				len=0;
			}
			len++;

			if (len-1==lfn.ord) {

				if (lfn.state==F_LFNSTATE_SFN) {
					ret=_f_seekdir(f,start,F_SEEK_SET);
					if (ret) return _f_closedir(f,ret);

					_f_initentry(&de,name,ext);	
#if F_FAT32
					if (gl_volume.mediatype==F_FAT32_MEDIA) {
						_f_setword(&de.crtdate,f_getdate());  /*if there is realtime clock then creation date could be set from*/
						_f_setword(&de.crttime,f_gettime());  /*if there is realtime clock then creation time could be set from*/
						_f_setword(&de.lastaccessdate,f_getdate());  /*if there is realtime clock then creation date could be set from*/
					}
#endif
					ret=_f_writedir(&de,f);
					if (ret) return _f_closedir(f,ret);
				}
				else {
					unsigned short a;
					unsigned char b,ok;
					F_LFN *plfn=(F_LFN *)(&de);

					for(a=1;;a++) {
					        unsigned char n;
					        char str_n[4];
					
						if (a>999) return _f_closedir(f,F_ERR_NOMOREENTRY);
					        n=num2str(a,str_n);
					        if (lfn.start>(unsigned long)(8-n)) lfn.start=(unsigned long)(8-n);
					        __memcpy(name+lfn.start,str_n,(__size_t)n);


						ret=_f_seekdir(f,0,F_SEEK_SET);
						if (ret) return _f_closedir(f,ret);

						for (;;) {
							ok=1;
							ret=_f_readdir(&de,f);
							if (ret==F_ERR_EOF) break;
							if (ret) return _f_closedir(f,ret);

							if ((!de.name[0]) || ((unsigned char)(de.name[0])==0xe5)) continue;

							for (b=0; b<8; b++) {
								if (de.name[b]!=name[b]) {
									ok=0;
									break;
								}
							}
							if (ok) {
								for (b=0; b<3; b++) {
									if (de.ext[b]!=ext[b]) {
										ok=0;
										break;
									}
								}
							}

							if (ok) { /*if its found*/
								ok=0; /*name is not ok*/
								break;
							}
						}

					 	if (ok) break;
					}

					ret=_f_seekdir(f,start,F_SEEK_SET);
					if (ret) return _f_closedir(f,ret);

					{
			            unsigned char chksum=0;

						for (a=0; a<8; a++) {
							chksum = (unsigned char)((((chksum&1)<<7) | (( chksum & 0xfe) >>1)) + name[a]);
						}

						for (a=0; a<3; a++) {
							chksum = (unsigned char)((((chksum&1)<<7) | (( chksum & 0xfe) >>1)) + ext[a]);
						}
						lfn.chksum=chksum;
					}

					for (a=lfn.ord; a>0; a--) {
						unsigned long npos=(unsigned long)((a-1)*13);
						__memset(plfn,0,sizeof(F_LFN));

						if (a==lfn.ord) plfn->ord=(unsigned char)(a+0x40);
						else plfn->ord=(unsigned char)a;

						plfn->chksum=lfn.chksum;
					
						{
						  unsigned char cpos[13] = {1,3,5,7,9,14,16,18,20,22,24,28,30};
						  char *bb=&fsname->lname[npos];
						  unsigned char i;

						  for (i=0;i<13;i++)
						  {
						    char *cc=(char *)plfn+cpos[i];
						    if (npos+i<lfn.end) *cc=*bb++;
						                   else *cc=*(cc+1)=(char)255;
						  }
						}

						plfn->attr=F_ATTR_LFN;

						ret=_f_writedir(&de,f);
						if (ret) return _f_closedir(f,ret);
					}

					_f_initentry(&de,name,ext);	
#if F_FAT32
					if (gl_volume.mediatype==F_FAT32_MEDIA) {
						_f_setword(&de.crtdate,f_getdate());  /*if there is realtime clock then creation date could be set from*/
						_f_setword(&de.crttime,f_gettime());  /*if there is realtime clock then creation time could be set from*/
						_f_setword(&de.lastaccessdate,f_getdate());  /*if there is realtime clock then creation date could be set from*/
					}
#endif
					ret=_f_writedir(&de,f);
					if (ret) return _f_closedir(f,ret);
				}

				if (pde) {

					if (!f) return _f_closedir(f,F_ERR_NOTOPEN);

					if (f->relpos>=sizeof(F_DIRENTRY)) {
						gl_volume.direntrysector=f->pos.sector;
						__memcpy(gl_volume.direntry,f->_tdata,F_SECTOR_SIZE);
						*pde=(F_DIRENTRY*)(gl_volume.direntry+f->relpos-sizeof(F_DIRENTRY));

						pos->cluster=f->pos.cluster;
						pos->pos=f->relpos/sizeof(F_DIRENTRY)-1;
						pos->sector=f->pos.sector;
						pos->sectorend=f->pos.sectorend;

					}
					else return	_f_closedir(f,F_ERR_NOTOPEN);

				}

				return _f_closedir(f,F_NO_ERROR); /*if ok*/
			}
		}
		else start=-1;

		num++;
	}

}
#endif








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
	if (_f_checknamewc(find->findfsname.lname,0)) return F_ERR_INVALIDNAME;/*invalid name, wildcard is ok*/
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
unsigned char fn_findnext(F_FIND *find)
{
F_LFNINT lfn;
F_DIRENTRY *de;
unsigned char a;
unsigned char ret;

	ret=_f_getvolume();
	if (ret) return ret;

	if (!_f_findlfilewc(find->findfsname.lname,&find->pos,&de,&lfn,1)) return F_ERR_NOTFOUND;

	for (a=0;a<F_MAXNAME; a++) find->name[a]=de->name[a];
	for (a=0;a<F_MAXEXT; a++) find->ext[a]=de->ext[a];

    __memcpy(find->filename,lfn.name,F_MAXPATH);

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
#if (F_CHDIR && F_DIRECTORIES)
unsigned char fn_chdir(const char *dirname)
{
F_POS pos;
F_NAME fsname;
unsigned short len;
unsigned short a;
unsigned char ret;

	ret=_f_setfsname(dirname,&fsname);
	if (ret==1) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsname.lname,1)) return F_ERR_INVALIDNAME;/*invalid name, wildcard is ok*/
#endif
	ret=_f_getvolume();
	if (ret) return ret;

	for (len=0;fsname.path[len];) len++;
	if (len) fsname.path[len++]='/';

	for (a=0;;) {
		char ch=fsname.lname[a++];
		fsname.path[len++]=ch;
		if (!ch) break;
	}
	
	if (!(_f_findpath(&fsname,&pos))) return F_ERR_NOTFOUND;

	for (a=0; a<F_MAXPATH;a++) gl_volume.cwd[a]=fsname.path[a];

	return F_NO_ERROR;
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
F_LFNINT lfn;
F_FILE *f;
F_POS posdir;
F_POS pos;
F_DIRENTRY *de;
F_NAME fsname;
unsigned long cluster;
unsigned char ret;

	if (_f_setfsname(dirname,&fsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsname.lname,1)) return F_ERR_INVALIDNAME;/*invalid name*/
	if (fsname.lname[0]=='.') return F_ERR_NOTFOUND;
#endif
	ret=_f_getvolume();
	if (ret) return ret;

	if (!_f_findpath(&fsname,&posdir)) return F_ERR_INVALIDDIR;

	pos=posdir;

#if F_FINDING
	if (_f_findlfilewc(fsname.lname,&pos,&de,&lfn,0)) return F_ERR_DUPLICATED;
#else
	if (_f_findlfilewc(fsname.lname,&pos,&de,&lfn)) return F_ERR_DUPLICATED;
#endif

	pos=posdir;

	ret=_f_alloccluster(&cluster);
	if (ret) return ret;

	ret=_f_addentry(&fsname,&pos,&de);
	if (ret) return ret;

	de->attr |= F_ATTR_DIR;		/*set as directory*/




	f=_f_getintfile();

	if (!f) return F_ERR_NOMOREENTRY;

	_f_clustertopos(cluster,&f->pos);

	_f_setdecluster(de,cluster); /*new dir*/


	de=(F_DIRENTRY *)(f->_tdata);

	_f_initentry(de,".       ","   ");
	de->attr = F_ATTR_DIR;		/*set as directory*/
	_f_setdecluster(de,cluster);	 /*current*/
	de++;

	_f_initentry(de,"..      ","   ");
	de->attr = F_ATTR_DIR;		/*set as directory*/
	_f_setdecluster(de,posdir.cluster); /*parent*/
	de++;

	__memset(de,0,(__size_t)(F_SECTOR_SIZE-2*sizeof(F_DIRENTRY)));
	

	ret=_f_writesector(f->_tdata,f->pos.sector);
	if (ret) return ret;

	f->pos.sector++;

	__memset(f->_tdata,0,(__size_t)(2*sizeof(F_DIRENTRY)));
	

	while(f->pos.sector<f->pos.sectorend) {
write_again:
		ret=_f_writesector(f->_tdata,f->pos.sector);
		if (ret) {
		 	if (gl_volume.state!=F_STATE_WORKING) return ret; /*check if still working volume*/

		 	ret=_f_dobadblock(f);
		 	if (ret) return ret;

		 	goto write_again;
		}

		f->pos.sector++;
	}

	ret=_f_setclustervalue(f->pos.cluster,F_CLUSTER_LAST);
	if (ret) return ret;

	ret=_f_writefatsector();
	if (ret) return ret;


	return _f_writedirsector(); /*writing  direntry*/
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
F_LFNINT lfn;
F_POS dirpos,pos;
F_DIRENTRY *pde;
F_NAME fsname;
F_FILE *file;
unsigned long dircluster;
unsigned char ret;

	if (_f_setfsname(dirname,&fsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsname.lname,1)) return F_ERR_INVALIDNAME;/*invalid name*/
	if (fsname.lname[0]=='.') return F_ERR_NOTFOUND;
#endif

	ret=_f_getvolume();
	if (ret) return ret;

	if (!(_f_findpath(&fsname,&dirpos))) return F_ERR_INVALIDDIR;

	__memcpy(&pos,&dirpos,sizeof(F_POS)); /*copy directory pos*/

#if F_FINDING
	if (!_f_findlfilewc(fsname.lname,&pos,&pde,&lfn,0)) return F_ERR_NOTFOUND;
#else
	if (!_f_findlfilewc(fsname.lname,&pos,&pde,&lfn)) return F_ERR_NOTFOUND;
#endif

	if (!(pde->attr & F_ATTR_DIR)) return F_ERR_INVALIDDIR;		   /*not a directory*/

	dircluster=_f_getdecluster(pde);

	file=_f_opendir(dircluster);
	if (!file) return F_ERR_INVALIDDIR;

	while (1) {
		F_DIRENTRY de;
		char ch;

		ret=_f_readdir(&de,file);
		if (ret==F_ERR_EOF) break;
		if (ret) return _f_closedir(file,ret);

		ch=de.name[0];
		if (!ch) break;	 /*last entry*/
		if ((unsigned char)ch==0xe5) continue; /*deleted*/
		if (ch=='.') continue;

		return _f_closedir(file,F_ERR_NOTEMPTY);/*something is there*/
	}

	ret=_f_closedir(file,F_NO_ERROR);
	if (ret) return ret;

	ret=_f_removelfn(dirpos.cluster,&lfn);
	if (ret) return ret;

	return _f_removechain(dircluster);
}

#endif






/****************************************************************************
 *
 * fn_settimedate
 *
 * set a file time and date
 *
 * INPUTS
 *
 * filename - which file time and date wanted to be set
 * ctime - new ctime of the file
 * cdate - new cdate of the file
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if (F_SETTIMEDATE & F_WRITING)

unsigned char fn_settimedate(const char *filename,unsigned short ctime,unsigned short cdate) {
F_LFNINT lfn;
F_POS pos;
F_DIRENTRY *de;
F_NAME fsname;
unsigned char ret;

	if (_f_setfsname(filename,&fsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsname.lname,1)) return F_ERR_INVALIDNAME;/*invalid name*/
#endif
	ret=_f_getvolume();
	if (ret) return ret;

	if (!_f_findpath(&fsname,&pos)) return F_ERR_INVALIDDIR;
#if F_FINDING
	if (!_f_findlfilewc(fsname.lname,&pos,&de,&lfn,0)) return F_ERR_NOTFOUND;
#else
	if (!_f_findlfilewc(fsname.lname,&pos,&de,&lfn)) return F_ERR_NOTFOUND;
#endif

	_f_setword(&de->ctime,ctime);
	_f_setword(&de->cdate,cdate);

	return _f_writedirsector();
}

#endif

/****************************************************************************
 *
 * fn_gettimedate
 *
 * get a file time and date
 *
 * INPUTS
 *
 * filename - which file time and date wanted to be retrive
 * pctime - ctime of the file where to store
 * pcdate - cdate of the file where to store
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if F_GETTIMEDATE

unsigned char fn_gettimedate(const char *filename,unsigned short *pctime,unsigned short *pcdate) {
F_LFNINT lfn;
F_POS pos;
F_DIRENTRY *de;
F_NAME fsname;
unsigned char ret;

	if (_f_setfsname(filename,&fsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsname.lname,1)) return F_ERR_INVALIDNAME;/*invalid name*/
#endif
	ret=_f_getvolume();
	if (ret) return ret;

	if (!_f_findpath(&fsname,&pos)) return F_ERR_INVALIDDIR;
#if F_FINDING
	if (!_f_findlfilewc(fsname.lname,&pos,&de,&lfn,0)) return F_ERR_NOTFOUND;
#else
	if (!_f_findlfilewc(fsname.lname,&pos,&de,&lfn)) return F_ERR_NOTFOUND;
#endif


	if (pctime) *pctime=_f_getword(&de->ctime);
	if (pcdate) *pcdate=_f_getword(&de->cdate);

	return F_NO_ERROR;
}

#endif




/****************************************************************************
 *
 * fn_getattr
 *
 * get file attribute
 *
 * INPUTS
 *
 * filename - which file attribute is needed
 * attr - pointer to a characterter where to store attribute information
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if F_GETATTR

unsigned char fn_getattr(const char *filename,unsigned char *attr) {
F_LFNINT lfn;
F_POS pos;
F_DIRENTRY *de;
F_NAME fsname;
unsigned char ret;

	if (_f_setfsname(filename,&fsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsname.lname,1)) return F_ERR_INVALIDNAME;/*invalid name*/
#endif
	ret=_f_getvolume();
	if (ret) return ret;

	if (!_f_findpath(&fsname,&pos)) return F_ERR_INVALIDDIR;
#if F_FINDING
	if (!_f_findlfilewc(fsname.lname,&pos,&de,&lfn,0)) return F_ERR_NOTFOUND;
#else
	if (!_f_findlfilewc(fsname.lname,&pos,&de,&lfn)) return F_ERR_NOTFOUND;
#endif

	if (attr) *attr=de->attr;

   return F_NO_ERROR;
}

#endif



/****************************************************************************
 *
 * fn_setattr
 *
 * set attribute of a file
 *
 * INPUTS
 *
 * filename - which file attribute set
 * attr - new attribute of the file
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if (F_SETATTR & F_WRITING)

unsigned char fn_setattr(const char *filename,unsigned char attr) {
F_LFNINT lfn;
F_POS pos;
F_DIRENTRY *de;
F_NAME fsname;
unsigned char ret;

	if (_f_setfsname(filename,&fsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsname.lname,1)) return F_ERR_INVALIDNAME;/*invalid name*/
#endif
	ret=_f_getvolume();
	if (ret) return ret;

	if (!_f_findpath(&fsname,&pos)) return F_ERR_INVALIDDIR;
#if F_FINDING
	if (!_f_findlfilewc(fsname.lname,&pos,&de,&lfn,0)) return F_ERR_NOTFOUND;
#else
	if (!_f_findlfilewc(fsname.lname,&pos,&de,&lfn)) return F_ERR_NOTFOUND;
#endif

	attr&=F_ATTR_HIDDEN|F_ATTR_READONLY|F_ATTR_ARC|F_ATTR_SYSTEM; /*keep only valid bits*/

	attr|=de->attr&(F_ATTR_DIR|F_ATTR_VOLUME);	/*add old settings, (dir,volume)*/
	if (de->attr==attr) return F_NO_ERROR; /*its the same*/

	de->attr=attr;

	return _f_writedirsector();
}

#endif

#endif   /* F_LONGFILENAME */

