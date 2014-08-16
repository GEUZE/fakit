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
** ID: S03076BS2                                                    **
** +Revision: 2.81 +                                                **
** +Date: 2008/01/16 13:02:02 +                                     **
** System wide constant definitions                                 **
**********************************************************************
EOC*/
/*
** For log see end of file:
*/
#ifndef _OSSYS_DEFINES_H_
#define _OSSYS_DEFINES_H_ 1

/*
** DEBUGGING 
*/
#ifdef SC_DEBUG
#define __X__ __FILE__,__LINE__
  #define PRINTF(a) kprintf(1,"%8s:%4d: " a,__X__)
  #define PRINTF1(a,b) kprintf(1,"%8s:%4d: " a,__X__,b)
  #define PRINTF2(a,b,c) kprintf(1,"%8s:%4d: " a,__X__,b,c)
  #define PRINTF3(a,b,c,d) kprintf(1,"%8s:%4d: " a,__X__,b,c,d)
  #define PRINTF4(a,b,c,d,e) kprintf(1,"%8s:%4d: " a,__X__,b,c,d,e)
  #define PRINTF5(a,b,c,d,e,f) kprintf(1,"%8s:%4d: " a,__X__,b,c,d,e,f)
  #define PRINTF6(a,b,c,d,e,f,g) kprintf(1,"%8s:%4d: " a,__X__,b,c,d,e,f,g)
  #define DONE(a) kprintf(1,a)
#else
  #define PRINTF(a)
  #define PRINTF1(a,b)
  #define PRINTF2(a,b,c)
  #define PRINTF3(a,b,c,d)
  #define PRINTF4(a,b,c,d,e)
  #define PRINTF5(a,b,c,d,e,f)
  #define PRINTF6(a,b,c,d,e,f,g)
  #define DONE(a) 
#endif

/* SOCNV */
/*
** MAX/MINs 
*/
#ifdef MSGID_16BIT
#define SC_NAME_MAX 31
#define SC_PATH_MAX 255
#else
#define SC_NAME_MAX 63
#define SC_PATH_MAX 511 
#endif

/*
** Kernel
*/
#ifdef MSGID_16BIT
#define SC_MSG_BASE     0x8000
#define SC_KERNEL_MAGIC 0xC000
#else
#define SC_MSG_BASE     0x80000000
#define SC_KERNEL_MAGIC 0xC0000000
#endif

#define SC_VARPOOL_ID   (SC_MSG_BASE + 0x1)

/*
** Connector
*/
#ifdef MSGID_16BIT
#define SCC_MSG_BASE            0x8100
#else
#define SCC_MSG_BASE            0x81000000
#endif

/*
** logging daemon
*/
#ifdef MSGID_16BIT
#define LOGD_MSG_BASE		0x8200
#else
#define LOGD_MSG_BASE		0x82000000
#endif
#define LOGD_ERR_BASE           LOGD_MSG_BASE

/*
** SDD base of all other stuff
*/
#ifdef MSGID_16BIT  		
#define SDD_MSG_BASE          	0x8300
#else
#define SDD_MSG_BASE		0x83000000
#endif
#define SDD_ERR_BASE            SDD_MSG_BASE

/*
** ioctl cmd base
*/
/* EOCNV */
#define IOCTLR(base, no, type)  ((base) + (no))
#define IOCTLW(base, no, type)  ((base) + (no))
#define IOCTLRW(base, no, type) ((base) + (no))
#define IOCTL(base, no)         ((base) + (no))
/* SOCNV */
#ifdef MSGID_16BIT
#define IOCTL_SERIAL_BASE	0x0080
#define IOCTL_TERMIOS_BASE	0x0081
#define IOCTL_BLKDEV_BASE	0x0082
#define IOCTL_HDLC_BASE		0x0083
#define IOCTL_CON_BASE		0x0084
#else
#define IOCTL_SERIAL_BASE	0x00800000
#define IOCTL_TERMIOS_BASE	0x00810000
#define IOCTL_BLKDEV_BASE	0x00820000
#define IOCTL_HDLC_BASE		0x00830000
#define IOCTL_CON_BASE		0x00840000
#endif

