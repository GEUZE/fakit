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
** ID: S03076BS5                                                    **
** +Date: 2003/08/06 09:46:50 +                                     ** 
** +Revision: 2.2 +                                                 **
** symbols for descriptors                                          **
**********************************************************************
*/

#ifndef _MACHINE_I386_GDT_H_
#define _MACHINE_I386_GDT_H_

/*
 *  Each descriptor should have exactly one of next 8 codes to define the
 *  type of descriptor
 */

#define D_LDT   0x200   /* LDT segment        */
#define D_TASK  0x500   /* Task gate          */
#define D_TSS   0x900   /* TSS                */
#define D_CALL  0x0C00  /* 386 call gate      */
#define D_INT   0x0E00  /* 386 interrupt gate */
#define D_TRAP  0x0F00  /* 386 trap gate      */
#define D_DATA  0x1000  /* Data segment       */
#define D_CODE  0x1800  /* Code segment       */


/*
 *  Descriptors may include the following as appropriate:
 */

#define D_DPL3         0x6000   /* DPL3 or mask for DPL                    */
#define D_DPL2         0x4000   /* DPL2 or mask for DPL                    */
#define D_DPL1         0x2000   /* DPL1 or mask for DPL                    */
#define D_PRESENT      0x8000   /* Present                                 */
#define D_NOT_PRESENT  0x8000   /* Not Present                             */
                                /* Note, the PRESENT bit is set by default */
                                /* Include NOT_PRESENT to turn it off      */
                                /* Do not specify D_PRESENT                */


/*
 *  Segment descriptors (not gates) may include:
 */

#define D_ACC      0x100  /* Accessed (Data or Code)          */

#define D_WRITE    0x200  /* Writable (Data segments only)    */
#define D_READ     0x200  /* Readable (Code segments only)    */
#define D_BUSY     0x200  /* Busy (TSS only)                  */

#define D_EXDOWN   0x400  /* Expand down (Data segments only) */
#define D_CONFORM  0x400  /* Conforming (Code segments only)  */

#define D_BIG      0x40	  /* Default to 32 bit mode           */
#define D_BIG_LIM  0x80	  /* Limit is in 4K units             */


#endif
