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
** ID: S05131BS1                                                    **
** +Revision: 1.3 +                                                **
** +Date: 2008/01/16 13:02:10 +                                     **
** random function                                                  **
**********************************************************************
EOC*/

#include <sciopta.h>
#include <ossys/libsc.h>

#if SIZEOF_INT == 4
static volatile __uint randSeed = 0x43674234;
#else
static volatile __uint randSeed = 0x4367;
#endif

void sc_miscRandSeed(int seed)
{
  if ( seed ){
    randSeed *= (__uint)seed;
  }
}

int sc_miscRand(void)
{
  __uint x;
#if SIZEOF_INT == 4
  x = randSeed * 1664525L+1013903223L;
#else
  x = randSeed * 16645+11390;
#endif
  if ( x ){
    randSeed = x;
  } else {
    randSeed = (__uint)sc_tickGet();
  }
  return (int)(x & SC_RAND_MAX);
}
