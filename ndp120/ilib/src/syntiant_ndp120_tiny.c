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
 	** SDK: v100 **
*/

#include <stdint.h>
#include <string.h>
#include <syntiant_ilib/syntiant_ndp120_tiny.h>

#ifdef __linux__
#include <stddef.h>
#include <unistd.h>
#include <sys/time.h>
#endif

#include "syntiant_common.h"

/* flag to compile in/out debug prints */
#define SYNTIANT_NDP120_DEBUG         0
#define SYNTIANT_NDP120_DEBUG_POLL    0
#define SYNTIANT_NDP120_DEBUG_MATCH   0

#if SYNTIANT_NDP120_DEBUG || SYNTIANT_NDP120_DEBUG_POLL || \
    SYNTIANT_NDP120_DEBUG_MATCH
#include <stdio.h>
#include <stdarg.h>
#endif

#define OP_SIZE(mcu) (mcu ? 4 : 1)

#define NDP_MBIN_REQUEST_IGNORE_START (0xa)
#define NDP_MBIN_REQUEST_IGNORE_END (0xf)

#define PRESET_VALUE_LEN 21
#define REFACTOR_PLL_INDEX 2

#define IS_HOST2_MCU(cmd) (cmd < NDP_MBIN_REQUEST_IGNORE_START)

#define ndp_spi_read_block(reg, data, len)                                     \
    syntiant_ndp120_tiny_read_block(ndp, SYNTIANT_NDP120_SPI_OP, reg, data, len)
#define ndp_spi_write_block(reg, data, len)                                    \
    syntiant_ndp120_tiny_write_block(                                          \
        ndp, SYNTIANT_NDP120_SPI_OP, reg, data, len)

#define ndp_spi_read(reg, data)                                                \
    syntiant_ndp120_tiny_read(ndp, SYNTIANT_NDP120_SPI_OP, reg, data)
#define ndp_spi_write(reg, data)                                               \
    syntiant_ndp120_tiny_write(ndp, SYNTIANT_NDP120_SPI_OP, reg, data)

#define SYNTIANT_NDP120_CMD_RAM SYNTIANT_NDP120_DL_WINDOW_LOWER

#define NDP120_SPI_INTSTS_MBIN_INT_SHIFT 1
#define NDP120_SPI_INTSTS_MBIN_INT(v) ((v) << NDP120_SPI_INTSTS_MBIN_INT_SHIFT)
#define NDP120_SPI_INTSTS_MBOUT_INT_SHIFT 2
#define NDP120_SPI_INTSTS_MBOUT_INT(v)                                         \
    ((v) << NDP120_SPI_INTSTS_MBOUT_INT_SHIFT)
#define NDP120_SPI_INTSTS_DNN_INT_SHIFT 3
#define NDP120_SPI_INTSTS_DNN_INT(v) ((v) << NDP120_SPI_INTSTS_DNN_INT_SHIFT)
#define NDP120_SPI_INTSTS_FEATURE_INT_SHIFT 4
#define NDP120_SPI_INTSTS_FEATURE_INT(v)                                       \
    ((v) << NDP120_SPI_INTSTS_FEATURE_INT_SHIFT)
#define NDP120_SPI_INTSTS_AE_INT_SHIFT 5
#define NDP120_SPI_INTSTS_AE_INT(v) ((v) << NDP120_SPI_INTSTS_AE_INT_SHIFT)
#define NDP120_SPI_INTSTS_WM_INT_SHIFT 6
#define NDP120_SPI_INTSTS_WM_INT(v) ((v) << NDP120_SPI_INTSTS_WM_INT_SHIFT)
#define NDP120_SPI_INTSTS_RF_INT_SHIFT 7
#define NDP120_SPI_INTSTS_RF_INT(v) ((v) << NDP120_SPI_INTSTS_RF_INT_SHIFT)

#define NDP120_SPI_CFG_INTEN_SHIFT 4
#define NDP120_SPI_CFG_INTEN(v) ((v) << NDP120_SPI_CFG_INTEN_SHIFT)
#define NDP120_SPI_CFG_INTNEG_SHIFT 5
#define NDP120_SPI_CFG_INTNEG(v) ((v) << NDP120_SPI_CFG_INTNEG_SHIFT)
#define NDP120_SPI_CFG_INTEN_INSERT(x, v)                                      \
    ((x) | ((v) << NDP120_SPI_CFG_INTEN_SHIFT))

#define NDP120_SPI_INTCTL_MBIN_INTEN_SHIFT 1
#define NDP120_SPI_INTCTL_MBIN_INTEN(v)                                        \
    ((v) << NDP120_SPI_INTCTL_MBIN_INTEN_SHIFT)
#define NDP120_SPI_INTCTL_MBIN_INTEN_INSERT(x, v)                              \
    ((x) | ((v) << NDP120_SPI_INTCTL_MBIN_INTEN_SHIFT))

#define NDP120_SPI_INTCTL_MBOUT_INTEN_SHIFT 2
#define NDP120_SPI_INTCTL_MBOUT_INTEN(v)                                       \
    ((v) << NDP120_SPI_INTCTL_MBOUT_INTEN_SHIFT)
#define NDP120_SPI_INTCTL_MBOUT_INTEN_INSERT(x, v)                             \
    ((x) | ((v) << NDP120_SPI_INTCTL_MBOUT_INTEN_SHIFT))

#define NDP120_SPI_INTCTL_DNN_INTEN_SHIFT 3
#define NDP120_SPI_INTCTL_DNN_INTEN(v)                                         \
    ((v) << NDP120_SPI_INTCTL_DNN_INTEN_SHIFT)
#define NDP120_SPI_INTCTL_FEATURE_INTEN_SHIFT 4
#define NDP120_SPI_INTCTL_FEATURE_INTEN(v)                                     \
    ((v) << NDP120_SPI_INTCTL_FEATURE_INTEN_SHIFT)

#define NDP120_SPI_INTCTL_AE_INTEN_SHIFT 5
#define NDP120_SPI_INTCTL_AE_INTEN(v) ((v) << NDP120_SPI_INTCTL_AE_INTEN_SHIFT)
#define NDP120_SPI_INTCTL_AE_INTEN_INSERT(x, v)                                \
    ((x) | ((v) << NDP120_SPI_INTCTL_AE_INTEN_SHIFT))

#define NDP120_SPI_INTCTL_WM_INTEN_SHIFT 6
#define NDP120_SPI_INTCTL_WM_INTEN(v) ((v) << NDP120_SPI_INTCTL_WM_INTEN_SHIFT)
#define NDP120_SPI_INTCTL_RF_INTEN_SHIFT 7
#define NDP120_SPI_INTCTL_RF_INTEN(v) ((v) << NDP120_SPI_INTCTL_RF_INTEN_SHIFT)

#define NDP120_SPI_CTL_PORSTN_SHIFT 1
#define NDP120_SPI_CTL_PORSTN_MASK 0x02U
#define NDP120_SPI_CTL_PORSTN(v) ((v) << NDP120_SPI_CTL_PORSTN_SHIFT)
#define NDP120_SPI_CTL_PORSTN_INSERT(x, v)                                     \
    ((x) | ((v) << NDP120_SPI_CTL_PORSTN_SHIFT))
#define NDP120_SPI_CTL_PORSTN_MASK_INSERT(x, v)                                \
    (((x) & ~NDP120_SPI_CTL_PORSTN_MASK) | ((v) << NDP120_SPI_CTL_PORSTN_SHIFT))

#define NDP120_SPI_CTL_RESETN_SHIFT 0
#define NDP120_SPI_CTL_RESETN_INSERT(x, v)                                     \
    ((x) | ((v) << NDP120_SPI_CTL_RESETN_SHIFT))

#define NDP120_SPI_CTL_FLASHCTL_DISABLE 0x0U
#define NDP120_SPI_CTL_FLASHCTL_SHIFT 6
#define NDP120_SPI_CTL_FLASHCTL_INSERT(x, v)                                   \
    ((x) | ((v) << NDP120_SPI_CTL_FLASHCTL_SHIFT))

#define NDP120_SPI_CTL_CLKEN_SHIFT 2
#define NDP120_SPI_CTL_CLKEN_INSERT(x, v)                                      \
    ((x) | ((v) << NDP120_SPI_CTL_CLKEN_SHIFT))

#define NDP120_SPI_CTL_RESETN_SHIFT 0
#define NDP120_SPI_CTL_RESETN_INSERT(x, v)                                     \
    ((x) | ((v) << NDP120_SPI_CTL_RESETN_SHIFT))

#define NDP120_CHIP_CONFIG_WATERMARKINT 0x40009094U
#define NDP120_DSP_CONFIG_BUFFILLLEVEL(i) (0x4000c14cU + ((i) << 2))

#define PACKAGE_MODE_DONE (0)

#define BUFDELTA(CUR, LAST, BUFSIZE) ((CUR) + ((CUR) < (LAST) ? (BUFSIZE) : 0) - (LAST))\

#define SYNTIANT_NDP120_MCU_FW_VER_LEN (SYNTIANT_NDP120_OPEN_RAM_RESULTS)
#define SYNTIANT_NDP120_MCU_DSP_FW_VER_LEN                                     \
    (SYNTIANT_NDP120_MCU_FW_VER_LEN + sizeof(uint32_t))
#define SYNTIANT_NDP120_MCU_PKG_VER_LEN                                        \
    (SYNTIANT_NDP120_MCU_DSP_FW_VER_LEN + sizeof(uint32_t))
#define SYNTIANT_NDP120_MCU_LABELS_LEN                                         \
    (SYNTIANT_NDP120_MCU_PKG_VER_LEN + sizeof(uint32_t))
#define SYNTIANT_NDP120_MCU_PBI_VER_LEN                                        \
    (SYNTIANT_NDP120_MCU_LABELS_LEN + sizeof(uint32_t))

#define SYNTIANT_NDP120_DSP_MB_PAYLOAD_SHIFT 0
#define SYNTIANT_NDP120_DSP_MB_PAYLOAD_MASK  0xFFFF
#define SYNTIANT_NDP120_DSP_MB_MESSAGE_SHIFT 0x10
#define SYNTIANT_NDP120_DSP_MB_MESSAGE_MASK  0xFF

#define SYNTIANT_NDP120_DSP_MB_GET_PAYLOAD(x) \
    ((x >> SYNTIANT_NDP120_DSP_MB_PAYLOAD_SHIFT) & \
     SYNTIANT_NDP120_DSP_MB_PAYLOAD_MASK)

#define SYNTIANT_NDP120_DSP_MB_GET_MESSAGE(x) \
    ((x >> SYNTIANT_NDP120_DSP_MB_MESSAGE_SHIFT) & \
     SYNTIANT_NDP120_DSP_MB_MESSAGE_MASK)

#define NDP120_CHIP_CONFIG_GPIOSEL (0x4000901cU)
#define NDP120_GPIO_OUTSET (0x40011010U)
#define NDP120_GPIO_OUTCLR (0x40011014U)
#define NDP120_GPIO1_OUTSET (0x40010010U)
#define NDP120_GPIO1_OUTCLR (0x40010014U)
#define NDP120_GPIO_DATAOUT (0x40011004U)
#define NDP120_GPIO1_DATAOUT (0x40010004U)

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define FIRMWARE_MAX_SIZE (0xC000)
#define FLL_CLOCK_FREQ      (32768)
#define EXT_CLOCK_FREQ      (21504000)

#if SYNTIANT_NDP120_DEBUG
static int SYNTIANT_NDP120_PRINTF(const char * fmt, ...){
    int ret;
    va_list args;
    va_start(args, fmt);
#ifdef __KERNEL__
    ret = vprintk(fmt, args);
#else
    ret = vfprintf(stderr, fmt, args);
#endif
    va_end(args);
    return ret;
}
#else
static int SYNTIANT_NDP120_PRINTF(const char *fmt, ...){(void)fmt;return 0;}
#endif

#if SYNTIANT_NDP120_DEBUG_MATCH
#define SYNTIANT_NDP120_MATCH_PRINTF printf
#else
static int SYNTIANT_NDP120_MATCH_PRINTF(const char *f, ...){(void)f;return 0;}
#endif

/* MCU --> HOST */
enum {
    SYNTIANT_NDP120_MB_MCU_RUNNING = 0x10,
    SYNTIANT_NDP120_MB_MCU_BOOTING = 0x11,
    SYNTIANT_NDP120_MB_MCU_LOAD_DONE = 0x12,
    SYNTIANT_NDP120_MB_MCU_RUNNING_TEST = 0x13,
    SYNTIANT_NDP120_MB_OTHER_LOAD_DONE = 0x14,
    SYNTIANT_NDP120_MB_MCU_GPIO_CHANGE_LOW = 0x15,
    SYNTIANT_NDP120_MB_MCU_GPIO_CHANGE_HIGH = 0x16,
    SYNTIANT_NDP120_MB_MCU_HEARTBEAT = 0x17,
    SYNTIANT_NDP120_MB_MCU_EXTRACT = 0x18,

    /* bitmasks */
    SYNTIANT_NDP120_MB_MCU_ERROR_MASK = 0x20,
    SYNTIANT_NDP120_MB_MCU_NETWORK_MASK = 0x40

};

/* DSP -> HOST REQ */
enum {
    /* req */
    /* IMPORTANT
       Values below that require a reply must be in the range of 0xA - 0x0F
       and cannot overlap with tne NDP_DSP_MB_H2D values above */

    SYNTIANT_NDP120_DSP_MB_D2H_WATERMARK             = 0x0F,
    SYNTIANT_NDP120_DSP_MB_D2H_EXTRACT_READY         = 0x10,
    SYNTIANT_NDP120_DSP_MB_D2H_RUNNING               = 0x12,
    SYNTIANT_NDP120_DSP_MB_D2H_DEBUG                 = 0x13,
    SYNTIANT_NDP120_DSP_MB_D2H_ALGO_ERROR_INIT       = 0x14,
    SYNTIANT_NDP120_DSP_MB_D2H_ALGO_ERROR_PROCESS    = 0x15,
    SYNTIANT_NDP120_DSP_MB_D2H_NO_DNN_MEM            = 0x16,
    SYNTIANT_NDP120_DSP_MB_D2H_NO_DSP_MEM            = 0x17,
    SYNTIANT_NDP120_DSP_MB_D2H_NO_VAD_MIC            = 0x18,

    /* resp */
    SYNTIANT_NDP120_DSP_MB_H2D_WATERMARK             =
        SYNTIANT_NDP120_DSP_MB_D2H_WATERMARK,
    SYNTIANT_NDP120_DSP_MB_H2D_EXTRACT_READY         =
        SYNTIANT_NDP120_DSP_MB_D2H_EXTRACT_READY,
    SYNTIANT_NDP120_DSP_MB_H2D_RUNNING               =
        SYNTIANT_NDP120_DSP_MB_D2H_RUNNING,
    SYNTIANT_NDP120_DSP_MB_H2D_DEBUG                 =
        SYNTIANT_NDP120_DSP_MB_D2H_DEBUG,
    SYNTIANT_NDP120_DSP_MB_H2D_ALGO_ERROR_INIT       =
        SYNTIANT_NDP120_DSP_MB_D2H_ALGO_ERROR_INIT
};

/* MCU to HOST */
enum {
    /* errors */
    SYNTIANT_NDP120_MB_MCU_ERROR_ALGO_INIT = SYNTIANT_NDP120_MB_MCU_ERROR_MASK |
            SYNTIANT_NDP120_DSP_MB_D2H_ALGO_ERROR_INIT,
    SYNTIANT_NDP120_MB_MCU_ERROR_NO_DNN_MEM = SYNTIANT_NDP120_MB_MCU_ERROR_MASK|
            SYNTIANT_NDP120_DSP_MB_D2H_NO_DNN_MEM,
    SYNTIANT_NDP120_MB_MCU_ERROR_NO_DSP_MEM = SYNTIANT_NDP120_MB_MCU_ERROR_MASK|
            SYNTIANT_NDP120_DSP_MB_D2H_NO_DSP_MEM,
    SYNTIANT_NDP120_MB_MCU_ERROR_NO_VAD_MIC = SYNTIANT_NDP120_MB_MCU_ERROR_MASK |
            SYNTIANT_NDP120_DSP_MB_D2H_NO_VAD_MIC
};

/* HOST -> DSP REQ */
enum {
    /* if you change the below, updated the code in
     syntiant_ndp120_do_mailbox_req_no_sync() in ILIB */

    /* req */
    /* NO values lower than 0x0A.  0x00 --> 0x09 are
       reserved my the MCU */
    SYNTIANT_NDP120_DSP_MB_H2D_RESTART               = 0x0A,
    SYNTIANT_NDP120_DSP_MB_H2D_ADX_UPPER             = 0x0B,
    SYNTIANT_NDP120_DSP_MB_H2D_ADX_LOWER             = 0x0C,
    SYNTIANT_NDP120_DSP_MB_H2D_MODE_CHANGE           = 0x0D,

    /* Value below is for an "extended" message, which
     will be placed in "base.mailbox_msg" in the
     DSP fw state structure, those enums are
     SYNTIANT_NDP120_DSP_MB_H2D_EXT_* */
    SYNTIANT_NDP120_DSP_MB_H2D_EXT                   = 0x0E,
    SYNTIANT_NDP120_DSP_MB_D2H_EXT                   =
        SYNTIANT_NDP120_DSP_MB_H2D_EXT,

    /* First extended message */
    SYNTIANT_NDP120_DSP_MB_H2D_EXT_NN_LOAD_COMPLETE  = 0x0F,
    SYNTIANT_NDP120_DSP_MB_H2D_EXT__START__          =
        SYNTIANT_NDP120_DSP_MB_H2D_EXT_NN_LOAD_COMPLETE,

    /* resp */
    SYNTIANT_NDP120_DSP_MB_D2H_RESTART               =
        SYNTIANT_NDP120_DSP_MB_H2D_RESTART,
    SYNTIANT_NDP120_DSP_MB_D2H_ADX_UPPER             =
        SYNTIANT_NDP120_DSP_MB_H2D_ADX_UPPER,
    SYNTIANT_NDP120_DSP_MB_D2H_ADX_LOWER             =
        SYNTIANT_NDP120_DSP_MB_H2D_ADX_LOWER,
    SYNTIANT_NDP120_DSP_MB_D2H_MODE_CHANGE           =
        SYNTIANT_NDP120_DSP_MB_H2D_MODE_CHANGE
};

