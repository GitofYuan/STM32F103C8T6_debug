/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : uart_bus.c
* Author            : Yuan.Zong
* Version           : V1.0.0/2025.12.18
* Description       : 当前UART_BUS初步调试完成，但是还没有实现关联UART通道地址以对
                      接收到的UART数据进行过滤，后续一定需要再完善。
******************************************************************************/
/* ==============================  INCLUDES  =============================== */
#include <string.h>
#include <stdio.h>
#include "uart_bus.h"


/* ==============================  DEFINES   =============================== */

/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */
/* UART数据队列链表（单向） 结构体*/
typedef struct uart_data_queue_s
{
    uart_data_s               uart_data;  /*UART数据*/
    struct uart_data_queue_s* p_next;     /*链表指针*/
}uart_data_queue;

/* UART数据队列指针结构体*/
typedef struct
{
    uart_data_queue* head;     /*链表头*/
    uart_data_queue* tail;     /*链表尾*/
}uart_data_pointer_s;
/* ==============================  EXTERNS   =============================== */
uart_data_queue        s_uart_rx_data_queue[UART_DATA_CHNL_NUM_MAX][UART_DATA_QUEUE_NUM];   /*UART接收数据队列*/
uart_data_pointer_s    s_uart_rx_data_pointer[UART_DATA_CHNL_NUM_MAX];                      /*UART接收数据队列指针*/

uart_data_queue        s_uart_tx_data_queue[UART_DATA_CHNL_NUM_MAX][UART_DATA_QUEUE_NUM];   /*UART发送数据队列*/
uart_data_pointer_s    s_uart_tx_data_pointer[UART_DATA_CHNL_NUM_MAX];                      /*UART发送数据队列指针*/

static uint8_t uart_send_interval_timmer[UART_DATA_CHNL_NUM_MAX] = {0};   /*UART通道报文发送间隔计时器，ms*/

uint8_t uart_addr[UART_DATA_CHNL_NUM_MAX] = {0};   /*各UART通道的设备地址*/

static uint8_t print_buf[UART_DATA_MAX] = {0};
static uint8_t print_len = 0;

//int fputc(int ch,FILE *f);
/* ========================= FUNCTION PROTOTYPES =========================== */
/*****************************************************************************************************
*Function   :uart_bus_addr_init(各UART通道设备地址初始化)
*Description:先判断输入的UART通道是否为有效UART通道，然后清空该UART通道对应的接收结构体数组数据，将数组连接为单向环形链表
*Input      :uint8_t* uart_bus_addr  输入的uart通道地址数组指针
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void uart_bus_addr_init(uint8_t* uart_bus_addr)
{
    for(int i = 0; i < UART_DATA_CHNL_NUM_MAX; i++)
    {
        uart_addr[i] = *(uart_bus_addr + i);
    }
}

/*****************************************************************************************************
*Function   :uart_rx_queue_init(UART接收队列链表初始化)
*Description:先判断输入的UART通道是否为有效UART通道，然后清空该UART通道对应的接收结构体数组数据，将数组连接为单向环形链表
*Input      :uart_chnl_e chnl   UART通道
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
static void uart_rx_queue_init(uart_chnl_e chnl)
{
    if (chnl < UART_DATA_CHNL_NUM_MAX)
    {
        memset(s_uart_rx_data_queue[chnl], 0 ,sizeof(s_uart_rx_data_queue[chnl]));

        for (int i = 0; i < UART_DATA_QUEUE_NUM; i++)
        {
            /*如果i是数组最后一个，则连接向数组首地址，以形成环形链表*/
            if (i == UART_DATA_QUEUE_NUM-1)
            {
                s_uart_rx_data_queue[chnl][i].p_next = &s_uart_rx_data_queue[chnl][0];
            }
            else
            {
                s_uart_rx_data_queue[chnl][i].p_next = &s_uart_rx_data_queue[chnl][i+1];
            }
        }
    }
}

