;;; SOC
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;  _______  _______ _________ _______  _______ _________ _______
;; (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )
;; | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |
;; | (_____ | |         | |   | |   | || (____)|   | |   | (___) |
;; (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |
;;       ) || |         | |   | |   | || (         | |   | (   ) |
;; /\____) || (____/\___) (___| (___) || )         | |   | )   ( |
;; \_______)(_______/\_______/(_______)|/          )_(   |/     \|
;;                            _  _     
;;                           |_||_)|V|
;;                           | || \| |
;;
;; Direct Message Passing Real Time Kernel
;;
;;              (c) 2009 SCIOPTA Systems AG/ Schweiz
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Sciopta Kernel for ARM
;; Revision: 1.9.6.8
;; Date: 19-11-2009
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; kernel.S             1.57.2.9 
;;; kernel_v7m.S         1.4.2.9 
;;; module.S             1.41.2.2 
;;; misc.S               1.23.2.2 
;;; msg.S                1.48.2.12 
;;; addrget.S            1.9.2.1 
;;; sizeget.S            1.9.2.1 
;;; sizeset.S            1.12.2.3 
;;; sndget.S             1.8.2.1 
;;; ownerget.S           1.8.2.1 
;;; poolget.S            1.14.2.1 
;;; proc.S               1.81.2.10 
;;; pool.S               1.31.2.3 
;;; tick.S               1.39.2.5 
;;; tmo.S                1.15.2.1 
;;; trigger.S            1.18.2.10 
;;; procd.S              1.11.2.1 
;;; kerneld.S            1.10.2.1 
;;; connector.S          1.13.2.3 
;;; syscall.S            1.6 
;;; iar.S                1.6 
;;; EOC
;
; possible host-systems
;
NATIVE_HOSTED	EQU	0
LINUX_HOSTED	EQU	1
WIN32_HOSTED	EQU	2
CYGWIN_HOSTED	EQU	3
;
; possible cpus
;
ANY	EQU	0
ARM	EQU	1
STRONGARM	EQU	2
SH	EQU	3
I386	EQU	4
PPC	EQU	5
COLDFIRE	EQU	6
M68K	EQU	7
M68HC12	EQU	8
C166	EQU	9
MSP430	EQU	10
M16C	EQU	11
R65C02	EQU	12
ATMEGA	EQU	13
V850	EQU	14
;
; possible Sciopta models
;
SC_MAXIMUM	EQU	0
SC_MEDIUM	EQU	1
SC_MINIMUM	EQU	2
;********************************************************************
;  _______  _______ _________ _______  _______ _________ _______   **
; (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
; | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
; | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
; (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
;       ) || |         | |   | |   | || (         | |   | (   ) |  **
; /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
; \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
;                                                                  **
; (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
;                                                                  **
;********************************************************************
; ID: S02338BS3                                                    **
; +Revision: 1.28 +                                                 **
; +Date: 2008/11/11 10:01:40 +                                     **
; Static kernel configuration                                      **
;********************************************************************
; generated
	;  CPU types 
ARMv4T	EQU	1
ARMv5T	EQU	2
ARMv5TE	EQU	3
Xscale	EQU	4
ARMv6	EQU	5
ARMv6M	EQU	6
ARMv7	EQU	7
ARMv7M	EQU	8
	INCLUDE sciopta.cnf
; Converted header-files
; ossys/defines.h
;********************************************************************
;  _______  _______ _________ _______  _______ _________ _______   **
; (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
; | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
; | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
; (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
;       ) || |         | |   | |   | || (         | |   | (   ) |  **
; /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
; \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
;                                                                  **
; (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
;                                                                  **
;********************************************************************
; ID: S03076BS2                                                    **
; +Revision: 2.81 +                                                **
; +Date: 2008/01/16 13:02:02 +                                     **
; System wide constant definitions                                 **
;********************************************************************
; generated
;
; MAX/MINs 
;
	IF :DEF: MSGID_16BIT
SC_NAME_MAX	EQU	31
SC_PATH_MAX	EQU	255
	ELSE
SC_NAME_MAX	EQU	63
SC_PATH_MAX	EQU	511 
	ENDIF
;
; Kernel
;
	IF :DEF: MSGID_16BIT
SC_MSG_BASE	EQU	0x8000
SC_KERNEL_MAGIC	EQU	0xC000
	ELSE
SC_MSG_BASE	EQU	0x80000000
SC_KERNEL_MAGIC	EQU	0xC0000000
	ENDIF
SC_VARPOOL_ID	EQU	(SC_MSG_BASE + 0x1)
;
; Connector
;
	IF :DEF: MSGID_16BIT
SCC_MSG_BASE	EQU	0x8100
	ELSE
SCC_MSG_BASE	EQU	0x81000000
	ENDIF
;
; logging daemon
;
	IF :DEF: MSGID_16BIT
LOGD_MSG_BASE	EQU	0x8200
	ELSE
LOGD_MSG_BASE	EQU	0x82000000
	ENDIF
LOGD_ERR_BASE	EQU	LOGD_MSG_BASE
;
; SDD base of all other stuff
;
	IF :DEF: MSGID_16BIT  		
SDD_MSG_BASE	EQU	0x8300
	ELSE
SDD_MSG_BASE	EQU	0x83000000
	ENDIF
SDD_ERR_BASE	EQU	SDD_MSG_BASE
;
; ioctl cmd base
;
	IF :DEF: MSGID_16BIT
IOCTL_SERIAL_BASE	EQU	0x0080
IOCTL_TERMIOS_BASE	EQU	0x0081
IOCTL_BLKDEV_BASE	EQU	0x0082
IOCTL_HDLC_BASE	EQU	0x0083
IOCTL_CON_BASE	EQU	0x0084
	ELSE
IOCTL_SERIAL_BASE	EQU	0x00800000
IOCTL_TERMIOS_BASE	EQU	0x00810000
IOCTL_BLKDEV_BASE	EQU	0x00820000
IOCTL_HDLC_BASE	EQU	0x00830000
IOCTL_CON_BASE	EQU	0x00840000
	ENDIF
;
; SDD object types  
;
	IF :DEF: MSGID_16BIT
SDD_NONE_TYPE	EQU	0x0000
SDD_OBJ_TYPE	EQU	0x0080
SDD_MAN_TYPE	EQU	0x0040
SDD_DEV_TYPE	EQU	0x0020
SDD_FILE_TYPE	EQU	0x0010
SDD_NET_TYPE	EQU	0x0008
SFS_DIR_TYPE	EQU	0x0004
SFS_MOUNTP_TYPE	EQU	0x0002
	ELSE
SDD_NONE_TYPE	EQU	0x00000000
SDD_OBJ_TYPE	EQU	0x00800000
SDD_MAN_TYPE	EQU	0x00400000
SDD_DEV_TYPE	EQU	0x00200000
SDD_FILE_TYPE	EQU	0x00100000
SDD_NET_TYPE	EQU	0x00080000
SFS_DIR_TYPE	EQU	0x00040000
SFS_MOUNTP_TYPE	EQU	0x00020000
	ENDIF
SFS_ATTR_FILE	EQU	(SDD_OBJ_TYPE | SDD_DEV_TYPE | SDD_FILE_TYPE)
SFS_ATTR_DIR	EQU	(SDD_OBJ_TYPE | SDD_MAN_TYPE | SFS_DIR_TYPE)
;
; 3RD PARTY MAGICs 
;
; if it is 16Bit: Day of year and the least siginficant digit in the year 
; number
; i.e. 10.10.2003 -> 13A 3
;
; if it is 32Bit: Day of year and the year
; i.e: 10.10.2003 -> 13A 7d3
; 
; i do not know if this scheme is a good one?!
;
;*
	IF :DEF: MSGID_16BIT
THIRD_PARTY_HCC_EFFS	EQU	0x13A3
	ELSE
THIRD_PARTY_HCC_EFFS	EQU	0x13A7d300
	ENDIF
;
; IPS
;
	IF :DEF: MSGID_16BIT
IPS_MSG_BASE	EQU	0x8400
TCP_LISTEN_MAGIC	EQU	(IPS_MSG_BASE+0x0002)
TCP_ACCEPT_MAGIC	EQU	(IPS_MSG_BASE+0x0003)
TCP_CONNECT_MAGIC	EQU	(IPS_MSG_BASE+0x0004)
	ELSE
IPS_MSG_BASE	EQU	0x84000000
TCP_LISTEN_MAGIC	EQU	(IPS_MSG_BASE+0x00020000)
TCP_ACCEPT_MAGIC	EQU	(IPS_MSG_BASE+0x00030000)
TCP_CONNECT_MAGIC	EQU	(IPS_MSG_BASE+0x00040000)
	ENDIF
IPS_ERR_BASE	EQU	IPS_MSG_BASE
;
; PPP
;
	IF :DEF: MSGID_16BIT
PPP_MSG_BASE	EQU	0x8500
	ELSE
PPP_MSG_BASE	EQU	0x85000000
	ENDIF
PPP_ERR_BASE	EQU	PPP_MSG_BASE
;
; BOOTP 
;
	IF :DEF: MSGID_16BIT
BOOTP_MSG_BASE	EQU	0x8600
	ELSE
BOOTP_MSG_BASE	EQU	0x86000000
	ENDIF
BOOTP_ERR_BASE	EQU	BOOTP_MSG_BASE
;
; DHCP 
;
	IF :DEF: MSGID_16BIT
DHCP_MSG_BASE	EQU	0x8700
	ELSE
DHCP_MSG_BASE	EQU	0x87000000
	ENDIF
DHCP_ERR_BASE	EQU	DHCP_MSG_BASE
;
; DNS
;
	IF :DEF: MSGID_16BIT
DNS_MSG_BASE	EQU	0x8800
	ELSE
DNS_MSG_BASE	EQU	0x88000000
	ENDIF
DNS_ERR_BASE	EQU	DNS_MSG_BASE
;
; TELNET
;
	IF :DEF: MSGID_16BIT
TELNET_MSG_BASE	EQU	0x8900
	ELSE
TELNET_MSG_BASE	EQU	0x89000000
	ENDIF
TELNET_ERR_BASE	EQU	TELNET_MSG_BASE
;
; HTTP 
;
	IF :DEF: MSGID_16BIT
HTTP_MSG_BASE	EQU	0x8a00
	ELSE
HTTP_MSG_BASE	EQU	0x8a000000
	ENDIF
HTTP_ERR_BASE	EQU	HTTP_MSG_BASE
;
; SFS
;
	IF :DEF: MSGID_16BIT
SFS_MSG_BASE	EQU	0x8b00
	ELSE
SFS_MSG_BASE	EQU	0x8b000000
	ENDIF
SFS_ERR_BASE	EQU	SFS_MSG_BASE
;
; SCIO 
;
	IF :DEF: MSGID_16BIT
SCIO_MSG_BASE	EQU	0x8c00
SCIO_ERR_BASE	EQU	0x8b00
	ELSE
SCIO_MSG_BASE	EQU	0x8c000000
SCIO_ERR_BASE	EQU	0x8c000000
	ENDIF
SCIO_MAGIC	EQU	(SCIO_MSG_BASE + 1)
SCIO_PROCVAR_ID	EQU	(SCIO_MSG_BASE + 2)
;
; USB 
;
	IF :DEF: MSGID_16BIT
USB_MSG_BASE	EQU	0x8d00
	ELSE
USB_MSG_BASE	EQU	0x8d000000
	ENDIF
USB_ERR_BASE	EQU	USB_MSG_BASE
;
; Semaphore
;
	IF :DEF: MSGID_16BIT
SEM_MSG_BASE	EQU	0x8e00
	ELSE
SEM_MSG_BASE	EQU	0x8e000000
	ENDIF
SEM_ERR_BASE	EQU	SEM_MSG_BASE
;
; scipota shell 
;
	IF :DEF: MSGID_16BIT
SH_MSG_BASE	EQU	0x8f00
	ELSE
SH_MSG_BASE	EQU	0x8f000000
	ENDIF
SH_ERR_BASE	EQU	SH_MSG_BASE
;
; SNMP
;
	IF :DEF: MSGID_16BIT
SNMP_MSG_BASE	EQU	0x9000
	ELSE
SNMP_MSG_BASE	EQU	0x90000000
	ENDIF
SNMP_ERR_BASE	EQU	SNMP_MSG_BASE
;
; TFTP
;
	IF :DEF: MSGID_16BIT
TFTP_MSG_BASE	EQU	0x9100
	ELSE
TFTP_MSG_BASE	EQU	0x91000000
	ENDIF
TFTP_ERR_BASE	EQU	TFTP_MSG_BASE
;
; Exception handling   
;
	IF :DEF: MSGID_16BIT
EXCEPT_PROCVAR_ID	EQU	0x9200
	ELSE
EXCEPT_PROCVAR_ID	EQU	0x92000000
	ENDIF
EXCEPT_ERR_BASE	EQU	EXCEPT_PROCVAR_ID 
;
; Container   
;
	IF :DEF: MSGID_16BIT
CONTAINER_MSG_BASE	EQU	0x9300
	ELSE
CONTAINER_MSG_BASE	EQU	0x93000000
	ENDIF
CONTAINER_ERR_BASE	EQU	CONTAINER_MSG_BASE 
;
; FTP
;
	IF :DEF: MSGID_16BIT
FTP_MSG_BASE	EQU	0x9400
	ELSE
FTP_MSG_BASE	EQU	0x94000000
	ENDIF
FTP_ERR_BASE	EQU	FTP_MSG_BASE
;
; RTDS from PragmaDev 
;
	IF :DEF: MSGID_16BIT
RTDS_MSG_BASE	EQU	0x9500
	ELSE
RTDS_MSG_BASE	EQU	0x95000000
	ENDIF
RTDS_ERR_BASE	EQU	RTDS_MSG_BASE
;
; 3rd Party
;
	IF :DEF: MSGID_16BIT
_3RD_MSG_BASE	EQU	0x9600
HCC_MSG_BASE	EQU	0x9610
PEG_MSG_BASE	EQU	0x9620
	ELSE
_3RD_MSG_BASE	EQU	0x96000000
HCC_MSG_BASE	EQU	0x96100000
PEG_MSG_BASE	EQU	0x96200000
	ENDIF
_3RD_ERR_BASE	EQU	_3RD_MSG_BASE
HCC_ERR_BASE	EQU	HCC_MSG_BASE
PEG_ERR_BASE	EQU	PEG_MSG_BASE
;********************************************************************
;  _______  _______ _________ _______  _______ _________ _______   **
; (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
; | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
; | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
; (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
;       ) || |         | |   | |   | || (         | |   | (   ) |  **
; /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
; \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
;                                                                  **
; (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
;                                                                  **
;********************************************************************
; ID: S01103BS46                                                   **
; +Revision: 2.36.4.1 +                                                **
; +Date: 2009/10/04 09:53:43 +                                     **
; Message pool API and definitions                                 **
;********************************************************************
; generated
SC_POOL_NAME_SIZE	EQU	31
	IF :DEF: _SC_CONF_H_
SC_SET_POOLCREATE_HOOK	EQU	(1)
SC_SET_POOLKILL_HOOK	EQU	(2)
	ENDIF
SC_DEFAULT_POOL	EQU	(0x100)
SC_ILLEGAL_POOLID	EQU	(-1)
POOL_ID	EQU	0x00
POOL_CREATOR	EQU	0x04
POOL_SIZE	EQU	0x08
POOL_BUFSIZE	EQU	0x0c
POOL_NBUFSIZES	EQU	(POOL_BUFSIZE+4*SC_MAX_NUM_BUFFERSIZES)
POOL_LOCK	EQU	(POOL_NBUFSIZES+4)
POOL_START	EQU	(POOL_LOCK+4)
POOL_END	EQU	(POOL_START+4)
POOL_CUR	EQU	(POOL_END+4)
POOL_FREED	EQU	(POOL_CUR+4)
POOL_WAITER	EQU	(POOL_FREED+8*SC_MAX_NUM_BUFFERSIZES)
POOL_FLAGS	EQU	(POOL_WAITER+8*SC_MAX_NUM_BUFFERSIZES)
POOL_NAME	EQU	(POOL_FLAGS+4)
POOL_STAT	EQU	(POOL_NAME + SC_POOL_NAME_SIZE+1)
POOL_STAT_REQ	EQU	(POOL_STAT)
POOL_STAT_ALLOC	EQU	(POOL_STAT_REQ+4*SC_MAX_NUM_BUFFERSIZES)
POOL_STAT_FREE	EQU	(POOL_STAT_ALLOC+4*SC_MAX_NUM_BUFFERSIZES)
POOL_STAT_WAIT	EQU	(POOL_STAT_FREE+4*SC_MAX_NUM_BUFFERSIZES)
POOL_STAT_MAX	EQU	(POOL_STAT_WAIT+4*SC_MAX_NUM_BUFFERSIZES)
	IF SC_MSG_STAT = 1
SIZEOF_POOL_CB	EQU	(POOL_STAT_MAX+4*SC_MAX_NUM_BUFFERSIZES)
SIZEOF_SC_POOL_STAT	EQU	(SC_MAX_NUM_BUFFERSIZES*4*5)
	ELSE
SIZEOF_POOL_CB	EQU	(POOL_STAT)
	ENDIF 	;  SC_MSG_STAT 
;********************************************************************
;  _______  _______ _________ _______  _______ _________ _______   **
; (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
; | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
; | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
; (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
;       ) || |         | |   | |   | || (         | |   | (   ) |  **
; /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
; \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
;                                                                  **
; (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
;                                                                  **
;********************************************************************
; ID: S01103BS16                                                   **
; +Revision: 2.42.4.2 +                                                **
; +Date: 2009/09/16 14:34:12 +                                     **
; Global system data                                               **
;********************************************************************
; generated
SC_CONF_MODULES	EQU	0x0000007f
SC_CONF_MSG_STAT	EQU	0x00000080
SC_CONF_CONNECTORS	EQU	0x00007f00
SC_CONF_PROC_STAT	EQU	0x00008000
SC_CONF_BUFFERS	EQU	0x00030000
SC_CONF_PL_CR_HOOK	EQU	0x00040000
SC_CONF_PL_KL_HOOK	EQU	0x00080000
SC_CONF_MS_TX_HOOK	EQU	0x00100000
SC_CONF_MS_RX_HOOK	EQU	0x00200000
SC_CONF_PR_CR_HOOK	EQU	0x00400000
SC_CONF_PR_KL_HOOK	EQU	0x00800000
SC_CONF_PR_SW_HOOK	EQU	0x01000000
SC_CONF_ERR_HOOK	EQU	0x02000000
SC_CONF_MSG_CHECK	EQU	0x04000000
SC_CONF_FRIENDS	EQU	0x08000000
SC_CONF_STACKCHK	EQU	0x10000000
SC_CONF_CPUTYPE	EQU	0xe0000000
SC_CONF_MCF_NOMAC	EQU	0x00000000
SC_CONF_MCF_MAC	EQU	0x20000000
SC_CONF_MCF_EMAC	EQU	0x40000000
SC_CONF_MCF_FPU	EQU	0x80000000
	;  ARM asm-kernel private 
	IF CPU = ARM
SC_RECI_TCKLENGTH	EQU	(-4)
	ENDIF
	;  all kernels 
SC_CURPID	EQU	(0x00)
SC_CURPCB	EQU	(0x04)
SC_CURPRIOPCB	EQU	(0x08)
SC_CURMODULE	EQU	(0x0c)
SC_PRIOMASK	EQU	(0x10)
SC_NESTED_IRQ_H	EQU	(0x14)
SC_VERSION_H	EQU	(0x16)
SC_SCHEDULER_LOCK	EQU	(0x18)
SC_SCHEDULE_REQ	EQU	(0x1c)
SC_SUSPEND_REQ	EQU	(0x20)
SC_TCK	EQU	(0x24)
SC_CONF	EQU	(0x28)
SC_NMODULE	EQU	(0x2c)
SC_TCKLENGTH	EQU	(0x30)
SC_KERNELD	EQU	(0x34)
SC_PROCD	EQU	(0x38)
SC_SPARE	EQU	(0x3c)
	IF :DEF: __C_KERNEL__
SC_MAXMODULE	EQU	(0x40)
SC_MAXCONNECTOR	EQU	(0x44)
SC_MAXINTVECTOR	EQU	(0x48)
SC_READYLIST	EQU	(0x4c)
	ELSE
SC_READYLIST	EQU	(0x40)
	ENDIF
;
; Following defines depend on the actual configuaration and
; thus are not available without sconf.h or sciopta.cnf
;
	IF _SC_CONF_H_ = 0x42
SC_SLEEPERLISTS	EQU	(SC_READYLIST+32*8)
SC_TMOLISTS	EQU	(SC_SLEEPERLISTS+256*8)
	IF SC_ASYNC_TMO = 1
SC_TMOLISTSE	EQU	((SC_TMOLISTS+256*8))
	ELSE
SC_TMOLISTSE	EQU	(SC_TMOLISTS)
	ENDIF
	IF SC_PROC_STAT = 1
SC_SWAPS	EQU	(SC_TMOLISTSE)
	ELSE
SC_SWAPS	EQU	(SC_TMOLISTSE-4)
	ENDIF
	IF SC_POOL_HOOK = 1
	IF SC_POOLCREATE_HOOK = 1
SC_POOLCREATEHOOK	EQU	(SC_SWAPS+4)
	ELSE
SC_POOLCREATEHOOK	EQU	(SC_SWAPS)
	ENDIF
	IF SC_POOLKILL_HOOK = 1
SC_POOLKILLHOOK	EQU	(SC_POOLCREATEHOOK+4)
	ELSE
SC_POOLKILLHOOK	EQU	(SC_POOLCREATEHOOK)
	ENDIF
	ELSE
SC_POOLKILLHOOK	EQU	(SC_SWAPS)
	ENDIF
	IF SC_MSG_HOOK = 1
	IF SC_MSGTX_HOOK = 1
SC_MSGTXHOOK	EQU	(SC_POOLKILLHOOK+4)
	ELSE
SC_MSGTXHOOK	EQU	(SC_POOLKILLHOOK)
	ENDIF
	IF SC_MSGRX_HOOK = 1
SC_MSGRXHOOK	EQU	(SC_MSGTXHOOK+4)
	ELSE
SC_MSGRXHOOK	EQU	(SC_MSGTXHOOK)
	ENDIF
	ELSE
SC_MSGRXHOOK	EQU	(SC_POOLKILLHOOK)
	ENDIF
	IF SC_PROC_HOOK = 1
	IF SC_PROCCREATE_HOOK = 1
SC_PROCCREATEHOOK	EQU	(SC_MSGRXHOOK+4)
	ELSE
SC_PROCCREATEHOOK	EQU	(SC_MSGRXHOOK)
	ENDIF
	IF SC_PROCKILL_HOOK = 1
SC_PROCKILLHOOK	EQU	(SC_PROCCREATEHOOK+4)
	ELSE
SC_PROCKILLHOOK	EQU	(SC_PROCCREATEHOOK)
	ENDIF
	IF SC_PROCSWAP_HOOK = 1
SC_PROCSWAPHOOK	EQU	(SC_PROCKILLHOOK+4)
	ELSE
SC_PROCSWAPHOOK	EQU	(SC_PROCKILLHOOK)
	ENDIF
	IF SC_PROCMMU_HOOK = 1
SC_PROCMMUHOOK	EQU	(SC_PROCSWAPHOOK+4)
	ELSE
SC_PROCMMUHOOK	EQU	(SC_PROCSWAPHOOK)
	ENDIF
	ELSE
SC_PROCMMUHOOK	EQU	(SC_MSGRXHOOK)
	ENDIF
	IF SC_ERR_HOOK = 1
SC_ERRHOOK	EQU	(SC_PROCMMUHOOK+4)
	ELSE
SC_ERRHOOK	EQU	(SC_PROCMMUHOOK)
	ENDIF
	IF CPU = PPC
SC_R2_SAVE	EQU	(SC_ERRHOOK+4)
SC_R13_SAVE	EQU	(SC_R2_SAVE+4)
	ELSE
SC_R13_SAVE	EQU	(SC_ERRHOOK)
	ENDIF
SIZEOF_SCIOPTA	EQU	(SC_R13_SAVE+4)
	ENDIF 	;  _SC_CONF_H_ = 0x42 
;********************************************************************
;  _______  _______ _________ _______  _______ _________ _______   **
; (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
; | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
; | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
; (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
;       ) || |         | |   | |   | || (         | |   | (   ) |  **
; /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
; \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
;                                                                  **
; (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
;                                                                  **
;********************************************************************
; ID: S01103BS13                                                   **
; +Revision: 2.49.12.1 +                                                **
; +Date: 2009/10/04 09:53:43 +                                     **
; Module control block definitions and API                         **
;********************************************************************
; generated
	IF :DEF: SC_PROC_NAME_SIZE
SC_MODULE_NAME_SIZE	EQU	(SC_PROC_NAME_SIZE)
	ELSE
SC_MODULE_NAME_SIZE	EQU	31
	ENDIF
;
; Flags
;
	IF :DEF: SC_PROCKILL_KILL
SC_MODULEKILL_KILL	EQU	(SC_PROCKILL_KILL)
	ELSE
SC_MODULEKILL_KILL	EQU	(0x4000)
	ENDIF
MDB_INIT	EQU	0
MDB_STACKSIZE	EQU	4
MDB_INITPRIO	EQU	8
MDB_MODSTART	EQU	12
MDB_SIZE	EQU	16
MDB_TEXTSIZE	EQU	20
MDB_MAX_POOLS	EQU	24
MDB_MAX_PROCS	EQU	28
MDB_NAME	EQU	32
SIZEOF_MDB	EQU	64
MODULE_ID	EQU	0x00
MODULE_TEXT	EQU	0x04
MODULE_DATA	EQU	0x08
MODULE_SIZE	EQU	0x0c
MODULE_DATAPTR	EQU	0x10
MODULE_FREESIZE	EQU	0x14
MODULE_NPOOLS	EQU	0x18
MODULE_MAX_POOLS	EQU	0x1c
MODULE_POOL	EQU	0x20
MODULE_PRIO	EQU	0x24
MODULE_PID_CNT	EQU	0x28
MODULE_PID_NXT	EQU	0x2c
MODULE_NPROCESS	EQU	0x30
MODULE_MAX_PROCESS	EQU	0x34
MODULE_PCBS	EQU	0x38
	IF SC_USE_FRIENDS = 1
MODULE_FRIENDSET	EQU	0x3c
MODULE_FRIENDSETE	EQU	0x48
	ELSE
MODULE_FRIENDSETE	EQU	0x38
	ENDIF
	IF SC_MSG_HOOK = 1
	IF SC_MSGTX_HOOK = 1
MODULE_MSGTXHOOK	EQU	(MODULE_FRIENDSETE+4)
	ELSE
MODULE_MSGTXHOOK	EQU	(MODULE_FRIENDSETE)
	ENDIF
	IF SC_MSGRX_HOOK = 1
MODULE_MSGRXHOOK	EQU	(MODULE_MSGTXHOOK+4)
	ELSE
MODULE_MSGRXHOOK	EQU	(MODULE_MSGTXHOOK)
	ENDIF
	ELSE
MODULE_MSGRXHOOK	EQU	(MODULE_FRIENDSETE)
	ENDIF
	IF SC_PROC_HOOK = 1
	IF SC_PROCCREATE_HOOK = 1
MODULE_PROCCREATEHOOK	EQU	(MODULE_MSGRXHOOK+4)
	ELSE
MODULE_PROCCREATEHOOK	EQU	(MODULE_MSGRXHOOK)
	ENDIF
	IF SC_PROCKILL_HOOK = 1
MODULE_PROCKILLHOOK	EQU	(MODULE_PROCCREATEHOOK+4)
	ELSE
MODULE_PROCKILLHOOK	EQU	(MODULE_PROCCREATEHOOK)
	ENDIF
	IF SC_PROCSWAP_HOOK = 1
MODULE_PROCSWAPHOOK	EQU	(MODULE_PROCKILLHOOK+4)
	ELSE
MODULE_PROCSWAPHOOK	EQU	(MODULE_PROCKILLHOOK)
	ENDIF
	ELSE
MODULE_PROCSWAPHOOK	EQU	(MODULE_MSGRXHOOK)
	ENDIF
	IF SC_ERR_HOOK = 1
MODULE_ERRHOOK	EQU	(MODULE_PROCSWAPHOOK+4)
	ELSE
MODULE_ERRHOOK	EQU	(MODULE_PROCSWAPHOOK)
	ENDIF
	IF SC_POOL_HOOK = 1
	IF SC_POOLKILL_HOOK = 1
MODULE_POOLKILLHOOK	EQU	(MODULE_ERRHOOK+4)
	ELSE
MODULE_POOLKILLHOOK	EQU	(MODULE_ERRHOOK)
	ENDIF
	IF SC_POOLCREATE_HOOK = 1
MODULE_POOLCREATEHOOK	EQU	(MODULE_POOLKILLHOOK+4)
	ELSE
MODULE_POOLCREATEHOOK	EQU	(MODULE_POOLKILLHOOK)
	ENDIF
	ELSE
MODULE_POOLCREATEHOOK	EQU	(MODULE_ERRHOOK)
	ENDIF
MODULE_HASH_H	EQU	(MODULE_POOLCREATEHOOK+4)
MODULE_SPARE_H	EQU	(MODULE_HASH_H+2)
MODULE_NAME	EQU	(MODULE_SPARE_H+2)
	IF CPU = PPC
MODULE_R2_SAVE	EQU	(MODULE_NAME+32)
MODULE_R13_SAVE	EQU	(MODULE_R2_SAVE+4)
	ELSE
MODULE_R13_SAVE	EQU	(MODULE_NAME+28)
	ENDIF
SIZEOF_MODULE_CB	EQU	(MODULE_R13_SAVE+4)
MODULE_INFO_MID	EQU	0
MODULE_INFO_NAME	EQU	4
MODULE_INFO_TEXT	EQU	(MODULE_INFO_NAME+SC_MODULE_NAME_SIZE+1)
MODULE_INFO_TEXTSIZE	EQU	(MODULE_INFO_TEXT+4)
MODULE_INFO_DATA	EQU	(MODULE_INFO_TEXTSIZE+4)
MODULE_INFO_DATASIZE	EQU	(MODULE_INFO_DATA+4)
MODULE_INFO_MAX_PROCESS	EQU	(MODULE_INFO_DATASIZE+4)
MODULE_INFO_NPROCESS	EQU	(MODULE_INFO_MAX_PROCESS+4)
MODULE_INFO_MAX_POOLS	EQU	(MODULE_INFO_NPROCESS+4)
MODULE_INFO_NPOOLS	EQU	(MODULE_INFO_MAX_POOLS+4)
SIZEOF_MODULE_INFO	EQU	(MODULE_INFO_NPOOLS+4)
SC_ILLEGAL_MID	EQU	(-2)
SC_CURRENT_MID	EQU	(-1)
;********************************************************************
;  _______  _______ _________ _______  _______ _________ _______   **
; (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
; | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
; | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
; (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
;       ) || |         | |   | |   | || (         | |   | (   ) |  **
; /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
; \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
;                                                                  **
; (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
;                                                                  **
;********************************************************************
; ID: S01103BS48                                                   **
; +Revision: 2.40.14.1 +                                                **
; +Date: 2009/10/04 09:53:43 +                                     **
; Message definitions and API                                      **
;********************************************************************
; generated
SC_MSGRX_PID	EQU	0x01
SC_MSGRX_PID_POS	EQU	0
SC_MSGRX_MSGID	EQU	0x02
SC_MSGRX_MSGID_POS	EQU	1
SC_MSGRX_BOTH	EQU	(SC_MSGRX_PID+SC_MSGRX_MSGID)
SC_MSGRX_NOT	EQU	0x04
SC_MSGRX_NOT_POS	EQU	2
	IF SC_MSG_HOOK = 1
SC_SET_MSGTX_HOOK	EQU	(1)
SC_SET_MSGRX_HOOK	EQU	(2)
	ENDIF
	;  flags for msgTx 
SC_MSGTX_OBSERVE	EQU	0x0001
SC_MSGTX_OBSERVE_POS	EQU	0
MSG_LIST	EQU	0
MSG_SIZE	EQU	8
MSG_PLID	EQU	12
MSG_BUFSIZE_IDX	EQU	16
MSG_SENDER	EQU	20
MSG_ADDRESSEE	EQU	24
MSG_OWNER	EQU	28
MSG_ID	EQU	32
MSG_HEAD	EQU	MSG_ID
TMOMSG_LIST	EQU	0
TMOMSG_SIZE	EQU	8
TMOMSG_PLID	EQU	12
TMOMSG_TMO	EQU	16
TMOMSG_SENDER	EQU	20
TMOMSG_TMOLIST	EQU	24
TMOMSG_ID	EQU	32
TMOMSG_HEAD	EQU	MSG_ID
;********************************************************************
;  _______  _______ _________ _______  _______ _________ _______   **
; (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
; | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
; | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
; (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
;       ) || |         | |   | |   | || (         | |   | (   ) |  **
; /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
; \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
;                                                                  **
; (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
;                                                                  **
;********************************************************************
; ID: S01103BS8                                                    **
; +Revision: 2.45.4.1 +                                                **
; +Date: 2009/10/04 09:53:43 +                                     **
; Process API                                                      **
;********************************************************************
; generated
	IF SC_PROC_HOOK = 1
SC_SET_PROCCREATE_HOOK	EQU	(1)
SC_SET_PROCKILL_HOOK	EQU	(2)
SC_SET_PROCSWAP_HOOK	EQU	(3)
SC_SET_PROCMMU_HOOK	EQU	(4)
	ENDIF
SC_PROCKILL_KILL	EQU	0x4000
SC_PROCKILL_KILL_POS	EQU	14
SC_PDB_STATE_RUN	EQU	1
SC_PDB_STATE_STP	EQU	0
	;  extended type information 
SC_PROC_TYPE_EXT_FPU	EQU	1
SC_PROC_TYPE_EXT_USR	EQU	2
SC_PROC_TYPE_EXT_FPU_POS	EQU	0
SC_PROC_TYPE_EXT_USR_POS	EQU	1
;********************************************************************
;  _______  _______ _________ _______  _______ _________ _______   **
; (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  **
; | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  **
; | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  **
; (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  **
;       ) || |         | |   | |   | || (         | |   | (   ) |  **
; /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  **
; \_______)(_______/\_______/(_______)|/          )_(   |/     \|  **
;                                                                  **
; (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         **
;                                                                  **
;********************************************************************
; ID: S01103BS49                                                   **
; +Revision: 2.48.12.1 +                                                **
; +Date: 2009/08/14 04:43:38 +                                     **
; Process control block definitions                                **
;********************************************************************
; generated
SC_PROC_NAME_SIZE	EQU	31
ST_TMO	EQU	0x0001  	;  a timeout occured 
ST_TMO_POS	EQU	0
ST_TRIGGER	EQU	0x0002  	;  process has been triggered 
ST_READY	EQU	0x000f
ST_READY_POS	EQU	3
ST_WAIT_MSGALLOC	EQU	0x0010  	;  process waits for a free msg 
ST_WAIT_MSGALLOC_POS	EQU	4
ST_WAIT_MSGRX	EQU	0x0020
ST_WAIT_MSGRX_POS	EQU	5
ST_WAIT_TRIGGER	EQU	0x0040
ST_WAIT_TRIGGER_POS	EQU	6
ST_TMO_MASK	EQU	0xff0f
ST_NOT_TMO_MASK	EQU	0x00f0
ST_WAIT_TMO	EQU	0x0080  	;  process wait for an timeout 
ST_WAIT_TMO_POS	EQU	7
ST_STOPPED	EQU	0x0200  	;  process is stopped 
ST_STOPPED_POS	EQU	9
ST_NOT_READY_MASK	EQU	0xfff0
PCB_FLG_READY	EQU	0x0001  	;  process is ready 
PCB_FLG_READY_POS	EQU	0
PCB_FLG_IS_TIMER	EQU	0x0002  	;  process is a timer 
PCB_FLG_IS_TIMER_POS	EQU	1
PCB_FLG_IS_IRQ	EQU	0x0004  	;  process is an interrupt 
PCB_FLG_IS_IRQ_POS	EQU	2 
PCB_FLG_IS_FIRQ	EQU	0x0008  	;  process is a fast interrupt 
PCB_FLG_IS_FIRQ_POS	EQU	3 
PCB_FLG_IS_UIRQ	EQU	0x0010  	;  process is a user interrupt 
PCB_FLG_IS_UIRQ_POS	EQU	4 
PCB_FLG_IS_PRIO	EQU	0x0020  	;  process is a prio. process 
PCB_FLG_IS_PRIO_POS	EQU	5
PCB_FLG_IS_STATIC	EQU	0x0040  	;  process is a static process 
PCB_FLG_IS_STATIC_POS	EQU	6
PCB_FLG_WAKEUP_DIS	EQU	0x0080  	;  wakeup not allowed 
PCB_FLG_WAKEUP_DIS_POS	EQU	7
PCB_FLG_WAKEUP_BY_TX	EQU	0x0100  	;  txMsg (CKernel) 
PCB_FLG_WAKEUP_BY_Tx_POS	EQU	8
PCB_FLG_FPU	EQU	0x0100  	;  process may use FPU 
PCB_FLG_FPU_POS	EQU	8
PCB_FLG_WAKEUP_BY_TRG	EQU	0x0200  	;  triggger (CKernel) 
PCB_FLG_WAKEUP_BY_TRG_POS	EQU	9
PCB_FLG_IS_CONNECTOR	EQU	0x0400  	;  process is a connector 
PCB_FLG_IS_CONNECTOR_POS	EQU	10
PCB_FLG_IS_IDLE	EQU	0x0800  	;  idle process 
PCB_FLG_IS_IDLE_POS	EQU	11
PCB_FLG_IS_RP	EQU	0x1000  	;  process is reverse-proxy 
PCB_FLG_IS_RP_POS	EQU	12
PCB_FLG_ACTIVE	EQU	0x4000  	;  timer is active 
PCB_FLG_ACTIVE_POS	EQU	14
PCB_FLG_PCB_LOCKED	EQU	0x8000
PCB_FLG_PCB_LOCKED_POS	EQU	15
SC_PCB_MAGIC	EQU	(SC_MSG_BASE + 0x2207)
SC_PCB_MAGIC_STATIC	EQU	(SC_MSG_BASE + 0x3008)
PCB_MAGIC	EQU	0
PCB_FLAGS_H	EQU	4
PCB_STATE_H	EQU	6
PCB_STOPPED_H	EQU	8
PCB_HASH_H	EQU	10
	;  prio process 
PCB_PRIO_B	EQU	12
PCB_PRIO0_B	EQU	13
PCB_DEFAULTPOOL_H	EQU	14
PCB_CURSLICE	EQU	16
PCB_PRI_SLICE	EQU	20
PCB_PRI_TMO	EQU	24
	;  timer process 
PCB_TIMSPARE_H	EQU	12
PCB_TIM_DEFAULTPOOL_H	EQU	14
PCB_INITIAL	EQU	16
PCB_SLICE	EQU	20
PCB_TMO	EQU	24
	;  interrupt 
PCB_NESTED_H	EQU	12
PCB_INT_DEFAULTPOOL_H	EQU	14
PCB_INTSPARE2	EQU	16
PCB_VECTOR	EQU	20
PCB_INTSPARE3	EQU	24
PCB_MSGQUEUE	EQU	28
PCB_ALLOCED	EQU	36
PCB_CONNECTED	EQU	44
PCB_PCBLIST	EQU	52
PCB_SLEEPERS	EQU	60
PCB_PID	EQU	68
PCB_STACKPTR	EQU	72
PCB_STACKBTM	EQU	76
PCB_STACKTOP	EQU	80
PCB_ERRORCODE	EQU	84
PCB_ENTRY	EQU	88
PCB_TRIGGER_VAL	EQU	92
PCB_NQUEUE	EQU	96
PCB_NALLOC	EQU	100
PCB_LOCALS	EQU	104
PCB_PRIO_MASK	EQU	108
PCB_READYLIST	EQU	112
PCB_NAME	EQU	116
PCB_CLINE	EQU	120
PCB_CFILE	EQU	124
SIZEOF_PCB	EQU	128
; end of converted files
	; SOCNV
;
; ID: S03352BS1
; +Date: 2009/03/26 08:06:15 +
; +Revision: 1.4.12.1 +
;
; -*-asm-*-
;**************************************************************************
; macro.h
;
; 1 specifc helper macros
;
; S02338BS2
;
; +Date: 2009/08/14 04:43:04 +
; +Revision: 1.44.2.5 +
;
;**************************************************************************
;
	IF :DEF: BIG_ENDIAN
LITTLE_ENDIAN EQU 0
	ELSE
LITTLE_ENDIAN EQU 1
	ENDIF
	MACRO
	WEAK $name
	EXTERN $name[WEAK]
	MEND
	MACRO
	SC_TEXT
	AREA |.text.krn|,CODE,READONLY,ALIGN=2
	CODE32
	MEND
	MACRO
	SC_BSS
	AREA |sc_bss|,NOINIT,ALIGN=3
	MEND
	MACRO
$label SC_FUNC $name
	AREA sciopta_$name,CODE,ALIGN=2
	IF CPUTYPE <= 4
	CODE32
	ELSE
	THUMB
	ENDIF
$name PROC
	EXPORT $name
	MEND
	MACRO
	SC_PUBLIC $name
	EXPORT $name
	IF CPUTYPE <= 4
	CODE32
	ELSE
	THUMB
	ENDIF
$name
	MEND
	MACRO
	SC_TPUBLIC $name
	EXPORT $name
	IF CPUTYPE <= 4
	CODE16
	ELSE
	THUMB
	ENDIF
$name
	MEND
	MACRO
	SC_TFUNC $name
	AREA sciopta_$name,CODE,ALIGN=2
	IF CPUTYPE <= 4
	CODE16
	ELSE
	THUMB
	ENDIF
$name PROC
	EXPORT $name
	MEND
	MACRO
	LSC_TFUNC $name,$para
	IF SC_LONGJUMP = 1
	AREA sciopta_l$name,CODE,ALIGN=2
	CODE16
l$name PROC
	EXPORT l$name
	IF $para > 4
	push {r0}
	ldr r0,llit$name
	mov r12,r0
	pop {r0}
	bx r12
	ELSE
	ldr r3,llit$name
	bx r3
	ENDIF
llit$name DCD $name
	ENDIF
	MEND
	MACRO
	       XSC_TFUNC $name,$off
	IF SC_CDEBUG = 1
	       AREA sciopta_x$name,CODE,ALIGN=2
	IF CPUTYPE <= 4
	       CODE16
	ELSE
	THUMB
	ENDIF
x$name PROC
	       EXPORT x$name
	IF $off = -4
	       mov r12,r0
	       ldr r0,xlit$name
	       ldr r0,[r0,#SC_CURPCB]
	cmp r0,#0
	beq m1$name
	       str r3,[r0,#120]
	       ldr r3,[sp,#0]
	       str r3,[r0,#124]
m1$name
	ldr r3,xlit1$name
	       mov r0,r12
	bx r3
	ELSE
	IF $off = -8
	       mov r12,r0
	       ldr r0,xlit$name
	       ldr r0,[r0,#SC_CURPCB]
	cmp r0,#0
	beq m1$name
	       str r2,[r0,#120]
	       str r3,[r0,#124]
m1$name
	ldr r2,xlit1$name
	       mov r0,r12
	bx r2
	ELSE
	IF $off = -12
	       ldr r3,xlit$name
	       ldr r3,[r3,#SC_CURPCB]
	cmp r3,#0
	beq m1$name
	       str r1,[r3,#120]
	       str r2,[r3,#124]
m1$name
	ldr r3,xlit1$name
	bx r3
	ELSE
	IF $off = -16
	       ldr r2,xlit$name
	       ldr r2,[r2,#SC_CURPCB]
	cmp r2,#0
	beq m1$name
	       str r0,[r2,#120]
	       str r1,[r2,#124]
m1$name
	ldr r2,xlit1$name
	bx r2
	ELSE
	       push {r0-r1}
	       ldr r0,xlit$name
	       ldr r0,[r0,#SC_CURPCB]
	cmp r0,#0
	beq m1$name
	       ldr r1,[sp,#8+$off]
	       str r1,[r0,#120]
	       ldr r1,[sp,#8+$off+4]
	       str r1,[r0,#124]
m1$name
	ldr r0,xlit1$name
	mov r12,r0
	       pop {r0-r1}
	bx r12
	ENDIF
	ENDIF
	ENDIF
	ENDIF
	       ALIGN 4
xlit$name DCD sciopta
xlit1$name DCD $name
	       ENDP
	ENDIF
	       MEND
	MACRO
	       XXSC_TFUNC $name,$off
	IF SC_CDEBUG = 1
	       AREA sciopta_xx$name,CODE,ALIGN=2
	IF CPUTYPE <= 4
	       CODE16
	ELSE
	THUMB
	ENDIF
xx$name PROC
	       EXPORT x$name
	IF $off = -4
	       mov r12,r0
	       ldr r0,xxlit$name
	       ldr r0,[r0,#SC_CURPCB]
	cmp r0,#0
	beq m2$name
	       str r3,[r0,#120]
	       ldr r3,[sp,#12]
	       str r3,[r0,#124]
m2$name
	ldr r3,xxlit1$name
	       mov r0,r12
	bx r3
	ELSE
	IF $off >= 0
	       push {r0-r1}
	       ldr r0,xxlit$name
	       ldr r0,[r0,#SC_CURPCB]
	cmp r0,#0
	beq m2$name
	       ldr r1,[sp,#20+$off]
	       str r1,[r0,#120]
	       ldr r1,[sp,#20+$off+4]
	       str r1,[r0,#124]
m2$name
	ldr r0,xxlit1$name
	mov r12,r0
	       pop {r0-r1}
	bx r12
	ELSE
	ERROR
	ENDIF
	ENDIF
	       ALIGN 4
xxlit$name DCD sciopta
xxlit1$name DCD $name
	       ENDP
	ENDIF
	       MEND
	MACRO
	SC_ENDFUNC $name
	ALIGN 4
	LTORG
	ENDP
	MEND
	MACRO
	RET_R0
	IF CPUTYPE >= 2
	pop {r0,pc}
	ELSE
	pop {r0,r1}
	bx r1
	ENDIF
	MEND
	MACRO
	RET $mask
	IF CPUTYPE >= 2
	pop {$mask,pc}
	ELSE
	pop {$mask}
	pop {r3}
	bx r3
	ENDIF
	MEND
	MACRO
	ARET $mask
	IF CPUTYPE >= 2
	ldmfd sp!,{$mask,pc}
	ELSE
	ldmfd sp!,{$mask,lr}
	bx lr
	ENDIF
	MEND
	MACRO
	EXIT
	IF CPUTYPE >= 2
	pop {pc}
	ELSE
	pop {r3}
	bx r3
	ENDIF
	MEND
	MACRO
	AEXIT
	IF CPUTYPE >= 2
	ldmfd sp!,{pc}
	ELSE
	ldmfd sp!,{lr}
	bx lr
	ENDIF
	MEND
	MACRO
	SWAP_OUT
	IF CPUTYPE > 4
	bl swap_out
	ELSE
	IF CPUTYPE >= 2
	blx swap_out32
	ELSE
	bl swap_out
	ENDIF
	ENDIF
	MEND
	MACRO
	SWAP_OUT_SAVE $mask
	push $mask
	IF CPUTYPE > 4
	bl swap_out
	ELSE
	IF CPUTYPE >= 2
	blx swap_out32
	ELSE
	bl swap_out
	ENDIF
	ENDIF
	pop $mask
	MEND
	MACRO
	LIT $name,$value
	ALIGN
$name DCD $value
	MEND
	MACRO
	SET $name,$value
$name EQU $value
	MEND
	MACRO
	BLX_R2_t
	IF CPUTYPE >= 2
	blx r2
	ELSE
	bl via_r2_t
	ENDIF
	MEND
	MACRO
	BLX_R2_a
	IF CPUTYPE >= 2
	blx r2
	ELSE
	mov lr,pc
	bx r2
	ENDIF
	MEND
	MACRO
	BLXEQ_R2_a
	IF CPUTYPE >= 2
	blxeq r2
	ELSE
	moveq lr,pc
	bxeq r2
	ENDIF
	MEND
	MACRO
	BLXNE_R2_a
	IF CPUTYPE >= 2
	blxne r2
	ELSE
	movne lr,pc
	bxne r2
	ENDIF
	MEND
	MACRO
	BLX_R12_a
	IF CPUTYPE >= 2
	blx r12
	ELSE
	mov lr,pc
	bx r12
	ENDIF
	MEND
dbl_prev EQU 0
dbl_next EQU 4
	MACRO
	LOCK
	IF CPUTYPE > 4
	bl cpu_lock_r12
	ELSE
	IF CPUTYPE >= 2
	blx cpu_lock_r12_a
	ELSE
	bl cpu_lock_r12
	ENDIF
	ENDIF
	MEND
	MACRO
	UNLOCK
	IF CPUTYPE > 4
	bl cpu_unlock_r12
	ELSE
	IF CPUTYPE >= 2
	blx cpu_unlock_r12_a
	ELSE
	bl cpu_unlock_r12
	ENDIF
	ENDIF
	MEND
; ***************************************************************************
; * IAR5
; ***************************************************************************
; SOCNV
;
; void dbl_tailAdd(dbl_t *head, dbl_t *item)
;
; static inline void dbl_tailAdd (dbl_t * head, dbl_t * item)
; {
; item->next = head;
; item->prev = head->TAIL;
;
; head->TAIL->next = item;
; head->TAIL = item;
; }
;
; Remove an item from the beginning of the list.
;
; static inline dbl_t *dbl_headDel (dbl_t * head)
; {
; dbl_t *item = head->HEAD;
; if (item = head) {
; return (dbl_t *) 0;
; }
; head->HEAD = item->next;
; head->HEAD->prev = head
; item->next = 0;
; item->prev = 0;
; return item;
; }
;
; Remove an item from the list.
;
; static inline void dbl_del (dbl_t * pos)
; {
; pos->prev->next = pos->next;
; pos->next->prev = pos->prev;
; }
;
;**********************************************************
; 1 mode macros
;**********************************************************
;
	INCLUDE machine/arm/arm_asm.inc
	; sanity checks
	IF SC_MAX_MODULE > 128 || SC_MAX_MODULE = 0
	INFO 4,"SC_MAX_MODULE out of range: 1..127"
	ENDIF
	IF SC_MAX_CONNECTOR > 128
	INFO 4,"SC_MAX_CONNECTOR out of range: 0..127"
	ENDIF
;
; 220349 is the Version of armasm 2.2 Build 349
;
	IF {ARMASM_VERSION} > 220348
	 PRESERVE8
	ENDIF
	IF SC_MAX_CONNECTOR > 0
	EXTERN sc_connectors
	ENDIF
	EXTERN sc_modules
	;
	; Reset entry
	;
;
; Cannot use WEAK for resethook. armlink will remove sections with
; WEAK imported symbols.
;
	IMPORT reset_hook
	IMPORT __main
	WEAK start_hook
	WEAK user_swi_handler
;
;**************************************************************************
; 1 Kernel
;
; S02338BS1
;
; +Date: 2009/10/08 07:20:59 +
; +Revision: 1.57.2.9 +
;**************************************************************************
;
;
; Domains: Startup check, und nur die Domains eintragen die auch
; wirklich R0 bzw RW sind.
;
	IF CPUTYPE <= 4
;
;**************************************
; Kernel entry
;**************************************
;
	SC_FUNC _start
;
; Call external function reset_hook if it exists.
;
	ldr r0,=reset_hook
	cmp r0,#0
	movne lr,pc
	bxne r0
;
; Call dispatcher if reset_hook returns !=0 (warm start)
; else continue (cold start).
;
	ldr r1,=sc_sysProcDispatcher
	cmp r0,#0
	bxne r1
;
; Call external function __main if it exists
; else continue with kernel main function.
;
	ldr r0,=__main
	cmp r0,#0
	ldreq r0,=main
	bx r0
	SC_ENDFUNC _start
;
;**************************************
; System SWI handler
;
; SWI 0..3 for sciopta.
; Call user-swi handler if exist if SWI > 3
;**************************************
;
	SC_FUNC sc_sysSWI
	mrs r12,spsr	; get saved CPU state
	tst r12,#0x20	; is it thumb mode ?
	ldrhne r12,[lr,#-2]	; yes, swi only 16bit
	bicne r12,r12,#0xff00
	ldreq r12,[lr,#-4]	; no, swi 32bit
	biceq r12,r12,#0xff000000
	cmp r12,#4	; sciopta only
	blo ssw1
;
; If a user-swi handler is defined, call it, if not
; is it a fatal error to use SWI
;
	ldr r12,=user_swi_handler	; user swi handler defined ?
	cmp r12,#0
	bxne r12	; yes, call it
ssw_error
	ldr r0,=(0x05000000|(0x017<<12)|(0x04|0x02|0x01))
	mov r1,lr
ssw_error_cont
	IF CPUTYPE >= 2
	blx sc_sysError
	ELSE
	ldr lr,=sc_sysError
	bx lr
	ENDIF
ssw1
	cmp r12,#0
	beq sc_sysFakeInt
	IF SC_USE_TRAP = 1
;
; Handle syscall-SWI
; syscall-number behind the SWI opcode
; If bit 7 set, call to CDEBUG-function
;
	cmp r12,#1
	bne ssw_error
	ldrh r12,[lr,#0]	; get function-number (behind SWI)
	mrs r5,spsr	; save caller's state
	; switch to sys-mode (leave I bit untouched)
	and r4,r5,#PSR_I_BIT
	orr r4,r4,#PSR_SYS_MODE
	msr cpsr_c,r4
	mov r4,lr	; save return address
	IF SC_PROCMMU_HOOK = 1
	mrc p15,0,r6,c3,c0,0	; save current domains
	mov lr,#-1
	mcr p15,0,lr,c3,c0,0	; master in all domains
	ENDIF
	; check function number and load function address
	IF SC_CDEBUG = 1
	tst r12,#0x80
	ldrne lr,=xsyscall_tab	; use xsc_-function if bit 7 set
	ldreq lr,=syscall_tab
	ELSE
	ldr lr,=syscall_tab
	ENDIF
	and r12,r12,#0x7f	; always clear bit 7
	cmp r12,#0x63
	bhi ssw_error1
	ldr r12,[lr,r12,lsl #2]
	cmp r12,#0
	beq ssw_error1
	; call function
	BLX_R12_a
	IF SC_PROCMMU_HOOK = 1
	mcr p15,0,r6,c3,c0,0	; restore caller's domains
	ENDIF
	bic r5,r5,#0x20	; clear Thumb state
	msr cpsr_cxsf,r5	; switch back to caller's mode
	mov r12,r4
	ldmfd sp!,{r4-r6}
	bx r12	; return and set Thumb state
	   	; (if set before)
ssw_error1
	ldr r0,=(0x06000000|(0x017<<12)|(0x04|0x02|0x01))
	mov r1,r12
	IF CPUTYPE >= 2
	blx sc_sysError
	ELSE
	b ssw_error_cont
	ENDIF
	ELSE	; SC_USE_TRAP = 1
	b ssw_error
	ENDIF	; SC_USE_TRAP = 1
;
;**************************************
; Software interrupt
; r0 - kind ( 1 => msgTX
; 2 => Trigger
; -1 => init
; -2 => exit
; r1 - pcb
;**************************************
;
	SC_PUBLIC sc_sysFakeInt
	mrs r2,spsr
	mov r3,lr
	sub sp,sp,#32
	stmia sp,{r2-r7,sp-lr}^
	msr cpsr_c,#PSR_SYS_MODE|PSR_I_BIT
	ldr sp,=sc_irq_stack
	ldr r4,=sciopta
	ldr r5,[r4,#SC_CURPCB]
	ldr r6,[r4,#SC_CURMODULE]
	str r1,[r4,#SC_CURPCB]
	ldr r2,[r1,#PCB_PID]
	str r2,[r4,#SC_CURPID]
	mov r2,r2,lsr #24
	ldr r3,=sc_modules
	ldr r3,[r3,r2,lsl #2]
	cmp r3,r6
	beq sswi_nochange1
	str r3,[r4,#SC_CURMODULE]
	IF SC_PROC_HOOK = 1
	IF SC_PROCMMU_HOOK = 1
	mrc p15,0,r7,c3,c0,0	; save old domains
	mov r3,#3
	adds r2,r2,r2
	mov r3,r3,lsl r2
	movne r2,#0x40000001	; client in global and system domain
	ldreq r2,=0x55555555	; client in all domains
	orr r2,r3,r2
	mcr p15,0,r2,c3,c0,0
	ENDIF
	ENDIF
sswi_nochange1
	ldr r2,[r1,#PCB_ENTRY]
	ldr sp,[r1,#PCB_STACKPTR]
	ldr r1,[r1,#PCB_VECTOR]
	BLX_R2_a
	ldr r1,[r4,#SC_CURMODULE]
	cmp r1,r6
	strne r6,[r4,#SC_CURMODULE]
	IF SC_PROC_HOOK = 1
	IF SC_PROCMMU_HOOK = 1
	mcrne p15,0,r7,c3,c0,0	; restore old domains
	ENDIF
	ENDIF
	ldr r0,[r5,#PCB_PID]
	str r5,[r4,#SC_CURPCB]
	str r0,[r4,#SC_CURPID]
	msr cpsr_c,#PSR_SVC_MODE+PSR_I_BIT
	ldmia sp,{r2-r7,sp-lr}^
	msr spsr_cxsf,r2
	add sp,sp,#32
	movs pc,r3
	SC_ENDFUNC sc_sysSWI
;
;**************************************
; Interrupt dispatcher
; r0 - vector
;**************************************
;
	SC_FUNC sc_sysIrqDispatcher
	ldr r2,=sc_irq_vectors
	IF SC_PROC_PARA_CHECK = 1
	cmp r0,#SC_MAX_INT_VECTORS
	bhs idp_error
	ENDIF
	ldr r3,[r2,r0,lsl #2]	; get pcb
	IF SC_PROC_PARA_CHECK = 1
	cmp r3,#0
	beq idp_error
	ENDIF
	ldr sp,[r3,#PCB_STACKTOP]	; get stackpointer
	IF SC_PROC_PARA_CHECK = 1
	ldrsh r1,[r3,#PCB_NESTED_H]
	adds r1,r1,#1
	bne idp_error1
	strh r1,[r3,#PCB_NESTED_H]
	ENDIF
	stmfd sp!,{r4,r5,r6,lr}
	ldr r4,=sciopta
	ldr r1,[r3,#PCB_PID]
	ldmia r4,{r2,r5}	; r2 = curpid	; r5 = curpcb
	ldr r6,[r4,#SC_CURMODULE]
	stmia r4,{r1,r3}	; set curpid and curpcb
	mov r1,r1,lsr #24
	eors r2,r1,r2,lsr #24	; check if cur. module = ISR module
	beq idp1
	ldr r2,=sc_modules
	ldr r2,[r2,r1,lsl #2]
	str r2,[r4,#SC_CURMODULE]
	IF SC_PROC_HOOK = 1
	IF SC_PROCMMU_HOOK = 1
	mov r2,#3
	adds r1,r1,r1
	mov r2,r2,lsl r1
	movne r1,#0x40000001	; client in global and system domain
	ldreq r1,=0x55555555	; client in all domains
	orr r1,r2,r1
	mcr p15,0,r1,c3,c0,0
	ENDIF
	ENDIF
idp1
	mov r1,r0	; vector as second parameter
	mov r0,#0
	ldr r2,[r3,#PCB_ENTRY]
	BLX_R2_a
	msr cpsr_c,#PSR_SYS_MODE|PSR_I_BIT	; disable ints again
	IF SC_PROC_HOOK = 1
	IF SC_PROCMMU_HOOK = 1
	mov r0,#-1
	mcr p15,0,r0,c3,c0,0
	ENDIF
	ENDIF
	ldr r3,[r4,#SC_CURPCB]
	IF SC_PROC_PARA_CHECK = 1
	ldrh r0,[r3,#PCB_NESTED_H]
	sub r0,r0,#1
	strh r0,[r3,#PCB_NESTED_H]
	ENDIF
	ldr r0,[r5,#PCB_PID]
	ldr r1,[r4,#SC_CURMODULE]
	stmia r4,{r0,r5}	; restore curpid and curpcb
	str r6,[r4,#SC_CURMODULE]
	ldmfd sp!,{r4,r5,r6,pc}
	IF SC_PROC_PARA_CHECK = 1
idp_error1
	mov r1,r0
	ldr r0,=((0x019<<12)|0x01)
	ldr r3,=sc_sysError
	bx r3
idp_error
	mov r1,r0
	bl sc_sysIllegalIrq
	ENDIF
	SC_ENDFUNC sc_sysIrqDispatcher
;
;**************************************
; Sciopta main function
;**************************************
;
	SC_FUNC main
;
; Initialize interrupt mode stack
;
	msr cpsr_c,#PSR_IRQ_MODE+PSR_I_BIT
	ldr sp,=sc_irq_stack
;
; Initialize supervisor stack
;
	msr cpsr_c,#PSR_SVC_MODE+PSR_I_BIT
	ldr sp,=sc_svc_stack
;
; Clear kernel local data (BSS segment)
;
	ldr r0,=__kernel_bss_start
	ldr r1,=__kernel_bss_size
	mov r2,#0
main1
	subs r1,r1,#4
	str r2,[r0],#4
	bne main1
;
; Setup configuration word
;
	mov r0,#SC_MAX_MODULE
	orr r0,r0,#SC_MAX_CONNECTOR<<8
	IF SC_MSG_STAT = 1
	orr r0,r0,#SC_CONF_MSG_STAT
	ENDIF
	IF SC_MSG_CHECK = 1
	orr r0,r0,#SC_CONF_MSG_CHECK
	ENDIF
	IF SC_PROC_STAT = 1
	orr r0,r0,#SC_CONF_PROC_STAT
	ENDIF
	IF SC_MAX_NUM_BUFFERSIZES = 8
	orr r0,r0,#0x10000
	ENDIF
	IF SC_MAX_NUM_BUFFERSIZES = 16
	orr r0,r0,#0x20000
	ENDIF
	IF SC_POOL_HOOK = 1
	IF SC_POOLCREATE_HOOK = 1
	orr r0,r0,#SC_CONF_PL_CR_HOOK
	ENDIF
	IF SC_POOLKILL_HOOK = 1
	orr r0,r0,#SC_CONF_PL_KL_HOOK
	ENDIF
	ENDIF
	IF SC_MSG_HOOK = 1
	IF SC_MSGTX_HOOK = 1
	orr r0,r0,#SC_CONF_MS_TX_HOOK
	ENDIF
	IF SC_MSGRX_HOOK = 1
	orr r0,r0,#SC_CONF_MS_RX_HOOK
	ENDIF
	ENDIF
	IF SC_PROC_HOOK = 1
	IF SC_PROCCREATE_HOOK = 1
	orr r0,r0,#SC_CONF_PR_CR_HOOK
	ENDIF
	IF SC_PROCKILL_HOOK = 1
	orr r0,r0,#SC_CONF_PR_KL_HOOK
	ENDIF
	IF SC_PROCSWAP_HOOK = 1
	orr r0,r0,#SC_CONF_PR_SW_HOOK
	ENDIF
	ENDIF
	IF SC_ERR_HOOK = 1
	orr r0,r0,#SC_CONF_ERR_HOOK
	ENDIF
	IF SC_USE_FRIENDS = 1
	orr r0,r0,#SC_CONF_FRIENDS
	ENDIF
	IF SC_STACK_CHECK = 1
	orr r0,r0,#SC_CONF_STACKCHK
	ENDIF
	ldr r11,=sciopta
	str r0,[r11,#SC_CONF]
	ldr r0,=10000
	str r0,[r11,#-8]
;
; Setup readylists
;
	add r0,r11,#SC_READYLIST
	mov r1,#32
main2
	subs r1,r1,#1
	str r0,[r0,#0]
	str r0,[r0,#4]
	add r0,r0,#8
	bne main2
;
; Setup sleeper lists
;
	add r0,r11,#SC_SLEEPERLISTS
	mov r1,#256
main3
	subs r1,r1,#1
	str r0,[r0,#0]
	str r0,[r0,#4]
	add r0,r0,#8
	bne main3
;
; If Async Timeout selected in sconf then setup timeout lists
;
	IF SC_ASYNC_TMO = 1
	mov r1,#256
main4
	subs r1,r1,#1
	str r0,[r0,#0]
	str r0,[r0,#4]
	add r0,r0,#8
	bne main4
	ENDIF
;
; Fill interrupt PCB table with 0
;
	mov r0,#0
	ldr r1,=sc_irq_vectors
	mov r2,#SC_MAX_INT_VECTORS
main5
	subs r2,r2,#1
	str r0,[r1],#4
	bne main5
;
; Call external function start_hook if it exists
;
	ldr r2,=start_hook
	cmp r2,#0
	IF CPUTYPE >= 2
	blxne r2
	ELSE
	movne lr,pc
	bxne r2
	ENDIF
;
; Call external function TargetSetup:
; This function creates the system module
; and the system module init process.
;
; TargetSetup is in file sconf.c
; which is generated by Sciopta config program
;
	IMPORT TargetSetup
	ldr r2,=TargetSetup
	BLX_R2_a
;
; Set current module
;
	ldr r1,=sc_modules
	ldr r0,[r1,#0]
	str r0,[r11,#SC_CURMODULE]
;
; Switch processor mode to SYS with disabled interrupts
;
	MSR cpsr_c,#PSR_SYS_MODE|PSR_I_BIT
;
; Starts 0, calls the dispatcher
;
	ldr r0,=sc_sysProcDispatcher
	bx r0
;
; Public labels for version info.
;
	SC_PUBLIC sciopta_cpu
	DATA
	DCD CPU
	SC_PUBLIC sciopta_version
	DATA
	DCB 0x1,0x9,0x6,0x8
	SC_ENDFUNC main
;
;**************************************
; Veneer function for
; thumb => arm
; arm => thumb
;**************************************
;
	IF CPUTYPE >= 2
	ELSE
	SC_TFUNC via_r2_t
	bx r2
	SC_ENDFUNC via_r2_t
	ENDIF
;
;**************************************
; disable interrupts and return old mask
;**************************************
;
	SC_TFUNC sc_sysLock
	bx pc
	ALIGN
	CODE32
	EXPORT sc_sysLock_a
sc_sysLock_a
	mrs r0,cpsr
	orr r1,r0,#PSR_I_BIT
	msr cpsr_c,r1
	bx lr
	SC_ENDFUNC sc_sysLock
;
;**************************************
; restore interrupt mask
;**************************************
;
	SC_TFUNC sc_sysUnlock
	bx pc
	ALIGN
	CODE32
	EXPORT sc_sysUnlock_a
sc_sysUnlock_a
	and r0,r0,#PSR_I_BIT
	mrs r1,cpsr
	bic r1,r1,#PSR_I_BIT
	orr r1,r1,r0
	msr cpsr_c,r1
	bx lr
	SC_ENDFUNC sc_sysUnlock
;
;**************************************
; dummy interrupt
; r0 = 0
; r1 - irq vector
;**************************************
;
	SC_FUNC sc_sysIllegalIrq
	ldr r0,=((0x016<<12)|0x01)
	IF CPUTYPE >= 2
	blx sc_sysError
	ELSE
	ldr r3,=sc_sysError
	bx r3
	ENDIF
	SC_ENDFUNC sc_sysIllegalIrq
	SC_TFUNC __rt_errno_addr
	ldr r3,=sciopta
	ldr r3,[r3,#SC_CURPCB]
	mov r0,#PCB_ERRORCODE
	add r0,r3
	bx lr
	SC_ENDFUNC __rt_errno_addr
	SC_TFUNC __rt_raise
	push {lr}
	bl sc_miscError
	EXIT
	SC_ENDFUNC __rt_raise
	SC_TFUNC __sys_exit
	mov r1,r0
	ldr cr0,=(0xfff<<12)|0x01
	bl sc_sysError
	SC_ENDFUNC __sys_exit
	SC_TFUNC __rt_stackheap_init
	movs r0,#0
	movs r1,#0
	bx lr
	SC_ENDFUNC __rt_stackheap_init
	SC_TFUNC __rt_stackheap_extend
	movs r0,#0
	bx lr
	SC_ENDFUNC __rt_stackheap_extend
	SC_TFUNC __user_initial_stackheap
	ldr r0,=heap_s
	ldr r1,=sc_svc_stack_s
	ldr r2,=heap
	ldr r3,=sc_svc_stack
	bx lr
	SC_ENDFUNC __user_initial_stackheap
	SC_TFUNC __user_heap_extend
	movs r0,#0
	bx lr
	SC_ENDFUNC __user_heap_extend
	SC_TFUNC __use_no_heap_region
	bx lr
	SC_ENDFUNC __use_no_heap_region
	SC_TFUNC __ARM_get_argv
	movs r0,#0
	bx lr
	SC_ENDFUNC __ARM_get_argv
;
;**************************************
; Kernel internal lock/unlock functions
;**************************************
;
	SC_TFUNC cpu_lock_r12
	bx pc
	ALIGN
	CODE32
cpu_lock_r12_a
	mrs r12,cpsr
	stmfd sp!,{r12}
	orr r12,r12,#PSR_I_BIT
	msr cpsr_c,r12
	bx lr
	SC_ENDFUNC cpu_lock_r12
	SC_TFUNC cpu_unlock_r12
	bx pc
	ALIGN
	CODE32
cpu_unlock_r12_a
	ldmfd sp!,{r12}
	msr cpsr_c,r12
	bx lr
	SC_ENDFUNC cpu_unlock_r12
;
;**************************************
; Kernel global data
; Kernel Control Block
;**************************************
;
	SC_BSS
	EXPORT sc_irq_stack
	EXPORT sc_irq_vectors
	EXPORT sc_svc_stack
	EXPORT sciopta
	; Note:
	; irq.S relies on this stack-layout
	; and that sciopta follows sc_irq_stack.
	;
__kernel_bss_start
	SPACE 256	; IRQ save
sc_irq_stack
	SPACE 8
	SPACE 4	; eval-counter
	SPACE 4	; 1/ticklength
sciopta
	SPACE SIZEOF_SCIOPTA
	EXPORT ERR_MSG
ERR_MSG
	SPACE 6*4
__kernel_bss_size EQU .-__kernel_bss_start
sc_irq_vectors
	SPACE SC_MAX_INT_VECTORS*4
sc_svc_stack_s
	SPACE SC_KERNELSTACK_SIZE
sc_svc_stack
heap_s
	SPACE 4
heap
	ENDIF
;SOC
;**************************************************************************
; 1 Kernel
;
; S02338BS1
;
; +Date: 2009/10/08 07:20:59 +
; +Revision: 1.4.2.9 +
;**************************************************************************
;EOC
	IF CPUTYPE = 8
;
;**************************************
; Kernel entry
;**************************************
;
	SC_FUNC _start
	cpsid if
	ldr r0,=sc_svc_stack
	msr msp,r0
;
; Call external function reset_hook if it exists.
;
	ldr r0,=reset_hook
	cmp r0,#0
#if defined(__IAR_SYSTEMS_ASM__) && (__VER__ = 5040000)
	; workaround for faulty 5.40 IAR assembler
	beq xx
	blx r0
xx
#else
	it ne
	blxne r0
#endif
;
; Call dispatcher if reset_hook returns !=0 (warm start)
; else continue (cold start).
;
	ldr r1,=sc_sysProcDispatcher
	cmp r0,#0
	it ne
	bxne r1
;
; Call external function __main if it exists
; else continue with kernel main function.
;
	ldr r0,=__main
	cmp r0,#0
	it eq
	ldreq r0,=main
	bx r0
	SC_ENDFUNC _start
;
;**************************************
; System SVC handler
;
; SWI 0..3 for sciopta.
; Call user-swi handler if exist if SWI > 3
; Registers r0-r3,r12,r14 stacked by Core
;**************************************
;
	SC_TFUNC sc_sysSVC
	tst.w lr,#4	; from thread-mode ?
	ite eq
	mrseq r0,msp	; process-mode => r0:= MSP
	mrsne r0,psp	; thread-mode => r0:= PSP
	ldr r1,[r0,#6*4]	; r1:= caller´s return-address
	ldrh r1,[r1,#-2]	; r1:= SVC opcode
	uxtb r1,r1	; r1:= SVC parameter
	cmp r1,#4	; sciopta only
	blo ssw1
;
; If a user-swi handler is defined, call it, if not
; is it a fatal error to use SWI
;
	ldr r12,=user_swi_handler	; user swi handler defined ?
	cmp r12,#0
	itt ne
	ldmfdne r0,{r0-r1}	; restore parameters
	bxne r12	; yes, call it
ssw_error
	ldr r0,=(0x05000000|(0x017<<12)|(0x04|0x02|0x01))
	mov r1,r12
ssw_error_cont
	b.w sc_sysError
ssw1
	cmp r1,#0
	beq sc_sysFakeInt
	IF SC_USE_TRAP = 1
;
; Handle syscall-SWI
; syscall-number behind the SWI opcode
; If bit 7 set, call to CDEBUG-function
;
	cmp r1,#1
	bne ssw_error
	ldr r1,[r0,#5*4]	; caller´s LR
	str r1,[r0,#6*4]	; = new return address
	ldr r1,[r0,#7*4]	; get xPSR
	push {r4-r6,lr}
	mov r6,r12
	IF SC_PROCMMU_HOOK = 1
	; update MPU here
	ENDIF
	; check function number and load function address
	IF SC_CDEBUG = 1
	lsrs r1,r6,#8
	ite cs
	ldrcs r5,=xsyscall_tab	; use xsc_-function if bit 7 set
	ldrcc r5,=syscall_tab
	ELSE
	ldr r5,=syscall_tab
	ENDIF
	and r6,r6,#0x7f	; always clear bit 7
	cmp r6,#0x63
	bhi ssw_error1
	ldr r5,[r5,r6,lsl #2]
	cmp r5,#0
	beq ssw_error1
	; call function
	ldmfd r0,{r0-r1}	; restore parameters
	blx r5
	cpsid i
	IF SC_PROCMMU_HOOK = 1
	; restore caller's MPU settings
	ENDIF
	pop {r4-r6,pc}	; return
ssw_error1
	ldr r0,=(0x06000000|(0x017<<12)|(0x04|0x02|0x01))
	mov r1,r6
	b.w sc_sysError
	ELSE
	b ssw_error
	ENDIF
;
;**************************************
; Software interrupt
; r0 - kind ( 1 => msgTX
; 2 => Trigger
; -1 => init
; -2 => exit
; r1 - pcb
;**************************************
;
	SC_PUBLIC sc_sysFakeInt
	ldmfd r0,{r0-r1}	; restore parameters
	push {r4-r7,lr}
	cpsid i
	ldr r4,=sciopta
	ldr r5,[r4,#SC_CURPCB]
	ldr r6,[r4,#SC_CURMODULE]
	str r1,[r4,#SC_CURPCB]
	ldr r2,[r1,#PCB_PID]
	str r2,[r4,#SC_CURPID]
	lsrs r2,r2,#24
	ldr r3,=sc_modules
	ldr r3,[r3,r2,lsl #2]
	cmp r3,r6
	beq sswi_nochange1
	str r3,[r4,#SC_CURMODULE]
	IF SC_PROC_HOOK = 1
	IF SC_PROCMMU_HOOK = 1
	; save current MPU setting and set IRQ´s
	ENDIF
	ENDIF
sswi_nochange1
	mov r7,sp	; save callers SP
	ldr r2,[r1,#PCB_STACKPTR]
	mov sp,r2
	ldr r2,[r1,#PCB_ENTRY]
	ldr r1,[r1,#PCB_VECTOR]
	blx r2
	cpsid i
	mov sp,r7
	ldr r0,[r5,#PCB_PID]
	ldr r1,[r4,#SC_CURMODULE]
	str r5,[r4,#SC_CURPCB]
	str r0,[r4,#SC_CURPID]
	cmp r1,r6
	beq sswi_nochange2
	str r6,[r4,#SC_CURMODULE]
	IF SC_PROC_HOOK = 1
	IF SC_PROCMMU_HOOK = 1
	; restore callers MPU settings
	ENDIF
	ENDIF
sswi_nochange2
	cpsie i
	pop {r4-r7,pc}
	SC_ENDFUNC sc_sysSVC
;
;**************************************
; Interrupt dispatcher
; r0 - vector
;**************************************
;
	SC_FUNC sc_sysIrqDispatcher
	push {r4-r7,lr}
	ldr r1,=sc_irq_vectors
	mrs r0,IPSR
	IF SC_PROC_PARA_CHECK = 1
	cmp r0,#SC_MAX_INT_VECTORS
	bhs sid_illirq
	ENDIF
	ldr.w r1,[r1,r0,lsl #2]
	ldr r4,=sciopta
	cbz r1,sid_illirq
	ldr r2,[r1,#PCB_PID]
	cpsid i
	ldrh r3,[r4,#SC_NESTED_IRQ_H]
	adds r3,r3,#1
	strh r3,[r4,#SC_NESTED_IRQ_H]
	ldr r5,[r4,#SC_CURPCB]
	ldr r6,[r4,#SC_CURMODULE]
	str r1,[r4,#SC_CURPCB]
	str r2,[r4,#SC_CURPID]
	lsrs r2,r2,#24
	ldr r3,=sc_modules
	ldr r3,[r3,r2,lsl #2]
	cmp r3,r6
	beq sid_nochange1
	str r3,[r4,#SC_CURMODULE]
	IF SC_PROC_HOOK = 1
	IF SC_PROCMMU_HOOK = 1
	; save current MPU setting and set IRQ´s
	ENDIF
	ENDIF
sid_nochange1
	mov r7,sp	; save callers SP
	ldr r2,[r1,#PCB_STACKPTR]
	mov sp,r2
	ldr r2,[r1,#PCB_ENTRY]
	ldr r1,[r1,#PCB_VECTOR]
	mov r1,r0	; r1:= vector
	movs r0,#0
	blx r2
	cpsid i	; disable interrupt (for sure)
	mov sp,r7	; restore SP of inter´ed process
	ldrh r0,[r4,#SC_NESTED_IRQ_H]
	subs r0,r0,#1
	strh r0,[r4,#SC_NESTED_IRQ_H]
	bne sid_no_sched
	ldr r0,[r4,#SC_SCHEDULE_REQ]
	ldr r1,[r4,#SC_SCHEDULER_LOCK]
	cbz r0,sid_no_sched
	cbnz r1,sid_no_sched
	movw r0,#0xed04	; NVIC_ICSR
	movt r0,#0xe000
	mov r1,#0x10000000
	str r1,[r0,#0]	; call pendSV
	;
	; restore interrupted personality, as we might have
	; a back-to-back interrupt coming in before pendSV is
	; handled.
;
sid_no_sched
	movs r2,r5	; set r2 for dispatcher
	ldr r0,[r5,#PCB_PID]	; restore personality
	ldr r1,[r4,#SC_CURMODULE]
	str r5,[r4,#SC_CURPCB]
	str r0,[r4,#SC_CURPID]
	cmp r1,r6
	beq sid_nochange2
	str r6,[r4,#SC_CURMODULE]
	IF SC_PROC_HOOK = 1
	IF SC_PROCMMU_HOOK = 1
	; restore callers MPU settings
	ENDIF
	ENDIF
sid_nochange2
	pop {r4-r7,lr}
	cpsie i
	bx lr
sid_illirq
	b.w sc_sysIllegalIrq
	SC_ENDFUNC sc_sysIrqDispatcher
;
;**************************************
; Sciopta main function
;**************************************
;
	SC_FUNC main
;
; disable all interrupts
;
	cpsid if
;
; Initialize supervisor stack
;
	ldr r0,=sc_svc_stack
	mov sp,r0
;
; Clear kernel local data (BSS segment)
;
	ldr r0,=__kernel_bss_start
	ldr r1,=__kernel_bss_size
	mov r2,#0
main1
	subs r1,r1,#4
	str r2,[r0],#4
	bne main1
;
; Setup configuration word
;
	ldr r7,=sciopta
	movs r0,#SC_MAX_MODULE
	orr r0,r0,#SC_MAX_CONNECTOR<<8
	IF SC_MSG_STAT = 1
	orr r0,r0,#SC_CONF_MSG_STAT
	ENDIF
	IF SC_MSG_CHECK = 1
	orr r0,r0,#SC_CONF_MSG_CHECK
	ENDIF
	IF SC_PROC_STAT = 1
	orr r0,r0,#SC_CONF_PROC_STAT
	ENDIF
	IF SC_MAX_NUM_BUFFERSIZES = 8
	orr r0,r0,#0x10000
	ENDIF
	IF SC_MAX_NUM_BUFFERSIZES = 16
	orr r0,r0,#0x20000
	ENDIF
	IF SC_POOL_HOOK = 1
	IF SC_POOLCREATE_HOOK = 1
	orr r0,r0,#SC_CONF_PL_CR_HOOK
	ENDIF
	IF SC_POOLKILL_HOOK = 1
	orr r0,r0,#SC_CONF_PL_KL_HOOK
	ENDIF
	ENDIF
	IF SC_MSG_HOOK = 1
	IF SC_MSGTX_HOOK = 1
	orr r0,r0,#SC_CONF_MS_TX_HOOK
	ENDIF
	IF SC_MSGRX_HOOK = 1
	orr r0,r0,#SC_CONF_MS_RX_HOOK
	ENDIF
	ENDIF
	IF SC_PROC_HOOK = 1
	IF SC_PROCCREATE_HOOK = 1
	orr r0,r0,#SC_CONF_PR_CR_HOOK
	ENDIF
	IF SC_PROCKILL_HOOK = 1
	orr r0,r0,#SC_CONF_PR_KL_HOOK
	ENDIF
	IF SC_PROCSWAP_HOOK = 1
	orr r0,r0,#SC_CONF_PR_SW_HOOK
	ENDIF
	ENDIF
	IF SC_ERR_HOOK = 1
	orr r0,r0,#SC_CONF_ERR_HOOK
	ENDIF
	IF SC_USE_FRIENDS = 1
	orr r0,r0,#SC_CONF_FRIENDS
	ENDIF
	IF SC_STACK_CHECK = 1
	orr r0,r0,#SC_CONF_STACKCHK
	ENDIF
	str r0,[r7,#SC_CONF]
;
; Setup readylists
;
	add r0,r7,#SC_READYLIST
	movs r1,#32
main2
	str r0,[r0,#0]
	str r0,[r0,#4]
	adds r0,r0,#8
	subs r1,r1,#1
	bne main2
;
; Setup sleeper lists
;
	add r0,r7,#SC_SLEEPERLISTS
	movs r1,#255
main3
	str r0,[r0,#0]
	str r0,[r0,#4]
	adds r0,r0,#8
	subs r1,r1,#1
	bpl main3
;
; If Async Timeout selected in sconf then setup timeout lists
;
	IF SC_ASYNC_TMO = 1
	movs r1,#255
main4
	str r0,[r0,#0]
	str r0,[r0,#4]
	adds r0,r0,#8
	subs r1,r1,#1
	bpl main4
	ENDIF
;
; Fill interrupt PCB table with 0
;
	movs r0,#0
	ldr r1,=sc_irq_vectors
	movs r2,#SC_MAX_INT_VECTORS
main5
	subs r2,r2,#1
	str r0,[r1],#4
	bne main5
	ldr r0,=10000
	str r0,[r7,#-4]
;
; Call external function start_hook if it exists
;
	ldr r2,=start_hook
	cmp r2,#0
#if defined(__IAR_SYSTEMS_ASM__) && (__VER__ = 5040000)
	; workaround for faulty 5.40 IAR assembler
	beq yy
	blx r2
yy
#else
	it ne
	blxne r2
#endif
;
; Call external function TargetSetup:
; This function creates the system module
; and the system module init process.
;
; TargetSetup is in file sconf.c
; which is generated by Sciopta config program
;
	IMPORT TargetSetup
	ldr r2,=TargetSetup
	blx r2
;
; Set current module
;
	ldr r6,=sc_modules
	ldr r0,[r6,#0]	; get first module
	str r0,[r7,#SC_CURMODULE]
;
; Start 0, call the dispatcher
;
	ldr r0,=0xe000ed00
	ldr r1,[r0,#0x20]
	orr r1,r1,#0x00ff0000	; pendSV lowest prio in system
	str r1,[r0,#0x20]
	movs r2,#0	; curPCB = 0 => do not save
	mov r1,#0x10000000
	str r1,[r0,#0x4]	; call pendSV (first dispatch
main6
	cpsie if
	b main6
;
; Public labels for version info.
;
	ALIGN
	SC_PUBLIC sciopta_cpu
	DATA
	DCD CPU
	SC_PUBLIC sciopta_version
	DATA
	DCB 0x1,0x9,0x6,0x8
	SC_ENDFUNC main
;
;**************************************
; disable interrupts and return old mask
;**************************************
;
	SC_TFUNC cpu_lock_r12
	mrs.w r12,PRIMASK
	push.w {r12}
	cpsid i
	bx lr
	SC_ENDFUNC cpu_lock_r12
	SC_TFUNC sc_sysLock
	mrs r0,PRIMASK
	cpsid i
	bx lr
	SC_ENDFUNC sc_sysLock
;
;**************************************
; restore interrupt mask
;**************************************
;
	SC_TFUNC cpu_unlock_r12
	pop.w {r12}
	msr.w PRIMASK,r12
	bx lr
	SC_ENDFUNC cpu_unlock_r12
	SC_TFUNC sc_sysUnlock
	lsrs r0,r0,#1
	it cs
	bxcs lr
	cpsie i
	bx lr
	SC_ENDFUNC sc_sysUnlock
;
;**************************************
; dummy interrupt
; r0 = 0
; r1 - irq vector
;**************************************
;
	SC_FUNC sc_sysIllegalIrq
	mov r1,r0
	ldr r0,=((0x016<<12)|0x01)
	b.w sc_sysError
	SC_ENDFUNC sc_sysIllegalIrq
	SC_TFUNC __rt_errno_addr
	ldr r3,=sciopta
	ldr r3,[r3,#SC_CURPCB]
	mov r0,#PCB_ERRORCODE
	add r0,r3
	bx lr
	SC_ENDFUNC __rt_errno_addr
	SC_TFUNC __rt_raise
	push {lr}
	bl sc_miscError
	EXIT
	SC_ENDFUNC __rt_raise
	SC_TFUNC __sys_exit
	mov r1,r0
	ldr r0,=(0xfff<<12)|0x01
	bl sc_sysError
	SC_ENDFUNC __sys_exit
	SC_TFUNC __rt_stackheap_init
	mov r0,#0
	mov r1,#0
	bx lr
	SC_ENDFUNC __rt_stackheap_init
	SC_TFUNC __rt_stackheap_extend
	mov r0,#0
	bx lr
	SC_ENDFUNC __rt_stackheap_extend
	SC_TFUNC __user_initial_stackheap
	ldr r0,=heap_s
	ldr r1,=sc_svc_stack_s
	ldr r2,=heap
	ldr r3,=sc_svc_stack
	bx lr
	SC_ENDFUNC __user_initial_stackheap
	SC_TFUNC __user_heap_extend
	mov r0,#0
	bx lr
	SC_ENDFUNC __user_heap_extend
	SC_TFUNC __use_no_heap_region
	bx lr
	SC_ENDFUNC __use_no_heap_region
	SC_TFUNC __ARM_get_argv
	mov r0,#0
	bx lr
	SC_ENDFUNC __ARM_get_argv
;
;**************************************
; Kernel global data
; Kernel Control Block
;**************************************
;
	SC_BSS
	EXPORT sc_irq_vectors
	EXPORT sc_svc_stack
	EXPORT sciopta
	; Note:
	; irq.S relies on this stack-layout
	; and that sciopta follows sc_irq_stack.
	;
__kernel_bss_start
	SPACE 8
	SPACE 4	; eval-counter
sciopta
	SPACE SIZEOF_SCIOPTA
	EXPORT ERR_MSG
ERR_MSG
	SPACE 6*4
__kernel_bss_size EQU .-__kernel_bss_start
sc_irq_vectors
	SPACE SC_MAX_INT_VECTORS*4
	ALIGN 8
sc_svc_stack_s
	SPACE SC_KERNELSTACK_SIZE
sc_svc_stack
heap_s
	SPACE 4
heap
	ENDIF	; 8
	;
;********************************************************************
; module.S
;
; Module specific functions
;
; ID: S03104BS1
; +Date: 2009/08/14 04:43:04 +
; +Revision: 1.41.2.2 +
;********************************************************************
;
;
;************************************
; void sc_sysModulePrioSet(sc_prio_t mprio,sc_moduleid_t mid)
;************************************
;
	XSC_TFUNC sc_sysModulePrioSet,-8
	SC_TFUNC sc_sysModulePrioSet
	push {r0-r1,r4-r7,lr}
	ldr r7,smps_sciopta
	ldr r6,smps_sc_modules
	cmp r0,#31
	bgt smps_err0
	lsrs r2,r1,#24
	cmp r2,#SC_MAX_MODULE
	bge smps_err1
	lsls r2,r2,#2
	ldr r6,[r6,r2]
	cmp r6,#1
	bls smps_warn
	movs r4,r0
	bl sc_procSchedLock
	ldr r3,[r6,#MODULE_PCBS]
	str r4,[r6,#MODULE_PRIO]
	ldr r5,[r6,#MODULE_MAX_PROCESS]
smps1
	ldmia r3!,{r2}
	cmp r2,#0
	beq smps3
	ldrh r1,[r2,#PCB_FLAGS_H]
	lsrs r1,r1,#PCB_FLG_IS_PRIO_POS+1
	bcc smps3
	ldrb r1,[r2,#PCB_PRIO0_B]
	adds r1,r1,r4
	cmp r1,#31
	bls smps2
	movs r1,#31
smps2
	ldrb r0,[r2,#PCB_PRIO_B]
	cmp r1,r0
	beq smps3
	LOCK
	ldrh r0,[r2,#PCB_FLAGS_H] 
	movs r6,#PCB_FLG_READY 
	tst r0,r6 
	beq smps9999 
	bics r0,r0,r6 
	strh r0,[r2,#PCB_FLAGS_H] 
	ldr r0,[r2,#PCB_PCBLIST+dbl_next] 
	ldr r6,[r2,#PCB_PCBLIST+dbl_prev] 
	str r0,[r6,#dbl_next] 
	str r6,[r0,#dbl_prev] 
	cmp r0,r6 
	bne smps9998 
	ldr r6,[r2,#PCB_PRIO_MASK] 
	ldr r0,[r7,#SC_PRIOMASK] 
	bics r0,r0,r6 
	str r0,[r7,#SC_PRIOMASK] 
smps9998 
	movs r0,#0 
	str r0,[r2,#PCB_PCBLIST+dbl_next] 
	str r0,[r2,#PCB_PCBLIST+dbl_prev] 
smps9999
	strb r1,[r2,#PCB_PRIO_B] 
	movs r0,#1 
	IF CPUTYPE >= 2 
	lsls r0,r0,#31 
	lsrs r0,r0,r1 
	ELSE 
	lsls r0,r0,r1 
	ENDIF 
	str r0,[r2,#PCB_PRIO_MASK] 
	lsls r1,r1,#3 
	movs r0,#SC_READYLIST 
	adds r0,r0,r1 
	adds r0,r0,r7 
	str r0,[r2,#PCB_READYLIST]
	ldrh r0,[r2,#PCB_FLAGS_H] 
	movs r1,#PCB_FLG_READY 
	tst r0,r1 
	bne smps1mr9999 
	orrs r0,r0,r1 
	strh r0,[r2,#PCB_FLAGS_H] 
	ldr r6,[r7,#SC_PRIOMASK] 
	ldr r1,[r2,#PCB_PRIO_MASK] 
	ldr r0,[r2,#PCB_READYLIST] 
	orrs r6,r6,r1 
	str r6,[r7,#SC_PRIOMASK] 
	adds r2,r2,#PCB_PCBLIST 
	str r0,[r2,#dbl_next] 
	ldr r1,[r0,#dbl_prev] 
	str r2,[r0,#dbl_prev] 
	str r1,[r2,#dbl_prev] 
	str r2,[r1,#dbl_next] 
	subs r2,r2,#PCB_PCBLIST 
smps1mr9999
	UNLOCK
smps3
	subs r5,r5,#1
	bne smps1
	bl sc_procSchedUnlock
smps_exit
	add sp,sp,#2*4
	RET r4-r7
smps_err1
	ldr r0,smps_lit1
	b smps_err_cont
smps_err0
	movs r1,r0
	ldr r0,smps_lit0
	b smps_err_cont
smps_warn
	ldr r0,smps_lit2
smps_err_cont
	bl sc_sysError
	b smps_exit
	ALIGN
smps_lit0
	DCD (0x45<<24)|(0x028<<12)|0x01
smps_lit1
	DCD (0x45<<24)|(0x027<<12)|0x01
smps_lit2
	DCD (0x45<<24)|(0x027<<12)
smps_sciopta
	DCD sciopta
smps_sc_modules
	DCD sc_modules
	SC_ENDFUNC sc_sysModulePrioSet
;
;*************************************
; sc_prio_t sc_sysModulePrioGet(sc_moduleid_t mid)
;*************************************
;
	XSC_TFUNC sc_sysModulePrioGet,-12
	SC_TFUNC sc_sysModulePrioGet
	ldr r3,smpg_sciopta
	ldr r2,smpg_sc_modules
	lsrs r1,r0,#24
	cmp r1,#SC_MAX_MODULE
	bge smpg_err
	lsls r1,r1,#2
	ldr r1,[r2,r1]
	ldr r0,[r1,#MODULE_PRIO]
	bx lr
smpg_err
	movs r1,r0
	ldr r0,smpg_lit0
	bl sc_sysError
	ALIGN
smpg_sciopta
	DCD sciopta
smpg_sc_modules
	DCD sc_modules
smpg_lit0
	DCD (0x027<<12)|(0x46<<24)|0x02
	SC_ENDFUNC sc_sysModulePrioGet
;
;*************************************
; const char * sc_moduleNameGet(sc_moduleid_t mid)
;*************************************
;
	XSC_TFUNC sc_moduleNameGet,-12
	SC_TFUNC sc_moduleNameGet
	push {lr}
	movs r1,r0
	ldr r3,mng_sciopta
	ldr r0,[r3,#SC_CURMODULE]
	cmp r0,#0
	beq mng_exit
	adds r2,r1,#-SC_CURRENT_MID
	beq mng9
	ldr r2,[r0,#MODULE_ID]
	eors r2,r2,r1
	lsrs r2,r2,#24
	beq mng9
	lsrs r2,r1,#24
	cmp r2,#SC_MAX_MODULE
	ldr r3,mng_sc_modules
	bge mng_err0
	lsls r2,r2,#2
	ldr r0,[r3,r2]
	cmp r0,#1
	bls mng_warn
mng9
	adds r0,r0,#MODULE_NAME
	EXIT
mng_warn
	ldr r0,mng_lit2
mng_err_cont
	bl sc_sysError
mng_exit
	adr r0,mng_empty
	EXIT
mng_err0
	ldr r0,mng_lit1
	b mng_err_cont
	ALIGN
mng_empty
	DCD 0
mng_sciopta
	DCD sciopta
mng_lit1
	DCD (0x027<<12)|(0x42<<24)|0x02
mng_lit2
	DCD (0x027<<12)|(0x42<<24)
mng_sc_modules
	DCD sc_modules
	SC_ENDFUNC sc_moduleNameGet
;
;*************************************
; void sc_moduleKill(sc_moduleid_t mid,flags_t flags)
;*************************************
;
	XSC_TFUNC sc_moduleKill,-8
	SC_TFUNC sc_moduleKill
	push {r0-r1,lr}
	ldr r3,mk_SC_KERNELD
	ldr r0,[r3,#0]
	cmp r0,#0
	beq mk_err
	ldr r1,mk_lit1
	movs r2,#0
	movs r3,#0
	push {r0-r3}	; select-array
	movs r0,#((4 +4)+4)
	subs r1,r1,#1
	movs r2,#1
	subs r3,r2,#1+(-(0xffffffff))
	adds r2,r2,#SC_DEFAULT_POOL-1
	bl sc_msgAlloc
	ldr r1,[sp,#4*4]
	ldr r2,[sp,#5*4]
	push {r0}
	str r1,[r0,#4]
	str r2,[r0,#8]
	mov r0,sp
	ldr r1,[sp,#4]
	movs r2,#0
	bl sc_msgTx
	add r1,sp,#4
	movs r2,#(0x01 +0x02)
	subs r0,r2,#(0x01 +0x02)+(-(0xffffffff))
	bl sc_msgRx
	str r0,[sp,#0]
	mov r0,sp
	bl sc_msgFree
	add sp,sp,#7*4
	EXIT
mk_err
	ldr r0,mk_lit0
	movs r1,#0
	bl sc_sysError
	add sp,sp,#2*4
	EXIT
	ALIGN
mk_SC_KERNELD
	DCD sciopta+SC_KERNELD
mk_lit0
	DCD (0x41<<24)|(0x010<<12)
mk_lit1
	DCD (0x80000000 +0x203)
	SC_ENDFUNC sc_moduleKill
;
;*************************************
; void sc_sysModuleKill(sc_moduleid_t mid,flags_t flag)
;*************************************
;
	XSC_TFUNC sc_sysModuleKill,-8
	SC_TFUNC sc_sysModuleKill
	push {r4-r7,lr}	; save non-volatile regs
	movs r5,r1	; r5:= flags
	ldr r1,smk_sc_modules	; r1:= sc_modules
	lsrs r2,r0,#24	; r2:= module-id
	cmp r2,#SC_MAX_MODULE	; valid ?
	bhs smk_err1	; mid >= SC_MAX_MODULE => error
	movs r3,#1	; r3:= mid = 0 ? 1 : 0
	lsls r2,r2,#2
	beq smk1
	movs r3,#0
smk1
	ldr r4,[r1,r2]	; r4:= MCB
	ldr r7,smk_sciopta	; r7:= sciopta
	cmp r4,#1	; MCB valid ?
	bls smk_err1	; no, error
	ldr r1,[r7,#SC_CURMODULE]
	cmp r1,r4	; killing current module ?
	bne smk1a	; no, keep stack
	ldr r1,=sc_svc_stack	; else switch stack
	mov sp,r1	; r4-r7,lr are lost, but this
	   	; process will die anyway.
	movs r1,#0
	str r1,[r7,#SC_CURPID]	; and fake CURPID to prevent
	   	; sc_sysProcKill from changing
	   	; stack and calling the dispatcher
smk1a
	;
	; lock scheduler
;
	LOCK
	ldr r0,[r7,#SC_SCHEDULER_LOCK]
	adds r0,r0,#1
	str r0,[r7,#SC_SCHEDULER_LOCK]
	UNLOCK
	;
	; Kill all processes
;
	ldr r6,[r4,#MODULE_MAX_PROCESS]
	ldr r0,[r4,#MODULE_PCBS]
	subs r6,r6,r3
	beq smk3a	; only one process in system-module =>
	lsls r7,r3,#2
	adds r7,r7,r0
smk2
	ldmia r7!,{r0}	; r0:= PCB
	cmp r0,#0	; valid ?
	beq smk3	; no, next
	ldr r0,[r0,#PCB_PID]	; r0:= PID
	movs r1,r5	; r1:= flags
	bl sc_sysProcKill	; kill process
smk3
	subs r6,r6,#1	; next process
	bne smk2
smk3a
	;
	; Kill all pools
;
	ldr r7,[r4,#MODULE_POOL]
	ldr r6,[r4,#MODULE_MAX_POOLS]
	lsrs r0,r5,#14 +1	; kill -9 ??
	bcs smk6	; yes, do it the hard way
	movs r5,#0
smk4
	ldmia r7!,{r0}	; r0:= pool-cb
	cmp r0,#0	; valid ?
	beq smk5	; no, next
	movs r0,r5	; r0:= pool-index
	ldr r1,[r4,#MODULE_ID]	; r1:= module-id
	bl sc_sysPoolKill	; kill pool
smk5
	adds r5,r5,#1	; next index
	subs r6,r6,#1	; all done ?
	bne smk4
	b smk8
smk6
	ldmia r7!,{r5}	; r5:= pool_cb
	bl sysPoolKill	; r4 = MCB
smk7
	subs r6,r6,#1
	bne smk6
smk8
	;
	; remove init0 from ready-list if system module
	; was killed.
;
	ldr r1,[r4,#MODULE_ID]
	ldr r0,smk_sc_modules
	ldr r7,smk_sciopta
	lsrs r1,r1,#24
	lsls r1,r1,#2
	bne smk9
	ldr r3,[r4,#MODULE_PCBS]
	ldr r3,[r3,#0]
	LOCK
	ldrh r6,[r3,#PCB_FLAGS_H] 
	movs r5,#PCB_FLG_READY 
	tst r6,r5 
	beq smk9999 
	bics r6,r6,r5 
	strh r6,[r3,#PCB_FLAGS_H] 
	ldr r6,[r3,#PCB_PCBLIST+dbl_next] 
	ldr r5,[r3,#PCB_PCBLIST+dbl_prev] 
	str r6,[r5,#dbl_next] 
	str r5,[r6,#dbl_prev] 
	cmp r6,r5 
	bne smk9998 
	ldr r5,[r3,#PCB_PRIO_MASK] 
	ldr r6,[r7,#SC_PRIOMASK] 
	bics r6,r6,r5 
	str r6,[r7,#SC_PRIOMASK] 
smk9998 
	movs r6,#0 
	str r6,[r3,#PCB_PCBLIST+dbl_next] 
	str r6,[r3,#PCB_PCBLIST+dbl_prev] 
smk9999
	UNLOCK
smk9
	;
	; Update statistics.
;
	movs r6,#0
	ldr r3,[r7,#SC_NMODULE]
	ldr r5,[r7,#SC_CURMODULE]
	str r6,[r0,r1]	; clear module-slot
	subs r3,r3,#1
	str r3,[r7,#SC_NMODULE]
	cmp r5,r4	; current module killed ?
	bne smk10	; no, normal continue
	str r6,[r7,#SC_CURMODULE]	; clear current process
	str r6,[r7,#SC_CURPCB]
	str r6,[r7,#SC_CURPRIOPCB]
	LOCK
	movs r0,#0
	str r0,[r7,#SC_SCHEDULER_LOCK]
	ldr r3,smk_dispatcher
	bx r3
smk10
	LOCK
	ldr r1,[r7,#SC_SCHEDULER_LOCK]
	subs r0,r1,#1
	bmi smk_err2
	str r0,[r7,#SC_SCHEDULER_LOCK]
	UNLOCK
;
; Check if scheduling should be performed
;
	ldr r0,[r7,#SC_SCHEDULE_REQ]
	cmp r0,#0
	beq smk11
;
; Call dispatcher
;
	SWAP_OUT
smk11
	RET r4-r7
smk_err2
	ldr r0,smk_unlockNoLock
	b smk_err_cont
smk_err1
	movs r1,r0
	ldr r0,smk_illModule
smk_err_cont
	bl sc_sysError
	ALIGN
smk_illModule
	DCD (0x027<<12)|(0x41<<24)|0x01
smk_unlockNoLock
	DCD ((0x01f<<12)|0x02|(0x2a<<24))
smk_sciopta
	DCD sciopta
smk_sc_modules
	DCD sc_modules
smk_dispatcher
	DCD sc_sysProcDispatcher
	SC_ENDFUNC sc_sysModuleKill
;
;**************************************
; int sc_moduleInfo(sc_moduleid_t mid,sc_moduleInfo_t *info)
;**************************************
; r0 - mid
; r1 - *info
;
	XSC_TFUNC sc_moduleInfo,-8
	SC_TFUNC sc_moduleInfo
	IF SC_PROC_PARA_CHECK = 1
	cmp r1,#0	; ptr = 0
	beq mi_err0	; yes => error
	ENDIF
	mov r12,r1	; save ptr
	ldr r3,mi_sciopta	; r3:= sciopta-base
	adds r2,r0,#1	; test for: 0xffffffff (-1)
	ldr r2,[r3,#SC_CURMODULE]	; get current module
	bne mi1	; != 0, not current pid
	cmp r2,#0	; SC_CURMODULE set ?
	bne mi2	; yes => fill info
mi_exit
	movs r0,#0	; no, return r0 (= 0)
	bx lr	; return
mi1
	lsrs r2,r0,#24	; r2:= module index
	cmp r2,#SC_MAX_MODULE	; in range ?
	ldr r3,mi_sc_modules
	bge mi_err1	; no => error
	lsls r2,r2,#2
	ldr r2,[r3,r2]	; get module-ptr
	cmp r2,#1	; set ?
	bls mi_exit	; no, return
mi2
	ldr r1,[r2,#MODULE_ID]	; r1:= module id
	mov r0,r12	; r0:= info
	mov r12,r2	; r12:= module_cb ptr
	stmia r0!,{r1}	; save module id in info
	adds r2,r2,#MODULE_NAME	; copy module name
	ldmia r2!,{r1,r3}
	stmia r0!,{r1,r3}
	ldmia r2!,{r1,r3}
	stmia r0!,{r1,r3}
	ldmia r2!,{r1,r3}
	stmia r0!,{r1,r3}
	ldmia r2!,{r1,r3}
	stmia r0!,{r1,r3}
	mov r2,r12	; restore module_cb ptr
	ldr r1,[r2,#MODULE_TEXT]	; r1:= start of module
	subs r3,r2,r1	; r3:= init-size
	stmia r0!,{r1,r3}	; save start,init-size
	ldr r1,[r2,#MODULE_DATA]
	ldr r3,[r2,#MODULE_SIZE]
	stmia r0!,{r1,r3}	; save data-start, data-size
	ldr r1,[r2,#MODULE_MAX_PROCESS]
	ldr r3,[r2,#MODULE_NPROCESS]
	stmia r0!,{r1,r3}	; save max und nProcess
	ldr r1,[r2,#MODULE_MAX_POOLS]
	ldr r3,[r2,#MODULE_NPOOLS]
	stmia r0!,{r1,r3}	; save max and nPools
	movs r0,#1	; return 1
	bx lr
mi_err1
	movs r1,r0
	ldr r0,mi_lit1
mi_err_cont
	bl sc_sysError
	IF SC_PROC_PARA_CHECK = 1
mi_err0
	ldr r0,mi_lit0
	b mi_err_cont
	ENDIF
	ALIGN
mi_lit1
	DCD (0x027<<12)|(0x44<<24)|0x01
	IF SC_PROC_PARA_CHECK = 1
mi_lit0
	DCD (0x00A<<12)|(0x44<<24)|0x01
	ENDIF
mi_sciopta
	DCD sciopta
mi_sc_modules
	DCD sc_modules
	SC_ENDFUNC sc_moduleInfo
;
;**************************************
; sc_moduleid_t sc_moduleIdGet(const char *name)
;**************************************
;
	XSC_TFUNC sc_moduleIdGet,-12
	SC_TFUNC sc_moduleIdGet
	push {r4-r6,lr}
	ldr r6,mig_sciopta
	ldr r2,[r6,#SC_CURMODULE]
	adds r4,r0,#0
	ldr r0,[r2,#MODULE_ID]
	beq mig_exit
	ldrb r3,[r4,#0]
	cmp r3,#0
	beq mig_exit
	movs r3,#SC_MODULE_NAME_SIZE
mig0
	ldrb r0,[r4,r3]
	cmp r0,#0
	beq mig1
	subs r3,r3,#1
	bpl mig0
	ldr r0,mig_lit0
	movs r1,r4
	bl sc_sysError
	ldr r0,mig_lit1
	b mig_exit
mig1
	movs r0,r4
	bl sc_miscCrcString
	mov lr,r0
	mov r12,r4
	movs r1,#SC_MAX_MODULE
	ldr r2,mig_sc_modules
mig2
	ldmia r2!,{r3}
	cmp r3,#1
	bls mig5
	movs r4,r3
	adds r4,r4,#MODULE_HASH_H
	ldrh r4,[r4,#0]
	cmp r4,lr
	bne mig5
	ldr r0,[r3,#MODULE_ID]
	adds r3,r3,#MODULE_NAME
	mov r4,r12
mig3
	ldrb r5,[r3,#0]
	adds r3,r3,#1
	ldrb r6,[r4,#0]
	adds r4,r4,#1
	cmp r5,r6
	bne mig5
	cmp r5,#0
	bne mig3
mig_exit
	RET r4-r6
mig5
	subs r1,r1,#1
	bne mig2
	ldr r0,mig_lit1
	RET r4-r6
	ALIGN
mig_sc_modules
	DCD sc_modules
mig_sciopta
	DCD sciopta
mig_lit0
	DCD (0x43<<24)|(0x025<<12)
mig_lit1
	DCD SC_ILLEGAL_MID
	SC_ENDFUNC sc_moduleIdGet
;
;**************************************
; sc_moduleid_t sc_moduleCreate(const char *name,
; void (*init)(void),
; sc_bufsize_t stacksize,
; sc_prio_t initprio,
; char *modstart,
; sc_modulesize_t size,
; sc_modulesize_t textsize,
; unsigned int max_pools,
; unsigned int max_procs)
;
; R0 name
; R1 init
; R2 stacksize
; R3 initprio
; 0(SP) modstart
; 4(SP) size
; 8(SP) textsize
; 12(SP) max_pools
; 16(SP) max_procs
;**************************************
;
; r4
; r5
; r6
; r0
; lr 32
	XSC_TFUNC sc_moduleCreate,20
	SC_TFUNC sc_moduleCreate
	push {r0-r3,lr}
	ldr r0,mc_sciopta
	ldr r0,[r0,#0]
	cmp r0,#0
	beq mc_err	; no kerneld => error
	movs r2,#0
	movs r3,#0
	ldr r1,mc_SC_MODULECREATEMSG_REPLY
	push {r0-r3}	; select-array
	movs r0,#(SC_MODULE_NAME_SIZE+1+9*4)
	subs r1,r1,#1
	movs r2,#1
	subs r3,r2,#1+(-(0xffffffff))
	adds r2,r2,#SC_DEFAULT_POOL-1
	bl sc_msgAlloc
	ldr r1,[sp,#16]
	adds r2,r0,#4
	push {r0}
	push {r4-r6}
	ldmia r1!,{r3-r6}	; copy name
	stmia r2!,{r3-r6}
	ldmia r1!,{r3-r6}
	stmia r2!,{r3-r6}
	add r1,sp,#20 +4*4
	ldmia r1!,{r3-r5}	; init,stack,prio
	adds r1,r1,#4
	stmia r2!,{r3-r5}
	ldmia r1!,{r0,r3-r6}
	stmia r2!,{r0,r3-r6}
	pop {r4-r6}
	mov r0,sp
	ldr r1,[sp,#4]
	movs r2,#0
	bl sc_msgTx
	add sp,sp,#4
	mov r1,sp
	movs r2,#(0x01 +0x02)
	subs r0,r2,#(0x01 +0x02)+(-(0xffffffff))
	bl sc_msgRx
	push {r0}
	ldr r0,[r0,#4]
	push {r0}
	add r0,sp,#4
	bl sc_msgFree
	pop {r0}
	add sp,sp,#9*4
	EXIT
mc_err
	ldr r0,mc_lit0
	movs r1,#0
	bl sc_sysError
	add sp,sp,#4*4
	ldr r0,mc_lit1
	EXIT
	ALIGN
mc_sciopta
	DCD sciopta+SC_KERNELD
mc_SC_MODULECREATEMSG_REPLY
	DCD (0x80000000 +0x201)
mc_lit0
	DCD (0x40<<24)|(0x010<<12)
mc_lit1
	DCD 0x7fffffff
	SC_ENDFUNC sc_moduleCreate
;
;**************************************
; sc_sysModuleCreate
;
; Create a new module and init process.
;
; Memory map:
; text segment
; module-control-block
; init-process' stack
; pcb-pointer array
; pool-pointer array
; free RAM
;**************************************
; sc_moduleid_t sc_sysModuleCreate(char *name,
; void (*init)(void),
; sc_bufsize_t stacksize,
; sc_prio_t initprio,
; char *modstart,
; sc_modulesize_t size,
; sc_modulesize_t textsize,
; unsigned int max_pools,
; unsigned int max_procs)
;
; R0 name
; R1 init
; R2 stacksize
; R3 initprio
; 0(SP) modstart
; 4(SP) size
; 8(SP) textsize
; 12(SP) max_pools
; 16(SP) max_procs
;
;
;
; Parameter offsets on stack
;
;
	XSC_TFUNC sc_sysModuleCreate,20
	SC_TFUNC sc_sysModuleCreate
	push {r1-r2,r4-r7,lr}
	mov r12,r0
	ldr r7,smc_sciopta
;
; Module prio <= 31 ?
;
	cmp r3,#32
	bhs smc_err4
;
; Parameter check if module name=NULL
;
	cmp r0,#0
	beq smc_ill_name
;
; Parameter check, string length and valid characters
;
smc00
	movs r4,#0xdf
	movs r6,#SC_PROC_NAME_SIZE+1
smc0
	ldrb r5,[r0,#0]
	adds r0,r0,#1
	cmp r5,#0
	beq smc1
	cmp r5,#'_'
	beq smc01
	cmp r5,#'0'
	blt smc_ill_name
	cmp r5,#'9'+1
	blt smc01
	ands r5,r5,r4
	cmp r5,#'A'
	blt smc_ill_name
	cmp r5,#'Z'
	bgt smc_ill_name
smc01
	subs r6,r6,#1
	bne smc0
smc_ill_name
	mov r1,r12
	ldr r0,smc_ill_name_lit
smc_err_cont
	bl sc_sysError
smc_err4
	movs r0,r3
smc_ill_value
	movs r1,r0
	ldr r0,smc_ill_value_lit
	b smc_err_cont
smc1
	subs r6,r6,#SC_PROC_NAME_SIZE+1
	beq smc_ill_name	; len = 0
;
; Parameter check, max processes
;
	ldr r0,[sp,#44]
	cmp r0,#0
	beq smc_ill_value
	lsrs r4,r0,#14	; > 0x3fff ?
	bne smc_ill_value
;
; Calculate size for all PCBs and PCB-array
; (max processes)*128+(max_process*4)
;
	movs r4,#128+4
	muls r0,r4,r0
;
; Parameter check, max pools
;
	ldr r4,[sp,#40]
	cmp r4,#0
	beq smc_ill_value
	cmp r4,#128
	bhi smc_ill_value
;
; Calculate space for all PoolCBs and PoolCB-array
;
	lsls r4,r4,#2	; size of array
	adds r0,r0,r4
	movs r5,#SIZEOF_POOL_CB>>2
	muls r4,r5,r4	; size of pool_cbs
;
; Check module size
;
	ldr r5,[sp,#36]
	adds r0,r0,r4	; size += sizeof(poolcb)*MAX_POOLS
	ldr r4,[sp,#32]
	adds r0,r0,r2	; size += stack size
	IF CPUTYPE >= 2
	adds r5,r5,#7
	lsrs r5,r5,#3
	lsls r5,r5,#3
	ELSE
	adds r5,r5,#3
	lsrs r5,r5,#2
	lsls r5,r5,#2
	ENDIF
	str r5,[sp,#36]
	adds r0,r0,r5	; size += init-size
	movs r5,#SIZEOF_MODULE_CB>>2
	lsls r5,r5,#2
	adds r0,r0,r5
	cmp r0,r4
	bhi smc_too_small
;
; Check if max modules reached
;
	ldr r0,[r7,#SC_NMODULE]
	cmp r0,#SC_MAX_MODULE
	beq smc_no_module
;
; Increase module counter
;
	adds r0,r0,#1
	str r0,[r7,#SC_NMODULE]
;
; Find a free slot in MCB pointer array
;
	ldr r5,=(sc_modules-4)
	movs r7,#0
	subs r7,r7,#1
smc2
	ldr r0,[r5,#4]
	adds r7,r7,#1
	adds r5,r5,#4
	cmp r0,#0
	bne smc2
;
; Clear MCB, PCB pointer array and
; PoolCB pointer array
;
	ldr r4,[sp,#28]
	ldr r0,[sp,#36]
;
; The module-cb may not start at 0, therefore we have to
; use a dummy-textsize of 4.
;
	cmp r4,#0
	bne smc21
	cmp r0,#0
	bne smc21
	movs r0,#4
smc21
	adds r4,r4,r0
	ldr r0,[sp,#40]
	ldr r1,[sp,#44]
	adds r1,r1,r0
	adds r1,r1,#SIZEOF_MODULE_CB>>2
	movs r2,#0
	movs r0,r4
smc3
	subs r1,r1,#1
	stmia r0!,{r2}
	bne smc3
;
; Fill Module Data Block with module virgin pattern
; if SC_DEBUG
;
	IF SC_CDEBUG = 1
	ldr r1,[sp,#32]
	ldr r2,[sp,#36]
	adds r1,r1,r4
	subs r1,r1,r0
	subs r1,r1,r2
	lsrs r1,r1,#2
	ldr r2,smc_modulevirgin
smc4
	subs r1,r1,#1
	stmia r0!,{r2}
	bne smc4
	ENDIF
;
; Setup MCB
;
	str r3,[r4,#MODULE_PRIO]
	add r0,sp,#28
	ldmia r0!,{r1-r2}
	str r1,[r4,#MODULE_TEXT]
	str r2,[r4,#MODULE_SIZE]
	adds r0,r0,#4
	ldmia r0!,{r1-r2}
	str r1,[r4,#MODULE_MAX_POOLS]
	str r2,[r4,#MODULE_MAX_PROCESS]
	mov r0,r12
	movs r3,r4
	adds r3,r3,#MODULE_NAME-1
smc5
	ldrb r1,[r0,#0]
	adds r3,r3,#1
	adds r0,r0,#1
	strb r1,[r3,#0]
	cmp r1,#0
	bne smc5
	mov r0,r12
	bl sc_miscCrcString
	movs r1,r4
	adds r1,r1,#MODULE_HASH_H
	strh r0,[r1,#0]
	lsls r0,r7,#24
	str r0,[r4,#MODULE_ID]
	IF SC_USE_FRIENDS = 1
	lsrs r0,r7,#5
	movs r1,#31
	ands r1,r1,r7
	lsls r0,r0,#2
	adds r0,r0,#MODULE_FRIENDSET
	movs r2,#1
	lsls r2,r2,r1
	str r2,[r4,r0]
	ENDIF
	movs r0,r4
	adds r0,r0,#SIZEOF_MODULE_CB
	str r0,[r4,#MODULE_PCBS]
	ldr r1,[r4,#MODULE_MAX_PROCESS]
	lsls r1,r1,#2
	adds r0,r0,r1
	str r0,[r4,#MODULE_POOL]
	ldr r1,[r4,#MODULE_MAX_POOLS]
	lsls r1,r1,#2
	adds r0,r0,r1
	str r0,[r4,#MODULE_DATA]
	str r0,[r4,#MODULE_DATAPTR]
	ldr r1,[r4,#MODULE_TEXT]
	subs r1,r1,r0
	ldr r0,[sp,#32]
	adds r0,r0,r1
	str r0,[r4,#MODULE_FREESIZE]
;
; Insert MCB pointer into MCB pointer array
;
	str r4,[r5,#0]
;
; Create init process
;
	pop {r1-r2}	; name, func
	movs r0,r4
	adds r0,r0,#MODULE_NAME
	movs r6,#(0x2b<<24)>>24
	lsls r6,r6,#24
	movs r5,#0	; PCB
	movs r3,#0	; stack
	push {r3,r5-r6}
	ldr r6,[r4,#MODULE_ID]
	movs r5,#1	; started
	push {r3,r5-r6}
	bl sc_sysProcCreate
	add sp,sp,#6*4
;
; Return module index as module id
;
	lsls r0,r7,#24
	RET r4-r7
;
; Error transfer
;
smc_too_small
	movs r1,r4
	ldr r0,smc_too_small_lit
	b smc_err_cont
smc_no_module
	mov r1,r12
	ldr r0,smc_no_module_lit
	b smc_err_cont
;
; Literal pool
;
	ALIGN
smc_sciopta
	DCD sciopta
smc_modulevirgin
	DCD 0xaaaaaaaa
smc_ill_name_lit
	DCD (0x025<<12)|(0x40<<24)|0x01
smc_ill_value_lit
	DCD ((0x40<<24)|(0x030<<12)|0x01)
smc_too_small_lit
	DCD ((0x022<<12)|(0x40<<24)|0x01)
smc_no_module_lit
	DCD ((0x02A<<12)|(0x40<<24)|0x01)
	SC_ENDFUNC sc_sysModuleCreate
	IF SC_USE_FRIENDS = 1
;
;*************************************
; void sc_moduleFriendAdd(sc_moduleid_t mid)
;*************************************
;
	XSC_TFUNC sc_moduleFriendAdd,-12
	SC_TFUNC sc_moduleFriendAdd
	push {lr}
	lsrs r1,r0,#24
	cmp r1,#SC_MAX_MODULE
	bhs mfa_err
	movs r0,#31
	ands r0,r0,r1
	lsrs r1,r1,#5
	lsls r1,r1,#2
	ldr r2,mfa_lit1
	adds r1,r1,#MODULE_FRIENDSET
	ldr r2,[r2,#SC_CURMODULE]
	movs r3,#1
	lsls r3,r3,r0
	LOCK
	ldr r0,[r2,r1]
	orrs r0,r0,r3
	str r0,[r2,r1]
	UNLOCK
	EXIT
mfa_err
	movs r1,r0
	ldr r0,mfa_lit0
	bl sc_sysError
	ALIGN
mfa_lit0
	DCD ((0x027<<12)|(0x47<<24)|0x01)
mfa_lit1
	DCD sciopta
	SC_ENDFUNC sc_moduleFriendAdd
;
;*************************************
; void sc_moduleFriendRm(sc_moduleid_t mid)
;*************************************
;
	XSC_TFUNC sc_moduleFriendRm,-12
	SC_TFUNC sc_moduleFriendRm
	push {lr}
	ldr r2,mfr_lit1
	lsrs r1,r0,#24
	cmp r1,#SC_MAX_MODULE
	bhs mfr_err
	ldr r2,[r2,#SC_CURMODULE]
	ldr r3,[r2,#MODULE_ID]
	lsrs r3,r3,#24
	cmp r3,r0	; rm(self) not allowed
	beq mfr_err
	movs r0,#31
	ands r0,r0,r1
	lsrs r1,r1,#5
	lsls r1,r1,#2
	adds r1,r1,#MODULE_FRIENDSET
	movs r3,#1
	lsls r3,r3,r0
	LOCK
	ldr r0,[r2,r1]
	bics r0,r0,r3
	str r0,[r2,r1]
	UNLOCK
	EXIT
mfr_err
	movs r1,r0
	ldr r0,mfr_lit0
	bl sc_sysError
	ALIGN
mfr_lit0
	DCD ((0x027<<12)|(0x48<<24)|0x01)
mfr_lit1
	DCD sciopta
	SC_ENDFUNC sc_moduleFriendRm
;
;*************************************
; int sc_moduleFriendGet(sc_moduleid_t mid)
;*************************************
;
	XSC_TFUNC sc_moduleFriendGet,-12
	SC_TFUNC sc_moduleFriendGet
	lsrs r1,r0,#24
	cmp r1,#SC_MAX_MODULE
	bhs mfg_err
	movs r0,#31
	ands r0,r0,r1
	lsrs r1,r1,#5
	lsls r1,r1,#2
	ldr r2,mfg_lit1
	adds r1,r1,#MODULE_FRIENDSET
	ldr r2,[r2,#SC_CURMODULE]
	movs r3,#1
	lsls r3,r3,r0
	ldr r0,[r2,r1]
	ands r0,r0,r3
	bx lr
mfg_err
	movs r1,r0
	ldr r0,mfg_lit0
	bl sc_sysError
	ALIGN
mfg_lit0
	DCD ((0x027<<12)|(0x49<<24)|0x01)
mfg_lit1
	DCD sciopta
	SC_ENDFUNC sc_moduleFriendGet
;
;**************************************
; void sc_moduleFriendNone(void)
;**************************************
;
	XSC_TFUNC sc_moduleFriendNone,-16
	SC_TFUNC sc_moduleFriendNone
	push {lr}
	ldr r3,mfn_lit1
	ldr r2,[r3,#SC_CURMODULE]
	ldr r1,[r3,#MODULE_ID]
	lsrs r1,r1,#24
	movs r0,#31
	ands r0,r0,r1
	lsrs r1,r1,#5
	lsls r1,r1,#2
	adds r1,r1,#MODULE_FRIENDSET
	movs r3,#1
	lsls r3,r3,r0
	movs r0,#0
	LOCK
	str r0,[r2,#MODULE_FRIENDSET]
	str r0,[r2,#MODULE_FRIENDSET+4]
	str r0,[r2,#MODULE_FRIENDSET+8]
	str r0,[r2,#MODULE_FRIENDSET+12]
	str r3,[r2,r1]
	UNLOCK
	EXIT
	ALIGN
mfn_lit1
	DCD sciopta
	SC_ENDFUNC sc_moduleFriendNone
;
;**************************************
; void sc_moduleFriendAll(void)
;**************************************
;
	XSC_TFUNC sc_moduleFriendAll,-16
	SC_TFUNC sc_moduleFriendAll
	push {lr}
	ldr r3,mfall_lit1
	movs r0,#0
	ldr r2,[r3,#SC_CURMODULE]
	subs r0,r0,#1
	LOCK
	str r0,[r2,#MODULE_FRIENDSET]
	str r0,[r2,#MODULE_FRIENDSET+4]
	str r0,[r2,#MODULE_FRIENDSET+8]
	str r0,[r2,#MODULE_FRIENDSET+12]
	UNLOCK
	EXIT
	ALIGN
mfall_lit1
	DCD sciopta
	SC_ENDFUNC sc_moduleFriendAll
	ENDIF
;
;**************************************************************************
; misc functions
;
; S03107BS1
;
; +Date: 2009/08/14 04:43:04 +
; +Revision: 1.23.2.2 +
;**************************************************************************
;
;
;**************************************
; int __CODE sc_miscKerneldRegister(void)
;**************************************
;
	XSC_TFUNC sc_miscKerneldRegister,-16
	SC_TFUNC sc_miscKerneldRegister
	push {lr}
	movs r0,#0
	ldr r3,mkr_sciopta
	ldr r2,[r3,#SC_CURPCB]
	ldr r1,[r3,#SC_CURPID]
	lsrs r1,r1,#24
	ldrh r1,[r2,#PCB_FLAGS_H]
	bne mkr_err0
	lsrs r1,r1,#PCB_FLG_IS_PRIO_POS+1
	bcc mkr_err0
	ldr r3,mkr_SC_KERNELD
	ldr r2,[r2,#PCB_PID]
	LOCK
	ldr r1,[r3,#0]
	cmp r1,#0
	bne mkr_err1
	str r2,[r3,#0]
	movs r0,#1
mkr_err1
	UNLOCK
mkr_err0
	EXIT
	ALIGN
mkr_sciopta
	DCD sciopta
mkr_SC_KERNELD
	DCD sciopta+SC_KERNELD
	SC_ENDFUNC sc_miscKerneldRegister
;
;**************************************
; sc_errcode_t __CODE sc_miscErrnoGet()
;**************************************
;
	XSC_TFUNC sc_miscErrnoGet,-16
	SC_TFUNC sc_miscErrnoGet
	ldr r3,meg_sciopta
	ldr r3,[r3,#SC_CURPCB]
	ldr r0,[r3,#PCB_ERRORCODE]
	bx lr
	ALIGN
meg_sciopta
	DCD sciopta
	SC_ENDFUNC sc_miscErrnoGet
;
;**************************************
; void __CODE sc_miscErrnoSet(sc_errcode_t err)
;**************************************
;
	XSC_TFUNC sc_miscErrnoSet,-12
	SC_TFUNC sc_miscErrnoSet
	ldr r3,mes_sciopta
	ldr r3,[r3,#SC_CURPCB]
	str r0,[r3,#PCB_ERRORCODE]
	bx lr
	ALIGN
mes_sciopta
	DCD sciopta
	SC_ENDFUNC sc_miscErrnoSet
;
;**************************************
; sc_errHook_t __CODE sc_miscErrorHookRegister(sc_errHook_t hook)
;**************************************
;
	IF SC_ERR_HOOK = 1
	XSC_TFUNC sc_miscErrorHookRegister,-12
	SC_TFUNC sc_miscErrorHookRegister
	ldr r3,mehr_sciopta
	ldr r2,[r3,#SC_CURMODULE]
	cmp r2,#0
	bne mehr1
	ldr r2,mehr_SC_ERRHOOK
	adds r2,r2,r3
mehr1
	ldr r1,[r2,#MODULE_ERRHOOK]
	str r0,[r2,#MODULE_ERRHOOK]
	movs r0,r1
	bx lr
	ALIGN
mehr_sciopta
	DCD sciopta
mehr_SC_ERRHOOK
	DCD SC_ERRHOOK-MODULE_ERRHOOK
	SC_ENDFUNC sc_miscErrorHookRegister
	ENDIF
;
;**************************************
; void sc_miscError(sc_errcode_t err,sc_extra_t misc)
; 3 - system errorhook
; MH - module errorhook
; SC - 3 returns true
; MC - MH returns true
;
; 3 MH SC MC
; 0 0 0 0 => stop
; 0 1 0 0 => stop
; 0 1 0 1 => cont
; 1 0 0 0 => stop
; 1 0 1 0 => cont
; 1 1 0 0 => stop
; 1 1 0 1 => stop
; 1 1 1 0 => stop
; 1 1 1 1 => cont
;
; sorted:
; 3 MH SC MC
; 0 0 0 0 => stop
; 0 1 0 0 => stop
; 1 0 0 0 => stop
; 1 1 0 0 => stop
; 1 1 0 1 => stop
; 1 1 1 0 => stop
;
; 1 0 1 0 => cont
; 0 1 0 1 => cont
; 1 1 1 1 => cont
;**************************************
;
	XSC_TFUNC sc_miscError,-8
	SC_TFUNC sc_miscError
	movs r2,#1
	b merr_cnt
	SC_TPUBLIC sc_sysError
	movs r2,#0
merr_cnt
	push {r4-r7,lr}
	LOCK
	ldr r6,merr_sciopta
	ldr r3,[r6,#SC_CURPCB]
	movs r4,#0x01	; if no process, all bugs are sysfatal
	cmp r3,#0
	beq se0
	movs r4,#0
	ldrh r7,[r3,#PCB_FLAGS_H]
	movs r5,#PCB_FLG_IS_TIMER|PCB_FLG_IS_IRQ	; timer or interrupt
	tst r7,r5
	beq se00	; no, leave error-code untouched
	movs r5,#(0x04|0x02|0x01)	; fatal error ?
	tst r0,r5
	beq se00	; no, leave error-code untouched
	movs r4,r5	; else make it system-fatal
se00
	orrs r0,r0,r4
	str r0,[r3,#PCB_ERRORCODE]
se0
	orrs r0,r0,r4
	ldr r7,merr_ERR_MSG
	ldr r4,[r6,#SC_CURPID]
	movs r5,r7	; save ERR_MSG pointer
	stmia r7!,{r0-r4}
	IF SC_ERR_HOOK = 1
	push {r0}	; save error-code
	movs r7,#0	; cont := 0
	ldr r4,[r6,#SC_CURMODULE]
	cmp r4,#0	; a module active ?
	beq se1	; no, skip
	ldr r4,[r4,#MODULE_ERRHOOK]	; module-errorhook ?
	cmp r4,#0
	beq se1	; no, skip
	bl via_r4	; else call it
	movs r7,#4	; set MH flag
	cmp r0,#0
	beq se1
	adds r7,r7,#1	; set MC flag
se1
	ldmia r5!,{r0-r3}	; restore error code etc.
	ldr r4,merr_lit2
	ldr r4,[r6,r4]	; system-errorhook ?
	cmp r4,#0
	beq se2	; no, skip
	bl via_r4
	adds r7,r7,#8	; set 3 flag
	cmp r0,#0
	beq se2
	adds r7,r7,#2	; set SC flag
se2
	pop {r2}	; restore error-code
	cmp r7,#15	; all go ?
	beq se2a
	cmp r7,#10	; only 3: go ?
	beq se2a
	cmp r7,#5	; only MH: go ?
	bne sc_fatal
se2a
	;
	; both error hooks returned != 0, so try to continue
;
	movs r3,#(0x04|0x02|0x01)
	tst r2,r3
	bne se3	; error fatal ?
	UNLOCK	; no, so return
	RET r4-r7
se3
	ldr r0,[r6,#SC_CURPID]
;---- FIXME: Should make it optional, if KILL or nice shut-down
	movs r1,#0
;----
	lsrs r2,r2,#1
	bcs sc_fatal	; system-fatal => stop
	lsrs r2,r2,#1
	bcs se_module	; module-fatal
	cmp r0,#0	; killing init0-process is fatal
	beq sc_fatal
	bl sc_sysProcKill
	b se5
se_module
	lsrs r2,r0,#24	; killing module 0 is fatal
	beq sc_fatal
	bl sc_sysModuleKill
se5
	LOCK
	ldr r4,p_sc_sysProcDispatcher
via_r4
	bx r4
	ENDIF
	SC_TPUBLIC sc_fatal
	mov r8,r8	; nop, but gas produces wrong code
	; must use bl, otherwise gnu-as/ld produce wrong code !
	bl sc_fatal
	ALIGN
merr_ERR_MSG
	DCD ERR_MSG
merr_sciopta
	DCD sciopta
	IF SC_ERR_HOOK = 1
merr_lit2
	DCD SC_ERRHOOK
p_sc_sysProcDispatcher
	DCD sc_sysProcDispatcher
	ENDIF
	SC_ENDFUNC sc_miscError
;
;************************************
; __u16 sc_miscCrc(char *p,int size)
;
; IN:
; R0 p
; R1 size
; xsc_miscCrc
; r2 line
; r3 file
;
; This routine destroys: r0-r3,r12
;
; __u16 sc_miscCrcContd(char *p,int size,__u16 init)
; IN:
; R0 p
; R1 size
; r2 init
; xsc_miscCrcContd
; r3 line
; (SP) file
;
; __u16 __CODE sc_miscCrc(__u8 *data,unsigned int len)
; {
; __u16 hash = 0xffff;
; while( len ){
; hash = crc[(__u8)hash ^ *data ] ^ (hash>>8);
; --len;
; ++data;
; }
; return (__u16)hash;
; }
;***************************************
;
	XSC_TFUNC sc_miscCrcString,-12
	SC_TFUNC sc_miscCrcString
	movs r3,r0
	movs r0,#0
	subs r0,r0,#1
	lsrs r0,r0,#16
	ldr r2,mcs_lit0
	b mcs1
mcs0
	adds r3,r3,#1
	eors r1,r1,r0
	lsls r1,r1,#24
	lsrs r1,r1,#23
	ldrh r1,[r2,r1]
	lsrs r0,r0,#8
	eors r0,r0,r1
mcs1
	ldrb r1,[r3,#0]
	cmp r1,#0
	bne mcs0
	bx lr
	ALIGN
mcs_lit0
	DCD sc_miscCrcTab
	SC_ENDFUNC sc_miscCrcString
	XSC_TFUNC sc_miscCrc,-8
	XSC_TFUNC sc_miscCrcContd,-4
	SC_TFUNC sc_miscCrc
	movs r2,#0
	subs r2,r2,#1
	SC_TPUBLIC sc_miscCrcContd
	push {r4,lr}
	movs r4,r0	; save data ptr
	lsls r0,r2,#16
	lsrs r0,r0,#16	; start-value
	cmp r1,#0	; len = 0 ?
	beq mcc9
	ldr r3,mcc_lit0
mcc0
	ldrb r2,[r4,#0]
	adds r4,r4,#1
	eors r2,r2,r0
	lsls r2,r2,#24
	lsrs r2,r2,#23
	ldrh r2,[r3,r2]
	lsrs r0,r0,#8
	eors r0,r0,r2
	subs r1,r1,#1
	bne mcc0
mcc9
	RET r4
	ALIGN
mcc_lit0
	DCD sc_miscCrcTab
	SC_ENDFUNC sc_miscCrc
	SC_FUNC sc_miscCrcTab
	DCW 0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf
	DCW 0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7
	DCW 0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e
	DCW 0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876
	DCW 0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd
	DCW 0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5
	DCW 0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c
	DCW 0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974
	DCW 0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb
	DCW 0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3
	DCW 0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a
	DCW 0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72
	DCW 0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9
	DCW 0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1
	DCW 0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738
	DCW 0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70
	DCW 0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7
	DCW 0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff
	DCW 0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036
	DCW 0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e
	DCW 0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5
	DCW 0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd
	DCW 0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134
	DCW 0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c
	DCW 0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3
	DCW 0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb
	DCW 0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232
	DCW 0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a
	DCW 0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1
	DCW 0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9
	DCW 0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330
	DCW 0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
	SC_ENDFUNC sc_miscCrcTab
;
;**************************************************************************
; message functions
;
; S03112BS1
;
; +Date: 2009/11/06 06:36:54 +
; +Revision: 1.48.2.12 +
;**************************************************************************
;
;
;**************************************
; void __CODE sc_msgAcquire(sc_msgptr_t msgptr)
;
; Change owner of a message.
; Copy message into a new buffer if
; - the current owner if in a different module we are
; not friend to
; - ALWAYS_COPY is selected.
;
; CHECKS: msgptr != 0
; *msgptr != 0
;**************************************
;
	XSC_TFUNC sc_msgAcquire,-12
	SC_TFUNC sc_msgAcquire
	push {r0,r4-r7,lr}	; save working regs and parameter
	IF SC_MSG_PARA_CHECK = 1
	cmp r0,#0	; msgptr = 0 ?
	beq mac_err0	; yes => error
	ldr r3,[r0,#0]
	cmp r3,#0	; *msgptr = 0 ?
	beq mac_err0	; yes => error
	ELSE
	ldr r3,[r0,#0]
	ENDIF
	ldr r7,mac_sciopta
	ldr r6,mac_sc_modules
	subs r3,r3,#32	; r3 := addr of msg-header
	ldr r1,[r3,#28]	; r1 := msg_owner
	ldr r5,[r7,#SC_CURPID]	; r5 := current process
	cmp r1,#0	; owner = kernel => msg freed or in mqueue
	beq mac_err1
	cmp r5,r1	; same ?
	beq mac_exit	; yes, nothing do to
	IF SC_NEVER_COPY = 1
	ELSE
	lsrs r0,r5,#24	; r0 := current module id
	ldr r1,[r3,#12]
	lsls r1,r1,#1
	lsrs r1,r1,#25	; r1 := module id of msg
	cmp r0,r1	; same ?
	beq mac_nocopy	; yes, no copy needed
	IF SC_USE_FRIENDS = 1
	lsls r1,r1,#2
	ldr r4,[r6,r1]	; r4 := MCB of msg
	movs r1,#31
	ands r1,r1,r0	; r1 := bitnumber
	lsrs r0,r0,#5	; r0 := word-number
	lsls r0,r0,#2	;
	adds r0,r0,#MODULE_FRIENDSET
	ldr r0,[r4,r0]	; get word
	adds r1,r1,#1	; bitnumber+1 for carry test
	lsrs r0,r0,r1	; check bit
	bcs mac_nocopy	; carry => no copy (friend)
	ENDIF
	; copy msg
	movs r4,r3	; save addr of msg-header
	ldr r5,[r4,#8]	; allocate new buffer from
	movs r2,#1	; the default pool
	subs r3,r2,#1+(-((0xffffffff)-1))	; no msg => fatal !
	adds r2,r2,#SC_DEFAULT_POOL-1
	movs r0,r5	; with the current msg's size
	bl sc_msgAlloc
	push {r0,r4}	; save addr of current and new buffer
	subs r0,r0,#32	; turn back msg ptr to msgheader
	ldr r1,[r4,#20]	; copy sender and addressee of
	ldr r2,[r4,#24]	; old buffer to new one
	str r1,[r0,#20]
	str r2,[r0,#24]
	adds r0,r0,#32	; restore
	adds r4,r4,#32	; r4 := addr of msg contents
	lsrs r2,r5,#3	; r2 := number of 8 byte chunks
	beq mac1	; = 0 => none
mac0
	ldmia r4!,{r1,r3}	; copy 8byte wise
	subs r2,r2,#1
	stmia r0!,{r1,r3}
	bne mac0
mac1
	lsrs r2,r5,#3	; remaining 4 byte chunk ?
	bcc mac12	; no carry => none
	ldmia r4!,{r3}
	stmia r0!,{r3}
mac12
	lsrs r2,r5,#2	; remaining 2 byte chunk ?
	bcc mac2	; no carry => none
	ldrh r3,[r4,#0]
	adds r4,r4,#2
	strh r3,[r0,#0]
	adds r0,r0,#2
mac2
	lsrs r2,r5,#1	; a byte left ?
	bcc mac3
	ldrb r3,[r4,#0]
	strb r3,[r0,#0]
mac3
	pop {r4}	; r4 := new msg-ptr
	pop {r0}	; r0 := old msg-ptr
	bl sc_sysMsgFree	; free it
	pop {r0}	; r0 := msgptr
	str r4,[r0,#0]	; return addr of new msg
	RET r4-r7
	ENDIF
mac_nocopy
	ldr r0,[r3,#28]	; r0 := msg_owner
	lsrs r1,r0,#24	; r1 := module id of msg_owner
	lsls r1,r1,#2
	ldr r1,[r6,r1]	; r1 := MCB of msg owner
	ldr r1,[r1,#MODULE_PCBS]
	lsls r0,r0,#18	; mask count and MID
	lsrs r0,r0,#16	; r0 := index to PCB
	ldr r2,[r1,r0]	; r2 := module->pcbs[r0]
	;
	; move msg from current to new owner
	;
	LOCK	; must lock interrupt while modifying alloc-queue !
	ldr r0,[r3,#dbl_next] 
	ldr r1,[r3,#dbl_prev] 
	str r0,[r1,#dbl_next] 
	str r1,[r0,#dbl_prev]	; remove from current owner
	ldr r0,[r2,#PCB_NALLOC]	; update stats
	subs r0,r0,#1
	str r0,[r2,#PCB_NALLOC]
	ldr r0,[r7,#SC_CURPCB]	; insert into current process'
	movs r1,r0
	adds r1,r1,#PCB_ALLOCED	; alloc-queue
	str r1,[r3,#dbl_next] 
	ldr r2,[r1,#dbl_prev] 
	str r3,[r1,#dbl_prev] 
	str r2,[r3,#dbl_prev] 
	str r3,[r2,#dbl_next]
	ldr r1,[r0,#PCB_NALLOC]	; update stats
	str r5,[r3,#28]	; set current process as new owner
	adds r1,r1,#1
	str r1,[r0,#PCB_NALLOC]
	UNLOCK
mac_exit
	add sp,sp,#4	; clean statck
	RET r4-r7
	IF SC_MSG_PARA_CHECK = 1
mac_err0
	movs r1,r0	; msgptr = 0 or *msptr = 0
	ldr r0,mac_lit0
	b mac_err_cont
	ENDIF
mac_err1
	movs r1,r3
	ldr r0,mac_lit1
mac_err_cont
	bl sc_sysError
	ALIGN
mac_sciopta
	DCD sciopta
mac_sc_modules
	DCD sc_modules
	IF SC_MSG_PARA_CHECK = 1
mac_lit0
	DCD ((0x00A<<12)|(0x0c<<24)|0x02)
	ENDIF
mac_lit1
	DCD ((0x009<<12)|(0x0c<<24)|0x02)
	SC_ENDFUNC sc_msgAcquire
;
;**************************************
; sc_msgHook_t __CODE sc_msgHookRegister(int type,sc_msgHook_t newhook)
;
; Register a msg hook
;
; CHECKS: type is valid.
;**************************************
;
	IF SC_MSG_HOOK = 1
	XSC_TFUNC sc_msgHookRegister,-8
	SC_TFUNC sc_msgHookRegister
	ldr r3,mhr_sciopta
	ldr r2,[r3,#SC_CURMODULE]	; get addr of current module
	cmp r2,#0	; = 0 => system hook
	beq mhr1
	IF SC_MSGTX_HOOK = 1
	movs r3,#MODULE_MSGTXHOOK
	cmp r0,#(1)
	beq mhr2
	ENDIF
	IF SC_MSGRX_HOOK = 1
	movs r3,#MODULE_MSGRXHOOK
	cmp r0,#(2)
	beq mhr2
	ENDIF
	; type parameter wrong, fatal error (module level)
	IF SC_MSG_PARA_CHECK = 1
mhr_err
	movs r1,r0
	ldr r0,mhr_illval0
	b mhr_err_cont	; => sc_sysError
	ENDIF
mhr1
	IF SC_MSGTX_HOOK = 1
	ldr r2,mhr_SC_MSGTXHOOK
	cmp r0,#(1)
	beq mhr2
	ENDIF
	IF SC_MSGRX_HOOK = 1
	ldr r2,mhr_SC_MSGRXHOOK
	cmp r0,#(2)
	beq mhr2
	ENDIF
	; type parameter wrong, fatal error (system level)
	IF SC_MSG_PARA_CHECK = 1
	movs r1,r0
	ldr r0,mhr_illval1
mhr_err_cont
	bl sc_sysError
	ELSE
	movs r0,#0
	bx lr
	ENDIF
mhr2
	ldr r0,[r2,r3]
	str r1,[r2,r3]
	bx lr
	ALIGN
mhr_sciopta
	DCD sciopta
	IF SC_MSGTX_HOOK = 1
mhr_SC_MSGTXHOOK
	DCD SC_MSGTXHOOK
	ENDIF
	IF SC_MSGRX_HOOK = 1
mhr_SC_MSGRXHOOK
	DCD SC_MSGRXHOOK
	ENDIF
	IF SC_MSG_PARA_CHECK = 1
mhr_illval0
	DCD (0x0e<<24)|(0x030<<12)|0x02
mhr_illval1
	DCD (0x0e<<24)|(0x030<<12)|0x01
	ENDIF
	SC_ENDFUNC sc_msgHookRegister
	ENDIF
;
;**************************************
; sc_msg_t sc_msgAllocClr(sc_bufsize_t size,
; sc_msgid_t id,
; sc_poolid_t plidx,
; sc_ticks_t tmo)
;
; Allocate a msg and clear user-area except msgid.
;
; CHECKS: see sc_msgAlloc
;
; R0 size
; R1 id
; R2 plidx
; R3 tmo
;
	XSC_TFUNC sc_msgAllocClr,0
	SC_TFUNC sc_msgAllocClr
	push {r0,lr}	; save size and return addr
	bl sc_msgAlloc	; get buffer
	pop {r2}	; r2 := size
	cmp r0,#0	; got a msg ?
	beq smac10	; no, leave
	subs r2,r2,#4	; skip msgid
	beq smac10	; = 0 => done
	mov lr,r0	; save addr
	adds r0,r0,#4	; skip msgid
	movs r3,#0
	lsrs r1,r2,#2	; r1 := number of words
	beq smac1
smac0
	stmia r0!,{r3}	; clear memory and incr. addr
	subs r1,r1,#1
	bne smac0
smac1
	lsls r2,r2,#31
	bcc smac2	; carry = 1 => hword
	strh r3,[r0,#0]
	adds r0,r0,#2
smac2
	lsls r2,r2,#1
	bcc smac9	; carry = 1 => byte
	strb r3,[r0,#0]
smac9
	mov r0,lr	; restore msg-addr
smac10
	EXIT
	SC_ENDFUNC sc_msgAllocClr
;
;**************************************
; sc_msg_t sc_msgAlloc(sc_bufsize_t size,
; sc_msgid_t id,
; sc_poolid_t plidx,
; sc_ticks_t tmo)
;
; Allocate a msg and pre-set msg-id.
;
; CHECKS:
; - size >= 4 and <= max. buffersize
; - plidx in range and pool exists
; - only prio uses tmo
;
; R0 size
; R1 id
; R2 plidx
; R3 tmo
; sysMsgAlloc
; r6 module
; r7 sciopta
;
;
	XSC_TFUNC sc_msgAlloc,0
	SC_TFUNC sc_sysMsgAlloc
	push {r0-r7,lr}
	b sma0
	SC_TPUBLIC sc_msgAlloc
	push {r0-r7,lr}
	ldr r7,sma_sciopta
	ldr r6,[r7,#SC_CURMODULE]	; r6 := addr of current MCB
sma0
	ldr r5,[r7,#SC_CURPCB]	; r5 := addr of current PCB
	IF SC_MSG_PARA_CHECK = 1
	cmp r6,#0	; module set ?
	beq sma_err6	; no => error
	movs r4,r3	; r4:= tmo (and test)
	beq sma02	; tmo = (0) => ok
	adds r4,r4,#1
	beq sma01	; tmo = (0xffffffff)
	adds r4,r4,#1
	beq sma02	; tmo = ((0xffffffff)-1)
	lsrs r4,r3,#24
	beq sma01
	movs r1,r3
	ldr r0,sma_illValue
	b sma_err_cont	; => sc_sysError
sma_err6
	movs r1,r6
	ldr r0,sma_illModule
	b sma_err_cont	; => sc_sysError
sma_err5
	movs r1,#0
	ldr r0,sma_notPrio
	b sma_err_cont
	; tmo set && process not prio => error
sma01
	ldrh r4,[r5,#PCB_FLAGS_H]
	lsrs r4,r4,#PCB_FLG_IS_PRIO_POS+1
	bcc sma_err5
sma02
	cmp r0,#4	; size < 4 ?
	blo sma_err0	; yes => error
	ENDIF
	subs r4,r2,#1	; plid = SC_DEFAULT_POOL
	cmp r4,#255
	bne sma1	; no => continue
	ldrh r2,[r5,#PCB_DEFAULTPOOL_H]	; r2 := default pool
	lsls r4,r2,#2
	ldr r7,[r6,#MODULE_POOL]	; r7 := array of pools
	ldr r4,[r7,r4]	; r4 := addr of pool-CB
	cmp r4,#1
	bhi sma1	; r4 > 1 => pool valid
	ldr r0,sma_illDefPool	; sc_sysError((0x013<<12),
	movs r1,r2	; plid)
sma_err_cont
	bl sc_sysError
	mov r4,sp
	ldmia r4!,{r0-r3}	; restore parameters
	movs r2,#0	; and modify default-pool
	strh r2,[r5,#PCB_DEFAULTPOOL_H]
	b sma1
	; errors ----------------------
sma_err0
	movs r2,#(0x007<<12)>>12	; r2 := error-code
	movs r1,r0	; extra := buffersize
sma_cont
	lsls r2,r2,#12	; shift errorocde in position
	movs r0,#(0x01<<24)>>24
	lsls r0,r0,#24	; shift function code in pos.
	orrs r0,r0,r2	; build complete error-code
	bl sc_sysError
	add sp,sp,#4*4	; clean stack, if returning
	movs r0,#0
	RET r4-r7
sma_err2
	movs r2,#(0x007<<12)>>12
	movs r1,r0
	LOCK
sma_cont2
	ldr r3,[r7,#POOL_LOCK]
	subs r3,r3,#1
	str r3,[r7,#POOL_LOCK]
	IF SC_MSG_PARA_CHECK = 1
	b sma_cont1
sma_err1
	movs r1,r2
	movs r2,#(0x001<<12)>>12
	ENDIF
sma_cont1
	UNLOCK
sma_cont3
	lsls r2,r2,#12
	movs r0,#(0x01<<24)>>24
	lsls r0,r0,#24
	orrs r0,r0,r2
	adds r0,r0,#0x02
	b sma_err_cont	; => sc_sysError
	; -----------------------------
	; r0 - size
	; r1 - msgid
	; r2 - plid
	; r3 - tmo
	; r6 - MCB
sma1
	LOCK
	IF SC_MSG_PARA_CHECK = 1
	ldr r4,[r6,#MODULE_MAX_POOLS]	; plid in range ?
	cmp r2,r4
	bhs sma_err1
	ENDIF
	lsls r4,r2,#2
	ldr r7,[r6,#MODULE_POOL]
	ldr r7,[r7,r4]	; get addr of pool-cb
	IF SC_MSG_PARA_CHECK = 1
	cmp r7,#1	; valid ?
	bls sma_err1	; no => sysError
	ENDIF
	ldr r4,[r7,#POOL_LOCK]	; Lock pool, so we may enable
	adds r4,r4,#1	; interrupts again
	str r4,[r7,#POOL_LOCK]
	UNLOCK
	; find fixed buffersize
	IF CPUTYPE <= 4
	movs r4,#POOL_BUFSIZE
	adds r4,r4,r7	; r4 := addr of bufsizes-array
	IF SC_MAX_NUM_BUFFERSIZES = 16
	ldr r6,[r4,#7*4]
	cmp r0,r6
	bls sma2
	adds r4,r4,#8*4
sma2
	ENDIF
	IF SC_MAX_NUM_BUFFERSIZES >= 8
	ldr r6,[r4,#3*4]
	cmp r0,r6
	bls sma3
	adds r4,r4,#4*4
sma3
	ENDIF
	ldr r6,[r4,#1*4]
	cmp r0,r6
	bls sma4
	adds r4,r4,#2*4
sma4
	ldr r6,[r4,#0]
	cmp r0,r6
	bls sma5
	ldr r6,[r4,#4]
	adds r4,r4,#4
	cmp r0,r6
	bhi sma_err2
sma5
	subs r4,r4,r7
	subs r4,r4,#POOL_BUFSIZE	; r4 := buf-size-index * 4
	IF SC_MSG_STAT = 1
	movs r1,r7
	IF SC_MAX_NUM_BUFFERSIZES > 8
	adds r1,r1,#POOL_STAT_REQ-255
	adds r1,r1,#255
	ELSE
	adds r1,r1,#POOL_STAT_REQ
	ENDIF
	ldr r2,[r1,r4]	; POOL_STAT_REQ[bufsize-index]++
	adds r2,r2,#1
	str r2,[r1,r4]
	 IF SC_MAX_NUM_BUFFERSIZES > 8
	adds r1,r1,#POOL_STAT_MAX-POOL_STAT_REQ-1
	adds r1,r1,#1
	 ELSE
	adds r1,r1,#POOL_STAT_MAX-POOL_STAT_REQ
	 ENDIF
	ldr r2,[r1,r4]
	cmp r0,r2
	bls sma6
	str r0,[r1,r4]	; POOL_STAT_MAX[bufsize-index]
sma6
	ENDIF
	ELSE	; CPUTYPE <= 4
	movs r4,r7	; r4 := pool-cb
	IF SC_MAX_NUM_BUFFERSIZES = 16
	ldr r6,[r4,#POOL_BUFSIZE+7*4]
	cmp r0,r6
	it hi
	addhi r4,r4,#8*4
	ENDIF
	IF SC_MAX_NUM_BUFFERSIZES >= 8
	ldr r6,[r4,#POOL_BUFSIZE+3*4]
	cmp r0,r6
	it hi
	addhi r4,r4,#4*4
	ENDIF
	ldr r6,[r4,#POOL_BUFSIZE+1*4]
	cmp r0,r6
	it hi
	addhi r4,r4,#2*4
	ldr r6,[r4,#POOL_BUFSIZE+0]
	cmp r0,r6
	bls sma5
	ldr r6,[r4,#POOL_BUFSIZE+4]
	adds r4,r4,#4
	cmp r0,r6
	bhi sma_err2
sma5
	subs r4,r4,r7	; r4 := buf-size-index * 4
	IF SC_MSG_STAT = 1
	add r1,r7,#POOL_STAT_REQ
	ldr r2,[r1,r4]	; POOL_STAT_REQ[bufsize-index]++
	adds r2,r2,#1
	str r2,[r1,r4]
	adds r1,r1,#POOL_STAT_MAX-POOL_STAT_REQ
	ldr r2,[r1,r4]
	cmp r0,r2
	it hi
	strhi r0,[r1,r4]	; POOL_STAT_MAX[bufsize-index]
	ENDIF	; SC_MSG_STAT = 1
	ENDIF	; CPUTYPE <= 4
	adds r4,r4,r4	; r4 := buf_idx*8
	LOCK
sma7
	movs r1,r7
	adds r1,r1,#POOL_FREED	; r1 := addr of free-list
	ldr r0,[r1,r4]	; r0 := head of free-list
	adds r1,r1,r4
	cmp r0,r1	; head = tail ?
	beq sma10	; yes, => empty list
	ldr r2,[r0,#dbl_prev]	; remove tail
	str r2,[r1,#dbl_prev]
	str r1,[r2,#dbl_next]
sma8
	movs r2,r5	; link into PCB
	adds r2,r2,#PCB_ALLOCED
	str r2,[r0,#dbl_next] 
	ldr r1,[r2,#dbl_prev] 
	str r0,[r2,#dbl_prev] 
	str r1,[r0,#dbl_prev] 
	str r0,[r1,#dbl_next]
	ldr r1,[r5,#PCB_NALLOC]	; update PCB-stats
	adds r1,r1,#1
	str r1,[r5,#PCB_NALLOC]
	ldr r2,[r7,#POOL_ID]	; setup important part of msg-head
	ldr r1,[sp,#0 +4]	; w/ interrupts locked
	str r4,[r0,#16]
	str r2,[r0,#12]
	str r1,[r0,#8]
	IF SC_MSG_CHECK = 1
	adds r1,r1,r0	; set endmark
	adds r1,r1,#4	; 32 is 32 !
	movs r2,#0xee	; and
	strb r2,[r1,#32 -4]	; imm offset is 5 bit
	strb r2,[r1,#32 -3]
	strb r2,[r1,#32 -2]
	strb r2,[r1,#32 -1]
	ENDIF
	ldr r2,[r5,#PCB_PID]
	str r2,[r0,#28]	; msg_owner := current PID
	ldrh r2,[r5,#PCB_STATE_H]	; check if tmo is pending
	movs r1,#ST_WAIT_TMO
	tst r2,r1
	beq sma9
	bics r2,r2,r1 
	adds r5,r5,#PCB_SLEEPERS 
	ldr r3,[r5,#dbl_next] 
	ldr r1,[r5,#dbl_prev] 
	str r3,[r1,#dbl_next] 
	str r1,[r3,#dbl_prev] 
	subs r5,r5,#PCB_SLEEPERS 
	movs r1,#0 
	str r1,[r5,#PCB_TMO]	; yes, remove from timer list
sma9
	movs r1,#ST_TMO
	bics r2,r2,r1
	strh r2,[r5,#PCB_STATE_H]
	UNLOCK
	ldr r2,[sp,#4]
	movs r1,#0
	str r1,[r0,#20]	; clear msg_sender and
	str r1,[r0,#24]	; msg_addressee
	str r2,[r0,#32]	; set MSGID
	adds r0,r0,#32	; r0 := address of MSGID
	add sp,sp,#4*4	; clean stack
	IF SC_MSG_STAT = 1
	IF SC_MAX_NUM_BUFFERSIZES > 8
	adds r7,r7,#255
	adds r7,r7,#POOL_STAT_ALLOC-124-255
	ELSE
	adds r7,r7,#POOL_STAT_ALLOC-124
	ENDIF
	lsrs r2,r4,#1
	adds r7,r7,r2
	ldr r2,[r7,#124]	; POOL_STAT_ALLOC[bufsize-index]++
	adds r2,r2,#1
	str r2,[r7,#124]
	ENDIF
	RET r4-r7
	; no buffer of this size in free-list, so create new
sma10
	ldr r0,[r7,#POOL_CUR]	; r0 := end of currently used pool
	ldr r2,[r7,#POOL_END]	; r2 := abs. end of pool
	IF SC_MSG_CHECK = 1
	adds r1,r0,#4	; extra space for endmark
	ELSE
	movs r1,r0
	ENDIF
	adds r1,r1,r6	; buffsersize
	adds r1,r1,#32
	cmp r1,r2	; addr. > end of pool
	bhs sma12	; yes => no mem
	str r1,[r7,#POOL_CUR]	; no => save new end
	IF SC_MSG_CHECK = 1
	movs r2,#0xee	; set buffer endmark
	lsls r3,r2,#8
	orrs r2,r2,r3
	lsls r3,r2,#16
	orrs r2,r2,r3
	subs r1,r1,#4
	str r2,[r1,#0]
	ENDIF
	b sma8	; => setup the header
sma7_cont
	b sma7
	; no more memory in pool
sma12
	adds r0,r3,#-((0xffffffff)-1)	; tmo = ((0xffffffff)-1)
	beq sma_err3	; yes => error
	cmp r3,#(0)	; tmo = (0)
	beq sma18	; yes => return 0
	ldrh r0,[r5,#PCB_STATE_H]	; old tmo expired ?
	movs r2,#ST_TMO
	tst r0,r2
	bne sma17	; yes, return 0
	ldr r2,sma_sciopta	; remove from ready-list
	ldrh r0,[r5,#PCB_FLAGS_H] 
	movs r1,#PCB_FLG_READY 
	tst r0,r1 
	beq sma9999 
	bics r0,r0,r1 
	strh r0,[r5,#PCB_FLAGS_H] 
	ldr r0,[r5,#PCB_PCBLIST+dbl_next] 
	ldr r1,[r5,#PCB_PCBLIST+dbl_prev] 
	str r0,[r1,#dbl_next] 
	str r1,[r0,#dbl_prev] 
	cmp r0,r1 
	bne sma9998 
	ldr r1,[r5,#PCB_PRIO_MASK] 
	ldr r0,[r2,#SC_PRIOMASK] 
	bics r0,r0,r1 
	str r0,[r2,#SC_PRIOMASK] 
sma9998 
	movs r0,#0 
	str r0,[r5,#PCB_PCBLIST+dbl_next] 
	str r0,[r5,#PCB_PCBLIST+dbl_prev] 
sma9999
	adds r0,r7,r4	; and insert into pool's waiter
	adds r0,r0,#POOL_WAITER	; list
	adds r5,r5,#PCB_PCBLIST
	str r0,[r5,#dbl_next] 
	ldr r1,[r0,#dbl_prev] 
	str r5,[r0,#dbl_prev] 
	str r1,[r5,#dbl_prev] 
	str r5,[r1,#dbl_next]
	subs r5,r5,#PCB_PCBLIST
	adds r0,r3,#-(0xffffffff)	; tmo = (0xffffffff) (and r0 := 0)
	beq sma13	; yes =>
	; insert process in timer-list (destr. r3 !)
	IF CPUTYPE <= 4
	ldr r0,[r2,#SC_TCK] 
	adds r0,r0,r3 
	lsrs r1,r3,#18 
	bne sma992 
	lsrs r1,r3,#12 
	bne sma991 
	lsrs r1,r3,#6 
	bne sma990 
	movs r1,#0x3f 
	ands r1,r1,r0 
	movs r3,#0 
	b sma996 
sma990 
	movs r3,#0x3f 
	ands r3,r3,r0 
	lsrs r0,r0,#6 
	movs r1,#0x3f 
	ands r1,r1,r0 
	adds r1,r1,#64 
	b sma996 
sma991 
	lsls r3,r0,#20 
	lsrs r3,r3,#20 
	lsrs r0,r0,#12 
	movs r1,#0x3f 
	ands r1,r1,r0 
	adds r1,r1,#128 
	b sma996 
sma992 
	lsls r3,r0,#14 
	lsrs r3,r3,#14 
	lsrs r0,r0,#18 
	movs r1,#0x3f 
	ands r1,r1,r0 
	adds r1,r1,#192 
sma996 
	lsls r1,r1,#3 
	str r3,[r5,#PCB_TMO] 
	adds r1,r1,#SC_SLEEPERLISTS-255 
	adds r1,r1,#255 
	movs r0,r5 
	adds r0,r0,#PCB_SLEEPERS 
	adds r1,r1,r2 
	str r1,[r0,#dbl_next] 
	ldr r3,[r1,#dbl_prev] 
	str r0,[r1,#dbl_prev] 
	str r3,[r0,#dbl_prev] 
	str r0,[r3,#dbl_next]
	ELSE
	ldr r0,[r2,#SC_TCK] 
	adds r0,r0,r3 
	lsrs r1,r3,#18 
	bne sma992 
	lsrs r1,r3,#12 
	bne sma991 
	lsrs r1,r3,#6 
	bne sma990 
	and r1,r0,#0x3f 
	movs r3,#0 
	b sma996 
sma990 
	and r3,r0,#0x3f 
	ubfx r1,r0,#6,#6 
	adds r1,r1,#64 
	b sma996 
sma991 
	ubfx r3,r0,#0,#12 
	ubfx r1,r0,#12,#6 
	adds r1,r1,#128 
	b sma996 
sma992 
	ubfx r3,r0,#0,#18 
	ubfx r1,r0,#18,#6 
	adds r1,r1,#192 
sma996 
	str r3,[r5,#PCB_TMO] 
	add r1,r2,r1,lsl #3 
	add r1,r1,#SC_SLEEPERLISTS 
	adds r0,r5,#PCB_SLEEPERS 
	str r1,[r0,#dbl_next] 
	ldr r3,[r1,#dbl_prev] 
	str r0,[r1,#dbl_prev] 
	str r3,[r0,#dbl_prev] 
	str r0,[r3,#dbl_next]
	ENDIF
	movs r0,#ST_WAIT_TMO
sma13
	ldrh r1,[r5,#PCB_STATE_H]
	movs r2,#ST_WAIT_MSGALLOC
	orrs r1,r1,r0
	orrs r1,r1,r2
	strh r1,[r5,#PCB_STATE_H]	; set new process state
	UNLOCK
	SWAP_OUT_SAVE {r4-r7}	; swap out and save working regs
	movs r3,#0
	subs r3,r3,#-(0xffffffff)	; mark: do not set tmo
	movs r1,#ST_WAIT_MSGALLOC
	movs r2,#ST_TMO
	LOCK
	cmp r0,#0	; swap-out succesful ?
	ldrh r0,[r5,#PCB_STATE_H]	; get state (don't change flags!)
	beq sma_err4	; no => error
	tst r0,r1	; wait_msgalloc cleared ?
	beq sma7_cont	; yes, maybe a buffer in freelist
	bics r0,r0,r1	; no, clear ST_WAIT_MSGALLOC state
	tst r0,r2	; tmo expired
	beq sma_ill_state	; no, illegal state
sma17
	bics r0,r0,r2
	strh r0,[r5,#PCB_STATE_H]	; yes, clear flag
	; no buffer, tmo = 0 or tmo expired
sma18
	ldr r0,[r7,#POOL_LOCK]	; release pool-lock
	subs r0,r0,#1
	str r0,[r7,#POOL_LOCK]
	UNLOCK
	movs r0,#0	; return nil
	add sp,sp,#4*4	; clean stack (remove saved parameters)
	RET r4-r7
sma_err3
	movs r2,#(0x00d<<12)>>12
	ldr r1,[sp,#0 +4]
	b sma_cont2
sma_err4
	movs r1,#ST_WAIT_TMO
	tst r0,r1
	beq sma_err4a
	bics r0,r0,r1 
	adds r5,r5,#PCB_SLEEPERS 
	ldr r3,[r5,#dbl_next] 
	ldr r1,[r5,#dbl_prev] 
	str r3,[r1,#dbl_next] 
	str r1,[r3,#dbl_prev] 
	subs r5,r5,#PCB_SLEEPERS 
	movs r1,#0 
	str r1,[r5,#PCB_TMO]	; yes, remove from timer list
	strh r0,[r5,#PCB_STATE_H]
sma_err4a
	UNLOCK
	movs r2,#(0x014<<12)>>12
	movs r1,#0
	b sma_cont3
sma_ill_state
	movs r1,r0
	ldr r0,sma_illValue
	b sma_err_cont	; => sc_sysError
	ALIGN
sma_sciopta
	DCD sciopta
sma_illDefPool
	DCD ((0x013<<12)|(0x01<<24))
sma_illValue
	DCD ((0x030<<12)|(0x01<<24)|0x02)
	IF SC_MSG_PARA_CHECK = 1
sma_illModule
	DCD ((0x027<<12)|(0x01<<24)|0x01)
sma_notPrio
	DCD ((0x02d<<12)|(0x01<<24)|0x02)
	ENDIF
	SC_ENDFUNC sc_sysMsgAlloc
;
;**************************************
; void sc_msgFree(sc_msgptr_t msgptr)
;
; r0 - msgptr
;
;
	XSC_TFUNC sc_msgFree,-12
	SC_TFUNC sc_msgFreex
	IF SC_MSG_PARA_CHECK = 1
smf_err0
	ldr r0,smf_err
	bl sc_sysError
	ENDIF
	SC_TPUBLIC sc_msgFree
	IF SC_MSG_PARA_CHECK = 1
	movs r1,r0	; msgptr = 0 ?
	beq smf_err0	; yes => error
	ldr r0,[r1,#0]
	cmp r0,#0	; *msgptr = 0 ?
	beq smf_err0	; yes => error
	movs r2,#0
	str r2,[r1,#0]	; *msgptr = 0 (take away from process)
	ELSE
	ldr r3,[r0,#0]
	movs r2,#0
	str r2,[r0,#0]
	movs r0,r3
	ENDIF
	subs r0,r0,#32	; r0 := addr of msg-header
	IF SC_MSG_CHECK = 1
	push {r4-r7,lr}
	movs r1,#(0x02<<24)>>24
	lsls r1,r1,#24
	bl sc_msgCheck	; msgCheck(msghead,function_code)
	b smf0
	ENDIF
	SC_TPUBLIC sc_sysMsgFree
	push {r4-r7,lr}
smf0
	ldr r3,[r0,#12]	; r3 := msg-pool id
	ldr r6,smf_sc_modules
	lsls r4,r3,#1	; (remove module0 flag)
	lsrs r4,r4,#24-1	; r4 := module-index
	ldr r5,[r6,r4]	; r5 := addr of pool MCB
	ldr r4,smf_sciopta
	ldr r1,[r5,#MODULE_POOL]	; r1 := pool-cb array
	lsls r3,r3,#8
	lsrs r3,r3,#6	; r3 := pool-index
	ldr r3,[r1,r3]	; r3 := pool-cb
	ldr r5,[r0,#16]	; r5 := bufsize index (*8 !!)
	IF SC_MSG_STAT = 1
	lsrs r1,r5,#1	; r1 := bufsize index (*4)
	adds r1,r1,#255
	IF SC_MAX_NUM_BUFFERSIZES > 8
	adds r1,r1,#255
	adds r1,r1,#POOL_STAT_FREE-510
	 ELSE
	adds r1,r1,#POOL_STAT_FREE-255
	 ENDIF
	ldr r2,[r3,r1]	; POOL_STAT_FREE[bufsize_index]++
	adds r2,r2,#1
	str r2,[r3,r1]
	ENDIF
	movs r1,#0
	LOCK
	ldr r2,[r0,#28]	; save msg-owner
	str r1,[r0,#28]	; clear msg_owner
	str r1,[r0,#20]	; and sender
	str r2,[r0,#24]	; and remember last owner
	cmp r2,#0	; last owner has been kernel ?
	beq smf1	; yes =>
	lsrs r1,r2,#24
	lsls r1,r1,#2
	ldr r1,[r6,r1]	; last owners MCB
	lsls r6,r2,#18	; pid index
	lsrs r6,r6,#18-2
	cmp r1,#0	; MCB = 0 => skip
	beq smf1
	ldr r1,[r1,#MODULE_PCBS]
	ldr r6,[r1,r6]	; last owner's PCB
	cmp r6,#0
	beq smf1	; not there => skip
	ldr r1,[r6,#PCB_PID]	; still the same PID ?
	cmp r1,r2
	bne smf1	; no, do not update stats
	ldr r1,[r6,#PCB_NALLOC]
	subs r1,r1,#1
	str r1,[r6,#PCB_NALLOC]
smf1
	ldr r1,[r0,#dbl_next] 
	ldr r2,[r0,#dbl_prev] 
	str r1,[r2,#dbl_next] 
	str r2,[r1,#dbl_prev]	; remove msg from current owners queue
	adds r1,r5,r3	; r1 := free-list
	adds r1,r1,#POOL_FREED
	str r1,[r0,#dbl_next] 
	ldr r2,[r1,#dbl_prev] 
	str r0,[r1,#dbl_prev] 
	str r2,[r0,#dbl_prev] 
	str r0,[r2,#dbl_next]	; and insert into free-list
	ldr r1,[r3,#POOL_LOCK]	; release pool
	subs r1,r1,#1
	str r1,[r3,#POOL_LOCK]
	adds r5,r5,r3	; check for processes waiting
	adds r5,r5,#POOL_WAITER
	ldr r0,[r5,#dbl_next]
	cmp r0,r5	; head = tail => list empty
	bne smf3
smf2
	UNLOCK
	RET r4-r7
smf3
	ldr r2,[r0,#dbl_next]	; ldr r2,[r0,#dbl_next] 
	ldr r5,[r0,#dbl_prev] 
	str r2,[r5,#dbl_next] 
	str r5,[r2,#dbl_prev]
	str r2,[r5,#dbl_next]
	str r5,[r2,#dbl_prev]
	subs r0,r0,#PCB_PCBLIST	; r0 := PCB of waiter
	ldrh r1,[r0,#PCB_STATE_H]	; r1 := process state
	movs r2,#ST_WAIT_MSGALLOC	; clear state
	bics r1,r1,r2
	strh r1,[r0,#PCB_STATE_H]
	lsrs r1,r1,#ST_STOPPED_POS+1	; process now ready
	bcs smf2	; no, => leave
	ldrh r1,[r0,#PCB_FLAGS_H] 
	movs r2,#PCB_FLG_READY 
	tst r1,r2 
	bne smfmr9999 
	orrs r1,r1,r2 
	strh r1,[r0,#PCB_FLAGS_H] 
	ldr r3,[r4,#SC_PRIOMASK] 
	ldr r2,[r0,#PCB_PRIO_MASK] 
	ldr r1,[r0,#PCB_READYLIST] 
	orrs r3,r3,r2 
	str r3,[r4,#SC_PRIOMASK] 
	adds r0,r0,#PCB_PCBLIST 
	str r1,[r0,#dbl_next] 
	ldr r2,[r1,#dbl_prev] 
	str r0,[r1,#dbl_prev] 
	str r2,[r0,#dbl_prev] 
	str r0,[r2,#dbl_next] 
	subs r0,r0,#PCB_PCBLIST 
smfmr9999	; else insert into ready-list
	UNLOCK
	ldrb r1,[r0,#PCB_PRIO_B]	; higher prio than current prio proc ?
	ldr r2,[r4,#SC_CURPRIOPCB]
	ldrb r2,[r2,#PCB_PRIO_B]
	cmp r1,r2
	bhs smf5	; no, leave
	SWAP_OUT
smf5
	RET r4-r7
	ALIGN
smf_sciopta
	DCD sciopta
smf_sc_modules
	DCD sc_modules
	IF SC_MSG_PARA_CHECK = 1
smf_err
	DCD (0x00A<<12)|(0x02<<24)|0x02
	ENDIF
	SC_ENDFUNC sc_msgFreex
;
;**************************************
; void sc_msgTx(sc_msgptr_t msgptr,sc_pid_t addr,flags_t flags)
; void sc_msgTxAlias(sc_msgptr_t msgptr,
; sc_pid_t addr,
; flags_t flags,
; sc_pid_t alias)
;
; CHECKS: - msgptr != 0 and *msgptr =! 0
; - msgheader valid
; - addr valid
; - flags correct
;**************************************
;
	XSC_TFUNC sc_msgTx,-4
	XSC_TFUNC sc_msgTxAlias,0
	SC_TFUNC sc_msgTx
	movs r3,#0	; clear alias
	push {r0-r7,lr}	; save working regs and parameter
	movs r4,#(0x08<<24)>>24	; set function code
	lsls r4,r4,#24
	b mtx1	; => common code
	SC_TPUBLIC sc_msgTxAlias
	push {r0-r7,lr}	; save working regs and parameter
	movs r4,#(0x09<<24)>>24	; set function code
	lsls r4,r4,#24
	b mtx1	; => common code
	IF SC_MSG_PARA_CHECK = 1
mtx_err0
	movs r1,r0
	ldr r0,mtx_lit1
	pop {r2}
	orrs r0,r0,r2
	bl sc_sysError
	ENDIF
	SC_TPUBLIC sc_sysMsgTx
	push {r0-r7,lr}	; internal tx entry
mtx1
	push {r4}	; save function code
	IF SC_MSG_PARA_CHECK = 1
	cmp r0,#0	; msgptr = 0 ?
	beq mtx_err0	; yes => error
	movs r3,#0
	ldr r4,[r0,#0]
	cmp r4,#0	; *msgptr = 0
	beq mtx_err0	; yes => error
	str r3,[r0,#0]	; take away from caller
	ELSE
	ldr r4,[r0,#0]
	movs r3,#0
	str r3,[r0,#0]
	ENDIF
	subs r4,r4,#32	; r4 := msg-header
	IF SC_MSG_CHECK = 1
	movs r0,r4
	push {r1,r4}	; save addressee and msg-header
	ldr r1,[sp,#8]	; r1 := function code
	bl sc_msgCheck
	pop {r1,r4}
	ENDIF
	ldr r7,mtx_sciopta
	adds r0,r1,#-0xffffffff	; addressee = 0xffffffff
	bne mtx2
	ldr r1,[r7,#SC_CURPID]	; yes, r1 := current pid
mtx2
	LOCK
	IF SC_PROC_PARA_CHECK = 1
	ldr r2,mtx_lit6
	cmp r1,r2	; addressee = 0x7fffffff ?
	beq mtx_err3	; yes => error
	ENDIF
	movs r5,#0	; clear original pid
	cmp r1,#0	; addressee >= 0 ?
	IF SC_PROC_PARA_CHECK = 1
	beq mtx_err3	; = 0 => error
	ENDIF
	bpl mtx3	; > 0 => internal pid
	IF SC_MAX_CONNECTOR > 0
	movs r5,r1	; save original pid
	lsls r0,r1,#1
	lsrs r0,r0,#25	; r0 := connector index
	cmp r0,#SC_MAX_CONNECTOR	; in range ?
	bhs mtx_err2	; no => error
	ldr r2,mtx_sc_connectors
	lsls r0,r0,#2
	ldr r0,[r2,r0]	; r0 := pcb of connector
	cmp r1,#1	; valid ?
	bls mtx_err2	; no => error
	ldr r1,[r0,#PCB_PID]	; addressee = connector pid
	b mtx3	; continue
mtx_err2
	ENDIF
mtx_err3
	ldr r0,mtx_lit4	; (0x020<<12)|0x02
mtx_err_cont1
	UNLOCK
	pop {r2}
	orrs r0,r0,r2
	bl sc_sysError
	; PID gone
mtx4
	IF SC_MAX_CONNECTOR > 0
	ldr r0,mtx_sc_connectors
	ldr r7,[r0,#0]	; default connector set ?
	cmp r7,#0
	beq mtx5	; no => free msg
	movs r5,r1
	ldr r1,[r7,#PCB_PID]	; get connector pid
	ldr r6,mtx_sc_modules
	lsrs r0,r1,#24
	lsls r0,r0,#2
	ldr r6,[r6,r0]	; r6 := connector MCB
	b mtx6
	ENDIF
mtx5
	UNLOCK
	movs r0,r4
	bl sc_sysMsgFree
	add sp,sp,#5*4
	RET r4-r7
mtx3
	lsrs r0,r1,#24	; r0 := addressee's module id
	IF SC_PROC_PARA_CHECK = 1
	cmp r0,#SC_MAX_MODULE	; in range ?
	bhs mtx_err3	; no => error
	ENDIF
	ldr r6,mtx_sc_modules
	lsls r0,r0,#2
	ldr r6,[r6,r0]	; r6 := addressee's MCB
	IF SC_PROC_PARA_CHECK = 1
	cmp r6,#0	; valid ?
	beq mtx_err3	; no => error
	ENDIF
	lsls r0,r1,#18
	lsrs r0,r0,#18-2	; r0 := process index (*4 !)
	IF SC_PROC_PARA_CHECK = 1
	ldr r2,[r6,#MODULE_MAX_PROCESS]
	lsrs r7,r0,#2	; index
	cmp r7,r2
	bhs mtx_err3
	ENDIF
	ldr r2,[r6,#MODULE_PCBS]
	ldr r7,[r2,r0]	; r7 := addressee's PCB
	cmp r7,#0	; valid ?
	beq mtx4	; no, default connector
	ldr r0,[r7,#PCB_PID]
	cmp r0,r1	; PID = addressee ?
mtx4_cont
	bne mtx4	; no, PID gone
mtx6
	str r1,[sp,#8 +4]	; save real addressee
	IF SC_NEVER_COPY = 1
	ELSE
	ldr r2,mtx_sciopta
	ldr r2,[r2,#SC_CURMODULE]
	cmp r2,r6	; addresee_module = sender_module ?
	beq mtx11	; yes, no copy
	IF 0 = 1
	ldrh r2,[r6,#MODULE_RPIDX_H]	;
	cmp #0,r2	; reverse proxy ?
	beq mtx6a	; = 0 => no => skip
	adds r2,r2,#MODULE_PCBS]
	ldr r2,[r6,r2]
	ldr r2,[r2,#PCB_PID]
mtx6a
	ENDIF
	IF SC_USE_FRIENDS = 1
	movs r2,#31
	lsrs r0,r1,#24	; r0 := modul id
	ands r2,r2,r0	; r2 := bit number
	lsrs r0,r0,#5	; r0 := word index
	ldr r3,[r4,#12]	; r3 := modul-id of msg
	ldr r1,mtx_sc_modules	; r1 := sc_modules[]
	lsls r3,r3,#1	; clear module-0 flag
	lsrs r3,r3,#25	; r3 := module-idx
	lsls r3,r3,#2	; r3 := long-offset
	ldr r1,[r1,r3]	; r1 := module[msg->plid]
	lsls r0,r0,#2	; r0 := long offset
	adds r0,r0,#MODULE_FRIENDSET
	ldr r3,[r1,r0]	; get friend word
	adds r2,r2,#1	; +1 for carry check
	lsrs r3,r3,r2	; test bit
	bcs mtx11	; carry => module is friend
	ENDIF	; SC_NEVER_COPY
	; copy message
mtx7
	UNLOCK
	push {r7}	; save addressee's PCB
	movs r3,#-((0xffffffff)-1)	; allocate new message
	rsbs r3,r3,#0	; from addressee's default-pool
	ldrh r2,[r7,#PCB_DEFAULTPOOL_H]
	ldr r1,[r4,#32]
	ldr r0,[r4,#8]
	ldr r7,mtx_sciopta
	bl sc_sysMsgAlloc
	pop {r7}	; restore PCB
	ldr r1,[r4,#8]	; copy msg contents
	push {r0,r4}
	adds r4,r4,#32
	lsrs r2,r1,#2
mtx8
	ldmia r4!,{r3}
	subs r2,r2,#1
	stmia r0!,{r3}
	bne mtx8
	lsrs r2,r1,#2
	bcc mtx9
	ldrh r3,[r4,#0]
	strh r3,[r0,#0]
	adds r4,r4,#2
	adds r0,r0,#2
mtx9
	lsrs r2,r1,#1
	bcc mtx10
	ldrb r3,[r4,#0]
	strb r3,[r0,#0]
mtx10
	pop {r0,r1}
	movs r4,r0
	movs r0,r1
	bl sc_sysMsgFree	; free original msg
	subs r4,r4,#32
	LOCK
	ENDIF	; SC_NEVER_COPY
mtx11
	ldr r6,mtx_sciopta	; remove msg from senders alloc-queue
	ldr r0,[r4,#dbl_next] 
	ldr r1,[r4,#dbl_prev] 
	str r0,[r1,#dbl_next] 
	str r1,[r0,#dbl_prev]
	ldr r0,[r6,#SC_CURPCB]	; r0 := sender's PCB
	ldr r1,[r0,#PCB_NALLOC]	; update stats
	subs r1,r1,#1
	str r1,[r0,#PCB_NALLOC]
	UNLOCK
	ldr r0,[r4,#28]	; r0 := msg_owner
	ldr r3,[sp,#16]	; r3 := alias
	cmp r3,#0	; = 0 ?
	beq mtx13	; yes, => not set
	movs r0,r3
mtx13
	str r0,[r4,#20]	; set msg sender
	ldr r0,[sp,#8]	; r0 := addressee
	cmp r5,#0	; connector ?
	bne mtx14	; yes
	movs r5,r0	; no, use original
mtx14
	str r5,[r4,#24]	; set addressee
	movs r5,#0
	str r5,[r4,#28]	; mark: msg in queue
	LOCK
	ldr r1,[r7,#PCB_PID]	; r1 := addressee's pid
	cmp r1,r0	; addressee still alive ?
	bne mtx4_cont	; no, pid gone ..
	ldr r1,[sp,#12 +4]	; get sc_msgTx-flags
	cmp r1,#0	; normal tx ?
	bne mtx20	; no, check more =>
	IF SC_MSG_HOOK = 1
	IF SC_MSGTX_HOOK = 1
	ldr r3,[r6,#SC_CURMODULE]
	ldr r2,[r3,#MODULE_MSGTXHOOK]
	cmp r2,#0
	beq mtx15
	movs r0,r4
	BLX_R2_t	; txhook(msg)
mtx15
	ldr r1,mtx_lit8
	ldr r2,[r1,#0]
	cmp r2,#0
	beq mtx16
	movs r0,r4
	BLX_R2_t	; txhook(msg)
mtx16
	ENDIF
	ENDIF
	movs r0,#PCB_MSGQUEUE	; insert into addressee's rx-queue
	adds r0,r0,r7
	str r0,[r4,#dbl_next] 
	ldr r1,[r0,#dbl_prev] 
	str r4,[r0,#dbl_prev] 
	str r1,[r4,#dbl_prev] 
	str r4,[r1,#dbl_next]
	ldr r0,[r7,#PCB_NQUEUE]	; and update stats
	adds r0,r0,#1
	str r0,[r7,#PCB_NQUEUE]
	ldr r1,[r7,#PCB_FLAGS_H]	; get flags and state
	IF LITTLE_ENDIAN = 1
	lsrs r0,r1,#PCB_FLG_IS_PRIO_POS+1
	ELSE
	lsrs r0,r1,#PCB_FLG_IS_PRIO_POS+1+16
	ENDIF
	bcc mtx18	; no carry, not a prio process
	IF LITTLE_ENDIAN = 0
	lsls r1,r1,#16	; BE: move state in position
	ENDIF
	lsrs r1,r1,#16	; r1 := process-state
	movs r0,#ST_WAIT_MSGRX	; addressee waiting for msg ?
	tst r1,r0
	beq mtx_exit	; no, leave =>
	bics r1,r1,r0	; clear state
	strh r1,[r7,#PCB_STATE_H]	; and save new state
	lsrs r1,r1,#ST_STOPPED_POS+1	; process stopped ?
	bcs mtx_exit	; yes, leave
	ldrh r0,[r7,#PCB_FLAGS_H] 
	movs r1,#PCB_FLG_READY 
	tst r0,r1 
	bne mtxmr9999 
	orrs r0,r0,r1 
	strh r0,[r7,#PCB_FLAGS_H] 
	ldr r2,[r6,#SC_PRIOMASK] 
	ldr r1,[r7,#PCB_PRIO_MASK] 
	ldr r0,[r7,#PCB_READYLIST] 
	orrs r2,r2,r1 
	str r2,[r6,#SC_PRIOMASK] 
	adds r7,r7,#PCB_PCBLIST 
	str r0,[r7,#dbl_next] 
	ldr r1,[r0,#dbl_prev] 
	str r7,[r0,#dbl_prev] 
	str r1,[r7,#dbl_prev] 
	str r7,[r1,#dbl_next] 
	subs r7,r7,#PCB_PCBLIST 
mtxmr9999	; yes, insert into readylist
	UNLOCK
	add sp,sp,#5*4	; clean stack
	ldr r0,[r6,#SC_CURPRIOPCB]
	ldrb r4,[r7,#PCB_PRIO_B]
	ldrb r1,[r0,#PCB_PRIO_B]
	cmp r4,r1	; addressee.prio < curpriopcb.prio ?
	bhs mtx17c	; no, leave
	SWAP_OUT
mtx17c
	RET r4-r7
	; addressee not a prio proc
mtx18
	lsrs r0,r1,#PCB_FLG_IS_IRQ_POS+1
	bcs mtx18a	; IRQ process => cont
	lsrs r0,r1,#PCB_FLG_IS_TIMER_POS+1
	bcc mtx_err4	; also not a timer => error
mtx18a
	UNLOCK
	add sp,sp,#5*4	; clean stack
		; 29.09.05
		; wakeup allowed ?
	IF LITTLE_ENDIAN = 1
	lsrs r0,r1,#PCB_FLG_WAKEUP_DIS_POS+1
	ELSE
	lsrs r0,r1,#PCB_FLG_WAKEUP_DIS+1+16
	ENDIF
	bcs mtx_exit1
		; *********
	movs r0,#1	; wake up process
	movs r1,r7	; PCB
	swi 0
	ldr r0,[r6,#SC_SCHEDULE_REQ]	; re-scheduling needed ?
	cmp r0,#0
	beq mtx_exit1	; no, return
	SWAP_OUT
mtx_exit1
	RET r4-r7
	; flags != 0
mtx19
	b mtx_err_cont1
mtx20
	lsrs r0,r1,#0 +1	; observe ?
	ldr r0,mtx_lit7
	bcc mtx19	; no, error
	movs r0,#PCB_CONNECTED	; yes, insert into connected-queue
	adds r0,r0,r7
	str r0,[r4,#dbl_next] 
	ldr r1,[r0,#dbl_prev] 
	str r4,[r0,#dbl_prev] 
	str r1,[r4,#dbl_prev] 
	str r4,[r1,#dbl_next]
	ldr r0,[r7,#PCB_NALLOC]	; update observants stats
	adds r0,r0,#1
	str r0,[r7,#PCB_NALLOC]
mtx_exit
	UNLOCK
	add sp,sp,#5*4
	RET r4-r7
mtx_err4
	ldr r0,mtx_illproc_type
	ldr r1,[r7,#PCB_PID]	; r1 := addressee's pid
	b mtx_err_cont1
	ALIGN
	IF SC_MSG_PARA_CHECK = 1
mtx_lit1
	DCD (0x00A<<12)|0x02
	ENDIF
mtx_sciopta
	DCD sciopta
	IF SC_MAX_CONNECTOR > 0
mtx_sc_connectors
	DCD sc_connectors
	ENDIF
mtx_lit4
	DCD (0x020<<12)|0x02
mtx_sc_modules
	DCD sc_modules
mtx_lit6
	DCD 0x7fffffff
mtx_lit7
	DCD (0x030<<12)|0x02
	IF SC_MSG_HOOK = 1
	IF SC_MSGTX_HOOK = 1
mtx_lit8
	DCD sciopta+SC_MSGTXHOOK
	ENDIF
	ENDIF
mtx_illproc_type
	DCD (0x015<<12)|0x02
	SC_ENDFUNC sc_msgTx
;
;**************************************
; sc_msg_t sc_msgRx(sc_ticks_t tmo,const void *wanted,flags_t flags)
;**************************************
;
	XSC_TFUNC sc_msgRx,-4
	SC_TFUNC sc_msgRx
	push {r0-r2,r4-r7,lr}	; save parameters and working regs
	ldr r7,mrx_sciopta
	ldr r6,[r7,#SC_CURPCB]
	IF SC_MSG_PARA_CHECK = 1
	ldrh r5,[r6,#PCB_FLAGS_H]	; r5:= caller's flags
	cmp r0,#(0)
	beq mrx0
	lsrs r5,r5,#PCB_FLG_IS_PRIO_POS+1	; is caller prio process ?
	bcc mrx_err2	; no => error
	adds r3,r0,#-((0xffffffff))	; tmo = (0xffffffff) ?
	beq mrx0	; yes =>
	lsrs r3,r0,#24	; tmo <= SC_MAX_TMO
	bne mrx_err0	; no, error =>
mrx0
	movs r3,#(0x01 +0x02)
	ands r3,r3,r2
	bne mrx1	; flags set => ok
	cmp r1,#0	; no flags and select = NULL ?
	beq mrx1	; ok =>
	ldr r3,[r1,#0]	; no flags and select empty ?
	cmp r3,#0
	bne mrx_err1	; no, error
mrx1
	ENDIF
	adds r6,r6,#PCB_MSGQUEUE	; r6 := mqueue
	cmp r1,#0	; select = NULL
	bne rx_w_select	; no, special handling
rx_wo_select
	LOCK
	b mrx3a
mrx3
	bl mrx_empty
mrx3a
	ldr r4,[r6,#dbl_next]	; r4 := head
	cmp r6,r4	; head = tail ?
	beq mrx3	; yes => list empty
mrx_found_msg
	ldr r0,[r4,#dbl_next] 
	ldr r1,[r4,#dbl_prev] 
	str r0,[r1,#dbl_next] 
	str r1,[r0,#dbl_prev]	; remove from mqueue
	subs r6,r6,#PCB_MSGQUEUE
	ldr r0,[r6,#PCB_NQUEUE]	; update stats
	ldr r1,[r6,#PCB_NALLOC]
	subs r0,r0,#1
	adds r1,r1,#1
	str r0,[r6,#PCB_NQUEUE]
	str r1,[r6,#PCB_NALLOC]
	movs r0,r6
	adds r0,r0,#PCB_ALLOCED	; insert into alloc-queue
	str r0,[r4,#dbl_next] 
	ldr r1,[r0,#dbl_prev] 
	str r4,[r0,#dbl_prev] 
	str r1,[r4,#dbl_prev] 
	str r4,[r1,#dbl_next]
	ldr r0,[r6,#PCB_PID]	; and set current process as
	str r0,[r4,#28]	; msg_owner
	ldr r1,[r6,#PCB_FLAGS_H]	; LE: state:flags
	IF LITTLE_ENDIAN = 1
	lsrs r0,r1,#PCB_FLG_IS_PRIO_POS+1
	bcc mrx42	; not a prio, ignore tmo-bit
	lsrs r1,r1,#16	; r1 := process-state
	ELSE
	lsrs r0,r1,#PCB_FLG_IS_PRIO_POS+1+16
	bcc mrx42	; not a prio, ignore tmo-bit
	ENDIF
	movs r0,#ST_WAIT_TMO	; TMO pending ?
	tst r1,r0	; clean up ?
	beq mrx41
	bics r1,r1,r0 
	adds r6,r6,#PCB_SLEEPERS 
	ldr r2,[r6,#dbl_next] 
	ldr r0,[r6,#dbl_prev] 
	str r2,[r0,#dbl_next] 
	str r0,[r2,#dbl_prev] 
	subs r6,r6,#PCB_SLEEPERS 
	movs r0,#0 
	str r0,[r6,#PCB_TMO]	; remove from tmo list
mrx41
	movs r0,#ST_TMO	; clear expired flag
	bics r1,r1,r0
	strh r1,[r6,#PCB_STATE_H]
mrx42
	UNLOCK
	movs r0,r4
	IF SC_MSG_HOOK = 1
	IF SC_MSGRX_HOOK = 1
	ldr r1,[r7,#SC_CURMODULE]
	ldr r2,[r1,#MODULE_MSGRXHOOK]
	cmp r2,#0
	beq mrx4
	BLX_R2_t	; rxHook(msg)
	movs r0,r4
mrx4
	ldr r2,mrx_SC_MSGRXHOOK
	ldr r2,[r7,r2]
	cmp r2,#0
	beq mrx5
	BLX_R2_t	; rxHook(msg)
	movs r0,r4
mrx5
	ENDIF
	ENDIF
	adds r0,r0,#32	; r0 := msg
	add sp,sp,#3*4	; clean stack
	RET r4-r7
mrx_err0
	movs r1,r0
	ldr r0,mrx_err0_const
	IF SC_MSG_PARA_CHECK = 1
	b mrx_err_cont
mrx_err1
	movs r1,#0
	ldr r0,mrx_err1_const
	ENDIF
mrx_err_cont
	bl sc_sysError
	IF SC_MSG_PARA_CHECK = 1
	movs r2,#0x02
	str r2,[sp,#8]
	ldr r1,[sp,#4]
	ldr r0,[sp,#0]
	b mrx1
mrx_err2
	movs r1,#0
	ldr r0,mrx_err2_const
	b mrx_err_cont
	ENDIF
	; rx with select array
rx_w_select
	ldr r3,[r1,#0]
	cmp r3,#0	; empty list, simple handling
	beq rx_wo_select
	movs r4,r6	; r4 := mqeue head
	lsrs r2,r2,#1	; 0x01 ?
	bcs rx_pid	; carry = 1 => yes
	lsrs r2,r2,#2	; 0x04 ?
	bcs rx_notmsgid	; carry = 1 => yes
mrx9
	movs r5,r4	; r5 := last list entry
	LOCK
	bl rx_msgid	; lr := rx_msgid
rx_msgid
	ldr r4,[r5,#dbl_next]	; r4 := current entry
	cmp r6,r4	; current entry = tail
	beq mrx_empty	; yes, end of list =>
	UNLOCK
	ldr r0,[sp,#4]	; r0 := select array
	ldr r1,[r4,#32]	; r1 := msgid
mrx10
	ldmia r0!,{r2}	; r2 := msgid from select, r0 += 4
	cmp r2,#0	; end of list ?
	beq mrx9	; yes, check next msg in mqueue
	cmp r2,r1	; wanted ?
	bne mrx10	; no, check next in select-list
mrx_found_msg2
	LOCK	; found
	b mrx_found_msg	; => common code
rx_notmsgid
	movs r5,r4	; r5 := last list entry
	LOCK
	bl mrx11	; lr := mrx11
mrx11
	ldr r4,[r5,#dbl_next]	; r4 := current entry
	cmp r6,r4	; current entry = tail
	beq mrx_empty	; yes, end of list =>
	UNLOCK
	ldr r0,[sp,#4]
	ldr r1,[r4,#32]	; r1 := msgid
mrx12
	ldmia r0!,{r2}	; r2 := msgid from select, r0 += 4
	cmp r2,#0	; end of list ?
	beq mrx_found_msg2	; yes, msg wanted
	cmp r2,r1	; msg not wanted ?
	beq rx_notmsgid	; yes, next message
	b mrx12	; next entry in select-list
rx_pid
	lsrs r2,r2,#1	; 0x02 set ?
	bcs rx_both	; carry = 1 => yes
	lsrs r2,r2,#1	; 0x04 set ?
	bcs rx_notpid	; carry = 1 => yes
mrx13
	movs r5,r4
	LOCK
	bl mrx14
mrx14
	ldr r4,[r5,#dbl_next]
	cmp r6,r4
	beq mrx_empty
	UNLOCK
	ldr r0,[sp,#4]
	ldr r1,[r4,#20]
mrx15
	ldmia r0!,{r2}
	cmp r2,#0
	beq mrx13
	cmp r2,r1
	bne mrx15
	LOCK
	b mrx_found_msg
rx_notpid
	movs r5,r4
	LOCK
	bl mrx16
mrx16
	ldr r4,[r5,#dbl_next]
	cmp r6,r4
	beq mrx_empty
	UNLOCK
	ldr r0,[sp,#4]
	ldr r1,[r4,#20]
mrx17
	ldmia r0!,{r2}
	cmp r2,#0
	beq mrx_found_msg2
	cmp r2,r1
	beq rx_notpid
	b mrx17
rx_both
	lsrs r2,r2,#1
	bcs rx_notboth
mrx18
	movs r5,r4
	LOCK
	bl mrx19
mrx19
	ldr r4,[r5,#dbl_next]
	cmp r6,r4
	beq mrx_empty
	UNLOCK
	ldr r0,[sp,#4]
	ldr r1,[r4,#32]
	ldr r2,[r4,#20]
mrx20
	ldmia r0!,{r3,r5}	; r3 = msgid, r5 = msg_sender
	cmp r3,#0	; EOL ?
	beq mrx18	; yes => next msg
	cmp r5,#0	; EOL ? (2nd mark)
	beq mrx18	; yes => next msg
	cmp r1,r5
	bne mrx20
	cmp r2,r3
	bne mrx20
	LOCK
	b mrx_found_msg
rx_notboth
	movs r5,r4
	LOCK
	bl mrx21
mrx21
	ldr r4,[r5,#dbl_next]
	cmp r6,r4
	beq mrx_empty
	UNLOCK
	ldr r0,[sp,#4]
	ldr r1,[r4,#32]
	ldr r2,[r4,#20]
mrx22
	ldmia r0!,{r3,r5}
	cmp r3,#0
	IF CPUTYPE > 3
	beq.n mrx_found_msg2	; XXX
	cmp r5,#0
	beq.n mrx_found_msg2	; XXX
	ELSE
	beq mrx_found_msg2	; XXX
	cmp r5,#0
	beq mrx_found_msg2	; XXX
	ENDIF
	cmp r1,r5
	bne mrx22
	cmp r2,r3
	bne mrx22
	b rx_notboth
mrx_err0_cont
	b mrx_err0
	; mqueue empty
	; r6 - &pcb.msgQueue
	; r5 - pcb.msgQueue.tail
mrx_empty
	ldr r0,[sp,#0 +4]	; r0 := tmo
	subs r6,r6,#PCB_MSGQUEUE	; r6 := PCB
	cmp r0,#(0)	; tmo = (0)
	beq mrx_ret0	; yes => leave
	adds r2,r0,#-(0xffffffff)	; tmo = (0xffffffff) (r2 = 0 !!)
	beq mrx50	; yes => skip next
	; insert into timer-list
	IF CPUTYPE <= 4
	ldr r1,[r7,#SC_TCK] 
	adds r1,r1,r0 
	lsrs r2,r0,#18 
	bne mrx992 
	lsrs r2,r0,#12 
	bne mrx991 
	lsrs r2,r0,#6 
	bne mrx990 
	movs r2,#0x3f 
	ands r2,r2,r1 
	movs r0,#0 
	b mrx996 
mrx990 
	movs r0,#0x3f 
	ands r0,r0,r1 
	lsrs r1,r1,#6 
	movs r2,#0x3f 
	ands r2,r2,r1 
	adds r2,r2,#64 
	b mrx996 
mrx991 
	lsls r0,r1,#20 
	lsrs r0,r0,#20 
	lsrs r1,r1,#12 
	movs r2,#0x3f 
	ands r2,r2,r1 
	adds r2,r2,#128 
	b mrx996 
mrx992 
	lsls r0,r1,#14 
	lsrs r0,r0,#14 
	lsrs r1,r1,#18 
	movs r2,#0x3f 
	ands r2,r2,r1 
	adds r2,r2,#192 
mrx996 
	lsls r2,r2,#3 
	str r0,[r6,#PCB_TMO] 
	adds r2,r2,#SC_SLEEPERLISTS-255 
	adds r2,r2,#255 
	movs r1,r6 
	adds r1,r1,#PCB_SLEEPERS 
	adds r2,r2,r7 
	str r2,[r1,#dbl_next] 
	ldr r0,[r2,#dbl_prev] 
	str r1,[r2,#dbl_prev] 
	str r0,[r1,#dbl_prev] 
	str r1,[r0,#dbl_next]
	ELSE
	ldr r1,[r7,#SC_TCK] 
	adds r1,r1,r0 
	lsrs r2,r0,#18 
	bne mrx992 
	lsrs r2,r0,#12 
	bne mrx991 
	lsrs r2,r0,#6 
	bne mrx990 
	and r2,r1,#0x3f 
	movs r0,#0 
	b mrx996 
mrx990 
	and r0,r1,#0x3f 
	ubfx r2,r1,#6,#6 
	adds r2,r2,#64 
	b mrx996 
mrx991 
	ubfx r0,r1,#0,#12 
	ubfx r2,r1,#12,#6 
	adds r2,r2,#128 
	b mrx996 
mrx992 
	ubfx r0,r1,#0,#18 
	ubfx r2,r1,#18,#6 
	adds r2,r2,#192 
mrx996 
	str r0,[r6,#PCB_TMO] 
	add r2,r7,r2,lsl #3 
	add r2,r2,#SC_SLEEPERLISTS 
	adds r1,r6,#PCB_SLEEPERS 
	str r2,[r1,#dbl_next] 
	ldr r0,[r2,#dbl_prev] 
	str r1,[r2,#dbl_prev] 
	str r0,[r1,#dbl_prev] 
	str r1,[r0,#dbl_next]
	ENDIF
	movs r1,#0	; mark timer as set
	subs r1,r1,#-(0xffffffff)
	str r1,[sp,#0 +4]
	movs r2,#ST_WAIT_TMO	; add state: TMO
mrx50
	ldrh r1,[r6,#PCB_STATE_H]
	movs r0,#ST_WAIT_MSGRX
	orrs r0,r0,r1	; set WAIT_MSGRX state
	orrs r0,r0,r2	; set WAIT_TMO state (or 0 !)
	movs r2,#ST_TMO	; tmo already expired ?
	tst r1,r2
	bne mrx_ret0a	; yes, return 0
	strh r0,[r6,#PCB_STATE_H]	; set new process state
	ldrh r0,[r6,#PCB_FLAGS_H] 
	movs r1,#PCB_FLG_READY 
	tst r0,r1 
	beq mrx9999 
	bics r0,r0,r1 
	strh r0,[r6,#PCB_FLAGS_H] 
	ldr r0,[r6,#PCB_PCBLIST+dbl_next] 
	ldr r1,[r6,#PCB_PCBLIST+dbl_prev] 
	str r0,[r1,#dbl_next] 
	str r1,[r0,#dbl_prev] 
	cmp r0,r1 
	bne mrx9998 
	ldr r1,[r6,#PCB_PRIO_MASK] 
	ldr r0,[r7,#SC_PRIOMASK] 
	bics r0,r0,r1 
	str r0,[r7,#SC_PRIOMASK] 
mrx9998 
	movs r0,#0 
	str r0,[r6,#PCB_PCBLIST+dbl_next] 
	str r0,[r6,#PCB_PCBLIST+dbl_prev] 
mrx9999	; remove from readylist
	mov r4,lr	; save lr during swap
	UNLOCK
	SWAP_OUT_SAVE {r4-r5}	; swapout and save r4 and r5
	movs r3,#ST_WAIT_MSGRX
	movs r2,#ST_TMO
	LOCK
	ldrh r1,[r6,#PCB_STATE_H]	; get state
	cmp r0,#0	; swap successful ?
	beq mrx_err3	; no, error
	tst r1,r3	; test, if message received
	beq mrx_cont	; = 0, yes, search again
	bics r1,r1,r3	; clear ST_WAIT_MSGRX
	tst r1,r2	; tmo expired ?
	beq mrx_err0_cont	; no => illegal state
mrx_ret0a
	bics r1,r1,r2	; clear ST_TMO
	strh r1,[r6,#PCB_STATE_H]	; and set state
	movs r0,#0	; return NIL
mrx_ret0
	UNLOCK
	add sp,sp,#3*4
	RET r4-r7
mrx_cont
	mov lr,r4	; restore lr
	adds r6,r6,#PCB_MSGQUEUE	; r6 := &pcb.msgQueue
	bx r4	; and back to list-check
mrx_err3
	movs r0,#ST_WAIT_TMO	; swap out failed
	tst r1,r0	; clean up ?
	beq mrx51
	bics r1,r1,r0 
	adds r6,r6,#PCB_SLEEPERS 
	ldr r2,[r6,#dbl_next] 
	ldr r0,[r6,#dbl_prev] 
	str r2,[r0,#dbl_next] 
	str r0,[r2,#dbl_prev] 
	subs r6,r6,#PCB_SLEEPERS 
	movs r0,#0 
	str r0,[r6,#PCB_TMO]	; remove from tmo list
	strh r1,[r6,#PCB_STATE_H]
mrx51
	UNLOCK
	movs r1,#0	; fatal error
	ldr r0,mrx_KERNEL_ELOCKED
	bl sc_sysError
	ALIGN
mrx_err0_const
	DCD ((0x030<<12)|(0x0a<<24)|0x02)
	IF SC_MSG_PARA_CHECK = 1
mrx_err1_const
	DCD ((0x02b<<12)|(0x0a<<24))
mrx_err2_const
	DCD ((0x02d<<12)|(0x0a<<24)|0x02)
	ENDIF
mrx_sciopta
	DCD sciopta
	IF SC_MSG_HOOK = 1
	IF SC_MSGRX_HOOK = 1
mrx_SC_MSGRXHOOK
	DCD SC_MSGRXHOOK
	ENDIF
	ENDIF
mrx_KERNEL_ELOCKED
	DCD ((0x014<<12)|(0x0a<<24)|0x02)
	SC_ENDFUNC sc_msgRx
;
;**************************************
; sc_msgCheck
;
; Check message header
; r0 - pointer to header
; r1 - function code
;
; destroys: r1,r2,r3,r4
;*************************************
;
	SC_TFUNC sc_msgCheck
	ldr r4,mchk_sciopta
	ldr r3,[r0,#28]	; r3 := msg_owner
	ldr r2,[r4,#SC_CURPID]	; r2 := current PID
	cmp r2,r3	; msg_owner = current PID ?
	bne mchk_err0	; no => error
	; entry point for pool-scan
	SC_TPUBLIC sc_msgCheck2
	ldr r4,mchk_sciopta	; set r4 again
	push {r1}
	ldr r1,[r0,#12]	; r1 := msg-pool id
	movs r2,#0x7f
	lsrs r1,r1,#24	; r1 := module index
	ands r1,r1,r2	; mask module0-flag
	ldr r2,mchk_sc_modules
	cmp r1,#SC_MAX_MODULE	; module index in range ?
	bhs mchk_err1	; no => error
	lsls r1,r1,#2
	ldr r2,[r2,r1]	; r2 := mcb
	IF SC_NEVER_COPY = 1
	; empty
	ELSE
	ldr r1,[r4,#SC_CURMODULE]
	cmp r1,r2	; pool-mcb = current mcb ?
	IF SC_USE_FRIENDS = 1
	beq mchk2	; yes => no further check
	ldr r1,[r1,#MODULE_ID]	; get current module-id
	lsrs r1,r1,#24	; r1 := module-index
	movs r3,#31
	ands r3,r3,r1	; bit pos
	adds r3,r3,#1	; +1 for carry test
	lsrs r1,r1,#5
	lsls r1,r1,#2	; word pos
	adds r1,r1,r2
	ldr r1,[r1,#MODULE_FRIENDSET]
	lsrs r1,r1,r3
	bcc mchk_err2	; msg from a module which is no friend
	ELSE
	bne mchk_err2	; pool-mcb != current mcb => error
	ENDIF	; SC_USE_FRIENDS = 1
	ENDIF	; SC_NEVER_COPY = 1
mchk2
	IF LITTLE_ENDIAN = 1
	ldrb r1,[r0,#12]	; r1 := pool-index
	ELSE
	ldrb r1,[r0,#12 +3]
	ENDIF
	ldr r3,[r2,#MODULE_MAX_POOLS]
	cmp r1,r3	; plidx < MAX_POOLS ?
	bge mchk_err2	; no => error
	ldr r3,[r2,#MODULE_POOL]	; r3 := pool-cb array
	lsls r1,r1,#2
	ldr r3,[r3,r1]	; r3 := pool-cb
	cmp r3,#1	; valid ?
	bls mchk_err2	; no => error
	IF SC_ASYNC_TMO = 1
	ldr r1,[r0,#12]
	ldr r2,mchk_MSG_TMO_MAGIC
	lsls r1,r1,#8
	lsrs r1,r1,#16
	beq mchk3	; no magic => normal handling
	cmp r1,r2
	bne mchk_err3	; wrong magic => error
	ldr r1,[r0,#8]	; get (bufsize-idx|msgsize)
	lsrs r1,r1,#28	; get bufsize-index
	lsls r1,r1,#2	; long offset
	b mchk4
mchk3
	ENDIF
	ldr r1,[r0,#16]	; r1 := bufsize-index (*8 !)
	lsrs r1,r1,#1	; r1 := long-offset
mchk4
	ldr r2,[r3,#POOL_NBUFSIZES]
	lsls r2,r2,#2
	cmp r1,r2	; bufsize-idx valid ?
	bhs mchk_err3	; no => error
	adds r1,r1,#POOL_BUFSIZE
	ldr r1,[r3,r1]	; r1 := buffersize
	ldr r2,[r3,#POOL_START]
	cmp r0,r2	; msg < pool_start
	blo mchk_err4	; yes => error
	ldr r2,mchk_msgmagic
	subs r0,r0,#4
	ldr r4,[r0,#0]	; r4 := previous endmark
	adds r0,r0,#4	; r0 := msg
	cmp r2,r4	; endmark ok ?
	bne mchk_err5	; no => error (scan pool)
	adds r1,r1,#32
	adds r1,r1,r0	; r1 := addr of endmark
	ldr r4,[r3,#POOL_END]
	cmp r1,r4	; msg+size+head >= pool.end
	bhs mchk_err4	; yes => error
	ldr r4,[r1,#0]	; buffer endmark
	cmp r2,r4
	bne mchk_err6	; corrupt => error
	ldr r1,[r0,#8]	; r1 := allocated size
	IF SC_ASYNC_TMO = 1
	lsls r1,r1,#4
	lsrs r1,r1,#4	; mask bufsize-idx
	ENDIF
	ldr r2,[r3,#POOL_NBUFSIZES]
	lsls r2,r2,#2
	adds r2,r2,#POOL_BUFSIZE-4
	ldr r2,[r3,r2]	; largest fixed buffersize
	cmp r1,r2
	bhi mchk_err3	; msg size > => error
	adds r1,r1,r0
	adds r1,r1,#32	; r1 := ptr to endmark behind alloc'ed
	ldrb r2,[r1,#0]	; size
	ldrb r3,[r1,#1]
	ldrb r4,[r1,#2]
	ldrb r1,[r1,#3]
	cmp r2,#0xee
	bne mchk_err6	; corrupt => error
	cmp r3,#0xee
	bne mchk_err6	; corrupt => error
	cmp r4,#0xee
	bne mchk_err6	; corrupt => error
	cmp r1,#0xee
	bne mchk_err6	; corrupt => error
	pop {r1}
	bx lr
mchk_err6
	ldr r2,mchk_lit8
	b mchk_err_cont
	; previous buffer-endmark corrupted
	; check all messages from pool-start until corrupted buffer
mchk_err5
	pop {r1}	; r1 := function code
	movs r5,r0	; r5 := end of scan
	ldr r0,[r3,#POOL_START]
	movs r6,r0	; r6 := corrupted buffer
	movs r7,r3
	adds r7,r7,#POOL_BUFSIZE	; r7 := addr bufsize-array
	ldmia r0!,{r4}	; r4 := endmark, r0 += 4
	cmp r2,r4	; endmark = magic
	bne mchk_err5a	; no => error
mchk5
	bl sc_msgCheck2	; now check this buffer
	ldr r2,[r0,#16]; r2 := bufsize-idx (*8 !)
	movs r6,r0
	lsrs r2,r2,#1	; long offset
	ldr r2,[r7,r2]	; r2: buffersize
	adds r0,r0,r2
	adds r0,r0,#32 +4	; r0: end of current buffer
	cmp r0,r5	; below msg to check ?
	blo mchk5	; yes, continue
mchk_err5a
	movs r1,r6
	ldr r2,mchk_lit7	; prev endmark corrupt, r1 last msg
	b mchk_err_cont2
mchk_err4
	ldr r2,mchk_lit5
	b mchk_err_cont
mchk_err3
	ldr r2,mchk_lit4
	b mchk_err_cont
mchk_err2
	ldr r2,mchk_lit3
	b mchk_err_cont
mchk_err1
	ldr r2,mchk_lit2
	b mchk_err_cont
mchk_err0
	ldr r2,mchk_lit1
	push {r1}
mchk_err_cont
	movs r1,r0
mchk_err_cont2
	pop {r0}
	orrs r0,r0,r2
	bl sc_sysError
	ALIGN
mchk_sciopta
	DCD sciopta
mchk_sc_modules
	DCD sc_modules
mchk_lit1
	DCD (0x00C<<12)|0x02
mchk_lit2
	DCD (0x027<<12)|0x02
mchk_lit3
	DCD (0x001<<12)|0x02
mchk_lit4
	DCD (0x009<<12)|0x02
mchk_lit5
	DCD (0x008<<12)|0x02
mchk_msgmagic
	DCD 0xeeeeeeee
mchk_lit7
	DCD (0x012<<12)|0x02
mchk_lit8
	DCD (0x011<<12)|0x02
mchk_MSG_TMO_MAGIC
	DCD 0x00AFFE00>>8
	SC_ENDFUNC sc_msgCheck
;
;**************************************************************************
; 1 Kernel
;
; S02339BS3
;
; +Date: 2009/02/17 09:24:17 +
; +Revision: 1.9.2.1 +
;**************************************************************************
;
	XSC_TFUNC sc_msgAddrGet,-12
	SC_TFUNC sc_msgAddrGet
	IF CPUTYPE <= 4
	IF SC_MSG_PARA_CHECK = 1
	adds r1,r0,#0
	beq mag1
	ldr r2,[r1,#0]
	cmp r2,#0
	beq mag1
	ELSE
	ldr r2,[r0,#0]
	ENDIF
	subs r2,r2,#32
	ldr r0,[r2,#24]
	bx lr
	IF SC_MSG_PARA_CHECK = 1
mag1
	ldr r0,mag_lit0
	bl sc_sysError
	ALIGN
mag_lit0
	DCD (0x03<<24)|(0x00A<<12)|0x02
	ENDIF
	ELSE
	IF SC_MSG_PARA_CHECK = 1
	cbz r0,mag1
	ldr r2,[r0,#0]
	cbz r2,mag1
	ELSE
	ldr r2,[r0,#0]
	ENDIF
	ldr r0,[r2,#24 -32]
	bx lr
	IF SC_MSG_PARA_CHECK = 1
mag1
	ldr r0,mag_lit0
	bl sc_sysError
	ALIGN
mag_lit0
	DCD (0x03<<24)|(0x00A<<12)|0x02
	ENDIF
	ENDIF
	SC_ENDFUNC sc_msgAddrGet
;
;**************************************************************************
; 1 Kernel
;
; S02339BS2
;
; +Date: 2009/02/17 09:24:17 +
; +Revision: 1.9.2.1 +
;**************************************************************************
;
	XSC_TFUNC sc_msgSizeGet,-12
	SC_TFUNC sc_msgSizeGet
	IF CPUTYPE <= 4
	IF SC_MSG_PARA_CHECK = 1
	adds r1,r0,#0	; NULL ptr ?
	beq msig1	; yes => error
	ldr r2,[r1,#0]	; get msgptr
	cmp r2,#0	; NULL ?
	beq msig1	; yes => error
	ELSE
	ldr r2,[r0,#0]	; get msgptr
	ENDIF
	subs r2,r2,#32	; r2:= ptr to msg-hd
	ldr r0,[r2,#8]	; r0:= size
	bx lr	; return to caller
	IF SC_MSG_PARA_CHECK = 1
msig1
	ldr r0,msig_lit0	; r0:= error code, r1:= extra (0)
	bl sc_sysError
	ALIGN
msig_lit0
	DCD (0x05<<24)|(0x00A<<12)|0x02
	ENDIF
	ELSE
	IF SC_MSG_PARA_CHECK = 1
	cbz r0,msig1	; NULL ptr => error
	ldr r2,[r0,#0]	; get msgptr
	cbz r2,msig1	; NULL ? => error
	ELSE
	ldr r2,[r0,#0]	; get msgptr
	ENDIF
	ldr r0,[r2,#8 -32]	; r0:= size
	bx lr	; return to caller
	IF SC_MSG_PARA_CHECK = 1
msig1
	ldr r0,msig_lit0	; r0:= error code, r1:= extra (0)
	bl sc_sysError
	ALIGN
msig_lit0
	DCD (0x05<<24)|(0x00A<<12)|0x02
	ENDIF
	ENDIF
	SC_ENDFUNC sc_msgSizeGet
;
;**************************************************************************
; 1 Kernel
;
; S02339BS1
;
; +Date: 2009/02/24 10:19:21 +
; +Revision: 1.12.2.3 +
;**************************************************************************
;
;
; sc_msgSizeSet
;
; Set new size of a message, actually shrink it
;
	XSC_TFUNC sc_msgSizeSet,-12
	SC_TFUNC sc_msgSizeSet
	IF CPUTYPE <= 4
	IF SC_MSG_PARA_CHECK = 1
	cmp r0,#0	; test msgptr-ptr and msgptr for 0
	beq mss_err0
	ldr r2,[r0,#0]
	cmp r2,#0
	beq mss_err0
	ELSE
	ldr r2,[r0,#0]
	ENDIF
	movs r3,r2
	subs r3,r3,#32
	ldr r0,[r3,#8]	; r0:= current size
	IF SC_MSG_PARA_CHECK = 1
	cmp r1,#4	; new size < 4 or > old size
	blo mss_err1	; < 4 => error
	cmp r1,r0	; > old size => error
	bgt mss_err2
	ENDIF
	str r1,[r3,#8]	; set new size
	IF SC_MSG_CHECK = 1
	movs r0,#0xee	; place new endmarks
	adds r3,r2,r1
	strb r0,[r3,#0]
	strb r0,[r3,#1]
	strb r0,[r3,#2]
	strb r0,[r3,#3]
	ENDIF
	movs r0,r1	; return new size
	bx lr
	IF SC_MSG_PARA_CHECK = 1
mss_err1
	ldr r0,mss_lit0
	b mss_err_cont
mss_err2
	ldr r0,mss_lit2
	b mss_err_cont
mss_err0
	movs r1,r0
	ldr r0,mss_lit1
mss_err_cont
	bl sc_sysError
	ALIGN
mss_lit0
	DCD (0x06<<24)|(0x030<<12)|0x02
mss_lit1
	DCD (0x06<<24)|(0x00A<<12)|0x02
mss_lit2
	DCD (0x06<<24)|(0x00B<<12)|0x02
	ENDIF	; SC_MSG_PARA_CHECK = 1
	ELSE
	IF SC_MSG_PARA_CHECK = 1
	cbz r0,mss_err0	; test msgptr-ptr and msgptr for 0
	ldr r2,[r0,#0]
	cbz r2,mss_err0a
	ELSE
	ldr r2,[r0,#0]
	ENDIF
	ldr r0,[r2,#8 -32]	; r0:= current size
	IF SC_MSG_PARA_CHECK = 1
	cmp r1,#4	; new size < 4 or > old size
	blo mss_err1	; < 4 => error
	cmp r1,r0	; > old size => error
	bgt mss_err2
	ENDIF
	str r1,[r2,#8 -32]; set new size
	IF SC_MSG_CHECK = 1
	movw r0,#0xeeee	; place new endmarks
	movt r0,#0xeeee
	str r0,[r2,r1]
	ENDIF
	movs r0,r1	; return new size
	bx lr
	IF SC_MSG_PARA_CHECK = 1
mss_err1
	movs r1,r0
	ldr r0,mss_lit0
	b mss_err_cont
mss_err2
	ldr r0,mss_lit2
	b mss_err_cont
mss_err0a
	movs r0,r2
mss_err0
	movs r1,r0
	ldr r0,mss_lit1
mss_err_cont
	bl sc_sysError
	ALIGN
mss_lit0
	DCD (0x06<<24)|(0x030<<12)|0x02
mss_lit1
	DCD (0x06<<24)|(0x00A<<12)|0x02
mss_lit2
	DCD (0x06<<24)|(0x00B<<12)|0x02
	ENDIF	; SC_MSG_PARA_CHECK = 1
	ENDIF
	SC_ENDFUNC sc_msgSizeSet
;
;**************************************************************************
; 1 Kernel
;
; S02339BS6
;
; +Date: 2009/02/17 09:24:17 +
; +Revision: 1.8.2.1 +
;**************************************************************************
;
	XSC_TFUNC sc_msgSndGet,-12
	SC_TFUNC sc_msgSndGet
	IF CPUTYPE <= 4
	IF SC_MSG_PARA_CHECK = 1
	adds r1,r0,#0
	beq msg1
	ldr r2,[r1,#0]
	cmp r2,#0
	beq msg1
	ELSE
	ldr r2,[r0,#0]
	ENDIF
	subs r2,r2,#32
	ldr r0,[r2,#20]
	bx lr
	IF SC_MSG_PARA_CHECK = 1
msg1
	ldr r0,msg_lit0
	bl sc_sysError
	ALIGN
msg_lit0
	DCD (0x04<<24)|(0x00A<<12)|0x02
	ENDIF
	ELSE
	IF SC_MSG_PARA_CHECK = 1
	cbz r0,msg1
	ldr r2,[r0,#0]
	cbz r2,msg1
	ELSE
	ldr r2,[r0,#0]
	ENDIF
	ldr r0,[r2,#20 -32]
	bx lr
	IF SC_MSG_PARA_CHECK = 1
msg1
	mov r1,r0
	ldr r0,msg_lit0
	bl sc_sysError
	ALIGN
msg_lit0
	DCD (0x04<<24)|(0x00A<<12)|0x02
	ENDIF
	ENDIF
	SC_ENDFUNC sc_msgSndGet
;
;**************************************************************************
; 1 Kernel
;
; S02339BS4
;
; +Date: 2009/02/17 09:24:17 +
; +Revision: 1.8.2.1 +
;**************************************************************************
;
	XSC_TFUNC sc_msgOwnerGet,-12
	SC_TFUNC sc_msgOwnerGet
	IF CPUTYPE <= 4
	IF SC_MSG_PARA_CHECK = 1
	adds r1,r0,#0
	beq mog1
	ldr r2,[r1,#0]
	cmp r2,#0
	beq mog1
	ELSE
	ldr r2,[r0,#0]
	ENDIF
	subs r2,r2,#32
	ldr r0,[r2,#28]
	bx lr
	IF SC_MSG_PARA_CHECK = 1
mog1
	ldr r0,mog_lit0
	bl sc_sysError
	ALIGN
mog_lit0
	DCD (0x07<<24)|(0x00A<<12)|0x02
	ENDIF
	ELSE
	IF SC_MSG_PARA_CHECK = 1
	cbz r0,mog1
	ldr r2,[r0,#0]
	cbz r2,mog1
	ELSE
	ldr r2,[r0,#0]
	ENDIF
	ldr r0,[r2,#28 -32]
	bx lr
	IF SC_MSG_PARA_CHECK = 1
mog1
	mov r1,r0
	ldr r0,mog_lit0
	bl sc_sysError
	ALIGN
mog_lit0
	DCD (0x07<<24)|(0x00A<<12)|0x02
	ENDIF
	ENDIF
	SC_ENDFUNC sc_msgOwnerGet
;
;**************************************************************************
; 1 Kernel
;
; S02339BS5
;
; +Date: 2009/02/20 13:55:46 +
; +Revision: 1.14.2.1 +
;**************************************************************************
;
;
;************************************
; sc_pid_t sc_msgPoolIdGet(sc_msgptr_t )
;
; IN:
; r0 ptr to msg ptr
; Debug:
; r1 line
; r2 file
;
; OUT:
; r0 pool-id of msg or SC_DEFAULT_POOL
;
; If the message is from outside the current
; module SC_DEFAULT_POOL is return.
;**************************************
;
	XSC_TFUNC sc_msgPoolIdGet,-12
	SC_TFUNC sc_msgPoolIdGet
	IF SC_MSG_PARA_CHECK = 1
	adds r1,r0,#0	; save and test ptr
	beq mpig1	; = 0 => error
	ldr r2,[r1,#0]	; r2:= msgptr
	cmp r2,#0	; = 0 ?
	beq mpig1	; yes => error
	ELSE
	ldr r2,[r0,#0]	; r2:= msgptr
	ENDIF
	subs r2,r2,#32	; rewind to msg-header
	ldr r0,[r2,#12]	; r0:= pool-id of msg
	IF (SC_NEVER_COPY = 1) || (SC_USE_FRIENDS = 1)
	ldr r1,mpig_sciopta	; r1:= varbase
	ldr r1,[r1,#SC_CURMODULE]	; r1:= caller's mcb
	ldr r1,[r1,#MODULE_ID]	; r1:= caller's module-id
	lsls r2,r0,#1
	lsrs r2,r2,#25	; clear module0-bit and pool-idx
	lsrs r1,r1,#24
	cmp r2,r1
	beq mpig2
	movs r0,#SC_DEFAULT_POOL-1
	adds r0,r0,#1
	bx lr
mpig2
	ENDIF
	lsls r0,r0,#25	; mask index
	lsrs r0,r0,#25
	bx lr
	IF SC_MSG_PARA_CHECK = 1
mpig1
	ldr r0,mpig_lit0
	bl sc_sysError
	ALIGN
mpig_lit0
	DCD (0x0B<<24)|(0x00A<<12)|0x02
	ENDIF
	IF (SC_NEVER_COPY = 1) || (SC_USE_FRIENDS = 1)
	ALIGN
mpig_sciopta
	DCD sciopta
	ENDIF
	SC_ENDFUNC sc_msgPoolIdGet
;
;**************************************************************************
; Process functions
;
; S03106BS1
;
; +Date: 2009/08/19 09:40:02 +
; +Revision: 1.81.2.10 +
;**************************************************************************
;
;
;**************************************
;
;**************************************
;
;
;**************************************
; void sc_procWakeupEnable()
;**************************************
;
	XSC_TFUNC sc_procWakeupEnable,-16
	SC_TFUNC sc_procWakeupEnable
	ldr r3,pwe_sciopta	; r3:= sciopta
	ldr r2,[r3,#SC_CURPCB]	; r2:= current PCB
	ldrh r1,[r2,#PCB_FLAGS_H]	; r1:= flags
	IF SC_PROC_PARA_CHECK = 1
	movs r0,#PCB_FLG_IS_TIMER|PCB_FLG_IS_IRQ
	tst r1,r0	; current process timer or interrupt
	beq pwe_error	; no, error (no wakeup possible)
	ENDIF
	movs r0,#PCB_FLG_WAKEUP_DIS
	bics r1,r1,r0	; clear wakeup-disable flag
	strh r1,[r2,#PCB_FLAGS_H]
	bx lr	; and return
	IF SC_PROC_PARA_CHECK = 1
pwe_error
	ldr r0,pwe_lit
	movs r1,#0
	bl sc_sysError
	ALIGN
pwe_lit
	DCD (0x1e<<24)|0x04|(0x015<<12)
	ENDIF
	ALIGN
pwe_sciopta
	DCD sciopta
	SC_ENDFUNC sc_procWakeupEnable
;
;**************************************
; void sc_procWakeupDisable()
;**************************************
;
	XSC_TFUNC sc_procWakeupDisable,-16
	SC_TFUNC sc_procWakeupDisable
	ldr r3,pwd_sciopta	; r3:= sciopta
	ldr r2,[r3,#SC_CURPCB]	; r2:= current PCB
	ldrh r1,[r2,#PCB_FLAGS_H]	; r1:= flags
	IF SC_PROC_PARA_CHECK = 1
	movs r0,#PCB_FLG_IS_TIMER|PCB_FLG_IS_IRQ
	tst r1,r0	; current process timer or irq ?
	beq pwd_error	; no, error
	ENDIF
	movs r0,#PCB_FLG_WAKEUP_DIS
	orrs r1,r1,r0	; set wakup-disable flag
	strh r1,[r2,#PCB_FLAGS_H]
	bx lr
	IF SC_PROC_PARA_CHECK = 1
pwd_error
	ldr r0,pwd_lit
	movs r1,#0
	bl sc_sysError
	ALIGN
pwd_lit
	DCD (0x1f<<24)|0x04|(0x015<<12)
	ENDIF
	ALIGN
pwd_sciopta
	DCD sciopta
	SC_ENDFUNC sc_procWakeupDisable
;
;**************************************
; int sc_procVarDel(sc_tag_t tag)
;**************************************
;
	XSC_TFUNC sc_procVarDel,-12
	SC_TFUNC sc_procVarDel
	ldr r3,pvd_sciopta
	mov r12,r0	; save parameter
	ldr r0,pvd_KERNEL_ENIL_PTR	; r0 := error code
	ldr r3,[r3,#SC_CURPCB]	; r3 := current pcb
	ldr r3,[r3,#PCB_LOCALS]	; r3 := procVar-array
	cmp r3,#0	; set ?
	beq pvd3	; no => error
	ldr r2,[r3,#4]	; r2 := number of variables
	movs r0,#0
pvd1
	ldr r1,[r3,#8]	; r1 : = tag
	adds r3,r3,#8	; r3 := next entry
	cmp r1,r12	; found ?
	beq pvd2	; yes =>
	subs r2,r2,#1	; cnt -= 1
	bne pvd1	; != 0 => continue
	bx lr	; return ERROR
pvd2
	str r0,[r3,#0]	; clear tag
	str r0,[r3,#4]	; and value
	movs r0,#1	; return OK
	bx lr
	ALIGN
pvd_sciopta
	DCD sciopta
pvd_KERNEL_ENIL_PTR
	DCD (0x3a<<24)|(0x00A<<12)|0x04
pvd3
	movs r1,#0
	bl sc_sysError
	SC_ENDFUNC sc_procVarDel
;
;**************************************
; int sc_procVarSet(sc_tag_t tag,sc_var_t value)
;**************************************
;
	XSC_TFUNC sc_procVarSet,-8
	SC_TFUNC sc_procVarSet
	push {r4-r5}	; save working regs
	ldr r3,pvs_sciopta
	ldr r3,[r3,#SC_CURPCB]
	ldr r3,[r3,#PCB_LOCALS]	; r3 := procVar-array
	cmp r3,#0	; set ?
	beq pvs5	; no => error
	adds r3,r3,#4	; skip msgid
	ldmia r3!,{r2}	; r2 := count
	mov r12,r0	; save tag
	movs r5,#0
pvs1
	ldr r0,[r3,#0]	; get tag
	cmp r0,r12	; wanted ?
	beq pvs3	; yes =>
	orrs r0,r0,r5	; test:slot empty & not already found ?
	bne pvs2	; non empty =>
	movs r5,r3	; save slot-address
pvs2
	adds r3,r3,#8	; next slot
	subs r2,r2,#1	;
	bne pvs1	; until all slots are tested
	movs r0,#0	; preset error
	movs r3,r5	; get saved slot-address
	beq pvs4	; r5 = 0 => return error
pvs3
	mov r0,r12	; get tag
	stmia r3!,{r0-r1}	; and store tag/value in slot
pvs3a
	movs r0,#1	; return 1 for ok
pvs4
	pop {r4-r5}
	bx lr
pvs5
	movs r1,#0
	ldr r0,pvs_lit0
	bl sc_sysError
	ALIGN
pvs_sciopta
	DCD sciopta
pvs_lit0
	DCD (0x39<<24)|(0x00A<<12)|0x04
	SC_ENDFUNC sc_procVarSet
;
;**************************************
; int sc_procVarGet(sc_tag_t tag,sc_var_t *value)
;**************************************
;
	XSC_TFUNC sc_procVarGet,-8
	SC_TFUNC sc_procVarGet
	cmp r1,#0	; value = NULL ?
	beq pvg5	; yes, error
	push {r4}	; save working register
	ldr r3,pvg_sciopta
	ldr r3,[r3,#SC_CURPCB]
	ldr r3,[r3,#PCB_LOCALS]	; r3 := procVar-array
	cmp r3,#0	; set ?
	beq pvg5	; no => error
	adds r3,r3,#4	; skip msgid
	ldmia r3!,{r2}	; r2 := count
	mov r12,r0	; save wanted tag
pvg1
	ldmia r3!,{r0,r4}	; r0:= tag, r4:= value
	cmp r0,r12	; found ?
	beq pvg4	; yes =>
	subs r2,r2,#1	; no, next slot
	bne pvg1
pvg2
	movs r0,#0	; return error
pvg3
	pop {r4}
	bx lr
pvg5
	movs r1,#0	; r1 := extra
	ldr r0,pvg_lit0	; r0 := error-code
	bl sc_sysError
	ALIGN
pvg_sciopta
	DCD sciopta
pvg_lit0
	DCD (0x38<<24)|(0x00A<<12)|0x04
pvg4
	str r4,[r1,#0]	; store value
	movs r0,#1	; and return ok
	b pvg3
	SC_ENDFUNC sc_procVarGet
;
;**************************************
; void sc_procVarInit(sc_msgptr_t varpool,unsigned int size)
;**************************************
;
	XSC_TFUNC sc_procVarInit,-8
	SC_TFUNC sc_procVarInit
	ldr r3,pvi_sciopta
	ldr r3,[r3,#SC_CURPCB]
	ldr r2,[r3,#PCB_LOCALS]	; r2 := procVar-array
	cmp r2,#0	; set ?
	bne pvi_err	; yes => error
	cmp r0,#0	; varpool = 0 ?
	beq pvi_err1	; yes => error
	ldr r2,[r0,#0]
	cmp r2,#0	; *varpool = 0 ?
	beq pvi_err1	; yes => error
	mov r12,r2	; save varpool
	movs r2,#0	; and take away
	str r2,[r0,#0]	; from caller
	mov r2,r12
	subs r2,r2,#32	; r2 := msg-head
	cmp r1,#2	; min. 2 var. needed
	blt pvi_err2	; no, => error
	ldr r0,[r2,#8]	; msg large enough
	lsrs r0,r0,#3	; a var needs 8 bytes
	cmp r1,r0
	bgt pvi_err2	; not enough space => error
	mov r2,r12	; r2 := msg
	str r2,[r3,#PCB_LOCALS]	; save procVar
	ldr r0,pvi_lit3
	subs r1,r1,#1
	stmia r2!,{r0-r1}	; set msgid and count-1
	movs r0,#0	; clear array
	movs r3,#0
pvi1
	subs r1,r1,#1
	stmia r2!,{r0,r3}
	bne pvi1
	bx lr
pvi_err1
	movs r1,#0
	ldr r0,pvi_lit2
	b pvi_err_cont
pvi_err
	movs r1,r2
	ldr r0,pvi_alreadyDefined
	b pvi_err_cont
pvi_err2
	ldr r0,pvi_lit1
pvi_err_cont
	bl sc_sysError
	ALIGN
pvi_sciopta
	DCD sciopta
pvi_lit1
	DCD ((0x29<<24)|(0x030<<12)|0x04)
pvi_lit2
	DCD ((0x29<<24)|(0x00A<<12)|0x04)
pvi_lit3
	DCD (0x80000000 + 0x1)
pvi_alreadyDefined
	DCD ((0x29<<24)|(0x031<<12)|0x04)
	SC_ENDFUNC sc_procVarInit
;
;**************************************
; sc_msg_t sc_procVarRm(void)
;**************************************
;
	XSC_TFUNC sc_procVarRm,-16
	SC_TFUNC sc_procVarRm
	ldr r3,pvr_sciopta
	ldr r3,[r3,#SC_CURPCB]
	ldr r0,[r3,#PCB_LOCALS]	; r0 := procVar-array
	movs r1,#0
	str r1,[r3,#PCB_LOCALS]	; delete
	cmp r0,#0	; set ?
	beq pvr_warn	; no, warning/error
	bx lr	; yes, return msg
pvr_warn
	ldr r0,pvr_lit0	; r0 := error code (r1 already set)
	bl sc_sysError
	ALIGN
pvr_sciopta
	DCD sciopta
pvr_lit0
	DCD (0x3b<<24)|(0x00A<<12)|0x04
	SC_ENDFUNC sc_procVarRm
;
;**************************************
; sc_pid_t sc_procIdGet(const char *path,sc_ticks_t tmo)
;**************************************
;
	XSC_TFUNC sc_procIdGet,-8
	SC_TFUNC sc_procIdGet
	push {r4-r7,lr}
	ldr r7,pig_sciopta
	movs r5,r1	; save tmo
	beq pig0
	lsrs r1,r5,#24	; < SC_MAX_TMO
	beq pig0	; yes => ok
	adds r1,r5,#-((0xffffffff)-1)	; = ((0xffffffff)-1)
	beq pig0
	adds r1,r5,#-(0xffffffff)
	bne pig_err_illtmo
pig0
	movs r4,r0	; save and test path
	ldr r0,[r7,#SC_CURPID]	; get current pid
	beq pig9	; NULL-path => current pid
	ldrb r2,[r4,#0]	; zero-lenght path ?
	cmp r2,#0
	beq pig9	; yes => current pid
	IF SC_PROC_PARA_CHECK = 1
	movs r2,#(511 +1)>>2
	lsls r2,r2,#2	; r2 : = 511 +1
	ENDIF
	movs r6,#0
	movs r0,r4	; restore path
pig1
	ldrb r1,[r0,#0]
	adds r0,r0,#1
	IF SC_PROC_PARA_CHECK = 1
	subs r2,r2,#1
	beq pig_err
	ENDIF
	cmp r1,#0
	bne pig1
	subs r6,r0,r4	; r6 := strlen(path)
	ldr r2,pig_SC_PROCD
	ldr r1,pig_lit1	; r1 := (0x80000000 +0x10d)
	ldr r0,[r2,#0]	; r0 := PID of procd
	movs r2,#0	; select-endmark
	movs r3,#0	; select-endmark
	cmp r0,#0	; procd set ?
	beq pig_ill	; no => error
	push {r0-r3}	; select-array on stack
	movs r0,r6
	adds r0,r0,#((4 +4)+1)-1	; r0 := msg size
	subs r1,r1,#1	; r1 := (0x80000000 +0x10c)
	movs r2,#SC_DEFAULT_POOL-1
	adds r2,r2,#1
	movs r3,#-((0xffffffff)-1)
	rsbs r3,r3,#0
	bl sc_msgAlloc	; allocate procIdGetMsg
	push {r0}	; save msg
	str r5,[r0,#4]	; insert tmo
	adds r0,r0,#(4 +4)-1	; -1 because of pipeline optim.
	subs r1,r4,#1	; r1 := path
	; copy path
pig2
	ldrb r2,[r1,#1]
	adds r1,r1,#1
	adds r0,r0,#1
	strb r2,[r0,#0]
	cmp r2,#0
	bne pig2
	mov r0,sp	; r0 := address of msg
	ldr r1,[sp,#4]	; r1 := addressee (procd)
	movs r2,#0	; r2 := flags
	bl sc_msgTx
	ldr r3,[r7,#SC_CURPCB]	; get flags of caller
	ldrh r3,[r3,#PCB_FLAGS_H]
	lsrs r3,r3,#PCB_FLG_IS_PRIO_POS+1
	bcc pig_ill0	; carry = 0 => not a prio process, leave
	add r1,sp,#4	; r1 := select array
	movs r2,#(0x01 +0x02)
	subs r0,r2,#(0x01 +0x02)+(-(0xffffffff))
	bl sc_msgRx	; rx from procd
	str r0,[sp,#0]	; save msg
	ldr r5,[r0,#4]
	ldr r6,[r0,#((4 +4)+4)]
	mov r0,sp
	bl sc_msgFree	; clear msg
	movs r0,r6	; copy and test for error
	bne pig_err_cont	; yes =>
	movs r0,r5	; r0 := pid
	add sp,sp,#5*4	; clean stack (msg + select array)
pig9
	RET r4-r7
	; return 0x7fffffff if procd not set or
	; caller is not a prio proc
pig_ill0
	add sp,sp,#5*4	; clean stack (msg + select array)
pig_ill
	ldr r0,pig_lit2
	RET r4-r7
pig_err_illtmo
	movs r1,r5
	ldr r0,pig_illtmo
	bl sc_sysError
	IF SC_PROC_PARA_CHECK = 1
pig_err
	ldr r0,pig_lit3
	ENDIF
pig_err_cont
	movs r1,#0x02
	orrs r0,r0,r1	; error always fatal (also procd errors)
	movs r1,r4
	bl sc_sysError
	ALIGN
pig_sciopta
	DCD sciopta
pig_SC_PROCD
	DCD sciopta+SC_PROCD
pig_lit1
	DCD (0x80000000 +0x10d)
pig_lit2
	DCD 0x7fffffff
	IF SC_PROC_PARA_CHECK = 1
pig_lit3
	DCD (0x24<<24)|(0x025<<12)|0x02
	ENDIF
pig_illtmo
	DCD (0x24<<24)|(0x00f<<12)|0x04
	SC_ENDFUNC sc_procIdGet
;
;**************************************
; sc_pid_t sc_sysProcIdGet(const char *process,sc_moduleid_t mid)
;
; Internal function used by the procd.
;**************************************
;
	SC_TFUNC sc_sysProcIdGet
	push {r4-r7,lr}	; save working regs
	ldr r5,spig_sc_modules	; r5 := sc_modules
	movs r4,r0	; save process-name
	IF SC_PROC_PARA_CHECK = 1
	beq spig_err0	; NULL => error
	lsrs r3,r1,#24	; r3 := module index
	cmp r3,#SC_MAX_MODULE	; in range ?
	bge spig_err1	; no => error
	mov r12,r1	; save MID
	movs r2,#SC_PROC_NAME_SIZE+1	; check process-name size
spig1
	ldrb r1,[r0,#0]
	subs r2,r2,#1
	beq spig_err0	; too long => error
	adds r0,r0,#1
	cmp r1,#0
	bne spig1
	mov r1,r12	; restore process-name
	ELSE
	lsrs r3,r1,#24	; r3 := module index
	ENDIF
	lsls r3,r3,#2
	ldr r5,[r5,r3]	; r5 := MCB
	cmp r5,#1	; valid ?
	bls spig_ill	; no => out
	movs r0,r4	; calculate hash on process name
	bl sc_miscCrcString
	mov r12,r0	; and save hash
	ldr r1,[r5,#MODULE_MAX_PROCESS]	; r1 := count
	ldr r5,[r5,#MODULE_PCBS]	; r5 := addr of PCB array
spig2
	ldmia r5!,{r0}	; r0 := PCB, r5 += 4
	cmp r0,#0	; slot used ?
	beq spig4	; no => contiue
	ldrh r2,[r0,#PCB_HASH_H]	; r2 := hash
	cmp r2,r12	; wanted ?
	bne spig4	; no, continue
	ldr r2,[r0,#PCB_NAME]	; r2 := process name
	movs r3,r4	; r3 := wanted process name
spig3
	ldrb r6,[r2,#0]
	adds r2,r2,#1
	ldrb r7,[r3,#0]
	adds r3,r3,#1
	cmp r6,r7	; equal bytes ?
	bne spig4	; no, next PCB
	cmp r6,#0	; yes, end of string ?
	bne spig3	; no, go on
	ldr r0,[r0,#PCB_PID]	; found, return PID
	RET r4-r7
spig4
	subs r1,r1,#1	; next pcb
	bne spig2
spig_ill
	ldr r0,spig_SC_ILLEGAL_PID
	RET r4-r7
	IF SC_PROC_PARA_CHECK = 1
spig_err0
	movs r1,#0
	ldr r0,spig_lit0
spig_err_cont
	bl sc_sysError
spig_err1
	ldr r0,spig_lit1
	b spig_err_cont
	ENDIF
	ALIGN
spig_sciopta
	DCD sciopta
spig_sc_modules
	DCD sc_modules
	IF SC_PROC_PARA_CHECK = 1
spig_lit0
	DCD ((0x24<<24)|(0x025<<12)|0x02)
spig_lit1
	DCD ((0x24<<24)|(0x020<<12)|0x02)
	ENDIF
spig_SC_ILLEGAL_PID
	DCD 0x7fffffff
	SC_ENDFUNC sc_sysProcIdGet
;
;**************************************
; void sc_procKill(sc_pid_t pid,flags_t flag)
;**************************************
;
	; XXX: Must disallow killing outside callers module for 61508
	XSC_TFUNC sc_procKill,-8
	SC_TFUNC sc_procKill
	push {r4,lr}
	ldr r2,pk_SC_KERNELD
	ldr r2,[r2,#0]	; r2 := pid of kerneld
	cmp r2,#0	; set ?
	beq pk_err	; no => error
	mov r12,r2	; save it
	adds r2,r0,#-0xffffffff	; caller wants to kill itself ?
	ldr r4,pk_sciopta
	bne pk1	; no =>
	ldr r0,[r4,#SC_CURPID]	; yes, get pid
	b pk2	; go on
pk1
	lsrs r2,r0,#24	; r2 := module index
	cmp r2,#SC_MAX_MODULE	; in range ?
	ldr r3,pk_sc_modules	; r3 := sc_modules
	ldr r4,[r4,#SC_CURMODULE]	; r4 := current active module
	bge pk_err1	; module index too large =>
	lsls r2,r2,#2
	ldr r3,[r3,r2]	; r3 := MCB of process to kill
	cmp r3,r4	; same module ?
	beq pk2	; yes => ok
	lsrs r2,r1,#14 +1	; check flag
	bcc pk_err1	; not set, killing not allowed
pk2
	push {r0-r1}	; save parameters
	mov r0,r12	; r0 := PID of kerneld
	ldr r1,pk_SC_PROCKILLMSG_REPLY
	movs r2,#0
	movs r3,#0
	push {r0-r3}	; stack := rx-select array
	movs r0,#((4 +4)+4)
	subs r1,r1,#1	; r1 := (0x80000000 +0x108)
	movs r2,#SC_DEFAULT_POOL-1
	adds r2,r2,#1
	movs r3,#-(0xffffffff)
	rsbs r3,r3,#0
	bl sc_msgAlloc	; allocate kill-msg
	push {r0}	; save msg
	adds r1,r0,#4	; skip msg-id
	ldr r2,[sp,#20]	; r2 := pid
	ldr r3,[sp,#24]	; r3 := flags
	stmia r1!,{r2-r3}	; store in msg
	mov r0,sp
	ldr r1,[sp,#4]
	movs r2,#0
	bl sc_msgTx	; send msg to kerneld
	add r1,sp,#4	; r1:= select array
	movs r2,#(0x01 +0x02)
	subs r0,r2,#(0x01 +0x02)+(-(0xffffffff))	; r0 = (0xffffffff)
	bl sc_msgRx	; wait for reply
	str r0,[sp,#0]
	mov r0,sp
	bl sc_msgFree	; and free it
	add sp,sp,#4*4+4+2*4	; clean stack
	RET r4
pk_err
	movs r1,#0
	ldr r0,pk_lit1
pk_err_cont
	bl sc_sysError
pk_err1
	movs r1,r0	; kill of process in another module
	ldr r0,pk_lit2
	b pk_err_cont
	ALIGN
pk_sc_modules
	DCD sc_modules
pk_sciopta
	DCD sciopta
pk_SC_KERNELD
	DCD sciopta+SC_KERNELD
pk_SC_PROCKILLMSG_REPLY
	DCD (0x80000000 +0x109)
pk_lit1
	DCD (0x33<<24)|(0x010<<12)|0x01
pk_lit2
	DCD (0x33<<24)|(0x020<<12)|0x02
	SC_ENDFUNC sc_procKill
;
;**************************************
; sc_pid_t sc_procTimCreate(const char *name,
; void (*entry)(int),
; sc_bufsize_t stacksize,
; sc_ticks_t period,
; sc_ticks_t initdelay,
; int state,
; sc_poolid_t plid)
;
; sc_pid_t sc_procPrioCreate(const char * name,
; void (*entry)(void),
; sc_bufsize_t stacksize,
; sc_ticks_t slice,
; sc_prio_t prio,
; int state,
; sc_poolid_t plid)
;
; sc_pid_t sc_procIntCreate(const char *name,
; void (*entry)(int),
; sc_bufsize_t stacksize,
; int vector,
; sc_prio_t prio,
; int state,
; sc_poolid_t plid)
;
; sc_pid_t sc_procUsrIntCreate(const char *name,
; void (*entry)(int),
; sc_bufsize_t stacksize,
; int vector,
; sc_prio_t prio,
; int state,
; sc_poolid_t plid)
;
; r0 name
; r1 entry
; r2 stacksize
; r3 period/slice/vector
; sp initdelay/prio/prio
; sp+4 state
; sp+8 plid
;**************************************
;
; r4 16
; lr 20
	XSC_TFUNC sc_procTimCreate,12
	XSC_TFUNC sc_procPrioCreate,12
	XSC_TFUNC sc_procIntCreate,12
	XSC_TFUNC sc_procUsrIntCreate,12
	SC_TFUNC sc_procTimCreate
	push {r0-r4,lr}	; save parameters
	adr r2,ptc_lit0	; r2 := data
	bl sc_procXCreate	; common code
	ALIGN
ptc_lit0
	DCD (0x80000000 +0x105)
	DCD sciopta+SC_KERNELD
	DCD (0x31<<24)
	SC_ENDFUNC sc_procTimCreate
	SC_TFUNC sc_procPrioCreate
	push {r0-r4,lr}
	adr r2,ppric_lit0
	bl sc_procXCreate
	ALIGN
ppric_lit0
	DCD (0x80000000 +0x101)
	DCD sciopta+SC_KERNELD
	DCD (0x2f<<24)
	SC_ENDFUNC sc_procPrioCreate
	SC_TFUNC sc_procIntCreate
	push {r0-r4,lr}
	adr r2,pic_lit0
	bl sc_procXCreate
	ALIGN
pic_lit0
	DCD (0x80000000 +0x103)
	DCD sciopta+SC_KERNELD
	DCD (0x30<<24)
	SC_ENDFUNC sc_procIntCreate
	SC_TFUNC sc_procUsrIntCreate
	push {r0-r4,lr}
	adr r2,puic_lit0
	bl sc_procXCreate
	ALIGN
puic_lit0
	DCD (0x80000000 +0x107)
	DCD sciopta+SC_KERNELD
	DCD (0x32<<24)
	SC_ENDFUNC sc_procUsrIntCreate
	SC_TFUNC sc_procXCreate
	IF SC_PROC_PARA_CHECK = 1
	movs r3,#SC_PROC_NAME_SIZE+1	; check name (size,syntax)
	movs r4,#0xdf
pxc1
	ldrb r1,[r0,#0]
	adds r0,r0,#1
	subs r3,r3,#1
	beq pxc_err0	; = 0 => name to long
	cmp r1,#0
	beq pxc2
	cmp r1,#'_'
	beq pxc1
	cmp r1,#'0'
	blo pxc_err0
	cmp r1,#'9'
	bls pxc1
	ands r1,r1,r4	; r1 := lower(r1)
	cmp r1,#'A'
	blo pxc_err0
	cmp r1,#'Z'
	bls pxc1
pxc_err0
	ldr r0,pxc_lit3
	ldr r1,[sp,#0]
	b pxc_err_cont
pxc2
	ENDIF
	ldmia r2!,{r1,r3,r4}	; r1:= msgid, r3 := &kerneld_pid,r4 := func code
	ldr r0,[r3,#0]	; r0 := PID of kerneld
	cmp r0,#0	; set ?
	beq pxc_err1	; no => error
	movs r2,#0	; endmark
	movs r3,#0
	push {r0-r3}	; rx-select array
	movs r0,#(((((((4 +SC_PROC_NAME_SIZE+1)+4)+4)+4)+4)+4)+4)
	subs r1,r1,#1	; r1:= SC_PROC???CREATEMSG
	ldr r2,[sp,#32 +16]
	; if default pool, then change to real pool
	subs r3,r2,#1
	ble pxc2a	; <0 => pool was 0
	cmp r3,#SC_DEFAULT_POOL-1
	bne pxc2a	; not default pool =>
	push {r0,r1}	; save parameters
	movs r0,#0
	subs r0,r0,#1
	bl sc_poolDefault	; get callers default-pool
	movs r2,r0
	pop {r0,r1}
	str r2,[sp,#32 +16]	; and modify parameter
pxc2a
	movs r3,#-(0xffffffff)
	rsbs r3,r3,#0
	bl sc_msgAlloc	; get create msg
	push {r0}	; and save
	adds r1,r0,#3	; skip msgid
	ldr r2,[sp,#20+0]
pxc3
	ldrb r3,[r2,#0]	; copy process name
	adds r2,r2,#1
	adds r1,r1,#1
	strb r3,[r1,#0]
	cmp r3,#0
	bne pxc3
	adds r0,r0,#(4 +SC_PROC_NAME_SIZE+1)	; copy parameters
	add r1,sp,#20+4
	ldmia r1!,{r2-r4}	; entry,stack,slice
	stmia r0!,{r2-r4}
	adds r1,r1,#2*4
	ldmia r1!,{r2-r4}	; prio,state,plid
	stmia r0!,{r2-r4}
	mov r0,sp
	ldr r1,[sp,#4]	; r1 := pid of kerneld
	movs r2,#0
	bl sc_msgTx
	add r1,sp,#4
	movs r2,#(0x01 +0x02)
	subs r0,r2,#(0x01 +0x02)+(-(0xffffffff))
	bl sc_msgRx
	str r0,[sp,#0]
	ldr r0,[r0,#4]
	push {r0}
	add r0,sp,#4
	bl sc_msgFree
	pop {r0}
	add sp,sp,#4*4+4+4*4
	RET r4
pxc_err1
	ldr r0,pxc_lit2
	movs r1,#0
pxc_err_cont
	orrs r0,r0,r4
	bl sc_sysError
	ALIGN
pxc_lit2
	DCD (0x010<<12)|0x01
pxc_lit3
	DCD (0x025<<12)|0x02
	SC_ENDFUNC sc_procXCreate
;
;**************************************
; void sc_procUnobserve(sc_pid_t pid,sc_pid_t observer)
;**************************************
;
	XSC_TFUNC sc_procUnobserve,-8
	SC_TFUNC sc_procUnobserve
	push {r4-r7,lr}
	ldr r7,puo_sciopta
	adds r2,r1,#-0xffffffff
	ldr r3,[r7,#SC_CURPID]
	bne puo0
	movs r1,r3
puo0
	IF SC_PROC_PARA_CHECK = 1
	adds r2,r0,#-0xffffffff
	beq puo_err0
	cmp r0,r3
	beq puo_err0
	ENDIF
	cmp r0,#0
	IF SC_MAX_CONNECTOR > 0
	bmi puo10
	ELSE
	bmi puo_err0
	ENDIF
	movs r6,r1
	LOCK
	lsrs r2,r0,#24
	cmp r2,#SC_MAX_MODULE
	ldr r3,puo_sc_modules
	bge puo_err1
	lsls r2,r2,#2
	ldr r3,[r3,r2]
	cmp r3,#1
	bls puo_err1
	ldr r4,[r3,#MODULE_MAX_PROCESS]
	lsls r2,r0,#18
	lsrs r2,r2,#18
	cmp r2,r4
	ldr r3,[r3,#MODULE_PCBS]
	bge puo_err1
	lsls r2,r2,#2
	ldr r4,[r3,r2]
	cmp r4,#0
	beq puo9
	ldr r3,[r4,#PCB_PID]
	cmp r3,r0
	bne puo9
	ldrh r7,[r4,#PCB_FLAGS_H]
	movs r0,#1
	lsls r0,r0,#PCB_FLG_PCB_LOCKED_POS
	orrs r0,r0,r3
	strh r0,[r4,#PCB_FLAGS_H]
	adds r4,r4,#PCB_CONNECTED
	movs r0,r4
	b puo2
puo1
	LOCK
puo2
	movs r5,r0
	ldr r0,[r5,#dbl_next]
	cmp r0,r4
	beq puo8
	UNLOCK
	ldr r2,[r0,#20]
	cmp r6,r2
	bne puo1
	ldr r2,[r0,#28]
	str r2,[r0,#24]
	bl sc_sysMsgFree
	movs r0,r5
	b puo1
puo8
	ldrh r2,[r4,#PCB_FLAGS_H]
	lsrs r7,r7,#PCB_FLG_PCB_LOCKED_POS
	beq puo9
	lsls r7,r7,#PCB_FLG_PCB_LOCKED_POS
	orrs r2,r2,r7
	strh r2,[r4,#PCB_FLAGS_H]
puo9
	UNLOCK
	RET r4-r7
puo10
	IF SC_MAX_CONNECTOR > 0
	LOCK
	lsls r2,r0,#1
	lsrs r2,r0,#25
	cmp r2,#SC_MAX_CONNECTOR
	bge puo_err1
	ldr r4,puo_sc_connectors
	lsls r2,r2,#2
	ldr r4,[r4,r2]
	cmp r4,#1
	bls puo_err1
	ldr r2,[r4,#PCB_PID]
	UNLOCK
	push {r0-r2}
	movs r0,#((4 +4)+4)
	ldr r1,puo_lit3
	movs r2,#SC_DEFAULT_POOL-1
	adds r2,r2,#1
	movs r3,#-((0xffffffff)-1)
	rsbs r3,r3,#0
	bl sc_msgAlloc
	pop {r1-r3}
	push {r0}
	str r2,[r0,#(4 +4)]
	ldr r2,[r1,#0]
	str r2,[r0,#4]
	movs r2,#0
	str r2,[r1,#0]
	mov r0,sp
	movs r1,r3
	movs r2,#0
	bl sc_msgTx
	add sp,sp,#4
	RET r4-r7
	ENDIF
puo_err1
	UNLOCK
puo_err0
	movs r1,r0
	ldr r0,puo_lit0
puo_err_cont
	bl sc_sysError
	ALIGN
puo_sciopta
	DCD sciopta
puo_sc_modules
	DCD sc_modules
	IF SC_MAX_CONNECTOR > 0
puo_sc_connectors
	DCD sc_connectors
puo_lit3
	DCD (0x80000000 +0x404)
	ENDIF
puo_lit0
	DCD ((0x020<<12)|(0x3c<<24)|0x02)
	SC_ENDFUNC sc_procUnobserve
;
;**************************************
; void sc_procObserve(union sc_msg **msgptr,sc_pid_t pid)
;**************************************
;
	XSC_TFUNC sc_procObserve,-8
	SC_TFUNC sc_procObserve
	push {r4,lr}
	ldr r3,po_sciopta
	IF SC_PROC_PARA_CHECK = 1
	adds r2,r1,#-0xffffffff
	ldr r2,[r3,#SC_CURPID]
	beq po_err0
	cmp r2,r1
	beq po_err0
	cmp r0,#0
	beq po_err1
	ldr r2,[r0,#0]
	cmp r2,#0
	beq po_err1
	ELSE
	ldr r2,[r0,#0]
	ENDIF
	cmp r1,#0
	bmi po1
	movs r2,#0x0001
	movs r3,#0
	movs r4,#(0x35<<24)>>24
	lsls r4,r4,#24
	bl sc_sysMsgTx
	RET r4
po1
	IF SC_MAX_CONNECTOR > 0
	LOCK
	lsls r2,r1,#1
	lsrs r2,r2,#25
	cmp r2,#SC_MAX_CONNECTOR
	bge po_err2
	ldr r4,po_sc_connectors
	lsls r2,r2,#2
	ldr r4,[r4,r2]
	cmp r4,#1
	bls po_err2
	ldr r2,[r4,#PCB_PID]
	UNLOCK
	push {r0-r2}
	movs r0,#((4 +4)+4)
	ldr r1,po_lit3
	movs r2,#SC_DEFAULT_POOL-1
	adds r2,r2,#1
	movs r3,#-((0xffffffff)-1)
	rsbs r3,r3,#0
	bl sc_msgAlloc
	pop {r1-r3}
	push {r0}
	str r2,[r0,#4]
	ldr r2,[r1,#0]
	str r2,[r0,#(4 +4)]
	movs r2,#0
	str r2,[r1,#0]
	mov r0,sp
	movs r1,r3
	movs r2,#0
	bl sc_msgTx
	add sp,sp,#4
	RET r4
po_err2
	UNLOCK
	ENDIF
po_err0
	ldr r0,po_lit2
po_err_cont
	bl sc_sysError
	IF SC_PROC_PARA_CHECK = 1
po_err1
	movs r1,#0
	ldr r0,po_lit1
	b po_err_cont
	ENDIF
	ALIGN
po_sciopta
	DCD sciopta
	IF SC_PROC_PARA_CHECK = 1
po_lit1
	DCD ((0x00A<<12)|(0x35<<24)|0x02)
	ENDIF
po_lit2
	DCD ((0x020<<12)|(0x35<<24)|0x02)
	IF SC_MAX_CONNECTOR > 0
po_lit3
	DCD (0x80000000 +0x402)
po_sc_connectors
	DCD sc_connectors
	ENDIF
	SC_ENDFUNC sc_procObserve
;
;**************************************
; int sc_procPathCheck(const char *path)
;**************************************
;
	XSC_TFUNC sc_procPathCheck,-12
	SC_TFUNC sc_procPathCheck
	adds r1,r0,#0
	IF SC_PROC_PARA_CHECK = 1
	beq ppc_err0
	ENDIF
	;
	; check length and characters
	;
	movs r2,#(511 +1)>>2
	lsls r2,r2,#2
	mov r12,r1
	movs r1,#0xdf
ppc1
	subs r2,r2,#1
	beq ppc_ret0
	ldrb r3,[r0,#0]
	adds r0,r0,#1
	cmp r3,#0
	beq ppc2
	cmp r3,#'_'
	beq ppc1
	cmp r3,#'/'
	beq ppc1
	cmp r3,#'0'
	blt ppc_ret0
	cmp r3,#'9'
	bls ppc1
	ands r3,r3,r1
	cmp r3,#'A'
	blo ppc_ret0
	cmp r3,#'Z'
	bls ppc1
ppc_ret0
	movs r0,#0
	bx lr
ppc2
	mov r0,r12
	ldrb r1,[r0,#0]
ppc3
	movs r3,#SC_PROC_NAME_SIZE+1
	cmp r1,#'/'
	bne ppc6
	ldrb r2,[r0,#1]
	adds r0,r0,#1
	cmp r1,r2
	bne ppc5
ppc4
	subs r3,r3,#1
	bmi ppc_ret0
	ldrb r1,[r0,#1]
	adds r0,r0,#1
	cmp r1,#'/'
	bne ppc4
	cmp r3,#SC_PROC_NAME_SIZE
	beq ppc_ret0
	b ppc3
ppc5
	subs r3,r3,#1
	bmi ppc_ret0
	ldrb r1,[r0,#0]
	adds r0,r0,#1
	cmp r1,#0
	beq ppc_ret1
	cmp r1,#'/'
	bne ppc5
	cmp r3,#SC_PROC_NAME_SIZE
	beq ppc_ret0
	movs r3,#SC_PROC_NAME_SIZE+1
ppc6
	subs r3,r3,#1
	bmi ppc_ret0
	ldrb r1,[r0,#0]
	adds r0,r0,#1
	cmp r1,#'/'
	beq ppc_ret0
	cmp r1,#0
	bne ppc6
ppc_ret1
	cmp r3,#SC_PROC_NAME_SIZE
	beq ppc_ret0
	movs r0,#1
	bx lr
	IF SC_PROC_PARA_CHECK = 1
ppc_err0
	ldr r0,ppc_lit0
	bl sc_sysError
	ENDIF
	ALIGN
	IF SC_PROC_PARA_CHECK = 1
ppc_lit0
	DCD (0x3e<<24)|(0x00A<<12)|0x01
	ENDIF
	SC_ENDFUNC sc_procPathCheck
;
;**************************************
; void sc_procDaemonUnregister(void)
;**************************************
;
	XSC_TFUNC sc_procDaemonUnregister,-16
	SC_TFUNC sc_procDaemonUnregister
	ldr r3,pdu_sciopta
	ldr r2,pdu_SC_PROCD
	ldr r3,[r3,#SC_CURPID]
	ldr r1,[r2,#0]
	cmp r1,r3
	bne pdu1
	movs r0,#0
	str r0,[r2,#0]
pdu1
	bx lr
	ALIGN
pdu_sciopta
	DCD sciopta
pdu_SC_PROCD
	DCD sciopta+SC_PROCD
	SC_ENDFUNC sc_procDaemonUnregister
;
;**************************************
; int sc_procDaemonRegister(void)
;**************************************
;
	XSC_TFUNC sc_procDaemonRegister,-16
	SC_TFUNC sc_procDaemonRegister
	push {lr}
	movs r0,#0
	ldr r3,pdr_sciopta
	ldr r2,[r3,#SC_CURPID]
	lsrs r1,r2,#24
	ldr r1,[r3,#SC_CURPCB]
	bne pdr_ret0
	ldrh r1,[r1,#PCB_FLAGS_H]
	lsrs r1,r1,#PCB_FLG_IS_PRIO_POS+1
	bcc pdr_ret0
	ldr r3,pdr_SC_PROCD
	LOCK
	ldr r1,[r3,#0]
	cmp r1,#0
	bne pdr1
	str r2,[r3,#0]
	movs r0,#1
pdr1
	UNLOCK
pdr_ret0
	EXIT
	ALIGN
pdr_sciopta
	DCD sciopta
pdr_SC_PROCD
	DCD sciopta+SC_PROCD
	SC_ENDFUNC sc_procDaemonRegister
;
;**************************************
; sc_procHook_t sc_procHookRegister(int type,sc_procHook_t newhook)
;**************************************
;
	IF SC_PROC_HOOK = 1
	XSC_TFUNC sc_procHookRegister,-8
	SC_TFUNC sc_procHookRegister
	ldr r3,phr_sciopta
	ldr r2,[r3,#SC_CURMODULE]
	cmp r2,#0
	beq phr1
	IF SC_PROCKILL_HOOK = 1
	movs r3,#MODULE_PROCKILLHOOK
	cmp r0,#(2)
	beq phr2
	ENDIF
	IF SC_PROCCREATE_HOOK = 1
	movs r3,#MODULE_PROCCREATEHOOK
	cmp r0,#(1)
	beq phr2
	ENDIF
	; type parameter wrong, fatal error (module level)
	IF SC_PROC_PARA_CHECK = 1
	movs r1,r0
	ldr r0,phr_illval1
	b phr_err_cont	; => sc_sysError
	ENDIF
phr1
	IF SC_PROCKILL_HOOK = 1
	ldr r2,phr_SC_PROCKILLHOOK
	cmp r0,#(2)
	beq phr2
	ENDIF
	IF SC_PROCCREATE_HOOK = 1
	ldr r2,phr_SC_PROCCREATEHOOK
	cmp r0,#(1)
	beq phr2
	ENDIF
	IF SC_PROCSWAP_HOOK = 1
	ldr r2,phr_SC_PROCSWAPHOOK
	cmp r0,#(3)
	beq phr2
	ENDIF
	IF SC_PROCMMU_HOOK = 1
	ldr r2,phr_SC_PROCMMUHOOK
	cmp r0,#(4)
	beq phr2
	ENDIF
	; type parameter wrong, fatal error (system level)
	IF SC_PROC_PARA_CHECK = 1
	movs r1,r0
	ldr r0,phr_illval2
phr_err_cont
	bl sc_sysError
	ELSE
	movs r0,#0
	bx lr
	ENDIF
phr2
	ldr r0,[r2,r3]
	str r1,[r2,r3]
	bx lr
	ALIGN
phr_sciopta
	DCD sciopta
	IF SC_PROCKILL_HOOK = 1
phr_SC_PROCKILLHOOK
	DCD SC_PROCKILLHOOK
	ENDIF
	IF SC_PROCCREATE_HOOK = 1
phr_SC_PROCCREATEHOOK
	DCD SC_PROCCREATEHOOK
	ENDIF
	IF SC_PROCSWAP_HOOK = 1
phr_SC_PROCSWAPHOOK
	DCD SC_PROCSWAPHOOK
	ENDIF
	IF SC_PROCMMU_HOOK = 1
phr_SC_PROCMMUHOOK
	DCD SC_PROCMMUHOOK
	ENDIF
	IF SC_PROC_PARA_CHECK = 1
phr_illval1
	DCD (0x3f<<24)|(0x030<<12)|0x02
phr_illval2
	DCD (0x3f<<24)|(0x030<<12)|0x01
	ENDIF
	SC_ENDFUNC sc_procHookRegister
	ENDIF
;
;**************************************
; void sc_procStop(sc_pid_t pid)
;**************************************
;
	XSC_TFUNC sc_procStop,-12
	SC_TFUNC sc_procStop
	push {r7,lr}	; save working register and lr
	LOCK	; disable interrupts
	ldr r7,pst_sciopta	; r7:= sciopta
	adds r1,r0,#-0xffffffff	; r0 = 0xffffffff ?
	ldr r2,[r7,#SC_CURPCB]	; r2:= current PCB
	beq pst1	; yes =>
	cmp r0,#0	; pid = 0
	ble pst_err	; yes => error
	lsrs r1,r0,#24	; r1 := module index
	cmp r1,#SC_MAX_MODULE	; r1 >= SC_MAX_MODULES ?
	ldr r2,pst_sc_modules	; r2:= sc_modules[]
	bge pst_err	; yes => error
	lsls r1,r1,#2	; r1 := long index
	ldr r2,[r2,r1]	; r2:= module controll block
	cmp r2,#1	; valid ?
	bls pst_warn	; no, warning (ill. pid)
	ldr r3,[r2,#MODULE_MAX_PROCESS]
	lsls r1,r0,#18
	lsrs r1,r1,#18	; r1:= process index
	cmp r1,r3	; r1 >= max. process ?
	ldr r3,[r2,#MODULE_PCBS]	; r3:= module.pcbs[]
	bge pst_err	; yes => error
	lsls r1,r1,#2	; r1:= long index
	ldr r2,[r3,r1]	; r2:= pcb
	cmp r2,#0	; valid ?
	beq pst_warn	; no => error
	ldr r3,[r2,#PCB_PID]	; r3:= pid
	cmp r3,r0	; equal parameter ?
	bne pst_warn	; no, warning
pst1
	ldrh r1,[r2,#PCB_FLAGS_H]	; r1:= flags
	lsrs r0,r1,#PCB_FLG_IS_PRIO_POS+1
	bcs pst2	; C = 1 => prio-process
	lsrs r0,r1,#PCB_FLG_IS_TIMER_POS+1
	bcc pst_err1	; C = 0 => no timer => error
pst2
	ldrh r3,[r2,#PCB_STOPPED_H]	; r3:= stop-counter
	adds r3,r3,#1	; increase
	lsrs r0,r3,#16	; and test for overflow
	bne pst_err2	; overflow => error
	strh r3,[r2,#PCB_STOPPED_H]	; save stop-counter
	ldrh r3,[r2,#PCB_STATE_H]	; r3:= state
	movs r0,#ST_STOPPED>>2
	lsls r0,r0,#2
	orrs r3,r3,r0	; r3 |= ST_STOPPED
	strh r3,[r2,#PCB_STATE_H]
	lsrs r0,r1,#PCB_FLG_IS_PRIO_POS+1
	bcs pst3	; C = 1 => prio process
	movs r0,#PCB_FLG_READY	; clear ready-flag
	bics r1,r1,r0 
	adds r2,r2,#PCB_SLEEPERS 
	ldr r3,[r2,#dbl_next] 
	ldr r0,[r2,#dbl_prev] 
	str r3,[r0,#dbl_next] 
	str r0,[r3,#dbl_prev] 
	subs r2,r2,#PCB_SLEEPERS 
	movs r0,#0 
	str r0,[r2,#PCB_TMO]	; and remove from timer list
	strh r1,[r2,#PCB_FLAGS_H]
	UNLOCK
	RET r7
pst3
	ldrh r0,[r2,#PCB_FLAGS_H] 
	movs r1,#PCB_FLG_READY 
	tst r0,r1 
	beq pst39999 
	bics r0,r0,r1 
	strh r0,[r2,#PCB_FLAGS_H] 
	ldr r0,[r2,#PCB_PCBLIST+dbl_next] 
	ldr r1,[r2,#PCB_PCBLIST+dbl_prev] 
	str r0,[r1,#dbl_next] 
	str r1,[r0,#dbl_prev] 
	cmp r0,r1 
	bne pst39998 
	ldr r1,[r2,#PCB_PRIO_MASK] 
	ldr r0,[r7,#SC_PRIOMASK] 
	bics r0,r0,r1 
	str r0,[r7,#SC_PRIOMASK] 
pst39998 
	movs r0,#0 
	str r0,[r2,#PCB_PCBLIST+dbl_next] 
	str r0,[r2,#PCB_PCBLIST+dbl_prev] 
pst39999	; remove from ready-list
	UNLOCK
	ldr r0,[r7,#SC_CURPRIOPCB]	; check if stopped process was running
	cmp r0,r2
	bne pst_exit	; no, leave
	push {r4-r6}
	SWAP_OUT	; yes, dispatch
	RET r4-r7
pst_err
	ldr r2,pst_lit0
	b pst_err_cont
pst_err1
	ldr r2,pst_lit2
	b pst_err_cont
pst_err2
	ldr r2,pst_lit3
	b pst_err_cont
pst_warn
	ldr r2,pst_lit1
pst_err_cont
	movs r1,r0
	movs r0,r2
	UNLOCK
	bl sc_sysError
pst_exit
	RET r7
	ALIGN
pst_sciopta
	DCD sciopta
pst_sc_modules
	DCD sc_modules
pst_lit0
	DCD (0x020<<12)|(0x28<<24)|0x01
pst_lit1
	DCD (0x020<<12)|(0x28<<24)
pst_lit2
	DCD (0x024<<12)|(0x28<<24)|0x02
pst_lit3
	DCD (0x030<<12)|(0x28<<24)|0x02
	SC_ENDFUNC sc_procStop
;
;**************************************
; void sc_procStart(sc_pid_t pid)
;**************************************
;
	XSC_TFUNC sc_procStart,-12
	SC_TFUNC sc_procStart
	push {r4-r7,lr}	; save working register + lr
	ldr r7,ps_sciopta	; r7:= sciopta
	movs r1,r0	; save and test pid
	ble ps_err0	; <= 0 => error
	lsrs r0,r1,#24	; r0:= module index
	cmp r0,#SC_MAX_MODULE	; >= SC_MAX_MODULE
	ldr r6,ps_sc_modules	; r6:= sc_modules[]
	bge ps_err0	; yes => error
	lsls r0,r0,#2	; r0:= long index
	LOCK
	ldr r6,[r6,r0]	; r6:= module controll block
	cmp r6,#1	; valid ?
	bls ps_warn0	; no, warning
	ldr r5,[r6,#MODULE_PCBS]	; r5:= module.pcbs[]
	ldr r4,[r6,#MODULE_MAX_PROCESS]	; r4:= max. process
	lsls r0,r1,#18
	lsrs r0,r0,#18	; r0:= process index
	cmp r0,r4	; >= max. process
	bge ps_err1	; yes => error
	lsls r0,r0,#2	; r0:= long index
	ldr r5,[r5,r0]	; r5:= pcb
	cmp r5,#0	; valid ?
	beq ps_warn0	; no => warning
	ldr r0,[r5,#PCB_PID]	; check if pid equal parameter
	cmp r0,r1
	bne ps_warn0	; no, warning
	ldrh r2,[r5,#PCB_FLAGS_H]	; r2:= flags
	lsrs r0,r2,#PCB_FLG_IS_TIMER_POS+1
	bcs ps1	; C = 1 => timer
	lsrs r0,r2,#PCB_FLG_IS_PRIO_POS+1
	bcc ps_err2	; C = 0 => no prio, => error
ps1
	ldrh r0,[r5,#PCB_STOPPED_H]	; r0:= stop-counter
	ldrh r1,[r5,#PCB_STATE_H]	; r1:= state
	cmp r0,#0	; counter = 0
	beq ps_warn1	; yes, warning
	subs r0,r0,#1	; else, decrease
	strh r0,[r5,#PCB_STOPPED_H]	; and store
	bne ps_exit	; still != 0, leave
	movs r0,#ST_STOPPED>>2
	lsls r0,r0,#2
	bics r1,r1,r0	; clear stopped-flag
	strh r1,[r5,#PCB_STATE_H]
	lsrs r0,r2,#PCB_FLG_IS_PRIO_POS+1
	bcc ps5	; C = 0 => timer
	lsrs r0,r1,#ST_READY_POS	; process ready ?
	bne ps_exit	; no, leave
	ldrh r0,[r5,#PCB_FLAGS_H] 
	movs r1,#PCB_FLG_READY 
	tst r0,r1 
	bne ps1mr9999 
	orrs r0,r0,r1 
	strh r0,[r5,#PCB_FLAGS_H] 
	ldr r2,[r7,#SC_PRIOMASK] 
	ldr r1,[r5,#PCB_PRIO_MASK] 
	ldr r0,[r5,#PCB_READYLIST] 
	orrs r2,r2,r1 
	str r2,[r7,#SC_PRIOMASK] 
	adds r5,r5,#PCB_PCBLIST 
	str r0,[r5,#dbl_next] 
	ldr r1,[r0,#dbl_prev] 
	str r5,[r0,#dbl_prev] 
	str r1,[r5,#dbl_prev] 
	str r5,[r1,#dbl_next] 
	subs r5,r5,#PCB_PCBLIST 
ps1mr9999	; insert into ready-list
	UNLOCK
	ldr r0,[r7,#SC_CURPRIOPCB]
	ldrb r1,[r5,#PCB_PRIO_B]
	ldrb r0,[r0,#PCB_PRIO_B]
	cmp r1,r0
	bhs ps3
	SWAP_OUT
ps3
	RET r4-r7
ps_err1
	UNLOCK
ps_err0
	ldr r0,ps_lit0
	b ps_err_cont
ps_warn1
	ldr r0,ps_lit3
	b ps_err_cont1
ps_warn0
	ldr r0,ps_lit1
ps_err_cont1
	UNLOCK
ps_err_cont
	bl sc_sysError
	RET r4-r7
ps_err2
	ldr r0,ps_lit2
	b ps_err_cont1
	; timer
ps5
	ldr r1,[r5,#PCB_SLICE]
	movs r0,#PCB_FLG_READY
	orrs r2,r2,r0
	strh r2,[r5,#PCB_FLAGS_H]
	ldr r0,[r5,#PCB_SLICE]
	ldr r1,[r7,#SC_TCK] 
	adds r1,r1,r0 
	lsrs r2,r0,#18 
	bne ps5992 
	lsrs r2,r0,#12 
	bne ps5991 
	lsrs r2,r0,#6 
	bne ps5990 
	movs r2,#0x3f 
	ands r2,r2,r1 
	movs r0,#0 
	b ps5996 
ps5990 
	movs r0,#0x3f 
	ands r0,r0,r1 
	lsrs r1,r1,#6 
	movs r2,#0x3f 
	ands r2,r2,r1 
	adds r2,r2,#64 
	b ps5996 
ps5991 
	lsls r0,r1,#20 
	lsrs r0,r0,#20 
	lsrs r1,r1,#12 
	movs r2,#0x3f 
	ands r2,r2,r1 
	adds r2,r2,#128 
	b ps5996 
ps5992 
	lsls r0,r1,#14 
	lsrs r0,r0,#14 
	lsrs r1,r1,#18 
	movs r2,#0x3f 
	ands r2,r2,r1 
	adds r2,r2,#192 
ps5996 
	lsls r2,r2,#3 
	str r0,[r5,#PCB_TMO] 
	adds r2,r2,#SC_SLEEPERLISTS-255 
	adds r2,r2,#255 
	movs r1,r5 
	adds r1,r1,#PCB_SLEEPERS 
	adds r2,r2,r7 
	str r2,[r1,#dbl_next] 
	ldr r0,[r2,#dbl_prev] 
	str r1,[r2,#dbl_prev] 
	str r0,[r1,#dbl_prev] 
	str r1,[r0,#dbl_next]
	ldrh r1,[r5,#PCB_STATE_H]
	movs r0,#ST_WAIT_TMO
	orrs r1,r1,r0
	strh r1,[r5,#PCB_STATE_H]
ps_exit
	UNLOCK
	RET r4-r7
	ALIGN
ps_sciopta
	DCD sciopta
ps_sc_modules
	DCD sc_modules
ps_lit0
	DCD (0x020<<12)|(0x27<<24)|0x02
ps_lit1
	DCD (0x020<<12)|(0x27<<24)
ps_lit2
	DCD (0x024<<12)|(0x27<<24)|0x02
ps_lit3
	DCD (0x023<<12)|(0x27<<24)
	SC_ENDFUNC sc_procStart
;
;**************************************
; void sc_procSliceSet(sc_pid_t pid,sc_ticks_t newslice)
;**************************************
;
	XSC_TFUNC sc_procSliceSet,-8
	SC_TFUNC sc_procSliceSet
	ldr r3,pss_sciopta	; r3:= sciopta
	adds r2,r0,#-0xffffffff	; r0 = 0xffffffff ?
	ldr r2,[r3,#SC_CURPCB]	; r2:= current PCB
	beq pss1	; yes => skip next
	cmp r0,#0	; init0 ?
	bmi pss_err	; yes => error
	mov r12,r1	; save slice
	lsrs r1,r0,#24	; r1:= module-id
	cmp r1,#SC_MAX_MODULE
	ldr r2,pss_sc_modules
	bhs pss_err	; MID to big or external process =>
	lsls r1,r1,#2
	ldr r2,[r2,r1]	; r2:= MCB
	cmp r2,#1
	bls pss_warnpid	; = 1 => pid gone
	ldr r3,[r2,#MODULE_MAX_PROCESS]
	lsls r1,r0,#18
	lsrs r1,r1,#18	; r1:= process-index
	cmp r1,r3	; too big ?
	ldr r3,[r2,#MODULE_PCBS]	; r3:= pcbs[]
	bge pss_err	; yes, too big => error
	lsls r1,r1,#2
	ldr r2,[r3,r1]	; r2:= PCB
	cmp r2,#0	; valid ?
	beq pss_warnpid	; no, pid gone
	ldr r3,[r2,#PCB_PID]
	cmp r3,r0	; PID still the same
	bne pss_warnpid	; no, process gone
	mov r1,r12	; restore slice
pss1
	lsrs r0,r1,#24
	bne pss_err_illslice
	ldrh r0,[r2,#PCB_FLAGS_H]
	lsrs r3,r0,#PCB_FLG_IS_TIMER_POS+1
	bcs pss2
	lsrs r0,r0,#PCB_FLG_IS_PRIO_POS+1
	bcc pss_err2
	; prio proc
	ldr r0,[r2,#PCB_CURSLICE]
	cmp r0,#0
	bne pss3
	mov r3,lr
	LOCK
	str r1,[r2,#PCB_SLICE]
	str r1,[r2,#PCB_CURSLICE]
	UNLOCK
	bx r3
	; timer
pss2
	cmp r1,#(0)
	beq pss_err_illslice
pss3
	str r1,[r2,#PCB_SLICE]
	bx lr
pss_err
	movs r1,r0
	ldr r0,pss_lit0
	b pss_err_cont0
pss_err_illslice
	ldr r0,pss_errillslice
	b pss_err_cont0
pss_err2
	ldr r0,pss_lit4
	ldr r1,[r2,#PCB_PID]
pss_err_cont0
	bl sc_sysError
pss_warnpid
	movs r1,r0
	ldr r0,pss_lit1
pss_err_cont
	push {lr}
	bl sc_sysError
	EXIT
	ALIGN
pss_sciopta
	DCD sciopta
pss_sc_modules
	DCD sc_modules
pss_lit0
	DCD (0x020<<12)|(0x23<<24)|0x04
pss_lit1
	DCD (0x020<<12)|(0x23<<24)|0x40
pss_errillslice
	DCD (0x00f<<12)|(0x23<<24)|0x04
pss_lit4
	DCD (0x015<<12)|(0x23<<24)|0x02
	SC_ENDFUNC sc_procSliceSet
;
;**************************************
; sc_ticks_t sc_procSliceGet(sc_pid_t pid)
;**************************************
;
	XSC_TFUNC sc_procSliceGet,-12
	SC_TFUNC sc_procSliceGet
	ldr r3,pslg_sciopta
	adds r1,r0,#-0xffffffff	;
	ldr r2,[r3,#SC_CURPCB]	; r2 = current PCB
	beq pslg_exit	; pid = 0xffffffff =>
	cmp r0,#0	; external PID
	bmi pslg9	; < 0 >= ask connector
	lsrs r1,r0,#24	; r1 := module index
	cmp r1,#SC_MAX_MODULE	; in range ?
	ldr r2,pslg_sc_modules
	bge pslg_err	; no => error
	lsls r1,r1,#2
	ldr r2,[r2,r1]	; r2:= module CB
	cmp r2,#1	; valid ?
	bls pslg_warn	; no => warning
	ldr r3,[r2,#MODULE_MAX_PROCESS]
	lsls r1,r0,#18
	lsrs r1,r1,#18
	cmp r1,r3	; process index in range ?
	ldr r3,[r2,#MODULE_PCBS]
	bge pslg_err	; no error
	lsls r1,r1,#2
	ldr r2,[r3,r1]	; r2:= pcb
	cmp r2,#0
	beq pslg_warn	; no valid => warning
	ldr r3,[r2,#PCB_PID]
	cmp r3,r0	; pid still the same ?
	bne pslg_warn	; no => warning
pslg_exit
	ldr r0,[r2,#PCB_SLICE]
	bx lr
pslg9
	IF SC_MAX_CONNECTOR > 0
	movs r0,#-(0xffffffff)
	rsbs r0,r0,#0
	bx lr
	ENDIF
pslg_err
	movs r1,r0
	ldr r0,pslg_lit0
	bl pslg_err_cont
pslg_warn
	movs r1,r0
	ldr r0,pslg_lit1
pslg_err_cont
	push {lr}
	bl sc_sysError
	movs r0,#-(0xffffffff)
	rsbs r0,r0,#0
	EXIT
	ALIGN
pslg_sciopta
	DCD sciopta
pslg_sc_modules
	DCD sc_modules
pslg_lit0
	DCD (0x020<<12)|(0x22<<24)|0x01
pslg_lit1
	DCD (0x020<<12)|(0x22<<24)
	SC_ENDFUNC sc_procSliceGet
;
;**************************************
; sc_prio_t sc_procPpidGet(sc_pid_t pid)
;**************************************
;
	XSC_TFUNC sc_procPpidGet,-12
	SC_TFUNC sc_procPpidGet
	ldr r3,ppidg_sciopta
	adds r1,r0,#-0xffffffff
	ldr r2,[r3,#SC_CURPCB]
	beq ppidg_exit
	cmp r0,#0
	bmi ppidg9
	lsrs r1,r0,#24
	cmp r1,#SC_MAX_MODULE
	ldr r2,ppidg_sc_modules
	bge ppidg_err
	lsls r1,r1,#2
	ldr r2,[r2,r1]	; r2:= module CB
	cmp r2,#1
	bls ppidg_warn
	ldr r3,[r2,#MODULE_MAX_PROCESS]
	lsls r1,r0,#18
	lsrs r1,r1,#18
	cmp r1,r3
	ldr r3,[r2,#MODULE_PCBS]
	bge ppidg_err
	lsls r1,r1,#2
	ldr r2,[r3,r1]	; r2:= pcb
	cmp r2,#0
	beq ppidg_warn
	ldr r3,[r2,#PCB_PID]
	cmp r3,r0
	bne ppidg_warn
ppidg_exit
	ldrh r0,[r2,#PCB_FLAGS_H]
	lsrs r0,r0,#PCB_FLG_IS_STATIC_POS+1
	movs r0,#0
	bcs ppidg8
	subs r2,r2,#32
	ldr r0,[r2,#20]
ppidg8
	bx lr
ppidg9
	IF SC_MAX_CONNECTOR > 0
	ldr r0,ppidg_SC_ILLEGAL_PID
	bx lr
	ENDIF
ppidg_err
	movs r1,r0
	ldr r0,ppidg_lit0
	bl ppidg_err_cont
ppidg_warn
	movs r1,r0
	ldr r0,ppidg_lit1
ppidg_err_cont
	push {lr}
	bl sc_sysError
	ldr r0,ppidg_SC_ILLEGAL_PID
	EXIT
	ALIGN
ppidg_sciopta
	DCD sciopta
ppidg_sc_modules
	DCD sc_modules
ppidg_lit0
	DCD (0x020<<12)|(0x25<<24)|0x01
ppidg_lit1
	DCD (0x020<<12)|(0x25<<24)
ppidg_SC_ILLEGAL_PID
	DCD 0x7fffffff
	SC_ENDFUNC sc_procPpidGet
;
;**************************************
; sc_prio_t sc_procPrioGet(sc_pid_t pid)
;**************************************
;
	XSC_TFUNC sc_procPrioGet,-12
	SC_TFUNC sc_procPrioGet
	ldr r3,ppg_sciopta
	adds r1,r0,#-0xffffffff
	ldr r2,[r3,#SC_CURPCB]
	beq ppg_exit
	cmp r0,#0
	bmi ppg9
	lsrs r1,r0,#24
	cmp r1,#SC_MAX_MODULE
	ldr r2,ppg_sc_modules
	bge ppg_err
	lsls r1,r1,#2
	ldr r2,[r2,r1]
	cmp r2,#1
	bls ppg_warn
	ldr r3,[r2,#MODULE_MAX_PROCESS]
	lsls r1,r0,#18
	lsrs r1,r1,#18
	cmp r1,r3
	ldr r3,[r2,#MODULE_PCBS]
	bge ppg_err
	lsls r1,r1,#2
	ldr r2,[r3,r1]
	cmp r2,#0
	beq ppg_warn
	ldr r3,[r2,#PCB_PID]
	cmp r3,r0
	bne ppg_warn
ppg_exit
	ldrb r0,[r2,#PCB_PRIO0_B]
	bx lr
ppg9
	IF SC_MAX_CONNECTOR > 0
	movs r0,#32
	bx lr
	ENDIF
ppg_err
	movs r1,r0
	ldr r0,ppg_lit0
	bl ppg_err_cont
ppg_warn
	movs r1,r0
	ldr r0,ppg_lit1
ppg_err_cont
	push {lr}
	bl sc_sysError
	movs r0,#32
	EXIT
	ALIGN
ppg_sciopta
	DCD sciopta
ppg_sc_modules
	DCD sc_modules
ppg_lit0
	DCD (0x020<<12)|(0x20<<24)|0x01
ppg_lit1
	DCD (0x020<<12)|(0x20<<24)
	SC_ENDFUNC sc_procPrioGet
;
;**************************************
; union sc_msg * sc_procNameGet(sc_pid_t pid)
;**************************************
;
	XSC_TFUNC sc_procNameGet,-12
	SC_TFUNC sc_procNameGet
	push {r4-r7,lr}
	ldr r7,nag_sc_modules	; r7 := sc_modules
	ldr r3,nag_sciopta	; r3 := sciopta
	ldr r6,[r3,#SC_CURMODULE]	; r6 := current module
	ldr r5,[r3,#SC_CURPCB]	; r5 := current process
	adds r1,r0,#-0xffffffff	; r0 = 0xffffffff ?
	beq nag0	; yes, skip
	movs r4,r0	; save and test PID
	IF SC_MAX_CONNECTOR > 0
	bmi nag9	; < 0 ask connector
	ELSE
	bmi nag_err	; < 0 => error
	ENDIF
	lsrs r1,r0,#24	; r1 := module id
	cmp r1,#SC_MAX_MODULE	; in range ?
	bge nag_err	; no => error
	lsls r1,r1,#2
	ldr r6,[r7,r1]	; r6 := module CB
	cmp r6,#1	; valid ?
	bls nag_warn	; <= 1 => warning
	ldr r2,[r6,#MODULE_MAX_PROCESS]	; r2 := max process
	ldr r5,[r6,#MODULE_PCBS]	; r5 := addr of PCB array
	lsls r1,r0,#18
	lsrs r1,r1,#18	; r1 := process index
	cmp r1,r2	; in range ?
	bge nag_err	; no => error
	lsls r1,r1,#2
	ldr r5,[r5,r1]	; r5 := PCB
	cmp r5,#0	; valid ?
	beq nag_warn	; no => warning
	ldr r1,[r5,#PCB_PID]	; PID identical ?
	cmp r1,r0
	bne nag_warn	; no => warning
nag0
	movs r0,#((((4 +4)+SC_MODULE_NAME_SIZE+1)+SC_MODULE_NAME_SIZE+1)+SC_MODULE_NAME_SIZE+1)
	ldr r1,nag_SC_PROCNAMEGETMSG_REPLY
	movs r2,#SC_DEFAULT_POOL-1
	adds r2,r2,#1
	movs r3,#-((0xffffffff)-1)
	rsbs r3,r3,#0
	bl sc_msgAllocClr
	adds r1,r0,#(4 +4)-1
	ldr r7,[r7,#0]	; system-module
	adds r7,r7,#MODULE_NAME	; strcpy(msg->target,systemname)
nag1
	ldrb r2,[r7,#0]
	adds r1,r1,#1
	adds r7,r7,#1
	strb r2,[r1,#0]
	cmp r2,#0
	bne nag1
	movs r1,r0	; strcpy(msg->module,modulename)
	adds r1,r1,#((4 +4)+SC_MODULE_NAME_SIZE+1)-1
	adds r6,r6,#MODULE_NAME
nag2
	ldrb r2,[r6,#0]
	adds r1,r1,#1
	adds r6,r6,#1
	strb r2,[r1,#0]
	cmp r2,#0
	bne nag2
	movs r1,r0
	adds r1,r1,#(((4 +4)+SC_MODULE_NAME_SIZE+1)+SC_MODULE_NAME_SIZE+1)-1
	ldr r5,[r5,#PCB_NAME]
nag3
	ldrb r2,[r5,#0]
	adds r1,r1,#1
	adds r5,r5,#1
	strb r2,[r1,#0]
	cmp r2,#0
	bne nag3
nag_exit
	RET r4-r7
nag_warn
	movs r1,r0
	ldr r0,nag_lit1
nag_err_cont
	bl sc_sysError
	movs r0,#0
	b nag_exit
nag_err
	movs r1,r0
	ldr r0,nag_lit0
	bl sc_sysError
	; connector
	IF SC_MAX_CONNECTOR > 0
nag9
	ldr r6,nag_sc_connectors	; r6 := connectors
	lsls r1,r4,#1
	lsrs r1,r1,#25	; r1 := connector index
	cmp r1,#SC_MAX_CONNECTOR	; valid ?
	bge nag_err	; no => illegal pid
	lsls r1,r1,#2
	ldr r6,[r6,r1]	; r6:= connector pcb
	cmp r6,#1	; valid ?
	bls nag_err	; no => error
	ldr r0,[r6,#PCB_PID]	; r0 := connector PID
	ldr r1,nag_SC_PROCNAMEGETMSG_REPLY
	movs r2,#0
	movs r3,#0
	push {r0-r3}	; create sel[] on stack
	movs r0,#((0 +4)+4)
	subs r1,r1,#1	; r1 := (0x80000000 +0x10a)
	movs r2,#SC_DEFAULT_POOL-1
	adds r2,r2,#1
	movs r3,#-((0xffffffff)-1)
	rsbs r3,r3,#0
	bl sc_msgAlloc
	push {r0}	; save msg
	str r4,[r0,#(0 +4)]
	mov r0,sp	; r0 := &msg
	ldr r1,[sp,#4]	; r1 := connector pid
	movs r2,#0
	bl sc_msgTx
	add r1,sp,#4	; r1 := &sel[0]
	movs r2,#(0x01 +0x02)
	subs r0,r2,#(0x01 +0x02)+(-(0xffffffff))	; r0 := (0xffffffff)
	bl sc_msgRx
	add sp,sp,#5*4	; clean stack
	ldr r1,[r0,#4]	; error ?
	cmp r1,#0
	beq nag_exit	; no => return msg
	push {r0}	; yes, free msg
	mov sp,r0
	bl sc_msgFree
	pop {r0}	; and return NULL
	b nag_exit
	ENDIF
	ALIGN
nag_sciopta
	DCD sciopta
nag_sc_modules
	DCD sc_modules
nag_lit0
	DCD (0x020<<12)|(0x26<<24)|0x02
nag_lit1
	DCD (0x020<<12)|(0x26<<24)
nag_SC_PROCNAMEGETMSG_REPLY
	DCD (0x80000000 +0x10b)
	IF SC_MAX_CONNECTOR > 0
nag_sc_connectors
	DCD sc_connectors
	ENDIF
	SC_ENDFUNC sc_procNameGet
;
;**************************************
; union sc_msg * sc_procPathGet(sc_pid_t pid, flags_t flag )
;**************************************
;
	XSC_TFUNC sc_procPathGet,-8
	SC_TFUNC sc_procPathGet
	push {r4-r7,lr}
	ldr r3,pag_sciopta
	ldr r7,pag_sc_modules
	ldr r6,[r3,#SC_CURMODULE]
	movs r5,r1
	ldr r4,[r3,#SC_CURPCB]
	adds r1,r0,#-0xffffffff
	beq pag0
	movs r4,r0
	IF SC_MAX_CONNECTOR > 0
	bmi pag9
	ELSE
	bmi pag_err
	ENDIF
	lsrs r1,r0,#24
	cmp r1,#SC_MAX_MODULE
	bge pag_err
	lsls r1,r1,#2
	ldr r6,[r7,r1]
	cmp r6,#1
	bls pag_warn
	ldr r2,[r6,#MODULE_MAX_PROCESS]
	ldr r4,[r6,#MODULE_PCBS]
	lsls r1,r0,#18
	lsrs r1,r1,#18
	cmp r1,r2
	bge pag_err
	lsls r1,r1,#2
	ldr r4,[r4,r1]
	cmp r4,#0
	beq pag_warn
	ldr r1,[r4,#PCB_PID]
	cmp r1,r0
	bne pag_warn
pag0
	movs r0,#((((0 +4)+4)+4)+1)
	movs r1,r6
	adds r1,r1,#MODULE_NAME
pag1
	ldrb r2,[r1,#0]
	adds r0,r0,#1
	adds r1,r1,#1
	cmp r2,#0
	bne pag1
	cmp r5,#0
	beq pag3
	ldr r1,[r7,#0]
	adds r1,r1,#MODULE_NAME
pag2
	ldrb r2,[r1,#0]
	adds r0,r0,#1
	adds r1,r1,#1
	cmp r2,#0
	bne pag2
	adds r0,r0,#1
pag3
	ldr r1,[r4,#PCB_NAME]
pag4
	ldrb r2,[r1,#0]
	adds r0,r0,#1
	adds r1,r1,#1
	cmp r2,#0
	bne pag4
	ldr r1,pag_lit2
	movs r2,#SC_DEFAULT_POOL-1
	adds r2,r2,#1
	movs r3,#-((0xffffffff)-1)
	rsbs r3,r3,#0
	bl sc_msgAlloc
	adds r1,r0,#4
	ldr r2,[r4,#PCB_PID]
	movs r3,#0
	stmia r1!,{r2-r3}
	movs r2,#'/'
	strb r2,[r1,#0]
	adds r6,r6,#MODULE_NAME
	cmp r5,#0
	beq pag6
	strb r2,[r1,#1]
	adds r1,r1,#1
	ldr r3,[r7,#0]
	adds r3,r3,#MODULE_NAME
pag5
	ldrb r5,[r3,#0]
	adds r1,r1,#1
	adds r3,r3,#1
	strb r5,[r1,#0]
	cmp r5,#0
	bne pag5
	strb r2,[r1,#0]
pag6
	ldrb r5,[r6,#0]
	adds r1,r1,#1
	adds r6,r6,#1
	strb r5,[r1,#0]
	cmp r5,#0
	bne pag6
	strb r2,[r1,#0]
	ldr r4,[r4,#PCB_NAME]
pag7
	ldrb r5,[r4,#0]
	adds r1,r1,#1
	adds r4,r4,#1
	strb r5,[r1,#0]
	cmp r5,#0
	bne pag7
pag_exit
	RET r4-r7
pag_warn
	movs r1,r0
	ldr r0,pag_lit1
pag_err_cont
	bl sc_sysError
	movs r0,#0
	b pag_exit
pag_err
	movs r1,r0
	ldr r0,pag_lit0
	bl sc_sysError
	; connector
	IF SC_MAX_CONNECTOR > 0
pag9
	ldr r6,pag_sc_connectors
	lsls r1,r4,#1
	lsrs r1,r1,#25
	cmp r1,#SC_MAX_CONNECTOR
	bge pag_err
	lsls r1,r1,#2
	ldr r6,[r6,r1]
	cmp r6,#1
	bls pag_err
	ldr r0,[r6,#PCB_PID]
	ldr r1,pag_lit2
	movs r2,#0
	movs r3,#0
	push {r0-r3}
	movs r0,#(((0 +4)+4)+4)
	subs r1,r1,#1
	movs r2,#SC_DEFAULT_POOL-1
	adds r2,r2,#1
	movs r3,#-((0xffffffff)-1)
	rsbs r3,r3,#0
	bl sc_msgAlloc
	push {r0}
	adds r1,r0,#4
	stmia r1!,{r4-r5}
	mov r0,sp
	ldr r1,[sp,#4]
	movs r2,#0
	bl sc_msgTx
	add r1,sp,#4
	movs r2,#(0x01 +0x02)
	subs r0,r2,#(0x01 +0x02)+(-(0xffffffff))
	bl sc_msgRx
	add sp,sp,#5*4
	ldr r1,[r0,#((0 +4)+4)]
	cmp r1,#0
	beq pag_exit
	push {r0}
	mov sp,r0
	bl sc_msgFree
	pop {r0}
	b pag_exit
	ENDIF
	ALIGN
pag_sciopta
	DCD sciopta
pag_sc_modules
	DCD sc_modules
pag_lit0
	DCD (0x020<<12)|(0x3d<<24)|0x02
pag_lit1
	DCD (0x020<<12)|(0x3d<<24)
pag_lit2
	DCD (0x80000000 +0x10f)
	IF SC_MAX_CONNECTOR > 0
pag_sc_connectors
	DCD sc_connectors
	ENDIF
	SC_ENDFUNC sc_procPathGet
;
;**************************************
; sc_pcb_t * sc_sysProcPcbGet(sc_pid_t pid)
;**************************************
;
	XSC_TFUNC sc_sysProcPcbGet,-12
	SC_TFUNC sc_sysProcPcbGet
	ldr r3,sppg_sciopta
	movs r1,r0
	adds r0,r0,#-0xffffffff
	ldr r0,[r3,#SC_CURPCB]
	beq sppg_exit
	cmp r1,#0
	bmi sppg_ret0
	lsrs r0,r1,#24
	cmp r0,#SC_MAX_MODULE
	bge sppg_ret0
	ldr r3,sppg_sc_modules
	lsls r0,r0,#2
	ldr r3,[r3,r0]
	cmp r3,#1
	bls sppg_ret0
	ldr r2,[r3,#MODULE_PCBS]
	ldr r0,[r3,#MODULE_MAX_PROCESS]
	lsls r3,r1,#18
	lsrs r3,r3,#18
	cmp r3,r0
	bge sppg_ret0
	lsls r3,r3,#2
	ldr r0,[r2,r3]
	cmp r0,#0
	beq sppg_exit
	ldr r2,[r0,#PCB_PID]
	cmp r1,r2
	beq sppg_exit
sppg_ret0
	movs r0,#0
sppg_exit
	bx lr
	ALIGN
sppg_sciopta
	DCD sciopta
sppg_sc_modules
	DCD sc_modules
	SC_ENDFUNC sc_sysProcPcbGet
;
;**************************************
; const char * sc_sysProcCurrentName(void)
;
;**************************************
;
	XSC_TFUNC sc_sysProcCurrentName,-16
	SC_TFUNC sc_sysProcCurrentName
	ldr r3,spcn_sciopta
	ldr r0,[r3,#SC_CURPCB]
	cmp r0,#0
	beq spcn1
	ldr r0,[r0,#PCB_NAME]
spcn1
	bx lr
	ALIGN
spcn_sciopta
	DCD sciopta
	SC_ENDFUNC sc_sysProcCurrentName
;
;**************************************
; void sc_procYield(void)
;
; Give CPU to another process with the
; same priority.
;
	XSC_TFUNC sc_procYield,-16
	SC_TFUNC sc_procYield
	push {lr}
	ldr r3,py_sciopta
	ldr r2,[r3,#SC_CURPCB]
	ldrh r0,[r2,#PCB_FLAGS_H]
	lsrs r0,r0,#PCB_FLG_IS_PRIO_POS+1
	bcc py_err0
	adds r2,r2,#PCB_PCBLIST
	ldr r3,[r2,#dbl_prev]
	ldr r0,[r2,#dbl_next]
	cmp r3,r0
	beq py_exit
	LOCK
	ldr r0,[r2,#dbl_next] 
	ldr r1,[r2,#dbl_prev] 
	str r0,[r1,#dbl_next] 
	str r1,[r0,#dbl_prev]
	str r3,[r2,#dbl_next] 
	ldr r1,[r3,#dbl_prev] 
	str r2,[r3,#dbl_prev] 
	str r1,[r2,#dbl_prev] 
	str r2,[r1,#dbl_next]
	UNLOCK
	SWAP_OUT_SAVE {r4-r7}
	EXIT
py_err0
	ldr r0,py_lit2
	bl sc_sysError
py_exit
	EXIT
	ALIGN
py_sciopta
	DCD sciopta
py_lit2
	DCD (0x34<<24)|(0x02d<<12)
	SC_ENDFUNC sc_procYield
;
;**************************************
; void sc_procPrioSet(sc_prio_t prio)
;
; Change priority
;
;
	XSC_TFUNC sc_procPrioSet,-12
	SC_TFUNC sc_procPrioSet
	push {r4-r7,lr}
;
; Check if call is a prio process.
;
	ldr r3,pps_lit1
	ldr r2,[r3,#SC_CURPCB]
	ldrh r1,[r2,#PCB_FLAGS_H]
	lsrs r1,r1,#PCB_FLG_IS_PRIO_POS+1
	bcc pps_err0
;
; Check if 0 <= prio <= 31 or > 32
;
	cmp r0,#31
	bls pps10
	cmp r0,#32
	bhi pps_err1
;
; Check if caller is a init-process if prio = 32
;
	ldr r0,[r2,#PCB_PID]
	lsls r0,r0,#18
	lsrs r0,r0,#18
	bne pps_err0
;
; Remove caller from ready-list, change prio
; and set Idle-Flag.
;
	LOCK
	ldrh r0,[r2,#PCB_FLAGS_H] 
	movs r7,#PCB_FLG_READY 
	tst r0,r7 
	beq pps9999 
	bics r0,r0,r7 
	strh r0,[r2,#PCB_FLAGS_H] 
	ldr r0,[r2,#PCB_PCBLIST+dbl_next] 
	ldr r7,[r2,#PCB_PCBLIST+dbl_prev] 
	str r0,[r7,#dbl_next] 
	str r7,[r0,#dbl_prev] 
	cmp r0,r7 
	bne pps9998 
	ldr r7,[r2,#PCB_PRIO_MASK] 
	ldr r0,[r3,#SC_PRIOMASK] 
	bics r0,r0,r7 
	str r0,[r3,#SC_PRIOMASK] 
pps9998 
	movs r0,#0 
	str r0,[r2,#PCB_PCBLIST+dbl_next] 
	str r0,[r2,#PCB_PCBLIST+dbl_prev] 
pps9999
	movs r0,#32
	strb r0,[r2,#PCB_PRIO0_B]
	strb r0,[r2,#PCB_PRIO_B]
	ldrh r0,[r2,#PCB_FLAGS_H]
	movs r1,#PCB_FLG_IS_IDLE>>8
	lsls r1,r1,#8
	orrs r0,r0,r1
	movs r1,#PCB_FLG_IS_PRIO
	bics r0,r0,r1
	strh r0,[r2,#PCB_FLAGS_H]
	UNLOCK
;
; Call dispatcher
;
	SWAP_OUT
	RET r4-r7
pps_err0
	ldr r0,pps_lit2
	movs r1,#0
	bl sc_sysError
pps_err1
	movs r1,r0
	ldr r0,pps_lit3
	bl sc_sysError
	movs r0,#31
	ldr r3,pps_lit1
	b pps10
;
; Compute effective priority and leave if unchanged
	;
pps10
	strb r0,[r2,#PCB_PRIO0_B]
	ldr r1,[r3,#SC_CURMODULE]
	ldr r1,[r1,#MODULE_PRIO]
	adds r1,r1,r0
	cmp r1,#31
	ldrb r0,[r2,#PCB_PRIO_B]
	bls pps11
	movs r1,#31
pps11
	cmp r1,r0
	beq pps_exit
;
; Remove caller from current ready-list and insert
; into new one.
;
	LOCK
	ldrh r0,[r2,#PCB_FLAGS_H] 
	movs r7,#PCB_FLG_READY 
	tst r0,r7 
	beq pps29999 
	bics r0,r0,r7 
	strh r0,[r2,#PCB_FLAGS_H] 
	ldr r0,[r2,#PCB_PCBLIST+dbl_next] 
	ldr r7,[r2,#PCB_PCBLIST+dbl_prev] 
	str r0,[r7,#dbl_next] 
	str r7,[r0,#dbl_prev] 
	cmp r0,r7 
	bne pps29998 
	ldr r7,[r2,#PCB_PRIO_MASK] 
	ldr r0,[r3,#SC_PRIOMASK] 
	bics r0,r0,r7 
	str r0,[r3,#SC_PRIOMASK] 
pps29998 
	movs r0,#0 
	str r0,[r2,#PCB_PCBLIST+dbl_next] 
	str r0,[r2,#PCB_PCBLIST+dbl_prev] 
pps29999
	strb r1,[r2,#PCB_PRIO_B] 
	movs r0,#1 
	IF CPUTYPE >= 2 
	lsls r0,r0,#31 
	lsrs r0,r0,r1 
	ELSE 
	lsls r0,r0,r1 
	ENDIF 
	str r0,[r2,#PCB_PRIO_MASK] 
	lsls r1,r1,#3 
	movs r0,#SC_READYLIST 
	adds r0,r0,r1 
	adds r0,r0,r3 
	str r0,[r2,#PCB_READYLIST]
	ldrh r0,[r2,#PCB_FLAGS_H] 
	movs r1,#PCB_FLG_READY 
	tst r0,r1 
	bne ppsmr9999 
	orrs r0,r0,r1 
	strh r0,[r2,#PCB_FLAGS_H] 
	ldr r7,[r3,#SC_PRIOMASK] 
	ldr r1,[r2,#PCB_PRIO_MASK] 
	ldr r0,[r2,#PCB_READYLIST] 
	orrs r7,r7,r1 
	str r7,[r3,#SC_PRIOMASK] 
	adds r2,r2,#PCB_PCBLIST 
	str r0,[r2,#dbl_next] 
	ldr r1,[r0,#dbl_prev] 
	str r2,[r0,#dbl_prev] 
	str r1,[r2,#dbl_prev] 
	str r2,[r1,#dbl_next] 
	subs r2,r2,#PCB_PCBLIST 
ppsmr9999
	UNLOCK
;
; Call dispatcher
;
	SWAP_OUT
pps_exit
	RET r4-r7
	ALIGN
pps_lit1
	DCD sciopta
pps_lit2
	DCD ((0x21<<24)|(0x02d<<12)|0x02)
pps_lit3
	DCD ((0x21<<24)|(0x028<<12))
	SC_ENDFUNC sc_procPrioSet
;
;**************************************
; int sc_procSchedLock(void)
;
; Lock scheduler
;
;
	XSC_TFUNC sc_procSchedLock,-16
	SC_TFUNC sc_procSchedLock
	push {lr}
;
; Check if the caller is prioritzed process.
;
	ldr r3,psl_lit1
	ldr r0,[r3,#SC_CURPCB]
	ldrh r1,[r0,#PCB_FLAGS_H]
	lsrs r1,r1,#PCB_FLG_IS_PRIO_POS+1
	bcc psl_err0
;
; Increment lock-counter
;
	LOCK
	ldr r0,[r3,#SC_SCHEDULER_LOCK]
	adds r0,r0,#1
	str r0,[r3,#SC_SCHEDULER_LOCK]
	UNLOCK
;
; return current lock-counter
;
	EXIT
;
; Call sysError with callers PID as extra.
;
psl_err0
	ldr r1,[r0,#PCB_PID]
	ldr r0,psl_lit0
	bl sc_sysError
	ALIGN
psl_lit0
	DCD ((0x37<<24)|(0x02d<<12)|0x02)
psl_lit1
	DCD sciopta
	SC_ENDFUNC sc_procSchedLock
;
;**************************************
; void sc_procSchedUnlock()
;
; Unlock the scheduler. If a schedule-
; request is pending, swap out.
;
;
	XSC_TFUNC sc_procSchedUnlock,-16
	SC_TFUNC sc_procSchedUnlock
;
; Check if the caller is prioritzed process.
;
	ldr r3,psul_lit1
	ldr r0,[r3,#SC_CURPCB]
	ldrh r1,[r0,#PCB_FLAGS_H]
	lsrs r1,r1,#PCB_FLG_IS_PRIO_POS+1
	bcc psul_err0
	mov r2,lr
;
; Decrement lock-counter. Error if < 0.
;
	LOCK
	ldr r1,[r3,#SC_SCHEDULER_LOCK]
	subs r0,r1,#1
	bmi psul_err1
	str r0,[r3,#SC_SCHEDULER_LOCK]
	UNLOCK
;
; Check if scheduling should be performed
;
	ldr r0,[r3,#SC_SCHEDULE_REQ]
	cmp r0,#0
	beq psul1
;
; Call dispatcher
;
	mov lr,r2
	push {r4-r7,lr}
	SWAP_OUT
	RET r4-r7
;
; Leave w/o dispatch.
;
psul1
	bx r2
;
; Call error with PID as extra.
;
psul_err0
	ldr r1,[r0,#PCB_PID]
	ldr r0,psul_lit0
	bl sc_sysError
;
; Call error with -1 as extra.
;
psul_err1
	UNLOCK
	ldr r0,psul_lit2
	bl sc_sysError
	ALIGN
psul_lit0
	DCD ((0x2a<<24)|(0x02d<<12)|0x02)
psul_lit1
	DCD sciopta
psul_lit2
	DCD ((0x01f<<12)|0x02|(0x2a<<24))
	SC_ENDFUNC sc_procSchedUnlock
;
;**************************************
;sc_pid_t sc_sysProcCreate(const char *name,
; void (*entry)(void),
; sc_bufsize_t stacksize,
; sc_ticks_t slice,
; sc_prio_t prio,
; int state,
; sc_moduleid_t moduleid,
; sc_pcb_t *pcb,
; char *stack,
; unsigned int type)
;
; r0 name
; r1 entry
; r2 stacksize
; r3 slice
; (sp) prio
; 4(sp) state
; 8(sp) moduleid
; 12(sp) pcb
; 16(sp) stack
; 20(sp) type
;
	XSC_TFUNC sc_sysProcCreate,24
	SC_TFUNC sc_sysProcCreate
	push {r0-r7,lr}
	IF SC_PROC_PARA_CHECK = 1
;
;
; Parameter check for prioritized process
; Check if priority is within valid range
; 0 < priority < 31
;
;
	ldr r0,[sp,#56]
	lsrs r0,r0,#24
	cmp r0,#(0x2f<<24)>>24
	beq spc1
	cmp r0,#(0x2b<<24)>>24
	bne spc2
spc1
	ldr r1,[sp,#36]
	cmp r1,#32
	blo spc6
	movs r2,#(0x028<<12)>>12
;
;
; Add process type and fatal error type
; to error word and jump to error
;
;
spc_cont
	ldr r0,[sp,#56]
	lsls r2,r2,#12
	orrs r0,r0,r2
	adds r0,r0,#0x02
	bl sc_sysError
;
;
; Parameter check for interrupt process.
; Check if interrupt vector is within valid range
; 0 < interrupt vector < SC_MAX_INT_VECTOR
;
;
spc2
	cmp r0,#(0x30<<24)>>24
	beq spc3
	cmp r0,#(0x2c<<24)>>24
	bne spc4
spc3
	cmp r3,#SC_MAX_INT_VECTOR
	bls spc6
	movs r1,r3
	movs r2,#(0x00e<<12)>>12
	b spc_cont
;
;
; Parameter check for timer process.
; Check if timer slice is within valid range
; 0 < timer slice < SC_MAX_TMO
;
;
spc4
	cmp r0,#(0x31<<24)>>24
	beq spc5
	cmp r0,#(0x2d<<24)>>24
	bne spc6
spc5
	lsrs r1,r3,#24	; check for SC_MAX_TMO
	bne spc5a
	adds r1,r3,#0	; move and test if 0
	bne spc6
spc5a
	movs r2,#(0x00f<<12)>>12
	b spc_cont
;
;
; Parameter check if stack is big enough
;
;
spc6
	cmp r2,#16*4	; enough space for all regs ?
	bhi spc7
	movs r1,r2
	movs r2,#(0x029<<12)>>12
	b spc_cont
;
;
; Parameter check PCB and stack
; Only for static processes parameters
; PCB and stack may be zero
;
;
spc7
	ldr r1,[sp,#52]
	cmp r1,#0
	beq spc9
	ldr r1,[sp,#48]
	cmp r1,#0
	bne spc10
spc8
	movs r2,#(0x015<<12)>>12
	b spc_cont
spc9
	cmp r0,#(0x30<<24)>>24
	beq spc8
	cmp r0,#(0x2f<<24)>>24
	beq spc8
	cmp r0,#(0x31<<24)>>24
	beq spc8
;
;
; Get the module control block
;
;
spc10
	ENDIF
	ldr r7,=sc_modules
	ldr r1,[sp,#44]
	lsrs r6,r1,#24-2
	ldr r6,[r7,r6]
	ldr r7,=sciopta
	IF SC_PROC_PARA_CHECK = 1
;
;
; Parameter check if module exists
;
;
	cmp r6,#1
	bhi spc11
	movs r2,#(0x027<<12)>>12
	b spc_cont
;
;
; Check if maximum number of processes reached
;
;
spc11
	ENDIF
	ldr r1,[r6,#MODULE_NPROCESS]
	ldr r0,[r6,#MODULE_MAX_PROCESS]
	cmp r0,r1
	bne spc12
	movs r2,#(0x021<<12)>>12
	IF SC_PROC_PARA_CHECK = 1
	b spc_cont
	ELSE
;
;
; If no parameter check
; Add process type and fatal error type
; to error word and jump to error
;
;
spc_cont
	ldr r0,[sp,#56]
	lsls r2,r2,#12
	orrs r0,r0,r2
	adds r0,r0,#0x02
	bl sc_sysError
	ENDIF
;
;
; Calculate length of process name
;
;
spc12
	ldr r0,[sp,#0]
	movs r1,#SC_PROC_NAME_SIZE+2
spc13
	subs r1,r1,#1
	ldrb r4,[r0,#0]
	adds r0,r0,#1
	cmp r4,#0
	bne spc13
;
;
; Parameter check
; Length of process name not correct
;
;
spc14
	IF SC_PROC_PARA_CHECK = 1
	cmp r1,#SC_PROC_NAME_SIZE+1
	beq spc15
	cmp r1,#0
	bgt spc16
spc15
	movs r2,#(0x025<<12)>>12
spc15a
	ldr r1,[sp,#0]
	b spc_cont
spc16
	ENDIF
;
;
; Calculate hash value of process name
;
;
	ldr r0,[sp,#0]
	bl sc_miscCrcString
;
;
; Parameter check
; Check if process name is unique
;
;
	IF SC_PROC_PARA_CHECK = 1
	mov r12,r6
	ldr r4,[r6,#MODULE_NPROCESS]
	ldr r1,[r6,#MODULE_PCBS]
	cmp r4,#0
	beq spc20
spc17
	ldr r2,[r1,#0]
	adds r1,r1,#4
	cmp r2,#0
	beq spc17
	ldrh r3,[r2,#PCB_HASH_H]
	cmp r3,r0
	bne spc19
	ldr r2,[r2,#PCB_NAME]
	ldr r3,[sp,#0]
spc18
	ldrb r5,[r2,#0]
	adds r2,r2,#1
	ldrb r6,[r3,#0]
	adds r3,r3,#1
	cmp r5,r6
	bne spc19
	cmp r5,#0
	bne spc18
	ldr r1,[sp,#0]
	movs r2,#(0x025<<12)>>12
	b spc_cont
spc19
	subs r4,r4,#1
	bne spc17
spc20
	mov r6,r12
	ENDIF
;
;
; Allocates memory for PCB if
; PCB address parameter = 0
;
;
	ldr r5,[sp,#48]
	ldr r1,[r6,#MODULE_FREESIZE]
	ldr r2,[r6,#MODULE_DATAPTR]
	cmp r5,#0
	bne spc22
	cmp r1,#SIZEOF_PCB
	bhs spc21
spc_oom
	movs r2,#(0x00d<<12)>>12
	b spc_cont
spc21
	movs r5,r2
	subs r1,r1,#SIZEOF_PCB
	str r1,[r6,#MODULE_FREESIZE]
	adds r2,r2,#SIZEOF_PCB
	str r2,[r6,#MODULE_DATAPTR]
;
;
; Allocates memory for stack if
; stack address parameter = 0
;
;
spc22
	ldr r4,[sp,#52]
	cmp r4,#0
	ldr r3,[sp,#8]
	bne spc23
	cmp r3,r1
	bhi spc_oom
	movs r4,r2
	adds r2,r2,r3
	str r2,[r6,#MODULE_DATAPTR]
	subs r1,r1,r3
	str r1,[r6,#MODULE_FREESIZE]
spc23
;
;
; Clear PCB
;
;
	movs r2,#0
	movs r1,#SIZEOF_PCB-4
spc24
	str r2,[r5,r1]
	subs r1,r1,#4
	bpl spc24
;
;
; Store hash value of process name in PCB
;
;
	strh r0,[r5,#PCB_HASH_H]
	mov r12,r4	; save addr. of stack
;
;
; Increase number of processes
;
;
	ldr r3,[r6,#MODULE_NPROCESS]
	adds r3,r3,#1
	str r3,[r6,#MODULE_NPROCESS]
;
;
; Insert PCB in PCB pointer array
;
;
	ldr r4,[r6,#MODULE_PID_NXT]
	lsls r1,r4,#2
	ldr r2,[r6,#MODULE_PCBS]
	str r5,[r2,r1]	; insert PCB in array
;
;
; Creates process ID
;
;
	movs r0,r4
	ldr r1,[r6,#MODULE_ID]
	orrs r0,r0,r1
	ldr r1,[r6,#MODULE_PID_CNT]
	lsls r1,r1,#14	; shift in position
	orrs r0,r0,r1
	str r0,[r5,#PCB_PID]
;
;
; Calculate next process ID
;
;
	ldr r0,[r6,#MODULE_MAX_PROCESS]
	lsrs r1,r1,#14	; shift back
	cmp r0,r3
	beq spc24c
spc24a
	adds r4,r4,#1	; increase index
	cmp r4,r0	; max process ?
	bne spc24b	; no =>
	movs r4,#1	; wrap to index 1
	adds r1,r1,#1	; increase count
	lsls r1,r1,#8+14	; wrap
	lsrs r1,r1,#8+14
spc24b
	lsls r3,r4,#2
	ldr r3,[r2,r3]	; check slot
	cmp r3,#0
	bne spc24a	; not free => next
	str r4,[r6,#MODULE_PID_NXT]	; save next index
	str r1,[r6,#MODULE_PID_CNT]	; save count
;
;
; Initialize PCB
;
;
spc24c
	mov r4,r12	; restore stack address
	ldr r0,spc_SC_PCB_MAGIC_STATIC
	ldr r1,[sp,#56]
	movs r2,#PCB_FLG_IS_STATIC
	lsrs r1,r1,#24
	movs r3,#PCB_FLG_IS_IRQ|PCB_FLG_READY
	cmp r1,#(0x30<<24)>>24
	beq spc25
	cmp r1,#(0x2c<<24)>>24
	beq spc26
	movs r3,#PCB_FLG_IS_PRIO
	cmp r1,#(0x2f<<24)>>24
	beq spc25
	cmp r1,#(0x2b<<24)>>24
	beq spc26
	movs r3,#PCB_FLG_IS_TIMER
	cmp r1,#(0x31<<24)>>24
	beq spc25
	cmp r1,#(0x2d<<24)>>24
	beq spc26
	movs r0,#(0x36<<24)>>24
	lsls r0,r0,#24
	adds r0,r0,#0x02
	ldr r1,[sp,#56]
spc_err_cont
	bl sc_sysError
spc25
	movs r2,#0
	ldr r0,spc_SC_PCB_MAGIC
spc26
	orrs r2,r2,r3
	strh r2,[r5,#PCB_FLAGS_H]
	str r0,[r5,#PCB_MAGIC]
	ldr r0,[sp,#0]
	str r0,[r5,#PCB_NAME]
	movs r0,r5
	adds r0,r0,#PCB_MSGQUEUE
	str r0,[r0,#0]
	str r0,[r0,#4]
	adds r0,r0,#8	; PCB_ALLOCED
	str r0,[r0,#0]
	str r0,[r0,#4]
	adds r0,r0,#8	; PCB_CONNECTED
	str r0,[r0,#0]
	str r0,[r0,#4]
	ldr r0,[sp,#4]
	str r0,[r5,#PCB_ENTRY]
	ldr r1,[sp,#36]
	ldr r0,[r6,#MODULE_PRIO]
	str r1,[r5,#PCB_INITIAL]
	strb r1,[r5,#PCB_PRIO0_B]
	adds r1,r1,r0
	cmp r1,#32
	blo spc27
	movs r1,#31
spc27
	movs r0,#1
	str r0,[r5,#PCB_TRIGGER_VAL]
	strb r1,[r5,#PCB_PRIO_B] 
	movs r0,#1 
	IF CPUTYPE >= 2 
	lsls r0,r0,#31 
	lsrs r0,r0,r1 
	ELSE 
	lsls r0,r0,r1 
	ENDIF 
	str r0,[r5,#PCB_PRIO_MASK] 
	lsls r1,r1,#3 
	movs r0,#SC_READYLIST 
	adds r0,r0,r1 
	adds r0,r0,r7 
	str r0,[r5,#PCB_READYLIST]
	ldr r1,[sp,#12]
	ldr r0,[sp,#8]
	str r1,[r5,#PCB_SLICE]
	adds r4,r4,#3
	lsrs r4,r4,#2
	lsls r4,r4,#2
	str r4,[r5,#PCB_STACKBTM]
	adds r0,r0,r4
	lsrs r0,r0,#3	; align 8 byte for EABI
	lsls r0,r0,#3
	IF SC_STACK_CHECK = 1
	subs r1,r0,#7
	subs r1,r1,#1
	ELSE
	movs r1,r0
	ENDIF
	str r1,[r5,#PCB_STACKTOP]
	ldr r1,spc_stackvirgin
spc28
	subs r0,r0,#4
	cmp r0,r4
	str r1,[r0,#0]
	bne spc28
	ldr r0,[sp,#40]
	cmp r0,#0
	bne spc29
	movs r0,#1
	strh r0,[r5,#PCB_STOPPED_H]
	lsls r0,r0,#ST_STOPPED_POS
	strh r0,[r5,#PCB_STATE_H]
;
;
; Call process create hook
;
;
spc29
	IF SC_PROC_HOOK = 1
	IF SC_PROCCREATE_HOOK = 1
	ldr r2,spc_SC_PROCCREATEHOOK
	ldr r2,[r7,r2]
	cmp r2,#0
	beq spc30
	movs r0,r5
	movs r1,#0
	BLX_R2_t
spc30
	ldr r2,[r6,#MODULE_PROCCREATEHOOK]
	cmp r2,#0
	beq spc31
	movs r0,r5
	movs r1,#0
	BLX_R2_t
spc31
	ENDIF
	ENDIF
;
;
; Send message to process daemon if exists
;
;
	ldr r4,spc_SC_PROCD
	ldr r4,[r7,r4]
	cmp r4,#0
	beq spc33
	movs r0,#((((((8 +SC_PROC_NAME_SIZE+1)+SC_PROC_NAME_SIZE+1)+SC_PROC_NAME_SIZE+1)+2)+2)+2)
	movs r1,#1
	lsls r1,r1,#31
	movs r2,#3
	lsls r2,r2,#8
	orrs r1,r1,r2	; => (0x80000000 +0x300)
	movs r2,#SC_DEFAULT_POOL-1
	adds r2,r2,#1
	movs r3,#(0xffffffff)+1
	subs r3,r3,#1
	bl sc_msgAlloc
	push {r0}
	ldr r1,[r5,#PCB_PID]
	str r1,[r0,#4]
	movs r2,#(((((8 +SC_PROC_NAME_SIZE+1)+SC_PROC_NAME_SIZE+1)+SC_PROC_NAME_SIZE+1)+2)+2)
	ldrh r1,[r5,#PCB_HASH_H]
	strh r1,[r0,r2]
	subs r2,r2,#2
	movs r1,#MODULE_HASH_H
	ldrh r3,[r6,r1]
	strh r3,[r0,r2]
	subs r2,r2,#2
	ldr r3,=sc_modules
	ldr r3,[r3,#0]
	ldrh r1,[r3,r1]
	strh r1,[r0,r2]
	ldr r1,[r5,#PCB_NAME]	; process
	adds r6,r6,#MODULE_NAME	; module
	adds r3,r3,#MODULE_NAME	; target
	movs r2,#0
spc32
	ldr r4,[r1,r2]
	str r4,[r0,#((8 +SC_PROC_NAME_SIZE+1)+SC_PROC_NAME_SIZE+1)]
	ldr r4,[r6,r2]
	str r4,[r0,#(8 +SC_PROC_NAME_SIZE+1)]
	ldr r4,[r3,r2]
	str r4,[r0,#8]
	adds r0,r0,#4
	adds r2,r2,#4
	cmp r2,#32
	bne spc32
	subs r6,r6,#MODULE_NAME
	mov r0,sp
	ldr r1,spc_SC_PROCD
	ldr r1,[r7,r1]
	movs r2,#0
	bl sc_msgTx
	add sp,sp,#4
;
;
; Switch case of process type
;
;
spc33
	ldr r0,[sp,#56]
	lsrs r0,r0,#24
	cmp r0,#(0x2f<<24)>>24
	beq spc_prio
	cmp r0,#(0x2b<<24)>>24
	beq spc_prio
	cmp r0,#(0x30<<24)>>24
	beq spc_int
	cmp r0,#(0x2c<<24)>>24
	beq spc_int
	cmp r0,#(0x31<<24)>>24
	beq spc_tim
	cmp r0,#(0x2d<<24)>>24
	beq spc_tim
spc34
	b spc34
;
;
; Setup a prioritized process
;
;
spc_prio
	IF CPUTYPE <= 4
;
; Stacklayout (USR/SYS stack)
; r14_sys
; r12
; r3
; ...
; r0
; IF MMU
; dac
; ENDIF
; IF XSCALE
; acc0.h
; acc0.l
; ENDIF
; r11
; ...
; r4
; r14_irq
; spsr
; SP->
;
	ldr r0,[r5,#PCB_STACKTOP]
	subs r0,r0,#6*4
	adr r1,process_exit
	str r1,[r0,#5*4]	; set lr_usr
	movs r1,#0xcc
	str r1,[r0,#4*4]	; r12
	movs r1,#0x33
	str r1,[r0,#3*4]	; r3
	movs r1,#0x22
	str r1,[r0,#2*4]	; r2
	movs r1,#0x11
	str r1,[r0,#1*4]	; r1
	movs r1,#0
	str r1,[r0,#0*4]	; r0
	IF SC_PROC_HOOK = 1
	IF SC_PROCMMU_HOOK = 1
;
; Non-system modules (id != 0) are always client in the system and
; global domain to allow USR-mode read access and SYS-mode write
; access from the Kernel. It is only manager in its own domain.
; The system-module is client in all domains to allow system
; startup and kerneld to work properly.
;
	sub r0,#4
	ldr r2,[r5,#PCB_PID]
	ldr r3,=0x40000001	; non-system-module
	lsr r2,r2,#23	; r2:= MIDX*2 / test for 0
	bne spc36a2
	ldr r3,=0x55555555
spc36a2
	mov r1,#3
	lsl r1,r2	; shift master-flag in position
	orr r1,r3
	str r1,[r0,#0]
	ENDIF
	ENDIF
	IF CPUTYPE = 4
	subs r0,r0,#2*4
	movs r1,#0xff
	str r1,[r0,#1*4]	; set acc0.h
	movs r1,#0xee
	str r1,[r0,#0*4]	; set acc0.l
	ENDIF
	movs r1,#0xbb
spc36a1
	subs r0,r0,#4
	str r1,[r0,#0]
	subs r1,r1,#0x11
	cmp r1,#0x33
	bne spc36a1
	ldr r1,[r5,#PCB_ENTRY]
	subs r0,r0,#4
	str r1,[r0,#0*4]	; lr_irq
	lsls r1,r1,#31	; mask bit 0 => Thumb or 1 mode
	lsrs r1,r1,#31-5
	ldr r3,[sp,#56]	; check if system or user-mode
	movs r4,#2
	movs r2,#PSR_SYS_MODE
	tst r3,r4
	beq spc36a
	movs r2,#PSR_USR_MODE
spc36a
	orrs r1,r1,r2
	subs r0,r0,#4
	str r1,[r0,#0*4]	; set process' spsr
	adds r0,r0,#1
	str r0,[r5,#PCB_STACKPTR]	; set full swapin flag
	ELSE
	IF CPUTYPE = 5
;
; Stacklayout (USR/SYS stack)
; spsr
; r14_irq
; r14_sys
; r12
; r3
; ...
; r0
; IF MMU
; dac
; ENDIF
; r11
; ...
; r4
; SP->
;
	ldr r0,[r5,#PCB_STACKTOP]
	subs r0,r0,#8*4
	ldr r1,[r5,#PCB_ENTRY]
	str r1,[r0,#6*4]	; lr_irq
	lsls r1,r1,#31	; mask bit 0 => Thumb or 1 mode
	lsrs r1,r1,#31-5
	ldr r3,[sp,#56]	; check if system or user-mode
	movs r4,#2
	movs r2,#PSR_SYS_MODE
	tst r3,r4
	beq spc36b
	movs r2,#PSR_USR_MODE
spc36b
	orrs r1,r1,r2
	subs r0,r0,#4
	str r1,[r0,#7*4]	; set process' spsr
	adr r1,process_exit
	str r1,[r0,#5*4]	; set lr_usr
	movs r1,#0xcc
	str r1,[r0,#4*4]	; r12
	movs r1,#0x33
	str r1,[r0,#3*4]	; r3
	movs r1,#0x22
	str r1,[r0,#2*4]	; r2
	movs r1,#0x11
	str r1,[r0,#1*4]	; r1
	movs r1,#0
	str r1,[r0,#0*4]	; r0
	IF SC_PROC_HOOK = 1
	IF SC_PROCMMU_HOOK = 1
;
; Non-system modules (id != 0) are always client in the system and
; global domain to allow USR-mode read access and SYS-mode write
; access from the Kernel. It is only manager in its own domain.
; The system-module is client in all domains to allow system
; startup and kerneld to work properly.
;
	sub r0,#4
	ldr r2,[r5,#PCB_PID]
	ldr r3,=0x40000001	; non-system-module
	lsr r2,r2,#23	; r2:= MIDX*2 / test for 0
	bne spc36b2
	ldr r3,=0x55555555
spc36b2
	mov r1,#3
	lsl r1,r2	; shift master-flag in position
	orr r1,r3
	str r1,[r0,#0]
	ENDIF
	ENDIF
	movs r1,#0xbb
spc36b1
	subs r0,r0,#4
	str r1,[r0,#0]
	subs r1,r1,#0x11
	cmp r1,#0x33
	bne spc36b1
	adds r0,r0,#1
	str r0,[r5,#PCB_STACKPTR]	; set full swapin flag
	ELSE
		; v7-M
;
; Stacklayout
; xPSR
; returnAddress
; LR
; r12
; r3
; r2
; r1
; r0
; ---------
; LR (0xfffffffn) (*)
; r11
; ..
; r4
; SP->
;
; (*) n = 0b0001 => handler-mode, MSP
; = 0b1001 => thread-mode, MSP
; = 0b1101 => thread-mode, PSP
;
;
	ldr r0,[r5,#PCB_STACKTOP]
	mov r3,#1<<24	; xPSR=T-bit
	ldr r2,[r5,#PCB_ENTRY]
	adr r1,process_exit
	stmfd r0!,{r1-r3}
	movs r3,#0xcc
	movs r2,#0x33
	movs r1,#0x22
	stmfd r0!,{r1-r3}
	movs r2,#0x11
	movs r1,#0
	stmfd r0!,{r1-r2}
	movs r3,#0
	sub r3,r3,#7	; thread-mode/MSP
	movs r2,#0xbb
	movs r1,#0xaa
	stmfd r0!,{r1-r3}
	movs r3,#0x99
	movs r2,#0x88
	movs r1,#0x77
	stmfd r0!,{r1-r3}
	movs r3,#0x66
	movs r2,#0x55
	movs r1,#0x44
	stmfd r0!,{r1-r3}
	str r0,[r5,#PCB_STACKPTR]	; set full swapin flag
	ENDIF	; CPUTYPE = 5
	ENDIF	; CPUTYPE
	ldr r0,[sp,#40]
	cmp r0,#0
	beq spc_exit
	LOCK
	ldrh r0,[r5,#PCB_FLAGS_H] 
	movs r1,#PCB_FLG_READY 
	tst r0,r1 
	bne spcmr9999 
	orrs r0,r0,r1 
	strh r0,[r5,#PCB_FLAGS_H] 
	ldr r2,[r7,#SC_PRIOMASK] 
	ldr r1,[r5,#PCB_PRIO_MASK] 
	ldr r0,[r5,#PCB_READYLIST] 
	orrs r2,r2,r1 
	str r2,[r7,#SC_PRIOMASK] 
	adds r5,r5,#PCB_PCBLIST 
	str r0,[r5,#dbl_next] 
	ldr r1,[r0,#dbl_prev] 
	str r5,[r0,#dbl_prev] 
	str r1,[r5,#dbl_prev] 
	str r5,[r1,#dbl_next] 
	subs r5,r5,#PCB_PCBLIST 
spcmr9999
	UNLOCK
	ldr r0,[r7,#SC_CURPRIOPCB]
	cmp r0,#0
	beq spc37
	ldrb r1,[r0,#PCB_PRIO_B]
	ldrb r2,[r5,#PCB_PRIO_B]
	cmp r2,r1
	bhs spc_exit
;
;
; Exit
;
;
spc37
	push {r5}
	SWAP_OUT
	pop {r5}
spc_exit
	add sp,sp,#4*4
	ldr r0,[r5,#PCB_PID]
	RET r4-r7
	LTORG
;
;
; Setup an interrupt process
;
;
spc_uint
spc_int
	ldr r1,[r5,#PCB_STACKTOP]
	str r1,[r5,#PCB_STACKPTR]
	ldr r0,spc_irq_vectors
	ldr r1,[r5,#PCB_VECTOR]
	lsls r1,r1,#2
	IF SC_PROC_PARA_CHECK = 1
	ldr r2,[r0,r1]
	cmp r2,#0
	bne spc_intinuse_err
	str r5,[r0,r1]
	movs r1,#0
	subs r1,r1,#1
	strh r1,[r5,#PCB_NESTED_H]
	ELSE
	str r5,[r0,r1]
	ENDIF
	ldr r2,[r5,#PCB_ENTRY]
	movs r0,#0
	subs r0,r0,#1
	movs r1,r5
	swi 0
	b spc_exit
	IF SC_PROC_PARA_CHECK = 1
spc_intinuse_err
	movs r2,#(0x031<<12)>>12
	b spc_cont
	ENDIF
;
;
; Setup a timer process
;
;
spc_tim
	ldr r1,[r5,#PCB_STACKTOP]
	str r1,[r5,#PCB_STACKPTR]
	movs r0,#0
	subs r0,r0,#1
	movs r1,r5
	swi 0
	ldr r0,[sp,#40]
	cmp r0,#0
	beq spc_exit
	ldrh r0,[r5,#PCB_FLAGS_H]
	movs r1,#PCB_FLG_READY
	orrs r0,r0,r1
	strh r0,[r5,#PCB_FLAGS_H]
	ldr r0,[r5,#PCB_INITIAL]
	ldr r1,[r5,#PCB_SLICE]
	cmp r0,#0
	bne spc38
	movs r0,r1
spc38
	mov r6,sp
	LOCK
	ldr r1,[r7,#SC_TCK] 
	adds r1,r1,r0 
	lsrs r2,r0,#18 
	bne spc38992 
	lsrs r2,r0,#12 
	bne spc38991 
	lsrs r2,r0,#6 
	bne spc38990 
	movs r2,#0x3f 
	ands r2,r2,r1 
	movs r0,#0 
	b spc38996 
spc38990 
	movs r0,#0x3f 
	ands r0,r0,r1 
	lsrs r1,r1,#6 
	movs r2,#0x3f 
	ands r2,r2,r1 
	adds r2,r2,#64 
	b spc38996 
spc38991 
	lsls r0,r1,#20 
	lsrs r0,r0,#20 
	lsrs r1,r1,#12 
	movs r2,#0x3f 
	ands r2,r2,r1 
	adds r2,r2,#128 
	b spc38996 
spc38992 
	lsls r0,r1,#14 
	lsrs r0,r0,#14 
	lsrs r1,r1,#18 
	movs r2,#0x3f 
	ands r2,r2,r1 
	adds r2,r2,#192 
spc38996 
	lsls r2,r2,#3 
	str r0,[r5,#PCB_TMO] 
	adds r2,r2,#SC_SLEEPERLISTS-255 
	adds r2,r2,#255 
	movs r1,r5 
	adds r1,r1,#PCB_SLEEPERS 
	adds r2,r2,r7 
	str r2,[r1,#dbl_next] 
	ldr r0,[r2,#dbl_prev] 
	str r1,[r2,#dbl_prev] 
	str r0,[r1,#dbl_prev] 
	str r1,[r0,#dbl_next]
	ldrh r1,[r5,#PCB_STATE_H]
	movs r0,#ST_WAIT_TMO
	orrs r1,r1,r0
	strh r1,[r5,#PCB_STATE_H]
	UNLOCK
	b spc_exit
;
;
; Literal pool
;
;
	ALIGN
spc_stackvirgin
	DCD 0x17171717
spc_SC_PROCD
	DCD SC_PROCD
spc_SC_PCB_MAGIC_STATIC
	DCD SC_PCB_MAGIC_STATIC
spc_SC_PCB_MAGIC
	DCD SC_PCB_MAGIC
spc_irq_vectors
	DCD sc_irq_vectors
	IF SC_PROC_HOOK = 1
	IF SC_PROCCREATE_HOOK = 1
spc_SC_PROCCREATEHOOK
	DCD SC_PROCCREATEHOOK
	ENDIF
	ENDIF
process_exit
	ldr r0,spc_lit0
	b spc_err_cont
	ALIGN
spc_lit0
	DCD (0xfff<<12)|0x02
	SC_ENDFUNC sc_sysProcCreate
;
;**************************************
; void sc_sysProcKill(sc_pid_t pid,flags_t flag)
;**************************************
;
	SC_TFUNC sc_sysProcKill
	push {r1,r4-r7,lr}
	ldr r7,spk_sciopta
	adds r1,r0,#-0xffffffff
	ldr r6,[r7,#SC_CURMODULE]
	ldr r5,[r7,#SC_CURPCB]
	beq spk1
	cmp r0,#0	; killing init0 or external process ?
	ble spk_err	; yes, error =>
	lsrs r1,r0,#24	; r1:= MID
	cmp r1,#SC_MAX_MODULE	; valid
	ldr r6,spk_sc_modules
	bge spk_err	; MID too large => error
	lsls r1,r1,#2
	ldr r6,[r6,r1]	; r6:= MCB
	cmp r6,#1	; valid ?
	bls spk_err	; no, error
	ldr r3,[r6,#MODULE_MAX_PROCESS]
	lsls r2,r0,#18
	lsrs r2,r2,#18	; r2:= process-index
	cmp r2,r3	; valid ?
	ldr r5,[r6,#MODULE_PCBS]
	bge spk_err	; no => error
	lsls r2,r2,#2
	ldr r5,[r5,r2]	; r5:= PCB
	cmp r5,#0	; valid ?
	beq spk_err	; no. error
	ldr r3,[r5,#PCB_PID]	; still same PID ?
	cmp r3,r0
	bne spk_err	; no error
spk1
	;
	; Stop process to be killed
;
	ldrh r3,[r5,#PCB_FLAGS_H]
	LOCK
	lsrs r0,r3,#PCB_FLG_IS_PRIO_POS+1
	bcc spk2
	ldrh r0,[r5,#PCB_FLAGS_H] 
	movs r1,#PCB_FLG_READY 
	tst r0,r1 
	beq spk19999 
	bics r0,r0,r1 
	strh r0,[r5,#PCB_FLAGS_H] 
	ldr r0,[r5,#PCB_PCBLIST+dbl_next] 
	ldr r1,[r5,#PCB_PCBLIST+dbl_prev] 
	str r0,[r1,#dbl_next] 
	str r1,[r0,#dbl_prev] 
	cmp r0,r1 
	bne spk19998 
	ldr r1,[r5,#PCB_PRIO_MASK] 
	ldr r0,[r7,#SC_PRIOMASK] 
	bics r0,r0,r1 
	str r0,[r7,#SC_PRIOMASK] 
spk19998 
	movs r0,#0 
	str r0,[r5,#PCB_PCBLIST+dbl_next] 
	str r0,[r5,#PCB_PCBLIST+dbl_prev] 
spk19999
spk2
	;
	; Remove pending timeout
;
	ldrh r2,[r5,#PCB_STATE_H]
	movs r0,#ST_WAIT_TMO
	tst r2,r0
	beq spk3
	bics r2,r2,r0 
	adds r5,r5,#PCB_SLEEPERS 
	ldr r4,[r5,#dbl_next] 
	ldr r0,[r5,#dbl_prev] 
	str r4,[r0,#dbl_next] 
	str r0,[r4,#dbl_prev] 
	subs r5,r5,#PCB_SLEEPERS 
	movs r0,#0 
	str r0,[r5,#PCB_TMO]
spk3
	;
	; Remove from waiter list for sc_msgAlloc
;
	movs r0,#ST_WAIT_MSGALLOC
	tst r2,r0
	beq spk4
	bics r2,r2,r0
	movs r0,r5
	adds r0,r0,#PCB_PCBLIST
	ldr r1,[r0,#dbl_next] 
	ldr r4,[r0,#dbl_prev] 
	str r1,[r4,#dbl_next] 
	str r4,[r1,#dbl_prev]
spk4
	;
	; update state
;
	strh r2,[r5,#PCB_STATE_H]
	;
	; Check if process is a connector and
	; remove from connector array.
;
	movs r0,#1
	lsls r0,r0,#PCB_FLG_IS_CONNECTOR_POS
	tst r3,r0
	IF SC_MAX_CONNECTOR > 0
	beq spk7
	bics r3,r3,r0
	movs r0,#SC_MAX_CONNECTOR
	ldr r1,spk_sc_connectors
spk5
	ldmia r1!,{r2}
	cmp r2,r5
	beq spk6
	subs r0,r0,#1
	bpl spk5
	b spk_err1
spk6
	subs r1,r1,#4
	movs r0,#0
	str r0,[r1,#0]
	ELSE
	bne spk_err1
	ENDIF
spk7
	;
	; Update flags
;
	strh r3,[r5,#PCB_FLAGS_H]
	;
	; Call kill hook, if any
;
	IF SC_PROC_HOOK = 1
	IF SC_PROCKILL_HOOK = 1
	ldr r2,[r6,#MODULE_PROCKILLHOOK]
	movs r0,r5
	movs r1,#0
	cmp r2,#0
	beq spk8
	BLX_R2_t
spk8
	ldr r2,spk_SC_PROCKILLHOOK
	ldr r2,[r7,r2]
	movs r0,r5
	movs r1,#0
	cmp r2,#0
	beq spk9
	BLX_R2_t
spk9
	ENDIF
	ENDIF
	;
	; Now remove process (clear PID)
;
	ldr r4,[r5,#PCB_PID]
	movs r0,#0
	str r0,[r5,#PCB_PID]
	;
	; Wait while PCB is locked
;
spk10
	lsrs r0,r3,#PCB_FLG_PCB_LOCKED_POS+1
	bcc spk11	; not locked =>
	UNLOCK
	movs r0,#1
	bl sc_sleep
	LOCK
	ldrh r3,[r5,#PCB_FLAGS_H]
	b spk10
spk_err_cont
	movs r1,r0
	movs r0,r2
	bl sc_sysError
	RET r4-r7
spk_err
	ldr r2,spk_lit1
	b spk_err_cont
spk_err1
	ldr r2,spk_lit2
	b spk_err_cont
		; continue
spk11
	UNLOCK
	ldr r1,[sp,#0]
	lsrs r0,r1,#14 +1	; kill -9 ?
	bcs spk20	; yes, skip cleaning up
	;
	; Free allocated messages and timeout messages
;
	push {r4}	; save pid
	adds r5,r5,#PCB_ALLOCED
spk12
	ldr r0,[r5,#dbl_next]
	cmp r0,r5
	beq spk15
	LOCK
	ldr r1,[r0,#12]
	lsls r2,r1,#8
	lsrs r2,r2,#16
	beq spk13
	lsls r2,r2,#8
	bics r1,r1,r2
	str r1,[r0,#12]
	adds r0,r0,#24
	ldr r1,[r0,#dbl_next] 
	ldr r2,[r0,#dbl_prev] 
	str r1,[r2,#dbl_next] 
	str r2,[r1,#dbl_prev]
	UNLOCK
	ldr r2,[r7,#SC_CURPID]
	subs r0,r0,#24
	movs r1,#0
	str r1,[r0,#20]
	str r1,[r0,#24]
	str r2,[r0,#28]
	ldr r2,[r0,#8]
	lsrs r1,r2,#28
	lsls r1,r1,#3
	str r1,[r0,#16]
	lsls r2,r2,#4
	lsrs r2,r2,#4
	str r2,[r0,#8]
	b spk14
spk13
	UNLOCK
spk14
	bl sc_sysMsgFree
	b spk12
spk15
	subs r5,r5,#PCB_ALLOCED-PCB_MSGQUEUE
spk16
	ldr r0,[r5,#dbl_next]
	cmp r0,r5
	beq spk17
	bl sc_sysMsgFree
	b spk16
spk17
	subs r5,r5,#PCB_MSGQUEUE
	pop {r4}
spk20
	;
	; Send back observe-messages
;
	adds r5,#PCB_CONNECTED
spk21
	ldr r0,[r5,#dbl_next]
	cmp r0,r5
	beq spk22
	ldr r1,[r5,#PCB_ERRORCODE-PCB_CONNECTED]
	str r4,[r0,#28]
	str r1,[r0,#32 +4]
	ldr r7,[r0,#20]
	adds r0,r0,#32
	push {r0}
	mov r0,sp
	bl sc_msgAcquire
	mov r0,sp
	movs r1,r7
	movs r2,#0
	movs r3,r4
	bl sc_msgTxAlias
	add sp,sp,#4
	b spk21
spk22
	subs r5,r5,#PCB_CONNECTED
	;
	; If process is a timer or interrupt wake it up one last time
	; to clean up.
	; XXX: Maybe should do this before message-clean up ?
;
	ldrh r1,[r5,#PCB_FLAGS_H]
	movs r0,#PCB_FLG_IS_IRQ|PCB_FLG_IS_TIMER|PCB_FLG_IS_UIRQ
	ands r1,r1,r0
	beq spk22a
	lsrs r1,r1,#PCB_FLG_IS_TIMER_POS+1
	bcs spk22b
	ldr r0,spk_irq_vectors
	movs r2,#0
	ldr r1,[r5,#PCB_VECTOR]
	lsls r1,r1,#2
	str r2,[r0,r1]
spk22b
	movs r0,#0
	subs r0,r0,#2	; r0:= -2
	movs r1,r5
	swi 0
	;
	; Remove process from module array
;
spk22a
	lsls r0,r4,#18
	lsrs r0,r0,#16
	ldr r1,[r6,#MODULE_PCBS]
	movs r2,#0
	LOCK
	str r2,[r1,r0]
	ldr r2,[r6,#MODULE_NPROCESS]
	ldr r1,[r6,#MODULE_MAX_PROCESS]
	cmp r2,r1
	bne spk23
	;
	; Update counter
;
	ldr r1,[r6,#MODULE_PID_CNT]
	lsrs r0,r0,#2
	str r0,[r6,#MODULE_PID_NXT]
	adds r1,r1,#1
	lsls r1,r1,#14+8	; mask
	lsrs r1,r1,#14+8
	str r1,[r6,#MODULE_PID_CNT]
spk23
	subs r2,r2,#1
	str r2,[r6,#MODULE_NPROCESS]
	UNLOCK
	;
	; Done killing
;
	ldr r0,spk_magic
	ldr r1,[r5,#PCB_MAGIC]
	pop {r2}	; r2:= flag
	ldr r3,spk_sciopta
	ldr r6,[r3,#SC_CURPID]
	cmp r6,r4	; current process killed ?
	bne spk25	; no, normal processing
	cmp r1,r0	; dynamic process ?
	bne spk24	; no, nothing to do
	lsrs r2,r2,#14 +1	; kill -9 ?
	bcs spk24	; yes, skip clean up
	ldr r0,spk_sc_svc_stack	; switch stack
	mov sp,r0
	movs r0,r5
	subs r0,r0,#32
	bl sc_sysMsgFree	; free PCB
spk24
	;
	; Call dispatcher directly
;
	ldr r3,spk_dispatcher
	LOCK
	bx r3
spk25
	cmp r1,r0	; dynamic pcb ?
	bne spk26	; no, just return
	lsrs r2,r2,#14 +1	; kill -9
	bcs spk26	; yes, just return
	movs r0,r5	; free pcb
	subs r0,r0,#32
	bl sc_sysMsgFree
spk26
	RET r4-r7
	ALIGN
spk_sciopta
	DCD sciopta
spk_sc_modules
	DCD sc_modules
spk_irq_vectors
	DCD sc_irq_vectors
spk_dispatcher
	DCD sc_sysProcDispatcher
	IF SC_MAX_CONNECTOR > 0
spk_sc_connectors
	DCD sc_connectors
	ENDIF
	IF SC_PROC_HOOK = 1
	IF SC_PROCKILL_HOOK = 1
spk_SC_PROCKILLHOOK
	DCD SC_PROCKILLHOOK
	ENDIF
	ENDIF
spk_lit1
	DCD (0x020<<12)|(0x33<<24)|0x01
spk_lit2
	DCD (0x33<<24)|0x01
spk_magic
	DCD SC_PCB_MAGIC
spk_sc_svc_stack
	DCD sc_svc_stack
	SC_ENDFUNC sc_sysProcKill
;
;**************************************
; dispatcher
; Returns: r6 - current PCB
; r7 - sciopta
;**************************************
;
	IF CPUTYPE <= 4
	SC_TFUNC swap_out
	add r0,pc,#0
	BX R0	; switch to arm-mode
	CODE32
swap_out32
;
;
; Test if swap is allowed
; If swap not allowed the request flag will be set
;
;
	mrs r1,cpsr
	ldr r3,so_sciopta
	ands r1,r1,#PSR_I_BIT
	mov r0,#0
	strne r1,[r3,#SC_SCHEDULE_REQ]
	ldreq r1,[r3,#SC_SCHEDULER_LOCK]
	bxne lr
	ldr r2,[r3,#SC_CURPCB]
	cmp r1,#0
	strne r1,[r3,#SC_SCHEDULE_REQ]
	ldrheq r1,[r2,#PCB_FLAGS_H]
	bxne lr
	ands r1,r1,#PCB_FLG_IS_PRIO|PCB_FLG_IS_IDLE
	streq r3,[r3,#SC_SCHEDULE_REQ]
	bxeq lr
;
; Stack check
;
	IF SC_STACK_CHECK = 1
	SC_PUBLIC dp_stackcheck
	ldr r1,dp_stackmagic	; r1:= 0x17171717
	ldr r0,[r2,#PCB_STACKBTM]
	ldr r3,[r2,#PCB_STACKTOP]
	ldr r4,[r0,#0]	; r0:= last word of stack
	cmp r4,r1	; correct ?
	bne dp_err
	ldr r4,[r3,#0]	; r0:= first word of stack
	cmp r4,r1
	bne dp_err
	cmp sp,r0	; SP < end of stack
	blt dp_err	; yes, overflow
	cmp sp,r3	; SP > start of stack
	bgt dp_err2
	ENDIF
;
;
; Save registers
;
	;
	IF CPUTYPE = 4
	mra r0,r1,acc0
	IF SC_PROC_HOOK = 1 && SC_PROCMMU_HOOK = 1
	mrc p15,0,r3,c3,c0,0
	stmfd sp!,{r0-r1,r3,r8-r11,lr}
	ELSE
	stmfd sp!,{r0-r1,r8-r11,lr}
	ENDIF
	ELSE
	IF SC_PROC_HOOK = 1 && SC_PROCMMU_HOOK = 1
	mrc p15,0,r0,c3,c0,0
	stmfd sp!,{r0,r8-r11,lr}
	ELSE
	stmfd sp!,{r8-r11,lr}
	ENDIF
	ENDIF
;
;
; Disable interrupts and save stackpointer in PCB
;
;
	msr cpsr_c,#PSR_SYS_MODE|PSR_I_BIT
	str sp,[r2,#PCB_STACKPTR]
;
; Get PCB of process with highest priority
;
;
	SC_PUBLIC sc_sysProcDispatcher
;
; Enable all domains during dispatch
;
	IF SC_PROC_HOOK = 1
	IF SC_PROCMMU_HOOK = 1
	mov r0,#-1
	mcr p15,0,r0,c3,c0,0
	ENDIF
	ENDIF
	ldr r7,so_sciopta
	mov r1,#0
	LDR R0,[R7,#SC_PRIOMASK]
	str r1,[r7,#SC_SCHEDULE_REQ]
	IF CPUTYPE >= 2
	cmp r0,#0
	beq dp_idle
	clz r1,r0
	ELSE
	rsbs r2, r0, #0	; mask least set bit (and test for 0)
	and r2, r0, r2
	beq dp_idle
	; now get the least set bit-number
	;
	; posted in comp.sys.arm by
	; (Paul Clifford <paul@plasma.demon.com.uk>)
	movs r1, r2, lsr #16
	movne r1, #16
	movne r2, r2, lsr #16
	tst r2, #0xff00
	addne r1, r1,#8
	movne r2, r2, lsr #8
	tst r2, #0xf0
	addne r1, r1, #4
	movne r2, r2, lsr #4
	add r1, r1, r2, lsr #1
	sub r1, r1, r2, lsr #3
	ENDIF
	add r2,r7,#SC_READYLIST+dbl_next
	ldr r6,[r2,r1,lsl #3]
;
; Call swap hook
;
dp2
	IF SC_PROC_HOOK = 1
	IF SC_PROCSWAP_HOOK = 1
	ldr r13,so_sc_irq_stack
	ldr r2,so_SC_PROCSWAPHOOK
	ldr r2,[r7,r2]
	sub r0,r6,#PCB_PCBLIST
	ldr r1,[r7,#SC_CURPCB]
	cmp r2,#0
	BLXNE_R2_a
	ENDIF
	ENDIF
;
; Set current process ID, current PCB and
; current prioritized PCB
; Load stackpointer
;
	ldr r1,[r7,#SC_CURMODULE]
	ldr r0,[r6,#PCB_PID-PCB_PCBLIST]
	sub r2,r6,#PCB_PCBLIST
	sub r6,r6,#PCB_PCBLIST
	stmia r7,{r0,r2,r6}	; set curpid,curpcb and curpriopcb
	ldr r2,so_sc_modules
	movs r0,r0,lsr #24
	ldr r5,[r2,r0,lsl #2]
;
; Set module
;
	str r5,[r7,#SC_CURMODULE]
;
; Swap in
;
;
; Stacklayout (USR/SYS stack) (<= 3)
; r14_sys
; r12
; r3
; ...
; r0
; IF MMU
; dac
; ENDIF
; IF XSCALE
; acc0.h
; acc0.l
; ENDIF
; r11
; ...
; r4
; r14_irq
; spsr
; SP->
;
;
	ldr sp,[r6,#PCB_STACKPTR]	; get SP with flag
	tst r13,#1	; flag set (odd SP) ?
	bne full_swapin	; yes, full restore
	msr cpsr_c,#PSR_SYS_MODE	; no, enable interrupts
	IF CPUTYPE = 4
	IF SC_PROC_HOOK = 1 && SC_PROCMMU_HOOK = 1
	ldmfd sp!,{r0-r2,r8-r11,lr}	; restore registers
	mcr p15,0,r2,c3,c0,0
	ELSE
	ldmfd sp!,{r0-r1,r8-r11,lr}	; restore registers
	ENDIF
	mar acc0,r0,r1	; includeing acc0 on XScale
	ELSE
	IF SC_PROC_HOOK = 1 && SC_PROCMMU_HOOK = 1
	ldmfd sp!,{r0,r8-r11,lr}
	mcr p15,0,r0,c3,c0,0
	ELSE
	ldmfd sp!,{r8-r11,lr}
	ENDIF
	ENDIF
	mov r0,#1	; return to caller with 1
	bx lr	; indicating swap
	SC_PUBLIC full_swapin
	bic r12,r13,#1	; clear flag
	IF CPUTYPE = 4
	ldmfd r12!,{r2-r11,r13-r14}	; restore non-banked registers
	mar acc0,r13,r14	; restore acc0 on XScale
	IF SC_PROC_HOOK = 1 && SC_PROCMMU_HOOK = 1
	ldmfd r12!,{r0}
	mcr p15,0,r0,c3,c0,0
	ENDIF
	ELSE
	IF SC_PROC_HOOK = 1 && SC_PROCMMU_HOOK = 1
	ldmfd r12!,{r2-r11,r13}	; restore non-banked registers
	mcr p15,0,r13,c3,c0,0
	ELSE
	ldmfd r12!,{r2-r11}	; restore non-banked registers
	ENDIF
	ENDIF
	add sp,r12,#6*4	; set SP_sys
	msr cpsr_c,#PSR_IRQ_MODE|PSR_I_BIT	; switch to IRQ mode, I set
	msr spsr_cxsf,r2	; set old mode
	mov r14,r3	; set return address
	ldmia r12,{r0-r3,r12,lr}^	; restore volatile registers
	nop	; mandatory nop, do not remove !!!
	movs pc,lr	; return from exception
;
; Find and call idle process
;
; First try current module's init process, if
; it is not ready, take module[0].init 'cause this
; one is not allowed to be suspended.
;
;
dp_idle
	ldr r4,[r7,#SC_CURMODULE]
	cmp r4,#0
	beq dp_idle3
dp_idle2
	ldr r4,[r4,#MODULE_PCBS]
	ldr r6,[r4,#0]
	ldrh r0,[r6,#PCB_FLAGS_H]
	tst r0,#PCB_FLG_IS_IDLE
	addne r6,r6,#PCB_PCBLIST
	bne dp2
dp_idle3
	ldr r4,so_sc_modules
	ldr r4,[r4,#0]
	b dp_idle2
	IF SC_STACK_CHECK = 1
dp_err
	ldr r0,dp_overrun
dp_err_cont
	movs r1,r2
	ldr r2,dp_sc_sysError
	bx r2
dp_err2
	ldr r0,dp_underrun
	b dp_err_cont
	ENDIF
;
; Literal pool
;
	ALIGN
	IF SC_PROC_HOOK = 1
	IF SC_PROCMMU_HOOK = 1
dp_55555555
	DCD 0x55555555
	ENDIF
	ENDIF
	IF SC_STACK_CHECK = 1
dp_sc_sysError
	DCD sc_sysError
dp_stackmagic
	DCD 0x17171717
dp_overrun
	DCD (0x62<<24)|(0x02e<<12)|0x02
dp_underrun
	DCD (0x62<<24)|(0x02f<<12)|0x02
	ENDIF
so_sciopta
	DCD sciopta
so_sc_modules
	DCD sc_modules
	IF SC_PROC_HOOK = 1
so_sc_irq_stack
	DCD sc_irq_stack
	IF SC_PROCSWAP_HOOK = 1
so_SC_PROCSWAPHOOK
	DCD SC_PROCSWAPHOOK
	ENDIF
	IF SC_PROCMMU_HOOK = 1
so_SC_PROCMMUHOOK
	DCD SC_PROCMMUHOOK
	ENDIF
	ENDIF
	SC_ENDFUNC swap_out
	ELSE
	; ***********************************************************
	; * Cortex *
	; ***********************************************************
	SC_TFUNC swap_out
;
;
; Test if swap is allowed
; If swap not allowed the request flag will be set
;
;
	mrs r1,PRIMASK
	ldr r3,so_sciopta
	movs r0,#0
	tst r1,#1
	itt ne
	strne r1,[r3,#SC_SCHEDULE_REQ]
	bxne lr
	ldr r1,[r3,#SC_SCHEDULER_LOCK]
	cmp r1,#0
	itt ne
	strne r1,[r3,#SC_SCHEDULE_REQ]
	bxne lr
	ldr r2,[r3,#SC_CURPCB]
	ldrh r1,[r2,#PCB_FLAGS_H]
	tst r1,#PCB_FLG_IS_PRIO|PCB_FLG_IS_IDLE
	itt eq
	streq r3,[r3,#SC_SCHEDULE_REQ]
	bxeq lr
;
; Stack check
;
	IF SC_STACK_CHECK = 1
	SC_PUBLIC dp_stackcheck
	ldr r1,dp_stackmagic	; r1:= 0x17171717
	ldr r0,[r2,#PCB_STACKBTM]
	ldr r3,[r2,#PCB_STACKTOP]
	ldr r4,[r0,#0]	; r0:= last word of stack
	cmp r4,r1	; correct ?
	bne dp_err
	ldr r4,[r3,#0]	; r0:= first word of stack
	cmp r4,r1
	bne dp_err
	cmp sp,r0	; SP < end of stack
	blt dp_err	; yes, overflow
	cmp sp,r3	; SP > start of stack
	bgt dp_err2
	ENDIF
	movw r0,#0xed04	; NVIC_ICSR
	movt r0,#0xe000
	mov r1,#0x10000000
	str r1,[r0,#0]	; call pendSV
	nop
	movs r0,#1
	bx lr
;
;
; Save registers
; IN: r2 - current process
;
;
	SC_PUBLIC sc_sysPendSV
	cpsid i
	cbz r2,no_save
	push.w {r4-r11,lr}
;
;
; Disable interrupts and save stackpointer in PCB
;
;
	mov r0,sp
	str r0,[r2,#PCB_STACKPTR]
;
; Get PCB of process with highest priority
;
;
	SC_PUBLIC sc_sysProcDispatcher
no_save
	ldr r7,so_sciopta
	movs r1,#0
	LDR R0,[R7,#SC_PRIOMASK]
	str r1,[r7,#SC_SCHEDULE_REQ]
	cbz r0,dp_idle
	clz r1,r0
	adds r2,r7,#SC_READYLIST+dbl_next
	ldr r6,[r2,r1,lsl #3]
;
; Call swap hook
;
dp2
	IF SC_PROC_HOOK = 1
	IF SC_PROCSWAP_HOOK = 1
	movw r3,#SC_PROCSWAPHOOK
	ldr r2,[r7,r3]
	subs r0,r6,#PCB_PCBLIST
	cbz r2,dp2a
	ldr r3,=sc_svc_stack
	ldr r1,[r7,#SC_CURPCB]
	mov sp,r3
	blx r2
dp2a
	ENDIF
	ENDIF
;
; Set current process ID, current PCB and
; current prioritized PCB
; Load stackpointer
;
	ldr r1,[r7,#SC_CURMODULE]
	ldr r0,[r6,#PCB_PID-PCB_PCBLIST]
	subs r2,r6,#PCB_PCBLIST
	subs r6,r6,#PCB_PCBLIST
	stmia r7,{r0,r2,r6}	; set curpid,curpcb and curpriopcb
	ldr r2,so_sc_modules
	lsrs r0,r0,#24
	ldr r5,[r2,r0,lsl #2]
;
; Set module and program MMU
;
	str r5,[r7,#SC_CURMODULE]
;
; Swap in
;
;
; Stacklayout
; xPSR
; returnAddress
; LR
; r12
; r3
; r2
; r1
; r0
; ---------
; lr
; r11
; ...
; r4
; SP->
;
	movs r0,#1	; return to caller with 1
	ldr r1,[r6,#PCB_STACKPTR]	; get SP with flag
	mov sp,r1
	pop.w {r4-r11,lr}
	cpsie i	; enable interrupts
	bx lr
;
; Find and call idle process
;
; First try current module's init process, if
; it is not ready, take module[0].init 'cause this
; one is not allowed to be suspended.
;
;
dp_idle
	ldr r4,[r7,#SC_CURMODULE]
	cbz r4,dp_idle3
dp_idle2
	ldr r4,[r4,#MODULE_PCBS]
	ldr r6,[r4,#0]
	ldrh r0,[r6,#PCB_FLAGS_H]
	tst r0,#PCB_FLG_IS_IDLE
	it ne
	addne r6,r6,#PCB_PCBLIST
	bne dp2
dp_idle3
	ldr r4,so_sc_modules
	ldr r4,[r4,#0]
	b dp_idle2
	IF SC_STACK_CHECK = 1
dp_err
	ldr r0,dp_overrun
dp_err_cont
	movs r1,r2
	ldr r2,dp_sc_sysError
	bx r2
dp_err2
	ldr r0,dp_underrun
	b dp_err_cont
	ENDIF
;
; Literal pool
;
	ALIGN
	IF SC_STACK_CHECK = 1
dp_sc_sysError
	DCD sc_sysError
dp_stackmagic
	DCD 0x17171717
dp_overrun
	DCD (0x62<<24)|(0x02e<<12)|0x02
dp_underrun
	DCD (0x62<<24)|(0x02f<<12)|0x02
	ENDIF
so_sciopta
	DCD sciopta
so_sc_modules
	DCD sc_modules
	SC_ENDFUNC swap_out
	ENDIF
;
;**************************************************************************
; Pool functions
;
; S03114BS2
;
; +Date: 2009/08/14 14:41:01 +
; +Revision: 1.31.2.3 +
;**************************************************************************
;
;
;**************************************
; sc_poolHook_t sc_poolHookRegister(int type,sc_poolHook_t newhook)
;**************************************
;
	IF SC_POOL_HOOK = 1
	XSC_TFUNC sc_poolHookRegister,-8
	SC_TFUNC sc_poolHookRegister
	ldr r3,plhr_sciopta
	ldr r2,[r3,#SC_CURMODULE]
	cmp r2,#0	; module hook ?
	beq plhr1	; = 0 => no global
	IF SC_POOLKILL_HOOK = 1
	movs r3,#MODULE_POOLKILLHOOK
	cmp r0,#SC_SET_POOLKILL_HOOK
	beq plhr2
	ENDIF
	IF SC_POOLCREATE_HOOK = 1
	movs r3,#MODULE_POOLCREATEHOOK
	cmp r0,#SC_SET_POOLCREATE_HOOK
	beq plhr2
	ENDIF
	; type parameter wrong, fatal error (module level)
	IF SC_POOL_PARA_CHECK = 1
	movs r1,r0
	ldr r0,plhr_illval1
	b plhr_err_cont	; => sc_sysError
	ENDIF
plhr1
	IF SC_POOLKILL_HOOK = 1
	ldr r2,plhr_SC_POOLKILLHOOK
	cmp r0,#SC_SET_POOLKILL_HOOK
	beq plhr2
	ENDIF
	IF SC_POOLCREATE_HOOK = 1
	ldr r2,plhr_SC_POOLCREATEHOOK
	cmp r0,#SC_SET_POOLCREATE_HOOK
	beq plhr2
	ENDIF
	; type parameter wrong, fatal error (system level)
	IF SC_POOL_PARA_CHECK = 1
	movs r1,r0
	ldr r0,plhr_illval2
plhr_err_cont
	bl sc_sysError
	ELSE
	movs r0,#0
	bx lr
	ENDIF
plhr2
	ldr r0,[r2,r3]
	str r1,[r2,r3]
	bx lr
	ALIGN
plhr_sciopta
	DCD sciopta
	IF SC_POOLKILL_HOOK = 1
plhr_SC_POOLKILLHOOK
	DCD SC_POOLKILLHOOK
	ENDIF
	IF SC_POOLCREATE_HOOK = 1
plhr_SC_POOLCREATEHOOK
	DCD SC_POOLCREATEHOOK
	ENDIF
	IF SC_POOL_PARA_CHECK = 1
plhr_illval1
	DCD (0x17<<24)|(0x030<<12)|0x02
plhr_illval2
	DCD (0x17<<24)|(0x030<<12)|0x01
	ENDIF
	SC_ENDFUNC sc_poolHookRegister
	ENDIF
;
;**************************************
; sc_poolid_t sc_poolDefault(int idx)
;
; Set default pool
;
; r0 - idx or -1 => regquest
;**************************************
;
	XSC_TFUNC sc_poolDefault,-12
	SC_TFUNC sc_poolDefault
	ldr r3,pd_sciopta	; r3:= sciopta
	adds r1,r0,#1	; check for -1
	ldr r2,[r3,#SC_CURPCB]	; r2:= current PCB
	ldr r3,[r3,#SC_CURMODULE]	; r3:= current MCB
	beq pd9	; = -1 > query
	bmi pd_err	; < -1 => error
	ldr r1,[r3,#MODULE_MAX_POOLS]	; r1:= max. pools
	cmp r0,r1	; idx > max pools ?
	bge pd_err	; yes => error
	ldr r1,[r3,#MODULE_POOL]	; get module.pool[]
	lsls r0,r0,#2	; r0:= long-offset
	ldr r1,[r1,r0]	; r1:= pool cb
	lsrs r0,r0,#2	; r0:= index
	cmp r1,#1	; pool valid
	bls pd_err	; <= 1 => no
	ldrh r1,[r2,#PCB_DEFAULTPOOL_H]	; r1:= old default-pool
	strh r0,[r2,#PCB_DEFAULTPOOL_H]	; set new default-pool
	movs r0,r1
	bx lr
pd9
	ldrh r0,[r2,#PCB_DEFAULTPOOL_H]	; get current default-pool
	bx lr
pd_err
	movs r1,r0	; extra code = index
	ldrh r0,[r2,#PCB_DEFAULTPOOL_H]
	push {r0,lr}	; save return and default-pool
	ldr r0,poold_lit0
	bl sc_sysError	; jump into error
	RET r0	; pop lr and return default
	ALIGN
pd_sciopta
	DCD sciopta
poold_lit0
	DCD (0x14<<24)|(0x001<<12)
	SC_ENDFUNC sc_poolDefault
;
;**************************************
; int sc_poolInfo(sc_moduleid_t mid, sc_poolid_t plid, sc_pool_cb_t *info)
;
; Copy a pool-cb block to a user buffer.
;
; r0 - module id
; r1 - pool id
; r2 - ptr pool_cb to be fille
;**************************************
;
	XSC_TFUNC sc_poolInfo,-4
	SC_TFUNC sc_poolInfo
	IF SC_POOL_PARA_CHECK = 1
	lsrs r0,r0,#24	; r0:= module index
	cmp r2,#0
	beq pi_err	; ptr = 0 => error
	lsrs r3,r1,#7	; mask pool-index
	beq pi0	; = 0 => no module id
	; check if module id of plid = mid parameter
	lsls r3,r1,#1	; clear module-0 flag
	lsrs r3,r3,#25
	cmp r0,r3
	bne pi_ret0
pi0
	ELSE
	lsrs r0,r0,#24	; r0:= module index
	ENDIF
	mov r12,r2	; save ptr
	ldr r3,pi_sciopta	; sciopta
	cmp r0,#SC_MAX_MODULE	; >= SC_MAX_MODULE
	bge pi_err1	; yes => error
	ldr r2,pi_sc_modules	; r2:= sc_modules
	lsls r0,r0,#2	; r0:= long offset
	ldr r2,[r2,r0]	; r2:= MCB
	cmp r2,#1	; valid ?
	bls pi_ret0	; no, return 0
	ldr r3,[r2,#MODULE_MAX_POOLS]	; r3:= max. pools
	lsls r0,r1,#8	; mask module-id
	lsrs r0,r0,#8
	cmp r0,r3
	bge pi_err2	; out of range => error
	ldr r1,[r2,#MODULE_POOL]	; r1:= module.pool[]
	lsls r0,r0,#2	; r0:= long offset
	ldr r1,[r1,r0]	; r1:= pool-cb
	cmp r1,#1	; valid
	bls pi_ret0	; no, return 0
	mov r0,r12	; restore ptr
	movs r2,#SIZEOF_POOL_CB>>2	; size of pool-cb in longs
pi1
	subs r2,r2,#1	; copy pool-cb
	ldmia r1!,{r3}
	stmia r0!,{r3}
	bne pi1
	movs r0,#1	; return 1
	bx lr
pi_ret0
	movs r0,#0
	bx lr
pi_err2
	ldr r0,pi_lit2
	b pi_err_cont
pi_err1
	lsls r1,r0,#24
	ldr r0,pi_lit1
pi_err_cont
	bl sc_sysError
	IF SC_POOL_PARA_CHECK = 1
pi_err
	movs r1,#0
	ldr r0,pi_lit0
	b pi_err_cont
	ENDIF
	ALIGN
pi_sciopta
	DCD sciopta
pi_sc_modules
	DCD sc_modules
pi_lit2
	DCD (0x001<<12)|(0x13<<24)|0x02
pi_lit1
	DCD (0x027<<12)|(0x13<<24)|0x02
	IF SC_POOL_PARA_CHECK = 1
pi_lit0
	DCD (0x00A<<12)|(0x13<<24)|0x04
	ENDIF
	SC_ENDFUNC sc_poolInfo
;
;**************************************
; sc_poolid_t sc_poolIdGet(const char *name)
;**************************************
;
	XSC_TFUNC sc_poolIdGet,-12
	SC_TFUNC sc_poolIdGet
	ldr r3,plig_sciopta
	cmp r0,#0
	bne plig1
plig0
	ldr r0,[r3,#SC_CURPCB]
	ldrh r0,[r0,#PCB_DEFAULTPOOL_H]
	bx lr
plig1
	ldrb r1,[r0,#0]
	cmp r1,#0
	beq plig0
	IF SC_POOL_PARA_CHECK = 1
	movs r1,#SC_POOL_NAME_SIZE
plig2
	ldrb r2,[r0,r1]
	cmp r2,#0
	beq plig3
	subs r1,r1,#1
	bpl plig2
	movs r1,r0
	ldr r0,plig_lit0
	bl sc_sysError
	ALIGN
plig_lit0
	DCD (0x15<<24)|(0x025<<12)|0x02
	ENDIF
plig3
	ldr r3,[r3,#SC_CURMODULE]
	ldr r2,[r3,#MODULE_MAX_POOLS]
	ldr r3,[r3,#MODULE_POOL]
	push {r4-r6}
	mov r12,r0
plig4
	ldmia r3!,{r1}
	cmp r1,#1
	bls plig7
	ldr r0,[r1,#POOL_ID]
	IF POOL_NAME > 255
	adds r1,r1,#POOL_NAME-255
	adds r1,r1,#255
	ELSE
	adds r1,r1,#POOL_NAME
	ENDIF
	lsls r0,r0,#8
	lsrs r0,r0,#8
	mov r4,r12
plig5
	ldrb r5,[r1,#0]
	adds r1,r1,#1
	ldrb r6,[r4,#0]
	adds r4,r4,#1
	cmp r5,r6
	bne plig7
	cmp r5,#0
	bne plig5
plig6
	pop {r4-r6}
	bx lr
plig7
	subs r2,r2,#1
	bne plig4
	movs r0,#-SC_ILLEGAL_POOLID
	rsbs r0,r0,#0
	b plig6
	ALIGN
plig_sciopta
	DCD sciopta
plig_sc_modules
	DCD sc_modules
	SC_ENDFUNC sc_poolIdGet
;
;**************************************
; void sc_poolKill(sc_poolid_t plid)
; void sc_sysPoolKill(sc_poolid_t plid,sc_moduleid_t mid)
;**************************************
;
	XSC_TFUNC sc_poolKill,-12
	SC_TFUNC sc_poolKill
	ldr r1,sspok_sciopta
	ldr r1,[r1,#SC_CURMODULE]
	ldr r1,[r1,#MODULE_ID]
	SC_TPUBLIC sc_sysPoolKill
	push {r4-r5,lr}
	ldr r3,sspok_sciopta
	ldr r4,sspok_sc_modules
	lsrs r2,r1,#24
	cmp r2,#SC_MAX_MODULE
	bge sspok_err
	lsls r2,r2,#2
	LOCK
	ldr r4,[r4,r2]
	cmp r4,#0
	beq sspok_err0
	ldr r3,[r4,#MODULE_MAX_POOLS]
	ldr r2,[r4,#MODULE_POOL]
	cmp r0,r3
	bge sspok_err1
	lsls r3,r0,#2
	ldr r5,[r2,r3]
	cmp r5,#1
	bls sspok_err1
	ldr r1,[r5,#POOL_LOCK]
	cmp r1,#0
	bne sspok_err2
	movs r1,#1
	str r1,[r2,r3]
	UNLOCK
	bl sysPoolKill
	RET r4-r5
sspok_err2
	movs r1,r0
	ldr r0,sspok_lit2
	b sspok_err_cont1
sspok_err1
	movs r1,r0
	ldr r0,sspok_lit1
sspok_err_cont1
	UNLOCK
	b sspok_err_cont
sspok_err0
	UNLOCK
sspok_err
	ldr r0,sspok_lit0
sspok_err_cont
	bl sc_sysError
	ALIGN
sspok_sciopta
	DCD sciopta
sspok_sc_modules
	DCD sc_modules
sspok_lit0
	DCD (0x027<<12)|(0x16<<24)|0x02
sspok_lit1
	DCD (0x001<<12)|(0x12<<24)|0x02
sspok_lit2
	DCD (0x004<<12)|(0x12<<24)|0x02
	SC_ENDFUNC sc_poolKill
;
;**************************************
; void sysPoolKill(sc_pool_cb_t *pool,sc_module_cb_t *module)
; r4 = module
; r5 = pool_cb
;**************************************
;
	SC_TFUNC sysPoolKill
	push {lr}
	cmp r5,#1
	bls spok9
	IF SC_POOL_HOOK = 1
	IF SC_POOLKILL_HOOK = 1
	ldr r2,[r4,#MODULE_POOLKILLHOOK]
	cmp r2,#0
	beq spok1
	movs r0,r5
	BLX_R2_t
spok1
	ldr r2,spok_SC_POOLKILLHOOK
	ldr r2,[r2,#0]
	cmp r2,#0
	beq spok2
	movs r0,r5
	BLX_R2_t
spok2
	ENDIF
	ENDIF
	ldr r1,[r5,#POOL_ID]
	IF SC_CDEBUG = 1
	ldr r3,spok_lit0
	ldr r0,[r5,#POOL_SIZE]
	lsrs r0,r0,#2
	adds r0,r0,#SIZEOF_POOL_CB>>2
spok4
	subs r0,r0,#1
	stmia r5!,{r3}
	bne spok4
	ELSE
	IF SC_MSG_STAT = 1
	adds r5,r5,#POOL_STAT
	movs r3,#0
	movs r0,#SIZEOF_SC_POOL_STAT>>2
spok3
	subs r0,r0,#1
	stmia r5!,{r3}
	bne spok3
	ENDIF
	ENDIF
	ldr r2,[r4,#MODULE_POOL]
	movs r3,#0
	lsls r1,r1,#24
	lsrs r1,r1,#22
	LOCK
	str r3,[r2,r1]
	ldr r0,[r4,#MODULE_NPOOLS]
	subs r0,r0,#1
	str r0,[r4,#MODULE_NPOOLS]
	UNLOCK
spok9
	EXIT
	ALIGN
	IF SC_POOL_HOOK = 1
	IF SC_POOLKILL_HOOK = 1
spok_SC_POOLKILLHOOK
	DCD sciopta+SC_POOLKILLHOOK
	ENDIF
	ENDIF
	IF SC_CDEBUG = 1
spok_lit0
	DCD 0xdddddddd
	ENDIF
	SC_ENDFUNC sysPoolKill
;
;**************************************
; sc_poolid_t sc_sysPoolCreate(char *start,
; sc_plsize_t size,
; unsigned int nbufsizes,
; sc_bufsize_t *bufsize,
; const char *name,
; sc_moduleid_t moduleid)
;
; R0 start
; R1 size
; R2 nbufsizes
; R3 bufsize
; (SP) name
; 4(SP) moduleid
;
;
	XSC_TFUNC sc_sysPoolCreate,8
	XSC_TFUNC sc_poolCreate,0
;
; Public function
;
	SC_TFUNC sc_poolCreate
	mov r12,r0
	ldr r0,spoc_sciopta
	ldr r0,[r0,#SC_CURMODULE]
	ldr r0,[r0,#MODULE_ID]
	push {r0,lr}
	ldr r0,[sp,#8]
	push {r0}
	mov r0,r12
	bl sc_sysPoolCreate
	add sp,sp,#8
	EXIT
	SC_TPUBLIC sc_sysPoolCreate
	push {r0-r7,lr}
;
; Check parameter number of buffer sizes
;
	cmp r2,#SC_MAX_NUM_BUFFERSIZES
	bhi spoc_err0
	cmp r2,#16
	beq spoc1
	cmp r2,#8
	beq spoc1
	cmp r2,#4
	beq spoc1
spoc_err0
	movs r1,r2
	movs r2,#(0x005<<12)>>12
;
; Prepare error and jump to error handler
;
spoc_cont
	lsls r2,r2,#12
	movs r0,#0x02
	orrs r0,r0,r2
	movs r2,#(0x10<<24)>>24
	lsls r2,r2,#24
	orrs r0,r0,r2
	bl sc_sysError
spoc_err1a
	movs r6,r7
spoc_err1
	movs r2,#(0x006<<12)>>12
	movs r1,r6
	b spoc_cont
spoc_err2
	movs r2,#(0x003<<12)>>12
	b spoc_cont
spoc_err3
	movs r2,#(0x025<<12)>>12
	ldr r1,[sp,#36]
	b spoc_cont
spoc_err4
	movs r2,#(0x002<<12)>>12
spoc_cont1
	UNLOCK
	b spoc_cont
spoc_err5
	movs r2,#(0x00d<<12)>>12
	b spoc_cont1
spoc_err6
	movs r1,r5
	movs r2,#(0x030<<12)>>12
	b spoc_cont1
spoc_err7
	movs r2,#(0x030<<12)>>12
	b spoc_cont
spoc_ill_module
	movs r2,#(0x027<<12)>>12
	b spoc_cont
;
; Check if pool buffer sizes are strictly monotonic
; increasing and not equal and 4byte aligned
;
spoc1
	movs r4,r3
	subs r5,r2,#1
	ldr r6,[r4,#0]
spoc2
	ldr r7,[r4,#4]
	cmp r6,r7
	bhi spoc_err1
	lsls r0,r6,#30	; check alignment
	bne spoc_err1
	movs r6,r7
	adds r4,r4,#4
	subs r5,r5,#1
	bne spoc2
	lsls r4,r7,#30	; check last buffer size alignment
	bne spoc_err1a
;
; Check pool size and size alignment
;
	lsls r4,r1,#30
	bne spoc_err7
	subs r6,r1,r7	; r6:= size-bufsize[nbufsize-1]
	IF SIZEOF_POOL_CB > 255
	IF SIZEOF_POOL_CB > 510
	subs r6,r6,#SIZEOF_POOL_CB-510
	subs r6,r6,#255
	ELSE
	subs r6,r6,#SIZEOF_POOL_CB-255
	ENDIF
	subs r6,r6,#255
	ELSE
	subs r6,r6,#SIZEOF_POOL_CB
	ENDIF
	subs r6,r6,#32
	IF SC_MSG_CHECK = 1
	subs r6,r6,#4
	ENDIF
	bmi spoc_err2
;
; Check pool string length
;
	ldr r4,[sp,#36]
	cmp r4,#0
	beq spoc_err3
	movs r5,#SC_POOL_NAME_SIZE+1
	rsbs r5,r5,#0
spoc3
	ldrb r6,[r4,#0]
	adds r4,r4,#1
	cmp r6,#0
	beq spoc4
	adds r5,r5,#1
	bne spoc3
	b spoc_err3
spoc4
	adds r5,r5,#SC_POOL_NAME_SIZE+1
	beq spoc_err3
;
; Check module id
;
	ldr r1,[sp,#40]
	lsrs r6,r1,#24
	cmp r6,#SC_MAX_MODULE
	bhs spoc_ill_module
;
; Get Module Control Block
;
	ldr r7,spoc_sc_modules
	lsls r6,r6,#2
	ldr r6,[r7,r6]
	cmp r6,#0
	beq spoc_ill_module
;
; Check if pool name is unique
;
	ldr r5,[r6,#MODULE_NPOOLS]
	cmp r5,#0
	beq spoc8
	ldr r4,[r6,#MODULE_POOL]
spoc5
	ldr r7,[r4,#0]
	adds r4,r4,#4
	cmp r7,#0
	beq spoc5
	cmp r7,#1
	beq spoc7
	IF POOL_NAME > 255
	adds r7,r7,#POOL_NAME-255
	adds r7,r7,#255
	ELSE
	adds r7,r7,#POOL_NAME
	ENDIF
	ldr r3,[sp,#36]
spoc6
	ldrb r2,[r7,#0]
	ldrb r1,[r3,#0]
	adds r7,r7,#1
	adds r3,r3,#1
	cmp r2,r1
	bne spoc7
	cmp r2,#0
	bne spoc6
	b spoc_err3	; illegal name
spoc7
	subs r5,r5,#1
	bne spoc5
;
; Check if maximum number of pools reached.
; If not increase pool number
;
spoc8
	LOCK
	ldr r1,[r6,#MODULE_NPOOLS]
	ldr r4,[r6,#MODULE_MAX_POOLS]
	cmp r1,r4
	beq spoc_err4
	adds r1,r1,#1
;
; Search for a free slot in pool CB pointer array
;
	ldr r4,[r6,#MODULE_POOL]
	str r1,[r6,#MODULE_NPOOLS]
	subs r7,r4,#4
spoc9
	ldr r0,[r7,#4]
	adds r7,r7,#4
	cmp r0,#0
	beq spoc10
	subs r1,r1,#1
	bne spoc9
spoc10
	subs r4,r4,r7
	rsbs r4,r4,#0
	lsrs r4,r4,#2	; index
;
; Get start address of Pool CB
; If zero allocate pool from module, if non zero check if aligned
;
	ldr r5,[sp,#0 +4]
	cmp r5,#0
	bne spoc10a
	ldr r0,[r6,#MODULE_FREESIZE]
	ldr r5,[r6,#MODULE_DATAPTR]
	ldr r1,[sp,#4 +4]
	cmp r1,r0
	IF CPUTYPE > 3
	bhi.n spoc_err5	; XXX
	ELSE
	bhi spoc_err5	; XXX
	ENDIF
	subs r0,r0,r1
	str r0,[r6,#MODULE_FREESIZE]
	adds r1,r1,r5
	str r1,[r6,#MODULE_DATAPTR]
	b spoc11
spoc10a
	lsls r0,r5,#30
	IF CPUTYPE > 3
	bne.n spoc_err6	; XXX
	ELSE
	bne spoc_err6	; XXX
	ENDIF
;
; Mark slot as used
;
spoc11
	movs r0,#1
	str r0,[r7,#0]
	UNLOCK
;
; Setup Pool CB
;
	movs r0,r5
	movs r1,#SIZEOF_POOL_CB>>2
	movs r2,#0
spoc12
	str r2,[r0,#0]
	adds r0,r0,#4
	subs r1,r1,#1
	bne spoc12
	ldr r0,[r6,#MODULE_ID]
	lsrs r0,r0,#24
	bne spoc13
	movs r0,#0x80
spoc13
	lsls r0,r0,#24
	orrs r4,r4,r0
	str r4,[r5,#POOL_ID]
	ldr r0,spoc_sciopta
	ldr r0,[r0,#SC_CURPID]
	str r0,[r5,#POOL_CREATOR]
	ldr r0,[sp,#4]
	adds r2,r5,r0	; end
	IF SIZEOF_POOL_CB > 255
	IF SIZEOF_POOL_CB > 510
	movs r1,#SIZEOF_POOL_CB-510
	adds r1,r1,#255
	ELSE
	movs r1,#SIZEOF_POOL_CB-255
	ENDIF
	adds r1,r1,#255
	ELSE
	movs r1,#SIZEOF_POOL_CB
	ENDIF
	subs r0,r0,r1
	str r0,[r5,#POOL_SIZE]
	adds r1,r1,r5
	str r1,[r5,#POOL_START]
	str r1,[r5,#POOL_CUR]
	str r2,[r5,#POOL_END]
	; copy buffersizes
	ldr r4,[sp,#8]
	movs r0,r4
	str r4,[r5,#POOL_NBUFSIZES]
	ldr r1,[sp,#12]
	movs r2,r5
	adds r2,r2,#POOL_BUFSIZE
spoc14
	ldmia r1!,{r3}
	stmia r2!,{r3}
	subs r0,r0,#1
	bne spoc14
	IF SC_MAX_NUM_BUFFERSIZES = 16
	cmp r4,#4
	bne spoc14b
	str r3,[r2,#12]
spoc14b
	ENDIF
	; init free and waiter list
	movs r0,r4
	movs r1,r5
	adds r1,r1,#POOL_FREED
	movs r2,r5
	adds r2,r2,#POOL_WAITER
spoc15
	str r1,[r1,#0]
	str r1,[r1,#4]
	str r2,[r2,#0]
	str r2,[r2,#4]
	adds r1,r1,#8
	adds r2,r2,#8
	subs r0,r0,#1
	bne spoc15
	; copy pool name
	movs r3,r5
	IF POOL_NAME > 255
	adds r3,r3,#POOL_NAME-255
	adds r3,r3,#254
	ELSE
	adds r3,r3,#POOL_NAME-1
	ENDIF
	ldr r0,[sp,#36]
spoc15a
	ldrb r2,[r0,#0]
	adds r0,r0,#1
	adds r3,r3,#1
	strb r2,[r3,#0]
	cmp r2,#0
	bne spoc15a
	IF SC_MSG_CHECK = 1
	ldr r0,[r5,#POOL_START]
	movs r2,#0xcc
	lsls r1,r2,#8
	orrs r2,r2,r1
	lsls r1,r2,#16
	orrs r2,r2,r1
	ldr r1,[r5,#POOL_SIZE]
spoc16
	str r2,[r0,#0]
	adds r0,r0,#4
	subs r1,r1,#4
	bne spoc16
	ldr r0,[r5,#POOL_CUR]
	movs r2,#0xee
	lsls r1,r2,#8
	orrs r2,r2,r1
	lsls r1,r2,#16
	orrs r2,r2,r1
	str r2,[r0,#0]
	adds r0,r0,#4
	str r0,[r5,#POOL_CUR]
	ENDIF
;
; Call pool-create hook(s) if defined and set
;
	IF SC_POOL_HOOK = 1
	IF SC_POOLCREATE_HOOK = 1
	ldr r2,[r6,#MODULE_POOLCREATEHOOK]
	cmp r2,#0
	beq spoc17
	movs r0,r5
	BLX_R2_t
spoc17
	ldr r0,spoc_SC_POOLCREATEHOOK
	ldr r2,[r0,#0]
	cmp r2,#0
	beq spoc18
	movs r0,r5
	BLX_R2_t
spoc18
	ENDIF
	ENDIF
;
; Insert Pool CB pointer into Pool CB pointer array
; and return Pool index
;
	str r5,[r7,#0]
	add sp,sp,#4*4
	ldr r0,[r5,#POOL_ID]
	lsls r0,r0,#8
	lsrs r0,r0,#8
	RET r4-r7
	ALIGN
spoc_sciopta
	DCD sciopta
spoc_sc_modules
	DCD sc_modules
	IF SC_POOL_HOOK = 1
	IF SC_POOLCREATE_HOOK = 1
spoc_SC_POOLCREATEHOOK
	DCD sciopta+SC_POOLCREATEHOOK
	ENDIF
	ENDIF
	SC_ENDFUNC sc_poolCreate
;
;**************************************
; void sc_poolReset(sc_poolid_t plid)
;**************************************
;
	XSC_TFUNC sc_poolReset,-12
	SC_TFUNC sc_poolReset
	push {lr}
	ldr r3,por_sciopta	; r3:= sciopta
	ldr r3,[r3,#SC_CURMODULE]	; r3:= current module
	ldr r2,[r3,#MODULE_MAX_POOLS]	; r2:= max. pools
	ldr r1,[r3,#MODULE_POOL]	; r1:= pool-array
	cmp r0,r2	; plid in range
	bge por_err	; >= MAX_POOLS => error
	lsls r2,r0,#2	; r2:= 4byte offset
	adds r2,r2,r1	; r2:= &pools[plid]
	LOCK
	ldr r3,[r2,#0]	; r3:= pool_cb
	cmp r3,#1	; valid `?
	bls por_err0	; no, illegal pool-id
	ldr r1,[r3,#POOL_LOCK]	; pool in use ?
	cmp r1,#0	; (lock != 0 )
	bne por_err1	; yes , error
	movs r1,#1
	str r1,[r2,#0]	; mark pool as invalid
	UNLOCK
	mov r12,r2	; save r2 (&pools[plid])
	IF SC_MSG_STAT = 1
	movs r0,#POOL_STAT>>2	; clear statistic
	lsls r0,r0,#2
	adds r0,r0,r3
	movs r1,#0
	movs r2,#SIZEOF_SC_POOL_STAT>>2
por1
	subs r2,r2,#1
	stmia r0!,{r1}
	bne por1
	ENDIF
	IF SC_CDEBUG = 1
	ldr r0,[r3,#POOL_START]	; fill pool with virgin pattern
	ldr r2,[r3,#POOL_SIZE]
	ldr r1,por_virgin
por2
	subs r2,r2,#4
	stmia r0!,{r1}
	bne por2
	ENDIF
	ldr r2,[r3,#POOL_NBUFSIZES]	; reset free and waiter list
	movs r0,r3
	adds r0,r0,#POOL_FREED
	movs r1,r3
	adds r1,r1,#POOL_WAITER
por3
	str r0,[r0,#0]
	str r0,[r0,#4]
	str r1,[r1,#0]
	str r1,[r1,#4]
	adds r0,r0,#8
	adds r1,r1,#8
	subs r2,r2,#1
	bne por3
	ldr r0,[r3,#POOL_START]
	mov r2,r12
	IF SC_MSG_CHECK = 1
	ldr r1,por_endmark
	stmia r0!,{r1}
	ENDIF
	str r0,[r3,#POOL_CUR]
	str r3,[r2,#0]
	EXIT
por_err1
	UNLOCK
	ldr r0,por_lit1
	ldr r1,[r3,#POOL_ID]
	b por_err_cont1
por_err0
	UNLOCK
por_err
	movs r1,r0
	ldr r0,por_lit0
por_err_cont1
	bl sc_sysError
	ALIGN
por_sciopta
	DCD sciopta
por_lit0
	DCD (0x001<<12)|(0x11<<24)|0x02
por_lit1
	DCD (0x004<<12)|(0x11<<24)|0x02
	IF SC_CDEBUG = 1
por_virgin
	DCD 0xcccccccc
	ENDIF
	IF SC_MSG_CHECK = 1
por_endmark
	DCD 0xeeeeeeee
	ENDIF
	SC_ENDFUNC sc_poolReset
;
;********************************************************************
; tick.S
;
; time functions
;
; ID: S04042BS1
; +Date: 2009/08/14 04:43:04 +
; +Revision: 1.39.2.5 +
;********************************************************************
;
;
;**************************************
;
;**************************************
;
;
;**************************************
; __u32 __CODE sc_tickTick2Ms(sc_ticks_t t)
;**************************************
;
	XSC_TFUNC sc_tickTick2Ms,-12
	SC_TFUNC sc_tickTick2Ms
	IF CPUTYPE <= 4
	bx pc
	nop
	CODE32
	ldr r12,tt2m_sciopta
	ldr r2,[r12,#SC_TCKLENGTH-256*8]	; get ticklength
	cmp r2,#0
	beq tt2m_err
	cmp r2,#1000	; tl = 1000 => tick = ms
	bxeq lr	; return
	cmp r0,#0	; t = 0 ?
	bxeq lr	; return
	ldr r12,[r12,#SC_RECI_TCKLENGTH-256*8]	; get 1/ticklenght
	movs r1,#-1
	umull r3,r1,r12,r1	; r1:= limit for mul/div
	cmp r0,r1
	bls tt2m0
	ldr r3,tt2m_by_1000	; r3 = 1/1000
	adds r0,r0,#500	; round up
	subcs r0,r0,#500	; oops, overflow, no round up
	umull r12,r0,r3,r0	; r0 = t/1000
	cmp r0,r1
	movhi r0,#-1
	bxhi lr
	mul r0,r2,r0
	bx lr
tt2m0
	ldr r3,tt2m_by_1000	; r3 = 1/1000
	mul r1,r0,r2	; r1 = t*tl
	adds r1,r1,#500	; round up
	umull r1,r0,r3,r1	; r0 = t*tl/1000
	cmp r0,#0
	moveq r0,#1
	bx lr
tt2m_err
	movs r1,#0
	ldr r0,tt2m_illval
	ldr r3,tt2m_sc_sysError
	bx r3
	ALIGN
tt2m_by_1000
	DCD 0x00418937	; 1/1000*2^32
tt2m_sciopta
	DCD sciopta+256*8
tt2m_illval
	DCD (0x5f<<24)|0x01|(0x030<<12)
tt2m_sc_sysError
	DCD sc_sysError
	ELSE
	ldr r3,=sciopta
	ldr r2,[r3,#SC_TCKLENGTH]	; get ticklength
	cbz r2,tt2m_err0	; tl set , no => error
	ldr r1,=1000
	cmp r1,r2	; tick = ms ?
	it eq
	bxeq lr	; yes, exit
	mov r3,#-1
	ldr r1,=1000
	udiv r3,r3,r2	; r3:= U32_MAX/ticklength
	cmp r0,r3
	bls tt2m0
	udiv r0,r0,r1
	cmp r0,r3
	ite hi
	movhi r0,#-1
	mulls r0,r0,r2
	b tt2m_exit
tt2m0
	muls r0,r2,r0
	udiv r0,r0,r1
tt2m_exit
	cmp r0,#0
	it eq
	moveq r0,#1
	bx lr
tt2m_err0
	ldr r0,=(0x5f<<24)|0x01|(0x030<<12)
	movs r1,#0
	bl sc_sysError
	ENDIF
	SC_ENDFUNC sc_tickTick2Ms
;
;**************************************
; sc_ticks_t __CODE sc_tickMs2Tick(__u32 ms)
;**************************************
;
	XSC_TFUNC sc_tickMs2Tick,-12
	SC_TFUNC sc_tickMs2Tick
	IF CPUTYPE <= 4
	bx pc
	nop
	CODE32
	ldr r3,tm2t_sciopta
	ldr r2,[r3,#SC_RECI_TCKLENGTH-256*8]	; get 1/ticklenght
	ldr r12,[r3,#SC_TCKLENGTH-256*8]	; get ticklength
	cmp r12,#0	; tl set ?
	beq tm2t_err0	; no, error
	cmp r0,#0	; ms = 0 ?
	bxeq lr	; yes, return
	cmp r12,#1000	; tl = 1ms (tick = ms) ?
	bxeq lr	; yes return
	ldr r1,tm2t_limit
	cmp r0,r1	; ms < limit
	bls tm2t1
	adds r0,r0,r12,lsr #1	; round (add ticklength/2)
	umull r0,r3,r2,r0	; r3 = (ms+tl/2)/tl
	cmp r3,r1	; r3 < limit
	movhi r0,#-1	; no, overflow
	bxhi lr
	movs r1,#1000
	mul r0,r3,r1	; r0 := tick
	bx lr
tm2t1
	movs r1,#1000
	mul r3,r0,r1
	adds r3,r3,r12,lsr #1	; round (add ticklength/2)
	umull r1,r0,r3,r2	; r0 = (ms*1000+tl/2)/tl
	cmp r0,#0
	moveq r0,#1
	bx lr
tm2t_err0
	movs r1,#0
	ldr r0,tm2t_illval
	ldr r3,tm2t_sc_sysError
	bx r3
	ALIGN
tm2t_sciopta
	DCD sciopta+256*8
tm2t_illval
	DCD (0x5e<<24)|0x01|(0x030<<12)
tm2t_limit
	DCD 4294967	; 0xffffffff/1000
tm2t_sc_sysError
	DCD sc_sysError
	ELSE
;
; if (ticklenght = 1000) return para	;
; if (para < (U32_MAX/1000)) {
; return para*1000/ticklenght;
; } else {
; para /= ticklength
; if ( para < (U32_MAX/1000) ) return para * 1000
; return -1
; }
	;
	ldr r3,=sciopta
	ldr r2,[r3,#SC_TCKLENGTH]	; get ticklength
	cbz r2,tm2t_err0	; tl set , no => error
	ldr r1,=1000
	cmp r1,r2	; tick = ms ?
	it eq
	bxeq lr	; yes, exit
	ldr r3,=4294967
	cmp r0,r3	; *1000 possible ?
	bls tm2t0	; yes, =>
	udiv r0,r0,r2
	cmp r0,r3	; now, *1000 possible ?
	ite hi
	movhi r0,#-1
	mulls r0,r1,r0
	b tm2t_exit
tm2t0
	muls r0,r1,r0
	udiv r0,r0,r2
tm2t_exit
	cmp r0,#0
	it eq
	moveq r0,#1
	bx lr
tm2t_err0
	movs r1,#0
	ldr r0,=(0x5e<<24)|0x01|(0x030<<12)
	bl sc_sysError
	ENDIF
	SC_ENDFUNC sc_tickMs2Tick
;
;**************************************
; __u32 __CODE sc_tickLength(__u32 tl)
;**************************************
;
	XSC_TFUNC sc_tickLength,-12
	SC_TFUNC sc_tickLength
	IF CPUTYPE <= 4
	bx pc
	nop
	CODE32
	ldr r3,tl_SC_TCKLENGTH
	movs r1,r0	; save tl and set flags
	ldr r0,[r3,#0]	; get old value
	bxeq lr	; tl = 0 => return
	str r1,[r3,#0]	; store new value
	; compute 1/ticklength
	movs r2,r1
	cmp r2,#0x00010000
	movlo r2,r2,lsl #16
	cmp r2,#0x01000000
	movlo r2,r2,lsl #8
	cmp r2,#0x10000000
	movlo r2,r2,lsl #4
	cmp r2,#0x40000000
	movlo r2,r2,lsl #2
	cmp r2,#0x80000000
	movlo r2,r2,lsl #1
	movs r3,#1
	rsc r0,r2,#0
	movs r2,r2,lsr #1
tl1
	cmp r0,r2
	adc r3,r3,r3
	subcs r0,r0,r2
	cmp r1,r2
	movne r2,r2,lsr #1
	bne tl1
	ldr r1,tl_sciopta
	cmp r0,#0
	addne r3,r3,#1
	str r3,[r1,#SC_RECI_TCKLENGTH]
	bx lr
	ALIGN
tl_sciopta
	DCD sciopta
tl_SC_TCKLENGTH
	DCD sciopta+SC_TCKLENGTH
	ELSE
	ldr r1,=sciopta
	ldr r2,[r1,#SC_TCKLENGTH]
	cbz r0,tl_get
	str r0,[r1,#SC_TCKLENGTH]
tl_get
	movs r0,r2
	bx lr
	ENDIF
	SC_ENDFUNC sc_tickLength
;
;**************************************
; sc_ticks_t __CODE sc_tickGet(void)
;**************************************
;
	XSC_TFUNC sc_tickGet,-16
	SC_TFUNC sc_tickGet
	ldr r3,tg_sciopta
	ldr r0,[r3,#SC_TCK]
	bx lr
	nop
	ALIGN
tg_sciopta
	DCD sciopta
	SC_ENDFUNC sc_tickGet
;
;**************************************
; void sc_tick(void
;**************************************
;
	XSC_TFUNC sc_tick,-16
	SC_TFUNC sc_tickx
	IF CPUTYPE <= 4
	IF SC_STACK_CHECK = 1
tick_err
	ldr r0,tick_overrun
tick_err_cont
	movs r1,r4
	bl sc_sysError
tick_err2
	ldr r0,tick_underrun
	b tick_err_cont
	ENDIF
	SC_TPUBLIC sc_tick
	mov r0,r8
	mov r1,r9
	mov r2,r10
	mov r3,r11
	push {r0-r7,lr}
	ldr r7,tick_sciopta
	ldr r5,[r7,#SC_TCK]	; incr. system-time
	adds r5,r5,#1
	str r5,[r7,#SC_TCK]
	IF SC_STACK_CHECK = 1
	ldr r0,[r7,#SC_CURPRIOPCB]	; r0:= pcb of current prio proc
	ldr r1,tick_stackmagic	; r1:= 0x17171717
	ldr r2,[r0,#PCB_STACKBTM]
	ldr r3,[r0,#PCB_STACKTOP]
	ldr r4,[r0,#PCB_STACKPTR]
	ldr r0,[r2,#0]	; r0:= last word of stack
	cmp r0,r1	; correct ?
	bne tick_err
	ldr r0,[r3,#0]	; r0:= first word of stack
	cmp r0,r1
	bne tick_err
	cmp r4,r2	; SP < end of stack
	blt tick_err	; yes, overflow
	cmp r4,r3	; SP > start of stack
	bgt tick_err2
	ENDIF
	ldr r0,[r7,#SC_CURPCB]
	ldr r1,[r7,#SC_CURMODULE]
	push {r0,r1}
	movs r2,#0
	mov r11,r2	; reset schedule-flag
	movs r0,#64
	lsls r0,r0,#3
	mov r10,r0	; prepare increment
	movs r0,#SC_SLEEPERLISTS-255
	adds r0,r0,#255
	adds r0,r0,r7
	mov r9,r0	; r9 := sleeper-lists head
	movs r6,#4
tick0
	movs r0,#0x3f
	ands r0,r0,r5	; mask least 6 bit of current time
	lsls r0,r0,#3	; 8byte offset
	add r0,r9	; r0 := &sleeplists[tick & 0x3f]
tick1
	LOCK
	ldr r1,[r0,#dbl_next]	; r1 := head
	cmp r0,r1	; head = tail ?
	beq tick9	; yes, list empty =>
	ldr r2,[r1,#dbl_next] 
	ldr r3,[r1,#dbl_prev] 
	str r2,[r3,#dbl_next] 
	str r3,[r2,#dbl_prev]	; remove PCB from list head
	subs r1,r1,#PCB_SLEEPERS	; r1 := PCB
	ldr r2,[r1,#PCB_TMO]	; r2 := remaining TMO
	cmp r2,#0	; = 0 ?
	bne tick6	; no => re-enter tmo
	ldrh r2,[r1,#PCB_STATE_H]	; r2 := process state
	subs r2,r2,#ST_WAIT_TMO	; clear ST_WAIT_TMO state
	ldrh r3,[r1,#PCB_FLAGS_H]	; get flags
	lsrs r3,r3,#PCB_FLG_IS_TIMER_POS+1	; and check if timer
	bcs tick5	; carry = 1 => handle special
	adds r2,r2,#ST_TMO	; set ST_TMO flag
	strh r2,[r1,#PCB_STATE_H]	; and write new state
	lsrs r4,r2,#ST_WAIT_MSGALLOC_POS+1	; was waiting for msgAlloc ?
	bcc tick3	; carry = 0 => no
	adds r1,r1,#PCB_PCBLIST	; else, remove from waiter list
	ldr r3,[r1,#dbl_next] 
	ldr r4,[r1,#dbl_prev] 
	str r3,[r4,#dbl_next] 
	str r4,[r3,#dbl_prev]
	subs r1,r1,#PCB_PCBLIST
tick3
	lsrs r2,r2,#ST_STOPPED_POS+1	; process stopped ?
	bne tick4	; yes, continue with next process
	ldrh r2,[r1,#PCB_FLAGS_H] 
	movs r3,#PCB_FLG_READY 
	tst r2,r3 
	bne tick1mr9999 
	orrs r2,r2,r3 
	strh r2,[r1,#PCB_FLAGS_H] 
	ldr r4,[r7,#SC_PRIOMASK] 
	ldr r3,[r1,#PCB_PRIO_MASK] 
	ldr r2,[r1,#PCB_READYLIST] 
	orrs r4,r4,r3 
	str r4,[r7,#SC_PRIOMASK] 
	adds r1,r1,#PCB_PCBLIST 
	str r2,[r1,#dbl_next] 
	ldr r3,[r2,#dbl_prev] 
	str r1,[r2,#dbl_prev] 
	str r3,[r1,#dbl_prev] 
	str r1,[r3,#dbl_next] 
	subs r1,r1,#PCB_PCBLIST 
tick1mr9999	; else insert into ready-list
	ldr r2,[r7,#SC_CURPRIOPCB]	; and check if newly ready process
	ldrb r1,[r1,#PCB_PRIO_B]	; has a higher priority than the
	ldrb r2,[r2,#PCB_PRIO_B]	; the currently running prio-proc
	cmp r1,r2
	bhs tick4
	mov r11,r2	; set schedule-flag (r2 != 0 !!)
tick4
	UNLOCK
	b tick1
tick9
	UNLOCK
	lsls r0,r5,#26	; check for overflow: (tick & 3f) = 0
	bne tick10	; no, finish
	lsrs r5,r5,#6	; next chunk
	subs r6,r6,#1
	beq tick10	; empty => finish
	add r9,r10	; r9 := next sleeperlists
	b tick0
	;
	; timer
	;
	ALIGN
tick5
	bx pc
	nop
	SC_PUBLIC tick5_a
	strh r2,[r1,#PCB_STATE_H]	; set new state
	ldr r3,[r1,#PCB_PID]	; set personality
	str r1,[r7,#SC_CURPCB]
	str r3,[r7,#SC_CURPID]
	; set timer's module
	ldr r2,tick_sc_modules
	lsrs r3,r3,#24
	ldr r2,[r2,r3,lsl #2]
	str r2,[r7,#SC_CURMODULE]
	IF SC_PROC_HOOK = 1
	IF SC_PROCMMU_HOOK = 1
	movs r2,#3
	adds r3,r3,r3
	movs r2,r2,lsl r3
	movne r3,#0x40000001	; client in global and system domain
	ldreq r3,tick_55555555	; client in all domains
	orr r2,r2,r3
	mrc p15,0,r3,c3,c0,0	; save old domains
	mcr p15,0,r2,c3,c0,0	; set new
	ENDIF
	ENDIF
	bl cpu_unlock_r12_a	; enable interrupts
	mov r2,sp	; save current stackpointer
	ldr sp,[r1,#PCB_STACKTOP]	; and load timer's
	stmfd sp!,{r0-r3}	; save registers
	movs r0,#0
	ldr r2,[r1,#PCB_ENTRY]
	BLX_R2_a	; call timer
	ldmfd sp!,{r0-r3}
	mov sp,r2	; restore tick's stackpointer
	bl cpu_lock_r12_a	; disable interrupts again
	IF SC_PROC_HOOK = 1
	IF SC_PROCMMU_HOOK = 1
	mcr p15,0,r3,c3,c0,0	; restore old
	ENDIF
	ENDIF
		; return to thumb-mode
	add r3,pc,#1
	bx r3
	SC_TPUBLIC tick_t
	ldr r2,[r1,#PCB_SLICE]	; get timeslice
	ldrh r3,[r1,#PCB_FLAGS_H]	; get timer's state
	lsrs r3,r3,#PCB_FLG_READY_POS+1	; still ready
	bcc tick7	; carry = 0 => no, continue
tick6
	ldr r3,[r7,#SC_TCK] 
	adds r3,r3,r2 
	lsrs r4,r2,#18 
	bne tick5992 
	lsrs r4,r2,#12 
	bne tick5991 
	lsrs r4,r2,#6 
	bne tick5990 
	movs r4,#0x3f 
	ands r4,r4,r3 
	movs r2,#0 
	b tick5996 
tick5990 
	movs r2,#0x3f 
	ands r2,r2,r3 
	lsrs r3,r3,#6 
	movs r4,#0x3f 
	ands r4,r4,r3 
	adds r4,r4,#64 
	b tick5996 
tick5991 
	lsls r2,r3,#20 
	lsrs r2,r2,#20 
	lsrs r3,r3,#12 
	movs r4,#0x3f 
	ands r4,r4,r3 
	adds r4,r4,#128 
	b tick5996 
tick5992 
	lsls r2,r3,#14 
	lsrs r2,r2,#14 
	lsrs r3,r3,#18 
	movs r4,#0x3f 
	ands r4,r4,r3 
	adds r4,r4,#192 
tick5996 
	lsls r4,r4,#3 
	str r2,[r1,#PCB_TMO] 
	adds r4,r4,#SC_SLEEPERLISTS-255 
	adds r4,r4,#255 
	movs r3,r1 
	adds r3,r3,#PCB_SLEEPERS 
	adds r4,r4,r7 
	str r4,[r3,#dbl_next] 
	ldr r2,[r4,#dbl_prev] 
	str r3,[r4,#dbl_prev] 
	str r2,[r3,#dbl_prev] 
	str r3,[r2,#dbl_next]	; enter process in timerlist
	ldrh r3,[r1,#PCB_STATE_H]	; and set state
	movs r2,#ST_WAIT_TMO
	orrs r3,r3,r2
	strh r3,[r1,#PCB_STATE_H]
tick7
	UNLOCK
	b tick1	; continue with next process
tick10
	pop {r0,r1}	; restore CURPCB,PID and MODULE
	LOCK
	ldr r2,[r0,#PCB_PID]
	str r1,[r7,#SC_CURMODULE]
	str r0,[r7,#SC_CURPCB]
	str r2,[r7,#SC_CURPID]
	UNLOCK
	;
	; handle prio-proc time slice
	;
	ldr r0,[r7,#SC_CURPRIOPCB]	; r0 := current prio proc
	ldr r3,[r0,#PCB_SLICE]	; r3 := time-slice (reload)
	ldr r1,[r0,#PCB_CURSLICE]	; r1 := time-slice (current value)
	cmp r3,#0	; reload = 0 => no slice
	beq tick20
	subs r1,r1,#1	; reduce slice
	bne tick19	; != 0 => not expired
	movs r1,r3	; get reload
	LOCK
	ldr r2,[r0,#PCB_PCBLIST+dbl_prev]	; r2 := list head
	ldr r3,[r0,#PCB_PCBLIST+dbl_next]	; r3 := list tail
	cmp r2,r3	; head = tail (only one in list) ?
	beq tick18	; yes, no re-scheduling
	str r3,[r2,#dbl_next]	; remove head from readylist
	str r2,[r3,#dbl_prev]
	movs r4,r0	; and insert at end
	adds r4,r4,#PCB_PCBLIST
	str r2,[r4,#dbl_next] 
	ldr r3,[r2,#dbl_prev] 
	str r4,[r2,#dbl_prev] 
	str r3,[r4,#dbl_prev] 
	str r4,[r3,#dbl_next]
	mov r11,r2	; request rescheduling (r2 != 0)
tick18
	UNLOCK
tick19
	str r1,[r0,#PCB_CURSLICE]	; set new time-slice
tick20
	IF SC_ASYNC_TMO = 1
	;
	; asynchronous timeouts
	;
	ldr r5,[r7,#SC_TCK]	; r5 := current system time
	ldr r0,tick_SC_TMOLISTS
	mov r9,r0	; r9 := start of tmo-lists
	movs r6,#4
tick21
	movs r0,#0x3f
	ands r0,r0,r5	; mask least 6 bits
	lsls r0,r0,#3	; 8byte offset
	add r0,r9	; r0 := &tmolists[tick & 0x3f]
	b tick21a
tick25
	ldr r3,[r7,#SC_TCK] 
	adds r3,r3,r2 
	lsrs r4,r2,#18 
	bne tick25992 
	lsrs r4,r2,#12 
	bne tick25991 
	lsrs r4,r2,#6 
	bne tick25990 
	movs r4,#0x3f 
	ands r4,r4,r3 
	movs r2,#0 
	b tick25996 
tick25990 
	movs r2,#0x3f 
	ands r2,r2,r3 
	lsrs r3,r3,#6 
	movs r4,#0x3f 
	ands r4,r4,r3 
	adds r4,r4,#64 
	b tick25996 
tick25991 
	lsls r2,r3,#20 
	lsrs r2,r2,#20 
	lsrs r3,r3,#12 
	movs r4,#0x3f 
	ands r4,r4,r3 
	adds r4,r4,#128 
	b tick25996 
tick25992 
	lsls r2,r3,#14 
	lsrs r2,r2,#14 
	lsrs r3,r3,#18 
	movs r4,#0x3f 
	ands r4,r4,r3 
	adds r4,r4,#192 
tick25996 
	ldr r3,tick_SC_TMOLISTS 
	lsls r4,r4,#3 
	str r2,[r1,#16] 
	adds r4,r4,r3 
	adds r1,r1,#24 
	str r4,[r1,#dbl_next] 
	ldr r3,[r4,#dbl_prev] 
	str r1,[r4,#dbl_prev] 
	str r3,[r1,#dbl_prev] 
	str r1,[r3,#dbl_next] 
	subs r1,r1,#24
tick21a
	ldr r1,[r0,#dbl_next]	; head
	cmp r0,r1	; = tail ?
	beq tick29	; yes => list empty
	ldr r2,[r1,#dbl_next] 
	ldr r3,[r1,#dbl_prev] 
	str r2,[r3,#dbl_next] 
	str r3,[r2,#dbl_prev]	; remove from tmo-list
	subs r1,r1,#24	; r1 := msgheader
	ldr r2,[r1,#16]	; r2 := remaining tmo
	cmp r2,#0	; = 0 ?
	bne tick25	; no, re-enter
	mov r8,r5	; save current tick
	ldr r2,[r1,#12]	; remove TMO_MAGIC from PLID
	movs r3,#255
	lsls r3,r3,#24
	adds r3,r3,#255
	ands r2,r2,r3
	str r2,[r1,#12]
	ldr r2,[r1,#8]	; restore msg-size and index
	lsrs r3,r2,#28	; r3 := bufsize-index
	lsls r3,r3,#3	; *8
	lsls r2,r2,#4	; remove bufsize-index from msg-size
	lsrs r2,r2,#4
	str r3,[r1,#16]	; restore bufsize-idx
	str r2,[r1,#8]	; restore-msg size
	movs r2,#0
	str r2,[r1,#28]	; clear owner
	ldr r2,[r1,#20]	; get sender (tmo requestor)
	str r2,[r1,#24]	; and set as addressee
	lsrs r3,r2,#24	; r3 := addressee's module index
	lsls r3,r3,#2	; long offset
	ldr r4,tick_sc_modules
	ldr r3,[r4,r3]	; r3 := addressee's MCB
	lsls r2,r2,#18
	lsrs r2,r2,#16	; r2 := PCB index
	ldr r4,[r3,#MODULE_PCBS]
	ldr r2,[r4,r2]	; get addressee's PCB
	ldr r4,[r1,#dbl_next] 
	ldr r5,[r1,#dbl_prev] 
	str r4,[r5,#dbl_next] 
	str r5,[r4,#dbl_prev]	; remove msg from alloc-queue
	ldr r4,[r2,#PCB_NALLOC]	; update stats
	subs r4,r4,#1
	str r4,[r2,#PCB_NALLOC]
	IF SC_MSG_HOOK = 1
	IF SC_MSGTX_HOOK = 1
	push {r0-r2}
	ldr r2,[r3,#MODULE_MSGTXHOOK]
	cmp r2,#0
	beq tick22
	movs r0,r1
	BLX_R2_t
tick22
	ldr r2,tick_sc_msgtxhook
	ldr r2,[r2,#0]
	cmp r2,#0
	beq tick23
	ldr r0,[sp,#4]
	BLX_R2_t
tick23
	pop {r0-r2}
	ENDIF
	ENDIF
	mov r5,r8	; restore tick
	movs r3,r2	; now insert into mqueue
	adds r3,r3,#PCB_MSGQUEUE
	LOCK
	str r3,[r1,#dbl_next] 
	ldr r4,[r3,#dbl_prev] 
	str r1,[r3,#dbl_prev] 
	str r4,[r1,#dbl_prev] 
	str r1,[r4,#dbl_next]
	ldr r4,[r2,#PCB_NQUEUE]	; and update stats
	adds r4,r4,#1
	str r4,[r2,#PCB_NQUEUE]
	ldrh r3,[r2,#PCB_STATE_H]	; get process-state
	movs r4,#ST_WAIT_MSGRX
	tst r3,r4
	beq tick26	; not waiting for msg => continue
	bics r3,r3,r4	; clear state
	strh r3,[r2,#PCB_STATE_H]	; save new state
	lsrs r3,r3,#ST_STOPPED_POS+1	; stopped
	bne tick26	; => yes, continue
	ldrh r1,[r2,#PCB_FLAGS_H] 
	movs r3,#PCB_FLG_READY 
	tst r1,r3 
	bne tick24mr9999 
	orrs r1,r1,r3 
	strh r1,[r2,#PCB_FLAGS_H] 
	ldr r4,[r7,#SC_PRIOMASK] 
	ldr r3,[r2,#PCB_PRIO_MASK] 
	ldr r1,[r2,#PCB_READYLIST] 
	orrs r4,r4,r3 
	str r4,[r7,#SC_PRIOMASK] 
	adds r2,r2,#PCB_PCBLIST 
	str r1,[r2,#dbl_next] 
	ldr r3,[r1,#dbl_prev] 
	str r2,[r1,#dbl_prev] 
	str r3,[r2,#dbl_prev] 
	str r2,[r3,#dbl_next] 
	subs r2,r2,#PCB_PCBLIST 
tick24mr9999	; insert into readylist
	ldr r4,[r7,#SC_CURPRIOPCB]	; and check if now ready
	ldrb r3,[r2,#PCB_PRIO_B]	; process has a higher priority
	ldrb r4,[r4,#PCB_PRIO_B]	; than the current running.
	cmp r3,r4
	bhs tick26
	mov r11,r4	; set schedule-flag (r4 != 0 !!!)
tick26
	UNLOCK
	b tick21a
tick29
	lsls r0,r5,#26	; test for overflow (tick & 3f = 0)
	bne tick30	; != 0 => finish
	lsrs r5,r5,#6	; get next 6 bits
	subs r6,r6,#1
	beq tick30
	add r9,r10	; r9 += 64*8
	b tick21
tick30
	ENDIF	; SC_ASYNC_TMO
tick99
	LOCK
	ldr r0,[r7,#SC_SCHEDULE_REQ]	; get schedule req set by timers
	mov r1,r11
	orrs r0,r0,r1	; merge it with ours
	str r0,[r7,#SC_SCHEDULE_REQ]	; and set
	UNLOCK
	pop {r0-r7}	; restore registers
	mov r8,r0
	mov r9,r1
	mov r10,r2
	mov r11,r3
	pop {r3}
	bx r3
	ALIGN
	IF SC_PROC_HOOK = 1
	IF SC_PROCMMU_HOOK = 1
tick_55555555
	DCD 0x55555555
	ENDIF
	ENDIF
	IF SC_STACK_CHECK = 1
tick_stackmagic
	DCD 0x17171717
tick_overrun
	DCD (0x57<<24)|(0x02e<<12)|0x02
tick_underrun
	DCD (0x57<<24)|(0x02f<<12)|0x02
	ENDIF
	IF SC_PROC_PARA_CHECK = 1
tick_ill_proc
	DCD (0x57<<24)|(0x024<<12)|0x01
	ENDIF
tick_sciopta
	DCD sciopta
tick_sc_modules
	DCD sc_modules
tick_SC_TMOLISTS
	DCD sciopta+SC_TMOLISTS
	IF SC_MSG_HOOK = 1
	IF SC_MSGTX_HOOK = 1
tick_sc_msgtxhook
	DCD sciopta+SC_MSGTXHOOK
	ENDIF
	ENDIF
	ELSE	; IF CPUTYPE <= 4
	; ************************************************************************
	; CM3
	IF SC_STACK_CHECK = 1
tick_err
	ldr r0,tick_overrun
tick_err_cont
	movs r1,r4
	bl sc_sysError
tick_err2
	ldr r0,tick_underrun
	b tick_err_cont
	ENDIF
	SC_TPUBLIC sc_tick
	push.w {r4-r11,lr}
	ldr r7,tick_sciopta
	ldr r5,[r7,#SC_TCK]	; incr. system-time
	adds r5,r5,#1
	str r5,[r7,#SC_TCK]
	IF SC_STACK_CHECK = 1
	movw r1,#0x1717
	movt r1,#0x1717
	ldr r0,[r7,#SC_CURPRIOPCB]	; r0:= pcb of current prio proc
	ldr r2,[r0,#PCB_STACKBTM]
	ldr r3,[r0,#PCB_STACKTOP]
	ldr r4,[r0,#PCB_STACKPTR]
	ldr r0,[r2,#0]	; r0:= last word of stack
	cmp r0,r1	; correct ?
	bne tick_err
	ldr r0,[r3,#0]	; r0:= first word of stack
	cmp r0,r1
	bne tick_err
	cmp r4,r2	; SP < end of stack
	blt tick_err	; yes, overflow
	cmp r4,r3	; SP > start of stack
	bgt tick_err2
	ENDIF
	ldr r0,[r7,#SC_CURPCB]
	ldr r1,[r7,#SC_CURMODULE]
	push {r0,r1}
	mov r11,#0	; reset schedule-flag
	mov r10,#64*8	; prepare increment
	addw r9,r7,#SC_SLEEPERLISTS	; r9 := sleeper-lists head
	mov r8,r5	; r8 := tick
	movs r6,#4
	b tick0	; enter loop
tick4
	UNLOCK
tick1
	LOCK
	ldr r1,[r0,#dbl_next]	; r1 := head
	cmp r0,r1	; head = tail ?
	beq tick9	; yes, list empty =>
	ldr r2,[r1,#dbl_next] 
	ldr r3,[r1,#dbl_prev] 
	str r2,[r3,#dbl_next] 
	str r3,[r2,#dbl_prev]	; remove PCB from list head
	subs r1,r1,#PCB_SLEEPERS	; r1 := PCB
	ldr r2,[r1,#PCB_TMO]	; r2 := remaining TMO
	cmp r2,#0
	bne tick6	; != 0 => re-enter tmo
	ldrh r2,[r1,#PCB_STATE_H]	; r4 := process state
	subs r2,r2,#ST_WAIT_TMO	; r4 := clear ST_WAIT_TMO state
	ldrh r3,[r1,#PCB_FLAGS_H]	; r3:= flags
	lsrs r3,r3,#PCB_FLG_IS_TIMER_POS+1	; and check if timer
	bcs tick5	; carry = 1 => handle special
	adds r2,r2,#ST_TMO	; set ST_TMO flag
	strh r2,[r1,#PCB_STATE_H]	; and write new state
	lsrs r4,r2,#ST_WAIT_MSGALLOC_POS+1	; was waiting for msgAlloc ?
	bcc tick3	; carry = 0 => no
	adds r5,r1,#PCB_PCBLIST	; else, remove from waiter list
	ldr r3,[r5,#dbl_next] 
	ldr r4,[r5,#dbl_prev] 
	str r3,[r4,#dbl_next] 
	str r4,[r3,#dbl_prev]
tick3
	lsrs r2,r2,#ST_STOPPED_POS+1	; process stopped ?
	bcs tick4	; yes, continue with next process
	ldrh r2,[r1,#PCB_FLAGS_H] 
	movs r3,#PCB_FLG_READY 
	tst r2,r3 
	bne tick1mr9999 
	orrs r2,r2,r3 
	strh r2,[r1,#PCB_FLAGS_H] 
	ldr r4,[r7,#SC_PRIOMASK] 
	ldr r3,[r1,#PCB_PRIO_MASK] 
	ldr r2,[r1,#PCB_READYLIST] 
	orrs r4,r4,r3 
	str r4,[r7,#SC_PRIOMASK] 
	adds r1,r1,#PCB_PCBLIST 
	str r2,[r1,#dbl_next] 
	ldr r3,[r2,#dbl_prev] 
	str r1,[r2,#dbl_prev] 
	str r3,[r1,#dbl_prev] 
	str r1,[r3,#dbl_next] 
	subs r1,r1,#PCB_PCBLIST 
tick1mr9999	; else insert into ready-list
	ldr r2,[r7,#SC_CURPRIOPCB]	; and check if newly ready process
	ldrb r1,[r1,#PCB_PRIO_B]	; has a higher priority than the
	ldrb r2,[r2,#PCB_PRIO_B]	; the currently running prio-proc
	cmp r1,r2
	bhs tick4
	mov r11,r2	; set schedule-flag (r2 != 0 !!)
	b tick4
tick9
	UNLOCK
	lsls r0,r8,#26	; check for overflow: (tick & 3f) = 0
	bne tick10	; no, finish
	lsrs r8,r8,#6	; next chunk
	subs r6,r6,#1
	beq tick10	; empty => finish
	add r9,r10	; r9 := next sleeperlists
tick0
	and r0,r8,#0x3f	; mask least 6 bit of current time
	add r0,r9,r0,lsl #3	; r0 := &sleeplists[tick & 0x3f]
	b tick1
	;
	; timer
	;
	ALIGN
tick5
	strh r2,[r1,#PCB_STATE_H]	; set new state
	ldr r3,[r1,#PCB_PID]	; set personality
	str r1,[r7,#SC_CURPCB]
	str r3,[r7,#SC_CURPID]
	; set timer's module
	ldr r2,tick_sc_modules
	lsrs r3,r3,#24
	ldr r2,[r2,r3,lsl #2]
	str r2,[r7,#SC_CURMODULE]
	IF SC_PROC_HOOK = 1
	IF SC_PROCMMU_HOOK = 1
	ENDIF
	ENDIF
	mrs r2,msp	; save current stackpointer
	ldr r12,[r1,#PCB_STACKTOP]	; and load timer's
	msr msp,r12
	cpsie i
	push {r0-r3}	; save registers
	movs r0,#0
	ldr r2,[r1,#PCB_ENTRY]
	BLX_R2_a	; call timer
	cpsid i
	pop {r0-r3}
	msr msp,r2	; restore tick's stackpointer
	IF SC_PROC_HOOK = 1
	IF SC_PROCMMU_HOOK = 1
	ENDIF
	ENDIF
	ldr r2,[r1,#PCB_SLICE]	; get timeslice
	ldrh r3,[r1,#PCB_FLAGS_H]	; get timer's state
	lsrs r3,r3,#PCB_FLG_READY_POS+1	; still ready
	bcc tick4	; carry = 0 => no, continue
tick6
	ldr r3,[r7,#SC_TCK] 
	adds r3,r3,r2 
	lsrs r4,r2,#18 
	bne tick5992 
	lsrs r4,r2,#12 
	bne tick5991 
	lsrs r4,r2,#6 
	bne tick5990 
	and r4,r3,#0x3f 
	movs r2,#0 
	b tick5996 
tick5990 
	and r2,r3,#0x3f 
	ubfx r4,r3,#6,#6 
	adds r4,r4,#64 
	b tick5996 
tick5991 
	ubfx r2,r3,#0,#12 
	ubfx r4,r3,#12,#6 
	adds r4,r4,#128 
	b tick5996 
tick5992 
	ubfx r2,r3,#0,#18 
	ubfx r4,r3,#18,#6 
	adds r4,r4,#192 
tick5996 
	str r2,[r1,#PCB_TMO] 
	add r4,r7,r4,lsl #3 
	add r4,r4,#SC_SLEEPERLISTS 
	adds r3,r1,#PCB_SLEEPERS 
	str r4,[r3,#dbl_next] 
	ldr r2,[r4,#dbl_prev] 
	str r3,[r4,#dbl_prev] 
	str r2,[r3,#dbl_prev] 
	str r3,[r2,#dbl_next]	; enter process in timerlist
	ldrh r3,[r1,#PCB_STATE_H]	; and set state
	orr r3,r3,#ST_WAIT_TMO
	strh r3,[r1,#PCB_STATE_H]
	b tick4
tick10
	pop {r0,r1}	; restore CURPCB,PID and MODULE
	LOCK
	ldr r2,[r0,#PCB_PID]
	str r1,[r7,#SC_CURMODULE]
	str r0,[r7,#SC_CURPCB]
	str r2,[r7,#SC_CURPID]
	UNLOCK
	;
	; handle prio-proc time slice
	;
	ldr r0,[r7,#SC_CURPRIOPCB]	; r0 := current prio proc
	ldr r3,[r0,#PCB_SLICE]	; r3 := time-slice (reload)
	ldr r1,[r0,#PCB_CURSLICE]	; r1 := time-slice (current value)
	cmp r3,#0	; reload = 0 => no slice
	beq tick20
	subs r1,r1,#1	; reduce slice
	bne tick19	; != 0 => not expired
	movs r1,r3	; get reload
	LOCK
	ldr r2,[r0,#PCB_PCBLIST+dbl_prev]	; r2 := list head
	ldr r3,[r0,#PCB_PCBLIST+dbl_next]	; r3 := list tail
	cmp r2,r3	; head = tail (only one in list) ?
	beq tick18	; yes, no re-scheduling
	str r3,[r2,#dbl_next]	; remove head from readylist
	str r2,[r3,#dbl_prev]
	movs r4,r0	; and insert at end
	adds r4,r4,#PCB_PCBLIST
	str r2,[r4,#dbl_next] 
	ldr r3,[r2,#dbl_prev] 
	str r4,[r2,#dbl_prev] 
	str r3,[r4,#dbl_prev] 
	str r4,[r3,#dbl_next]
	mov r11,r2	; request rescheduling (r2 != 0)
tick18
	UNLOCK
tick19
	str r1,[r0,#PCB_CURSLICE]	; set new time-slice
tick20
	IF SC_ASYNC_TMO = 1
	;
	; asynchronous timeouts
	;
	ldr r8,[r7,#SC_TCK]	; r8 := current system time
	add r9,r7,#SC_TMOLISTS	; r9 := start of tmo-lists
	movs r6,#4
	b tick29
tick21
	ldr r3,[r7,#SC_TCK] 
	adds r3,r3,r2 
	lsrs r4,r2,#18 
	bne tick21992 
	lsrs r4,r2,#12 
	bne tick21991 
	lsrs r4,r2,#6 
	bne tick21990 
	movs r4,#0x3f 
	ands r4,r4,r3 
	movs r2,#0 
	b tick21996 
tick21990 
	movs r2,#0x3f 
	ands r2,r2,r3 
	lsrs r3,r3,#6 
	movs r4,#0x3f 
	ands r4,r4,r3 
	adds r4,r4,#64 
	b tick21996 
tick21991 
	lsls r2,r3,#20 
	lsrs r2,r2,#20 
	lsrs r3,r3,#12 
	movs r4,#0x3f 
	ands r4,r4,r3 
	adds r4,r4,#128 
	b tick21996 
tick21992 
	lsls r2,r3,#14 
	lsrs r2,r2,#14 
	lsrs r3,r3,#18 
	movs r4,#0x3f 
	ands r4,r4,r3 
	adds r4,r4,#192 
tick21996 
	add r3,r7,#SC_TMOLISTS 
	str r2,[r1,#16] 
	add r4,r3,r4,lsl #3 
	adds r2,r1,#24 
	str r4,[r2,#dbl_next] 
	ldr r3,[r4,#dbl_prev] 
	str r2,[r4,#dbl_prev] 
	str r3,[r2,#dbl_prev] 
	str r2,[r3,#dbl_next]
tick22
	ldr r1,[r0,#dbl_next]	; head
	cmp r0,r1	; = tail ?
	beq tick28	; yes => list empty
	ldr r2,[r1,#dbl_next] 
	ldr r3,[r1,#dbl_prev] 
	str r2,[r3,#dbl_next] 
	str r3,[r2,#dbl_prev]	; remove from tmo-list
	subs r1,r1,#24	; r1 := msgheader
	ldr r2,[r1,#16]	; r2 := remaining tmo
	cmp r2,#0	; = 0 ?
	bne tick21	; no, re-enter
	ldr r2,[r1,#12]	; remove TMO_MAGIC from PLID
	bfc r2,#8,#16	; r2 &= 0xff0000ff
	str r2,[r1,#12]
	ldr r2,[r1,#8]	; restore msg-size and index
	lsrs r3,r2,#28	; r3 := bufsize-index
	lsls r3,r3,#3	; *8
	bic r2,r2,#0xf0000000	; remove bufsize-index from msg-size
	str r3,[r1,#16]	; restore bufsize-idx
	str r2,[r1,#8]	; restore-msg size
	movs r2,#0
	str r2,[r1,#28]	; clear owner
	ldr r2,[r1,#20]	; get sender (tmo requestor)
	str r2,[r1,#24]	; and set as addressee
	lsrs r3,r2,#24	; r3 := addressee's module index
	ldr r4,tick_sc_modules
	ldr r3,[r4,r3,lsl #2]	; r3 := addressee's MCB
	bfc r2,#14,#18	; r2 := PCB index
	ldr r4,[r3,#MODULE_PCBS]
	ldr r2,[r4,r2,lsl #2]	; get addressee's PCB
	ldr r4,[r1,#dbl_next] 
	ldr r5,[r1,#dbl_prev] 
	str r4,[r5,#dbl_next] 
	str r5,[r4,#dbl_prev]	; remove msg from alloc-queue
	ldr r4,[r2,#PCB_NALLOC]	; update stats
	subs r4,r4,#1
	str r4,[r2,#PCB_NALLOC]
	IF SC_MSG_HOOK = 1
	IF SC_MSGTX_HOOK = 1
	push {r0-r2}
	ldr r2,[r3,#MODULE_MSGTXHOOK]
	cmp r2,#0
	beq tick24
	movs r0,r1
	BLX_R2_t
tick24
	ldr r2,tick_sc_msgtxhook
	ldr r2,[r2,#0]
	cmp r2,#0
	beq tick25
	ldr r0,[sp,#4]
	BLX_R2_t
tick25
	pop {r0-r2}
	ENDIF
	ENDIF
	adds r3,r2,#PCB_MSGQUEUE	; now insert into mqueue
	LOCK
	str r3,[r1,#dbl_next] 
	ldr r4,[r3,#dbl_prev] 
	str r1,[r3,#dbl_prev] 
	str r4,[r1,#dbl_prev] 
	str r1,[r4,#dbl_next]
	ldr r4,[r2,#PCB_NQUEUE]	; and update stats
	adds r4,r4,#1
	str r4,[r2,#PCB_NQUEUE]
	ldrh r3,[r2,#PCB_STATE_H]	; get process-state
	movs r4,#ST_WAIT_MSGRX
	tst r3,r4
	beq tick27	; not waiting for msg => continue
	bics r3,r3,r4	; clear state
	strh r3,[r2,#PCB_STATE_H]	; save new state
	lsrs r3,r3,#ST_STOPPED_POS+1	; stopped ?
	bcs tick27	; => yes, continue
	ldrh r1,[r2,#PCB_FLAGS_H] 
	movs r3,#PCB_FLG_READY 
	tst r1,r3 
	bne tick24mr9999 
	orrs r1,r1,r3 
	strh r1,[r2,#PCB_FLAGS_H] 
	ldr r4,[r7,#SC_PRIOMASK] 
	ldr r3,[r2,#PCB_PRIO_MASK] 
	ldr r1,[r2,#PCB_READYLIST] 
	orrs r4,r4,r3 
	str r4,[r7,#SC_PRIOMASK] 
	adds r2,r2,#PCB_PCBLIST 
	str r1,[r2,#dbl_next] 
	ldr r3,[r1,#dbl_prev] 
	str r2,[r1,#dbl_prev] 
	str r3,[r2,#dbl_prev] 
	str r2,[r3,#dbl_next] 
	subs r2,r2,#PCB_PCBLIST 
tick24mr9999	; insert into readylist
	ldr r4,[r7,#SC_CURPRIOPCB]	; and check if now ready
	ldrb r3,[r2,#PCB_PRIO_B]	; process has a higher priority
	ldrb r4,[r4,#PCB_PRIO_B]	; than the current running.
	cmp r3,r4
	bhs tick27
	mov r11,r4	; set schedule-flag (r4 != 0 !!!)
tick27
	UNLOCK
	b tick22
tick28
	lsls r0,r8,#26	; test for overflow (tick & 3f = 0)
	bne tick30	; != 0 => finish
	lsrs r8,r8,#6	; get next 6 bits
	subs r6,r6,#1
	beq tick30
	add r9,r10	; r9 += 64*8
tick29
	and r0,r8,#0x3f	; mask least 6 bits
	add r0,r9,r0,lsl #3	; r0 := &tmolists[tick & 0x3f]
	b tick22
tick30
	ENDIF	; SC_ASYNC_TMO
tick99
	LOCK
	ldr r0,[r7,#SC_SCHEDULE_REQ]	; get schedule req set by timers
	orr r0,r0,r11	; merge it with ours
	str r0,[r7,#SC_SCHEDULE_REQ]	; and set
	UNLOCK
	pop.w {r4-r11,pc}	; restore registers
	ALIGN
	IF SC_STACK_CHECK = 1
tick_overrun
	DCD (0x57<<24)|(0x02e<<12)|0x02
tick_underrun
	DCD (0x57<<24)|(0x02f<<12)|0x02
	ENDIF
	IF SC_PROC_PARA_CHECK = 1
tick_ill_proc
	DCD (0x57<<24)|(0x024<<12)|0x01
	ENDIF
tick_sciopta
	DCD sciopta
tick_sc_modules
	DCD sc_modules
	IF SC_MSG_HOOK = 1
	IF SC_MSGTX_HOOK = 1
tick_sc_msgtxhook
	DCD sciopta+SC_MSGTXHOOK
	ENDIF
	ENDIF
	ENDIF
	SC_ENDFUNC sc_tickx
;
;**************************************
; void sc_sleep(sc_ticks_t ticks)
;**************************************
;
	XSC_TFUNC sc_sleep,-12
	SC_TFUNC sc_sleep
	push {r4-r7,lr}	; save working regs
	ldr r7,sleep_sciopta
	ldr r6,[r7,#SC_CURPCB]
	ldrh r1,[r6,#PCB_FLAGS_H]	; test if current process
	lsrs r1,r1,#PCB_FLG_IS_PRIO_POS+1	; is prio
	bcc sleep_err1	; no => error
	cmp r0,#(0)	; tmo = (0)
	beq sleep99	; yes, leave
	lsrs r1,r0,#24	; tmo < SC_MAX_TMO
	bne sleep_err2	; no => error
	LOCK
	ldrh r1,[r6,#PCB_FLAGS_H] 
	movs r2,#PCB_FLG_READY 
	tst r1,r2 
	beq sleep9999 
	bics r1,r1,r2 
	strh r1,[r6,#PCB_FLAGS_H] 
	ldr r1,[r6,#PCB_PCBLIST+dbl_next] 
	ldr r2,[r6,#PCB_PCBLIST+dbl_prev] 
	str r1,[r2,#dbl_next] 
	str r2,[r1,#dbl_prev] 
	cmp r1,r2 
	bne sleep9998 
	ldr r2,[r6,#PCB_PRIO_MASK] 
	ldr r1,[r7,#SC_PRIOMASK] 
	bics r1,r1,r2 
	str r1,[r7,#SC_PRIOMASK] 
sleep9998 
	movs r1,#0 
	str r1,[r6,#PCB_PCBLIST+dbl_next] 
	str r1,[r6,#PCB_PCBLIST+dbl_prev] 
sleep9999	; remove from readylist
	IF CPUTYPE <= 4
	ldr r1,[r7,#SC_TCK] 
	adds r1,r1,r0 
	lsrs r2,r0,#18 
	bne sleep992 
	lsrs r2,r0,#12 
	bne sleep991 
	lsrs r2,r0,#6 
	bne sleep990 
	movs r2,#0x3f 
	ands r2,r2,r1 
	movs r0,#0 
	b sleep996 
sleep990 
	movs r0,#0x3f 
	ands r0,r0,r1 
	lsrs r1,r1,#6 
	movs r2,#0x3f 
	ands r2,r2,r1 
	adds r2,r2,#64 
	b sleep996 
sleep991 
	lsls r0,r1,#20 
	lsrs r0,r0,#20 
	lsrs r1,r1,#12 
	movs r2,#0x3f 
	ands r2,r2,r1 
	adds r2,r2,#128 
	b sleep996 
sleep992 
	lsls r0,r1,#14 
	lsrs r0,r0,#14 
	lsrs r1,r1,#18 
	movs r2,#0x3f 
	ands r2,r2,r1 
	adds r2,r2,#192 
sleep996 
	lsls r2,r2,#3 
	str r0,[r6,#PCB_TMO] 
	adds r2,r2,#SC_SLEEPERLISTS-255 
	adds r2,r2,#255 
	movs r1,r6 
	adds r1,r1,#PCB_SLEEPERS 
	adds r2,r2,r7 
	str r2,[r1,#dbl_next] 
	ldr r0,[r2,#dbl_prev] 
	str r1,[r2,#dbl_prev] 
	str r0,[r1,#dbl_prev] 
	str r1,[r0,#dbl_next]	; and enter into timerlist
	ELSE
	ldr r1,[r7,#SC_TCK] 
	adds r1,r1,r0 
	lsrs r2,r0,#18 
	bne sleep992 
	lsrs r2,r0,#12 
	bne sleep991 
	lsrs r2,r0,#6 
	bne sleep990 
	and r2,r1,#0x3f 
	movs r0,#0 
	b sleep996 
sleep990 
	and r0,r1,#0x3f 
	ubfx r2,r1,#6,#6 
	adds r2,r2,#64 
	b sleep996 
sleep991 
	ubfx r0,r1,#0,#12 
	ubfx r2,r1,#12,#6 
	adds r2,r2,#128 
	b sleep996 
sleep992 
	ubfx r0,r1,#0,#18 
	ubfx r2,r1,#18,#6 
	adds r2,r2,#192 
sleep996 
	str r0,[r6,#PCB_TMO] 
	add r2,r7,r2,lsl #3 
	add r2,r2,#SC_SLEEPERLISTS 
	adds r1,r6,#PCB_SLEEPERS 
	str r2,[r1,#dbl_next] 
	ldr r0,[r2,#dbl_prev] 
	str r1,[r2,#dbl_prev] 
	str r0,[r1,#dbl_prev] 
	str r1,[r0,#dbl_next]	; and enter into timerlist
	ENDIF
	ldrh r1,[r6,#PCB_STATE_H]	; get current state
	movs r0,#ST_WAIT_TMO	; and merge TMO
	orrs r1,r1,r0
	strh r1,[r6,#PCB_STATE_H]	; set new stat
	UNLOCK
	SWAP_OUT
	LOCK
	ldrh r1,[r6,#PCB_STATE_H]	; get state
	cmp r0,#0	; swap-out successfull
	beq sleep_err3	; no ?! => schedule locked => error
	movs r0,#ST_TMO	; clear tmo-flag
	bics r1,r1,r0
	strh r1,[r6,#PCB_STATE_H]	; and set state
	UNLOCK
sleep99
	RET r4-r7
sleep_err1
	movs r1,#0
	ldr r0,sleep_err1_lit
	b sleep_err_cont
sleep_err2
	movs r1,r0
	ldr r0,sleep_err2_lit
sleep_err_cont
	bl sc_sysError
	b sleep99
	; scheduler was locked
sleep_err3
	movs r0,#ST_WAIT_TMO	; tmo-flag still set ?
	tst r1,r0
	beq sleep1	; no, skip
	bics r1,r1,r0 
	adds r6,r6,#PCB_SLEEPERS 
	ldr r2,[r6,#dbl_next] 
	ldr r0,[r6,#dbl_prev] 
	str r2,[r0,#dbl_next] 
	str r0,[r2,#dbl_prev] 
	subs r6,r6,#PCB_SLEEPERS 
	movs r0,#0 
	str r0,[r6,#PCB_TMO]	; yes, remove from timer-list
	bics r1,r1,r0
	strh r1,[r6,#PCB_STATE_H]	; and restore state
sleep1
	UNLOCK
	movs r1,#0
	ldr r0,sleep_err3_lit
	b sleep_err_cont
	ALIGN
sleep_sciopta
	DCD sciopta
sleep_err1_lit
	DCD ((0x5a<<24)|(0x02d<<12)|0x02)
sleep_err2_lit
	DCD ((0x5a<<24)|(0x030<<12))
sleep_err3_lit
	DCD ((0x014<<12)|(0x5a<<24)|0x02)
	SC_ENDFUNC sc_sleep
;
;********************************************************************
; tmo.S
;
; Asynchronous timeout
;
; ID: S04057BS1
; +Date: 2009/08/14 04:43:04 +
; +Revision: 1.15.2.1 +
;********************************************************************
;
	IF SC_ASYNC_TMO = 1
;
;**************************************
; sc_tmoid_t __CODE sc_tmoAdd(sc_ticks_t tmo, union sc_msg **msgptr)
;**************************************
;
	XSC_TFUNC sc_tmoAdd,-8
	SC_TFUNC sc_tmoAdd
	push {r4-r5,lr}
	IF SC_MSG_PARA_CHECK = 1
	cmp r1,#0
	beq tma_err0
	ldr r4,[r1,#0]
	cmp r4,#0
	beq tma_err0
	ELSE
	ldr r4,[r1,#0]
	ENDIF
	ldr r5,tma_sciopta
	IF SC_PROC_PARA_CHECK = 1
	ldr r3,[r5,#SC_CURPCB]
	ldrh r3,[r3,#PCB_FLAGS_H]
	lsrs r3,r3,#PCB_FLG_IS_PRIO_POS+1
	bcc tma_err1
	lsrs r2,r0,#24
	bne tma_err2
	ENDIF
	subs r4,r4,#32
	ldr r3,[r4,#28]
	IF SC_MSG_PARA_CHECK = 1
	mov r12,r5
	ldr r5,[r5,#SC_CURPCB]
	ldr r5,[r5,#PCB_PID]
	cmp r3,r5
	bne tma_err3
	mov r5,r12
	ENDIF
	str r3,[r4,#20]
	cmp r0,#0
	bne tma1
	movs r4,#(0x58<<24)>>24
	lsls r4,r4,#24
	movs r0,r1
	movs r1,r3
	movs r2,#0
	bl sc_sysMsgTx
	RET r4-r5
tma1
	movs r2,#0
	str r2,[r1,#0]
	ldr r2,tma_magic
	LOCK
	ldr r1,[r4,#12]
	orrs r1,r1,r2
	str r1,[r4,#12]
	ldr r2,[r4,#16]
	ldr r1,[r4,#8]
	lsls r2,r2,#28-3
	orrs r1,r1,r2
	str r1,[r4,#8]
	ldr r1,[r5,#SC_TCK] 
	adds r1,r1,r0 
	lsrs r2,r0,#18 
	bne tma992 
	lsrs r2,r0,#12 
	bne tma991 
	lsrs r2,r0,#6 
	bne tma990 
	movs r2,#0x3f 
	ands r2,r2,r1 
	movs r0,#0 
	b tma996 
tma990 
	movs r0,#0x3f 
	ands r0,r0,r1 
	lsrs r1,r1,#6 
	movs r2,#0x3f 
	ands r2,r2,r1 
	adds r2,r2,#64 
	b tma996 
tma991 
	lsls r0,r1,#20 
	lsrs r0,r0,#20 
	lsrs r1,r1,#12 
	movs r2,#0x3f 
	ands r2,r2,r1 
	adds r2,r2,#128 
	b tma996 
tma992 
	lsls r0,r1,#14 
	lsrs r0,r0,#14 
	lsrs r1,r1,#18 
	movs r2,#0x3f 
	ands r2,r2,r1 
	adds r2,r2,#192 
tma996 
	ldr r1,tma_tmolist 
	lsls r2,r2,#3 
	str r0,[r4,#16] 
	adds r2,r2,r1 
	adds r4,r4,#24 
	str r2,[r4,#dbl_next] 
	ldr r1,[r2,#dbl_prev] 
	str r4,[r2,#dbl_prev] 
	str r1,[r4,#dbl_prev] 
	str r4,[r1,#dbl_next] 
	subs r4,r4,#24
	UNLOCK
	movs r0,r4
	RET r4-r5
	IF SC_PROC_PARA_CHECK = 1
tma_err2
	ldr r0,tma_lit2
tma_err_cont
	bl sc_sysError
	add sp,sp,#2*4
	RET r4-r5
tma_err1
	ldr r1,[r2,#SC_CURPID]
	ldr r0,tma_lit1
	b tma_err_cont
	ENDIF
	IF SC_MSG_PARA_CHECK = 1
tma_err3
	movs r5,r1
	ldr r0,tma_lit3
	b tma_err_cont
tma_err0
	ldr r0,tma_lit0
	IF SC_PROC_PARA_CHECK = 1
	b tma_err_cont
	ELSE
tma_err_cont
	bl sc_sysError
	ENDIF
	ENDIF
	ALIGN
	IF SC_MSG_PARA_CHECK = 1
tma_lit3
	DCD ((0x00C<<12)|(0x58<<24)|0x02)
tma_lit0
	DCD ((0x00A<<12)|(0x58<<24)|0x02)
	ENDIF
	IF SC_PROC_PARA_CHECK = 1
tma_lit1
	DCD ((0x015<<12)|(0x58<<24)|0x02)
tma_lit2
	DCD ((0x58<<24)|(0x030<<12))
	ENDIF
tma_sciopta
	DCD sciopta
tma_magic
	DCD 0x00AFFE00
tma_tmolist
	DCD sciopta+SC_TMOLISTS
	SC_ENDFUNC sc_tmoAdd
;
;**************************************
; union sc_msg * __CODE sc_tmoRm(sc_tmoid_t *id)
;**************************************
;
	XSC_TFUNC sc_tmoRm,-12
	SC_TFUNC sc_tmoRm
	push {r4-r5,lr}
	movs r1,#0
	IF SC_MSG_PARA_CHECK = 1
	cmp r0,#0	; ptr = 0 ?
	beq tmr_err3
	ENDIF
	ldr r3,[r0,#0]	; r3 := tmoid
	cmp r3,#0	; = 0 ?
	beq tmr_err0	; yes => tmo already removed
	str r1,[r0,#0]	; take away from caller
	ldr r4,tmr_sciopta
	ldr r4,[r4,#SC_CURPCB]	; r4 := current pcb
	ldr r5,[r4,#PCB_PID]	; r5 := pid of caller
	ldr r1,[r3,#20]	; r1 := tmo requestor
	cmp r1,r5	; = caller ?
	bne tmr_err1	; not our tmo =>
	ldr r1,tmr_magic
	LOCK
	ldr r0,[r3,#12]
	lsls r2,r0,#8
	lsrs r2,r2,#16
	cmp r2,r1	; magic set ?
	beq tmr10	; yes => not yet delivered
	ldr r0,[r3,#28]	; msg still in the mqueue ?
	ldr r1,[r3,#24]	; r2 := addressee
	cmp r0,#0	; onwer != 0 => already recv'ed
	bne tmr_err2
	cmp r5,r1	; addressee = current
	bne tmr_err2	; no, => already recv'ed
	ldr r1,[r4,#PCB_NQUEUE]	; update stats
	subs r1,r1,#1
	str r1,[r4,#PCB_NQUEUE]
	ldr r1,[r3,#dbl_next] 
	ldr r2,[r3,#dbl_prev] 
	str r1,[r2,#dbl_next] 
	str r2,[r1,#dbl_prev]	; remove from mqueue
	ldr r1,[r4,#PCB_NALLOC]	; update stats
	adds r1,r1,#1
	str r1,[r4,#PCB_NALLOC]
	adds r4,r4,#PCB_ALLOCED
	str r4,[r3,#dbl_next] 
	ldr r1,[r4,#dbl_prev] 
	str r3,[r4,#dbl_prev] 
	str r1,[r3,#dbl_prev] 
	str r3,[r1,#dbl_next]	; insert into alloc-queue
tmr_exit
	movs r0,#0	; clean up msg-header
	str r0,[r3,#20]	; clear sender
	str r0,[r3,#24]	; clear addressee
	str r5,[r3,#28]	; make caller owner
	UNLOCK
	movs r0,r3
	adds r0,r0,#32
	RET r4-r5
tmr10
	movs r4,r3	; r4 := msg-header
	adds r4,r4,#24	; r4 := tmolist-hook
	ldr r1,[r4,#dbl_next] 
	ldr r2,[r4,#dbl_prev] 
	str r1,[r2,#dbl_next] 
	str r2,[r1,#dbl_prev]	; remove from list
	movs r1,#255
	lsls r1,r1,#24
	adds r1,r1,#255
	ands r0,r0,r1	; r0 &= 0xff0000ff (mask magic)
	str r0,[r3,#12]
	ldr r0,[r3,#8]
	lsrs r1,r0,#28	; get bufsize-idx
	lsls r1,r1,#3
	str r1,[r3,#16]	; store bufsize-idx*8
	lsls r0,r0,#4
	lsrs r0,r0,#4	; mask buf-size-idx
	str r0,[r3,#8]
	b tmr_exit
tmr_err2
	UNLOCK
tmr_err1
	ldr r0,tmr_lit1
	b tmr_err_cont
	IF SC_MSG_PARA_CHECK = 1
tmr_err3
	ldr r0,tmr_nilptr
	b tmr_err_cont
	ENDIF
tmr_err0
	ldr r0,tmr_lit0
tmr_err_cont
	bl sc_sysError
	movs r0,#0
	RET r4-r5
	ALIGN
tmr_lit0
	DCD ((0x030<<12)|(0x5b<<24))
tmr_lit1
	DCD ((0x00C<<12)|(0x5b<<24)|0x02)
	IF SC_MSG_PARA_CHECK = 1
tmr_nilptr
	DCD ((0x00A<<12)|(0x5b<<24)|0x02)
	ENDIF
tmr_sciopta
	DCD sciopta
tmr_magic
	DCD 0x00AFFE00>>8
	SC_ENDFUNC sc_tmoRm
	ENDIF
;
;**************************************************************************
; trigger.S
;
; S04082BS2
;
; +Date: 2009/11/16 10:43:39 +
; +Revision: 1.18.2.10 +
;**************************************************************************
;
;
;**************************************
; int __CODE sc_triggerWait(sc_triggerval_t dec,sc_ticks_t tmo)
;**************************************
;
	XSC_TFUNC sc_triggerWait,-8
	SC_TFUNC sc_triggerWaitx
tw_err
	movs r1,r0
tw_err2
	ldr r0,tw_lit1
tw_err_cont
	bl sc_sysError
	movs r0,#0
	RET r4-r7
	SC_TPUBLIC sc_triggerWait
	push {r4-r7,lr}
	ldr r7,tw_sciopta	; r7:= sciopta variables
	adds r5,r0,#0	; r5:= decrement (copy 'n test)
	ble tw_err	; error if <= 0 =>
	cmp r1,#(0)	; no tmo ?
	beq tw_err2	; yes => error
	ldr r6,[r7,#SC_CURPCB]	; r6:= current process
	ldrh r2,[r6,#PCB_FLAGS_H]	; check type
	lsrs r2,r2,#PCB_FLG_IS_PRIO_POS+1
	bcc tw_err0	; not prio => error
	movs r0,#0	; r0:= return value for direct exit
	LOCK
	ldr r2,[r6,#PCB_TRIGGER_VAL]
	subs r2,r2,r5
	str r2,[r6,#PCB_TRIGGER_VAL]	; trigger_val -= dec
	bgt tw_exit	; trigger still > 0 ? => exit
	adds r2,r1,#-(0xffffffff)	; tmo ?
	beq tw1	; endless => do not enter
	IF CPUTYPE <= 4
	ldr r0,[r7,#SC_TCK] 
	adds r0,r0,r1 
	lsrs r2,r1,#18 
	bne tw992 
	lsrs r2,r1,#12 
	bne tw991 
	lsrs r2,r1,#6 
	bne tw990 
	movs r2,#0x3f 
	ands r2,r2,r0 
	movs r1,#0 
	b tw996 
tw990 
	movs r1,#0x3f 
	ands r1,r1,r0 
	lsrs r0,r0,#6 
	movs r2,#0x3f 
	ands r2,r2,r0 
	adds r2,r2,#64 
	b tw996 
tw991 
	lsls r1,r0,#20 
	lsrs r1,r1,#20 
	lsrs r0,r0,#12 
	movs r2,#0x3f 
	ands r2,r2,r0 
	adds r2,r2,#128 
	b tw996 
tw992 
	lsls r1,r0,#14 
	lsrs r1,r1,#14 
	lsrs r0,r0,#18 
	movs r2,#0x3f 
	ands r2,r2,r0 
	adds r2,r2,#192 
tw996 
	lsls r2,r2,#3 
	str r1,[r6,#PCB_TMO] 
	adds r2,r2,#SC_SLEEPERLISTS-255 
	adds r2,r2,#255 
	movs r0,r6 
	adds r0,r0,#PCB_SLEEPERS 
	adds r2,r2,r7 
	str r2,[r0,#dbl_next] 
	ldr r1,[r2,#dbl_prev] 
	str r0,[r2,#dbl_prev] 
	str r1,[r0,#dbl_prev] 
	str r0,[r1,#dbl_next]	; insert into tmo-list
	ELSE
	ldr r0,[r7,#SC_TCK] 
	adds r0,r0,r1 
	lsrs r2,r1,#18 
	bne tw992 
	lsrs r2,r1,#12 
	bne tw991 
	lsrs r2,r1,#6 
	bne tw990 
	and r2,r0,#0x3f 
	movs r1,#0 
	b tw996 
tw990 
	and r1,r0,#0x3f 
	ubfx r2,r0,#6,#6 
	adds r2,r2,#64 
	b tw996 
tw991 
	ubfx r1,r0,#0,#12 
	ubfx r2,r0,#12,#6 
	adds r2,r2,#128 
	b tw996 
tw992 
	ubfx r1,r0,#0,#18 
	ubfx r2,r0,#18,#6 
	adds r2,r2,#192 
tw996 
	str r1,[r6,#PCB_TMO] 
	add r2,r7,r2,lsl #3 
	add r2,r2,#SC_SLEEPERLISTS 
	adds r0,r6,#PCB_SLEEPERS 
	str r2,[r0,#dbl_next] 
	ldr r1,[r2,#dbl_prev] 
	str r0,[r2,#dbl_prev] 
	str r1,[r0,#dbl_prev] 
	str r0,[r1,#dbl_next]	; insert into tmo-list
	ENDIF
	movs r2,#ST_WAIT_TMO	; and set flag
tw1
	ldrh r0,[r6,#PCB_STATE_H]	; get current state
	movs r1,#ST_WAIT_TRIGGER
	orrs r0,r0,r2
	orrs r0,r0,r1
	strh r0,[r6,#PCB_STATE_H]	; and set new one
	ldrh r1,[r6,#PCB_FLAGS_H] 
	movs r2,#PCB_FLG_READY 
	tst r1,r2 
	beq tw19999 
	bics r1,r1,r2 
	strh r1,[r6,#PCB_FLAGS_H] 
	ldr r1,[r6,#PCB_PCBLIST+dbl_next] 
	ldr r2,[r6,#PCB_PCBLIST+dbl_prev] 
	str r1,[r2,#dbl_next] 
	str r2,[r1,#dbl_prev] 
	cmp r1,r2 
	bne tw19998 
	ldr r2,[r6,#PCB_PRIO_MASK] 
	ldr r1,[r7,#SC_PRIOMASK] 
	bics r1,r1,r2 
	str r1,[r7,#SC_PRIOMASK] 
tw19998 
	movs r1,#0 
	str r1,[r6,#PCB_PCBLIST+dbl_next] 
	str r1,[r6,#PCB_PCBLIST+dbl_prev] 
tw19999	; and remove from ready-list
	UNLOCK
	SWAP_OUT_SAVE {r5}	; re-schedule, save r5 (decrement)
	LOCK
	ldrh r1,[r6,#PCB_STATE_H]
	cmp r0,#0	; r0 = 0 => no swap-out possible ?
	beq tw_err1	; yes, error
	movs r0,#1	; set return value: triggered
	movs r2,#ST_TMO
	tst r1,r2	; tmo happened ?
	beq tw_exit	; no => leave
	adds r2,#ST_WAIT_TRIGGER
	bics r1,r1,r2	; clear tmo flag
	strh r1,[r6,#PCB_STATE_H]	; and set state
	subs r0,r0,#2	; r0:= -1
	ldr r1,[r6,#PCB_TRIGGER_VAL]	; restore trigger_value
	adds r1,r1,r5
	str r1,[r6,#PCB_TRIGGER_VAL]
tw_exit
	UNLOCK
	RET r4-r7
tw_err1
	movs r0,#ST_WAIT_TMO	; swap out failed
	tst r1,r0	; clean up ?
	beq tw5
	bics r1,r1,r0 
	adds r4,r4,#PCB_SLEEPERS 
	ldr r2,[r4,#dbl_next] 
	ldr r0,[r4,#dbl_prev] 
	str r2,[r0,#dbl_next] 
	str r0,[r2,#dbl_prev] 
	subs r4,r4,#PCB_SLEEPERS 
	movs r0,#0 
	str r0,[r4,#PCB_TMO]	; remove from tmo list
	strh r1,[r4,#PCB_STATE_H]
tw5
	UNLOCK
	ldr r0,tw_lit2
	movs r1,#0
	b tw_err_cont
tw_err0
	movs r1,r3
	ldr r0,tw_lit2
	b tw_err_cont
	ALIGN
tw_sciopta
	DCD sciopta
tw_lit1
	DCD (0x53<<24)|(0x030<<12)|0x40
tw_lit2
	DCD (0x53<<24)|(0x02d<<12)|0x04
	SC_ENDFUNC sc_triggerWaitx
;
;**************************************
; void __CODE sc_trigger(sc_pid_t pid)
;**************************************
;
	XSC_TFUNC sc_trigger,-12
	SC_TFUNC sc_trigger
	push {r4-r7,lr}
	ldr r7,t_sciopta	; r7:= base of kernel variables
	ldr r6,t_sc_modules	; r6:= module-array
	ldr r5,[r7,#SC_CURPCB]	; r5:= current process
	LOCK
	adds r1,r0,#-0xffffffff	; pid = 0xffffffff
	beq t1	; yes => found
	adds r1,r0,#0	; pid <= 0 => extern or init0
	ble t_err	; yes => illegal pid
	lsrs r2,r0,#24	; r2:= module index
	cmp r2,#SC_MAX_MODULE	; in range ?
	bhs t_err	; no => illegal pid
	lsls r2,r2,#2
	ldr r2,[r6,r2]	; r2:= module of triggered process
	cmp r2,#1	; valid ? (>1)
	bls t_err	; no => illegale pid
	ldr r3,[r2,#MODULE_MAX_PROCESS]	; r3:= max. process in this module
	lsls r0,r0,#18
	lsrs r0,r0,#18	; r0:= process index
	cmp r0,r3	; in range ?
	ldr r3,[r2,#MODULE_PCBS]	; r3:= pcb array
	bhs t_err	; process index wrong => illegal pid
	lsls r0,r0,#2
	ldr r5,[r3,r0]	; r5:= pcb
	cmp r5,#0	; valid ?
	beq t_err	; no => illegal pid
	ldr r2,[r5,#PCB_PID]	; pid still the same
	cmp r2,r1
	bne t_warn	; no, process changed,warn =>illegal pid
t1
	ldrh r4,[r5,#PCB_FLAGS_H]
	ldr r3,[r5,#PCB_TRIGGER_VAL]	; r3:= trigger value
	movs r2,#PCB_FLG_IS_PRIO|PCB_FLG_IS_TIMER|PCB_FLG_IS_IRQ
	tst r4,r2	; check process type
	beq t_err	; wrong type => error
	adds r3,r3,#1
	str r3,[r5,#PCB_TRIGGER_VAL]	; increase trigger and store
	ble t_exit	; still <= 0 => exit
	ldrh r3,[r5,#PCB_STATE_H]
	lsrs r0,r4,#PCB_FLG_IS_PRIO_POS+1
	bcc t3	; no carry, process not prio =>
	movs r1,#ST_WAIT_TRIGGER
	tst r3,r1	; process waits for trigger ?
	beq t_exit	; no => exit
	bics r3,r3,r1	; else clear state
	movs r1,#ST_WAIT_TMO
	tst r3,r1	; also waiting for tmo ?
	beq t2
	bics r3,r3,r1 
	adds r5,r5,#PCB_SLEEPERS 
	ldr r0,[r5,#dbl_next] 
	ldr r1,[r5,#dbl_prev] 
	str r0,[r1,#dbl_next] 
	str r1,[r0,#dbl_prev] 
	subs r5,r5,#PCB_SLEEPERS 
	movs r1,#0 
	str r1,[r5,#PCB_TMO]	; yes, remove from tmo-list
t2
	strh r3,[r5,#PCB_STATE_H]	; store new state
	lsrs r3,r3,#ST_READY_POS+1	; now ready ?
	bne t_exit	; no => leave
	ldrh r0,[r5,#PCB_FLAGS_H] 
	movs r1,#PCB_FLG_READY 
	tst r0,r1 
	bne t2mr9999 
	orrs r0,r0,r1 
	strh r0,[r5,#PCB_FLAGS_H] 
	ldr r2,[r7,#SC_PRIOMASK] 
	ldr r1,[r5,#PCB_PRIO_MASK] 
	ldr r0,[r5,#PCB_READYLIST] 
	orrs r2,r2,r1 
	str r2,[r7,#SC_PRIOMASK] 
	adds r5,r5,#PCB_PCBLIST 
	str r0,[r5,#dbl_next] 
	ldr r1,[r0,#dbl_prev] 
	str r5,[r0,#dbl_prev] 
	str r1,[r5,#dbl_prev] 
	str r5,[r1,#dbl_next] 
	subs r5,r5,#PCB_PCBLIST 
t2mr9999	; insert into ready-list
	UNLOCK
	ldr r0,[r7,#SC_CURPRIOPCB]	; check for re-schedule
	ldrb r1,[r5,#PCB_PRIO_B]
	ldrb r0,[r0,#PCB_PRIO_B]
	cmp r1,r0
	bhs t_exit1	; current prio higher or same => exit
t5
	SWAP_OUT
	RET r4-r7
t_exit
	UNLOCK
	RET r4-r7
t3
	UNLOCK
		; 29.09.05
	movs r0,#PCB_FLG_WAKEUP_DIS
	tst r4,r0	; wakeup allowed ?
	bne t_exit1
		; *********
	movs r0,#2	; r0:= wakeup by trigger
	movs r1,r5	; r1:= PCB
	swi 0
	ldr r0,[r7,#SC_SCHEDULE_REQ]	; scheduling needed ?
	cmp r0,#0
	bne t5	; yes, check current process-type
t_exit1
	RET r4-r7
t_warn
	ldr r0,t_lit1
	b t_err_cont
t_err
	ldr r0,t_lit0
t_err_cont
	UNLOCK
	bl sc_sysError
	RET r4-r7
	ALIGN
t_sciopta
	DCD sciopta
t_sc_modules
	DCD sc_modules
t_lit0
	DCD (0x020<<12)|(0x52<<24)|0x04
t_lit1
	DCD (0x020<<12)|(0x52<<24)
	SC_ENDFUNC sc_trigger
;
;**************************************
; void __CODE sc_triggerValueSet(sc_triggerval_t value)
;**************************************
;
	XSC_TFUNC sc_triggerValueSet,-12
	SC_TFUNC sc_triggerValueSet
	adds r1,r0,#0
	ble tvs_err
	ldr r3,tvs_sciopta
	ldr r2,[r3,#SC_CURPCB]
	str r0,[r2,#PCB_TRIGGER_VAL]
	bx lr
tvs_err
	push {lr}
	ldr r0,tvs_lit0
	bl sc_sysError
	EXIT
	ALIGN
tvs_sciopta
	DCD sciopta
tvs_lit0
	DCD (0x50<<24)|(0x030<<12)
	SC_ENDFUNC sc_triggerValueSet
;
;**************************************
; sc_triggerval_t __CODE sc_triggerValueGet(sc_pid_t pid)
;**************************************
;
	XSC_TFUNC sc_triggerValueGet,-12
	SC_TFUNC sc_triggerValueGet
	ldr r3,tvg_sciopta
	ldr r2,[r3,#SC_CURPCB]
	adds r1,r0,#-0xffffffff
	bne tvg1
	ldr r0,[r2,#PCB_TRIGGER_VAL]
	bx lr
tvg1
	push {lr}
	cmp r0,#0
	ble tvg_err
	lsrs r1,r0,#24
	cmp r1,#SC_MAX_MODULE
	ldr r3,tvg_sc_modules
	bhs tvg_err
	LOCK
	lsls r1,r1,#2
	ldr r3,[r3,r1]
	lsls r1,r0,#18
	lsrs r1,r1,#18
	cmp r3,#1
	bls tvg_err0
	ldr r2,[r3,#MODULE_MAX_PROCESS]
	cmp r1,r2
	ldr r2,[r3,#MODULE_PCBS]
	bhs tvg_err0
	lsls r1,r1,#2
	ldr r2,[r2,r1]
	cmp r2,#0
	beq tvg_err0
; FIXME: should issue warning here
	ldr r0,[r2,#PCB_TRIGGER_VAL]
	UNLOCK
	EXIT
tvg_err0
	UNLOCK
tvg_err
	movs r1,r0
	ldr r0,tvg_lit1
	bl sc_sysError
	ALIGN
tvg_sciopta
	DCD sciopta
tvg_sc_modules
	DCD sc_modules
tvg_lit1
	DCD (0x020<<12)|(0x51<<24)|0x04
	SC_ENDFUNC sc_triggerValueGet
;
; procd.S
;
; ID: S04075BS4
; +Revision: 1.11.2.1 +
; +Date: 2009/08/14 04:43:04 +
;
	; r8 - request-msg
	SC_TFUNC sc_procdx
	; IDGET
pd_idget
	movs r0,#((((((((4 +4*2)+4)+4)+4)+SC_MODULE_NAME_SIZE+1)+SC_PROC_NAME_SIZE+1)+2)+2)
	ldr r1,pd_SC_PROCDREQUESTMSG
	movs r2,#0
	subs r3,r2,#-((0xffffffff)-1)
	bl sc_msgAlloc
	movs r4,r0
	mov r5,r8
	subs r5,r5,#32
	ldr r1,[r5,#20]
	movs r0,#0
	str r0,[r4,#(((4 +4*2)+4)+4)]
	str r0,[r4,#((4 +4*2)+4)]
	str r1,[r4,#(4 +4*2)]
	adds r5,r5,#32 +(4 +4)
	movs r0,r5
	bl sc_procPathCheck
	cmp r0,#0
	beq pd_error_cont
	ldrb r1,[r5,#0]
	cmp r1,#'/'
	bne pd_no_module
pd_again
	ldrb r2,[r5,#1]
	adds r5,r5,#1
	cmp r1,r2
	bne pd_no_target
	mov r2,r10
	b pd12
pd_error_cont
	b pd_error
pd11
	strb r1,[r2,#0]
pd12
	ldrb r1,[r5,#1]
	adds r5,r5,#1
	adds r2,r2,#1
	cmp r1,#'/'
	bne pd11
	movs r1,#0
	strb r1,[r2,#0]
	add r0,sp,#0
	bl sc_moduleIdGet
	movs r1,#'/'
	cmp r0,#0
	beq pd_again
	IF SC_MAX_CONNECTOR > 0
	; find connector
	add r0,sp,#0
	bl sc_miscCrcString
	mov r12,r0
	ldr r7,pd_sc_connectors
	movs r6,#SC_MAX_CONNECTOR
pd13
	ldmia r7!,{r3}
	subs r6,r6,#1
	bmi pd_error_cont
	cmp r3,#1
	bls pd13
	ldrh r0,[r3,#PCB_HASH_H]
	cmp r0,r12
	bne pd13
	ldr r0,[r3,#PCB_PID]
	mov r11,r0
	add r0,sp,#0
	ldr r3,[r3,#PCB_NAME]
pd14
	ldrb r1,[r0,#0]
	adds r0,r0,#1
	ldrb r2,[r3,#0]
	adds r3,r3,#1
	cmp r2,r1
	bne pd13
	cmp r2,#0
	bne pd14
	movs r1,r5
	movs r0,#(((4 +4)+4)+1)-1
pd15
	ldrb r2,[r1,#0]
	adds r0,r0,#1
	adds r1,r1,#1
	cmp r2,#0
	bne pd15
	ldr r1,pd_lit1
	movs r2,#0
	subs r3,r2,#-((0xffffffff)-1)
	bl sc_msgAlloc
	str r0,[sp,#44]
	mov r1,r8
	ldr r2,[r1,#4]
	str r2,[r0,#4]
	mov r2,r11
	str r2,[r0,#(4 +4)]
	adds r0,r0,#((4 +4)+4)-1
	movs r1,r5
pd16
	ldrb r2,[r1,#0]
	adds r0,r0,#1
	adds r1,r1,#1
	strb r2,[r0,#0]
	cmp r2,#0
	bne pd16
	add r0,sp,#44
	mov r1,r11
	movs r2,#0
	bl sc_msgTx
	mov r0,r11
	str r0,[r4,#(((4 +4*2)+4)+4)]
	adds r0,r0,#1
	mov r11,r0
	ldr r0,pd_lit2
	str r0,[r4,#0]
	b pd_loopadd
	ELSE
	b pd_error
	ENDIF
pd_no_module
	ldr r0,[r4,#(4 +4*2)]
	bl sc_moduleNameGet
	movs r1,r4
	adds r1,r1,#((((4 +4*2)+4)+4)+4)
	ldmia r0!,{r2-r3,r6-r7}
	stmia r1!,{r2-r3,r6-r7}
	ldmia r0!,{r2-r3,r6-r7}
	stmia r1!,{r2-r3,r6-r7}
	ldr r7,[r4,#(4 +4*2)]
	b pd_idget_proc
	; /module/process
pd_no_target
	movs r1,r4
	adds r1,r1,#((((4 +4*2)+4)+4)+4)
	movs r0,r1
pd17
	strb r2,[r1,#0]
pd18
	ldrb r2,[r5,#1]
	adds r5,r5,#1
	adds r1,r1,#1
	cmp r2,#0
	beq pd_target_proc
	cmp r2,#'/'
	bne pd17
	movs r2,#0
	strb r2,[r1,#0]
	bl sc_moduleIdGet
	movs r7,r0	; save module-id
	adds r5,r5,#1
	b pd_idget_proc
pd_target_proc
	strb r2,[r1,#0]
	movs r1,r4
	adds r1,r1,#(((((4 +4*2)+4)+4)+4)+SC_MODULE_NAME_SIZE+1)
	ldmia r0!,{r2-r3,r5-r6}
	stmia r1!,{r2-r3,r5-r6}
	ldmia r0!,{r2-r3,r5-r6}
	stmia r1!,{r2-r3,r5-r6}
	movs r7,#0
	movs r0,r7
	bl sc_moduleNameGet
	movs r1,r4
	adds r1,r1,#((((4 +4*2)+4)+4)+4)
	ldmia r0!,{r2-r3,r5-r6}
	stmia r1!,{r2-r3,r5-r6}
	ldmia r0!,{r2-r3,r5-r6}
	stmia r1!,{r2-r3,r5-r6}
	b pd20
pd_idget_proc
	movs r1,r4
	adds r1,r1,#(((((4 +4*2)+4)+4)+4)+SC_MODULE_NAME_SIZE+1)-1
pd19
	ldrb r2,[r5,#0]
	adds r5,r5,#1
	adds r1,r1,#1
	strb r2,[r1,#0]
	cmp r2,#0
	bne pd19
pd20
	movs r6,#0
	adds r0,r7,#-SC_ILLEGAL_MID
	beq pd21
	movs r0,r4
	adds r0,r0,#(((((4 +4*2)+4)+4)+4)+SC_MODULE_NAME_SIZE+1)
	movs r1,r7
	bl sc_sysProcIdGet
	ldr r1,pd_SC_ILLEGAL_PID
	movs r7,r0
	cmp r0,r1
	beq pd21
	b pd_txreply
pd21
	mov r0,r8
	ldr r5,[r0,#4]
	adds r2,r5,#-((0xffffffff)-1)
	beq pd_error
	cmp r5,#(0)
	beq pd_tmoreply
	movs r0,r4
	adds r0,r0,#((((4 +4*2)+4)+4)+4)
	bl sc_miscCrcString
	movs r6,#((((((4 +4*2)+4)+4)+4)+SC_MODULE_NAME_SIZE+1)+SC_PROC_NAME_SIZE+1)
	strh r0,[r4,r6]
	adds r6,r6,#(((((((4 +4*2)+4)+4)+4)+SC_MODULE_NAME_SIZE+1)+SC_PROC_NAME_SIZE+1)+2)-((((((4 +4*2)+4)+4)+4)+SC_MODULE_NAME_SIZE+1)+SC_PROC_NAME_SIZE+1)
	movs r0,r4
	adds r0,r0,#(((((4 +4*2)+4)+4)+4)+SC_MODULE_NAME_SIZE+1)
	bl sc_miscCrcString
	strh r0,[r4,r6]
	adds r0,r5,#-(0xffffffff)
	beq pd_loopadd
	IF SC_ASYNC_TMO = 1
	movs r0,#(4 +4)
	ldr r1,pd_SC_PROCDTMOREQMSG
	movs r2,#0
	subs r3,r2,#-(0xffffffff)
	bl sc_msgAlloc
	str r4,[r0,#4]
	str r0,[sp,#44]
	movs r0,r5
	add r1,sp,#44
	bl sc_tmoAdd
	str r0,[r4,#((4 +4*2)+4)]
	b pd_loopadd
	ELSE
	b pd_error
	ENDIF
	; -----------------------------
	; -- sc_procd
	; -----------------------------
	SC_TPUBLIC sc_procd
	bl sc_procDaemonRegister
	cmp r0,#0
	bne pd1
	ldr r0,pd_lit0
	movs r1,#0
	bl sc_procKill
pd1
	movs r0,#42
	mov r11,r0
	sub sp,sp,#SC_PROC_NAME_SIZE+1+12+8
	add r0,sp,#32
	str r0,[sp,#32]
	str r0,[sp,#32 +4]
	mov r0,sp
	subs r0,r0,#1
	mov r10,r0
	b pd10
pd_error
	ldr r6,pd_lit3	; error-code
pd_tmoreply
	ldr r7,pd_SC_ILLEGAL_PID	; pid = 0x7fffffff
pd_txreply
	movs r0,#(((4 +4)+4)+4)
	ldr r1,pd_SC_PROCIDGETMSG_REPLY
	movs r2,#0
	subs r3,r2,#-((0xffffffff)-1)
	bl sc_msgAlloc
	str r0,[sp,#44]
	str r6,[r0,#((4 +4)+4)]
	str r7,[r0,#4]
	add r0,sp,#44
	ldr r1,[r4,#(4 +4*2)]
	movs r2,#0
	bl sc_msgTx
	str r4,[sp,#48]
	add r0,sp,#48
	bl sc_msgFree
	b pd_loop
pd_loopadd
	add r0,sp,#32
	adds r4,r4,#4
	str r0,[r4,#dbl_next] 
	ldr r1,[r0,#dbl_prev] 
	str r4,[r0,#dbl_prev] 
	str r1,[r4,#dbl_prev] 
	str r4,[r1,#dbl_next]
pd_loop
	add r0,sp,#40
	bl sc_msgFree
pd10
	movs r0,#-(0xffffffff)
	rsbs r0,r0,#0
	adr r4,pd_select
	movs r1,r4
	movs r2,#0x02
	bl sc_msgRx
	str r0,[sp,#40]
	mov r8,r0
	ldr r5,[r0,#0]
	IF SC_MAX_CONNECTOR > 0
	ldmia r4!,{r1-r3}
	ELSE
	ldmia r4!,{r1-r2}
	ENDIF
	cmp r5,r1
	beq pd_notify
	IF SC_ASYNC_TMO = 1
	cmp r5,r2
	beq pd_tmoreq
	ENDIF
	IF SC_MAX_CONNECTOR > 0
	cmp r5,r3
	beq pd_connidget
	ENDIF
	b pd_idget
	IF SC_ASYNC_TMO = 1
	; tmoreg
pd_tmoreq
	ldr r0,[r0,#4]
	movs r4,r0
	adds r0,r0,#4
	ldr r1,[r0,#dbl_next] 
	ldr r2,[r0,#dbl_prev] 
	str r1,[r2,#dbl_next] 
	str r2,[r1,#dbl_prev]
	movs r6,#0
	b pd_tmoreply
	ENDIF
	; notify
pd_notify
	add r5,sp,#32
	mov r9,r5
pd30
	ldr r5,[r5,#dbl_next]
	cmp r5,r9
	beq pd_loop
	movs r4,r5
	subs r4,r4,#4
	ldr r0,[r4,#0]
	ldr r1,pd_SC_PROCDREQUESTMSG
	cmp r1,r0
	bne pd30
	movs r2,r4
	adds r2,r2,#((((((4 +4*2)+4)+4)+4)+SC_MODULE_NAME_SIZE+1)+SC_PROC_NAME_SIZE+1)
	mov r1,r8
	adds r1,r1,#((((8 +SC_PROC_NAME_SIZE+1)+SC_PROC_NAME_SIZE+1)+SC_PROC_NAME_SIZE+1)+2)
	ldrh r1,[r1,#0]
	ldrh r3,[r2,#0]
	cmp r1,r3
	bne pd30
	mov r1,r8
	adds r1,r1,#(((((8 +SC_PROC_NAME_SIZE+1)+SC_PROC_NAME_SIZE+1)+SC_PROC_NAME_SIZE+1)+2)+2)
	ldrh r1,[r1,#0]
	ldrh r2,[r2,#(((((((4 +4*2)+4)+4)+4)+SC_MODULE_NAME_SIZE+1)+SC_PROC_NAME_SIZE+1)+2)-((((((4 +4*2)+4)+4)+4)+SC_MODULE_NAME_SIZE+1)+SC_PROC_NAME_SIZE+1)]
	cmp r1,r2
	bne pd30
	movs r2,r4
	adds r2,r2,#((((4 +4*2)+4)+4)+4)
	mov r3,r8
	adds r3,r3,#(8 +SC_PROC_NAME_SIZE+1)
pd31
	ldrb r0,[r2,#0]
	ldrb r1,[r3,#0]
	adds r2,r2,#1
	adds r3,r3,#1
	cmp r0,r1
	bne pd30
	cmp r0,#0
	bne pd31
	movs r2,r4
	adds r2,r2,#(((((4 +4*2)+4)+4)+4)+SC_MODULE_NAME_SIZE+1)
	mov r3,r8
	adds r3,r3,#((8 +SC_PROC_NAME_SIZE+1)+SC_PROC_NAME_SIZE+1)
pd32
	ldrb r0,[r2,#0]
	ldrb r1,[r3,#0]
	adds r2,r2,#1
	adds r3,r3,#1
	cmp r0,r1
	bne pd30
	cmp r0,#0
	bne pd32
	ldr r0,[r5,#dbl_next] 
	ldr r1,[r5,#dbl_prev] 
	str r0,[r1,#dbl_next] 
	str r1,[r0,#dbl_prev]
	IF SC_ASYNC_TMO = 1
	ldr r0,[r4,#((4 +4*2)+4)]
	cmp r0,#0
	beq pd_txreply2a
	movs r0,r4
	adds r0,r0,#((4 +4*2)+4)
	bl sc_tmoRm
	str r0,[sp,#44]
	add r0,sp,#44
	bl sc_msgFree
	ENDIF
pd_txreply2a
	mov r0,r8
	movs r6,#0
	ldr r7,[r0,#4]
	bl pd_txreply2
	b pd30
	IF SC_MAX_CONNECTOR > 0
pd_connidget
	mov r7,r8
	add r5,sp,#32
	mov r9,r5
pd40
	ldr r5,[r5,#dbl_next]
	cmp r5,r9
	beq pd_loop
	movs r4,r5
	subs r4,r4,#4
	ldr r0,[r4,#0]
	ldr r1,pd_SC_PROCDCONNREQUESTMSG
	cmp r0,r1
	bne pd40
	ldr r0,[r4,#(((4 +4*2)+4)+4)]
	ldr r1,[r7,#(4 +4)]
	cmp r0,r1
	bne pd40
	ldr r0,[r5,#dbl_next] 
	ldr r1,[r5,#dbl_prev] 
	str r0,[r1,#dbl_next] 
	str r1,[r0,#dbl_prev]
	ldr r6,[r7,#((4 +4)+4)]
	ldr r7,[r7,#4]
	cmp r6,#0
	beq pd41
	ldr r6,pd_lit3
pd41
	bl pd_txreply2
	b pd40
	ENDIF
pd_txreply2
	push {lr}
	movs r0,#(((4 +4)+4)+4)
	ldr r1,pd_SC_PROCIDGETMSG_REPLY
	movs r2,#0
	subs r3,r2,#-((0xffffffff)-1)
	bl sc_msgAlloc
	str r0,[sp,#44 +4]
	str r6,[r0,#((4 +4)+4)]
	str r7,[r0,#4]
	add r0,sp,#44 +4
	ldr r1,[r4,#(4 +4*2)]
	movs r2,#0
	bl sc_msgTx
	str r4,[sp,#48 +4]
	add r0,sp,#48 +4
	bl sc_msgFree
	pop {pc}
	ALIGN
pd_SC_PROCDCONNREQUESTMSG
	DCD (0x80000000 +0x501)
pd_SC_PROCDREQUESTMSG
	DCD (0x80000000 +0x500)
pd_SC_PROCDTMOREQMSG
	DCD (0x80000000 +0x502)
pd_SC_ILLEGAL_PID
	DCD 0x7fffffff
pd_SC_PROCIDGETMSG_REPLY
	DCD (0x80000000 +0x10d)
pd_lit3
	DCD ((0x24<<24)|(0x025<<12)|0x04)
	IF SC_MAX_CONNECTOR > 0
pd_sc_connectors
	DCD sc_connectors
pd_lit2
	DCD (0x80000000 +0x501)
pd_lit1
	DCD (0x80000000 +0x400)
	ENDIF
pd_lit0
	DCD 0xffffffff
pd_select
	DCD (0x80000000 +0x300)
	DCD (0x80000000 +0x502)
	IF SC_MAX_CONNECTOR > 0
	DCD (0x80000000 +0x401)
	ENDIF
	DCD (0x80000000 +0x10c)
	DCD 0
	SC_ENDFUNC sc_procdx
;
; kerneld.S
;
; ID: S04077BS1
; +Revision: 1.10.2.1 +
; +Date: 2009/08/14 04:43:04 +
;
	SC_TFUNC sc_kerneld
	sub sp,sp,#8
	bl sc_miscKerneldRegister
	cmp r0,#0
	bne k_loop
	movs r0,#-0xffffffff
	rsbs r0,r0,#0
	movs r1,#0
	bl sc_procKill
	; r10 - sender
	; r2 - msgptr
	; r1 - new size
k_reply
	ldr r0,[r2,#0]
	adds r0,r0,#1
	str r0,[r2,#0]
	add r0,sp,#0
	bl sc_msgSizeSet
	add r0,sp,#0
	mov r1,r10
	movs r2,#0
	bl sc_msgTx
k_loop
	movs r0,#-(0xffffffff)
	rsbs r0,r0,#0
	adr r7,k_select
	movs r1,r7
	movs r2,#0x02
	bl sc_msgRx
	str r0,[sp,#0]
	mov r8,r0
	subs r0,r0,#32
	ldr r1,[r0,#20]
	mov r10,r1
	ldr r0,[r0,#32]
	adr r6,k_lit0
	ldmia r7!,{r1-r4}
	ldmia r6!,{r5}
	cmp r0,r1
	beq k_create
	ldmia r6!,{r5}
	cmp r0,r2
	beq k_create
	ldmia r6!,{r5}
	cmp r0,r3
	beq k_create
	ldmia r6!,{r5}
	cmp r0,r4
	beq k_create
	ldmia r7!,{r1-r2}
	cmp r0,r1
	beq k_kill
	cmp r0,r2
	beq k_mcreate
	;
	; Kill a module
	;
k_mkill
	mov r4,r8
	ldr r0,[r4,#4]
	ldr r1,[r4,#(4 +4)]
	adds r2,r0,#-SC_CURRENT_MID
	bne k_mk1
	mov r0,r10
k_mk1
	movs r5,r0
	bl sc_sysModuleKill
	movs r1,#(4 +4)
	mov r2,r8
	cmp r5,r10
	str r5,[r2,#4]
	bne k_reply
k_free
	add r0,sp,#0
	bl sc_msgFree
	b k_loop
	;
	; Create module
	;
k_mcreate
	mov r0,r8
	adds r0,r0,#((4 +SC_MODULE_NAME_SIZE+1)+4)
	ldmia r0!,{r1-r7}
	subs r0,r0,#(SC_MODULE_NAME_SIZE+1+9*4)-4
	push {r3-r7}
	movs r3,r2	; prio
	movs r2,r1	; stacksize
	ldr r1,[r0,#(4 +SC_MODULE_NAME_SIZE+1)-4]
	bl sc_sysModuleCreate
	add sp,sp,#5*4
	mov r2,r8
	str r0,[r2,#4]
	movs r1,#(4 +4)
	b k_reply
	;
	; Kill a process
	;
k_kill
	mov r3,r8
	ldr r0,[r3,#4]
	ldr r1,[r3,#(4 +4)]
	movs r4,r0
	bl sc_sysProcKill
	movs r1,#(4 +4)
	mov r2,r8
	cmp r4,r10
	bne k_reply
	b k_free
	;
	; Create a process
	;
k_create
	mov r3,r8
	mov r1,r10
	ldr r6,k_sc_modules
	lsrs r2,r1,#24
	lsls r2,r2,#2
	ldr r6,[r6,r2]	; module
	ldr r0,[r3,#((4 +SC_PROC_NAME_SIZE+1)+4)]
	IF CPUTYPE = 4
	lsrs r0,r0,#3
	lsls r0,r0,#3
	ELSE
	lsrs r0,r0,#2
	lsls r0,r0,#2
	ENDIF
	adds r0,r0,#SIZEOF_PCB+SC_PROC_NAME_SIZE+1
	movs r4,r0	; save size if error
	ldr r1,k_lit1
	ldr r2,[r3,#((((((4 +SC_PROC_NAME_SIZE+1)+4)+4)+4)+4)+4)]
	movs r3,#(0)
	ldr r7,k_sciopta
	bl sc_sysMsgAlloc
	cmp r0,#0
	beq k_err
	str r0,[sp,#4]
	mov r9,r0
	movs r4,r0	; stack
	adds r4,r4,#SIZEOF_PCB+SC_PROC_NAME_SIZE+1
	movs r3,r0	; pcb
	ldr r2,[r6,#MODULE_ID]
	mov r7,r8
	ldr r1,[r7,#(((((4 +SC_PROC_NAME_SIZE+1)+4)+4)+4)+4)]
	ldr r0,[r7,#((((4 +SC_PROC_NAME_SIZE+1)+4)+4)+4)]
	push {r0-r5}
	adds r5,r7,#4
	mov r1,r9
	adds r1,r1,#SIZEOF_PCB
	movs r0,r1
	ldmia r5!,{r2-r4,r6}
	stmia r1!,{r2-r4,r6}
	ldmia r5!,{r2-r4,r6}
	stmia r1!,{r2-r4,r6}
	adds r7,r7,#(4 +SC_PROC_NAME_SIZE+1)
	ldmia r7!,{r1-r3}
	bl sc_sysProcCreate
	add sp,sp,#6*4
	mov r1,r10	; sender
	mov r2,r9	; pcb
	subs r2,r2,#32
	str r1,[r2,#20]
	mov r2,r8
	str r0,[r2,#4]
	movs r1,#(4 +4)
	b k_reply
k_err
	ldr r0,k_lit2
	movs r1,r4
	orrs r0,r0,r5
	bl sc_sysError
	ALIGN
k_sciopta
	DCD sciopta
k_lit2
	DCD (0x00d<<12)|0x02
k_lit1
	DCD SC_PCB_MAGIC
k_lit0
	DCD (0x2f<<24)
	DCD (0x30<<24)
	DCD (0x32<<24)
	DCD (0x31<<24)
k_sc_modules
	DCD sc_modules
k_select
	DCD (0x80000000 +0x100)
	DCD (0x80000000 +0x102)
	DCD (0x80000000 +0x106)
	DCD (0x80000000 +0x104)
	DCD (0x80000000 +0x108)
	DCD (0x80000000 +0x200)
	DCD (0x80000000 +0x202)
	DCD 0
	SC_ENDFUNC sc_kerneld
;
;********************************************************************
; connector.S
;
; connector related functions
;
; ID: S04082BS1
; +Date: 2009/10/08 07:20:59 +
; +Revision: 1.13.2.3 +
;********************************************************************
;
	IF SC_MAX_CONNECTOR > 0
;
;**************************************
; sc_pid_t __CODE sc_connectorRegister(int defaultConn)
;**************************************
;
	XSC_TFUNC sc_connectorRegister,-12
	SC_TFUNC sc_connectorRegister
	push {r7,lr}
	ldr r3,cr_sciopta
	ldr r2,[r3,#SC_CURPCB]
	ldrh r1,[r2,#PCB_FLAGS_H]
	lsrs r1,r1,#PCB_FLG_IS_CONNECTOR_POS+1
	bcs cr_err
	ldr r3,cr_sc_connectors
	cmp r0,#0
	beq cr_nodef
	LOCK
	ldr r0,[r3,#0]
	cmp r0,#0
	bne cr_err0
	str r2,[r3,#0]
	ldrh r3,[r2,#PCB_FLAGS_H]
	movs r1,#1
	lsls r1,r1,#PCB_FLG_IS_CONNECTOR_POS
	orrs r3,r3,r1
	strh r3,[r2,#PCB_FLAGS_H]
	UNLOCK
	movs r0,#0x80
	lsls r0,r0,#24
	RET r7
cr_nodef
	movs r0,#0
	movs r7,#0
	LOCK
cr1
	adds r0,r0,#4
	cmp r0,#SC_MAX_CONNECTOR*4
	beq cr2
	ldr r1,[r3,r0]	; get slot
	cmp r1,#1	; used before ?
	blo cr3	; < 1 => free, finished searching
	bne cr1	; != 1 => currently used
	cmp r7,#0	; already one slot saved ?
	bne cr1	; != 0 , yes
	movs r7,r0	; save slot-index
	b cr1
cr2
	adds r0,r7,#0	; get saved slot-index (and test)
	beq cr_err1	; none found => error
cr3
	str r2,[r3,r0]	; save pcb in slot
	ldrh r3,[r2,#PCB_FLAGS_H]
	movs r1,#1
	lsls r1,r1,#PCB_FLG_IS_CONNECTOR_POS
	orrs r3,r3,r1
	strh r3,[r2,#PCB_FLAGS_H]
	UNLOCK
	lsrs r0,r0,#2	; index/4
	adds r0,#0x80
	lsls r0,r0,#24
	RET r7
cr_err1
	UNLOCK
	ldr r0,cr_lit1
	b cr_err_cont
cr_err0
	UNLOCK
cr_err
	movs r1,r2
	ldr r0,cr_lit0
cr_err_cont
	bl sc_sysError
	ALIGN
cr_sc_connectors
	DCD sc_connectors
cr_sciopta
	DCD sciopta
cr_lit0
	DCD (0x60<<24)|(0x031<<12)|0x01
cr_lit1
	DCD (0x60<<24)|(0x032<<12)|0x01
	SC_ENDFUNC sc_connectorRegister
;
;**************************************
; void __CODE sc_connectorUnregister(void)
;**************************************
;
	XSC_TFUNC sc_connectorUnregister,-16
	SC_TFUNC sc_connectorUnregister
	ldr r3,cur_sciopta
	ldr r2,[r3,#SC_CURPCB]
	ldrh r1,[r2,#PCB_FLAGS_H]
	lsrs r0,r1,#PCB_FLG_IS_CONNECTOR_POS+1
	bcc cur_err
	ldr r3,cur_sc_connectors
	movs r1,#SC_MAX_CONNECTOR
cur0
	ldr r0,[r3,#4]
	adds r3,r3,#4
	subs r1,r1,#1
	bmi cur_err
	cmp r0,r2
	bne cur0
	movs r0,#1
	str r0,[r3,#0]
	lsls r0,r0,#PCB_FLG_IS_CONNECTOR_POS
	mov r3,lr
	LOCK
	ldrh r1,[r2,#PCB_FLAGS_H]
	bics r1,r1,r0
	strh r1,[r2,#PCB_FLAGS_H]
	UNLOCK
	bx r3
cur_err
	push {lr}
	ldr r0,cur_lit0
	movs r1,#0
	bl sc_sysError
	EXIT
	ALIGN
cur_sciopta
	DCD sciopta
cur_sc_connectors
	DCD sc_connectors-4
cur_lit0
	DCD (0x61<<24)|0x01
	SC_ENDFUNC sc_connectorUnregister
	ENDIF
;
; +Revision: 1.6 +
; +Date: 2008/11/12 10:44:10 +
;
; XXX: Must write special functions to cope additional register
; push for SWI
	IF SC_USE_TRAP = 1
	SC_FUNC sc_sysCall
syscall_tab
	DCD 0
	DCD sc_msgAlloc
	DCD sc_msgFree
	DCD sc_msgAddrGet
	DCD sc_msgSndGet
	DCD sc_msgSizeGet
	DCD sc_msgSizeSet
	DCD sc_msgOwnerGet
	DCD sc_msgTx
	DCD sc_msgTxAlias
	DCD sc_msgRx
	DCD sc_msgPoolIdGet
	DCD sc_msgAcquire
	DCD sc_msgAllocClr
	IF SC_MSG_HOOK = 1
	DCD sc_msgHookRegister
	ELSE
	DCD 0
	ENDIF
	DCD 0	; 0xf
	DCD 0	; XXX: sc_poolCreate
	DCD sc_poolReset
	DCD sc_poolKill
	DCD sc_poolInfo
	DCD sc_poolDefault
	DCD sc_poolIdGet
	DCD 0	; sc_sysPoolKill
	IF SC_POOL_HOOK = 1
	DCD sc_poolHookRegister
	ELSE
	DCD 0
	ENDIF
	IF SC_ERR_HOOK = 1
	DCD sc_miscErrorHookRegister
	ELSE
	DCD 0	; 18
	ENDIF
	DCD sc_miscKerneldRegister
	DCD sc_miscCrcContd
	DCD sc_miscCrc
	DCD sc_miscErrnoSet
	DCD sc_miscErrnoGet
	DCD sc_procWakeupEnable
	DCD sc_procWakeupDisable
	DCD sc_procPrioGet
	DCD sc_procPrioSet
	DCD sc_procSliceGet
	DCD sc_procSliceSet
	DCD sc_procIdGet
	DCD sc_procPpidGet
	DCD sc_procNameGet
	DCD sc_procStart
	DCD sc_procStop
	DCD sc_procVarInit
	DCD sc_procSchedUnlock
	DCD 0	; sc_procPrioCreateStatic
	DCD 0	; sc_procIntCreateStatic
	DCD 0	; sc_procTimCreateStatic
	DCD 0	; sc_procUsrintCreateStatic
	DCD 0	; XXX: sc_procPrioCreate
	DCD 0	; XXX: sc_procIntCreate
	DCD 0	; XXX: sc_procTimCreate
	DCD 0	; XXX: sc_procUsrIntCreate
	DCD sc_procKill
	DCD sc_procYield
	DCD sc_procObserve
	DCD 0	; sc_sysProcCreate
	DCD sc_procSchedLock
	DCD sc_procVarGet
	DCD sc_procVarSet
	DCD sc_procVarDel
	DCD sc_procVarRm
	DCD sc_procUnobserve
	DCD sc_procPathGet
	DCD sc_procPathCheck
	IF SC_PROC_HOOK = 1
	DCD sc_procHookRegister
	ELSE
	DCD 0	; 3f
	ENDIF
	DCD 0	; XXX: sc_moduleCreate
	DCD sc_moduleKill
	DCD sc_moduleNameGet
	DCD sc_moduleIdGet
	DCD sc_moduleInfo
	DCD sc_sysModulePrioSet
	DCD sc_sysModulePrioGet
	IF SC_USE_FRIENDS = 1
	DCD sc_moduleFriendAdd
	DCD sc_moduleFriendRm
	DCD sc_moduleFriendGet
	DCD sc_moduleFriendNone
	DCD sc_moduleFriendAll
	ELSE
	DCD 0
	DCD 0
	DCD 0
	DCD 0
	DCD 0
	ENDIF	; SC_USE_FRIENDS = 1
	DCD 0	; 4c
	DCD 0	; 4d
	DCD sc_procDaemonUnregister
	DCD sc_procDaemonRegister
	DCD sc_triggerValueSet
	DCD sc_triggerValueGet
	DCD sc_trigger
	DCD sc_triggerWait
	DCD 0	; sc_sysError
	DCD sc_miscError
	DCD 0	; 56
	DCD sc_tick
	IF SC_ASYNC_TMO = 1
	DCD sc_tmoAdd
	ELSE
	DCD 0
	ENDIF
	DCD 0
	DCD sc_sleep
	IF SC_ASYNC_TMO = 1
	DCD sc_tmoRm
	ELSE
	DCD 0
	ENDIF
	DCD sc_tickGet
	DCD sc_tickLength
	DCD sc_tickMs2Tick
	DCD sc_tickTick2Ms
	IF SC_MAX_CONNECTOR > 0
	DCD sc_connectorRegister
	DCD sc_connectorUnregister
	ELSE
	DCD 0
	DCD 0
	ENDIF
	DCD 0	; 62 (dispatcher)
	DCD 0
	IF SC_CDEBUG = 1
xsyscall_tab
	DCD 0
	DCD xxsc_msgAlloc
	DCD xsc_msgFree
	DCD xsc_msgAddrGet
	DCD xsc_msgSndGet
	DCD xsc_msgSizeGet
	DCD xsc_msgSizeSet
	DCD xsc_msgOwnerGet
	DCD xsc_msgTx
	DCD xxsc_msgTxAlias
	DCD xsc_msgRx
	DCD xsc_msgPoolIdGet
	DCD xsc_msgAcquire
	DCD xxsc_msgAllocClr
	IF SC_MSG_HOOK = 1
	DCD xsc_msgHookRegister
	ELSE
	DCD 0
	ENDIF
	DCD 0	; 0xf
	DCD 0	; XXX: xsc_poolCreate
	DCD xsc_poolReset
	DCD xsc_poolKill
	DCD xsc_poolInfo
	DCD xsc_poolDefault
	DCD xsc_poolIdGet
	DCD 0	; sc_sysPoolKill
	IF SC_POOL_HOOK = 1
	DCD xsc_poolHookRegister
	ELSE
	DCD 0
	ENDIF
	IF SC_ERR_HOOK = 1
	DCD xsc_miscErrorHookRegister
	ELSE
	DCD 0	; 18
	ENDIF
	DCD xsc_miscKerneldRegister
	DCD xsc_miscCrcContd
	DCD xsc_miscCrc
	DCD xsc_miscErrnoSet
	DCD xsc_miscErrnoGet
	DCD xsc_procWakeupEnable
	DCD xsc_procWakeupDisable
	DCD xsc_procPrioGet
	DCD xsc_procPrioSet
	DCD xsc_procSliceGet
	DCD xsc_procSliceSet
	DCD xsc_procIdGet
	DCD xsc_procPpidGet
	DCD xsc_procNameGet
	DCD xsc_procStart
	DCD xsc_procStop
	DCD xsc_procVarInit
	DCD xsc_procSchedUnlock
	DCD 0	; sc_procPrioCreateStatic
	DCD 0	; sc_procIntCreateStatic
	DCD 0	; sc_procTimCreateStatic
	DCD 0	; sc_procUsrintCreateStatic
	DCD 0	; XXX: xsc_procPrioCreate
	DCD 0	; XXX: xsc_procIntCreate
	DCD 0	; XXX: xsc_procTimCreate
	DCD 0	; XXX: xsc_procUsrIntCreate
	DCD xsc_procKill
	DCD xsc_procYield
	DCD xsc_procObserve
	DCD 0	; sc_sysProcCreate
	DCD xsc_procSchedLock
	DCD xsc_procVarGet
	DCD xsc_procVarSet
	DCD xsc_procVarDel
	DCD xsc_procVarRm
	DCD xsc_procUnobserve
	DCD xsc_procPathGet
	DCD xsc_procPathCheck
	IF SC_PROC_HOOK = 1
	DCD xsc_procHookRegister
	ELSE
	DCD 0	; 3f
	ENDIF
	DCD 0	; XXX: xsc_moduleCreate
	DCD xsc_moduleKill
	DCD xsc_moduleNameGet
	DCD xsc_moduleIdGet
	DCD xsc_moduleInfo
	DCD xsc_sysModulePrioSet
	DCD xsc_sysModulePrioGet
	IF SC_USE_FRIENDS = 1
	DCD xsc_moduleFriendAdd
	DCD xsc_moduleFriendRm
	DCD xsc_moduleFriendGet
	DCD xsc_moduleFriendNone
	DCD xsc_moduleFriendAll
	ELSE
	DCD 0
	DCD 0
	DCD 0
	DCD 0
	DCD 0
	ENDIF	; SC_USE_FRIENDS = 1
	DCD 0	; 4c
	DCD 0	; 4d
	DCD xsc_procDaemonUnregister
	DCD xsc_procDaemonRegister
	DCD xsc_triggerValueSet
	DCD xsc_triggerValueGet
	DCD xsc_trigger
	DCD xsc_triggerWait
	DCD 0	; sc_sysError
	DCD xsc_miscError
	DCD 0	; 56
	DCD xsc_tick
	IF SC_ASYNC_TMO = 1
	DCD xsc_tmoAdd
	ELSE
	DCD 0
	ENDIF
	DCD 0
	DCD xsc_sleep
	IF SC_ASYNC_TMO = 1
	DCD xsc_tmoRm
	ELSE
	DCD 0
	ENDIF
	DCD xsc_tickGet
	DCD xsc_tickLength
	DCD xsc_tickMs2Tick
	DCD xsc_tickTick2Ms
	IF SC_MAX_CONNECTOR > 0
	DCD xsc_connectorRegister
	DCD xsc_connectorUnregister
	ELSE
	DCD 0
	DCD 0
	ENDIF
	DCD 0	; 62 (dispatcher)
	DCD 0
	ENDIF
	SC_ENDFUNC sc_sysCall
	IF SC_CDEBUG = 1
	XXSC_TFUNC sc_msgAlloc,0
	XXSC_TFUNC sc_msgAllocClr,0
	XXSC_TFUNC sc_msgTxAlias,0
	ENDIF
	ENDIF
;
; IAR specific stuff
;
; ID: S05164BS1
; +Date: 2008/11/12 10:44:10 +
; +Revision: 1.6 +
;
	END 
