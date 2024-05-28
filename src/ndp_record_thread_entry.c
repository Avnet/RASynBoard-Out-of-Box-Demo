#include "ndp_record_thread.h"
#include "syntiant_platform.h"
#include "syntiant_common.h"
#include "fat_load.h"
#include "button.h"
#include "led.h"
#include <stdio.h>
#include "ndp_irq_service.h"
#include "led.h"
#include "usb_pcdc_vcom.h"

#define   AUDIO_REC_BYTES_PER_SEC         32000U
#define   AUDIO_REC_BUFFER_SIZE            2048
#define   AUDIO_REC_FILE_NAME_PREFIX    "ndp_audio_record_"

#define   IMU_REC_BYTES_PER_SEC          200
#define   IMU_REC_BUFFER_SIZE            1024
#define   IMU_REC_FILE_NAME_PREFIX      "ndp_imu_record_raw_"
#define   IMU_REC_FILE_NAME_CONVERTED_PREFIX  "ndp_imu_record_converted_"


#define   SHORT_PRESS_TIME        pdMS_TO_TICKS(400UL)
#define   LONG_PRESS_TIME         pdMS_TO_TICKS(3000UL)

#define   IMU_SENSOR_INDEX         0

#define CONVERT_G_TO_MS2            (9.80665f)
#define ACC_RAW_SCALING             (32767.5f)
#define ACC_SCALE_FACTOR            (float)((2.0f*CONVERT_G_TO_MS2)/ACC_RAW_SCALING)

#define CONVERT_ADC_GYR             (float)(250.0f/32768.0f)
#define NORMALIZE_GYR               (250.0f)

/** Number of axis used and sample data format */
#define INERTIAL_AXIS_SAMPLED       6
#define ARRAY_SIZE(x)   (sizeof(x)/sizeof(*(x)))

/** IMU DEBUG */
//#define MSPI_READ_IMU

enum short_press_button_to_record {
	REC_AUDIO = 1,
	REC_IMU = 2,
	REC_NONE = 11,
};

static int get_button_mapping_event(void)
{
	int ret = REC_NONE;
	if (memcmp (config_items.button_switch, "audio", 5) == 0 )
		ret = REC_AUDIO;
	if (memcmp (config_items.button_switch, "imu", 3) == 0 )
		ret = REC_IMU;

    return ret;
}

static int is_record_motion(void)
{
    return (get_button_mapping_event() == REC_IMU);
}

char* extract_process_percent(uint32_t extracted_len, uint32_t wanted_len, 
        uint32_t sample_size)
{
    uint32_t quarter_len = wanted_len>>2;
    uint32_t half_len = wanted_len>>1;
    uint32_t most_quarter_len = quarter_len*3;

    if ((extracted_len - most_quarter_len) < sample_size) {
        return "...75%";
    }
    else if ((extracted_len - half_len) < sample_size) {
        return "...50%";
    }
    else if ((extracted_len - quarter_len) < sample_size) {
        return "...25%";
    }
    else
        return NULL;
}

/******************************************
 * IMU record
******************************************/
static int imu_record_operation(int isstart)
{
    int s = 0;

    if (isstart) {
        ndp_irq_disable();
        
        // disable pdm clk for confusion if audio enabled
        if (get_event_watch_mode() & WATCH_TYPE_AUDIO) {
            s = ndp_core2_platform_tiny_feature_set(NDP_CORE2_FEATURE_NONE);
            if (s){
                printf("feature set 0x%x failed %d\r\n", NDP_CORE2_FEATURE_NONE, s);
            }
        }
        
        // enable sensor if sensor disable
        if (!(get_event_watch_mode() & WATCH_TYPE_MOTION)) {
            s = ndp_core2_platform_tiny_sensor_ctl(IMU_SENSOR_INDEX, 1);
            if (s) {
                printf("enable sneosr[%d] failed: %d\n", IMU_SENSOR_INDEX, s);
            }
        }

        s = ndp_core2_platform_tiny_config_interrupts(
                    NDP_CORE2_INTERRUPT_EXTRACT_READY, 1);
        if (s) {
            printf("enable extract interrupt failed: %d\n", s);
            return s;
        }
    }
    else {
        s = ndp_core2_platform_tiny_config_interrupts(
                    NDP_CORE2_INTERRUPT_EXTRACT_READY, 0);
        if (s) {
            printf("disable extract interrupt failed: %d\n", s);
            return s;
        }
        
        // enable pdm clk if audio enabled
        if (get_event_watch_mode() & WATCH_TYPE_AUDIO) {
            s = ndp_core2_platform_tiny_feature_set(NDP_CORE2_FEATURE_PDM);
            if (s){
                printf("feature set 0x%x failed %d\r\n", NDP_CORE2_FEATURE_PDM, s);
            }
        }
        
        // disable sensor if sensor disable
        if (!(get_event_watch_mode() & WATCH_TYPE_MOTION)) {
            s = ndp_core2_platform_tiny_sensor_ctl(IMU_SENSOR_INDEX, 0);
            if (s) {
                printf("disable sneosr[%d] failed: %d\n", IMU_SENSOR_INDEX, s);
            }
        }

        ndp_irq_enable();
    }

    return s;
}

