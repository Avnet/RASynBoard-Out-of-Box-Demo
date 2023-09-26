/*
 * Copyright (c) 2022 Syntiant Corp.  All rights reserved.
 * Contact at http://www.syntiant.com
 *
 * This software is available to you under a choice of one of two licenses.
 * You may choose to be licensed under the terms of the GNU General Public
 * License (GPL) Version 2, available from the file LICENSE in the main
 * directory of this source tree, or the OpenIB.org BSD license below.  Any
 * code involving Linux software will require selection of the GNU General
 * Public License (GPL) Version 2.
 *
 * OPENIB.ORG BSD LICENSE
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 	** SDK: v103 **
*/

#ifndef _SYNTIANT_COMMON_H_
#define _SYNTIANT_COMMON_H_


/* exclude feature that load from code */
#define EXCLUDE_HOST_LOAD_CODE

/* exclude feature that load from FLASH */
#define EXCLUDE_HOST_LOAD_FLASH
#ifndef EXCLUDE_HOST_LOAD_FLASH
/** please use pad3synpkg.py to pad the flash
 * the flash storage just like :
 *    MCU DATA padding DSP DATA padding FLASH
 * to make sure the data head is at the beginning of sector
 */
#define FLASH_MCU_ADDR					0x0
#define FLASH_MCU_LENGTH				22636
#define FLASH_DSP_ADDR					FLASH_MCU_LENGTH
#define FLASH_DSP_LENGTH				64844

/* use this definition when use host_flash_xxxx.bin */
#define FLASH_NN_ADDR					(FLASH_DSP_ADDR+FLASH_DSP_LENGTH)
#define FLASH_NN_LENGTH					574328

#endif


/* exclude feature that boot from flash */
//#define EXCLUDE_BOOT_FROM_FLASH

/* load from SD card Preferentially, otherwise boot from Flash */
//#define EXCLUDE_HOST_LOAD_FILE
#define LOAD_SPLIT_SIZE_FAT             4096

/* boot from flash */
//#define BOOT_FROM_FLASH

/* SPI rate */
#define DEFAULT_SPI_RATE                12000000
#define SPI_READ_DELAY                  4

/* maximum transfer size on the device */
#define SYNTIANT_MAX_BLOCK_SIZE         (32)

/* enable debug print for spi transfer */
//#define SPI_TRANS_DEBUG

/* remove unused clock option structure */
#define EXCLUDE_CLOCK_OPTION

/* exclude CSPI codes */
//#define EXCLUDE_TINY_CSPI

/* exclude extraction codes */
//#define EXCLUDE_TINY_EXTRACTION

/* exclude sensor feature */
//#define EXCLUDE_SENSOR_FEATURE

/* exclude info getting*/
//#define EXCLUDE_GET_INFO

/* exclude debug printing */
#define EXCLUDE_PRINT_DEBUG

#endif
