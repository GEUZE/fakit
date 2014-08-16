/****************************************************************************
 *
 *            Copyright (c) 2005-2007 by HCC Embedded
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

#include "../f_atmel.h"
//#include "../m_atmel.h"

#include "spi.h"

typedef struct {
unsigned char flash[ADF_PAGE_SIZE*ADF_REAL_PAGE_COUNT];
unsigned char buf1[ADF_PAGE_SIZE];
unsigned char buf2[ADF_PAGE_SIZE];
unsigned char cmd;
int cnt,addr_cnt;
unsigned long page_addr;
unsigned long byte_addr;
unsigned long all_addr;
} M_DEV;

// legkisebb
//#define ADF_ID		         (0x05 << 2)
// 161
#define ADF_ID		         (0x0b << 2)
// legnagyobb
//#define ADF_ID		         (0x0f << 2)
#define ADF_BREADY		(1u<<7)

#ifndef F_ATMEL_NO_OF_DEVICES
#define F_ATMEL_NO_OF_DEVICES 1
#endif

M_DEV m_devs[F_ATMEL_NO_OF_DEVICES];
M_DEV *dev_sel;

#define ADF_READ_CONT	        0xe8
#define ADF_READ		          0xd2
#define ADF_READ_BUF1	        0xd4
#define ADF_READ_BUF2	        0xd6
#define ADF_STATUS		        0xd7
#define ADF_WRITE_BUF1	      0x84
#define ADF_WRITE_BUF2	      0x87
#define ADF_PROGERASE_BUF1	  0x83
#define ADF_PROGERASE_BUF2	  0x86
#define ADF_PROG_BUF1	        0x88
#define ADF_PROG_BUF2	        0x89
#define ADF_ERASE_PAGE	      0x81
#define ADF_ERASE_BLOCK	      0x50
#define ADF_READ_MAIN2BUF1	  0x53
#define ADF_READ_MAIN2BUF2	  0x55

void myassert(char *s) {
	for (;;);
}

void memset_l(void *dest, long data,long size) {
long *d=(long*)dest;
	size>>=2;
	data=(long)0xffffffffUL;
	while (size--) {
		*d++=data;
	}
}


void memcpy_l(void *dest, void *sou,long size) {
long *d=(long*)dest;
long *s=(long*)sou;
	size>>=2;
	while (size--) {
		*d++=*s++;
	}
}

void spi_init() {
	dev_sel=0;
}

void spi_fast_read_buf1(unsigned char *dst,unsigned int read) {
long *dl=(long*)dst;
long *sl=(long*)(&dev_sel->buf1[dev_sel->byte_addr]);
unsigned char *sst;

	if (dev_sel->byte_addr+read>ADF_PAGE_SIZE) myassert("spi_fast_read_buf1");

	dev_sel->byte_addr+=read;

	while (read>3) {
		*dl++=*sl++;
		read-=4;
	}

	if (!read) return;

	dst=(unsigned char*)dl;
	sst=(unsigned char*)sl;

	while (read--) {
		*dst++=*sst++;
	}
}

void spi_fast_write_buf1(unsigned char *src,unsigned int write) {
long *dl=(long*)(&dev_sel->buf1[dev_sel->byte_addr]);
long *sl=(long*)src;
unsigned char *sst;

	if (dev_sel->byte_addr+write>ADF_PAGE_SIZE) myassert("spi_fast_write_buf1");

	dev_sel->byte_addr+=write;

	while (write>3) {
		*dl++=*sl++;
		write-=4;
	}

	if (!write) return;

	src=(unsigned char*)dl;
	sst=(unsigned char*)sl;

	while (write--) {
		*src++=*sst++;
	}
}


unsigned char spi_rx_8() {
	if (!dev_sel) {
		myassert("spi_rx_8");
		return 0xff;
	}

	switch (dev_sel->cmd) {
	case ADF_STATUS:
		return ADF_ID | ADF_BREADY;

	case ADF_READ:
		{
			unsigned char ch=dev_sel->flash[dev_sel->page_addr*ADF_PAGE_SIZE+dev_sel->byte_addr];
			if (dev_sel->page_addr*ADF_PAGE_SIZE+dev_sel->byte_addr > ADF_REAL_PAGE_COUNT*ADF_PAGE_SIZE) myassert("spi_rx_8 ADF_READ");
			dev_sel->byte_addr++;
			return ch;
		}

	case ADF_READ_BUF1:
		if (dev_sel->byte_addr>=ADF_PAGE_SIZE) myassert("spi_rx_8 ADF_READ_BUF1");
		return dev_sel->buf1[dev_sel->byte_addr++];
	case ADF_READ_BUF2:
		if (dev_sel->byte_addr>=ADF_PAGE_SIZE) myassert("spi_rx_8 ADF_READ_BUF2");
		return dev_sel->buf2[dev_sel->byte_addr++];

	default:
		return 0xff;
	}
}

void spi_tx_8(unsigned char data) {
	if (!dev_sel) {
		myassert("spi_tx_8");
		return;
	}

	if (!dev_sel->cmd) {
		dev_sel->cmd=data;
		dev_sel->cnt=0;
		dev_sel->all_addr=0;
		dev_sel->addr_cnt=0;
		return;
	}

	if (dev_sel->addr_cnt<3) {
		dev_sel->addr_cnt++;
		dev_sel->all_addr<<=8;
		dev_sel->all_addr|=data;

		if (dev_sel->addr_cnt!=3) return;
		else {
			dev_sel->page_addr=dev_sel->all_addr >> ADF_BYTE_ADDRESS_WIDTH;
			dev_sel->byte_addr=dev_sel->all_addr & ((1 << ADF_BYTE_ADDRESS_WIDTH) -1);
		}
	}

	switch (dev_sel->cmd) {
	case ADF_STATUS:
			break;
	case ADF_READ:
			break;
	case ADF_READ_BUF1:
	case ADF_READ_BUF2:
			break;
	case ADF_WRITE_BUF1:
			if (dev_sel->byte_addr>=ADF_PAGE_SIZE) myassert("spi_tx_8 ADF_WRITE_BUF1");
			if (dev_sel->cnt) dev_sel->buf1[dev_sel->byte_addr++]=data;
			break;
	case ADF_WRITE_BUF2:
			if (dev_sel->byte_addr>=ADF_PAGE_SIZE) myassert("spi_tx_8 ADF_WRITE_BUF2");
			if (dev_sel->cnt) dev_sel->buf2[dev_sel->byte_addr++]=data;
			break;
	case ADF_PROGERASE_BUF1:
			if (dev_sel->page_addr>=ADF_REAL_PAGE_COUNT) myassert("spi_tx_8 ADF_PROGERASE_BUF1");
			memcpy_l(&dev_sel->flash[dev_sel->page_addr*ADF_PAGE_SIZE],dev_sel->buf1,ADF_PAGE_SIZE);
			break;
	case ADF_PROGERASE_BUF2:
			if (dev_sel->page_addr>=ADF_REAL_PAGE_COUNT) myassert("spi_tx_8 ADF_PROGERASE_BUF2");
			memcpy_l(&dev_sel->flash[dev_sel->page_addr*ADF_PAGE_SIZE],dev_sel->buf2,ADF_PAGE_SIZE);
			break;
	case ADF_PROG_BUF1:
			break;
	case ADF_PROG_BUF2:
			break;
	case ADF_ERASE_PAGE:
			if (dev_sel->page_addr>=ADF_REAL_PAGE_COUNT) myassert("spi_tx_8 ADF_ERASE_PAGE");
			memset_l(&dev_sel->flash[dev_sel->page_addr*ADF_PAGE_SIZE],0xff,ADF_PAGE_SIZE);
			break;
	case ADF_ERASE_BLOCK:
			break;
	case ADF_READ_MAIN2BUF1:
			if (dev_sel->page_addr>=ADF_REAL_PAGE_COUNT) myassert("spi_tx_8 ADF_READ_MAIN2BUF1");
			memcpy_l(dev_sel->buf1,&dev_sel->flash[dev_sel->page_addr*ADF_PAGE_SIZE],ADF_PAGE_SIZE);
			break;
	case ADF_READ_MAIN2BUF2:
			if (dev_sel->page_addr>=ADF_REAL_PAGE_COUNT) myassert("spi_tx_8 ADF_READ_MAIN2BUF2");
			memcpy_l(dev_sel->buf2,&dev_sel->flash[dev_sel->page_addr*ADF_PAGE_SIZE],ADF_PAGE_SIZE);
			break;
	default:
		return;
	}

	dev_sel->cnt++;
}

void spi_cs0_lo() {
	dev_sel=&m_devs[0];
	dev_sel->cmd=0;
}

void spi_cs0_hi() {
	dev_sel=0;
}

void spi_cs1_lo() {
	dev_sel=&m_devs[1];
	dev_sel->cmd=0;
}

void spi_cs1_hi() {
	dev_sel=0;
}

