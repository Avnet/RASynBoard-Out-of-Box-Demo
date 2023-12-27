#include "ndp_thread.h"
#include <stdio.h>
#include "console.h"
#include "spi_drv.h"
#include "sdcard.h"
#include "led.h"
#include "button.h"
#include "DA9231.h"
#include "fat_load.h"
#include "ndp_flash.h"
#include "ndp_irq_service.h"
#include "syntiant_platform.h"
#include "rm_atcmd.h"
#include "usb_pcdc_vcom.h"
#include "iotc_thread_entry.h"

#define led_event_color(x)	(config_items.led_event_color[x])

typedef struct blink_msg
{
    int led;
	TickType_t  timestamp;
} blink_msg_t;

enum at_cmd_voice{
    V_WAKEUP = 0,
    V_UP = 1,
    V_DOWN = 2,
    V_BACK = 3,
    V_NEXT = 4,
    V_IDLE = 5,
    V_STOP = 6,
};

static char ble_at_string[][36] = {
     "AT+BLETX={\"Action\":\"WakeUp\"}\r\n",
     "AT+BLETX={\"Action\":\"Up\"}\r\n",
     "AT+BLETX={\"Action\":\"Down\"}\r\n",
     "AT+BLETX={\"Action\":\"Back\"}\r\n",
     "AT+BLETX={\"Action\":\"Next\"}\r\n",
     "AT+BLETX={\"Action\":\"Idle\"}\r\n",
     "AT+ADVSTOP\r\n",
};

#define SYNTIANT_NDP120_MAX_CLASSES     32
#define SYNTIANT_NDP120_MAX_NNETWORKS   4
#define NDP120_MCU_LABELS_MAX_LEN       (0x200)

static char *labels[SYNTIANT_NDP120_MAX_CLASSES];
static char *labels_per_network[SYNTIANT_NDP120_MAX_NNETWORKS]
            [SYNTIANT_NDP120_MAX_CLASSES];
static char numlabels_per_network[SYNTIANT_NDP120_MAX_NNETWORKS];
static char label_data[NDP120_MCU_LABELS_MAX_LEN] = "";

static void send_ble_update(char* ,int ,char*, int);
extern void printConfg(void);

static void set_decimation_inshift( void );


#define   IMU_SENSOR_INDEX         0
#define   IMU_SENSOR_MSSB          1
#define   IMU_FLASH_MSSB           0
#define   GPIO_LEVEL_LOW           0
#define   GPIO_LEVEL_HIGH          1

void ndp_print_imu(void)
{
    uint8_t imu_val = 0;
    uint8_t reg = 0x75 | 0x80 ; /*WHO_AM_I*/

    ndp_core2_platform_tiny_mspi_config();
    ndp_core2_platform_tiny_mspi_write(IMU_SENSOR_MSSB, 1, &reg, 0);
    ndp_core2_platform_tiny_mspi_read(IMU_SENSOR_MSSB, 1, &imu_val, 1);
    printf("attched IMU ID = 0x%02x\n", imu_val); /*id = 0x67*/
}

void ndp_info_display(void)
{
    int s, total_nn, total_labels;
    int j, class_num, nn_num, prev_nn_num, num_labels, labels_len;
    char *label_string;

    s = ndp_core2_platform_tiny_get_info(&total_nn, &total_labels, 
            label_data, &labels_len); 
    if (s) return;
        
    printf("ndp120 has %d network and %d labels loaded\n", total_nn, total_labels);
  
    /* get pointers to the labels */
    num_labels = 0;
    j = 0;

    /* labels_len is 4 byte aligned. We continue processing
       labels until the running sum of label characters
       processed is within 3 bytes of labels_len */
    while ((labels_len - j > 3) &&
            (num_labels < SYNTIANT_NDP120_MAX_CLASSES)) {
        labels[num_labels] = &label_data[j];
        (num_labels)++;
        for (; label_data[j]; j++)
            ;
        j++;
    }

    /* build an array that hold all labels based on network number */
    class_num = 0;
    nn_num = 0;
    prev_nn_num = 0;

    for (j = 0; j < num_labels; j++) {
        label_string = labels[j];
        nn_num = *(label_string + 2) - '0';
        if (nn_num < 0 || nn_num >= SYNTIANT_NDP120_MAX_NNETWORKS) {
            s = NDP_CORE2_ERROR_INVALID_NETWORK;
            return;
        }
        if (nn_num != prev_nn_num) {
            class_num = 0;
        }
        labels_per_network[nn_num][class_num++] = label_string;
        numlabels_per_network[nn_num] = class_num;
        prev_nn_num = nn_num;
    }
}