struct cb_sensor_arg_s {
    char file_name[64];
    uint32_t sets_count;
    uint32_t wanted_sets;
};

#define SENSOR_SAMPLE_SIZE  (6)
void icm42670_extraction_cb(uint32_t sample_size, uint8_t *sensor_data, void *sensor_arg)
{
    struct cb_sensor_arg_s *cb_sensor_arg = (struct cb_sensor_arg_s*)sensor_arg;
    uint16_t i, j, index = 0;
    int16_t *acc_samples = (int16_t *)(sensor_data);
    char *percent_ptr = NULL;
    float acc_converted_samples[sample_size];


    // If we're capturing converted IMU data, then do the conversion.  acc_converted_samples will
    // hold the converted float data.
    if(is_imu_convertion_enabled()){

        float acc_converted_samples[sample_size];

        for (j = 0; j < (sample_size/INERTIAL_AXIS_SAMPLED); j++) {
            for (i = 0; i < 3; i++) {
                acc_converted_samples[(j * INERTIAL_AXIS_SAMPLED) + i] = 
                        acc_samples[(j * INERTIAL_AXIS_SAMPLED) + i] * ACC_SCALE_FACTOR;

            }

            for (i = 3; i < INERTIAL_AXIS_SAMPLED; i++) {
                acc_converted_samples[(j * INERTIAL_AXIS_SAMPLED) + i] = 
                        acc_samples[(j * INERTIAL_AXIS_SAMPLED) + i] * CONVERT_ADC_GYR;
            }
        }

        if (is_imu_data_to_terminal()) {
            // show data on the serial console

            // Output converted samples
            if(is_imu_convertion_enabled()){

                index = sample_size / 2 - 1;
                for (i = 0; i < index; i++) {
                    printf("%f,", acc_converted_samples[i]);
                }
                printf("%f\n", acc_converted_samples[index]);

            }
            // Output RAW ADC values
            else{

                index = sample_size / 2 - 1;
                for (i = 0; i < index; i++) {
                    printf("%x,", acc_samples[i]);
                }
                printf("%x\n", acc_samples[index]);
            }
        }

        if (is_imu_data_to_file()) {
            // save data to sdcard
            xSemaphoreTake(g_ndp_mutex,portMAX_DELAY);
            write_sensor_file(cb_sensor_arg->file_name, sample_size, NULL, 0, acc_converted_samples);

            // Catch the case where we're recording IMU data to a file and to the debug UART.  I'm not sure if
            // there is a valid use case to do this, but the user can configure the app to do this.  If we're
            // outputting data to the terminal we don't want to output the % done
            if(!is_imu_data_to_terminal()){

                percent_ptr = extract_process_percent(cb_sensor_arg->sets_count, cb_sensor_arg->wanted_sets, 1);
                if (percent_ptr) printf("%s", percent_ptr);
            }
            xSemaphoreGive(g_ndp_mutex);
        }


    }
    // Otherwise, we capture the ADC values read from the IMU sensor
    else{
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
            write_sensor_file(cb_sensor_arg->file_name, sample_size, acc_samples, 0, NULL);

            // Catch the case where we're recording IMU data to a file and to the debug UART.  I'm not sure if
            // there is a valid use case to do this, but the user can configure the app to do this.  If we're
            // outputting data to the terminal we don't want to output the % done
            if(!is_imu_data_to_terminal()){

                percent_ptr = extract_process_percent(cb_sensor_arg->sets_count, cb_sensor_arg->wanted_sets, 1);
                if (percent_ptr) printf("%s", percent_ptr);
            }
            xSemaphoreGive(g_ndp_mutex);
        }
    }
    cb_sensor_arg->sets_count ++;
}

