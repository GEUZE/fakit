/****************************************************************************
 *
 *            Copyright (c) 2003-2007 by HCC Embedded
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

#include "hdd_ide.h"

#define HDDDRV_CNT 2

/* Structure definition for multiple device support */
typedef struct {
  unsigned long base;		/* base pointer */
  unsigned long select_card;	/* card select register value */
  int use;			/* driver in use */
  F_DRIVER *driver;
} t_HddDrv;

static F_DRIVER hdd_driver[HDDDRV_CNT];

/* Array of drivers */
static t_HddDrv HddDrv[HDDDRV_CNT] =
{
/* Drive 0 */
  { HDD_BASE0, 0x00e00000, 0, &hdd_driver[0] },
/* Drive 1 */
  { HDD_BASE0, 0x00f00000, 0, &hdd_driver[1] }
};


#if (!HDD16BITDATA)
static int gl_need8bitset=1;
#endif


/******************************************************************************
 *
 * fnHDDwaitBusy
 *
 * wait untile last write  operation is finished
 *
 * RETURNS
 * 0 - ok
 * 1 - any error
 *
 *****************************************************************************/

static long fnHDDwaitBusy(volatile unsigned long *State) {
volatile unsigned long dwto=HDD_TOVALUE;

   while (*State & HDD_STATE_BUSY) {
      if (!dwto) return 1;
      dwto--;
   }
   return 0;
}


/******************************************************************************
 *
 * fnHDDwaitReady
 *
 * wait until next command can be written
 *
 * RETURNS
 * 0 - ok
 * 1 - any error
 *
 *****************************************************************************/

static long fnHDDwaitReady(volatile unsigned long *State) {
volatile unsigned long dwto=HDD_TOVALUE;

   while ((*State & (HDD_STATE_BUSY|HDD_STATE_RDY))!=HDD_STATE_RDY) {
      if (!dwto) return 1;
      dwto--;
   }
   return 0;
}

/******************************************************************************
 *
 * fnHDDwaitDrq
 *
 * wait for data request
 *
 * RETURNS
 * 0 - ok
 * 1 - any error
 *
 *****************************************************************************/

static long fnHDDwaitDrq(volatile unsigned long *State) {
volatile unsigned long dwto=HDD_TOVALUE;

   while ((*State & (HDD_STATE_BUSY|HDD_STATE_DRQ))!=HDD_STATE_DRQ) {
      if (!dwto) return 1;
      dwto--;
   }

   return 0;
}

/******************************************************************************
 *
 * fnHDDsetCommand
 *
 * Write a command into flash card command register, it writes also
 * select drive
 *
 * INPUTS
 * dwcmd - command to write
 * hdd - hdd structure pointer
 *
 * RETURNS
 * 0 - ok
 * 1 - any error
 *
 *****************************************************************************/

static long fnHDDsetCommand (unsigned long dwcmd, t_HddDrv *hdd)
{
   HDD_VAL(hdd,HDD_COMMAND)=dwcmd;          /* pass the command */

   if (fnHDDwaitBusy(HDD_PTR(hdd,HDD_STATE))) {
      return 1;  /* command is not accepted */
   }

   if (fnHDDwaitDrq(HDD_PTR(hdd,HDD_STATE))) {
      return 1;   /* no data requested set */
   }


   return 0;                       /* ready */
}

/******************************************************************************
 *
 * fnHDD
 *
 * Write or Read data from flash card
 *
 * INPUTS
 * hdd - hdd structure pointer
 * data   - where to read or where to write to the data
 * length - data length of reading or writing
 * cfaddr - flash card address
 * dwcmd  - HDD_READ-for reading  HDD_WRITE-for writing
 *
 * RETURNS
 * 0 - ok
 * 1 - any error
 *
 *****************************************************************************/

