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
 */

#ifndef SYNTIANT_NDP120_TINY_H
#define SYNTIANT_NDP120_TINY_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file syntiant_ndp120_tiny.h
 * @brief Interface to Syntiant NDP120 minimal chip interface library
 */

#if 0
/**
 * @brief maximum transfer size on the device
 */
#define SYNTIANT_MAX_BLOCK_SIZE     (2048)
#endif

/**
 * @brief package load state flags
 */
#define SYNTIANT_NDP120_MCU_LOADED (0x1)
#define SYNTIANT_NDP120_DSP_LOADED (0x2)
#define SYNTIANT_NDP120_NN_LOADED  (0x4)

/** maximum length (in bytes) of various verison strings
 */
#define NDP120_MCU_FW_VER_MAX_LEN       (0x20)
#define NDP120_MCU_DSP_FW_VER_MAX_LEN   (0x20)
#define NDP120_MCU_PKG_VER_MAX_LEN      (0x30)
#define NDP120_MCU_LABELS_MAX_LEN       (0x200)
#define NDP120_MCU_PBI_VER_MAX_LEN      (0x10)

/**
 * @brief SPI registers of the device
 */
#define NDP120_SPI_ID0 (0x00U)
#define NDP120_SPI_ID0_SERIAL_BOOT_SHIFT 0
#define NDP120_SPI_ID0_SERIAL_BOOT_MASK 0x01U
#define NDP120_SPI_ID0_SERIAL_BOOT_EXTRACT(x) \
        (((x) & NDP120_SPI_ID0_SERIAL_BOOT_MASK) >> \
         NDP120_SPI_ID0_SERIAL_BOOT_SHIFT)
#define NDP120_SPI_INTCTL (0x10U)
#define NDP120_SPI_CTL (0x11U)
#define NDP120_SPI_CTL_MCUHALT_SHIFT 3
#define NDP120_SPI_CTL_MCUHALT_INSERT(x, v) \
        ((x) | ((v) << NDP120_SPI_CTL_MCUHALT_SHIFT))
#define NDP120_SPI_CFG (0x12U)
#define NDP120_SPI_SAMPLE (0x20U)
#define NDP120_SPI_MBIN (0x30U)
#define NDP120_SPI_MBIN_RESP (0x31U)
#define NDP120_SPI_MBOUT (0x32U)
#define NDP120_SPI_MBOUT_RESP (0x33U)
#define NDP120_SPI_INTSTS (0x02U)

#define NDP120_SPI_MATCHSTS 0x03U
#define NDP120_SPI_MATCHSTS_STATUS_MASK 0xffU
#define NDP120_SPI_MATCHSTS_STATUS_SHIFT 0
#define NDP120_SPI_MATCHSTS_STATUS_EXTRACT(x) \
        (((x) & NDP120_SPI_MATCHSTS_STATUS_MASK) >> \
         NDP120_SPI_MATCHSTS_STATUS_SHIFT)

#define NDP120_SPI_MATCH_WINNER_MASK 0x3fU
#define NDP120_SPI_MATCH_WINNER_SHIFT 0
#define NDP120_SPI_MATCH_WINNER_EXTRACT(x) \
        (((x) & NDP120_SPI_MATCH_WINNER_MASK) >> NDP120_SPI_MATCH_WINNER_SHIFT)

#define NDP120_SPI_INTCTL_WM_INTEN_SHIFT 6
#define NDP120_SPI_INTCTL_WM_INTEN_INSERT(x, v) \
        ((x) | ((v) << NDP120_SPI_INTCTL_WM_INTEN_SHIFT))

/* register array ndp120_spi.maddr[4] */
#define NDP120_SPI_MADDR(i) (0x40U + ((i) << 0))
#define NDP120_SPI_MADDR_COUNT 4
/* register array ndp120_spi.mdata[4] */
#define NDP120_SPI_MDATA(i) (0x44U + ((i) << 0))
#define NDP120_SPI_MDATA_COUNT 4
#define NDP120_SPI_MATCH_MATCH_MASK 0x40

#define NDP120_ILIB_SCRATCH_ORIGIN 0x20001C00

/* Bootloader window for MCU fw download */
#define SYNTIANT_NDP120_BL_WINDOW_LOWER (0x20007C00U)
#define SYNTIANT_NDP120_BL_WINDOW_UPPER (0x20008000U)

/* Download window used in MCU fw for other packages */
#define SYNTIANT_NDP120_DL_WINDOW_LOWER (0x20007400U)
#define SYNTIANT_NDP120_DL_WINDOW_UPPER (0x20007C00U)

#define SYNTIANT_NDP120_OPEN_RAM_RESULTS (0x20007500U)
#define SYNTIANT_NDP120_OPEN_RAM_MATCH_RESULTS (0x20007BC0U)

#define SYNTIANT_NDP120_TINY_AUDIO_SAMPLES_PER_WORD 2
#define SYNTIANT_NDP120_TINY_AUDIO_SAMPLE_RATE 16000

#define SYNTIANT_NDP120_TINY_PLAY_OUTSTANDING_FRAMES 3

#define SYNTIANT_NDP120_DEF_SPI_SPEED   (1000000)
#define SYNTIANT_NDP115_MAX_SPI_SPEED   (10000000)
#define SYNTIANT_NDP120_MAX_SPI_SPEED   (12000000)

/* soft boot from flash */
#define NDP120_SOFT_FLASH_BOOT_SIG      0x53594E54U
#define NDP120_SOFT_FLASH_BOOT_ADDR     NDP120_ILIB_SCRATCH_ORIGIN

#define NDP120_RESULT_NUM_CLASSES (32)

#define NDP120_DNN_ISA_COMP0_ACTIVATION_LINEAR 0x0U
#define NDP120_DNN_ISA_COMP0_ACTIVATION_LINEAR_16 0x1U
#define NDP120_DNN_ISA_COMP0_ACTIVATION_RELU 0x2U
#define NDP120_DNN_ISA_COMP0_ACTIVATION_TANH0 0x4U
#define NDP120_DNN_ISA_COMP0_ACTIVATION_TANH1 0x5U
#define NDP120_DNN_ISA_COMP0_ACTIVATION_SIGMOID0 0x6U
#define NDP120_DNN_ISA_COMP0_ACTIVATION_SIGMOID1 0x7U

/**
 * @brief secure input config mode
 */
enum syntiant_ndp120_tiny_input_config_mode_e {
    SYNTIANT_NDP120_TINY_GET_INPUT_CONFIG = 0,
    SYNTIANT_NDP120_TINY_INPUT_CONFIG_PDM = 1,
    SYNTIANT_NDP120_TINY_INPUT_CONFIG_SPI = 16
};

/**
 * @brief vad mic control
 */
