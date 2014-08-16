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
** ID: S08350BS2                                                    **
** +Revision: 1.2.4.1 +                                                 **
** +Date: 2009/07/29 12:06:08 +                                     **
** 29GL128P driver                                                  **
**********************************************************************
EOC*/
#ifndef _29GL128P_H_
#define _29GL128P_H_

#include <hcc/effs/common/fsf.h>

#ifdef __cplusplus
extern "C" {
#endif


/*  128x128K  */

/*  The following is the physical/logical map  */
/* 	Size	logical number	rel. address	usage	         */
/* 	128k	0		0		file storage	 */
/* 	128k	1		0x0020000	file storage	 */
/* 	128k	2		0x0040000	file storage	 */
/* 	..	.		........	file storage	 */
/* 	128k	125		0xfa00000	file storage	 */
/* 	128k	126		0xfc00000	descriptor	 */
/* 	128k	127		0xfe00000	descriptor	 */


#define BLOCKSIZE	0x20000		/*  128k - size of file storage blocks  */
#define BLOCKSTART	0      		/*  logical number of first file storage block  */
#define MAXBLOCKS	126		/*  number of logical blocks for file storage  */

#define DESCSIZE	0x20000		/*  descriptor block size is 128K  */
#define DESCBLOCKSTART	126		/*  logical number of first descriptor block  */
#define DESCBLOCKS	2		/*  number of descriptor blocks  */
#define DESCCACHE	0x08000		/*  32k write cache in descriptor  */


#define SECTORSIZE	8192			/*  8k - sector size  */
#define SECTORPERBLOCK	(BLOCKSIZE/SECTORSIZE)	/*  number of sectors per block  */

#define FLASH_SIZE (BLOCKSIZE * 128)   /* size of _one_ flash chip */

/* Please check maximum storeable file and used sector number in FSmem.exe */

#define EFFS_RAMUSAGE 176660

/* functions implemented */

extern int fs_phy_nor_29gl128p(FS_FLASH *flash);

#ifdef __cplusplus
}
#endif

#endif /* _29GL128P_H_ */
