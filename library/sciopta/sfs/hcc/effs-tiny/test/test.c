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
#include "../tiny.h"
#include "test.h"


#if TEST_ENABLE

#define TEST_FILE "test.bin"
#define ERR(x) ((S<<12)+(T<<8)+x)


static void _get_magic (char *s)
{
  if (s[1]=='z')
  {
    ++s[0];
    s[1]='a';
  }
  else
  {
    ++s[1];
  }
}



static unsigned short _filetest (unsigned char S)
{
  F_FIND find;
  F_SPACE sp,sp2;
  char s0[]="abcdefghij";
  char s1[]="zyxwv";
  char si0[sizeof(s0)+sizeof(s1)];
  F_FILE *f;
  long rc,i,max;
  unsigned char T;

  T=0;
  if (f_format()) return ERR(0);

  /* check getfreespace + open + close + findfirst(name / *) + length + space + read nothing*/
  T=1;
  if (f_getfreespace(&sp)) return ERR(0);
  f=f_open(TEST_FILE,"w");
  if (f)
  {
    f_close(f);
    if (f_findfirst(TEST_FILE,&find)) return ERR(1);
    if (f_findfirst("*",&find)) return ERR(2);
    if (find.length) return ERR(3);
  }
  else return ERR(4);
  if (f_getfreespace(&sp2)) return ERR(5);
  if (sp.free!=sp2.free) return ERR(6);
  
  f=f_open(TEST_FILE,"r");
  if (f)
  {
    if (f_read(si0,1,1,f)) return ERR(7);
    f_close(f);
  }
  else return ERR(8);

  /* check mode "w" */
  T=2;
  f=f_open(TEST_FILE,"w");
  if (f)
  {
    rc=f_write(s0,1,sizeof(s0)-1,f);
    f_close(f);
    if (rc!=sizeof(s0)-1) return ERR(0);
  }
  else return ERR(1);

  if (f_getfreespace(&sp)) return ERR(2);

  /* check mode "r" + write + read + compare buf */
  T=3;
  f=f_open(TEST_FILE,"r");
  if (f)
  {
    rc=f_write(s0,1,1,f);
    if (rc) return ERR(0);
    rc=f_read(si0,1,sizeof(si0),f);
    f_close(f);
    if (rc!=sizeof(s0)-1) return ERR(1);
    if (_memcmp(si0,s0,sizeof(s0)-1)) return ERR(2);
  }
  else return ERR(3);

  /* check mode "a" + append + seek to zero + read */
  T=4;
  f=f_open(TEST_FILE,"a");
  if (f)
  {
    if (f_seek(f,0,F_SEEK_SET)) return ERR(0);
    rc=f_write(s1,1,sizeof(s1)-1,f);
    if (rc!=sizeof(s1)-1) return ERR(0);
    if (f_seek(f,0,F_SEEK_SET)) return ERR(1);
    if (f_read(si0,1,1,f)) return ERR(2);
    f_close(f);
  }
  else return ERR(3);
  
  /* check appended file content */
  T=5;
  f=f_open(TEST_FILE,"r");
  if (f)
  {
    rc=f_read(si0,1,sizeof(si0),f);
    if (rc!=sizeof(s0)+sizeof(s1)-2) return ERR(0);
    f_close(f);
    if (_memcmp(si0,s0,sizeof(s0)-1)) return ERR(1);
    if (_memcmp(si0+sizeof(s0)-1,s1,sizeof(s1)-1)) return ERR(2);
  }
  else return ERR(3);

  /* check mode "a+", seek to 0, write, read, cmp, cmp, seek to 0, cmp */
  T=6;
  f=f_open(TEST_FILE,"r+");
  if (f)
  {
    if (f_write(s1,1,sizeof(s1)-1,f)!=sizeof(s1)-1) return ERR(1);
    if (f_read(si0,1,sizeof(si0),f)!=2*(sizeof(s1)-1)) return ERR(2);
    if (_memcmp(si0,s0+sizeof(s1)-1,sizeof(s1)-1)) return ERR(3);
    if (_memcmp(si0+sizeof(s1)-1,s1,sizeof(s1)-1)) return ERR(4);
    if (f_seek(f,0,F_SEEK_SET)) return ERR(5);
    if (f_read(si0,1,sizeof(si0),f)!=sizeof(s0)+sizeof(s1)-2) return ERR(6);
    if (_memcmp(si0,s1,sizeof(s1)-1)) return ERR(7);
    f_close(f);
  }
  else return ERR(8);
  
  if (f_getfreespace(&sp2)) return ERR(9);
  if (sp.free!=sp2.free) return ERR(10);

  /* chek mode "w+" */
  f=f_open(TEST_FILE,"w+");
  if (f)
  {
    f_close(f);
    if (f_getfreespace(&sp2)) return ERR(11);
    if (sp.free+F_CLUSTER_SIZE!=sp2.free) return ERR(12);
    f=f_open(TEST_FILE,"w+");
    if (f)
    {
      if (f_write(s0,1,sizeof(s0)-1,f)!=sizeof(s0)-1) return ERR(13);
      if (f_seek(f,1,F_SEEK_SET)) return ERR(14);
      if (f_read(si0,1,sizeof(si0),f)!=sizeof(s0)-2) return ERR(15);
      if (_memcmp(si0,s0+1,sizeof(s0)-2)) return ERR(16);
      f_close(f);
    }
    else return ERR(17);
  }
  else return ERR(18);

  if (f_getfreespace(&sp2)) return ERR(19);
  if (sp.free!=sp2.free) return ERR(20);

  /* check read/write on cluster boundary */
  T=7;
  max=(F_CLUSTER_SIZE/(sizeof(s0)-1))+5; 
  f=f_open(TEST_FILE,"w");
  if (f)
  {
    for (i=0;i<max;i++)
    {
      rc=f_write(s0,1,sizeof(s0)-1,f);
      if (rc==0) break;
    }
    if (rc==0) return ERR(0);
    f_close(f);
  }
  else return ERR(1);

     /* change chars on cluster boundary */
  f=f_open(TEST_FILE,"r+");
  if (f)
  {
    if (f_seek(f,F_CLUSTER_SIZE-2,F_SEEK_SET)) return ERR(2);
    if (f_write(s1,1,sizeof(s1)-1,f)!=sizeof(s1)-1) return ERR(3);
    f_close(f);
  }
  else return ERR(4);

     /* check new file */
  f=f_open(TEST_FILE,"r");
  if (f)
  {
    for (i=0;i<F_CLUSTER_SIZE/(sizeof(s0)-1);i++)
    {
      _memset(si0,0,sizeof(s0)-1);
      if (f_read(si0,1,sizeof(s0)-1,f)!=sizeof(s0)-1) return ERR(5);
      if (_memcmp(si0,s0,sizeof(s0)-1)) return ERR(6);
    }

    if (f_read(si0,1,F_CLUSTER_SIZE%(sizeof(s0)-1)-2,f)!=F_CLUSTER_SIZE%(sizeof(s0)-1)-2) return ERR(7);
    if (_memcmp(si0,s0,F_CLUSTER_SIZE%(sizeof(s0)-1)-2)) return ERR(8);
    if (f_read(si0,1,sizeof(s1)-1,f)!=sizeof(s1)-1) return ERR(9);
    if (_memcmp(si0,s1,sizeof(s1)-1)) return ERR(10);
    max=F_CLUSTER_SIZE%(sizeof(s0)-1)-2+sizeof(s1)-1;
    if (max>sizeof(s0)-1)
    {
      max-=(sizeof(s0)-1);
      i=3;
    }
    else
    {
      i=4;
    }

    if (f_read(si0,1,sizeof(s0)-1-max,f)!=(long)(sizeof(s0)-1-max)) return ERR(11);
    if (_memcmp(si0,s0+max,sizeof(s0)-1-max)) return ERR(12);

    max=i;
    for (i=0;i<max;i++)
    {
      _memset(si0,0,sizeof(s0)-1);
      if (f_read(si0,1,sizeof(s0)-1,f)!=sizeof(s0)-1) return ERR(13);
      if (_memcmp(si0,s0,sizeof(s0)-1)) return ERR(14);
    }
    f_close(f);
  }
  else return ERR(15);
 

     /* check append to end of the file */
  f=f_open(TEST_FILE,"a+");
  if (f)
  {
    if (f_write(s1,1,sizeof(s1)-1,f)!=sizeof(s1)-1) return ERR(16);
    _memset(si0,0,sizeof(s1)-1);
    if (f_seek(f,0,F_SEEK_SET)) return ERR(17);
    if (f_seek(f,0,F_SEEK_END)) return ERR(18);
    if (f_seek(f,-1*((long)sizeof(s1)-1),F_SEEK_CUR)) return ERR(19);
    if (f_read(si0,1,sizeof(s1)-1,f)!=sizeof(s1)-1) return ERR(20);
    if (_memcmp(si0,s1,sizeof(s1)-1)) return ERR(21);
    f_close(f);
  }
  else return ERR(22);

  /* test maximum width file name */
  T=8;
  {
    char sfn[]="a";
    char lfn[F_MAX_FILE_NAME_LENGTH+2];
    char *afn;
    for (i=0;i<F_MAX_FILE_NAME_LENGTH+1;lfn[i++]='a');
    lfn[i]=0;

    f=f_open(lfn,"w");
    if (f) return ERR(0);
    lfn[i-1]=0;

    f=f_open(lfn,"w");
    if (f)
    {
      if (f_write(s0,1,sizeof(s0)-1,f)!=sizeof(s0)-1) return ERR(1);
      f_close(f);

      for (i=0;i<2;i++)
      {
        if (i==0) 
        {
          if (f_rename(lfn,sfn)) return ERR(2);
          afn=sfn;
        }
        else
        {
          if (f_rename(sfn,lfn)) return ERR(3);
          afn=lfn;
        }
        
        f=f_open(afn,"r");
        if (f)
        {
          if (f_read(si0,1,sizeof(s0)-1,f)!=sizeof(s0)-1) return ERR(4);
          if (_memcmp(si0,s0,sizeof(s0)-1)) return ERR(5);
          f_close(f);
        }
        else return ERR(6);
      }

      if (f_delete(lfn)) return ERR(7);
    }
    else return ERR(8);
  }

  /* test maximum number of files */
  T=9;
  {
    char f_max[]="f_aa";
    F_FILE_ID_TYPE i;

    for (i=0;i<F_MAX_FILE;i++)
    {
      f=f_open(f_max,"w");
      if (f)
      {
        f_close(f);
      }
      else break;
      _get_magic(f_max+2);
    }
    if (i!=F_MAX_FILE-1) return ERR(0);
  }

  /* test maximum number of open files */
  T=10;
  {
    F_FILE *_f[F_MAX_OPEN_FILE];
    int i;
    char f_max[]="f_aa";
    char r[]="f_zz";
    
    for (i=0;i<F_MAX_OPEN_FILE;i++)
    {
      _f[i]=f_open(f_max,"w");
      if (_f[i])
      {
        if (f_write(f_max,1,sizeof(f_max),_f[i])!=sizeof(f_max)) return ERR(0);
        _get_magic(f_max+2);
      }
      else return ERR(1);
    }
    if (i!=F_MAX_OPEN_FILE) return ERR(2);
    for (i=0;i<F_MAX_OPEN_FILE;i++) f_close(_f[i]);
    
    f_max[2]='a';
    f_max[3]='a';
    for (i=0;i<F_MAX_OPEN_FILE;i++)
    {
      _f[i]=f_open(f_max,"r");
      if (_f[i])
      {
        if (f_read(r,1,sizeof(f_max),_f[i])!=sizeof(f_max)) return ERR(3);
        if (_strcmp(r,f_max)) return ERR(4);
        _get_magic(f_max+2);
      }
      else return ERR(5);
    }
    for (i=0;i<F_MAX_OPEN_FILE;i++) f_close(_f[i]);
  }

 
  return 0;
}

