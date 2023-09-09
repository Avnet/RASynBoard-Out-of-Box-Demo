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
 	** SDK: v103**
*/

#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syntiant_common.h"
#include "syntiant_platform.h"
#include "syntiant_util.h"
#include "syntiant_driver.h"

#include <syntiant_ilib/syntiant_ndp_ilib_version.h>
#include <syntiant_ilib/syntiant_ndp120_tiny.h>
#ifndef EXCLUDE_TINY_CSPI
#include <syntiant_ilib/syntiant_tiny_cspi.h>
#endif


#if 1
#define SYNTIANT_TRACE(...)      do {printf(__VA_ARGS__);}while(0)
#else
#define SYNTIANT_TRACE(...)
#endif

//#define NDP_BU_TEST    //disable it after bring up done

#ifndef EXCLUDE_GET_INFO
static char *labels[SYNTIANT_NDP120_MAX_CLASSES];
static char *labels_per_network[SYNTIANT_NDP120_MAX_NNETWORKS]
            [SYNTIANT_NDP120_MAX_CLASSES];
static char numlabels_per_network[SYNTIANT_NDP120_MAX_NNETWORKS];

static uint32_t sensor_info_per_sensor[SYNTIANT_NDP120_SENSOR_MAX];
#endif

struct ndp_core2_platform_tiny_s {
    int initialized;
    int loop_sequency;
    int nn_load_index;
    uint8_t clock_option;
    uint8_t use_xtal;
    uint8_t flash_inited;
    uint8_t numlabels_per_network[SYNTIANT_NDP120_MAX_NNETWORKS];
    uint32_t sample_size;
    uint32_t clk_started;
    uint32_t curr_notification;
    struct syntiant_ndp120_tiny_device_s ndp;
    struct ndp120_tiny_handle_t ndp_handle;
};

static struct ndp_core2_platform_tiny_s ndp120_app_state;
static struct ndp_core2_platform_tiny_s *ndp120 = &ndp120_app_state;
static struct syntiant_ndp120_tiny_integration_interfaces_s iif;

/*****************************************************
*
* sub functions
*
*****************************************************/

/** try to do SPI read/write to verify SPI operation */
#define NDP_PRE_TEST
#ifdef NDP_PRE_TEST
static int do_pre_test(void)
{
    int s;
    uint8_t u8_val = 0xAB;

    /*********** SPI MEM ***********/
    s = syntiant_tiny_transfer(&ndp120->ndp_handle, 0, 0x20, &u8_val, NULL, 1);
    if(s) {
        SYNTIANT_TRACE("write spi:0x20 failed\n");
        return s;
    }
	SYNTIANT_TRACE("write spi:0x20: 0x%x\n", u8_val);

    s = syntiant_tiny_transfer(&ndp120->ndp_handle, 0, 0x20, NULL, &u8_val, 1);
    if(s) {
        SYNTIANT_TRACE("read spi:0x20 failed\n");
        return s;
    }
    SYNTIANT_TRACE("read spi:0x20: 0x%x\n", u8_val);
   
    return 0;
}
#endif

/** to do MEM read/write to make sure firmware running */
#ifdef NDP_BU_TEST
static void
print_unexpected(uint8_t *got, uint8_t *expected, int size)
{
    int i;

    SYNTIANT_TRACE("read:    ");
    for (i = 0; i < size; i++) {
        SYNTIANT_TRACE(" 0x%02x", got[i]);
    }
    SYNTIANT_TRACE("\nexpected:");
    for (i = 0; i < size; i++) {
        SYNTIANT_TRACE(" 0x%02x", expected[i]);
    }
    SYNTIANT_TRACE("\n");
}


static int
bu_test_id(struct syntiant_ndp120_tiny_device_s *ndp, int *miscompare,
           uint8_t *id)
{
    int i, s;
    const uint8_t expected_ids[] = {0x30, 0x34, 0x38};

    *miscompare = 0;
    s = syntiant_ndp120_tiny_read_block(ndp, 0, NDP120B0_SPI_ID0, id, 1);
    if (s) {
        return s;
    }

    for (i = 0; i < sizeof(expected_ids); i++) {
        if (*id == expected_ids[i]) {
            return SYNTIANT_NDP120_ERROR_NONE;
        }
    }

    *miscompare = 1;
    return SYNTIANT_NDP120_ERROR_FAIL;
}

static int
bu_test_mem(struct syntiant_ndp120_tiny_device_s *ndp,
            int mcu, uint32_t addr, int read_only,
            uint8_t *expected, uint8_t *buf, int size,
            int *mismatch)
{
    int i, s;

    *mismatch = 0;
    if (!read_only) {
        for (i = 0; i < size; i++) {
            buf[i] = 0;
        }
        s = syntiant_ndp120_tiny_write_block(ndp, mcu, addr, buf, size);
        if (s) {
            return s;
        }

        s = syntiant_ndp120_tiny_write_block(ndp, mcu, addr, expected, size);
        if (s) {
            return s;
        }
    }

    s = syntiant_ndp120_tiny_read_block(ndp, mcu, addr, buf, size);
    if (s) {
        return s;
    }

    for (i = 0; i < size;  i++) {
        if (expected[i] != buf[i]) {
            *mismatch = 1;
            return SYNTIANT_NDP120_ERROR_FAIL;
        }
    }

    return SYNTIANT_NDP120_ERROR_NONE;
}

static int
bu_test_spi(struct syntiant_ndp120_tiny_device_s *ndp,
            uint8_t **expected, uint8_t **read, int *size)
{
    int s, mismatch;
    static uint8_t spi_expected[] = {0x11, 0x22, 0x44, 0x88};
    static uint8_t buf[sizeof(spi_expected)];

    *expected = NULL;
    s = bu_test_mem(ndp, 0, NDP120B0_SPI_MADDR, 0, spi_expected, buf,
                    sizeof(buf), &mismatch);
    if (mismatch) {
        *expected = spi_expected;
        *read = buf;
        *size = sizeof(buf);
    }

    return s;
}

static int
bu_test_rom(struct syntiant_ndp120_tiny_device_s *ndp,
            uint8_t **expected, uint8_t **read, int *size)
{
    int s, mismatch;
#ifndef ENABLE_REG_DUMP
    const uint32_t NDP120_BOOTROM = 0x01000000;
#endif
    static uint8_t rom_expected[] =
        {0x80, 0x6f, 0x00, 0x20, 0x01, 0x01, 0x00, 0x01};
    static uint8_t buf[sizeof(rom_expected)];

    *expected = NULL;
    s = bu_test_mem(ndp, 1, NDP120_BOOTROM, 1, rom_expected, buf,
                    sizeof(buf), &mismatch);
    if (mismatch) {
        *expected = rom_expected;
        *read = buf;
        *size = sizeof(buf);
    }

    return s;
}


static int
bu_test_ram(struct syntiant_ndp120_tiny_device_s *ndp,
            uint8_t **expected, uint8_t **read, int *size)
{
    int s, mismatch;
    const uint32_t NDP120_TESTRAM = 0x20007400;
    static uint8_t ram_expected[] =
        {0x11, 0x22, 0x44, 0x88, 0xa5, 0x5a, 0x6c, 0xc6};
    static uint8_t buf[sizeof(ram_expected)];

    *expected = NULL;
    s = bu_test_mem(ndp, 1, NDP120_TESTRAM, 0, ram_expected, buf,
                    sizeof(buf), &mismatch);
    if (mismatch) {
        *expected = ram_expected;
        *read = buf;
        *size = sizeof(buf);
    }

    return s;
}

