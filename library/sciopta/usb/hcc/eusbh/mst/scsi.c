/***************************************************************************
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
 * Vaci Ut 110
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/
#include <sciopta.h>
//#include <dev/sem.h>

#include "fatfs_cfg.h"
#include <hcc/eusbh/mst/scsi.h>
#include <hcc/eusbh/usb_driver/usb_host.h>

/******************************************************************************
 Macro definitions
 *****************************************************************************/

/* Mandatory requests for all usb mass-storage devices. */
#define SCSI_CMD_INQUIRY            0x12u
#define SCSI_CMD_REQUEST_SENSE      0x03u
#define SCSI_CMD_SEND_DIAGNOSTIC    0x1du
#define SCSI_CMD_TEST_UNIT_READY    0x00u

/* Requests for direct access devices. */
#define SCSI_CMD_FORMAT_UNIT        0x04u
#define SCSI_CMD_READ_10            0x28u
#define SCSI_CMD_READ_12            0xa8u
#define SCSI_CMD_READ_CAPACITY      0x25u
#define SCSI_CMD_RELEASE            0x17u
#define SCSI_CMD_RESERVE            0x16u
#define SCSI_CMD_WRITE_10           0x2au
#define SCSI_CMD_WRITE_12           0xaau
#define SCSI_CMD_START_STOP         0x1bu
#define SCSI_CMD_TOCPMAATIP         0x43u

/* Sense keys fot direct access devices: */
#define SCSI_SK_NO_SENSE          0x00	/* no problem */
#define SCSI_SK_RECOVERED_ERROR   0x01	/* recovered error */
#define SCSI_SK_NOT_READY         0x02
#define SCSI_SK_MEDIUM_ERROR      0x03	/* unrecovered error */
#define SCSI_SK_HARDWARE_ERROR    0x04	/* unrecovered error */
#define SCSI_SK_ILLEGAL_REQUEST   0x05	/* illegal request */
#define SCSI_SK_UNIT_ATTENTION    0x06


#define SCSI_SENSE_NONE					0x000000
#define SCSI_SENSE_NOT_PRESENT			0x023a00
#define SCSI_SENSE_MEDIA_CHANGED		0x062800
#define SCSI_SENSE_WRITE_PROTECT		0x072700

#define GET_SENSE_CODE(key,asc,ascq)	((((hcc_u32)key)<<16)|(((hcc_u32)asc)<<8)|((hcc_u32)ascq))

#define SET_LUNID(dev,lun)				(((hcc_u16)dev<<8)|lun)
#define GET_DEV(v)						((hcc_u8)(v>>8))
#define GET_LUN(v)						((hcc_u8)(v&0xff))

#undef kprintf
//->#define kprintf(...)


/******************************************************************************
 Function pre-definitions
 *****************************************************************************/
static int scsi_cmd_inquiry(hcc_u8 dev, hcc_u8 lun);
static int scsi_cmd_test_unit_ready(hcc_u8 dev, hcc_u8 lun);
static int scsi_cmd_read_capacity(hcc_u8 dev, hcc_u8 lun);
static int scsi_cmd_request_sense(hcc_u8 dev, hcc_u8 lun);


/******************************************************************************
 Local variables
 *****************************************************************************/
typedef struct sem_s {
  int cnt;
  sc_pid_t owner;
} sem_t;

typedef struct {
  rw_func_t *transfer;
  void *dev_hdl;
  t_lun_info lun[SCSI_MAX_LUN];
  volatile sem_t sem;
} t_scsi_dsc;

static t_scsi_dsc scsi_dsc[SCSI_MAX_DEVICE];		/* SCSI descriptor */
static hcc_u16 lunid[SCSI_MAX_DEVICE*SCSI_MAX_LUN];	/* unit identification */


/******************************************************************************
 Function declaration
 *****************************************************************************/

static void sem_decr(volatile sem_t *sem)
{
  sc_cpuflags_t oldmask;
  LOCK(oldmask);
  if ( sem->owner == sc_procIdGet(NULL, SC_NO_TMO)){
    --sem->cnt;
    UNLOCK(oldmask);
    return;
  }
  while( sem->owner ){
    UNLOCK(oldmask);
    sc_sleep(2);
    LOCK(oldmask);
  }
  --sem->cnt;
  sem->owner = sc_procIdGet(NULL, SC_NO_TMO);
  UNLOCK(oldmask);
}

static void sem_incr(volatile sem_t *sem)
{
  sc_cpuflags_t oldmask;
  LOCK(oldmask);
  ++sem->cnt;
  if ( sem->cnt > 0 ){
    sem->owner = 0;
  }
  UNLOCK(oldmask);
}


