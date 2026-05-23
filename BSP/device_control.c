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
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
// 辅助函数：根据类型+名称查找设备状态
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
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
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