static int do_butest(void)
{
    int s, size, mismatch;
    uint8_t id;
    struct syntiant_ndp120_tiny_device_s *ndp = &ndp120->ndp;
    uint8_t *expected;
    uint8_t *read;

    s = bu_test_id(ndp, &mismatch, &id);
    if (mismatch) {
        SYNTIANT_TRACE("unexpected ID read: 0x%02x\n", id);
    }
    if (s) {
        return s;
    }

    s = bu_test_spi(ndp, &expected, &read, &size);
    if (expected) {
        SYNTIANT_TRACE("SPI registers miscompare\n");
        print_unexpected(read, expected, size);
    }
    if (s) {
        return s;
    }

    s = bu_test_rom(ndp, &expected, &read, &size);
    if (expected) {
        SYNTIANT_TRACE("MCU ROM miscompare\n");
        print_unexpected(read, expected, size);
    }
    if (s) {
        return s;
    }

    s = bu_test_ram(ndp, &expected, &read, &size);
    if (expected) {
        SYNTIANT_TRACE("MCU RAM miscompare\n");
        print_unexpected(read, expected, size);
    }

    return s;
}
#endif

/**
 * function to configurate clock
*/
static int do_clock_config(struct syntiant_ndp120_tiny_device_s *ndp, 
        uint8_t clock_option, int use_xtal)
{
    int s = SYNTIANT_NDP_ERROR_NONE;
    int clock_options[2];

    clock_options[0] = clock_option;
    clock_options[1] = use_xtal;
    s = syntiant_ndp120_tiny_clock_cfg(ndp, clock_options);
    if (s) {
        SYNTIANT_TRACE("clock config failed: %d\n", s);
    }

    return s;
}

/**
 * Loading firmware
*/
enum {
    TYPE_MCU_FW_SYNPKG = 0,
    TYPE_DSP_FW_SYNPKG,
    TYPE_NN_SYNPKG,
    TYPE_TOTAL_SYNPKGS
};

#define BL_DOWNLOAD_WIN_SIZE          (SYNTIANT_NDP120_BL_WINDOW_UPPER -\
                                        SYNTIANT_NDP120_BL_WINDOW_LOWER)
#define MCU_DOWNLOAD_WIN_SIZE         (SYNTIANT_NDP120_DL_WINDOW_UPPER -\
                                        SYNTIANT_NDP120_DL_WINDOW_LOWER)


#ifndef EXCLUDE_HOST_LOAD_CODE
/**
 * host load from codes
*/
#include "synpkg_files/mcu_fw_120.h"
#define MCU_FIRMWARE_ARRAY    MCU_FW_120_SYNPKG

/** DSP firmware
*/
#include "synpkg_files/dsp_firmware.h"
#define DSP_FIRMWARE_ARRAY    DSP_FIRMWARE_SYNPKG

/** NN Firmware
*/
#include "synpkg_files/alexa_334_NDP120_B0_v11.h"
#define NN_FIRMWARE_ARRAY   ALEXA_334_NDP120_B0_V11_SYNPKG


static int do_code_load_synpkg(struct syntiant_ndp120_tiny_device_s *ndp, 
        int package_type, const unsigned char *package_data, int package_len) 
{
    int s;
    uint8_t split_data[2048];
    int split_index = 0;
    int split_len;
    int chunk_size = 0;

    SYNTIANT_TRACE("Loading %d bytes of code data\n", package_len);

    if (package_type == TYPE_MCU_FW_SYNPKG) {
        chunk_size = BL_DOWNLOAD_WIN_SIZE;
    } else {
        chunk_size = MCU_DOWNLOAD_WIN_SIZE;
    }

    /* reset parser state */
    s = syntiant_ndp120_tiny_load(ndp, NULL, 0);
    if (s != SYNTIANT_NDP_ERROR_MORE) {
        SYNTIANT_TRACE("Error resetting package load state\n");
        return s;  
    }

    while (split_index < package_len) {
        split_len = (chunk_size<(package_len-split_index))?
                    chunk_size:(package_len-split_index);

        memcpy(split_data, &package_data[split_index], split_len);
        s = syntiant_ndp120_tiny_load(ndp, split_data, split_len);
        if ((s != SYNTIANT_NDP_ERROR_NONE) && (s != SYNTIANT_NDP_ERROR_MORE)) {
            SYNTIANT_TRACE("split_len: %d, split_index: %d\n", split_len, split_index);
            SYNTIANT_TRACE("loading failure: %d\n", s);
            return s;
        }
        split_index += split_len;
    }

    return s;
}
#endif


#ifndef EXCLUDE_HOST_LOAD_FLASH
/**
 * host Load from flash
*/
static int do_flash_load_synpkg(struct syntiant_ndp120_tiny_device_s *ndp, 
        int package_type, uint32_t package_addr, int package_len) 
{
    int s = 0;
    uint8_t split_data[2048];
    int split_index = 0;
    int split_len;
    int read_len;
    int chunk_size = 0;

    SYNTIANT_TRACE("Loading %d bytes of binary data\n", package_len);

    if (package_type == TYPE_MCU_FW_SYNPKG) {
        chunk_size = BL_DOWNLOAD_WIN_SIZE;
    } else {
        chunk_size = MCU_DOWNLOAD_WIN_SIZE;
    }

    /* reset parser state */
    s = syntiant_ndp120_tiny_load(ndp, NULL, 0);
    if (s != SYNTIANT_NDP_ERROR_MORE) {
        SYNTIANT_TRACE("Error resetting package load state\n");
        return s;  
    }

    while (split_index < package_len) {
        split_len = (chunk_size<(package_len-split_index))?
                    chunk_size:(package_len-split_index);
        read_len = syntiant_tiny_flash_read(package_addr+split_index, split_data, split_len);
        if(read_len < split_len) {
            SYNTIANT_TRACE("read less data: [%d->%d]\n", read_len, split_len);
            s = SYNTIANT_NDP_ERROR_FAIL;
            break;
        }
        s = syntiant_ndp120_tiny_load(ndp, split_data, split_len);
        if ((s != SYNTIANT_NDP_ERROR_NONE) && (s != SYNTIANT_NDP_ERROR_MORE)) {
            SYNTIANT_TRACE("split_len: %d, split_index: %d\n", split_len, split_index);
            SYNTIANT_TRACE("loading failure: %d\n", s);
            return s;
        }
        split_index += split_len;
    }

    return s;
}
#endif


#ifndef EXCLUDE_BOOT_FROM_FLASH
/**
 * boot from flash
*/
static int boot_from_flash(struct syntiant_ndp120_tiny_device_s *ndp)
{
    int s = SYNTIANT_NDP120_ERROR_NONE;

    s = syntiant_ndp120_tiny_poll_notification(ndp,
            SYNTIANT_NDP120_NOTIFICATION_BOOTING);
    if (s) {
        SYNTIANT_TRACE("poll_notification failed %d\n", s);
        goto done;
    }

    s = syntiant_ndp120_tiny_boot_from_flash(ndp);
    if (s) {
        SYNTIANT_TRACE("boot from flash error %d\n", s);
        goto done;
    }

    SYNTIANT_TRACE("boot from flash ok\n");

done:
    return s;
}
#endif

