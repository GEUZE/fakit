#ifndef NAND_H_
#define NAND_H_


#define IOWR_NAND_CE(data)              IOWR(NAND_FLASH_INTERFACE_0_BASE, 0, data)
#define IOWR_NAND_READ(data)            IOWR(NAND_FLASH_INTERFACE_0_BASE, 1, data)
#define IOWR_NAND_WRITE(data)           IOWR(NAND_FLASH_INTERFACE_0_BASE, 2, data)
#define IOWR_NAND_ADDRESS_LATCH(data)   IOWR(NAND_FLASH_INTERFACE_0_BASE, 3, data)
#define IOWR_NAND_COMMAND_LATCH(data)   IOWR(NAND_FLASH_INTERFACE_0_BASE, 4, data)
#define IOWR_NAND_WP(data)              IOWR(NAND_FLASH_INTERFACE_0_BASE, 5, data)
#define IOWR_NAND_DATA(data)            IOWR(NAND_FLASH_INTERFACE_0_BASE, 6, data)
#define IORD_NAND_DATA                  IORD(NAND_FLASH_INTERFACE_0_BASE, 6)
#define IORD_NAND_RB                    IORD(NAND_FLASH_INTERFACE_0_BASE, 7)
#endif /*NAND_H_*/
