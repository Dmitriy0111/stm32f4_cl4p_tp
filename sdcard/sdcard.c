#include "sdcard.h"

uint8_t cmd_sarr [8];
uint8_t cmd_rarr [8];

uint8_t send_byte = 0xff;
uint8_t rec_byte = 0xff;

void set_sd_cs(GPIO_TypeDef * GPIO_X, int GPIO_NSS, int val)
{
    if( val )
        GPIO_X->BSRRL = 0x1 << GPIO_NSS;
    else
        GPIO_X->BSRRH = 0x1 << GPIO_NSS;
}

int sdcard_WaitNotBusy(sdcard_typedef * sdcard_x)
{
    int ret_val;
    int cnt = 0;

    send_byte = 0xff;

    do
    {
        ret_val = spi_rec_send_byte_blocked(sdcard_x->SPI_X,&send_byte,&rec_byte);
        if( ( cnt == 0xff ) && ( !ret_val ) )
        {
            set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 1);
            return 0;
        }
        cnt++;
    } while( rec_byte != 0xff );
    
    return 1;
}

int sdcard_WaitDataToken(sdcard_typedef * sdcard_x, int TV)
{
    int ret_val;
    int cnt = 0;

    send_byte = 0xff;

    do
    {
        ret_val = spi_rec_send_byte_blocked(sdcard_x->SPI_X,&send_byte,&rec_byte);
        if( ( cnt == 0xff ) && ( !ret_val ) )
        {
            set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 1);
            return 0;
        }
        cnt++;
    } while( rec_byte != TV );
    
    return 1;
}

int sdcard_send_init_clk(sdcard_typedef * sdcard_x)
{
    int ret_val;

    send_byte = 0xff;

    // MISO is pullup, NSS is high, make more clocks (74)
    set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 1);
    for( int i = 0 ; i < 10 ; i++ )
    {
        ret_val = spi_rec_send_byte_blocked(sdcard_x->SPI_X,&send_byte,&rec_byte);
        if( !ret_val )
            return 0;
    }
    return 1;
}

