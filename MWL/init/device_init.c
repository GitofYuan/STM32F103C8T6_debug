/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : device_init.c
* Author            : Yuan.Zong
* Version           : V1.0.0
* Description       : 
******************************************************************************/

/* ==============================  INCLUDES  =============================== */
#include <stdbool.h>
#include <string.h>
#include "cmsis_os.h"

#include "device_control.h"
#include "uart_bus.h"
#include "can_bus.h"
#include "J1939.h"

/* ==============================  DEFINES   =============================== */
#define GPIO_DEV_NUM   14   /*GPIO设备数量*/
#define UART_DEV_NUM   2    /*UART设备数量*/
#define CAN_DEV_NUM    1    /*CAN设备数量*/

#define RUN_LED_TIMER_INTERVAL   500 /*运行指示灯闪烁间隔，单位为ms*/
/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */
static uint8_t device_init_flag = 0;    /*设备初始化标志，0表示未初始化，1表示已初始化*/
static uint16_t run_led_timer = 0;      /*运行指示灯定时器，单位为ms*/
/* ==============================  EXTERNS   =============================== */
device_ctrl_content_u gpio_init_config[GPIO_DEV_NUM] = 
{                           /*写入电平             初始电平          GPIO模式                  GPIO上下拉           GPIO输出速度*/  
/*address1*/          {.gpio={GPIO_RESET,         GPIO_RESET,     INPUT,                    GPIO_PULL_NONE,      OUTPUT_LOW}},
/*address2*/          {.gpio={GPIO_RESET,         GPIO_RESET,     INPUT,                    GPIO_PULL_NONE,      OUTPUT_LOW}},
/*address3*/          {.gpio={GPIO_RESET,         GPIO_RESET,     INPUT,                    GPIO_PULL_NONE,      OUTPUT_LOW}},
/*address4*/          {.gpio={GPIO_RESET,         GPIO_RESET,     INPUT,                    GPIO_PULL_NONE,      OUTPUT_LOW}},
/*run_led*/           {.gpio={GPIO_RESET,         GPIO_RESET,     OUTPUT_PUSH_PULL,         GPIO_PULL_NONE,      OUTPUT_LOW}},
/*iso_ac_ctrl*/       {.gpio={GPIO_RESET,         GPIO_RESET,     OUTPUT_PUSH_PULL,         GPIO_PULL_NONE,      OUTPUT_LOW}},
/*iso_power_ctrl*/    {.gpio={GPIO_RESET,         GPIO_RESET,     OUTPUT_PUSH_PULL,         GPIO_PULL_NONE,      OUTPUT_LOW}},
/*iso_select*/        {.gpio={GPIO_RESET,         GPIO_RESET,     OUTPUT_PUSH_PULL,         GPIO_PULL_NONE,      OUTPUT_LOW}},
/*iso_a2_ctrl*/       {.gpio={GPIO_RESET,         GPIO_RESET,     OUTPUT_PUSH_PULL,         GPIO_PULL_NONE,      OUTPUT_LOW}},
/*iso_a1_ctrl*/       {.gpio={GPIO_RESET,         GPIO_RESET,     OUTPUT_PUSH_PULL,         GPIO_PULL_NONE,      OUTPUT_LOW}},
/*iso_a0_ctrl*/       {.gpio={GPIO_RESET,         GPIO_RESET,     OUTPUT_PUSH_PULL,         GPIO_PULL_NONE,      OUTPUT_LOW}},
/*relay_ctrl1*/       {.gpio={GPIO_RESET,         GPIO_RESET,     OUTPUT_PUSH_PULL,         GPIO_PULL_NONE,      OUTPUT_LOW}},
/*relay_ctrl2*/       {.gpio={GPIO_RESET,         GPIO_RESET,     OUTPUT_PUSH_PULL,         GPIO_PULL_NONE,      OUTPUT_LOW}},
/*led*/               {.gpio={GPIO_RESET,         GPIO_RESET,     OUTPUT_PUSH_PULL,         GPIO_PULL_NONE,      OUTPUT_LOW}},
};
const char *gpio_dev_list[GPIO_DEV_NUM] = 
{
    "address1", "address2", "address3", "address4", "run_led", 
    "iso_ac_ctrl", "iso_power_ctrl", "iso_select", "iso_a2_ctrl", "iso_a1_ctrl", 
    "iso_a0_ctrl", "relay_ctrl1", "relay_ctrl2", "led"
};

device_ctrl_content_u uart_init_config[UART_DEV_NUM] = 
{                     /*收发缓冲区   数据长度   超时时间    传输模式               收发模式       波特率        数据位          停止位         校验位*/  
/*usart1*/      {.uart={0,           0,          0,          UART_MODE_DMA_RX_TX,  UART_TX_RX,   BAUD_115200,   DATA_BITS_8,   STOP_BITS_1,  PARITY_NONE}},
/*usart2*/      {.uart={0,           0,          0,          UART_MODE_POLLING,    UART_TX_RX,   BAUD_19200,   DATA_BITS_8,   STOP_BITS_1,  PARITY_NONE}},
};
const char *uart_dev_list[UART_DEV_NUM] = 
{
    "usart1", "usart2"
};

device_ctrl_content_u can_init_config[CAN_DEV_NUM] = 
{                  /*canid     数据指针    数据长度    波特率*/  
/*can1*/      {.can={0,         0,        0,        BAUD_CAN_250K}},
};
const char *can_dev_list[CAN_DEV_NUM] = 
{
    "can1"
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

    for(int i = 0; i < CAN_DEV_NUM; i++)
    {
        device_control(DEV_TYPE_CAN, can_dev_list[i], DEV_CTRL_OPEN, &can_init_config[i]);
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
        case DEV_TYPE_CAN:
            for(int i = 0; i < CAN_DEV_NUM; i++)
            {
                if(strcmp(can_dev_list[i], dev_name) == 0)
                {
                    return device_control(DEV_TYPE_CAN, can_dev_list[i], DEV_CTRL_OPEN, &can_init_config[i]);
                }
            }
            break;
        /*ADC*/

        /*I2C*/

        /*SPI*/
        
        default:
            return false;
    }
    return false;
}

/*****************************************************************************************************
*Function   :BSP_Task(驱动层任务)
*Description:负责进行设备初始化、系统运行指示灯控制以及调用支撑中间层功能正常运行的定时处理函数。
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void BSP_Task(void *argument)
{
    /* USER CODE BEGIN BSP_Task */
    /* Infinite loop */
    for(;;)
    {
        osDelay(1);
        /*设备初始化*/
        if(device_init_flag == 0)
        {
            all_device_init();
            device_ctrl_content_u content;
            content.gpio.level = GPIO_SET;
            device_control(DEV_TYPE_GPIO, "iso_ac_ctrl", DEV_CTRL_WRITE, &content);
            
            device_init_flag = 1;   /*设置设备初始化标志，避免重复初始化*/        
        }

        /*运行指示灯控制*/
        if(run_led_timer >= RUN_LED_TIMER_INTERVAL)
        {
            run_led_timer = 0;
            device_ctrl_content_u content;
            content.gpio.level = GPIO_TOGGLE;
            device_control(DEV_TYPE_GPIO, "led", DEV_CTRL_WRITE, &content);
        }
        else
        {
            run_led_timer += 1; /*每次循环增加1ms*/
        }

        /*其他中间层任务处理*/
        
        uart_tx_dequeue();
    }
    /* USER CODE END BSP_Task */
}

/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
