#include "ndp_record_thread.h"
#include "syntiant_platform.h"
#include "syntiant_common.h"
#include "fat_load.h"
#include "button.h"
#include "led.h"
#include <stdio.h>
#include "ndp_irq_service.h"
#include "led.h"

#define   AUDIO_REC_BYTES_PER_SEC         32000U
#define   AUDIO_REC_BUFFER_SIZE            2048
#define   AUDIO_REC_FILE_NAME_PREFIX    "ndp_audio_record_"

#define   IMU_REC_BYTES_PER_SEC          100
#define   IMU_REC_BUFFER_SIZE            256
#define   IMU_REC_FILE_NAME_PREFIX      "ndp_imu_record_"

#define   SHORT_PRESS_TIME        pdMS_TO_TICKS(400UL)
#define   LONG_PRESS_TIME         pdMS_TO_TICKS(3000UL)

#define   IMU_SENSOR_INDEX         0

extern int firmware_idx;

enum short_press_button_to_record {
	REC_AUDIO = 1,
	REC_IMU = 2,
	REC_NONE = 11,
};

static int get_button_mapping_event(void)
{
	int ret = REC_NONE;
	if (memcmp (button_switch, "audio", 5) == 0 )
		ret = REC_AUDIO;
	if (memcmp (button_switch, "imu", 3) == 0 )
		ret = REC_IMU;

    return ret;
}

static int is_record_motion(void)
{
    return (get_button_mapping_event() == REC_IMU);
}

/* assuming little endian, and structure with no padding */
struct wav_header_s {
    char riff[4];
    uint32_t file_size;
    char wave[4];
    char fmt[4];
    uint32_t fmt_size;
    uint16_t type;
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t bytes_per_second;
    uint16_t bytes_per_frame;
    uint16_t bits_per_sample;
    char data[4];
    uint32_t data_size;
};

static void create_wav_header(struct wav_header_s *wav_hdr, int sample_bytes, int channels, int total_len)
{
    wav_hdr->riff[0] = 'R';
    wav_hdr->riff[1] = 'I';
    wav_hdr->riff[2] = 'F';
    wav_hdr->riff[3] = 'F';
    wav_hdr->file_size = 36 + total_len;
    wav_hdr->wave[0] = 'W';
    wav_hdr->wave[1] = 'A';
    wav_hdr->wave[2] = 'V';
    wav_hdr->wave[3] = 'E';
    wav_hdr->fmt[0] = 'f';
    wav_hdr->fmt[1] = 'm';
    wav_hdr->fmt[2] = 't';
    wav_hdr->fmt[3] = ' ';
    wav_hdr->fmt_size = 16;
    wav_hdr->type = 1;
    wav_hdr->channels = channels;
    wav_hdr->sample_rate = 16000;
    wav_hdr->bytes_per_second = 16000 * 1 * sample_bytes;
    wav_hdr->bytes_per_frame = 1 * sample_bytes;
    wav_hdr->bits_per_sample = sample_bytes * 8;
    wav_hdr->data[0] = 'd';
    wav_hdr->data[1] = 'a';
    wav_hdr->data[2] = 't';
    wav_hdr->data[3] = 'a';
    wav_hdr->data_size = total_len;
}

/******************************************
 * IMU record
******************************************/
static int imu_record_operation(int isstart)
{
    int s = 0;
    int ints;

    if (isstart) {
        ints = 0;
        s = ndp_core2_platform_tiny_interrupts(&ints);
        if (s) {
            printf("set interrupts to %d failed: %d\n", ints, s);
            return s;
        }

        /* enable sensor */
        s = ndp_core2_platform_tiny_sensor_ctl(IMU_SENSOR_INDEX, 1);
        if (s) {
            printf("enable sensor[%d] failed: %d\n", IMU_SENSOR_INDEX, s);
            return s;
        }
    }
    else {
        s = ndp_core2_platform_tiny_sensor_ctl(IMU_SENSOR_INDEX, 0);
        if (s) {
            printf("disable sneosr[%d] failed: %d\n", IMU_SENSOR_INDEX, s);
            return s;
        }

        ints = 0x100;
        s = ndp_core2_platform_tiny_interrupts(&ints);
        if (s) {
            printf("set interrupts to %d failed: %d\n", ints, s);
            return s;
        }
    }

    return s;
}

