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
** ID: S03030BS1                                                    **
** +Revision: 1.14 +                                                **
** +Date: 2008/11/11 09:57:02 +                                     **
** Minimal printf used by logd and kernel.                          **
** A function __putchar needs to be provided.                       **
**********************************************************************
*/

#include <sciopta.h>
#include <stdarg.h>
#include <string.h>

#if defined __ARMCC_VERSION
/* suppress: assignment in condition warning */
#pragma diag_suppress 1293
#endif

extern void __putchar(int );

static const char hex[16] = {
  '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
};

void kprintf(int loglevel,const char *fmt,...)
{
  char c,f;
  char fill;
  char *p;
  const char FARPTR fp;
  int width;
  char h[16];
  va_list arg;
  unsigned long n;

  if ( loglevel > 9 ){
    loglevel = 9;
  }
  if ( loglevel ){
    __putchar('<');
    __putchar(loglevel+'0');
    __putchar('>');
    __putchar('(');
    fp = sc_sysProcCurrentName();
    while( fp && *fp ){
      __putchar(*fp++);
    }
    __putchar(')');
  }
  va_start(arg,fmt);
  while ( (c = *fmt++) ){
    if ( c != '%' ){
      __putchar(c);
      continue;
    }
    c = *fmt++;
    fill = ' ';
    if ( c == '0' ){
      fill = c;
      c = *fmt++;
    }
    for( width = 0; '0' <= c && c <= '9'; c = *fmt++ ){
      width *= 10;
      width += c - '0';
    }
    width = ( width > 15 ) ? 15 : width;
    p = h;
    *p = '\0';
    f = 0;
    if ( c == 'l' || c == 'L' ){
      f = 1;
      c = *fmt++;
    }
    switch( c ){
	case 'x':
	case 'X':
	case 'p':
	  if ( f ){
	    n = va_arg(arg,unsigned long);
	  } else {
	    n = (unsigned long)va_arg(arg,unsigned int);
	  }
	  do{
	    *p++ = hex[(int)(n & 0xf)];
	    n >>= 4;
	  }while(n);
	  break;
	case 'd':
	case 'u':
	  if ( f ){
	    n = va_arg(arg,unsigned long);	  
	  } else {
	    n = (unsigned long)va_arg(arg,unsigned int);
	  }
	  if ( c == 'd' && (signed long)n < 0 ){
	    __putchar('-');
	    n = (unsigned long)(-(signed long)n);
	  }
	  do{
	    *p++ = (char)(n % 10UL) +(char)'0';
	    n /= 10;
	  }while(n);
	  break;	  
	case 'c':
#if defined __GNUC__ || defined __ARMCC_VERSION
	  c = va_arg(arg,int);
#else
	  c = va_arg(arg,char);
#endif
	  *p++ = c;
	  break;
	case 's':
	  p = va_arg(arg,char *);	  
	  if ( p ){
	    for( width -= strlen(p); width > 0 ; --width ){
	      __putchar(fill);
	    }	  
	    while( *p ){
	      __putchar(*p);
	      ++p;
	    }
	  } else {
	    __putchar('(');
	    __putchar('N');
	    __putchar('I');
	    __putchar('L');
	    __putchar(')');
	  }
	  continue;
	default:
	  __putchar(c);
	  continue;
    }
    for( width = width - (int)(p-h); width > 0 ; --width ){
      __putchar(fill);
    }
    for( --p; p >= h ; --p){
      __putchar(*p);
    }
  }
  va_end(arg);
}
