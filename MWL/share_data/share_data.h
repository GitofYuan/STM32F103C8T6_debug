/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : share_data.h
* Author            : Yuan.Zong
* Version           : V1.0.0
* Description       : 共享数据管理
******************************************************************************/
#ifndef __SHARE_DATA_H__
#define __SHARE_DATA_H__
/* ==============================  INCLUDES  =============================== */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "share_data_can.h"
#include "share_data_uart.h"

/* ==============================  DEFINES   =============================== */
#define disDEBUG_SHARE_DATA
#define CHAR_GUN_NUM   1          /*需要记录数据的充电枪数量*/

/*s为结构体类型名称，m为结构体字段名称,该宏定义获取结构体中某字段的相对偏移量*/
#define  OFFSET_OF(s,m)   (uint32_t)&(((s *)0)->m)  
/* ==============================   ENUMS    =============================== */
typedef  enum
{
    SYSTEM_INFO = 0,                                            /* 系统信息 system_info_t*/
    CONTROL_INFO,                                               /* 控制信息 control_info_t*/
    MESURE_INFO,                                                /* 采样信息 mesure_info_t*/
    FAULT_INFO,                                                 /* 故障信息 prot_info_t*/
    MAX_SHARE_DATA                                            
}share_data_type_e;

#pragma pack (1)                                                /* 设置1字节对齐 */
/****************************系统信息相关*******************************/
/* 控制器类型 */
typedef enum
{
    /*PCU*/
    PCU_V1 = 0,
    PCU_MAX_GRID,
    
    /*CCU*/
    CCU_V1 = 0x10,
    CCU_MAX_GRID,
    
    /*PDU*/
    PDU_V1 = 0x20,
    PDU_MAX_GRID,
}grid_type_e;

/*工作状态*/
typedef enum
{
    work_false = 0,
    work_true,
}work_flag_type_e;

/*软件版本号*/
typedef struct
{ 
    uint8_t      version1;            /*主版本号*/
    uint8_t      version2;            /*子版本号*/
    uint8_t      version3;            /*修订版本号*/
}soft_version_t;

/*充电枪基本工作状态*/
typedef  enum
{
    C_IDLE = 0,                                                   /*空闲*/
    C_CONNECTED ,                                                 /*已插枪*/
    C_SELF_CHECK ,                                                /*自检*/
    C_PRE_CHARGING,                                               /*预充*/
    C_CHARGING,                                                   /*充电中*/
    C_FINISHING,                                                  /*结束中*/
    C_FINISHED,                                                   /*已结束*/
}gun_bass_status_e;

/*充电枪逻辑工作状态*/
typedef struct
{
    uint8_t      fault_flag;                                      /*故障标志位*/
    uint8_t      disable_flag;                                    /*禁用标志位*/
    uint8_t      maintenance_flag;                                /*维护标志位*/
    uint8_t      reserving_flag;                                  /*预约标志位*/
}gun_logical_status_t;

/*充电枪业务状态*/
typedef  enum
{
    B_IDLE = 0,                                                   /*空闲*/
    B_CONNECTED ,                                                 /*已插枪*/
    B_STARTING ,                                                  /*启动中*/
    B_CHARGINR,                                                   /*充电中*/
    B_FINISH,                                                     /*结束*/
    B_FAULT,                                                      /*故障*/
    B_RESERVING,                                                  /*预约中*/
    B_DISABLE,                                                    /*禁用*/
    B_MAINTENANCE,                                                /*维护中*/
}gun_business_status_e;

/*系统时间*/
typedef struct
{
    uint8_t      year;                /*年，2000偏移量*/
    uint8_t      month;               /*月*/
    uint8_t      day;                 /*日*/
    uint8_t      hour;                /*时*/
    uint8_t      minute;              /*分*/
    uint8_t      second;              /*秒*/
}system_time_t;

/****************************控制信息相关*******************************/
/*由板子向外输出的控制信号归类为控制信息，比如发送的CAN报文，DO输出信号，PWM波，串口发送的数据等等*/
/*CAN数据由于太多，所以专门创建了share_data_can.h，相关信息都在里面*/

/****************************采样数据相关*******************************/
/*从板子外部获取的数据均归类为采样数据，比如接收到的报文，DI反馈信号，ADC采样数据，串口获取的数据等等*/
/*CAN数据由于太多，所以专门创建了share_data_can.h，相关信息都在里面*/

/*充电枪连接状态*/
typedef  enum
{
    DEFAULT = 0,                                                /*初始化默认状态（非实际物理状态）*/
    DISCONNECTED ,                                              /*未连接*/
    HALF_CONNECTED,                                             /*半连接*/
    CONNECTED,                                                  /*完全连接*/
}gun_connect_status_e;

/****************************故障信息相关*******************************/

