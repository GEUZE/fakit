/*SOC
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
** (c) 2002..2004 Sciopta Systems GmbH, Litronic AG/ Schweiz        **
**                                                                  **
**********************************************************************
** ID: S05022BS1                                                    **
** +Revision: 1.2 +                                                 **
** +Date: 2005/02/16 06:43:17 +                                     **
** Defines for cs8900a                                              **
**********************************************************************
EOC*/

#ifndef _CS8900A_H_
#define _CS8900A_H_ 1

/* 
** CS8900 Packed Page mapped registers. 
*/
#define CS_DATA            0 /* Data port 0 */
#define CS_DATA_HIGH       2 /* Data port 1 */
#define CS_TXCMD           4 /* Transmit Command */
#define CS_TXLENGTH        6 /* Transmit length */
#define CS_ISQ             8 /* Interrupt Status Queue */
#define CS_PP_PTR         10 /* Packet Page Pointer */
#define CS_PP_DATA        12 /* Packet Page Data 0 */
#define CS_PP_DATA_HIGH   14 /* Packet Page Data 1 */

#define CS_PP_PTR_AUTOINC 0x8000 /* auto increment packet page ptr */

/* Bus Interface Registers. */
#define PP_MANID         0x0000       /* Manufacture ID. */
#define PP_PRODID        0x0002       /* Product ID. */
#define PP_IO_BASE       0x0020
#define PP_ISAINT        0x0022        /*  ISA interrupt select. */
#define PP_DMA_CHANNEL   0x0024
#define PP_DMA_SOF       0x0026        /* Start Of Frame */
#define PP_DMA_LOF       0x0028        /* Length Of Frame */
#define PP_RXDMA_LENGTH  0x002a
#define PP_MEMORYBASE    0x002c
#define PP_ISABOOTBASE   0x0030         /*  Boot Prom base. */
#define PP_ISABOOTMASK   0x0034         /*  Boot Prom Mask. */

/* EEPROM data and command registers. */
#define PP_EE_CMD        0x0040         /*  NVR Interface Command register. */
#define PP_EE_DATA       0x0042         /*  NVR Interface Data Register. */

#define PP_RXDMA_LENGTH_2  0x0050         /* receive frame count */

/* Status and Control Registers. */
#define PP_CNF_CTRL      0x0100
#define PP_RX_CFG        0x0102        /*  Rx Bus config. */
#define PP_RX_CTL        0x0104        /*  Receive Control Register. */
#define PP_TX_CFG        0x0106        /*  Transmit Config Register. */
#define PP_TX_CMD        0x0108        /*  Transmit Command Register. */
#define PP_BUF_CFG       0x010A        /*  Bus configuration Register. */
#define PP_LINE_CTL      0x0112        /*  Line Config Register. */
#define PP_SELF_CTL      0x0114        /*  Self Command Register. */
#define PP_BUS_CTL       0x0116        /*  ISA bus control Register. */
#define PP_TEST_CTL      0x0118        /*  Test Register. */

#define PP_ISQ           0x0120        /*  Interrupt Status. */
#define PP_RXEVENT       0x0124        /*  Rx Event Register. */
#define PP_TXEVENT       0x0128        /*  Tx Event Register. */
#define PP_BUFEVENT      0x012C        /*  Bus Event Register. */
#define PP_RXMISS        0x0130        /*  Receive Miss Count. */
#define PP_TXCOL         0x0132        /*  Transmit Collision Count. */
#define PP_LINEST        0x0134        /*  Line State Register. */
#define PP_SELFST        0x0136        /*  Self State register. */
#define PP_BUSST         0x0138        /*  Bus Status. */

/* Initiate Transmit Registers (Packed Page memory locations). */
#define PP_TXCOMMAND     0x0144        /*  Tx Command. */
#define PP_TXLENGTH      0x0146        /*  Tx Length. */

/* Address Filter Registers. */
#define PP_LAF           0x0150        /*  Hash Table. */
#define PP_IA            0x0158        /*  Physical Address Register. */

/* Frame Location. */
#define PP_RXSTATUS      0x0400        /*  Receive start of frame. */
#define PP_RXLENGTH      0x0402        /*  Receive Length of frame. */
#define PP_RXFRAME       0x0404        /*  Receive frame pointer. */
#define PP_TXFRAME       0x0A00        /*  Transmit frame pointer. */

/* PP_CHIPID. */
#define CS8900 0x0000 /* Product ID. */
#define REVISON_BITS 0x1F00

