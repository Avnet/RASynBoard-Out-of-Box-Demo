/*
 * Copyright (c) 2022 Syntiant Corp.  All rights reserved.
 * Contact at http://www.syntiant.com
 *
 * This software is available to you under a choice of one of two licenses.
 * You may choose to be licensed under the terms of the GNU General Public
 * License (GPL) Version 2, available from the file LICENSE in the main
 * directory of this source tree, or the OpenIB.org BSD license below.  Any
 * code involving Linux software will require selection of the GNU General
 * Public License (GPL) Version 2.
 *
 * OPENIB.ORG BSD LICENSE
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 	** SDK: v103 **
*/

#ifndef _SYNTIANT_PLATFORM_H_
#define _SYNTIANT_PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "syntiant_common.h"

#define SYNTIANT_NDP_SDK_VERSION    "v103"

#define EXT_CLOCK_FREQ      (21504000)
#define PLL_FLL_CLOCK_FREQ  (32768)
#define PDM_CLOCK_FREQ      (768000)

#define STRING_LEN 256
#define MAX_LABELS 64

enum {
    NDP_CORE2_ERROR_NONE = 0,    /**< operation successful */
    NDP_CORE2_ERROR_FAIL = 1,    /**< general failure */
    NDP_CORE2_ERROR_ARG = 2,     /**< invalid argument error */
    NDP_CORE2_ERROR_UNINIT = 3,  /**< device unintialized or no fw loaded */
    NDP_CORE2_ERROR_PACKAGE = 4, /**< package format error */
    NDP_CORE2_ERROR_UNSUP = 5,   /**< operation not supported */
    NDP_CORE2_ERROR_NOMEM = 6,   /**< out of memory */
    NDP_CORE2_ERROR_BUSY = 7,    /**< operation in progress */
    NDP_CORE2_ERROR_TIMEOUT = 8, /**< operation timeout */
    NDP_CORE2_ERROR_MORE = 9,    /**< more data is expected */
    NDP_CORE2_ERROR_CONFIG = 10, /**< config error */
    NDP_CORE2_ERROR_CRC = 11,    /**< CRC mismatch */
    NDP_CORE2_ERROR_INVALID_NETWORK = 12, /**< invalid network id */
    NDP_CORE2_ERROR_DATA_REREAD = 13, /**<data has already been read before */
    NDP_CORE2_ERROR_PACKAGE_PBI_TAG = 14, /**< package missing PBI tag */
    NDP_CORE2_ERROR_PACKAGE_PBI_VER = 15, /**< package bad PBI version */
    NDP_CORE2_ERROR_INVALID_LENGTH  = 16, /**< bad version length */
    NDP_CORE2_ERROR_DSP_HDR_CRC = 17, /**< DSP header mismatch  */
    NDP_CORE2_ERROR_SPI_READ_FAILURE = 18, /**< Spi Read Failure */
    NDP_CORE2_ERROR_LAST = NDP_CORE2_ERROR_SPI_READ_FAILURE
};

/**
 * @brief interrupt enable types
 */
enum {
    NDP_CORE2_INTERRUPT_MATCH = 0x01,
    /**< external INT on match */
    NDP_CORE2_INTERRUPT_MAILBOX_IN = 0x02,
    /**< mbox response */
    NDP_CORE2_INTERRUPT_MAILBOX_OUT = 0x04,
    /**< mbox request */
    NDP_CORE2_INTERRUPT_DNN_FRAME = 0x08,
    /**< DNN frame */
    NDP_CORE2_INTERRUPT_FEATURE = 0x10,
    /**< filterbank completion */
    NDP_CORE2_INTERRUPT_ADDRESS_ERROR = 0x20,
    /**< SPI address error */
    NDP_CORE2_INTERRUPT_WATER_MARK = 0x40,
    /**< input buffer water mark */
    NDP_CORE2_INTERRUPT_SPI_READ_FAILURE = 0x80,
    /**< spi read failure */
    NDP_CORE2_INTERRUPT_EXTRACT_READY = 0x100,
    /**< extract ready */
    NDP_CORE2_INTERRUPT_ALL = 0x1FF,
    NDP_CORE2_INTERRUPT_DEFAULT = 0x200
    /**< enable the chip-specific default interrupts */
};

/**
 * @brief extract source enable types
 */
enum {
    /**< PCM0 */
    NDP_CORE2_FLOW_SRC_TYPE_PCM0 = 0,
    /**< PCM1 */
    NDP_CORE2_FLOW_SRC_TYPE_PCM1 = 1,
    /**< FUNC */
    NDP_CORE2_FLOW_SRC_TYPE_FUNC = 2,

    NDP_CORE2_FLOW_SRC_TYPE_NONE
};

enum {
    NDP_CORE2_FEATURE_NONE = 0,
    NDP_CORE2_FEATURE_PDM = 1,
    NDP_CORE2_FEATURE_SPI = 2,
    NDP_CORE2_FEATURE_DNN = 3,
    NDP_CORE2_FEATURE_LAST = NDP_CORE2_FEATURE_DNN
};