/* ======================== STRUCTURES AND UNIONS ========================== */
/* 系统信息*/
typedef struct
{
    uint8_t                  board_id;                                  /* 板子编号  */
    grid_type_e              pro_type;                                  /* 控制器类型*/
    soft_version_t           soft_version;                              /* 软件版本号 */
    work_flag_type_e         work_flag;                                 /* 1:工作中,0:空闲*/
    uint8_t                  fct_flag;                                  /* 1:工厂模式,0:普通模式*/
    uint8_t                  ota_flag;                                  /* OTA标志位*/
    gun_logical_status_t     gun_logical_status[CHAR_GUN_NUM];          /*枪逻辑工作状态*/
    gun_bass_status_e        gun_bass_status[CHAR_GUN_NUM];             /*枪基本工作状态*/
    gun_business_status_e    gun_business_status[CHAR_GUN_NUM];         /*枪业务状态*/
    system_time_t            system_time;                               /*系统时间*/
    uint16_t                 out_of_bounds;                             /*防越界标志位*/
}system_info_t;


/* 采样信息 */
typedef struct
{
    gun_connect_status_e     gun_connect_status[CHAR_GUN_NUM];          /*枪连接状态*/
    uint8_t                  e_lock_feedback_status[CHAR_GUN_NUM];      /*电子锁反馈状态，0解锁，1上锁*/
    uint8_t                  k1k2_feedback_status[CHAR_GUN_NUM];        /*主接触器反馈状态，0断开，1闭合*/
    uint8_t                  gun_dc1_tem[CHAR_GUN_NUM];                 /*枪DC+温度，℃，1精度，-50偏移量*/
    uint8_t                  gun_dc2_tem[CHAR_GUN_NUM];                 /*枪DC-温度，℃，1精度，-50偏移量*/
    bms_charging_info_t      bms_charging_info[CHAR_GUN_NUM];           /*车辆充电信息*/
    ble_frame_t              ble_frame;                                 /*BLE串口数据*/
    uint16_t                 out_of_bounds;                             /*防越界标志位*/
}mesure_info_t;

/* 控制信息 */
typedef struct
{
    uint8_t                  low_vol_power_control_status[CHAR_GUN_NUM];    /*低压供电控制状态，0断开，1闭合*/
    uint8_t                  e_lock_conctol_status[CHAR_GUN_NUM];           /*电子锁控制状态，0解锁，1上锁*/
    uint8_t                  k1k2_conctol_status[CHAR_GUN_NUM];             /*主接触器控制状态，0断开，1闭合*/
    charger_charging_info_t  charger_charging_info[CHAR_GUN_NUM];           /*充电机充电信息*/
    can_frame_control_t      bms_can_control[CHAR_GUN_NUM][CAN27930MAX];    /*充电CAN报文收发标志位*/
    uart_frame_control_t     ble_uart_control[BLE_FRAME_MAX];               /*BLE串口报文收发标志位*/
    uint16_t                 out_of_bounds;                                 /*防越界标志位*/
}control_info_t;

/* 故障信息 */
typedef struct
{
    /*一级故障：须100ms内切断输出或输入的紧急故障*/

    /*二级故障：需按正常流程进行停止工作的严重故障*/

    /*三级故障：需系统做出应对动作但不致于停机的故障*/

    /*四级故障：仅需系统进行记录和上报的轻微故障*/

    uint16_t                 out_of_bounds;                          /*防越界标志位*/
}fault_info_t;

#pragma pack ()                                                 /* 恢复系统默认对齐 */
/* ==============================  EXTERNS   =============================== */



/* ========================= FUNCTION PROTOTYPES =========================== */
/*功能实现接口函数*/
/*****************************************************************************************************
*Function   :share_data_read（读取共享内存数据）
*Description:
*Input      :const share_data_type_t    type      读取的数据类型
             const uint32_t             offset    读取的数据偏移量
             const uint32_t             size      读取数据长度（单位：字节）
*Output     :const void*                out       读取数据存放地址
*Returns    :bool    false/true    失败/成功
*Note       :rtos初始化后,读取系统参数前调用。该函数会阻塞不能在中断中调用。
*****************************************************************************************************/
bool share_data_read(const share_data_type_e type, const uint32_t offset, void* out, const uint32_t size);

/*****************************************************************************************************
*Function   :share_data_write（写入数据到共享内存）
*Description:
*Input      :const share_data_type_t    type      写入的数据类型
             const uint32_t             offset    写入的数据偏移量
             void*                      in        待写入的数据地址
             const uint32_t             size      写入数据长度（单位：字节）
*Output     :
*Returns    :bool    false/true    失败/成功
*Note       :rtos初始化后,读取系统参数前调用。该函数会阻塞不能在中断中调用。
*****************************************************************************************************/
bool share_data_write(const share_data_type_e type, const uint32_t offset, const void* in, const uint32_t size);

/*外侧依赖接口函数*/
/*****************************************************************************************************
*Function   :share_data_init（共享内存初始化）
*Description:
*Input      :grid_type_e    type    要申请内存的设备类型
*Output     :
*Returns    :bool    false/true    失败/成功
*Note       :rtos初始化后,读取系统参数前调用。
*****************************************************************************************************/
bool share_data_init(grid_type_e type);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/


