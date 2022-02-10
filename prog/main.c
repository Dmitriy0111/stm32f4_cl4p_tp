/**
  ******************************************************************************
  * @file    main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    Apr-2013
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#ifndef MAIN__H
#define MAIN__H

#include "main.h"

/** @addtogroup STM32F4xx_IEC60335_Example
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef* GPIO_;

sdcard_typedef sdcard_0;

uint8_t send_arr [1024];
uint8_t rec_arr  [1024];
uint32_t blockNum;
/* Private function prototypes -----------------------------------------------*/

static void NVIC_Configuration(void);

void SystemInit (void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  : None
  * @retval : None
  */
int32_t main(void)
{
    uint32_t delay;

    SystemCoreClockUpdate();
    /* NVIC configuration ------------------------------------------------------*/
    NVIC_Configuration();
    //gpiob_init();
    gpiod_init();
    spi2_init();
    set_sd_cs(GPIOB, SPI2_NSS, 1);

    for(int i = 0 ; i < 512; i++)
        send_arr[i] = i;


    sdcard_0.SPI_X = SPI2;
    sdcard_0.GPIO_X = GPIOB;
    sdcard_0.GPIO_NSS = SPI2_NSS;
    sdcard_0.state = 0;
    sdcard_0.echc_sc = 0;

    blockNum = 0;

    GPIOD->BSRRL = 0x1 << LED3;

    if( sdcard_init(&sdcard_0) )
    {
        GPIOD->BSRRL = 0x1 << LED3;
    }
    else
    {
        GPIOD->BSRRH = 0x1 << LED3;
    }
    
    /* Add your application initialization code here  */

    /* Infinite loop */
    while(1)
    {
        for( delay = 0 ; delay < 8000000 ; delay++ ) ;
        GPIOD->ODR |= 0x2000;
        for( delay = 0 ; delay < 8000000 ; delay++ ) ;
        GPIOD->ODR &= ~0x2000;
        if( sdcard_0.state )
        {
            // if( sdcard_WriteSingleBlock(&sdcard_0, blockNum, send_arr) == 1 )
            // {
            //     GPIOD->BSRRL = 0x1 << LED0;
            // }
            // else
            // {
            //     GPIOD->BSRRH = 0x1 << LED0;
            // }
            if( sdcard_ReadSingleBlock(&sdcard_0, blockNum, rec_arr) == 1 )
            {
                GPIOD->BSRRL = 0x1 << LED0;
            }
            else
            {
                GPIOD->BSRRH = 0x1 << LED0;
            }
        }
        if( sdcard_0.echc_sc )
            blockNum++;
        else
            blockNum += 512;
    }
}

/* -------------------------------------------------------------------------*/
/**
  * @brief  Configures NVIC and Vector Table base location.
  * @param :  None
  * @retval : None
  */
static void NVIC_Configuration(void)
{
}
/* -------------------------------------------------------------------------*/
/**
  * @brief  Configures GPIO B.
  * @param :  None
  * @retval : None
  */
void gpiob_init()
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_PinAFConfig(GPIOB, SPI2_SCK , GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB, SPI2_MOSI, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB, SPI2_MISO, GPIO_AF_SPI2);

    GPIOB->MODER   |= (0x1 << (SPI2_NSS * 2)) | (0x2 << (SPI2_SCK * 2)) | (0x2 << (SPI2_MOSI * 2)) | (0x2 << (SPI2_MISO * 2));
    GPIOB->OTYPER  |= (0x0 << SPI2_NSS) | (0x0 << SPI2_SCK) | (0x0 << SPI2_MOSI) | (0x1 << SPI2_MISO);
    GPIOB->OSPEEDR |= (0x1 << (SPI2_NSS * 2)) | (0x1 << (SPI2_SCK * 2)) | (0x1 << (SPI2_MOSI * 2)) | (0x1 << (SPI2_MISO * 2));
    GPIOB->PUPDR   |= (0x1 << (SPI2_NSS * 2)) | (0x0 << (SPI2_SCK * 2)) | (0x0 << (SPI2_MOSI * 2)) | (0x1 << (SPI2_MISO * 2));
}
/* -------------------------------------------------------------------------*/
/**
  * @brief  Configures GPIO D.
  * @param :  None
  * @retval : None
  */
void gpiod_init()
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    GPIOD->MODER = (0x1 << (LED0 * 2)) | (0x1 << (LED1 * 2)) | (0x1 << (LED2 * 2)) | (0x1 << (LED3 * 2));
}
/* -------------------------------------------------------------------------*/
/**
  * @brief  Configures SPI2.
  * @param :  None
  * @retval : None
  */
void spi2_init()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    gpiob_init();
    // [15 : 15] BIDIMODE   = 0
    // [14 : 14] BIDIOE     = 0
    // [13 : 13] CRCEN      = 0
    // [12 : 12] CRCNEXT    = 0
    // [11 : 11] DFF        = 0
    // [10 : 10] RXONLY     = 0
    // [9  :  9] SSM        = 1
    // [8  :  8] SSI        = 1
    // [7  :  7] LSBFIRST   = 0
    // [6  :  6] SPE        = 0
    // [5  :  3] BR         = 111
    // [2  :  2] MSTR       = 1
    // [1  :  1] CPHA       = 0
    // [0  :  0] CPOL       = 0
    SPI2->CR1 |= 0b0000001100111100;
    // [15 :  8] Reserved   = 0
    // [7  :  7] TXEIE      = 0
    // [6  :  6] RXNEIE     = 0
    // [5  :  5] ERRIE      = 0
    // [4  :  4] FRF        = 0
    // [3  :  3] Reserved   = 0
    // [2  :  2] SSOE       = 0
    // [1  :  1] TXDMAEN    = 0
    // [0  :  0] RXDMAEN    = 0
    SPI2->CR2 |= 0b0000000000000000;
    // Enable SPI2
    SPI2->CR1 |= 0b0000000001000000;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval : None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

#endif /* MAIN__H */


/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