#if F_DIRECTORY
static unsigned short _dirtest (unsigned char S)
{
  unsigned char T;
  F_DIR_ID_TYPE i;
  F_FILE *f;
  char d_max[]="d_aa";
  char cwd[8];

  /* test maximum number of directories */
  T=0;
  for (i=0;i<F_MAX_DIR+1;i++)
  {
    if (f_mkdir(d_max)) break;
    _get_magic(d_max+2);
  }
  if (i!=F_MAX_DIR) return ERR(0);
  if (f_format()) return ERR(1);

  /* test directory create/remove/rename */
  T=1;
  if (f_mkdir("/d0")) return ERR(0);
  if (f_mkdir("/d0")==F_NOERR) return ERR(1);
  if (f_mkdir("d0")==F_NOERR) return ERR(2);
  if (f_mkdir("/d0/")==F_NOERR) return ERR(3);
  if (f_mkdir("/d0/d1")) return ERR(4);
  if (f_mkdir("/d1")) return ERR(5);
  if (f_chdir("d0/d1/")) return ERR(6);
  if (f_chdir("d0/d1/")==F_NOERR) return ERR(7);
  if (f_chdir("/d0/d1/")) return ERR(8);
  if (f_getcwd(cwd,8)) return ERR(9);
  if (_strcmp(cwd,"/d0/d1/")) return ERR(10);
  if (f_rmdir("../d1")==F_NOERR) return ERR(11);
  if (f_rmdir("/d1")) return ERR(12);
  if (f_chdir("../d1")) return ERR(13);
  if (f_chdir(".")) return ERR(14);
  if (f_rmdir("../d1")==F_NOERR) return ERR(15);
  if (f_chdir("..")) return ERR(16);
  if (f_rmdir("d1")) return ERR(17);
  if (f_rename("/d0","d2")==F_NOERR) return ERR(18);
  if (f_chdir("/")) return ERR(19);
  if (f_rename("/d0","/d2")==F_NOERR) return ERR(20);
  if (f_rename("/d0","d2")) return ERR(21);

  f=f_open("d2/f0","w");
  if (f)
  {
    if (f_rename("d2","d3")==F_NOERR) return ERR(22);
    f_close(f);
    if (f_rename("d2","d3")) return ERR(23);
    if (f_rmdir("d3")==F_NOERR) return ERR(24);
    if (f_delete("d3/f0")) return ERR(25);
    if (f_rmdir("d3")) return ERR(26);
  }
  else return ERR(27);

  return 0;
}
#endif