enum syntiant_ndp120_tiny_vad_mic_control_e {
    SYNTIANT_NDP120_TINY_VAD_MIC_MODE_AON = 0x0, /* always on */
    SYNTIANT_NDP120_TINY_VAD_MIC_MODE_VAD = 0x1  /* voice activity detect */
};

/**
 * @brief Device info structure holding data about deployed model, fimrware etc.
 */
struct syntiant_ndp120_tiny_info {
    char *fw_version;                       /**< MCU fw version */
    char *dsp_fw_version;                   /**< DSP fw version */
    char *pkg_version;                      /**< pkg version */
    char *labels;                           /**< NN labels */
    uint8_t *pbi;                           /**< PBI version */
    unsigned int fw_version_len;            /**< MCU fw version length */
    unsigned int dsp_fw_version_len;        /**< DSP fw version length */
    unsigned int pkg_version_len;           /**< pkg version length */
    unsigned int labels_len;                /**< NN labels length */
    unsigned int total_nn;                  /**< Total deployed NNs */
    unsigned int *scale_factor;             /**< Scale factor of each NN*/
    unsigned int *sensor_info;              /**< sensor info for 4 sensors */
};

enum {
    SYNTIANT_NDP120_MAX_NNETWORKS = 4,
    /**< maximum number of neural networks on device */
    SYNTIANT_NDP120_MAX_CLASSES = 32,
    /**< mabimum number of classes in a network */
    SYNTIANT_NDP120_SENSOR_MAX = 4,
    /**< maximum number of sensors on device */
    SYNTIANT_NDP120_SENSOR_PARAM_MAX = 16,
    /**< maximum number of parameters per sensor on device */
    SYNTIANT_NDP120_SENSOR_CONTROL_ID_MASK = 0xff,
    SYNTIANT_NDP120_SENSOR_CONTROL_ID_SHIFT = 0,
    SYNTIANT_NDP120_SENSOR_CONTROL_ADDRESS_MASK = 0xff00,
    SYNTIANT_NDP120_SENSOR_CONTROL_ADDRESS_SHIFT = 8,
    SYNTIANT_NDP120_SENSOR_CONTROL_INT_GPIO_MASK = 0x1f0000,
    SYNTIANT_NDP120_SENSOR_CONTROL_INT_GPIO_SHIFT = 16,
    SYNTIANT_NDP120_SENSOR_CONTROL_INT_GPIO1_MASK = 0x3E00000,
    SYNTIANT_NDP120_SENSOR_CONTROL_INT_GPIO1_SHIFT = 21,
    SYNTIANT_NDP120_SENSOR_CONTROL_AXES_MASK = 0x3C000000,
    SYNTIANT_NDP120_SENSOR_CONTROL_AXES_SHIFT = 26,

    SYNTIANT_NDP120_SENSOR_ID_NONE = 0,   /** no sensor */
    SYNTIANT_NDP120_SENSOR_ID_BMI160 = 1, /** Bosch BMI160 (<= 10 axis) */
    SYNTIANT_NDP120_SENSOR_ID_VM3011 = 2,  /** energy detecting mic */
    SYNTIANT_NDP120_SENSOR_ID_EDGE_INT = 3,/** generic edge interrupt such as
                                               LiteOn ltr559 */
    SYNTIANT_NDP120_SENSOR_ID_DA217 = 4,   /** Miramems DA217 motion sensor */
    SYNTIANT_NDP120_SENSOR_ID_KX120 = 5,   /** Kionix KX120 motion sensor */
    SYNTIANT_NDP120_SENSOR_ID_MC3419 = 6,  /** Memsic MC3419 motion sensor */
    SYNTIANT_NDP120_SENSOR_ID_GPIO_DEBOUNCE = 7, /** Debounced GPIO pin */
    SYNTIANT_NDP120_SENSOR_ID_SPL_THRESHOLD = 8, /** SPL threshold */
    SYNTIANT_NDP120_SENSOR_ID_T5838 = 9,   /** TDK T5838 */
    SYNTIANT_NDP120_SENSOR_ID_LAST = SYNTIANT_NDP120_SENSOR_ID_T5838
};

/** @brief debug counters of DSP firmware
 */
struct syntiant_ndp120_dsp_debug_cnt_s {
    uint32_t frame_cnt;         /* processed frames             */
    uint32_t dnn_int_cnt;       /* dnn interrupts               */
    uint32_t dnn_err_cnt;       /* dnn error counts             */
    uint32_t h2d_mb_cnt;        /* host   -> DSP mb             */
    uint32_t d2m_mb_cnt;        /* DSP    -> MCU mb             */
    uint32_t m2d_mb_cnt;        /* MCU    -> DSP mb             */
    uint32_t watermark_cnt;     /* DSP WM -> host               */
    uint32_t fifo_overflow_cnt; /* FW detected fifo overflow    */
    uint32_t nn_cycle_cnt[SYNTIANT_NDP120_MAX_NNETWORKS];  /* cycle count per NN */
    uint32_t nn_run_cnt[SYNTIANT_NDP120_MAX_NNETWORKS];  /* DNN run count per NN */
};

/** @brief debug counters of MCU firmware
 */
struct syntiant_ndp120_debug_cnt_s {
   uint32_t signature;          /* identifier for this struct */
   uint32_t frame_cnt;          /* Frame count */
   uint32_t dsp2mcu_intr_cnt;   /* DSP 2 MCU int cnt */
   uint32_t dsp2mcu_nn_done_cnt;/* DSP 2 MCU done cnt */
   uint32_t mcu2host_match_cnt; /* MCU 2 Host match cnt */
   uint32_t mcu2host_mpf_cnt;   /* MCU 2 Host match per frame cnt */
   uint32_t mcu2dsp_done_cnt;   /* MCU 2 DSP done cnt */
   uint32_t matches;            /* Matches detected in posterior handler */
   uint32_t dsp2mcu_queue_cnt;  /* DSP 2 MCU queue cnt */
   uint32_t nn_orch_flwchg_cnt; /* Flow map change cnt in NN orchestrator */
   uint32_t unknown_activation_cnt;/* Unknown activation count */
   uint32_t unknown_int_count;  /* Unknown interrupt count */
   uint32_t inv_nn_orch_node_cnt;/* Invalid NN graph node error */
   uint32_t accumulator_error;  /* Counter for tracking accumulator error in softmax */
   uint32_t mbin_int_cnt;       /* MBIN int count */
   uint32_t mbout_int_cnt;      /* MBOUT int count */
   uint32_t buffer_bgn;         /* Debug buffer begin */
   uint32_t buffer_end;         /* Debug buffer end */
   uint32_t curr_ptr;           /* Debug buffer current pointer */
   uint32_t inv_num_class_cnt;  /* Invalid number of classes */
   uint32_t num_frames;         /* maximum frames for logging */
   uint32_t dbg1;
   uint32_t dbg2;
   uint32_t ph_heap_allocation_error; /* Counter for tracking ph heap allocation error */
   uint32_t enable;             /* Debug logging enable flag */
};

