/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
I2C_HandleTypeDef* Get_I2C1_Instance(void);
I2C_HandleTypeDef* Get_I2C2_Instance(void);
SPI_HandleTypeDef* Get_SPI1_Instance(void);
UART_HandleTypeDef* Get_UART1_Instance(void);
UART_HandleTypeDef* Get_UART2_Instance(void);
RTC_HandleTypeDef* Get_RTC_Instance(void);
TIM_HandleTypeDef* Get_TIM2_Instance(void);
TIM_HandleTypeDef* Get_TIM3_Instance(void);
TIM_HandleTypeDef* Get_TIM5_Instance(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LR_NSS_Pin GPIO_PIN_13
#define LR_NSS_GPIO_Port GPIOC
#define LSE_IN_Pin GPIO_PIN_14
#define LSE_IN_GPIO_Port GPIOC
#define LSE_OUT_Pin GPIO_PIN_15
#define LSE_OUT_GPIO_Port GPIOC
#define HSE_IN_Pin GPIO_PIN_0
#define HSE_IN_GPIO_Port GPIOH
#define HSE_OUT_Pin GPIO_PIN_1
#define HSE_OUT_GPIO_Port GPIOH
#define LEDA_Pin GPIO_PIN_0
#define LEDA_GPIO_Port GPIOC
#define LEDB_Pin GPIO_PIN_1
#define LEDB_GPIO_Port GPIOC
#define LEDC_Pin GPIO_PIN_2
#define LEDC_GPIO_Port GPIOC
#define LEDD_Pin GPIO_PIN_3
#define LEDD_GPIO_Port GPIOC
#define LR_DIO0_Pin GPIO_PIN_4
#define LR_DIO0_GPIO_Port GPIOC
#define LR_DIO1_Pin GPIO_PIN_5
#define LR_DIO1_GPIO_Port GPIOC
#define EN_R_Pin GPIO_PIN_0
#define EN_R_GPIO_Port GPIOB
#define MOT_STBY_Pin GPIO_PIN_2
#define MOT_STBY_GPIO_Port GPIOB
#define PH_R_Pin GPIO_PIN_12
#define PH_R_GPIO_Port GPIOB
#define PH_L_Pin GPIO_PIN_14
#define PH_L_GPIO_Port GPIOB
#define EN_L_Pin GPIO_PIN_7
#define EN_L_GPIO_Port GPIOC
#define SDIO_SW_Pin GPIO_PIN_8
#define SDIO_SW_GPIO_Port GPIOA
#define GPS_RX_Pin GPIO_PIN_9
#define GPS_RX_GPIO_Port GPIOA
#define GPS_TX_Pin GPIO_PIN_10
#define GPS_TX_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define LR_RESET_Pin GPIO_PIN_15
#define LR_RESET_GPIO_Port GPIOA
#define LR_SCK_Pin GPIO_PIN_3
#define LR_SCK_GPIO_Port GPIOB
#define LR_MISO_Pin GPIO_PIN_4
#define LR_MISO_GPIO_Port GPIOB
#define LR_MOSI_Pin GPIO_PIN_5
#define LR_MOSI_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB
#define BTN_1_Pin GPIO_PIN_8
#define BTN_1_GPIO_Port GPIOB
#define BTN_2_Pin GPIO_PIN_9
#define BTN_2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
