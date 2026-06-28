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

/*蓝牙鉴权结果*/
typedef enum
{
    AUTHOR_RET_INIT = 0,      /*初始化*/
    AUTHOR_SUCCESS,           /*鉴权成功*/
    AUTHOR_FAIL,              /*鉴权失败*/
    GUN_DISCONNECTED,         /*枪未连接*/
}author_ret_e;

/*设备类型*/
typedef enum
{
    DEVICE_TYPE_INIT = 0,                 /*初始化*/
    HOME_AUTO_CHARGER,        /*家充机器人*/
    HOME_CHARGER,             /*家充*/
    HOME_CHARGER_LOCK,        /*家充地锁*/
    PUBLIC_CHARGER_LOCK,      /*公充地锁*/
}charger_device_type_e;

/*鉴权失败原因*/
typedef enum
{
    FAIL_REASON_INIT = 0,                 /*初始化*/
    NOT_IN_WHITE_LIST,        /*不在白名单*/
    DEVICE_UNAVAILABLE,       /*设备不可用*/
}author_fail_reason_e;

/*车辆充电请求*/
typedef enum
{
    CHARGE_REQUEST_INIT = 0,                 /*初始化*/
    REQUEST_CHARGE,           /*请求插枪*/
    REQUEST_UNCHARGE,         /*请求拔枪*/
    REQUEST_RESERVE,          /*请求撤销*/
}vehicle_charge_request_e;

/*车辆位置判断*/
typedef enum
{
    INIT = 0,               /*初始化*/
    OUT_OF_RANGE,           /*超范围*/
    IN_RANGE,               /*在范围内*/
}vehicle_position_e;

/*机械臂运行状态*/
typedef enum
{
    ARM_STATUS_INIT = 0,               /*初始化*/
    LOCATING,               /*正在定位*/
    INSERTING,              /*插枪中*/
    INSERTED,               /*插枪完成*/
    INSERT_FAILED,          /*插枪失败*/
    REMOVING,               /*拔枪中*/
    REMOVED,                /*拔枪完成*/
    REMOVE_FAILED,          /*拔枪失败*/
}charger_arm_status_e;

/*失败原因*/
typedef enum
{
    ARM_FAIL_INIT = 0,                    /*初始化*/
    INSERT_OUT_OF_RANGE,                /*超范围*/
    GET_GUN_FAILED,              /*取枪失败*/
    CHARGING_PORT_NOT_OPEN,      /*充电口盖未开启*/
    CHARGING_PORT_BLOCK,         /*充电口有障碍物*/
    GO_MOTION_PATH_BLOCK,        /*插枪运动轨迹受限*/
    OTHER_REASON_INSERT_FAILED,  /*其他原因插枪失败*/
    DEVICE_FAULT_INSERT_FAILED,  /*设备故障插枪失败*/
    RELEASE_MOTION_PATH_BLOCK,   /*还枪运动轨迹受限*/
    OTHER_REASON_RELEASE_FAILED, /*其他原因还枪失败*/
    LOOSEN_FAILED,               /*分离失败*/
    BACK_MOTION_PATH_BLOCK,      /*归枪运动轨迹受限*/
    BACK_GET_GUN_FAILED,         /*取枪失败*/
    RELEASE_GUN_FAILED,          /*还枪失败*/
    OTHER_REASON_REMOVE_FAILED,  /*其他原因拔枪失败*/
    DEVICE_FAULT_REMOVE_FAILED,  /*设备故障拔枪失败*/
}arm_fail_reason_e;

/*BLE串口数据**/
typedef struct
{
    uint8_t                  bms_vin[17];            /*车辆识别码VIN*/
    vehicle_charge_request_e vehicle_charge_request; /*车辆充电请求*/
    author_ret_e             author_ret;             /*鉴权结果*/
    author_fail_reason_e     fail_reason;            /*鉴权失败原因*/
    charger_device_type_e    device_type;            /*设备类型*/
    vehicle_position_e       vehicle_position;       /*车辆位置判断*/
    charger_arm_status_e     charger_arm_status;     /*机械臂运行状态*/
    arm_fail_reason_e        arm_fail_reason;        /*机械臂失败原因*/
    uint8_t                  recv_vin[17];           /*接收的VIN码*/
    
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
