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
#ifndef __TINY_H_
#define __TINY_H_

#ifdef __cplusplus
//->extern "C" {
#endif

#include "flashset.h"


#define _MAXVAL(s)	((((1<<((s<<3)-1))-1)<<1)+1)		/* maximum size of a variable (_SUL/_SUS/_SUC) */
#define _ALIGN(x)	((((x)+(_SALIGN-1))/_SALIGN)*_SALIGN)	/* calculate aligned size */

#define F_MAX_SIZE              (F_PAGE_SIZE*F_PAGE_COUNT)
#define F_CLUSTER_SIZE		(F_PAGE_PER_CLUSTER*F_PAGE_SIZE)
#define F_MAX_CLUSTER_COUNT	(F_PAGE_COUNT/F_PAGE_PER_CLUSTER)	/* number of maximum clusters */


/*******************************************************************************************************
************************************* define basic variable types **************************************
*******************************************************************************************************/
/* set FILE_ID type depending on the maximum number of file allowed in the system */
#if (F_MAX_FILE>(_MAXVAL(_SUS)-1))
  #define F_FILE_ID_TYPE	unsigned long
  #define F_FILE_ID_TYPE_SIZE	_SUL
#elif (F_MAX_FILE>(_MAXVAL(_SUC)-1))
  #define F_FILE_ID_TYPE	unsigned short
  #define F_FILE_ID_TYPE_SIZE	_SUS
#else
  #define F_FILE_ID_TYPE	unsigned char
  #define F_FILE_ID_TYPE_SIZE	_SUC
#endif

/* set LENGTH type depending on the size of the flash */
#if (F_MAX_SIZE>(_MAXVAL(_SUS)-1))
  #define F_LENGTH_TYPE		unsigned long
  #define F_LENGTH_TYPE_SIZE	_SUL
#elif (F_MAX_SIZE>(_MAXVAL(_SUC)-1))
  #define F_LENGTH_TYPE		unsigned short
  #define F_LENGTH_TYPE_SIZE	_SUS
#else
  #define F_LENGTH_TYPE		unsigned char
  #define F_LENGTH_TYPE_SIZE	_SUC
#endif

/* set DIR_ID type depending on the maximum number of directories allowed in the system */
#if F_DIRECTORY
  #if (F_MAX_DIR>(_MAXVAL(_SUS)-2))
    #define F_DIR_ID_TYPE	unsigned long
    #define F_DIR_ID_TYPE_SIZE	_SUL
  #elif (F_MAX_DIR>(_MAXVAL(_SUC)-2))
    #define F_DIR_ID_TYPE	unsigned short
    #define F_DIR_ID_TYPE_SIZE	_SUS
  #else
    #define F_DIR_ID_TYPE	unsigned char
    #define F_DIR_ID_TYPE_SIZE	_SUC
  #endif

  #define F_DIR_ROOT		((F_DIR_ID_TYPE)-2)	/* value to indicate root directory */
#endif

/* set FIND_ID type for finding routines, depends on maximum files+maximum directories */
#if F_DIRECTORY
  #if ((F_MAX_DIR+F_MAX_FILE)>((_MAXVAL(_SUS)-2)+(_MAXVAL(_SUS)-1)))
    #define F_FIND_ID_TYPE	unsigned long
  #elif ((F_MAX_DIR+F_MAX_FILE)>((_MAXVAL(_SUC)-2)+(_MAXVAL(_SUC)-1)))
    #define F_FIND_ID_TYPE	unsigned short
  #else
    #define F_FIND_ID_TYPE	unsigned char
  #endif
#else
  #define F_FIND_ID_TYPE	F_FILE_ID_TYPE
#endif

/* set type to address an element inside a cluster */
#if (F_CLUSTER_SIZE>(_MAXVAL(_SUS)-1))
  #define F_CLUSTER_TYPE	unsigned long
  #define F_CLUSTER_TYPE_SIZE	_SUL
#elif (F_CLUSTER_SIZE>(_MAXVAL(_SUC)-1))
  #define F_CLUSTER_TYPE	unsigned short
  #define F_CLUSTER_TYPE_SIZE	_SUS
#else
  #define F_CLUSTER_TYPE	unsigned char
  #define F_CLUSTER_TYPE_SIZE	_SUC
#endif

