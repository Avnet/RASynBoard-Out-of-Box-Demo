/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
#ifdef __cplusplus
        extern "C" {
        #endif
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (24)
#endif
/* ISR prototypes */
void sci_uart_rxi_isr(void);
void sci_uart_txi_isr(void);
void sci_uart_tei_isr(void);
void sci_uart_eri_isr(void);
void spi_rxi_isr(void);
void spi_txi_isr(void);
void spi_tei_isr(void);
void spi_eri_isr(void);
void r_icu_isr(void);
void usbfs_interrupt_handler(void);
void usbfs_resume_handler(void);
void usbfs_d0fifo_handler(void);
void usbfs_d1fifo_handler(void);
void dmac_int_isr(void);
void iic_master_rxi_isr(void);
void iic_master_txi_isr(void);
void iic_master_tei_isr(void);
void iic_master_eri_isr(void);

/* Vector table allocations */
#define VECTOR_NUMBER_SCI4_RXI ((IRQn_Type) 0) /* SCI4 RXI (Received data full) */
#define SCI4_RXI_IRQn          ((IRQn_Type) 0) /* SCI4 RXI (Received data full) */
#define VECTOR_NUMBER_SCI4_TXI ((IRQn_Type) 1) /* SCI4 TXI (Transmit data empty) */
#define SCI4_TXI_IRQn          ((IRQn_Type) 1) /* SCI4 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI4_TEI ((IRQn_Type) 2) /* SCI4 TEI (Transmit end) */
#define SCI4_TEI_IRQn          ((IRQn_Type) 2) /* SCI4 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI4_ERI ((IRQn_Type) 3) /* SCI4 ERI (Receive error) */
#define SCI4_ERI_IRQn          ((IRQn_Type) 3) /* SCI4 ERI (Receive error) */
#define VECTOR_NUMBER_SPI1_RXI ((IRQn_Type) 4) /* SPI1 RXI (Receive buffer full) */
#define SPI1_RXI_IRQn          ((IRQn_Type) 4) /* SPI1 RXI (Receive buffer full) */
#define VECTOR_NUMBER_SPI1_TXI ((IRQn_Type) 5) /* SPI1 TXI (Transmit buffer empty) */
#define SPI1_TXI_IRQn          ((IRQn_Type) 5) /* SPI1 TXI (Transmit buffer empty) */
#define VECTOR_NUMBER_SPI1_TEI ((IRQn_Type) 6) /* SPI1 TEI (Transmission complete event) */
#define SPI1_TEI_IRQn          ((IRQn_Type) 6) /* SPI1 TEI (Transmission complete event) */
#define VECTOR_NUMBER_SPI1_ERI ((IRQn_Type) 7) /* SPI1 ERI (Error) */
#define SPI1_ERI_IRQn          ((IRQn_Type) 7) /* SPI1 ERI (Error) */
#define VECTOR_NUMBER_ICU_IRQ13 ((IRQn_Type) 8) /* ICU IRQ13 (External pin interrupt 13) */
#define ICU_IRQ13_IRQn          ((IRQn_Type) 8) /* ICU IRQ13 (External pin interrupt 13) */
#define VECTOR_NUMBER_SCI3_RXI ((IRQn_Type) 9) /* SCI3 RXI (Received data full) */
#define SCI3_RXI_IRQn          ((IRQn_Type) 9) /* SCI3 RXI (Received data full) */
#define VECTOR_NUMBER_SCI3_TXI ((IRQn_Type) 10) /* SCI3 TXI (Transmit data empty) */
#define SCI3_TXI_IRQn          ((IRQn_Type) 10) /* SCI3 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI3_TEI ((IRQn_Type) 11) /* SCI3 TEI (Transmit end) */
#define SCI3_TEI_IRQn          ((IRQn_Type) 11) /* SCI3 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI3_ERI ((IRQn_Type) 12) /* SCI3 ERI (Receive error) */
#define SCI3_ERI_IRQn          ((IRQn_Type) 12) /* SCI3 ERI (Receive error) */
#define VECTOR_NUMBER_USBFS_INT ((IRQn_Type) 13) /* USBFS INT (USBFS interrupt) */
#define USBFS_INT_IRQn          ((IRQn_Type) 13) /* USBFS INT (USBFS interrupt) */
#define VECTOR_NUMBER_USBFS_RESUME ((IRQn_Type) 14) /* USBFS RESUME (USBFS resume interrupt) */
#define USBFS_RESUME_IRQn          ((IRQn_Type) 14) /* USBFS RESUME (USBFS resume interrupt) */
#define VECTOR_NUMBER_USBFS_FIFO_0 ((IRQn_Type) 15) /* USBFS FIFO 0 (DMA transfer request 0) */
#define USBFS_FIFO_0_IRQn          ((IRQn_Type) 15) /* USBFS FIFO 0 (DMA transfer request 0) */
#define VECTOR_NUMBER_USBFS_FIFO_1 ((IRQn_Type) 16) /* USBFS FIFO 1 (DMA transfer request 1) */
#define USBFS_FIFO_1_IRQn          ((IRQn_Type) 16) /* USBFS FIFO 1 (DMA transfer request 1) */
#define VECTOR_NUMBER_DMAC0_INT ((IRQn_Type) 17) /* DMAC0 INT (DMAC transfer end 0) */
#define DMAC0_INT_IRQn          ((IRQn_Type) 17) /* DMAC0 INT (DMAC transfer end 0) */
#define VECTOR_NUMBER_DMAC1_INT ((IRQn_Type) 18) /* DMAC1 INT (DMAC transfer end 1) */
#define DMAC1_INT_IRQn          ((IRQn_Type) 18) /* DMAC1 INT (DMAC transfer end 1) */
#define VECTOR_NUMBER_IIC0_RXI ((IRQn_Type) 19) /* IIC0 RXI (Receive data full) */
#define IIC0_RXI_IRQn          ((IRQn_Type) 19) /* IIC0 RXI (Receive data full) */
#define VECTOR_NUMBER_IIC0_TXI ((IRQn_Type) 20) /* IIC0 TXI (Transmit data empty) */
#define IIC0_TXI_IRQn          ((IRQn_Type) 20) /* IIC0 TXI (Transmit data empty) */
#define VECTOR_NUMBER_IIC0_TEI ((IRQn_Type) 21) /* IIC0 TEI (Transmit end) */
#define IIC0_TEI_IRQn          ((IRQn_Type) 21) /* IIC0 TEI (Transmit end) */
#define VECTOR_NUMBER_IIC0_ERI ((IRQn_Type) 22) /* IIC0 ERI (Transfer error) */
#define IIC0_ERI_IRQn          ((IRQn_Type) 22) /* IIC0 ERI (Transfer error) */
#define VECTOR_NUMBER_ICU_IRQ5 ((IRQn_Type) 23) /* ICU IRQ5 (External pin interrupt 5) */
#define ICU_IRQ5_IRQn          ((IRQn_Type) 23) /* ICU IRQ5 (External pin interrupt 5) */
#ifdef __cplusplus
        }
        #endif
#endif /* VECTOR_DATA_H */