struct syntiant_ndp120_flow_rule {
    uint32_t src_param;         /* src parameter, e.g. channel number */
    uint32_t dst_param;         /* destination parameter */
    uint8_t  dst_type;          /* DSP function, NN, or post processor */
    int8_t   algo_config_index; /* -1 means "none" */
    uint8_t  set_id;            /* Id of the flowset for this flow rule */
    uint8_t _dummy0;
};

struct syntiant_ndp120_sensor_config {
    uint32_t control;   /* sensor control fields */
    uint32_t enable;    /* tank and input enable bits */
    uint8_t  parameter[SYNTIANT_NDP120_SENSOR_PARAM_MAX];
};

struct syntiant_ndp120_sensor_state {
    uint32_t int_count;
    uint32_t other_sensor_state[9];
};

#define NDP120_PCM_DATA_FLOW_RULE_MAX 16
#define NDP120_FUNC_DATA_FLOW_RULE_MAX 32
#define NDP120_NN_DATA_FLOW_RULE_MAX 32
#define NDP120_MCU_DATA_FLOW_RULE_MAX 8

/* pass in a _rule_ */
#define NDP120_DSP_DATA_FLOW_RULE_DST_STR(x) \
    (x).dst_type == NDP120_DSP_DATA_FLOW_DST_TYPE_NONE ? "<INVALID>" : \
    (x).dst_type == NDP120_DSP_DATA_FLOW_DST_TYPE_FUNCTION ? "FUNC" : \
    (x).dst_type == NDP120_DSP_DATA_FLOW_DST_TYPE_NN ? "NN" : \
    (x).dst_type == NDP120_DSP_DATA_FLOW_DST_TYPE_HOST_EXTRACT ? \
    ((x).dst_param ==  NDP120_DSP_DATA_FLOW_DST_SUBTYPE_AUDIO ? "HOST_EXT_AUDIO" : "HOST_EXT_FEATURE") : \
    (x).dst_type == NDP120_DSP_DATA_FLOW_DST_TYPE_I2S ? "I2S" : \
    (x).dst_type == NDP120_DSP_DATA_FLOW_DST_TYPE_MCU ? "MCU" : "UNKNOWN"

struct syntiant_ndp120_flow_setup {
    struct syntiant_ndp120_flow_rule src_pcm_audio[NDP120_PCM_DATA_FLOW_RULE_MAX];
    /**< flow rule array for PCM audio source */
    struct syntiant_ndp120_flow_rule src_function[NDP120_FUNC_DATA_FLOW_RULE_MAX];
    /**< flow rule array for DSP algo source */
    struct syntiant_ndp120_flow_rule src_nn[NDP120_NN_DATA_FLOW_RULE_MAX];
    /**< flow rule array for NN source */
    struct syntiant_ndp120_flow_rule src_mcu[NDP120_MCU_DATA_FLOW_RULE_MAX];
    /**< flow rule array for MCU firmware source */
};

struct syntiant_ndp120_dsp_dev_data {
    uint8_t last_network;           /* Last network executed */
    uint8_t db_measured;            /* Decibel level measured */
    uint8_t sampletank_mem_type;    /* Memoty type for sample tank */
    uint8_t device_state;           /* Device state flag */
};

enum {
    SYNTIANT_NDP120_DSP_MEM_TYPE_HEAP = 1,
    SYNTIANT_NDP120_DSP_MEM_TYPE_DNN_DATA
};

enum {
    NDP120_DSP_DATA_FLOW_DST_TYPE_NONE,
    NDP120_DSP_DATA_FLOW_DST_TYPE_FUNCTION,
    NDP120_DSP_DATA_FLOW_DST_TYPE_NN,
    NDP120_DSP_DATA_FLOW_DST_TYPE_HOST_EXTRACT,
    NDP120_DSP_DATA_FLOW_DST_TYPE_I2S,
    NDP120_DSP_DATA_FLOW_DST_TYPE_MCU
    /* If you update this, update the _STR
       macro above */
};
enum {
    NDP120_DSP_SAMPLE_TYPE_PCM_AUDIO,
    NDP120_DSP_SAMPLE_TYPE_FUNCTION
};
enum {
    NDP120_DSP_DATA_FLOW_DST_SUBTYPE_AUDIO = NDP120_DSP_SAMPLE_TYPE_PCM_AUDIO,
    NDP120_DSP_DATA_FLOW_DST_SUBTYPE_FEATURE = NDP120_DSP_SAMPLE_TYPE_FUNCTION
};

/**
 * @brief debug counters of both the firmwares
 */
struct syntiant_ndp120_tiny_debug {
    struct syntiant_ndp120_debug_cnt_s mcu_dbg_cnt;
    struct syntiant_ndp120_dsp_debug_cnt_s dsp_dbg_cnt;
    struct syntiant_ndp120_flow_setup flow_rules;
    struct syntiant_ndp120_dsp_dev_data dsp_dev;
    struct syntiant_ndp120_sensor_config
           sensor_config[SYNTIANT_NDP120_SENSOR_MAX];
    struct syntiant_ndp120_sensor_state
           sensor_state[SYNTIANT_NDP120_SENSOR_MAX];
};

/**
 * @brief pdm clock start, stop options
 */
enum syntiant_ndp120_tiny_clock_mode_e {
    SYNTIANT_NDP120_PDM_CLK_START_CLEAN     =   0x1,
    /**< Start clock for the first time*/
    SYNTIANT_NDP120_PDM_CLK_START_RESUME    =   0x2,
    /**< Resume clock */
    SYNTIANT_NDP120_PDM_CLK_START_PAUSE     =   0x3
    /**< Pause clock */
};

/**
 * @brief main clock options
 */
enum {
    SYNTIANT_NDP120_MAIN_CLK_SRC_EXT = 1,
    SYNTIANT_NDP120_MAIN_CLK_SRC_PLL = 2,
    SYNTIANT_NDP120_MAIN_CLK_SRC_FLL = 3
};

/**
 * @brief DSP restart command. Before turning on PDM clock, issue DSP restart
 * to initialize all the configurations in DSP firmware data structures
 */
enum syntiant_ndp120_tiny_mb_cmds {
    SYNTIANT_NDP120_DSP_RESTART   = 0x71
};

/*
 * Integration Interfaces
 */

/**
 * @brief handle/pointer to integration-specific device instance state
 *
 * An @c syntiant_ndp120_tiny_handle_t typically points to a structure containing
 * all information required to access and manipulate an NDP device instance
 * in a particular environment.  For example, a Linux user mode integration
 * might require a Linux file descriptor to access the hardware, or a
 * 'flat, embedded' integration might require a hardware memory address.
 *
 * It is not used directly by the NDP ILib, but rather is passed unchanged to
 * integration interface functions.
 */
