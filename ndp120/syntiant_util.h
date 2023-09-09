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

#ifndef _SYNTIANT_UTIL_H_
#define _SYNTIANT_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "syntiant_common.h"
#include "syntiant_ilib/syntiant_ndp120_tiny.h"

struct ndp120_tiny_handle_t {
    struct syntiant_ndp120_tiny_device_s *ndp;
    int spifd;
    void *io_handle;
    uint32_t notification_save;
};

extern int syntiant_tiny_io_init(void *d, int spi_default_speed);
extern int syntiant_tiny_mbwait(void *d);
extern int syntiant_tiny_transfer(void *d, int mcu, uint32_t addr, 
                    void *out, void *in, unsigned int count);
extern int syntiant_tiny_flash_read(uint32_t flash_addr, 
                    uint8_t *read_buffer, int read_size);
extern int syntiant_tiny_file_len(char *file_name);
extern int syntiant_tiny_file_read(char *file_name, uint32_t offset, 
                    uint8_t *read_buffer, int read_size);
extern int syntiant_tiny_delayus(unsigned int delay_us);

#ifdef __cplusplus
}
#endif

#endif // end of _SYNTIANT_UTIL_H_