/*
** SDD object types  
*/

#ifdef MSGID_16BIT
#define SDD_NONE_TYPE		0x0000
#define SDD_OBJ_TYPE		0x0080
#define SDD_MAN_TYPE		0x0040
#define SDD_DEV_TYPE		0x0020
#define SDD_FILE_TYPE		0x0010
#define SDD_NET_TYPE		0x0008
#define SFS_DIR_TYPE		0x0004
#define SFS_MOUNTP_TYPE		0x0002
#else
#define SDD_NONE_TYPE		0x00000000
#define SDD_OBJ_TYPE		0x00800000
#define SDD_MAN_TYPE		0x00400000
#define SDD_DEV_TYPE		0x00200000
#define SDD_FILE_TYPE		0x00100000
#define SDD_NET_TYPE		0x00080000
#define SFS_DIR_TYPE		0x00040000
#define SFS_MOUNTP_TYPE		0x00020000
#endif

#define SFS_ATTR_FILE (SDD_OBJ_TYPE | SDD_DEV_TYPE | SDD_FILE_TYPE)
#define SFS_ATTR_DIR  (SDD_OBJ_TYPE | SDD_MAN_TYPE | SFS_DIR_TYPE)



/*
** 3RD PARTY MAGICs 
**
** if it is 16Bit: Day of year and the least siginficant digit in the year 
** number
** i.e. 10.10.2003 -> 13A 3
**
** if it is 32Bit: Day of year and the year
** i.e: 10.10.2003 -> 13A 7d3
** 
** i do not know if this scheme is a good one?!
**
**/

#ifdef MSGID_16BIT
#define THIRD_PARTY_HCC_EFFS	0x13A3
#else
#define THIRD_PARTY_HCC_EFFS	0x13A7d300
#endif


/*
** IPS
*/
#ifdef MSGID_16BIT
#define IPS_MSG_BASE		0x8400
#define TCP_SOCKET_MAGIC	(IPS_MSG_BASE+0x0001)
#define TCP_LISTEN_MAGIC	(IPS_MSG_BASE+0x0002)
#define TCP_ACCEPT_MAGIC	(IPS_MSG_BASE+0x0003)
#define TCP_CONNECT_MAGIC	(IPS_MSG_BASE+0x0004)
#define UDP_SOCKET_MAGIC	(IPS_MSG_BASE+0x0005)
#else
#define IPS_MSG_BASE		0x84000000
#define TCP_SOCKET_MAGIC	(IPS_MSG_BASE+0x00010000)
#define TCP_LISTEN_MAGIC	(IPS_MSG_BASE+0x00020000)
#define TCP_ACCEPT_MAGIC	(IPS_MSG_BASE+0x00030000)
#define TCP_CONNECT_MAGIC	(IPS_MSG_BASE+0x00040000)
#define UDP_SOCKET_MAGIC	(IPS_MSG_BASE+0x0005000)
#endif
#define IPS_ERR_BASE            IPS_MSG_BASE

/*
** PPP
*/
#ifdef MSGID_16BIT
#define PPP_MSG_BASE		0x8500
#else
#define PPP_MSG_BASE		0x85000000
#endif
#define PPP_ERR_BASE            PPP_MSG_BASE

/*
** BOOTP 
*/
#ifdef MSGID_16BIT
#define BOOTP_MSG_BASE	0x8600
#else
#define BOOTP_MSG_BASE	0x86000000
#endif
#define BOOTP_ERR_BASE          BOOTP_MSG_BASE

/*
** DHCP 
*/
#ifdef MSGID_16BIT
#define DHCP_MSG_BASE		0x8700
#else
#define DHCP_MSG_BASE		0x87000000
#endif
#define DHCP_ERR_BASE           DHCP_MSG_BASE

/*
** DNS
*/
#ifdef MSGID_16BIT
#define DNS_MSG_BASE		0x8800
#else
#define DNS_MSG_BASE		0x88000000
#endif
#define DNS_ERR_BASE		DNS_MSG_BASE
/*
** TELNET
*/
#ifdef MSGID_16BIT
#define TELNET_MSG_BASE	0x8900
#else
#define TELNET_MSG_BASE	0x89000000
#endif
#define TELNET_ERR_BASE	        TELNET_MSG_BASE

