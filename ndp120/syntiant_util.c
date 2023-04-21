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

#include "syntiant_common.h"

#include "syntiant_util.h"
#include "syntiant_driver.h"

/************************************************************/
/* SPI handler declaration */
int syntiant_tiny_io_init(void *d, int spi_default_speed)
{
    int s;
    int spi_read_delay = 1;
    struct ndp120_tiny_handle_t *ndp_handle = (struct ndp120_tiny_handle_t*)d;

    /*##-1- Initialize the SPI peripheral ##################*/
    s = syntiant_ndp_spi_init(&ndp_handle->spifd, spi_default_speed, spi_read_delay);
    return s;
}

int syntiant_tiny_transfer(void *d, int mcu, uint32_t addr, 
        void *out, void *in, unsigned int count)
{
    struct ndp120_tiny_handle_t *ndp_handle = (struct ndp120_tiny_handle_t*)d;

    return syntiant_ndp_transfer(&(ndp_handle->spifd), mcu, addr, out, in, count);
}

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

int syntiant_tiny_delayms(unsigned int delay_ms)
{
    syntiant_ndp_delay_ms(delay_ms);
    return 0;
}

int syntiant_tiny_delayus(unsigned int delay_us)
{
    syntiant_ndp_delay_us(delay_us);
    return 0;
}
