/*********************************Copyright(C)********************************
*-------------------------------File Information------------------------------
* FileName          : can_bus.c
* Author            : Yuan.Zong
* Version           : V1.0.0/2025.12.18
* Description       : 当前CAN_BUS初步调试完成，但是还没有实现关联CAN通道地址以对
                      接收到的CAN数据进行过滤，后续一定需要再完善。
******************************************************************************/
/* ==============================  INCLUDES  =============================== */
#include <string.h>
#include "can_bus.h"
#include "device_control.h"
/* ==============================  DEFINES   =============================== */

/* ==============================   ENUMS    =============================== */

/* ======================== STRUCTURES AND UNIONS ========================== */
/* CAN数据队列链表（单向） 结构体*/
typedef struct can_data_queue
{
    Can_Rcv_Data           can_data;  /*CAN数据*/
    struct can_data_queue* p_next;    /*链表指针*/
}stru_can_data_queue;

/* CAN数据队列指针结构体*/
typedef struct
{
    stru_can_data_queue* head;     /*链表头*/
    stru_can_data_queue* tail;     /*链表尾*/
}stru_can_data_pointer;
/* ==============================  EXTERNS   =============================== */
stru_can_data_queue     g_can_rx_data_queue[CAN_DATA_CHNL_NUM_MAX][CAN_DATA_QUEUE_NUM];   /*CAN接收数据队列*/
stru_can_data_pointer   g_can_rx_data_pointer[CAN_DATA_CHNL_NUM_MAX];                     /*CAN接收数据队列指针*/

stru_can_data_queue     g_can_tx_data_queue[CAN_DATA_CHNL_NUM_MAX][CAN_DATA_QUEUE_NUM];   /*CAN发送数据队列*/
stru_can_data_pointer   g_can_tx_data_pointer[CAN_DATA_CHNL_NUM_MAX];                     /*CAN发送数据队列指针*/

static uint8_t send_interval_timmer[CAN_DATA_CHNL_NUM_MAX] = {0};   /*CAN通道报文发送间隔计时器，ms*/

uint8_t can_addr[CAN_DATA_CHNL_NUM_MAX] = {0};   /*各CAN通道的设备地址*/
/* ========================= FUNCTION PROTOTYPES =========================== */
/*****************************************************************************************************
*Function   :can_bus_addr_init(各CAN通道设备地址初始化)
*Description:先判断输入的CAN通道是否为有效CAN通道，然后清空该CAN通道对应的接收结构体数组数据，将数组连接为单向环形链表
*Input      :uint8_t* can_bus_addr  输入的can通道地址数组指针
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void can_bus_addr_init(uint8_t* can_bus_addr)
{
    for(int i = 0; i < CAN_DATA_CHNL_NUM_MAX; i++)
    {
        can_addr[i] = *(can_bus_addr + i);
    }
}

/*****************************************************************************************************
*Function   :can_data_queue_init(CAN接收队列链表初始化)
*Description:先判断输入的CAN通道是否为有效CAN通道，然后清空该CAN通道对应的接收结构体数组数据，将数组连接为单向环形链表
*Input      :CAN_CHNL chnl   CAN通道
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
static void can_rx_queue_init(CAN_CHNL chnl)
{
    if (chnl < CAN_DATA_CHNL_NUM_MAX)
    {
        memset(g_can_rx_data_queue[chnl], 0 ,sizeof(g_can_rx_data_queue[chnl]));

        for (int i = 0; i < CAN_DATA_QUEUE_NUM; i++)
        {
            /*如果i是数组最后一个，则连接向数组首地址，以形成环形链表*/
            if (i == CAN_DATA_QUEUE_NUM-1)
            {
                g_can_rx_data_queue[chnl][i].p_next = &g_can_rx_data_queue[chnl][0];
            }
            else
            {
                g_can_rx_data_queue[chnl][i].p_next = &g_can_rx_data_queue[chnl][i+1];
            }
        }
    }
}

