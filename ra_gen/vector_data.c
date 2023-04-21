/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = sci_uart_rxi_isr, /* SCI4 RXI (Received data full) */
            [1] = sci_uart_txi_isr, /* SCI4 TXI (Transmit data empty) */
            [2] = sci_uart_tei_isr, /* SCI4 TEI (Transmit end) */
            [3] = sci_uart_eri_isr, /* SCI4 ERI (Receive error) */
            [4] = spi_rxi_isr, /* SPI1 RXI (Receive buffer full) */
            [5] = spi_txi_isr, /* SPI1 TXI (Transmit buffer empty) */
            [6] = spi_tei_isr, /* SPI1 TEI (Transmission complete event) */
            [7] = spi_eri_isr, /* SPI1 ERI (Error) */
            [8] = r_icu_isr, /* ICU IRQ13 (External pin interrupt 13) */
            [9] = sci_uart_rxi_isr, /* SCI3 RXI (Received data full) */
            [10] = sci_uart_txi_isr, /* SCI3 TXI (Transmit data empty) */
            [11] = sci_uart_tei_isr, /* SCI3 TEI (Transmit end) */
            [12] = sci_uart_eri_isr, /* SCI3 ERI (Receive error) */
            [13] = usbfs_interrupt_handler, /* USBFS INT (USBFS interrupt) */
            [14] = usbfs_resume_handler, /* USBFS RESUME (USBFS resume interrupt) */
            [15] = usbfs_d0fifo_handler, /* USBFS FIFO 0 (DMA transfer request 0) */
            [16] = usbfs_d1fifo_handler, /* USBFS FIFO 1 (DMA transfer request 1) */
            [17] = dmac_int_isr, /* DMAC0 INT (DMAC transfer end 0) */
            [18] = dmac_int_isr, /* DMAC1 INT (DMAC transfer end 1) */
            [19] = iic_master_rxi_isr, /* IIC0 RXI (Receive data full) */
            [20] = iic_master_txi_isr, /* IIC0 TXI (Transmit data empty) */
            [21] = iic_master_tei_isr, /* IIC0 TEI (Transmit end) */
            [22] = iic_master_eri_isr, /* IIC0 ERI (Transfer error) */
            [23] = r_icu_isr, /* ICU IRQ5 (External pin interrupt 5) */
        };
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] =
        {
            [0] = BSP_PRV_IELS_ENUM(EVENT_SCI4_RXI), /* SCI4 RXI (Received data full) */
            [1] = BSP_PRV_IELS_ENUM(EVENT_SCI4_TXI), /* SCI4 TXI (Transmit data empty) */
            [2] = BSP_PRV_IELS_ENUM(EVENT_SCI4_TEI), /* SCI4 TEI (Transmit end) */
            [3] = BSP_PRV_IELS_ENUM(EVENT_SCI4_ERI), /* SCI4 ERI (Receive error) */
            [4] = BSP_PRV_IELS_ENUM(EVENT_SPI1_RXI), /* SPI1 RXI (Receive buffer full) */
            [5] = BSP_PRV_IELS_ENUM(EVENT_SPI1_TXI), /* SPI1 TXI (Transmit buffer empty) */
            [6] = BSP_PRV_IELS_ENUM(EVENT_SPI1_TEI), /* SPI1 TEI (Transmission complete event) */
            [7] = BSP_PRV_IELS_ENUM(EVENT_SPI1_ERI), /* SPI1 ERI (Error) */
            [8] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ13), /* ICU IRQ13 (External pin interrupt 13) */
            [9] = BSP_PRV_IELS_ENUM(EVENT_SCI3_RXI), /* SCI3 RXI (Received data full) */
            [10] = BSP_PRV_IELS_ENUM(EVENT_SCI3_TXI), /* SCI3 TXI (Transmit data empty) */
            [11] = BSP_PRV_IELS_ENUM(EVENT_SCI3_TEI), /* SCI3 TEI (Transmit end) */
            [12] = BSP_PRV_IELS_ENUM(EVENT_SCI3_ERI), /* SCI3 ERI (Receive error) */
            [13] = BSP_PRV_IELS_ENUM(EVENT_USBFS_INT), /* USBFS INT (USBFS interrupt) */
            [14] = BSP_PRV_IELS_ENUM(EVENT_USBFS_RESUME), /* USBFS RESUME (USBFS resume interrupt) */
            [15] = BSP_PRV_IELS_ENUM(EVENT_USBFS_FIFO_0), /* USBFS FIFO 0 (DMA transfer request 0) */
            [16] = BSP_PRV_IELS_ENUM(EVENT_USBFS_FIFO_1), /* USBFS FIFO 1 (DMA transfer request 1) */
            [17] = BSP_PRV_IELS_ENUM(EVENT_DMAC0_INT), /* DMAC0 INT (DMAC transfer end 0) */
            [18] = BSP_PRV_IELS_ENUM(EVENT_DMAC1_INT), /* DMAC1 INT (DMAC transfer end 1) */
            [19] = BSP_PRV_IELS_ENUM(EVENT_IIC0_RXI), /* IIC0 RXI (Receive data full) */
            [20] = BSP_PRV_IELS_ENUM(EVENT_IIC0_TXI), /* IIC0 TXI (Transmit data empty) */
            [21] = BSP_PRV_IELS_ENUM(EVENT_IIC0_TEI), /* IIC0 TEI (Transmit end) */
            [22] = BSP_PRV_IELS_ENUM(EVENT_IIC0_ERI), /* IIC0 ERI (Transfer error) */
            [23] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ5), /* ICU IRQ5 (External pin interrupt 5) */
        };
        #endif