typedef void *syntiant_ndp120_tiny_handle_t;

/**
 * @brief exchange bytes with NDP device integration interface
 *
 * A provider of @c syntiant_ndp120_tiny_transfer will send @p count bytes
 * to the device and retrieve @p count bytes.
 *
 * This interface supports an underlying underlying SPI bus connection
 * to NDP where equal numbers of bytes are sent and received for cases
 * where full duplex transfer is relevant.  Simplex transfer is accomplished
 * by setting either @p out or @p in to NULL as appropriate.
 *
 * The interface provider may (should) support burst style efficiency
 * optimizations even across transfers where appropriate.  For example,
 * a multidrop SPI bus can continue to leave the interface selected
 * if subsequent transfers pick up where the previous left off and the
 * NDP device has not been deselected for other reasons.
 *
 * Transfers to the MCU space are generally expected to be simplex and
 * out == NULL -> read operation, in == NULL -> write operation.
 *
 * @param d handle to the NDP device
 * @param mcu for MCU address space (attachment bus space otherwise)
 * @param addr starting address
 * @param out bytes to send, or NULL if '0' bytes should be sent
 * @param in bytes to receive, or NULL if received bytes should be ignored
 * @param count number of bytes to exchange
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
typedef int (*syntiant_ndp120_tiny_transfer_f)(syntiant_ndp120_tiny_handle_t d,
        int mcu, uint32_t addr, void *out, void *in, unsigned int count);

/**
 * @brief await NDP mailbox interrupt integration interface
 *
 * A provider of @c syntiant_ndp_mbwait must not return until the current
 * mailbox request is completed as indicated by @c syntiant_ndp_poll, or
 * an implementation-dependent timeout is reached.
 * The provider may yield control until an NDP interrupt is delivered, or
 * may poll by calling @c syntiant_ndp_poll periodically.
 *
 * If the provider of @c syntiant_ndp_mbwait yields control, it must
 * also perform the equivalent of a @c syntiant_ndp_unsync_f before
 * yielding and a @c syntiant_ndp_sync_f before returning.
 *
 * The client code may only call these functions during an mbwait:
 *   - @c syntiant_ndp120_tiny_poll
 *   - @c syntiant_ndp120_tiny_read
 *   - @c syntiant_ndp120_tiny_write
 *   - @c syntiant_ndp120_tiny_send_data
 *
 * @param d handle to the NDP device
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
typedef int (*syntiant_ndp120_tiny_mbwait_f)(syntiant_ndp120_tiny_handle_t d);

/**
 * @brief begin a critical section of NDP device access integration interface
 *
 * A provider of @c syntiant_ndp120_tiny_sync should prevent any other device
 * to be selected, in the sense lf @c syntiant_ndp_select, and should only
 * allow a single thread of execution between a call of @c syntiant_ndp_sync
 * and @c syntiant_ndp_unsync.
 * In a single core execution enviroment with interrupts and a shared SPI
 * interface, this typically consists of blocking interrupts for
 * all devices sharing the SPI interface, including the NDP.
 * In a polled environment with the NDP on a point-to-point SPI
 * interface it would be a NOP.
 *
 * @param d handle to the NDP device
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
typedef int (*syntiant_ndp120_tiny_sync_f)(syntiant_ndp120_tiny_handle_t d);

/**
 * @brief end a critical section of NDP device access integration interface
 *
 * A provider of @c syntiant_ndp_unsync should lift the restrictions
 * applied by @c syntiant_ndp_sync.
 * In a single core execution enviroment with interrupts, and NDP on a
 * shared SPI bus, this typically consists of enabling interrupts for
 * all devices sharing the SPI interface, including the NDP.
 * In a polled environment it would be a NOP.
 *
 * @param d handle to the NDP device
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
typedef int (*syntiant_ndp120_tiny_unsync_f)(syntiant_ndp120_tiny_handle_t d);

/**
 * @brief microsecond delay function
 *
 * A provider of @c syntiant_ndp_udelay will delay for the specified
 * number of microseconds, allowing this thread/process to sleep and
 * other threads/processes to execute.
 *
 * @param microseconds
 * @return a @c clib error code
 */
typedef int (*syntiant_ndp120_tiny_udelay_f)(unsigned int useconds);

/**
 * @brief integration interfaces
 */
struct syntiant_ndp120_tiny_integration_interfaces_s {
    syntiant_ndp120_tiny_handle_t d;           /**< device handle */
    syntiant_ndp120_tiny_transfer_f transfer;  /**< data transfer function */
    syntiant_ndp120_tiny_mbwait_f mbwait;      /**< mailbox wait function */
    syntiant_ndp120_tiny_sync_f sync;          /**< interrupt sync function */
    syntiant_ndp120_tiny_unsync_f unsync;      /**< interrupt unsync function */
    syntiant_ndp120_tiny_udelay_f udelay;      /**< microsecond delay function */
};

/**
 * @brief interrupt enable types
 */
enum syntiant_ndp_interrupt_e {
    SYNTIANT_NDP120_INTERRUPT_MATCH = 0x01,
    /**< external INT on match */
    SYNTIANT_NDP120_INTERRUPT_MAILBOX_IN = 0x02,
    /**< mbox response */
    SYNTIANT_NDP120_INTERRUPT_MAILBOX_OUT = 0x04,
    /**< mbox request */
    SYNTIANT_NDP120_INTERRUPT_DNN_FRAME = 0x08,
    /**< DNN frame */
    SYNTIANT_NDP120_INTERRUPT_FEATURE = 0x10,
    /**< filterbank completion */
    SYNTIANT_NDP120_INTERRUPT_ADDRESS_ERROR = 0x20,
    /**< SPI address error */
    SYNTIANT_NDP120_INTERRUPT_WATER_MARK = 0x40,
    /**< input buffer water mark */
    SYNTIANT_NDP120_INTERRUPT_SPI_READ_FAILURE = 0x80,
    /**< spi read failure */
    SYNTIANT_NDP120_INTERRUPT_ALL = 0xFF,
    SYNTIANT_NDP120_INTERRUPT_DEFAULT = 0x100
    /**< enable the chip-specific default interrupts */
};

/**
 * @brief data structure to maintain mailbox state of the device
 */
struct syntiant_ndp120_tiny_mb_state_s {
    uint8_t mbin_seq;               /**< MB in sequence data */
    uint8_t mbin_resp_seq;          /**< MB in response sequence data */
    uint8_t mbout_seq;              /**< MB out sequence data */
    uint8_t mbin_resync;            /**< MB in resync state */

