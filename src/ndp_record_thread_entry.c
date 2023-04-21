#include "ndp_record_thread.h"
#include "syntiant_platform.h"
#include "fat_load.h"
#include "button.h"


#define   ENABLE_RECORD_THREAD
#define   REC_BYTES_PER_SEC         32000U
#define   REC_TIME_SEC                   30 /* record 30s default */
#define   REC_FILE_NAME_PREFIX    "ndp_record_"
#define   REC_BUFFER_SIZE               1024
#define   SHORT_PRESS_TIME         pdMS_TO_TICKS(400UL)
#define   LONG_PRESS_TIME         pdMS_TO_TICKS(3000UL)

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

static void create_wav_header(struct wav_header_s *wav_hdr, int sample_bytes, int channels, int total_len);

/* Record Thread entry function */
/* pvParameters contains TaskHandle_t */
void ndp_record_thread_entry(void *pvParameters)
{
#ifdef  ENABLE_RECORD_THREAD
    int s;
	uint32_t sample_size;
    uint32_t sample_bytes;
	char data_filename[32];
	int match_count = 1;
	uint32_t file_create = 0;
	uint32_t actual_len = 0;
	int total_len = REC_TIME_SEC * REC_BYTES_PER_SEC;
	struct wav_header_s wav_hdr;
	EventBits_t   evbits;
	TickType_t time1 = 0, delta_time = 0;
	bool  rec_process = false;

    FSP_PARAMETER_NOT_USED (pvParameters);
	/* Start recording after 6 seconds */
	vTaskDelay (pdMS_TO_TICKS(3000UL));
	printf("Record_thread running\n");

	if (get_synpkg_boot_mode() != BOOT_MODE_SD) {
		printf("INFO: NDP record data should be saved to SD card,  \n");
		printf("Exit Record_thread!\n");
		return;
	}
#if 0
    s = ndp_core2_platform_tiny_get_samplesize(&sample_size);
    if (s) {
        perror("get sample size failed\n");
        return;;
    }

    sample_bytes = ndp_core2_platform_tiny_get_samplebytes();
#endif
	/*sample_bytes =2, sample_size=772*/

#ifdef   ENABLE_IMU_DBG
    {
        uint8_t imu_val = 0;
        uint8_t reg = 0x75 | 0x80 ; /*WHO_AM_I*/

        ndp_core2_platform_tiny_mspi_config();
        ndp_core2_platform_tiny_mspi_write(1, 1, &reg, 0);
        ndp_core2_platform_tiny_mspi_read(1, 1, &imu_val, 1);
        printf("IMU ID = 0x%02x\n", imu_val); /*id = 0x67*/
    }
#endif

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

#if 0
        while ( rec_process ) {
            if (file_create == 0) {
                snprintf(data_filename, sizeof(data_filename), "%s%04d.wav", REC_FILE_NAME_PREFIX, match_count);
                /* Reserve the position of a wav header */
                printf("Start to record data to %s \n", data_filename);
                create_wav_header(&wav_hdr, sample_bytes, 1, total_len);

                xSemaphoreTake(g_ndp_mutex,portMAX_DELAY);
                write_wav_file(data_filename, (uint8_t *)&wav_hdr,  sizeof(wav_hdr), 1);
                xSemaphoreGive(g_ndp_mutex);
                file_create = 1;
            }
            else
            {
                uint32_t extract_len;
                uint8_t *data_ptr;

                data_ptr = pvPortMalloc(REC_BUFFER_SIZE);
                if ( ! data_ptr)   continue;
                memset( data_ptr, 0x0, REC_BUFFER_SIZE );

                /* Get FIFO data about 48000 bytes , aka 1.5s record */
                while(1) {
                    /* to record */
                    extract_len = REC_BUFFER_SIZE;
                    s = ndp_core2_platform_tiny_extract_data(data_ptr, &extract_len);
                    if((s) || (!extract_len)) {
                        //printf("totally extracted %d bytes from wakeword\n", actual_len);
                        break;
                    }

                    /* single channel */
                    xSemaphoreTake(g_ndp_mutex,portMAX_DELAY);
                    write_wav_file(data_filename, (uint8_t *)data_ptr,  extract_len, 0);
                    xSemaphoreGive(g_ndp_mutex);

                    actual_len += extract_len;
                    if (actual_len >= total_len) {
                        printf("enough query %d bytes for recording\n", actual_len);
                        break;
                    }
                }

                if (actual_len >= total_len) {
                    file_create = 0;
                    actual_len = 0;
                    match_count ++;
                    xEventGroupClearBits(g_ndp_event_group, EVENT_BIT_RISING);
                    break;
                }
                vPortFree( data_ptr );

                vTaskDelay (pdMS_TO_TICKS(500));
            }
        }
#endif
    }
#else
    vTaskDelete(NULL);
    vTaskDelay (1);
#endif
}

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