#ifndef EXCLUDE_HOST_LOAD_FILE
/**
 * host Load from file
*/
#include "fat_load.h"
/**
 * Load from FATFS on uSD card or MSPI Flash automatically
*/
static int do_file_load_synpkg(struct syntiant_ndp120_tiny_device_s *ndp,
        char * file_name)
{
    int s = 0;
    unsigned char split_data[LOAD_SPLIT_SIZE_FAT];
    int split_index = 0;
    int split_len;
    int package_len;

    SYNTIANT_TRACE("    Loading %s\n", file_name);

    /* reset parser state */
    s = syntiant_ndp120_tiny_load(ndp, NULL, 0);
    if (s != SYNTIANT_NDP_ERROR_MORE) {
        SYNTIANT_TRACE("   Error resetting package load state\n");
        return s;
    }

    package_len = syntiant_tiny_file_len(file_name);
    while (split_index < package_len) {
        split_len = (LOAD_SPLIT_SIZE_FAT<(package_len-split_index))?
                LOAD_SPLIT_SIZE_FAT:(package_len-split_index);
        syntiant_tiny_file_read(file_name, split_index, split_data, split_len);
        s = syntiant_ndp120_tiny_load(ndp, split_data, split_len);
        if ((s != SYNTIANT_NDP_ERROR_NONE) && (s != SYNTIANT_NDP_ERROR_MORE)) {
            SYNTIANT_TRACE("split_len: %d, split_index: %d\n", split_len, split_index);
            SYNTIANT_TRACE("loading failure: %d\n", s);
            return s;
        }
        split_index += split_len;
    }

    return s;
}
#endif


/** *******************************************************
 * **********************************************************/
static int do_binary_loading(struct syntiant_ndp120_tiny_device_s *ndp, 
        int boot_mode)
{
    int s;
    
    /* load synpkg from host code */
    if (boot_mode == NDP_CORE2_BOOT_MODE_HOST_CODE) {
#ifndef EXCLUDE_HOST_LOAD_CODE 
        /* load MCU firmware */
        s = do_code_load_synpkg(ndp, TYPE_MCU_FW_SYNPKG, MCU_FIRMWARE_ARRAY, 
                        sizeof(MCU_FIRMWARE_ARRAY));
        if(s) {
            SYNTIANT_TRACE("load ndp120 mcu fw failed\n");
            goto error;
        }

        /* load DSP firmware */
        if (DSP_FIRMWARE_ARRAY != NULL) {
            s = do_code_load_synpkg(ndp, TYPE_DSP_FW_SYNPKG, DSP_FIRMWARE_ARRAY, 
                            sizeof(DSP_FIRMWARE_ARRAY));
            if(s) {
                SYNTIANT_TRACE("load ndp120 dsp fw failed\n");
                goto error;
            }
        }
        s = syntiant_ndp120_tiny_poll_notification(ndp,
                SYNTIANT_NDP120_NOTIFICATION_DSP_RUNNING);
        if (s) {
            SYNTIANT_TRACE("check dsp running failed\n");
            goto error;
        }

        /* load NN firmware */
        if (NN_FIRMWARE_ARRAY != NULL) {
            s = do_code_load_synpkg(ndp, TYPE_NN_SYNPKG, NN_FIRMWARE_ARRAY, 
                            sizeof(NN_FIRMWARE_ARRAY));
            if(s) {
                SYNTIANT_TRACE("load ndp120 nn fw failed\n");
                goto error;
            }
        }
#else
        SYNTIANT_TRACE("host load code unsupported\n");
#endif
    }
    else if (boot_mode == NDP_CORE2_BOOT_MODE_HOST_FLASH) {
#ifndef EXCLUDE_HOST_LOAD_FLASH
    /**
     * FLASH is attched to Host 
     * please use pad3synpkg.py to pad the flash
     * the flash storage just like :
     *    MCU DATA padding DSP DATA padding FLASH
     * to make sure the data head is at beginning of sector
     */
        /* load mcu file */
        s = do_flash_load_synpkg(ndp, TYPE_MCU_FW_SYNPKG, FLASH_MCU_ADDR, 
                FLASH_MCU_LENGTH);
        if(s) {
            SYNTIANT_TRACE("load ndp120 mcu failed\n");
            goto error;
        }
            
        /* load dsp file */
        s = do_flash_load_synpkg(ndp, TYPE_DSP_FW_SYNPKG, FLASH_DSP_ADDR, 
                FLASH_DSP_LENGTH);
        if(s) {
            SYNTIANT_TRACE("load ndp120 dsp failed\n");
            goto error;
        }

        s = syntiant_ndp120_tiny_poll_notification(ndp,
                SYNTIANT_NDP120_NOTIFICATION_DSP_RUNNING);
        if (s) {
            SYNTIANT_TRACE("check dsp running failed\n");
            goto error;
        }

        /* load nn file */
        s = do_flash_load_synpkg(ndp, TYPE_NN_SYNPKG, FLASH_NN_ADDR, 
                FLASH_NN_LENGTH);
        if(s) {
            SYNTIANT_TRACE("load ndp120 nn failed\n");
            goto error;
        }
#else
        SYNTIANT_TRACE("host load flash unsupported\n");
#endif
    }
    else if (boot_mode == NDP_CORE2_BOOT_MODE_BOOT_FLASH) {
#ifndef EXCLUDE_BOOT_FROM_FLASH
        /**
         * FLASH is attched to NDP 
         */
		SYNTIANT_TRACE("   Loading NDP120 images and Booting From SPI Flash\n");
#if 1
		/* set HOLD pin */
		s = ndp_core2_platform_tiny_gpio_config(7, NDP_CORE2_CONFIG_VALUE_GPIO_DIR_OUT, 1);
		if (s) {
			SYNTIANT_TRACE("      set HOLD pin failed %d\n", s);
			return s;
		}
        /* set MSSB1/GPIO1 pin */
        s = ndp_core2_platform_tiny_gpio_config(1, NDP_CORE2_CONFIG_VALUE_GPIO_DIR_OUT, 1);
        if (s) {
            SYNTIANT_TRACE("      set IMU MSSB1 failed %d\n", s);
            return s;
        }
#endif
	    /* via MB command */
	    SYNTIANT_TRACE("MB BOOT FROM FLASH...\n");
	    s = syntiant_ndp120_tiny_soft_flash_boot(ndp);
	    if (s) {
	        SYNTIANT_TRACE("set soft boot from flash failed %d\n", s);
	        /* fall thru anyway */
	    }
	    s =  boot_from_flash(ndp);
	    if (s) {
	        SYNTIANT_TRACE("Error loading from flash %d\n", s);
	        goto error;
	    }
#else
	        SYNTIANT_TRACE("boot from flash unsupported\n");
#endif
    }
    else if (boot_mode == NDP_CORE2_BOOT_MODE_HOST_FILE) {
#ifndef EXCLUDE_HOST_LOAD_FILE
		SYNTIANT_TRACE("    Loading NDP120 images from SD card . . .\n");

		/* load mcu file */
		s = do_file_load_synpkg(ndp, mcu_file_name);
		if(s) {
			SYNTIANT_TRACE("      Loading the NDP120 MCU image failed\n");
			return s;
		}

		/* load dsp file */
		s = do_file_load_synpkg(ndp, dsp_file_name);
		if(s) {
			SYNTIANT_TRACE("      Loading the NDP120 DSP image failed\n");
			return s;
		}
		s = syntiant_ndp120_tiny_poll_notification(ndp,
				SYNTIANT_NDP120_NOTIFICATION_DSP_RUNNING);
		if (s) {
			SYNTIANT_TRACE("      Check DSP running failed\n");
			return s;
		}

		/* load nn file */
		s = do_file_load_synpkg(ndp, model_file_name);
		if(s) {
			SYNTIANT_TRACE("      Loading NDP120 nn failed\n");
			return s;
		}
#else
        SYNTIANT_TRACE("host load file unsupported\n");
#endif
    }
    SYNTIANT_TRACE("    Finished loading NDP120 binaries\n");

#ifdef NDP_DEBUG_INFO
    ndp120_print_load_info(ndp);
#endif

error:
    return s;
}

/**
 * function call to SPI transfer
*/
int ndp_core2_platform_tiny_transfer(int mcu, uint32_t addr, 
        void *out, void *in, unsigned int count)
{
    return syntiant_tiny_transfer(&ndp120->ndp_handle, mcu, addr, out, in, count);
}

