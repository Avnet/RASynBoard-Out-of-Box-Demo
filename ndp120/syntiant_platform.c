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

#include "syntiant_common.h"

#ifndef TELINK_8X_REFERENCE
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#endif

#include "syntiant_platform.h"
#include "syntiant_util.h"
#include "syntiant_driver.h"

#include <syntiant_ilib/syntiant_ndp120_tiny.h>
#include <syntiant_ilib/syntiant_tiny_cspi.h>

#ifdef ENABLE_REG_DUMP
#include <syntiant_ilib/ndp120_regs.h>
#include <syntiant_ilib/ndp120_spi_regs.h>
#include <syntiant-dsp-firmware/ndp120_dsp_fw_state.h>
#include <syntiant-firmware/ndp120_firmware.h>
#endif

#if 1
#define SYNTIANT_TRACE(...)      do {printf(__VA_ARGS__);}while(0)
#else
#define SYNTIANT_TRACE(...)
#endif

#define NDP_BU_TEST    //disable it when bring up done

static char *labels[SYNTIANT_NDP120_MAX_CLASSES];
static char *labels_per_network[SYNTIANT_NDP120_MAX_NNETWORKS]
            [SYNTIANT_NDP120_MAX_CLASSES];
static char numlabels_per_network[SYNTIANT_NDP120_MAX_NNETWORKS];

