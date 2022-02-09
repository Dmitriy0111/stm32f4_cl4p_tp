#include "sdcard.h"

extern uint8_t send_arr [128];
extern uint8_t rec_arr  [128];

uint8_t send_byte = 0xff;
uint8_t rec_byte = 0xff;

void set_sd_cs(GPIO_TypeDef * GPIO_X, int GPIO_NSS, int val)
{
    if( val )
        GPIO_X->BSRRL = 0x1 << GPIO_NSS;
    else
        GPIO_X->BSRRH = 0x1 << GPIO_NSS;
}

int sdcard_WaitNotBusy(SPI_TypeDef * SPI_X, GPIO_TypeDef * GPIO_X, int GPIO_NSS)
{
    int ret_val;
    int cnt = 0;

    send_byte = 0xff;

    do
    {
        ret_val = spi_rec_send_byte_blocked(SPI_X,&send_byte,&rec_byte);
        if( ( cnt == 0xff ) && ( !ret_val ) )
        {
            set_sd_cs(GPIO_X, GPIO_NSS, 1);
            return 0;
        }
        cnt++;
    } while( rec_byte != 0xff );
    
    return 1;
}

int sdcard_send_init_clk(SPI_TypeDef * SPI_X, GPIO_TypeDef * GPIO_X, int GPIO_NSS)
{
    int ret_val;

    send_byte = 0xff;

    // MISO is pullup, NSS is high, make more clocks (74)
    set_sd_cs(GPIO_X, GPIO_NSS, 1);
    for( int i = 0 ; i < 10 ; i++ )
    {
        ret_val = spi_rec_send_byte_blocked(SPI_X,&send_byte,&rec_byte);
        if( !ret_val )
            return 0;
    }
    return 1;
}

