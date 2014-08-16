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

#include <hcc/fatfs_thin/util.h>

#if F_LONGFILENAME

/****************************************************************************
 *
 * _f_strtolower
 *
 * convert a string into lower case
 *
 * INPUTS
 *
 * s - input string to convert
 *
 ***************************************************************************/
void _f_strtolower(char *s) 
{
  for (;;) 
  {
     char ch=*s;
     if (!ch) break;
     if (ch>='A' && ch<='Z') *s=(char)(ch-'A'+'a');
     s++;
  }
}


#if F_CHECKNAME
/****************************************************************************
 *
 * _f_checknamewc
 *
 * checking filename for wildcard character
 *
 * INPUTS
 * lname - filename (e.g.: filename)
 * wc - check for wildcards too?
 *
 * RETURNS
 *
 * 0 - if no contains wildcard character (? or *)
 * other - if contains any wildcard character
 *
 ***************************************************************************/
unsigned char _f_checknamewc(const char *lname, char wc) 
{
	for (;;) 
	{
		char ch=*lname++;
		if (!ch) return 0;
		if (wc && (ch=='?' || ch=='*')) return 1; 
		if (ch=='|' || ch=='<' || ch=='>' || ch=='/' || ch=='\\' || ch==':') return 1;
	}
}

#endif



/****************************************************************************
 *
 * _f_setfsname
 *
 * convert a single string into F_NAME structure
 *
 * INPUTS
 *
 * name - combined name with drive,path,filename,extension used for source
 * fsname - where to fill this structure with separated drive,path,name,ext
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if name contains invalid path or name
 *
 ***************************************************************************/
unsigned char _f_setfsname(const char *name,F_NAME *fsname) 
{
unsigned short namepos=0;
#if F_DIRECTORIES
unsigned short pathpos=0;
unsigned short a;
#endif

	if (!name[0]) return 1; /*no name*/

	if (name[1]==':') name+=2;

#if F_DIRECTORIES
	if (name[0]!='/' && name[0]!='\\') {
   		if (fn_getcwd(fsname->path,F_MAXPATH,0)) return 1; /*error*/
   		for (pathpos=0; fsname->path[pathpos];) pathpos++;
	}
#endif
	for (;;) {
		char ch=*name++; 

		if (!ch) break;

		if (ch==':') return 1; /*not allowed*/

		if (ch=='/' || ch=='\\') {
#if F_DIRECTORIES
			if (pathpos) {
				if (fsname->path[pathpos-1]=='/') return 1; /*not allowed double */

				if (pathpos>=F_MAXPATH-2) return 1; /*path too long*/
				fsname->path[pathpos++]='/';
			}

			/* remove end spaces */
			for (;namepos;) {
				if (fsname->lname[namepos-1]!=' ') break;
				namepos--;
			}

			for (a=0; a<namepos; a++) {
				if (pathpos>=F_MAXPATH-2) return 1; /*path too long*/
				fsname->path[pathpos++]=fsname->lname[a];
			}
			namepos=0;
			continue;
#else
			if (namepos) return 1;
#endif
		}

		if (ch==' ' && (!namepos)) continue; /*remove start spaces*/

		if (namepos>=F_MAXLNAME-2) return 1; /*name too long*/

		fsname->lname[namepos++]=ch;
	}

	fsname->lname[namepos]=0; /*terminates it*/
#if F_DIRECTORIES
	fsname->path[pathpos]=0;  /*terminates it*/
#endif
	/* remove end spaces */
	for (;namepos;) {
   		if (fsname->lname[namepos-1]!=' ') break;
   		fsname->lname[namepos-1]=0;
   		namepos--;
	}


	if (!namepos) return 2; /*no name*/
	return 0;
}

#endif /* F_LONGFILENAME */

