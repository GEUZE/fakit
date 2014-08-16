/****************************************************************************
 *
 *            Copyright (c) 2005-2008 by HCC Embedded
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

#include "test.h"
#include "string.h"

/* set RAMDRIVETEST to 1 if RAM drive is tested */
#define RAMDRIVETEST 0

#ifdef HCC_UNICODE
#define STRCMP wcscmp
#else
#define STRCMP strcmp
#define _fsm_towchar(x,y) y
#endif

static W_CHAR cwd[F_MAXPATH];
static F_FIND find;
static char buffer[32768];
static unsigned char chkbuffer[32768];

/****************************************************************************
*
* checkfilecontent
*
* checking content of a file
*
* INPUTS
*
* nums - how many char to be checked
* value - what is the proper value
* file - file pointer to a file
*
* RETURNS
*
* 0 - if match
* other if differs
*
***************************************************************************/

int	checkfilecontent(long nums,unsigned char value,F_FILE *file) 
{
#if 1  /* faster	read 32k blocks at once then comparing */

	while (nums) 
	{
		unsigned char *ptr=chkbuffer;
		long size=nums;

		if (size>(long)sizeof(chkbuffer)) size=sizeof(chkbuffer);

   		if (f_eof(file)) 
		{
			return 1; /* eof ? */
		}

   		if (size!=f_read(chkbuffer,1,size,file)) 
		{
			return 1;
		}

		nums-=size;

		while (size) 
		{
			if (*ptr++!=value) 
			{
				return 1; /* differs */
			}
			size--;
		}
	}
#else  /* slower byte by byte */
	unsigned char ch;

	while (nums--) 
	{
		if (f_eof(file)) return 1; /* eof ? */

		if (1!=f_read(&ch,1,1,file)) return 1;
		if (ch!=value) return 1;
	}
#endif

	return 0;
}

/****************************************************************************
*
* f_formatting
*
* Internal test function
*
* RETURNS
*
* error code
*
***************************************************************************/

int f_formatting(void) 
{
	int ret;

	_f_dump ("f_formatting");

/* checking formatting */
	ret=f_format(0);
	if (ret) return _f_result(__LINE__,ret);

	ret=_f_poweroff();
	if (ret) return _f_result(__LINE__,ret);
	ret=_f_poweron();
	if (ret) return _f_result(__LINE__,ret);

	ret=f_findfirst("*.*",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);

	_f_dump("passed...");
	return 0;
}

/****************************************************************************
 *
 * _f_checkcwd
 *
 * check current working directory
 *
 * INPUTS
 *
 * orig - original path to be compare
 *
 * RETURNS
 *
 * 0 - if match
 * other if differs
 *
 ***************************************************************************/

int _f_checkcwd(char *orig) 
{
#ifdef HCC_UNICODE
	W_CHAR nconv[F_MAXPATH];
#endif
	int ret;

#ifdef HCC_UNICODE
	ret=f_wgetcwd(cwd,F_MAXPATH);
#else
	ret=f_getcwd(cwd,F_MAXPATH);
#endif
	if (ret) return ret;

	if (STRCMP(_fsm_towchar(nconv,orig),cwd+1)) return -1; /* +1 because of slash in getcwd */

	return 0;
}

/****************************************************************************
*
* f_dirtest
*
* Internal test function
*
* RETURNS
*
* error code
*
***************************************************************************/

int f_dirtest(void) 
{
	int ret;

	_f_dump("f_dirtest");
/* creates a ab abc abcd */
	ret=f_mkdir("a");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_mkdir("ab");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_mkdir("abc");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_mkdir("abca");
	if (ret) return _f_result(__LINE__,ret);

/* creates directories in /a  -  a ab abc abcd */
	ret=f_mkdir("a/a");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_mkdir("a/ab");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_mkdir("a/abc");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_mkdir("a/abcd");
	if (ret) return _f_result(__LINE__,ret);

/* change into a/abcd and check cwd */
	ret=f_chdir("a/abcd");
	if (ret) return _f_result(__LINE__,ret);
#if F_SEPARATOR=='/'
	ret=_f_checkcwd("a/abcd");
#else
	ret=_f_checkcwd("a\\abcd");
#endif
	if (ret) return _f_result(__LINE__,ret);

/* make directory t change into t and check cwd="a/abcd/t" */
	ret=f_mkdir("t");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_chdir("t");
	if (ret) return _f_result(__LINE__,ret);
#if F_SEPARATOR=='/'
	ret=_f_checkcwd("a/abcd/t");
#else
	ret=_f_checkcwd("a\\abcd\\t");
#endif
	if (ret) return _f_result(__LINE__,ret);
	ret=f_chdir(".");
	if (ret) return _f_result(__LINE__,ret);
#if F_SEPARATOR=='/'
	ret=_f_checkcwd("a/abcd/t");
#else
	ret=_f_checkcwd("a\\abcd\\t");
#endif
	if (ret) return _f_result(__LINE__,ret);
	ret=f_chdir("../.");
	if (ret) return _f_result(__LINE__,ret);
#if F_SEPARATOR=='/'
	ret=_f_checkcwd("a/abcd");
#else
	ret=_f_checkcwd("a\\abcd");
#endif
	if (ret) return _f_result(__LINE__,ret);

/* removing t dir */
	ret=f_rmdir("t");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_chdir("t");
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);

/* removing /a dir */
	ret=f_rmdir("/ab");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_chdir("/ab");
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);

/* removing /a dir */
	ret=f_rmdir("../../a");
	if (ret!=F_ERR_NOTEMPTY) return _f_result(__LINE__,ret);

/* removing /abca dir */
	ret=f_rmdir("a:/abca");
	if (ret) return _f_result(__LINE__,ret);

/* changing invalid dirs */
	ret=f_chdir("");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);
	ret=f_chdir(" ");
	if (ret) return _f_result(__LINE__,ret);
#if F_SEPARATOR=='/'
	ret=_f_checkcwd("a/abcd");
#else
	ret=_f_checkcwd("a\\abcd");
#endif
	if (ret) return _f_result(__LINE__,ret);
	ret=f_chdir("?");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);
	ret=f_chdir("*.*");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);
#if F_SEPARATOR=='/'
	ret=_f_checkcwd("a/abcd");
#else
	ret=_f_checkcwd("a\\abcd");
#endif
	if (ret) return _f_result(__LINE__,ret);

/* changing into /abc and removes subfolder from /a/ */
	ret=f_chdir("/abc");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_rmdir("/a/a");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_rmdir("A:../a/ab");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_rmdir("A:/a/abc");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_rmdir(".././abc/.././a/../a/abcd");
	if (ret) return _f_result(__LINE__,ret);

/* some invalid rmdir */
	ret=f_rmdir(".");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);
	ret=f_rmdir("..");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);

/* create again abc remove abc */
	ret=f_mkdir(".././abc");
	if (ret!=F_ERR_DUPLICATED) return _f_result(__LINE__,ret);
	ret=f_rmdir("../abc");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_mkdir(".././abc");
	if (ret!=F_ERR_INVALIDDIR) return _f_result(__LINE__,ret); /* cwd is not exist */
	ret=f_chdir("/");
	if (ret) return _f_result(__LINE__,ret);

/* try . and .. in the root */
	ret=f_chdir(".");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_chdir("./././.");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_chdir("..");
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);
	ret=_f_checkcwd(""); /* root! */
	if (ret) return _f_result(__LINE__,ret);

/* test . and .. in a and remove a */
	ret=f_chdir("a");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_chdir("..");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_chdir("a");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_chdir(".");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_chdir("a");
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);
	ret=f_chdir("./..");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_rmdir("a");
	if (ret) return _f_result(__LINE__,ret);

/* test 13-12 long filename */
	ret=f_mkdir("123412341234a");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_mkdir("123412341234");
	if (ret) return _f_result(__LINE__,ret);

	ret=f_rmdir("123412341234a");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_rmdir("123412341234");
	if (ret) return _f_result(__LINE__,ret);



/* check if all are removed */
	ret=f_findfirst("*.*",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);

	_f_dump("passed...");
	return 0;
}