// Helper function to create
void enqueTelemetryJson(int inferenceIndex, const char* inferenceString)
{
#define MAX_TELEMETRY_LEN 256

    static int msgCnt = 0;
    char telemetryMsg[MAX_TELEMETRY_LEN] = {'\0'};
    telemetryQueueMsg_t newMsg;
    char* telemetryPtr;
    int telemetryMsgLen;

    // Create the JSON
    snprintf(telemetryMsg, sizeof(telemetryMsg), "{\"msgCount\": %d, \"inferenceIdx\": %d, \"inferenceStr\": \"%s\"}", msgCnt++, inferenceIndex, inferenceString);

    // Allocate memory on the heap for the JSON string.
    // We allocate the memory here, then free the memory
    // when the message is pulled from the queue
    telemetryMsgLen = strlen(telemetryMsg);
    telemetryPtr = pvPortMalloc(telemetryMsgLen);

    // Verify we have memory for the message
    if(NULL == telemetryPtr){
        return;
    }

    // Copy the incomming telemetry to the heap memory
    strncpy(telemetryPtr, telemetryMsg, telemetryMsgLen+1);
    
    // Populate the queue data structure, and enqueue the message 
    newMsg.msgSize = telemetryMsgLen;
    newMsg.msgPtr = telemetryPtr;
    xQueueSend(g_telemetry_queue, (void *)&newMsg, 0U);
    return;
}

int bff_reinit_imu(void)
{
    int s;
    
    s = ndp_core2_platfom_tiny_gpio_release(MSPI_IMU_SSB);
    if (s) {
        printf("release gpio%d failed: %d\n", MSPI_IMU_SSB, s);
        return s;
    }

    s = ndp_core2_platform_tiny_dsp_restart();
    if (s) {
        printf("restart DSP failed: %d\n", IMU_SENSOR_INDEX, s);
        return s;
    }
    vTaskDelay (pdMS_TO_TICKS(1000UL));

    return s;
}

