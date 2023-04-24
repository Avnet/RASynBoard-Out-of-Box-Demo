/*****************************************************************************
 *
 *	Copyright(c) AVNET, 2023. All rights reserved.
 *
 *****************************************************************************/
#ifndef _NDP_FLASH_H_
#define _NDP_FLASH_H_

/* mspi ssb */
#define MSPI_FLASH_SSB         0
#define MSPI_IMU_SSB             1
/* The address of storing */
#define FLASH_MCU_ADDR                  0x0
#define FLASH_MCU_LENGTH                21612
#define FLASH_DSP_ADDR                  0x5800
#define FLASH_DSP_LENGTH                84828
#define FLASH_NN_ADDR                   0x1a400
#define FLASH_NN_LENGTH                 358756

/* Commands for Dialog AT25XE161D */
/* Read Commands */
#define FLASH_READ_ARRAY				0x03
#define FLASH_FAST_READ_ARRAY			0x0B
/** Program/Erase Commands */
#define FLASH_PAGE_ERASE				0x81
#define FLASH_4KB_BLOCK_ERASE			0x20
#define FLASH_32KB_BLOCK_ERASE			0x52
#define FLASH_64KB_BLOCK_ERASE			0xD8
#define FLASH_CHIP_ERASE				0xC7
#define FLASH_PAGE_PROGRAM				0x02
#define FLASH_SEQ_PROGRAM				0xAD
#define FLASH_PROGRAM_SUSPEND			0x75
#define FLASH_PROGRAM_RESUME			0x7A
/* Protection Commands */
#define FLASH_WRITE_ENABLE				0x06
#define FLASH_WRITE_DISABLE				0x04
/* Status Register Commands */
#define FLASH_READ_STATUS_REG1			0x05
#define FLASH_READ_STATUS_REG2			0x35
#define FLASH_READ_STATUS_REG3			0x15
#define FLASH_READ_ALLSTATUS			0x65
#define FLASH_WRITE_STATUS_REG1			0x01
#define FLASH_WRITE_STATUS_REG2			0x31
#define FLASH_WRITE_STATUS_REG3			0x11
#define FLASH_WRITE_ALLSTATUS			0x71
#define FLASH_STATUS_LOCK				0x6F
/* Reset Commands */
#define FLASH_RESET_ENABLE				0x66
#define FLASH_RESET_DEVICE				0x99
#define FLASH_TERMINATE					0xF0
/* Manufacturer/Device Commands */
#define FLASH_DEVICE_ID					0x90
#define FLASH_JEDEC_ID					0x9F	

#define FLASH_MAP_TOP					0x1FFFFF
#define FLASH_MAP_BOTTOM				0x000000
#define FLASH_PAGE_SIZE					0x100
#define FLASH_BLOCK_4KB_SIZE			0x1000
#define FLASH_PAGE_32KB_SIZE			0x8000
#define FLASH_PAGE_64KB_SIZE			0x10000

#define FLASH_FW_INFO_ADDR            0x1FD00

enum SPI_FLASH_TRANSFER_TYPE {
	SPI_FLASH_WRITE  = 1,
	SPI_FLASH_READ  = 2,
	SPI_FLASH_TYPE_NONE  = 3,
};

int ndp_flash_init(void);
uint16_t ndp_flash_get_deviceid(void);
uint32_t ndp_flash_get_JEDEC_ID(void);

int ndp_flash_fast_read(uint32_t address, uint8_t *buff, uint32_t length);
int ndp_flash_page_erase(uint32_t address);
int ndp_flash_4kblock_erase(uint32_t address);
int ndp_flash_32kblock_erase(uint32_t address);
int ndp_flash_64kblock_erase(uint32_t address);
int ndp_flash_chip_erase(void);

int ndp_flash_read_page(uint32_t address, uint8_t *buff, uint32_t length);
int ndp_flash_read_block(uint32_t address, uint8_t *buff, uint32_t length);
int ndp_flash_write_page(uint32_t address, uint8_t *buff, uint32_t length);
int ndp_flash_write_block(uint32_t address, uint8_t *buff, uint32_t length);

void ndp_flash_print_data(uint32_t address, uint32_t count);
int ndp_flash_program_all_fw(void);

#endif