/****************************************************************************
*
* f_findingtest
*
* Internal test function
*
* RETURNS
*
* error code
*
***************************************************************************/

int f_findingtest(void) 
{
#ifdef HCC_UNICODE
	//W_CHAR nconv[F_MAXPATH];
#endif
	int ret;

	_f_dump("f_findingtest");

/* check empty */
	ret=f_findfirst("*",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);

/* create Hello.dir */
	ret=f_mkdir("Hello.dir");
	if (ret) return _f_result(__LINE__,ret);

/* check if it is exist, and only exist */
	ret=f_findfirst("*",&find);
	if (ret) return _f_result(__LINE__,ret);
	if (strcmp(find.filename,"Hello.dir")) return _f_result(__LINE__,0);
	if (find.attr!=F_ATTR_DIR) return _f_result(__LINE__,0);
	ret=f_findnext(&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);

/* check some not founds */
	ret=f_findfirst("q*.*",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);
	ret=f_findfirst("Hello.",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);
	ret=f_findfirst("a/*.*",&find);
	if (ret!=F_ERR_INVALIDDIR) return _f_result(__LINE__,ret);
	ret=f_findfirst(".",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);
	ret=f_findfirst("..",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);
	ret=f_findfirst("?e.*",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);
	ret=f_findfirst("*.",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);
	ret=f_findfirst("*.?",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);
	ret=f_findfirst("*.??",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);


/* check some founds */
	ret=f_findfirst("*.dir",&find);
	if (ret) return _f_result(__LINE__,ret);
	ret=f_findfirst("*.d?r",&find);
	if (ret) return _f_result(__LINE__,ret);
	ret=f_findfirst("*.d??",&find);
	if (ret) return _f_result(__LINE__,ret);
	ret=f_findfirst("*.???",&find);
	if (ret) return _f_result(__LINE__,ret);
	ret=f_findfirst("?ello.???",&find);
	if (ret) return _f_result(__LINE__,ret);
	ret=f_findfirst("he??o.dir",&find);
	if (ret) return _f_result(__LINE__,ret);
	ret=f_findfirst("he?*.dir",&find);
	if (ret) return _f_result(__LINE__,ret);
	ret=f_findfirst("HELLO.DIR",&find); /* no capitals sensitivity in find!! */
	if (ret) return _f_result(__LINE__,ret);

/* change into hello.dir */
	ret=f_chdir("hello.dir");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_findfirst("*.*",&find);
	if (ret) return _f_result(__LINE__,ret);
	ret=f_findfirst("..",&find);
	if (ret) return _f_result(__LINE__,ret);
	ret=f_findfirst("??",&find);
	if (ret) return _f_result(__LINE__,ret);
	ret=f_findfirst(".",&find);
	if (ret) return _f_result(__LINE__,ret);
	ret=f_findfirst("k*.*",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);
	ret=f_findfirst("*.",&find);
	if (ret) return _f_result(__LINE__,ret); /* .. */
	ret=f_findfirst("*.a",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);

/* creating testdir and find it */
	ret=f_mkdir("testdir");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_findfirst("*.",&find);
	if (ret) return _f_result(__LINE__,ret);
	if (strcmp(find.filename,".")) return _f_result(__LINE__,0);

	ret=f_findnext(&find);
	if (ret) return _f_result(__LINE__,ret);
	if (strcmp(find.filename,"..")) return _f_result(__LINE__,0);

	ret=f_findnext(&find);
	if (ret) return _f_result(__LINE__,ret);
	if (strcmp(find.filename,"testdir")) return _f_result(__LINE__,0);

	ret=f_findnext(&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);

	ret=f_findfirst("*.*",&find);
	if (ret) return _f_result(__LINE__,ret);
	if (strcmp(find.filename,".")) return _f_result(__LINE__,0);
	ret=f_findnext(&find);
	if (ret) return _f_result(__LINE__,ret);
	if (strcmp(find.filename,"..")) return _f_result(__LINE__,0);
	ret=f_findnext(&find);
	if (ret) return _f_result(__LINE__,ret);
	if (strcmp(find.filename,"testdir")) return _f_result(__LINE__,0);
	ret=f_findnext(&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);

/* search exact file */
	ret=f_findfirst("testDir",&find); /* no capitals! */
	if (ret) return _f_result(__LINE__,ret);
	if (strcmp(find.filename,"testdir")) return _f_result(__LINE__,0);
	ret=f_findnext(&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);


/* go back to root and remove dirs */
	ret=f_chdir("\\");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_rmdir("Hello.dir/testdir");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_rmdir("Hello.dir");
	if (ret) return _f_result(__LINE__,ret);

/* check if all are removed */
	ret=f_findfirst("*.*",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);

	_f_dump("passed...");
	return 0;
}

/****************************************************************************
*
* f_powerfail
*
* Internal test function
*
* RETURNS
*
* error code
*
***************************************************************************/

int f_powerfail(void) 
{
#ifdef HCC_UNICODE
//W_CHAR nconv[F_MAXPATH];
#endif
	int ret;
	int a;
	long size;
	F_FILE *file;

	_f_dump ("f_powerfail");

/* checking if its power fail system (RAMDRIVE is not powerfail!) */
	ret=f_mkdir("testdir");
	if (ret) return _f_result(__LINE__,ret);

	ret=_f_poweroff();
	if (ret) return _f_result(__LINE__,ret);
	ret=_f_poweron();
	if (ret) return _f_result(__LINE__,ret);
	ret=f_findfirst("testdir",&find);
	if (ret) return _f_result(__LINE__,ret);

/* checking formatting */
	ret=f_format(0);
	if (ret) return _f_result(__LINE__,ret);

	ret=_f_poweroff();
	if (ret) return _f_result(__LINE__,ret);
	ret=_f_poweron();
	if (ret) return _f_result(__LINE__,ret);
	ret=f_findfirst("*.*",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);

/* checking formatting, 1st creating */
	ret=f_format(0);
	if (ret) return _f_result(__LINE__,ret);
	ret=f_mkdir("testdir");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_findfirst("testdir",&find);
	if (ret) return _f_result(__LINE__,ret);
	if (strcmp(find.filename,"testdir")) return _f_result(__LINE__,0);

	ret=_f_poweroff();
	if (ret) return _f_result(__LINE__,ret);
	ret=_f_poweron();
	if (ret) return _f_result(__LINE__,ret);

	ret=f_findfirst("*",&find);
	if (ret) return _f_result(__LINE__,ret);
	if (strcmp(find.filename,"testdir")) return _f_result(__LINE__,0);

/* checking formatting, 2nd creating */
	ret=f_format(0);
	if (ret) return _f_result(__LINE__,ret);
	ret=f_mkdir("testdir");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_findfirst("testdir",&find);
	if (ret) return _f_result(__LINE__,ret);
	if (strcmp(find.filename,"testdir")) return _f_result(__LINE__,0);
	ret=f_mkdir("testdir2");
	if (ret) return _f_result(__LINE__,ret);
	ret=f_findfirst("testdir2",&find);
	if (ret) return _f_result(__LINE__,ret);
	if (strcmp(find.filename,"testdir2")) return _f_result(__LINE__,0);

	ret=_f_poweroff();
	if (ret) return _f_result(__LINE__,ret);
	ret=_f_poweron();
	if (ret) return _f_result(__LINE__,ret);

	ret=f_findfirst("*",&find);
	if (ret) return _f_result(__LINE__,ret);
	if (strcmp(find.filename,"testdir")) return _f_result(__LINE__,0);
	ret=f_findnext(&find);
	if (ret) return _f_result(__LINE__,ret);
	if (strcmp(find.filename,"testdir2")) return _f_result(__LINE__,0);
	ret=f_findnext(&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);


/* checking empty */
	ret=_f_poweroff();
	if (ret) return _f_result(__LINE__,ret);
	ret=_f_poweron();
	if (ret) return _f_result(__LINE__,ret);

	ret=f_format(0);
	if (ret) return _f_result(__LINE__,ret);

	ret=_f_poweroff();
	if (ret) return _f_result(__LINE__,ret);
	ret=_f_poweron();
	if (ret) return _f_result(__LINE__,ret);

	ret=f_findfirst("*",&find);
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);