#ifdef MSPI_READ_IMU
#define   IMU_SENSOR_MSSB          1
#define REG_READ                  (0x80)
#define REG_FIFO_COUNT_H          (0x3D)
#define REG_FIFO_DATA             (0x3F)
#define FIFO_HEADER_ACCEL         (0x40)
#define FIFO_HEADER_GYRO          (0x20)
#define FIFO_HEADER_ACCEL_GYRO   (FIFO_HEADER_ACCEL | FIFO_HEADER_ACCEL)

#define INVALID_FIFO_COUNT       (0xffff)
uint16_t read_fifo_count(void)
{
    uint16_t count;
    uint8_t xdata;
    uint8_t rdata[16];

    xdata = REG_FIFO_COUNT_H | REG_READ;
    ndp_core2_platform_tiny_mspi_write(IMU_SENSOR_MSSB, 1, &xdata, 0);
    ndp_core2_platform_tiny_mspi_read(IMU_SENSOR_MSSB, 2, &rdata, 1);

    memcpy(&count, rdata, 2);
    return count;
}

typedef union {
    uint16_t value;
    struct {
        uint16_t acc_x: 1;
        uint16_t acc_y: 1;
        uint16_t acc_z: 1;
        uint16_t gyro_x: 1;
        uint16_t gyro_y: 1;
        uint16_t gyro_z: 1;
    } bits;
} axes_t;

typedef enum {
    SCALE_8_BIT,
    SCALE_16_BIT,
} dnn_scale_t;

#define NUM_AXES 6
#define BYTES_PER_AXIS 2

uint8_t read_samples(const uint8_t *data, axes_t axes, dnn_scale_t scale,
                            uint8_t *output) {
    int i, output_i = 0;
    // `axes` will be either for the holding tank or the DNN, read the axes
    // which are enabled
    for (i = 0; i < NUM_AXES; i++) {
        if (axes.value & (1 << i)) {
            // data is little endian
            if (scale == SCALE_16_BIT) {
                output[output_i++] = *data;
            }
            output[output_i++] = *(data + 1);
        }
        data += 2;
    }

    return output_i;
}

uint8_t sensor_data[NUM_AXES * BYTES_PER_AXIS];
void imu_mspi_record_process(struct cb_sensor_arg_s *sensor_arg)
{
    uint16_t fifo_count;
    uint8_t xdata;
    uint8_t rdata[16];
    axes_t tank_axes;
    uint8_t output_size;

    fifo_count = read_fifo_count();
    if (fifo_count == INVALID_FIFO_COUNT) {
        return;
    }

    tank_axes.value = 0x3F;
    for (int i = 0; i < fifo_count; i++) {
        xdata =  REG_FIFO_DATA | REG_READ;
        ndp_core2_platform_tiny_mspi_write(IMU_SENSOR_MSSB, 1, &xdata, 0);
        ndp_core2_platform_tiny_mspi_read(IMU_SENSOR_MSSB, 16, &rdata, 1);

        // make sure we've got both acc and gyro data in the packet
        if ((rdata[0] & FIFO_HEADER_ACCEL_GYRO) != FIFO_HEADER_ACCEL_GYRO) {
            continue;
        }

        output_size = read_samples(&rdata[1], tank_axes, SCALE_16_BIT, sensor_data);
        if (output_size > 0) {
            icm42670_extraction_cb(output_size, sensor_data, sensor_arg);
        }
    }
}
#endif

