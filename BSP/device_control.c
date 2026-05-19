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

// ====================== ADC适配实现（简化版） ======================
/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
//static bool adc_stm32_init(const char *dev_name, void *init_config) {
//    DeviceRuntimeInfo *dev = alloc_device_slot();
//    if (dev == NULL) return false;

//    // 绑定ADC硬件资源
//    if (strcmp(dev_name, "ADC1_CH0") == 0) {
//        dev->hw_res.adc.hadc = &hadc1;
//        dev->hw_res.adc.channel = ADC_CHANNEL_0;
//    } else if (strcmp(dev_name, "ADC1_CH1") == 0) {
//        dev->hw_res.adc.hadc = &hadc1;
//        dev->hw_res.adc.channel = ADC_CHANNEL_1;
//    } else return false;

//    // 初始化ADC（默认12位分辨率）
//    ADC_HandleTypeDef *hadc = dev->hw_res.adc.hadc;
//    hadc->Init.Resolution = ADC_RESOLUTION_12B;
//    hadc->Init.ScanConvMode = DISABLE;
//    hadc->Init.ContinuousConvMode = DISABLE;
//    hadc->Init.DiscontinuousConvMode = DISABLE;
//    hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
//    hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
//    hadc->Init.NbrOfConversion = 1;
//    if (HAL_ADC_Init(hadc) != HAL_OK) {
//        return false;
//    }

//    // 初始化设备状态
//    dev->type = DEV_TYPE_ADC;
//    strncpy(dev->name, dev_name, sizeof(dev->name)-1);
//    dev->is_inited = true;
//    dev->is_opened = true;
//    return true;
//}

/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
//static bool adc_stm32_control(DeviceRuntimeInfo *dev, Device_Ctrl_Type ctrl_type, DeviceCtrlContent *content) {
//    if (dev == NULL || content == NULL) return false;

//    ADC_HandleTypeDef *hadc = dev->hw_res.adc.hadc;
//    switch (ctrl_type) {
//        case DEV_CTRL_READ:
//            // 读取ADC值，写入content->adc.adc_value
//            HAL_ADC_Start(hadc);
//            if (HAL_ADC_PollForConversion(hadc, 1000) == HAL_OK) {
//                content->adc.adc_value = HAL_ADC_GetValue(hadc);
//            } else {
//                return false;
//            }
//            break;
//        case DEV_CTRL_CONFIG:
//            // 修改ADC分辨率
//            hadc->Init.Resolution = (content->adc.resolution == 12) ? ADC_RESOLUTION_12B : ADC_RESOLUTION_10B;
//            if (HAL_ADC_Init(hadc) != HAL_OK) return false;
//            break;
//        case DEV_CTRL_CALIBRATE:
//            // ADC校准
//            if (HAL_ADCEx_Calibration_Start(hadc) != HAL_OK) return false;
//            content->adc.is_calibrated = true;
//            break;
//        case DEV_CTRL_GET_STATUS:
//            content->adc.is_calibrated = true; // 简化：默认已校准
//            break;
//        default:
//            return false;
//    }
//    return true;
//}

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
        case DEV_TYPE_ADC:
//            return adc_stm32_control(dev, ctrl_type, content);
        /* 可扩展CAN/I2C等外设 */
        default:
            return false;
    }
}


/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
