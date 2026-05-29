/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : device_control.h
* Author            : Yuan.Zong
* Version           : V1.0.0/2026.02.27
* Description       :对驱动层外设的操作封装文件，主要核心实现为device_control接口函数，
                     覆盖GPIO/UART/ADC/CAN/I2C/SPI等常见外设，后续可扩展。
                     主要控制类型:open       打开/即初始化使能外设
                                close      关闭/即禁用外设
                                read       读取数据（GPIO电平/ADC值/UART数据/CAN帧）
                                write      写入数据（GPIO电平/UART数据/CAN帧）
                                config     修改配置（如UART波特率/GPIO模式）
                                get_status 获取状态（如外设是否打开/ADC校准状态）
                                calibrate  校准（如ADC校准）
******************************************************************************/
#ifndef __DEVICE_CONTROL_H__
#define __DEVICE_CONTROL_H__
/* ==============================  INCLUDES  =============================== */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "gpio.h"
#include "usart.h"
#include "can.h"
#include "device_control_gpio.h"
#include "device_control_uart.h"
#include "device_control_can.h"
#include "device_control_adc.h"
/* ==============================  DEFINES   =============================== */
#define MAX_DEVICE_CNT 32
/* ==============================   ENUMS    =============================== */
/* 外设类型枚举（覆盖所有常见外设，可扩展）*/ 
typedef enum 
{
    DEV_TYPE_GPIO,    // GPIO
    DEV_TYPE_ADC,     // ADC
    DEV_TYPE_UART,    // UART
    DEV_TYPE_CAN,     // CAN
    DEV_TYPE_I2C,     // I2C
    DEV_TYPE_SPI,     // SPI
    DEV_TYPE_FLASH,   // Flash
    DEV_TYPE_UNKNOWN  // 未知外设
} device_type_e;

/* 控制类型枚举（覆盖所有外设的通用操作，可扩展）*/
typedef enum 
{
    DEV_CTRL_OPEN,        // 打开/使能外设
    DEV_CTRL_CLOSE,       // 关闭/禁用外设
    DEV_CTRL_READ,        // 读取数据（GPIO电平/ADC值/UART数据/CAN帧）
    DEV_CTRL_WRITE,       // 写入数据（GPIO电平/UART数据/CAN帧）
    DEV_CTRL_CONFIG,      // 修改配置（如UART波特率/GPIO模式）
    DEV_CTRL_GET_STATUS,  // 获取状态（如外设是否打开/ADC校准状态）
    DEV_CTRL_CALIBRATE,   // 校准（如ADC校准）
    DEV_CTRL_UNKNOWN      // 未知操作
} device_ctrl_type_e;

/* ======================== STRUCTURES AND UNIONS ========================== */
// 3. 通用控制内容联合体（承载不同外设的差异化参数）
// 核心设计：用union适配不同外设的参数，上层按需赋值/解析S
typedef union 
{
    /* GPIO相关控制内容 */
    struct 
    {
        GPIO_level_e level;         /*GPIO读取或写入状态(READ/WRITE)*/
        GPIO_level_e init_level;    /*GPIO初始化时的默认状态（OPEN/CONFIG）*/
        GPIO_mode_e  mode;          /*GPIO模式（OPEN/CONFIG）*/
        GPIO_pull_e  pull;          /*GPIO上下拉（OPEN/CONFIG）*/
        GPIO_speed_e speed;         /*GPIO输出速度（OPEN/CONFIG）*/
    } gpio;

    /* UART相关控制内容 */
    struct 
    {
        uint8_t             *buf;           /* 收发缓冲区（READ/WRITE）*/
        size_t              len;            /* 数据长度（READ/WRITE）*/
        uint32_t            timeout_ms;     /* 超时时间（READ/WRITE）*/
        uart_mode_e         mode;           /* 传输模式：轮询/中断/DMA（CONFIG）*//*预留*/
        uart_rt_mode_e      rt_mode;        /* 收发模式：只收/只发/收发（CONFIG）*/
        uart_baud_rate_e    baud_rate;      /* 波特率（CONFIG）*/
        uart_data_bits_e    data_bits;      /* 数据位（CONFIG）*/
        uart_stop_bits_e    stop_bits;      /* 停止位（CONFIG）*/
        uart_parity_e       parity;         /* 校验位（CONFIG）*/
    } uart;

    /* CAN相关控制内容 */
    struct 
    {
        uint32_t         id;            // CAN帧ID（READ/WRITE）
        uint8_t          *data;         // CAN帧数据（READ/WRITE）
        uint8_t          dlc;           // 数据长度（READ/WRITE）
        can_baud_rate_e  baudrate;      // 波特率（CONFIG）
    } can;

    // ADC相关控制内容
    struct 
    {
        uint16_t adc_value;     // ADC读取值（READ）
        uint8_t resolution;     // 分辨率：12/10/8（CONFIG）
        bool is_calibrated;     // 是否校准（GET_STATUS）
    } adc;

    // 通用参数（用于简单操作）
    bool is_open;               // 是否打开（GET_STATUS）
    uint64_t raw_data;          // 原始数据（适配无结构化参数的操作）
}device_ctrl_content_u;

