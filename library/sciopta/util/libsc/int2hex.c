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
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S04041RS1                                                    **
** +Revision: 1.5 +                                                 **
** +Date: 2005/07/01 11:26:06 +                                     **
** Integer to Hex String                                            **
**********************************************************************
EOC*/
#include <sciopta.h>

#if defined __ARMCC_VERSION
/* suppress: assignment in condition warning */
#pragma diag_suppress 1293
#endif

int sc_miscInt2Hex (__u32 number, char *to, unsigned int digits)
{
  unsigned int ret;
  char tmp[9];
  char *p = tmp;
  if (digits == 0 || digits > 8) {
    return -1;
  }
  ret = 0;
  *p++ = 0;
  do {
    *p++ = "0123456789abcdef"[(int) (number & 0xf)];
    number >>= 4;
    ++ret;
    if (ret > digits) {
      return -1;
    }
  } while (number);
  do {
    /* empty */
  } while ((*to++ = *--p));
  return (int)ret;
}
