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
#include "ble_uart.h"
#include "usb_pcdc_vcom.h"


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
            s = SYNTIANT_NDP_ERROR_INVALID_NETWORK;
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


/* NDP Thread entry function */
/* pvParameters contains TaskHandle_t */
void ndp_thread_entry(void *pvParameters)
{
    int ret;
    uint8_t ndp_class_idx, ndp_nn_idx, sec_val;
    EventBits_t   evbits;
    uint32_t q_event, notifications;
	blink_msg_t  last_stat, current_stat;

    FSP_PARAMETER_NOT_USED (pvParameters);
    R_BSP_PinAccessEnable(); /* Enable access to the PFS registers. */
    R_BSP_PinWrite(LED_USER, BSP_IO_LEVEL_HIGH); /* Turn off User Led */
    /* WIFI reset */
    R_BSP_PinWrite(DA16600_RstPin, BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);
    R_BSP_PinWrite(DA16600_RstPin, BSP_IO_LEVEL_HIGH);
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
    ble_uart_init();

    /* Delay 100 ms */
    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);
    /* read config info of ndp firmwares */
    get_synpkg_config_info();

    /* Start NDP120 program */
    ret = ndp_core2_platform_tiny_start(0, 1);
    if(ret == 0) {
        printf("ndp_core2_platform_tiny_start done\r\n");
        xSemaphoreGive(g_binary_semaphore);
    } else {
        printf("ndp_core2_platform_tiny_start failed %d\r\n", ret);
    }

    ret = ndp_core2_platform_tiny_feature_set(SYNTIANT_NDP_FEATURE_PDM);
    if (ret){
        printf("ndp_core2_platform_tiny_feature_set set 0x%x failed %d\r\n",
                       SYNTIANT_NDP_FEATURE_PDM, ret);
    }

    if (get_synpkg_boot_mode() != BOOT_MODE_SD) {
        // read back info from FLASH
        int mode_val = 0;
        char button_val[32] = {0};
        ret = ndp_flash_read_infos(&mode_val, button_val);
        if (!ret) {
            mode_circular_motion = mode_val;
            strcpy(button_switch, button_val);
            printf("read back from FLASH got mode_circular_motion: %s, button_switch: %s\n",
                    (mode_circular_motion?"disable":"enable"), button_switch);
        }
    }

    ndp_info_display();

	if (motion_to_disable() == CIRCULAR_MOTION_DISABLE) {
        ret = ndp_core2_platform_tiny_sensor_ctl(0, 0);
        if (!ret){
            printf("disable sensor[0] functionality\n");
        }
	}

    /* Start USB thread to enable CDC serial communication and MSC mass storage function */
    start_usb_pcdc_thread();
    /* Enable NDP IRQ */
    ndp_irq_enable();

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
			ndp_core2_platform_tiny_match_process(&ndp_nn_idx, &ndp_class_idx, &sec_val, NULL);
            printf("\nNDP MATCH!!! -- [%d:%d]:%s %s sec-val\n\n", 
                    ndp_nn_idx, ndp_class_idx, labels_per_network[ndp_nn_idx][ndp_class_idx], 
                    (sec_val>0)?"with":"without");
			xSemaphoreGive(g_ndp_mutex);

			switch (ndp_class_idx) {
				case 0:
				    /* Voice: OK-Syntiant; light Amber Led */
					current_stat.led = LED_EVENT_NONE;
					q_event = led_event_color[ndp_class_idx];
					xQueueSend(g_led_queue, (void *)&q_event, 0U );
					ble_send(ble_at_string[V_WAKEUP], strlen(ble_at_string[V_WAKEUP]));
					break;
				case 1:
				    /* Voice: Up; light Cyan Led */
					current_stat.led = LED_EVENT_NONE;
					q_event = led_event_color[ndp_class_idx];
					xQueueSend(g_led_queue, (void *)&q_event, 0U );
					ble_send(ble_at_string[V_UP], strlen(ble_at_string[V_UP]));
					break;
				case 2:
				    /* Voice: Down; light Magenta Led */
					current_stat.led = LED_COLOR_MAGENTA;
					current_stat.timestamp = xTaskGetTickCount();

					if (last_stat.led != LED_COLOR_MAGENTA)
					{
						/* first receive 'Down'  keyword */
						q_event = led_event_color[ndp_class_idx];
						xQueueSend(g_led_queue, (void *)&q_event, 0U );
						ble_send(ble_at_string[V_DOWN], strlen(ble_at_string[V_DOWN]));
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
							ble_send(ble_at_string[V_IDLE], strlen(ble_at_string[V_IDLE]));
							ble_send(ble_at_string[V_STOP], strlen(ble_at_string[V_STOP]));
							/* clear led state */
							current_stat.led = LED_EVENT_NONE;
						}
						else
						{
							/* invalid time */
							q_event = led_event_color[ndp_class_idx];
							xQueueSend(g_led_queue, (void *)&q_event, 0U );
							ble_send(ble_at_string[V_DOWN], strlen(ble_at_string[V_DOWN]));
						}
					}
					break;
				case 3:
				    /* Voice: Back; light Red Led */
					current_stat.led = LED_EVENT_NONE;
					q_event = led_event_color[ndp_class_idx];
					xQueueSend(g_led_queue, (void *)&q_event, 0U );
					ble_send(ble_at_string[V_BACK], strlen(ble_at_string[V_BACK]));
					break;
				case 4:
				    /* Voice: Next; light Green Led */
					current_stat.led = LED_EVENT_NONE;
					q_event = led_event_color[ndp_class_idx];
					xQueueSend(g_led_queue, (void *)&q_event, 0U );
					ble_send(ble_at_string[V_NEXT], strlen(ble_at_string[V_NEXT]));
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

