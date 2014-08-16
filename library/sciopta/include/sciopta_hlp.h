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
**              (c) 2009 SCIOPTA Systems AG/ Schweiz                **
**                                                                  **
**********************************************************************
** ID: S09300BS1                                                    **
** +Revision: 1.1.2.2 +                                                 **
** +Date: 2009/10/29 10:25:30 +                                     **
** Workaround for IAR assembler not supporting variadic macros.     **
**********************************************************************
EOC*/

/*
** Modify function, needs C99 support in the pre-processor
*/
#if SC_CDEBUG == 1
#define _FUNC0(name) __SCC(__P__,x##name)(__LINE__,__FILE__)
#define _FUNCn(name,...) __SCC(__P__,x##name)(__VA_ARGS__,__LINE__,__FILE__)
#define SCFUNC0(type,name) \
   externC type __SCC(__P__,name)(void);\
   externC type __SCC(__P__,x##name)(int,const char *)
#define SCFUNCn(type,name,...) \
   externC type __SCC(__P__,name)(__VA_ARGS__);\
   externC type __SCC(__P__,x##name)(__VA_ARGS__,int,const char *)
#else
#define _FUNC0(name) __SCC(__P__,name)()
#define _FUNCn(name,...) __SCC(__P__,name)(__VA_ARGS__)
#define SCFUNC0(type,name) externC type __SCC(__P__,name)(void)
#define SCFUNCn(type,name,...) externC type __SCC(__P__,name)(__VA_ARGS__)
#endif /* SC_CDEBUG == 1 */

#define SYSSCFUNC0(type,name) externC type name(void)
#define SYSSCFUNCn(type,name,...) externC type name(__VA_ARGS__)
#define _SYSFUNC0(name) name()
#define _SYSFUNCn(name,...) name(__VA_ARGS__)
