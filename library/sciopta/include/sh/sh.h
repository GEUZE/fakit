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
** ID: S03070CL89                                                   **
** +Revision: 1.37 +                                                **
** +Date: 2008/01/16 13:02:10 +                                     **
** shell                                                            **
**********************************************************************
EOC*/

#ifndef _UTIL_SH_H_
#define _UTIL_SH_H_

#include <sciopta.h>
EXTERN_C_BEGIN

/**
   * All shell process/commands are described in a global variable
   * sh_processes which is defined as sh_process_t sh_processes[].
   * The sh_processes array will be terminated with a process which entries
   * are all 0.
   * \param[in]  name     name of the command
   * \param[in]  descr    a short command description
   * \param[in]  flags    type of command: 
   *                      SH_NOT_BE_PROC    is a buildin command 
   *                      SH_CAN_BE_PROC    is a command which can run in the
   *                                        background.
   *                      SH_MUST_BE_PROC   is a command which allways run as a
   *                                        process 
   *                      SH_MUST_BE_ZOMBIE is a command which allways run in
   *                                        the background.
   * \param[in]  stacksz  stack size of the process, not needed if it is a
   *                      buildin.
   * \param[in]  main     command itself with the wellknown argc and argv
   *                      parameters.
   */
typedef struct sh_process_s {
  const char *name;
  const char *descr;
  flags_t flags;
#define SH_NOT_BE_PROC		1
#define SH_CAN_BE_PROC 		2
#define SH_MUST_BE_PROC	 	3	
#define SH_MUST_BE_ZOMBIE	4
  int stacksz;
  int (*main) (int argc, char *argv[]);
} sh_process_t;

/* wrapper process to start shell commands as process and or zombies i
   */ 
SC_PROCESS (sh_process);

/**
   * sh_system can be used to execute sciopta shell command(s). The line
   * will be interpreted from the sciopta shell and the shell then executes
   * the commands as they are wirtten.
   * \param[in]  line     The sciopta shell command(s).
   * \return              Return the commands return value.
   */
int sh_system (const char *line);
/**
   * this behaves nearly like the sh_system but it does attach a '&' to execute
   * the line in the background!
   * \param[in]  msg      message of type sh_inherit_t which descripes the
   *                      command and its inherit file table.
   * \param[in]  bg       tell if you want to start the command in the 
   *                      background.
   * \param[in]  pid      the pid of the started command
   * \return              returns the commands return value, also the error
   *                      - if any - will be set -> sc_miscErrnoGet ().
   */
int sh_launcher (sc_msg_t msg, int bg, sc_pid_t * pid);
  
/* predefined shell commands
   */
  /**
   * Concatonate files together and put it on the stdout.
   */
int sh_cat (int argc, char *argv[]);
/**
   * Echo text on stdout.
   */
int sh_echo (int argc, char *argv[]);
/**
   * Command which will be called on error. This is allready build in the
   * sciopta shell.
   */
int sh_error (int argc, char *argv[]);
/**
   * Exit a shell. Will close all open filedescriptors.
   */
int sh_exit (int argc, char *argv[]);
/**
   * The sciopta shell.
   */
int sh_shell (int argc, char *argv[]);
/**
   * List directories (managers also treaded like directories!).
   */
int sh_ls (int argc, char *argv[]);
/**
 * Show all commands with its description text, this command could build in
 * as "?" or "help" or both
 */
int sh_cmds (int argc, char *argv[]);
/**
 * Login command
 */
int sh_login (int argc, char *argv[]);
/**
 * Do a message trace
 */
int sh_trace (int argc, char *argv[]);

/*
** Return value of a hex-string
*/
int sh_hextoi(const char *s);
/*
** return value of a string (decimal or hex)
*/
int sh_atoi(const char *s);

EXTERN_C_END
#endif
