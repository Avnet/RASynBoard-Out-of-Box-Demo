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
 	** SDK: v105 **
*/

#include <stdint.h>

#include "syntiant_ilib/syntiant_ndp120_tiny.h"
#include "syntiant_ilib/syntiant_spictl_regs.h"
#include "syntiant_ilib/syntiant_tiny_cspi.h"

#define MSPI_CLK_DIV 2
#define MSSB_OE_USED 7

/* Wait for SPI operation to complete */
static int cspi_poll_done(struct syntiant_ndp120_tiny_device_s *ndp)
{
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint8_t done = 0;
    uint32_t spictl;

    while (!done) {
        s = syntiant_ndp120_tiny_read_block(
            ndp, SYNTIANT_NDP120_MCU_OP, NDP120_CHIP_CONFIG_SPICTL, &spictl, 4);
        if (s) break;
        done = NDP120_CHIP_CONFIG_SPICTL_DONE_EXTRACT(spictl);
    }

    return s;
}

/* end a SPI packet */
static int cspi_end(struct syntiant_ndp120_tiny_device_s *ndp)
{
    int s;
    uint32_t data;

    s = syntiant_ndp120_tiny_read_block(
        ndp, SYNTIANT_NDP120_MCU_OP, NDP120_CHIP_CONFIG_SPICTL, &data, 4);
    if (s) goto done;

    data = NDP120_CHIP_CONFIG_SPICTL_MODE_MASK_INSERT(data,
        NDP120_CHIP_CONFIG_SPICTL_MODE_STANDBY);
    s = syntiant_ndp120_tiny_write_block(
        ndp, SYNTIANT_NDP120_MCU_OP, NDP120_CHIP_CONFIG_SPICTL, &data, 4);
    if (s) goto done;

done:
    return s;
}

/*
 * Write up to 16 bytes of data
 */
static int _cspi_write(struct syntiant_ndp120_tiny_device_s *ndp,
    unsigned int ssb, unsigned int num_bytes, uint8_t *buf, int end_packet)
{
    int s;
    uint32_t data;
    unsigned len = (num_bytes + 3) & ~0x3U;

    if (len > 16) {
        s = SYNTIANT_NDP120_ERROR_ARG;
        goto done;
    }

    /* Turn on SSB and get ready */
    s = syntiant_ndp120_tiny_read_block(
        ndp, SYNTIANT_NDP120_MCU_OP, NDP120_CHIP_CONFIG_SPICTL, &data, 4);
    if (s) goto done;

    data = NDP120_CHIP_CONFIG_SPICTL_MODE_MASK_INSERT(data,
        NDP120_CHIP_CONFIG_SPICTL_MODE_SS);
    data = NDP120_CHIP_CONFIG_SPICTL_MSPI_READ_MASK_INSERT(data, 0x0);
    data = NDP120_CHIP_CONFIG_SPICTL_MSSB_OE_MASK_INSERT(data, MSSB_OE_USED);
    data = NDP120_CHIP_CONFIG_SPICTL_MSSB_MASK_INSERT(data, ssb);
    s = syntiant_ndp120_tiny_write_block(
        ndp, SYNTIANT_NDP120_MCU_OP, NDP120_CHIP_CONFIG_SPICTL, &data, 4);
    if (s) goto done;

    /* copy data */
    s = syntiant_ndp120_tiny_write_block(
        ndp, SYNTIANT_NDP120_MCU_OP, NDP120_CHIP_CONFIG_SPITX(0), buf, len);
    if (s) goto done;

    s = syntiant_ndp120_tiny_read_block(
        ndp, SYNTIANT_NDP120_MCU_OP, NDP120_CHIP_CONFIG_SPICTL, &data, 4);
    if (s) goto done;
    data = NDP120_CHIP_CONFIG_SPICTL_MODE_MASK_INSERT(data,
        NDP120_CHIP_CONFIG_SPICTL_MODE_TRANSFER);
    data = NDP120_CHIP_CONFIG_SPICTL_NUMBYTES_MASK_INSERT(data, num_bytes - 1);
    s = syntiant_ndp120_tiny_write_block(
        ndp, SYNTIANT_NDP120_MCU_OP, NDP120_CHIP_CONFIG_SPICTL, &data, 4);
    if (s) goto done;

    /* Wait for MSPI Done */
    s = cspi_poll_done(ndp);
    if (s) goto done;

    /* Wait for next packet or end sequence */
    s = syntiant_ndp120_tiny_read_block(
        ndp, SYNTIANT_NDP120_MCU_OP, NDP120_CHIP_CONFIG_SPICTL, &data, 4);
    if (s) goto done;
        data = NDP120_CHIP_CONFIG_SPICTL_MODE_MASK_INSERT(data, end_packet ?
            NDP120_CHIP_CONFIG_SPICTL_MODE_SS :
            NDP120_CHIP_CONFIG_SPICTL_MODE_UPDATE);

    s = syntiant_ndp120_tiny_write_block(
        ndp, SYNTIANT_NDP120_MCU_OP, NDP120_CHIP_CONFIG_SPICTL, &data, 4);
    if (s) goto done;

    if (end_packet) {
        s = cspi_end(ndp);
        if (s) goto done;
    }

done:
    return s;
}

/*
 * Read up to 16 bytes of data
 * Generally, the function assumes that write_cspi is already called with end_packet=False
 */
