/*****************************************************************************
 *
 *	Copyright(c) AVNET, 2023. All rights reserved.
 *
 *	Description: ndp_flash.c
 *
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "syntiant_platform.h"
#include "ndp_flash.h"
#include <string.h>

#define SYNTIANT_NDP_ERROR_NONE  0
#define DUMMY_0_BYTES			0
#define DUMMY_1_BYTES			1
#define DUMMY_2_BYTES			2
#define DUMMY_3_BYTES			3

#define GPIO_PIN_6              6
#define GPIO_PIN_7              7
#define GPIO_LEVEL_LOW          0
#define GPIO_LEVEL_HIGH         1
#define FLASH_PIN_HOLD          GPIO_PIN_7
#define FLASH_PIN_WP            GPIO_PIN_6

#define ndp_flash_write_enable()	ndp_flash_spi_transfer(FLASH_WRITE_ENABLE, \
										NULL, 0, DUMMY_0_BYTES, NULL, 0, SPI_FLASH_WRITE)
#define ndp_flash_write_disable()	ndp_flash_spi_transfer(FLASH_WRITE_DISABLE, \
										NULL, 0, DUMMY_0_BYTES, NULL, 0, SPI_FLASH_WRITE)
#define ndp_flash_get_ready()		ndp_flash_get_status(1)&0x01

extern void syntiant_ndp_delay_us(int ms_time);


char* flash_process_percent(uint32_t processed_len, uint32_t planed_len, 
        uint32_t process_size)
{
    uint32_t quarter_len = planed_len>>2;
    uint32_t half_len = planed_len>>1;
    uint32_t most_quarter_len = quarter_len*3;

    if ((processed_len - most_quarter_len) < process_size) {
        return "...75%";
    }
    else if ((processed_len - half_len) < process_size) {
        return "...50%";
    }
    else if ((processed_len - quarter_len) < process_size) {
        return "...25%";
    }
    else
        return NULL;
}

int ndp_flash_init(void)
{
    int ret;
    ret = ndp_core2_platform_tiny_mspi_config();
    if (ret) {
        printf("**** mspi init failed: %d\n", ret);
    }
	/* set HOLD pin */
	ret = ndp_core2_platform_tiny_gpio_config(FLASH_PIN_HOLD, NDP_CORE2_CONFIG_VALUE_GPIO_DIR_OUT, GPIO_LEVEL_HIGH);
	if (ret) {
        printf("**** set gpio failed: %d\n", ret);
    }
    return ret;
}

static int ndp_flash_spi_transfer(uint8_t cmd, uint8_t *addr, int addr_len,
								uint8_t dummy, uint8_t *data, int data_len, int type)
{
    int ret = SYNTIANT_NDP_ERROR_NONE;
	uint8_t send_cmd[8];
	uint8_t *pdata = data;
	uint8_t end = 1;
	enum SPI_FLASH_TRANSFER_TYPE flag = SPI_FLASH_TYPE_NONE;
	int index = 0;
	int num;
	
	send_cmd[ index++ ] = cmd;
	if (addr_len > 0) {
		for (num = 0; num < addr_len; num ++) {
			send_cmd[ index++ ] = addr[num];
		}
	}

	while (dummy > 0) {
		send_cmd[ index++ ] = 0x0f;
		dummy --;
	}

	if (data_len > 0){ /* data_len should be less than or equal to 16 */
		flag = (enum SPI_FLASH_TRANSFER_TYPE) type;
		end = 0;
	}

	ret = ndp_core2_platform_tiny_mspi_write(MSPI_FLASH_SSB, index, send_cmd, end);
	if (ret) return ret;

	end = 1;
	switch (flag) {
	case SPI_FLASH_WRITE:
		ret = ndp_core2_platform_tiny_mspi_write(MSPI_FLASH_SSB, data_len, pdata, end);
		break;
	case SPI_FLASH_READ:
		ret = ndp_core2_platform_tiny_mspi_read(MSPI_FLASH_SSB, data_len, pdata, end);
		break;
	default:
		break;
	}

    return ret;
}

static uint8_t ndp_flash_get_status(int reg)
{
    int ret;
    uint8_t addr;
	uint8_t status;

	addr = ((uint8_t)reg) & 0x7;
	ret = ndp_flash_spi_transfer(FLASH_READ_ALLSTATUS, &addr, 1, DUMMY_1_BYTES, \
						&status, 1, SPI_FLASH_READ);
	if (ret) return 0xFF;
    return status;
}

uint16_t ndp_flash_get_deviceid(void)
{
    int ret;
    uint8_t pdata[2];
    uint16_t id = 0xFFFF;

	ret = ndp_flash_spi_transfer(FLASH_DEVICE_ID, NULL, 0, DUMMY_3_BYTES, \
							pdata, 2, SPI_FLASH_READ);
	if (ret) return id;

	id = ((uint16_t)pdata[0] << 8 ) + (uint16_t)pdata [1];
    return id;
}

