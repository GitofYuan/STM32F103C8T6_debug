/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : can_protocol_handle.h
* Author            : Yuan.Zong
* Version           : V1.0.0
* Description       : can协议处理，含协议解析、收发。
******************************************************************************/
#ifndef __CAN_PROTOCOL_HANDLE_H__
#define __CAN_PROTOCOL_HANDLE_H__
/* ==============================  INCLUDES  =============================== */
#include "share_data.h"
#include "J1939.h"

/* ==============================  DEFINES   =============================== */
#define BMS1_CAN            CAN_DATA_QUEUE_CHNL_1       /*定义CAN1通道为BMS CAN通道*/
#define BMS2_CAN            CAN_DATA_QUEUE_CHNL_2       /*定义CAN2通道为BMS CAN通道*/
#define INTER_BOAED_CAN     CAN_DATA_QUEUE_CHNL_3       /*定义CAN3通道为板间CAN通道*/

#define BMS_CAN_ADDR        0xF4                       /*BMS CAN地址*/
#define CHARGER_CAN_ADDR    0x56                       /*充电桩 CAN地址*/
/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */
/* CAN报文信息属性*/
typedef struct
{
   uint8_t   pgn;             /*报文PGN*/
   uint8_t   priority;        /*报文优先级*/
   uint8_t   size;            /*数据长度*/
   uint16_t  send_cycle;      /*报文默认发送周期*/
   uint16_t  send_time;       /*报文默认发送次数*/
   uint16_t  send_cycle2;    /*报文临时发送周期，一般用于特定场景要求周期变化*/
   uint16_t  send_time2;     /*总发送次数，一般用于报文发送达到时间后停止*/
}Can_Frame_Attribute;

/*can帧发送定时器结构体*/
typedef struct
{
    uint16_t  send_timer;        /*默认周期发送定时器*/
    uint16_t  send_count;        /*默认发送计数器*/
    uint16_t  send_timer2;       /*临时周期发送定时器*/
    uint16_t  send_count2;       /*临时发送计数器*/
}frame_send_timer_t;
/* ==============================  EXTERNS   =============================== */


/* ========================= FUNCTION PROTOTYPES =========================== */
/*****************************************************************************************************
*Function   :can_protocol_handle_task（CAN协议处理任务）
*Description:该函数负责从J1939接收队列中接收CAN报文数据并对其进行解析处理，同时负责轮询CAN报文控制属性表，
             按照通讯协议要求对CAN报文进行数据整理并发送。
*Input      :
*Output     :
*Returns    :
*Note       :须放在任务函数中，内部包含计数器，对任务周期要求为1ms
*****************************************************************************************************/
void can_protocol_handle_task(void);


#endif
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
