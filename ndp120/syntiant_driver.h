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

#ifndef _SYNTIANT_DRIVER_H_
#define _SYNTIANT_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "syntiant_common.h"

enum ndp120_spi_regs_e {
    NDP120B0_SPI_ID0 = 0x00,
    NDP120B0_SPI_MADDR = 0x40,
    NDP120B0_SPI_MDATA = 0x44
};

int syntiant_ndp_spi_init(void *d, int spi_default_speed, int spi_read_delay);

int syntiant_ndp_transfer(void *d, int mcu, uint32_t addr,
        void *out, void *in, unsigned int count);

int syntiant_flash_spi_read(uint32_t addr, uint8_t *read_buff, int read_len);

void syntiant_ndp_delay_ms(int ms_time);

void syntiant_ndp_delay_us(int us_time);

#ifdef __cplusplus
}
#endif

#endif //end of _SYNTIANT_DRIVER_H_
