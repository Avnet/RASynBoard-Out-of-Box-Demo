/* generated HAL source file - do not edit */
#include "hal_data.h"

dmac_instance_ctrl_t g_transfer1_ctrl;
transfer_info_t g_transfer1_info =
{ .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
  .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
  .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
  .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
  .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .transfer_settings_word_b.size = TRANSFER_SIZE_2_BYTE,
  .transfer_settings_word_b.mode = TRANSFER_MODE_BLOCK,
  .p_dest = (void*) 0,
  .p_src = (void const*) USB_SRC_ADDRESS,
  .num_blocks = 0,
  .length = 0, };
const dmac_extended_cfg_t g_transfer1_extend =
{ .offset = 0, .src_buffer_size = 1,
#if defined(VECTOR_NUMBER_DMAC1_INT)
    .irq                 = VECTOR_NUMBER_DMAC1_INT,
#else
  .irq = FSP_INVALID_VECTOR,
#endif
  .ipl = (3),
  .channel = 1, .p_callback = NULL, .p_context = NULL, .activation_source = ELC_EVENT_USBFS_FIFO_0, };
const transfer_cfg_t g_transfer1_cfg =
{ .p_info = &g_transfer1_info, .p_extend = &g_transfer1_extend, };
/* Instance structure to use this module. */
const transfer_instance_t g_transfer1 =
{ .p_ctrl = &g_transfer1_ctrl, .p_cfg = &g_transfer1_cfg, .p_api = &g_transfer_on_dmac };
dmac_instance_ctrl_t g_transfer0_ctrl;
transfer_info_t g_transfer0_info =
{ .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
  .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
  .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
  .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
  .transfer_settings_word_b.size = TRANSFER_SIZE_2_BYTE,
  .transfer_settings_word_b.mode = TRANSFER_MODE_BLOCK,
  .p_dest = (void*) USB_DEST_ADDRESS,
  .p_src = (void const*) 0,
  .num_blocks = 0,
  .length = 0, };
const dmac_extended_cfg_t g_transfer0_extend =
{ .offset = 0, .src_buffer_size = 1,
#if defined(VECTOR_NUMBER_DMAC0_INT)
    .irq                 = VECTOR_NUMBER_DMAC0_INT,
#else
  .irq = FSP_INVALID_VECTOR,
#endif
  .ipl = (3),
  .channel = 0, .p_callback = usb_ip0_d1fifo_callback, .p_context = NULL, .activation_source = ELC_EVENT_USBFS_FIFO_1, };
const transfer_cfg_t g_transfer0_cfg =
{ .p_info = &g_transfer0_info, .p_extend = &g_transfer0_extend, };
/* Instance structure to use this module. */
const transfer_instance_t g_transfer0 =
{ .p_ctrl = &g_transfer0_ctrl, .p_cfg = &g_transfer0_cfg, .p_api = &g_transfer_on_dmac };
usb_instance_ctrl_t g_basic_ctrl;

#if !defined(g_usb_descriptor)
extern usb_descriptor_t g_usb_descriptor;
#endif
#define RA_NOT_DEFINED (1)
const usb_cfg_t g_basic_cfg =
{ .usb_mode = USB_MODE_PERI,
  .usb_speed = USB_SPEED_FS,
  .module_number = 0,
  .type = USB_CLASS_PMSC,
#if defined(g_usb_descriptor)
                .p_usb_reg = g_usb_descriptor,
#else
  .p_usb_reg = &g_usb_descriptor,
#endif
  .usb_complience_cb = NULL,
#if defined(VECTOR_NUMBER_USBFS_INT)
                .irq       = VECTOR_NUMBER_USBFS_INT,
#else
  .irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_USBFS_RESUME)
                .irq_r     = VECTOR_NUMBER_USBFS_RESUME,
#else
  .irq_r = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_USBFS_FIFO_0)
                .irq_d0    = VECTOR_NUMBER_USBFS_FIFO_0,
#else
  .irq_d0 = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_USBFS_FIFO_1)
                .irq_d1    = VECTOR_NUMBER_USBFS_FIFO_1,
#else
  .irq_d1 = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_USBHS_USB_INT_RESUME)
                .hsirq     = VECTOR_NUMBER_USBHS_USB_INT_RESUME,
