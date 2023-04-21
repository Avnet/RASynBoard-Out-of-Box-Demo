/*
 * fat_load.h
 *
 *  Created on: 2022年12月29日
 *      Author: david
 */

#ifndef FAT_LOAD_H_
#define FAT_LOAD_H_

#define MCU_FILE_NAME           "mcu_fw_120.synpkg"
#define DSP_FILE_NAME           "dsp_firmware.synpkg"
#define MODEL_FILE_NAME         "ei_model.synpkg"

enum FW_LOAD_TYPE {
	BOOT_MODE_EMMC = 0,
	BOOT_MODE_SD,
	BOOT_MODE_NONE,
};

#define   LED_EVENT_NUM        10

extern char mcu_file_name[32];
extern char dsp_file_name[64];
extern char model_file_name[64];
extern int  led_event_color[LED_EVENT_NUM];


void init_fatfs(void);
int binary_loading(char * file_name);
void test_bianry_loading(void);

uint32_t get_synpkg_size(char * file_name);
uint32_t read_synpkg_block(char * file_name, uint32_t offset, uint8_t *buff,  uint32_t split_len);
int check_sdcard_env(void);
uint32_t write_wav_file(char * file_name, uint8_t *buff,  uint32_t len,  int header);
uint32_t get_synpkg_config_info( void );
uint32_t get_synpkg_boot_mode( void );

#endif /* FAT_LOAD_H_ */
