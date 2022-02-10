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

// CMD17 Read Single Block
#define CMD17_0     0x51
#define CMD17_5     0xFF

// CMD24 Write Single Block
#define CMD24_0     0x58
#define CMD24_5     0xFF

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

// DATA TOKENS
#define DT_CMD9     0xFE
#define DT_CMD17    0xFE
#define DT_CMD18    0xFE
#define DT_CMD24    0xFE
#define DT_CMD25    0xFC

typedef struct 
{
    SPI_TypeDef *   SPI_X;
    GPIO_TypeDef *  GPIO_X;
    int             GPIO_NSS;
    uint8_t         state;
    uint8_t         echc_sc;
} sdcard_typedef;

void set_sd_cs(GPIO_TypeDef * GPIO_X, int GPIO_NSS, int val);
// init sd card
int sdcard_init(sdcard_typedef * sdcard_x);

int sdcard_WaitNotBusy(sdcard_typedef * sdcard_x);

int sdcard_WaitDataToken(sdcard_typedef * sdcard_x, int TV);

int sdcard_send_init_clk(sdcard_typedef * sdcard_x);

int sdcard_ReadSingleBlock(sdcard_typedef * sdcard_x, uint32_t BlockNum, uint8_t * read_buf);
int sdcard_WriteSingleBlock(sdcard_typedef * sdcard_x, uint32_t BlockNum, uint8_t * write_buf);

#endif /* __SDCARD_H */
