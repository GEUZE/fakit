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
** (c) 2006 Sciopta Systems GmbH Deutschland, Sciopta AG/ Schweiz   **
**                                                                  **
**********************************************************************
** ID: S06055BS7                                                    **
** +Revision: 1.4 +                                                 **
** +Date: 2006/03/24 09:55:12 +                                     **
** Kernel internal string functions.                                **
**********************************************************************
EOC*/
#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif


#ifndef _KERNEL_STRING_H_
#define _KERNEL_STRING_H_

char *strcpy8(char *dest, const char *src);
char *strncpy8(char *dest, const char *src,size_t n);

size_t strlen8(const char *s);
size_t strnlen8(const char *s,size_t maxlen);
char * strchr8(const char *src, char ch);
int strcmp8(const char *s1, const char *s2);
int strncmp8(const char *s1, const char *s2,size_t n);
char *strcat8(char *dst, const char *src);
void memset32(void *from,__u32 c, size_t n);
void memcpy32(void *dest, const void *src, size_t n);
void strcpy32(const char *s1,const char *s2);

#endif /* _KERNEL_STRING_H_ */