enum {
    NDP120_D2M_INVALID_REQ = 0xBAD4,
    NDP120_D2H_INVALID_REQ = 0xDEAD
};

enum {
    NDP120_FW_STATE_ADDRESS_INDEX_FW_STATE = 0x0,
    NDP120_FW_STATE_ADDRESS_INDEX_PH_DATA_COLL = 0x1,
    NDP120_FW_STATE_ADDRESS_INDEX_SMAX_SMOOTHER = 0x2,
    NDP120_FW_STATE_ADDRESS_INDEX_ORCHESTRATOR_PARAMS = 0x3,
    NDP120_FW_STATE_ADDRESS_INDEX_DBG_STATE = 0x4,
    NDP120_FW_STATE_ADDRESS_INDEX_MAX_CNT = 0x8
};

struct syntiant_ndp120_mb_names_s {
    uint32_t op;
    const char *name;
};

/**
 * @brief extended MB command for SYNTIANT_NDP120_MB_MCU_CMD (host to device)
 */
enum {
    SYNTIANT_NDP120_MB_LOAD_DSP = 0x60,
    SYNTIANT_NDP120_MB_LOAD_NN = 0x61,
    SYNTIANT_NDP120_MAIN_CLK_CONFIG = 0x62,
    SYNTIANT_NDP120_PDM_CLK_START = 0x63,
    SYNTIANT_NDP120_PDM_CLK_STOP = 0x64,
    SYNTIANT_NDP120_EXTRACT_START = 0x65,
    SYNTIANT_NDP120_GET_INFO = 0x66,
    SYNTIANT_NDP120_GET_NN_GRAPH = 0x67,
    SYNTIANT_NDP120_GET_DEBUG_INFO = 0x68,
    SYNTIANT_NDP120_GET_NN_FLOW = 0x69,
    SYNTIANT_NDP120_GET_AUDIO_CHUNK_SIZE = 0x6a,
    SYNTIANT_NDP120_INPUT_CONFIG = 0x6b,
    SYNTIANT_NDP120_SET_SPI_DIRECT = 0x6c,
    SYNTIANT_NDP120_SWITCH_FLOW_SET_ID = 0x6d,
    SYNTIANT_NDP120_EXTRACT_STOP = 0x6e,
    SYNTIANT_NDP120_GET_RECORDING_METADATA = 0x6f,
    SYNTIANT_NDP120_SET_TANK_SIZE = 0x70,
    SYNTIANT_NDP120_VAD_MIC_CONTROL = 0x72,
    SYNTIANT_NDP120_SERIAL_TRANSFER = 0x73
};

enum {
    SYNTIANT_NDP120_MB_MCU_MATCH = 0x2
};

enum {
    SYNTIANT_NDP120_MB_BOOT_FROM_FLASH = 0x20
};

#if SYNTIANT_NDP120_DEBUG
static struct syntiant_ndp120_mb_names_s syntiant_ndp120_tiny_mcu_mb_op_names[]
    = {
          { SYNTIANT_NDP120_MB_MCU_NOP, "NOP" },
          { SYNTIANT_NDP120_MB_MCU_CONT, "CONT" },
          { SYNTIANT_NDP120_MB_MCU_MATCH, "MATCH" },
          { SYNTIANT_NDP120_MB_MCU_DATA, "DATA" },
          { SYNTIANT_NDP120_MB_MCU_CMD, "CMD" },
          { SYNTIANT_NDP120_MB_MCU_MIADDR, "MIADDR" },
          { SYNTIANT_NDP120_MB_MCU_LOAD, "LOAD" },
          { SYNTIANT_NDP120_MB_MCU_RUNNING, "RUNNING" },
          { SYNTIANT_NDP120_MB_MCU_RUNNING_TEST, "RUNNING_TEST" },
          { SYNTIANT_NDP120_MB_MCU_BOOTING, "BOOTING" },
          { SYNTIANT_NDP120_MB_MCU_LOAD_DONE, "LOAD_DONE" },
          { SYNTIANT_NDP120_MB_OTHER_LOAD_DONE, "OTHER_PKG_LOAD_DONE" },
          { SYNTIANT_NDP120_MB_MCU_GPIO_CHANGE_LOW, "GPIO_LOW" },
          { SYNTIANT_NDP120_MB_MCU_GPIO_CHANGE_HIGH, "GPIO_HIGH" },
          { SYNTIANT_NDP120_MB_MCU_HEARTBEAT, "HEARTBEAT" },
          { SYNTIANT_NDP120_DSP_MB_H2D_RESTART,    "RESTART"},
          { SYNTIANT_NDP120_DSP_MB_H2D_ADX_UPPER,  "ADX_UPPER"},
          { SYNTIANT_NDP120_DSP_MB_H2D_ADX_LOWER,  "ADX_LOWER"},
          { SYNTIANT_NDP120_MB_BOOT_FROM_FLASH, "BOOT_FROM_FLASH"},
          { 0xff, NULL },
      };
#endif


/* error codes (6-bit) */
enum {
    SYNTIANT_NDP120_MB_ERROR_NONE = 0x0,
    SYNTIANT_NDP120_MB_ERROR_UNEXPECTED = 0x1,
    SYNTIANT_NDP120_MB_ERROR_PACKAGE_MAGIC_TLV = 0x2,
    SYNTIANT_NDP120_MB_ERROR_PACKAGE_FW_SIZE = 0x3,
    SYNTIANT_NDP120_MB_ERROR_PACKAGE_INTEGRITY = 0x4,
    SYNTIANT_NDP120_MB_ERROR_PACKAGE_MISSING_FW = 0x5,
    SYNTIANT_NDP120_MB_ERROR_PACKAGE_FORMAT = 0x6
};

#if SYNTIANT_NDP120_DEBUG
static struct syntiant_ndp120_mb_names_s syntiant_ndp120_mcu_mb_error_names[]
    = {   { SYNTIANT_NDP120_MB_ERROR_NONE, "NONE" },
          { SYNTIANT_NDP120_MB_ERROR_UNEXPECTED, "UNEXPECTED" },
          { SYNTIANT_NDP120_MB_ERROR_PACKAGE_MAGIC_TLV, "MAGIC_TLV" },
          { SYNTIANT_NDP120_MB_ERROR_PACKAGE_FW_SIZE, "FW_SIZE" },
          { SYNTIANT_NDP120_MB_ERROR_PACKAGE_INTEGRITY, "INTEGRITY" },
          { SYNTIANT_NDP120_MB_ERROR_PACKAGE_MISSING_FW, "MISSING_FW" },
          { SYNTIANT_NDP120_MB_ERROR_PACKAGE_FORMAT, "PACKAGE_FORMAT" },
          { 0xff, NULL } };
#endif

enum {
    SYNTIANT_NDP120_PKG_LOADER_MODE_START = 0,
    SYNTIANT_NDP120_PKG_LOADER_MODE_IN_PROGRESS = 1,
    SYNTIANT_NDP120_PKG_LOADER_MODE_COMPLETE = 2
};

struct ndp120_fw_state_pointers_s {
    uint32_t addresses[NDP120_FW_STATE_ADDRESS_INDEX_MAX_CNT];
};

enum syntiant_ndp120_mcu_mbin_op_e {
    SYNTIANT_NDP120_MB_MCU_DATA_MASK        = 0x40
};

static uint8_t syntiant_ndp115_a0_device_id[] = {
    0x48, 0x49, 0x4A, 0x4B, 0
};
static uint8_t syntiant_ndp120_b0_device_id[] = {
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0
};
static uint8_t *syntiant_sc2_device_id[] = {
    syntiant_ndp115_a0_device_id,
    syntiant_ndp120_b0_device_id,
    0
};

typedef struct {
    const char *name;
    uint32_t operating_voltage;
    uint32_t input_freq;
    uint32_t output_freq;
    uint32_t pdm_freq;
} ndp120_fll_preset_t;

/* Define the table of FLL settings */
static ndp120_fll_preset_t ndp120_fll_presets[] = {
#ifndef EXCLUDE_CLOCK_OPTION
    {"mode_fll_0p9v_15p360MHz_32p768kHz", PLL_PRESET_OP_VOLTAGE_0p9, 32768, 15360000, 768000},
#endif
    {"mode_fll_0p9v_16p896MHz_32p768kHz", PLL_PRESET_OP_VOLTAGE_0p9, 32768, 16896000, 768000},
    { NULL, 0, 0, 0, 0}
};

typedef struct {
    uint16_t offset;
    uint16_t value;
} ndp120_pll_preset_value_t;

typedef struct {
    const char *name;
    uint32_t operating_voltage;
    uint32_t input_freq;
    uint32_t output_freq;
    ndp120_pll_preset_value_t *values;
} ndp120_pll_preset_t;

/**
 * @brief data structure for core clock configuration
 */
struct syntiant_ndp120_tiny_clk_config_data {
    uint32_t src;       /**< enum indicating ext, fll, or pll */
    uint32_t ref_freq;  /**< input frequency, valid only for fll or pll */
    uint32_t ref_type;  /**< clkpad or xtalin, valid only for fll or pll */
    uint32_t core_freq; /**< core clock frequency */
    uint32_t voltage;   /**< core voltage */
    ndp120_pll_preset_value_t preset_value[PRESET_VALUE_LEN];
};

#ifndef EXCLUDE_CLOCK_OPTION
/* mode_0p9v_10p752MHz_32p768kHz */
static ndp120_pll_preset_value_t mode_0p9v_10p752MHz_32p768kHz[] = {
    {0x0,0x1f8},
    {0x1,0x1},
    {0x2,0x148},
    {0x3,0x1},
    {0x4,0xd94},
    {0x5,0x14bc},
    {0x7,0xc94},
    {0x8,0x1285},
    {0xa,0x1fe},
    {0xb,0x9d8},
    {0xc,0x445},
    {0x17,0x0},
    {0x18,0x0},
    {0x19,0x0},
    {0x1a,0x0},
    {0x20,0x6},
    {0x23,0x0},
    {0x24,0x0},
    {0x27,0x14},
    {0x38,0x0},
    {0x0, 0x0}
};

/* mode_0p9v_15p360MHz_32p768kHz */
static ndp120_pll_preset_value_t mode_0p9v_15p360MHz_32p768kHz[] = {
    {0x0,0x1f8},
    {0x1,0x1},
    {0x2,0x1d4},
    {0x3,0x6},
    {0x4,0xd94},
    {0x5,0x14bc},
    {0x7,0xc94},
    {0x8,0x1285},
    {0xa,0x1fe},
    {0xb,0xe10},
    {0xc,0x445},
    {0x17,0x0},
    {0x18,0x0},
    {0x19,0x0},
    {0x1a,0x0},
    {0x20,0x6},
    {0x23,0x0},
    {0x24,0x0},
    {0x27,0x14},
    {0x38,0x0},
    {0x0, 0x0}
};
#endif

/* mode_0p9v_21p504MHz_32p768kHz */
static ndp120_pll_preset_value_t mode_0p9v_21p504MHz_32p768kHz[] = {
    {0x0,0x1f8},
    {0x1,0x1},
    {0x2,0x290},
    {0x3,0x2},
    {0x4,0xd94},
    {0x5,0x14bc},
    {0x7,0xc94},
    {0x8,0x1285},
    {0xa,0x1fe},
    {0xb,0x13b0},
    {0xc,0x445},
    {0x17,0x0},
    {0x18,0x0},
    {0x19,0x0},
    {0x1a,0x0},
    {0x20,0x6},
    {0x23,0x0},
    {0x24,0x0},
    {0x27,0x14},
    {0x38,0x0},
    {0x0, 0x0}
};

#ifndef EXCLUDE_CLOCK_OPTION
/* mode_0p9v_15p360MHz_4p096MHz */
static ndp120_pll_preset_value_t mode_0p9v_15p360MHz_4p096MHz[] = {
    {0x0,0x1f8},
    {0x1,0xf},
    {0x2,0x38},
    {0x3,0x2},
    {0x4,0x9cc},
    {0x5,0xf80},
    {0x7,0x9fa},
    {0x8,0xfaf},
    {0xa,0x1fe},
    {0xb,0x1b0},
    {0xc,0x32},
    {0x17,0x0},
    {0x18,0x0},
    {0x19,0x0},
    {0x1a,0x0},
    {0x20,0x6},
    {0x23,0x0},
    {0x24,0x0},
    {0x27,0x14},
    {0x38,0x0},
    {0x0, 0x0}
};

/* mode_0p9v_21p504MHz_4p096MHz */
static ndp120_pll_preset_value_t mode_0p9v_21p504MHz_4p096MHz[] = {
    {0x0,0x1f8},
    {0x1,0xf},
    {0x2,0x4e},
    {0x3,0x6},
    {0x4,0x992},
    {0x5,0x10b7},
    {0x7,0x9b3},
    {0x8,0x10fa},
    {0xa,0x1fe},
    {0xb,0x25c},
    {0xc,0x46},
    {0x17,0x0},
    {0x18,0x0},
    {0x19,0x0},
    {0x1a,0x0},
    {0x20,0x6},
    {0x23,0x0},
    {0x24,0x0},
    {0x27,0x14},
    {0x38,0x0},
    {0x0, 0x0}
};

/* mode_1p0v_49p152MHz_32p768kHz */
static ndp120_pll_preset_value_t mode_1p0v_49p152MHz_32p768kHz[] = {
    {0x0,0x1f8},
    {0x1,0x1},
    {0x2,0x5dc},
    {0x3,0x0},
    {0x4,0xfc8},
    {0x5,0x1580},
    {0x7,0xec8},
    {0x8,0x14b5},
    {0xa,0x1fe},
    {0xb,0x2d00},
    {0xc,0xc83},
    {0x17,0x0},
    {0x18,0x4},
    {0x19,0x0},
    {0x1a,0x0},
    {0x20,0x1},
    {0x23,0x0},
    {0x24,0x0},
    {0x27,0x14},
    {0x38,0x0},
    {0x0, 0x0}
};

/* mode_1p0v_55p296MHz_32p768kHz */
static ndp120_pll_preset_value_t mode_1p0v_55p296MHz_32p768kHz[] = {
    {0x0,0x1f8},
    {0x1,0x1},
    {0x2,0x697},
    {0x3,0x4},
    {0x4,0xfc8},
    {0x5,0x1580},
    {0x7,0xec8},
    {0x8,0x14b5},
    {0xa,0x1fe},
    {0xb,0x32a0},
    {0xc,0xc83},
    {0x17,0x0},
    {0x18,0x4},
    {0x19,0x0},
    {0x1a,0x0},
    {0x20,0x1},
    {0x23,0x0},
    {0x24,0x0},
    {0x27,0x14},
    {0x38,0x0},
    {0x0, 0x0}
};

/* mode_1p0v_49p152MHz_4p096MHz */
static ndp120_pll_preset_value_t mode_1p0v_49p152MHz_4p096MHz[] = {
    {0x0,0x1f8},
    {0x1,0x8},
    {0x2,0x60},
    {0x3,0x0},
    {0x4,0xbc7},
    {0x5,0x12fa},
    {0x7,0xbf4},
    {0x8,0x11aa},
    {0xa,0x1fe},
    {0xb,0x2e1},
    {0xc,0xcd},
    {0x17,0x0},
    {0x18,0x4},
    {0x19,0x0},
    {0x1a,0x0},
    {0x20,0x1},
    {0x23,0x0},
    {0x24,0x0},
    {0x27,0x14},
    {0x38,0x0},
    {0x0, 0x0}
};

/* mode_1p1v_55p296MHz_32p768kHz */
static ndp120_pll_preset_value_t mode_1p1v_55p296MHz_32p768kHz[] = {
    {0x0,0x1f8},
    {0x1,0x1},
    {0x2,0x697},
    {0x3,0x4},
    {0x4,0xf88},
    {0x5,0x16ac},
    {0x7,0xe88},
    {0x8,0x15f4},
    {0xa,0x1fe},
    {0xb,0x32a0},
    {0xc,0xfff},
    {0x17,0x0},
    {0x18,0x4},
    {0x19,0x0},
    {0x1a,0x0},
    {0x20,0x1},
    {0x23,0x0},
    {0x24,0x0},
    {0x27,0x14},
    {0x38,0x0},
    {0x0, 0x0}
};

/* mode_1p1v_76p800MHz_32p768kHz */
static ndp120_pll_preset_value_t mode_1p1v_76p800MHz_32p768kHz[] = {
    {0x0,0x1f8},
    {0x1,0x1},
    {0x2,0x927},
    {0x3,0x6},
    {0x4,0xf88},
    {0x5,0x16ac},
    {0x7,0xe88},
    {0x8,0x15f4},
    {0xa,0x1fe},
    {0xb,0x4650},
    {0xc,0xfff},
    {0x17,0x0},
    {0x18,0x4},
    {0x19,0x0},
    {0x1a,0x0},
    {0x20,0x1},
    {0x23,0x0},
    {0x24,0x0},
    {0x27,0x14},
    {0x38,0x0},
    {0x0, 0x0}
};

/* mode_1p1v_98p304MHz_32p768kHz */
static ndp120_pll_preset_value_t mode_1p1v_98p304MHz_32p768kHz[] = {
    {0x0,0x1f8},
    {0x1,0x1},
    {0x2,0xbb8},
    {0x3,0x0},
    {0x4,0xf88},
    {0x5,0x16ac},
    {0x7,0xe88},
    {0x8,0x15f4},
    {0xa,0x1fe},
    {0xb,0x5a00},
    {0xc,0xfff},
    {0x17,0x0},
    {0x18,0x4},
    {0x19,0x0},
    {0x1a,0x0},
    {0x20,0x1},
    {0x23,0x0},
    {0x24,0x0},
    {0x27,0x14},
    {0x38,0x0},
    {0x0, 0x0}
};