/**
 * function call to configure feature
*/
int ndp_core2_platform_tiny_feature_set(int feature_flag)
{
    int s = SYNTIANT_NDP_ERROR_NONE;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    switch (feature_flag) {
    case NDP_CORE2_FEATURE_PDM:
        if (!ndp120->clk_started) {
            s = syntiant_ndp120_tiny_pdm_clock_exe_mode(ndpp, 
                    SYNTIANT_NDP120_PDM_CLK_START_CLEAN);
            if (!s) ndp120->clk_started ++;
        }
        else {
            s = syntiant_ndp120_tiny_pdm_clock_exe_mode(ndpp, 
                    SYNTIANT_NDP120_PDM_CLK_START_RESUME);
        }
        break;

    case NDP_CORE2_FEATURE_NONE:
        s = syntiant_ndp120_tiny_pdm_clock_exe_mode(ndpp, 
                SYNTIANT_NDP120_PDM_CLK_START_PAUSE);
        break;

    default:
        break;
    }
    
    return s;
}


/**
 * function call to poll ndp
*/
int ndp_core2_platform_tiny_poll(uint32_t *notifications, int clear)
{
    int s;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;
	
    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    s =  syntiant_ndp120_tiny_poll(ndpp, notifications, clear);
    ndp120->curr_notification = *notifications;

    return s;
}

#ifdef ENABLE_REG_DUMP
static void dump_raw_strengths(struct syntiant_ndp120_tiny_match_data *match) {
    uint32_t idx;

    SYNTIANT_TRACE("raw_strengths:");
    switch (match->activation_type) {
        case NDP120_DNN_ISA_COMP0_ACTIVATION_RELU:
        case NDP120_DNN_ISA_COMP0_ACTIVATION_SIGMOID0:
        case NDP120_DNN_ISA_COMP0_ACTIVATION_SIGMOID1:
            for (idx = 0; idx < match->num_classes; idx++) {
                SYNTIANT_TRACE("%4x", match->raw_strengths.u8[idx]);
            }
            break;
        case NDP120_DNN_ISA_COMP0_ACTIVATION_LINEAR:
        case NDP120_DNN_ISA_COMP0_ACTIVATION_TANH0:
        case NDP120_DNN_ISA_COMP0_ACTIVATION_TANH1:
            for (idx = 0; idx < match->num_classes; idx++) {
                SYNTIANT_TRACE("%4x", match->raw_strengths.s8[idx]);
            }
            break;
        case NDP120_DNN_ISA_COMP0_ACTIVATION_LINEAR_16:
            for (idx = 0; idx < match->num_classes; idx++) {
                SYNTIANT_TRACE("%x", match->raw_strengths.s16[idx]);
            }
            break;
        default:
            SYNTIANT_TRACE("\nInvalid activation type\n");
            break;
    }
    SYNTIANT_TRACE("\n");
}
#endif

/**
 * function call to ndp match
 * to get class index if with multi-words
*/
int ndp_core2_platform_tiny_match_process(uint8_t *nn_id, uint8_t *match_id,
        uint8_t *sec_val, char *label_string)
{
    int s;
    uint8_t match_found = 0;
    uint32_t summary;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;
    char *label_match = NULL;
    struct syntiant_ndp120_tiny_match_data match;

    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    if (!(ndp120->curr_notification & SYNTIANT_NDP120_NOTIFICATION_MATCH)) {
        return SYNTIANT_NDP_ERROR_FAIL;
    }

    ndp120->curr_notification = 0x0;

    s = syntiant_ndp120_tiny_get_match_result(ndpp, &match);
    if(s) return s;

    summary = match.summary;

    if (summary & NDP120_SPI_MATCH_MATCH_MASK) {
        match_found = 1;
        *nn_id = ndpp->last_network_id;
        *match_id = summary & NDP120_SPI_MATCH_WINNER_MASK;
        *sec_val = match.sec_val_status;
#ifndef EXCLUDE_GET_INFO
        if (label_string) {
            label_match = labels_per_network[ndpp->last_network_id]
                       [summary & NDP120_SPI_MATCH_WINNER_MASK];
            if (label_match) strcpy(label_string, label_match);
        }
#endif

#ifdef ENABLE_REG_DUMP
    dump_raw_strengths(&match);
#endif
    }

    return (match_found==1)?SYNTIANT_NDP_ERROR_NONE:SYNTIANT_NDP_ERROR_FAIL;
}

int ndp_core2_platform_tiny_interrupts(int *cause)
{
    int s;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    s = syntiant_ndp120_tiny_interrupts(ndpp, cause);
    return s;
}

int ndp_core2_platform_tiny_config_interrupts(uint32_t interrupt, int enable)
{
    int s;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    s = syntiant_ndp120_tiny_config_interrupts(ndpp, interrupt, enable);
    return s;
}

int ndp_core2_platform_tiny_vadmic_ctl(int mode)
{
    int s = SYNTIANT_NDP_ERROR_NONE;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }
    s = syntiant_ndp120_tiny_vad_mic_control(ndpp,
            mode ? SYNTIANT_NDP120_TINY_VAD_MIC_MODE_VAD :
                SYNTIANT_NDP120_TINY_VAD_MIC_MODE_AON);
    if (s == SYNTIANT_NDP120_ERROR_DSP_NO_VAD_MIC) {
        SYNTIANT_TRACE("Error setting vadctrl: no vad mic\n");
    } 

    return s;
}

int ndp_core2_platform_tiny_halt_mcu(void)
{
    int s;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    s = syntiant_ndp120_tiny_halt_mcu(ndpp);
    return s;
}

#ifndef EXCLUDE_TINY_CSPI
int ndp_core2_platform_tiny_mspi_config(void)
{
    int s;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    s = syntiant_cspi_init(ndpp);
    return s;
}

int ndp_core2_platform_tiny_mspi_read(int ssb, int num_bytes, 
        uint8_t* data, int end_packet)
{
    int s;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    s = syntiant_cspi_read(ndpp, ssb, num_bytes, data, end_packet);
    return s;
}

int ndp_core2_platform_tiny_mspi_write(int ssb, int num_bytes, 
        uint8_t* data, int end_packet)
{
    int s;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    s = syntiant_cspi_write(ndpp, ssb, num_bytes, data, end_packet);
    return s;
}

/****
 *  FLASH operation 
 * */

static int flash_init(struct syntiant_ndp120_tiny_device_s *ndpp)
{
    int s;

    s = syntiant_cspi_init(ndpp);
    if (!s) {
        ndp120->flash_inited = 1;
    }

    return s;
}

static int flash_get_sr1(struct syntiant_ndp120_tiny_device_s *ndpp, 
        uint8_t *data)
{
    int s = SYNTIANT_NDP_ERROR_NONE;
    uint8_t cmd[] = { 0x05 };

    *data = 0;

    s = syntiant_cspi_write(ndpp, 0, sizeof(cmd), cmd, 0);
    if (s) return s;

    s = syntiant_cspi_read(ndpp, 0, 1, data, 1);

    return s;
}

static int flash_poll_busy(struct syntiant_ndp120_tiny_device_s *ndpp)
{
    int s = SYNTIANT_NDP_ERROR_NONE;
    uint8_t data[1];
    int count = 1000;

    while (count--) {
        s = flash_get_sr1(ndpp, data);
        if (s) {
            SYNTIANT_TRACE("%s: flash_get_sr1 failed %d\n", __func__, s);
            goto done;
        }
        if ((*data & 1) == 0) {
            goto done;
        }
    }
    SYNTIANT_TRACE("flash poll busy timeout\n");
    s = SYNTIANT_NDP_ERROR_TIMEOUT;

done:
    return s;
}

