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

/* NDP Thread entry function */
/* pvParameters contains TaskHandle_t */
void ndp_thread_entry(void *pvParameters)
{
	bool isRunning = false;
    int ret;
    TickType_t duration = 0;

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

    ret = ndp_core2_platform_tiny_feature_set(NDP_CORE2_FEATURE_PDM);
    if (ret){
        printf("ndp_core2_platform_tiny_feature_set set 0x%x failed %d\r\n",
                       NDP_CORE2_FEATURE_PDM, ret);
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

    if (motion_to_disable() == CIRCULAR_MOTION_DISABLE) {
        ret = ndp_core2_platform_tiny_sensor_ctl(0, 0);
        if (!ret){
            printf("disable sensor[0] functionality\n");
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
			ndp_core2_platform_tiny_poll(&notifications, 1);
			ret = ndp_core2_platform_tiny_match_process(&ndp_nn_idx, &ndp_class_idx, &sec_val, NULL);
            if (!ret) {
                printf("\nNDP MATCH!!! -- [%d:%d]:%s %s sec-val\n\n", 
                    ndp_nn_idx, ndp_class_idx, labels_per_network[ndp_nn_idx][ndp_class_idx], 
                    (sec_val>0)?"with":"without");
            }
			xSemaphoreGive(g_ndp_mutex);

			switch (ndp_class_idx) {
				case 0:
				    /* Sound: out of water; light Amber Led */
					if(isRunning)
					{
						current_stat.timestamp = xTaskGetTickCount();
						duration = duration + (current_stat.timestamp - last_stat.timestamp);
						printf("duration time =%d \n", duration);
					}
					else
					{
						isRunning = true;
						printf("it's running!\n");
					}
					current_stat.led = LED_EVENT_NONE;
					q_event = led_event_color(ndp_class_idx);
					xQueueSend(g_led_queue, (void *)&q_event, 0U );
					send_ble_update(ble_at_string[V_WAKEUP], 1000, buf, sizeof(buf));
					enqueTelemetryJson(ndp_class_idx, labels_per_network[ndp_nn_idx][ndp_class_idx]);
					break;
				case 1:
					/* Sound: pump clogged; light Cyan Led */
					isRunning = false;
					duration = 0;

					current_stat.led = LED_EVENT_NONE;
					q_event = led_event_color(ndp_class_idx);
					xQueueSend(g_led_queue, (void *)&q_event, 0U );
					send_ble_update(ble_at_string[V_UP],1000,buf, sizeof(buf));
                    enqueTelemetryJson(ndp_class_idx, labels_per_network[ndp_nn_idx][ndp_class_idx]);
					break;
				case 2:
				    /* Sound: pump off; light Magenta Led */
					isRunning = false;
					duration = 0;
					current_stat.led = LED_COLOR_MAGENTA;
					current_stat.timestamp = xTaskGetTickCount();
					q_event = led_event_color(ndp_class_idx);
					xQueueSend(g_led_queue, (void *)&q_event, 0U );
					send_ble_update(ble_at_string[V_DOWN],1000,buf, sizeof(buf));
					enqueTelemetryJson(ndp_class_idx, labels_per_network[ndp_nn_idx][ndp_class_idx]);
					break;
				case 3:
				    /* Sound: pump on; light Red Led */
					if(isRunning)
					{
						current_stat.timestamp = xTaskGetTickCount();
						duration = duration + (current_stat.timestamp - last_stat.timestamp);
						printf("duration time =%d \n", duration);
					}
					else
					{
						isRunning = true;
					}
					current_stat.led = LED_EVENT_NONE;
					q_event = led_event_color(ndp_class_idx);
					xQueueSend(g_led_queue, (void *)&q_event, 0U );
					send_ble_update(ble_at_string[V_BACK],1000,buf, sizeof(buf));
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
				turn_led(BSP_LEDRED, BSP_LEDON);
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
