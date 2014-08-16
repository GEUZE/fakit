/*SOC -*-c++-*-
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
** ID: S01232BS2                                                    **
** +Revision: 2.5.14.1 +                                                 **
** +Date: 2009/10/04 09:53:43 +                                     **
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
***********************
** System heart-beat 
***********************
*/
SCFUNC0(void,sc_tick);
/*
***************************************
** sc_tickGet
** 
** Returns the current system tick
***************************************
*/
SCFUNC0(sc_time_t,sc_tickGet);
/*
***************************************
** sc_tickLength
** 
** Returns the current system tick-length
** in us.
** If tl != 0 sets it.
***************************************
*/
SCFUNCn(__u32,sc_tickLength,__u32);
/*
***************************************
** sc_tickMs2Tick
** 
** Converts a time in ms into ticks
***************************************
*/
SCFUNCn(sc_ticks_t,sc_tickMs2Tick,__u32);
/*
***************************************
** sc_tickTick2Ms
** 
** Converts a time in ticks into ms
***************************************
*/
SCFUNCn(__u32,sc_tickTick2Ms,sc_ticks_t);
#ifndef __KERNEL__
#define sc_tick()            _FUNC0(sc_tick)
#define sc_tickGet()         _FUNC0(sc_tickGet)
#define sc_tickLength(us)    _FUNCn(sc_tickLength,us)
#define sc_tickMs2Tick(ms)   _FUNCn(sc_tickMs2Tick,ms)
#define sc_tickTick2Ms(tick) _FUNCn(sc_tickTick2Ms,tick)
#endif /* __KERNEL__ */
#endif /* __ASM_INCLUDED__ */
#endif