    uint32_t watermarkint_state;    /**< Watermark state */
    uint32_t watermarkint;          /**< watermark interrupt */

    unsigned int mbin_state;        /**< MB in state */
    unsigned int mbin_sync_count;   /**< MB in sync event count */
    unsigned int mbin_data_count;   /**< MB in data event count */

    uint32_t mbin_data;             /**< MB in data */
    uint32_t watermarkint_data;     /**< Watermark interrupt data */

    uint8_t mbin_resp;              /**< MB in response */
    uint8_t mbout;                  /**< MB out */
    uint8_t watermarkint_resync;    /**< Watermark resung */
    uint8_t rsvd;                   /**< Unused */
};

/**
 * @brief data structure to maintain package download state
 */
struct syntiant_ndp120_tiny_dl_state_s {
    uint32_t window_lower;      /**< Address of the lower end of DL window */
    uint32_t window_upper;      /**< Address of the uppoer end of DL window */
    uint32_t window_idx;        /**< Window index during download */
    uint32_t remainder_len;
    /**< chunk remainder since MCU transfers must be multiples of 4 bytes */
    uint8_t remainder[4];       /**< Remainder data during download */
    int mode;                   /**< Package loading mode */
};

enum {
  SYNTIANT_NDP120_HOST_BOOT   = 0,
  SYNTIANT_NDP120_SERIAL_BOOT = 1
};
/**
 * @brief NDP interface library internal state object
 */
struct syntiant_ndp120_tiny_device_s {
    struct syntiant_ndp120_tiny_integration_interfaces_s *iif;
    struct syntiant_ndp120_tiny_dl_state_s dl_state;
    struct syntiant_ndp120_tiny_mb_state_s mb_state;
    uint32_t mcu_fw_pointers_addr; /**< 0 means MCU is not running */

    uint32_t mcu_fw_state_addr;
    uint32_t mcu_fw_ph_data_coll_addr;
    uint32_t mcu_fw_smax_smoother_addr;
    uint32_t mcu_fw_orchestrator_graph_addr;
    uint32_t mcu_fw_dbg_state_addr;
    uint32_t dsp_fw_state_addr;

    unsigned int dnn_input;
    unsigned int input_clock_rate;
    unsigned int core_clock_rate;
    unsigned int audio_frame_size;
    unsigned int audio_frame_step;
    unsigned int audio_sample_size_bytes;
    unsigned int dnn_frame_size;

    uint32_t fwver_len;
    uint32_t matches;
    uint32_t tankptr_last;
    uint32_t tankptr_match;

    uint32_t classes[SYNTIANT_NDP120_MAX_NNETWORKS];
    uint32_t match_producer[SYNTIANT_NDP120_MAX_NNETWORKS];
    uint32_t match_consumer[SYNTIANT_NDP120_MAX_NNETWORKS];

    uint8_t init;
    uint8_t pkg_load_flag;
    uint8_t num_networks;
    uint8_t last_network_id;

    uint8_t* ptr;
    uint8_t* open_ram_begin;
    uint8_t* open_ram_end;
    uint32_t match_frame_count;

    union {
        struct {
            uint32_t buffer_start;
            uint32_t prod_ptr_addr;
            uint32_t buffer_size_bytes;
            uint32_t audio_sample_size;
        } buffer_metadata;
        uint32_t meta_bytes[4];
    } u;
    uint32_t last_ptr;
    uint8_t boot_flag;
    uint8_t device_type;
    uint8_t device_id;
};

/**
 * @brief data structure for core clock configuration
 */
struct syntiant_ndp120_tiny_clk_config_data {
    uint32_t src;       /**< enum indicating ext, fll, or pll */
    uint32_t ref_freq;  /**< input frequency, valid only for fll or pll */
    uint32_t ref_type;  /**< clkpad or xtalin, valid only for fll or pll */
    uint32_t core_freq; /**< core clock frequency */
    uint32_t voltage;   /**< core voltage */
};

/**
 *  @Brief Represent results data corresponding to a match
 */
struct syntiant_ndp120_tiny_match_data {
    uint32_t tank_ptr;          /**< tank pointer at match */
    uint32_t summary;           /**< match summary */
    uint32_t match_frame_count; /**< match frame number */
    uint32_t match_producer;    /**< producer pointer for match */
    uint16_t total_nn;          /**< total number of NNs on the device */
    uint8_t nn_id;              /**< NN id for the match event */
    uint8_t sec_val_status;     /**< indicates secondary validation status */
    uint16_t num_classes;       /**< number of classes in a network */
    uint16_t activation_type;   /**< activation type */
    union {
        uint8_t u8[NDP120_RESULT_NUM_CLASSES];
        int8_t s8[NDP120_RESULT_NUM_CLASSES];
        int16_t s16[NDP120_RESULT_NUM_CLASSES];
    } raw_strengths;
};

/**
 *  @Brief Represent data corresponding to audio sample annotation
 */
struct syntiant_ndp120_tiny_dsp_audio_sample_annotation_t {
    uint8_t src_type;
    uint8_t src_param;
    uint8_t monotonic_cnt;
    uint8_t _dummy0; /* maintain 4-align */
};

#define SYNTIANT_NDP_SENSOR_ID_NAMES                                        \
    {                                                                       \
        "none", "bmi160", "vm3011", "edge_int", "da217", "kx120", "mc3419", \
            "gpio_debounce", "spl_threshold", "t5838" \
    }
#define SYNTIANT_NDP_SENSOR_ID_NAME(id)                                       \
    ((SYNTIANT_NDP120_SENSOR_ID_LAST < (id))  \
            ? "*unknown*"                                                      \
            : syntiant_ndp_sensor_id_names[id])

#define SYNTIANT_NDP_ERROR_NAMES                                               \
    {                                                                          \
        "none", "fail", "arg", "uninit", "package", "unsup", "nomem", "busy",  \
            "timeout", "more", "config", "crc", "inv_net", "reread", "pbi_tag",\
            "pbi_ver", "invalid_length", "dsp_hdr_crc" \
    }
#define SYNTIANT_NDP_ERROR_NAME(e)                                             \
    (((e) < SYNTIANT_NDP120_ERROR_NONE || SYNTIANT_NDP120_ERROR_LAST < (e))    \
            ? "*unknown*"                                                      \
            : syntiant_ndp_error_names[e])

/**
 * @brief polling loop timeout iteration count
 */
enum poll_wait_timeout {
    SYNTIANT_NDP_POLL_TIMEOUT_COUNT = 10000U
};

/**
 * @brief dsp data flow src types
 */
enum {
    SYNTIANT_NDP120_DSP_DATA_FLOW_SRC_TYPE_PCM_AUDIO,
    SYNTIANT_NDP120_DSP_DATA_FLOW_SRC_TYPE_FUNCTION,
    SYNTIANT_NDP120_DSP_DATA_FLOW_SRC_TYPE_NN,
    SYNTIANT_NDP120_DSP_DATA_FLOW_SRC_TYPE_MCU
};