/* PP_RXCFG - Receive  Configuration and Interrupt Mask bit definition -  Read/write. */
#define SKIP_1               0x0040
#define RX_STREAM_ENBL       0x0080
#define RX_OK_ENBL           0x0100
#define BUFFER_CRC           0x0800
#define RX_CRC_ERROR_ENBL    0x1000
#define RX_RUNT_ENBL         0x2000
#define RX_EXTRA_DATA_ENBL   0x4000

/* PP_RxCTL - Receive Control bit definition - Read/write. */
#define RX_IA_HASH_ACCEPT    0x0040
#define RX_PROM_ACCEPT       0x0080
#define RX_OK_ACCEPT         0x0100
#define RX_MULTCAST_ACCEPT   0x0200
#define RX_IA_ACCEPT         0x0400
#define RX_BROADCAST_ACCEPT  0x0800
#define RX_BAD_CRC_ACCEPT    0x1000
#define RX_RUNT_ACCEPT       0x2000
#define RX_EXTRA_DATA_ACCEPT 0x4000
#define RX_ALL_ACCEPT \
   (RX_PROM_ACCEPT|RX_BAD_CRC_ACCEPT|RX_RUNT_ACCEPT|RX_EXTRA_DATA_ACCEPT)

/*  Default receive mode - individually addressed, broadcast, and error free. */
#define DEF_RX_ACCEPT (RX_IA_ACCEPT | RX_BROADCAST_ACCEPT | RX_OK_ACCEPT)

/* PP_TxCFG - Transmit Configuration Interrupt Mask bit definition - Read/write. */
#define TX_LOST_CRS_ENBL     0x0040
#define TX_SQE_ERROR_ENBL    0x0080
#define TX_OK_ENBL           0x0100
#define TX_LATE_COL_ENBL     0x0200
#define TX_JBR_ENBL          0x0400
#define TX_ANY_COL_ENBL      0x0800
#define TX_16_COL_ENBL       0x8000

/* PP_TxCMD - Transmit Command bit definition - Read-only. */
#define TX_FORCE             0x0100
#define TX_ONE_COL           0x0200
#define TX_TWO_PART_DEFF_DISABLE 0x0400
#define TX_NO_CRC            0x1000
#define TX_RUNT              0x2000

/* PP_BufCFG - Buffer Configuration Interrupt Mask bit definition - Read/write. */
#define GENERATE_SW_INTERRUPT 0x0040
#define RX_DMA_ENBL 0x0080
#define READY_FOR_TX_ENBL 0x0100
#define TX_UNDERRUN_ENBL 0x0200
#define RX_MISS_ENBL 0x0400
#define RX_128_BYTE_ENBL 0x0800
#define TX_COL_COUNT_OVRFLOW_ENBL 0x1000
#define RX_MISS_COUNT_OVRFLOW_ENBL 0x2000
#define RX_DEST_MATCH_ENBL 0x8000

/* PP_LineCTL - Line Control bit definition - Read/write. */
#define TENBASET 0
#define SERIAL_RX_ON 0x0040
#define SERIAL_TX_ON 0x0080
#define AUI_ONLY 0x0100
#define AUTO_AUI_10BASET 0x0200
#define MODIFIED_BACKOFF 0x0800
#define NO_AUTO_POLARITY 0x1000
#define TWO_PART_DEFDIS 0x2000
#define LOW_RX_SQUELCH 0x4000

/* PP_SelfCTL - Software Self Control bit definition - Read/write. */
#define POWER_ON_RESET 0x0040
#define SW_STOP 0x0100
#define SLEEP_ON 0x0200
#define AUTO_WAKEUP 0x0400
#define HCB0_ENBL 0x1000
#define HCB1_ENBL 0x2000
#define HCB0 0x4000
#define HCB1 0x8000

/* PP_BusCTL - ISA Bus Control bit definition - Read/write. */
#define RESET_RX_DMA 0x0040
#define MEMORY_ON 0x0400
#define DMA_BURST_MODE 0x0800
#define IO_CHANNEL_READY_ON 0x1000
#define RX_DMA_SIZE_64K 0x2000
#define ENABLE_IRQ 0x8000

/* PP_TestCTL - Test Control bit definition - Read/write. */
#define LINK_OFF 0x0080
#define ENDEC_LOOPBACK 0x0200
#define AUI_LOOPBACK 0x0400
#define BACKOFF_OFF 0x0800
#define FAST_TEST 0x8000

