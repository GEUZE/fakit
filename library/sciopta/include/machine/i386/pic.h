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
** ID: S03076BS8                                                    **
** +Date: 2003/10/23 11:41:46 +                                     ** 
** +Revision: 2.6 +                                                 **
** Functions to control PIC                                         **
**********************************************************************
*/
#ifndef _MACHINE_I386_PIC_H__
#define _MACHINE_I386_PIC_H__

#ifndef __ASM_INCLUDED__
#ifdef __cplusplus
extern "C" {
#endif
void pic_init(void);
void pic_irqEnable(unsigned short irq_num);
void pic_irqDisable(unsigned short irq_num);
#ifdef __cplusplus
}
#endif
#endif

#define PIC0_IO   0x20      /* I/O for master PIC              */
#define PIC0_IMR  0x21      /* I/O for master IMR              */
#define PIC1_IO   0xA0      /* I/O for slave PIC               */
#define PIC1_IMR  0xA1      /* I/O for slace IMR               */

#define PIC_CMD_EOI 0x20     /* EOI command                     */

#define ICW1   0x11     /* Cascade, Edge triggered         */
                        /* ICW2 is vector                  */
                        /* ICW3 is slave bitmap or number  */
#define ICW4   0x01     /* 8088 mode                       */

#define PIC0_VEC  0x20     /* Vector for master               */
#define PIC1_VEC  0x28     /* Vector for slave                */

#define OCW3_IRR  0x0A  /* Read IRR                        */
#define OCW3_ISR  0x0B  /* Read ISR                        */

#define PIC_SRC_PIT0  0
#define PIC_SRC_KBD   1
#define PIC_SRC_FDC   6
#define PIC_SRC_COM1  4
#define PIC_SRC_COM2  3	
#endif