static int imu_record_process(int extract_sets, struct cb_sensor_arg_s *sensor_arg)
{
    int s;
    uint32_t save_sample_size;
    int max_num_frames;
    uint8_t *data_ptr = NULL;

    data_ptr = pvPortMalloc(IMU_REC_BUFFER_SIZE);
    if (!data_ptr) return -1;    
    
    s = ndp_core2_platform_tiny_get_sensor_sample_size(&save_sample_size);
    if (s) return s;

//    printf("save_sample_size; %d\n", save_sample_size);
    max_num_frames = IMU_REC_BUFFER_SIZE / save_sample_size;

	if (is_imu_data_to_file()) {
		xSemaphoreTake(g_ndp_mutex,portMAX_DELAY);
		write_sensor_file(sensor_arg->file_name, 0, NULL, 1, NULL);
		xSemaphoreGive(g_ndp_mutex);
	}
	if (is_imu_data_to_terminal()) {
		printf("\naccX,accY,accZ,gyrX,gyrY,gyrZ\n");
	}

    while (extract_sets > sensor_arg->sets_count) {
#ifdef MSPI_READ_IMU
        imu_mspi_record_process(sensor_arg);
        vTaskDelay (pdMS_TO_TICKS(2UL));
#else
        s = ndp_core2_platform_tiny_sensor_extract_data(data_ptr, 
                IMU_SENSOR_INDEX, save_sample_size, max_num_frames, 
                (!sensor_arg->sets_count)?1:0, 
                icm42670_extraction_cb, sensor_arg);
        if ((s) && (s != NDP_CORE2_ERROR_DATA_REREAD)) {
            printf("sensor extract data failed: %d\n", s);
            break;
        }
#endif
    }

    write_extraction_file_end();

    if (data_ptr) vPortFree(data_ptr);

    return s;
}

/******************************************
 * Audio record
******************************************/
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

static void create_wav_header(struct wav_header_s *wav_hdr, int sample_bytes, int channels, int extracted_len)
{
    wav_hdr->riff[0] = 'R';
    wav_hdr->riff[1] = 'I';
    wav_hdr->riff[2] = 'F';
    wav_hdr->riff[3] = 'F';
    wav_hdr->file_size = 36 + extracted_len;
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
    wav_hdr->data_size = extracted_len;
}

static void audio_record_operation(int isstart)
{
    int s;
    if (isstart) {
        ndp_irq_disable();

        // enable pdm clk if audio disabled
        if (!(get_event_watch_mode() & WATCH_TYPE_AUDIO)) {
            s = ndp_core2_platform_tiny_feature_set(NDP_CORE2_FEATURE_PDM);
            if (s){
                printf("feature set 0x%x failed %d\r\n", NDP_CORE2_FEATURE_PDM, s);
            }
        }
        
        // disable sensor for confusion if sensor enabled
        if (get_event_watch_mode() & WATCH_TYPE_MOTION) {
            s = ndp_core2_platform_tiny_sensor_ctl(IMU_SENSOR_INDEX, 0);
            if (s) {
                printf("disable sneosr[%d] failed: %d\n", IMU_SENSOR_INDEX, s);
            }
        }

        s = ndp_core2_platform_tiny_config_interrupts(
                    NDP_CORE2_INTERRUPT_EXTRACT_READY, 1);
        if (s) {
            printf("enable extract interrupt failed: %d\n", s);
        }
    }
    else {
        s = ndp_core2_platform_tiny_feature_set(NDP_CORE2_FEATURE_NONE);
        if (s){
            printf("feature set 0x%x failed %d\r\n", NDP_CORE2_FEATURE_NONE, s);
        }
        
        s = ndp_core2_platform_tiny_config_interrupts(
                    NDP_CORE2_INTERRUPT_EXTRACT_READY, 0);
        if (s) {
            printf("disable extract interrupt failed: %d\n", s);
        }
        
        // re-enable pdm clock if audio enabled
        if (get_event_watch_mode() & WATCH_TYPE_AUDIO) {
            s = ndp_core2_platform_tiny_feature_set(NDP_CORE2_FEATURE_PDM);
            if (s){
                printf("feature set 0x%x failed %d\r\n", NDP_CORE2_FEATURE_PDM, s);
            }
        }
        
        // re-enable sensor if sensor enabled
        if (get_event_watch_mode() & WATCH_TYPE_MOTION) {
            s = ndp_core2_platform_tiny_sensor_ctl(IMU_SENSOR_INDEX, 1);
            if (s) {
                printf("enable sneosr[%d] failed: %d\n", IMU_SENSOR_INDEX, s);
            }
        }

        ndp_irq_enable();
    }
}