static int flash_write_enable(struct syntiant_ndp120_tiny_device_s *ndpp)
{
    int s = SYNTIANT_NDP_ERROR_NONE;
    uint8_t cmd[] = { 0x06 };

    s = syntiant_cspi_write(ndpp, 0, sizeof(cmd), cmd, 1);

    return s;
}

int ndp_core2_platform_tiny_flash_get_id(uint8_t *data)
{
    int s = SYNTIANT_NDP_ERROR_NONE;
    uint8_t cmd[] = { 0x90, 0x00, 0x00, 0x00 };
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    if (!ndp120->flash_inited) {
        /* initialization flash interface */
        s = flash_init(ndpp);
        if (s) {
            SYNTIANT_TRACE("flash init failed: %d\n", s);
            return s;
        }
    }

    memset(data, 0, 2);

    s = syntiant_cspi_write(ndpp, 0, sizeof(cmd), cmd, 0);
    if (s) return s;

    s = syntiant_cspi_read(ndpp, 0, 2, data, 1);

    return s;
}

int ndp_core2_platform_tiny_flash_read_data(uint32_t addr, 
        uint8_t *data, unsigned int len)
{
    int s = SYNTIANT_NDP_ERROR_NONE;
    uint8_t cmd[] = { 0x03, addr >> 16 & 0xFF, addr >> 8 & 0xFF, addr & 0xFF };
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    if (!ndp120->flash_inited) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    memset(data, 0, len);

    s = syntiant_cspi_write(ndpp, 0, sizeof(cmd), cmd, 0);
    if (s) {
        SYNTIANT_TRACE("%s: cspi write failed %d\n", __func__, s);
        goto done;
    }

    s = syntiant_cspi_read(ndpp, 0, len, data, 1);
    if (s) {
        SYNTIANT_TRACE("%s: cspi read failed %d\n", __func__, s);
    }

done:
    return s;
}

int ndp_core2_platform_tiny_flash_sector_erase(uint32_t addr)
{
    int s = SYNTIANT_NDP_ERROR_NONE;
    uint8_t cmd[] = { 0x20, addr >> 16 & 0xFF, addr >> 8 & 0xFF, 0x00 };
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    if (!ndp120->flash_inited) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    s = flash_write_enable(ndpp);
    if (s) {
        SYNTIANT_TRACE("%s: flash_write_enable failed %d\n", __func__, s);
        goto done;
    }
    s = syntiant_cspi_write(ndpp, 0, sizeof(cmd), cmd, 1);
    if (s) {
        SYNTIANT_TRACE("%s: cspi write failed %d\n", __func__, s);
        goto done;
    }
    s = flash_poll_busy(ndpp);
    if (s) {
        SYNTIANT_TRACE("%s: flash_poll_busy failed %d\n", __func__, s);
    }

done:
    return s;

}

int ndp_core2_platform_tiny_flash_page_program(uint32_t addr, 
        uint8_t *data, unsigned int len)
{

    int s = SYNTIANT_NDP_ERROR_NONE;
    uint8_t cmd[] = { 0x02, addr >> 16 & 0xFF, addr >> 8 & 0xFF, 0x00 };
    unsigned int page = 256;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    if (!ndp120->flash_inited) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    if (len > page) {
        SYNTIANT_TRACE("%s: length %u exceeds page length\n", __func__, len);
        s = SYNTIANT_NDP_ERROR_CONFIG;
        goto done;
    }

    if (addr % page) {
        SYNTIANT_TRACE("%s: address %#x not page boundary\n", __func__, addr);
        s = SYNTIANT_NDP_ERROR_CONFIG;
        goto done;
    }

    s = flash_write_enable(ndpp);
    if (s) {
        SYNTIANT_TRACE("%s: flash_write_enable failed %d\n", __func__, s);
        goto done;
    }

    s = syntiant_cspi_write(ndpp, 0, 4, cmd, 0);
    if (s) {
        SYNTIANT_TRACE("%s: cspi write failed %d\n", __func__, s);
        goto done;
    }

    s = syntiant_cspi_write(ndpp, 0, len, data, 1);
    if (s) {
        SYNTIANT_TRACE("%s: cspi write failed %d\n", __func__, s);
        goto done;
    }

    s = flash_poll_busy(ndpp);
    if (s) {
        SYNTIANT_TRACE("%s: flash_poll_busy failed %d\n", __func__, s);
    }
done:
    return s;
}

#endif //EXCLUDE_TINY_CSPI

#ifndef EXCLUDE_TINY_EXTRACTION

uint32_t ndp_core2_platform_tiny_get_samplebytes(void)
{
    return SYNTIANT_NDP120_TINY_AUDIO_SAMPLES_PER_WORD;
}

uint32_t ndp_core2_platform_tiny_get_samplerate(void)
{
    return SYNTIANT_NDP120_TINY_AUDIO_SAMPLE_RATE;
}

int ndp_core2_platform_tiny_get_recording_metadata(uint32_t *sample_size, 
        int get_from)
{
    int s;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    s = syntiant_ndp120_tiny_get_recording_metadata(ndpp,
        sample_size, get_from);
    if (!s) {
        ndp120->sample_size = *sample_size;
    }

    return s;
}

int ndp_core2_platform_tiny_notify_extract_data(uint8_t *data_buffer, 
        uint32_t sample_size, audio_data_cb_f audio_data_cb, void *audio_arg)
{

    int s;
    uint32_t notifications, extract_size;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    if (!data_buffer) {
        return SYNTIANT_NDP_ERROR_ARG;
    }

    s =  syntiant_ndp120_tiny_poll(ndpp, &notifications, 1);
    if (s) return SYNTIANT_NDP_ERROR_FAIL;

    if (!(notifications & SYNTIANT_NDP120_NOTIFICATION_EXTRACT_READY)) {
        return NDP_CORE2_ERROR_DATA_REREAD;
    }

    ndp120->curr_notification = 0x0;

    while (1) {
        extract_size = sample_size;
        s = syntiant_ndp120_tiny_extract_data(ndpp, data_buffer, &extract_size, 1);
        if (s == SYNTIANT_NDP_ERROR_DATA_REREAD) {
            break;
        } 
        else if (!s) {
            if (extract_size) {
                audio_data_cb(extract_size, data_buffer, audio_arg);
            }
        }
        else {
            SYNTIANT_TRACE("extract data failed: %d\n", s);
            return s;
        }
    }

    return s;
}

int ndp_core2_platform_tiny_extract_start(void)
{
    int s;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    s = syntiant_ndp120_tiny_audio_extract_start(ndpp, 
            SYNTIANT_NDP120_EXTRACT_FROM_UNREAD);

    return s;
}

int ndp_core2_platform_tiny_extract_stop(void)
{
    int s;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    s = syntiant_ndp120_tiny_audio_extract_stop(ndpp);

    return s;
}

int ndp_core2_platform_tiny_src_type(uint8_t *data, uint32_t *data_size)
{
    struct syntiant_ndp120_tiny_dsp_audio_sample_annotation_t *annotation;

    if (!ndp120->sample_size) {
        return NDP_CORE2_FLOW_SRC_TYPE_NONE;
    }

    *data_size = ndp120->sample_size;

    annotation = (struct syntiant_ndp120_tiny_dsp_audio_sample_annotation_t *)
            (data + ndp120->sample_size);
    
    /* read annotation to determine which file to write to */
    if (annotation->src_type == SYNTIANT_NDP120_DSP_DATA_FLOW_SRC_TYPE_PCM_AUDIO) {
        if (annotation->src_param == SYNTIANT_NDP120_DSP_DATA_FLOW_SRC_PARAM0 ||
                annotation->src_param == SYNTIANT_NDP120_DSP_DATA_FLOW_SRC_PARAM2) {
            return NDP_CORE2_FLOW_SRC_TYPE_PCM0;
        } else if (annotation->src_param == SYNTIANT_NDP120_DSP_DATA_FLOW_SRC_PARAM1 ||
                annotation->src_param == SYNTIANT_NDP120_DSP_DATA_FLOW_SRC_PARAM3) {
            return NDP_CORE2_FLOW_SRC_TYPE_PCM1;
        }
    }
    else if (annotation->src_type == SYNTIANT_NDP120_DSP_DATA_FLOW_SRC_TYPE_FUNCTION) {
        return NDP_CORE2_FLOW_SRC_TYPE_FUNC;
    }

    return NDP_CORE2_FLOW_SRC_TYPE_NONE;
}
#endif

