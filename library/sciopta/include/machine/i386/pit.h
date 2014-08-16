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
** ID: S03076BS11                                                   **
** +Date: 2003/10/23 11:41:46 +                                     ** 
** +Revision: 2.4 +                                                 **
** Functions controlling PIT                                        **
**********************************************************************
*/
#ifndef _MACHINE_I386_PIT_H__
#define _MACHINE_I386_PIT_H__
#ifdef __cplusplus
extern "C" {
#endif
void pit_init(unsigned long h, unsigned char channel);
unsigned int pit_getchannel(unsigned char channel);
#ifdef __cplusplus
}
#endif

#define 	PIT_CTRL	0x43	/*	I/O for control		*/
#define		PIT_CNT0	0x40	/*	I/O for counter 0	*/
#define		PIT_CNT1	0x41	/*	I/O for counter 1	*/
#define		PIT_CNT2	0x42	/*	I/O for counter 2	*/

#define		PIT_SC0		0	/*	Select channel 0 	*/
#define		PIT_SC1		0x40	/*	Select channel 1 	*/
#define		PIT_SC2		0x80	/*	Select channel 2 	*/

#define		PIT_LOW		0x10	/*	RW low byte only 	*/
#define		PIT_HIGH	0x20	/*	RW high byte only 	*/
#define		PIT_BOTH	0x30	/*	RW both bytes 		*/

#define		PIT_MD0		0	/*	Mode 0 			*/
#define		PIT_MD1		0x2	/*	Mode 1 			*/
#define		PIT_MD2		0x4	/*	Mode 2 			*/
#define		PIT_MD3		0x6	/*	Mode 3 			*/
#define		PIT_MD4		0x8	/*	Mode 4 			*/
#define		PIT_MD5		0xA	/*	Mode 5 			*/

#define		PIT_BCD		1	/*	BCD mode 		*/

#define		PIT_LATCH	0	/*	Latch command 		*/

#define		PIT_READ	0xF0	/*    Read command 		*/
#define		PIT_CNT		0x20	/*    CNT bit  (Active low, subtract it) */
#define		PIT_STAT	0x10	/*    Status bit  (Active low, subtract it) */
#define		PIT_CH2		0x8	/*    Channel 2 bit 		*/
#define		PIT_CH1		0x4	/*    Channel 1 bit 		*/
#define		PIT_CH0		0x2	/*    Channel 0 bit 		*/

#endif
