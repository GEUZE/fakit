/****************************************************************************
 *
 *            Copyright (c) 2003 by HCC Embedded
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
 * Budapest 1132
 * Victor Hugo Utca 11-15
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/


/****************************************************************************
 *
 * fw_mutex_create
 *
 * user function to create a mutex.
 *
 * INPUTS
 *
 * mutex - where to create mutex
 *
 * RETURNS
 *
 *   0 - success
 *   1 - error
 *
 ***************************************************************************/
#include <sciopta.h>
#include <ossys/libsc.h>
#include <hcc/capi/fw_port.h>
#include <sfs/sc2hcc.h>

int fw_mutex_create (F_MUTEX_TYPE *mutex)
{
  static int cnt = 0;
  sdd_obj_t *sem;
  char name[12] = "CAPImutex00";

  name[9] = (cnt >> 4)+'0';
  name[10] = (cnt & 0xf)+'0';
  
  sem = sem_new(name,1,"/SCP_sem",SC_DEFAULT_POOL);
  if ( sem == NULL ) return 1;

  ++cnt;
  *mutex = (FN_MUTEX_TYPE)sem;
  return 0;
}

/****************************************************************************
 *
 * fw_mutex_delete
 *
 * user function to delete a mutex.
 *
 * INPUTS
 *
 * mutex - pointer to mutex to be deleted
 *
 * RETURNS
 *
 *   0 - success
 *   1 - error
 *
 ***************************************************************************/

int fw_mutex_delete (F_MUTEX_TYPE *mutex)
{
  return sem_rm((sdd_obj_t **)mutex);
}

/****************************************************************************
 *
 * fw_mutex_get
 *
 * user function to get a mutex.
 *
 * INPUTS
 *
 * mutex - pointer to mutex to be get used
 *
 * RETURNS
 *   0 - success
 *   1 - error
 *
 ***************************************************************************/

int fw_mutex_get (F_MUTEX_TYPE *mutex)
{
  return sem_decr((sdd_obj_t *)*mutex,1);
}

/****************************************************************************
 *
 * fw_mutex_put
 *
 * user function to release a mutex.
 *
 * INPUTS
 *
 * mutex - pointer to mutex to be released
 *
 * RETURNS
 *   0 - success
 *   1 - error
 *
 ***************************************************************************/

int fw_mutex_put (F_MUTEX_TYPE *mutex)
{
  return sem_incr((sdd_obj_t *)*mutex,1);
}

/****************************************************************************
 *
 * fn_gettaskID
 *
 * user function to get current task ID
 * zero return value cannot be used in this ID system.
 *
 * RETURNS
 *
 *   task ID
 *
 ***************************************************************************/

#ifndef _FN_GETTASKID_

long fn_gettaskID(void)
{
  return (long)sc_procIdGet(NULL,SC_NO_TMO);
}

#endif

/****************************************************************************
 *
 * end of fw_port.c
 *
 ***************************************************************************/