/******************************************************************************
 scsi_init

 Description:
   Inicializes the SCSI layer. Shall be called once before any other SCSI
   function.
 *****************************************************************************/
int scsi_init(void)
{
  _memset(&scsi_dsc,0,sizeof(scsi_dsc));	/* reset descriptor area */
  _memset(lunid,0xff,sizeof(lunid));		/* set lunid-s to invalid */
  return 0;
}


/******************************************************************************
 scsi_open_device

 Description:
   Add a new SCSI device. All logical units of the device will be initialized
   too.
 *****************************************************************************/
int scsi_open_device(hcc_u8 dev, hcc_u8 nlun, rw_func_t *f, void *dev_hdl)
{
  t_scsi_dsc *dsc=scsi_dsc+dev;		/* get device descriptor position */
  int rc=SCSI_SUCCESS;
  int i,j;
  int pstate;

  dsc->transfer=f;					/* save access function */
  dsc->dev_hdl=dev_hdl;				/* save device handle */

  for (i=0;i<SCSI_MAX_LUN;i++)
  {
    pstate=dsc->lun[i].pstate;						/* save previous state */
    _memset(&dsc->lun[i],0,sizeof(t_lun_info));		/* reset LUN variables */
    dsc->lun[i].pstate=pstate;						/* restore previous state to be able to indicate change */
    dsc->lun[i].state=0;							/* set current state */
  }
  
  dsc->sem.owner = 0;
  dsc->sem.cnt = 1;

  j=0;
  for (i=0;i<nlun;i++)
  {
    for (;j<SCSI_MAX_DEVICE*SCSI_MAX_LUN && lunid[j]!=(hcc_u16)-1;j++)
      ;
    if (j<SCSI_MAX_DEVICE*SCSI_MAX_LUN)
    {
      lunid[j]=SET_LUNID(dev,i);	/* set dev/lun identification */
      rc=scsi_cmd_inquiry(dev,i);	/* read LUN information */
      if (rc!=SCSI_SUCCESS) break;
    }
    else rc=SCSI_ERR_NLUN;
  }
  return rc;
}


/******************************************************************************
 scsi_close_device

 Description:
   Remove an SCSI device.
 *****************************************************************************/
int scsi_close_device(hcc_u8 dev)
{
  t_scsi_dsc *dsc=scsi_dsc+dev;
  int i;
  
  for (i=0;i<SCSI_MAX_DEVICE*SCSI_MAX_LUN;i++)
  {							/* set all LUN ID-s belonging to this device to invalid */
    if (GET_DEV(lunid[i])==dev) 
    {
      scsi_dsc[dev].lun[GET_LUN(lunid[i])].state=0;
      lunid[i]=(hcc_u16)-1;
    }
  }
  dsc->sem.owner = 0;

  return 0;
}


/******************************************************************************
 scsi_cmd_inquiry

 Description:
   Issue inquiry command on the communication channel, and set LUN status
   to reflect reported values.
 *****************************************************************************/
static int scsi_cmd_inquiry (hcc_u8 dev, hcc_u8 lun)
{
  t_scsi_dsc *dsc=scsi_dsc+dev;
  rw_func_t *transfer=dsc->transfer;
  int err;
  int rc=SCSI_SUCCESS;
  union {
    hcc_u8 cmd[6];
    hcc_u8 resp[36];
  } data;

  sem_decr(&dsc->sem);

  /* Build SCSI command block */
  data.cmd[0]=SCSI_CMD_INQUIRY;
  data.cmd[1]=lun<<5; /* EVPD is not supported. */
  data.cmd[2]=0;      /* page code is allways 0 (because EVPD=0) */
  data.cmd[3]=0;      /* reserved */
  data.cmd[4]=36;     /* allocationlength is 36 */
  data.cmd[5]=0;      /* Control (flag=0, link=0) */

  /* Start transfer. */
  err=transfer(dsc->dev_hdl, 6, &data.cmd[0], DIR_IN, 36, &data.resp[0]);
  if (err!=ERR_IO_CHECK && err!=ERR_IO_REMOVED)
  {
    /* Process returned data. */
    /* We only process the PDT field, and identification strings. */
    dsc->lun[lun].pdt=(char)(data.resp[0] & 0x1f);
    _memcpy(dsc->lun[lun].vendor, data.resp+8, 8);
    _memcpy(dsc->lun[lun].prod_id, data.resp+16, 16);
    _memcpy(dsc->lun[lun].rev, data.resp+32, 4);
  }
  else
  {
    rc=SCSI_ERR_TRANSFER;
  }
  sem_incr(&dsc->sem);
  return rc;
}