/* mode_1p1v_98p304MHz_0p512MHz */
static ndp120_pll_preset_value_t mode_1p1v_98p304MHz_0p512MHz[] = {
    {0x0,0x1f8},
    {0x1,0x1},
    {0x2,0xc0},
    {0x3,0x0},
    {0x4,0xb86},
    {0x5,0x12a8},
    {0x7,0xba4},
    {0x8,0x12e5},
    {0xa,0x1fe},
    {0xb,0x5c2},
    {0xc,0x131},
    {0x17,0x0},
    {0x18,0x4},
    {0x19,0x0},
    {0x1a,0x0},
    {0x20,0x1},
    {0x23,0x0},
    {0x24,0x0},
    {0x27,0x14},
    {0x38,0x0},
    {0x0, 0x0}
};


/* mode_1p1v_76p800MHz_4p096MHz */
static ndp120_pll_preset_value_t mode_1p1v_76p800MHz_4p096MHz[] = {
    {0x0,0x1f8},
    {0x1,0x4},
    {0x2,0x4b},
    {0x3,0x0},
    {0x4,0xa86},
    {0x5,0x11a8},
    {0x7,0xaa4},
    {0x8,0x11e5},
    {0xa,0x1fe},
    {0xb,0x240},
    {0xc,0x98},
    {0x17,0x0},
    {0x18,0x4},
    {0x19,0x0},
    {0x1a,0x0},
    {0x20,0x1},
    {0x23,0x0},
    {0x24,0x0},
    {0x27,0x14},
    {0x38,0x0},
    {0x0, 0x0}
};

/* mode_1p1v_98p304MHz_4p096MHz */
static ndp120_pll_preset_value_t mode_1p1v_98p304MHz_4p096MHz[] = {
    {0x0,0x1f8},
    {0x1,0x4},
    {0x2,0x60},
    {0x3,0x0},
    {0x4,0xa86},
    {0x5,0x11a8},
    {0x7,0xaa4},
    {0x8,0x11e5},
    {0xa,0x1fe},
    {0xb,0x2e1},
    {0xc,0x98},
    {0x17,0x0},
    {0x18,0x4},
    {0x19,0x0},
    {0x1a,0x0},
    {0x20,0x1},
    {0x23,0x0},
    {0x24,0x0},
    {0x27,0x14},
    {0x38,0x0},
    {0x0, 0x0}
};
#endif

/* Define the table of PLL settings */
ndp120_pll_preset_t ndp120_pll_presets[] = {
#ifndef EXCLUDE_CLOCK_OPTION
    {"mode_0p9v_10p752MHz_32p768kHz", PLL_PRESET_OP_VOLTAGE_0p9, 32768, 10752000, mode_0p9v_10p752MHz_32p768kHz},
    {"mode_0p9v_15p360MHz_32p768kHz", PLL_PRESET_OP_VOLTAGE_0p9, 32768, 15360000, mode_0p9v_15p360MHz_32p768kHz},
#endif
    {"mode_0p9v_21p504MHz_32p768kHz", PLL_PRESET_OP_VOLTAGE_0p9, 32768, 21504000, mode_0p9v_21p504MHz_32p768kHz},
#ifndef EXCLUDE_CLOCK_OPTION
    {"mode_0p9v_15p360MHz_4p096MHz", PLL_PRESET_OP_VOLTAGE_0p9, 4096000, 15360000, mode_0p9v_15p360MHz_4p096MHz},
    {"mode_0p9v_21p504MHz_4p096MHz", PLL_PRESET_OP_VOLTAGE_0p9, 4096000, 21504000, mode_0p9v_21p504MHz_4p096MHz},
    {"mode_1p0v_49p152MHz_32p768kHz", PLL_PRESET_OP_VOLTAGE_1p0, 32768, 49152000, mode_1p0v_49p152MHz_32p768kHz},
    {"mode_1p0v_55p296MHz_32p768kHz", PLL_PRESET_OP_VOLTAGE_1p0, 32768, 55296000, mode_1p0v_55p296MHz_32p768kHz},
    {"mode_1p0v_49p152MHz_4p096MHz", PLL_PRESET_OP_VOLTAGE_1p0, 4096000, 49152000, mode_1p0v_49p152MHz_4p096MHz},
    {"mode_1p1v_55p296MHz_32p768kHz", PLL_PRESET_OP_VOLTAGE_1p1, 32768, 55296000, mode_1p1v_55p296MHz_32p768kHz},
    {"mode_1p1v_76p800MHz_32p768kHz", PLL_PRESET_OP_VOLTAGE_1p1, 32768, 76800000, mode_1p1v_76p800MHz_32p768kHz},
    {"mode_1p1v_98p304MHz_32p768kHz", PLL_PRESET_OP_VOLTAGE_1p1, 32768, 98304000, mode_1p1v_98p304MHz_32p768kHz},
    {"mode_1p1v_98p304MHz_0p512MHz", PLL_PRESET_OP_VOLTAGE_1p1, 512000, 98304000, mode_1p1v_98p304MHz_0p512MHz},
    {"mode_1p1v_76p800MHz_4p096MHz", PLL_PRESET_OP_VOLTAGE_1p1, 4096000, 76800000, mode_1p1v_76p800MHz_4p096MHz},
    {"mode_1p1v_98p304MHz_4p096MHz", PLL_PRESET_OP_VOLTAGE_1p1, 4096000, 98304000, mode_1p1v_98p304MHz_4p096MHz},
#endif
    {  NULL, 0, 0, 0, NULL}
};


/* helper functions */

int
syntiant_ndp120_tiny_read(struct syntiant_ndp120_tiny_device_s *ndp, int mcu,
    uint32_t address, void *value)
{
    return syntiant_ndp120_tiny_read_block(
        ndp, mcu, address, value, OP_SIZE(mcu));
}

int
syntiant_ndp120_tiny_write(struct syntiant_ndp120_tiny_device_s *ndp, int mcu,
    uint32_t address, uint32_t value)
{
    return syntiant_ndp120_tiny_write_block(
        ndp, mcu, address, &value, OP_SIZE(mcu));
}

enum { MB_STATE_NONE = 0, MB_STATE_SYNC = 1, MB_STATE_DATA = 2 };

static uint8_t
get_mbin_seq(struct syntiant_ndp120_tiny_device_s *ndp)
{
    struct syntiant_ndp120_tiny_mb_state_s *mb_state = &ndp->mb_state;
    mb_state->mbin_seq ^= 0x80;
    return mb_state->mbin_seq;
}

static void
mailbox_reset_state(struct syntiant_ndp120_tiny_device_s *ndp)
{
    struct syntiant_ndp120_tiny_mb_state_s *mb_state = &ndp->mb_state;
    mb_state->mbin_resync = 1;
    mb_state->watermarkint_resync = 1;
}

#if SYNTIANT_NDP120_DEBUG
static void ndp_strncpy(char *dst, const char *src, unsigned len)
{
    if (!len) {
        return;
    }
    while (*src && --len){
        *dst++ = *src++;
    }
    *dst = 0;
}
#endif

#if SYNTIANT_NDP120_DEBUG
static char ndp_toascii(unsigned char c)
{
    if (c > 0xF) {
        return 0;
    }
    return (char)(c + (c < 0xA ? '0' : 'A'));
}
#endif

#if SYNTIANT_NDP120_DEBUG
static void
syntiant_ndp120_mcu_mb_error_decoder(uint8_t error, char *buf, uint32_t len)
{
    struct syntiant_ndp120_mb_names_s *p;
    error &= 0x1f;

    if (!buf || !len) return;
    buf[0] = 0;

    for (p = syntiant_ndp120_mcu_mb_error_names; p->name; ++p) {
        if (p->op == error) {
            ndp_strncpy(buf, p->name, len);
            return;
        }
    }
    ndp_strncpy(buf, "<UNKNOWN>", len);
}
#endif

#if SYNTIANT_NDP120_DEBUG
static void
syntiant_ndp120_mcu_mb_op_decoder(uint8_t op, char *buf, uint32_t len)
{
    char errorBuf[32];
    struct syntiant_ndp120_mb_names_s *p;
    op &= 0x7f;

    if (!buf || !len) return;
    buf[0] = 0;

    /* error condition */
    if (op & SYNTIANT_NDP120_MB_MCU_DATA_MASK) {
        ndp_strncpy(buf, "DATA: 0x", len);
        while (*buf) buf++, len--;
        *buf++ = ndp_toascii(op >> 4 & 0x3);
        *buf++ = ndp_toascii(op & 0xF);
        *buf++ = 0;
    } else if ((op & SYNTIANT_NDP120_MB_MCU_ERROR_MASK) &&
               (op & (SYNTIANT_NDP120_MB_MCU_ERROR_MASK - 1))) {
        syntiant_ndp120_mcu_mb_error_decoder(op, errorBuf, sizeof(errorBuf));
        ndp_strncpy(buf, "ERROR: ", len);
        while (*buf) buf++, len--;
        ndp_strncpy(buf, errorBuf, len);
        while (*buf) buf++, len--;
        ndp_strncpy(buf, " (0x", len);
        while (*buf) buf++, len--;
        *buf++ = ndp_toascii(op >> 4 & 0x1);
        *buf++ = ndp_toascii(op & 0xF);
        *buf++ = ')';
        *buf++ = 0;
    } else {
        for (p = syntiant_ndp120_tiny_mcu_mb_op_names; p->name; ++p) {
            if (p->op == op) {
                ndp_strncpy(buf, p->name, len);
                return;
            }
        }
        ndp_strncpy(buf, "<UNKNOWN>", len);
    }
}
#endif

#if SYNTIANT_NDP120_DEBUG
static struct syntiant_ndp120_mb_names_s syntiant_ndp120_tiny_dsp_mb_op_names[]
    = {
        { SYNTIANT_NDP120_DSP_MB_D2H_WATERMARK,           "WATERMARK"},
        { SYNTIANT_NDP120_DSP_MB_D2H_EXTRACT_READY,       "EXTRACT_READY"},
        { SYNTIANT_NDP120_DSP_MB_D2H_RUNNING,             "RUNNING"},
        { SYNTIANT_NDP120_DSP_MB_D2H_DEBUG,               "DEBUG"},
        { SYNTIANT_NDP120_DSP_MB_H2D_RESTART,             "RESTART"},
        { SYNTIANT_NDP120_DSP_MB_H2D_ADX_UPPER,           "ADX_UPPER"},
        { SYNTIANT_NDP120_DSP_MB_H2D_ADX_LOWER,           "ADX_LOWER"},
        {0xFF, NULL}
    };

static void
syntiant_ndp120_dsp_mb_op_decoder(uint32_t msg, char * buf, uint32_t len)
{
    struct syntiant_ndp120_mb_names_s  *p;
    for (p = syntiant_ndp120_tiny_dsp_mb_op_names; p->name; ++p) {
        if (p->op == SYNTIANT_NDP120_DSP_MB_GET_MESSAGE(msg)) {
            ndp_strncpy(buf, p->name, len);
            return;
        }
    }
    ndp_strncpy(buf, "UNKNOWN", len);
}
#endif

static int
mcu_fw_pointers(struct syntiant_ndp120_tiny_device_s *ndp120, int clear)
{
    int s = 0;
    struct ndp120_fw_state_pointers_s fwps;

    if (clear) {
        ndp120->mcu_fw_pointers_addr = ndp120->mcu_fw_state_addr
            = ndp120->mcu_fw_ph_data_coll_addr
            = ndp120->mcu_fw_smax_smoother_addr = 0;
        goto error;
    }

    s = syntiant_ndp120_tiny_read_block(ndp120, SYNTIANT_NDP120_MCU_OP,
        ndp120->mcu_fw_pointers_addr, fwps.addresses, sizeof(fwps.addresses));
    if (s) goto error;

    ndp120->mcu_fw_state_addr
        = fwps.addresses[NDP120_FW_STATE_ADDRESS_INDEX_FW_STATE];
    ndp120->mcu_fw_ph_data_coll_addr
        = fwps.addresses[NDP120_FW_STATE_ADDRESS_INDEX_PH_DATA_COLL];
    ndp120->mcu_fw_smax_smoother_addr
        = fwps.addresses[NDP120_FW_STATE_ADDRESS_INDEX_SMAX_SMOOTHER];
    ndp120->mcu_fw_orchestrator_graph_addr
        = fwps.addresses[NDP120_FW_STATE_ADDRESS_INDEX_ORCHESTRATOR_PARAMS];
    ndp120->mcu_fw_dbg_state_addr
        = fwps.addresses[NDP120_FW_STATE_ADDRESS_INDEX_DBG_STATE];

    SYNTIANT_NDP120_PRINTF("  miaddr: 0x%08X\n", ndp120->mcu_fw_pointers_addr);
    SYNTIANT_NDP120_PRINTF("  fw_state: 0x%08X\n", ndp120->mcu_fw_state_addr);
    SYNTIANT_NDP120_PRINTF(
        "  ph_data_coll: 0x%08X\n", ndp120->mcu_fw_ph_data_coll_addr);
    SYNTIANT_NDP120_PRINTF(
        "  smoother: 0x%08X\n", ndp120->mcu_fw_smax_smoother_addr);
    SYNTIANT_NDP120_PRINTF(
        "  orch_graph: 0x%08X\n", ndp120->mcu_fw_orchestrator_graph_addr);
    SYNTIANT_NDP120_PRINTF(
        "  dbg_state: 0x%08X\n", ndp120->mcu_fw_dbg_state_addr);

error:
    return s;
}

static int
syntiant_ndp120_tiny_mbin_send(
    struct syntiant_ndp120_tiny_device_s *ndp, uint8_t data)
{
    data = (uint8_t)((data & 0x7F) | get_mbin_seq(ndp));
#if SYNTIANT_NDP120_DEBUG
    {
        char buf[32] = "";
        syntiant_ndp120_mcu_mb_op_decoder(data & 0x7F, buf, sizeof(buf));
        SYNTIANT_NDP120_PRINTF("0x%02X (%s) --> mbin\n", data, buf);
    }
#endif
     return ndp_spi_write(NDP120_SPI_MBIN, data);
}

static int
syntiant_ndp120_tiny_mbin_resp_get(
    struct syntiant_ndp120_tiny_device_s *ndp, uint8_t *data)
{
    int s;
    struct syntiant_ndp120_tiny_mb_state_s *mb_state = &ndp->mb_state;
    s = ndp_spi_read(NDP120_SPI_MBIN_RESP, data);
    if (s) goto error;

#if SYNTIANT_NDP120_DEBUG
    {
        char buf[32] = "";
        syntiant_ndp120_mcu_mb_op_decoder(*data, buf, sizeof(buf));
        SYNTIANT_NDP120_PRINTF("0x%02X (%s) <-- mbin_resp\n", *data, buf);
    }
#endif

    if ((*data & 0x80) != mb_state->mbin_resp_seq && !mb_state->mbin_resync) {
        SYNTIANT_NDP120_PRINTF("mbin_resp sequence number is incorrect\n");
        s = SYNTIANT_NDP120_ERROR_FAIL;
        goto error;
    }

    if (mb_state->mbin_resync) {
        mb_state->mbin_resync = 0;
        mb_state->mbin_resp_seq = *data & 0x80;
    }

    mb_state->mbin_resp_seq ^= 0x80;
    *data &= 0x7f;
error:
    return s;
}

static int
syntiant_ndp120_tiny_do_mailbox_req_no_sync(
    struct syntiant_ndp120_tiny_device_s *ndp, uint8_t req, uint32_t *resp)
{
    int s = 0;

    switch (req) {
    case SYNTIANT_NDP120_MB_MCU_MIADDR:
        mcu_fw_pointers(ndp, 1);
        ndp->mb_state.mbin_state = MB_STATE_DATA;
        ndp->mb_state.mbin_data_count = 0;
        ndp->mb_state.mbin_data = 0;
        break;
    case SYNTIANT_NDP120_DSP_MB_H2D_ADX_LOWER:
        ndp->dsp_fw_state_addr = 0;
        ndp->mb_state.watermarkint_state = MB_STATE_SYNC;
        ndp->mb_state.watermarkint_data = 0;
        break;
    default:
        break;
    }

    /* Special case for H2D or D2H */
    /* Put the actual request into dsp_fw_state.base.mailbox_msg, and send
       NDP120_DSP_MB_H2D to the mailbox */

    s = syntiant_ndp120_tiny_mbin_send(ndp, req);
    if (s) goto error;
    s = ndp->iif->mbwait(ndp->iif->d);
    if (s) {
        SYNTIANT_NDP120_PRINTF(
            "Error in syntiant_ndp120_tiny_do_mailbox_req_no_sync, %d\n", s);
        goto error;
    }
    if (resp != NULL) {
        *resp = ndp->mb_state.mbin_resp;
    }

    switch (req) {
    case SYNTIANT_NDP120_MB_MCU_MIADDR:
        ndp->mcu_fw_pointers_addr = ndp->mb_state.mbin_data;
        mcu_fw_pointers(ndp, 0);
        break;
    case SYNTIANT_NDP120_DSP_MB_H2D_ADX_LOWER:
        ndp->dsp_fw_state_addr = (ndp->dsp_fw_state_addr & 0xFFFF0000) |
            ndp->mb_state.watermarkint_data;
        SYNTIANT_NDP120_PRINTF("LOWER: dsp fw ptr: 0x%x\n",
                ndp->dsp_fw_state_addr);
        break;
    case SYNTIANT_NDP120_MB_MCU_CMD:
        if (ndp->mb_state.mbin_resp ==
            SYNTIANT_NDP120_MB_MCU_ERROR_NO_VAD_MIC) {
            s = SYNTIANT_NDP120_ERROR_DSP_NO_VAD_MIC;
        }
        break;
    default:
        break;
    }
error:
    return s;
}

