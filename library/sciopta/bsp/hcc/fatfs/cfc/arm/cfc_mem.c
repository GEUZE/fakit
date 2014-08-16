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

#include "cfc_mem.h"

#define CFCDRV_CNT 2

/* Structure definition for multiple device support */
typedef struct {
  unsigned long base;		/* base pointer */
  unsigned short select_card;	/* card select register value */
  int use;			/* driver in use */

  /* int state;		*/	/* actual state of CFC */
  int changed;			/* changed */

  volatile unsigned short *cdch_reg;	/* CDCH access register */
  unsigned long ch_bit;			/* card detect bit */
  unsigned long cd_bit;			/* card change bit */
  F_DRIVER *driver;
} t_CfcDrv;


static F_DRIVER cfc_drivers[CFCDRV_CNT];

/* Array of drivers */
static t_CfcDrv CfcDrv[CFCDRV_CNT] =
{
/* Drive 0 */
  { CFC_BASE0, 0x00e0, 0, /*0,*/ 1,
    CFC_CPLDSTATE0, CFC_CPLDSTATE_CDCH0, CFC_CPLDSTATE_CFCD0, &cfc_drivers[0] },
/* Drive 1 */
  { CFC_BASE0, 0x00f0, 0, /*0,*/ 1,
    CFC_CPLDSTATE0, CFC_CPLDSTATE_CDCH0, CFC_CPLDSTATE_CFCD0, &cfc_drivers[0] }
};


static unsigned char buf_align[F_DEF_SECTOR_SIZE];

/******************************************************************************
 *
 * fnCFCwaitBusy
 *
 * wait untile last write  operation is finished
 *
 * RETURNS
 * 0 - ok
 * 1 - any error
 *
 *****************************************************************************/
static long fnCFCwaitBusy(const volatile unsigned short *StateSelc) 
{
	volatile unsigned long dwto=CFC_TOVALUE;

	while (((*StateSelc)>>8) & CFC_STATE_BUSY) 
	{
		if (!dwto) return 1;
		dwto--;
	}
	return 0;
}

/****************************************************************************
 *
 * cfc_getstatus
 *
 * get status of flash card, missing or/and removed,changed
 *
 * RETURNS
 *
 * F_ST_xxx code for high level
 *
 ***************************************************************************/

/*lint -e{818} driver can be const */
static long cfc_getstatus(F_DRIVER *driver)
{
	t_CfcDrv *cfc=(t_CfcDrv *)(driver->user_ptr);
	long state=0;
	unsigned long cpldstate;

	cpldstate=*(cfc->cdch_reg);

	if (!(cpldstate & (cfc->cd_bit))) state|=F_ST_MISSING;

	if (cpldstate & (cfc->ch_bit)) cfc->changed=1;

	if (cfc->changed)
	{
		state|=F_ST_CHANGED;

		if (!(state&F_ST_MISSING))
		{
			unsigned long dwto=CFC_TOVALUE;

			while (((CFC_VAL(CFC_STATESELC))>>8) & CFC_STATE_BUSY)
			{
				if (!dwto)
				{
					return state;
				}
				dwto--;
			}

			cfc->changed=0; /* clear only if its in */
		}
	}
	return state;
}


/******************************************************************************
 *
 * fnCFCwaitReady
 *
 * wait until next command can be written
 *
 * RETURNS
 * 0 - ok
 * 1 - any error
 *
 *****************************************************************************/

static long fnCFCwaitReady(const volatile unsigned short *StateSelc) 
{
	volatile unsigned long dwto=CFC_TOVALUE;

	while ((((*StateSelc)>>8) & (CFC_STATE_BUSY|CFC_STATE_RDY))!=CFC_STATE_RDY) 
	{
		if (!dwto) return 1;
		dwto--;
	}
	return 0;
}

