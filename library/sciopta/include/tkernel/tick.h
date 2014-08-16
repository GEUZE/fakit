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
** ID: S05223BS3                                                    **
** +Revision: 1.1 +                                                 **
** +Date: 2005/08/11 13:55:38 +                                     **
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
/* Dummy, Tiny does not supply this */
#define sc_tickLength(a) a

/*
***************************************
** sc_tickMs2Tick
** 
** Converts a time in ms into ticks
***************************************
*/
/* Dummy, Tiny does not supply this */
#define sc_tickMs2Tick(a) (a)

/*
***************************************
** sc_tickTick2Ms
** 
** Converts a time in ticks into ms
***************************************
*/
/* Dummy, Tiny does not supply this */
#define sc_tickTick2Ms(a) (a)

#ifndef __KERNEL__
#define sc_tick()         _FUNC0(sc_tick)
#define sc_tickGet()      _FUNC0(sc_tickGet)
#endif /* __KERNEL__ */
#endif /* __ASM_INCLUDED__ */
#endif