int ndp_core2_platform_tiny_gpio_config(int gpio_num, uint32_t dir, uint32_t value)
{
    int s;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;
    struct syntiant_ndp120_tiny_config_gpio_s config;


    memset(&config, 0, sizeof(struct syntiant_ndp120_tiny_config_gpio_s));
    config.gpio_num = gpio_num;
    config.dir = dir;
    config.value = value;
    s = syntiant_ndp120_tiny_config_gpio(ndpp, &config);
    return s;
}

#ifndef EXCLUDE_SENSOR_FEATURE
int ndp_core2_platform_tiny_sensor_ctl(int sensor_num, int enable)
{
    int s;
    uint8_t sensor_id;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;
    sensor_id = sensor_info_per_sensor[sensor_num];

    /* enabling the sensor */
    s = syntiant_ndp120_tiny_enable_disable_sensor(ndpp, sensor_id, enable);
    return s;
}

int ndp_core2_platform_tiny_sensor_extract_data(uint8_t *data_buffer, 
        int sensor_num, 
        sensor_data_cb_f sensor_data_cb, void *sensor_arg)
{
    int s;
    uint32_t notifications, saved_size, sample_size;
    uint8_t event_type;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;
    struct syntiant_ndp120_tiny_match_data match = {0};

    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    if (!data_buffer) {
        return SYNTIANT_NDP_ERROR_ARG;
    }

    s =  syntiant_ndp120_tiny_poll(ndpp, &notifications, 1);
    if (s) return SYNTIANT_NDP_ERROR_FAIL;

    if (!(notifications & SYNTIANT_NDP120_NOTIFICATION_MATCH)) {
        return SYNTIANT_NDP_ERROR_DATA_REREAD;
    }

    ndp120->curr_notification = 0x0;

    s = syntiant_ndp120_tiny_get_match_result(ndpp, &match);
    if (s) {
        SYNTIANT_TRACE("syntiant_ndp120_tiny_get_match_result fail: %d\n", s);
        return s;
    }

    s = syntiant_ndp120_tiny_get_recording_metadata(ndpp, &sample_size, 
            SYNTIANT_NDP120_GET_FROM_ILIB);
    if (s) {
        SYNTIANT_TRACE("audio record get metadata from ilib with notify failed: %d\n", s);
        return s;
    }

    event_type = match.summary >> NDP120_MATCH_MISC_EVENT_SHIFT;
    if (event_type == NDP120_SENSOR_DATA_READY_EVENT) {
        while (1) {
            /* reinitialize sampe_size before every call to
                * syntiant_ndp_extract_data */
            saved_size = sample_size;
            s = syntiant_ndp120_tiny_extract_data(ndpp, data_buffer, &saved_size, 1);
            if (s == SYNTIANT_NDP_ERROR_DATA_REREAD) {
                s = SYNTIANT_NDP_ERROR_NONE;
                break;
            } else if (s) {
                SYNTIANT_TRACE("ndp120_tiny_extract_data fail: %d\n", s);
                return s;
            }
            
            sensor_data_cb(saved_size, data_buffer, sensor_arg);
        }
    } else {
        //SYNTIANT_TRACE("Not a sensor event %d\n", event_type);
    }

    return s;
}
#endif


/**
 * function call to fully initialize ndp120
*/
int ndp_core2_platform_tiny_start(uint8_t clock_option, int use_xtal, 
        int boot_mode)
{
    int s = 0;
    struct syntiant_ndp120_tiny_device_s *ndpp = NULL;
    int on = SYNTIANT_NDP120_INTERRUPT_DEFAULT;

    SYNTIANT_TRACE("ndp120 init start...\n");
    memset(ndp120, 0, sizeof(struct ndp_core2_platform_tiny_s));

    s = syntiant_tiny_io_init(&ndp120->ndp_handle, DEFAULT_SPI_RATE);
    if(s) {
        SYNTIANT_TRACE("io_init failed\n");
        return s;
    }

    /* initialize the ndp based on the interface functions */
    ndpp = &ndp120->ndp;
    ndp120->ndp_handle.ndp = &ndp120->ndp;

#ifdef NDP_PRE_TEST
    s = do_pre_test();
    if(s) {
        SYNTIANT_TRACE("pre test failed s: %d\n", s);
        return s;
    }
#endif

    /* setup the integration interface functions */
    memset(&iif, 0, sizeof(iif));
    iif.d = &ndp120->ndp_handle;
    iif.transfer = syntiant_tiny_transfer;
    iif.mbwait = syntiant_tiny_mbwait;
    iif.udelay = syntiant_tiny_delayus;
		
    s = syntiant_ndp120_tiny_init(ndpp, &iif, SYNTIANT_NDP_INIT_MODE_RESET);
    if(s) {
        SYNTIANT_TRACE("ndp_init failed s: %d\n", s);
        return s;
    }

#ifdef NDP_BU_TEST
    s = do_butest();
    if(s) {
        SYNTIANT_TRACE("do butest failed s: %d\n", s);
        return s;
    }
    SYNTIANT_TRACE("bu test passed!\n");
#endif

    if (boot_mode == NDP_CORE2_BOOT_MODE_NONE) {
        /* doesn't load firmware/model */
        return s;
    }

    /* load mcu/dsp/nn binary */
    s = do_binary_loading(ndpp, boot_mode);
    if(s) {
        SYNTIANT_TRACE("binary loading failed s: %d\n", s);
        return s;
    }

    /* enable interrupts */
    s = syntiant_ndp120_tiny_interrupts(ndpp, &on);
    if(s) {
        SYNTIANT_TRACE("interrupts failed s: %d\n", s);
        return s;
    }

    /* configure input clock */
    s = do_clock_config(ndpp, clock_option, use_xtal);
    if(s) {
        SYNTIANT_TRACE("configure clock failed s: %d\n", s);
        return s;
    }

    /* ndp120 initialization done */
    ndp120->initialized = 1;

    /* ndp120 initialization done */
    SYNTIANT_TRACE("ndp120 init done...\n");
    return s;
}

/**
 * when system wakeup from flash, call this to recover ndp120
 * ndp120 doesn't not need initialization again
*/
int ndp_core2_platform_tiny_recover(void)
{
    int s = 0;
    struct syntiant_ndp120_tiny_device_s *ndpp = NULL;

    SYNTIANT_TRACE("ndp120 recover start...\n");
    memset(ndp120, 0, sizeof(struct ndp_core2_platform_tiny_s));

    s = syntiant_tiny_io_init(&ndp120->ndp_handle, DEFAULT_SPI_RATE);
    if(s) {
        SYNTIANT_TRACE("io_init failed\n");
        return s;
    }

    /* initialize the ndp based on the interface functions */
    ndpp = &ndp120->ndp;
    ndp120->ndp_handle.ndp = &ndp120->ndp;

    /* setup the integration interface functions */
    memset(&iif, 0, sizeof(iif));
    iif.d = &ndp120->ndp_handle;
    iif.transfer = syntiant_tiny_transfer;
    iif.mbwait = syntiant_tiny_mbwait;
    iif.udelay = syntiant_tiny_delayus;
		
    s = syntiant_ndp120_tiny_init(ndpp, &iif, 1);
    if(s) {
        SYNTIANT_TRACE("ndp120 recover init failed s: %d\n", s);
        return s;
    }

    /* ndp120 initialization done */
    ndp120->initialized = 1;

    /* ndp120 initialization done */
    SYNTIANT_TRACE("ndp120 recover done...\n");
    return s;
}

