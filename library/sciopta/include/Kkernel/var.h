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
** ID: S01345BS2                                                    **
** +Revision: 1.4 +                                                **
** +Date: 2004/10/07 07:37:47 +                                     **
** Process variables                                                **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_VAR_H_
#define _KERNEL_VAR_H_ 1

#if SIZEOF_PTR == 2
#define SC_TAG_SIZE (4)
#elif SIZEOF_PTR == 4
#define SC_TAG_SIZE (8)
#else
#error Unknown ptr size
#endif

#ifndef __ASM_INCLUDED__
typedef struct sc_local_s{
  sc_tag_t tag;
  sc_var_t value;
}sc_local_t;

SCFUNC2(void,sc_procVarInit,sc_msgptr_t,unsigned int);
SCFUNC2(int,sc_procVarGet,sc_tag_t,sc_var_t NEARPTR);
SCFUNC2(int,sc_procVarSet,sc_tag_t tag,sc_var_t);
SCFUNC1(int,sc_procVarDel,sc_tag_t);
SCFUNC0(sc_msg_t,sc_procVarRm);
#if !defined(__KERNEL__)
#define sc_procVarInit(a,b) _FUNC2(sc_procVarInit,a,b)
#define sc_procVarGet(a,b)  _FUNC2(sc_procVarGet,a,b)
#define sc_procVarSet(a,b)  _FUNC2(sc_procVarSet,a,b)
#define sc_procVarDel(a)    _FUNC1(sc_procVarDel,a)
#define sc_procVarRm()      _FUNC0(sc_procVarRm)
#endif /* !defined(__KERNEL__) */

#endif /* __ASM_INCLUDED__ */

#endif

/*
****************************************************************************
** $Log: var.h,v $
** Revision 1.4  2004/10/07 07:37:47  bastian
** fixed NEAR * to NEARPTR
**
** Revision 1.3  2004/10/06 07:42:24  bastian
** introduced NEAR
**
** Revision 1.2  2004/10/04 12:11:37  bastian
** introduced sc_msg_t and sc_msgptr_t
**
** Revision 1.1  2003/05/19 12:06:05  bastian
** 1st
**
** Revision 2.8  2003/03/26 13:59:46  bastian
** added system-call/trap interface
** changed 2nd parameter of miscError
**
** Revision 2.7  2003/03/17 09:00:04  bastian
** added header
**
** Revision 2.6  2002/12/09 09:31:25  bastian
** renamed msgPool functions
** fixed typos
** added pool-name for sciopta.con
**
** Revision 2.5  2002/12/04 08:29:54  bastian
** started release-preparing
**
** Revision 2.4  2002/06/03 11:48:22  bastian
** Fixed const char *
**
** Revision 2.3  2002/03/21 14:46:41  bastian
** Added debugging support for nearly all functions.
** Added error-checking if swap-out fails.
** Fixed friend-concept.
** Misc. fixes.
**
** Revision 2.2  2001/12/19 11:55:51  bastian
** varSet not by reference
** varDel added
**
** Revision 2.1  2001/12/11 10:02:40  bastian
** Added local vars.
**
**
*/
