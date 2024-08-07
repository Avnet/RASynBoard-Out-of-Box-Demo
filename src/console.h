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
#include "fat_load.h"


/* UART micro */
extern int _write(int fd, char *pBuffer, int size);

#endif /* CONSOLE_H_ */
