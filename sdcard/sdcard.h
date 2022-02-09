#ifndef __SDCARD_H
#define __SDCARD_H

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_spi.h"

#include "spi_drv.h"

// CMD0
#define CMD0_0      0x40
#define CMD0_1      0x00
#define CMD0_2      0x00
#define CMD0_3      0x00
#define CMD0_4      0x00
#define CMD0_5      0x95
// CMD8
#define CMD8_0      0x48
#define CMD8_1      0x00
#define CMD8_2      0x00
#define CMD8_3      0x01
#define CMD8_4      0xAA
#define CMD8_5      0x87

// ACMD41
#define ACMD41_0    0x69
#define ACMD41_1    0x40
#define ACMD41_2    0x00
#define ACMD41_3    0x00
#define ACMD41_4    0x00
#define ACMD41_5    0xff

// CMD55
#define CMD55_0     0x77
#define CMD55_1     0x00
#define CMD55_2     0x00
#define CMD55_3     0x00
#define CMD55_4     0x00
#define CMD55_5     0xff

// CMD58
#define CMD58_0     0x7A
#define CMD58_1     0x00
#define CMD58_2     0x00
#define CMD58_3     0x00
#define CMD58_4     0x00
#define CMD58_5     0xff

void set_sd_cs(GPIO_TypeDef * GPIO_X, int GPIO_NSS, int val);
// init sd card
int sdcard_init(SPI_TypeDef * SPI_X, GPIO_TypeDef * GPIO_X, int GPIO_NSS);

int sdcard_WaitNotBusy(SPI_TypeDef * SPI_X, GPIO_TypeDef * GPIO_X, int GPIO_NSS);

int sdcard_send_init_clk(SPI_TypeDef * SPI_X, GPIO_TypeDef * GPIO_X, int GPIO_NSS);

#endif /* __SDCARD_H */
