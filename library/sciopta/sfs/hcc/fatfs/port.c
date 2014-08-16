/****************************************************************************
 *
 *            Copyright (c) 2003-2008 by HCC Embedded
 *
 * This software is copyrighted by and is the sole property of
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1133
 * Vaci ut 110
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/

#include <hcc/fatfs/fat.h>
#include <hcc/fatfs/port.h>
#include <sfs/sc2hcc.h>
#include <dev/sem.h>

/****************************************************************************
 *
 * f_getrand
 *
 * This function should be ported. It has to return a different 32bit
 * random number whenever it is called. Random number generator could be
 * get from system time, this algorithm below is just a simple random
 * number generator
 *
 * INPUTS
 *
 * rand - a number which could be used for random number generator
 *
 * RETURNS
 *
 * 32bit random number
 *
 ***************************************************************************/

#if (!FN_CAPI_USED)
static unsigned long dwrand=0x729a8fb3;

unsigned long f_getrand(unsigned long rand)
{
	int a;
	unsigned short time,date;

	f_getcurrenttimedate(&time,&date);

	dwrand^=(unsigned long)((unsigned long)(time) | ((unsigned long)(date) << 16));

	for (a=0; a<32; a++)
	{
		if (rand&1)
		{
			dwrand^=0x34098bc2;
		}
		if (dwrand&0x8000000)
		{
			dwrand<<=1;
			dwrand|=1;
		}
		else dwrand<<=1;
		rand>>=1;
	}

	return dwrand;
}
#endif

/****************************************************************************
 *
 * f_getcurrenttimedate
 *
 * need to be ported depending on system, it retreives the
 * current time and date in DOS format. User must solve roll-over when reading
 * time and date. Roll-over problem to read a date at 23.59.59 and then reading time at
 * 00:00.00.
 *
 * INPUTS
 *
 * ptime - pointer where to store time or 0 if don't store time
 * pdata - pointer where to store date or 0 if don't store date
 *
 ***************************************************************************/

#if (!FN_CAPI_USED)
void f_getcurrenttimedate(unsigned short *ptime, unsigned short *pdate)
{
	unsigned short hour=12;
	unsigned short min=0;
	unsigned short sec=0;

	unsigned short time= (unsigned short)(((hour     <<  F_CTIME_HOUR_SHIFT) & F_CTIME_HOUR_MASK) |
						   ((min      <<  F_CTIME_MIN_SHIFT)  & F_CTIME_MIN_MASK) |
						   (((sec>>1) <<  F_CTIME_SEC_SHIFT)  & F_CTIME_SEC_MASK));

	unsigned short year=1980;
	unsigned short month=1;
	unsigned short day=1;

	unsigned short date= (unsigned short)((((year-1980) <<  F_CDATE_YEAR_SHIFT)  & F_CDATE_YEAR_MASK) |
						   ((month		 <<  F_CDATE_MONTH_SHIFT) & F_CDATE_MONTH_MASK) |
						   ((day		 <<  F_CDATE_DAY_SHIFT)   & F_CDATE_DAY_MASK));

	if (ptime)
	{
		*ptime = time;
	}

	if (pdate)
	{
		*pdate = date;
	}
}
#endif

/****************************************************************************
 *
 * f_mutex_create
 *
 * user function to create a mutex.
 *
 * RETURNS
 *   0 - success
 *   1 - error
 *
 ***************************************************************************/

#if (!FN_CAPI_USED)
int f_mutex_create (FN_MUTEX_TYPE *mutex)
{
  static int cnt = 0;
  sdd_obj_t *sem;
  char name[12] = "FATFSmutex00";

  name[9] = (cnt >> 4)+'0';
  name[10] = (cnt & 0xf)+'0';
  
  sem = sem_new(name,1,"/SCP_sem",SC_DEFAULT_POOL);
  if ( sem == NULL ) return 1;

  ++cnt;
  *mutex = (FN_MUTEX_TYPE)sem;
  return 0;
}
#endif

/****************************************************************************
 *
 * f_mutex_delete
 *
 * user function to delete a mutex.
 *
 * RETURNS
 *   0 - success
 *   1 - error
 *
 ***************************************************************************/

#if (!FN_CAPI_USED)
int f_mutex_delete (FN_MUTEX_TYPE *mutex)
{
  return sem_rm((sdd_obj_t **)mutex);
}
#endif

/****************************************************************************
 *
 * f_mutex_get
 *
 * user function to get a mutex.
 *
 * RETURNS
 *   0 - success
 *   1 - error
 *
 ***************************************************************************/

#if (!FN_CAPI_USED)
int f_mutex_get (FN_MUTEX_TYPE *mutex)
{
  return sem_decr((sdd_obj_t *)*mutex,1);
}
#endif

/****************************************************************************
 *
 * f_mutex_put
 *
 * user function to release a mutex.
 *
 * RETURNS
 *   0 - success
 *   1 - error
 *
 ***************************************************************************/

#if (!FN_CAPI_USED)
int f_mutex_put (FN_MUTEX_TYPE *mutex)
{
  return sem_incr((sdd_obj_t *)*mutex,1);
}
#endif

/****************************************************************************
 *
 * fn_gettaskID
 *
 * user function to get current task ID, valid return value must be get
 * from the current running task if its a multitask system, another case
 * this function can always returns with 1. Return value zero is not a valid
 * value.
 *
 * RETURNS
 *   task ID
 *
 ***************************************************************************/

#if (!FN_CAPI_USED)
#ifndef _FN_GETTASKID_
long fn_gettaskID(void)
{
  return (long)sc_procIdGet(NULL,SC_NO_TMO);
}
#endif
#endif

/****************************************************************************
 *
 * end of port_f.c
 *
 ***************************************************************************/
