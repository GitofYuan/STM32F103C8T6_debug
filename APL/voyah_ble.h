/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : voyah_ble.h
* Author            : Yuan.Zong
* Version           : V1.0.0
* Description       : 
******************************************************************************/
#ifndef __VOYAH_BLE_H__
#define __VOYAH_BLE_H__
/* ==============================  INCLUDES  =============================== */
#include "share_data.h"
#include "atk_ble03.h"
/* ==============================  DEFINES   =============================== */
#define  DEBUG_VOYAH_BLE           /*debug使能，完整保留“DEBUG_VOYAH_BLE”为使能debug*/

#define  VEHICLE_BLE_ADDR         0x01       /*车辆蓝牙协议地址*/
#define  CHAEGER_BLE_ADDR         0x02       /*充电机蓝牙协议地址*/
#define  BLE_FRAME_HEAD           0x68       /*蓝牙报文头*/
#define  AES_KEY_SIZE             16         /*AES加密密钥长度*/
#define  AES_BLOCK_SIZE           16         /*AES加密块大小*/
#define  FIXED_KEY_LEN            12         /*密钥固定值长度*/
#define  FIXED_KEY           "LANTU1234567"  /*密钥固定值，长度为12字节*/
#define  BT_MAX_PLAIN_LEN         32         /*最大明文数据长度*/
#define  BT_MAX_ENC_LEN           32         /*最大加密数据长度*/
/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */
/* BLE报文信息属性*/
typedef struct
{
   uint8_t   cmd_num;         /*报文命令代号*/
   uint8_t   size;            /*数据长度*/
   uint16_t  send_cycle;      /*报文默认发送周期*/
   uint16_t  send_time;       /*报文默认发送次数*/
   uint16_t  send_cycle2;    /*报文临时发送周期，一般用于特定场景要求周期变化*/
   uint16_t  send_time2;     /*总发送次数，一般用于报文发送达到时间后停止*/
}ble_frame_attribute_t;

/*BLE报文发送定时器结构体*/
typedef struct
{
    uint16_t  send_timer;        /*默认周期发送定时器*/
    uint16_t  send_count;        /*默认发送计数器*/
    uint16_t  send_timer2;       /*临时周期发送定时器*/
    uint16_t  send_count2;       /*临时发送计数器*/
}ble_frame_send_timer_t;

/*BLE报文收发数据结构体（未加密）*/
typedef struct
{
    uint8_t   plain_len;         /*有效数据长度*/
    uint8_t   data[BT_MAX_PLAIN_LEN];  /*铭文数据（未加密）*/
}ble_plain_t;

/*BLE报文收发数据结构体（已加密）*/
typedef struct
{
    uint8_t   frame_head;           /*报文命令代号*/
    uint8_t   effective_len;        /*有效数据长度*/
    uint8_t   encrypt_len;          /*加密数据长度（含填充位）*/
    uint8_t   encrypt_data[BT_MAX_ENC_LEN];      /*加密数据*/
    uint16_t  check_num;         /*校验数值*/
}ble_frame_data_t;
/* ==============================  EXTERNS   =============================== */


/* ========================= FUNCTION PROTOTYPES =========================== */
/*****************************************************************************************************
*Function   :ble_protocol_handle_task（BLE协议处理任务）
*Description:该函数负责从BLE接收队列中接收BLE报文数据并对其进行解析处理，同时负责轮询BLE报文控制属性表，
             按照通讯协议要求对BLE报文进行数据整理并发送。
*Input      :
*Output     :
*Returns    :
*Note       :须放在任务函数中，内部包含计数器，对任务周期要求为1ms
*****************************************************************************************************/
void ble_protocol_handle_task(void);

#endif
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
