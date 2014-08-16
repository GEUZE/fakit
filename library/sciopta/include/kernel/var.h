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
** +Revision: 2.12.14.1 +                                                **
** +Date: 2009/10/04 09:53:43 +                                     **
** Process variables                                                **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_VAR_H_
#define _KERNEL_VAR_H_ 1

#define SC_TAG_SIZE (8)

#ifndef __ASM_INCLUDED__
typedef struct sc_local_s{
  sc_tag_t tag;
  sc_var_t value;
}sc_local_t;

typedef struct sc_varpool_s {
  sc_msgid_t id;
  __u32 count;
  sc_local_t vars[1];
} sc_varpool_t;

SCFUNCn(void,sc_procVarInit,sc_msgptr_t,unsigned int);
SCFUNCn(int,sc_procVarGet,sc_tag_t,sc_var_t *);
SCFUNCn(int,sc_procVarSet,sc_tag_t,sc_var_t);
SCFUNCn(int,sc_procVarDel,sc_tag_t);
SCFUNC0(sc_msg_t,sc_procVarRm);
#if !defined(__KERNEL__)
#define sc_procVarInit(msgptr,size) _FUNCn(sc_procVarInit,msgptr,size)
#define sc_procVarGet(tag,valueptr) _FUNCn(sc_procVarGet,tag,valueptr)
#define sc_procVarSet(tag,value)    _FUNCn(sc_procVarSet,tag,value)
#define sc_procVarDel(tag)          _FUNCn(sc_procVarDel,tag)
#define sc_procVarRm()              _FUNC0(sc_procVarRm)
#endif /* !defined(__KERNEL__) */

#endif /* __ASM_INCLUDED__ */

#endif

/*
****************************************************************************
** $Log: var.h,v $
** Revision 2.12.14.1  2009/10/04 09:53:43  bastian
** added more function and fixes for das bug
**
** Revision 2.12  2006/10/11 14:01:40  bastian
** merger
**
** Revision 2.11.2.1  2006/06/29 12:27:36  bastian
** cleartext parameters
**
** Revision 2.11  2006/01/31 07:43:51  bastian
** fixed prototypes
**
** Revision 2.10  2004/04/29 07:09:27  bastian
** added structure to reflect local vars
**
** Revision 2.9  2003/05/19 13:38:29  bastian
** added special var-value type
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