/*****************************************************************************************************
*Function   :uart_rx_enqueue（接收入队函数）
*Description:先判断输入的UART通道是否为有效UART通道，然后判断申请入队的指针是否有效，有效则入队，无效则返回false
             再判断当前队列是否已初始化，如没有则先进行初始化，然后再对入参数据进行入队操作；
             如果队列非空，则先判断队列是否已满，如满则返回false，如队列非满则进行入队操作。
*Input      :uart_chnl_e       chnl   UART通道
             uart_data_s*      data   UART接收数据结构体
*Output     :
*Returns    :bool ret       入队结果(true成功/false失败)
*Note       :
*****************************************************************************************************/
bool uart_rx_enqueue(uart_chnl_e chnl, uart_data_s* data)
{
    bool ret;    /*定义一个返回值*/
    
    if(chnl >= UART_DATA_CHNL_NUM_MAX)
        return false;
    if(data == NULL)
        return false;
    if((s_uart_rx_data_pointer[chnl].tail == NULL)
        ||(s_uart_rx_data_pointer[chnl].head == NULL))
    {
        /*空队列*/
        s_uart_rx_data_pointer[chnl].tail = &s_uart_rx_data_queue[chnl][0];
        s_uart_rx_data_pointer[chnl].head = &s_uart_rx_data_queue[chnl][0];
        if (s_uart_rx_data_pointer[chnl].tail->p_next == NULL)
        {
            uart_rx_queue_init(chnl);
        }
        
        s_uart_rx_data_queue[chnl][0].uart_data.timeout = data->timeout;
        s_uart_rx_data_queue[chnl][0].uart_data.len = data->len;
        memcpy(s_uart_rx_data_queue[chnl][0].uart_data.data, data->data, UART_DATA_MAX);
        ret = true;
    }
    else if((s_uart_rx_data_pointer[chnl].tail != NULL)
        &&(s_uart_rx_data_pointer[chnl].head != NULL))
    {
        /*队列满了*/
        if (s_uart_rx_data_pointer[chnl].tail->p_next == s_uart_rx_data_pointer[chnl].head) 
        {
            ret = false;
        }
        else
        {
            s_uart_rx_data_pointer[chnl].tail = s_uart_rx_data_pointer[chnl].tail->p_next;
            s_uart_rx_data_pointer[chnl].tail->uart_data.timeout = data->timeout;
            s_uart_rx_data_pointer[chnl].tail->uart_data.len = data->len;
            memcpy(s_uart_rx_data_pointer[chnl].tail->uart_data.data, data->data, UART_DATA_MAX);
            ret = true;
        }
    }
    else
    {
        ret = false;
    }

    return ret;
}

/*****************************************************************************************************
*Function   :uart_rx_data_queue_recv（接收出队函数）
*Description:先判断输入的UART通道是否为有效UART通道，然后判断申请入队的指针是否有效，有效则入队，无效则返回false
             禁用中断，目的是防止入队出队同时被执行造成数据混乱；
             如果队列为空，则返回false；
             如果队列非空，则再判断当前是否只有一组数据，如果是，则出队后清空队列头尾指针；如不是，则正常出队。
             恢复中断。
*Input      :uart_chnl_e       chnl   UART通道
*Output     :uart_data_s*      data   UART接收数据结构体
*Returns    :bool ret       出队结果(true成功/false失败)
*Note       :
*****************************************************************************************************/
bool uart_rx_dequeue(uart_chnl_e chnl, uart_data_s* data)
{
    bool ret;
    
    if(chnl >= UART_DATA_CHNL_NUM_MAX)
        return false;
    if(data == NULL)
        return false;
    
    __set_PRIMASK(1);  /*这里禁用中断，目的是防止入队出队同时被执行造成数据混乱*/
    
    if ((s_uart_rx_data_pointer[chnl].tail == NULL)
        ||(s_uart_rx_data_pointer[chnl].head == NULL))
    {
        /*空队列*/
        ret = false;
    }
    else if((s_uart_rx_data_pointer[chnl].tail != NULL)
        &&(s_uart_rx_data_pointer[chnl].head != NULL))
    {
        if (s_uart_rx_data_pointer[chnl].head == s_uart_rx_data_pointer[chnl].tail)
        {
            /*只有一个数据*/
            data->timeout = s_uart_rx_data_pointer[chnl].head->uart_data.timeout;
            data->len = s_uart_rx_data_pointer[chnl].head->uart_data.len;
            memcpy(data->data, s_uart_rx_data_pointer[chnl].head->uart_data.data, UART_DATA_MAX);
            s_uart_rx_data_pointer[chnl].head = NULL;
            s_uart_rx_data_pointer[chnl].tail = NULL;
        }
        else
        {
            data->timeout = s_uart_rx_data_pointer[chnl].head->uart_data.timeout;
            data->len = s_uart_rx_data_pointer[chnl].head->uart_data.len;
            memcpy(data->data, s_uart_rx_data_pointer[chnl].head->uart_data.data, UART_DATA_MAX);
            s_uart_rx_data_pointer[chnl].head =  s_uart_rx_data_pointer[chnl].head->p_next;
        }
        ret = true;
    }
    else
    {
        ret = false;
    }
    __set_PRIMASK(0);  /*中断使能*/
    
    return ret;
}

