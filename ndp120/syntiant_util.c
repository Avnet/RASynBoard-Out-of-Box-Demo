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

#include "syntiant_common.h"

#include "syntiant_util.h"
#include "syntiant_driver.h"

#ifndef NULL
#define NULL (0)
#endif

/************************************************************/
/* SPI handler declaration */
int syntiant_tiny_io_init(void *d, int spi_default_speed)
{
    int s;
    struct ndp120_tiny_handle_t *ndp_handle = (struct ndp120_tiny_handle_t*)d;

    /*##-1- Initialize the SPI peripheral ##################*/
    s = syntiant_ndp_spi_init(&ndp_handle->spifd, spi_default_speed, SPI_READ_DELAY);
    return s;
}

int syntiant_tiny_transfer(void *d, int mcu, uint32_t addr, 
        void *out, void *in, unsigned int count)
{
    struct ndp120_tiny_handle_t *ndp_handle = (d)?(struct ndp120_tiny_handle_t*)d:NULL;
    
    return syntiant_ndp_transfer((ndp_handle)?&(ndp_handle->spifd):NULL, 
            mcu, addr, out, in, count);
}

#include <stdio.h>
/* iif mailbox-exchange wait. this sample implementation just polls for mailbox
 * completion */
#define MBWAIT_MAX_TRY  30000
int syntiant_tiny_mbwait(void *d)
{
    int s, max_try = 0;
    uint32_t notifications;

    struct ndp120_tiny_handle_t *ndp_handle = (struct ndp120_tiny_handle_t*)d;
    struct syntiant_ndp120_tiny_device_s *ndpp = ndp_handle->ndp;

    do {
        s = syntiant_ndp120_tiny_poll(ndpp, &notifications, 1);
        if(s) {
            printf("mbwait tiny_poll failed: %d\n", s);
            return s;
        }
        max_try ++;
        ndp_handle->notification_save = notifications;
    } while (((notifications & SYNTIANT_NDP120_NOTIFICATION_MAILBOX_IN) == 0) 
        && ((notifications & SYNTIANT_NDP120_NOTIFICATION_EXTRACT_READY) == 0)
        && ((notifications & SYNTIANT_NDP120_NOTIFICATION_MAILBOX_OUT) == 0)
        && (max_try <= MBWAIT_MAX_TRY));

    return s;
}

int syntiant_tiny_flash_read(uint32_t flash_addr, uint8_t *read_buffer, 
        int read_size)
{
    syntiant_flash_spi_read(flash_addr, read_buffer, read_size);
    return read_size;
}

int syntiant_tiny_file_len(char *file_name)
{
    return syntiant_file_host_len(file_name);
}

int syntiant_tiny_file_read(char *file_name, uint32_t offset, 
        uint8_t *read_buffer, int read_size)
{
    syntiant_file_host_read(file_name, offset, read_buffer, 
        read_size);
    return read_size;
}

int syntiant_tiny_delayus(unsigned int delay_us)
{
    syntiant_ndp_delay_us(delay_us);
    return 0;
}
