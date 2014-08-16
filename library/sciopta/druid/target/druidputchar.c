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
** (c) 2002..2005 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S05025CL1
** +Revision: 1.2 +
** +Date: 2005/03/01 13:42:21 +
** Description 
**********************************************************************
EOC*/

#include <druid/device.h>

char druid_text[256];
int druid_i = 0;

void druid_textDisplay (const char *);

void
druid_putchar (char c)
{
  if (druid_i >= 255) {
    druid_text[255] = 0;
    druid_textDisplay (druid_text);
    druid_text[0] = c;
    druid_i = 1;
  }
  else if (c == '\n') {
    druid_text[druid_i] = 0;
    druid_textDisplay (druid_text);
    druid_i = 0;
  }
  else {
    druid_text[druid_i] = c;
    ++druid_i;
  }
}

