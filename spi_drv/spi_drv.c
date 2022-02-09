#include "spi_drv.h"

uint8_t spi_rec_send_byte_blocked(SPI_TypeDef * SPI_X, uint8_t * sdata, uint8_t * rdata)
{
    int timeout = 0;
    
    SPI_X->DR = *sdata;

    while( !( SPI_X->SR & SPI_I2S_FLAG_RXNE ) ) {
        timeout++;
        if( timeout == 0xffff )
            return 0;
    }
    *rdata = SPI_X->DR;
    return 1;
}

uint8_t spi_rec_send_byte_array_blocked(SPI_TypeDef * SPI_X, uint8_t * sdata, uint8_t * rdata, int count)
{
    int ret_val;
    for( int i = 0 ; i < count ; i++ )
    {
        ret_val = spi_rec_send_byte_blocked(SPI_X, &sdata[i], &rdata[i]);
        if( !ret_val )
            return 0;
    }
    return 1;
}