static int fnHDD(t_HddDrv *hdd,void *pdata, long sector, long sectorcount, unsigned long dwcmd)
{

#if (HDD16BITDATA)
	register unsigned short *wdata=(unsigned short*)pdata;
#else
	register unsigned char *cdata=(unsigned char*)pdata;
#endif
	unsigned long length;
	unsigned long page_pos;
	long cylinder;

	volatile unsigned long *Data=HDD_PTR(hdd,HDD_DATA);
	volatile unsigned long *State=HDD_PTR(hdd,HDD_STATE);


	if (fnHDDwaitBusy(HDD_PTR(hdd,HDD_STATE)))
	{
		return HDD_ERR_BUSY_ATCYL; /* check if last write performed */
	}

	if (fnHDDwaitReady(HDD_PTR(hdd,HDD_STATE)))
	{
		return HDD_ERR_BUSY_ATCYL; /* not ready to accept new command */
	}


#if (!HDD16BITDATA)
    if (gl_need8bitset)
	{
		HDD_VAL(HDD_FEATURE)=0x01;       /* set 8 bit mode */
        HDD_VAL(HDD_SECTORCOU)=0;
        HDD_VAL(HDD_SELC)=HDD_SELCARD(p);
        HDD_VAL(HDD_COMMAND)=HDD_SETFEA;

        if (fnHDDwaitBusy(State))
		{
            return HDD_ERR_BUSY_ATCYL; /* check if last write performed */
        }

        if (HDD_VAL(HDD_STATE) & (HDD_STATE_ERR|HDD_STATE_DRQ|HDD_STATE_BUSY))
		{
            return HDD_ERR_BUSY_ATCYL; /* check if last write performed */
        }

        gl_need8bitset=0;
	}
#endif


	cylinder=(sector>>8)&0xffff;      /* /get the current cylinder (256sector / cylinder) */
	HDD_VAL(hdd,HDD_CYLINDERLO)=(cylinder&0xff)<<16;    /* xxxxcccc cccccccs sssssssb bbbbbbbb */
	HDD_VAL(hdd,HDD_CYLINDERHI)=(cylinder>>8)<<16; /* xxxxcccc cccccccs sssssssb bbbbbbbb */
	HDD_VAL(hdd,HDD_SELC)=HDD_SELCARD(hdd)+((sector&0x0f000000)>>(24-16));
	HDD_VAL(hdd,HDD_SECTORNO)=sector<<16;        /* set sector no and cou=1 */
	if (sectorcount==256) HDD_VAL(hdd,HDD_SECTORCOU)=0;
	else HDD_VAL(hdd,HDD_SECTORCOU)=sectorcount<<16;    /* set sector no and cou=1 */

	if (fnHDDsetCommand(dwcmd,hdd))
	{
		return HDD_ERR_BUSY_ATCMD; /* check if command can be started */
	}

	length=HDD_PAGE_SIZE*sectorcount;
	if (dwcmd==HDD_WRITE)
	{
		while (length)
		{
			page_pos=HDD_PAGE_SIZE>>1;
			while (page_pos)
			{
				register unsigned short data=*wdata++;
				data=MOTOWORD(data);
				*Data=((unsigned long)data)<<16;
				--page_pos;
			}

			if (length>HDD_PAGE_SIZE)
			{
				if (fnHDDwaitBusy(State)) return HDD_ERR_BUSY_ATCYL;
				if (fnHDDwaitDrq(State)) return HDD_ERR_BUSY_ATDRQ;
			}
			length-=HDD_PAGE_SIZE;
		}
	}
	else
	{
		while (length)
		{
			page_pos=HDD_PAGE_SIZE>>1;
			while (page_pos)
			{
				register unsigned short data=(unsigned short)(*Data>>16);
				data=MOTOWORD(data);
				*wdata++=data;         /* 512byte at once */
				--page_pos;
			}
			if (length>HDD_PAGE_SIZE)
			{
				if (fnHDDwaitBusy(State)) return HDD_ERR_BUSY_ATCYL;
				if (fnHDDwaitDrq(State)) return HDD_ERR_BUSY_ATDRQ;
			}
			length-=HDD_PAGE_SIZE;
		}
	}

	if (fnHDDwaitBusy(State))
	{
		return HDD_ERR_BUSY_ATCMD;
	}

	if (HDD_VAL(hdd,HDD_STATE) & HDD_STATE_ERR)
	{
		return HDD_ERR_STATE;
	}

	if (dwcmd!=HDD_ID)
	{
		if (HDD_VAL(hdd,HDD_SECTORCOU) & (0x00ff<<16))
		{
			return HDD_ERR_SECCOU;
		}
	}

	return HDD_NO_ERROR;
}

/****************************************************************************
 *
 * hdd_writesector
 *
 * this function writes one sector into flash card
 *
 * INPUTS
 *
 * driver - driver structure
 * data - data pointer
 * sector - where to write data
 *
 * RETURNS
 *
 * 0  - successful
 * -1 - card has been removed
 * other - any error
 *
 ***************************************************************************/

int hdd_writesector(F_DRIVER *driver,void *data,unsigned long sector)
{
   return fnHDD((t_HddDrv *)(driver->user_ptr),data,sector,1,HDD_WRITE);
}

/****************************************************************************
 *
 * hdd_writemultiplesector
 *
 * Writes multiple sectors
 *
 * INPUTS
 *
 * driver - driver structure
 * data - data pointer
 * sector - where to write data
 * cnt - number of sectors to write
 *
 * RETURNS
 *
 * 0  - successful
 * -1 - card has been removed
 * other - any error
 *
 ***************************************************************************/

int hdd_writemultiplesector(F_DRIVER *driver,void *data,unsigned long sector, int cnt)
{
	return fnHDD((t_HddDrv *)(driver->user_ptr),data,sector,cnt,HDD_WRITE);
}

/****************************************************************************
 *
 * hdd_readsector
 *
 * this function reads one sector from flash card
 *
 * INPUTS
 *
 * driver - driver structure
 * data - data pointer where to store data
 * sector - where to read data from
 *
 * RETURNS
 *
 * 0  - successful
 * -1 - card has been removed
 * other - any error
 *
 ***************************************************************************/

