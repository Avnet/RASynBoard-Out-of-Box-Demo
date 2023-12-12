/*
 * fat_load.h
 *
 *  Created on: 2022年12月29日
 *      Author: david
 */

#ifndef FAT_LOAD_H_
#define FAT_LOAD_H_

#include <stdint.h>
#include <stdbool.h>
#include "common_data.h"

#define MCU_FILE_NAME           "mcu_fw_120.synpkg"
#define DSP_FILE_NAME           "dsp_firmware.synpkg"
#define MODEL_FILE_NAME         "ei_model.synpkg"

// Define default certificate filenames
#define AWS_ROOT_CERT_FILE_NAME     "AmazonRootCA1.pem"
#define DEVICE_CERT_FILE_NAME       "cert_DEVICE_NAME.crt"
#define DEVICE_PUBLIC_KEY_FILENAME  "pk_DEVICE_NAME.pem"

#define BLE_DEFAULT_NAME "DA16600-"

#define LED_EVENT_NUM           10

enum FW_LOAD_TYPE {
	BOOT_MODE_FLASH = 0,
	BOOT_MODE_SD,
	BOOT_MODE_NONE,
};

enum SDCARD_EXIST_TYPE {
    SDCARD_IN_SLOT = 1,
    SDCARD_NOT_IN_SLOT,
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

enum BLE_MODE_TYPE {
    BLE_DISABLE = 0,
    BLE_ENABLE = 1,
};

enum TARGET_CLOUD_TYPE {
    CLOUD_NONE = 0,
    CLOUD_IOTCONNECT = 1,
    CLOUD_AWS = 2,
    CLOUD_AZURE = 3,
};

enum AWS_CERT_LOAD_FROM_TYPE {
    LOAD_CERTS_FROM_HEADER = 0,        // Developer manually copies certificate contents to src/certs.h file
    LOAD_CERTS_FROM_FILES = 1,         // Load certificates from the microSD card
    LOAD_CERTS_USE_DA16600_CERTS = 2,  // Certs are already loaded in DA16600, don't write any certs from the application
};

// Note these enumerations can not be changed as they are used to construct the DA16600 message when we send
// each certificate to the DA16600.
enum CERT_ID_TYPE {
    ROOT_CA = 0,
    DEVICE_CERT = 1,
    DEVICE_PUBLIC_KEY = 2,
};

enum WIF_CONFIG_TYPE {
    USE_RENESAS_TOOL_FOR_CONFIG = 0,
    USE_CONFIG_WIFI_SETTINGS = 1,
};

struct config_ini_items {
	/* save the settings from config.ini */
	char button_switch[8];		/** [Function_x]-->Button_shift **/
	int led_event_color[LED_EVENT_NUM];	/** [Led]-->IDXn **/

	int recording_period;		/** [Recording Period]-->Recording_Period **/
	int imu_write_to_file;		/** [IMU data stream]-->Write_to_file **/
	int imu_print_to_terminal;	/** [IMU data stream]-->Print_to_terminal **/

	int low_power_mode;			/** [Low Power Mode]-->Power_Mode **/
	int ble_mode;				/** [BLE Mode]-->BLE_Enabled **/

	int cert_location;          /** [CERTS]-->Cert_Location **/

	int target_cloud;			/** [Cloud Connectivity]-->Target_Cloud **/
    int wifi_config;            /** [WIFI]--> Use_Config_AP_Details**/
	char wifi_ap_name[64];		/** [WIFI]-->Access_Point **/
	char wifi_passwd[64];		/** [WIFI]-->Access_Point_Password **/
	char wifi_cc[4];			/** [WIFI]-->Country_Code **/

	char iotc_uid[64];			/** [IoTConnect]-->Device_Unique_ID **/
	char iotc_cpid[64];			/** [IoTConnect]-->CPID **/
	char iotc_env[64];			/** [IoTConnect]-->Environment **/

	char ble_name[32];          /** [BLE Mode]-->BLE_Name **/

	char ntp_time_server[32];   /** [WIFI]-->NTP_Time_Server **/

	// AWS configuration items
	char aws_endpoint[64];      /** [AWS]-->Endpoint **/
	char aws_device_id[64];     /** [AWS]-->Device_Unique_ID **/
    char aws_pub_topic[64];     /** [AWS]-->MQTT_Pub_Topic **/
    char aws_sub_topic[64];     /** [AWS]-->MQTT_Sub_Topic **/

};

extern struct config_ini_items config_items;
extern int mode_circular_motion;
extern char mcu_file_name[32];
extern char dsp_file_name[64];
extern char model_file_name[64];

void init_fatfs(void);
int binary_loading(char * file_name);
void test_bianry_loading(void);

uint32_t get_synpkg_size(char * file_name);
uint32_t read_synpkg_block(char * file_name, uint32_t offset, uint8_t *buff,  uint32_t split_len);
int check_sdcard_env(void);
uint32_t write_wav_file(char * file_name, uint8_t *buff,  uint32_t len,  int header);
uint32_t write_sensor_file(char * file_name, uint32_t sample_size, int16_t *acc_samples, int header);
#if 1
void write_extraction_file_end(void);
#endif
uint32_t get_synpkg_config_info( void );
uint32_t get_synpkg_boot_mode( void );
uint32_t get_sdcard_total_sectors( void );
uint32_t get_sdcard_slot_status( void );
int get_print_console_type( void );
int get_recording_period( void );
int get_low_power_mode( void );
int is_imu_data_to_file( void );
int is_imu_data_to_terminal( void );
int is_file_exist_in_sdcard( char *filename );
int get_ble_mode( void );
char* get_ble_name( void );
char* get_wifi_ap( void );
char* get_wifi_pw( void );
char* get_wifi_cc( void );
char* get_device_uid( void );
char* get_iotc_env( void );
char* get_iotc_cpid( void );
int get_target_cloud( void );
int get_load_certificate_from( void );
char* get_certificate_file_name( int );
bool get_certificate_data( char*, int, char*);
int get_wifi_config( void );
char* get_ntp_time_server( void );
char* get_aws_endpoint( void );
char* get_aws_deviceId( void );
char* get_aws_sub_topic( void );
char* get_aws_pub_topic( void );


uint32_t cat_file(char * src_file, char * dst_file, int flag);
uint32_t remove_file(char * file_name);
int motion_to_disable(void);

#endif /* FAT_LOAD_H_ */
