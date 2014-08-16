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
** +Revision: 1.2 +                                                 **
** +Date: 2008/01/16 13:02:02 +                                     **
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
#if SIZEOF_PTR == 2
SCFUNC1(__u16,sc_tickLength,__u16);
#else
SCFUNC1(__u32,sc_tickLength,__u32);
#endif
/*
***************************************
** sc_tickMs2Tick
** 
** Converts a time in ms into ticks
***************************************
*/
#if SIZEOF_PTR == 2
SCFUNC1(sc_time_t,sc_tickMs2Tick,__u16);
#else
SCFUNC1(sc_time_t,sc_tickMs2Tick,__u32);
#endif
/*
***************************************
** sc_tickTick2Ms
** 
** Converts a time in ticks into ms
***************************************
*/
SCFUNC1(__u32,sc_tickTick2Ms,sc_time_t);
#ifndef __KERNEL__
#define sc_tick()         _FUNC0(sc_tick)
#define sc_tickGet()      _FUNC0(sc_tickGet)
#define sc_tickLength(a)  _FUNC1(sc_tickLength,a)
#define sc_tickMs2Tick(a)  _FUNC1(sc_tickMs2Tick,a)
#define sc_tickTick2Ms(a) _FUNC1(sc_tickTick2Ms,a)
#endif /* __KERNEL__ */
#endif /* __ASM_INCLUDED__ */
#endif
