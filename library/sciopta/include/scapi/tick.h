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
** (c) 2006 Sciopta Systems GmbH Deutschland, Sciopta AG/ Schweiz   **
**                                                                  **
**********************************************************************
** ID: S06059BS3                                                    **
** +Revision: 1.3 +                                                 **
** +Date: 2006/05/12 10:26:22 +                                     **
** Systemtick related functions                                     **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_TICK_H
#define _KERNEL_TICK_H
#ifndef __ASM_INCLUDED__

/*
***************************************
** sc_tickGet
** 
** Returns time in ms since API init.
***************************************
*/
SCFUNC0(DLLEXP sc_time_t,sc_tickGet);
/*
***************************************
** sc_tickLength
** 
** Returns the current system tick-length
** in us.
** If tl != 0 sets it.
***************************************
*/
SCFUNC1(DLLEXP __u32,sc_tickLength,__u32);
/*
***************************************
** sc_tickMs2Tick
** 
** Converts a time in ms into ticks
***************************************
*/
SCFUNC1(DLLEXP sc_ticks_t,sc_tickMs2Tick,__u32);
/*
***************************************
** sc_tickTick2Ms
** 
** Converts a time in ticks into ms
***************************************
*/
SCFUNC1(DLLEXP __u32,sc_tickTick2Ms,sc_ticks_t);
#ifndef __KERNEL__
#define sc_tickGet()	      _FUNC0(sc_tickGet)
#define sc_tickLength(us)     _FUNC1(sc_tickLength,us)
#define sc_tickMs2Tick(ms)    _FUNC1(sc_tickMs2Tick,ms)
#define sc_tickTick2Ms(ticks) _FUNC1(sc_tickTick2Ms,ticks)
#endif /* __KERNEL__ */
#endif /* __ASM_INCLUDED__ */
#endif










