/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : device_ctrl.h
* Author            : Yuan.Zong
* Version           : V1.0.0/2026.02.27
* Description       : 
******************************************************************************/
/* ==============================  INCLUDES  =============================== */
#include "device_control.h"
#include "device_control_global.h"
#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdio.h>
#include "cmsis_os.h"

/* ==============================  DEFINES   =============================== */

/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */

/* ==============================  EXTERNS   =============================== */

/* ========================= FUNCTION PROTOTYPES =========================== */
/*****************************************************************************************************
*Function   : find_device（设备查找函数）
*Description: 根据设备类型和名称查找设备状态
*Input      : dev_type - 设备类型
*             dev_name - 设备名称
*Output     : 找到的设备状态指针，未找到返回NULL
*Returns    : bool  false/true  查找结果失败/成功
*Note       : 该函数用于在设备列表中查找指定类型的设备
*****************************************************************************************************/
static DeviceRuntimeInfo *find_device(device_type_e dev_type, const char *dev_name) 
{
    if (dev_name == NULL) 
    {
        return NULL;
    }
    for (int i = 0; i < MAX_DEVICE_CNT; i++) 
    {
        if (dev_list[i].type == dev_type 
            && strcmp(dev_list[i].name, dev_name) == 0) 
        {
            return &dev_list[i];
        }
    }
    return NULL;
}

// ====================== 核心接口实现 ======================
/*****************************************************************************************************
*Function   :device_control（设备控制接口）
*Description:根据设备类型、设备名称、控制类型和配置内容，执行相应的设备操作，包括打开、关闭、读取、写入和配置等。
*Input      :device_type_e         dev_type   设备类型枚举值，指定要操作的设备类型（如GPIO、UART、CAN等）。
             const char            *dev_name  设备名称字符串，指定要操作的具体设备。
             device_ctrl_type_e    ctrl_type  控制类型枚举值，指定要执行的操作类型（如打开、关闭、读取、写入、配置等）。
             device_ctrl_content_u *content   配置内容联合体指针，包含设备的相关参数和数据。
*Output     :
*Returns    :bool    false/true    操作失败/成功
*Note       :
*****************************************************************************************************/
bool device_control(device_type_e dev_type, const char *dev_name, device_ctrl_type_e ctrl_type, device_ctrl_content_u *content) 
{
    /* 入参检查 */
    if (dev_name == NULL || content == NULL) 
    {
        return false;
    }

    /* 查找设备 */
    DeviceRuntimeInfo *dev = find_device(dev_type, dev_name);
    if (dev == NULL) 
    {
        return false;
    }

    /* 根据外设类型调用对应控制函数 */
    switch (dev_type) 
    {
        case DEV_TYPE_GPIO:
            return gpio_stm32_control(dev, ctrl_type, content);
        case DEV_TYPE_UART:
            return uart_stm32_control(dev, ctrl_type, content);
        case DEV_TYPE_CAN:
            return can_stm32_control(dev, ctrl_type, content);
        case DEV_TYPE_ADC:
            return adc_stm32_control(dev, ctrl_type, content);
        /* 可扩展其他外设 */
        default:
            return false;
    }
}


/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
