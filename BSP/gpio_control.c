/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : gpio_ctrl.c
* Author            : Yuan.Zong
* Version           : V1.0.0/2026.02.27
* Description       : 
******************************************************************************/
/* ==============================  INCLUDES  =============================== */
#include "device_control.h"

/* ==============================  DEFINES   =============================== */

/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */

/* ==============================  EXTERNS   =============================== */

/* ========================= FUNCTION PROTOTYPES =========================== */

// ====================== GPIO适配实现 ======================
/*****************************************************************************************************
*Function   :gpio_stm32_init（GPIO初始化函数）
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
static bool gpio_stm32_init(DeviceRuntimeInfo *dev, device_ctrl_content_u *content) 
{
    /* 初始化GPIO（默认输入模式，可后续通过control修改）*/
    GPIO_InitTypeDef gpio_init = {0};
    gpio_init.Pin = dev->hw_res.gpio.pin;
    switch(content->gpio.mode)
    {
        case INPUT:
            gpio_init.Mode = GPIO_MODE_INPUT;
            break;
        case OUTPUT_PUSH_PULL:
            gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
            break;
        case OUTPUT_OPEN_DRAIN:
            gpio_init.Mode = GPIO_MODE_OUTPUT_OD;
            break;
        default:
            return false;
    }
    switch(content->gpio.pull)
    {
        case GPIO_PULL_NONE:
            gpio_init.Pull = GPIO_NOPULL;
            break;
        case GPIO_PULL_UP:
            gpio_init.Pull = GPIO_PULLUP;
            break;
        case GPIO_PULL_DOWN:
            gpio_init.Pull = GPIO_PULLDOWN;
            break;
        default:
            return false;
    }
    switch(content->gpio.speed)
    {
        case OUTPUT_LOW:
            gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
            break;
        case OUTPUT_MEDIUM:
            gpio_init.Speed = GPIO_SPEED_FREQ_MEDIUM;
            break;
        case OUTPUT_HIGH:
            gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
            break;
        default:
            return false;
    }
    HAL_GPIO_Init(dev->hw_res.gpio.port, &gpio_init);

    switch(content->gpio.level)
    {
        case GPIO_RESET:
            HAL_GPIO_WritePin(dev->hw_res.gpio.port, dev->hw_res.gpio.pin, GPIO_PIN_RESET);
            break;
        case GPIO_SET:
            HAL_GPIO_WritePin(dev->hw_res.gpio.port, dev->hw_res.gpio.pin, GPIO_PIN_SET);
            break;
        default:
            return false;
    }

    dev->is_opened = true;
    return true;
}

/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
bool gpio_stm32_control(DeviceRuntimeInfo *dev, device_ctrl_type_e ctrl_type, device_ctrl_content_u *content) 
{
    if (dev == NULL || content == NULL) 
    {
        return false;
    }

    switch (ctrl_type) {
        case DEV_CTRL_READ:
            /* 读取电平，写入content->gpio.level */
            if(HAL_GPIO_ReadPin(dev->hw_res.gpio.port, dev->hw_res.gpio.pin) == GPIO_PIN_SET)
            {
                content->gpio.level = GPIO_SET;
            }
            else
            {
                content->gpio.level = GPIO_RESET;
            }
            break;
        case DEV_CTRL_WRITE:
            /* 写入电平，从content->gpio.level读取 */
            switch(content->gpio.level)
            {
                case GPIO_RESET:
                    HAL_GPIO_WritePin(dev->hw_res.gpio.port, dev->hw_res.gpio.pin, GPIO_PIN_RESET);
                    break;
                case GPIO_SET:
                    HAL_GPIO_WritePin(dev->hw_res.gpio.port, dev->hw_res.gpio.pin, GPIO_PIN_SET);
                    break;
                case GPIO_TOGGLE:
                    HAL_GPIO_TogglePin(dev->hw_res.gpio.port, dev->hw_res.gpio.pin);
                    break;
                default:
                    return false;
            }
            break;
        case DEV_CTRL_CONFIG:
            /* 调用复位函数，重置GPIO配置 */
            HAL_GPIO_DeInit(dev->hw_res.gpio.port, dev->hw_res.gpio.pin);
            gpio_stm32_init(dev, content);
            break;
        case DEV_CTRL_GET_STATUS:
            /* 返回GPIO是否打开 */
            content->is_open = dev->is_opened;
            break;
        case DEV_CTRL_OPEN:
            gpio_stm32_init(dev, content);
            break;
        case DEV_CTRL_CLOSE:
            /* 调用复位函数，重置GPIO配置 */
            HAL_GPIO_DeInit(dev->hw_res.gpio.port, dev->hw_res.gpio.pin);
            dev->is_opened = false;
            break;
        default:
            return false;
    }
    return true;
}

/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
