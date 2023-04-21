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

#ifndef _SYNTIANT_UTIL_H_
#define _SYNTIANT_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "syntiant_common.h"

#include <syntiant_ilib/syntiant_ndp120_tiny.h>

struct ndp120_tiny_handle_t {
    struct syntiant_ndp120_tiny_device_s *ndp;
    int spifd;
    uint32_t notification_save;
};

extern int syntiant_tiny_io_init(void *d, int spi_default_speed);
extern int syntiant_tiny_mbwait(void *d);
extern int syntiant_tiny_transfer(void *d, int mcu, uint32_t addr, 
                    void *out, void *in, unsigned int count);
extern int syntiant_tiny_flash_read(uint32_t flash_addr, 
                    uint8_t *read_buffer, int read_size);
extern int syntiant_tiny_delayms(unsigned int delay_ms);
extern int syntiant_tiny_delayus(unsigned int delay_us);

#ifdef __cplusplus
}
#endif

#endif // end of _SYNTIANT_UTIL_H_
