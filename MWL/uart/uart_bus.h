/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : uart_bus.h
* Author            : Yuan.Zong
* Version           : V1.0.0
* Description       : 这是一个UART收发数据队列功能模块，也可称为UART_BUS，核心是通过先入先出队列实现的；
                      该功能模块最多支持3个UART通道数据处理，且每个UART通道的队列数可通过宏定义进行调整；
                      该功能模块可实现UART数据的接收和发送，具体接口函数的使用说明，见函数的功能描述；
                      需要注意的是，该功能模块的建议应用层级为介于UART驱动层之后以及协议栈层之前。
                      该功能模块预计消耗的栈空间为：***K。
                      强烈建议将该功能模块的发送出队函数放在1ms周期的任务中，为该模块独立创建一个任务则效果更佳。
******************************************************************************/
#ifndef _UART_BUS_H_
#define _UART_BUS_H_
/* ==============================  INCLUDES  =============================== */
#include <stdbool.h>
#include "device_control.h"
/* ==============================  DEFINES   =============================== */
#define SEND_INTERVAL           10     /*两帧之间发送的最小间隔 ms*/
#define UART_DATA_QUEUE_NUM     10     /*UART数据数据队列数量*/
#define UART_DATA_MAX           50    /*UART单次最大可收发字节数*/

#define disDEBUG_UART_BUS            /*debug使能，完整保留“DEBUG_UART_BUS”为使能debug*/
/* ==============================   ENUMS    =============================== */
/*UART通道枚举，如果实际用不到那么多UART通道，建议将用不到的UART通道注掉以节省资源*/
typedef enum
{
    UART_DATA_QUEUE_CHNL_1 = 0,
    UART_DATA_QUEUE_CHNL_2,
//    UART_DATA_QUEUE_CHNL_3,
    UART_DATA_CHNL_NUM_MAX,
}uart_chnl_e;

/* ======================== STRUCTURES AND UNIONS ========================== */
/* UART数据接收结构体*/
typedef struct
{
    uint8_t     len;                          /*数据长度*/
    uint8_t     *data;                        /*数据内容*/
    uint8_t     timeout;                      /*超时时间*/
}uart_data_s;

/* ==============================  EXTERNS   =============================== */

/* ========================= FUNCTION PROTOTYPES =========================== */
/*功能实现接口函数*/
/*****************************************************************************************************
*Function   :uart_rx_enqueue（UART接收入队函数）
*Description:这是一个UART数据接收的入队函数，通常需要在驱动层中断接收后调用此函数将接收到的数据入队。
*Input      :uart_chnl_e       chnl   UART通道
             uart_data_s*      data   UART接收数据结构体
*Output     :
*Returns    :bool值         入队结果（true成功/false失败）
*Note       :
*****************************************************************************************************/
bool uart_rx_enqueue(uart_chnl_e chnl, uart_data_s* data);

/*****************************************************************************************************
*Function   :can_rx_dequeue（UART接收出队函数）
*Description:这是一个UART数据接收的出队函数，通常需要在后级的协议栈层级或者直接在数据解析层级调用将数据取出处理。
*Input      :uart_chnl_e       chnl   UART通道
*Output     :uart_data_s*      data   UART接收数据结构体
*Returns    :bool值         出队结果（true成功/false失败）
*Note       :
*****************************************************************************************************/
bool uart_rx_dequeue(uart_chnl_e chnl, uart_data_s* data);

/*****************************************************************************************************
*Function   :uart_tx_enqueue（UART发送入队函数）
*Description:这是一个UART数据发送的入队函数，通常在后级的协议栈层级或者直接在数据发送等级调用将处理好的数据入队等待发送。
*Input      :uart_chnl_e       chnl   UART通道
             uart_data_s*      data   UART接收数据结构体
*Output     :
*Returns    :bool值         入队结果（true成功/false失败）
*Note       :
*****************************************************************************************************/
bool uart_tx_enqueue(uart_chnl_e chnl, uart_data_s* data);

/*外侧依赖接口函数*/
/*****************************************************************************************************
*Function   :uart_tx_dequeue（UART发送出队函数）
*Description:这是一个自动的UART数据发送的出队函数，通常应该将其放在一个1ms周期的任务中，使其及时将队列的数据发送出去。
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void uart_tx_dequeue(void);

#endif
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