int
syntiant_ndp120_tiny_mb_cmd(struct syntiant_ndp120_tiny_device_s *ndp,
        uint8_t req, uint32_t *msg)
{
    int s = 0;
    uint8_t mb_req = SYNTIANT_NDP120_MB_MCU_CMD;
    uint32_t cmd = req;

    if (ndp->iif->sync) {
        s = (ndp->iif->sync)(ndp->iif->d);
        if (s) {
            SYNTIANT_NDP120_PRINTF("Error in syntiant_ndp120_tiny_mb_cmd\n");
            return s;
        }
    }
    switch (req) {
    case SYNTIANT_NDP120_MB_LOAD_DSP:
        /* fall through */
    case SYNTIANT_NDP120_MB_LOAD_NN:
        break;
    case SYNTIANT_NDP120_MB_BOOT_FROM_FLASH:
        /* Fall through */
    default:
        /* Not an extended MB command*/
        mb_req = req;
        cmd = 0;
        break;
    }
    /* write the extended mbox command */
    if (cmd) {
        SYNTIANT_NDP120_PRINTF("write 0x%x at 0x%x\n", cmd,
            SYNTIANT_NDP120_CMD_RAM);
        s = syntiant_ndp120_tiny_write_block(ndp, 1,
            SYNTIANT_NDP120_CMD_RAM, &cmd, sizeof(cmd));
        if (s) goto error;
    }
    /* send the MB command */
    s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(ndp, mb_req, msg);
    if (s) goto error;
    if (ndp->iif->sync) {
        s = (ndp->iif->unsync)(ndp->iif->d);
    }
error:
    return s;
}


static int
syntiant_read_match_producer(struct syntiant_ndp120_tiny_device_s *ndp)
{
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint32_t net_id = ndp->last_network_id;
    struct syntiant_ndp120_tiny_match_data match;
    uint8_t match_sts;

    s = ndp_spi_read(NDP120_SPI_MATCHSTS, &match_sts);
    if (s) {
        return s;
    }
    if (!match_sts) {
        /* MPF interrupt, not an event match */
        return s;
    }

    /* read from Open RAM */
    s = syntiant_ndp120_tiny_read_block(ndp, SYNTIANT_NDP120_MCU_OP,
        SYNTIANT_NDP120_OPEN_RAM_MATCH_RESULTS, &match, sizeof(match));
    if (s) return s;
    ndp->match_producer[net_id] = match.match_producer;
    return s;
}

/* MBOUT */
static void
mbout_recv(struct syntiant_ndp120_tiny_device_s *ndp, uint8_t *data)
{
    int s = 0;
    struct syntiant_ndp120_tiny_mb_state_s *mb_state = &ndp->mb_state;

    mb_state->mbout = 0;
    s = ndp->iif->mbwait(ndp->iif->d);
    if (s) {
        SYNTIANT_NDP120_PRINTF("Error in mbout receiving, %d\n", s);
        return;
    }

    *data = mb_state->mbout;
}

static void
mbout_send_resp(struct syntiant_ndp120_tiny_device_s *ndp, uint8_t data)
{
    struct syntiant_ndp120_tiny_mb_state_s *mb_state = &ndp->mb_state;
    data = (uint8_t)((data & 0x7f) | mb_state->mbout_seq);

#if SYNTIANT_NDP120_DEBUG
    {
        char buf[32] = "";
        syntiant_ndp120_mcu_mb_op_decoder(data, buf, sizeof(buf));
        SYNTIANT_NDP120_PRINTF("0x%02X (%s) --> mbout_resp\n", data, buf);
    }
#endif
    ndp_spi_write(NDP120_SPI_MBOUT_RESP, data);
}

static uint8_t
mb_resp_is_data(uint8_t result)
{
    return result & SYNTIANT_NDP120_MB_MCU_DATA_MASK;
}

static uint8_t
mb_resp_is_error(uint8_t result, uint8_t *error)
{
    if (mb_resp_is_data(result)) return 0;

    if (result & SYNTIANT_NDP120_MB_MCU_ERROR_MASK) {
        if (error) *error = result & (SYNTIANT_NDP120_MB_MCU_ERROR_MASK - 1);
        return 1;
    }
    return 0;
}

static int
mbout_get(struct syntiant_ndp120_tiny_device_s *ndp, uint8_t *data)
{
    int s;
    struct syntiant_ndp120_tiny_mb_state_s *mb_state = &ndp->mb_state;

    s = ndp_spi_read(NDP120_SPI_MBOUT, data);
    if (s) goto error;

#if SYNTIANT_NDP120_DEBUG
    {
        char buf[32] = "";
        syntiant_ndp120_mcu_mb_op_decoder(*data, buf, sizeof(buf));
        SYNTIANT_NDP120_PRINTF("0x%02X (%s) <-- mbout\n", *data, buf);
    }
#endif
    mb_state->mbout_seq = (*data & 0x80);
    *data &= 0x7f;

error:
    return s;
}

static int
mbout_processor(struct syntiant_ndp120_tiny_device_s *ndp120, uint32_t *notify)
{
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint8_t data;
    char buf[32] = "LOAD DONE";

    *notify = 0;
    s = mbout_get(ndp120, &data);
    if (s) goto error;

    ndp120->mb_state.mbout = data;

    switch (data) {
    case SYNTIANT_NDP120_MB_MCU_RUNNING:
        ndp120->mb_state.mbin_state = MB_STATE_SYNC;
        ndp120->mb_state.mbin_sync_count = 0;
        mailbox_reset_state(ndp120);
        syntiant_ndp120_tiny_mbin_send(ndp120, SYNTIANT_NDP120_MB_MCU_NOP);
        break;

    case SYNTIANT_NDP120_MB_MCU_RUNNING_TEST:
        ndp120->mb_state.mbin_state = MB_STATE_SYNC;
        ndp120->mb_state.mbin_sync_count = 0;
        mailbox_reset_state(ndp120);
        *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_OUT;
        break;

    case SYNTIANT_NDP120_MB_MCU_NOP:
        mbout_send_resp(ndp120, SYNTIANT_NDP120_MB_MCU_NOP);
        *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_OUT;
        break;

    case SYNTIANT_NDP120_MB_MCU_BOOTING:
        SYNTIANT_NDP120_PRINTF("Received BOOTING in mbout\n");
        *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_OUT |
            SYNTIANT_NDP120_NOTIFICATION_BOOTING;
        break;

    case SYNTIANT_NDP120_MB_MCU_CONT:
        SYNTIANT_NDP120_PRINTF("Received CONT in mbout\n");
        *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_OUT;
        break;

    case SYNTIANT_NDP120_MB_MCU_LOAD_DONE:
        *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_OUT;
        break;

    case SYNTIANT_NDP120_MB_OTHER_LOAD_DONE:
#if SYNTIANT_NDP120_DEBUG
        syntiant_ndp120_mcu_mb_op_decoder(data, buf, sizeof(buf));
#endif
        SYNTIANT_NDP120_PRINTF("Received (%s)(0x%x) in mbout\n", buf, data);
        *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_OUT |
                  SYNTIANT_NDP120_NOTIFICATION_OTHER_LOAD_DONE;
        break;

    case SYNTIANT_NDP120_MB_MCU_HEARTBEAT:
        *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_OUT;
        break;

   case SYNTIANT_NDP120_MB_MCU_EXTRACT:
        *notify = SYNTIANT_NDP120_NOTIFICATION_EXTRACT_READY;
        break;

    case SYNTIANT_NDP120_MB_MCU_ERROR_ALGO_INIT:
        *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_OUT |
            SYNTIANT_NDP120_NOTIFICATION_ALGO_ERROR;
        break;

    case SYNTIANT_NDP120_MB_MCU_ERROR_NO_DSP_MEM:
        *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_OUT |
            SYNTIANT_NDP120_NOTIFICATION_DSP_NOMEM_ERROR;
        break;

    case SYNTIANT_NDP120_MB_MCU_ERROR_NO_DNN_MEM:
        *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_OUT |
            SYNTIANT_NDP120_NOTIFICATION_DNN_NOMEM_ERROR;
        break;

    default:
        /* check for a match */
        if (data & SYNTIANT_NDP120_MB_MCU_NETWORK_MASK) {
            ndp120->last_network_id
                = (data & ~SYNTIANT_NDP120_MB_MCU_NETWORK_MASK) & 0xFF;
            s = syntiant_read_match_producer(ndp120);
            if (s) {
                goto error;
            }
            mbout_send_resp(ndp120, SYNTIANT_NDP120_MB_MCU_NOP);
            ndp120->matches++;
            *notify = SYNTIANT_NDP120_NOTIFICATION_MATCH;
        } else {
            SYNTIANT_NDP120_PRINTF("POLL: got unknown mbout: 0x%02X\n", data);
        }
        break;
    }

error:
    return s;
}

static int
mbin_processor(struct syntiant_ndp120_tiny_device_s *ndp, uint32_t *notify)
{
    int s;
    uint8_t data;
    struct syntiant_ndp120_tiny_mb_state_s *mb_state = &ndp->mb_state;

    s = syntiant_ndp120_tiny_mbin_resp_get(ndp, &data);
    if (s) {
        SYNTIANT_NDP120_PRINTF("mbin processor(mbin_resp_get) failed s=%d\n", s);
        goto error;
    }

    mb_state->mbin_resp = data;

    /* post fw load sync code */
    if (mb_resp_is_error(data, NULL)) {
        if (mb_state->mbin_state == MB_STATE_SYNC) {
            ++mb_state->mbin_sync_count;
            if (mb_state->mbin_sync_count < 3) {
                /* error handling? */
                s = syntiant_ndp120_tiny_mbin_send(ndp, SYNTIANT_NDP120_MB_MCU_NOP);
                if (s) SYNTIANT_NDP120_PRINTF("mbin processor(mbin_send 1) failed s=%d\n", s);
            } else {
                mb_state->mbin_state = MB_STATE_NONE;
                *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_IN;
            }
        } else if (mb_state->mbin_state == MB_STATE_NONE) {
            if (data == SYNTIANT_NDP120_MB_MCU_ERROR_NO_VAD_MIC) {
                SYNTIANT_NDP120_PRINTF("mbin processor error: no vad mic\n");
                *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_IN |
                    SYNTIANT_NDP120_NOTIFICATION_DSP_NO_VAD_MIC_ERROR;
            }
        }
        goto error;
    }

    if (data == SYNTIANT_NDP120_MB_MCU_NOP) {
        if (mb_state->mbin_state == MB_STATE_SYNC) {
            mb_state->mbin_state = MB_STATE_NONE;
        }
        *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_IN;
    } else if (mb_resp_is_data(data)) {
        if (mb_state->mbin_state == MB_STATE_DATA) {
            int last, addr_shift, mb_shift;
            ++mb_state->mbin_data_count;
            if (mb_state->mbin_data_count < 6) {
                /* TODO plz use literrals in place of hard coded values */
                last = 0;
                addr_shift = 6;
                mb_shift = 26;
            } else {
                last = 1;
                addr_shift = 2;
                mb_shift = 30;
            }
            mb_state->mbin_data >>= addr_shift;
            mb_state->mbin_data |= (uint32_t)((data & 0x3F) << mb_shift);
            if (last) {
                mb_state->mbin_state = MB_STATE_NONE;
                *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_IN;
            } else {
                /* error handling? */
                s = syntiant_ndp120_tiny_mbin_send(
                    ndp, SYNTIANT_NDP120_MB_MCU_DATA);
                if (s) SYNTIANT_NDP120_PRINTF("mbin processor(mbin_send 2) failed s=%d\n", s);
            }
        }
    } else if (data == SYNTIANT_NDP120_MB_MCU_CONT) {
        *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_IN;
        SYNTIANT_NDP120_PRINTF("mbin processor(mcu cont)\n");
        goto error;
    }
error:
    return s;
}

static int
dsp_mb_processor(struct syntiant_ndp120_tiny_device_s *ndp120, uint32_t *notify)
{
    int s = 0;
#if SYNTIANT_NDP120_DEBUG || SYNTIANT_NDP120_DEBUG_POLL
    uint32_t data;
    static int extract_count = 0;
#endif
    s = syntiant_ndp120_tiny_read(ndp120, SYNTIANT_NDP120_MCU_OP,
            NDP120_CHIP_CONFIG_WATERMARKINT, &ndp120->mb_state.watermarkint);
    if (s) goto error;

#if SYNTIANT_NDP120_DEBUG
    if (SYNTIANT_NDP120_DSP_MB_GET_MESSAGE(ndp120->mb_state.watermarkint) !=
            SYNTIANT_NDP120_DSP_MB_D2H_DEBUG) {
        char buf[32];
        syntiant_ndp120_dsp_mb_op_decoder(ndp120->mb_state.watermarkint, buf,
            sizeof(buf));
        SYNTIANT_NDP120_PRINTF("DSP sent: %s\n", buf);
    }
#endif

    switch (SYNTIANT_NDP120_DSP_MB_GET_MESSAGE(ndp120->mb_state.watermarkint)) {
    case SYNTIANT_NDP120_DSP_MB_D2H_WATERMARK:
        if (SYNTIANT_NDP120_DSP_MB_GET_PAYLOAD(ndp120->mb_state.watermarkint)
            != NDP120_D2H_INVALID_REQ) {
            *notify = SYNTIANT_NDP120_NOTIFICATION_WATER_MARK;
#if SYNTIANT_NDP120_DEBUG || SYNTIANT_NDP120_DEBUG_POLL
            syntiant_ndp120_tiny_read(ndp120, SYNTIANT_NDP120_MCU_OP,
                NDP120_DSP_CONFIG_BUFFILLLEVEL(4), &data);
            SYNTIANT_NDP120_PRINTF("FILL LEVEL: 0x%X\n", data);
            SYNTIANT_NDP120_PRINTF("sending watermark ack\n");
#endif
            syntiant_ndp120_tiny_mbin_send(
                ndp120, SYNTIANT_NDP120_DSP_MB_H2D_WATERMARK);
        }
        break;

    case SYNTIANT_NDP120_DSP_MB_D2H_EXTRACT_READY:
        if (SYNTIANT_NDP120_DSP_MB_GET_PAYLOAD(ndp120->mb_state.watermarkint)
            != NDP120_D2H_INVALID_REQ) {
            *notify |= SYNTIANT_NDP120_NOTIFICATION_EXTRACT_READY;
#if SYNTIANT_NDP120_DEBUG || SYNTIANT_NDP120_DEBUG_POLL
            syntiant_ndp120_tiny_read(ndp120, SYNTIANT_NDP120_MCU_OP,
                NDP120_DSP_CONFIG_BUFFILLLEVEL(4), &data);
            SYNTIANT_NDP120_PRINTF("FILL LEVEL: 0x%X\n", data);
            SYNTIANT_NDP120_PRINTF("Sending extract ack\n");
#endif
#if SYNTIANT_NDP120_DEBUG || SYNTIANT_NDP120_DEBUG_POLL
            extract_count += 1;
            SYNTIANT_NDP120_PRINTF("extract cnt: %d, payload: %d\n",
                extract_count,
                SYNTIANT_NDP120_DSP_MB_GET_PAYLOAD(
                    ndp120->mb_state.watermarkint));
#endif

            /* send watermark as ack, as extract ready won't be accepted */
            syntiant_ndp120_tiny_mbin_send(
                ndp120, SYNTIANT_NDP120_DSP_MB_H2D_WATERMARK);
        }
        break;

    case SYNTIANT_NDP120_DSP_MB_D2H_RESTART:
        *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_IN;
        break;

    case SYNTIANT_NDP120_DSP_MB_D2H_MODE_CHANGE:
        *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_IN;
        break;

    case SYNTIANT_NDP120_DSP_MB_D2H_DEBUG:
        *notify = SYNTIANT_NDP120_NOTIFICATION_DEBUG;
        break;

    case SYNTIANT_NDP120_DSP_MB_D2H_ALGO_ERROR_INIT:
        *notify = SYNTIANT_NDP120_NOTIFICATION_ALGO_ERROR;
#if SYNTIANT_NDP120_DEBUG || SYNTIANT_NDP120_DEBUG_POLL
        SYNTIANT_NDP120_PRINTF("algo_config_index : %d\n",
             SYNTIANT_NDP120_DSP_MB_GET_PAYLOAD(ndp120->mb_state.watermarkint));
#endif
        break;

    case SYNTIANT_NDP120_DSP_MB_D2H_ADX_LOWER:
        ndp120->mb_state.watermarkint_data =
            SYNTIANT_NDP120_DSP_MB_GET_PAYLOAD(ndp120->mb_state.watermarkint);
        SYNTIANT_NDP120_PRINTF("LOWER: DSP FW PTR: 0x%x\n",
                ndp120->dsp_fw_state_addr);
        syntiant_ndp120_tiny_mbin_send(ndp120,
                SYNTIANT_NDP120_DSP_MB_H2D_ADX_UPPER);
        break;

    case SYNTIANT_NDP120_DSP_MB_D2H_ADX_UPPER:
        ndp120->dsp_fw_state_addr = (ndp120->dsp_fw_state_addr & 0xFFFF) |
            (uint32_t)(SYNTIANT_NDP120_DSP_MB_GET_PAYLOAD(
                        ndp120->mb_state.watermarkint) << 16);
        SYNTIANT_NDP120_PRINTF("UPPER: DSP FW PTR: 0x%x\n",
                ndp120->dsp_fw_state_addr);
        if (ndp120->dsp_fw_state_addr &&
              !(ndp120->pkg_load_flag & SYNTIANT_NDP120_DSP_LOADED)) {
            ndp120->pkg_load_flag |= SYNTIANT_NDP120_DSP_LOADED;
        }
        *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_IN;
        break;

    case SYNTIANT_NDP120_DSP_MB_D2H_RUNNING:
        *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_IN |
            SYNTIANT_NDP120_NOTIFICATION_DSP_RUNNING;
        SYNTIANT_NDP120_PRINTF("got DSP RUNNING, mailbox notify\n");
        break;

    case SYNTIANT_NDP120_DSP_MB_D2H_EXT:
        *notify = SYNTIANT_NDP120_NOTIFICATION_MAILBOX_IN;
        SYNTIANT_NDP120_PRINTF("got EXT, mailbox notify\n");
        break;

    default:
        SYNTIANT_NDP120_PRINTF("Invalid DSP mailbox value: 0x%X\n",
                ndp120->mb_state.watermarkint);
        break;
    }
error:
    return s;
}

enum {
    SYNTIANT_NDP120_CONFIG_CLOCK_OPTION_PLL          = 0,
    SYNTIANT_NDP120_CONFIG_CLOCK_OPTION_FLL         = 1,
    SYNTIANT_NDP120_CONFIG_CLOCK_OPTION_EXT         = 2
};

