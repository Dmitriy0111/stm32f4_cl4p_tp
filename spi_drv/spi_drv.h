#ifndef __SPI_DRV_H
#define __SPI_DRV_H

#include "stm32f4xx_spi.h"

uint8_t spi_rec_send_byte_blocked(SPI_TypeDef * SPI_X, uint8_t * sdata, uint8_t * rdata);
uint8_t spi_rec_send_byte_array_blocked(SPI_TypeDef * SPI_X, uint8_t * sdata, uint8_t * rdata, int count);

uint8_t spi_send_byte_blocked(SPI_TypeDef * SPI_X, uint8_t * sdata);
uint8_t spi_send_byte_array_blocked(SPI_TypeDef * SPI_X, uint8_t * sdata, int count);

uint8_t spi_rec_byte_blocked(SPI_TypeDef * SPI_X, uint8_t * rdata);
uint8_t spi_rec_byte_array_blocked(SPI_TypeDef * SPI_X, uint8_t * rdata, int count);

#endif /* __SPI_DRV_H */
