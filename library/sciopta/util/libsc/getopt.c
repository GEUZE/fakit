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
** (c) 2002,2003 Sciopta GmbH, Litronic AG/ Schweiz                 **
**                                                                  **
**********************************************************************
** ID: S03216CL3                                                    **
** +Revision: 1.6 +                                                 **
** getopt function                                                  **
**********************************************************************
*/
/*
 * Copyright (c) 1987, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if defined __ARMCC_VERSION
/* suppress: assignment in condition warning */
#pragma diag_suppress 1293
#endif

#if defined(LIBC_SCCS) && !defined(lint)
/* static char sccsid[] = "from: @(#)getopt.c	8.2 (Berkeley) 4/2/94"; */
static char *rcsid = "$Id: getopt.c,v 1.6 2008/01/16 13:02:10 bastian Exp $";
#endif /* LIBC_SCCS and not lint */

#include <string.h>
#include <ossys/libsc.h>
#include <scio.h>

#define	BADCH	(int)'?'
#define	BADARG	(int)':'
#ifdef NC30
#define	EMSG	_emsg
#else
#define	EMSG	(char *)""
#endif
/*
 * getopt --
 *	Parse argc/argv argument vector.
 */
void
initopt_r (opt_r_t * reent)
{
  reent->opterr = 1;
  reent->optind = 1;
}
int
getopt_r (opt_r_t * reent, int nargc, char *const *nargv, const char *ostr)
{
#ifdef NC30
  char _emsg[] = "";
  static char * place = NULL;	/* option letter processing */
#else
  static char *place = EMSG;	/* option letter processing */
#endif
  const char *oli = NULL;	/* option letter list index */
#ifdef NC30
  if ( !place ) {
    place = EMSG;
  }
#endif
  if (reent->optreset || !*place) {	/* update scanning pointer */
    reent->optreset = 0;
    if (reent->optind >= nargc || *(place = nargv[reent->optind]) != '-') {
      place = EMSG;
      return (-1);
    }
    if (place[1] && *++place == '-') {	/* found "--" */
      ++reent->optind;
      place = EMSG;
      return (-1);
    }
  }				/* option letter okay? */
  if ((reent->optopt = (int) *place++) == (int) ':' ||
      !(oli = strchr (ostr, reent->optopt))) {
    /*
     * if the user didn't specify '-' as an option,
     * assume it means -1.
     */
    if (reent->optopt == (int) '-')
      return (-1);
    if (!*place)
      ++reent->optind;
    if (reent->opterr && *ostr != ':')
      (void) fdprintf (scio_stderr,
		      "%s: illegal option -- %c\n", nargv[0], reent->optopt);
    return (BADCH);
  }
  if (*++oli != ':') {		/* don't need argument */
    reent->optarg = NULL;
    if (!*place)
      ++reent->optind;
  }
  else {			/* need an argument */
    if (*place)			/* no white space */
      reent->optarg = place;
    else if (nargc <= ++reent->optind) {	/* no arg */
      place = EMSG;
      if (*ostr == ':')
	return (BADARG);
      if (reent->opterr)
	(void) fdprintf (scio_stderr,
			"%s: option requires an argument -- %c\n",
			nargv[0], reent->optopt);
      return (BADCH);
    }
    else			/* white space */
      reent->optarg = nargv[reent->optind];
    place = EMSG;
    ++reent->optind;
  }
  return (reent->optopt);	/* dump back option letter */
}