int
syntiant_ndp120_tiny_get_ext_freq(int clock_option, uint32_t *core_clock_freq)
{
    int s = SYNTIANT_NDP120_ERROR_NONE;

    if (clock_option == SYNTIANT_NDP120_CONFIG_CLOCK_OPTION_FLL) { /* for FLL */
        *core_clock_freq = ndp120_fll_presets[0].input_freq;
    } else if (clock_option == SYNTIANT_NDP120_CONFIG_CLOCK_OPTION_PLL) { /* for PLL */
        *core_clock_freq = ndp120_pll_presets[0].input_freq;
    } else {
        s = SYNTIANT_NDP120_ERROR_ARG;
    }
    return s;
}


int
syntiant_ndp120_tiny_clock_cfg(struct syntiant_ndp120_tiny_device_s *ndp,
    int *clock_options, uint32_t *mb_resp)
{
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint32_t size;
    uint32_t addr = SYNTIANT_NDP120_CMD_RAM;
    uint32_t cmd[2] = {SYNTIANT_NDP120_MAIN_CLK_CONFIG, 0};
    struct syntiant_ndp120_tiny_clk_config_data cfg;

    memset(&cfg, 0, sizeof(cfg));
    if (!clock_options) {
        s = SYNTIANT_NDP120_ERROR_ARG;
        goto error;
    }
    if (clock_options[0] == SYNTIANT_NDP120_CONFIG_CLOCK_OPTION_EXT) {
        cfg.src = SYNTIANT_NDP120_MAIN_CLK_SRC_EXT;
        cfg.ref_type = 0; /* ext clock uses clkpad as refsel */
        cfg.ref_freq = EXT_CLOCK_FREQ;
        cfg.core_freq = cfg.ref_freq;
        cfg.voltage = PLL_PRESET_OP_VOLTAGE_0p9;
    } else if (clock_options[0] == SYNTIANT_NDP120_CONFIG_CLOCK_OPTION_FLL) { /* use FLL */
        ndp120_fll_preset_t *fll_preset;
        cfg.src = SYNTIANT_NDP120_MAIN_CLK_SRC_FLL;
        if (clock_options[1]) { /* XTAL */
            cfg.ref_type = 1;
        } else { /* clkpad */
            cfg.ref_type = 0;
        }
        fll_preset = &ndp120_fll_presets[0];
        cfg.core_freq = fll_preset->output_freq;
        cfg.voltage = fll_preset->operating_voltage;
        cfg.ref_freq = FLL_CLOCK_FREQ;
    } else if (clock_options[0] == SYNTIANT_NDP120_CONFIG_CLOCK_OPTION_PLL) { /* use PLL */
        ndp120_pll_preset_t *pll_preset;
        cfg.src = SYNTIANT_NDP120_MAIN_CLK_SRC_PLL;
        if (clock_options[1]) { /* XTAL */
            cfg.ref_type = 1;
        } else { /* clkpad */
            cfg.ref_type = 0;
        }
        pll_preset = &ndp120_pll_presets[0];
        cfg.core_freq = pll_preset->output_freq;
        cfg.voltage = pll_preset->operating_voltage;
        cfg.ref_freq = pll_preset->input_freq;
        memcpy(cfg.preset_value, pll_preset->values, sizeof(cfg.preset_value));
    } else {
        s = SYNTIANT_NDP120_ERROR_ARG;
        goto error;
    }

    /* copy preset table only for PLL */
    if (cfg.src != SYNTIANT_NDP120_MAIN_CLK_SRC_PLL) {
        size = (uint32_t) offsetof(struct syntiant_ndp120_tiny_clk_config_data,
            preset_value);
    } else {
        size = sizeof(cfg);
    }
    cmd[1] = size;

    /* write cmd */
    s = syntiant_ndp120_tiny_write_block(ndp, SYNTIANT_NDP120_MCU_OP, addr,
            cmd, sizeof(cmd));
    if (s) goto error;
    addr += (uint32_t)sizeof(cmd);
    /* write payload */
    s = syntiant_ndp120_tiny_write_block(ndp, SYNTIANT_NDP120_MCU_OP, addr,
            &cfg, size);
    if (s) goto error;

    /* alerts DSP to start/stop execution */
    s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(
        ndp, SYNTIANT_NDP120_MB_MCU_CMD, mb_resp);

error:
    return s;
}

int
syntiant_ndp120_tiny_dsp_restart(struct syntiant_ndp120_tiny_device_s *ndp)
{
    int s;
    uint32_t cmd[2] = {SYNTIANT_NDP120_DSP_RESTART, 0};

    s = syntiant_ndp120_tiny_write_block(ndp, SYNTIANT_NDP120_MCU_OP,
        SYNTIANT_NDP120_CMD_RAM, cmd, sizeof(cmd));
    if (s) goto error;

    /* alerts DSP to start/stop execution */
    s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(
        ndp, SYNTIANT_NDP120_MB_MCU_CMD, NULL);

error:
    return s;
}

int
syntiant_ndp120_tiny_pdm_clock_exe_mode(
    struct syntiant_ndp120_tiny_device_s *ndp, uint8_t execution_mode)
{
    int s;
    uint32_t cmd_len[3] = {0};

    switch (execution_mode) {
    default:
        s = SYNTIANT_NDP120_ERROR_ARG;
        goto error;
    case SYNTIANT_NDP120_PDM_CLK_START_CLEAN:
        cmd_len[0] = SYNTIANT_NDP120_PDM_CLK_START;
        cmd_len[1] = sizeof(cmd_len[0]);
        break;
    case SYNTIANT_NDP120_PDM_CLK_START_RESUME:
        cmd_len[0] = SYNTIANT_NDP120_PDM_CLK_START;
        cmd_len[1] = sizeof(cmd_len[0]);
        cmd_len[2] = 1;
        break;
    case SYNTIANT_NDP120_PDM_CLK_START_PAUSE:
        cmd_len[0] = SYNTIANT_NDP120_PDM_CLK_STOP;
        memset(&ndp->match_consumer, 0, sizeof(ndp->match_consumer));
        break;
    }

    s = syntiant_ndp120_tiny_write_block(ndp, SYNTIANT_NDP120_MCU_OP,
        SYNTIANT_NDP120_CMD_RAM, cmd_len, sizeof(cmd_len));
    if (s) goto error;

    /* alerts DSP to start/stop execution */
    s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(
        ndp, SYNTIANT_NDP120_MB_MCU_CMD, NULL);

error:
    return s;
}

int
syntiant_ndp120_tiny_poll(struct syntiant_ndp120_tiny_device_s *ndp,
    uint32_t *notifications, int clear)
{
    int s;
    uint32_t notify = 0;
    uint8_t intsts;

    *notifications = 0;

    s = syntiant_ndp120_tiny_read(
        ndp, SYNTIANT_NDP120_SPI_OP, NDP120_SPI_INTSTS, &intsts);
    if (s) {
        SYNTIANT_NDP120_PRINTF("poll(read intsts) failed s=%d\n", s);
        goto error;
    }

    if (clear) {
        s = syntiant_ndp120_tiny_write(
            ndp, SYNTIANT_NDP120_SPI_OP, NDP120_SPI_INTSTS, intsts);
        if (s) {
            SYNTIANT_NDP120_PRINTF("poll(clear intsts) failed s=%d\n", s);
            /* error handling? */
        }
    }

    /* H2M response interrupt */
    if (intsts & NDP120_SPI_INTSTS_MBIN_INT(1)) {
        s = mbin_processor(ndp, &notify);
        if (s) {
            SYNTIANT_NDP120_PRINTF("poll(mbin processor) failed s=%d\n", s);
            goto error;
        }
        *notifications |= notify;
    }

    if (intsts & NDP120_SPI_INTSTS_DNN_INT(1)) {
        *notifications |= SYNTIANT_NDP120_NOTIFICATION_DNN;
    }

    /* M2H request interrupt */
    /* Match comes via mailbox out event */
    if (intsts & NDP120_SPI_INTSTS_MBOUT_INT(1)) {
#if SYNTIANT_NDP120_DEBUG_POLL
        SYNTIANT_NDP120_PRINTF("poll(): mbout interrupt\n");
#endif
        s = mbout_processor(ndp, &notify);
        if (s) {
            SYNTIANT_NDP120_PRINTF("poll(mbout processor) failed s=%d\n", s);
            goto error;
        }

#if SYNTIANT_NDP120_DEBUG_POLL || SYNTIANT_NDP120_DEBUG_POLL_MATCH
        if (notify & SYNTIANT_NDP120_NOTIFICATION_MATCH) {
            SYNTIANT_NDP120_PRINTF("poll(): match message from mbout\n");
        }
#endif
        *notifications |= notify;
    }

    /* M2H Frequency domain completion (filter bank) interrupt */
    if (intsts & NDP120_SPI_INTSTS_FEATURE_INT(1)) {
        *notifications |= SYNTIANT_NDP120_NOTIFICATION_FEATURE;
    }

    if (intsts & NDP120_SPI_INTSTS_AE_INT(1)) {
        *notifications |= SYNTIANT_NDP120_NOTIFICATION_ERROR;
    }

    if (intsts & NDP120_SPI_INTSTS_RF_INT(1)) {
        *notifications |= SYNTIANT_NDP120_NOTIFICATION_ERROR;
    }

    if (intsts & NDP120_SPI_INTSTS_WM_INT(1)) {
        s = dsp_mb_processor(ndp, &notify);
        if (s) {
            SYNTIANT_NDP120_PRINTF("poll(dsp mb processor) failed s=%d\n", s);
            goto error;
        }
#if SYNTIANT_NDP120_DEBUG_POLL
        SYNTIANT_NDP120_PRINTF(
            "updating DSP notifications with %#x s=%d\n", notify, s);
#endif
        *notifications |= notify;
    }
error:
    return s;
}

int
syntiant_ndp120_tiny_init(struct syntiant_ndp120_tiny_device_s *ndp,
        struct syntiant_ndp120_tiny_integration_interfaces_s *iif,
        int mode)
{
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint32_t data;
    uint8_t spi_data;
    uint8_t **dev, *devid, i;

    if (iif) {
        ndp->iif = iif;
    }
    if (mode == SYNTIANT_NDP_INIT_MODE_RESET) {
        /* full POR */
        s = ndp_spi_read(NDP120_SPI_CTL, &data);
        if (s) goto error;
        data = NDP120_SPI_CTL_PORSTN_MASK_INSERT(data, 0);
        s = ndp_spi_write(NDP120_SPI_CTL, data);
        if (s) goto error;
        while (1) {
            s = ndp_spi_read(NDP120_SPI_ID0, &spi_data);
            if (s) goto error;
            if (spi_data != 0xff) break;
        }
        /* Drive the interrupt line output active high interrupts */
        spi_data = NDP120_SPI_CFG_INTEN(1) | NDP120_SPI_CFG_INTNEG(0);
        s = ndp_spi_write(NDP120_SPI_CFG, spi_data);
    } else {
        /* NO_TOUCH case, don't do anything  */
    }

    /* Read the boot state from chip id0 */
    s = ndp_spi_read(NDP120_SPI_ID0, &data);
    if (s) goto error;

    ndp->boot_flag = NDP120_SPI_ID0_SERIAL_BOOT_EXTRACT(data) ?
        SYNTIANT_NDP120_SERIAL_BOOT : SYNTIANT_NDP120_HOST_BOOT;

    ndp->device_id = (uint8_t)data;
    for (i = SYNTIANT_NDP_NONE, dev = syntiant_sc2_device_id; *dev; dev++, i++) {
        for (devid = *dev; *devid; devid++) {
            if (ndp->device_id == *devid) {
                ndp->device_type = ++i;
                goto error;
            }
        }
    }

error:
    SYNTIANT_NDP120_PRINTF("device_id=%#x device_type=%u\n",
        ndp->device_id, ndp->device_type);
    ndp->init = 0;
    return s;
}

int
syntiant_ndp120_tiny_interrupts(
    struct syntiant_ndp120_tiny_device_s *ndp, int *causes)
{
    int s;
    uint8_t intctl, intctl_old;
    int cs = *causes;

    if (cs > SYNTIANT_NDP120_INTERRUPT_ALL
        && cs != SYNTIANT_NDP120_INTERRUPT_DEFAULT) {
        s = SYNTIANT_NDP120_ERROR_ARG;
        goto error;
    }

    s = syntiant_ndp120_tiny_read(
        ndp, SYNTIANT_NDP120_SPI_OP, NDP120_SPI_INTCTL, &intctl);
    if (s) goto error;

    intctl_old = intctl;

    if (cs >= 0) {
        if (cs == SYNTIANT_NDP120_INTERRUPT_DEFAULT) {
            intctl = NDP120_SPI_INTCTL_MBIN_INTEN(1)
                | NDP120_SPI_INTCTL_WM_INTEN(1)
                | NDP120_SPI_INTCTL_MBOUT_INTEN(1)
                | NDP120_SPI_INTCTL_AE_INTEN(1);
        } else {
            intctl = (uint8_t)(NDP120_SPI_INTCTL_MBIN_INTEN(!!(
                                   cs & SYNTIANT_NDP120_INTERRUPT_MAILBOX_IN))
                | NDP120_SPI_INTCTL_MBOUT_INTEN(
                    !!(cs & SYNTIANT_NDP120_INTERRUPT_MAILBOX_OUT))
                | NDP120_SPI_INTCTL_DNN_INTEN(
                    !!(cs & SYNTIANT_NDP120_INTERRUPT_DNN_FRAME))
                | NDP120_SPI_INTCTL_FEATURE_INTEN(
                    !!(cs & SYNTIANT_NDP120_INTERRUPT_FEATURE))
                | NDP120_SPI_INTCTL_AE_INTEN(
                    !!(cs & SYNTIANT_NDP120_INTERRUPT_ADDRESS_ERROR))
                | NDP120_SPI_INTCTL_WM_INTEN
                (!!(cs & SYNTIANT_NDP120_INTERRUPT_WATER_MARK))
                | NDP120_SPI_INTCTL_RF_INTEN(
                    !!(cs & SYNTIANT_NDP120_INTERRUPT_SPI_READ_FAILURE)));
        }
        s = syntiant_ndp120_tiny_write(
            ndp, SYNTIANT_NDP120_SPI_OP, NDP120_SPI_INTCTL, intctl);
        if (s) goto error;
    }

    cs = ((NDP120_SPI_INTCTL_MBIN_INTEN(1) & intctl_old)
                 ? SYNTIANT_NDP120_INTERRUPT_MAILBOX_IN
                 : 0)
        | ((NDP120_SPI_INTCTL_MBOUT_INTEN(1) & intctl_old)
                ? SYNTIANT_NDP120_INTERRUPT_MAILBOX_OUT
                : 0)
        | ((NDP120_SPI_INTCTL_DNN_INTEN(1) & intctl_old)
                ? SYNTIANT_NDP120_INTERRUPT_DNN_FRAME
                : 0)
        | ((NDP120_SPI_INTCTL_FEATURE_INTEN(1) & intctl_old)
                ? SYNTIANT_NDP120_INTERRUPT_FEATURE
                : 0)
        | ((NDP120_SPI_INTCTL_AE_INTEN(1) & intctl_old)
                ? SYNTIANT_NDP120_INTERRUPT_ADDRESS_ERROR
                : 0)
        | ((NDP120_SPI_INTCTL_WM_INTEN(1) & intctl_old)
                ? SYNTIANT_NDP120_INTERRUPT_WATER_MARK
                : 0)
        | ((NDP120_SPI_INTCTL_RF_INTEN(1) & intctl_old)
                ? SYNTIANT_NDP120_INTERRUPT_SPI_READ_FAILURE
                : 0);

    *causes = cs;

error:
    return s;
}