/*****************************************************************************************************
*Function   :can_rx_enqueue（接收入队函数）
*Description:先判断输入的CAN通道是否为有效CAN通道，然后判断申请入队的指针是否有效，有效则入队，无效则返回false
             再判断当前队列是否已初始化，如没有则先进行初始化，然后再对入参数据进行入队操作；
             如果队列非空，则先判断队列是否已满，如满则返回false，如队列非满则进行入队操作。
*Input      :CAN_CHNL       chnl   CAN通道
             Can_Rcv_Data*  data   CAN接收数据结构体
*Output     :
*Returns    :bool ret       入队结果
*Note       :
*****************************************************************************************************/
bool can_rx_enqueue(CAN_CHNL chnl, Can_Rcv_Data* data)
{
    bool ret;    /*定义一个返回值*/
    
    if(chnl >= CAN_DATA_CHNL_NUM_MAX)
        return false;
    if(data == NULL)
        return false;
    if((g_can_rx_data_pointer[chnl].tail == NULL)
        ||(g_can_rx_data_pointer[chnl].head == NULL))
    {
        /*空队列*/
        g_can_rx_data_pointer[chnl].tail = &g_can_rx_data_queue[chnl][0];
        g_can_rx_data_pointer[chnl].head = &g_can_rx_data_queue[chnl][0];
        if (g_can_rx_data_pointer[chnl].tail->p_next == NULL)
        {
            can_rx_queue_init(chnl);
        }
        
        g_can_rx_data_queue[chnl][0].can_data.id = data->id;
        g_can_rx_data_queue[chnl][0].can_data.len = data->len;
        memcpy(g_can_rx_data_queue[chnl][0].can_data.data, data->data, 8);
        ret = true;
    }
    else if((g_can_rx_data_pointer[chnl].tail != NULL)
        &&(g_can_rx_data_pointer[chnl].head != NULL))
    {
        /*队列满了*/
        if (g_can_rx_data_pointer[chnl].tail->p_next == g_can_rx_data_pointer[chnl].head) 
        {
            ret = false;
        }
        else
        {
            g_can_rx_data_pointer[chnl].tail = g_can_rx_data_pointer[chnl].tail->p_next;
            g_can_rx_data_pointer[chnl].tail->can_data.id = data->id;
            g_can_rx_data_pointer[chnl].tail->can_data.len = data->len;
            memcpy(g_can_rx_data_pointer[chnl].tail->can_data.data, data->data, 8);
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
*Function   :can_rx_data_queue_recv（接收出队函数）
*Description:先判断输入的CAN通道是否为有效CAN通道，然后判断申请入队的指针是否有效，有效则入队，无效则返回false
             禁用中断，目的是防止入队出队同时被执行造成数据混乱；
             如果队列为空，则返回false；
             如果队列非空，则再判断当前是否只有一组数据，如果是，则出队后清空队列头尾指针；如不是，则正常出队。
             恢复中断。
*Input      :CAN_CHNL       chnl   CAN通道
*Output     :Can_Rcv_Data*  data   CAN接收数据结构体
*Returns    :bool ret       出队结果
*Note       :
*****************************************************************************************************/
bool can_rx_dequeue(CAN_CHNL chnl, Can_Rcv_Data* data)
{
    bool ret;
    
    if(chnl >= CAN_DATA_CHNL_NUM_MAX)
        return false;
    if(data == NULL)
        return false;
    
    __set_PRIMASK(1);  /*这里禁用中断，目的是防止入队出队同时被执行造成数据混乱*/
    
    if ((g_can_rx_data_pointer[chnl].tail == NULL)
        ||(g_can_rx_data_pointer[chnl].head == NULL))
    {
        /*空队列*/
        ret = false;
    }
    else if((g_can_rx_data_pointer[chnl].tail != NULL)
        &&(g_can_rx_data_pointer[chnl].head != NULL))
    {
        if (g_can_rx_data_pointer[chnl].head == g_can_rx_data_pointer[chnl].tail)
        {
            /*只有一个数据*/
            data->id = g_can_rx_data_pointer[chnl].head->can_data.id;
            data->len = g_can_rx_data_pointer[chnl].head->can_data.len;
            memcpy(data->data, g_can_rx_data_pointer[chnl].head->can_data.data, 8);
            g_can_rx_data_pointer[chnl].head = NULL;
            g_can_rx_data_pointer[chnl].tail = NULL;
        }
        else
        {
            data->id = g_can_rx_data_pointer[chnl].head->can_data.id;
            data->len = g_can_rx_data_pointer[chnl].head->can_data.len;
            memcpy(data->data, g_can_rx_data_pointer[chnl].head->can_data.data, 8);
            g_can_rx_data_pointer[chnl].head =  g_can_rx_data_pointer[chnl].head->p_next;
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
*Function   :can_data_queue_init(CAN发送队列链表初始化)
*Description:先判断输入的CAN通道是否为有效CAN通道，然后清空该CAN通道对应的接收结构体数组数据，将数组连接为单向环形链表
*Input      :CAN_CHNL chnl   CAN通道
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
static void can_tx_queue_init(CAN_CHNL chnl)
{
    if (chnl < CAN_DATA_CHNL_NUM_MAX)
    {
        memset(g_can_tx_data_queue[chnl], 0 ,sizeof(g_can_tx_data_queue[chnl]));

        for (int i = 0; i < CAN_DATA_QUEUE_NUM; i++)
        {
            /*如果i是数组最后一个，则连接向数组首地址，以形成环形链表*/
            if (i == CAN_DATA_QUEUE_NUM-1)
            {
                g_can_tx_data_queue[chnl][i].p_next = &g_can_tx_data_queue[chnl][0];
            }
            else
            {
                g_can_tx_data_queue[chnl][i].p_next = &g_can_tx_data_queue[chnl][i+1];
            }
        }
    }
}

/*****************************************************************************************************
*Function   :can_tx_enqueue（发送入队函数）
*Description:先判断输入的CAN通道是否为有效CAN通道，然后判断申请入队的指针是否有效，有效则入队，无效则返回false
             再判断当前队列是否已初始化，如没有则先进行初始化，然后再对入参数据进行入队操作；
             如果队列非空，则先判断队列是否已满，如满则返回false，如队列非满则进行入队操作。
*Input      :CAN_CHNL       chnl   CAN通道
             Can_Rcv_Data*  data   CAN发送数据结构体
*Output     :
*Returns    :bool ret       入队结果
*Note       :
*****************************************************************************************************/
bool can_tx_enqueue(CAN_CHNL chnl, Can_Rcv_Data* data)
{
    bool ret;    /*定义一个返回值*/
    
    if(chnl >= CAN_DATA_CHNL_NUM_MAX)
        return false;
    if(data == NULL)
        return false;
    if((g_can_tx_data_pointer[chnl].tail == NULL)
        ||(g_can_tx_data_pointer[chnl].head == NULL))
    {
        /*空队列*/
        g_can_tx_data_pointer[chnl].tail = &g_can_tx_data_queue[chnl][0];
        g_can_tx_data_pointer[chnl].head = &g_can_tx_data_queue[chnl][0];
        if (g_can_tx_data_pointer[chnl].tail->p_next == NULL)
        {
            can_tx_queue_init(chnl);
        }
        
        g_can_tx_data_queue[chnl][0].can_data.id = data->id;
        g_can_tx_data_queue[chnl][0].can_data.len = data->len;
        memcpy(g_can_tx_data_queue[chnl][0].can_data.data, data->data, 8);
        ret = true;
    }
    else if((g_can_tx_data_pointer[chnl].tail != NULL)
        &&(g_can_tx_data_pointer[chnl].head != NULL))
    {
        /*队列满了*/
        if (g_can_tx_data_pointer[chnl].tail->p_next == g_can_tx_data_pointer[chnl].head) 
        {
            ret = false;
        }
        else
        {
            g_can_tx_data_pointer[chnl].tail = g_can_tx_data_pointer[chnl].tail->p_next;
            g_can_tx_data_pointer[chnl].tail->can_data.id = data->id;
            g_can_tx_data_pointer[chnl].tail->can_data.len = data->len;
            memcpy(g_can_tx_data_pointer[chnl].tail->can_data.data, data->data, 8);
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
*Function   :can_send（CAN发送函数）
*Description:按照输入的CAN通道和要发送的数据，进行数据头编辑，发送长度设置，然后调用相应的CAN通道将CAN数据发送出去。
             如果发送失败则需要重新初始化CAN。
*Input      :CAN_CHNL       chnl   CAN通道
             Can_Rcv_Data*  data   CAN发送数据结构体
*Output     :
*Returns    :
*Note       :这个函数当前还没想好放在驱动层还是CAN_BUS。
*****************************************************************************************************/
bool can_send(CAN_CHNL chnl, Can_Rcv_Data* data)
{
    bool                 ret = false;        /*定义一个返回值*/
    device_ctrl_content_u can_send_data;
    can_send_data.can.id = data->id;
    can_send_data.can.dlc = data->len;
    can_send_data.can.data = data->data;
    
    switch(chnl)
    {
        case CAN_DATA_QUEUE_CHNL_1:
            if(device_control(DEV_TYPE_CAN, "can1", DEV_CTRL_WRITE, &can_send_data) == true)
            {
                ret = true;
            }
            break;
        default: break;
    }
    return ret;
}

/*****************************************************************************************************
*Function   :can_tx_dequeue（发送出队函数）
*Description:依次处理每个CAN通道队列，
             如果队列为空或未到最小发送时间间隔，则跳过当前CAN通道；
             如果队列非空，则再判断当前是否只有一组数据，如果是，则出队后清空队列头尾指针；如不是，则正常出队。
*Input      :
*Output     :
*Returns    :
*Note       :
*****************************************************************************************************/
void can_tx_dequeue(void)
{
    CAN_CHNL chnl;
    
    for(chnl = CAN_DATA_QUEUE_CHNL_1; chnl < CAN_DATA_CHNL_NUM_MAX; chnl++)
    {
        send_interval_timmer[chnl]++;
        if ((g_can_tx_data_pointer[chnl].tail == NULL)
            ||(g_can_tx_data_pointer[chnl].head == NULL)
            ||(send_interval_timmer[chnl] < SEND_INTERVAL))   /*空队列或未到最小发送时间间隔*/
        {
            continue;
        }
        else if((g_can_tx_data_pointer[chnl].tail != NULL)
            &&(g_can_tx_data_pointer[chnl].head != NULL))
        {
            /*只有一个数据*/
            if (g_can_tx_data_pointer[chnl].head == g_can_tx_data_pointer[chnl].tail)
            {
                if(can_send(chnl, &(g_can_tx_data_pointer[chnl].head->can_data)) == true)
                {
                    memset(&g_can_tx_data_pointer[chnl].head->can_data, 0 ,sizeof(Can_Rcv_Data));
                    g_can_tx_data_pointer[chnl].head = NULL;
                    g_can_tx_data_pointer[chnl].tail = NULL;
                    send_interval_timmer[chnl] = 0;
                }
            }
            else
            {
                if(can_send(chnl, &(g_can_tx_data_pointer[chnl].head->can_data)) == true)
                {
                    memset(&g_can_tx_data_pointer[chnl].head->can_data, 0 ,sizeof(Can_Rcv_Data));
                    g_can_tx_data_pointer[chnl].head =  g_can_tx_data_pointer[chnl].head->p_next;
                    send_interval_timmer[chnl] = 0;
                }
            }
        }
    }
}

/***************** (C)COPYRIGHT 2022 XXXXXXXX*****END OF FILE*****************/