/* 4 files test */

	for (a=0; a<4; a++) 
	{
		char name[2];
		name[0]=(char)(a+'1');
		name[1]=0;

		(void)memset(buffer,a+1,sizeof(buffer));

		file=f_open(name,"w+");
		if (!file) return _f_result(__LINE__,0);

   		size=f_write(buffer,1,sizeof(buffer),file); /* 32K !! */
   		if (size!=0x8000UL) return _f_result(__LINE__,size);

		ret=f_close(file);
		if (ret) return _f_result(__LINE__,ret);
	}

	ret=f_delete("2");
	if (ret) return _f_result(__LINE__,ret);

	ret=f_delete("3");
	if (ret) return _f_result(__LINE__,ret);

/* store 10 */
	(void)memset(buffer,10,sizeof(buffer));

	file=f_open("10","w+");
	if (!file) return _f_result(__LINE__,0);

   	size=f_write(buffer,1,sizeof(buffer),file); /* 32K !! */
   	if (size!=0x8000UL) return _f_result(__LINE__,size);
   	size=f_write(buffer,1,sizeof(buffer),file); /* 32K !! */
   	if (size!=0x8000UL) return _f_result(__LINE__,size);

   	size=f_write(buffer,1,sizeof(buffer),file); /* 32K !! */
   	if (size!=0x8000UL) return _f_result(__LINE__,size);

	ret=_f_poweroff();
	if (ret) return _f_result(__LINE__,ret);
	ret=_f_poweron();
	if (ret) return _f_result(__LINE__,ret);

/* check 1 */
	file=f_open("1","r");
	if (!file) return _f_result(__LINE__,0);

	if (checkfilecontent(sizeof(buffer),1,file)) return _f_result(__LINE__,a);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);

/* check 4 */
	file=f_open("4","r");
	if (!file) return _f_result(__LINE__,0);

	if (checkfilecontent(sizeof(buffer),4,file)) return _f_result(__LINE__,a);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);


	ret=f_delete("1");
	if (ret) return _f_result(__LINE__,ret);

	ret=f_delete("4");
	if (ret) return _f_result(__LINE__,ret);

	_f_dump("passed...");
	return 0;
}

/****************************************************************************
*
* f_seeking
*
* Internal test function
*
* RETURNS
*
* error code
*
***************************************************************************/

int f_seeking(long sectorsize) 
{
	F_STAT stat;
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
	if (!file) return _f_result(__LINE__,0);

/* write sectorsize times 0 */
	(void)memset(buffer,0,sectorsize);
	size=f_write(buffer,1,sectorsize,file);
	if (size!=sectorsize) return _f_result(__LINE__,size);
	pos=f_tell(file);
	if (pos!=sectorsize) return _f_result(__LINE__,pos);

/* seek back and read some */
	ret=f_seek(file,0,F_SEEK_SET);   /* seek back */
	if (ret) return _f_result(__LINE__,ret);
	pos=f_tell(file);
	if (pos) return _f_result(__LINE__,pos);
	size=f_read(buffer,1,sectorsize,file);
	if (size!=sectorsize) return _f_result(__LINE__,size);
	pos=f_tell(file);
	if (pos!=sectorsize) return _f_result(__LINE__,pos);

/* fake read at eof */
	size=f_read(buffer,1,2,file); /* eof! */
	if (size!=0) return _f_result(__LINE__,size);
	pos=f_tell(file);
	if (pos!=sectorsize) return _f_result(__LINE__,pos);

/* seek after eof */
	ret=f_seek(file,sectorsize+8,F_SEEK_SET); /* eof! */
	if (ret) return _f_result(__LINE__,ret);
	pos=f_tell(file);
	if (pos!=sectorsize+8) return _f_result(__LINE__,pos);


/* seek back */
	ret=f_seek(file,sectorsize,F_SEEK_SET);
	if (ret) return _f_result(__LINE__,ret);
	pos=f_tell(file);
	if (pos!=sectorsize) return _f_result(__LINE__,pos);


/* writing sectorsize times 1 at the end */
	(void)memset(buffer,1,sectorsize);
	size=f_write(buffer,1,sectorsize,file);
	if (size!=sectorsize) return _f_result(__LINE__,size);
	pos=f_tell(file);
	if (pos!=sectorsize*2) return _f_result(__LINE__,pos);

/* seeking back and read 1byte less */
	ret=f_seek(file,0,F_SEEK_SET);
	if (ret) return _f_result(__LINE__,ret);
	pos=f_tell(file);
	if (pos) return _f_result(__LINE__,pos);
	size=f_read(buffer,1,sectorsize-1,file);
	if (size!=sectorsize-1) return _f_result(__LINE__,size);
	pos=f_tell(file);
	if (pos!=sectorsize-1) return _f_result(__LINE__,pos);


/* write 2 times 2 */
	(void)memset(buffer,2,sectorsize);
	size=f_write(buffer,1,2,file);
	if (size!=2) return _f_result(__LINE__,size);
	pos=f_tell(file);
	if (pos!=sectorsize+1) return _f_result(__LINE__,pos);

/* read 2 bytes */
	size=f_read(buffer,1,2,file);
	if (size!=2) return _f_result(__LINE__,size);
	pos=f_tell(file);
	if (pos!=sectorsize+3) return _f_result(__LINE__,pos);


/* write 4 times 3 */
	(void)memset(buffer,3,sectorsize);
	size=f_write(buffer,1,4,file);
	if (size!=4) return _f_result(__LINE__,size);
	pos=f_tell(file);
	if (pos!=sectorsize+3+4) return _f_result(__LINE__,pos);

/* seek at 2 */
	ret=f_seek(file,2,F_SEEK_SET); /* here *f->sector */
	if (ret) return _f_result(__LINE__,ret);
	pos=f_tell(file);
	if (pos!=2) return _f_result(__LINE__,pos);

/* write 6 times 4 */
	(void)memset(buffer,4,sectorsize);
	size=f_write(buffer,1,6,file);
	if (size!=6) return _f_result(__LINE__,size);
	pos=f_tell(file);
	if (pos!=8) return _f_result(__LINE__,pos);

/* seek end -4 */
	ret=f_seek(file,-4,F_SEEK_END);
	if (ret) return _f_result(__LINE__,ret);
	pos=f_tell(file);
	if (pos!=2*sectorsize-4) return _f_result(__LINE__,pos);

/* read 2 bytes */
	size=f_read(buffer,1,2,file);
	if (size!=2) return _f_result(__LINE__,size);
	pos=f_tell(file);
	if (pos!=2*sectorsize-2) return _f_result(__LINE__,pos);

/* write 8 times 5 */
	(void)memset(buffer,5,sectorsize);
	size=f_write(buffer,1,8,file);
	if (size!=8) return _f_result(__LINE__,size);
	pos=f_tell(file);
	if (pos!=2*sectorsize+6) return _f_result(__LINE__,pos);

/* seek to the begining */
	ret=f_seek(file,0,F_SEEK_SET);
	if (ret) return _f_result(__LINE__,ret);
	pos=f_tell(file);
	if (pos) return _f_result(__LINE__,pos);

/* seek to the end */
	ret=f_seek(file,2*sectorsize+6,F_SEEK_SET);
	if (ret) return _f_result(__LINE__,ret);
	pos=f_tell(file);
	if (pos!=2*sectorsize+6) return _f_result(__LINE__,pos);

/* write 2 times 6 */
	(void)memset(buffer,6,sectorsize);
	size=f_write(buffer,1,2,file);
	if (size!=2) return _f_result(__LINE__,size);
	pos=f_tell(file);
	if (pos!=2*sectorsize+8) return _f_result(__LINE__,pos);

/* seek to the begining */
	ret=f_seek(file,-(2*sectorsize+8),F_SEEK_CUR);
	if (ret) return _f_result(__LINE__,ret);
	pos=f_tell(file);
	if (pos) return _f_result(__LINE__,pos);

/* read 2 times sector */
	size=f_read(buffer,1,sectorsize,file);
	if (size!=sectorsize) return _f_result(__LINE__,size);
	pos=f_tell(file);
	if (pos!=sectorsize) return _f_result(__LINE__,pos);

	size=f_read(buffer,1,sectorsize,file);
	if (size!=sectorsize) return _f_result(__LINE__,size);
	pos=f_tell(file);
	if (pos!=2*sectorsize) return _f_result(__LINE__,pos);

/* write 1 once 7 */
	(void)memset(buffer,7,sectorsize);
	size=f_write(buffer,1,1,file);
	if (size!=1) return _f_result(__LINE__,size);
	pos=f_tell(file);
	if (pos!=2*sectorsize+1) return _f_result(__LINE__,pos);

/* close it */
	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);