/* set type to count the total amount of clusters */
#if (F_MAX_CLUSTER_COUNT>(_MAXVAL(_SUS)-1))
  #define F_CLUSTER_COUNT_TYPE	unsigned long
  #define F_CLUSTER_COUNT_TYPE_SIZE	_SUL
#elif (F_MAX_CLUSTER_COUNT>(_MAXVAL(_SUC)-1))
  #define F_CLUSTER_COUNT_TYPE	unsigned short
  #define F_CLUSTER_COUNT_TYPE_SIZE	_SUS
#else
  #define F_CLUSTER_COUNT_TYPE	unsigned char
  #define F_CLUSTER_COUNT_TYPE_SIZE	_SUC
#endif



/*******************************************************************************************************
******* define file and directory structures and calculate the amount of management pages we need ******
*******************************************************************************************************/
/*
** File parameter block
*/
typedef struct {
  unsigned char attr;			/* attribute */
  unsigned short ctime;			/* creation time */
  unsigned short cdate;			/* creation date */
  F_LENGTH_TYPE length;			/* file length */
} F_FILE_PAR;
#define _F_FILE_PAR_SIZE	_ALIGN(_SUS+_SUS+F_LENGTH_TYPE_SIZE+_SUC)

/*
** file descriptor holding the name, the parameter block and optionaly the directory ID
*/
typedef struct {
  char name[F_MAX_FILE_NAME_LENGTH+1];	/* file name */
#if F_DIRECTORY
  F_DIR_ID_TYPE dirid;			/* current directory position */
#endif
  F_FILE_PAR par;			/* parameters */
} F_FILE_DSC;

/* calculate size the system will need to store F_FILE_DSC */
#if F_DIRECTORY
#define _F_FILE_DSC_SIZE	_ALIGN(_ALIGN(_SUC*(F_MAX_FILE_NAME_LENGTH+1))+_F_FILE_PAR_SIZE+F_DIR_ID_TYPE_SIZE)
#else
#define _F_FILE_DSC_SIZE	_ALIGN(_ALIGN(_SUC*(F_MAX_FILE_NAME_LENGTH+1))+_F_FILE_PAR_SIZE)
#endif


/*
** Directory parameter block
*/
#if F_DIRECTORY
typedef struct {
  unsigned char attr;		/* attribute */
  unsigned short ctime;		/* creation time */
  unsigned short cdate;		/* creation date */
} F_DIR_PAR;
#define _F_DIR_PAR_SIZE		_ALIGN(_SUS+_SUS+_SUC)

/*
** Directory descriptor holding the name, the parameter block and dirid
*/
typedef struct {
  char name[F_MAX_FILE_NAME_LENGTH+1];	/* dir name */
  F_DIR_ID_TYPE dirid;			/* parent directory */
  F_DIR_PAR par;			/* parameters */
} F_DIR_DSC;
/* calculate size the system will need to store F_DIR_DSC */
#define _F_DIR_DSC_SIZE		_ALIGN(_ALIGN(_SUC*(F_MAX_FILE_NAME_LENGTH+1))+_F_DIR_PAR_SIZE+F_DIR_ID_TYPE_SIZE)
#endif


/* set the maximum size the system can use for the header structure */
#define F_FILE_MGM_HEADER_MAX_SIZE	12

#define F_MGM_PAGE_SIZE			F_PAGE_SIZE	/* management page size = flash page size */

/* give an error if management page is too small to hold file descriptor + header */ 
#if (_F_FILE_DSC_SIZE+F_FILE_MGM_HEADER_MAX_SIZE)>F_MGM_PAGE_SIZE
 #error F_MAX_FILE_NAME_LENGTH too big to keep it in one page, try to decrease it!
#endif