static int
syntiant_ndp120_tiny_load_other_pkgs(
    struct syntiant_ndp120_tiny_device_s *ndp120, uint8_t *chunk_ptr,
    uint32_t chunk_len)
{
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint8_t data, error_code, is_error;
    uint32_t msg, window_size, bytes_to_copy, sum = 0;
    struct syntiant_ndp120_tiny_dl_state_s *dl_state = &ndp120->dl_state;

    window_size = dl_state->window_upper - dl_state->window_lower;

    dl_state->mode = SYNTIANT_NDP120_PKG_LOADER_MODE_IN_PROGRESS;
    /* MCU requires writes to be on 4-byte boundaries.  Oddball chunk sizes
     * remainders are stored in dl_state->remainder */
    if (dl_state->remainder_len > 0) {
        bytes_to_copy = (unsigned int)MIN(
            (int)(4 - dl_state->remainder_len), (int)chunk_len);
        if (bytes_to_copy > 0) {
            /* try to bring remainder up to 4 bytes */
            memcpy(dl_state->remainder + dl_state->remainder_len, chunk_ptr,
                bytes_to_copy);
            chunk_len -= bytes_to_copy;
            chunk_ptr += bytes_to_copy;
            dl_state->remainder_len += bytes_to_copy;
        }
        /* cover edge case where chunks are < 4 bytes */
        if (dl_state->remainder_len == 4) {
            s = syntiant_ndp120_tiny_write_block(ndp120, SYNTIANT_NDP120_MCU_OP,
                dl_state->window_lower + dl_state->window_idx,
                dl_state->remainder, 4);
            dl_state->window_idx += 4;
            dl_state->remainder_len = 0;
        }
    }
    while (chunk_len) {
        if (chunk_len < 4) {
            memcpy(dl_state->remainder, chunk_ptr, chunk_len);
            dl_state->remainder_len = chunk_len;
            chunk_len = 0;
            break;
        }
        bytes_to_copy = MIN(window_size - dl_state->window_idx, chunk_len);
        bytes_to_copy -= bytes_to_copy % 4; /* trim to 4 byte boundary */

        /* this is sometimes 0 for odd chunk sizes*/
        if (bytes_to_copy > 0) {
            syntiant_ndp120_tiny_write_block(ndp120, SYNTIANT_NDP120_MCU_OP,
                dl_state->window_lower + dl_state->window_idx, chunk_ptr,
                bytes_to_copy);
            chunk_len -= bytes_to_copy;
            chunk_ptr += bytes_to_copy;
            sum += bytes_to_copy;
            dl_state->window_idx += bytes_to_copy;
            SYNTIANT_NDP120_PRINTF("total: %d\n", sum);
        }

        if (dl_state->window_idx == bytes_to_copy
            || dl_state->mode == PACKAGE_MODE_DONE) {
            s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(
                ndp120, SYNTIANT_NDP120_MB_MCU_CONT, &msg);
            if (s) goto error;
            dl_state->window_idx = 0;
            data = (uint8_t)msg;

            if (data != SYNTIANT_NDP120_MB_MCU_NOP) {
                SYNTIANT_NDP120_PRINTF(
                    "received 0x%02X instead of expected NOP\n", data);
                s = SYNTIANT_NDP120_ERROR_FAIL;
                goto error;
            }
            mbout_recv(ndp120, &data);
            mbout_send_resp(ndp120, SYNTIANT_NDP120_MB_MCU_NOP);

            is_error = mb_resp_is_error(data, &error_code);
            if (data == SYNTIANT_NDP120_MB_MCU_LOAD_DONE) {
                SYNTIANT_NDP120_PRINTF("load done\n");
                dl_state->mode = SYNTIANT_NDP120_PKG_LOADER_MODE_COMPLETE;
                s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(
                    ndp120, SYNTIANT_NDP120_MB_MCU_CONT, NULL);
                if (s) goto error;
            } else if (data == SYNTIANT_NDP120_MB_OTHER_LOAD_DONE) {
                SYNTIANT_NDP120_PRINTF("load done for other packages\n");
                dl_state->mode = SYNTIANT_NDP120_PKG_LOADER_MODE_COMPLETE;
            } else if (is_error) {
                char buf[32] = "ERROR";
#if SYNTIANT_NDP120_DEBUG
                syntiant_ndp120_mcu_mb_error_decoder(
                    error_code, buf, sizeof(buf));
#endif
                SYNTIANT_NDP120_PRINTF(
                    "load error: %s (0x%02X)\n", buf, error_code);
                s = SYNTIANT_NDP120_ERROR_FAIL;
                goto error;
            } else if (data != SYNTIANT_NDP120_MB_MCU_CONT) {
                SYNTIANT_NDP120_PRINTF("Received 0x%02X, aborting\n", data);
                s = SYNTIANT_NDP120_ERROR_FAIL;
                goto error;
            }
        } /* if ("window full") */
    }     /* while (chunk_len) */
error:
    if (dl_state->mode == SYNTIANT_NDP120_PKG_LOADER_MODE_COMPLETE) {
        if (!(ndp120->pkg_load_flag & SYNTIANT_NDP120_MCU_LOADED)) {
            ndp120->pkg_load_flag |= SYNTIANT_NDP120_MCU_LOADED;
#if SYNTIANT_NDP120_DEBUG
            s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(
                ndp120, SYNTIANT_NDP120_MB_MCU_MIADDR, NULL);
            if (s) goto error;
#endif
        } else if (!(ndp120->pkg_load_flag & SYNTIANT_NDP120_DSP_LOADED)) {
            ndp120->pkg_load_flag |= SYNTIANT_NDP120_DSP_LOADED;
            SYNTIANT_NDP120_PRINTF("DSP loaded\n");
        }
    } else {
        s = SYNTIANT_NDP120_ERROR_MORE;
    }
    return s;
}

static int
syntiant_ndp120_load_via_bootloader(struct syntiant_ndp120_tiny_device_s *ndp,
    uint8_t *chunk_ptr, uint32_t chunk_len)
{
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint8_t data;
    uint32_t msg;
    int i;
    struct syntiant_ndp120_tiny_dl_state_s *dl_state = &ndp->dl_state;

    if (dl_state->mode == SYNTIANT_NDP120_PKG_LOADER_MODE_START) {
        /* Step 0: Reset the chip, wait for BOOTING on MBOUT */

        /* clear interrupts */
        ndp_spi_write(NDP120_SPI_INTSTS, 0xff);

        /* enable interrupts */
        s = ndp_spi_read(NDP120_SPI_CFG, &data);
        if (s) goto error;

        data = NDP120_SPI_CFG_INTEN_INSERT(data, 1);
        s = ndp_spi_write(NDP120_SPI_CFG, data);
        if (s) goto error;

        /* enable MBIN, MBOUT, WM, and AE interrupts */
        data = 0;
        data = NDP120_SPI_INTCTL_MBIN_INTEN_INSERT(data, 1);
        data = NDP120_SPI_INTCTL_MBOUT_INTEN_INSERT(data, 1);
        data = NDP120_SPI_INTCTL_AE_INTEN_INSERT(data, 1);
        ndp_spi_write(NDP120_SPI_INTCTL, data);
        mailbox_reset_state(ndp);

        /* Reset the chip */
        data = 0;
        data = NDP120_SPI_CTL_RESETN_INSERT(data, 0);
        data = NDP120_SPI_CTL_FLASHCTL_INSERT(
            data, NDP120_SPI_CTL_FLASHCTL_DISABLE);
        data = NDP120_SPI_CTL_PORSTN_INSERT(data, 1);
        data = NDP120_SPI_CTL_CLKEN_INSERT(data, 1);
        ndp_spi_write(NDP120_SPI_CTL, data);

        /* deassert reset */
        data = NDP120_SPI_CTL_RESETN_INSERT(data, 1);
        ndp_spi_write(NDP120_SPI_CTL, data);

        /* Should receive BOOTING now*/
        mbout_recv(ndp, &data);

        if (data != SYNTIANT_NDP120_MB_MCU_BOOTING) {
            SYNTIANT_NDP120_PRINTF(
                "expecting BOOTING on mbout; received: 0x%02X", data);
            s = SYNTIANT_NDP120_ERROR_FAIL;
            goto error;
        }
        /* Step 1: Handshake with bootloader */
        /* Handshake 0 */

        for (i = 0; i < 2; ++i) {
            s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(
                ndp, SYNTIANT_NDP120_MB_MCU_NOP, NULL);
            if (s) goto error;
        }
        SYNTIANT_NDP120_PRINTF("handhake complete\n");
        /* Step 2:
               send LOAD
               await CONT
        */
        s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(
            ndp, SYNTIANT_NDP120_MB_MCU_LOAD, &msg);
        if (s) goto error;
        data = (uint8_t)msg;
        if (data != SYNTIANT_NDP120_MB_MCU_CONT) {
            s = SYNTIANT_NDP120_ERROR_FAIL;
            goto error;
        }

        /* Step 3: get "load area" from
                addrprotlo_0 and
                addrprothi_0
        */
        s = syntiant_ndp120_tiny_load_other_pkgs(ndp, chunk_ptr, chunk_len);
    } else if (dl_state->mode == SYNTIANT_NDP120_PKG_LOADER_MODE_IN_PROGRESS) {
        s = syntiant_ndp120_tiny_load_other_pkgs(ndp, chunk_ptr, chunk_len);
    } else {
        s = SYNTIANT_NDP120_ERROR_FAIL;
    }

error:
    return s;
}

static int
syntiant_ndp120_tiny_load_package(struct syntiant_ndp120_tiny_device_s *ndp,
    uint8_t *chunk, uint32_t chunk_len)
{
    int s = SYNTIANT_NDP120_ERROR_NONE;

    if (!(ndp->pkg_load_flag & SYNTIANT_NDP120_MCU_LOADED)) {
        /* load MCU fw via boot loader */
        SYNTIANT_NDP120_PRINTF("load MCU fw via boot loader");
        s = syntiant_ndp120_load_via_bootloader(ndp, chunk, chunk_len);
    } else {
        /* load other synpkgs via MCU fw */
        if (!(ndp->pkg_load_flag & SYNTIANT_NDP120_DSP_LOADED)) {
            SYNTIANT_NDP120_PRINTF("load DSP fw via MCU fw\n");
        } else {
            SYNTIANT_NDP120_PRINTF("load NN synpkg via MCU fw\n");
        }
        /* continue load process */
        s = syntiant_ndp120_tiny_load_other_pkgs(ndp, chunk, chunk_len);
    }
    return s;
}

int
syntiant_ndp120_tiny_load(
    struct syntiant_ndp120_tiny_device_s *ndp, void *chunk, unsigned int len)
{
    uint8_t data;
    uint32_t msg;
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint8_t cmd = 0;
    struct syntiant_ndp120_tiny_dl_state_s *dl_state = &ndp->dl_state;

    SYNTIANT_NDP120_PRINTF("syntiant_ndp120_load: %d bytes mode:%d\n", len,
            dl_state->mode);
    if (len == 0) {
        if (ndp->pkg_load_flag & SYNTIANT_NDP120_MCU_LOADED &&
            !(ndp->pkg_load_flag & SYNTIANT_NDP120_DSP_LOADED)) {
            cmd = SYNTIANT_NDP120_MB_LOAD_DSP;
        } else if (ndp->pkg_load_flag & SYNTIANT_NDP120_DSP_LOADED) {
            cmd = SYNTIANT_NDP120_MB_LOAD_NN;
        }
        if (cmd) {
            s = syntiant_ndp120_tiny_mb_cmd(ndp, cmd, &msg);
            if (s) {
                SYNTIANT_NDP120_PRINTF("Error from MB: 0x%x\n", s);
                goto error;
            }
            data = (uint8_t)msg;
            if (data != SYNTIANT_NDP120_MB_MCU_CONT) {
                s = SYNTIANT_NDP120_ERROR_FAIL;
                SYNTIANT_NDP120_PRINTF(
                    "Error, didn't receive cont, got: %x\n", data);
                goto error;
            }
        }
        memset(dl_state, 0, sizeof(*dl_state));
        if (SYNTIANT_NDP120_MCU_LOADED & ndp->pkg_load_flag) {
            dl_state->window_lower = SYNTIANT_NDP120_DL_WINDOW_LOWER;
            dl_state->window_upper = SYNTIANT_NDP120_DL_WINDOW_UPPER;
        } else {
            dl_state->window_lower = SYNTIANT_NDP120_BL_WINDOW_LOWER;
            dl_state->window_upper = SYNTIANT_NDP120_BL_WINDOW_UPPER;
        }
        dl_state->mode = SYNTIANT_NDP120_PKG_LOADER_MODE_START;
        s = SYNTIANT_NDP120_ERROR_MORE;
    } else {
        return syntiant_ndp120_tiny_load_package(ndp, (uint8_t*) chunk, len);
    }
error:
    return s;
}

/* Send data */
int
syntiant_ndp120_tiny_send_data(struct syntiant_ndp120_tiny_device_s *ndp,
    uint8_t *data, unsigned int len, int type, uint32_t offset)
{
    int s;

    /* boundary check for DNN static feature */
    if ((type == SYNTIANT_NDP120_SEND_DATA_TYPE_STREAMING) && offset) {
        s = SYNTIANT_NDP120_ERROR_ARG;
        goto error;
    }
#if SYNTIANT_NDP120_DEBUG
    SYNTIANT_NDP120_PRINTF("ndp120_send_data, sending %d bytes", len);
#endif

    s = syntiant_ndp120_tiny_write_block(
        ndp, SYNTIANT_NDP120_SPI_OP, NDP120_SPI_SAMPLE, data, len);

error:
    return s;
}
/**
 * Deprecation Warning: This function will be removed in SDK v97 release.
 * Instead, use syntiant_ndp120_tiny_get_match_result() API.
 */
int syntiant_ndp120_tiny_get_match_summary(
    struct syntiant_ndp120_tiny_device_s *ndp, uint32_t *summary)
{
    uint32_t cons;
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint32_t network_id = ndp->last_network_id;
    struct syntiant_ndp120_tiny_match_data match;
    *summary = 0;

    s = ndp_spi_read(NDP120_SPI_MATCHSTS, summary);
    if (s) goto error;
    if (!*summary) {
        /* match per frame interrupt, not an event match */
        goto error;
    }
    cons = ndp->match_consumer[network_id];
    if (ndp->match_producer[network_id] != cons) {
        /* read from Open RAM */
        s = syntiant_ndp120_tiny_read_block(ndp, SYNTIANT_NDP120_MCU_OP,
            SYNTIANT_NDP120_OPEN_RAM_MATCH_RESULTS, &match, sizeof(match));
        if (s) goto error;
        ndp->tankptr_match = match.tank_ptr & ~0x3U;
        ndp->match_frame_count = match.match_frame_count;
        *summary = match.summary;
        SYNTIANT_NDP120_MATCH_PRINTF(
                "summary, 0x%x, tankptr at match: 0x%x\n",
                 match.summary, match.tank_ptr);
        cons++;
        cons = cons == SYNTIANT_NDP120_MATCH_RING_SIZE ? 0 : cons;
        ndp->match_consumer[network_id] = cons;
    }

error:
    return s;
}

int syntiant_ndp120_tiny_get_match_result(
    struct syntiant_ndp120_tiny_device_s *ndp,
    struct syntiant_ndp120_tiny_match_data *match) {

    uint32_t cons;
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint32_t network_id = ndp->last_network_id;
    uint32_t match_sts;
    uint8_t event_type;

    s = ndp_spi_read(NDP120_SPI_MATCHSTS, &match_sts);
    if (s) goto error;
    if (!match_sts) {
        /* match per frame interrupt, not an event match */
        goto error;
    }
    cons = ndp->match_consumer[network_id];
    if (ndp->match_producer[network_id] != cons) {
        /* read from Open RAM */
        s = syntiant_ndp120_tiny_read_block(ndp, SYNTIANT_NDP120_MCU_OP,
            SYNTIANT_NDP120_OPEN_RAM_MATCH_RESULTS,
	    match, sizeof(struct syntiant_ndp120_tiny_match_data));
        if (s) goto error;
        ndp->tankptr_match = match->tank_ptr & ~0x3U;
        ndp->match_frame_count = match->match_frame_count;
        SYNTIANT_NDP120_MATCH_PRINTF(
                "summary, 0x%x, tankptr at match: 0x%x\n",
                 match->summary, match->tank_ptr);
        cons++;
        cons = cons == SYNTIANT_NDP120_MATCH_RING_SIZE ? 0 : cons;
        ndp->match_consumer[network_id] = cons;
    }
    event_type = (uint8_t)(match->summary >> NDP120_MATCH_MISC_EVENT_SHIFT);
    if (event_type == NDP120_SENSOR_DATA_READY_EVENT) {
        s = syntiant_ndp120_tiny_read_block(ndp, SYNTIANT_NDP120_MCU_OP,
            SYNTIANT_NDP120_OPEN_RAM_RESULTS, &ndp->u.meta_bytes,
            sizeof(ndp->u.meta_bytes));
    }

error:
    return s;
}

int
syntiant_ndp120_tiny_read_block(struct syntiant_ndp120_tiny_device_s *ndp,
    int mcu, uint32_t address, void *value, unsigned int count)
{
    int s;

    do {
        uint32_t max_bytes_allowed = 0;
        uint32_t bytes_to_read;

        if (address % SYNTIANT_MAX_BLOCK_SIZE == 0) {
            max_bytes_allowed = SYNTIANT_MAX_BLOCK_SIZE;
        } else {
            max_bytes_allowed
                = SYNTIANT_MAX_BLOCK_SIZE - (address % SYNTIANT_MAX_BLOCK_SIZE);
        }

        bytes_to_read = (count > max_bytes_allowed) ? max_bytes_allowed : count;

        s = ndp->iif->transfer(
            ndp->iif->d, mcu, address, NULL, value, bytes_to_read);
        if (s) goto error;

        address += bytes_to_read;
        value = (uint8_t *)value + bytes_to_read;
        count -= bytes_to_read;

    } while (count != 0);

error:
    return s;
}

int
syntiant_ndp120_tiny_write_block(struct syntiant_ndp120_tiny_device_s *ndp,
    int mcu, uint32_t address, void *value, unsigned int count)
{
    int s;
    do {
        uint32_t max_bytes_allowed = 0;
        uint32_t bytes_to_write;
        if (address % SYNTIANT_MAX_BLOCK_SIZE == 0) {
            max_bytes_allowed = SYNTIANT_MAX_BLOCK_SIZE;
        } else {
            max_bytes_allowed
                = SYNTIANT_MAX_BLOCK_SIZE - (address % SYNTIANT_MAX_BLOCK_SIZE);
        }

        bytes_to_write
            = (count > max_bytes_allowed) ? max_bytes_allowed : count;

        s = ndp->iif->transfer(
            ndp->iif->d, mcu, address, value, NULL, bytes_to_write);

        if (s) goto error;

        if (mcu || address != NDP120_SPI_SAMPLE) {
            address += bytes_to_write;
        }
        value = (uint8_t *)value + bytes_to_write;
        count -= bytes_to_write;

    } while (count != 0);

error:
    return s;
}

int syntiant_ndp120_tiny_enable_disable_sensor(
    struct syntiant_ndp120_tiny_device_s *ndp, uint8_t sensor_id,
    int enable)
{
    int s = SYNTIANT_NDP120_ERROR_NONE;
    unsigned int ifc_type, ifc_addr;
    uint8_t out[2];

    switch (sensor_id) {
        case SYNTIANT_NDP120_SENSOR_ID_NONE:
            break;

        case SYNTIANT_NDP120_SENSOR_ID_ICM42670:
            if (enable) {
                s = syntiant_ndp120_tiny_dsp_restart(ndp);
                if (s) {
                    goto error;
                }
                /* reset the last ptr */
                ndp->last_ptr = 0;
            } else {
                ifc_type = SYNTIANT_NDP120_SERIAL_INTERFACE_SPI1;
                ifc_addr = 9;
                out[0] = 0x1f;
                out[1] = 0x00;
                s = syntiant_ndp120_tiny_serial_transfer(
                    ndp, ifc_type, ifc_addr, out, sizeof(out), NULL, 0, 0);
            }
            break;

        default:
            s = SYNTIANT_NDP120_ERROR_ARG;
            break;
    }

error:
    return s;
}