int sdcard_init(sdcard_typedef * sdcard_x)
{
    int ret_val = 0;
    int cnt = 0;
    uint8_t send_byte = 0xff;
    uint8_t rec_byte = 0xff;
    ////////////////////////////////////////////////////////////////////////////////
    // send init clock
    ////////////////////////////////////////////////////////////////////////////////
    ret_val = sdcard_send_init_clk(sdcard_x);
    if( !ret_val )
        return 0;

    ////////////////////////////////////////////////////////////////////////////////
    // Send CMD0 and get response
    ////////////////////////////////////////////////////////////////////////////////
    set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 0);

    ret_val = sdcard_WaitNotBusy(sdcard_x);
    if( !ret_val )
        return 0;

    // form CMD0
    cmd_sarr[0] = CMD0_0;   // CMD0
    cmd_sarr[1] = CMD0_1;   // ARG0
    cmd_sarr[2] = CMD0_2;   // ARG1
    cmd_sarr[3] = CMD0_3;   // ARG2
    cmd_sarr[4] = CMD0_4;   // ARG3
    cmd_sarr[5] = CMD0_5;   // CRC
    // send CMD0
    ret_val = spi_rec_send_byte_array_blocked(sdcard_x->SPI_X, cmd_sarr, cmd_rarr, 6);
    // wait CMD0 R1
    cnt = 0;
    do
    {
        ret_val = spi_rec_send_byte_blocked(sdcard_x->SPI_X,&send_byte,&rec_byte);
        if( cnt == 0xff )
            break;
        cnt++;
    }
    while( rec_byte & 0x80 );

    set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 1);
    if( rec_byte != 0x01 )
        return 0;

    ////////////////////////////////////////////////////////////////////////////////
    // Send CMD8 and get response
    ////////////////////////////////////////////////////////////////////////////////
    set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 0);

    ret_val = sdcard_WaitNotBusy(sdcard_x);
    if( !ret_val )
        return 0;
    // form CMD8
    cmd_sarr[0] = CMD8_0;   // CMD8
    cmd_sarr[1] = CMD8_1;   // ARG0
    cmd_sarr[2] = CMD8_2;   // ARG1
    cmd_sarr[3] = CMD8_3;   // ARG2
    cmd_sarr[4] = CMD8_4;   // ARG3
    cmd_sarr[5] = CMD8_5;   // CRC
    // send CMD8
    ret_val = spi_rec_send_byte_array_blocked(sdcard_x->SPI_X, cmd_sarr, cmd_rarr, 6);
    // wait CMD8 R1
    cnt = 0;
    do
    {
        ret_val = spi_rec_send_byte_blocked(sdcard_x->SPI_X,&send_byte,&rec_byte);
        if( cnt == 0xff )
            break;
        cnt++;
    }
    while( rec_byte & 0x80 );

    if( rec_byte != 0x01 )
        return 0;   // SDSC is not supported

    cmd_sarr[0] = 0xff;
    cmd_sarr[1] = 0xff;
    cmd_sarr[2] = 0xff;
    cmd_sarr[3] = 0xff;
    ret_val = spi_rec_send_byte_array_blocked(sdcard_x->SPI_X, cmd_sarr, cmd_rarr, 4);
    if( !( (cmd_rarr[2] == 0x01) && (cmd_rarr[3] == 0xAA) ) )
        return 0;

    set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 1);

    ////////////////////////////////////////////////////////////////////////////////    
    // Send (CMD55 and ACMD41) and get response
    ////////////////////////////////////////////////////////////////////////////////
    while(1)
    {
        ////////////////////////////////////////////////////////////////////////////////
        // Send CMD55 and get response
        ////////////////////////////////////////////////////////////////////////////////
        set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 0);

        ret_val = sdcard_WaitNotBusy(sdcard_x);
        if( !ret_val )
            return 0;
        // form CMD55
        cmd_sarr[0] = CMD55_0;  // CMD55
        cmd_sarr[1] = CMD55_1;  // ARG0
        cmd_sarr[2] = CMD55_2;  // ARG1
        cmd_sarr[3] = CMD55_3;  // ARG2
        cmd_sarr[4] = CMD55_4;  // ARG3
        cmd_sarr[5] = CMD55_5;  // CRC
        // send CMD55
        ret_val = spi_rec_send_byte_array_blocked(sdcard_x->SPI_X, cmd_sarr, cmd_rarr, 6);
        // wait CMD55 R1
        cnt = 0;
        do
        {
            ret_val = spi_rec_send_byte_blocked(sdcard_x->SPI_X,&send_byte,&rec_byte);
            if( cnt == 0xff )
                break;
            cnt++;
        }
        while( rec_byte & 0x80 );

        if( rec_byte != 0x01 )
            return 0;

        set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 1);
        ////////////////////////////////////////////////////////////////////////////////
        // Send ACMD41 and get response
        ////////////////////////////////////////////////////////////////////////////////
        set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 0);

        ret_val = sdcard_WaitNotBusy(sdcard_x);
        if( !ret_val )
            return 0;
        // form ACMD41
        cmd_sarr[0] = ACMD41_0; // ACMD41
        cmd_sarr[1] = ACMD41_1; // ARG0
        cmd_sarr[2] = ACMD41_2; // ARG1
        cmd_sarr[3] = ACMD41_3; // ARG2
        cmd_sarr[4] = ACMD41_4; // ARG3
        cmd_sarr[5] = ACMD41_5; // CRC
        // send ACMD41
        ret_val = spi_rec_send_byte_array_blocked(sdcard_x->SPI_X, cmd_sarr, cmd_rarr, 6);
        // wait ACMD41 R1
        cnt = 0;
        do
        {
            ret_val = spi_rec_send_byte_blocked(sdcard_x->SPI_X,&send_byte,&rec_byte);
            if( cnt == 0xff )
                break;
            cnt++;
        }
        while( rec_byte & 0x80 );

        set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 1);

        if( rec_byte == 0x00 )
            break;

        if( rec_byte != 0x01 )
            return 0;
    }
    sdcard_x->state = 1;

    ////////////////////////////////////////////////////////////////////////////////
    // Send CMD58 and get response
    ////////////////////////////////////////////////////////////////////////////////
    set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 0);

    ret_val = sdcard_WaitNotBusy(sdcard_x);
    if( !ret_val )
        return 0;
    // form CMD58
    cmd_sarr[0] = CMD58_0;  // CMD58
    cmd_sarr[1] = CMD58_1;  // ARG0
    cmd_sarr[2] = CMD58_2;  // ARG1
    cmd_sarr[3] = CMD58_3;  // ARG2
    cmd_sarr[4] = CMD58_4;  // ARG3
    cmd_sarr[5] = CMD58_5;  // CRC
    // send CMD58
    ret_val = spi_rec_send_byte_array_blocked(sdcard_x->SPI_X, cmd_sarr, cmd_rarr, 6);
    // wait CMD58 R1
    cnt = 0;
    do
    {
        ret_val = spi_rec_send_byte_blocked(sdcard_x->SPI_X,&send_byte,&rec_byte);
        if( cnt == 0xff )
            break;
        cnt++;
    }
    while( rec_byte & 0x80 );

    if( rec_byte != 0x00 )
        return 0;

    cmd_sarr[0] = 0xff;
    cmd_sarr[1] = 0xff;
    cmd_sarr[2] = 0xff;
    cmd_sarr[3] = 0xff;
    ret_val = spi_rec_send_byte_array_blocked(sdcard_x->SPI_X, cmd_sarr, cmd_rarr, 4);
    if( cmd_rarr[0] & 0x40 )
        sdcard_x->echc_sc = 1;

    set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 1);

    return 1;
}