/* check the result */
	size=f_filelength("test.bin");
	if (size!=2*sectorsize+8) return _f_result(__LINE__,size);

	size=f_stat("test.bin",&stat);
	if (stat.filesize!=(unsigned long)(2*sectorsize+8)) return _f_result(__LINE__,size);

	ret=f_checkvolume(0);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_checkvolume(5);
	if (ret!=F_ERR_INVALIDDRIVE) return _f_result(__LINE__,ret);
/* opens it */
	file=f_open("test.bin","r");
	if (!file) return _f_result(__LINE__,size);

	if (checkfilecontent(2,0,file)) return _f_result(__LINE__,sectorsize);
	if (checkfilecontent(6,4,file)) return _f_result(__LINE__,sectorsize);
	if (checkfilecontent(sectorsize-8-1,0,file)) return _f_result(__LINE__,sectorsize);
	if (checkfilecontent(2,2,file)) return _f_result(__LINE__,sectorsize);
	if (checkfilecontent(2,1,file)) return _f_result(__LINE__,sectorsize);
	if (checkfilecontent(4,3,file)) return _f_result(__LINE__,sectorsize);
	if (checkfilecontent(sectorsize-7-2,1,file)) return _f_result(__LINE__,sectorsize);
	if (checkfilecontent(2,5,file)) return _f_result(__LINE__,sectorsize);
	if (checkfilecontent(1,7,file)) return _f_result(__LINE__,sectorsize);
	if (checkfilecontent(5,5,file)) return _f_result(__LINE__,sectorsize);
	if (checkfilecontent(2,6,file)) return _f_result(__LINE__,sectorsize);

/* check pos result */
	pos=f_tell(file);
	if (pos!=2*sectorsize+8) return _f_result(__LINE__,pos);

/* this has to be eof */
	pos=f_eof(file);
	if (pos!=F_ERR_EOF) return _f_result(__LINE__,pos);

/* close it */
	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);

/* deletes it */
	ret=f_delete("test.bin");
	if (ret) return _f_result(__LINE__,ret);

	_f_dump("passed...");
	return 0;
}

/****************************************************************************
*
* f_opening
*
* Internal test function
*
* RETURNS
*
* error code
*
***************************************************************************/

int f_opening(void) 
{
	F_FILE *file;
	F_FILE *file2;
	long ret,size,pos;

	_f_dump("f_opening");

/* test non existing file open r, r+ */
	file=f_open("file.bin","r");
	if (file) return _f_result(__LINE__,0);
	file=f_open("file.bin","r+");
	if (file) return _f_result(__LINE__,0);

/* test non existing appends	"a" a+ */
	file=f_open("file.bin","a");
	if (!file) return _f_result(__LINE__,0);
	file2=f_open("file.bin","a+");  /* open again */
	if (file2) return _f_result(__LINE__,0);
	ret=f_close(file);
	if (ret) return _f_result(__LINE__,1);
	ret=f_close(file2);
	if (ret!=F_ERR_NOTOPEN) return _f_result(__LINE__,2);


/* try to creates it w */
	file=f_open("file.bin","w");
	if (!file) return _f_result(__LINE__,0);

/* write 512 times 1 */
	(void)memset(buffer,1,512); /* set all 1 */
	size=f_write(buffer,1,512,file); /* test write */
	if (size!=512) return _f_result(__LINE__,size);

/* go back, and read it */
	ret=f_rewind(file); /* back to the begining */
	if (ret) return _f_result(__LINE__,ret); /* it should fail */
	size=f_read(buffer,1,512,file); /* test read */
	if (size) return _f_result(__LINE__,size); /* it should fail */

/* close and check size */
	size=f_filelength("file.bin");
	if (size) return _f_result(__LINE__,size); /* has to be zero */
	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);
	size=f_filelength("file.bin");
	if (size!=512) return _f_result(__LINE__,size);

/* try to owerwrites it it */
	file=f_open("file.bin","w+");
	if (!file) return _f_result(__LINE__,0);

/* close and check size */
	size=f_filelength("file.bin");
	if (size) return _f_result(__LINE__,size); /* has to be zero */
	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);
	size=f_filelength("file.bin");
	if (size) return _f_result(__LINE__,size);



/* test non existing appends	"a"  */
	file=f_open("file.bin","a");
	if (!file) return _f_result(__LINE__,0);

/* write 512 times 1 */
	(void)memset(buffer,1,512); /* set all 1 */
	size=f_write(buffer,1,512,file); /* test write */
	if (size!=512) return _f_result(__LINE__,size);

/* go back, and read it */
	ret=f_rewind(file); /* back to the begining */
	size=f_read(buffer,1,512,file); /* test read */
	if (size) return _f_result(__LINE__,size); /* it should fail */

/* write 256 times 2 */
	(void)memset(buffer,2,512); /* set all 2 */
	size=f_write(buffer,1,256,file); /* test write */
	if (size!=256) return _f_result(__LINE__,size);
	pos=f_tell(file);
	if (pos!=256) return _f_result(__LINE__,pos); /* position has to be 512 */
	size=f_filelength("file.bin");
	if (size) return _f_result(__LINE__,size); /* has to be zero */
/* close and check size */
	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);
	size=f_filelength("file.bin");
	if (size!=512) return _f_result(__LINE__,size);


/* test non existing appends a+ */
	file=f_open("file.bin","a+");
	if (!file) return _f_result(__LINE__,0);
	pos=f_tell(file);
	if (pos!=512) return _f_result(__LINE__,pos); /* position has to be 512 */

/* write 512 times 3 */
	(void)memset(buffer,3,512); /* set all 3 */
	size=f_write(buffer,1,512,file); /* test write */
	if (size!=512) return _f_result(__LINE__,size);

/* go back, and read it */
	ret=f_rewind(file); /* back to the begining */
	if (ret) return _f_result(__LINE__,ret); /* it should fail */
	size=f_read(buffer,1,512,file); /* test read */
	if (size!=512) return _f_result(__LINE__,size); /* it should fail */
	pos=f_tell(file);
	if (pos!=512) return _f_result(__LINE__,pos); /* position has to be 512 */

/* close and check size */
	size=f_filelength("file.bin");
	if (size!=512) return _f_result(__LINE__,size); /* has to be zero */
	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);
	size=f_filelength("file.bin");
	if (size!=1024) return _f_result(__LINE__,size);

/* close again! */
	ret=f_close(file);
	if (ret!=F_ERR_NOTOPEN) return _f_result(__LINE__,pos);


/* check mulituser / file shareing access */
	file = f_open("file.bin","a+");
	if (!file) return _f_result(__LINE__,0);
	file2 = f_open("file.bin","r");
	if (!file2) return _f_result(__LINE__,0);
	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);
	ret=f_close(file2);
	if (ret) return _f_result(__LINE__,ret);

	file = f_open("file.bin","r");
	if (!file) return _f_result(__LINE__,0);
	file2 = f_open("file.bin","a+");
	if (!file2) return _f_result(__LINE__,0);
	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);
	file = f_open("file.bin","r");
	if (!file) return _f_result(__LINE__,0);
	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_close(file2);
	if (ret) return _f_result(__LINE__,ret);


	_f_dump("passed...");
	return 0;
}

