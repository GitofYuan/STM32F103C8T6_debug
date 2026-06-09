/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : device_control_can.h
* Author            : Yuan.Zong
* Version           : V1.0.0/2026.05.14
* Description       :
******************************************************************************/
/* ==============================  INCLUDES  =============================== */
#include <string.h>
#include <stdio.h>
#include "cmsis_os.h"

#include "can_protocol_handle.h"
#include "uart_bus.h"
#include "voyah_ble.h"
#include "voyah_rf.h"
/* ==============================  DEFINES   =============================== */

/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */

/* ==============================  EXTERNS   =============================== */
//static uint8_t send_flag = 0;
//static uint16_t R485_tx_flag = 0;    
//static uint8_t  R485_tx_buf[UART_DATA_MAX] = {0};           /*R485发送缓冲区，大小根据实际需求定义*/
//uint8_t chm_send[8] = {0};
//Can_Rcv_Data can_send_data;

static uint8_t key0_state = 0; /*按键状态，0表示未按下，1表示按下*/





/* ========================= FUNCTION PROTOTYPES =========================== */
/*****************************************************************************************************
*Function   :APL_Task(应用层任务)
*Description:负责即使运行维持应用层功能的正常运行，当前主要为CAN协议处理任务，后续可增加其他应用层任务。
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void APL_Task(void *argument)
{
    /* USER CODE BEGIN APL_Task */
    /* Infinite loop */
    for(;;)
    {
        osDelay(1);
        device_ctrl_content_u content;
        device_control(DEV_TYPE_GPIO, "key0", DEV_CTRL_READ, &content);
        if(content.gpio.level == GPIO_SET)
        {
            key0_state++;
            if(key0_state > 100)
            {
                rf_command_send();
                key0_state = 0;
            }
        }
        else
        {
            key0_state = 0;
        }
    }
    /* USER CODE END APL_Task */
}

void CAN_Task(void *argument)
{
  /* USER CODE BEGIN CAN_Task */
  /* Infinite loop */
    for(;;)
    {
        osDelay(1);
//        if(send_flag == 0)
//        {
//            send_flag = 1;
//            uint32_t prot_offset = OFFSET_OF(control_info_t, bms_can_control[BMS1_CAN][CHM].send_flag);
//            share_data_write(CONTROL_INFO, prot_offset, &send_flag, sizeof(send_flag));
//        }
        can_protocol_handle_task();
        J1939_handle_task();
        
  }
  /* USER CODE END CAN_Task */
}
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