int sdcard_init(SPI_TypeDef * SPI_X, GPIO_TypeDef * GPIO_X, int GPIO_NSS)
{
    int ret_val = 0;
    int cnt = 0;
    uint8_t send_byte = 0xff;
    uint8_t rec_byte = 0xff;
    ////////////////////////////////////////////////////////////////////////////////
    // send init clock
    ////////////////////////////////////////////////////////////////////////////////
    ret_val = sdcard_send_init_clk(SPI_X, GPIO_X, GPIO_NSS);
    if( !ret_val )
        return 0;

    ////////////////////////////////////////////////////////////////////////////////
    // Send CMD0 and get response
    ////////////////////////////////////////////////////////////////////////////////
    set_sd_cs(GPIO_X, GPIO_NSS, 0);

    ret_val = sdcard_WaitNotBusy(SPI_X, GPIO_X, GPIO_NSS);
    if( !ret_val )
        return 0;

    // form CMD0
    send_arr[0] = CMD0_0;   // CMD0
    send_arr[1] = CMD0_1;   // ARG0
    send_arr[2] = CMD0_2;   // ARG1
    send_arr[3] = CMD0_3;   // ARG2
    send_arr[4] = CMD0_4;   // ARG3
    send_arr[5] = CMD0_5;   // CRC
    // send CMD0
    ret_val = spi_rec_send_byte_array_blocked(SPI_X, send_arr, rec_arr, 6);
    // wait CMD0 R1
    cnt = 0;
    do
    {
        ret_val = spi_rec_send_byte_blocked(SPI_X,&send_byte,&rec_byte);
        if( cnt == 0xff )
            break;
        cnt++;
    }
    while( rec_byte & 0x80 );

    set_sd_cs(GPIO_X, GPIO_NSS, 1);
    if( rec_byte != 0x01 )
        return 0;

    ////////////////////////////////////////////////////////////////////////////////
    // Send CMD8 and get response
    ////////////////////////////////////////////////////////////////////////////////
    set_sd_cs(GPIO_X, GPIO_NSS, 0);

    ret_val = sdcard_WaitNotBusy(SPI_X, GPIO_X, GPIO_NSS);
    if( !ret_val )
        return 0;
    // form CMD8
    send_arr[0] = CMD8_0;   // CMD8
    send_arr[1] = CMD8_1;   // ARG0
    send_arr[2] = CMD8_2;   // ARG1
    send_arr[3] = CMD8_3;   // ARG2
    send_arr[4] = CMD8_4;   // ARG3
    send_arr[5] = CMD8_5;   // CRC
    // send CMD8
    ret_val = spi_rec_send_byte_array_blocked(SPI_X, send_arr, rec_arr, 6);
    // wait CMD8 R1
    cnt = 0;
    do
    {
        ret_val = spi_rec_send_byte_blocked(SPI_X,&send_byte,&rec_byte);
        if( cnt == 0xff )
            break;
        cnt++;
    }
    while( rec_byte & 0x80 );

    if( rec_byte != 0x01 )
        return 0;   // SDSC is not supported

    send_arr[0] = 0xff;
    send_arr[1] = 0xff;
    send_arr[2] = 0xff;
    send_arr[3] = 0xff;
    ret_val = spi_rec_send_byte_array_blocked(SPI_X, send_arr, rec_arr, 4);
    if( !( (rec_arr[2] == 0x01) && (rec_arr[3] == 0xAA) ) )
        return 0;

    set_sd_cs(GPIO_X, GPIO_NSS, 1);

    ////////////////////////////////////////////////////////////////////////////////    
    // Send (CMD55 and ACMD41) and get response
    ////////////////////////////////////////////////////////////////////////////////
    while(1)
    {
        ////////////////////////////////////////////////////////////////////////////////
        // Send CMD55 and get response
        ////////////////////////////////////////////////////////////////////////////////
        set_sd_cs(GPIO_X, GPIO_NSS, 0);

        ret_val = sdcard_WaitNotBusy(SPI_X, GPIO_X, GPIO_NSS);
        if( !ret_val )
            return 0;
        // form CMD55
        send_arr[0] = CMD55_0;  // CMD55
        send_arr[1] = CMD55_1;  // ARG0
        send_arr[2] = CMD55_2;  // ARG1
        send_arr[3] = CMD55_3;  // ARG2
        send_arr[4] = CMD55_4;  // ARG3
        send_arr[5] = CMD55_5;  // CRC
        // send CMD55
        ret_val = spi_rec_send_byte_array_blocked(SPI_X, send_arr, rec_arr, 6);
        // wait CMD55 R1
        cnt = 0;
        do
        {
            ret_val = spi_rec_send_byte_blocked(SPI_X,&send_byte,&rec_byte);
            if( cnt == 0xff )
                break;
            cnt++;
        }
        while( rec_byte & 0x80 );

        if( rec_byte != 0x01 )
            return 0;

        set_sd_cs(GPIO_X, GPIO_NSS, 1);
        ////////////////////////////////////////////////////////////////////////////////
        // Send ACMD41 and get response
        ////////////////////////////////////////////////////////////////////////////////
        set_sd_cs(GPIO_X, GPIO_NSS, 0);

        ret_val = sdcard_WaitNotBusy(SPI_X, GPIO_X, GPIO_NSS);
        if( !ret_val )
            return 0;
        // form ACMD41
        send_arr[0] = ACMD41_0; // ACMD41
        send_arr[1] = ACMD41_1; // ARG0
        send_arr[2] = ACMD41_2; // ARG1
        send_arr[3] = ACMD41_3; // ARG2
        send_arr[4] = ACMD41_4; // ARG3
        send_arr[5] = ACMD41_5; // CRC
        // send ACMD41
        ret_val = spi_rec_send_byte_array_blocked(SPI_X, send_arr, rec_arr, 6);
        // wait ACMD41 R1
        cnt = 0;
        do
        {
            ret_val = spi_rec_send_byte_blocked(SPI_X,&send_byte,&rec_byte);
            if( cnt == 0xff )
                break;
            cnt++;
        }
        while( rec_byte & 0x80 );

        set_sd_cs(GPIO_X, GPIO_NSS, 1);

        if( rec_byte == 0x00 )
            break;

        if( rec_byte != 0x01 )
            return 0;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Send CMD58 and get response
    ////////////////////////////////////////////////////////////////////////////////
    set_sd_cs(GPIO_X, GPIO_NSS, 0);

    ret_val = sdcard_WaitNotBusy(SPI_X, GPIO_X, GPIO_NSS);
    if( !ret_val )
        return 0;
    // form CMD58
    send_arr[0] = CMD58_0;  // CMD58
    send_arr[1] = CMD58_1;  // ARG0
    send_arr[2] = CMD58_2;  // ARG1
    send_arr[3] = CMD58_3;  // ARG2
    send_arr[4] = CMD58_4;  // ARG3
    send_arr[5] = CMD58_5;  // CRC
    // send CMD58
    ret_val = spi_rec_send_byte_array_blocked(SPI_X, send_arr, rec_arr, 6);
    // wait CMD58 R1
    cnt = 0;
    do
    {
        ret_val = spi_rec_send_byte_blocked(SPI_X,&send_byte,&rec_byte);
        if( cnt == 0xff )
            break;
        cnt++;
    }
    while( rec_byte & 0x80 );

    if( rec_byte != 0x00 )
        return 0;

    send_arr[0] = 0xff;
    send_arr[1] = 0xff;
    send_arr[2] = 0xff;
    send_arr[3] = 0xff;
    ret_val = spi_rec_send_byte_array_blocked(SPI_X, send_arr, rec_arr, 4);
    if( (rec_arr[2] & 0xC0) != 0xC0 )
        return 0;

    set_sd_cs(GPIO_X, GPIO_NSS, 1);

    return 1;
}