#else
  .hsirq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_USBHS_FIFO_0)
                .hsirq_d0  = VECTOR_NUMBER_USBHS_FIFO_0,
#else
  .hsirq_d0 = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_USBHS_FIFO_1)
                .hsirq_d1  = VECTOR_NUMBER_USBHS_FIFO_1,
#else
  .hsirq_d1 = FSP_INVALID_VECTOR,
#endif
  .ipl = (13),
  .ipl_r = (13),
  .ipl_d0 = (13),
  .ipl_d1 = (13),
  .hsipl = (BSP_IRQ_DISABLED),
  .hsipl_d0 = (BSP_IRQ_DISABLED),
  .hsipl_d1 = (BSP_IRQ_DISABLED),
#if (BSP_CFG_RTOS != 0)
                .p_usb_apl_callback = usb_composite_callback,
#else
  .p_usb_apl_callback = NULL,
#endif
#if defined(g_rm_block_media)
                .p_context = g_rm_block_media,
#else
  .p_context = &g_rm_block_media,
#endif
#if (RA_NOT_DEFINED == g_transfer0)
#else
  .p_transfer_tx = &g_transfer0,
#endif
#if (RA_NOT_DEFINED == g_transfer1)
#else
  .p_transfer_rx = &g_transfer1,
#endif
        };
#undef RA_NOT_DEFINED

/* Instance structure to use this module. */
const usb_instance_t g_basic =
{ .p_ctrl = &g_basic_ctrl, .p_cfg = &g_basic_cfg, .p_api = &g_usb_on_usb, };

lpm_instance_ctrl_t g_lpm0_ctrl;

const lpm_cfg_t g_lpm0_cfg =
{ .low_power_mode = LPM_MODE_STANDBY,
  .snooze_cancel_sources = LPM_SNOOZE_CANCEL_SOURCE_NONE,
  .standby_wake_sources = LPM_STANDBY_WAKE_SOURCE_IRQ5 | LPM_STANDBY_WAKE_SOURCE_IRQ13 | (lpm_standby_wake_source_t) 0,
  .snooze_request_source = LPM_SNOOZE_REQUEST_RXD0_FALLING,
  .snooze_end_sources = (lpm_snooze_end_t) 0,
  .dtc_state_in_snooze = LPM_SNOOZE_DTC_DISABLE,
#if BSP_FEATURE_LPM_HAS_SBYCR_OPE
    .output_port_enable         = LPM_OUTPUT_PORT_ENABLE_RETAIN,
#endif
#if BSP_FEATURE_LPM_HAS_DEEP_STANDBY
    .io_port_state              = LPM_IO_PORT_NO_CHANGE,
    .power_supply_state         = LPM_POWER_SUPPLY_DEEPCUT0,
    .deep_standby_cancel_source =  (lpm_deep_standby_cancel_source_t) 0,
    .deep_standby_cancel_edge   =  (lpm_deep_standby_cancel_edge_t) 0,
#endif
  .p_extend = NULL, };

const lpm_instance_t g_lpm0 =
{ .p_api = &g_lpm_on_lpm, .p_ctrl = &g_lpm0_ctrl, .p_cfg = &g_lpm0_cfg };
sci_uart_instance_ctrl_t g_uart3_ctrl;

baud_setting_t g_uart3_baud_setting =
        {
        /* Baud rate calculated with 0.469% error. */.semr_baudrate_bits_b.abcse = 0,
          .semr_baudrate_bits_b.abcs = 0, .semr_baudrate_bits_b.bgdm = 1, .cks = 0, .brr = 53, .mddr = (uint8_t) 256, .semr_baudrate_bits_b.brme =
                  false };

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t g_uart3_cfg_extend =
{ .clock = SCI_UART_CLOCK_INT, .rx_edge_start = SCI_UART_START_BIT_FALLING_EDGE, .noise_cancel =
          SCI_UART_NOISE_CANCELLATION_DISABLE,
  .rx_fifo_trigger = SCI_UART_RX_FIFO_TRIGGER_MAX, .p_baud_setting = &g_uart3_baud_setting, .flow_control =
          SCI_UART_FLOW_CONTROL_RTS,
#if 0xFF != 0xFF
                .flow_control_pin       = BSP_IO_PORT_FF_PIN_0xFF,
                #else
  .flow_control_pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif
  .rs485_setting =
  { .enable = SCI_UART_RS485_DISABLE, .polarity = SCI_UART_RS485_DE_POLARITY_HIGH,
#if 0xFF != 0xFF
                    .de_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
                #else
    .de_control_pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif
          }, };

