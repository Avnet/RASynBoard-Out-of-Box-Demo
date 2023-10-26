#include "fat_load.h"
#include "diskio.h"
#include "rm_block_media_custom.h"
#include "common_data.h"

/* Instance structure to use this module. */
static rm_block_media_sd_instance_ctrl_t g_rm_block_media0_ctrl;

const rm_block_media_cfg_t g_rm_block_media_cfg =
{ .p_extend = NULL, .p_callback = r_usb_pmsc_block_media_event_callback, .p_context = NULL, };

rm_block_media_instance_t g_rm_block_media =
{ .p_api = &g_rm_block_media_on_user_media, .p_ctrl = &g_rm_block_media0_ctrl, .p_cfg = &g_rm_block_media_cfg, };

/* Global function prototypes */
extern DRESULT SD_disk_write(BYTE pdrv, const BYTE* buff, DWORD sector, UINT count);
extern DRESULT SD_disk_read(BYTE pdrv, BYTE* buff, DWORD sector, UINT count);

/*******************************************************************************************************************//**
 * @addtogroup usb_composite_ep
 * @{
 **********************************************************************************************************************/

/* Global variables */
const rm_block_media_api_t g_rm_block_media_on_user_media =
{
    .open       = RM_BLOCK_MEDIA_SD_Open,
    .mediaInit  = RM_BLOCK_MEDIA_SD_MediaInit,
    .read       = RM_BLOCK_MEDIA_SD_Read,
    .write      = RM_BLOCK_MEDIA_SD_Write,
    .erase      = RM_BLOCK_MEDIA_SD_Erase,
    .infoGet    = RM_BLOCK_MEDIA_SD_InfoGet,
    .statusGet  = RM_BLOCK_MEDIA_SD_StatusGet,
    .close      = RM_BLOCK_MEDIA_SD_Close,
};

/*******************************************************************************************************************//**
 * Opens the module.
 *
 * Implements @ref rm_block_media_api_t::open().
 *
 * @retval     FSP_SUCCESS                     Module is available and is now open.
 * @retval     Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_SD_Open (rm_block_media_ctrl_t * const p_ctrl, rm_block_media_cfg_t const * const p_cfg)
{
    rm_block_media_sd_instance_ctrl_t * p_instance_ctrl = (rm_block_media_sd_instance_ctrl_t *) p_ctrl;

    p_instance_ctrl->p_cfg = p_cfg;
    /* This module is now open. */
    p_instance_ctrl->initialized     = false;
    p_instance_ctrl->write_protected = false;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Initializes the media. This function blocks until all identification and configuration commands are complete.
 *
 * Implements @ref rm_block_media_api_t::mediaInit().
 *
 * @retval     FSP_SUCCESS               Module is initialized and ready to access the memory device.
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_SD_MediaInit (rm_block_media_ctrl_t * const p_ctrl)
{
    rm_block_media_sd_instance_ctrl_t * p_instance_ctrl = (rm_block_media_sd_instance_ctrl_t *) p_ctrl;

    if (SDCARD_IN_SLOT == get_sdcard_slot_status()) {
        p_instance_ctrl->sector_count      = get_sdcard_total_sectors();
        p_instance_ctrl->sector_size_bytes = 512;
    }
    p_instance_ctrl->initialized       = true;

    /* set the block media complete event flag.*/
    g_blockmedia_complete_event = true;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Reads data from a media.
 * Implements @ref rm_block_media_api_t::read().
 *
 * @retval     FSP_SUCCESS                   Data read successfully.
 * @retval     Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_SD_Read (rm_block_media_ctrl_t * const p_ctrl,
                                     uint8_t * const               p_dest_address,
                                     uint32_t const                block_address,
                                     uint32_t const                num_blocks)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);

    xSemaphoreTake(g_sd_mutex,portMAX_DELAY);
    /* Call the underlying driver. */
    SD_disk_read(0, p_dest_address, block_address, num_blocks);
    xSemaphoreGive(g_sd_mutex);

    /* set the block media complete event flag.*/
    g_blockmedia_complete_event = true;
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Writes data to a media.
 * Implements @ref rm_block_media_api_t::write().
 *
 * @retval     FSP_SUCCESS                   Write finished successfully.
 * @retval     Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_SD_Write (rm_block_media_ctrl_t * const p_ctrl,
                                      uint8_t const * const         p_src_address,
                                      uint32_t const                block_address,
                                      uint32_t const                num_blocks)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);

    xSemaphoreTake(g_sd_mutex,portMAX_DELAY);
    /* Call the underlying driver. */
    SD_disk_write(0, p_src_address, block_address, num_blocks);
    xSemaphoreGive(g_sd_mutex);

    /* set the block media complete event flag.*/
    g_blockmedia_complete_event = true;
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Erases sectors of a media.
 * Implements @ref rm_block_media_api_t::erase().
 *
 * @retval     FSP_SUCCESS                   Erase operation requested.
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_SD_Erase (rm_block_media_ctrl_t * const p_ctrl,
                                      uint32_t const                block_address,
                                      uint32_t const                num_blocks)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(block_address);
    FSP_PARAMETER_NOT_USED(num_blocks);

    /* set the block media complete event flag.*/
    g_blockmedia_complete_event = true;
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Provides driver status.  Implements @ref rm_block_media_api_t::statusGet().
 *
 * @retval     FSP_SUCCESS                   Status stored in p_status.
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_SD_StatusGet (rm_block_media_ctrl_t * const   p_api_ctrl,
                                          rm_block_media_status_t * const p_status)
{
    rm_block_media_sd_instance_ctrl_t * p_instance_ctrl = (rm_block_media_sd_instance_ctrl_t *) p_api_ctrl;

    /* update the block media driver current status.*/
    p_status->media_inserted = true;
    p_status->initialized    = p_instance_ctrl->initialized;
    p_status->busy           = false;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Retrieves module information.  Implements @ref rm_block_media_api_t::infoGet().
 *
 * @retval     FSP_SUCCESS                   Erase operation requested.
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_SD_InfoGet (rm_block_media_ctrl_t * const p_ctrl, rm_block_media_info_t * const p_info)
{
    rm_block_media_sd_instance_ctrl_t * p_instance_ctrl = (rm_block_media_sd_instance_ctrl_t *) p_ctrl;

    /* Update the block media driver info.*/
    p_info->sector_size_bytes = p_instance_ctrl->sector_size_bytes;
    p_info->num_sectors       = p_instance_ctrl->sector_count;
    p_info->reentrant         = false;
    p_info->write_protected   = p_instance_ctrl->write_protected;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Closes an open media.  Implements @ref rm_block_media_api_t::close().
 *
 * @retval     FSP_SUCCESS                   Successful close.
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_SD_Close (rm_block_media_ctrl_t * const p_ctrl)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_composite_ep)
 **********************************************************************************************************************/