static int _cspi_read(struct syntiant_ndp120_tiny_device_s *ndp,
    unsigned int ssb, unsigned int num_bytes, uint8_t *buf, int end_packet)
{
    int s;
    uint32_t data;
    unsigned len = (num_bytes + 3) & ~0x3U;

    if (len > 16) {
        s = SYNTIANT_NDP120_ERROR_ARG;
        goto done;
    }

    s = syntiant_ndp120_tiny_read_block(
        ndp, SYNTIANT_NDP120_MCU_OP, NDP120_CHIP_CONFIG_SPICTL, &data, 4);
    if (s) goto done;
    data = NDP120_CHIP_CONFIG_SPICTL_MODE_MASK_INSERT(data,
        NDP120_CHIP_CONFIG_SPICTL_MODE_TRANSFER);
    data = NDP120_CHIP_CONFIG_SPICTL_MSPI_READ_MASK_INSERT(data, 0x1);
    data = NDP120_CHIP_CONFIG_SPICTL_NUMBYTES_MASK_INSERT(data, num_bytes - 1);
    data = NDP120_CHIP_CONFIG_SPICTL_MSSB_OE_MASK_INSERT(data, MSSB_OE_USED);
    data = NDP120_CHIP_CONFIG_SPICTL_MSSB_MASK_INSERT(data, ssb);
    s = syntiant_ndp120_tiny_write_block(
        ndp, SYNTIANT_NDP120_MCU_OP, NDP120_CHIP_CONFIG_SPICTL, &data, 4);
    if (s) goto done;

    /* wait for mspi done */
    s = cspi_poll_done(ndp);
    if (s) goto done;

    s = syntiant_ndp120_tiny_read_block(
        ndp, SYNTIANT_NDP120_MCU_OP, NDP120_CHIP_CONFIG_SPIRX(0), buf, len);
    if (s) goto done;

    /* Wait for next packet or end sequence */
    s = syntiant_ndp120_tiny_read_block(
        ndp, SYNTIANT_NDP120_MCU_OP, NDP120_CHIP_CONFIG_SPICTL, &data, 4);
    if (s) goto done;
        data = NDP120_CHIP_CONFIG_SPICTL_MODE_MASK_INSERT(data, end_packet ?
            NDP120_CHIP_CONFIG_SPICTL_MODE_SS :
            NDP120_CHIP_CONFIG_SPICTL_MODE_UPDATE);
    s = syntiant_ndp120_tiny_write_block(
        ndp, SYNTIANT_NDP120_MCU_OP, NDP120_CHIP_CONFIG_SPICTL, &data, 4);
    if (s) goto done;

    if (end_packet) {
        s = cspi_end(ndp);
        if (s) goto done;
    }

done:
    return s;
}

int syntiant_cspi_init(struct syntiant_ndp120_tiny_device_s *ndp)
{
    int s;
    uint32_t data;

    s = syntiant_ndp120_tiny_read_block(
        ndp, SYNTIANT_NDP120_MCU_OP, NDP120_CHIP_CONFIG_SPICTL, &data, 4);
    if (s) goto done;
    data = NDP120_CHIP_CONFIG_SPICTL_MSPI_MODE_MASK_INSERT(data,
        NDP120_CHIP_CONFIG_SPICTL_MSPI_MODE_FOUR_WIRE);
    data = NDP120_CHIP_CONFIG_SPICTL_MSSB_OE_MASK_INSERT(data, MSSB_OE_USED);
    data = NDP120_CHIP_CONFIG_SPICTL_MODE_MASK_INSERT(data,
        NDP120_CHIP_CONFIG_SPICTL_MODE_STANDBY);
    data = NDP120_CHIP_CONFIG_SPICTL_MSPI_CLKDIV_MASK_INSERT(data, MSPI_CLK_DIV);
    s = syntiant_ndp120_tiny_write_block(
        ndp, SYNTIANT_NDP120_MCU_OP, NDP120_CHIP_CONFIG_SPICTL, &data, 4);
    if (s) goto done;

    if (ndp->device_type != SYNTIANT_NDP115_A0) {
        goto done;
    }

    /* setup pinmux for ndp115 */
    s = syntiant_ndp120_tiny_read_block(
        ndp, SYNTIANT_NDP120_MCU_OP, NDP120_CHIP_CONFIG_PINOUTMUX, &data, 4);
    if (s) goto done;
    data = NDP120_CHIP_CONFIG_PINOUTMUX_MSPI_PIN_SEL_MASK_INSERT(data,
        NDP120_CHIP_CONFIG_PINOUTMUX_MSPI_PIN_SEL_MSPI);
    s = syntiant_ndp120_tiny_write_block(
        ndp, SYNTIANT_NDP120_MCU_OP, NDP120_CHIP_CONFIG_PINOUTMUX, &data, 4);
    if (s) goto done;

done:
    return s;
}

int syntiant_cspi_read(struct syntiant_ndp120_tiny_device_s *ndp,
    unsigned int ssb, unsigned int num_bytes, void* buf, int end_packet)
{
    int s;
    unsigned int chunk = 16;
    uint8_t *data = (uint8_t *)buf;

    /* split up read in chunks of 16 bytes */
    while (num_bytes) {
        if (num_bytes < chunk) {
            chunk = num_bytes;
        }
        num_bytes -= chunk;
        s = _cspi_read(ndp, ssb, chunk, data, num_bytes ? 0 : end_packet);
        if (s) break;
        data += chunk;
    }

    return s;
}

int syntiant_cspi_write(struct syntiant_ndp120_tiny_device_s *ndp,
    unsigned int ssb, unsigned int num_bytes, void* buf, int end_packet)
{
    int s;
    unsigned int chunk = 16;
    uint8_t *data = (uint8_t *)buf;

    /* split up write in chunks of max 16 bytes */
    while (num_bytes) {
        if (num_bytes < chunk) {
            chunk = num_bytes;
        }
        num_bytes -= chunk;
        s = _cspi_write(ndp, ssb, chunk, data, num_bytes ? 0 : end_packet);
        if (s) break;
        data += chunk;
    }

    return s;
}
