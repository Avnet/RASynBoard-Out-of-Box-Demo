/*
 * spi_drv.h
 *
 *  Created on: Dec 18, 2022
 *      Author: david
 */

#ifndef SPI_DRV_H_
#define SPI_DRV_H_

#include <stdint.h>

int spi_write(uint8_t *data, uint32_t bytes);
int spi_read(uint8_t *buf, uint32_t size);
int spi_write_read(uint8_t *out, uint8_t *in, uint32_t size);
int spi_init(void);

void spi_deassert_sd(void);
void spi_deassert_ndp(void);
void spi_assert_sd(void);
void spi_assert_ndp(void);
int sdmmc_exist_check(void);

#endif /* SPI_DRV_H_ */
