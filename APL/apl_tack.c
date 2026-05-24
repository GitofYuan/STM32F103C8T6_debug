/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : device_control_can.h
* Author            : Yuan.Zong
* Version           : V1.0.0/2026.05.14
* Description       :
******************************************************************************/
/* ==============================  INCLUDES  =============================== */
#include "cmsis_os.h"

#include "can_protocol_handle.h"
#include "uart_bus.h"
/* ==============================  DEFINES   =============================== */

/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */

/* ==============================  EXTERNS   =============================== */
static uint16_t R485_tx_flag = 0;    
static uint8_t  R485_tx_buf[UART_DATA_MAX] = {0};           /*R485发送缓冲区，大小根据实际需求定义*/
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
        can_protocol_handle_task();
        if(R485_tx_flag >= 500)
        {
//            uart_data_s data;
//            data.len = sprintf((char*)R485_tx_buf, "R485 test message %d", R485_tx_flag);
//            data.data = R485_tx_buf;
//            uart_tx_enqueue(UART_DATA_QUEUE_CHNL_1, &data);
            printf("enter if, flag=%u\n", R485_tx_flag);
            printf("flag=%u\n", R485_tx_flag);
            R485_tx_flag = 0;
        }
        R485_tx_flag ++;
    }
    /* USER CODE END APL_Task */
}
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