/**********************************************************************
 * 
**********************************************************************/
#ifndef EXCLUDE_GET_INFO
/**
 * function call to get info
*/
#define PLATFORM_INFO_PRINT

static char *syntiant_ndp_sensor_id_names[] = SYNTIANT_NDP_SENSOR_ID_NAMES;
static char *syntiant_ndp_sensor_id_name(int id)
{
    return SYNTIANT_NDP_SENSOR_ID_NAME(id);
}

int ndp_core2_platform_tiny_get_info(int *total_nn, int *total_labels, 
        char *labels_get, int *labels_len)
{
    int s = SYNTIANT_NDP_ERROR_NONE;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;
    struct syntiant_ndp120_tiny_info info;
    int j, class_num, nn_num, prev_nn_num, num_labels;
    char fwver[NDP120_MCU_FW_VER_MAX_LEN] = "";
    char dspfwver[NDP120_MCU_DSP_FW_VER_MAX_LEN] = "";
    char pkgver[NDP120_MCU_PKG_VER_MAX_LEN] = "";
    uint8_t pbiver[NDP120_MCU_PBI_VER_MAX_LEN] = "";
    char label_data[NDP120_MCU_LABELS_MAX_LEN] = "";
    uint32_t scale_factor_per_nn[SYNTIANT_NDP120_MAX_CLASSES];
    char *label_string;
    
    SYNTIANT_TRACE("ilib version : %s\n", SYNTIANT_NDP_ILIB_VERSION);
    SYNTIANT_TRACE("SDK version : %s\n", SYNTIANT_NDP_SDK_VERSION);

    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    info.fw_version = fwver;
    info.dsp_fw_version = dspfwver;
    info.pkg_version = pkgver;
    info.pbi = pbiver;
    info.labels = label_data;
    info.scale_factor = scale_factor_per_nn;
    info.sensor_info = sensor_info_per_sensor;
    s = syntiant_ndp120_tiny_get_info(ndpp, &info);
    if (s) return s;

    *total_nn = info.total_nn;
    
    /* get pointers to the labels */
    num_labels = 0;
    j = 0;

    /* labels_len is 4 byte aligned. We continue processing
       labels until the running sum of label characters
       processed is within 3 bytes of labels_len */
    while ((info.labels_len - j > 3) &&
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
            return -1;
        }
        if (nn_num != prev_nn_num) {
            class_num = 0;
        }
        labels_per_network[nn_num][class_num++] = label_string;
        numlabels_per_network[nn_num] = class_num;
        prev_nn_num = nn_num;
    }

    *total_labels = num_labels;
    if (labels_get) {
        memcpy(labels_get, label_data, info.labels_len);
        *labels_len = info.labels_len;
    }

#ifdef PLATFORM_INFO_PRINT
    int i;
    uint32_t *pbi_version;

    pbi_version = (uint32_t *)&pbiver[0];
    SYNTIANT_TRACE("dsp firmware version: %s\n", info.dsp_fw_version);
    SYNTIANT_TRACE("package version: %s\n", info.pkg_version);
    SYNTIANT_TRACE("pbi version: ");
    SYNTIANT_TRACE("%d.",*pbi_version++);
    SYNTIANT_TRACE("%d.",*pbi_version++);
    SYNTIANT_TRACE("%d-",*pbi_version++);
    SYNTIANT_TRACE("%d\n",*pbi_version);
    SYNTIANT_TRACE("num of labels: %d\n", num_labels);
    SYNTIANT_TRACE("labels: ");
    for (i = 0; i < num_labels; i++) {
        SYNTIANT_TRACE("%s", labels[i]);
        if (i < num_labels - 1) {
            SYNTIANT_TRACE(", ");
        }
    }
    SYNTIANT_TRACE("\ntotal deployed neural networks: %d\n", info.total_nn);
    for (i = 0; i < info.total_nn; i++) {
        SYNTIANT_TRACE("scale factor of NN%d: %d\n", i, info.scale_factor[i]);
    }
    for (i = 0; i < SYNTIANT_NDP120_SENSOR_MAX; i++) {
        SYNTIANT_TRACE("sensor id of sensor num%d: [%u] %s\n", i,
               sensor_info_per_sensor[i], 
               syntiant_ndp_sensor_id_name(sensor_info_per_sensor[i]));
    }
#endif

    return s;
}
#endif