uint32_t ndp_flash_get_JEDEC_ID(void)
{
    int ret;
    uint8_t pdata[5];
    uint32_t id = 0xFFFF;

	ret = ndp_flash_spi_transfer(FLASH_JEDEC_ID, NULL, 0, DUMMY_0_BYTES, \
							pdata, 5, SPI_FLASH_READ);
	if (ret) return id;

	id = ((uint32_t)pdata[0] << 24 ) + ((uint32_t)pdata[1] << 16 ) + \
			((uint32_t)pdata[2] << 8 ) + (uint32_t)pdata [3];

    return id;
}

int ndp_flash_page_erase(uint32_t address)
{
    int ret;
    uint8_t paddr[3];

	paddr[0] = (address >> 16) & 0x1F;
	paddr[1] = (address >> 8) & 0xFF;
	paddr[2] = 0x00;
	
	ndp_flash_write_enable();
	ret = ndp_flash_spi_transfer(FLASH_PAGE_ERASE, paddr, sizeof(paddr), \
						DUMMY_0_BYTES, NULL, 0, SPI_FLASH_WRITE);
	while(ndp_flash_get_ready()){
		syntiant_ndp_delay_us(10000);
	}
	return ret;
}

int ndp_flash_4kblock_erase(uint32_t address)
{
    int ret;
    uint8_t paddr[3];

	paddr[0] = (address >> 16) & 0x1F;
	paddr[1] = (address >> 8) & 0xF0;
	paddr[2] = 0x00;
	
	ndp_flash_write_enable();
	ret = ndp_flash_spi_transfer(FLASH_4KB_BLOCK_ERASE, paddr, sizeof(paddr), \
						DUMMY_0_BYTES, NULL, 0, SPI_FLASH_WRITE);
    while(ndp_flash_get_ready()){
		syntiant_ndp_delay_us(10000);
	}
	return ret;
}

int ndp_flash_32kblock_erase(uint32_t address)
{
    int ret;
    uint8_t paddr[3];

	paddr[0] = (address >> 16) & 0x1F;
	paddr[1] = (address >> 8) & 0x80;
	paddr[2] = 0x00;
	
	ndp_flash_write_enable();
	ret = ndp_flash_spi_transfer(FLASH_32KB_BLOCK_ERASE, paddr, sizeof(paddr), \
						DUMMY_0_BYTES, NULL, 0, SPI_FLASH_WRITE);
    while(ndp_flash_get_ready()){
		syntiant_ndp_delay_us(10000);
	}
	return ret;
}

int ndp_flash_64kblock_erase(uint32_t address)
{
    int ret;
    uint8_t paddr[3];

	paddr[0] = (address >> 16) & 0x1F;
	paddr[1] = 0x00;
	paddr[2] = 0x00;
	
	ndp_flash_write_enable();
	ret = ndp_flash_spi_transfer(FLASH_64KB_BLOCK_ERASE, paddr, sizeof(paddr), \
						DUMMY_0_BYTES, NULL, 0, SPI_FLASH_WRITE);
	while(ndp_flash_get_ready()){
		syntiant_ndp_delay_us(10000);
	}
	return ret;
}

int ndp_flash_chip_erase(void)
{
    int ret;

	ndp_flash_write_enable();
	ret = ndp_flash_spi_transfer(FLASH_CHIP_ERASE, NULL, 0, \
						DUMMY_0_BYTES, NULL, 0, SPI_FLASH_WRITE);
	while(ndp_flash_get_ready()){
		syntiant_ndp_delay_us(10000);
	}
	return ret;
}

int ndp_flash_read_page(uint32_t address, uint8_t *buff, uint32_t length)
{
	int ret;
    uint8_t paddr[3];
	uint8_t *pdata = buff;

	if (length > FLASH_PAGE_SIZE)
		length = FLASH_PAGE_SIZE;

	paddr[0] = (address >> 16) & 0xFF;
	paddr[1] = (address >> 8) & 0xFF;
	paddr[2] = address & 0xFF;
	
	ret = ndp_flash_spi_transfer(FLASH_READ_ARRAY, paddr, sizeof(paddr), \
						DUMMY_0_BYTES, pdata, length, SPI_FLASH_READ);
	while(ndp_flash_get_ready()){
		syntiant_ndp_delay_us(10000);
	}
	return ret;
}

int ndp_flash_fast_read_page(uint32_t address, uint8_t *buff, uint32_t length)
{
	int ret;
    uint8_t paddr[3];
	uint8_t *pdata = buff;

	if (length > FLASH_PAGE_SIZE)
		length = FLASH_PAGE_SIZE;

	paddr[0] = (address >> 16) & 0xFF;
	paddr[1] = (address >> 8) & 0xFF;
	paddr[2] = address & 0xFF;
	
	ret = ndp_flash_spi_transfer(FLASH_FAST_READ_ARRAY, paddr, sizeof(paddr), \
						DUMMY_1_BYTES, pdata, length, SPI_FLASH_READ);
	while(ndp_flash_get_ready()){
		syntiant_ndp_delay_us(10000);
	}
	return ret;
}