/** UART interface configuration */
const uart_cfg_t g_uart3_cfg =
{ .channel = 3, .data_bits = UART_DATA_BITS_8, .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
          ble_uart_callback,
  .p_context = NULL, .p_extend = &g_uart3_cfg_extend,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_tx = NULL,
#else
                .p_transfer_tx       = &RA_NOT_DEFINED,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_rx = NULL,
#else
                .p_transfer_rx       = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
  .rxi_ipl = (3),
  .txi_ipl = (3), .tei_ipl = (3), .eri_ipl = (3),
#if defined(VECTOR_NUMBER_SCI3_RXI)
                .rxi_irq             = VECTOR_NUMBER_SCI3_RXI,
#else
  .rxi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI3_TXI)
                .txi_irq             = VECTOR_NUMBER_SCI3_TXI,
#else
  .txi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI3_TEI)
                .tei_irq             = VECTOR_NUMBER_SCI3_TEI,
#else
  .tei_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI3_ERI)
                .eri_irq             = VECTOR_NUMBER_SCI3_ERI,
#else
  .eri_irq = FSP_INVALID_VECTOR,
#endif
        };

/* Instance structure to use this module. */
const uart_instance_t g_uart3 =
{ .p_ctrl = &g_uart3_ctrl, .p_cfg = &g_uart3_cfg, .p_api = &g_uart_on_sci };
#define RA_NOT_DEFINED (UINT32_MAX)
#if (RA_NOT_DEFINED) != (RA_NOT_DEFINED)

/* If the transfer module is DMAC, define a DMAC transfer callback. */
#include "r_dmac.h"
extern void spi_tx_dmac_callback(spi_instance_ctrl_t const * const p_ctrl);

void g_spi1_master_tx_transfer_callback (dmac_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    spi_tx_dmac_callback(&g_spi1_master_ctrl);
}
#endif

#if (RA_NOT_DEFINED) != (RA_NOT_DEFINED)

/* If the transfer module is DMAC, define a DMAC transfer callback. */
#include "r_dmac.h"
extern void spi_rx_dmac_callback(spi_instance_ctrl_t const * const p_ctrl);

void g_spi1_master_rx_transfer_callback (dmac_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    spi_rx_dmac_callback(&g_spi1_master_ctrl);
}
#endif
#undef RA_NOT_DEFINED

spi_instance_ctrl_t g_spi1_master_ctrl;

/** SPI extended configuration for SPI HAL driver */
const spi_extended_cfg_t g_spi1_master_ext_cfg =
{ .spi_clksyn = SPI_SSL_MODE_CLK_SYN,
  .spi_comm = SPI_COMMUNICATION_FULL_DUPLEX,
  .ssl_polarity = SPI_SSLP_LOW,
  .ssl_select = SPI_SSL_SELECT_SSL0,
  .mosi_idle = SPI_MOSI_IDLE_VALUE_FIXING_DISABLE,
  .parity = SPI_PARITY_MODE_DISABLE,
  .byte_swap = SPI_BYTE_SWAP_DISABLE,
  .spck_div =
  {
  /* Actual calculated bitrate: 16666667. */.spbr = 2,
    .brdv = 0 },
  .spck_delay = SPI_DELAY_COUNT_1,
  .ssl_negation_delay = SPI_DELAY_COUNT_1,
  .next_access_delay = SPI_DELAY_COUNT_1 };

/** SPI configuration for SPI HAL driver */
const spi_cfg_t g_spi1_master_cfg =
{ .channel = 1,

#if defined(VECTOR_NUMBER_SPI1_RXI)
    .rxi_irq             = VECTOR_NUMBER_SPI1_RXI,
#else
  .rxi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SPI1_TXI)
    .txi_irq             = VECTOR_NUMBER_SPI1_TXI,
