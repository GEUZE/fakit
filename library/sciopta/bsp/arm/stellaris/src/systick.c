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
** ID: S08284BS2                                                    **
** +Revision: 1.3.2.1 +                                                 **
** +Date: 2009/07/24 14:49:14 +                                     **
** System tick for Stellaris                                        **
**********************************************************************
EOC*/

#include <sciopta.h>
#include "config.h"
/* LMI Driver Lib */
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

SC_INT_PROCESS(SCI_sysTick,src)
{
  if ( src == 0 ){
    sc_tick();
  } else if ( src == -1 ){
    __u32 cs = SysCtlClockGet();
    SysTickPeriodSet(cs/CNF_TICKLENGTH);
    SysTickIntEnable();
    SysTickEnable();
    IntPrioritySet(FAULT_SYSTICK,255);

    sc_tickLength(CNF_TICKLENGTH);
  }
}
