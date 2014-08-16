/*SOC
**********************************************************************
**  ______  _______ _________ _______  _______ _________ _______    **
** (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
** | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
** | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
** (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
**       ) || |         | |   | |   | || (         | |   | (   ) |  **
** /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
** \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
**                                                                  **
** (c) 2002/2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
**                                                                  **
**********************************************************************
** ID: S030051CL1                                                   **
** +Revision: 1.5 +                                                 **
** +Date: 2003/08/06 06:27:51 +                                     **
** pipe device                                                      **
**********************************************************************
** Author: Christian Liesch                                         **
**********************************************************************
EOC*/

#ifndef _DEV_PIPEDEV_H_
#define _DEV_PIPEDEV_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

  void pipedev (const char *name);
  SC_PROCESS (SCP_pipe);
  
#ifdef __cplusplus
}
#endif
#endif
/*
** $Log: pipedev.h,v $
** Revision 1.5  2003/08/06 06:27:51  bastian
** fixed header
**
** Revision 1.4  2003/03/11 14:58:30  bastian
** added header
**
** Revision 1.3  2003/02/21 13:36:16  christian
** Added a dualopen message for devices which like to give backe a read AND
** a write handle (i.e. pipe).
**
** Revision 1.2  2003/02/21 10:56:36  christian
** Pipe could be used also for tty. Made some preparation for future develop.
**
** Revision 1.1  2003/02/20 13:25:40  christian
** Restart implementing pipe device.
**
**
**/