#ifndef EXCLUDE_PRINT_DEBUG
static void print_debug_counters(struct syntiant_ndp120_tiny_debug *cnt)
{
    int i, j;
    struct syntiant_ndp120_flow_rule *flow;
    uint8_t sensor_id, sensor_adr, gpio_int, gpio1_int, axes, parameter;
    struct syntiant_ndp120_sensor_config *sensor_config;
    struct syntiant_ndp120_sensor_state *sensor_state;
    struct syntiant_ndp120_tiny_t5838_status_s *sensor_status;

    if (cnt) {
        SYNTIANT_TRACE("DSP counters:\n");
        SYNTIANT_TRACE("frame_cnt: 0x%x\n", cnt->dsp_dbg_cnt.frame_cnt);
        SYNTIANT_TRACE("dnn_int_cnt: 0x%x\n", cnt->dsp_dbg_cnt.dnn_int_cnt);
        SYNTIANT_TRACE("dnn_err_cnt: 0x%x\n", cnt->dsp_dbg_cnt.dnn_err_cnt);
        SYNTIANT_TRACE("h2d_mb_cnt: 0x%x\n", cnt->dsp_dbg_cnt.h2d_mb_cnt);
        SYNTIANT_TRACE("d2m_mb_cnt: 0x%x\n", cnt->dsp_dbg_cnt.d2m_mb_cnt);
        SYNTIANT_TRACE("m2d_mb_cnt: 0x%x\n", cnt->dsp_dbg_cnt.m2d_mb_cnt);
        SYNTIANT_TRACE("watermark_cnt: 0x%x\n", cnt->dsp_dbg_cnt.watermark_cnt);
        SYNTIANT_TRACE("fifo_overflow_cnt: 0x%x\n", cnt->dsp_dbg_cnt.fifo_overflow_cnt);
        SYNTIANT_TRACE("MCU counters:\n");
        SYNTIANT_TRACE("signature: 0x%x\n", cnt->mcu_dbg_cnt.signature);
        SYNTIANT_TRACE("frame_cnt: 0x%x\n", cnt->mcu_dbg_cnt.frame_cnt);
        SYNTIANT_TRACE("dsp2mcu_intr_cnt: 0x%x\n", cnt->mcu_dbg_cnt.dsp2mcu_intr_cnt);
        SYNTIANT_TRACE("dsp2mcu_nn_done_cnt: 0x%x\n", cnt->mcu_dbg_cnt.dsp2mcu_nn_done_cnt);
        SYNTIANT_TRACE("mcu2host_match_cnt: 0x%x\n", cnt->mcu_dbg_cnt.mcu2host_match_cnt);
        SYNTIANT_TRACE("mcu2host_mpf_cnt: 0x%x\n", cnt->mcu_dbg_cnt.mcu2host_mpf_cnt);
        SYNTIANT_TRACE("matches: 0x%x\n", cnt->mcu_dbg_cnt.matches);
        SYNTIANT_TRACE("dsp2mcu_queue_cnt: 0x%x\n", cnt->mcu_dbg_cnt.dsp2mcu_queue_cnt);
        SYNTIANT_TRACE("mbin_int_cnt: 0x%x\n", cnt->mcu_dbg_cnt.mbin_int_cnt);
        SYNTIANT_TRACE("mbout_int_cnt: 0x%x\n", cnt->mcu_dbg_cnt.mbout_int_cnt);
        SYNTIANT_TRACE("nn_orch_flwchg_cnt: 0x%x\n",
                cnt->mcu_dbg_cnt.nn_orch_flwchg_cnt);
        SYNTIANT_TRACE("unknown_activation_cnt: 0x%x\n",
                cnt->mcu_dbg_cnt.unknown_activation_cnt);
        SYNTIANT_TRACE("unknown_int_count: 0x%x\n", cnt->mcu_dbg_cnt.unknown_int_count);
        SYNTIANT_TRACE("dbg1: 0x%x\n", cnt->mcu_dbg_cnt.dbg1);
        SYNTIANT_TRACE("dbg2: 0x%x\n", cnt->mcu_dbg_cnt.dbg2);
        SYNTIANT_TRACE("accumulator_error: %d\n", cnt->mcu_dbg_cnt.accumulator_error);
        for (i = 0; i < NDP120_PCM_DATA_FLOW_RULE_MAX; i++) {
            flow = &cnt->flow_rules.src_pcm_audio[i];
            if (flow->dst_type != NDP120_DSP_DATA_FLOW_DST_TYPE_NONE) {
                SYNTIANT_TRACE("pcm%d: %s%d {%d}\n", flow->src_param,
                    NDP120_DSP_DATA_FLOW_RULE_DST_STR(*flow),
                    flow->dst_param, flow->algo_config_index);
            }
        }
        for (i = 0; i < NDP120_FUNC_DATA_FLOW_RULE_MAX; i++) {
            flow = &cnt->flow_rules.src_function[i];
            if (flow->dst_type != NDP120_DSP_DATA_FLOW_DST_TYPE_NONE) {
                SYNTIANT_TRACE("func%d: %s%d {%d}\n", flow->src_param,
                    NDP120_DSP_DATA_FLOW_RULE_DST_STR(*flow),
                    flow->dst_param, flow->algo_config_index);
            }
        }
        for (i = 0; i < NDP120_NN_DATA_FLOW_RULE_MAX; i++) {
            flow = &cnt->flow_rules.src_nn[i];
            if (flow->dst_type != NDP120_DSP_DATA_FLOW_DST_TYPE_NONE) {
                SYNTIANT_TRACE("nn%d: %s%d\n", flow->src_param,
                    NDP120_DSP_DATA_FLOW_RULE_DST_STR(*flow),
                    flow->dst_param);
            }
        }
        for (i = 0; i < NDP120_SENSOR_DATA_FLOW_RULE_MAX; i++) {
            flow = &cnt->flow_rules.src_sensor[i];
            if (flow->dst_type != NDP120_DSP_DATA_FLOW_DST_TYPE_NONE) {
                SYNTIANT_TRACE("sensor%d: %s%d\n", flow->src_param,
                    NDP120_DSP_DATA_FLOW_RULE_DST_STR(*flow),
                    flow->dst_param);
            }
        }
        SYNTIANT_TRACE("last_network_id: %d\n", cnt->dsp_dev.last_network);
        SYNTIANT_TRACE("decibel: %d\n", cnt->dsp_dev.db_measured);
        SYNTIANT_TRACE("sample tank memory type: %s\n",
            cnt->dsp_dev.sampletank_mem_type ==
                SYNTIANT_NDP120_DSP_MEM_TYPE_HEAP ? "DSP" :
            cnt->dsp_dev.sampletank_mem_type ==
                SYNTIANT_NDP120_DSP_MEM_TYPE_DNN_DATA ? "DNN" : "None");
        SYNTIANT_TRACE("device_state: 0x%x\n", cnt->dsp_dev.device_state);
        SYNTIANT_TRACE("sensor config:\n");
        for (i = 0; i < SYNTIANT_NDP120_SENSOR_MAX; i++) {
            sensor_config = &cnt->sensor_config[i];
            sensor_id = (sensor_config->control &
                         SYNTIANT_NDP120_SENSOR_CONTROL_ID_MASK) >>
                         SYNTIANT_NDP120_SENSOR_CONTROL_ID_SHIFT;
            if (!sensor_id) continue;

            sensor_adr = (sensor_config->control &
                          SYNTIANT_NDP120_SENSOR_CONTROL_ADDRESS_MASK) >>
                          SYNTIANT_NDP120_SENSOR_CONTROL_ADDRESS_SHIFT;
            gpio_int = (sensor_config->control &
                        SYNTIANT_NDP120_SENSOR_CONTROL_INT_GPIO_MASK) >>
                        SYNTIANT_NDP120_SENSOR_CONTROL_INT_GPIO_SHIFT;
            gpio1_int = (sensor_config->control &
                         SYNTIANT_NDP120_SENSOR_CONTROL_INT_GPIO1_MASK) >>
                         SYNTIANT_NDP120_SENSOR_CONTROL_INT_GPIO1_SHIFT;
            axes = (sensor_config->control &
                    SYNTIANT_NDP120_SENSOR_CONTROL_AXES_MASK) >>
                    SYNTIANT_NDP120_SENSOR_CONTROL_AXES_SHIFT;
            SYNTIANT_TRACE("sensor num %d: ", i);
            SYNTIANT_TRACE("type=%d,", sensor_id);
            SYNTIANT_TRACE("addr=0x%x,", sensor_adr & 0x7f);
            SYNTIANT_TRACE("gpio int=%d,", gpio_int -1);
            if (gpio1_int) {
                SYNTIANT_TRACE("gpio1 int=%d,", gpio1_int -1);
            }
            SYNTIANT_TRACE("axes=%d,", axes);
            SYNTIANT_TRACE("enable=0x%x,", sensor_config->enable);
            for (j = 0; j < SYNTIANT_NDP120_SENSOR_PARAM_MAX; j++) {
                parameter = sensor_config->parameter[j];
                SYNTIANT_TRACE("param[%d]=0x%x,", j, parameter);
            }
            SYNTIANT_TRACE("\n");
            SYNTIANT_TRACE("interrupt count=%d\n", cnt->sensor_state[i].int_count);
        }
        for (i = 0; i < SYNTIANT_NDP120_SENSOR_MAX; i++) {
            sensor_state = &cnt->sensor_state[i];
            if (sensor_info_per_sensor[i] == SYNTIANT_NDP120_SENSOR_ID_T5838) {
                SYNTIANT_TRACE("t5838 sensor status:\n");
                sensor_status = (struct syntiant_ndp120_tiny_t5838_status_s *)
                    sensor_state->other_sensor_state;
                SYNTIANT_TRACE("num_frames_below_10fe_spl=%u\n"
                       "prev reg 0x35=0x%x\nprev reg 0x36=0x%x\n"
                       "curr reg 0x35=0x%x\ncurr reg 0x36=0x%x\n",
                        sensor_status->num_frames_below_10fe_spl,
                        sensor_status->prev_registers_36_35 & 0xff,
                        (sensor_status->prev_registers_36_35 & 0xff00) >> 8,
                        sensor_status->curr_registers_36_35 & 0xff,
                        (sensor_status->curr_registers_36_35 & 0xff00) >> 8);
            }
        }
    }
}

void ndp_core2_platform_tiny_debug(void)
{
    int s;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;
    struct syntiant_ndp120_tiny_debug dbg_cnts;

    memset(&dbg_cnts, 0, sizeof(dbg_cnts));
    s = syntiant_ndp120_tiny_get_debug(ndpp, &dbg_cnts);
    if (!s) {
        SYNTIANT_TRACE("******Debug counters\n");
        print_debug_counters(&dbg_cnts);
    }
}
#endif
