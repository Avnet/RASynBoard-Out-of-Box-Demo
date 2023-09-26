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
 	** SDK: v103**
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

enum ndp120_spi_type_e {
    NDP120_SPI_TYPE_LINUX,
    NDP120_SPI_TYPE_NORDIC,
    NDP120_SPI_TYPE_REALTEK,
    NDP120_SPI_TYPE_TELINK_8X,
    NDP120_SPI_TYPE_TELINK_9X,
    NDP120_SPI_TYPE_DECT,
    NDP120_SPI_TYPE_FT4222,
    NDP120_SPI_TYPE_MAX = NDP120_SPI_TYPE_LINUX
};

int syntiant_ndp_spi_init(void *d, int spi_default_speed, int spi_read_delay);

int syntiant_ndp_transfer(void *d, int mcu, uint32_t addr,
        void *out, void *in, unsigned int count);

int syntiant_flash_spi_read(uint32_t addr, uint8_t *read_buff, int read_len);

int syntiant_file_host_len(char *file_name);

int syntiant_file_host_read(char *file_name, uint32_t offset, 
        uint8_t *read_buffer, int read_size);

void syntiant_ndp_delay_us(int us_time);

#ifdef __cplusplus
}
#endif

#endif //end of _SYNTIANT_DRIVER_H_
