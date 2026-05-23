/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f1xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Address1_Pin GPIO_PIN_5
#define Address1_GPIO_Port GPIOA
#define Address2_Pin GPIO_PIN_6
#define Address2_GPIO_Port GPIOA
#define Address3_Pin GPIO_PIN_7
#define Address3_GPIO_Port GPIOA
#define Address4_Pin GPIO_PIN_0
#define Address4_GPIO_Port GPIOB
#define Run_LED_Pin GPIO_PIN_1
#define Run_LED_GPIO_Port GPIOB
#define ISO_AC_Ctrl_Pin GPIO_PIN_2
#define ISO_AC_Ctrl_GPIO_Port GPIOB
#define ISO_Power_Ctrl_Pin GPIO_PIN_10
#define ISO_Power_Ctrl_GPIO_Port GPIOB
#define ISO_Select_Pin GPIO_PIN_12
#define ISO_Select_GPIO_Port GPIOB
#define ISO_A2_Ctrl_Pin GPIO_PIN_13
#define ISO_A2_Ctrl_GPIO_Port GPIOB
#define ISO_A1_Ctrl_Pin GPIO_PIN_14
#define ISO_A1_Ctrl_GPIO_Port GPIOB
#define ISO_A0_Ctrl_Pin GPIO_PIN_15
#define ISO_A0_Ctrl_GPIO_Port GPIOB
#define Relay_Ctrl1_Pin GPIO_PIN_3
#define Relay_Ctrl1_GPIO_Port GPIOB
#define Relay_Ctrl2_Pin GPIO_PIN_4
#define Relay_Ctrl2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