/******************************************************************************
 scsi_cmd_read_capacity

 Description:
   Issue read_capacity request to the specified LUN.
 *****************************************************************************/
static int scsi_cmd_read_capacity (hcc_u8 dev, hcc_u8 lun)
{
  t_scsi_dsc *dsc=scsi_dsc+dev;
  rw_func_t *transfer=dsc->transfer;
  struct {
    hcc_u8 cmd[10];
    hcc_u8 resp[8];
  } data;
  int err;
  int retry=0;
  int rc=SCSI_SUCCESS;

  dsc->lun[lun].no_of_sectors=0;
  dsc->lun[lun].sector_size=0;

  /* Build SCSI command block. */
  data.cmd[0]=SCSI_CMD_READ_CAPACITY;
  data.cmd[1]=lun<<5;        /* No relative addressing. */
  data.cmd[2]=data.cmd[3]=data.cmd[4]=data.cmd[5]=0; /* LBA=0 (because PMI=0) */
  data.cmd[6]=data.cmd[7]=0; /* reserved */
  data.cmd[8]=0;             /* PMI =0 */
  data.cmd[9]=0;             /* Control (flag=0, link=0) */

  /* Send command block. */
  for (;;)
  {
    err=transfer(dsc->dev_hdl, 10, &data.cmd[0], DIR_IN, 8, &data.resp[0]);
    if (err)
    {
      if (err==ERR_IO_CHECK) /* Request status from device. */
      {
        if (scsi_cmd_request_sense(dev,lun)==0)
        {
          if (++retry<=RETRY_COUNT)
          {
            usbh_delay(RETRY_WAIT_MS);  /* retry */
            continue;
          }
        }
      }

      rc=SCSI_ERR_TRANSFER;
    }
    break;
  }

  if (rc==SCSI_SUCCESS)
  {
    /* Process returned data. */
    dsc->lun[lun].no_of_sectors=RD_BE32(data.resp);
    dsc->lun[lun].sector_size=RD_BE32(data.resp+4);
  }

  return rc;
}


/******************************************************************************
 scsi_cmd_test_unit_ready

 Description:
   Query the status of the device (see if it is ready to answer the next
   request).
 *****************************************************************************/
static int scsi_cmd_test_unit_ready (hcc_u8 dev, hcc_u8 lun)
{
  t_scsi_dsc *dsc=scsi_dsc+dev;
  rw_func_t *transfer=dsc->transfer;
  hcc_u8 cmd[6];
  int err;
  int retry=0;
  int rc=SCSI_SUCCESS;

  /* Build SCSI command block. */
  cmd[0]=SCSI_CMD_TEST_UNIT_READY;
  cmd[1]=lun<<5;
  cmd[2]=cmd[3]=cmd[4]=0; /* reserved */
  cmd[5]=0;               /* Control (flag=0, link=0) */

  for (;;)
  {
    sem_decr(&dsc->sem);

    err=transfer(dsc->dev_hdl, 6, &cmd[0], DIR_NONE, 0, 0);

    sem_incr(&dsc->sem);

    if (err)
    {
      if (err==ERR_IO_CHECK) /* Request status from device. */
      {
        if (scsi_cmd_request_sense(dev,lun)==0)
        {
          if (++retry<=RETRY_COUNT)
          {
            usbh_delay(RETRY_WAIT_MS);  /* retry */
            continue;
          }
        }
      }

      rc=SCSI_ERR_TRANSFER;
    }
    break;
  }
  return rc;
}


/******************************************************************************
 scsi_cmd_request_sense

 Description:
   Get detailed error information from device.
 *****************************************************************************/