/****************************************************************************
*
* f_appending
*
* Internal test function
*
* RETURNS
*
* error code
*
***************************************************************************/

int f_appending(void) 
{
	F_FILE *file;
	F_FILE *file2;
	F_FILE *file3;
	long size,a,tsize,pos,b;
	int ret;

	_f_dump("f_appending");

	ret=f_format(0);
	if (ret) return _f_result(__LINE__,ret);

	for (tsize=0,a=0; a<256; a++) 
	{
		file=f_open("ap.bin","a");
		if (!file) return _f_result(__LINE__,0);

		(void)memset(buffer,a,sizeof(buffer));
		size=f_write(buffer,1,a+128,file);
		if (size!=a+128) return _f_result(__LINE__,size);

		size=f_filelength("ap.bin");
		if (size != tsize) return _f_result(__LINE__,size);

		tsize+=a+128;

		ret=f_close(file);
		if (ret) return _f_result(__LINE__,ret);

		size=f_filelength("ap.bin");
		if (size != tsize) return _f_result(__LINE__,size);
	}

	file=f_open("ap.bin","r");
	if (!file) return _f_result(__LINE__,0);

	for (a=0; a<256; a++) 
	{
		if (checkfilecontent(a+128,(char)a,file)) return _f_result(__LINE__,a);
	}

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);

	for (tsize=0,a=0; a<256; a++) 
	{
		file=f_open("ap.bin","r");
		if (!file) return _f_result(__LINE__,0);

		ret=f_seek(file,tsize,F_SEEK_SET);
		if (ret) return _f_result(__LINE__,ret);

		pos=f_tell(file);
		if (pos!=tsize) return _f_result(__LINE__,pos);

		size=f_read(buffer,1,a+128,file);
		if (size!=a+128) return _f_result(__LINE__,size);

		for (b=0; b<a+128; b++) {
			if (buffer[b]!=(char)a) return _f_result(__LINE__,a);
		}

		tsize+=a+128;

		pos=f_tell(file);
		if (pos!=tsize) return _f_result(__LINE__,pos);

		ret=f_close(file);
		if (ret) return _f_result(__LINE__,ret);
	}

	ret=f_close(file);
	if (ret!=F_ERR_NOTOPEN) return _f_result(__LINE__,ret);

	ret=f_delete("ap.bin");
	if (ret) return _f_result(__LINE__,ret);

/* append and read */
	file=f_open("ap.bin","w+");
	if (!file) return _f_result(__LINE__,0);

	(void)memset(buffer,1,sizeof(buffer));
	size=f_write(buffer,1,8192,file);
	if (size!=8192) return _f_result(__LINE__,size);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);


	file=f_open("ap.bin","a+");
	if (!file) return _f_result(__LINE__,0);

	file2=f_open("ap.bin","r");
	if (!file2) return _f_result(__LINE__,0);

	if (checkfilecontent(8192,1,file2)) return _f_result(__LINE__,a);

	(void)memset(buffer,2,sizeof(buffer));
	size=f_write(buffer,1,2048,file);
	if (size!=2048) return _f_result(__LINE__,size);

	if (checkfilecontent(2048,2,file2)) return _f_result(__LINE__,a);


	(void)memset(buffer,3,sizeof(buffer));
	size=f_write(buffer,1,8192,file);
	if (size!=8192) return _f_result(__LINE__,size);

	if (checkfilecontent(8192-10,3,file2)) return _f_result(__LINE__,a);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);

	if (checkfilecontent(10,3,file2)) return _f_result(__LINE__,a);

	ret=f_close(file2);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_delete("ap.bin");
	if (ret) return _f_result(__LINE__,ret);

	/* zero start test */

	file=f_open("ap.bin","a+");
	if (!file) return _f_result(__LINE__,0);

	file2=f_open("ap.bin","r");
	if (!file2) return _f_result(__LINE__,0);

	file3=f_open("ap.bin","r");
	if (!file3) return _f_result(__LINE__,0);

	(void)memset(buffer,1,sizeof(buffer));
	size=f_write(buffer,1,2048,file);
	if (size!=2048) return _f_result(__LINE__,size);

	if (checkfilecontent(2048,1,file2)) return _f_result(__LINE__,a);

	(void)memset(buffer,3,sizeof(buffer));
	size=f_write(buffer,1,8192,file);
	if (size!=8192) return _f_result(__LINE__,size);

	if (checkfilecontent(8192-2048,3,file2)) return _f_result(__LINE__,a);

	if (checkfilecontent(2048,1,file3)) return _f_result(__LINE__,a);
	if (checkfilecontent(8192,3,file3)) return _f_result(__LINE__,a);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);

	if (checkfilecontent(2048,3,file2)) return _f_result(__LINE__,a);

	ret=f_close(file2);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_close(file3);
	if (ret) return _f_result(__LINE__,ret);


	_f_dump("passed...");
	return 0;
}

/****************************************************************************
*
* f_writing
*
* Internal test function
*
* RETURNS
*
* error code
*
***************************************************************************/

int f_writing(void) 
{
	F_FILE *file;
	long size,a;
	int ret;
	F_SPACE before,after;

	_f_dump("f_writing");

	ret=f_getfreespace(0,&before);
	if (ret) return _f_result(__LINE__,ret);

	for (a=0; a<256; a++) 
	{
		file=f_open("wr.bin","w");
		if (!file) return _f_result(__LINE__,0);

		(void)memset(buffer,a,sizeof(buffer));
		size=f_write(buffer,1,a*128,file);
		if (size!=a*128) return _f_result(__LINE__,size);

		ret=f_close(file);
		if (ret) return _f_result(__LINE__,ret);

		size=f_filelength("wr.bin");
		if (size != a*128) return _f_result(__LINE__,size);

		file=f_open("wr.bin","r");
		if (!file) return _f_result(__LINE__,0);

		if (checkfilecontent(a*128,(unsigned char)a,file)) return _f_result(__LINE__,a);

		ret=f_close(file);
		if (ret) return _f_result(__LINE__,ret);
	}


	for (a=0; a<256; a++) 
	{
		file=f_open("wr.bin","w+");
		if (!file) return _f_result(__LINE__,0);

		(void)memset(buffer,a,sizeof(buffer));
		size=f_write(buffer,1,a*128,file);
		if (size!=a*128) return _f_result(__LINE__,size);

		ret=f_close(file);
		if (ret) return _f_result(__LINE__,ret);

		size=f_filelength("wr.bin");
		if (size != a*128) return _f_result(__LINE__,size);

		file=f_open("wr.bin","r+");
		if (!file) return _f_result(__LINE__,0);

		if (checkfilecontent(a*128,(char)a,file)) 
		{
			return _f_result(__LINE__,a);
		}

		ret=f_close(file);
		if (ret) return _f_result(__LINE__,ret);
	}

	ret=f_getfreespace(0,&after);
	if (ret) return _f_result(__LINE__,ret);

	if (before.bad!=after.bad) return _f_result(__LINE__,0);
	if (before.free==after.free) return _f_result(__LINE__,0);
	if (before.used==after.used) return _f_result(__LINE__,0);
	if (before.total!=after.total) return _f_result(__LINE__,0);

	if (before.used+before.free!=after.used+after.free) return _f_result(__LINE__,0);

	ret=f_delete("wr.bin");
	if (ret) return _f_result(__LINE__,ret);

	ret=f_getfreespace(0,&after);
	if (ret) return _f_result(__LINE__,ret);

	if (before.bad!=after.bad) return _f_result(__LINE__,0);
	if (before.free!=after.free) return _f_result(__LINE__,0);
	if (before.used!=after.used) return _f_result(__LINE__,0);
	if (before.total!=after.total) return _f_result(__LINE__,0);



	file=f_open("wr.bin","w+");
	if (!file) return _f_result(__LINE__,0);

	(void)memset(buffer,0xff,sizeof(buffer));

	for (a=0; a<16; a++) 
	{ /* writes 16*32K 0xff-s */
   		size=f_write(buffer,1,sizeof(buffer),file);
   		if (size!=sizeof(buffer)) return _f_result(__LINE__,size);
	}

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);


	file=f_open("wr.bin","r");
	if (!file) return _f_result(__LINE__,0);

	if (checkfilecontent(16*(long)sizeof(buffer),0xff,file)) return _f_result(__LINE__,a);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_delete("wr.bin");
	if (ret) return _f_result(__LINE__,ret);