/* NDP Thread entry function */
/* pvParameters contains TaskHandle_t */
void ndp_thread_entry(void *pvParameters)
{
    int ret, fatal_error;
    uint8_t ndp_class_idx, ndp_nn_idx, sec_val;
    EventBits_t   evbits;
    uint32_t q_event, notifications;
	blink_msg_t  last_stat, current_stat;
	char buf[32];
    int ndp_boot_mode = NDP_CORE2_BOOT_MODE_BOOT_FLASH;

    FSP_PARAMETER_NOT_USED (pvParameters);
    R_BSP_PinAccessEnable(); /* Enable access to the PFS registers. */
    R_BSP_PinWrite(LED_USER, BSP_IO_LEVEL_HIGH); /* Turn off User Led */

    /* will create a task if using USB CDC */
    console_init();

    // Free the initialization Semaphore
    xSemaphoreGive(g_xInitialSemaphore);

    spi_init();

    printf("FreeRTOS ndp_thread running\n");
    /* Set BUCK and LDO for NDP120 */
    DA9231_open();
    DA9231_disable();
    DA9231_enable();
    //R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);
   // DA9231_dump_regs();
    DA9231_close();

    ndp_irq_init();
    init_fatfs();
    button_init();

    /* Delay 100 ms */
    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);

    /* read config info of ndp firmwares */
    get_synpkg_config_info();

    if (get_synpkg_boot_mode() == BOOT_MODE_SD)
    {
        ndp_boot_mode = NDP_CORE2_BOOT_MODE_HOST_FILE;
    }

    /* Start NDP120 program */
    ret = ndp_core2_platform_tiny_start(1, 1, ndp_boot_mode);
    if(ret == 0) {
        printf("ndp_core2_platform_tiny_start done\r\n");
        xSemaphoreGive(g_binary_semaphore);
    } else {
        printf("ndp_core2_platform_tiny_start failed %d\r\n", ret);
    }

    if (ndp_boot_mode == NDP_CORE2_BOOT_MODE_BOOT_FLASH) {
    // read back info from FLASH
        config_data_in_flash_t flash_data = {0};
        if (0 == ndp_flash_read_infos(&flash_data)){
            mode_circular_motion = flash_data.ndp_mode_motion;
	        memcpy(&config_items, &flash_data.cfg, sizeof(struct config_ini_items));
	    }
        
        // Output the current configuration for the user
        if (get_print_console_type() != CONSOLE_USB_CDC) {
            printConfg();
        }
    }

    ndp_info_display();

    if (motion_running() == CIRCULAR_MOTION_DISABLE) {
        set_decimation_inshift();

        ret = ndp_core2_platform_tiny_feature_set(NDP_CORE2_FEATURE_PDM);
        if (ret){
            printf("ndp_core2_platform_tiny_feature_set set 0x%x failed %d\r\n",
                        NDP_CORE2_FEATURE_PDM, ret);
        }
    }
    else {
        if (ndp_boot_mode == NDP_CORE2_BOOT_MODE_BOOT_FLASH) {
            ret = bff_reinit_imu();
            if (ret) {
                printf("bff reinit IMU failed: %d\n", ret);
            }
        }
        
	    ndp_print_imu();

        ret = ndp_core2_platform_tiny_sensor_ctl(IMU_SENSOR_INDEX, 1);
        if (ret) {
            printf("Enable sensor[%d] icm-42670 failed: %d\n", IMU_SENSOR_INDEX, ret);
        }
        else {
            printf("Enable sensor[%d] icm-42670 done\n", IMU_SENSOR_INDEX);
        }
    }

    /* Enable NDP IRQ */
    ndp_irq_enable();

    /* Start USB thread to enable CDC serial communication and MSC mass storage function */
    start_usb_pcdc_thread();
    vTaskDelay (pdMS_TO_TICKS(1000UL));

    memset(&last_stat, 0, sizeof(blink_msg_t));
    memset(&current_stat, 0, sizeof(blink_msg_t));
    /* TODO: add your own code here */
    while (1)
    {
        /* Wait until NDP recognized voice keywords */
		evbits = xEventGroupWaitBits(g_ndp_event_group, EVENT_BIT_VOICE | EVENT_BIT_FLASH, 
            pdTRUE, pdFALSE , portMAX_DELAY);
		if( evbits & EVENT_BIT_VOICE ) 
		{
			xSemaphoreTake(g_ndp_mutex,portMAX_DELAY);
			ndp_core2_platform_tiny_poll(&notifications, 1, &fatal_error);
            if (fatal_error) {
                printf("\nNDP Fatal Error!!!\n\n");
            }

			ret = ndp_core2_platform_tiny_match_process(&ndp_nn_idx, &ndp_class_idx, &sec_val, NULL);
            if (!ret) {
                printf("\nNDP MATCH!!! -- [%d:%d]:%s %s sec-val\n\n", 
                    ndp_nn_idx, ndp_class_idx, labels_per_network[ndp_nn_idx][ndp_class_idx], 
                    (sec_val>0)?"with":"without");
            }
			xSemaphoreGive(g_ndp_mutex);

			switch (ndp_class_idx) {
				case 0:
				    /* Voice: OK-Syntiant; light Amber Led */
					current_stat.led = LED_EVENT_NONE;
					q_event = led_event_color(ndp_class_idx);
					xQueueSend(g_led_queue, (void *)&q_event, 0U );
					send_ble_update(ble_at_string[V_WAKEUP], 1000, buf, sizeof(buf));
					enqueTelemetryJson(ndp_class_idx, labels_per_network[ndp_nn_idx][ndp_class_idx]);
					break;
				case 1:
				    /* Voice: Up; light Cyan Led */
					current_stat.led = LED_EVENT_NONE;
					q_event = led_event_color(ndp_class_idx);
					xQueueSend(g_led_queue, (void *)&q_event, 0U );
					send_ble_update(ble_at_string[V_UP],1000,buf, sizeof(buf));
                    enqueTelemetryJson(ndp_class_idx, labels_per_network[ndp_nn_idx][ndp_class_idx]);
					break;
				case 2:
				    /* Voice: Down; light Magenta Led */
					current_stat.led = LED_COLOR_MAGENTA;
					current_stat.timestamp = xTaskGetTickCount();

					if (last_stat.led != LED_COLOR_MAGENTA)
					{
						/* first receive 'Down'  keyword */
						q_event = led_event_color(ndp_class_idx);
						xQueueSend(g_led_queue, (void *)&q_event, 0U );
						send_ble_update(ble_at_string[V_DOWN],1000,buf, sizeof(buf));
	                    enqueTelemetryJson(ndp_class_idx, labels_per_network[ndp_nn_idx][ndp_class_idx]);
					}
					else
					{
						/*Judging the received 'Down""Down' keyword*/
						TickType_t duration = current_stat.timestamp - last_stat.timestamp;
						printf("duration time =%d \n", duration);
						if ( duration < pdMS_TO_TICKS(3600UL) )
						{
							/* valid, send led blink envent */
							q_event =  LED_BLINK_DOUBLE_BLUE;
							xQueueSend(g_led_queue, (void *)&q_event, 0U );
							/* Send 'idle' and 'advstop' to bluetooth */
							send_ble_update(ble_at_string[V_IDLE],1000,buf, sizeof(buf));
							send_ble_update(ble_at_string[V_STOP],1000,buf, sizeof(buf));
							/* clear led state */
							current_stat.led = LED_EVENT_NONE;
						}
						else
						{
							/* invalid time */
							q_event = led_event_color(ndp_class_idx);
							xQueueSend(g_led_queue, (void *)&q_event, 0U );
							send_ble_update(ble_at_string[V_DOWN],1000,buf, sizeof(buf));
						}
					}
					break;
				case 3:
				    /* Voice: Back; light Red Led */
					current_stat.led = LED_EVENT_NONE;
					q_event = led_event_color(ndp_class_idx);
					xQueueSend(g_led_queue, (void *)&q_event, 0U );
					send_ble_update(ble_at_string[V_BACK],1000,buf, sizeof(buf));
                    enqueTelemetryJson(ndp_class_idx, labels_per_network[ndp_nn_idx][ndp_class_idx]);
					break;
				case 4:
				    /* Voice: Next; light Green Led */
					current_stat.led = LED_EVENT_NONE;
					q_event = led_event_color(ndp_class_idx);
					xQueueSend(g_led_queue, (void *)&q_event, 0U );
					send_ble_update(ble_at_string[V_NEXT],1000,buf, sizeof(buf));
                    enqueTelemetryJson(ndp_class_idx, labels_per_network[ndp_nn_idx][ndp_class_idx]);
					break;
				default :
					break;
			}
			xSemaphoreGive(g_binary_semaphore);
			/* Store the led state */
			memcpy(&last_stat, &current_stat, sizeof(blink_msg_t));
		}
		else if( evbits & EVENT_BIT_FLASH )
		{
			if ( 0 == check_sdcard_env())
			{
				usb_disable();
				printf ("\nBegin to program the spi flash ..... \n");
                ndp_irq_disable();
				turn_led(BSP_LEDRED, BSP_LEDON);
                if (motion_running() == CIRCULAR_MOTION_DISABLE) {
                    ret = ndp_core2_platform_tiny_feature_set(NDP_CORE2_FEATURE_NONE);
                    if (ret) {
                        printf("Feature set NONE failed: %d\n", ret);
                        break;
                    }
                }
                else {
                    ret = ndp_core2_platform_tiny_sensor_ctl(IMU_SENSOR_INDEX, 0);
                    if (ret) {
                        printf("disable sneosr[%d] failed: %d\n", IMU_SENSOR_INDEX, ret);
                        break;
                    }
                }

				ndp_flash_init();
				ndp_flash_program_all_fw();

				turn_led(BSP_LEDRED, BSP_LEDOFF);
				turn_led(BSP_LEDGREEN, BSP_LEDON);
#ifdef  FLASH_READBACK_CHECK
                for(int i=0; i<3000 ; i++)
                {
                    uint8_t pdata[256];
                    uint32_t address = i * 256;
                    memset(pdata, 0, sizeof(pdata));
                    ndp_flash_read_block(address, pdata, 256);
                    write_wav_file("readback_flash.bin", pdata,  256, i+1);
                }
#endif
				vTaskDelay (100);
				turn_led(BSP_LEDGREEN, BSP_LEDOFF);
				printf ("Finished programming!\n\n");
				usb_enable();
            
                if (motion_running() == CIRCULAR_MOTION_DISABLE) {
                    ret = ndp_core2_platform_tiny_feature_set(NDP_CORE2_FEATURE_PDM);
                    if (ret) {
                        printf("Feature set NONE failed: %d\n", ret);
                        break;
                    }
                }
                else {
                    ret = bff_reinit_imu();
                    if (ret) {
                        printf("bff reinit IMU failed: %d\n", ret);
                    }

                    ret = ndp_core2_platform_tiny_sensor_ctl(IMU_SENSOR_INDEX, 1);
                    if (ret) {
                        printf("enable sneosr[%d] failed: %d\n", IMU_SENSOR_INDEX, ret);
                        break;
                    }
                }
                ndp_irq_enable();
			}
			else
			{
			    printf("Cannot find sdcard or firmware files !");
			}
		}
        vTaskDelay (5);
    }
}

