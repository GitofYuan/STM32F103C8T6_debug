/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : device_control_global.h
* Author            : Yuan.Zong
* Version           : V1.0.0/2026.02.27
* Description       :全局设备控制函数声明，思考：我已经忘记了为啥不把这些函数接口分别声明在各自的外设控制文件中，
                     而是放在了一个全局的头文件中，可能是为了统一管理和调用这些控制函数。
******************************************************************************/
#ifndef __DEVICE_CONTROL_GLOBAL_H__
#define __DEVICE_CONTROL_GLOBAL_H__
/* ==============================  INCLUDES  =============================== */
#include "device_control.h"
/* ==============================  DEFINES   =============================== */

/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */

/* ==============================  EXTERNS   =============================== */

/* ========================= FUNCTION PROTOTYPES =========================== */
/*****************************************************************************************************
*Function   :gpio_stm32_control（GPIO控制接口）
*Description:根据传入的设备信息、控制类型和配置内容，执行相应的GPIO操作，包括打开、关闭、读取、写入和配置等。
*Input      :DeviceRuntimeInfo     *dev     设备运行时信息结构体指针，包含GPIO端口和引脚等信息。
             device_ctrl_type_e    ctrl_type  控制类型枚举值，指定要执行的操作类型（如打开、关闭、读取、写入、配置等）。
             device_ctrl_content_u *content   配置内容联合体指针，包含GPIO的模式、上下拉、速度和电平等参数。
*Output     :
*Returns    :bool    false/true    操作失败/成功
*Note       :
*****************************************************************************************************/
bool gpio_stm32_control(DeviceRuntimeInfo *dev, device_ctrl_type_e ctrl_type, device_ctrl_content_u *content);

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
bool uart_stm32_control(DeviceRuntimeInfo *dev, device_ctrl_type_e ctrl_type, device_ctrl_content_u *content);

/*****************************************************************************************************
*Function   :can_stm32_control（CAN控制接口）
*Description:根据传入的设备信息、控制类型和配置内容，执行相应的CAN操作，包括打开、关闭、读取、写入和配置等。
*Input      :DeviceRuntimeInfo     *dev       设备运行时信息结构体指针，包含CAN句柄和实例等信息。
             device_ctrl_type_e    ctrl_type  控制类型枚举值，指定要执行的操作类型（如打开、关闭、读取、写入、配置等）。
             device_ctrl_content_u *content   配置内容联合体指针，包含CAN的ID、数据、DLC和波特率等参数。
*Output     :
*Returns    :bool    false/true    操作失败/成功
*Note       :
*****************************************************************************************************/
bool can_stm32_control(DeviceRuntimeInfo *dev, device_ctrl_type_e ctrl_type, device_ctrl_content_u *content);

/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
bool adc_stm32_control(DeviceRuntimeInfo *dev, device_ctrl_type_e ctrl_type, device_ctrl_content_u *content);

#endif
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