/* f_abortclose test */
	file=f_open("wr.bin","w+");
	if (!file) return _f_result(__LINE__,0);

	(void)memset(buffer,0x1,sizeof(buffer));

	for (a=0; a<14; a++) 
	{ /* writes 14*32K 0x01-s */
   		size=f_write(buffer,1,sizeof(buffer),file);
   		if (size!=sizeof(buffer)) return _f_result(__LINE__,size);
	}

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);


	file=f_open("wr.bin","r+");
	if (!file) return _f_result(__LINE__,0);

	(void)memset(buffer,0x2,sizeof(buffer));

	for (a=0; a<14; a++) 
	{ /* writes 14*32K 0x02-s */
   		size=f_write(buffer,1,sizeof(buffer),file);
   		if (size!=sizeof(buffer)) return _f_result(__LINE__,size);
	}

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);

	file=f_open("wr.bin","r");
	if (!file) return _f_result(__LINE__,0);

	if (checkfilecontent(14*(long)sizeof(buffer),0x2,file)) return _f_result(__LINE__,a);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);


	file=f_open("wr.bin","r+");
	if (!file) return _f_result(__LINE__,0);

	(void)memset(buffer,0x3,sizeof(buffer));

	for (a=0; a<15; a++) 
	{ /* writes 15*32K 0x03-s */
   		size=f_write(buffer,1,sizeof(buffer),file);
   		if (size!=sizeof(buffer)) return _f_result(__LINE__,size);
	}

	ret=f_abortclose(file);
	if (ret) return _f_result(__LINE__,ret);

	file=f_open("wr.bin","r");
	if (!file) return _f_result(__LINE__,0);

	if (checkfilecontent(14*(long)sizeof(buffer),0x2,file)) return _f_result(__LINE__,a);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);

/* check the result */
	size=f_filelength("wr.bin");
	if (size!=14*(long)sizeof(buffer)) return _f_result(__LINE__,size);

	ret=f_delete("wr.bin");
	if (ret) return _f_result(__LINE__,ret);



	_f_dump("passed...");
	return 0;
}

/****************************************************************************
*
* f_dots
*
* Internal test function
*
* RETURNS
*
* error code
*
***************************************************************************/

int f_dots(void) 
{
	int ret;
	long a,size;
	F_FILE *file;
	_f_dump("f_dots");

	ret=f_mkdir("/tt");
	if (ret) return _f_result(__LINE__,ret);

	ret=f_chdir("/tt");
	if (ret) return _f_result(__LINE__,ret);

	ret=f_rename(".","test");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);

	ret=f_rename("..","test");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);

	ret=f_rmdir(".");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);

	ret=f_rmdir("..");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);

	ret=f_chdir(".");
	if (ret) return _f_result(__LINE__,ret);

	ret=_f_checkcwd("tt");
	if (ret) return _f_result(__LINE__,ret);

	ret=f_delete(".");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);

	ret=f_delete("..");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);

	ret=f_mkdir(".");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);

	ret=f_mkdir("..");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);

	ret=f_mkdir("...");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);

	for (a=0; a<6; a++) 
	{
		char *mode;
		switch (a) 
		{
			case 0: mode="r"; break;
			case 1: mode="r+"; break;
			case 2: mode="w"; break;
			case 3: mode="w+"; break;
			case 4: mode="a"; break;
			case 5: mode="a+"; break;
			default: return _f_result(__LINE__,a);
		}

		file=f_open(".",mode);
		if (file) return _f_result(__LINE__,a);
		file=f_open("..",mode);
		if (file) return _f_result(__LINE__,a);
		file=f_open("...",mode);
		if (file) return _f_result(__LINE__,a);
	}

	size=f_filelength(".");
	if (size!=-1) return _f_result(__LINE__,size);
	size=f_filelength("..");
	if (size!=-1) return _f_result(__LINE__,size);
	size=f_filelength("...");
	if (size!=-1) return _f_result(__LINE__,size);


	ret=f_chdir("...");
	if (ret!=F_ERR_NOTFOUND) return _f_result(__LINE__,ret);

	ret=f_chdir("..");
	if (ret) return _f_result(__LINE__,ret);


	ret=f_rename("tt",".");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);

	ret=f_rename("tt","..");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);

	ret=f_rename("tt","...");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);

	ret=f_rename("tt","  ");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);

	ret=f_rename("  ","");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);

	ret=f_rename("tt","t2");
	if (ret) return _f_result(__LINE__,ret);

	ret=f_rmdir("t2");
	if (ret) return _f_result(__LINE__,ret);

	/* test filename started with dot(s) */
	ret=f_mkdir(".test");
	if (ret) return _f_result(__LINE__,ret);

	ret=f_rename(".test","..test");
	if (ret) return _f_result(__LINE__,ret);

	ret=f_rename("..test","...test");
	if (ret) return _f_result(__LINE__,ret);

	ret=f_rename("...test","....test");
	if (ret) return _f_result(__LINE__,ret);

	ret=f_rmdir("....test");
	if (ret) return _f_result(__LINE__,ret);

	ret=f_mkdir(" . .. ....");
	if (ret!=F_ERR_INVALIDNAME) return _f_result(__LINE__,ret);

	_f_dump("passed...");
	return 0;
}

/****************************************************************************
*
* f_flushing
*
* Internal test function
*
* RETURNS
*
* errorcode
*
***************************************************************************/

