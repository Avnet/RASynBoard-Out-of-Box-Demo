/*
 * fat_load.h
 *
 *  Created on: 2022年12月29日
 *      Author: david
 */

#ifndef FAT_LOAD_H_
#define FAT_LOAD_H_

#include <stdint.h>

#define MCU_FILE_NAME           "mcu_fw_120.synpkg"
#define DSP_FILE_NAME           "dsp_firmware.synpkg"
#define MODEL_FILE_NAME         "ei_model.synpkg"

enum FW_LOAD_TYPE {
	BOOT_MODE_EMMC = 0,
	BOOT_MODE_SD,
	BOOT_MODE_NONE,
};

enum DBG_PRINT_CONSOLE_TYPE {
    CONSOLE_UART = 1,
    CONSOLE_USB_CDC = 2,
    CONSOLE_NONE,
};

enum LOW_POWER_MODE_TYPE {
    DOWN_DOWN_LP_MODE = 0,
    ALWAYS_ENTER_LP_MODE = 1,
    LOW_POWER_MODE_NONE,
};

enum CIRCULAR_MOTION_TYPE {
    CIRCULAR_MOTION_ENABLE = 0,
    CIRCULAR_MOTION_DISABLE = 1,
};

enum IMU_FUNC_TYPE {
	IMU_FUNC_DISABLE = 0,
    IMU_FUNC_ENABLE = 1,
};

#define   LED_EVENT_NUM        10

extern int mode_circular_motion;
extern char mcu_file_name[32];
extern char dsp_file_name[64];
extern char model_file_name[64];
extern int  led_event_color[LED_EVENT_NUM];
extern char button_switch[32];

void init_fatfs(void);
int binary_loading(char * file_name);
void test_bianry_loading(void);

uint32_t get_synpkg_size(char * file_name);
uint32_t read_synpkg_block(char * file_name, uint32_t offset, uint8_t *buff,  uint32_t split_len);
int check_sdcard_env(void);
uint32_t write_wav_file(char * file_name, uint8_t *buff,  uint32_t len,  int header);
uint32_t write_sensor_file(char * file_name, uint32_t sample_size, int16_t *acc_samples, int header);
uint32_t get_synpkg_config_info( void );
uint32_t get_synpkg_boot_mode( void );
uint32_t get_sdcard_total_sectors( void );
int get_print_console_type( void );
int get_recording_period( void );
int get_low_power_mode( void );
int is_imu_data_to_file( void );
int is_imu_data_to_terminal( void );

uint32_t cat_file(char * src_file, char * dst_file, int flag);
uint32_t remove_file(char * file_name);
int motion_to_disable(void);

#endif /* FAT_LOAD_H_ */
