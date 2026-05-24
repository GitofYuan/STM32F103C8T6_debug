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
//            uart_data_s data = {0};
//            data.len = snprintf((char*)R485_tx_buf, UART_DATA_MAX, "R485 test message %d", R485_tx_flag);
//            memcpy(data.data, R485_tx_buf, data.len);
//            data.timeout = data.len+1;
//            uart_tx_enqueue(UART_DATA_QUEUE_CHNL_1, &data);
//            
//            printf("UART send failure,init again!\n");
            R485_tx_flag = 0;
        }
        R485_tx_flag ++;

        uart_data_s uart_rx_data = {0};
        uart_rx_dequeue(UART_DATA_QUEUE_CHNL_1, &uart_rx_data);
        if(uart_rx_data.len > 0)
        {
            printf("Received UART data: %.*s\n", uart_rx_data.len, uart_rx_data.data);
        }
    }
    /* USER CODE END APL_Task */
}
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