#else
  .txi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SPI1_TEI)
    .tei_irq             = VECTOR_NUMBER_SPI1_TEI,
#else
  .tei_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SPI1_ERI)
    .eri_irq             = VECTOR_NUMBER_SPI1_ERI,
#else
  .eri_irq = FSP_INVALID_VECTOR,
#endif

  .rxi_ipl = (9),
  .txi_ipl = (10), .tei_ipl = (12), .eri_ipl = (12),

  .operating_mode = SPI_MODE_MASTER,

  .clk_phase = SPI_CLK_PHASE_EDGE_ODD,
  .clk_polarity = SPI_CLK_POLARITY_LOW,

  .mode_fault = SPI_MODE_FAULT_ERROR_DISABLE,
  .bit_order = SPI_BIT_ORDER_MSB_FIRST, .p_transfer_tx = g_spi1_master_P_TRANSFER_TX, .p_transfer_rx =
          g_spi1_master_P_TRANSFER_RX,
  .p_callback = spi1_master_callback,

  .p_context = NULL,
  .p_extend = (void*) &g_spi1_master_ext_cfg, };

/* Instance structure to use this module. */
const spi_instance_t g_spi1_master =
{ .p_ctrl = &g_spi1_master_ctrl, .p_cfg = &g_spi1_master_cfg, .p_api = &g_spi_on_spi };
sci_uart_instance_ctrl_t g_uart4_ctrl;

baud_setting_t g_uart4_baud_setting =
        {
        /* Baud rate calculated with 0.469% error. */.semr_baudrate_bits_b.abcse = 0,
          .semr_baudrate_bits_b.abcs = 0, .semr_baudrate_bits_b.bgdm = 1, .cks = 0, .brr = 53, .mddr = (uint8_t) 256, .semr_baudrate_bits_b.brme =
                  false };

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t g_uart4_cfg_extend =
{ .clock = SCI_UART_CLOCK_INT, .rx_edge_start = SCI_UART_START_BIT_FALLING_EDGE, .noise_cancel =
          SCI_UART_NOISE_CANCELLATION_DISABLE,
  .rx_fifo_trigger = SCI_UART_RX_FIFO_TRIGGER_MAX, .p_baud_setting = &g_uart4_baud_setting, .flow_control =
          SCI_UART_FLOW_CONTROL_RTS,
#if 0xFF != 0xFF
                .flow_control_pin       = BSP_IO_PORT_FF_PIN_0xFF,
                #else
  .flow_control_pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif
  .rs485_setting =
  { .enable = SCI_UART_RS485_DISABLE, .polarity = SCI_UART_RS485_DE_POLARITY_HIGH,
#if 0xFF != 0xFF
                    .de_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
                #else
    .de_control_pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif
          }, };

/** UART interface configuration */
const uart_cfg_t g_uart4_cfg =
{ .channel = 4, .data_bits = UART_DATA_BITS_8, .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
          console_callback,
  .p_context = NULL, .p_extend = &g_uart4_cfg_extend,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_tx = NULL,
#else
                .p_transfer_tx       = &RA_NOT_DEFINED,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_rx = NULL,
#else
                .p_transfer_rx       = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
  .rxi_ipl = (3),
  .txi_ipl = (3), .tei_ipl = (3), .eri_ipl = (3),
#if defined(VECTOR_NUMBER_SCI4_RXI)
                .rxi_irq             = VECTOR_NUMBER_SCI4_RXI,
#else
  .rxi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI4_TXI)
                .txi_irq             = VECTOR_NUMBER_SCI4_TXI,
#else
  .txi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI4_TEI)
                .tei_irq             = VECTOR_NUMBER_SCI4_TEI,
#else
  .tei_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI4_ERI)
                .eri_irq             = VECTOR_NUMBER_SCI4_ERI,
#else
  .eri_irq = FSP_INVALID_VECTOR,
#endif
        };

/* Instance structure to use this module. */
const uart_instance_t g_uart4 =
{ .p_ctrl = &g_uart4_ctrl, .p_cfg = &g_uart4_cfg, .p_api = &g_uart_on_sci };
void g_hal_init(void)
{
    g_common_init ();
}