/* PP_RxEvent - Receive Event Bit definition - Read-only. */
#define RX_IA_HASHED 0x0040
#define RX_DRIBBLE 0x0080
#define RX_OK 0x0100
#define RX_HASHED 0x0200
#define RX_IA 0x0400
#define RX_BROADCAST 0x0800
#define RX_CRC_ERROR 0x1000
#define RX_RUNT 0x2000
#define RX_EXTRA_DATA 0x4000

#define HASH_INDEX_MASK 0x0FC00

/* PP_TxEvent - Transmit Event Bit definition - Read-only. */
#define TX_LOST_CRS 0x0040
#define TX_SQE_ERROR 0x0080
#define TX_OK 0x0100
#define TX_LATE_COL 0x0200
#define TX_JBR 0x0400
#define TX_16_COL 0x8000
#define TX_SEND_OK_BITS (TX_OK|TX_LOST_CRS)
#define TX_COL_COUNT_MASK 0x7800

/* PP_BufEvent - Buffer Event Bit definition - Read-only. */
#define SW_INTERRUPT 0x0040
#define RX_DMA 0x0080
#define READY_FOR_TX 0x0100
#define TX_UNDERRUN 0x0200
#define RX_MISS 0x0400
#define RX_128_BYTE 0x0800
#define TX_COL_OVRFLW 0x1000
#define RX_MISS_OVRFLW 0x2000
#define RX_DEST_MATCH 0x8000

/* PP_LineST - Ethernet Line Status bit definition - Read-only. */
#define LINK_OK 0x0080
#define AUI_ON 0x0100
#define TENBASET_ON 0x0200
#define POLARITY_OK 0x1000
#define CRS_OK 0x4000

/* PP_SelfST - Chip Software Status bit definition. */
#define ACTIVE_33V 0x0040
#define INIT_DONE 0x0080
#define SI_BUSY 0x0100
#define EEPROM_PRESENT 0x0200
#define EEPROM_OK 0x0400
#define EL_PRESENT 0x0800
#define EE_SIZE_64 0x1000

/* PP_BusST - ISA Bus Status bit definition. */
#define TX_BID_ERROR 0x0080
#define READY_FOR_TX_NOW 0x0100

/*  The following block defines the ISQ event types. */
#define ISQ_RECEIVER_EVENT 0x04
#define ISQ_TRANSMITTER_EVENT 0x08
#define ISQ_BUFFER_EVENT 0x0c
#define ISQ_RX_MISS_EVENT 0x10
#define ISQ_TX_COL_EVENT 0x12

#define ISQ_EVENT_MASK 0x003F   /*  ISQ mask to find out type of event. */
#define ISQ_HIST 16             /*  small history buffer. */
#define AUTOINCREMENT 0x8000    /*  Bit mask to set bit-15 for autoincrement. */

#define TXRXBUFSIZE 0x0600
#define RXDMABUFSIZE 0x8000
#define RXDMASIZE 0x4000
#define TXRX_LENGTH_MASK 0x07FF

#define TX_NOW          0x0000       /*  Tx packet after   5 bytes copied. */
#define TX_AFTER_381    0x0040       /*  Tx packet after 381 bytes copied. */
#define TX_AFTER_1021    0x0080       /*  Tx packet after 381 bytes copied. */
#define TX_AFTER_ALL    0x00C0       /*  Tx packet after all bytes copied. */

#define EEPROM_WRITE_EN         0x00F0
#define EEPROM_WRITE_DIS        0x0000
#define EEPROM_WRITE_CMD        0x0100
#define EEPROM_READ_CMD         0x0200

#define CHKSUM_LEN 0x14
#define CHKSUM_VAL 0x0000
#define START_EEPROM_DATA 0x001c /*  Offset into eeprom for start of data. */

/*  Byte offsets into the EEPROM configuration buffer. */
#define ISA_CNF_OFFSET 0x6
#define TX_CTL_OFFSET (ISA_CNF_OFFSET + 8) /*  8900 eeprom. */

#define EE_FORCE_FDX  0x8000
#define EE_NLP_ENABLE 0x0200
#define EE_AUTO_NEG_ENABLE 0x0100
#define EE_ALLOW_FDX 0x0080
#define EE_AUTO_NEG_CNF_MASK (EE_FORCE_FDX|EE_NLP_ENABLE|EE_AUTO_NEG_ENABLE|EE_ALLOW_FDX)

#define IMM_BIT 0x0040          /*  Ignore missing media. */

#define ADAPTER_CNF_OFFSET (TX_CTL_OFFSET + 2)
#define A_CNF_10B_T 0x0001
#define A_CNF_MEDIA_AUTO 0x0000
#define A_CNF_MEDIA_10B_T 0x0020
#define A_CNF_DC_DC_POLARITY 0x0080
#define A_CNF_NO_AUTO_POLARITY 0x2000
#define A_CNF_LOW_RX_SQUELCH 0x4000
#define A_CNF_EXTND_10B_2 0x8000