static unsigned char buf[TEST_BUF_SIZE];
unsigned short _bigfiletest (unsigned char S)
{
  F_FILE *f;
  unsigned char T;
  
#if F_DIRECTORY==0
  T=0;
  if (f_format()) return ERR(0);
#endif
  
  T=1;
#if BIGFILETEST_MAXFSSIZE
  {
    F_SPACE sp;
    f=f_open("bigfile.bin","w");
    if (f)
    {
      unsigned long i,j;
      
      if (f_getfreespace(&sp)) return ERR(0);
      j=sp.free/TEST_BUF_SIZE;
      _memset(buf,'a',TEST_BUF_SIZE);
      for (i=0;i<j+1;i++)
      {
        if (f_write(buf,TEST_BUF_SIZE,1,f)!=1) break;
      }
      if (i!=j) return ERR(1);
      
      f_close(f);
    }
    else return ERR(2);
    if (f_getfreespace(&sp)) return ERR(3);
    if (sp.free) return ERR(4);
  }
#else
  f=f_open("bigfile.bin","w");
  if (f)
  {
    char c;
    unsigned long i,j;

    j=BIGFILETEST_SIZE/TEST_BUF_SIZE;
    c='a';
    for (i=0;i<j;i++)
    {
      _memset(buf,c,TEST_BUF_SIZE);
      if (f_write(buf,TEST_BUF_SIZE,1,f)!=1) return ERR(0);
      if (++c>'z') c='a';
    }
    if (i!=j) return ERR(1);
    f_close(f);

    c='a';
    f=f_open("bigfile.bin","r");
    if (f)
    {
      unsigned long k;
      for (i=0;i<j;i++)
      {
        if (f_read(buf,TEST_BUF_SIZE,1,f)!=1) return ERR(2);
        for (k=0;k<TEST_BUF_SIZE && buf[k]==c;k++);
        if (k!=TEST_BUF_SIZE) return ERR(3);
        if (++c>'z') c='a';
      }
      f_close(f);
    }
  }
  else return ERR(4);
#endif

  if (f_delete("bigfile.bin")) return ERR(5);
  
  return 0;
}



unsigned short tinytest (void)
{
  unsigned short rc;

  rc=_filetest(0);
  if (rc) return rc;

#if F_DIRECTORY
  rc=_dirtest(1);
  if (rc) return rc;
#endif

  rc=_bigfiletest(2);
  if (rc) return rc;
  
  return 0;
}

#endif