int ndp_flash_read_block(uint32_t address, uint8_t *buff, uint32_t length)
{
	int ret = -1;
	uint32_t page;
	uint32_t index = 0;
	uint32_t read_addr = address;
	uint8_t *pdata = buff;

    while (index < length) {
		if ((length-index) > FLASH_PAGE_SIZE) {
			page = FLASH_PAGE_SIZE;
		} else {
			page = length-index;
		}

		ret = ndp_flash_read_page(read_addr, pdata, page);
		if (ret) return ret;

        index += page;
		read_addr += page;
		pdata += page;
    }
	return ret;
}

int ndp_flash_write_page(uint32_t address, uint8_t *buff, uint32_t length)
{
	int ret;
    uint8_t paddr[3];
	uint8_t *pdata = buff;

	if (length > FLASH_PAGE_SIZE)
		length = FLASH_PAGE_SIZE;

	paddr[0] = (address >> 16) & 0xFF;
	paddr[1] = (address >> 8) & 0xFF;
	paddr[2] = address & 0xFF;
	
	ndp_flash_write_enable();
	ret = ndp_flash_spi_transfer(FLASH_PAGE_PROGRAM, paddr, sizeof(paddr), \
						DUMMY_0_BYTES, pdata, length, SPI_FLASH_WRITE);
	while(ndp_flash_get_ready()){
		syntiant_ndp_delay_us(10000);
	}
	ndp_flash_write_disable();
	return ret;
}

int ndp_flash_write_block(uint32_t address, uint8_t *buff, uint32_t length)
{
	int ret = -1;
	uint32_t page;
	uint32_t index = 0;
	uint32_t burn_addr = address;
	uint8_t *pdata = buff;

    while (index < length) {
		if ((length-index) > FLASH_PAGE_SIZE) {
			page = FLASH_PAGE_SIZE;
		} else {
			page = length-index;
		}

		ret = ndp_flash_write_page(burn_addr, pdata, page);
		if (ret) return ret;

        index += page;
		burn_addr += page;
		pdata += page;
    }
	return ret;
}

void ndp_flash_print_data(uint32_t address, uint32_t count)
{
	uint8_t pdata[count];
	uint32_t read_num = count;

	printf("\nread data from Flash [%06X]", address);

	ndp_flash_read_block(address, pdata, read_num);
	for (int i = 0; i < read_num; i ++) {
		if( i % 16 == 0)
			printf("\n");
		printf(" 0x%02x", pdata[i]);
	}
	printf("\n");
}

#define  FLASH_SPLIT_SIZE		1024
static int ndp_flash_program_firmware(uint32_t address, char * file_name)
{
	int ret = -1;
    uint8_t split_data[FLASH_SPLIT_SIZE];
    uint32_t split_index = 0;
    uint32_t split_len;
    uint32_t package_len;
	uint32_t burn_addr = address;
    char *process_ptr = NULL;

    package_len = get_synpkg_size(file_name);
	printf("FLASH programming %s %ld Bytes \n",file_name, package_len);

    while (split_index < package_len) {
		if ((package_len-split_index) > FLASH_SPLIT_SIZE) {
			split_len = FLASH_SPLIT_SIZE;
		} else {
			split_len = package_len-split_index;
		}

        read_synpkg_block(file_name, split_index, split_data, split_len);
		ret = ndp_flash_write_block(burn_addr, split_data, split_len);
		if (ret) return ret;

        split_index += split_len;
		burn_addr += split_len;
		
        process_ptr = flash_process_percent(split_index, package_len, split_len);
        if (process_ptr) printf("%s", process_ptr);
    }

    printf("...100%% \n");
    return ret;
}

int ndp_flash_program_infos(void)
{
	config_data_in_flash_t info = {0};

	info.PINcode = PINCODE_VALUE;
	info.ndp_mode_motion = mode_circular_motion;
	memcpy(&info.cfg, &config_items, sizeof(struct config_ini_items));

	//ndp_flash_4kblock_erase(FLASH_INFO_ADDR);
	ndp_flash_write_block(FLASH_INFO_ADDR, &info, sizeof(info));

	printf("Store the configurations to Flash: %d bytes\n", sizeof(info));

	return 0;
}

int ndp_flash_program_all_fw(void)
{
    int ret;
	char flash_file_name[] = "temp_flash.bin";

	printf("FLASH chip erase ...\n");
	ret = ndp_flash_chip_erase();
	if (ret) return ret;

    printf("FLASH programming starts ...\n");
	//concatenate files
	printf("concatenate synpkg files\n");
	cat_file(mcu_file_name, flash_file_name, 0);
	cat_file(dsp_file_name, flash_file_name, 1);
	cat_file(model_file_name, flash_file_name, 1);

	//flash boot file
	ret = ndp_flash_program_firmware(0x00, flash_file_name);
    if (ret) return ret;

	//remove file
	printf("remove %s \n", flash_file_name);
	remove_file(flash_file_name);

	ret = ndp_flash_program_infos();

    return ret;
}

int ndp_flash_read_infos(config_data_in_flash_t *pdata)
{
	int ret = 0;
	ndp_flash_read_block(FLASH_INFO_ADDR, pdata, sizeof(config_data_in_flash_t));
	if (PINCODE_VALUE != pdata->PINcode)
		ret = 1;

	return ret;
}

