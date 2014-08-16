#ifndef _TEST_C_
#define _TEST_C_

/****************************************************************************
 *
 *            Copyright (c) 2003 by HCC Embedded 
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

#include "test.h"

static W_CHAR cwd[F_MAXPATH];
static F_FIND find;


#if F_LONGFILENAME /* long file name contains exact names */
#ifdef HCC_UNICODE
static wchar tmpwch[F_MAXPATH];
#endif

W_CHAR *f_nameconv(char *s) {
#ifdef HCC_UNICODE
	return _towchar(tmpwch,s);
#else
	return s;
#endif
}
#else

char stmp[1024]; 

char *f_nameconv(char *s) {
char *ss=stmp;
	for (;;) {
		char ch=*s++;
		if (ch>='a' && ch<='z') {
			ch+='A'-'a';
		}
		*ss++=ch;
		if (!ch) break;
	}
	return stmp;
}
#endif

#ifdef HCC_UNICODE
int STRWCMP (wchar *s1, wchar *s2)
{
  while (*s1)
  {
    if (*s1!=*s2) return 1;
    ++s1;
    ++s2;
  }
  if (*s2) return 1;
  return 0;
}
#endif

int STRCMP (char *s1, char *s2)
{
  while (*s1)
  {
    if (*s1!=*s2) return 1;
    ++s1;
    ++s2;
  }
  if (*s2) return 1;
  return 0;
}

