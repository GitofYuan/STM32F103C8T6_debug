/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : can_bus.h
* Author            : Yuan.Zong
* Version           : V1.0.0
* Description       : 这是一个CAN收发数据队列功能模块，也可称为CAN_BUS，核心是通过先入先出队列实现的；
                      该功能模块最多支持3个CAN通道数据处理，且每个CAN通道的队列数可通过宏定义进行调整；
                      该功能模块可实现CAN数据的接收和发送，具体接口函数的使用说明，见函数的功能描述；
                      需要注意的是，该功能模块的建议应用层级为介于CAN驱动层之后以及协议栈层之前。
                      该功能模块预计消耗的栈空间为：***K。
                      强烈建议将该功能模块的发送出队函数放在1ms周期的任务中，为该模块独立创建一个任务则效果更佳。
******************************************************************************/
#ifndef _CAN_BUS_H_
#define _CAN_BUS_H_
/* ==============================  INCLUDES  =============================== */
#include <stdbool.h>
#include "can.h"
/* ==============================  DEFINES   =============================== */
#define SEND_INTERVAL           2     /*两帧之间发送的最小间隔 ms*/
#define CAN_DATA_QUEUE_NUM      10    /*CAN数据数据队列数量*/
/*如果决定在CAN_BUS中对非自身地址和广播地址进行过滤，则要想好自身地址通过尽可能简单的方式传递进来*/
#define CAN_CHNL_1_ADDR
#define CAN_CHNL_2_ADDR
#define CAN_CHNL_3_ADDR

#define DEBUG_CAN_BUS            /*debug使能，完整保留“DEBUG_CAN_BUS”为使能debug*/
/* ==============================   ENUMS    =============================== */
/*CAN通道枚举，如果实际用不到那么多CAN通道，建议将用不到的CAN通道注掉以节省资源*/
typedef enum
{
    CAN_DATA_QUEUE_CHNL_1 = 0,
//    CAN_DATA_QUEUE_CHNL_2,
//    CAN_DATA_QUEUE_CHNL_3,
    CAN_DATA_CHNL_NUM_MAX,
}CAN_CHNL;

/* ======================== STRUCTURES AND UNIONS ========================== */
/* CAN数据接收结构体*/
typedef struct
{
    uint32_t    id;          /*CAN ID*/
    uint8_t     len;         /*数据长度*/
    uint8_t     data[8];     /*数据内容*/
}Can_Rcv_Data;

/* ==============================  EXTERNS   =============================== */

/* ========================= FUNCTION PROTOTYPES =========================== */
/*功能实现接口函数*/
/*****************************************************************************************************
*Function   :can_rx_enqueue（CAN接收入队函数）
*Description:这是一个CAN数据接收的入队函数，通常需要在驱动层中断接收后调用此函数将接收到的数据入队。
*Input      :CAN_CHNL       chnl   CAN通道
             Can_Rcv_Data*  data   CAN接收数据结构体
*Output     :
*Returns    :bool值         入队结果（true成功/false失败）
*Note       :
*****************************************************************************************************/
bool can_rx_enqueue(CAN_CHNL chnl, Can_Rcv_Data* data);

/*****************************************************************************************************
*Function   :can_rx_dequeue（CAN接收出队函数）
*Description:这是一个CAN数据接收的出队函数，通常需要在后级的协议栈层级或者直接在数据解析层级调用将数据取出处理。
*Input      :CAN_CHNL       chnl   CAN通道
*Output     :Can_Rcv_Data*  data   CAN接收数据结构体
*Returns    :bool值         出队结果（true成功/false失败）
*Note       :
*****************************************************************************************************/
bool can_rx_dequeue(CAN_CHNL chnl, Can_Rcv_Data* data);

/*****************************************************************************************************
*Function   :can_tx_enqueue（CAN发送入队函数）
*Description:这是一个CAN数据发送的入队函数，通常在后级的协议栈层级或者直接在数据发送等级调用将处理好的数据入队等待发送。
*Input      :CAN_CHNL       chnl   CAN通道
             Can_Rcv_Data*  data   CAN接收数据结构体
*Output     :
*Returns    :bool值         入队结果（true成功/false失败）
*Note       :
*****************************************************************************************************/
bool can_tx_enqueue(CAN_CHNL chnl, Can_Rcv_Data* data);

/*外侧依赖接口函数*/
/*****************************************************************************************************
*Function   :can_tx_dequeue（CAN发送出队函数）
*Description:这是一个自动的CAN数据发送的出队函数，通常应该将其放在一个1ms周期的任务中，使其及时将队列的数据发送出去。
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void can_tx_dequeue(void);

#endif
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
