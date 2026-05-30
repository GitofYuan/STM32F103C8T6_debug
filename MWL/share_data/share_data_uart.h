/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : share_data_uart.h
* Author            : Yuan.Zong
* Version           : V1.0.0
* Description       : 
******************************************************************************/
#ifndef __SHARE_DATA_UART_H__
#define __SHARE_DATA_UART_H__
/* ==============================  INCLUDES  =============================== */
#include "share_data.h"
/* ==============================  DEFINES   =============================== */

/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */
#pragma pack(1)          /*设置1字节对齐*/

/* CAN报文控制属性*/
typedef struct
{
    uint8_t   receive_flag;  /*接收标志位*/
    uint8_t   send_flag;     /*默认发送标志位*/
    uint8_t   send_flag2;    /*临时发送标志位，使能的前提是默认发送标志位使能，否则无效*/
}uart_frame_control_t;

/*蓝牙协议报文*/
enum
{
    BLE_AUTHOR = 0,       /*蓝牙鉴权报文*/
    BLE_AUTHOR_ACK1,      /*蓝牙鉴权应答报文1*/
    BLE_AUTHOR_ACK2,      /*蓝牙鉴权应答报文2*/
    BLE_AUTO_CHARGE,      /*车辆自动充电请求报文*/
    BLE_AUTO_CHARGE_ACK,  /*车辆自动充电应答报文*/
    BLE_FRAME_MAX,
};

/*蓝牙健全结果*/
typedef enum
{
    AUTHOR_SUCCESS = 0,       /*鉴权成功*/
    AUTHOR_FAIL,              /*鉴权失败*/
    GUN_DISCONNECTED,         /*枪未连接*/
    
}author_ret_e;

/*BLE串口数据**/
typedef struct
{
    uint8_t         bms_vin[17];                 /*车辆识别码VIN*/
    author_ret_e    author_ret;                  /*鉴权结果*/
}ble_frame_t;
/* ==============================  EXTERNS   =============================== */



#pragma pack()         /*恢复系统默认对其*/
/* ========================= FUNCTION PROTOTYPES =========================== */
/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/


#endif
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