static int scsi_cmd_request_sense (hcc_u8 dev, hcc_u8 lun)
{
  t_scsi_dsc *dsc=scsi_dsc+dev;
  rw_func_t *transfer=dsc->transfer;
  union {
    hcc_u8 cmd[6];
    hcc_u8 resp[18];
  } data;
  int err;
  int rc=SCSI_SUCCESS;
  int n;
  hcc_u32 sense_code,psense_code;

  psense_code=(hcc_u32)-1;

  sem_decr(&dsc->sem);

  for (n=0;rc == SCSI_SUCCESS;n++)/* repeat while new sense codes available */
  {
    /* Build SCSI command block. */
    data.cmd[0]=SCSI_CMD_REQUEST_SENSE;
    data.cmd[1]=lun<<5;
    data.cmd[2]=data.cmd[3]=0;	/* reserved */
    data.cmd[4]=18;				/* Allocation length (only read basic info)*/
    data.cmd[5]=0;				/* Control (flag=0, link=0) */
  
    /* Send command block. */
    err=transfer(dsc->dev_hdl, 6, &data.cmd[0], DIR_IN, 18, &data.resp[0]);
    if (err!=ERR_IO_CHECK && err!=ERR_IO_REMOVED)
    {
      sense_code=GET_SENSE_CODE(data.resp[2],data.resp[12],data.resp[13]);
      if (sense_code!=psense_code)	/* if previous sense code is different than the current one */
      {
        if (sense_code || !n)		/* if no sense and first iteration */
        {
          int state=SCSI_ST_CONNECTED;
  
          if (data.resp[0]&0x80) dsc->lun[lun].sense_info=RD_BE32(&data.resp[3]);
          if (sense_code==SCSI_SENSE_NOT_PRESENT) state=0;
          else if (sense_code==SCSI_SENSE_MEDIA_CHANGED) state|=SCSI_ST_CHANGED;
          else if (sense_code==SCSI_SENSE_WRITE_PROTECT) state|=SCSI_ST_WRPROTECT;
          if (state)				/* if connected state */
          {
            /* set changed flag if previous state was connected and current state is disconnected */
            if (dsc->lun[lun].state==0 && dsc->lun[lun].pstate) state|=SCSI_ST_CHANGED;
            if (state&SCSI_ST_CHANGED) dsc->lun[lun].state=state;
                                  else dsc->lun[lun].state|=state;	/* if not changed then add new flag - WP */
          }
          else 
          {
            dsc->lun[lun].state=0;	/* set disconnected state */
          }
        }
        if (sense_code==0) break;
      }
      else break;
      psense_code=sense_code;
    }
    else rc=SCSI_ERR_TRANSFER;
  }
  sem_incr(&dsc->sem);

  return rc;
}


/******************************************************************************
 scsi_cmd_read

 Description:
   Read blocks to memory.
 *****************************************************************************/
int scsi_cmd_read (hcc_u8 unit, hcc_u32 lba, hcc_u32 length, void *buffer)
{
  hcc_u16 id=lunid[unit];
  t_scsi_dsc *dsc;
  rw_func_t *transfer;
  hcc_u8 cmd[12];
  hcc_u8 dev,lun;
  int err;
  int retry=0;
  int rc=SCSI_SUCCESS;

  if (id!=(hcc_u16)-1)
  {
    dev=GET_DEV(id);
    lun=GET_LUN(id);
    dsc=scsi_dsc+dev;
    transfer=dsc->transfer;

    if (transfer)
    {
      if ((dsc->lun[lun].state&(SCSI_ST_CONNECTED|SCSI_ST_CHANGED))==SCSI_ST_CONNECTED)
      {
        int r12=(length>0xffff);

        /* Build SCSI command block. */
        cmd[1]=lun<<5;
        WR_BE32(cmd+2,lba)
          ;
        if (r12)
        {
          cmd[0]=SCSI_CMD_READ_12;
          WR_BE32(cmd+6,length)
            ;
          cmd[10]=0;       /* reserved */
          cmd[11]=0;       /* Control (flag=0, link=0) */
        }
        else
        {
          cmd[0]=SCSI_CMD_READ_10;
          cmd[6]=0; /* reserved */
          WR_BE16(cmd+7,length)
            ;
          cmd[9]=0;        /* Control (flag=0, link=0) */
        }

        for (;;)
        {

	  sem_decr(&dsc->sem);

          err=transfer(dsc->dev_hdl, r12?12:10, cmd, DIR_IN, length * dsc->lun[lun].sector_size, buffer);

	  sem_incr(&dsc->sem);  

          if (err)
          {
            if (err==ERR_IO_CHECK) /* Request status from device. */
            {
              rc=scsi_cmd_request_sense(dev,lun);
              if (rc==SCSI_SUCCESS)
              {
                if (++retry<=RETRY_COUNT)
                {
                  usbh_delay(RETRY_WAIT_MS);  /* retry */
                  continue;
                }
              }
            }
            else rc=SCSI_ERR_TRANSFER;
          }
          break;
        }
      }
      else rc=SCSI_ERR_UNIT;  
    }
    else rc=SCSI_ERR_TRANSFER;
  }
  else rc=SCSI_ERR_UNIT;
  return rc;
}


