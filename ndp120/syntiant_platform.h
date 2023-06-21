/*
 * SYNTIANT CONFIDENTIAL
 * _____________________
 *
 *   Copyright (c) 2018-2020 Syntiant Corporation
 *   All Rights Reserved.
 *
 *  NOTICE:  All information contained herein is, and remains the property of
 *  Syntiant Corporation and its suppliers, if any.  The intellectual and
 *  technical concepts contained herein are proprietary to Syntiant Corporation
 *  and its suppliers and may be covered by U.S. and Foreign Patents, patents in
 *  process, and are protected by trade secret or copyright law.  Dissemination
 *  of this information or reproduction of this material is strictly forbidden
 *  unless prior written permission is obtained from Syntiant Corporation.
 */

#ifndef _SYNTIANT_PLATFORM_H_
#define _SYNTIANT_PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "syntiant_common.h"

#define SYNTIANT_NDP_SDK_VERSION    "v100"

#define EXT_CLOCK_FREQ      (21504000)
#define PLL_FLL_CLOCK_FREQ  (32768)
#define PDM_CLOCK_FREQ      (768000)

#define STRING_LEN 256
#define MAX_LABELS 64

enum {
    SYNTIANT_NDP_ERROR_NONE = 0,    /**< operation successful */
    SYNTIANT_NDP_ERROR_FAIL = 1,    /**< general failure */
    SYNTIANT_NDP_ERROR_ARG = 2,     /**< invalid argument error */
    SYNTIANT_NDP_ERROR_UNINIT = 3,  /**< device unintialized or no fw loaded */
    SYNTIANT_NDP_ERROR_PACKAGE = 4, /**< package format error */
    SYNTIANT_NDP_ERROR_UNSUP = 5,   /**< operation not supported */
    SYNTIANT_NDP_ERROR_NOMEM = 6,   /**< out of memory */
    SYNTIANT_NDP_ERROR_BUSY = 7,    /**< operation in progress */
    SYNTIANT_NDP_ERROR_TIMEOUT = 8, /**< operation timeout */
    SYNTIANT_NDP_ERROR_MORE = 9,    /**< more data is expected */
    SYNTIANT_NDP_ERROR_CONFIG = 10, /**< config error */
    SYNTIANT_NDP_ERROR_CRC = 11,    /**< CRC mismatch */
    SYNTIANT_NDP_ERROR_INVALID_NETWORK = 12, /**< invalid network id */
    SYNTIANT_NDP_ERROR_DATA_REREAD = 13, /**<data has already been read before */
    SYNTIANT_NDP_ERROR_PACKAGE_PBI_TAG = 14, /**< package missing PBI tag */
    SYNTIANT_NDP_ERROR_PACKAGE_PBI_VER = 15, /**< package bad PBI version */
    SYNTIANT_NDP_ERROR_INVALID_LENGTH  = 16, /**< bad version length */
    SYNTIANT_NDP_ERROR_DSP_HDR_CRC = 17, /**< DSP header mismatch  */
    SYNTIANT_NDP_ERROR_LAST = SYNTIANT_NDP_ERROR_DSP_HDR_CRC
};

enum {
    SYNTIANT_NDP_FEATURE_NONE = 0,
    SYNTIANT_NDP_FEATURE_PDM = 1,
    SYNTIANT_NDP_FEATURE_SPI = 2,
    SYNTIANT_NDP_FEATURE_DNN = 3,
    SYNTIANT_NDP_FEATURE_LAST = SYNTIANT_NDP_FEATURE_DNN
};

enum {
    /* gpio */
    NDP_CORE2_CONFIG_VALUE_GPIO_DIR_OUT            = 0x00,
    NDP_CORE2_CONFIG_VALUE_GPIO_DIR_IN             = 0x01
};

enum {
    NDP_CORE2_CONFIG_CLOCK_OPTION_PLL               = 0,
    NDP_CORE2_CONFIG_CLOCK_OPTION_FLL               = 1,
    NDP_CORE2_CONFIG_CLOCK_OPTION_EXT               = 2
};



// sensor data callback
typedef void (*sensor_data_cb_f)(uint32_t extract_size, uint8_t *sensor_data, 
                    void *sensor_arg);
typedef void (*audio_data_cb_f)(uint32_t extract_size, uint8_t *audio_data, 
                    void *audio_arg);

extern int ndp_core2_platform_tiny_start(uint8_t clock_option, int use_xtal);
extern int ndp_core2_platform_tiny_feature_set(int feature_flag);

extern int ndp_core2_platform_tiny_transfer(int mcu, uint32_t addr, 
        void *out, void *in, unsigned int count);

extern int ndp_core2_platform_tiny_poll(uint32_t *notifications, int clear);
extern int ndp_core2_platform_tiny_match_process(uint8_t *match_id, uint8_t *nn_id, 
		uint8_t *sec_val, char *label_string);
        
extern int ndp_core2_platform_tiny_mspi_config(void);
extern int ndp_core2_platform_tiny_mspi_read(int ssb, int num_bytes, 
        uint8_t* data, int end_packet);
extern int ndp_core2_platform_tiny_mspi_write(int ssb, int num_bytes, 
        uint8_t* data, int end_packet);

extern int ndp_core2_platform_tiny_interrupts(int *cause);
extern int ndp_core2_platform_tiny_halt_mcu(void);
extern int ndp_core2_platform_tiny_vadmic_ctl(int mode);

extern int ndp_core2_platform_tiny_get_samplesize(uint32_t *sample_size);
extern uint32_t ndp_core2_platform_tiny_get_samplebytes(void);
extern uint32_t ndp_core2_platform_tiny_get_samplerate(void);

extern int ndp_core2_platform_tiny_get_recording_metadata(uint32_t *sample_size, 
        int get_from, uint32_t notify);
extern int ndp_core2_platform_tiny_notify_extract_data(uint8_t *data_buffer, 
        uint32_t sample_size, audio_data_cb_f audio_data_cb, void *audio_arg);

extern int ndp_core2_platform_tiny_extract_start(void);
extern int ndp_core2_platform_tiny_extract_stop(void);
#if 0
extern int ndp_core2_platform_tiny_match_extract_data(uint8_t *data_buffer, 
        audio_data_cb_f audio_data_cb);
#endif

extern int ndp_core2_platform_tiny_get_info(int *total_nn, int *total_labels);

extern int ndp_core2_platform_gpio_config(int gpio_num, 
        uint32_t dir, uint32_t value);

extern int ndp_core2_platform_tiny_sensor_ctl(int sensor_num, int enable);
extern int ndp_core2_platform_tiny_sensor_extract_data(uint8_t *data_buffer, 
        uint32_t sample_size, int sensor_num, 
        sensor_data_cb_f sensor_data_cb, void *sensor_arg);

extern void ndp_core2_platform_tiny_debug(void);

#ifdef __cplusplus
}
#endif

#endif //end of _SYNTIANT_PLATFORM_H_
