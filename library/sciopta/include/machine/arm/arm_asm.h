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
** (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
**                                                                  **
**********************************************************************
** ID: S03217BS1                                                    **
** +Revision: 1.4 +                                                 **
** +Date: 2004/07/05 14:09:45 +                                     **
** CPU definitions                                                  **
**********************************************************************
EOC*/
// SOCNV
#define	PSR_I_BIT		0x80
#define	PSR_F_BIT		0x40
	
#define	PSR_MODE_MASK		0x1F
#define	PSR_SYS_MODE		0x1F
#define	PSR_USR_MODE		0x10
#define	PSR_FIQ_MODE		0x11
#define	PSR_IRQ_MODE		0x12
#define	PSR_SVC_MODE		0x13
#define	PSR_ABT_MODE		0x17
#define	PSR_UND_MODE		0x1b
	
#define	PSR_THUMB_MODE		0x20
#define	PSR_THUMB_USER_MODE	0x30
#define	PSR_THUMB_SYS_MODE	0x3f
// EOCNV
