
#include "main.h"


#include "..\src\common\fsf.h"    
#include "..\src\common\fsm.h"    
#include "..\src\nor\flashdrv.h"    
#include "..\src\nand\nflshdrv.h"    
#include "..\src\ram\ramdrv.h"       
#include "..\src\nand\phy\samsung\k9f2816x0c.h"    
#include "..\src_int\nor\phy\amd\29lvxxxc.h"    
#include "..\src\test\test.h"    

#define RAM_DRIVE_TEST 1
#define NOR_DRIVE_TEST 0
#define NAND_DRIVE_TEST 0

#define RAM_DRIVE_SIZE 0x100000	// 1 Meg for RAM drive

static char *p=0;
static long mem_size;

int _fs_poweron()
{
    f_init();

#if RAM_DRIVE_TEST

    if(!p){   
	p=malloc(RAM_DRIVE_SIZE);
	if(!p)	    
	    return(1); // not enough memory to allocate
    }
    memset(p,0x55,RAM_DRIVE_SIZE); // fill with random value

    return f_mountdrive(
		0,
		p,
		RAM_DRIVE_SIZE,
		fs_mount_ramdrive,
		0);
    
#elif NOR_DRIVE_TEST
    if(!p)
    {
	memsize=fs_getmem_flashdrive(fs_phy_nor_29lvxxx);
	p=(char*)malloc(memsize);
	if (!p) return(1);	//Not enough memory to allocate
    }
    memset(p,0x55,memsize);	// fill with random value
    f_mountdrive(
	    0,
	    p,
	    memsize,
	    fs_mount_flashdrive, 
	    fs_phy_nor_29lvxxx); 
#elif NAND_DRIVE_TEST
    if(!p)
    {
	memsize=fs_getmem_nandflashdrive(fs_phy_nand_K9F2816X0C);
	p=(char*)malloc(memsize);	
	if (!p) return(1);//Not enough memory to allocate
    }
    memset(p,0x55,memsize);	// fill with random value
    
    f_mountdrive(
	    0,
	    p,
	    memsize,
	    fs_mount_nandflashdrive,
	    fs_phy_nand_K9F2816X0C);
#endif
}

void _fs_dump(char *s) {
  printf("%s\n",s);
}

long _fs_result(long test, long result) {
  printf("#### %d Test FAILED!! errorcode: %d\n",test,result);
  return test;
}


int main(long arg) {

   printf("\nFilesystem tester started\n%s\n",fs_getversion());

   fs_dotest();

   if(p)
       free(p);	// free malloc'd buffer
   return 0;
}