/*
** HTTP 
*/
#ifdef MSGID_16BIT
#define HTTP_MSG_BASE		0x8a00
#else
#define HTTP_MSG_BASE		0x8a000000
#endif
#define HTTP_ERR_BASE	        HTTP_MSG_BASE

/*
** SFS
*/
#ifdef MSGID_16BIT
#define SFS_MSG_BASE		0x8b00
#else
#define SFS_MSG_BASE		0x8b000000
#endif
#define SFS_ERR_BASE		SFS_MSG_BASE

/*
** SCIO 
*/
#ifdef MSGID_16BIT
#define SCIO_MSG_BASE		0x8c00
#define SCIO_ERR_BASE		0x8b00
#else
#define SCIO_MSG_BASE		0x8c000000
#define SCIO_ERR_BASE		0x8c000000
#endif
#define SCIO_MAGIC              (SCIO_MSG_BASE + 1)
#define SCIO_PROCVAR_ID      	(SCIO_MSG_BASE + 2)


/*
** USB 
*/
#ifdef MSGID_16BIT
#define USB_MSG_BASE		0x8d00
#else
#define USB_MSG_BASE		0x8d000000
#endif
#define USB_ERR_BASE		USB_MSG_BASE

/*
** Semaphore
*/
#ifdef MSGID_16BIT
#define SEM_MSG_BASE		0x8e00
#else
#define SEM_MSG_BASE		0x8e000000
#endif
#define SEM_ERR_BASE		SEM_MSG_BASE

/*
** scipota shell 
*/
#ifdef MSGID_16BIT
#define SH_MSG_BASE		0x8f00
#else
#define SH_MSG_BASE		0x8f000000
#endif
#define SH_ERR_BASE             SH_MSG_BASE

/*
** SNMP
*/
#ifdef MSGID_16BIT
#define SNMP_MSG_BASE		0x9000
#else
#define SNMP_MSG_BASE		0x90000000
#endif
#define SNMP_ERR_BASE           SNMP_MSG_BASE

/*
** TFTP
*/
#ifdef MSGID_16BIT
#define TFTP_MSG_BASE		0x9100
#else
#define TFTP_MSG_BASE		0x91000000
#endif
#define TFTP_ERR_BASE           TFTP_MSG_BASE

/*
** Exception handling   
*/
#ifdef MSGID_16BIT
#define EXCEPT_PROCVAR_ID	0x9200
#else
#define EXCEPT_PROCVAR_ID	0x92000000
#endif
#define EXCEPT_ERR_BASE		EXCEPT_PROCVAR_ID 

/*
** Container   
*/
#ifdef MSGID_16BIT
#define CONTAINER_MSG_BASE	0x9300
#else
#define CONTAINER_MSG_BASE	0x93000000
#endif
#define CONTAINER_ERR_BASE	CONTAINER_MSG_BASE 

/*
** FTP
*/
#ifdef MSGID_16BIT
#define FTP_MSG_BASE		0x9400
#else
#define FTP_MSG_BASE		0x94000000
#endif
#define FTP_ERR_BASE            FTP_MSG_BASE

/*
** RTDS from PragmaDev 
*/
#ifdef MSGID_16BIT
#define RTDS_MSG_BASE		0x9500
#else
#define RTDS_MSG_BASE		0x95000000
#endif
#define RTDS_ERR_BASE            RTDS_MSG_BASE

/*
** 3rd Party
*/
#ifdef MSGID_16BIT
#define _3RD_MSG_BASE		0x9600
#define HCC_MSG_BASE            0x9610
#define PEG_MSG_BASE            0x9620
#else
#define _3RD_MSG_BASE		0x96000000
#define HCC_MSG_BASE            0x96100000
#define PEG_MSG_BASE            0x96200000
#endif
#define _3RD_ERR_BASE           _3RD_MSG_BASE
#define HCC_ERR_BASE            HCC_MSG_BASE
#define PEG_ERR_BASE            PEG_MSG_BASE

/* EOCNV */
#endif
