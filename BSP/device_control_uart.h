/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : device_control_uart.h
* Author            : Yuan.Zong
* Version           : V1.0.0/2026.02.27
* Description       :
******************************************************************************/
#ifndef __DEVICE_CONTROL_UART_H__
#define __DEVICE_CONTROL_UART_H__
/* ==============================  INCLUDES  =============================== */

/* ==============================  DEFINES   =============================== */

/* ==============================   ENUMS    =============================== */
/*串口模式枚举（覆盖常用模式）*/
typedef enum 
{
    UART_MODE_POLLING = 0,    /*普通模式*/
    UART_MODE_IT_RX,          /*中断接收模式*/
    UART_MODE_IT_TX,          /*中断发送模式*/
    UART_MODE_IT_RX_TX,       /*中断收发模式*/
    UART_MODE_DMA_RX,         /*DMA接收模式*/
    UART_MODE_DMA_TX,         /*DMA发送模式*/
    UART_MODE_DMA_RX_TX,      /*DMA收发模式*/
} uart_mode_e;

/*串口收发模式枚举（覆盖常用模式）*/
typedef enum 
{
    UART_RX = 0,    /*只收模式*/
    UART_TX,        /*只发模式*/
    UART_TX_RX,     /*收发模式*/
} uart_rt_mode_e;

/*串口波特率枚举（覆盖常用值）*/
typedef enum 
{
    BAUD_9600 = 9600,
    BAUD_19200 = 19200,
    BAUD_115200 = 115200,
} uart_baud_rate_e;

/*串口数据位枚举（覆盖常用值）*/
typedef enum 
{
    DATA_BITS_5 = 5,
    DATA_BITS_6 = 6,
    DATA_BITS_7 = 7,
    DATA_BITS_8 = 8,
    DATA_BITS_9 = 9,
} uart_data_bits_e;

/*串口停止位枚举（覆盖常用值）*/
typedef enum 
{
    STOP_BITS_1 = 1,
    STOP_BITS_2 = 2,
} uart_stop_bits_e;

/*串口校验位枚举（覆盖常用值）*/
typedef enum 
{
    PARITY_NONE = 'N',   /*无校验*/
    PARITY_ODD  = 'O',   /*奇校验*/
    PARITY_EVEN = 'E',   /*偶校验*/
} uart_parity_e;


/* ======================== STRUCTURES AND UNIONS ========================== */

/* ==============================  EXTERNS   =============================== */

/* ========================= FUNCTION PROTOTYPES =========================== */

#endif
/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
