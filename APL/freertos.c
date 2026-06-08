/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ZSC_RFPOWER99          "ZSC+RFPOWER:99\r\n"
#define ZSC_RFPOWER12          "ZSC+RFPOWER:12\r\n"
#define ZSC_SWR                "ZSC+SWR\r\n"
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* ========================= FUNCTION PROTOTYPES =========================== */
/*功能实现接口函数*/

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for RF_TASK */
osThreadId_t RF_TASKHandle;
const osThreadAttr_t RF_TASK_attributes = {
  .name = "RF_TASK",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void RF_TASK01(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of RF_TASK */
  RF_TASKHandle = osThreadNew(RF_TASK01, NULL, &RF_TASK_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_RF_TASK01 */
/**
* @brief Function implementing the RF_TASK thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_RF_TASK01 */
void RF_TASK01(void *argument)
{
  /* USER CODE BEGIN RF_TASK01 */
  /* Infinite loop */
  for(;;)
  {
      osDelay(1);
      osDelay(499);
      HAL_GPIO_TogglePin(RUN_LED_GPIO_Port, RUN_LED_Pin);
      
      HAL_UART_Transmit(&huart1, (uint8_t *)ZSC_RFPOWER99, strlen((const char *)ZSC_RFPOWER99), 100);
      osDelay(499);
      HAL_UART_Transmit(&huart1, (uint8_t *)ZSC_RFPOWER12, strlen((const char *)ZSC_RFPOWER12), 100);
      osDelay(499);
      HAL_UART_Transmit(&huart1, (uint8_t *)ZSC_SWR, strlen((const char *)ZSC_SWR), 100);
      osDelay(499);
      if(send_flag == 0 )
      {
          rf_command_send();
          send_flag = 1;
      }
  }
  /* USER CODE END RF_TASK01 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