int
syntiant_ndp120_tiny_extract_data(struct syntiant_ndp120_tiny_device_s *ndp,
    uint8_t *data, unsigned int *lenp, int extract_from_buffer)
{
    uint32_t frames;
    uint32_t buf_start, prod_ptr, buf_end;
    uint32_t adx, read_start_ptr;
    uint32_t read_len_bytes, bytes_available;
    uint32_t sample_size;
    size_t offset;
    uint32_t addr = SYNTIANT_NDP120_CMD_RAM + sizeof(uint32_t);
    int s = SYNTIANT_NDP120_ERROR_NONE;
    if (extract_from_buffer) {
        sample_size = ndp->u.buffer_metadata.sample_size;
        buf_start = ndp->u.buffer_metadata.buffer_start;
        buf_end = buf_start + ndp->u.buffer_metadata.buffer_size_bytes;
        s = syntiant_ndp120_tiny_read_block(ndp, SYNTIANT_NDP120_MCU_OP,
            ndp->u.buffer_metadata.prod_ptr_addr, &prod_ptr, sizeof(prod_ptr));
        if (s) {
            goto error;
        }
        *lenp = *lenp / sample_size * sample_size;
        if (!ndp->last_ptr) {
            /* extract from the newest sample*/
            if (prod_ptr == buf_start) {
                ndp->last_ptr = buf_end - sample_size;
            } else {
                ndp->last_ptr = prod_ptr - sample_size;
            }
        }
        bytes_available = (uint32_t)BUFDELTA(prod_ptr, ndp->last_ptr,
                            ndp->u.buffer_metadata.buffer_size_bytes);
        if (!bytes_available) {
            s = SYNTIANT_NDP120_ERROR_DATA_REREAD;
            goto error;
        }
        *lenp = MIN(*lenp, bytes_available);
        read_len_bytes = *lenp;
        read_start_ptr = ndp->last_ptr;
        adx = read_start_ptr;
        offset = 0;

        if (adx + read_len_bytes > buf_end) {
            read_len_bytes = buf_end - adx;
            s = syntiant_ndp120_tiny_read_block(ndp, SYNTIANT_NDP120_MCU_OP,
                adx, data, read_len_bytes);
            if (s) goto error;
            offset = read_len_bytes;
            read_len_bytes = *lenp - read_len_bytes;
            adx = buf_start;
        }
        s = syntiant_ndp120_tiny_read_block(ndp, SYNTIANT_NDP120_MCU_OP, adx,
            data + offset, read_len_bytes);
        if (s) goto error;

        ndp->last_ptr = adx + read_len_bytes;

    } else {
        /* reading frames dumped count */
        s = syntiant_ndp120_tiny_read_block(
            ndp, SYNTIANT_NDP120_MCU_OP, SYNTIANT_NDP120_CMD_RAM, &frames,
            sizeof(frames));
        if (s) goto error;
        *lenp = frames * (*lenp);

        /* reading the audio data */
        s = syntiant_ndp120_tiny_read_block(ndp, SYNTIANT_NDP120_MCU_OP, addr,
            data, *lenp);
        if (s) goto error;
    }
error:
    return s;
}

int syntiant_ndp120_tiny_get_audio_chunk_size(struct
    syntiant_ndp120_tiny_device_s *ndp, uint32_t *audio_chunk_size)
{
    int s0;
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint32_t cmd[2] = {SYNTIANT_NDP120_GET_AUDIO_CHUNK_SIZE, 0};
    if (ndp->iif->sync) {
        s = (ndp->iif->sync)(ndp->iif->d);
        if (s) {
            SYNTIANT_NDP120_PRINTF(
                "Error in syntiant_ndp120_tiny_get_audio_chunk_size\n");
            return s;
        }
    }
    if (audio_chunk_size) {
        /* write cmd */
        s = syntiant_ndp120_tiny_write_block(ndp, 1,
            SYNTIANT_NDP120_CMD_RAM, cmd, sizeof(cmd));
        if (s) goto error;

        /* send command */
        s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(ndp,
            SYNTIANT_NDP120_MB_MCU_CMD, NULL);
        if (s) goto error;

        s = syntiant_ndp120_tiny_read_block(ndp, 1,
            SYNTIANT_NDP120_OPEN_RAM_RESULTS, audio_chunk_size,
            sizeof(*audio_chunk_size));
        if (s) goto error;
    }
error:
    if (ndp->iif->unsync) {
        s0 = (ndp->iif->unsync)(ndp->iif->d);
        s = s ? s : s0;
    }
    return s;
}

int syntiant_ndp120_tiny_get_recording_metadata(struct
    syntiant_ndp120_tiny_device_s *ndp, uint32_t *sample_size, 
    int get_from, uint32_t notify)
{
    int s0;
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint32_t cmd[3] = {SYNTIANT_NDP120_GET_RECORDING_METADATA, 1, 0};
    if (ndp->iif->sync) {
        s = (ndp->iif->sync)(ndp->iif->d);
        if (s) {
            SYNTIANT_NDP120_PRINTF(
                "Error in syntiant_ndp120_tiny_get_audio_chunk_size\n");
            return s;
        }
    }
    if (get_from == SYNTIANT_NDP120_GET_FROM_ILIB) {
        /* get from tiny ilib struct */
        *sample_size = ndp->u.buffer_metadata.sample_size;
        goto error;
    }
    
    cmd[2] = notify;
    /* write cmd */
    s = syntiant_ndp120_tiny_write_block(ndp, 1,
        SYNTIANT_NDP120_CMD_RAM, cmd, sizeof(cmd));
    if (s) goto error;

    /* send command */
    s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(ndp,
        SYNTIANT_NDP120_MB_MCU_CMD, NULL);
    if (s) goto error;

    s = syntiant_ndp120_tiny_read_block(ndp, 1,
        SYNTIANT_NDP120_CMD_RAM, &ndp->u.meta_bytes,
        sizeof(ndp->u.meta_bytes));
    if (s) goto error;

    *sample_size = ndp->u.buffer_metadata.sample_size;
    ndp->last_ptr = 0;

error:
    if (ndp->iif->unsync) {
        s0 = (ndp->iif->unsync)(ndp->iif->d);
        s = s ? s : s0;
    }
    return s;
}

int syntiant_ndp120_tiny_audio_extract_start(struct
    syntiant_ndp120_tiny_device_s *ndp, uint32_t extract_from)
{
    int s0;
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint32_t cmd[3] = {SYNTIANT_NDP120_EXTRACT_START, sizeof(uint32_t), 0};

    if (ndp->iif->sync) {
        s = (ndp->iif->sync)(ndp->iif->d);
        if (s) {
            SYNTIANT_NDP120_PRINTF(
                "Error in syntiant_ndp120_tiny_audio_extract_start\n");
            return s;
        }
    }
    cmd[2] = extract_from;

    /* write cmd */
    s = syntiant_ndp120_tiny_write_block(ndp, 1,
        SYNTIANT_NDP120_CMD_RAM, cmd, sizeof(cmd));
        if (s) goto error;

    /* send command */
    s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(ndp,
        SYNTIANT_NDP120_MB_MCU_CMD, NULL);
    if (s) goto error;

error:
    if (ndp->iif->unsync) {
        s0 = (ndp->iif->unsync)(ndp->iif->d);
        s = s ? s : s0;
    }
    return s;
}

int syntiant_ndp120_tiny_audio_extract_stop(struct
    syntiant_ndp120_tiny_device_s *ndp)
{
    int s0;
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint32_t cmd[2] = {SYNTIANT_NDP120_EXTRACT_STOP, 0};

    if (ndp->iif->sync) {
        s = (ndp->iif->sync)(ndp->iif->d);
        if (s) {
            SYNTIANT_NDP120_PRINTF(
                "Error in syntiant_ndp120_tiny_audio_extract_stop\n");
            return s;
        }
    }

    /* write cmd */
    s = syntiant_ndp120_tiny_write_block(ndp, 1,
        SYNTIANT_NDP120_CMD_RAM, cmd, sizeof(cmd));
        if (s) goto error;

    /* send command */
    s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(ndp,
        SYNTIANT_NDP120_MB_MCU_CMD, NULL);
    if (s) goto error;

error:
    if (ndp->iif->unsync) {
        s0 = (ndp->iif->unsync)(ndp->iif->d);
        s = s ? s : s0;
    }
    return s;
}

int syntiant_ndp120_tiny_spi_direct_config(struct
    syntiant_ndp120_tiny_device_s *ndp, uint32_t threshold_bytes,
    uint32_t *fifo_threshold_value)
{
    uint8_t data_8;
    uint32_t msg = 0;
    int s0;
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint32_t cmd[3] = {SYNTIANT_NDP120_SET_SPI_DIRECT, sizeof(threshold_bytes),
                        0};
    if (ndp->iif->sync) {
        s = (ndp->iif->sync)(ndp->iif->d);
        if (s) {
            SYNTIANT_NDP120_PRINTF(
                "Error in syntiant_ndp120_tiny_spi_direct_config\n");
            return s;
        }
    }
    cmd[2] = threshold_bytes;
    /* write cmd, length of the payload and payload */
    s = syntiant_ndp120_tiny_write_block(ndp, SYNTIANT_NDP120_MCU_OP,
        SYNTIANT_NDP120_CMD_RAM, cmd, sizeof(cmd));
    if (s) goto error;

    /* send command */
    s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(ndp,
        SYNTIANT_NDP120_MB_MCU_CMD, &msg);
    if (s) {
      goto error;
    }

    data_8 = (uint8_t)msg;
    if (data_8 != SYNTIANT_NDP120_MB_MCU_NOP) {
        SYNTIANT_NDP120_PRINTF("Error in configuring the fifo threshold register\n");
        s = SYNTIANT_NDP120_ERROR_FAIL;
        goto error;
    }
    s = syntiant_ndp120_tiny_read_block(ndp, 1,
        SYNTIANT_NDP120_OPEN_RAM_RESULTS, fifo_threshold_value,
        sizeof(uint32_t));
    if (s) {
        SYNTIANT_NDP120_PRINTF("Error reading the fifo threshold value from open ram\n");
        goto error;
    }
    /*enabling watermark interrupt*/
    s = ndp_spi_read(NDP120_SPI_INTCTL, &data_8);
    if(s) goto error;
    data_8 = NDP120_SPI_INTCTL_WM_INTEN_INSERT(data_8, 1);
    ndp_spi_write(NDP120_SPI_INTCTL, data_8);

error:
    if (ndp->iif->unsync) {
        s0 = (ndp->iif->unsync)(ndp->iif->d);
        s = s ? s : s0;
    }
    return s;
}

int
syntiant_ndp120_tiny_switch_dnn_flow(struct syntiant_ndp120_tiny_device_s *ndp,
    uint32_t flow_set_id, uint32_t *input_mode)
{
    uint8_t data_8;
    int s0;
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint32_t cmd[3] = {SYNTIANT_NDP120_SWITCH_FLOW_SET_ID,
            sizeof(flow_set_id), 0};
    if (ndp->iif->sync) {
        s = (ndp->iif->sync)(ndp->iif->d);
        if (s) {
            SYNTIANT_NDP120_PRINTF(
                "Error in syntiant_ndp120_tiny_switch_dnn_flow\n");
            return s;
        }
    }
    cmd[2] = flow_set_id;

    /* write cmd, length of the payload and payload */
    s = syntiant_ndp120_tiny_write_block(ndp, SYNTIANT_NDP120_MCU_OP,
            SYNTIANT_NDP120_CMD_RAM, cmd, sizeof(cmd));
    if (s) goto error;

    s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(ndp,
        SYNTIANT_NDP120_MB_MCU_CMD, NULL);
    if (s) {
      SYNTIANT_NDP120_PRINTF(
          "Error in switching dnn flow\n");
      goto error;
    }

    /* read input_mode */
    s = syntiant_ndp120_tiny_read_block(ndp,SYNTIANT_NDP120_MCU_OP,
        SYNTIANT_NDP120_OPEN_RAM_RESULTS, input_mode,
        sizeof(*input_mode));
    if (s) goto error;

    if (*input_mode == SYNTIANT_NDP120_TINY_INPUT_CONFIG_SPI) {
        /*enabling watermark interrupt*/
        s = ndp_spi_read(NDP120_SPI_INTCTL, &data_8);
        if (s) goto error;
        data_8 = NDP120_SPI_INTCTL_WM_INTEN_INSERT(data_8, 1);
        s = ndp_spi_write(NDP120_SPI_INTCTL, data_8);
        if (s) goto error;
    }

error:
    if (ndp->iif->unsync) {
        s0 = (ndp->iif->unsync)(ndp->iif->d);
        s = s ? s : s0;
    }
    return s;
}


int
syntiant_ndp120_tiny_get_info(struct syntiant_ndp120_tiny_device_s *ndp,
    struct syntiant_ndp120_tiny_info *idata)
{
    int s0;
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint32_t ver_len = NDP120_MCU_PBI_VER_MAX_LEN;
    uint32_t address = SYNTIANT_NDP120_OPEN_RAM_RESULTS;
    uint32_t cmd[2] = { SYNTIANT_NDP120_GET_INFO, 0 };
    uint32_t sf_len, sensor_info_len;

    if (ndp->iif->sync) {
        s = (ndp->iif->sync)(ndp->iif->d);
        if (s) {
            SYNTIANT_NDP120_PRINTF("Error in syntiant_ndp120_tiny_get_info\n");
            return s;
        }
    }
    if (idata && idata->fw_version && idata->dsp_fw_version
        && idata->pkg_version && idata->labels) {

        /* write MB cmd */
        s = syntiant_ndp120_tiny_write_block(ndp, SYNTIANT_NDP120_MCU_OP,
            SYNTIANT_NDP120_CMD_RAM, cmd, sizeof(cmd));
        if (s) goto error;

        /* send command */
        s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(
            ndp, SYNTIANT_NDP120_MB_MCU_CMD, NULL);
        if (s) goto error;
        /* read MCU returned data */
        s = syntiant_ndp120_tiny_read(
            ndp, SYNTIANT_NDP120_MCU_OP, address, &idata->fw_version_len);
        if (s) goto error;
        if (idata->fw_version_len > NDP120_MCU_FW_VER_MAX_LEN) {
            SYNTIANT_NDP120_PRINTF(
                "Firmware sent invalid FW version length:%d bytes\n",
                idata->fw_version_len);
        }

        address = SYNTIANT_NDP120_MCU_DSP_FW_VER_LEN;
        s = syntiant_ndp120_tiny_read(
            ndp, SYNTIANT_NDP120_MCU_OP, address, &idata->dsp_fw_version_len);
        if (s) goto error;
        if (idata->fw_version_len > NDP120_MCU_DSP_FW_VER_MAX_LEN) {
            SYNTIANT_NDP120_PRINTF(
                "Firmware sent invalid DSP FW version length:%d bytes\n",
                idata->dsp_fw_version_len);
        }

        address = SYNTIANT_NDP120_MCU_PKG_VER_LEN;
        s = syntiant_ndp120_tiny_read(
            ndp, SYNTIANT_NDP120_MCU_OP, address, &idata->pkg_version_len);
        if (s) goto error;
        if (idata->pkg_version_len > NDP120_MCU_PKG_VER_MAX_LEN) {
            SYNTIANT_NDP120_PRINTF(
                "Firmware sent invalid package version length:%d bytes\n",
                idata->pkg_version_len);
        }

        address = SYNTIANT_NDP120_MCU_LABELS_LEN;
        s = syntiant_ndp120_tiny_read(
            ndp, SYNTIANT_NDP120_MCU_OP, address, &idata->labels_len);
        if (s) goto error;
        if (idata->labels_len > NDP120_MCU_LABELS_MAX_LEN) {
            SYNTIANT_NDP120_PRINTF(
                "Firmware sent invalid labels length:%d bytes\n",
                idata->labels_len);
        }

        address = SYNTIANT_NDP120_MCU_PBI_VER_LEN;
        s = syntiant_ndp120_tiny_read(
            ndp, SYNTIANT_NDP120_MCU_OP, address, &ver_len);
        if (s) goto error;
        if (ver_len > NDP120_MCU_PBI_VER_MAX_LEN) {
            SYNTIANT_NDP120_PRINTF(
               "Firmware sent invalid PBI version length:%d bytes\n", ver_len);
        }

        address += (uint32_t)sizeof(ver_len);
        s = syntiant_ndp120_tiny_read_block(ndp, SYNTIANT_NDP120_MCU_OP,
            address, idata->fw_version, idata->fw_version_len);
        if (s) goto error;

        address += idata->fw_version_len;
        s = syntiant_ndp120_tiny_read_block(ndp, SYNTIANT_NDP120_MCU_OP,
            address, idata->dsp_fw_version, idata->dsp_fw_version_len);
        if (s) goto error;

        address += idata->dsp_fw_version_len;
        s = syntiant_ndp120_tiny_read_block(ndp, SYNTIANT_NDP120_MCU_OP,
            address, idata->pkg_version, idata->pkg_version_len);
        if (s) goto error;

        address += idata->pkg_version_len;
        s = syntiant_ndp120_tiny_read_block(ndp, SYNTIANT_NDP120_MCU_OP,
            address, idata->labels, idata->labels_len);
        if (s) goto error;

        address += idata->labels_len;
        s = syntiant_ndp120_tiny_read_block(
            ndp, SYNTIANT_NDP120_MCU_OP, address, idata->pbi, ver_len);
        if (s) goto error;

        address += ver_len;
        s = syntiant_ndp120_tiny_read(
            ndp, SYNTIANT_NDP120_MCU_OP, address, &idata->total_nn);
        if (s) goto error;

        address += (uint32_t)sizeof(uint32_t);
        sf_len = (uint32_t)(idata->total_nn * sizeof(uint32_t));
        s = syntiant_ndp120_tiny_read_block(ndp, SYNTIANT_NDP120_MCU_OP,
            address, idata->scale_factor, sf_len);
        if (s) goto error;

        address += sf_len;
        sensor_info_len = (uint32_t)(SYNTIANT_NDP120_SENSOR_MAX *
                                     sizeof(uint32_t));
        s = syntiant_ndp120_tiny_read_block(ndp, SYNTIANT_NDP120_MCU_OP,
            address, idata->sensor_info, sensor_info_len);
        if (s) goto error;
    }
error:
    if (ndp->iif->unsync) {
        s0 = (ndp->iif->unsync)(ndp->iif->d);
        s = s ? s : s0;
    }
    return s;
}