int sdcard_ReadSingleBlock(sdcard_typedef * sdcard_x, uint32_t BlockNum, uint8_t * read_buf)
{
    int ret_val;
    int cnt;
    uint8_t crc[2];

    // check sd card last state
    if( !sdcard_x->state )
        return -1;
    
    set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 0);

    ret_val = sdcard_WaitNotBusy(sdcard_x);
    if( !ret_val )
        return 0;
    // form CMD17
    cmd_sarr[0] = CMD17_0;
    cmd_sarr[1] = (BlockNum >> 24) & 0xff;
    cmd_sarr[2] = (BlockNum >> 16) & 0xff;
    cmd_sarr[3] = (BlockNum >>  8) & 0xff;
    cmd_sarr[4] = (BlockNum >>  0) & 0xff;
    cmd_sarr[5] = CMD17_5;
    // send CMD17
    ret_val = spi_rec_send_byte_array_blocked(sdcard_x->SPI_X, cmd_sarr, cmd_rarr, 6);
    // wait CMD17 R1
    cnt = 0;
    do
    {
        ret_val = spi_rec_send_byte_blocked(sdcard_x->SPI_X,&send_byte,&rec_byte);
        if( cnt == 0xff )
            break;
        cnt++;
    }
    while( rec_byte & 0x80 );

    if( rec_byte != 0x00 )
    {
        set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 1);
        return 0;
    }

    ret_val = sdcard_WaitDataToken(sdcard_x, DT_CMD17);
    if( !ret_val )
    {
        set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 1);
        return -2;
    }

    ret_val = spi_rec_byte_array_blocked(sdcard_x->SPI_X, read_buf, 512);
    if( !ret_val )
    {
        set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 1);
        return 0;
    }

    ret_val = spi_rec_byte_array_blocked(sdcard_x->SPI_X, crc, 2);
    if( !ret_val )
    {
        set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 1);
        return 0;
    }
    set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 1);

    return 1;
}

int sdcard_WriteSingleBlock(sdcard_typedef * sdcard_x, uint32_t BlockNum, uint8_t * write_buf)
{
    int ret_val;
    int cnt;
    uint8_t crc[2] = { 0xff, 0xff };

    // check sd card last state
    if( !sdcard_x->state )
        return -1;
    
    set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 0);

    ret_val = sdcard_WaitNotBusy(sdcard_x);
    if( !ret_val )
        return 0;
    // form CMD24
    cmd_sarr[0] = CMD24_0;
    cmd_sarr[1] = (BlockNum >> 24) & 0xff;
    cmd_sarr[2] = (BlockNum >> 16) & 0xff;
    cmd_sarr[3] = (BlockNum >>  8) & 0xff;
    cmd_sarr[4] = (BlockNum >>  0) & 0xff;
    cmd_sarr[5] = CMD24_5;
    // send CMD24
    ret_val = spi_rec_send_byte_array_blocked(sdcard_x->SPI_X, cmd_sarr, cmd_rarr, 6);
    // wait CMD24 R1
    cnt = 0;
    do
    {
        ret_val = spi_rec_send_byte_blocked(sdcard_x->SPI_X,&send_byte,&rec_byte);
        if( cnt == 0xff )
            break;
        cnt++;
    }
    while( rec_byte & 0x80 );

    if( rec_byte != 0x00 )
    {
        set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 1);
        return 0;
    }
    send_byte = DT_CMD24;
    // send Data Token
    ret_val = spi_rec_send_byte_blocked(sdcard_x->SPI_X, &send_byte,&rec_byte);
    send_byte = 0xff;
    // send data
    ret_val &= spi_send_byte_array_blocked(sdcard_x->SPI_X, write_buf, 512);
    // send crc
    ret_val &= spi_rec_send_byte_array_blocked(sdcard_x->SPI_X, crc, crc, 2);
    // rec responce
    ret_val &= spi_rec_byte_blocked(sdcard_x->SPI_X, &rec_byte);
    if( !ret_val )
    {
        set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 1);
        return 0;
    }

    if( ( rec_byte & 0x1F ) != 0x05 ) {
        set_sd_cs(sdcard_x->GPIO_X, sdcard_x->GPIO_NSS, 1);
        return -2;
    }

    return 1;
}