/*****************************************************************************************************
*Function   :uart_data_queue_init(UART发送队列链表初始化)
*Description:先判断输入的UART通道是否为有效UART通道，然后清空该UART通道对应的接收结构体数组数据，将数组连接为单向环形链表
*Input      :uart_chnl_e chnl   UART通道
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
static void uart_tx_queue_init(uart_chnl_e chnl)
{
    if (chnl < UART_DATA_CHNL_NUM_MAX)
    {
        memset(s_uart_tx_data_queue[chnl], 0 ,sizeof(s_uart_tx_data_queue[chnl]));

        for (int i = 0; i < UART_DATA_QUEUE_NUM; i++)
        {
            /*如果i是数组最后一个，则连接向数组首地址，以形成环形链表*/
            if (i == UART_DATA_QUEUE_NUM-1)
            {
                s_uart_tx_data_queue[chnl][i].p_next = &s_uart_tx_data_queue[chnl][0];
            }
            else
            {
                s_uart_tx_data_queue[chnl][i].p_next = &s_uart_tx_data_queue[chnl][i+1];
            }
        }
    }
}

/*****************************************************************************************************
*Function   :uart_tx_enqueue（发送入队函数）
*Description:先判断输入的UART通道是否为有效UART通道，然后判断申请入队的指针是否有效，有效则入队，无效则返回false
             再判断当前队列是否已初始化，如没有则先进行初始化，然后再对入参数据进行入队操作；
             如果队列非空，则先判断队列是否已满，如满则返回false，如队列非满则进行入队操作。
*Input      :uart_chnl_e       chnl   UART通道
             uart_data_s*  data   UART发送数据结构体
*Output     :
*Returns    :bool ret       入队结果
*Note       :
*****************************************************************************************************/
bool uart_tx_enqueue(uart_chnl_e chnl, uart_data_s* data)
{
    bool ret;    /*定义一个返回值*/
    
    if(chnl >= UART_DATA_CHNL_NUM_MAX)
        return false;
    if(data == NULL)
        return false;
    if((s_uart_tx_data_pointer[chnl].tail == NULL)
        ||(s_uart_tx_data_pointer[chnl].head == NULL))
    {
        /*空队列*/
        s_uart_tx_data_pointer[chnl].tail = &s_uart_tx_data_queue[chnl][0];
        s_uart_tx_data_pointer[chnl].head = &s_uart_tx_data_queue[chnl][0];
        if (s_uart_tx_data_pointer[chnl].tail->p_next == NULL)
        {
            uart_tx_queue_init(chnl);
        }
        
        s_uart_tx_data_queue[chnl][0].uart_data.timeout = data->timeout;
        s_uart_tx_data_queue[chnl][0].uart_data.len = data->len;
        memcpy(s_uart_tx_data_queue[chnl][0].uart_data.data, data->data, data->len);
        ret = true;
    }
    else if((s_uart_tx_data_pointer[chnl].tail != NULL)
        &&(s_uart_tx_data_pointer[chnl].head != NULL))
    {
        /*队列满了*/
        if (s_uart_tx_data_pointer[chnl].tail->p_next == s_uart_tx_data_pointer[chnl].head) 
        {
            ret = false;
        }
        else
        {
            s_uart_tx_data_pointer[chnl].tail = s_uart_tx_data_pointer[chnl].tail->p_next;
            s_uart_tx_data_pointer[chnl].tail->uart_data.timeout = data->timeout;
            s_uart_tx_data_pointer[chnl].tail->uart_data.len = data->len;
            memcpy(s_uart_tx_data_pointer[chnl].tail->uart_data.data, data->data, data->len);
            ret = true;
        }
    }
    else
    {
        ret = false;
    }

    return ret;
}

/*****************************************************************************************************
*Function   :uart_send（UART发送函数）
*Description:按照输入的UART通道和要发送的数据，进行数据头编辑，发送长度设置，然后调用相应的UART通道将UART数据发送出去。
             如果发送失败则需要重新初始化UART。
*Input      :uart_chnl_e       chnl   UART通道
             uart_data_s*  data   UART发送数据结构体
*Output     :
*Returns    :
*Note       :这个函数当前还没想好放在驱动层还是CAN_BUS。
*****************************************************************************************************/
bool uart_send(uart_chnl_e chnl, uart_data_s* data)
{
    bool                     ret;        /*定义一个返回值*/
    device_ctrl_content_u    content;    /*UART消息头结构体*/
    
    /*设置消息头*/
    content.uart.len = data->len;                      /*设置标准ID*/
    content.uart.buf = data->data;                     /*设置扩展ID*/
    
    /*设置超时时间，如果timeout大于发送间隔，则使用发送间隔，否则使用正常输入的timeout*/
    if(data->timeout >= UART_SEND_INTERVAL)
    {
        content.uart.timeout_ms = UART_SEND_INTERVAL;
    }
    else
    {
        content.uart.timeout_ms = data->timeout;              
    }
    
    switch(chnl)
    {
        case UART_DATA_QUEUE_CHNL_1:
            if(device_control(DEV_TYPE_UART, "usart1", DEV_CTRL_WRITE, &content) == true)
            {
                ret = true;
                break;
            }
            else
            {
                /*发送错误处理*/
                #ifdef DEBUG_UART_BUS
                printf("UART send failure,init again!\n");
                #endif
                /*重新初始化UART，这部分需要等中间层init功能编写完成后再补充*/

                ret = false;
                break;
            }
        case UART_DATA_QUEUE_CHNL_2:
            if(device_control(DEV_TYPE_UART, "usart2", DEV_CTRL_WRITE, &content) == true)
            {
                ret = true;
                break;
            }
            else
            {
                /*发送错误处理*/
                #ifdef DEBUG_UART_BUS
                printf("UART send failure,init again!\n");
                #endif
                /*重新初始化UART，这部分需要等中间层init功能编写完成后再补充*/

                ret = false;
                break;
            }
        default: break;
    }
    return ret;
}

