/* -*-c++-*-
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
** (c) 2002,2003 Sciopta GmbH, Litronic AG/ Schweiz                 **
**                                                                  **
**********************************************************************
** ID: S01222BS2                                                    **
** +Date: 2004/03/11 11:01:43 +                                     ** 
** +Revision: 2.7 +                                                 **
** Kernel internal video functions.                                 **
**********************************************************************
*/

#ifndef _MACHINE_I386_VIDEO_H_
#define _MACHINE_I386_VIDEO_H_

#define CRT_INDEX_REG 	0x03D4
#define CRT_DATA_REG  	0x03D5
#ifdef __cplusplus
extern "C" {
#endif
void cls(void);
void setxy(int x1,int y1);
void getxy(int *x1,int *y1);
void setfgcolor(int fg);
void setbgcolor(int bg);
void sys_putchar(char c);
#ifdef __cplusplus
}
#endif

/* colors */
#define C_BLACK 	0x0
#define C_BLUE 		0x1
#define C_GREEN 	0x2
#define C_CYAN 		0x3
#define C_RED 		0x4
#define C_MAGENTA 	0x5
#define C_BROWN 	0x6
#define C_WHITE 	0x7
#define C_GRAY 		0x8
#define C_LBLUE 	0x9
#define C_LGREEN 	0xa
#define C_LCYAN 	0xb
#define C_LRED 		0xc
#define C_LMAGENTA 	0xd
#define C_YELLOW 	0xe
#define C_LWHITE 	0xf

#define C_BCK_BLACK 	0x00
#define C_BCK_BLUE 	0x10
#define C_BCK_GREEN 	0x20
#define C_BCK_CYAN 	0x30
#define C_BCK_RED 	0x40
#define C_BCK_MAGENTA 	0x50
#define C_BCK_BROWN 	0x60
#define C_BCK_WHITE 	0x70

#define C_BCK_BLINK 	0x80

#endif