struct cb_audio_arg_s {
    char file_name[64];
    uint32_t sample_size;
    uint32_t extracted_len;
    uint32_t wanted_len;
};

void audio_extraction_cb (uint32_t extract_size, uint8_t *audio_data, 
                    void *audio_arg)
{
    struct cb_audio_arg_s *cb_audio_arg = (struct cb_audio_arg_s*)audio_arg;
    char *percent_ptr = NULL;

    if (extract_size > 0) {
        uint32_t data_size;
        int audio_type = ndp_core2_platform_tiny_src_type(audio_data, &data_size);

        switch (audio_type) {
            case NDP_CORE2_FLOW_SRC_TYPE_PCM0:
                xSemaphoreTake(g_ndp_mutex,portMAX_DELAY);
                write_wav_file(cb_audio_arg->file_name, audio_data, data_size, 0);

                percent_ptr = extract_process_percent(cb_audio_arg->extracted_len, 
                        cb_audio_arg->wanted_len, extract_size);
                if (percent_ptr) printf("%s", percent_ptr);

                xSemaphoreGive(g_ndp_mutex);
            break;

            case NDP_CORE2_FLOW_SRC_TYPE_PCM1:
            case NDP_CORE2_FLOW_SRC_TYPE_FUNC:
            default:
            break;
        }

        cb_audio_arg->extracted_len += data_size;
    }
    vTaskDelay (pdMS_TO_TICKS(1UL));
}

static int audio_record_process(int wanted_len, struct cb_audio_arg_s *audio_arg)
{
    int s = 0;
	struct wav_header_s wav_hdr;
    uint32_t sample_size;
    uint32_t sample_bytes = ndp_core2_platform_tiny_get_samplebytes();
    uint32_t audio_chunk_size;
    uint8_t *data_ptr = NULL;

    data_ptr = pvPortMalloc(AUDIO_REC_BUFFER_SIZE);
    if (!data_ptr) return -1;

    create_wav_header(&wav_hdr, sample_bytes, 1, wanted_len);
    xSemaphoreTake(g_ndp_mutex,portMAX_DELAY);
    write_wav_file(audio_arg->file_name, (uint8_t *)&wav_hdr, sizeof(wav_hdr), 1);
    xSemaphoreGive(g_ndp_mutex);

    printf("To audio record %d bytes for %d seconds\n", wanted_len, get_recording_period());
    fflush(stdin);
    /* sample ready interrupt is enabled in MCU firmware */
    s = ndp_core2_platform_tiny_get_audio_chunk_size(&audio_chunk_size, &sample_size);
    if (s) {
        printf("audio record get audio chunk size failed: %d\n", s);
        goto process_out;
    }

    while (wanted_len > audio_arg->extracted_len) {
        s = ndp_core2_platform_tiny_notify_extract_data(data_ptr, 
                sample_size, audio_extraction_cb, audio_arg);
        if (s == NDP_CORE2_ERROR_DATA_REREAD) {
            vTaskDelay (pdMS_TO_TICKS(1UL));
            continue;
        }

        if (s) {
            printf("audio extract data failed: %d\n", s);
            break;
        }
    }

process_out:
#if 1
    write_extraction_file_end();
#endif
    if (data_ptr) vPortFree(data_ptr);

    return s;
}