/**
 * @brief error codes
 */
enum syntiant_ndp_errors_e {
    SYNTIANT_NDP120_ERROR_NONE = 0,
    /**< operation successful */
    SYNTIANT_NDP120_ERROR_FAIL = 1,
    /**< general failure */
    SYNTIANT_NDP120_ERROR_ARG = 2,
    /**< invalid argument error */
    SYNTIANT_NDP120_ERROR_UNINIT = 3,
    /**< device unintialized or no fw loaded */
    SYNTIANT_NDP120_ERROR_UNSUP = 5,
    /**< operation not supported */
    SYNTIANT_NDP120_ERROR_NOMEM = 6,
    /**< out of memory */
    SYNTIANT_NDP120_ERROR_BUSY = 7,
    /**< operation in progress */
    SYNTIANT_NDP120_ERROR_TIMEOUT = 8,
    /**< operation timeout */
    SYNTIANT_NDP120_ERROR_MORE = 9,
    /**< more data is expected */
    SYNTIANT_NDP120_ERROR_CONFIG = 10,
    /**< config error */
    SYNTIANT_NDP120_ERROR_DATA_REREAD = 13,
    /**<data has already been read before */
    SYNTIANT_NDP120_ERROR_INVALID_LENGTH  = 16,
    /**< bad version length */
    SYNTIANT_NDP120_ERROR_DSP_NO_VAD_MIC = 18,
    /**< no vad mic in dsp */
    SYNTIANT_NDP120_ERROR_LAST = SYNTIANT_NDP120_ERROR_DSP_NO_VAD_MIC
};

/**
 * @brief input data types to a network
 */
enum syntiant_ndp120_tiny_send_data_type_e {
    SYNTIANT_NDP120_SEND_DATA_TYPE_STREAMING = 0x00,
    /**< streaming vector input features */
    SYNTIANT_NDP120_SEND_DATA_TYPE_FEATURE_STATIC  = 0x01,
    /**< send to static features */
    SYNTIANT_NDP120_NDP_SEND_DATA_TYPE_LAST =
        SYNTIANT_NDP120_SEND_DATA_TYPE_FEATURE_STATIC
};

/**
 * @brief size of the ring containing matches in the firmware
 */
enum {
    SYNTIANT_NDP120_MATCH_RING_SIZE = 2
};

/**
 * @brief notification causes (device to host)
 */
enum syntiant_ndp_notification_e {
    SYNTIANT_NDP120_NOTIFICATION_ERROR = 0x01,
    /**< unexpected error */
    SYNTIANT_NDP120_NOTIFICATION_MATCH = 0x02,
    /**< algorithm match reported */
    SYNTIANT_NDP120_NOTIFICATION_DNN = 0x04,
    /**< Interrupt for DNN reported */
    SYNTIANT_NDP120_NOTIFICATION_MAILBOX_IN = 0x08,
    /**< host mailbox request complete */
    SYNTIANT_NDP120_NOTIFICATION_MAILBOX_OUT = 0x10,
    /**< ndp mailbox request start */
    SYNTIANT_NDP120_NOTIFICATION_WATER_MARK = 0x20,
    /**< input buffer water mark reached  */
    SYNTIANT_NDP120_NOTIFICATION_FEATURE = 0x40,
    /**< fequency domain processing completion event */
    SYNTIANT_NDP120_NOTIFICATION_EXTRACT_READY = 0X80,
    /**< Data ready to be extracted from DSP */
    SYNTIANT_NDP120_NOTIFICATION_ALGO_ERROR = 0x100,
    /**< algorithm error */
    SYNTIANT_NDP120_NOTIFICATION_DEBUG = 0x200,
    /**< debug */
    SYNTIANT_NDP120_NOTIFICATION_DSP_NOMEM_ERROR = 0x400,
    /**< DSP memory unavailable error */
    SYNTIANT_NDP120_NOTIFICATION_DNN_NOMEM_ERROR = 0x800,
    /**< DNN memory unavailable error */
    SYNTIANT_NDP120_NOTIFICATION_DSP_RUNNING = 0x1000,
    /**< DSP running state notification */
    SYNTIANT_NDP120_NOTIFICATION_BOOTING = 0x2000,
    /**< DSP running state notification */
    SYNTIANT_NDP120_NOTIFICATION_OTHER_LOAD_DONE = 0x4000,
    /**< Other load done notification  */
    SYNTIANT_NDP120_NOTIFICATION_DSP_NO_VAD_MIC_ERROR = 0x8000,
    /**< DSP no vad mic error */
    SYNTIANT_NDP120_NOTIFICATION_ALL_M
    = ((SYNTIANT_NDP120_NOTIFICATION_DSP_NO_VAD_MIC_ERROR << 1) - 1)
};

/**
 * @brief mailbox op codes (host to device)
 */
enum {
    SYNTIANT_NDP120_MB_MCU_NOP = 0x0,
    /**< send a NOP to the device */
    SYNTIANT_NDP120_MB_MCU_ACK = SYNTIANT_NDP120_MB_MCU_NOP,
    /**< send an ack during data transfer */
    SYNTIANT_NDP120_MB_MCU_CONT = 0x1,
    /**< device sends a match event to the host */
    SYNTIANT_NDP120_MB_MCU_DATA = 0x4,
    /**< command for data transfer to the device */
    SYNTIANT_NDP120_MB_MCU_CMD = 0x6,
    /**< extended mailbox command */
    SYNTIANT_NDP120_MB_MCU_MIADDR = 0x8,
    /**< fetch MCU firmware base address */
    SYNTIANT_NDP120_MB_MCU_LOAD = 0x9,
    /**< load a package */
    SYNTIANT_NDP120_MB_DSP_ADX_UPPER = 0x0B,
    /**< fetch dsp firmware base address (upper nibble) */
    SYNTIANT_NDP120_MB_DSP_ADX_LOWER = 0x0C
    /**< fetch lower nibble of the dsp firmware base address */
};

/**
 * @brief ndp initialization modes
 */
enum {
    /**< reset the chip in init/uninit */
    SYNTIANT_NDP_INIT_MODE_RESET = 0
};

/**
 * @brief data extraction ranges
 */
