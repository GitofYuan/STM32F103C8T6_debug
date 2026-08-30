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

uint8_t uart1_rx_buf[UART_DATA_MAX] = {0};           /*UART1接收缓冲区，大小根据实际需求定义*/

/* ========================= FUNCTION PROTOTYPES =========================== */

// ====================== UART适配实现 ======================
/*****************************************************************************************************
*Function   :uart_stm32_init（UART初始化函数）
*Description:根据传入的设备信息和配置内容，初始化UART外设，包括波特率、数据位、停止位、校验位和收发模式等参数的配置。
*Input      :DeviceRuntimeInfo     *dev       设备运行时信息结构体指针，包含UART句柄和实例等信息。
             device_ctrl_content_u *content   配置内容联合体指针，包含UART的波特率、数据位、停止位、校验位和收发模式等参数。
*Output     :
*Returns    :bool    false/true    初始化失败/成功
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
    
    /* 根据模式选择是否启动接收（或其它预留操作） */
    switch(content->uart.mode)
    {
       case UART_MODE_IT_RX:
       case UART_MODE_IT_RX_TX:
           HAL_UART_Receive_IT(huart, uart1_rx_buf, UART_DATA_MAX);
           __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
           break;
       case UART_MODE_DMA_RX:
       case UART_MODE_DMA_RX_TX:
           if (HAL_UART_Receive_DMA(huart, uart1_rx_buf, UART_DATA_MAX) != HAL_OK)
           {
               return false;
           }
           __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
           break;
       case UART_MODE_POLLING:
       case UART_MODE_IT_TX:
       case UART_MODE_DMA_TX:
       default:
           /* 其他模式在初始化时无需额外动作 */
           break;
    }

    dev->is_opened = true; // UART初始化后需手动打开
    return true;
}

/*****************************************************************************************************
*Function   :uart_stm32_control（UART控制接口）
*Description:根据传入的设备信息、控制类型和配置内容，执行相应的UART操作，包括打开、关闭、读取、写入和配置等。
*Input      :DeviceRuntimeInfo     *dev       设备运行时信息结构体指针，包含UART句柄和实例等信息。
             device_ctrl_type_e    ctrl_type  控制类型枚举值，指定要执行的操作类型（如打开、关闭、读取、写入、配置等）。
             device_ctrl_content_u *content   配置内容联合体指针，包含UART的波特率、数据位、停止位、校验位和收发模式等参数。
*Output     :
*Returns    :bool    false/true    操作失败/成功
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
                        if (HAL_UART_Transmit_DMA(huart, content->uart.buf, content->uart.len) != HAL_OK)
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
            注：每次使用cube MX重建工程时，都会将stm32f1xx_it.c中原函数的__weak抹除，编译报错时注意完善。
*****************************************************************************************************/
void USART1_IRQHandler(void)
{
    if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET)
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart1); /* 正确清除IDLE中断标志 */
        HAL_UART_DMAStop(&huart1); /* 停止DMA接收 */
        uint16_t huart1_rx_len = UART_DATA_MAX - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx); /* 计算接收到的数据长度 */
        
        if(huart1_rx_len > 0 && huart1_rx_len <= UART_DATA_MAX)
        {
            uart_data_s huart1_rx_data;
            huart1_rx_data.len = (uint8_t)huart1_rx_len;  /*用于计算有效数据长度*/
            while (huart1_rx_data.len > 0 && uart1_rx_buf[huart1_rx_data.len - 1] == 0)
            {
                huart1_rx_data.len--;
            }
            memcpy(huart1_rx_data.data, uart1_rx_buf, UART_DATA_MAX);

            uart_rx_enqueue(UART_DATA_QUEUE_CHNL_1, &huart1_rx_data);
            memset(uart1_rx_buf, 0, UART_DATA_MAX);
        }

        HAL_UART_Receive_DMA(&huart1, uart1_rx_buf, UART_DATA_MAX); /* 重新启动DMA接收 */
    }
    HAL_UART_IRQHandler(&huart1);
}

/*****************************************************************************************************
*Function   :HAL_UART_TxCpltCallback（UART发送完成回调函数）
*Description:该函数是HAL库提供的UART发送完成回调函数，当UART发送完成时会被调用。
             在这里，我们将其转发到uart_bus模块的回调处理函数uart_dma_tx_complete，以便在UART发送完成后进行队列管理和下一次发送的启动。
*Input      :UART_HandleTypeDef *huart  UART句柄
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    /* 转发到 uart_bus 的回调处理（最小工作量，uart_bus 会在 IRQ 上做原子出队操作） */
    uart_dma_tx_complete(huart);
}

/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
