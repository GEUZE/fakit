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
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S04281CL1                                                    **
** +Revision: 1.24 +                                                 **
** +Date: 2006/10/11 13:38:12 +                                     **
** Druid device definition                                          **
**********************************************************************
EOC*/

#ifndef _DRUID_DEVICE_H_ 
#define _DRUID_DEVICE_H_ 1


#include <sciopta.h>
  
/**
 * for druid all scipota calls are forbidden, no triggers, no messages
 * The device part is board depending and belongs to the bsp, therefore
 * a interface will be defined which must be implemented by the bsp
 */ 

/**
 * --------------------------------------------------------------------------
 *  Structures need for following methode implementation
 * --------------------------------------------------------------------------
 */
  

/**
 * define druid cmd function
 */
typedef void (*druid_cmd_f) (void);
 
/**
 * define druid ISR function
 */
typedef void (*druid_isr_f) (int);

/**
 * --------------------------------------------------------------------------
 * Implement the following methdodes for the druid file
 * --------------------------------------------------------------------------
 */
  
/**
 * Init device specific stuff which is needed for the druid driver. As long
 * there is no read request around, the byte you read is index for the
 * command array! 
 */
void druid_init (druid_isr_f druid_rxisr,druid_isr_f druid_txisr);

/**
 * Get your timer configuration 
 */
int druid_timeUsPerTick (void);

/**
 * Reset timer
 */
void druid_timeReset (void);
  
/**
 * Get time 
 */
__u32 druid_timeGet (void);

/*
** Write len bytes
*/
void druid_write(const __u8 *,int len);

/*
** read a byte, return -1 on tmo
*/
int druid_read(void);

/**
 * --------------------------------------------------------------------------
 *  Methodes provided by the druid
 * --------------------------------------------------------------------------
 */

/**
 * Call this in the start hook
 */
void druid_startup (int stopMode);
  
/**
 * Error hook to extent your own error hook 
 */
int druid_errorHook(sc_errcode_t, sc_extra_t, int, const sc_pcb_t *);
 
/**
 * Put this in the __putchar methode. kprintf depends on this methode.
 */
void druid_putchar (char c);

#endif
