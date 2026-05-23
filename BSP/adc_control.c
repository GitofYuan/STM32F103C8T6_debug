/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : adc_control.c
* Author            : Yuan.Zong
* Version           : V1.0.0/2026.05.21
* Description       :
******************************************************************************/
/* ==============================  INCLUDES  =============================== */
#include <stdio.h>
#include <string.h>

#include "device_control.h"
/* ==============================  DEFINES   =============================== */

/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */

/* ==============================  EXTERNS   =============================== */

/* ========================= FUNCTION PROTOTYPES =========================== */
/*====================== ADC适配实现（简化版） ======================*/
/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
//static bool adc_stm32_init(DeviceRuntimeInfo *dev, device_ctrl_content_u *content) 
//{
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
bool adc_stm32_control(DeviceRuntimeInfo *dev, device_ctrl_type_e ctrl_type, device_ctrl_content_u *content) 
{
   if (dev == NULL || content == NULL)
   {
       return false;
   }

//   ADC_HandleTypeDef *hadc = dev->hw_res.adc.hadc;
   switch (ctrl_type) 
   {
        case DEV_CTRL_OPEN:
            break;
        case DEV_CTRL_CLOSE:
            break;
        case DEV_CTRL_READ:
            break;
        case DEV_CTRL_CONFIG:
            break;
        case DEV_CTRL_CALIBRATE:
            break;
        case DEV_CTRL_GET_STATUS:
            break;
        default:break;
   }
   return true;
}

/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