struct cb_sensor_arg_s {
    char file_name[64];
    uint32_t sets_count;
};

#define SENSOR_SAMPLE_SIZE  (6)
void icm42670_extraction_cb(uint32_t sample_size, uint8_t *sensor_data, void *sensor_arg)
{
    struct cb_sensor_arg_s *cb_sensor_arg = (struct cb_sensor_arg_s*)sensor_arg;
    int i, index = 0;
    int16_t *acc_samples = (int16_t *)(sensor_data);

	if (is_imu_data_to_terminal()) {
		// show data on the serial console
		index = sample_size / 2 - 1;
		for (i = 0; i < index; i++) {
			printf("%d,", acc_samples[i]);
		}
		printf("%d\n", acc_samples[index]);
	}

	if (is_imu_data_to_file()) {
		// save data to sdcard
		xSemaphoreTake(g_ndp_mutex,portMAX_DELAY);
		write_sensor_file(cb_sensor_arg->file_name, sample_size, acc_samples, 0);
		xSemaphoreGive(g_ndp_mutex);
	}

    cb_sensor_arg->sets_count ++;
}

static int imu_record_process(int extract_sets, struct cb_sensor_arg_s *sensor_arg)
{
    int s;
    uint32_t sample_size;
    uint8_t *data_ptr = NULL;

    data_ptr = pvPortMalloc(IMU_REC_BUFFER_SIZE);
    if (!data_ptr) return -1;

	if (is_imu_data_to_file()) {
		xSemaphoreTake(g_ndp_mutex,portMAX_DELAY);
		write_sensor_file(sensor_arg->file_name, 0, NULL, 1);
		xSemaphoreGive(g_ndp_mutex);
	}
	if (is_imu_data_to_terminal()) {
		printf("\nAcc_x,Acc_y,Acc_z,Gyro_x,Gyro_y,Gyro_z\n");
	}

    while (extract_sets > sensor_arg->sets_count) {
        s = ndp_core2_platform_tiny_sensor_extract_data(data_ptr, 
                IMU_SENSOR_INDEX, icm42670_extraction_cb, sensor_arg);
        if ((s) && (s != SYNTIANT_NDP_ERROR_DATA_REREAD)) {
            printf("sensor extract data failed: %d\n", s);
            break;
        }
    }

    s = ndp_core2_platform_tiny_get_recording_metadata(&sample_size, 0, 0);
    if (s) {
        printf("audio record get metadata from ilib without notify failed: %d\n", s);
        goto process_out;
    }

process_out:
    if (data_ptr) vPortFree(data_ptr);

    return s;
}

/******************************************
 * Audio record
******************************************/
static int audio_record_operation(int isstart, uint32_t *sample_size)
{
    int s = 0;
    int ints;

    if (isstart) {
        ints = 0;
        s = ndp_core2_platform_tiny_interrupts(&ints);
        if (s) {
            printf("set interrupts to %d failed: %d\n", ints, s);
            return s;
        }
    }
    else {
        ints = 0x100;
        s = ndp_core2_platform_tiny_interrupts(&ints);
        if (s) {
            printf("set interrupts to %d failed: %d\n", ints, s);
            return s;
        }
    }

    return s;
}

struct cb_audio_arg_s {
    char file_name[64];
    uint32_t total_len;
};

void audio_extraction_cb (uint32_t extract_size, uint8_t *audio_data, 
                    void *audio_arg)
{
    struct cb_audio_arg_s *cb_audio_arg = (struct cb_audio_arg_s*)audio_arg;

    if (extract_size > 0) {
        xSemaphoreTake(g_ndp_mutex,portMAX_DELAY);
        write_wav_file(cb_audio_arg->file_name, audio_data, extract_size, 0);
        cb_audio_arg->total_len += extract_size;
        xSemaphoreGive(g_ndp_mutex);
    }
}