enum syntiant_ndp_extract_from_e {
     SYNTIANT_NDP120_EXTRACT_FROM_MATCH = 0x00,
    /**< from 'length' older than a match.  The matching extraction position
       is only updated when @c syntiant_ndp_get_match_summary() is called
       and reports a match has been detected */
     SYNTIANT_NDP120_EXTRACT_FROM_UNREAD = 0x01,
    /**< from the oldest unread */
     SYNTIANT_NDP120_EXTRACT_FROM_OLDEST = 0x02,
    /**< from the oldest recorded */
     SYNTIANT_NDP120_EXTRACT_FROM_NEWEST = 0x03,
    /**< from the newest recorded */
     SYNTIANT_NDP120_NDP_EXTRACT_FROM_LAST =  SYNTIANT_NDP120_EXTRACT_FROM_NEWEST
};

enum syntiant_sc2_device_type {
    SYNTIANT_NDP115_A0 = 0,
    /**< NDP115A0 device */
    SYNTIANT_NDP120_B0
    /**< NDP120B0 device */
};

/**
 * @brief mcu/spi op values for read/write functions
 */
enum syntiant_ndp_read_write_mcu_spi_select {
    SYNTIANT_NDP120_SPI_OP = 0,
    SYNTIANT_NDP120_MCU_OP = 1
};

enum {
    /* gpio */
    SYNTIANT_NDP120_CONFIG_SET_GPIO_DIR                  = 0x01,
    SYNTIANT_NDP120_CONFIG_SET_GPIO_VALUE                = 0x02,
    SYNTIANT_NDP120_CONFIG_VALUE_GPIO_DIR_OUT            = 0x00,
    SYNTIANT_NDP120_CONFIG_VALUE_GPIO_DIR_IN             = 0x01
};

struct syntiant_ndp120_config_gpio_s{
    uint32_t gpio_num;
    uint32_t dir;
    uint32_t value;
};

/**
 * @brief configure clock
 *
 * Start, Stop, or Resume the PDM clock execution.
 *
 * @param ndp NDP state object
 * @param data for clock configuration
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_clock_cfg(struct syntiant_ndp120_tiny_device_s *ndp,
        struct syntiant_ndp120_tiny_clk_config_data *cfg);

/**
 * @brief get info from the device
 *
 * @param ndp NDP state object
 * @param pointer to caller provided info data structure
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_get_info(struct syntiant_ndp120_tiny_device_s *ndp,
        struct syntiant_ndp120_tiny_info *idata);

/**
 * @brief NDP120 get match summary.
 * Deprecation Warning: This function will be removed in SDK v97 release.
 * Instead, use syntiant_ndp120_tiny_get_match_result() API.
 *
 * Gets the match summary from mcu firmware state structure or from register
 * NDP120_SPI_MATCHSTS.
 *
 * @param ndp NDP state object
 * @param summary indicates summary of particular label match detection.
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_get_match_summary(
        struct syntiant_ndp120_tiny_device_s *ndp, uint32_t *summary);

/**
 * @brief NDP120 get match result.
 *
 * Gets the match summary and raw strengths from mcu firmware state
 * structure or from register NDP120_SPI_MATCHSTS.
 *
 * @param ndp NDP state object
 * @param match tiny match data object.
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_get_match_result(
        struct syntiant_ndp120_tiny_device_s *ndp,
        struct syntiant_ndp120_tiny_match_data *match);

/**
 * @brief NDP120 send data.
 *
 * Writes sample data into sample register SPI_SAMPLE.
 *
 * @param ndp NDP state object
 * @param data represents 8 bit of data.
 * @param length indicates the length of data.
 * @param type indicates the type whether it is NDP_SEND_DATA_TYPE_STREAMING
 * @param offset.
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_send_data(struct syntiant_ndp120_tiny_device_s *ndp,
        uint8_t *data, unsigned int len, int type, uint32_t offset);

/**
 * @brief NDP120 load packages.
 *
 * Loads the length number of chunk data into device.
 *
 * @param ndp NDP state object
 * @param chunk inicates the particular chunk number of data.
 * @param len indicates the length of the chunk.
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_load(struct syntiant_ndp120_tiny_device_s *ndp,
        void *chunk, unsigned int len);

/**
 * @brief NDP120 read data.
 *
 * Reads data from the address specified by address field.
 *
 * @param ndp NDP state object
 * @param mcu indicates whether we are reading  mcu address or not.
 * @param address indicates the location from where the data is to be read.
 * @param value gets updated with the data.
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int
syntiant_ndp120_tiny_read(struct syntiant_ndp120_tiny_device_s *ndp, int mcu,
    uint32_t address, void *value);

/**
 * @brief NDP120 read block of data.
 *
 * Reads the count number of bytes of data from the address specified by
 * address field.
 *
 * @param ndp NDP state object
 * @param mcu indicates whether we are reading  mcu address or not.
 * @param address indicates the location from where the data is to be read.
 * @param value gets updated with the data.
 * @param count indicates the length of data.
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_read_block(struct syntiant_ndp120_tiny_device_s *ndp,
        int mcu, uint32_t address, void *value, unsigned int count);

/**
 * @brief NDP120 write data.
 *
 * Writes the data from the param value into the address specified by address field.
 *
 * @param ndp NDP state object
 * @param mcu indicates whether we are writing into mcu address or not.
 * @param address indicates the location to where the data is to be written.
 * @param value indicates the data.
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int
syntiant_ndp120_tiny_write(struct syntiant_ndp120_tiny_device_s *ndp, int mcu,
    uint32_t address, uint32_t value);

/**
 * @brief NDP120 write block of data.
 *
 * Writes the count number of bytes of data from the param value into the address specified by address field.
 *
 * @param ndp NDP state object
 * @param mcu indicates whether we are writing into mcu address or not.
 * @param address indicates the location to where the data is to be written.
 * @param value indicates the data.
 * @param count indicates the length of data.
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_write_block(struct syntiant_ndp120_tiny_device_s *ndp,
        int mcu, uint32_t address, void *value, unsigned int count);

/**
 * @brief NDP120 set the PDM clock
 *
 * Start, Stop, or Resume the PDM clock execution.
 *
 * @param ndp NDP state object
 * @param execution_mode - syntiant_ndp120_tiny_clock_mode_e
 *          Indicates whether this is to start, stop, or resume.
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_pdm_clock_exe_mode(
        struct syntiant_ndp120_tiny_device_s *ndp, uint8_t execution_mode);

/**
 * @brief NDP120 initialization
 *
 * Initialize NDP120 in requested RESET mode
 *
 * @param ndp       NDP state object
 * @param init_mode indicating type of the mode.
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_init(struct syntiant_ndp120_tiny_device_s *ndp,
        struct syntiant_ndp120_tiny_integration_interfaces_s *iif,
        int init_mode);

/**
 * @brief NDP120 Poll for interrupts.
 *
 * Check for interrupts triggering and updates notifications param
 * corresponding to interrupt triggerred.
 *
 * @param ndp NDP state object
 * @param notifications indicates bitmask of interrupts triggered.
 * @param clear if clear param is 1, then updates the SPI_INTSTS register.
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_poll(struct syntiant_ndp120_tiny_device_s *ndp,
        uint32_t *notifications, int clear);

/**
 * @brief NDP120 extract data.
 *
 * Extract data from the dsp firmware buffers.
 *
 * @param ndp NDP state object
 * @param type indicates data types like audio(input/annotated), feature.
 * @param from indicates from where to extract like FROM_MATCH, FROM_NEWEST,
 *        FROM_OLDEST, FROM_UNREAD.
 * @param data represents the buffer and gets updated with data.
 * @param lenp indicates the length of data. The inherent assumption is
 *        that the user provided buffer is at least as large as one frame's data
 * @return a @c SYNTIANT_NDP_ERROR_* code. The return value could be
 *        SYNTIANT_NDP_ERROR_DATA_REREAD, which indicates that enough data is
 *        not available and caller might retry.
 */