struct ndp_core2_platform_tiny_s {
    int initialized;
    int loop_sequency;
    int nn_load_index;
    uint8_t clock_option;
    uint8_t use_xtal;
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
#ifdef NDP_PRE_TEST

static int do_pre_test(void)
{
    int s;
    uint8_t u8_val = 0xAB;
    uint32_t u32_val = 0;

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
   
    /*********** MCU MEM ***********/
    u32_val = 0x44332211;
    s = syntiant_tiny_transfer(&ndp120->ndp_handle, 1, 0x20000000, NULL, &u32_val, 4);
    if(s) {
        SYNTIANT_TRACE("read mcu:0x20000000 failed\n");
        return s;
    }
    SYNTIANT_TRACE("read mcu:0x20000000: 0x%x\n", u32_val);

    u32_val = 0x11223344;
    s = syntiant_tiny_transfer(&ndp120->ndp_handle, 1, 0x20000000, &u32_val, NULL, 4);
    if(s) {
        SYNTIANT_TRACE("write mcu:0x20000000 failed\n");
        return s;
    }
	SYNTIANT_TRACE("write mcu:0x20000000: 0x%x\n", u32_val);

    u32_val = 0;
    s = sytiant_ndp_transfer(&ndp120->ndp_handle, 1, 0x20000000, NULL, &u32_val, 4);
    if(s) {
        SYNTIANT_TRACE("read mcu:0x20000000 failed\n");
        return s;
    }
    SYNTIANT_TRACE("read mcu:0x20000000: 0x%x\n", u32_val);

    return 0;
}
#endif

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

enum {
    PLL_PRESET_OP_VOLTAGE_0p9,
    PLL_PRESET_OP_VOLTAGE_1p0,
    PLL_PRESET_OP_VOLTAGE_1p1
};

#define ARRAY_LEN(v)    (sizeof(v)/sizeof(v[0]))

typedef struct {
    const char *name;
    int operating_voltage;
    uint32_t input_freq;
    uint32_t output_freq;
    uint32_t pdm_freq;
} ndp120_fll_preset_t;

/* Define the table of FLL settings */
ndp120_fll_preset_t ndp120_fll_presets[] = {
    {"mode_fll_0p9v_15p360MHz_32p768kHz", PLL_PRESET_OP_VOLTAGE_0p9, 32768, 15360000, 768000},
    {"mode_fll_0p9v_16p896MHz_32p768kHz", PLL_PRESET_OP_VOLTAGE_0p9, 32768, 16896000, 768000},
    { NULL, 0, 0, 0, 0}
};

uint8_t ndp120_fll_presets_elements = ARRAY_LEN(ndp120_fll_presets);

static int do_clock_config(struct syntiant_ndp120_tiny_device_s *ndp, 
        uint8_t clock_option, int use_xtal)
{
    int s = SYNTIANT_NDP_ERROR_NONE;
    struct syntiant_ndp120_tiny_clk_config_data cfg;
    memset(&cfg, 0, sizeof(cfg));

    ndp120_fll_preset_t *fll_preset;
    uint8_t fll_start_idx = FLL_START_INDEX;
    uint8_t fll_end_idx = fll_start_idx + ndp120_fll_presets_elements - 2;

    if (clock_option == EXT_CLK_INDEX) {
        cfg.src = SYNTIANT_NDP120_MAIN_CLK_SRC_EXT;
        cfg.ref_type = 0; /* ext clock uses clkpad as refsel */
        cfg.ref_freq = EXT_CLOCK_FREQ;
        cfg.core_freq = cfg.ref_freq;
        cfg.voltage = PLL_PRESET_OP_VOLTAGE_0p9;
    } else if ((clock_option <= fll_end_idx) &&
               (clock_option >= fll_start_idx)) { /* use FLL */
        cfg.src = SYNTIANT_NDP120_MAIN_CLK_SRC_FLL;
        if (use_xtal) { /* XTAL */
            cfg.ref_type = 1;
        } else { /* clkpad */
            cfg.ref_type = 0;
        }
        fll_preset = &ndp120_fll_presets[clock_option - fll_start_idx];
        cfg.core_freq = fll_preset->output_freq;
        cfg.voltage = fll_preset->operating_voltage;
        cfg.ref_freq = FLL_CLOCK_FREQ;
    } else {
        SYNTIANT_TRACE("Invalid option: %d\n", clock_option);
        s = SYNTIANT_NDP_ERROR_ARG;
        goto error;
    }

    s = syntiant_ndp120_tiny_clock_cfg(ndp, &cfg);

error:
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

#ifdef NDP_LOAD_CODE
/**
 * Load from codes
*/
#include "synpkg_files/mcu_fw_120.h"
#define MCU_FIRMWARE_ARRAY    MCU_FW_120_SYNPKG

/** DSP firmware
*/
#include "synpkg_files/dsp_firmware_noaec_ff.h"
#define DSP_FIRMWARE_ARRAY    DSP_FIRMWARE_NOAEC_FF_SYNPKG

/** NN Firmware
*/

#if 1
#include "synpkg_files/menu_demo_512_noaec_newph_v96_RASYN.h"
#define NN_FIRMWARE_ARRAY   MENU_DEMO_512_NOAEC_NEWPH_V96_RASYN_SYNPKG
#endif


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

#elif defined(NDP_LOAD_FLASH)
/**
 * Load from flash
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
#if 0
    syntiant_tiny_flash_read(package_addr, split_data, 1024);
    SYNTIANT_TRACE("read type[%d] got:\n    ");
    for (int i = 0; i < 16; i ++) {
    	SYNTIANT_TRACE("%02x ", split_data[i]);
    }
    SYNTIANT_TRACE("\n");
#endif

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
#elif defined(NDP_LOAD_SD)

#include "fat_load.h"

/**
 * Load from FATFS on uSD card or MSPI Flash automatically
*/
static int do_fat_load_synpkg(struct syntiant_ndp120_tiny_device_s *ndp,
        char * file_name)
{
    int s = 0;
    unsigned char split_data[LOAD_SPLIT_SIZE_FAT];
    int split_index = 0;
    int split_len;

    uint32_t package_len;

    SYNTIANT_TRACE("Loading %s\n", file_name);

    /* reset parser state */
    s = syntiant_ndp120_tiny_load(ndp, NULL, 0);
    if (s != SYNTIANT_NDP_ERROR_MORE) {
        SYNTIANT_TRACE("Error resetting package load state\n");
        return s;
    }

    package_len = get_synpkg_size(file_name);
    while (split_index < package_len) {
        split_len = (LOAD_SPLIT_SIZE_FAT<(package_len-split_index))?
                LOAD_SPLIT_SIZE_FAT:(package_len-split_index);
        read_synpkg_block(file_name, split_index, split_data, split_len);
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

#elif defined(LOAD_FROM_FLASH)

static int load_from_flash(struct syntiant_ndp120_tiny_device_s *ndp)
{
    int s = SYNTIANT_NDP120_ERROR_NONE;

    s = syntiant_ndp120_tiny_poll_notification(ndp,
            SYNTIANT_NDP120_NOTIFICATION_BOOTING);
    if (s) {
        printf("poll_notification failed %d\n", s);
        goto done;
    }

    s = syntiant_ndp120_tiny_boot_from_flash(ndp);
    if (s) {
        printf("boot from flash error %d\n", s);
        goto done;
    }

    printf("boot from flash ok\n");

done:
    return s;
}
#endif

/************************************************************/
static int do_binary_loading(struct syntiant_ndp120_tiny_device_s *ndp)
{
    int s;
    
    /* load synpkg */
#ifdef NDP_LOAD_CODE 

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

#elif defined(NDP_LOAD_FLASH)
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

#elif defined(NDP_LOAD_SD)
    if (get_synpkg_boot_mode() != BOOT_MODE_SD){
        SYNTIANT_TRACE("SD card is not available\n");
        s = SYNTIANT_NDP_ERROR_PACKAGE;
        goto error;
    }

    /* load mcu file */
    s = do_fat_load_synpkg(ndp, mcu_file_name);
    if(s) {
        SYNTIANT_TRACE("load ndp120 mcu failed\n");
        goto error;
    }

    /* load dsp file */
    s = do_fat_load_synpkg(ndp, dsp_file_name);
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
    s = do_fat_load_synpkg(ndp, model_file_name);
    if(s) {
        SYNTIANT_TRACE("load ndp120 nn failed\n");
        goto error;
    }

#elif defined(LOAD_FROM_FLASH)
/**
 * FLASH is attched to NDP 
 */
    /* via MB command */
    SYNTIANT_TRACE("MB BOOT FROM FLASH...\n");
    s = syntiant_ndp120_tiny_soft_flash_boot(ndp);
    if (s) {
        SYNTIANT_TRACE("set soft boot from flash failed %d\n", s);
        /* fall thru anyway */
    }
    s =  load_from_flash(ndp);
    if (s) {
        SYNTIANT_TRACE("Error loading from flash %d\n", s);
        goto error;
    }
#endif

    SYNTIANT_TRACE("finished loading ndp120 binary\n");

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
    case SYNTIANT_NDP_FEATURE_PDM:
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

    case SYNTIANT_NDP_FEATURE_NONE:
        s = syntiant_ndp120_tiny_pdm_clock_exe_mode(ndpp, 
                SYNTIANT_NDP120_PDM_CLK_START_PAUSE);
        break;

    default:
        break;
    }
    
    return s;
}

/**
 * function call to get info
*/
int ndp_core2_platform_tiny_get_info(char *ndp_info_data, 
        uint32_t *total_nn, uint32_t *labels_len)
{
    int s = SYNTIANT_NDP_ERROR_NONE;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;
    struct syntiant_ndp120_tiny_info info;
    int j, class_num, nn_num, prev_nn_num, num_labels;
    char *label_string;
    char *fwver = ndp_info_data;
    char *dspfwver = fwver + NDP120_MCU_FW_VER_MAX_LEN;
    char *pkgver = dspfwver + NDP120_MCU_DSP_FW_VER_MAX_LEN;
    uint8_t *pbiver = (uint8_t*)(pkgver + NDP120_MCU_PKG_VER_MAX_LEN);
    char *label_data = (char*)(pbiver + NDP120_MCU_PBI_VER_MAX_LEN);
    unsigned int *scale_factor = (unsigned int *)(label_data + NDP120_MCU_LABELS_MAX_LEN);
    uint32_t *sensor_info_per_sensor = (uint32_t*)(scale_factor + (sizeof(uint32_t)*SYNTIANT_NDP120_MAX_CLASSES));
    
    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    info.fw_version = fwver;
    info.dsp_fw_version = dspfwver;
    info.pkg_version = pkgver;
    info.pbi = pbiver;
    info.labels = label_data;
    info.scale_factor = scale_factor;
    info.sensor_info = sensor_info_per_sensor;
    s = syntiant_ndp120_tiny_get_info(ndpp, &info);
    if (s) return s;

    *labels_len = info.labels_len;
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

    return s;
}

/**
 * function call to poll ndp
*/
int ndp_core2_platform_tiny_poll(uint32_t *notifications, int clear)
{
    int s;
    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;
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
         char *label_string)
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

    if (!(ndp120->curr_notification&SYNTIANT_NDP120_NOTIFICATION_MATCH)) {
        return SYNTIANT_NDP_ERROR_FAIL;
    }

    ndp120->curr_notification = 0x0;

    s = syntiant_ndp120_tiny_get_match_result(ndpp, &match);
    if(s) return s;

    summary = match.summary;
    SYNTIANT_TRACE("get_match_summary: summary=0x%x, last_network_id=%d\n",
           summary, ndpp->last_network_id);

    if (summary & NDP120_SPI_MATCH_MATCH_MASK) {
        match_found = 1;
        *nn_id = ndpp->last_network_id;
        *match_id = summary & NDP120_SPI_MATCH_WINNER_MASK;
        if (label_string) {
            label_match = labels_per_network[ndpp->last_network_id]
                       [summary & NDP120_SPI_MATCH_WINNER_MASK];
            if (label_match) strcpy(label_string, label_match);
        }

#ifdef ENABLE_REG_DUMP
    dump_raw_strengths(&match);
#endif
    }

    return (match_found==1)?SYNTIANT_NDP_ERROR_NONE:SYNTIANT_NDP_ERROR_FAIL;
}

int ndp_core2_platform_tiny_mspi_config(void)
{
    int s;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    s = syntiant_cspi_init(ndpp);
    return s;
}

int ndp_core2_platform_tiny_mspi_read(int ssb, int num_bytes, 
        uint8_t* data, int end_packet)
{
    int s;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    s = syntiant_cspi_read(ndpp, ssb, num_bytes, data, end_packet);
    return s;
}

int ndp_core2_platform_tiny_mspi_write(int ssb, int num_bytes, 
        uint8_t* data, int end_packet)
{
    int s;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    s = syntiant_cspi_write(ndpp, ssb, num_bytes, data, end_packet);
    return s;
}

int ndp_core2_platform_tiny_halt_mcu(void)
{
    int s;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;

    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    s = syntiant_ndp120_tiny_halt_mcu(ndpp);
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

/**
 * function call to get extract info
*/
int ndp_core2_platform_tiny_get_samplesize(uint32_t *sample_size)
{
    int s;
    uint32_t audio_chunk_size, cur_sample_size;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;
    
    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    s = syntiant_ndp120_tiny_get_audio_chunk_size(ndpp, &audio_chunk_size);
    if (s) return s;

    cur_sample_size  = (uint32_t) (audio_chunk_size * SYNTIANT_NDP120_TINY_AUDIO_SAMPLE_RATE
                          * SYNTIANT_NDP120_TINY_AUDIO_SAMPLES_PER_WORD / 1000000);
    /* add annotation size since annotation is enabled */
    cur_sample_size = cur_sample_size + sizeof(struct syntiant_ndp120_tiny_dsp_audio_sample_annotation_t);
    *sample_size = cur_sample_size;

    return s;
}

uint32_t ndp_core2_platform_tiny_get_samplebytes(void)
{
    return SYNTIANT_NDP120_TINY_AUDIO_SAMPLES_PER_WORD;
}

uint32_t ndp_core2_platform_tiny_get_samplerate(void)
{
    return SYNTIANT_NDP120_TINY_AUDIO_SAMPLE_RATE;
}

int ndp_core2_platform_tiny_get_extract(void)
{
    struct ndp120_tiny_handle_t *ndp_handle = &ndp120->ndp_handle;

    if (ndp_handle->notification_save&SYNTIANT_NDP120_NOTIFICATION_EXTRACT_READY) {
        ndp_handle->notification_save = 0x0;
        return SYNTIANT_NDP_ERROR_NONE;
    }

    return SYNTIANT_NDP_ERROR_FAIL;
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

int ndp_core2_platform_tiny_extract_data(uint8_t *extract_data, uint32_t *extract_len)
{
    int s;
    uint32_t sample_size;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;
    struct syntiant_ndp120_tiny_dsp_audio_sample_annotation_t *annotation;

    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    if (!extract_data) {
        return SYNTIANT_NDP_ERROR_ARG;
    }

    sample_size = ndp120->sample_size;
    s = syntiant_ndp120_tiny_extract_data(ndpp, extract_data, &sample_size, 0);
    if (s) {
        SYNTIANT_TRACE("extract data failed: %d\n", s);
        return s;
    }
    
    if (sample_size > sizeof(struct syntiant_ndp120_tiny_dsp_audio_sample_annotation_t))
        sample_size -= sizeof(struct syntiant_ndp120_tiny_dsp_audio_sample_annotation_t);

    annotation = (struct syntiant_ndp120_tiny_dsp_audio_sample_annotation_t*)(extract_data
                                                        + sample_size);

    *extract_len = 0;
    if (sample_size) {
        /* read annotation to determine which file to write to */
        if ((annotation->src_type
                    == SYNTIANT_NDP120_DSP_DATA_FLOW_SRC_TYPE_PCM_AUDIO) ||
                (annotation->src_type
                    == SYNTIANT_NDP120_DSP_DATA_FLOW_SRC_TYPE_FUNCTION)) {
            *extract_len = sample_size;
            SYNTIANT_TRACE("extract data got %d bytes with type: %d, src: %d\n", 
                    sample_size, annotation->src_type, annotation->src_param);
        }
    }

    return s;
}

int ndp_core2_platform_gpio_config(int gpio_num, uint32_t dir, uint32_t value)
{
    int s;
    struct syntiant_ndp120_tiny_device_s *ndpp = &ndp120->ndp;
    struct syntiant_ndp120_config_gpio_s config;

    if (!ndp120->initialized) {
        return SYNTIANT_NDP_ERROR_UNINIT;
    }

    memset(&config, 0, sizeof(struct syntiant_ndp120_config_gpio_s));
    config.gpio_num = gpio_num;
    config.dir = dir;
    config.value = value;
    s = syntiant_ndp120_tiny_config_gpio(ndpp, &config);
    return s;
}

/**
 * function call to initialize ndp
*/
int ndp_core2_platform_tiny_start(uint8_t clock_option, int use_xtal)
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

    /* load mcu/dsp/nn binary */
    s = do_binary_loading(ndpp);
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

    /* get chunk size */
    s = ndp_core2_platform_tiny_get_samplesize(&ndp120->sample_size);
    if(s) {
        SYNTIANT_TRACE("get sample sizes failed: %d\n", s);
        return s;
    }
    SYNTIANT_TRACE("sample_size %d\n", ndp120->sample_size);

    /* ndp120 initialization done */
    SYNTIANT_TRACE("ndp120 init done...\n");
    return s;
}
