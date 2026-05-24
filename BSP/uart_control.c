/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : uart_control.c
* Author            : Yuan.Zong
* Version           : V1.0.0/2026.02.27
* Description       :
******************************************************************************/
/* ==============================  INCLUDES  =============================== */
#include <stdio.h>
#include <string.h>
#include "usart.h"
#include "stm32f1xx.h"

#include "device_control.h"
#include "uart_bus.h"
/* ==============================  DEFINES   =============================== */


/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */

/* ==============================  EXTERNS   =============================== */
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

static uint8_t uart1_rx_buf[UART_DATA_MAX] = {0};           /*UART1接收缓冲区，大小根据实际需求定义*/

/* ========================= FUNCTION PROTOTYPES =========================== */

// ====================== UART适配实现 ======================
/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :本初始化函数是不会操作串口DMA和中断的初始化的，这个在系统底层搭建的时候配置好就行了。
*****************************************************************************************************/
static bool uart_stm32_init(DeviceRuntimeInfo *dev, device_ctrl_content_u *content) 
{
    // 初始化UART（默认115200-8-1-无校验，可后续通过control修改）
    UART_HandleTypeDef *huart = dev->hw_res.uart.huart;

    /* UART初始化参数配置 */
    huart->Instance = dev->hw_res.uart.instance;       /* 串口实例 */
    huart->Init.BaudRate = content->uart.baud_rate;    /* 波特率 */

    switch(content->uart.data_bits)                    /* 数据位 */
    {
        case DATA_BITS_8:
            huart->Init.WordLength = UART_WORDLENGTH_8B;
            break;
        case DATA_BITS_9:
            huart->Init.WordLength = UART_WORDLENGTH_9B;
            break;
        default:
            return false;
    }

    switch(content->uart.stop_bits)                     /* 停止位 */
    {
        case STOP_BITS_1:
            huart->Init.StopBits = UART_STOPBITS_1;
            break;
        case STOP_BITS_2:
            huart->Init.StopBits = UART_STOPBITS_2;
            break;
        default:
            return false;
    }
    switch(content->uart.parity)                        /* 校验位 */
    {
        case PARITY_NONE:
            huart->Init.Parity = UART_PARITY_NONE;
            break;
        case PARITY_EVEN:
            huart->Init.Parity = UART_PARITY_EVEN;
            break;
        case PARITY_ODD:
            huart->Init.Parity = UART_PARITY_ODD;
            break;
        default:
            return false;
    }
    switch(content->uart.rt_mode)                       /* 收发模式 */
    {
        case UART_TX_RX:
            huart->Init.Mode = UART_MODE_TX_RX;
            dev->hw_res.uart.rt_mode = UART_TX_RX;
            break;
        case UART_RX:
            huart->Init.Mode = UART_MODE_RX;
            dev->hw_res.uart.rt_mode = UART_RX;
            break;
        case UART_TX:
            huart->Init.Mode = UART_MODE_TX;
            dev->hw_res.uart.rt_mode = UART_TX;
            break;
        default:
            return false;
    }
    huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart->Init.OverSampling = UART_OVERSAMPLING_16;
    
    if (HAL_UART_Init(huart) != HAL_OK) 
    {
       return false;
    }
    
    /*预留*/
    switch(content->uart.mode)
    {
       case UART_MODE_IT_RX:
       case UART_MODE_IT_RX_TX:
           HAL_UART_Receive_IT(huart, uart1_rx_buf, UART_DATA_MAX);
           __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE); 
           break;
       case UART_MODE_DMA_RX:
       case UART_MODE_DMA_RX_TX:
           HAL_UART_Receive_DMA(huart, uart1_rx_buf, UART_DATA_MAX);
           __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
           break;
       default:
           return false;
    }

    // 初始化设备状态
    dev->is_opened = true; // UART初始化后需手动打开
    return true;
}