int f_formatting(void) {
int ret;

	_f_dump ("f_formatting");

/* checking formatting */
	ret=f_format(0,F_FAT_MEDIA);
	if (ret) return _f_result(0,ret);
	ret=_f_poweron();
	if (!ret) return _f_result(1,ret);
	ret=f_findfirst("*.*",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(2,ret);

	_f_dump("passed...");
	return 0;
}

int _f_checkcwd(W_CHAR *orig) {
int ret;
#ifdef HCC_UNICODE
	ret=f_wgetcwd(cwd,F_MAXPATH);
#else
	ret=f_getcwd(cwd,F_MAXPATH);
#endif

	if (ret) return ret;

#ifdef HCC_UNICODE
	if (STRWCMP(orig,cwd)) return -1;
#else
	if (STRCMP(orig,cwd)) return -1;
#endif
	return 0;
}

int f_dirtest(void) {
int ret;

	_f_dump("f_dirtest");
/* creates a ab abc abcd */
	ret=f_mkdir("a");
	if (ret) return _f_result(1,ret);
	ret=f_mkdir("ab");
	if (ret) return _f_result(2,ret);
	ret=f_mkdir("abc");
	if (ret) return _f_result(3,ret);
	ret=f_mkdir("abca");
	if (ret) return _f_result(4,ret);

/* creates directories in /a  -  a ab abc abcd */
	ret=f_mkdir("a/a");
	if (ret) return _f_result(5,ret);
	ret=f_mkdir("a/ab");
	if (ret) return _f_result(6,ret);
	ret=f_mkdir("a/abc");
	if (ret) return _f_result(7,ret);
	ret=f_mkdir("a/abcd");
	if (ret) return _f_result(8,ret);

/* change into a/abcd and check cwd */
	ret=f_chdir("a/abcd");
	if (ret) return _f_result(9,ret);
	ret=_f_checkcwd(f_nameconv("/a/abcd"));
	if (ret) return _f_result(10,ret);

/* make directory t change into t and check cwd="a/abcd/t" */
	ret=f_mkdir("t");
	if (ret) return _f_result(11,ret);
	ret=f_chdir("t");
	if (ret) return _f_result(12,ret);
	ret=_f_checkcwd(f_nameconv("/a/abcd/t"));
	if (ret) return _f_result(13,ret);
	ret=f_chdir(".");
	if (ret) return _f_result(14,ret);
	ret=_f_checkcwd(f_nameconv("/a/abcd/t"));
	if (ret) return _f_result(15,ret);
	ret=f_chdir("../.");
	if (ret) return _f_result(16,ret);
	ret=_f_checkcwd(f_nameconv("/a/abcd"));
	if (ret) return _f_result(17,ret);

/* removing t dir */
	ret=f_rmdir("t");
	if (ret) return _f_result(18,ret);
	ret=f_chdir("t");
	if (ret!=F_ERR_NOTFOUND) return _f_result(19,ret);

/* removing /a dir */
	ret=f_rmdir("/ab");
	if (ret) return _f_result(20,ret);
	ret=f_chdir("/ab");
	if (ret!=F_ERR_NOTFOUND) return _f_result(21,ret);

/* removing /a dir */
	ret=f_rmdir("../../a");
	if (ret!=F_ERR_NOTEMPTY) return _f_result(22,ret);

/* removing /abca dir */
	ret=f_rmdir("a:/abca");
	if (ret) return _f_result(24,ret);

/* changing invalid dirs */
	ret=f_chdir("");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(25,ret);
	ret=f_chdir(" ");
	if (ret) return _f_result(26,ret);
	ret=_f_checkcwd(f_nameconv("/a/abcd"));
	if (ret) return _f_result(27,ret);
	ret=f_chdir("?");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(28,ret);
	ret=f_chdir("*.*");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(29,ret);
	ret=_f_checkcwd(f_nameconv("/a/abcd"));
	if (ret) return _f_result(30,ret);

/* changing into /abc and removes subfolder from /a/ */
	ret=f_chdir("/abc");
	if (ret) return _f_result(31,ret);
	ret=f_rmdir("/a/a");
	if (ret) return _f_result(32,ret);
	ret=f_rmdir("A:../a/ab");
	if (ret) return _f_result(33,ret);
	ret=f_rmdir("A:/a/abc");
	if (ret) return _f_result(34,ret);
	ret=f_rmdir(".././abc/.././a/../a/abcd");
	if (ret) return _f_result(35,ret);

/* some invalid rmdir */
	ret=f_rmdir(".");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(36,ret);
	ret=f_rmdir("..");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(37,ret);

/* create again abc remove abc */
	ret=f_mkdir(".././abc");
	if (ret!=F_ERR_DUPLICATED) return _f_result(38,ret);
	ret=f_rmdir("../abc");
	if (ret) return _f_result(39,ret);
	ret=f_mkdir(".././abc");
	if (ret!=F_ERR_INVALIDDIR) return _f_result(40,ret); /* cwd is not exist */
	ret=f_chdir("/");
	if (ret) return _f_result(41,ret);

/* try . and .. in the root */
	ret=f_chdir(".");
	if (ret) return _f_result(42,ret);
	ret=f_chdir("./././.");
	if (ret) return _f_result(43,ret);
	ret=f_chdir("..");
	if (ret!=F_ERR_NOTFOUND) return _f_result(44,ret);
#ifdef HCC_UNICODE
	ret=_f_checkcwd(_towchar(tmpwch,"/")); /* root! */
#else
	ret=_f_checkcwd("/"); /* root! */
#endif
	if (ret) return _f_result(45,ret);

/* test . and .. in a and remove a */
	ret=f_chdir("a");
	if (ret) return _f_result(46,ret);
	ret=f_chdir("..");
	if (ret) return _f_result(47,ret);
	ret=f_chdir("a");
	if (ret) return _f_result(48,ret);
	ret=f_chdir(".");
	if (ret) return _f_result(49,ret);
	ret=f_chdir("a");
	if (ret!=F_ERR_NOTFOUND) return _f_result(50,ret);
	ret=f_chdir("./..");
	if (ret) return _f_result(51,ret);
	ret=f_rmdir("a");
	if (ret) return _f_result(52,ret);

/* check if all are removed */
	ret=f_findfirst("*.*",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(53,ret);

	_f_dump("passed...");
	return 0;
}

int f_findingtest(void) {
int ret;

	_f_dump("f_findingtest");

/* check empty */
	ret=f_findfirst("*.*",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(0,ret);

/* create Hello.dir */
	ret=f_mkdir("Hello.dir");
	if (ret) return _f_result(1,ret);

/* check if it is exist, and only exist */
	ret=f_findfirst("*.*",&find);
	if (ret) return _f_result(2,ret);

#ifdef HCC_UNICODE
	if (STRCMP(find.filename,"Hello.dir")) return _f_result(3,0);
#else
	if (STRCMP(find.filename,f_nameconv("Hello.dir"))) return _f_result(3,0);
#endif
	if (find.attr!=F_ATTR_DIR) return _f_result(4,0);
	ret=f_findnext(&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(5,ret);

/* check some not founds */
	ret=f_findfirst("q*.*",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(6,ret);
	ret=f_findfirst("Hello.",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(7,ret);
	ret=f_findfirst("a/*.*",&find);
	if (ret!=F_ERR_INVALIDDIR) return _f_result(8,ret);
	ret=f_findfirst(".",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(9,ret);
	ret=f_findfirst("..",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(10,ret);
	ret=f_findfirst("?e.*",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(11,ret);
	ret=f_findfirst("*.",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(12,ret);
	ret=f_findfirst("*.?",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(13,ret);
	ret=f_findfirst("*.??",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(14,ret);


/* check some founds */
	ret=f_findfirst("*.dir",&find);
	if (ret) return _f_result(15,ret);
	ret=f_findfirst("*.d?r",&find);
	if (ret) return _f_result(16,ret);
	ret=f_findfirst("*.d??",&find);
	if (ret) return _f_result(17,ret);
	ret=f_findfirst("*.???",&find);
	if (ret) return _f_result(18,ret);
	ret=f_findfirst("?ello.???",&find);
	if (ret) return _f_result(19,ret);
	ret=f_findfirst("he??o.dir",&find);
	if (ret) return _f_result(20,ret);
	ret=f_findfirst("he?*.dir",&find);
	if (ret) return _f_result(21,ret);
	ret=f_findfirst("HELLO.DIR",&find); /* no capitals sensitivity in find!! */
	if (ret) return _f_result(22,ret);

/* change into hello.dir */
	ret=f_chdir("hello.dir");
	if (ret) return _f_result(23,ret);
	ret=f_findfirst("*.*",&find);
	if (ret) return _f_result(24,ret);
	ret=f_findfirst("..",&find);
	if (ret) return _f_result(25,ret);
	ret=f_findfirst("??",&find);
	if (ret) return _f_result(26,ret);
	ret=f_findfirst(".",&find);
	if (ret) return _f_result(27,ret);
	ret=f_findfirst("k*.*",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(28,ret);
	ret=f_findfirst("*.",&find);
	if (ret) return _f_result(29,ret);
	if (STRCMP(find.filename,".")) return _f_result(29,0);

	ret=f_findnext(&find);
	if (ret) return _f_result(29,ret);
	if (STRCMP(find.filename,"..")) return _f_result(29,0);

	ret=f_findnext(&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(29,ret);


	ret=f_findfirst("*.a",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(30,ret);

/* creating testdir and find it */
	ret=f_mkdir("testdir");
	if (ret) return _f_result(31,ret);
	ret=f_findfirst("*.",&find);
	if (ret) return _f_result(32,ret);

	if (STRCMP(find.filename,".")) return _f_result(32,0);
	ret=f_findnext(&find);
	if (ret) return _f_result(32,ret);
	if (STRCMP(find.filename,"..")) return _f_result(32,0);

	ret=f_findnext(&find);
	if (ret) return _f_result(32,ret);
#ifdef HCC_UNICODE
	if (STRCMP(find.filename,"testdir")) return _f_result(33,0);
#else
	if (STRCMP(find.filename,f_nameconv("testdir"))) return _f_result(33,0);
#endif
	ret=f_findfirst("*.*",&find);
	if (ret) return _f_result(34,ret);
	if (STRCMP(find.filename,".")) return _f_result(35,0);
	ret=f_findnext(&find);
	if (ret) return _f_result(35,ret);
	if (STRCMP(find.filename,"..")) return _f_result(35,0);
	ret=f_findnext(&find);
	if (ret) return _f_result(36,ret);
#ifdef HCC_UNICODE
	if (STRCMP(find.filename,"testdir")) return _f_result(37,0);
#else
	if (STRCMP(find.filename,f_nameconv("testdir"))) return _f_result(37,0);
#endif
	ret=f_findnext(&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(38,ret);

/* search exact file */
	ret=f_findfirst("testDir",&find); /* no capitals! */
	if (ret) return _f_result(39,ret);
#ifdef HCC_UNICODE
	if (STRCMP(find.filename,"testdir")) return _f_result(40,0);
#else
	if (STRCMP(find.filename,f_nameconv("testdir"))) return _f_result(40,0);
#endif
	ret=f_findnext(&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(41,ret);


/* go back to root and remove dirs */
	ret=f_chdir("\\");
	if (ret) return _f_result(42,ret);
	ret=f_rmdir("Hello.dir/testdir");
	if (ret) return _f_result(43,ret);
	ret=f_rmdir("Hello.dir");
	if (ret) return _f_result(44,ret);

/* check if all are removed */
	ret=f_findfirst("*.*",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(45,ret);

	_f_dump("passed...");
	return 0;
}

int f_powerfail(void) {
int ret;

	_f_dump ("f_powerfail");

/* checking if its power fail system (RAMDRIVE is not powerfail!) */
	ret=f_mkdir("testdir");
	if (ret) return _f_result(0,ret);
	ret=_f_poweroff();
	if (ret) return _f_result(30,ret);
	ret=_f_poweron();
	if (ret) return _f_result(1,ret);
	ret=f_findfirst("testdir",&find);
	if (ret) return _f_result(2,ret);

/* checking formatting */
	ret=f_format(0,F_FAT_MEDIA);
	if (ret) return _f_result(3,ret);
	ret=_f_poweroff();
	if (ret) return _f_result(31,ret);
	ret=_f_poweron();
	if (ret) return _f_result(4,ret);
	ret=f_findfirst("*.*",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(5,ret);

/* checking formatting, 1st creating */
	ret=f_format(0,F_FAT_MEDIA);
	if (ret) return _f_result(6,ret);
	ret=f_mkdir("testdir");
	if (ret) return _f_result(7,ret);
	ret=f_findfirst("testdir",&find);
	if (ret) return _f_result(8,ret);
#ifdef HCC_UNICODE
	if (STRCMP(find.filename,"testdir")) return _f_result(9,0);
#else
	if (STRCMP(find.filename,f_nameconv("testdir"))) return _f_result(9,0);
#endif

	ret=_f_poweroff();
	if (ret) return _f_result(32,ret);
	ret=_f_poweron();
	if (ret) return _f_result(10,ret);
	ret=f_findfirst("*.*",&find);
	if (ret) return _f_result(11,ret);
#ifdef HCC_UNICODE
	if (STRCMP(find.filename,"testdir")) return _f_result(12,0);
#else
	if (STRCMP(find.filename,f_nameconv("testdir"))) return _f_result(12,0);
#endif

/* checking formatting, 2nd creating */
	ret=f_format(0,F_FAT_MEDIA);
	if (ret) return _f_result(13,ret);
	ret=f_mkdir("testdir");
	if (ret) return _f_result(14,ret);
	ret=f_findfirst("testdir",&find);
	if (ret) return _f_result(15,ret);
#ifdef HCC_UNICODE
	if (STRCMP(find.filename,"testdir")) return _f_result(16,0);
#else
	if (STRCMP(find.filename,f_nameconv("testdir"))) return _f_result(16,0);
#endif
	ret=f_mkdir("testdir2");
	if (ret) return _f_result(17,ret);
	ret=f_findfirst("testdir2",&find);
	if (ret) return _f_result(18,ret);
#ifdef HCC_UNICODE
	if (STRCMP(find.filename,"testdir2")) return _f_result(19,0);
#else
	if (STRCMP(find.filename,f_nameconv("testdir2"))) return _f_result(19,0);
#endif

	ret=_f_poweroff();
	if (ret) return _f_result(33,ret);
	ret=_f_poweron();
	if (ret) return _f_result(20,ret);
	ret=f_findfirst("*.*",&find);
	if (ret) return _f_result(21,ret);
#ifdef HCC_UNICODE
	if (STRCMP(find.filename,"testdir")) return _f_result(22,0);
#else
	if (STRCMP(find.filename,f_nameconv("testdir"))) return _f_result(22,0);
#endif
	ret=f_findnext(&find);
	if (ret) return _f_result(23,ret);
#ifdef HCC_UNICODE
	if (STRCMP(find.filename,"testdir2")) return _f_result(24,0);
#else
	if (STRCMP(find.filename,f_nameconv("testdir2"))) return _f_result(24,0);
#endif
	ret=f_findnext(&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(25,ret);


/* checking empty */
	ret=_f_poweroff();
	if (ret) return _f_result(34,ret);
	ret=_f_poweron();
	if (ret) return _f_result(26,ret);
	ret=f_format(0,F_FAT_MEDIA);
	if (ret) return _f_result(27,ret);
	ret=_f_poweroff();
	if (ret) return _f_result(35,ret);
	ret=_f_poweron();
	if (ret) return _f_result(28,ret);
	ret=f_findfirst("*.*",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(29,ret);


	_f_dump("passed...");
	return 0;
}


char buffer[32768];

int	checkfilecontent(long nums,unsigned char value,F_FILE *file) {
unsigned char ch;
	while (nums--) {

		if (f_eof(file)) return 1; /* eof ? */

		if (1!=f_read(&ch,1,1,file)) return 1;
		if (ch!=value) 
		{
		  return 1;
		}
	}
	return 0;
}

int f_seeking(int sectorsize) {
F_FILE *file;
int ret;
long size;
long pos;

	if (sectorsize == 128) _f_dump("f_seeking with 128");
	else if (sectorsize == 256) _f_dump("f_seeking with 256");
	else if (sectorsize == 512) _f_dump("f_seeking with 512");
	else if (sectorsize == 1024) _f_dump("f_seeking with 1024");
	else if (sectorsize == 2048) _f_dump("f_seeking with 2048");
	else if (sectorsize == 4096) _f_dump("f_seeking with 4096");
	else if (sectorsize == 8192) _f_dump("f_seeking with 8192");
	else if (sectorsize == 16384) _f_dump("f_seeking with 16384");
	else if (sectorsize == 32768) _f_dump("f_seeking with 32768");
	else _f_dump("f_seeking with random");

/* checking sector boundary seekeng */
	file=f_open("test.bin","w+");
	if (!file) return _f_result(0,0);

/* write sectorsize times 0 */
	(void)_memset(buffer,0,sectorsize);
	size=f_write(buffer,1,sectorsize,file);
	if (size!=sectorsize) return _f_result(1,size);
	pos=f_tell(file);
	if (pos!=sectorsize) return _f_result(2,pos);

/* seek back and read some */
	ret=f_seek(file,0,F_SEEK_SET);   /* seek back */
	if (ret) return _f_result(3,ret);
	pos=f_tell(file);
	if (pos) return _f_result(4,pos);
	size=f_read(buffer,1,sectorsize,file);
	if (size!=sectorsize) return _f_result(5,size);
	pos=f_tell(file);
	if (pos!=sectorsize) return _f_result(6,pos);

/* fake read at eof */
	size=f_read(buffer,1,2,file); /* eof! */
	if (size!=0) return _f_result(7,size);
	pos=f_tell(file);
	if (pos!=sectorsize) return _f_result(8,pos);

/* seek beyonf eof */
	ret=f_seek(file,sectorsize+8,F_SEEK_SET); /* beyond eof! */
	if (ret) return _f_result(9,ret);
	pos=f_tell(file);
	if (pos!=sectorsize+8) return _f_result(10,pos);

/* seek back to eof*/
	ret=f_seek(file,sectorsize,F_SEEK_SET); 
	if (ret) return _f_result(9,ret);
	pos=f_tell(file);
	if (pos!=sectorsize) return _f_result(10,pos);


/* writing sectorsize times 1 at the end */
	(void)_memset(buffer,1,sectorsize);
	size=f_write(buffer,1,sectorsize,file);
	if (size!=sectorsize) return _f_result(11,size);
	pos=f_tell(file);
	if (pos!=sectorsize*2) return _f_result(12,pos);

/* seeking back and read 1byte less */
	ret=f_seek(file,0,F_SEEK_SET);
	if (ret) return _f_result(13,ret);
	pos=f_tell(file);
	if (pos) return _f_result(14,pos);
	size=f_read(buffer,1,sectorsize-1,file);
	if (size!=sectorsize-1) return _f_result(15,size);
	pos=f_tell(file);
	if (pos!=sectorsize-1) return _f_result(16,pos);


/* write 2 times 2 */
	(void)_memset(buffer,2,sectorsize);
	size=f_write(buffer,1,2,file);
	if (size!=2) return _f_result(17,size);
	pos=f_tell(file);
	if (pos!=sectorsize+1) return _f_result(18,pos);

/* read 2 bytes */
	size=f_read(buffer,1,2,file);
	if (size!=2) return _f_result(19,size);
	pos=f_tell(file);
	if (pos!=sectorsize+3) return _f_result(20,pos);


/* write 4 times 3 */
	(void)_memset(buffer,3,sectorsize);
	size=f_write(buffer,1,4,file);
	if (size!=4) return _f_result(21,size);
	pos=f_tell(file);
	if (pos!=sectorsize+3+4) return _f_result(22,pos);

/* seek at 2 */
	ret=f_seek(file,2,F_SEEK_SET); /* here *f->sector */
	if (ret) return _f_result(23,ret);
	pos=f_tell(file);
	if (pos!=2) return _f_result(24,pos);

/* write 6 times 4 */
	(void)_memset(buffer,4,sectorsize);
	size=f_write(buffer,1,6,file);
	if (size!=6) return _f_result(25,size);
	pos=f_tell(file);
	if (pos!=8) return _f_result(26,pos);

/* seek end -4 */
	ret=f_seek(file,-4,F_SEEK_END);
	if (ret) return _f_result(27,ret);
	pos=f_tell(file);
	if (pos!=2*sectorsize-4) return _f_result(28,pos);

/* read 2 bytes */
	size=f_read(buffer,1,2,file);
	if (size!=2) return _f_result(29,size);
	pos=f_tell(file);
	if (pos!=2*sectorsize-2) return _f_result(30,pos);

/* write 8 times 5 */
	(void)_memset(buffer,5,sectorsize);
	size=f_write(buffer,1,8,file);
	if (size!=8) return _f_result(31,size);
	pos=f_tell(file);
	if (pos!=2*sectorsize+6) return _f_result(32,pos);

/* seek to the begining */
	ret=f_seek(file,0,F_SEEK_SET);
	if (ret) return _f_result(33,ret);
	pos=f_tell(file);
	if (pos) return _f_result(34,pos);

/* seek to the end */
	ret=f_seek(file,2*sectorsize+6,F_SEEK_SET);
	if (ret) return _f_result(35,ret);



	
	pos=f_tell(file);
	if (pos!=2*sectorsize+6) return _f_result(36,pos);

/* write 2 times 6 */
	(void)_memset(buffer,6,sectorsize);
	size=f_write(buffer,1,2,file);
	if (size!=2) return _f_result(37,size);
	pos=f_tell(file);
	if (pos!=2*sectorsize+8) return _f_result(38,pos);

/* seek to the begining */
	(void)f_seek(file,-(2*sectorsize+8),F_SEEK_CUR);
	if (ret) return _f_result(39,ret);
	pos=f_tell(file);
	if (pos) return _f_result(40,pos);

/* read 2 times sector */
	size=f_read(buffer,1,sectorsize,file);
	if (size!=sectorsize) return _f_result(41,size);
	pos=f_tell(file);
	if (pos!=sectorsize) return _f_result(42,pos);

	size=f_read(buffer,1,sectorsize,file);
	if (size!=sectorsize) return _f_result(43,size);
	pos=f_tell(file);
	if (pos!=2*sectorsize) return _f_result(44,pos);

/* write 1 once 7 */
	(void)_memset(buffer,7,sectorsize);
	size=f_write(buffer,1,1,file);
	if (size!=1) return _f_result(45,size);
	pos=f_tell(file);
	if (pos!=2*sectorsize+1) return _f_result(46,pos);

/* close it */
	ret=f_close(file);
	if (ret) return _f_result(47,ret);


/* check the result */
	size=f_filelength("test.bin");
	if (size!=2*sectorsize+8) return _f_result(48,size);

/* opens it */
	file=f_open("test.bin","r");
	if (!file) return _f_result(49,size);

	if (checkfilecontent(2,0,file)) return _f_result(50,0);
	if (checkfilecontent(6,4,file)) return _f_result(51,0);
	if (checkfilecontent(sectorsize-8-1,0,file)) return _f_result(52,0);
	if (checkfilecontent(2,2,file)) return _f_result(53,0);
	if (checkfilecontent(2,1,file)) return _f_result(54,0);
	if (checkfilecontent(4,3,file)) return _f_result(55,0);
	if (checkfilecontent(sectorsize-7-2,1,file)) return _f_result(56,0);
	if (checkfilecontent(2,5,file)) return _f_result(57,0);
	if (checkfilecontent(1,7,file)) return _f_result(58,0);
	if (checkfilecontent(5,5,file)) return _f_result(59,0);
	if (checkfilecontent(2,6,file)) return _f_result(60,0);

/* check pos result */
	pos=f_tell(file);
	if (pos!=2*sectorsize+8) return _f_result(61,pos);

/* this has to be eof */
	pos=f_eof(file);
	if (!pos) return _f_result(62,pos);

/* close it */
	ret=f_close(file);
	if (ret) return _f_result(63,ret);

/* deletes it */
	ret=f_delete("test.bin");
	if (ret) return _f_result(64,ret);

	_f_dump("passed...");
	return 0;
}

long f_opening(void) {
F_FILE *file;
F_FILE *file2;
long ret,size,pos;

	_f_dump("f_opening");

/* test non existing file open r, r+ */
	file=f_open("file.bin","r"); 
	if (file) return _f_result(0,0);
	file=f_open("file.bin","r+"); 
	if (file) return _f_result(1,0);

/* test non existing appends	"a" a+ */
	file=f_open("file.bin","a"); 
	if (!file) return _f_result(2,0);
	file2=f_open("file.bin","a+");  /* open again */
	if (file2) return _f_result(3,0);
	ret=f_close(file);
	if (ret) return _f_result(3,1);
	ret=f_close(file2);
	if (ret!=F_ERR_NOTOPEN) return _f_result(3,2);


/* try to creates it w */
	file=f_open("file.bin","w"); 
	if (!file) return _f_result(4,0);

/* write 512 times 1 */
	(void)_memset(buffer,1,512); /* set all 1 */
	size=f_write(buffer,1,512,file); /* test write */
	if (size!=512) return _f_result(5,size);

/* go back, and read it */
	ret=f_rewind(file); /* back to the begining */
	if (ret) return _f_result(6,ret); /* it should fail */
	size=f_read(buffer,1,512,file); /* test read */
	if (size) return _f_result(7,size); /* it should fail */

/* close and check size */
	size=f_filelength("file.bin");
	if (size) return _f_result(8,size); /* has to be zero */
	ret=f_close(file);
	if (ret) return _f_result(9,ret);
	size=f_filelength("file.bin");
	if (size!=512) return _f_result(10,size); 

/* try to owerwrites it it */
	file=f_open("file.bin","w+"); 
	if (!file) return _f_result(11,0);

/* close and check size */
	size=f_filelength("file.bin");
	if (size) return _f_result(12,size); /* has to be zero */
	ret=f_close(file);
	if (ret) return _f_result(13,ret);
	size=f_filelength("file.bin");
	if (size) return _f_result(14,size); 



/* test non existing appends	"a"  */
	file=f_open("file.bin","a"); 
	if (!file) return _f_result(15,0);

/* write 512 times 1 */
	(void)_memset(buffer,1,512); /* set all 1 */
	size=f_write(buffer,1,512,file); /* test write */
	if (size!=512) return _f_result(16,size);

/* go back, and read it */
	ret=f_rewind(file); /* back to the begining */
	size=f_read(buffer,1,512,file); /* test read */
	if (size) return _f_result(17,size); /* it should fail */

/* write 256 times 2 */
	(void)_memset(buffer,2,512); /* set all 2 */
	size=f_write(buffer,1,256,file); /* test write */
	if (size!=256) return _f_result(18,size);
	pos=f_tell(file);
	if (pos!=256) return _f_result(19,pos); /* position has to be 512 */
	size=f_filelength("file.bin");
	if (size) return _f_result(20,size); /* has to be zero */
/* close and check size */
	ret=f_close(file);
	if (ret) return _f_result(21,ret);
	size=f_filelength("file.bin");
	if (size!=512) return _f_result(22,size); 


/* test non existing appends a+ */
	file=f_open("file.bin","a+"); 
	if (!file) return _f_result(23,0);
	pos=f_tell(file);
	if (pos!=512) return _f_result(24,pos); /* position has to be 512 */

/* write 512 times 3 */
	(void)_memset(buffer,3,512); /* set all 3 */
	size=f_write(buffer,1,512,file); /* test write */
	if (size!=512) return _f_result(25,size);

/* go back, and read it */
	ret=f_rewind(file); /* back to the begining */
	if (ret) return _f_result(26,ret); /* it should fail */
	size=f_read(buffer,1,512,file); /* test read */
	if (size!=512) return _f_result(27,size); /* it should fail */
	pos=f_tell(file);
	if (pos!=512) return _f_result(28,pos); /* position has to be 512 */

/* close and check size */
	size=f_filelength("file.bin");
	if (size!=512) return _f_result(29,size); /* has to be zero */
	ret=f_close(file);
	if (ret) return _f_result(30,ret);
	size=f_filelength("file.bin");
	if (size!=1024) return _f_result(31,size); 

/* close again! */
	ret=f_close(file); 
	if (ret!=F_ERR_NOTOPEN) return _f_result(32,pos);




	_f_dump("passed...");
	return 0;
}


long f_appending(void) {
F_FILE *file;
long size,a,tsize,pos,b;
int ret;

	_f_dump("f_appending");

	ret=f_format(0,F_FAT_MEDIA);
	if (ret) return _f_result(0,ret);

	for (tsize=0,a=0; a<32; a++) {
		file=f_open("ap.bin","a");
		if (!file) return _f_result(1,0);
	
		(void)_memset(buffer,a,sizeof(buffer));
		size=f_write(buffer,1,a+128,file);
		if (size!=a+128) return _f_result(2,size);

		size=f_filelength("ap.bin");
		if (size != tsize) return _f_result(3,size);

		tsize+=a+128;

		ret=f_close(file);
		if (ret) return _f_result(4,ret);

		size=f_filelength("ap.bin");
		if (size != tsize) return _f_result(5,size);
	}

	file=f_open("ap.bin","r");
	if (!file) return _f_result(6,0);

	for (tsize=0,a=0; a<32; a++) {
		if (checkfilecontent(a+128,(char)a,file)) return _f_result(7,a);
	}

	ret=f_close(file);
	if (ret) return _f_result(8,ret);

	for (tsize=0,a=0; a<32; a++) {
		file=f_open("ap.bin","r");
		if (!file) return _f_result(9,0);

		ret=f_seek(file,tsize,F_SEEK_SET);
		if (ret) return _f_result(10,ret);

		pos=f_tell(file);
		if (pos!=tsize) return _f_result(11,pos);

		size=f_read(buffer,1,a+128,file);
		if (size!=a+128) return _f_result(12,size);

		for (b=0; b<a+128; b++) {
			if (buffer[b]!=(char)a) return _f_result(13,a);
		}

		tsize+=a+128;

		pos=f_tell(file);
		if (pos!=tsize) return _f_result(13,pos);

		ret=f_close(file);
		if (ret) return _f_result(14,ret);
	}

	ret=f_close(file);
	if (ret!=F_ERR_NOTOPEN) return _f_result(9,ret);


	ret=f_delete("ap.bin");
	if (ret) return _f_result(14,ret);


	_f_dump("passed...");
	return 0;
}


long f_writing(void) {
F_FILE *file;
long size,i;
int ret;
F_SPACE before,after;
	_f_dump("f_writing");

	ret=f_getfreespace(0,&before);
	if (ret) return _f_result(0,ret);

	for (i=0; i<16; i++) {
		file=f_open("wr.bin","w");
		if (!file) return _f_result(1,0);
	
		(void)_memset(buffer,i,sizeof(buffer));
		size=f_write(buffer,1,i*128,file);
		if (size!=i*128) return _f_result(2,size);

		ret=f_close(file);
		if (ret) return _f_result(3,ret);

		size=f_filelength("wr.bin");
		if (size != i*128) return _f_result(4,size);

		file=f_open("wr.bin","r");
		if (!file) return _f_result(5,0);

		if (checkfilecontent(i*128,(char)i,file)) return _f_result(6,i);

		ret=f_close(file);
		if (ret) return _f_result(7,ret);
	}


	for (i=0; i<16; i++) {
		file=f_open("wr.bin","w+");
		if (!file) return _f_result(8,0);
	
		(void)_memset(buffer,i,sizeof(buffer));
		size=f_write(buffer,1,i*128,file);
		if (size!=i*128) return _f_result(9,size);

		ret=f_close(file);
		if (ret) return _f_result(10,ret);

		size=f_filelength("wr.bin");
		if (size != i*128) return _f_result(11,size);

		file=f_open("wr.bin","r+");
		if (!file) return _f_result(12,0);

		if (checkfilecontent(i*128,(char)i,file)) return _f_result(13,i);

		ret=f_close(file);
		if (ret) return _f_result(14,ret);
	}

	ret=f_getfreespace(0,&after);
	if (ret) return _f_result(15,ret);

	if (before.bad!=after.bad) return _f_result(16,0);
	if (before.free==after.free) return _f_result(17,0);
	if (before.used==after.used) return _f_result(18,0);
	if (before.total!=after.total) return _f_result(19,0);

	if (before.used+before.free!=after.used+after.free) return _f_result(20,0);

	ret=f_delete("wr.bin");
	if (ret) return _f_result(21,ret);

	ret=f_getfreespace(0,&after);
	if (ret) return _f_result(22,ret);

	if (before.bad!=after.bad) return _f_result(23,0);
	if (before.free!=after.free) return _f_result(24,0);
	if (before.used!=after.used) return _f_result(25,0);
	if (before.total!=after.total) return _f_result(26,0);



	ret=f_format(0,F_FAT_MEDIA);
	if (ret) return _f_result(27,ret);

	ret=f_getfreespace(0,&before);
	if (ret) return _f_result(28,ret);


        {
          unsigned char a[2048];
          unsigned char b[16];
          unsigned long *la=(unsigned long *)a;
      
          for (i=0;i<2048/4;i++) *(la+i)=i;
          
          file=f_open("wr.bin","w");
          if (file) { (void)f_write(a,1,2048,file); (void)f_close(file); }
               else return _f_result(29,0);
      
          file=f_open("wr.bin","r+");
          if (file)
          {
            for (i=0;i<2048;a[i++]=0);
            (void)f_read(a,1,2048,file);
            for (i=0;i<2048/4 && *(la+i)==(unsigned long)i;i++);
            if (i==2048/4)
            {
              (void)f_seek(file,2032,F_SEEK_SET);
              for (i=0;i<16;i++) b[i]=(unsigned char)(0x10+i);
              (void)f_write(b,1,16,file);
              (void)f_close(file);
      
              file=f_open("wr.bin","r");
              if (file)
              {
                for (i=0;i<2048;a[i++]=0);
                (void)f_read(a,1,2048,file);
                for (i=0;i<(2048-16)/4 && *(la+i)==(unsigned long)i;i++);
                if (i==(2048-16)/4)
                {
                  for (i=0;i<16 && a[i+2032]==0x10+i;i++);
                  if (i!=16) return _f_result(30,0);
                }
                else return _f_result(31,0);
              }
              else return _f_result(32,0);
            }
            else return _f_result(33,0);

            (void)f_close(file);
          }
          else return _f_result(34,0);
        }

	ret=f_format(0,F_FAT_MEDIA);
	if (ret) return _f_result(35,ret);

	if (before.bad!=after.bad) return _f_result(36,0);
	if (before.free!=after.free) return _f_result(37,0);
	if (before.used!=after.used) return _f_result(38,0);
	if (before.total!=after.total) return _f_result(39,0);


	_f_dump("passed...");
	return 0;
}

long f_dots(void) {
int ret;
long a,size;
F_FILE *file;
	_f_dump("f_dots");

	ret=f_mkdir("/tt");
	if (ret) return _f_result(0,ret);
	
	ret=f_chdir("/tt");
	if (ret) return _f_result(1,ret);

	ret=f_rename(".","test");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(2,ret);

	ret=f_rename("..","test");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(3,ret);

	ret=f_rmdir(".");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(4,ret);

	ret=f_rmdir("..");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(5,ret);

	ret=f_chdir(".");
	if (ret) return _f_result(6,ret);

	ret=_f_checkcwd(f_nameconv("/tt"));
	if (ret) return _f_result(7,ret);

	ret=f_delete(".");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(8,ret);

	ret=f_delete("..");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(9,ret);

	ret=f_mkdir(".");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(10,ret);

	ret=f_mkdir("..");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(11,ret);

	ret=f_mkdir("...");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(12,ret);

	for (a=0; a<6; a++) {
		char *mode;
		switch (a) {
		case 0: mode="r"; break;
		case 1: mode="r+"; break;
		case 2: mode="w"; break;
		case 3: mode="w+"; break;
		case 4: mode="a"; break;
		case 5: mode="a+"; break;
		default: return _f_result(13,a);
		}

		file=f_open(".",mode);
		if (file) return _f_result(14,a);
		file=f_open("..",mode);
		if (file) return _f_result(15,a);
		file=f_open("...",mode);
		if (file) return _f_result(16,a);
	}

	size=f_filelength(".");
	if (size) return _f_result(17,size);
	size=f_filelength("..");
	if (size) return _f_result(18,size);
	size=f_filelength("...");
	if (size) return _f_result(19,size);


	ret=f_chdir("...");
	if (ret!=F_ERR_NOTFOUND) return _f_result(20,ret);

	ret=f_chdir("..");
	if (ret) return _f_result(21,ret);


	ret=f_rename("tt",".");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(22,ret);

	ret=f_rename("tt","..");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(23,ret);

	ret=f_rename("tt","...");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(24,ret);

	ret=f_rename("tt","  ");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(25,ret);

	ret=f_rename("  ","");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(26,ret);


	ret=f_rmdir("tt");
	if (ret) return _f_result(27,ret);


	_f_dump("passed...");
	return 0;
}

typedef struct 
{ 
        unsigned long     MagicNum; 
        int     Line; 
        char    Buf[87]; 
}struct_TestFileSysEntry; 

#define NUM_OF_RECORDS  100 

long f_rit(void) {
        
	unsigned long i;
	int ret; 
    F_FILE* File; 
    struct_TestFileSysEntry Entry; 
    int  Pos; 
    char Ch; 
    long Founded; 
    
    _f_dump("f_rit"); 

    File = f_open("MyTest","a+"); 
    if (!File)  {
		return _f_result(1,0);
    } 

    /*  add records   */
    for(i=0 ; i<NUM_OF_RECORDS ; i++) 
    { 
            Ch = (char)(i % 10); 
            Entry.MagicNum = 0xbcbcbcbc; 
            Entry.Line = i; 
            Entry.Buf[0] = Ch; 
            Entry.Buf[10] = (char)(Ch+1); 

            /* 

            if(f_NOERR != f_seek(File,0,f_SEEK_SET)) 
            { 
                    
                    mnuhdl_PrintData("Fail, could not go to the start of the file"); 
                    break; 
            } 

            */ 

            if(F_NO_ERROR != f_seek(File,0,F_SEEK_END)) 
            { 
                    /*  error   */
					return _f_result(2,i); /*  Fail, could not go to the end of the file  */
            } 

            if(sizeof(struct_TestFileSysEntry) != f_write((void*)&Entry,1,sizeof(struct_TestFileSysEntry),File)) 
            { 
                    /*  error   */
					return _f_result(3,i); /*  Fail, could not write new entry  */
            } 

            Pos = f_tell(File); 
            if((((unsigned long)Pos / sizeof(struct_TestFileSysEntry))-1) != i) 
            { 
                    /*  error   */
					return _f_result(4,i); /*  Fail, wrong file position  */
            } 
            if(F_NO_ERROR != f_seek(File,Pos - sizeof(struct_TestFileSysEntry),F_SEEK_SET)) 
            { 
                    /*  error   */
					return _f_result(5,i); /*  Fail, could not go to new entry position  */
            } 

            if(sizeof(struct_TestFileSysEntry) != f_read((void*)&Entry,1,sizeof(struct_TestFileSysEntry),File)) 
            { 
					/*  error   */
					return _f_result(6,i); /*  Fail, could not read the new entry  */
            } 

            if((Entry.MagicNum != 0xbcbcbcbc) || 
               (Entry.Line != (int)i) || 
               (Entry.Buf[0] != Ch) || 
               (Entry.Buf[10] != Ch+1))  {
				   	return _f_result(7,i); /* Fail, the new entry is corrupted" */
            } 
    } 

    ret=f_close(File); 
	if (ret) return _f_result(8,ret);


    /* -----------------  */

    /* Open file again */
    File = f_open("MyTest","a+"); 
    if (!File) return _f_result(9,0);
    
    /*  read records   */
    for(i=0 ; i<NUM_OF_RECORDS ; i++) 
    { 
            Ch = (char)(i % 10); 
    
            if(F_NO_ERROR != f_seek(File,0,F_SEEK_SET)) 
            { 
                    /*  error   */
					return _f_result(10,0); /*  Fail, could not go to the start of the file  */
            } 
    
            Founded = 0; 
            while(sizeof(struct_TestFileSysEntry) == f_read((void*)&Entry,1,sizeof(struct_TestFileSysEntry),File)) 
            { 
                    if((Entry.MagicNum == 0xbcbcbcbc) && 
                       (Entry.Line == (int)i) && 
                       (Entry.Buf[0] == Ch) && 
                       (Entry.Buf[10] == Ch+1)) 
                    { 
                            Founded = 1; 
                            break; 
                    } 
            } 
            if(!Founded) return _f_result(11,0); /*  Entry not founded  */
    } 
    
    ret=f_close(File); 
	if (ret) return _f_result(12,ret);


    ret=f_delete("MyTest"); 
	if (ret) return _f_result(13,ret);

	_f_dump("passed...");

    return 0; 
} 

long f_truncating(void) {
F_FILE *file;
unsigned long size;
int ret;
    _f_dump("f_truncating"); 


	file=f_open("test.bin","w+");
	if (!file) return _f_result(0,0);

	(void)_memset(buffer,1,16384);
	size=f_write(buffer,1,16384,file);
	if (size!=16384) return _f_result(1,size);

	ret=f_close(file);
	if (ret) return _f_result(2,ret);

	file=f_truncate("test.bin",16380);
	if (!file) return _f_result(3,0);
	ret=f_close(file);
	if (ret) return _f_result(4,ret);
	size=f_filelength("test.bin");
	if (size!=16380) return _f_result(5,size);


	file=f_truncate("test.bin",16384);
	if (!file) return _f_result(3,0);
	ret=f_close(file);
	if (ret) return _f_result(4,ret);
	size=f_filelength("test.bin");
	if (size!=16384) return _f_result(5,size);


	file=f_truncate("test.bin",8100);
	if (!file) return _f_result(6,0);
	(void)_memset(buffer,2,92);
	size=f_write(buffer,1,92,file);
	if (size!=92) return _f_result(7,size);

	ret=f_close(file);
	if (ret) return _f_result(8,ret);
	size=f_filelength("test.bin");
	if (size!=8192) return _f_result(9,size);


	file=f_truncate("test.bin",1);
	if (!file) return _f_result(10,0);
	(void)_memset(buffer,3,2); 
	size=f_write(buffer,1,2,file);
	if (size!=2) return _f_result(11,size);

	ret=f_close(file);
	if (ret) return _f_result(12,ret);
	size=f_filelength("test.bin");
	if (size!=3) return _f_result(13,size);



	_f_dump("passed...");
	return 0;
}

long f_chk(void) {
int ret;
    _f_dump("f_chk"); 

	ret=f_checkdisk(0,CHKDSK_ERASE_BAD_CHAIN|CHKDSK_ERASE_LOST_CHAIN|CHKDSK_ERASE_LOST_BAD_CHAIN);
	if (ret) return _f_result(1,ret);

	_f_dump("passed...");
	return 0;
}


long f_seekpos(void) {
int ret;
long a,b,size;
F_FILE *file;
    _f_dump("f_seekpos"); 

  for (b=0; b<5; b++) {
	  if (!b) {
		file=f_open("seek.bin","w+");
		if (!file) return _f_result(0,0);

		for (a=0; a<256; a++) {
	   		(void)_memset(buffer,(unsigned char)a,1024);
	   		size=f_write(buffer,1,1024,file);
	   		if (size!=1024) return _f_result(1,size);
		}
	  }
	  else if (b==1) {
		file=f_open("seek.bin","r");
		if (!file) return _f_result(100,0);

	  }
	  else if (b==2) {
		file=f_open("seek.bin","r");
		if (!file) return _f_result(100,0);

	    for (a=0; a<256; a++) {
	   	 if (checkfilecontent(1024,(unsigned char)a,file)) return _f_result(15,0);
		}
	  }
	  else if (b==3) {
		file=f_open("seek.bin","a+");
		if (!file) return _f_result(100,0);

	  }
	  else if (b==4) {
		file=f_open("seek.bin","r");
		if (!file) return _f_result(100,0);

		ret=f_seek(file,256*1024,F_SEEK_SET);   /* seek back */
		if (ret) return _f_result(29,ret);
	  }


	ret=f_seek(file,128*1024,F_SEEK_SET);   /* seek back */
	if (ret) return _f_result(2,ret);
	if (checkfilecontent(1024,128,file)) return _f_result(3,0);

	ret=f_seek(file,64*1024,F_SEEK_SET);   /* seek back */
	if (ret) return _f_result(4,ret);
	if (checkfilecontent(1024,64,file)) return _f_result(5,0);

	ret=f_seek(file,192*1024,F_SEEK_SET);   /* seek back */
	if (ret) return _f_result(6,ret);
	if (checkfilecontent(1024,192,file)) return _f_result(7,0);


	ret=f_seek(file,32*1024,F_SEEK_SET);   /* seek back */
	if (ret) return _f_result(8,ret);
	if (checkfilecontent(1024,32,file)) return _f_result(9,0);


	ret=f_seek(file,224*1024,F_SEEK_SET);   /* seek back */
	if (ret) return _f_result(10,ret);
	if (checkfilecontent(1024,224,file)) return _f_result(11,0);

	{
		long c=F_MAXSEEKPOS;
		if (!c) c=8;
	   for (a=0; a<256; a+=c) {
	   	ret=f_seek(file,a*1024,F_SEEK_SET);   /* seek back */
	   	if (ret) return _f_result(12,ret);
	   	if (checkfilecontent(1024,(unsigned char)a,file)) return _f_result(13,0);
	   }
	}


   for (a=0; a<256; a++) {
   	ret=f_seek(file,a*1024,F_SEEK_SET);   /* seek back */
   	if (ret) return _f_result(14,ret);
   	if (checkfilecontent(1024,(unsigned char)a,file)) return _f_result(15,0);
   }

	ret=f_close(file);
	if (ret) return _f_result(16,ret);
  }

/* truncate test */
	file=f_truncate("seek.bin",251*1024);
	if (!file) return _f_result(17,0);

   for (a=0; a<251; a++) {
   	ret=f_seek(file,a*1024,F_SEEK_SET);   /* seek back */
   	if (ret) return _f_result(18,ret);
   	if (checkfilecontent(1024,(unsigned char)a,file)) return _f_result(19,0);
   }

	ret=f_close(file);
	if (ret) return _f_result(20,ret);



	file=f_truncate("seek.bin",128*1024);
	if (!file) return _f_result(21,0);

   for (a=0; a<128; a++) {
   	ret=f_seek(file,a*1024,F_SEEK_SET);   /* seek back */
   	if (ret) return _f_result(22,ret);
   	if (checkfilecontent(1024,(unsigned char)a,file)) return _f_result(23,0);
   }


  	ret=f_seek(file,126*1024,F_SEEK_SET);   /* seek back */
  	if (ret) return _f_result(24,ret);
   ret=f_seteof(file);
  	if (ret) return _f_result(25,ret);


   for (a=0; a<126; a++) {
   	ret=f_seek(file,a*1024,F_SEEK_SET);   /* seek back */
   	if (ret) return _f_result(26,ret);
   	if (checkfilecontent(1024,(unsigned char)a,file)) return _f_result(27,0);
   }

  	ret=f_seek(file,64*1024,F_SEEK_SET);   /* seek back */
  	if (ret) return _f_result(28,ret);
   ret=f_seteof(file);
  	if (ret) return _f_result(29,ret);


   for (a=0; a<64; a++) {
   	ret=f_seek(file,a*1024,F_SEEK_SET);   /* seek back */
   	if (ret) return _f_result(30,ret);
   	if (checkfilecontent(1024,(unsigned char)a,file)) return _f_result(31,0);
   }


	ret=f_close(file);
	if (ret) return _f_result(32,ret);



	ret=f_delete("seek.bin");
	if (ret) return _f_result(64,ret);

	_f_dump("passed...");
	return 0;
}

void f_dotest(void) {
	_f_dump("File system test started...");
	(void)_f_poweron();

	(void)f_formatting();
	(void)f_dirtest();
	(void)f_findingtest();
	(void)f_powerfail();
	(void)f_seeking(128);
	(void)f_seeking(256);
	(void)f_seeking(512);
	(void)f_seeking(1024);
	(void)f_seeking(2048);
	(void)f_seeking(4096);
	(void)f_seeking(8192);
    (void)f_seeking(16384);
	(void)f_seeking(32768);
	(void)f_opening();
	(void)f_appending();
	(void)f_writing();
	(void)f_dots();
	(void)f_rit();
	(void)f_truncating();
	(void)f_chk();
	(void)f_seekpos();

    (void)_f_poweroff();
	_f_dump("End of tests...");
}

/****************************************************************************
 *
 * end of test.c
 *
 ***************************************************************************/

#endif	/* _TEST_C_ */
