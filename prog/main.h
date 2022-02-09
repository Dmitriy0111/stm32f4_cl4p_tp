/**
  ******************************************************************************
  * @file    main.h 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    Apr-2013
  * @brief   Header for main.c module
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "spi_drv.h"
#include "sdcard.h"

#define SPI2_GPIO   GPIOB
#define SPI2_NSS    12
#define SPI2_SCK    13
#define SPI2_MISO   14
#define SPI2_MOSI   15

// SD card
//         ____________________________________
//        / 9 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
//       /    |   |   |   |   |   |   |   |   |
//       |    |___|___|___|___|___|___|___|___|
//       |____|                               |
//       |                                    |
//       **************************************
//       |                                    |
//       |____________________________________|
//
// 1 - CS (NSS)
// 2 - DI (MOSI)
// 3 - VSS
// 4 - VDD
// 5 - CLK
// 6 - VSS
// 7 - DO (MISO)
// 8 - NC
// 9 - NC

#define LED_GPIO    GPIOD
#define LED0        12
#define LED1        13
#define LED2        14
#define LED3        15

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void gpiob_init(void);
void gpiod_init(void);

void spi2_init(void);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