/******************************************************************************
 *
 * fnCFCwaitDrq
 *
 * wait for data request
 *
 * RETURNS
 * 0 - ok
 * 1 - any error
 *
 *****************************************************************************/

static long fnCFCwaitDrq(const volatile unsigned short *StateSelc) 
{
	volatile unsigned long dwto=CFC_TOVALUE;

	while ((((*StateSelc)>>8) & (CFC_STATE_BUSY|CFC_STATE_DRQ))!=CFC_STATE_DRQ) 
	{
		if (!dwto) return 1;
		dwto--;
	}

	return 0;
}

/******************************************************************************
 *
 * fnCFCsetCommand
 *
 * Write a command into flash card command register, it writes also
 * select drive
 *
 * INPUTS
 * dwcmd - command to write
 * p - user_ptr
 *
 * RETURNS
 * 0 - ok
 * 1 - any error
 *
 *****************************************************************************/

static long fnCFCsetCommand (const t_CfcDrv *cfc,unsigned long dwcmd, unsigned char top4bit)
{

	CFC_VAL(CFC_CMDSELC)=(((unsigned char)dwcmd)<<8)+CFC_SELCARD(cfc);    /* command + select card */

	if (fnCFCwaitBusy(CFC_PTR(CFC_STATESELC)))
	{
		return 1;  /* command is not accepted */
	}

	if (fnCFCwaitDrq(CFC_PTR(CFC_STATESELC)))
	{
		return 1;   /* no data requested set */
	}

	return 0;                       /* ready */
}

/******************************************************************************
 *
 * fnCFC
 *
 * Write or Read data from flash card
 *
 * INPUTS
 * data   - where to read or where to write to the data
 * length - data length of reading or writing
 * cfaddr - flash card address
 * dwcmd  - CFC_READ-for reading  CFC_WRITE-for writing
 * p - user_ptr
 *
 * RETURNS
 * 0 - ok
 * 1 - any error
 *
 *****************************************************************************/

static int fnCFC(F_DRIVER *driver,void *pdata, unsigned long sector, unsigned long sectorcount, unsigned long dwcmd)
{
	t_CfcDrv *cfc=(t_CfcDrv *)(driver->user_ptr);

	unsigned short *wdata=(unsigned short*)pdata;
	unsigned long length;
	unsigned long page_pos;
	unsigned long cylinder;

	volatile unsigned short *Data;
	volatile unsigned short *StateSelc=CFC_PTR(CFC_STATESELC);

	if (cfc_getstatus(driver)&F_ST_MISSING) return CFC_ERR_NOTPLUGGED; /* check if FlashCard is there */

	if (fnCFCwaitBusy(CFC_PTR(CFC_STATESELC)))
	{
		return CFC_ERR_BUSY_ATCYL; /* check if last write performed */
	}

	if (fnCFCwaitReady(CFC_PTR(CFC_STATESELC)))
	{
		return CFC_ERR_BUSY_ATCYL; /* not ready to accept new command */
	}


	cylinder=(sector>>8)&0xffff;      /* get the current cylinder (256sector / cylinder) */
	CFC_VAL(CFC_CYLINDERHILO)=(unsigned short)cylinder;
	if (sectorcount==256) CFC_VAL(CFC_SECTORNOCOU)=(unsigned short)((sector<<8)+0UL);
	else CFC_VAL(CFC_SECTORNOCOU)=(unsigned short)((sector<<8)+sectorcount); /* set sector no and cou=count */

	if (fnCFCsetCommand(cfc,dwcmd,(unsigned char)((sector>>24)&0xf)))
	{
		return CFC_ERR_BUSY_ATCMD; /* check if command can be started */
	}

	length=CFC_PAGE_SIZE*sectorcount;
	if (dwcmd==CFC_WRITE)
	{
		while (length)
		{
			Data=CFC_PTR(CFC_DATABASE);
			page_pos=CFC_PAGE_SIZE>>1;
			while (page_pos)
			{
				register unsigned short data=*wdata++;
				data=MOTOWORD(data);
				*Data=data;
				--page_pos;
				++Data;
			}
			if (length>CFC_PAGE_SIZE)
			{
				if (fnCFCwaitBusy(StateSelc)) return CFC_ERR_BUSY_ATCYL;
				if (fnCFCwaitDrq(StateSelc)) return CFC_ERR_BUSY_ATDRQ;
			}
			length-=CFC_PAGE_SIZE;
		}
	}
	else
	{
		while (length)
		{
			Data=CFC_PTR(CFC_DATABASE);
			page_pos=CFC_PAGE_SIZE>>1;
			while (page_pos)
			{
				register unsigned short data=(unsigned short)(*Data);
				data=MOTOWORD(data);
				*wdata++=data;         /* F_DEF_SECTOR_SIZEbyte at once */
				--page_pos;
				++Data;
			}
			if (length>CFC_PAGE_SIZE)
			{
				if (fnCFCwaitBusy(StateSelc)) return CFC_ERR_BUSY_ATCYL;
				if (fnCFCwaitDrq(StateSelc)) return CFC_ERR_BUSY_ATDRQ;
			}
			length-=CFC_PAGE_SIZE;
		}
	}

	if (fnCFCwaitBusy(StateSelc))
	{
		return CFC_ERR_BUSY_ATCMD;
	}

	if (((CFC_VAL(CFC_STATESELC))>>8) & CFC_STATE_ERR) {
		return CFC_ERR_STATE;
	}

	if (dwcmd!=CFC_ID)
	{
		if (CFC_VAL(CFC_SECTORNOCOU) & (0x00ff))
		{
			return CFC_ERR_SECCOU;
		}
	}

	if (cfc_getstatus(driver)&F_ST_MISSING) return CFC_ERR_NOTPLUGGED; /* check if FlashCard is there */

	return CFC_NO_ERROR;
}


