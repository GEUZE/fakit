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
** ID: S03070CL69                                                   **
** +Revision: 1.22 +                                                **
** +Date: 2008/01/16 13:02:10 +                                     **
** libsc                                                            **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <ossys/libsc.h>

#include <string.h>
#include <stdlib.h>

#if defined __ARMCC_VERSION
/* suppress: assignment in condition warning */
#pragma diag_suppress 1293
#endif

static const __u32 _10tab[11] = {
  1,
  10,
  100,
  1000,
  10000,
  100000,
  1000000,
  10000000,
  100000000,
  1000000000,
  2147483648U
};

int
sc_miscInt2Str (__s32 number, char *to, unsigned int digits)
{
  unsigned int ret;
  char tmp[12];
  char *p = tmp;
  ret = (number < 0);
  if ( number < 0 ){
    --digits;    
    number = -number;
  }
  if (digits == 0 || digits > 10) {
    digits = 10;
  }
  if ((__u32)number >= _10tab[digits]) {
    return -1;
  }
  *p++ = 0;
  do {
    *p++ = (char) ((number % 10) + '0');
    number /= 10;
  } while (number);
  if ( ret ){    
    *p++ = '-';
  }
  ret = (unsigned int)(p - tmp) - 1;
  do {
    /* empty */
  } while ((*to++ = *--p));
  return (int)ret;
}

sc_pid_t
sc_procTmpCreate (const char *base, void (*process) (void), int stack,
		  sc_poolid_t plid)
{
  char name[SC_PROC_NAME_SIZE + 1];
  int len, digits;
  unsigned int rnum, i, num;
  len = strlen (base);
  if ( len > SC_PROC_NAME_SIZE-1 ){
    return SC_ILLEGAL_PID;
  }
  strcpy (name, base);
  digits = SC_PROC_NAME_SIZE - len;
#ifdef __KKERNEL__
  if (digits > 4) {
    digits = 4;
  }
#else
  if (digits > 5) {
    digits = 5;
  }
#endif
  num = (unsigned int)_10tab[digits];
  for( i = 0; i < num; ++i ){
    rnum = sc_miscRand () % num;
    (void)sc_miscInt2Str (rnum, &name[len], digits);
    if ( sc_procIdGet (name, SC_NO_TMO) == SC_ILLEGAL_PID) {
#ifndef __KKERNEL__
      return sc_procPrioCreate (name, 
				process, 
				stack, 
				0,
				sc_procPrioGet (SC_CURRENT_PID), 
				SC_PDB_STATE_RUN,
				plid);
#else
      sc_pdbprio_t FARPTR pdb;
      sc_pcb_t NEARPTR pcb;
      sc_pid_t pid;
      pdb = (sc_pdbprio_t FARPTR)sc_msgAlloc(sizeof(sc_pdbprio_t),
					0,
					plid,
					SC_FATAL_IF_TMO);
      pcb = (sc_pcb_t NEARPTR)sc_msgAlloc(stack+sizeof(sc_pcb_t)+SC_PROC_NAME_SIZE+1,
					 0,
					 plid,
					 SC_FATAL_IF_TMO);
      strcpy((char *)(pcb+1)+stack,name);
      pdb->stacksize = stack;
      pdb->pcb = pcb;
      pdb->stack = (char NEARPTR) (pcb+1);
      pdb->entry = process;
      pdb->name = ((char *)(pcb+1))+stack;
      pdb->type = (int)SC_PDB_PRIO;
      pdb->prio = (__u8)sc_procPrioGet(SC_CURRENT_PID);
      pid = sc_procCreate((sc_pdb_t FARPTR)pdb,1);
      sc_msgFree((sc_msgptr_t)&pdb);
      return pid;
#endif      
    }
  }
  return SC_ILLEGAL_PID;
}
