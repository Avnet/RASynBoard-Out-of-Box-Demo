#ifndef RM_BLOCK_MEDIA_CUSTOM_H
#define RM_BLOCK_MEDIA_CUSTOM_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "rm_block_media_api.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/*******************************************************************************************************************//**
 * @addtogroup RM_BLOCK_MEDIA_CUSTOM
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/** Instance control block.  This is private to the FSP and should not be used or modified by the application. */
typedef struct st_rm_block_media_sd_instance_ctrl
{
//    uint32_t open;
    rm_block_media_cfg_t const * p_cfg;
    uint32_t sector_count;
    uint32_t sector_size_bytes;
    bool     initialized;
    bool     write_protected;
} rm_block_media_sd_instance_ctrl_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
/* Global variables related to custom block media*/
extern const rm_block_media_api_t g_rm_block_media_on_user_media;
extern void r_usb_pmsc_block_media_event_callback(rm_block_media_callback_args_t *p_args);
extern volatile bool g_blockmedia_complete_event;

/**********************************************************************************************************************
 * Function Prototypes
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_SD_Open(rm_block_media_ctrl_t * const p_ctrl, rm_block_media_cfg_t const * const p_cfg);
fsp_err_t RM_BLOCK_MEDIA_SD_MediaInit(rm_block_media_ctrl_t * const p_ctrl);
fsp_err_t RM_BLOCK_MEDIA_SD_Read(rm_block_media_ctrl_t * const p_ctrl,
                                    uint8_t * const               p_dest_address,
                                    uint32_t const                block_address,
                                    uint32_t const                num_blocks);
fsp_err_t RM_BLOCK_MEDIA_SD_Write(rm_block_media_ctrl_t * const p_ctrl,
                                     uint8_t const * const         p_src_address,
                                     uint32_t const                block_address,
                                     uint32_t const                num_blocks);
fsp_err_t RM_BLOCK_MEDIA_SD_Erase(rm_block_media_ctrl_t * const p_ctrl,
                                     uint32_t const                block_address,
                                     uint32_t const                num_blocks);
fsp_err_t RM_BLOCK_MEDIA_SD_StatusGet(rm_block_media_ctrl_t * const   p_api_ctrl,
                                         rm_block_media_status_t * const p_status);
fsp_err_t RM_BLOCK_MEDIA_SD_InfoGet(rm_block_media_ctrl_t * const p_ctrl, rm_block_media_info_t * const p_info);
fsp_err_t RM_BLOCK_MEDIA_SD_Close(rm_block_media_ctrl_t * const p_ctrl);

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 // RM_BLOCK_MEDIA_CUSTOM_H

/*******************************************************************************************************************//**
 * @} (end addtogroup RM_BLOCK_MEDIA_CUSTOM)
 **********************************************************************************************************************/
