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

#include "cfc_ide.h"

#define CFCDRV_CNT 2

/* Structure definition for multiple device support */
typedef struct
{
	unsigned long base;		/* base pointer */
	unsigned short select_card;	/* card select register value */
	int use;			/* driver in use */

/*	int state;			*//* actual state of CFC */
	int changed;			/* changed */
#if (!CFC16BITDATA)
	int set8bit;			/* 8 bit mode*/
#endif

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
#if (!CFC16BITDATA)
	1,
#endif
	CFC_CPLDSTATE0, CFC_CPLDSTATE_CDCH0, CFC_CPLDSTATE_CFCD0,&cfc_drivers[0] },

/* Drive 1 */
	{ CFC_BASE0, 0x00f0, 0, /*0,*/ 1,
#if (!CFC16BITDATA)
	1,
#endif
	CFC_CPLDSTATE0, CFC_CPLDSTATE_CDCH0, CFC_CPLDSTATE_CFCD0,&cfc_drivers[1] }
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

static long fnCFCwaitBusy(const volatile unsigned short *State)
{
	volatile unsigned long dwto=CFC_TOVALUE;

	while (*State & CFC_STATE_BUSY)
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

			while (CFC_VAL(CFC_STATE) & CFC_STATE_BUSY)
			{
				if (!dwto)
				{
					return state;
				}
				dwto--;
			}

			cfc->changed=0; /* clear only if its in */

#if (!CFC16BITDATA)
			cfc->set8bit=1;
#endif
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

static long fnCFCwaitReady(const volatile unsigned short *State) {
volatile unsigned long dwto=CFC_TOVALUE;

   while ((*State & (CFC_STATE_BUSY|CFC_STATE_RDY))!=CFC_STATE_RDY) {
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

static long fnCFCwaitDrq(const volatile unsigned short *State) {
volatile unsigned long dwto=CFC_TOVALUE;

   while ((*State & (CFC_STATE_BUSY|CFC_STATE_DRQ))!=CFC_STATE_DRQ) {
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

static long fnCFCsetCommand (unsigned long dwcmd,const t_CfcDrv *cfc)
{
	CFC_VAL(CFC_COMMAND)=(unsigned short)dwcmd;          /* pass the command */

	if (fnCFCwaitBusy(CFC_PTR(CFC_STATE)))
	{
		return 1;  /* command is not accepted */
	}

	if (fnCFCwaitDrq(CFC_PTR(CFC_STATE)))
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
#if (CFC16BITDATA)
	register unsigned short *wdata=(unsigned short*)pdata;
#else
	register unsigned char *cdata=(unsigned char*)pdata;
#endif
	unsigned long length;
	unsigned long page_pos;
	unsigned long cylinder;

	volatile unsigned short *Data=CFC_PTR(CFC_DATA);
	volatile unsigned short *State=CFC_PTR(CFC_STATE);

   if (cfc_getstatus(driver)&F_ST_MISSING) return CFC_ERR_NOTPLUGGED; /* check if FlashCard is there */

   if (fnCFCwaitBusy(CFC_PTR(CFC_STATE))) {
      return CFC_ERR_BUSY_ATCYL; /* check if last write performed */
   }

   if (fnCFCwaitReady(CFC_PTR(CFC_STATE))) {
      return CFC_ERR_BUSY_ATCYL; /* not ready to accept new command */
   }


#if (!CFC16BITDATA)
      if (((t_CfcDrv *)p)->set8bit) {
         CFC_VAL(CFC_FEATURE)=0x01;       /* set 8 bit mode */
         CFC_VAL(CFC_SECTORCOU)=0;
         CFC_VAL(CFC_SELC)=CFC_SELCARD(p);
         CFC_VAL(CFC_COMMAND)=CFC_SETFEA;

         if (fnCFCwaitBusy(State)) {
            return CFC_ERR_BUSY_ATCYL; /* check if last write performed */
         }

         if (CFC_VAL(CFC_STATE) & (CFC_STATE_ERR|CFC_STATE_DRQ|CFC_STATE_BUSY)) {
            return CFC_ERR_BUSY_ATCYL; /* check if last write performed */
         }

         if (cfc_getstatus(p)) {
            return CFC_ERR_NOTPLUGGED; /* check if FlashCard is still there */
         }

         ((t_CfcDrv *)p)->set8bit=0;
      }
#endif


   cylinder=(sector>>8)&0xffff;      /* get the current cylinder (256sector / cylinder) */
   CFC_VAL(CFC_CYLINDERLO)=(unsigned short)(cylinder&0xff);    /* xxxxcccc cccccccs sssssssb bbbbbbbb */
   CFC_VAL(CFC_CYLINDERHI)=(unsigned short)(cylinder>>8); /* xxxxcccc cccccccs sssssssb bbbbbbbb */
   CFC_VAL(CFC_SELC)=CFC_SELCARD(cfc)+((sector&0x0f000000)>>24);
   CFC_VAL(CFC_SECTORNO)=(unsigned short)sector;        /* set sector no and cou=1 */
   if (sectorcount==256) CFC_VAL(CFC_SECTORCOU)=0;
   else CFC_VAL(CFC_SECTORCOU)=(unsigned short)sectorcount; /* set sector no and cou=1 */

   if (fnCFCsetCommand(dwcmd,cfc)) {
      return CFC_ERR_BUSY_ATCMD; /* check if command can be started */
   }

   length=CFC_PAGE_SIZE*sectorcount;
   if (dwcmd==CFC_WRITE)
   {
     while (length)
     {
       page_pos=CFC_PAGE_SIZE>>1;
       while (page_pos)
       {
         register unsigned short data=*wdata++;
         data=MOTOWORD(data);
         *Data=data;
         --page_pos;
       }
       if (length>CFC_PAGE_SIZE)
       {
         if (fnCFCwaitBusy(State)) return CFC_ERR_BUSY_ATCYL;
         if (fnCFCwaitDrq(State)) return CFC_ERR_BUSY_ATDRQ;
       }
       length-=CFC_PAGE_SIZE;
     }
   }
   else
   {
     while (length)
     {
       page_pos=CFC_PAGE_SIZE>>1;
       while (page_pos)
       {
         register unsigned short data=(unsigned short)(*Data);
         data=MOTOWORD(data);
         *wdata++=data;         /* F_DEF_SECTOR_SIZE byte at once */
         --page_pos;
       }
       if (length>CFC_PAGE_SIZE)
       {
         if (fnCFCwaitBusy(State)) return CFC_ERR_BUSY_ATCYL;
         if (fnCFCwaitDrq(State)) return CFC_ERR_BUSY_ATDRQ;
       }
       length-=CFC_PAGE_SIZE;
     }
   }

   if (fnCFCwaitBusy(State)) {
      return CFC_ERR_BUSY_ATCMD;
   }

   if (CFC_VAL(CFC_STATE) & CFC_STATE_ERR) {
      return CFC_ERR_STATE;
   }

   if (dwcmd!=CFC_ID)
   {
     if (CFC_VAL(CFC_SECTORCOU) & (0x00ff)) {
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

	if ((unsigned long)data&3)
	{
		char *datab=(char*)data;
		while (cnt--)
		{
			int rc;
			(void)_memcpy(buf_align,datab,F_DEF_SECTOR_SIZE);
			rc=fnCFC(driver,buf_align,sector,1UL,CFC_WRITE);
			if (rc) return rc;

			datab+=F_DEF_SECTOR_SIZE;
			sector++;
		}
	}
	else
	{
		return fnCFC(driver,data,sector,(unsigned long)cnt,CFC_WRITE);
	}

	return 0;
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

	if ((unsigned long)data&3)
	{
		char *datab=(char*)data;
		while (cnt--)
		{
			int rc=fnCFC(driver,buf_align,sector,1,CFC_READ);
			if (rc) return rc;

			(void)_memcpy(datab,buf_align,F_DEF_SECTOR_SIZE);
			datab+=F_DEF_SECTOR_SIZE;
			sector++;
		}
	}
	else
	{
		return fnCFC(driver,data,sector,(unsigned long)cnt,CFC_READ);
	}

	return 0;
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
 * Releases an existing driver
 *
 * INPUTS
 *
 * driver - driver structure
 *
 ***************************************************************************/

/*lint -e{818} driver can be const */
static void cfc_release(F_DRIVER *driver)
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
 * driver_param - driver parameter
 *
 * RETURNS
 *
 * driver structure pointer
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
	CFC_CPLDCMD1=0x0009;
	CFC_CPLDCMD2=0x0002;
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