/******************************************************************************
 scsi_cmd_write

 Description:
   Write blocks to media.
 *****************************************************************************/
int scsi_cmd_write (hcc_u8 unit, hcc_u32 lba, hcc_u32 length, void *buffer)
{
  hcc_u16 id=lunid[unit];
  t_scsi_dsc *dsc;
  rw_func_t *transfer;
  hcc_u8 cmd[12];
  hcc_u8 dev,lun;
  int err;
  int retry=0;
  int rc=SCSI_SUCCESS;

  
  if (id!=(hcc_u16)-1)
  {
    dev=GET_DEV(id);
    lun=GET_LUN(id);
    dsc=scsi_dsc+dev;
    transfer=dsc->transfer;

    if (transfer)
    {
      if ((dsc->lun[lun].state&(SCSI_ST_CONNECTED|SCSI_ST_WRPROTECT|SCSI_ST_CHANGED))==SCSI_ST_CONNECTED)
      {
        int w12=(length>0xffff);
      
        /* Build SCSI command block. */
        cmd[1]=lun<<5;
        WR_BE32(cmd+2,lba)
          ;
        if (w12)
        {
          cmd[0]=SCSI_CMD_WRITE_12;
          WR_BE32(cmd+6,length)
            ;
          cmd[10]=0;       /* reserved */
          cmd[11]=0;       /* Control (flag=0, link=0) */
        }
        else
        {
          cmd[0]=SCSI_CMD_WRITE_10;
          cmd[6]=0; /* reserved */
          WR_BE16(cmd+7,length)
            ;
          cmd[9]=0;        /* Control (flag=0, link=0) */
        }
      
        for (;;)
        {
	  sem_decr(&dsc->sem);

          err=transfer(dsc->dev_hdl, w12?12:10, cmd, DIR_OUT, length * dsc->lun[lun].sector_size, buffer);

	  sem_incr(&dsc->sem);  

          if (err)
          {
            if (err==ERR_IO_CHECK) /* Request status from device. */
            {
              rc=scsi_cmd_request_sense(dev,lun);
              if (rc==SCSI_SUCCESS)
              {
                if (++retry<=RETRY_COUNT)
                {
                  usbh_delay(RETRY_WAIT_MS);  /* retry */
                  continue;
                }
              }
            }
            else rc=SCSI_ERR_TRANSFER;
          }
          break;
        }
      }
      else rc=SCSI_ERR_UNIT;
            
    }
    else rc=SCSI_ERR_TRANSFER;
  }
  else rc=SCSI_ERR_UNIT;

  return rc;
}


/******************************************************************************
 scsi_test_unit_ready

 Description:
   Check device status.
 *****************************************************************************/
int scsi_get_unit_state (hcc_u8 unit)
{
  hcc_u16 id=lunid[unit];
  int state=0;

  if (id!=(hcc_u16)-1)
  {
    hcc_u8 dev,lun;
    t_scsi_dsc *dsc;
    int rc;
    
    dev=GET_DEV(id);
    lun=GET_LUN(id);  
    dsc=scsi_dsc+dev;
    
    rc=scsi_cmd_test_unit_ready(dev,lun);
    if (rc==SCSI_SUCCESS)
    {
      if (dsc->lun[lun].state==0)
      {
        rc=scsi_cmd_request_sense(dev,lun);
      }
      if (rc==SCSI_SUCCESS)
      {
        if (dsc->lun[lun].state&SCSI_ST_CHANGED || (dsc->lun[lun].pstate==0 && dsc->lun[lun].state))
        {
          rc=scsi_cmd_read_capacity(dev,lun);
        }
      }
    }
    if (rc) dsc->lun[lun].state=0;

    state=dsc->lun[lun].state;
    dsc->lun[lun].state&=~(SCSI_ST_CHANGED);
    dsc->lun[lun].pstate=dsc->lun[lun].state;
  }
  
  return state;
}


/******************************************************************************
 scsi_get_lun_info

 Description:
   Return structure with LUN specific information.
 *****************************************************************************/
t_lun_info *scsi_get_lun_info (hcc_u8 unit)
{
  hcc_u16 id=lunid[unit];
  hcc_u8 dev,lun;
  t_lun_info *rc=NULL;
  
  if (id!=(hcc_u16)-1)
  {
    dev=GET_DEV(id);
    lun=GET_LUN(id);
    if (scsi_cmd_test_unit_ready(dev,lun)==0)
    {
      rc=&scsi_dsc[dev].lun[lun];
    }
  }

  return rc;
}