/* calculate number of pages the directory management will need */
#if F_DIRECTORY
  #define F_MAX_DIR_ENTRY_PER_PAGE	(F_MGM_PAGE_SIZE/_F_DIR_DSC_SIZE)

  #if F_DIR_OPTIMIZE
    #if F_MAX_DIR_ENTRY_PER_PAGE>255
      #define F_DIR_ENTRY_PER_PAGE_SHIFT		8
    #elif F_MAX_DIR_ENTRY_PER_PAGE>127
      #define F_DIR_ENTRY_PER_PAGE_SHIFT		7
    #elif F_MAX_DIR_ENTRY_PER_PAGE>63
      #define F_DIR_ENTRY_PER_PAGE_SHIFT		6
    #elif F_MAX_DIR_ENTRY_PER_PAGE>31
      #define F_DIR_ENTRY_PER_PAGE_SHIFT		5
    #elif F_MAX_DIR_ENTRY_PER_PAGE>15
      #define F_DIR_ENTRY_PER_PAGE_SHIFT		4
    #elif F_MAX_DIR_ENTRY_PER_PAGE>7
      #define F_DIR_ENTRY_PER_PAGE_SHIFT		3
    #elif F_MAX_DIR_ENTRY_PER_PAGE>3
      #define F_DIR_ENTRY_PER_PAGE_SHIFT		2
    #elif F_MAX_DIR_ENTRY_PER_PAGE>1
      #define F_DIR_ENTRY_PER_PAGE_SHIFT		1
    #else 
      #define F_DIR_ENTRY_PER_PAGE_SHIFT		0
    #endif
    #define F_DIR_ENTRY_PER_PAGE		(1<<F_DIR_ENTRY_PER_PAGE_SHIFT)
  #else
    #define F_DIR_ENTRY_PER_PAGE		F_MAX_DIR_ENTRY_PER_PAGE
  #endif

  #define F_DIR_PAGE_COUNT		((F_MAX_DIR+(F_DIR_ENTRY_PER_PAGE-1))/F_DIR_ENTRY_PER_PAGE)
#endif  


/* file management area calculations */
#define F_FILE_MGM_PAGE_SIZE		(F_MGM_PAGE_SIZE-F_FILE_MGM_HEADER_MAX_SIZE)
#define F_FILE_MGM_PAGE0_SIZE		(F_FILE_MGM_PAGE_SIZE-_F_FILE_DSC_SIZE)
#define F_FILE_MGM_CLUSTER_PER_PAGE	(F_FILE_MGM_PAGE_SIZE/F_CLUSTER_COUNT_TYPE_SIZE)
#define F_FILE_MGM_CLUSTER_PER_PAGE0	(F_FILE_MGM_PAGE0_SIZE/F_CLUSTER_COUNT_TYPE_SIZE)

/* calculate number of extra pages needed to represent a file on cluster basis (1+n) */
#define F_FILE_MGM_EXTRA_PAGE_COUNT_CL	((((F_MAX_CLUSTER_COUNT-F_FILE_MGM_CLUSTER_PER_PAGE0)+(F_FILE_MGM_CLUSTER_PER_PAGE-1))/F_FILE_MGM_CLUSTER_PER_PAGE))
/* calculate number of extra pages needed if all files has base page number of clusters+1 in the chain */
/* !!!!!! NEED TO CHANGE !!!!!   F_MAX_FILE should be included */
#define F_FILE_MGM_EXTRA_PAGE_COUNT_1	(F_MAX_CLUSTER_COUNT/(F_FILE_MGM_CLUSTER_PER_PAGE0+1))
/* define the number of maximum extra pages needed */
#if (F_FILE_MGM_EXTRA_PAGE_COUNT_1>F_FILE_MGM_EXTRA_PAGE_COUNT_CL)
  #define F_FILE_MGM_EXTRA_PAGE_COUNT	F_FILE_MGM_EXTRA_PAGE_COUNT_1
#else
  #define F_FILE_MGM_EXTRA_PAGE_COUNT	F_FILE_MGM_EXTRA_PAGE_COUNT_CL
#endif

/* calculate number of pages for the file management area */
#define F_FILE_MGM_PAGE_COUNT		(F_MAX_FILE+F_MAX_OPEN_FILE+2*(F_FILE_MGM_EXTRA_PAGE_COUNT))

/* set type to count through the file management pages */
#if (F_FILE_MGM_PAGE_COUNT>(_MAXVAL(_SUS)-1))
  #define F_FILE_MGM_PAGE_COUNT_TYPE	unsigned long
#elif (F_FILE_MGM_PAGE_COUNT>(_MAXVAL(_SUC)-1))
  #define F_FILE_MGM_PAGE_COUNT_TYPE	unsigned short
#else
  #define F_FILE_MGM_PAGE_COUNT_TYPE	unsigned char
#endif

