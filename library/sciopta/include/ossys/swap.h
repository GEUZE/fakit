/*SOC -*-c++-*-
**********************************************************************
**  _____   _______ _________ _______  _______ _________ _______    **
** (  __ \ (  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )   **
** | (  \/ | (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |   **
** | (___  | |         | |   | |   | || (____)|   | |   | (___) |   **
** (___  ) | |         | |   | |   | ||  _____)   | |   |  ___  |   **
**     ) | | |         | |   | |   | || (         | |   | (   ) |   **
** /\__) | | (____/\___) (___| (___) || )         | |   | )   ( |   **
** \_____) (_______/\_______/(_______)|/          )_(   |/     \|   **
**                                                                  **
** (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
**                                                                  **
**********************************************************************
** ID: S03303CL3
** +Revision: 1.4 + 
** +Date: 2004/07/03 05:47:22 + 
** little endian und bigendian stuff 
**********************************************************************
EOC*/
/*
** For log see end of file:
*/
#ifndef _OSSYS_SWAP_H_
#define _OSSYS_SWAP_H_ 1

#include <arch/endian.h>


#define __constSwap16(x) \
	((__u16)( \
		(((__u16)(x) & (__u16)0x00ffU) << 8) | \
		(((__u16)(x) & (__u16)0xff00U) >> 8) ))
#define __constSwap32(x) \
	((__u32)( \
		(((__u32)(x) & (__u32)0x000000ffUL) << 24) | \
		(((__u32)(x) & (__u32)0x0000ff00UL) <<  8) | \
		(((__u32)(x) & (__u32)0x00ff0000UL) >>  8) | \
		(((__u32)(x) & (__u32)0xff000000UL) >> 24) ))
#define __constSwap64(x) \
	((__u64)( \
		(__u64)(((__u64)(x) & (__u64)0x00000000000000ffULL) << 56) | \
		(__u64)(((__u64)(x) & (__u64)0x000000000000ff00ULL) << 40) | \
		(__u64)(((__u64)(x) & (__u64)0x0000000000ff0000ULL) << 24) | \
		(__u64)(((__u64)(x) & (__u64)0x00000000ff000000ULL) <<  8) | \
	        (__u64)(((__u64)(x) & (__u64)0x000000ff00000000ULL) >>  8) | \
		(__u64)(((__u64)(x) & (__u64)0x0000ff0000000000ULL) >> 24) | \
		(__u64)(((__u64)(x) & (__u64)0x00ff000000000000ULL) >> 40) | \
		(__u64)(((__u64)(x) & (__u64)0xff00000000000000ULL) >> 56) ))

  
#if (__BYTE_ORDER == __LITTLE_ENDIAN)

#define ntohl(x)      ((((__u32)(x) & 0x000000ff)<<24)| \
		       (((__u32)(x) & 0x0000ff00)<< 8)| \
		       (((__u32)(x) & 0x00ff0000)>> 8)| \
		       (((__u32)(x) & 0xff000000)>>24))

#define ntohs(x)     ((__u16)((((x) & 0xff) << 8) | (((x) >> 8) & 0x00ff)))



#define htonl(x) ntohl(x)
#define htons(x) ntohs(x)

#define __constCpu2Le64(x) ((__u64)(x))
#define __constCpu2Le32(x) ((__u32)(x))
#define __constCpu2Le16(x) ((__u16)(x))
#define __constCpu2Be64(x) __constSwap64((x))
#define __constCpu2Be32(x) __constSwap32((x))
#define __constCpu2Be16(x) __constSwap16((x))
  
#elif (__BYTE_ORDER == __BIG_ENDIAN)

#define ntohl(x) (x)
#define ntohs(x) (x)
#define htonl(x) (x)
#define htons(x) (x)
  
#define __constCpu2Le64(x) __constSwap64((x))
#define __constCpu2Le32(x) __constSwap32((x))
#define __constCpu2Le16(x) __constSwap16((x))
#define __constCpu2Be64(x) ((__u64)(x))
#define __constCpu2Be32(x) ((__u32)(x))
#define __constCpu2Be16(x) ((__u16)(x))
  
#else

#error must_include_arch_endian_header_file

#endif


#endif
/*
** $Log: swap.h,v $
** Revision 1.4  2004/07/03 05:47:22  bastian
** fixed ntohl
**
** Revision 1.3  2004/02/27 05:46:40  christian
** Added htons (include this header in netinet/in.h)
**
** Revision 1.2  2003/11/12 06:35:58  bastian
** fixed header
**
** Revision 1.1  2003/10/30 14:43:22  christian
** Added this header for big/little endian stuff, perhaps will add a hton
** ntoh later.
**
*/