static void check_record_file_name(char *fname, int *findex)
{
    char valid_filename[40] = {0};
    int count = *findex;
    do {
        if (is_record_motion()){//imu
            if(is_imu_convertion_enabled()){
                snprintf(valid_filename, sizeof(valid_filename), "%s%04d.csv", \
                IMU_REC_FILE_NAME_CONVERTED_PREFIX, count);
            }
            else { // capturing RAW values
                snprintf(valid_filename, sizeof(valid_filename), "%s%04d.csv", \
                IMU_REC_FILE_NAME_PREFIX, count);
            }
            if (is_imu_data_to_file() == IMU_FUNC_DISABLE)
                break;
        } else {//audio
            snprintf(valid_filename, sizeof(valid_filename), "%s%04d.wav", \
            AUDIO_REC_FILE_NAME_PREFIX, count);
        }
        /* check if the file exist */
        if (is_file_exist_in_sdcard(valid_filename))
            break;

        count ++;
    }while(1);
    *findex = count;
    memcpy(fname, valid_filename, sizeof(valid_filename));
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

    FSP_PARAMETER_NOT_USED (pvParameters);
	/* Start recording after 6 seconds */
	vTaskDelay (pdMS_TO_TICKS(1000UL));
	printf("Record_thread running\n");

	if (SDCARD_IN_SLOT != get_sdcard_slot_status()) {
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
			if (BOOT_MODE_FLASH != get_synpkg_boot_mode())
			{
				xEventGroupSetBits(g_ndp_event_group, EVENT_BIT_FLASH);
			}
			else
			{
				printf("Not support updating Flash when booting from flash!\n");
			}
            vTaskDelay (1);
        }
        else
        {
            rec_process = false;
        }

        // If we're recording IMU data, but have not defined a place to put the data, output
        // an error message and abort the record logic.
        if(rec_process && is_record_motion() && !is_imu_data_to_file() && !is_imu_data_to_terminal()){
            printf("Invalid IMU recording configuration!  Not recording any data!\n");
            printf("Enable at least one recording location in the config.ini file \n");
            printf("[IMU data stream] section.\n");

            xEventGroupClearBits(g_ndp_event_group, EVENT_BIT_RISING);

        }
        else{

            while ( rec_process ) {
                if (file_create == 0) {
                    // Turn on the recording LED
                    turn_led(BSP_LEDGREEN, BSP_LEDON);

                    check_record_file_name(data_filename, &record_count);

                    /* Reserve the position of a wav header */
                    printf("Start to record extraction data \n");
                    file_create = 1;
                    record_count ++;
                    usb_disable();

                    if (is_record_motion()) { //imu
                        s = imu_record_operation(1);
                        if (s)  break;
                    }
                    else {
                        audio_record_operation(1);
                    }
                }
                else
                {
                    if (is_record_motion()) { //imu
                        struct cb_sensor_arg_s cb_sensor_arg;
                        int wanted_sets = IMU_REC_BYTES_PER_SEC * get_recording_period();

                        memset(&cb_sensor_arg, 0, sizeof(struct cb_sensor_arg_s));
                        strcpy(cb_sensor_arg.file_name, data_filename);
                        cb_sensor_arg.sets_count = 0;
                        cb_sensor_arg.wanted_sets = wanted_sets;

                        s = imu_record_process(wanted_sets, &cb_sensor_arg);
                        if ((!s) || (s == NDP_CORE2_ERROR_DATA_REREAD)) {
                            printf("...100%% ");
                            fflush(stdin);
                            printf("\nimu_record done got %d data_sets", cb_sensor_arg.sets_count-1);
                            if (is_imu_data_to_file()) {
                                printf(" and saved to %s", cb_sensor_arg.file_name);
                            }
                            printf("\n");
                        }
                        else {
                            printf("imu_record failed: %d\n", s);
                        }

                        s = imu_record_operation(0);
                        if (s)  break;
                    }
                    else {
                        struct cb_audio_arg_s cb_audio_arg;
                        int wanted_len = AUDIO_REC_BYTES_PER_SEC * get_recording_period();

                        memset(&cb_audio_arg, 0, sizeof(struct cb_audio_arg_s));
                        strcpy(cb_audio_arg.file_name, data_filename);
                        cb_audio_arg.extracted_len = 0;
                        cb_audio_arg.wanted_len = wanted_len;

                        s = audio_record_process(wanted_len, &cb_audio_arg);
                        if ((!s) || (s == NDP_CORE2_ERROR_DATA_REREAD)) {
                            printf("...100%% ");
                            fflush(stdin);
                            printf("\naudio_record done saved %d bytes to %s",
                                    cb_audio_arg.extracted_len, cb_audio_arg.file_name);
                            printf("\n");
                        }
                        else {
                            printf("audio_record failed: %d\n", s);
                        }
                        audio_record_operation(0);
                    }
                    // Turn off the recording LED
                    turn_led(BSP_LEDGREEN, BSP_LEDOFF);

                    file_create = 0;
                    rec_process = false;
                    usb_enable();

                    xEventGroupClearBits(g_ndp_event_group, EVENT_BIT_RISING);
                    break;
                }
            }
        }
    }
}
