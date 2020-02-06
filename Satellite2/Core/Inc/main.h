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
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
I2C_HandleTypeDef* Get_I2C1_Instance();
SPI_HandleTypeDef* Get_SPI1_Instance();
RTC_HandleTypeDef* Get_RTC_Instance();
TIM_HandleTypeDef* Get_TIM2_Instance();

//todelete
TIM_HandleTypeDef* Get_TIM3_Instance();
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define P1_Pin GPIO_PIN_0
#define P1_GPIO_Port GPIOC
#define P4_Pin GPIO_PIN_1
#define P4_GPIO_Port GPIOC
#define P2_Pin GPIO_PIN_2
#define P2_GPIO_Port GPIOC
#define P5_Pin GPIO_PIN_3
#define P5_GPIO_Port GPIOC
#define PH_L_Pin GPIO_PIN_0
#define PH_L_GPIO_Port GPIOA
#define EN_L_Pin GPIO_PIN_1
#define EN_L_GPIO_Port GPIOA
#define PH_R_Pin GPIO_PIN_2
#define PH_R_GPIO_Port GPIOA
#define EN_R_Pin GPIO_PIN_3
#define EN_R_GPIO_Port GPIOA
#define P7_Pin GPIO_PIN_4
#define P7_GPIO_Port GPIOA
#define LR_DIO0_Pin GPIO_PIN_6
#define LR_DIO0_GPIO_Port GPIOA
#define LR_DIO0_EXTI_IRQn EXTI9_5_IRQn
#define LR_RESET_Pin GPIO_PIN_5
#define LR_RESET_GPIO_Port GPIOC
#define P6_Pin GPIO_PIN_0
#define P6_GPIO_Port GPIOB
#define P3_Pin GPIO_PIN_1
#define P3_GPIO_Port GPIOB
#define GPS_RX_Pin GPIO_PIN_10
#define GPS_RX_GPIO_Port GPIOB
#define GPS_TX_Pin GPIO_PIN_11
#define GPS_TX_GPIO_Port GPIOB
#define LEDD_Pin GPIO_PIN_12
#define LEDD_GPIO_Port GPIOB
#define LEDC_Pin GPIO_PIN_13
#define LEDC_GPIO_Port GPIOB
#define LEDB_Pin GPIO_PIN_6
#define LEDB_GPIO_Port GPIOC
#define LEDA_Pin GPIO_PIN_7
#define LEDA_GPIO_Port GPIOC
#define SDIO_SW_Pin GPIO_PIN_8
#define SDIO_SW_GPIO_Port GPIOA
#define BTN_USR_Pin GPIO_PIN_15
#define BTN_USR_GPIO_Port GPIOA
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
#define LR_NSS_Pin GPIO_PIN_9
#define LR_NSS_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
