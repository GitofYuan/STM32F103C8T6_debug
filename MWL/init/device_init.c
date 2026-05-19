/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : device_init.c
* Author            : Yuan.Zong
* Version           : V1.0.0
* Description       : 
******************************************************************************/

/* ==============================  INCLUDES  =============================== */
#include <stdbool.h>
#include "device_init.h"
/* ==============================  DEFINES   =============================== */
#define GPIO_DEV_NUM 13
#define UART_DEV_NUM 2
/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */

/* ==============================  EXTERNS   =============================== */
device_ctrl_content_u gpio_init_config[GPIO_DEV_NUM] = 
{                           /*空         初始电平          GPIO模式                  GPIO上下拉            GPIO输出速度*/  
/*address1*/          {.gpio={0,         0,              INPUT,                    GPIO_PULL_NONE,      0}},
/*address2*/          {.gpio={0,         0,              INPUT,                    GPIO_PULL_NONE,      0}},
/*address3*/          {.gpio={0,         0,              INPUT,                    GPIO_PULL_NONE,      0}},
/*address4*/          {.gpio={0,         0,              INPUT,                    GPIO_PULL_NONE,      0}},
/*run_led*/           {.gpio={0,         GPIO_RESET,     OUTPUT_PUSH_PULL,         GPIO_PULL_NONE,      OUTPUT_LOW}},
/*iso_ac_ctrl*/       {.gpio={0,         GPIO_RESET,     OUTPUT_PUSH_PULL,         GPIO_PULL_NONE,      OUTPUT_LOW}},
/*iso_power_ctrl*/    {.gpio={0,         GPIO_RESET,     OUTPUT_PUSH_PULL,         GPIO_PULL_NONE,      OUTPUT_LOW}},
/*iso_select*/        {.gpio={0,         GPIO_RESET,     OUTPUT_PUSH_PULL,         GPIO_PULL_NONE,      OUTPUT_LOW}},
/*iso_a2_ctrl*/       {.gpio={0,         GPIO_RESET,     OUTPUT_PUSH_PULL,         GPIO_PULL_NONE,      OUTPUT_LOW}},
/*iso_a1_ctrl*/       {.gpio={0,         GPIO_RESET,     OUTPUT_PUSH_PULL,         GPIO_PULL_NONE,      OUTPUT_LOW}},
/*iso_a0_ctrl*/       {.gpio={0,         GPIO_RESET,     OUTPUT_PUSH_PULL,         GPIO_PULL_NONE,      OUTPUT_LOW}},
/*relay_ctrl1*/       {.gpio={0,         GPIO_RESET,     OUTPUT_PUSH_PULL,         GPIO_PULL_NONE,      OUTPUT_LOW}},
/*relay_ctrl2*/       {.gpio={0,         GPIO_RESET,     OUTPUT_PUSH_PULL,         GPIO_PULL_NONE,      OUTPUT_LOW}},
};
const char *gpio_dev_list[GPIO_DEV_NUM] = 
{
    "address1", "address2", "address3", "address4", "run_led", 
    "iso_ac_ctrl", "iso_power_ctrl", "iso_select", "iso_a2_ctrl", "iso_a1_ctrl", 
    "iso_a0_ctrl", "relay_ctrl1", "relay_ctrl2"
};

device_ctrl_content_u uart_init_config[UART_DEV_NUM] = 
{                     /*收发缓冲区   数据长度   超时时间    传输模式               收发模式       波特率        数据位          停止位         校验位*/  
/*usart1*/      {.uart={0,         0,        0,        UART_MODE_DMA_RX_TX,  UART_TX_RX,   BAUD_9600,   DATA_BITS_8,   STOP_BITS_1,  PARITY_NONE}},
/*usart2*/      {.uart={0,         0,        0,        UART_MODE_DMA_RX_TX,  UART_TX_RX,   BAUD_9600,   DATA_BITS_8,   STOP_BITS_1,  PARITY_NONE}},
};
const char *uart_dev_list[UART_DEV_NUM] = 
{
    "usart1", "usart2"
};
/* ========================= FUNCTION PROTOTYPES =========================== */
/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void all_device_init(void)
{
    for(int i = 0; i < GPIO_DEV_NUM; i++)
    {
        device_control(DEV_TYPE_GPIO, gpio_dev_list[i], DEV_CTRL_OPEN, &gpio_init_config[i]);
    }

    for(int i = 0; i < UART_DEV_NUM; i++)
    {
        device_control(DEV_TYPE_UART, uart_dev_list[i], DEV_CTRL_OPEN, &uart_init_config[i]);
    }
}

/*****************************************************************************************************
*Function   :
*Description:
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
bool device_init(device_type_e dev_type, const char *dev_name)
{
    switch(dev_type)
    {
        case DEV_TYPE_GPIO:
            for(int i = 0; i < GPIO_DEV_NUM; i++)
            {
                if(strcmp(gpio_dev_list[i], dev_name) == 0)
                {
                    return device_control(DEV_TYPE_GPIO, gpio_dev_list[i], DEV_CTRL_OPEN, &gpio_init_config[i]);
                }
            }
            break;
        case DEV_TYPE_UART:
            for(int i = 0; i < UART_DEV_NUM; i++)
            {
                if(strcmp(uart_dev_list[i], dev_name) == 0)
                {
                    return device_control(DEV_TYPE_UART, uart_dev_list[i], DEV_CTRL_OPEN, &uart_init_config[i]);
                }
            }
            break;
        /*CAN*/

        /*ADC*/

        /*I2C*/

        /*SPI*/
        
        default:
            return false;
    }
}
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