// 4. 设备初始化配置结构体（初始化时传入，区分外设）
typedef struct 
{
    device_type_e dev_type;        // 外设类型
    const char *dev_name;       // 设备名称（如"GPIOA_0"/"USART1"/"ADC1_CH0"）
    void *init_config;          // 初始化专属配置（指向各外设的初始化结构体）
}DeviceInitConfig;

/*串口数据位/停止位/校验位配置*/
typedef struct 
{
    uint8_t data_bits;   // 数据位：5/6/7/8
    uint8_t stop_bits;   // 停止位：1/2
    char parity;         // 校验位：'N'无校验 'O'奇校验 'E'偶校验
}SerialConfig;

/*串口设备句柄（隔离底层具体实现，上层仅持有句柄）*/
typedef void* SerialHandle;

/*存储设备运行时状态（关联名称、类型、底层资源）*/
typedef struct 
{
    device_type_e type;
    char name[32];
    bool is_opened;
    // 底层硬件资源（按外设分类）
    union 
    {
        struct 
        { 
            GPIO_TypeDef *port; 
            uint16_t      pin; 
        } gpio;

        struct 
        { 
            UART_HandleTypeDef *huart; 
            USART_TypeDef      *instance;
            uart_mode_e         mode;           /* 传输模式：轮询/中断/DMA（CONFIG）*//*预留*/
            uart_rt_mode_e      rt_mode;        /* 收发模式：只收/只发/收发（CONFIG）*/
        } uart;

        struct 
        { 
//            ADC_HandleTypeDef *hadc; 
            uint32_t channel; 
        } adc;
        
        struct 
        { 
            CAN_HandleTypeDef *hcan; 
            CAN_TypeDef       *instance;
            can_baud_rate_e   baud_rate;
        } can;
    } hw_res;
} DeviceRuntimeInfo;

/* ==============================  EXTERNS   =============================== */
static DeviceRuntimeInfo dev_list[MAX_DEVICE_CNT] = 
{
     /*外设类型        外设名称               开启状态       底层硬件资源信息*/
     /*GPIO*/
    {DEV_TYPE_GPIO,   "address1",          false,       .hw_res.gpio={GPIOA, GPIO_PIN_5}},
    {DEV_TYPE_GPIO,   "address2",          false,       .hw_res.gpio={GPIOA, GPIO_PIN_6}},
    {DEV_TYPE_GPIO,   "address3",          false,       .hw_res.gpio={GPIOA, GPIO_PIN_7}},
    {DEV_TYPE_GPIO,   "address4",          false,       .hw_res.gpio={GPIOB, GPIO_PIN_0}},
    {DEV_TYPE_GPIO,   "run_led",           false,       .hw_res.gpio={GPIOB, GPIO_PIN_1}},
    {DEV_TYPE_GPIO,   "iso_ac_ctrl",       false,       .hw_res.gpio={GPIOB, GPIO_PIN_2}},
    {DEV_TYPE_GPIO,   "iso_power_ctrl",    false,       .hw_res.gpio={GPIOB, GPIO_PIN_10}},
    {DEV_TYPE_GPIO,   "iso_select",        false,       .hw_res.gpio={GPIOB, GPIO_PIN_12}},
    {DEV_TYPE_GPIO,   "iso_a2_ctrl",       false,       .hw_res.gpio={GPIOB, GPIO_PIN_13}},
    {DEV_TYPE_GPIO,   "iso_a1_ctrl",       false,       .hw_res.gpio={GPIOB, GPIO_PIN_14}},
    {DEV_TYPE_GPIO,   "iso_a0_ctrl",       false,       .hw_res.gpio={GPIOB, GPIO_PIN_15}},
    {DEV_TYPE_GPIO,   "relay_ctrl1",       false,       .hw_res.gpio={GPIOB, GPIO_PIN_3}},
    {DEV_TYPE_GPIO,   "relay_ctrl2",       false,       .hw_res.gpio={GPIOB, GPIO_PIN_4}},
    {DEV_TYPE_GPIO,   "led",               false,       .hw_res.gpio={GPIOA, GPIO_PIN_8}},
    /*UART*/
    {DEV_TYPE_UART,   "usart1",            false,       .hw_res.uart={&huart1, USART1, UART_MODE_DMA_RX_TX, UART_TX_RX}},
    {DEV_TYPE_UART,   "usart2",            false,       .hw_res.uart={&huart2, USART2, UART_MODE_POLLING, UART_TX_RX}},
    /*CAN*/
    {DEV_TYPE_CAN,    "can1",              false,       .hw_res.can={&hcan, CAN1, BAUD_CAN_250K}},
    /*ADC*/

    /*I2C*/

    /*SPI*/
    
};

/* ========================= FUNCTION PROTOTYPES =========================== */
/*****************************************************************************************************
*Function   :device_control设备通用控制接口（覆盖所有运行时操作）
*Description:
*Input      :dev_type  外设类型
             dev_name  设备名称（与初始化时一致）
             ctrl_type 控制类型（读/写/配置/打开/关闭等）
             content   控制内容（传入/传出参数，不同外设不同结构）
*Output     :
*Returns    :bool true=成功，false=失败
*Note       :
*****************************************************************************************************/
bool device_control(device_type_e dev_type, const char *dev_name, device_ctrl_type_e ctrl_type, device_ctrl_content_u *content);


#endif
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