int f_flushing(void) 
{
	F_FILE *file;
	long pos,size;
	int ret;

	_f_dump("f_flushing");

	file=f_open("tpow.bin","w");
	if (!file) return _f_result(__LINE__,0);

	(void)memset(buffer,0,sizeof(buffer));
	size=f_write(buffer,1,128,file);
	if (size!=128) return _f_result(__LINE__,size);

	ret=_f_poweroff();
	if (ret) return _f_result(__LINE__,ret);
	ret=_f_poweron(); /* try to power off */
	if (ret) return _f_result(__LINE__,ret);

	file=f_open("tpow.bin","r");
	if (file) return _f_result(__LINE__,0);


	file=f_open("tpow.bin","w");
	if (!file) return _f_result(__LINE__,0);

	(void)memset(buffer,1,sizeof(buffer));
	size=f_write(buffer,1,128,file);
	if (size!=128) return _f_result(__LINE__,size);

	ret=f_flush(file);
	if (ret) return _f_result(__LINE__,ret);

	pos=f_tell(file);
	if (pos!=128) return _f_result(__LINE__,pos);

	(void)memset(buffer,2,sizeof(buffer));
	size=f_write(buffer,1,128,file);
	if (size!=128) return _f_result(__LINE__,size);

	pos=f_tell(file);
	if (pos!=256) return _f_result(__LINE__,pos);



	ret=_f_poweroff();
	if (ret) return _f_result(__LINE__,ret);
	ret=_f_poweron(); /* try to power off */
	if (ret) return _f_result(__LINE__,ret);

	file=f_open("tpow.bin","r");
	if (!file) return _f_result(__LINE__,0);

	if (checkfilecontent(128,1,file)) return _f_result(__LINE__,1);


	ret=_f_poweroff();
	if (ret) return _f_result(__LINE__,ret);
	ret=_f_poweron(); /* try to power off */
	if (ret) return _f_result(__LINE__,ret);

	file=f_open("tpow.bin","a+");
	if (!file) return _f_result(__LINE__,0);

	(void)memset(buffer,4,sizeof(buffer));
	size=f_write(buffer,1,1024,file);
	if (size!=1024) return _f_result(__LINE__,size);

	(void)memset(buffer,5,sizeof(buffer));
	size=f_write(buffer,1,4096,file);
	if (size!=4096) return _f_result(__LINE__,size);

	(void)memset(buffer,8,sizeof(buffer));
	size=f_write(buffer,1,10240,file);
	if (size!=10240) return _f_result(__LINE__,size);

	ret=f_flush(file);
	if (ret) return _f_result(__LINE__,ret);

	(void)memset(buffer,9,sizeof(buffer));
	size=f_write(buffer,1,10240,file);
	if (size!=10240) return _f_result(__LINE__,size);

	(void)memset(buffer,10,sizeof(buffer));
	size=f_write(buffer,1,10240,file);
	if (size!=10240) return _f_result(__LINE__,size);


	ret=_f_poweroff();
	if (ret) return _f_result(__LINE__,ret);
	ret=_f_poweron(); /* try to power off */
	if (ret) return _f_result(__LINE__,ret);

	file=f_open("tpow.bin","r");
	if (!file) return _f_result(__LINE__,0);

	if (checkfilecontent(128,1,file)) return _f_result(__LINE__,1);
	if (checkfilecontent(1024,4,file)) return _f_result(__LINE__,4);
	if (checkfilecontent(4096,5,file)) return _f_result(__LINE__,5);
	if (checkfilecontent(10240,8,file)) return _f_result(__LINE__,8);

	if (!f_eof(file)) return _f_result(__LINE__,0);


	ret=_f_poweroff();
	if (ret) return _f_result(__LINE__,ret);
	ret=_f_poweron(); /* try to power off */
	if (ret) return _f_result(__LINE__,ret);

	file=f_open("tpow.bin","r+");
	if (!file) return _f_result(__LINE__,0);

	(void)memset(buffer,10,sizeof(buffer));
	size=f_write(buffer,1,64,file);
	if (size!=64) return _f_result(__LINE__,size);


	ret=_f_poweroff();
	if (ret) return _f_result(__LINE__,ret);
	ret=_f_poweron(); /* try to power off */
	if (ret) return _f_result(__LINE__,ret);

	file=f_open("tpow.bin","r");
	if (!file) return _f_result(__LINE__,0);

	if (checkfilecontent(128,1,file)) return _f_result(__LINE__,1);
	if (checkfilecontent(1024,4,file)) return _f_result(__LINE__,4);
	if (checkfilecontent(4096,5,file)) return _f_result(__LINE__,5);
	if (checkfilecontent(10240,8,file)) return _f_result(__LINE__,8);

	if (!f_eof(file)) return _f_result(__LINE__,0);



	ret=_f_poweroff();
	if (ret) return _f_result(__LINE__,ret);
	ret=_f_poweron(); /* try to power off */
	if (ret) return _f_result(__LINE__,ret);

	file=f_open("tpow.bin","r+");
	if (!file) return _f_result(__LINE__,0);

	(void)memset(buffer,10,sizeof(buffer));
	size=f_write(buffer,1,64,file);
	if (size!=64) return _f_result(__LINE__,size);

	ret=f_flush(file);
	if (ret) return _f_result(__LINE__,ret);


	ret=_f_poweroff();
	if (ret) return _f_result(__LINE__,ret);
	ret=_f_poweron(); /* try to power off */
	if (ret) return _f_result(__LINE__,ret);

	file=f_open("tpow.bin","r");
	if (!file) return _f_result(__LINE__,0);

	if (checkfilecontent(64,10,file)) return _f_result(__LINE__,10);
	if (checkfilecontent(64,1,file)) return _f_result(__LINE__,1);
	if (checkfilecontent(1024,4,file)) return _f_result(__LINE__,4);
	if (checkfilecontent(4096,5,file)) return _f_result(__LINE__,5);
	if (checkfilecontent(10240,8,file)) return _f_result(__LINE__,8);

	if (!f_eof(file)) return _f_result(__LINE__,0);

	ret=f_delete("tpow.bin");
	if (ret!=F_ERR_BUSY) return _f_result(__LINE__,ret);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_delete("tpow.bin");
	if (ret) return _f_result(__LINE__,ret);

	_f_dump("passed...");
	return 0;
}

/****************************************************************************
*
* f_rit
*
* Internal test function
*
* RETURNS
*
* errorcode
*
***************************************************************************/

typedef struct
{
	unsigned long MagicNum;
	int Line;
	char Buf[87];
} struct_TestFileSysEntry;

#define NUM_OF_RECORDS  100

int f_rit() 
{
	unsigned long i;
	int ret;
    F_FILE* File;
    struct_TestFileSysEntry Entry;
    int  Pos;
    char Ch;
    long Founded;

    _f_dump("f_rit");

    File = f_open("MyTest","a+");
    if (!File)  
	{
		return _f_result(__LINE__,0);
    }

    /*  add records   */
    for(i=0 ; i<NUM_OF_RECORDS ; i++)
    {
		Ch = (char)(i % 10);
		Entry.MagicNum = 0xbcbcbcbc;
        Entry.Line = i;
        Entry.Buf[0] = Ch;
        Entry.Buf[10] =(char)(Ch+1);

        /*

        if(F_NO_ERROR != f_seek(File,0,F_SEEK_SET))
        {
			mnuhdl_PrintData("Fail, could not go to the start of the file");
            break;
        }

        */

        if(F_NO_ERROR != f_seek(File,0,F_SEEK_END))
        {
            /*  error   */
			return _f_result(__LINE__,0); /*  Fail, could not go to the end of the file  */
        }

        if(sizeof(struct_TestFileSysEntry) != f_write((void*)&Entry,1,sizeof(struct_TestFileSysEntry),File))
        {
            /*  error   */
			return _f_result(__LINE__,0); /*  Fail, could not write new entry  */
        }

        Pos = f_tell(File);
        if ( (unsigned long)((Pos / (int)sizeof(struct_TestFileSysEntry))-1) != i)
        {
            /*  error   */
			return _f_result(__LINE__,0); /*  Fail, wrong file position  */
        }
        if(F_NO_ERROR != f_seek(File,Pos - sizeof(struct_TestFileSysEntry),F_SEEK_SET))
        {
            /*  error   */
			return _f_result(__LINE__,0); /*  Fail, could not go to new entry position  */
        }

        if(sizeof(struct_TestFileSysEntry) != f_read((void*)&Entry,1,sizeof(struct_TestFileSysEntry),File))
        {
			/*  error   */
			return _f_result(__LINE__,0); /*  Fail, could not read the new entry  */
        }

        if((Entry.MagicNum != 0xbcbcbcbc) ||
           (Entry.Line != (int)i) ||
           (Entry.Buf[0] != Ch) ||
           (Entry.Buf[10] != Ch+1))  
		{
			return _f_result(__LINE__,0); /* Fail, the new entry is corrupted" */
        }
    }

    ret=f_close(File);
	if (ret) return _f_result(__LINE__,ret);


    /* -----------------  */

    /* Open file again */
    File = f_open("MyTest","a+");
    if (!File) return _f_result(__LINE__,0);

    /*  read records   */
    for(i=0 ; i<NUM_OF_RECORDS ; i++)
    {
		Ch = (char)(i % 10);

        if(F_NO_ERROR != f_seek(File,0,F_SEEK_SET))
        {
			/*  error   */
			return _f_result(__LINE__,0); /*  Fail, could not go to the start of the file  */
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
        if(!Founded) return _f_result(__LINE__,0); /*  Entry not founded  */
    }

    ret=f_close(File);
	if (ret) return _f_result(__LINE__,ret);


    ret=f_delete("MyTest");
	if (ret) return _f_result(__LINE__,ret);

	_f_dump("passed...");

    return 0;
}

/****************************************************************************
*
* f_truncating
*
* Internal test function
*
* RETURNS
*
* errorcode
*
***************************************************************************/

int f_truncating(void) 
{
	F_FILE *file;
	unsigned long size;
	int ret;
	F_SPACE before,after;

    _f_dump("f_truncating");

	ret=f_getfreespace(0,&before);
	if (ret) return _f_result(__LINE__,ret);

	file=f_open("test.bin","w+");
	if (!file) return _f_result(__LINE__,0);

	(void)(void)memset(buffer,1,16384);
	size=f_write(buffer,1,16384,file);
	if (size!=16384) return _f_result(__LINE__,size);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);

	file=f_truncate("test.bin",16380);
	if (!file) return _f_result(__LINE__,0);
	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);
	size=f_filelength("test.bin");
	if (size!=16380) return _f_result(__LINE__,size);


	file=f_truncate("test.bin",16384);
	if (!file) return _f_result(__LINE__,0);
	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);
	size=f_filelength("test.bin");
	if (size!=16384) return _f_result(__LINE__,size);


	file=f_truncate("test.bin",8100);
	if (!file) return _f_result(__LINE__,0);
	(void)(void)memset(buffer,2,92);
	size=f_write(buffer,1,92,file);
	if (size!=92) return _f_result(__LINE__,size);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);
	size=f_filelength("test.bin");
	if (size!=8192) return _f_result(__LINE__,size);


	file=f_truncate("test.bin",1);
	if (!file) return _f_result(__LINE__,0);
	(void)(void)memset(buffer,3,2);
	size=f_write(buffer,1,2,file);
	if (size!=2) return _f_result(__LINE__,size);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);
	size=f_filelength("test.bin");
	if (size!=3) return _f_result(__LINE__,size);


	ret=f_getfreespace(0,&after);
	if (ret) return _f_result(__LINE__,ret);

	if (before.bad!=after.bad) return _f_result(__LINE__,0);
	if (before.free==after.free) return _f_result(__LINE__,0);
	if (before.used==after.used) return _f_result(__LINE__,0);
	if (before.total!=after.total) return _f_result(__LINE__,0);
	if (before.used+before.free!=after.used+after.free) return _f_result(__LINE__,0);

	ret=f_delete("test.bin");
	if (ret) return _f_result(__LINE__,ret);


	ret=f_getfreespace(0,&after);
	if (ret) return _f_result(__LINE__,ret);

	if (before.bad!=after.bad) return _f_result(__LINE__,0);
	if (before.free!=after.free) return _f_result(__LINE__,0);
	if (before.used!=after.used) return _f_result(__LINE__,0);
	if (before.total!=after.total) return _f_result(__LINE__,0);


	_f_dump("passed...");
	return 0;
}