#ifndef __ASM_INCLUDED__
typedef struct cs8900io_s {
  __u16 rxtx_data;
  __u16 rxtx_data_high;
  __u16 txcmd;
  __u16 txlength;
  __u16 isq;
  __u16 pp_ptr;
  __u16 pp_data;
  __u16 pp_data_high;
} cs8900io_t;

typedef struct cs8900mem_s {
  __u32 ProductIdentificationCode;   /* 0x0000 */
  __u8  reserved1[28];               /* 0x0004 */
  __u16 IOBaseAddress;               /* 0x0020 */
  __u16 InterruptNumber;             /* 0x0022 */
  __u16 DMAChannelNumber;            /* 0x0024 */
  __u16 DMAStartOfFrame;             /* 0x0026 */
  __u16 DMAFrameCount;               /* 0x0028 */
  __u16 RxDMAByteCount;              /* 0x002A */
  __u32 MemBaseAddress;              /* 0x002C */
  __u32 BootPROMBaseAddress;         /* 0x0030 */
  __u32 BootPROMAddressMask;         /* 0x0034 */
  __u8  reserved2[8];                /* 0x0038 */
  __u16 EEPROMCommand;               /* 0x0040 */
  __u16 EEPROMData;                  /* 0x0042 */
  __u8  reserved3[12];               /* 0x0044 */
  __u16 ReceivedFrameByteCounter;    /* 0x0050 */
  __u8  reserved4[174];              /* 0x0052 */
/* Status and Contol Register */
/*   Configuration and Control Register */
  __u16 reserved5;                    /* 0x0100 */
  __u16 RxCFG;                        /* 0x0102 */
  __u16 RxCTL;                        /* 0x0104 */
  __u16 TxCFG;                        /* 0x0106 */
  __u16 TxCMD;                        /* 0x0108 */
  __u16 BufCFG;                       /* 0x010A */
  __u16 reserved6[3];                 /* 0x010C */
  __u16 LineCTL;                      /* 0x0112 */
  __u16 SelfCTL;                      /* 0x0114 */
  __u16 BusCTL;                       /* 0x0116 */
  __u16 TestCTL;                      /* 0x0118 */
  __u16 reserved7[3];                 /* 0x011A */
/*   Status and Event Register */
  __u16 ISQ;                          /* 0x0120 */
  __u16 reserved8;                    /* 0x0122 */
  __u16 RxEVENT;                      /* 0x0124 */
  __u16 reserved9;                    /* 0x0126 */
  __u16 TxEVENT;                      /* 0x0128 */
  __u16 reserved10;                   /* 0x012A */
  __u16 BufEvent;                     /* 0x012C */
  __u16 reserved11;                   /* 0x012E */
  __u16 RxMISS;                       /* 0x0130 */
  __u16 TxCOL;                        /* 0x0132 */
  __u16 LineST;                       /* 0x0134 */
  __u16 SelfST;                       /* 0x0136 */
  __u16 BusST;                        /* 0x0138 */
  __u16 reserved12;                   /* 0x013A */
  __u16 TDR;                          /* 0x013C */
  __u16 reserved13;                   /* 0x013E */
/* Initiate Transmit Register */
  __u16 reserved14[2];                /* 0x0140 */
  __u16 TxCMD2;                       /* 0x0144 */
  __u16 TxLength;                     /* 0x0146 */
  __u16 reserved15[4];                /* 0x0148 */
/* Address Filter Register */
  __u8 LogicalAddressFilter[8];       /* 0x0150 */
  __u8 IndividualAddress[6];          /* 0x0158 */
  __u8 reserved16[418];               /* 0x015E */
/* IO Interface mapped to this address */
  cs8900io_t IO;                      /* 0x0300 */
  __u8 reserved17[240];               /* 0x0310 */
/* Frame Location */
  __u16 RxStatus;                     /* 0x0400 */
  __u16 RxLength;                     /* 0x0402 */
  __u8 ReceiveFrameLocation[1532];    /* 0x0404 */
  __u8 TransmitFrameLocation[1536];   /* 0x0A00 */
} cs8900mem_t;


int cs8900a_init(char *mac);
int cs8900a_alloc(int length);
void cs8900a_ei();
void cs8900a_di();

#endif /*  __ASM_INCLUDED__ */

#endif /* _CS8900A_H_ */