/* set type to count clusters in one file management page */
#if (F_FILE_MGM_CLUSTER_PER_PAGE>(_MAXVAL(_SUS)-1))
  #define F_FILE_MGM_CLUSTER_PER_PAGE_COUNT_TYPE	unsigned long
#elif (F_FILE_MGM_CLUSTER_PER_PAGE>(_MAXVAL(_SUC)-1))
  #define F_FILE_MGM_CLUSTER_PER_PAGE_COUNT_TYPE	unsigned short
#else
  #define F_FILE_MGM_CLUSTER_PER_PAGE_COUNT_TYPE	unsigned char
#endif

/* calculate the number of data clusters in the system (-1 is the ID page)*/
#if F_DIRECTORY
#define F_CLUSTER_COUNT		((F_PAGE_COUNT-1-F_DIR_PAGE_COUNT-F_FILE_MGM_PAGE_COUNT)/F_PAGE_PER_CLUSTER)
#else
#define F_CLUSTER_COUNT		((F_PAGE_COUNT-1-F_FILE_MGM_PAGE_COUNT)/F_PAGE_PER_CLUSTER)
#endif
#define F_SIZE			(((unsigned long)F_CLUSTER_COUNT*F_CLUSTER_SIZE))


/*******************************************************************************************************
********** define the begin address of FSID, directory, file management and data sections **************
*******************************************************************************************************/
#define F_FS_ID_ADDR		(F_BEGIN_ADDR)
#define F_FS_ID_SIZE		(F_MGM_PAGE_SIZE)
#if F_DIRECTORY
  #define F_DIR_ADDR		(F_FS_ID_ADDR+F_FS_ID_SIZE)
  #define F_DIR_SIZE		(F_DIR_PAGE_COUNT*F_MGM_PAGE_SIZE)
#else
  #define F_DIR_ADDR		(F_FS_ID_ADDR+F_FS_ID_SIZE)
  #define F_DIR_SIZE		(0)
#endif
#define F_FILE_MGM_ADDR		(F_DIR_ADDR+F_DIR_SIZE)
#define F_FILE_MGM_SIZE		(F_FILE_MGM_PAGE_COUNT*F_MGM_PAGE_SIZE)
#define F_FIRST_CLUSTER_ADDR	((((F_FILE_MGM_ADDR+F_FILE_MGM_SIZE)+(F_PAGE_PER_CLUSTER-1))/F_PAGE_PER_CLUSTER)*F_PAGE_PER_CLUSTER)


/*******************************************************************************************************
*********************** defines the address to access different sections *******************************
*******************************************************************************************************/
#define _SADDR(s,e) ((unsigned long)(&(((s *)0)->e)))

/* address of a directory descriptor */
#if F_DIRECTORY
  #if F_DIR_OPTIMIZE
    #define _DIR_DSC_ADDR(x)		(F_DIR_ADDR+((unsigned long)x>>F_DIR_ENTRY_PER_PAGE_SHIFT)*F_MGM_PAGE_SIZE+(((unsigned long)x&(F_DIR_ENTRY_PER_PAGE-1))*_F_DIR_DSC_SIZE))
  #else
    #define _DIR_DSC_ADDR(x)		(F_DIR_ADDR+((unsigned long)x/F_DIR_ENTRY_PER_PAGE)*F_MGM_PAGE_SIZE+(((unsigned long)x%F_DIR_ENTRY_PER_PAGE)*_F_DIR_DSC_SIZE))
  #endif
#endif

/* address of a file management page */
#define _FILE_ID_MGM_PAGE(id)			(f_volume.file_id_page[(id)])
#define _FILE_MGM_ADDR(page)			(F_FILE_MGM_ADDR+(unsigned long)(page)*F_MGM_PAGE_SIZE)
#define _FILE_MGM_HEADER_ADDR(page)		(_FILE_MGM_ADDR(page))
#define _FILE_ID_MGM_HEADER_ADDR(fileid)	(_FILE_MGM_HEADER_ADDR(_FILE_ID_MGM_PAGE(fileid)))
#define _FILE_MGM_PAGE_CLUSTER_ADDR(page,x)	(_FILE_MGM_ADDR(page)+F_FILE_MGM_HEADER_MAX_SIZE+(unsigned long)(x)*sizeof(F_CLUSTER_COUNT_TYPE))
#define _FILE_MGM_DSC_ADDR(page)		(_FILE_MGM_ADDR((page)+1)-sizeof(F_FILE_DSC))
#define _FILE_ID_MGM_DSC_ADDR(fileid)		(_FILE_MGM_DSC_ADDR(_FILE_ID_MGM_PAGE(fileid)))