/****************************************************************************
 *
 * cfc_writesector
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

static int cfc_writesector(F_DRIVER *driver,void *data,unsigned long sector)
{
	int rc;

	if ((unsigned long)data&3)	/* unaligned data */
	{
		(void)_memcpy(buf_align,data,F_DEF_SECTOR_SIZE);
		rc=fnCFC(driver,buf_align,sector,1,CFC_WRITE);
	}
	else
	{
		rc=fnCFC(driver,data,sector,1,CFC_WRITE);
	}
	return rc;
}


/****************************************************************************
 * cfc_writemultiplesector
 * Writes multiple sectors
 *
 * INPUT - same as cfc_writesector
 *         cnt - number of sectors to write
 ***************************************************************************/

static int cfc_writemultiplesector(F_DRIVER *driver,void *data,unsigned long sector, int cnt)
{
	int rc=0;

	if ((unsigned long)data&3)
	{
		char *bdata=data;
		while (cnt--)
		{
			(void)_memcpy(buf_align,bdata,F_DEF_SECTOR_SIZE);
			rc=fnCFC(driver,buf_align,sector,1,CFC_WRITE);
			if (rc) break;

			bdata+=F_DEF_SECTOR_SIZE;
			++sector;
		}
	}
	else
	{
		rc=fnCFC(driver,data,sector,(unsigned long)cnt,CFC_WRITE);
	}

	return rc;
}


/****************************************************************************
 *
 * cfc_readsector
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

static int cfc_readsector(F_DRIVER *driver,void *data,unsigned long sector)
{
	int rc;

	if ((unsigned long)data&3)
	{
		rc=fnCFC(driver,buf_align,sector,1,CFC_READ);
		(void)_memcpy(data,buf_align,F_DEF_SECTOR_SIZE);
	}
	else
	{
		rc=fnCFC(driver,data,sector,1,CFC_READ);
	}

	return rc;
}


/****************************************************************************
 * cfc_readmultiplesector
 * Reads multiple sectors
 *
 * INPUT - same as cfc_readsector
 *         cnt - number of sectors to read
 ***************************************************************************/

