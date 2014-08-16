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
** ID: S05219BS1                                                   **
** +Revision: 1.2 +                                                **
** +Date: 2005/12/06 13:54:38 +                                     **
** Message pool API and definitions                                 **
**********************************************************************
EOC*/

#ifndef _SCIOPTA_H_
#error Must not be included directly !
#endif

#ifndef _KERNEL_POOL_H_
#define _KERNEL_POOL_H_

/*SOCNV*/
#if SIZEOF_PTR == 2
#define POOL_BUFSIZES   0
#define POOL_CUR        2*SC_MAX_NUM_BUFFERSIZES
#define POOL_END        ((POOL_CUR)+2)
#define POOL_FREED      ((POOL_END)+2)
#define SIZEOF_POOL_CB  ((POOL_FREED)+2*SC_MAX_NUM_BUFFERSIZES)

#define POOL_DB_START   0
#define POOL_DB_BUFSIZES 2
#define POOL_DB_SIZE     2+2*SC_MAX_NUM_BUFFERSIZES
#define SIZEOF_POOL_DB   ((2+2*SC_MAX_NUM_BUFFERSIZES)+2)
#else
#define POOL_BUFSIZES   0
#define POOL_CUR        16
#define POOL_END        20
#define POOL_FREED      24
#define SIZEOF_POOL_CB  56

#define POOL_DB_START   0
#define POOL_DB_BUFSIZES 4
#define POOL_DB_SIZE    4+8*2
#define SIZEOF_POOL_DB  24
#endif
/*EOCNV*/
#ifndef __ASM_INCLUDED__
struct sc_pool_cb_s {
  sc_bufsize_t bufsizes[8];
  __u8 * pool_cur;
  __u8 * pool_end;
  sgl_t freed[8];  
} PACKED;

typedef struct sc_pool_cb_s sc_pool_cb_t;

struct sc_pool_db_s {
  __u8 *pool_start;
  sc_bufsize_t bufsizes[8];
  __uint pool_size;
}PACKED;

typedef struct sc_pool_db_s sc_pool_db_t;
#endif

/*SOCNV*/
#define SC_DEFAULT_POOL 127
/*EOCNV*/
#endif /*_KERNEL_POOL_H_*/