/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
bool uart_stm32_control(DeviceRuntimeInfo *dev, device_ctrl_type_e ctrl_type, device_ctrl_content_u *content) 
{
    if (dev == NULL || content == NULL) 
    {
       return false;
    }

    UART_HandleTypeDef *huart = dev->hw_res.uart.huart;
    switch (ctrl_type) 
    {
        case DEV_CTRL_OPEN:
           uart_stm32_init(dev, content);
           break;
        case DEV_CTRL_CLOSE:
           HAL_UART_DeInit(huart);
           dev->is_opened = false;
           break;
        case DEV_CTRL_READ:
            if(dev->hw_res.uart.rt_mode == UART_TX)
            {
                return false;
            }
            else
            {
                switch(dev->hw_res.uart.mode)
                {
                    case UART_MODE_POLLING:
                        if (HAL_UART_Receive(huart, content->uart.buf, content->uart.len, content->uart.timeout_ms) != HAL_OK)
                        {
                            return false;
                        }
                        break;
                    case UART_MODE_IT_RX:
                    case UART_MODE_IT_RX_TX:
                        if (HAL_UART_Receive_IT(huart, content->uart.buf, content->uart.len) != HAL_OK)
                        {
                            return false;
                        }
                        break;
                    case UART_MODE_DMA_RX:
                    case UART_MODE_DMA_RX_TX:
                        if (HAL_UART_Receive_DMA(huart, content->uart.buf, content->uart.len) != HAL_OK)
                        {
                            return false;
                        }
                        break;
                    default:
                        return false;
                }
            }
            break;
        case DEV_CTRL_WRITE:
            if(dev->hw_res.uart.rt_mode == UART_RX)
            {
                return false;
            }
            else
            {
                switch(dev->hw_res.uart.mode)
                {
                    case UART_MODE_POLLING:
                        if (HAL_UART_Transmit(huart, content->uart.buf, content->uart.len, content->uart.timeout_ms) != HAL_OK)
                        {
                            return false;
                        }
                        break;
                    case UART_MODE_IT_TX:
                    case UART_MODE_IT_RX_TX:
                        if (HAL_UART_Transmit_IT(huart, content->uart.buf, content->uart.len) != HAL_OK)
                        {
                            return false;
                        }
                        break;
                    case UART_MODE_DMA_TX:
                    case UART_MODE_DMA_RX_TX:
                        /* 当前UART发送队列实现为同步出队后立即释放缓冲区，
                           所以这里不使用DMA异步发送，改为同步阻塞发送以避免
                           DMA传输过程中发送缓冲区被覆盖。 */
                        if (HAL_UART_Transmit(huart, content->uart.buf, content->uart.len, content->uart.timeout_ms) != HAL_OK)
                        {
                            return false;
                        }
                        break;
                    default:
                        return false;
                }
            }
            break;
        case DEV_CTRL_CONFIG:
            HAL_UART_DeInit(huart);
            uart_stm32_init(dev, content);
            break;
        case DEV_CTRL_GET_STATUS:
            content->is_open = dev->is_opened;
            break;
        default:
            return false;
    }
    return true;
}

/*****************************************************************************************************
*Function   :USART1_IRQHandler（UART1中断接收处理函数）
*Description:原函数定义在stm32f1xx_it.c中，使用__weak将其修饰为弱函数，这里重定义覆盖它以实现UART1的空闲中断接收处理。
*Input      :
*Output     :
*Returns    :
*Note       :原本使用__weak void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)是不对的，
             因为这个回调函数是DMA接收完成的回调，而我们需要的是空闲中断的处理，
             所以直接重定义USART1_IRQHandler来处理空闲中断更合适。
*****************************************************************************************************/
void USART1_IRQHandler(void)
{
    if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET)
    {
        __HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_IDLE); /* 清除空闲中断标志 */
        HAL_UART_DMAStop(&huart1); /* 停止DMA接收 */
        uint8_t huart1_rx_len = UART_DATA_MAX - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx); /* 计算接收到的数据长度 */
        
        if(huart1_rx_len > 0 && huart1_rx_len <= UART_DATA_MAX)
        {
            uart_data_s huart1_rx_data;
            huart1_rx_data.len = huart1_rx_len;  /*用于计算有效数据长度*/
            while(uart1_rx_buf[huart1_rx_data.len-1] == 0)
            {
                huart1_rx_data.len--;
            }
            memcpy(huart1_rx_data.data, uart1_rx_buf, huart1_rx_data.len);

            uart_rx_enqueue(UART_DATA_QUEUE_CHNL_1, &huart1_rx_data);
        }

        HAL_UART_Receive_DMA(&huart1, uart1_rx_buf, UART_DATA_MAX); /* 重新启动DMA接收 */
    }
    HAL_UART_IRQHandler(&huart1);
}

/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