int hdd_readsector(F_DRIVER *driver,void *data,unsigned long sector)
{
	return fnHDD((t_HddDrv *)(driver->user_ptr),data,sector,1,HDD_READ);
}

/****************************************************************************
 * hdd_readmultiplesector
 * Reads multiple sectors
 *
 * INPUTS
 *
 * driver - driver structure
 * data - data pointer where to store data
 * sector - where to read data from
 * cnt - number of sectors to read
 *
 * RETURNS
 *
 * 0  - successful
 * -1 - card has been removed
 * other - any error
 *
 ***************************************************************************/

int hdd_readmultiplesector(F_DRIVER *driver,void *data,unsigned long sector, int cnt)
{
	return fnHDD((t_HddDrv *)(driver->user_ptr),data,sector,cnt,HDD_READ);
}

/****************************************************************************
 *
 * get16
 *
 * get a 16bit value always in little endian
 *
 * INPUTS
 *
 * s - data pointer where the data is
 *
 * RETURNS
 *
 * 16bit number
 *
 ***************************************************************************/

static unsigned short get16(unsigned char *s)
{
	unsigned short ret;

	ret=s[1];
	ret<<=8;
	ret|=s[0];
	return ret;
}

/****************************************************************************
 *
 * get32
 *
 * get a 32bit value always in little endian
 *
 * INPUTS
 *
 * s - data pointer where the data is
 *
 * RETURNS
 *
 * 32bit number
 *
 ***************************************************************************/

static unsigned long get32(unsigned char *s)
{
	unsigned long ret;

	ret=get16(s+2);
	ret<<=16;
	ret|=get16(s);

	return ret;
}

/****************************************************************************
 *
 * hdd_getphy
 *
 * determinate flash card physicals
 *
 * INPUTS
 *
 * driver - driver structure
 * phy - this structure has to be filled with physical information
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

static unsigned char gl_data[512]; /* this variable need for determination */

int hdd_getphy(F_DRIVER *driver, F_PHY *phy )
{
	int ret;
	ret=fnHDD((t_HddDrv *)(driver->user_ptr),gl_data,0,1,HDD_ID);
	if (ret) return ret;

	phy->number_of_cylinders=get16(gl_data+1*2);
	phy->sector_per_track=get16(gl_data+3*2);
	phy->number_of_heads=get16(gl_data+6*2);
	phy->number_of_sectors=get32(gl_data+60*2);
	phy->media_descriptor=0xf8;

	return HDD_NO_ERROR;
}


/****************************************************************************
 *
 * hdd_release
 *
 * Releases an existing driver
 *
 * INPUTS
 *
 * driver - driver structure
 *
 ***************************************************************************/

void hdd_release (F_DRIVER *driver)
{
	t_HddDrv *hdd=(t_HddDrv*)(driver->user_ptr);

	if (hdd->use) hdd->use=0;
}

/****************************************************************************
 *
 * hdd_initfunc
 *
 * this init function has to be passed for highlevel to initiate the
 * driver functions
 *
 * INPUTS
 *
 * driver_param - driver parameter
 *
 * RETURNS
 *
 * driver structure pointer
 *
 ***************************************************************************/

F_DRIVER *hdd_initfunc(unsigned long driver_param)
{
	t_HddDrv *hdd=0;
	unsigned int i;

	if (driver_param==F_AUTO_ASSIGN)
	{
		for (i=0;i<HDDDRV_CNT;i++)
		{
			if (!HddDrv[i].use)
			{
				hdd=&HddDrv[i];
				break;
			}
		}
	}
	else
	{
		if (driver_param<HDDDRV_CNT)
		{
			hdd=&HddDrv[driver_param];
		}
		else
		{
			return 0;
		}
	}

	if (!hdd) return 0;
	if (hdd->use) return 0;

#ifdef HCC_HW
	/* Initialize CS to have CFC_BASE0 available */
	HDD_CSAR=0x01000001;
	HDD_CSOR=0xFFFF040c; /* 32 bit,external TA */
#endif

	HDD_VAL(hdd,HDD_SELC)=HDD_SELCARD(hdd);

	if (fnHDD(hdd,gl_data,0,1,HDD_ID))
	{
		return 0;
	}

	/* check if lba is supported */
	{
		unsigned short lbasup=get16(gl_data+49*2);

		if ((lbasup&(1<<9))==0)
		{
			return 0;
		}
	}

	(void)memset(hdd->driver,0,sizeof(F_DRIVER));

	hdd->driver->readsector=hdd_readsector;
	hdd->driver->writesector=hdd_writesector;
	hdd->driver->readmultiplesector=hdd_readmultiplesector;
	hdd->driver->writemultiplesector=hdd_writemultiplesector;
	hdd->driver->getphy=hdd_getphy;
	hdd->driver->release=hdd_release;
	hdd->driver->user_ptr=hdd;
	hdd->use=1;

	return hdd->driver;
}

/******************************************************************************
 *
 *  End of hdd_ide.c
 *
 *****************************************************************************/