static int audio_record_process(int wanted_len, struct cb_audio_arg_s *audio_arg)
{
    int s = 0;
    uint8_t *data_ptr = NULL;
	struct wav_header_s wav_hdr;
    uint32_t sample_size;
    uint32_t sample_bytes = ndp_core2_platform_tiny_get_samplebytes();

    data_ptr = pvPortMalloc(AUDIO_REC_BUFFER_SIZE);
    if (!data_ptr) return -1;

    /* sample ready interrupt is enabled in MCU firmware */
    s = ndp_core2_platform_tiny_get_recording_metadata(&sample_size, 1, 1);
    if (s) {
        printf("audio record get metadata from mcu with notify failed: %d\n", s);
        goto process_out;
    }

    create_wav_header(&wav_hdr, sample_bytes, 1, wanted_len);
    xSemaphoreTake(g_ndp_mutex,portMAX_DELAY);
    write_wav_file(audio_arg->file_name, (uint8_t *)&wav_hdr, sizeof(wav_hdr), 1);
    xSemaphoreGive(g_ndp_mutex);

    printf("To audio record %d bytes for %d seconds\n", wanted_len, get_recording_period());
    while (wanted_len > audio_arg->total_len) {
        s = ndp_core2_platform_tiny_notify_extract_data(data_ptr, 
                sample_size, audio_extraction_cb, audio_arg);
        if ((s) && (s != SYNTIANT_NDP_ERROR_DATA_REREAD)) {
            printf("audio extract data failed: %d\n", s);
            break;
        }
    }
    
    /* sample ready interrupt is enabled in MCU firmware */
    s = ndp_core2_platform_tiny_get_recording_metadata(&sample_size, 1, 0);
    if (s) {
        printf("audio record get metadata from mcu without notify failed: %d\n", s);
        goto process_out;
    }

process_out:
    if (data_ptr) vPortFree(data_ptr);

    return s;
}

void ndp_print_imu(void)
{
    uint8_t imu_val = 0;
    uint8_t reg = 0x75 | 0x80 ; /*WHO_AM_I*/

    /* set MSSB0/GPIO0 pin */
    ndp_core2_platform_gpio_config(0, NDP_CORE2_CONFIG_VALUE_GPIO_DIR_OUT, 1);
    /* reset MSSB1/GPIO1 pin */
    ndp_core2_platform_gpio_config(1, NDP_CORE2_CONFIG_VALUE_GPIO_DIR_OUT, 0);

    //ndp_core2_platform_tiny_mspi_config();
    ndp_core2_platform_tiny_mspi_write(1, 1, &reg, 0);
    ndp_core2_platform_tiny_mspi_read(1, 1, &imu_val, 1);
    printf("attched IMU ID = 0x%02x\n", imu_val); /*id = 0x67*/

}