/****************************************************************************
*
* f_seteofing
*
* Internal test function
*
* RETURNS
*
* errorcode
*
***************************************************************************/

int f_seteofing(void) 
{
	F_FILE *file;
	unsigned long size;
	int ret;
	F_SPACE before,after;

    _f_dump("f_seteofing");

	ret=f_getfreespace(0,&before);
	if (ret) return _f_result(__LINE__,ret);

	file=f_open("test.bin","w+");
	if (!file) return _f_result(__LINE__,0);

	(void)(void)memset(buffer,1,16384);
	size=f_write(buffer,1,16384,file);
	if (size!=16384) return _f_result(__LINE__,size);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);


	size=f_filelength("test.bin");
	if (size!=16384) return _f_result(__LINE__,size);



	file=f_open("test.bin","r+");
	if (!file) return _f_result(__LINE__,0);

	ret=f_seek(file,16384,F_SEEK_SET);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_seteof(file);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);


	size=f_filelength("test.bin");
	if (size!=16384) return _f_result(__LINE__,size);



	file=f_open("test.bin","r+");
	if (!file) return _f_result(__LINE__,0);

	ret=f_seek(file,8191,F_SEEK_SET);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_seteof(file);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);


	size=f_filelength("test.bin");
	if (size!=8191) return _f_result(__LINE__,size);



	file=f_open("test.bin","r+");
	if (!file) return _f_result(__LINE__,0);

	ret=f_seek(file,2,F_SEEK_SET);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_seteof(file);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);


	size=f_filelength("test.bin");
	if (size!=2) return _f_result(__LINE__,size);


	file=f_open("test.bin","r+");
	if (!file) return _f_result(__LINE__,0);

	ret=f_seek(file,1,F_SEEK_SET);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_seteof(file);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);


	size=f_filelength("test.bin");
	if (size!=1) return _f_result(__LINE__,size);



	file=f_open("test.bin","r+");
	if (!file) return _f_result(__LINE__,0);

	ret=f_seteof(file);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);

	size=f_filelength("test.bin");
	if (size!=0) return _f_result(__LINE__,size);



	file=f_open("test.bin","w+");
	if (!file) return _f_result(__LINE__,0);

	(void)(void)memset(buffer,1,16384);
	size=f_write(buffer,1,16384,file);
	if (size!=16384) return _f_result(__LINE__,size);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);

	size=f_filelength("test.bin");
	if (size!=16384) return _f_result(__LINE__,size);



	file=f_open("test.bin","r+");
	if (!file) return _f_result(__LINE__,0);

	ret=f_seteof(file);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);

	size=f_filelength("test.bin");
	if (size!=0) return _f_result(__LINE__,size);



	file=f_open("test.bin","w+");
	if (!file) return _f_result(__LINE__,0);

	(void)(void)memset(buffer,1,16384);
	size=f_write(buffer,1,16384,file);
	if (size!=16384) return _f_result(__LINE__,size);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);

	size=f_filelength("test.bin");
	if (size!=16384) return _f_result(__LINE__,size);



	file=f_open("test.bin","r+");
	if (!file) return _f_result(__LINE__,0);

	ret=f_seek(file,12288,F_SEEK_SET);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_seteof(file);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_seek(file,8192,F_SEEK_SET);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_seteof(file);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_seek(file,4096,F_SEEK_SET);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_seteof(file);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_seek(file,0,F_SEEK_SET);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_seteof(file);
	if (ret) return _f_result(__LINE__,ret);

	ret=f_close(file);
	if (ret) return _f_result(__LINE__,ret);

	size=f_filelength("test.bin");
	if (size!=0) return _f_result(__LINE__,size);



	ret=f_getfreespace(0,&after);
	if (ret) return _f_result(__LINE__,ret);

	if (before.bad!=after.bad) return _f_result(__LINE__,0);
	if (before.free!=after.free) return _f_result(__LINE__,0);
	if (before.used!=after.used) return _f_result(__LINE__,0);
	if (before.total!=after.total) return _f_result(__LINE__,0);

	ret=f_delete("test.bin");
	if (ret) return _f_result(__LINE__,ret);

	ret=f_getfreespace(0,&after);
	if (ret) return _f_result(__LINE__,ret);

	if (before.bad!=after.bad) return _f_result(__LINE__,0);
	if (before.free!=after.free) return _f_result(__LINE__,0);
	if (before.used!=after.used) return _f_result(__LINE__,0);
	if (before.total!=after.total) return _f_result(__LINE__,0);


	_f_dump("passed...");
	return 0;
}

/****************************************************************************
 *
 * f_dotest
 *
 * main function to run the test
 *
 ***************************************************************************/

void f_dotest(void) 
{
	int err=0;

	_f_dump("File system test started...");

	/* here return value is not checked, because drive maybe not formatted */
	_f_poweron();

	if (f_formatting()) err++;
	if (f_dirtest()) err++;
	if (f_findingtest()) err++;
#if (!RAMDRIVETEST)
	if (f_powerfail()) err++;
#endif
	if (f_seeking(128)) err++;
	if (f_seeking(256)) err++;
	if (f_seeking(512)) err++;
	if (f_seeking(1024)) err++;
	if (f_seeking(2048)) err++;
	if (f_seeking(4096)) err++;
	if (f_seeking(8192)) err++;
	if (f_seeking(16384)) err++;
	if (f_seeking(32768)) err++;
	if (f_opening()) err++;
	if (f_appending()) err++;
	if (f_writing()) err++;
	if (f_dots()) err++;
#if (!RAMDRIVETEST)
	if (f_flushing()) err++;
#endif
	if (f_rit()) err++;
	if (f_truncating()) err++;
	if (f_seteofing()) err++;

	if (_f_poweroff()) err++;

	_f_dump("End of tests...");

	if (err) (void)_f_result(-1,err);
	else if (!err) _f_dump("No error found...");
}

/****************************************************************************
 *
 * end of test_s.c
 *
 ***************************************************************************/