int syntiant_ndp120_tiny_get_debug(struct syntiant_ndp120_tiny_device_s *ndp,
        struct syntiant_ndp120_tiny_debug *mcu_dsp_dbg_cnts)
{
    int s0;
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint32_t cmd[2] = {SYNTIANT_NDP120_GET_DEBUG_INFO, 0};

    if (ndp->iif->sync) {
        s = (ndp->iif->sync)(ndp->iif->d);
        if (s) {
            SYNTIANT_NDP120_PRINTF(
                "Error in syntiant_ndp120_tiny_get_nn_graph\n");
            return s;
        }
    }

    if (mcu_dsp_dbg_cnts) {
        /* write cmd data */
        s = syntiant_ndp120_tiny_write_block(ndp, SYNTIANT_NDP120_MCU_OP,
                SYNTIANT_NDP120_CMD_RAM, cmd, sizeof(cmd));
        if (s) goto error;

        /* send command */
        s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(ndp,
                SYNTIANT_NDP120_MB_MCU_CMD, NULL);
        if (s) goto error;

        s = syntiant_ndp120_tiny_read_block(ndp, SYNTIANT_NDP120_MCU_OP,
                SYNTIANT_NDP120_OPEN_RAM_RESULTS, mcu_dsp_dbg_cnts,
                sizeof(*mcu_dsp_dbg_cnts));
        if (s) goto error;
    }
error:
    if (ndp->iif->unsync) {
        s0 = (ndp->iif->unsync)(ndp->iif->d);
        s = s ? s : s0;
    }
    return s;
}

int syntiant_ndp120_tiny_input_config(
    struct syntiant_ndp120_tiny_device_s *ndp, uint32_t input_mode)
{
    int s0;
    uint8_t data_8;
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint32_t configured_input = SYNTIANT_NDP120_TINY_GET_INPUT_CONFIG;
    uint32_t data[3] = {SYNTIANT_NDP120_INPUT_CONFIG, sizeof(input_mode),
        SYNTIANT_NDP120_TINY_GET_INPUT_CONFIG};
    if (ndp->iif->sync) {
        s = (ndp->iif->sync)(ndp->iif->d);
        if (s) {
            SYNTIANT_NDP120_PRINTF(
                "Error in syntiant_ndp120_tiny_input_config\n");
            return s;
        }
    }
    /* write cmd, length of payload, payload */
    s = syntiant_ndp120_tiny_write_block(ndp, SYNTIANT_NDP120_MCU_OP,
        SYNTIANT_NDP120_CMD_RAM, data, sizeof(data));
    if (s) goto error;

    /* send command */
    s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(ndp,
        SYNTIANT_NDP120_MB_MCU_CMD, NULL);
    if (s) goto error;

    s = syntiant_ndp120_tiny_read_block(ndp, SYNTIANT_NDP120_MCU_OP,
        SYNTIANT_NDP120_OPEN_RAM_RESULTS, &configured_input,
        sizeof(configured_input));
    if (s) goto error;

    if(configured_input != input_mode) {
        data[2] =  input_mode;
        /* write payload */
        s = syntiant_ndp120_tiny_write_block(ndp, SYNTIANT_NDP120_MCU_OP,
            SYNTIANT_NDP120_CMD_RAM, data, sizeof(data));
        if (s) goto error;

        /* send command */
        s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(ndp,
            SYNTIANT_NDP120_MB_MCU_CMD, NULL);
        if (s) goto error;
    } else {
        s = SYNTIANT_NDP120_ERROR_CONFIG;
        goto error;
    }
    if(input_mode == SYNTIANT_NDP120_TINY_INPUT_CONFIG_SPI) {
        /*enabling watermark interrupt*/
        s = ndp_spi_read(NDP120_SPI_INTCTL, &data_8);
        if(s) goto error;
        data_8 = NDP120_SPI_INTCTL_WM_INTEN_INSERT(data_8, 1);
        s = ndp_spi_write(NDP120_SPI_INTCTL, data_8);
        if (s) goto error;
    }
error:
    if (ndp->iif->unsync) {
        s0 = (ndp->iif->unsync)(ndp->iif->d);
        s = s ? s : s0;
    }
    return s;
}

int syntiant_ndp120_tiny_dsp_tank_size_config(struct
    syntiant_ndp120_tiny_device_s *ndp, uint32_t pcm_tank_size_in_msec)
{
    int s0;
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint32_t cmd[3] = {SYNTIANT_NDP120_SET_TANK_SIZE,
            sizeof(pcm_tank_size_in_msec), 0};
    if (ndp->iif->sync) {
        s = (ndp->iif->sync)(ndp->iif->d);
        if (s) {
            SYNTIANT_NDP120_PRINTF(
                "Error in syntiant_ndp120_tiny_dsp_tank_size_config\n");
            return s;
        }
    }
    /* dsp restart required to have correct winstep value */
    s = syntiant_ndp120_tiny_dsp_restart(ndp);
    if (s) {
        goto error;
    }
    cmd[2] = pcm_tank_size_in_msec;
    /* write cmd, length of the payload and payload */
    s = syntiant_ndp120_tiny_write_block(ndp, SYNTIANT_NDP120_MCU_OP,
        SYNTIANT_NDP120_CMD_RAM, cmd, sizeof(cmd));
    if (s) goto error;

    s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(ndp,
        SYNTIANT_NDP120_MB_MCU_CMD, NULL);
    if (s) goto error;

error:
    if (ndp->iif->unsync) {
        s0 = (ndp->iif->unsync)(ndp->iif->d);
        s = s ? s : s0;
    }
    return s;
}

int syntiant_ndp120_tiny_vad_mic_control(struct
    syntiant_ndp120_tiny_device_s *ndp, uint32_t vad_mic_control)
{
    int s0;
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint32_t cmd[3] = {SYNTIANT_NDP120_VAD_MIC_CONTROL,
             sizeof(vad_mic_control), 0};

    if (ndp->iif->sync) {
        s = (ndp->iif->sync)(ndp->iif->d);
        if (s) {
            SYNTIANT_NDP120_PRINTF(
                "Error in syntiant_ndp120_tiny_vad_mic_control\n");
            return s;
        }
    }
    cmd[2] = vad_mic_control;
    /* write cmd, length of the payload and payload */
    s = syntiant_ndp120_tiny_write_block(ndp, SYNTIANT_NDP120_MCU_OP,
        SYNTIANT_NDP120_CMD_RAM, cmd, sizeof(cmd));
    if (s) goto error;

    s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(ndp,
        SYNTIANT_NDP120_MB_MCU_CMD, NULL);

    if (s) {
        if (s == SYNTIANT_NDP120_ERROR_DSP_NO_VAD_MIC) {
            SYNTIANT_NDP120_PRINTF(
                "syntiant_ndp120_tiny_vad_mic_control: no vad mic\n");
        }
        goto error;
    }

error:
    if (ndp->iif->unsync) {
        s0 = (ndp->iif->unsync)(ndp->iif->d);
        s = s ? s : s0;
    }
    return s;
}

int syntiant_ndp120_tiny_halt_mcu(struct syntiant_ndp120_tiny_device_s *ndp)
{
    int s;
    uint8_t data;

    s = ndp_spi_read(NDP120_SPI_CTL, &data);
    if (s) goto error;
    data = NDP120_SPI_CTL_MCUHALT_INSERT(data, 1);
    s = ndp_spi_write(NDP120_SPI_CTL, data);
    if (s) goto error;

error:
    return s;
}

int syntiant_ndp120_tiny_poll_notification(
    struct syntiant_ndp120_tiny_device_s *ndp,
    uint32_t notification_mask)
{
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint32_t notify = 0;
    unsigned counter = SYNTIANT_NDP_POLL_TIMEOUT_COUNT;

    while (counter--) {
        s = syntiant_ndp120_tiny_poll(ndp, &notify, 1);
        if (s) {
            goto done;
        }
        if (notify & notification_mask) {
            goto done;
        }
        if (ndp->iif->udelay) {
            ndp->iif->udelay(1000);
        }
    }
    s = SYNTIANT_NDP120_ERROR_TIMEOUT;

done:
    return s;
}

int syntiant_ndp120_tiny_boot_from_flash(
    struct syntiant_ndp120_tiny_device_s *ndp)
{
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint32_t msg = 0;

    mailbox_reset_state(ndp);

    s = syntiant_ndp120_tiny_mb_cmd(ndp, SYNTIANT_NDP120_MB_BOOT_FROM_FLASH, &msg);
    if (s) {
        goto done;
    }

    s = syntiant_ndp120_tiny_poll_notification(ndp,
            SYNTIANT_NDP120_NOTIFICATION_OTHER_LOAD_DONE);
    if (s) {
        goto done;
    }

    s = syntiant_ndp120_tiny_mb_cmd(ndp, SYNTIANT_NDP120_MB_MCU_MIADDR, &msg);
    if (s) {
        goto done;
    }

done:
    return s;
}

int syntiant_ndp120_tiny_soft_flash_boot(
    struct syntiant_ndp120_tiny_device_s *ndp)
{
    int s;
    uint32_t data = NDP120_SOFT_FLASH_BOOT_SIG;

    /* indicate to mcufw to do soft boot from flash */
    s = syntiant_ndp120_tiny_write_block(ndp, SYNTIANT_NDP120_MCU_OP,
        NDP120_SOFT_FLASH_BOOT_ADDR, &data, sizeof(data));
    if (s) {
        goto done;
    }

done:
    return s;
}

int syntiant_ndp120_tiny_config_gpio(
    struct syntiant_ndp120_tiny_device_s *ndp,
    struct syntiant_ndp120_tiny_config_gpio_s *config)
{
    int s;
    uint32_t data, data_old;
    uint32_t mask, mask_lower, mask_upper;

    mask = (uint32_t) (1u << config->gpio_num);
    mask_lower = mask & 0xffff;
    mask_upper = (mask >> 16) & 0xffff;

    s = syntiant_ndp120_tiny_read(ndp, 1, NDP120_CHIP_CONFIG_GPIOSEL, &data);
    if (s) goto error;

    data |= mask;
    s = syntiant_ndp120_tiny_write(ndp, 1, NDP120_CHIP_CONFIG_GPIOSEL, data);
    if (s) goto error;

    if (config->dir == SYNTIANT_NDP120_CONFIG_VALUE_GPIO_DIR_OUT) {
        if (mask_lower) {
            s = syntiant_ndp120_tiny_write(ndp, 1, NDP120_GPIO_OUTSET, mask_lower);
            if (s) goto error;
        } else {
            s = syntiant_ndp120_tiny_write(ndp, 1, NDP120_GPIO1_OUTSET, mask_upper);
            if (s) goto error;
        }
    } else {
        if (mask_lower) {
            s = syntiant_ndp120_tiny_write(ndp, 1, NDP120_GPIO_OUTCLR, mask_lower);
            if (s) goto error;
        } else {

            s = syntiant_ndp120_tiny_write(ndp, 1, NDP120_GPIO1_OUTCLR, mask_upper);
            if (s) goto error;
        }
    }

    s = syntiant_ndp120_tiny_read(ndp, 1,
        mask_lower ? NDP120_GPIO_DATAOUT : NDP120_GPIO1_DATAOUT, &data);
    if (s) goto error;

    data_old = data;
    if (config->value) {
        data |= (mask_lower | mask_upper);
    } else {
        data &= ~(mask_lower | mask_upper);
    }

    if (data != data_old) {
        s = syntiant_ndp120_tiny_write(ndp, 1,
            mask_lower ? NDP120_GPIO_DATAOUT : NDP120_GPIO1_DATAOUT, data);
        if (s) goto error;
    }

error:
    return s;
}

static int syntiant_serial_address(
    uint8_t interface, uint8_t interface_address, uint8_t *addressp)
{
    int s = SYNTIANT_NDP120_ERROR_NONE;
    unsigned char address = interface_address;

    if (interface == SYNTIANT_NDP120_SERIAL_INTERFACE_I2C) {
        if (0x7f < interface_address) {
            s = SYNTIANT_NDP120_ERROR_ARG;
            goto out;
        }
        address |= SYNTIANT_NDP120_SERIAL_ADDRESS_I2C_MASK;
    } else if (SYNTIANT_NDP120_SERIAL_INTERFACE_MAX < interface) {
        s = SYNTIANT_NDP120_ERROR_ARG;
        goto out;
    } else {
        address |= (uint8_t)
            ((interface - SYNTIANT_NDP120_SERIAL_INTERFACE_SPI0)
            << SYNTIANT_NDP120_SERIAL_ADDRESS_SPI_MODE_SHIFT);
    }
    *addressp = address;

out:
    return s;
}

static int syntiant_tiny_ndp120_read_write_sensor_serial(
    struct syntiant_ndp120_tiny_device_s *ndp,
    struct ndp120_serial_s *serial, int set)
{
    int s0;
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint32_t cmd[] = {SYNTIANT_NDP120_SERIAL_TRANSFER, 0};
    uint32_t length = sizeof(struct ndp120_serial_s);
    uint32_t addr = SYNTIANT_NDP120_CMD_RAM;

    if (ndp->iif->sync) {
        s = (ndp->iif->sync)(ndp->iif->d);
        if (s) {
            SYNTIANT_NDP120_PRINTF(
                "Error in syntiant_ndp120_read_sensor_serial\n");
            return s;
        }
    }
    if (!serial) {
        s = SYNTIANT_NDP120_ERROR_UNINIT;
        goto error;
    }
    if (set) {
        cmd[1] = length;
    }

    s = syntiant_ndp120_tiny_write_block(ndp, SYNTIANT_NDP120_MCU_OP,
        addr, cmd, sizeof(cmd));
    if (s) {
        goto error;
    }

    if (set) {
        addr += (uint32_t) sizeof(cmd);
        s = syntiant_ndp120_tiny_write_block(ndp, SYNTIANT_NDP120_MCU_OP,
            addr, serial, length);
        if (s) {
            goto error;
        }
    }
    /* send command */
    s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(ndp,
        SYNTIANT_NDP120_MB_MCU_CMD, NULL);
    if (s) {
        goto error;
    }

    if (!set) {
        s = syntiant_ndp120_tiny_read_block(ndp, SYNTIANT_NDP120_MCU_OP,
            SYNTIANT_NDP120_OPEN_RAM_RESULTS, serial, length);
        if (s) {
            goto error;
        }
    }

error:
    if (ndp->iif->unsync) {
        s0 = (ndp->iif->unsync)(ndp->iif->d);
        s = s ? s : s0;
    }
    return s;
}

int syntiant_ndp120_tiny_serial_transfer(
    struct syntiant_ndp120_tiny_device_s *ndp, unsigned int ifc_type,
    unsigned int ifc_addr, uint8_t *out, unsigned int outlen, uint8_t *in,
    unsigned int inlen, int continue_)
{
    int s = SYNTIANT_NDP120_ERROR_NONE;
    struct ndp120_serial_s serial = {0};
    unsigned int outn, outl, inn, inl;
    uint8_t * outp,*inp;
    uint8_t serial_address;

    if ((unsigned char)ifc_type != SYNTIANT_NDP120_SERIAL_INTERFACE_I2C
        && SYNTIANT_NDP120_GPIO_MAX <= (unsigned char)ifc_addr) {
        s = SYNTIANT_NDP120_ERROR_ARG;
        goto error;
    }

    s = syntiant_serial_address(
        (uint8_t)ifc_type, (uint8_t)ifc_addr, &serial_address);
    if (s) {
        goto error;
    }

    outn = outlen;
    inn = inlen;
    outp = out;
    inp = in;
    while (outn || inn) {
        outl = 0;
        if (outn) {
            outl = SYNTIANT_NDP120_SERIAL_BYTE_MAX <= outn
                ? SYNTIANT_NDP120_SERIAL_BYTE_MAX
                : outn;
            memcpy(serial.data, outp, outl);
            outn -= outl;
            outp += outl;
        }
        inl = 0;
        if (!outn && inn) {
            inl = SYNTIANT_NDP120_SERIAL_BYTE_MAX <= inn
                ? SYNTIANT_NDP120_SERIAL_BYTE_MAX
                : inn;
            inn -= inl;
        }

        serial.control =
            (((unsigned int)serial_address)
            << SYNTIANT_NDP120_SERIAL_CONTROL_ADDRESS_SHIFT)
            | (((unsigned int)(outn || inn || continue_))
            << SYNTIANT_NDP120_SERIAL_CONTROL_CONTINUE_SHIFT)
            | (outl << SYNTIANT_NDP120_SERIAL_CONTROL_OUTLEN_SHIFT)
            | (inl << SYNTIANT_NDP120_SERIAL_CONTROL_INLEN_SHIFT)
            | SYNTIANT_NDP120_SERIAL_CONTROL_RUN_MASK;

        s = syntiant_tiny_ndp120_read_write_sensor_serial(ndp, &serial, 1);
        if (s) {
            goto error;
        }

        s = syntiant_ndp120_tiny_do_mailbox_req_no_sync(ndp,
            SYNTIANT_NDP120_DSP_MB_H2D_MODE_CHANGE, NULL);
        if (s) {
            goto error;
        }

        s = syntiant_tiny_ndp120_read_write_sensor_serial(ndp, &serial, 0);
        if (s) {
            goto error;
        }

        if (serial.control & SYNTIANT_NDP120_SERIAL_CONTROL_RUN_MASK) {
            s = SYNTIANT_NDP120_ERROR_FAIL;
            goto error;
        }
        s = (serial.control & SYNTIANT_NDP120_SERIAL_CONTROL_STATUS_MASK)
            >> SYNTIANT_NDP120_SERIAL_CONTROL_STATUS_SHIFT;
        if (s) {
            s = (s == SYNTIANT_NDP120_SERIAL_CONTROL_STATUS_TIMEOUT)
                ? SYNTIANT_NDP120_ERROR_TIMEOUT
                : SYNTIANT_NDP120_ERROR_FAIL;
            goto error;
        }
        memcpy(inp, serial.data, inl);
        inp += inl;
    }

error:
    return s;
}