/* address of cluster */
#define _CLUSTER_ADDR(x)		(F_FIRST_CLUSTER_ADDR+(unsigned long)x*F_CLUSTER_SIZE)

#define _SET_BIT(array,pos)	(array[(pos)>>3]|=(1U<<((pos)&7)))
#define _CLEAR_BIT(array,pos)	(array[(pos)>>3]&=~(1U<<((pos)&7)))
#define _GET_BIT(array,pos)	(array[(pos)>>3]&(1U<<((pos)&7)))


/****************************************************************************
 *  File open modes 
 ***************************************************************************/
#define F_MODE_CLOSE	0x00
#define F_MODE_READ	0x01
#define F_MODE_WRITE	0x02
#define F_MODE_APPEND	0x04
#define F_MODE_PLUS	0x08

#define F_MODE_CHANGED	0x80


/****************************************************************************
 * define file management page header 
 ***************************************************************************/
#define _FM_STATE_CLOSE	0x01	/* management page holding a closed file */
#define _FM_STATE_EXT	0x02	/* extension page */
#define _FM_STATE_OPEN	0xfe	/* opened file */
/*
** header of a file management page
*/
typedef struct {
  F_FILE_MGM_PAGE_COUNT_TYPE next;	/* next page */
  F_FILE_ID_TYPE fileid;		/* fileid */
  unsigned char seq;			/* Sequence number */
  unsigned char state;			/* Possible states: _FM_STATE_* */
} F_FILE_MGM_HEADER;



/****************************************************************************
 * define volume
 ***************************************************************************/
/* states of the volume */
#define F_STATE_NOTFORMATTED	0	/* volume state not formatted */
#define F_STATE_WORKING		1	/* volume state working */

/* set bit table sizes */
#define _CLUSTER_TABLE_SIZE	((F_CLUSTER_COUNT+7)/8)
#define _FILE_MGM_TABLE_SIZE	((F_FILE_MGM_PAGE_COUNT+7)/8)
#if F_CHANGE_NOTIFY
  #if F_DIRECTORY
    #define _CHANGE_TABLE_SIZE	(((F_MAX_DIR+F_MAX_FILE)+7)/8)
  #else
    #define _CHANGE_TABLE_SIZE	((F_MAX_FILE+7)/8)
  #endif
#endif

/*
** structure holding file system information
*/
typedef struct {
  unsigned char file_mgm_table[_FILE_MGM_TABLE_SIZE];	/* file management page table */
  unsigned char cluster_table[_CLUSTER_TABLE_SIZE];	/* free cluster table */
#if F_CHANGE_NOTIFY
  unsigned char changed[_CHANGE_TABLE_SIZE];		/* indicates changed entries */
#endif
  F_FILE_MGM_PAGE_COUNT_TYPE file_id_page[F_MAX_FILE];	/* start management page of a file */

  F_CLUSTER_COUNT_TYPE cluster_empty_pos;		/* empty cluster position */
  F_FILE_MGM_PAGE_COUNT_TYPE file_mgm_empty_pos;	/* empty management position */
  
#if F_DIRECTORY  
  F_DIR_ID_TYPE dir_empty_pos;				/* current directory */
  F_DIR_ID_TYPE current_dir;				/* current directory */
#endif  
  unsigned char state;					/* state of the volume */
} F_VOLUME;


#if F_CHECKMEDIA
/*
** Identification structure
*/
typedef struct {
  unsigned short version;
  unsigned short f_cluster_size;
  unsigned short f_mgm_page_size;
  unsigned short f_dir_page_count;  
  unsigned short f_file_mgm_page_count;
  unsigned short f_cluster_count;
  char tinystr[6];
} F_FS_ID;
#define _TINYSTR	"_TINY"
#endif

/*
** define relative directory strings
*/
#define _DIR_ACT	"."		/* points to actual directory */
#define _DIR_TOP	".."		/* points to parent directory */


/*
** Tiny version number
*/
#define _TINY_VERSION	0x0104


#ifdef __cplusplus
//->}
#endif

#endif 

