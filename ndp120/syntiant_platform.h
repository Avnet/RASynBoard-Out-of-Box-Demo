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


/** @brief debug counters of MCU firmware
 */
struct ndp_core2_debug_cnt_s {
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

enum {
    NDP_CORE2_MAX_NNETWORKS = 4,
    /**< maximum number of neural networks on device */
    NDP_CORE2_MAX_CLASSES = 32
    /**< mabimum number of classes in a network */
};

/** @brief debug counters of DSP firmware
 */
struct ndp_core2_dsp_debug_cnt_s {
    uint32_t frame_cnt;         /* processed frames             */
    uint32_t dnn_int_cnt;       /* dnn interrupts               */
    uint32_t dnn_err_cnt;       /* dnn error counts             */
    uint32_t h2d_mb_cnt;        /* host   -> DSP mb             */
    uint32_t d2m_mb_cnt;        /* DSP    -> MCU mb             */
    uint32_t m2d_mb_cnt;        /* MCU    -> DSP mb             */
    uint32_t watermark_cnt;     /* DSP WM -> host               */
    uint32_t fifo_overflow_cnt; /* FW detected fifo overflow    */
    uint32_t nn_cycle_cnt[NDP_CORE2_MAX_NNETWORKS];  /* cycle count per NN */
    uint32_t nn_run_cnt[NDP_CORE2_MAX_NNETWORKS];  /* DNN run count per NN */
};


struct ndp_core2_flow_rule {
    uint32_t src_param;         /* src parameter, e.g. channel number */
    uint32_t dst_param;         /* destination parameter */
    uint8_t  dst_type;          /* DSP function, NN, or post processor */
    int8_t   algo_config_index; /* -1 means "none" */
    uint8_t  set_id;            /* Id of the flowset for this flow rule */
    uint8_t _dummy0;
};

#define NDP_CORE2_PCM_DATA_FLOW_RULE_MAX 16
#define NDP_CORE2_FUNC_DATA_FLOW_RULE_MAX 32
#define NDP_CORE2_NN_DATA_FLOW_RULE_MAX 32
#define NDP_CORE2_MCU_DATA_FLOW_RULE_MAX 8

/* pass in a _rule_ */
#define NDP_CORE2_DSP_DATA_FLOW_RULE_DST_STR(x) \
    (x).dst_type == NDP_CORE2_DSP_DATA_FLOW_DST_TYPE_NONE ? "<INVALID>" : \
    (x).dst_type == NDP_CORE2_DSP_DATA_FLOW_DST_TYPE_FUNCTION ? "FUNC" : \
    (x).dst_type == NDP_CORE2_DSP_DATA_FLOW_DST_TYPE_NN ? "NN" : \
    (x).dst_type == NDP_CORE2_DSP_DATA_FLOW_DST_TYPE_HOST_EXTRACT ? \
    ((x).dst_param ==  NDP_CORE2_DSP_DATA_FLOW_DST_SUBTYPE_AUDIO ? "HOST_EXT_AUDIO" : "HOST_EXT_FEATURE") : \
    (x).dst_type == NDP_CORE2_DSP_DATA_FLOW_DST_TYPE_I2S ? "I2S" : \
    (x).dst_type == NDP_CORE2_DSP_DATA_FLOW_DST_TYPE_MCU ? "MCU" : "UNKNOWN"

struct ndp_core2_flow_setup {
    struct ndp_core2_flow_rule src_pcm_audio[NDP_CORE2_PCM_DATA_FLOW_RULE_MAX];
    /**< flow rule array for PCM audio source */
    struct ndp_core2_flow_rule src_function[NDP_CORE2_FUNC_DATA_FLOW_RULE_MAX];
    /**< flow rule array for DSP algo source */
    struct ndp_core2_flow_rule src_nn[NDP_CORE2_NN_DATA_FLOW_RULE_MAX];
    /**< flow rule array for NN source */
    struct ndp_core2_flow_rule src_mcu[NDP_CORE2_MCU_DATA_FLOW_RULE_MAX];
    /**< flow rule array for MCU firmware source */
};

enum {
    NDP_CORE2_DSP_MEM_TYPE_HEAP = 1,
    NDP_CORE2_DSP_MEM_TYPE_DNN_DATA
};

enum {
    NDP_CORE2_DSP_DATA_FLOW_DST_TYPE_NONE,
    NDP_CORE2_DSP_DATA_FLOW_DST_TYPE_FUNCTION,
    NDP_CORE2_DSP_DATA_FLOW_DST_TYPE_NN,
    NDP_CORE2_DSP_DATA_FLOW_DST_TYPE_HOST_EXTRACT,
    NDP_CORE2_DSP_DATA_FLOW_DST_TYPE_I2S,
    NDP_CORE2_DSP_DATA_FLOW_DST_TYPE_MCU
    /* If you update this, update the _STR
       macro above */
};

enum {
    NDP_CORE2_DSP_SAMPLE_TYPE_PCM_AUDIO,
    NDP_CORE2_DSP_SAMPLE_TYPE_FUNCTION
};
enum {
    NDP_CORE2_DSP_DATA_FLOW_DST_SUBTYPE_AUDIO = NDP_CORE2_DSP_SAMPLE_TYPE_PCM_AUDIO,
   NDP_CORE2_DSP_DATA_FLOW_DST_SUBTYPE_FEATURE = NDP_CORE2_DSP_SAMPLE_TYPE_FUNCTION
};

struct ndp_core2_dsp_dev_data {
    uint8_t last_network;           /* Last network executed */
    uint8_t db_measured;            /* Decibel level measured */
    uint8_t sampletank_mem_type;    /* Memoty type for sample tank */
    uint8_t device_state;           /* Device state flag */
};

struct ndp_core2_tiny_debug {
    struct ndp_core2_debug_cnt_s mcu_dbg_cnt;
    struct ndp_core2_dsp_debug_cnt_s dsp_dbg_cnt;
    struct ndp_core2_flow_setup flow_rules;
    struct ndp_core2_dsp_dev_data dsp_dev;
};

enum {
    /* gpio */
    NDP_CORE2_CONFIG_VALUE_GPIO_DIR_OUT            = 0x00,
    NDP_CORE2_CONFIG_VALUE_GPIO_DIR_IN             = 0x01
};

enum {
    NDP_CORE2_CONFIG_CLOCK_OPTION_PLL               = 0,
    NDP_CORE2_CONFIG_CLOCK_OPTION_FLL               = 1,
    NDP_CORE2_CONFIG_CLOCK_OPTION_EXT               = 2,
    NDP_CORE2_CONFIG_CLOCK_OPTION_NONE              = -1
};


extern int ndp_core2_platform_tiny_start(uint8_t clock_option, int use_xtal);
extern int ndp_core2_platform_tiny_feature_set(int feature_flag);

extern int ndp_core2_platform_tiny_transfer(int mcu, uint32_t addr, 
        void *out, void *in, unsigned int count);

extern int ndp_core2_platform_tiny_poll(uint32_t *notifications, int clear);
extern int ndp_core2_platform_tiny_match_process(uint8_t *match_id, uint8_t *nn_id, 
        char *label_string);
        
extern int ndp_core2_platform_tiny_mspi_config(void);
extern int ndp_core2_platform_tiny_mspi_read(int ssb, int num_bytes, 
        uint8_t* data, int end_packet);
extern int ndp_core2_platform_tiny_mspi_write(int ssb, int num_bytes, 
        uint8_t* data, int end_packet);

extern int ndp_core2_platform_tiny_halt_mcu(void);
extern int ndp_core2_platform_tiny_vadmic_ctl(int mode);

extern int ndp_core2_platform_tiny_get_samplesize(uint32_t *sample_size);
extern uint32_t ndp_core2_platform_tiny_get_samplebytes(void);
extern uint32_t ndp_core2_platform_tiny_get_samplerate(void);
extern int ndp_core2_platform_tiny_get_extract(void);
extern int ndp_core2_platform_tiny_extract_start(void);
extern int ndp_core2_platform_tiny_extract_stop(void);
extern int ndp_core2_platform_tiny_extract_data(uint8_t *extract_data, 
        uint32_t *extract_len);

extern int ndp_core2_platform_tiny_get_info(char *ndp_info_data, 
        uint32_t *total_nn, uint32_t *labels_len);

extern int ndp_core2_platform_gpio_config(int gpio_num, 
        uint32_t dir, uint32_t value);

#ifdef __cplusplus
}
#endif

#endif //end of _SYNTIANT_PLATFORM_H_