/******************************************
 *Record Thread entry function
* pvParameters contains TaskHandle_t
*******************************************/
void ndp_record_thread_entry(void *pvParameters)
{
    int s;
	char data_filename[32];
	uint32_t file_create = 0;
	EventBits_t   evbits;
	TickType_t time1 = 0, delta_time = 0;
	bool rec_process = false;
    int record_count = 0;
    uint32_t sample_size;

    FSP_PARAMETER_NOT_USED (pvParameters);
	/* Start recording after 6 seconds */
	vTaskDelay (pdMS_TO_TICKS(3000UL));
	printf("Record_thread running\n");

	//ndp_print_imu();
   
	if (get_synpkg_boot_mode() != BOOT_MODE_SD) {
	    printf("Cannot find sdcard to save record data, exit Record_thread! \n");
	    vTaskDelete(NULL);
	    vTaskDelay (1);
	}

	// Check to see if we started up in low power mode.  If so, we can't enable the
	// record feature.  Exit this thread.
	if(get_low_power_mode() == ALWAYS_ENTER_LP_MODE){

	    // Exit the Record_thread
	    vTaskDelete(NULL);
	    vTaskDelay (1);
	}

    while (1)
    {
        evbits = xEventGroupWaitBits(g_ndp_event_group, EVENT_BIT_RISING, pdTRUE, pdFALSE , portMAX_DELAY);
        time1 = xTaskGetTickCount();
        evbits = xEventGroupWaitBits(g_ndp_event_group, EVENT_BIT_FALLING, pdTRUE, pdFALSE , LONG_PRESS_TIME);
        delta_time = xTaskGetTickCount() - time1;
        printf("press time=%d\n",delta_time);
        if ((delta_time < SHORT_PRESS_TIME ) && ( evbits == EVENT_BIT_FALLING ))
        {
            rec_process = true; // click event to record
        }
        else if ((delta_time > (LONG_PRESS_TIME - 1) ) && ( evbits == 0 ))
        {
            rec_process = false; // long press button to flash
            xEventGroupSetBits(g_ndp_event_group, EVENT_BIT_FLASH);
            vTaskDelay (1);
        }
        else
        {
            rec_process = false;
        }

        while ( rec_process ) {
            if (file_create == 0) {
                // Turn on the recording LED
                turn_led(BSP_LEDGREEN, BSP_LEDON);

                if (is_record_motion()) //imu
                    snprintf(data_filename, sizeof(data_filename), "%s%04d.csv", IMU_REC_FILE_NAME_PREFIX, record_count);
                else //audio
                    snprintf(data_filename, sizeof(data_filename), "%s%04d.wav", AUDIO_REC_FILE_NAME_PREFIX, record_count);

                /* Reserve the position of a wav header */
                printf("Start to record extraction data \n");
                file_create = 1;
                record_count ++;

                if (is_record_motion()) { //imu
                    s = imu_record_operation(1);
                }
                else {
                    s = audio_record_operation(1, &sample_size);
                }
                    
                if (s) break;
            }
            else
            {
                if (is_record_motion()) { //imu
                    struct cb_sensor_arg_s cb_sensor_arg;
                    int wanted_sets = IMU_REC_BYTES_PER_SEC * get_recording_period();

                    memset(&cb_sensor_arg, 0, sizeof(struct cb_sensor_arg_s));
                    strcpy(cb_sensor_arg.file_name, data_filename);
                    cb_sensor_arg.sets_count = 0;

                    s = imu_record_process(wanted_sets, &cb_sensor_arg);
                    if ((!s) || (s == SYNTIANT_NDP_ERROR_DATA_REREAD)) {
                        printf("imu_record done got %d data_sets", cb_sensor_arg.sets_count);	
						if (is_imu_data_to_file()) {
							printf("and saved to %s", cb_sensor_arg.file_name);
						}
						printf("\n");
                    }
                    else {
                        printf("imu_record failed: %d\n", s);
                    }

                    s = imu_record_operation(0);
                }
                else {
                    struct cb_audio_arg_s cb_audio_arg;
                    int wanted_len = AUDIO_REC_BYTES_PER_SEC * get_recording_period();

                    memset(&cb_audio_arg, 0, sizeof(struct cb_audio_arg_s));
                    strcpy(cb_audio_arg.file_name, data_filename);
                    cb_audio_arg.total_len = 0;

                    s = audio_record_process(wanted_len, &cb_audio_arg);
                    if ((!s) || (s == SYNTIANT_NDP_ERROR_DATA_REREAD)) {
                        printf("audio_record done saved %d bytes to %s\n", 
                                cb_audio_arg.total_len, cb_audio_arg.file_name);
                    }
                    else {
                        printf("audio_record failed: %d\n", s);
                    }

                    s = audio_record_operation(0, NULL);
                }
				// Turn off the recording LED
				turn_led(BSP_LEDGREEN, BSP_LEDOFF);

                file_create = 0;
                rec_process = false;

                xEventGroupClearBits(g_ndp_event_group, EVENT_BIT_RISING);
                break;
            }
        }
    }
}