int syntiant_ndp120_tiny_extract_data(struct syntiant_ndp120_tiny_device_s *ndp,
                        uint8_t *data, unsigned int *lenp, int sample_interrupt);

/**
 * @brief NDP120 interrupts enable
 *
 * Enables the differrent NDP120 interrupts by writing into SPI_INTCTL register.
 *
 * @param ndp NDP state object
 * @param causes indicates the mask of interrupts you want to enable.
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_interrupts(struct syntiant_ndp120_tiny_device_s *ndp,
        int *causes);

/**
 * @brief NDP120 send mailbox command
 *
 * @param ndp NDP state object
 * @param causes indicates the mask of interrupts you want to enable.
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_mb_cmd(struct syntiant_ndp120_tiny_device_s *ndp,
        uint8_t req, uint32_t *msg);

/**
 * @brief NDP120 get debug info
 *
 * @param ndp NDP state object
 * @param mcu_dsp_dbg_cnts caller provided data struct for fetching debug info
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_get_debug(struct syntiant_ndp120_tiny_device_s *ndp,
        struct syntiant_ndp120_tiny_debug *mcu_dsp_dbg_cnts);


/**
 * @brief NDP120 get audio chunk size
 *
 * @param ndp NDP state object
 * @param audio_chunk_size caller provided integer pointer for
 * fetching chunk size
 *
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */

int syntiant_ndp120_tiny_get_audio_chunk_size(
    struct syntiant_ndp120_tiny_device_s *ndp, uint32_t *audio_chunk_size);

/**
 * @brief NDP120 get recording metadata like sample buffer addresses
 * and audio sample size
 * Enables the sample ready interrupt in mcu firmware
 *
 * @param ndp NDP state object
 *
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_get_recording_metadata(
    struct syntiant_ndp120_tiny_device_s *ndp);


/**
 * @brief NDP120 audio extract start enables the match_per_frame interrupt
 * for audio extraction.
 *
 * @param ndp NDP state object
 * @param extract_from indicates the extraction point like
 * from MATCH|UNREAD|NEWEST
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */

int syntiant_ndp120_tiny_audio_extract_start(
    struct syntiant_ndp120_tiny_device_s *ndp, uint32_t extract_from);

/**
 * @brief NDP120 audio extract stop disables the match_per_frame interrupt
 * after audio extraction
 *
 * @param ndp NDP state object
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */

int syntiant_ndp120_tiny_audio_extract_stop(
    struct syntiant_ndp120_tiny_device_s *ndp);

/**
 * @brief NDP120 tiny spi direct config
 *
 * @param ndp NDP state object
 * @param threshod_bytes indicates the threshold bytes for SPI FIFO in dsp.
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_spi_direct_config(
    struct syntiant_ndp120_tiny_device_s *ndp, uint32_t threshold_bytes,
    uint32_t *fifo_threshold_value);

/**
 * @brief NDP120 tiny input config sets the input mode of the dsp.
 *
 * @param ndp NDP state object
 * @param input_mode indicates the input mode like PDM or SPI.
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_input_config(struct syntiant_ndp120_tiny_device_s *ndp,
             uint32_t input_mode);

/**
 * @brief NDP120 tiny_switch_dnn_flow changes the flow of the dsp indicated
 * by flow_set_id.
 *
 * @param ndp NDP state object
 * @param flow_set_id indicates the flow set id that is to be changed.
 * @param input_mode gets the input mode after changing the flow.
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_switch_dnn_flow(
    struct syntiant_ndp120_tiny_device_s *ndp, uint32_t flow_set_id,
    uint32_t *input_mode);

/**
 * @brief NDP120 syntiant_ndp120_tiny_dsp_tank_size_config changes the tank
 * size of the dsp indicated by pcm_tank_size_in_msec.
 *
 * @param ndp NDP state object
 * @param pcm_tank_size_in_msec indicates the tank size that is to be changed.
 * @param pcm_tank_size_in_msec gets the actual tank size after configuration.
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_dsp_tank_size_config(struct
    syntiant_ndp120_tiny_device_s *ndp, uint32_t pcm_tank_size_in_msec);

/**
 * @brief NDP120 syntiant_ndp120_tiny_vad_mic_control changes
 * vad mic operating modes (e.g. vad or always on mode, etc).
 *
 * @param ndp NDP state object
 * @param vad_mic_control controls vad mic operating mode
 * @return a @c SYNTIANT_NDP_ERROR_* code
 */
int syntiant_ndp120_tiny_vad_mic_control(struct
    syntiant_ndp120_tiny_device_s *ndp, uint32_t vad_mic_control);

/**
 * @brief halt mcu
 * @param ndp NDP state object
 */
int syntiant_ndp120_tiny_halt_mcu(
    struct syntiant_ndp120_tiny_device_s *ndp);

/**
 * @brief invoke boot from flash
 * @param ndp NDP state object
 */
int syntiant_ndp120_tiny_boot_from_flash(
    struct syntiant_ndp120_tiny_device_s *ndp);

/**
 * @brief invoke soft boot from flash
 * @param ndp NDP state object
 */
int syntiant_ndp120_tiny_soft_flash_boot(
    struct syntiant_ndp120_tiny_device_s *ndp);

/**
 * @brief poll for notification
 * @param ndp NDP state object
 * @param notification bitmask
 */
int syntiant_ndp120_tiny_poll_notification(
    struct syntiant_ndp120_tiny_device_s *ndp, uint32_t notification_mask);

/**
 * @brief Issue DSP restart via MCU firmware
 * @param ndp NDP state object
 */
int syntiant_ndp120_tiny_dsp_restart(
    struct syntiant_ndp120_tiny_device_s *ndp);

int syntiant_ndp120_tiny_config_gpio(
    struct syntiant_ndp120_tiny_device_s *ndp, 
    struct syntiant_ndp120_config_gpio_s *config);

#ifdef __cplusplus
}
#endif
#endif /* SYNTIANT_NDP120_H */
