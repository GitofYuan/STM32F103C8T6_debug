/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : device_control_gpio.h
* Author            : Yuan.Zong
* Version           : V1.0.0/2026.02.27
* Description       :
******************************************************************************/
#ifndef __DEVICE_CONTROL_GPIO_H__
#define __DEVICE_CONTROL_GPIO_H__
/* ==============================  INCLUDES  =============================== */

/* ==============================  DEFINES   =============================== */

/* ==============================   ENUMS    =============================== */
/*GPIO电平枚举*/
typedef enum 
{
    GPIO_RESET = 0,   /*低电平*/
    GPIO_SET,         /*高电平*/
    GPIO_TOGGLE,      /*翻转电平*/
} GPIO_level_e;

/*GPIO模式枚举（覆盖常用模式）*/
typedef enum 
{
    INPUT = 0,         /*输入模式*/
    OUTPUT_PUSH_PULL,  /*推挽输出*/
    OUTPUT_OPEN_DRAIN, /*开漏输出*/
} GPIO_mode_e;

/*GPIO上下拉枚举（覆盖常用模式）*/
typedef enum 
{
    GPIO_PULL_NONE = 0,   /*无上下拉*/
    GPIO_PULL_UP,         /*上拉*/
    GPIO_PULL_DOWN,       /*下拉*/
} GPIO_pull_e;

/*GPIO输出速度枚举（覆盖常用模式）*/
typedef enum 
{
    OUTPUT_LOW = 0,    /*低速输出*/
    OUTPUT_MEDIUM,     /*中速输出*/
    OUTPUT_HIGH,       /*高速输出*/
} GPIO_speed_e;

/* ======================== STRUCTURES AND UNIONS ========================== */

/* ==============================  EXTERNS   =============================== */

/* ========================= FUNCTION PROTOTYPES =========================== */

#endif
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
