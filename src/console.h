/*
 * console.h
 *
 *  Created on: 2022年11月24日
 *      Author: Wenxue
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <stdio.h>
#include "r_sci_uart.h"
#include "hal_data.h"

/* To implement _write() to redirect the printf(),
 * enable DEBUG_CONSOLE_USB_CDC to print via USB CDC
 * or enable DEBUG_CONSOLE_UART to print by UART.
 */
//#define DEBUG_CONSOLE_USB_CDC
#ifndef DEBUG_CONSOLE_USB_CDC
  #define DEBUG_CONSOLE_UART
#endif

/* UART micro */
#define g_console(x)    &g_uart4_##x
#define g_console_ctrl  g_console(ctrl)
#define g_console_cfg   g_console(cfg)

/* Function declaration */
extern fsp_err_t console_init(void);
extern fsp_err_t console_deinit(void);
extern void console_callback(uart_callback_args_t *p_args);
extern int _write(int fd, char *pBuffer, int size);

#endif /* CONSOLE_H_ */