enum {
    NDP_CORE2_GET_FROM_ILIB = 0,
    NDP_CORE2_GET_FROM_MCU
};

enum {
    /* gpio */
    NDP_CORE2_CONFIG_VALUE_GPIO_DIR_OUT            = 0x00,
    NDP_CORE2_CONFIG_VALUE_GPIO_DIR_IN             = 0x01
};

enum {
    NDP_CORE2_CONFIG_CLOCK_OPTION_EXT               = -1,
    NDP_CORE2_CONFIG_CLOCK_OPTION_FLL               = 0,
    NDP_CORE2_CONFIG_CLOCK_OPTION_PLL               = 1
};

enum {
    NDP_CORE2_BOOT_MODE_HOST_CODE                   = 0,
    NDP_CORE2_BOOT_MODE_HOST_FLASH                  = 1,
    NDP_CORE2_BOOT_MODE_BOOT_FLASH                  = 2,
    NDP_CORE2_BOOT_MODE_HOST_FILE                   = 3,
    NDP_CORE2_BOOT_MODE_NONE
};


extern int ndp_core2_platform_tiny_start(uint8_t clock_option, int use_xtal, 
        int boot_mode);
extern int ndp_core2_platform_tiny_recover(void);
extern int ndp_core2_platform_tiny_feature_set(int feature_flag);

extern int ndp_core2_platform_tiny_transfer(int mcu, uint32_t addr, 
        void *out, void *in, unsigned int count);

extern int ndp_core2_platform_tiny_poll(uint32_t *notifications, int clear);
extern int ndp_core2_platform_tiny_match_process(uint8_t *match_id, uint8_t *nn_id, 
		uint8_t *sec_val, char *label_string);

extern int ndp_core2_platform_tiny_interrupts(int *cause);
extern int ndp_core2_platform_tiny_config_interrupts(uint32_t interrupt, int enable);
extern int ndp_core2_platform_tiny_vadmic_ctl(int mode);
extern int ndp_core2_platform_tiny_halt_mcu(void);
		
#ifndef EXCLUDE_TINY_CSPI        
extern int ndp_core2_platform_tiny_mspi_config(void);
extern int ndp_core2_platform_tiny_mspi_read(int ssb, int num_bytes, 
        uint8_t* data, int end_packet);
extern int ndp_core2_platform_tiny_mspi_write(int ssb, int num_bytes, 
        uint8_t* data, int end_packet);

extern int ndp_core2_platform_tiny_flash_get_id(uint8_t *data);
extern int ndp_core2_platform_tiny_flash_read_data(uint32_t addr, 
        uint8_t *data, unsigned int len);
extern int ndp_core2_platform_tiny_flash_sector_erase(uint32_t addr);
extern int ndp_core2_platform_tiny_flash_page_program(uint32_t addr, 
        uint8_t *data, unsigned int len);
#endif //EXCLUDE_TINY_CSPI

#ifndef EXCLUDE_TINY_EXTRACTION
typedef void (*audio_data_cb_f)(uint32_t extract_size, uint8_t *audio_data, 
                    void *audio_arg);

extern uint32_t ndp_core2_platform_tiny_get_samplebytes(void);
extern uint32_t ndp_core2_platform_tiny_get_samplerate(void);

extern int ndp_core2_platform_tiny_get_recording_metadata(uint32_t *sample_size, 
        int get_from);
extern int ndp_core2_platform_tiny_notify_extract_data(uint8_t *data_buffer, 
        uint32_t sample_size, audio_data_cb_f audio_data_cb, void *audio_arg);

extern int ndp_core2_platform_tiny_extract_start(void);
extern int ndp_core2_platform_tiny_extract_stop(void);
extern int ndp_core2_platform_tiny_src_type(uint8_t *data, uint32_t *data_size);
#endif //EXCLUDE_TINY_EXTRACTION

extern int ndp_core2_platform_tiny_gpio_config(int gpio_num, 
        uint32_t dir, uint32_t value);

#ifndef EXCLUDE_SENSOR_FEATURE
typedef void (*sensor_data_cb_f)(uint32_t extract_size, uint8_t *sensor_data, 
                    void *sensor_arg);

extern int ndp_core2_platform_tiny_sensor_ctl(int sensor_num, int enable);
extern int ndp_core2_platform_tiny_sensor_extract_data(uint8_t *data_buffer, 
        int sensor_num, sensor_data_cb_f sensor_data_cb, void *sensor_arg);
#endif //EXCLUDE_SENSOR_FEATURE

#ifndef EXCLUDE_GET_INFO
extern int ndp_core2_platform_tiny_get_info(int *total_nn, int *total_labels, 
        char *labels_get, int *labels_len);
#endif //EXCLUDE_GET_INFO

#ifndef EXCLUDE_PRINT_DEBUG
extern void ndp_core2_platform_tiny_debug(void);
#endif //EXCLUDE_PRINT_DEBUG

#ifdef __cplusplus
}
#endif

#endif //end of _SYNTIANT_PLATFORM_H_
