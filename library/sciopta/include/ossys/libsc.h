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
** ID: S03070CL88                                                   **
** +Revision: 1.20 +                                                 **
** +Date: 2008/01/16 13:02:10 +                                     **
** libsc header                                                     **
**********************************************************************
EOC*/

#ifndef _UTIL_LIBSC_H_
#define _UTIL_LIBSC_H_

#include <sciopta.h>

EXTERN_C_BEGIN

typedef struct opt_r_s {
  int opterr;			/* if error message should be printed */
  int optind;			/* index into parent argv vector */
  int optopt;			/* character checked for validity */
  int optreset;			/* reset getopt */
  char *optarg;			/* argument associated with option */
} opt_r_t;

/*
 * getopt --
 *	Parse argc/argv argument vector.
 */
void initopt_r (opt_r_t * reent);
int getopt_r (opt_r_t * reent, int nargc, char *const *nargv,
	      const char *ostr);

/*
 * scprintf --
 *      Sciopta printf which operates on given fd.
 */ 
int scprintf (int fd, const char *format, ...);

/* sc_int2Str -- 
 *      integer to string converters
 */
int sc_miscInt2Str (__s32 number, char *to, unsigned int digits);
int sc_miscInt2Hex (__u32 number, char *to, unsigned int digits);
/*
** Return a decimal with max. 9 digits at the start of s
*/
int dectoi(const char *s);

/*
 * sc_miscChksum
 *      Sciopta cheksum calculator mainly used for internet procotols
 */
__u16 sc_miscChksum (const void FARPTR dp, __u16 count);
__u16 sc_miscChksumContd (const void FARPTR dp, __u16 count,__u16 total_init);

/*
 * sc_procTmpCreate 
 *      Sciopta launcher to start processes with a unique name
 */
sc_pid_t sc_procTmpCreate (const char *base, void (*process) (void), int stack, 
                           sc_poolid_t plid);

#if SIZEOF_INT == 4
#define SC_RAND_MAX 0x7fffffff
#else
#define SC_RAND_MAX 0x7fff
#endif
void sc_miscRandSeed(int seed);
int sc_miscRand(void);

/* CRC32 functions (Ethernet) */
__u32 sc_miscCrc32Contd(const __u8 FARPTR data,unsigned int len,__u32 hash);
__u32 sc_miscCrc32(const __u8 FARPTR data,unsigned int len);
__u32 sc_miscCrc32String(const char FARPTR data);

EXTERN_C_END

#endif /* _UTIL_LIBSC_H */
