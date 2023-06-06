/*
 * SYNTIANT CONFIDENTIAL
 * _____________________
 *
 *   Copyright (c) 2018-2020 Syntiant Corporation
 *   All Rights Reserved.
 *
 *  NOTICE:  All information contained herein is, and remains the property of
 *  Syntiant Corporation and its suppliers, if any.  The intellectual and
 *  technical concepts contained herein are proprietary to Syntiant Corporation
 *  and its suppliers and may be covered by U.S. and Foreign Patents, patents in
 *  process, and are protected by trade secret or copyright law.  Dissemination
 *  of this information or reproduction of this material is strictly forbidden
 *  unless prior written permission is obtained from Syntiant Corporation.
 */

#ifndef _SYNTIANT_COMMON_H_
#define _SYNTIANT_COMMON_H_

/* load from code */
//#define HOST_LOAD_CODE

/* or load from FLASH */
//#define HOST_LOAD_FLASH
#ifdef HOST_LOAD_FLASH
/** please use pad3synpkg.py to pad the flash
 * the flash storage just like :
 *    MCU DATA padding DSP DATA padding FLASH
 * to make sure the data head is at the beginning of sector
 */
#define FLASH_MCU_ADDR					0x0
#define FLASH_MCU_LENGTH				22636
#define FLASH_DSP_ADDR					0x5c00
#define FLASH_DSP_LENGTH				60352

/* use this definition when use host_flash_glassbreak_v98.bin */
#define FLASH_NN_ADDR					0x14800
#define FLASH_NN_LENGTH					574268

#endif


/* load from SD card Preferentially, otherwise boot from Flash */
#define HOST_LOAD_AUTO
#define LOAD_SPLIT_SIZE_FAT     4096

/* boot from flash */
//#define BOOT_FROM_FLASH

/* SPI rate */
#define DEFAULT_SPI_RATE                8000000

/* maximum transfer size on the device */
#define SYNTIANT_MAX_BLOCK_SIZE     (128)

/* enable debug print for spi transfer */
//#define SPI_TRANS_DEBUG

/* remove unused clock option structure */
#define EXCLUDE_CLOCK_OPTION

#endif
