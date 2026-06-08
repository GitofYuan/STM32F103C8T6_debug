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
/* USER CODE BEGIN Variables */
uint8_t long_preamble[25] = {0};
const uint8_t long_frame_preamble[] = {0x00, 0x00};
const uint8_t long_frame_start_bit_id[] = {0xA5, 0x5A};
uint8_t long_frame_data[] = {0x81, 0x00, 0x00, 0x00, 0x00, 0x10};
uint8_t long_frame_gun_maker[] = {0x00, 0x01, 0x00, 0x00};
const uint8_t long_frame_reserved[] = {0x00, 0x00, 0x00, 0x00};

//const uint8_t button_release[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA5, 0x5A, 0x82, 0x00, 0x00, 0x00, 0x00, 0x10, 0xCE};
/**/
typedef struct
{
    uint8_t preamble[2];
    uint8_t start_bit_id[2];
    uint8_t data[6];
    uint8_t gun_maker[4];
    uint8_t reserved[4];
    uint8_t crc;
}long_frame_t;

const uint8_t button_release_preamble[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8_t button_release_start_bit_id[] = {0xA5, 0x5A};
uint8_t button_release_data[] = {0x82, 0x00, 0x00, 0x00, 0x00, 0x10};

typedef struct
{
    uint8_t preamble[8];
    uint8_t start_bit_id[2];
    uint8_t data[6];
    uint8_t crc;
}button_release_t;


long_frame_t       long_frame;
button_release_t   button_release;

uint8_t rf_tx_buffer[50];

uint8_t send_flag = 0;
/* ========================= FUNCTION PROTOTYPES =========================== */
/*功能实现接口函数*/
/*****************************************************************************************************
*Function   :
*Description:CRC8计算函数，计算公式：x^8+x^2+1，多项式0x07，初始值0x00，输入输出不反转，结果亦或值0x00
*Input      :uint8_t *data   待计算的数据指针
             uint8_t len     待计算的数据长度
*Output     :
*Returns    :uint8_t crc     校验值
*Note       :
*****************************************************************************************************/
uint8_t crc8_calculate(uint8_t *data, uint8_t len)
{
    const uint8_t crc8_poly = 0x07;
    uint8_t crc = 0x00;
    
    if(data == NULL || len == 0)
    {
        return crc;
    }
    
    for(uint8_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for(uint8_t j = 0; j < 8; j++)
        {
            if(crc & 0x80)
            {
                crc = (crc << 1) ^ crc8_poly;
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}
                  
/*****************************************************************************************************
*Function   :
*Description:计算曼彻斯特编码后的输出字节长度
*Input      :uint8_t input_len   输入16进制数组的字节长�?
*Output     :
*Returns    :编码后输出数组的字节长度
*Note       :
*****************************************************************************************************/
uint8_t manchester_get_encoded_len(uint8_t input_len)
{
    
    return input_len*2;
}

/*****************************************************************************************************
*Function   :
*Description:曼彻斯特编码函数，将输入的uint8_t数组，进行曼彻斯特编码，然后再输出出�?
*Input      :uint8_t *input_data 待处理的数组指针
             uint8_t input_len   待处理的数组字节长度
*Output     :uint8_t *output_data  编码后的数组指针
*Returns    :
*Note       :
*****************************************************************************************************/
void manchester_encoded_hex(uint8_t *input_data, uint8_t input_len, uint8_t *output_data)
{
    if(input_data == NULL || output_data == NULL || input_len <= 0)
    {
        return;
    }
    
    uint32_t bit_idx = 0;
    memset(output_data, 0, 50);
    
    for(int byte_idx = 0; byte_idx < input_len; byte_idx++)
    {
        uint8_t current_byte = input_data[byte_idx];
        
        for(int bit_pos = 7; bit_pos >= 0; bit_pos--)
        {
            uint8_t current_bit = (current_byte >> bit_pos) & 0x01;
            
            uint8_t level1,level2;
            if(current_bit == 1)
            {
                level1 = 0;
                level2 = 1;
            }
            else
            {
                level1 = 1;
                level2 = 0;
            }
            
            uint8_t byte_pos1 = bit_idx / 8;
            uint8_t bit_pos1 = 7 - (bit_idx % 8);
            if(level1 == 1)
            {
                output_data[byte_pos1] |= (1 << bit_pos1);
            }
            bit_idx++;
            
            int byte_pos2 = bit_idx / 8;
            int bit_pos2 = 7 - (bit_idx % 8);
            if(level2 == 1)
            {
                output_data[byte_pos2] |= (1 << bit_pos2);
            }
            bit_idx++;
        }
    }
    return;
}

/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void rf_command_send(void)
{
    memcpy(long_frame.preamble, long_frame_preamble, sizeof(long_frame.preamble));
    memcpy(long_frame.start_bit_id, long_frame_start_bit_id, sizeof(long_frame.start_bit_id));
    memcpy(long_frame.data, long_frame_data, sizeof(long_frame.data));
    memcpy(long_frame.gun_maker, long_frame_gun_maker, sizeof(long_frame.gun_maker));
    memcpy(long_frame.reserved, long_frame_reserved, sizeof(long_frame.reserved));
    long_frame.crc = crc8_calculate((uint8_t *)&long_frame.data, (sizeof(long_frame.data)+sizeof(long_frame.gun_maker)+sizeof(long_frame.reserved)));
    
    memcpy(button_release.preamble, button_release_preamble, sizeof(button_release.preamble));
    memcpy(button_release.start_bit_id, button_release_start_bit_id, sizeof(button_release.start_bit_id));
    memcpy(button_release.data, button_release_data, sizeof(button_release.data));
    button_release.crc = crc8_calculate((uint8_t *)&button_release.data, sizeof(button_release.data));
    
    uint8_t long_preamble_m_len = 0;
    uint8_t long_frame_m_len = 0;
    uint8_t button_release_m_len = 0;
    
    long_preamble_m_len = manchester_get_encoded_len(sizeof(long_preamble));
    manchester_encoded_hex(long_preamble, sizeof(long_preamble), rf_tx_buffer);
    HAL_UART_Transmit_DMA(&huart1, rf_tx_buffer, long_preamble_m_len);
    
    long_frame_m_len = manchester_get_encoded_len(sizeof(long_frame));
    manchester_encoded_hex((uint8_t *)&long_frame, sizeof(long_frame), rf_tx_buffer);
    HAL_UART_Transmit_DMA(&huart1, rf_tx_buffer, long_frame_m_len);

    osDelay(200+26+19+200);
    long_preamble_m_len = manchester_get_encoded_len(sizeof(long_preamble));
    manchester_encoded_hex(long_preamble, sizeof(long_preamble), rf_tx_buffer);
    HAL_UART_Transmit_DMA(&huart1, rf_tx_buffer, long_preamble_m_len);
    
    long_frame_m_len = manchester_get_encoded_len(sizeof(long_frame));
    manchester_encoded_hex((uint8_t *)&long_frame, sizeof(long_frame), rf_tx_buffer);
    HAL_UART_Transmit_DMA(&huart1, rf_tx_buffer, long_frame_m_len);

//    osDelay(200+26+19+200);
//    button_release_m_len = manchester_get_encoded_len(sizeof(button_release));
//    manchester_encoded_hex((uint8_t *)&button_release, sizeof(button_release), rf_tx_buffer);
//    HAL_UART_Transmit_DMA(&huart1, rf_tx_buffer, button_release_m_len);
    
}
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

