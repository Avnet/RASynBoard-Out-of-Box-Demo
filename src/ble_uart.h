/*
 * console.h
 *
 *  Created on: 2022年11月24日
 *      Author: Wenxue
 */

#ifndef BLE_UART_H_
#define BLE_UART_H_

#include <stdio.h>
#include "r_sci_uart.h"
#include "hal_data.h"
#include "fat_load.h"

extern volatile bool            g_uart3_txComplete;  /* Tx complete flags */

/* Function declaration */
extern fsp_err_t ble_uart_init(void);
extern fsp_err_t ble_uart_deinit(void);
extern void ble_uart_callback(uart_callback_args_t *p_args);
extern int ble_send(char *pBuffer, int size);

#endif /* BLE_UART_H_ */
