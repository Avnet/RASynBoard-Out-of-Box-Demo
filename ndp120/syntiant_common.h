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
//#define NDP_LOAD_CODE

/* or load from FLASH */
//#define NDP_LOAD_FLASH
#ifdef NDP_LOAD_FLASH
/** please use pad3synpkg.py to pad the flash
 * the flash storage just like :
 *    MCU DATA padding DSP DATA padding FLASH
 * to make sure the data head is at the beginning of sector
 */
#define FLASH_MCU_ADDR					0x0
#define FLASH_MCU_LENGTH				22636
#define FLASH_DSP_ADDR					0x5c00
#define FLASH_DSP_LENGTH				79828
#define FLASH_NN_ADDR					0x19400
#define FLASH_NN_LENGTH					417828
#endif

/* boot from SD card */
#define NDP_LOAD_SD
#define LOAD_SPLIT_SIZE_FAT     4096

/* load from FATFS on uSD card Preferentially */
#define NDP_LOAD_AUTO


/* boot from flash */
//#define LOAD_FROM_FLASH

/* SPI rate */
#define DEFAULT_SPI_RATE                8000000

/* maximum transfer size on the device */
#define SYNTIANT_MAX_BLOCK_SIZE     (128)

/* enable debug print for spi transfer */
//#define SPI_TRANS_DEBUG


/* enable loop start feature */
//#define ENABLE_LOOP_START

#endif