static int cfc_readmultiplesector(F_DRIVER *driver,void *data,unsigned long sector, int cnt)
{
	int rc=0;

	if ((unsigned long)data&3)
	{
		char *bdata=data;
		while (cnt--)
		{
			rc=fnCFC(driver,buf_align,sector,1,CFC_READ);
			if (rc) break;
			(void)_memcpy(bdata,buf_align,F_DEF_SECTOR_SIZE);
			bdata+=F_DEF_SECTOR_SIZE;
			++sector;
		}
	}
	else
	{
		rc=fnCFC(driver,data,sector,(unsigned long)cnt,CFC_READ);
	}

	return rc;
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

static unsigned short get16(const unsigned char *s) 
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

static unsigned long get32(const unsigned char *s) 
{
	unsigned long ret;

	ret=get16(s+2);
	ret<<=16;
	ret|=get16(s);
	return ret;
}

/****************************************************************************
 *
 * cfc_getphy
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

static int cfc_getphy(F_DRIVER *driver,F_PHY *phy)
{
	int ret;
	ret=fnCFC(driver,buf_align,0,1,CFC_ID);

	if (ret) return ret;

	phy->number_of_cylinders=get16(buf_align+1*2);
	phy->sector_per_track=get16(buf_align+3*2);
	phy->number_of_heads=get16(buf_align+6*2);
	phy->number_of_sectors=get32(buf_align+60*2);
	phy->media_descriptor=0xf0;

	return CFC_NO_ERROR;
}


/****************************************************************************
 *
 * cfc_release
 *
 * Deletes an existing driver
 *
 * INPUTS
 *
 * driver - driver structure
 *
 ***************************************************************************/

/*lint -e{818} driver can be const */
static void cfc_release (F_DRIVER *driver)
{
	t_CfcDrv *cfc=(t_CfcDrv *)(driver->user_ptr);

	if (cfc->use) cfc->use=0;
}

/****************************************************************************
 *
 * cfc_initfunc
 *
 * this init function has to be passed for highlevel to initiate the
 * driver functions
 *
 * INPUTS
 *
 * func - structure, where to fill functions
 * p - user_ptr
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

F_DRIVER *cfc_initfunc(unsigned long driver_param)
{
	t_CfcDrv *cfc=0;

	if (driver_param==F_AUTO_ASSIGN)
	{
		int i;
		for (i=0;i<CFCDRV_CNT;i++)
		{
			if (!CfcDrv[i].use)
			{
				cfc=&CfcDrv[i];
				break;
			}
		}
	}
	else
	{
		if (driver_param<CFCDRV_CNT)
		{
			cfc=&CfcDrv[driver_param];
		}
		else
		{
			return 0;
		}
	}

	if (!cfc) return 0;
	if (cfc->use) return 0;

#ifdef HCC_HW
	/* Initialize CS to have CFC_BASE0 available */
	CFC_CSOPTB=0x00000001;
	CFC_CSOPTA=0xffff0004;
	CFC_CSBASE=0x01000251;

	CFC_CPLDCMD2=0x0001;
	CFC_CPLDCMD1=0x0003;
	CFC_CPLDCMD2=0x0000;
#endif

	(void)memset(cfc->driver,0,sizeof(F_DRIVER));

	cfc->driver->readsector=cfc_readsector;
	cfc->driver->writesector=cfc_writesector;
	cfc->driver->readmultiplesector=cfc_readmultiplesector;
	cfc->driver->writemultiplesector=cfc_writemultiplesector;
	cfc->driver->getstatus=cfc_getstatus;
	cfc->driver->getphy=cfc_getphy;
	cfc->driver->release=cfc_release;
	cfc->driver->user_ptr=cfc;

	cfc->use=1;

	return cfc->driver;
}

