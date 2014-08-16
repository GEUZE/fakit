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
**                                                                  **
** ID: S01222BS4                                                    **
** +Date: 2003/10/23 11:41:46 +                                     ** 
** +Revision: 2.3 +                                                 **
** Kernel internal io functions.                                    **
**********************************************************************
*/
#ifndef _MACHINE_I386_IO_H__
#define _MACHINE_I386_IO_H__
#include <ossys/types.h>
#ifdef __cplusplus
extern "C" {
#endif

static inline __u8 inb_p(__u16 port)
{
  __u8 ret;  
  __asm__ __volatile__("inb %%dx,%%al;outb %%al,$0x80":"=a" (ret):"d" (port));
  return ret;
}

static inline __u8 inb(__u16 port)
{
  __u8 ret;  
  __asm__ __volatile__("inb %%dx,%%al":"=a" (ret):"d" (port));
  return ret;
}

static inline __u16 inw_p(__u16 port)
{
  __u16 ret;  
  __asm__ __volatile__("inw %%dx,%%ax;outb %%al,$0x80":"=a" (ret):"d" (port));
  return ret;
}

static inline __u16 inw(__u16 port)
{
  __u16 ret;  
  __asm__ __volatile__("inw %%dx,%%ax":"=a" (ret):"d" (port));
  return ret;
}

static inline __u32 inl(__u16 port)
{
  __u32 ret;  
  __asm__ __volatile__("inl %%dx,%%eax":"=a" (ret):"d" (port));
  return ret;
}


static inline void outb(__u16 port,__u8 data)
{
  __asm__ __volatile__("outb %%al,%%dx"::"d" (port),"a" (data));
}

static inline void outb_p(__u16 port,__u8 data)
{
  __asm__ __volatile__("outb %%al,%%dx; outb %%al,$0x80"::"d" (port),"a" (data));
}

static inline void outw(__u16 port,__u16 data)
{
  __asm__ __volatile__("outw %%ax,%%dx"::"d" (port),"a" (data));
}

static inline void outw_p(__u16 port,__u16 data)
{
  __asm__ __volatile__("outw %%ax,%%dx; outb %%al,$0x80"::"d" (port),"a" (data));
}

static inline void outl(__u16 port,__u32 data)
{
  __asm__ __volatile__("outl %%eax,%%dx"::"d" (port),"a" (data));
}
#ifdef __cplusplus
}
#endif

#endif
