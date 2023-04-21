/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_dmac.h"
#include "r_transfer_api.h"
#include "r_usb_basic.h"
#include "r_usb_basic_api.h"
#include "r_usb_pcdc_api.h"
#include "r_lpm.h"
#include "r_lpm_api.h"
#include "r_sci_uart.h"
#include "r_uart_api.h"
#include "r_spi.h"
FSP_HEADER
/* Transfer on DMAC Instance. */
extern const transfer_instance_t g_transfer1;

/** Access the DMAC instance using these structures when calling API functions directly (::p_api is not used). */
extern dmac_instance_ctrl_t g_transfer1_ctrl;
extern const transfer_cfg_t g_transfer1_cfg;

#ifndef NULL
void NULL(dmac_callback_args_t *p_args);
#endif
/* Transfer on DMAC Instance. */
extern const transfer_instance_t g_transfer0;

/** Access the DMAC instance using these structures when calling API functions directly (::p_api is not used). */
extern dmac_instance_ctrl_t g_transfer0_ctrl;
extern const transfer_cfg_t g_transfer0_cfg;

#ifndef usb_ip0_d1fifo_callback
void usb_ip0_d1fifo_callback(dmac_callback_args_t *p_args);
#endif
/* Basic on USB Instance. */
extern const usb_instance_t g_basic;

/** Access the USB instance using these structures when calling API functions directly (::p_api is not used). */
extern usb_instance_ctrl_t g_basic_ctrl;
extern const usb_cfg_t g_basic_cfg;

#ifndef NULL
void NULL(void*);
#endif

#if 2 == BSP_CFG_RTOS
#ifndef usb_pcdc_callback
void usb_pcdc_callback(usb_event_info_t *, usb_hdl_t, usb_onoff_t);
#endif
#endif
/** CDC Driver on USB Instance. */
/** lpm Instance */
extern const lpm_instance_t g_lpm0;

/** Access the LPM instance using these structures when calling API functions directly (::p_api is not used). */
extern lpm_instance_ctrl_t g_lpm0_ctrl;
extern const lpm_cfg_t g_lpm0_cfg;
/** UART on SCI Instance. */
extern const uart_instance_t g_uart3;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_uart_instance_ctrl_t g_uart3_ctrl;
extern const uart_cfg_t g_uart3_cfg;
extern const sci_uart_extended_cfg_t g_uart3_cfg_extend;

#ifndef ble_uart_callback
void ble_uart_callback(uart_callback_args_t *p_args);
#endif
/** SPI on SPI Instance. */
extern const spi_instance_t g_spi1_master;

/** Access the SPI instance using these structures when calling API functions directly (::p_api is not used). */
extern spi_instance_ctrl_t g_spi1_master_ctrl;
extern const spi_cfg_t g_spi1_master_cfg;

/** Callback used by SPI Instance. */
#ifndef spi1_master_callback
void spi1_master_callback(spi_callback_args_t *p_args);
#endif

#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
#define g_spi1_master_P_TRANSFER_TX (NULL)
#else
    #define g_spi1_master_P_TRANSFER_TX (&RA_NOT_DEFINED)
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
#define g_spi1_master_P_TRANSFER_RX (NULL)
#else
    #define g_spi1_master_P_TRANSFER_RX (&RA_NOT_DEFINED)
#endif
#undef RA_NOT_DEFINED
/** UART on SCI Instance. */
extern const uart_instance_t g_uart4;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_uart_instance_ctrl_t g_uart4_ctrl;
extern const uart_cfg_t g_uart4_cfg;
extern const sci_uart_extended_cfg_t g_uart4_cfg_extend;

#ifndef console_callback
void console_callback(uart_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