static void send_ble_update(char* ble_string ,int timeout ,char* buf, int buf_size){

    // Only send the BLE message if the mode is enabled
    if(BLE_ENABLE == get_ble_mode()){
        rm_atcmd_send(ble_string ,timeout, buf, buf_size);
    }
}

static void set_decimation_inshift( void ){

#define INSHIFT_AUDIO_ID 0
#define INSHIFT_SINGLE_MIC_ID 0
#define INSHIFT_DUAL_MIC_ID 1

    int decimation_inshift_mic0_read_value = 0;
    int decimation_inshift_mic1_read_value = 0;
    int decimation_inshift_calculated_value = 0;

    int decimation_inshift_value_mic0 = get_dec_inshift_value();
    int decimation_inshift_offset = get_dec_inshift_offset();

    // Catch the case where we don't make any changes; just bail out.
    if((decimation_inshift_value_mic0 == DEC_INSHIFT_VALUE_DEFAULT) &&
       (decimation_inshift_offset == DEC_INSHIFT_OFFSET_DEFAULT )){
        return;
    }

    // Read the decimation_inshift values for both mics
    ndp_core2_platform_tiny_audio_config_get(INSHIFT_AUDIO_ID, INSHIFT_SINGLE_MIC_ID, 0, &decimation_inshift_mic0_read_value);
    ndp_core2_platform_tiny_audio_config_get(INSHIFT_AUDIO_ID, INSHIFT_DUAL_MIC_ID, 0, &decimation_inshift_mic1_read_value);

    printf("/n   Decimation Inshift Details\n");
    printf("    Model Decimation Inshift Mic0: %d\n", decimation_inshift_mic0_read_value);
    printf("    Model Decimation Inshift Mic1: %d\n", decimation_inshift_mic1_read_value);

    // Check the configuration
    if(decimation_inshift_value_mic0 != DEC_INSHIFT_VALUE_DEFAULT){

        printf("DECIMATION_INSHIFT_VALUE     : %d\n", decimation_inshift_value_mic0);
        decimation_inshift_calculated_value = decimation_inshift_value_mic0;

    }
    else { // User did not define a custom decimation_inshift value, apply the offset.  Note the default
           // value for the offset is zero.  So we can safely apply this offset without any validation.  We'll
           // verify the final value below before applying them to the NDP120.

        printf("DECIMATION_INSHIFT_OFFSET    : %d\n", decimation_inshift_offset);
        decimation_inshift_calculated_value = decimation_inshift_mic0_read_value + decimation_inshift_offset;

    }

    // Check for invalid low value
    if(decimation_inshift_calculated_value < DEC_INSHIFT_VALUE_MIN ){

        printf("Warning calculated value %d is below the min allowed value of %d\n", decimation_inshift_calculated_value, DEC_INSHIFT_VALUE_MIN);
        decimation_inshift_calculated_value = DEC_INSHIFT_VALUE_MIN;
    }

    // Check for invalid low value
    else if(decimation_inshift_calculated_value > DEC_INSHIFT_VALUE_MAX){

        printf("Warning calculated value %d is above max allowed value of %d\n", decimation_inshift_calculated_value, DEC_INSHIFT_VALUE_MAX);
        decimation_inshift_calculated_value = DEC_INSHIFT_VALUE_MAX;
    }

    printf("Setting new value to %d\n", decimation_inshift_calculated_value);

    // Write the new value(s) into the NDP120
    ndp_core2_platform_tiny_audio_config_set(INSHIFT_AUDIO_ID, INSHIFT_SINGLE_MIC_ID, &decimation_inshift_calculated_value);
    printf("Final Decimation Inshift Mic0: %d\n", decimation_inshift_calculated_value);


    // Only update the mic1 value if one was set in the model
    if(0 != decimation_inshift_mic1_read_value){
        ndp_core2_platform_tiny_audio_config_set(INSHIFT_AUDIO_ID, INSHIFT_DUAL_MIC_ID, &decimation_inshift_calculated_value);
        printf("Final Decimation Inshift Mic1: %d\n", decimation_inshift_calculated_value);
    }

    printf("\n");
}