/*****************************************************************************************************
*Function   :uart_tx_dequeue（发送出队函数）
*Description:依次处理每个UART通道队列，
             如果队列为空或未到最小发送时间间隔，则跳过当前UART通道；
             如果队列非空，则再判断当前是否只有一组数据，如果是，则出队后清空队列头尾指针；如不是，则正常出队。
*Input      :
*Output     :
*Returns    :
*Note       :经验教训，使用DMA库函数发送，调用后不能立刻清空发送buffer，要预留大概每字节1ms的时间，所以
最好的办法是发送不适用DMA，而使用标准库函数，使用timeout参数强行拉长发送时间。
*****************************************************************************************************/
void uart_tx_dequeue(void)
{
    uart_chnl_e chnl;
    for(chnl = UART_DATA_QUEUE_CHNL_1; chnl < UART_DATA_CHNL_NUM_MAX; chnl++)
    {
        uart_send_interval_timmer[chnl]++;
        if ((s_uart_tx_data_pointer[chnl].tail == NULL)
            ||(s_uart_tx_data_pointer[chnl].head == NULL)
            ||(uart_send_interval_timmer[chnl] < UART_SEND_INTERVAL))   /*空队列或未到最小发送时间间隔*/
        {
            continue;
        }
        else if((s_uart_tx_data_pointer[chnl].tail != NULL)
            &&(s_uart_tx_data_pointer[chnl].head != NULL))
        {
            /*只有一个数据*/
            if (s_uart_tx_data_pointer[chnl].head == s_uart_tx_data_pointer[chnl].tail)
            {
                if(uart_send(chnl, &(s_uart_tx_data_pointer[chnl].head->uart_data)) == true)
                {
                    memset(&s_uart_tx_data_pointer[chnl].head->uart_data, 0 ,sizeof(uart_data_s));
                    s_uart_tx_data_pointer[chnl].head = NULL;
                    s_uart_tx_data_pointer[chnl].tail = NULL;
                    uart_send_interval_timmer[chnl] = 0;
                }
            }
            else
            {
                if(uart_send(chnl, &(s_uart_tx_data_pointer[chnl].head->uart_data)) == true)
                {
                    memset(&s_uart_tx_data_pointer[chnl].head->uart_data, 0 ,sizeof(uart_data_s));
                    s_uart_tx_data_pointer[chnl].head =  s_uart_tx_data_pointer[chnl].head->p_next;
                    uart_send_interval_timmer[chnl] = 0;
                }
            }
        }
    }
}

/*****************************************************************************************************
*Function   :fputc（prtinf函数重定向）
*Description:将数据先同一存入print_buf数组内，待数据填充完成或数组填满，再统一进行串口发送入队，这样确保
             打印数据不会出现瞬时阻塞的情况。
*Input      :
*Output     :
*Returns    :
*Note       :1、打印字符串长度受宏定义UART_DATA_MAX限制；2、由于使用UTF-8编译，所以不能打印中文。
*****************************************************************************************************/
int fputc(int ch,FILE *f)
{
    if(print_len < UART_DATA_MAX)
    {
        print_buf[print_len] = ch;
        print_len++;
    }
    if(ch == '\n' || print_len >= UART_DATA_MAX)
    {
        uart_data_s print_data;
        print_data.len = print_len;
        memcpy(print_data.data, print_buf, print_len);
        print_data.timeout = UART_SEND_INTERVAL;
        uart_tx_enqueue(UART_DATA_QUEUE_CHNL_1, &print_data);
        
        print_len = 0;
    }
	return ch;
}



/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
