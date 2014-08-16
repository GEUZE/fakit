#include <sciopta.h>
#include <sfs/sc2hcc.h>

#define FS_NAME "sdd_fatfs"
/*
** Filesystem process
*/
SC_PROCESS(SCP_fatfs)
{
  sc_poolid_t pl;

  pl = sc_poolIdGet("fs_pool");
  if ( pl != SC_ILLEGAL_POOLID ){
    sc_poolDefault(pl);
  }

  fatfs_process(FS_NAME);
}
