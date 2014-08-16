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
**              (c) 2007 SCIOPTA Systems AG/ Schweiz                **
**                                                                  **
**********************************************************************
** ID: S03080BS1                                                    **
** +Date: 2008/01/16 13:02:02 +                                     **
** +Revision: 2.4 +                                                 **
** API definition for Sciopta using system-call                     **
**********************************************************************
*/
#ifndef _SCIOPTA_SC_H_
#define _SCIOPTA_SC_H_ 1

#ifdef _SCIOPTA_H_
#error Include only sciopta.h or sciopta_sc.h
#endif

#define __SYSCALL__
#include <sciopta.h>
#endif /* _SCIOPTA_SC_H_ */
/*
** $Log: sciopta_sc.h,v $
** Revision 2.4  2008/01/16 13:02:02  bastian
** merge from 1.9.1.12
**
** Revision 2.3.12.1  2007/11/28 10:41:19  bastian
** fixed ifdef order
**
** Revision 2.3  2003/08/05 11:08:58  bastian
** fixed header
**
** Revision 2.2  2003/04/11 14:38:53  bastian
** add SC_CDEBUG
**
** Revision 2.1  2003/03/26 13:59:46  bastian
** added system-call/trap interface
** changed 2nd parameter of miscError
**
**
*/
