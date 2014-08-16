/*SOC
**********************************************************************
**  _______  _______ _________ _______  _______ _________ _______   **
** (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
** | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
** | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
** (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
**       ) || |         | |   | |   | || (         | |   | (   ) |  **
** /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
** \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
**                                                                  **
**              (c) 2008 SCIOPTA Systems AG/ Schweiz                **
**                                                                  **
**********************************************************************
** ID: S09054BS2                                                    **
** +Revision: 1.1.2.1 +                                                 **
** +Date: 2009/02/23 08:55:11 +                                     **
** 29GL128M driver                                                  **
**********************************************************************
EOC*/
#ifndef _29GL128M_H_
#define _29GL128M_H_

#include <hcc/effs/common/fsf.h>

#ifdef __cplusplus
extern "C" {
#endif


/*  256x64K  */
/*  The following is the physical/logical map  */
/* 	Size	logical number	rel. address	usage	         */
/* 	64k	0		0		program code	 */
/* 	64k	1		0x0010000	    -"- 	 */
/* 	..	.		........	    -"- 	 */
/* 	64k	128		0x0800000	file storage	 */
/* 	..	.		........	file storage	 */
/* 	64k	253		0xfd00000	file storage	 */
/* 	64k	254		0xfe00000	descriptor	 */
/* 	64k	255		0xff00000	descriptor	 */


#define BLOCKSIZE	0x10000		/*  64k - size of file storage blocks  */
#define BLOCKSTART	0    		/*  logical number of first file storage block  */
#define MAXBLOCKS	254		/*  number of logical blocks for file storage  */

#define DESCSIZE	0x10000		/*  descriptor block size is 64K  */
#define DESCBLOCKSTART	254		/*  logical number of first descriptor block  */
#define DESCBLOCKS	2		/*  number of descriptor blocks  */
#define DESCCACHE	0x08000		/*  32k write cache in descriptor  */


#define SECTORSIZE	2048			/*  8k - sector size  */
#define SECTORPERBLOCK	(BLOCKSIZE/SECTORSIZE)	/*  number of sectors per block  */


/* Please check maximum storeable file and used sector number in FSmem.exe */

#define EFFS_RAMUSAGE 92600

/* functions implemented */

extern int fs_phy_nor_29gl128m(FS_FLASH *flash);

#ifdef __cplusplus
}
#endif

#endif /* _29GL128P_H_ */
