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
** ID: S03051BS2                                                    **
** +Revision: 1.8 +                                                 **
** +Date: 2006/02/24 14:51:24 +                                     **
** Defines endianess.                                               **
**********************************************************************
*/
#ifndef _ARCH_ENDIAN_H_
#define _ARCH_ENDIAN_H_

#ifdef __ARMCC_VERSION

#ifdef __BIG_ENDIAN
#define __BYTE_ORDER __BIG_ENDIAN
#define __LITTLE_ENDIAN (~__BIG_ENDIAN)
#else
#define __BIG_ENDIAN 0x12345678
#define __LITTLE_ENDIAN 0x78563412
#define __BYTE_ORDER __LITTLE_ENDIAN
#endif

#elif defined __GNUC__

#define __BIG_ENDIAN 0x12345678
#define __LITTLE_ENDIAN 0x78563412

#ifdef __ARMEB__
#define __BYTE_ORDER __BIG_ENDIAN
#else
#define __BYTE_ORDER __LITTLE_ENDIAN
#endif

#ifndef BIG_ENDIAN
#define BIG_ENDIAN      4321
#define LITTLE_ENDIAN   1234
#endif

#ifndef BYTE_ORDER
#ifdef __ARMEB__
#define BYTE_ORDER BIG_ENDIAN
#else
#define BYTE_ORDER LITTLE_ENDIAN
#endif
#endif
#else
#define __BIG_ENDIAN 0x12345678
#define __LITTLE_ENDIAN 0x78563412

#define __BYTE_ORDER __LITTLE_ENDIAN

#endif /* __GNUC__ */



#endif /*_ARCH_ENDIAN_H_*/
